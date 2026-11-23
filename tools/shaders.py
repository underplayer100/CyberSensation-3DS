"""Rendus pré-calculés de shaders GLSL du mod (la 3DS ne peut pas les exécuter).

Chaque fonction reproduit le .frag d'origine avec numpy et renvoie une image RGB (sur fond noir : les
pixels transparents du shader ne font qu'ajouter leur lueur, en alpha prémultiplié).
"""
import numpy as np
from PIL import Image


def _fract(x):
    return x - np.floor(x)


def _shash3(p):
    """shash3 de iq : [-1;1[ par composante. p : (..., 3)."""
    q = np.stack([p @ np.array([127.1, 311.7, 74.7]), p @ np.array([269.5, 183.3, 246.1]),
                  p @ np.array([113.5, 271.9, 124.6])], -1)
    return -1.0 + 2.0 * _fract(np.sin(q) * 43758.5453123)


def _rot2d(x, y, a):
    s, c = np.sin(a), np.cos(a)
    return x * c - y * s, x * s + y * c


def _half_circle(p):
    """sdHalfCircle : (distance, matériau 0/1)."""
    ofs = _shash3(np.floor(p * 13.0) / 13.0)
    lv = 32.0 + ofs
    h2 = _shash3(np.floor(p * lv) / lv)
    h3 = _shash3(np.floor(p * 16.0) / 16.0)
    p = p + 0.02 * h2 + 0.01 * h3
    ln = np.linalg.norm(p, axis=-1)
    ds = np.maximum(ln - 1.0, -(ln - 0.9))
    ds = np.maximum(ds, p[..., 0] - 0.1)
    ds = np.maximum(ds, -p[..., 0] - 0.1)
    m = np.minimum(np.maximum(h3[..., 0], h2[..., 0]), np.maximum(ofs[..., 0], h2[..., 1]))
    return ds - 0.05, (m >= 0.75).astype(np.float64)


def _scene_coolbg(p, t):
    q = (p - np.array([0.0, -10.0, 0.0])) / 10.0
    x, y, z = q[..., 0], q[..., 1], q[..., 2]
    # dp.zx = rot2d(dp.xz, 0.3 t)
    rx, ry = _rot2d(x, z, 0.3 * t)
    z, x = rx, ry
    best_d, best_m = _half_circle(np.stack([x, y, z], -1))
    x, y = _rot2d(x, y, -0.7 * t)
    for k, a in ((1.3, None), (1.8, 1.1), (2.8, -1.3)):
        if a is not None:
            x, z = _rot2d(x, z, a * t)
        d, m = _half_circle(np.stack([x, y, z], -1) * k)
        sel = d < best_d
        best_d, best_m = np.where(sel, d, best_d), np.where(sel, m, best_m)
    sel = 30.0 < best_d
    best_d, best_m = np.where(sel, 30.0, best_d), np.where(sel, 0.0, best_m)
    return best_d * 10.0, best_m


def _rgb_to_hsv(c):
    mx, mn = c.max(-1), c.min(-1)
    delta = mx - mn
    h = np.zeros_like(mx)
    s = np.where(delta != 0, delta / np.where(mx == 0, 1, mx), 0)
    safe = np.where(delta == 0, 1, delta)
    dr, dg, db = [((mx - c[..., k]) / 6.0 + delta / 2.0) / safe for k in range(3)]
    h = np.where(c[..., 0] == mx, db - dg, np.where(c[..., 1] == mx, 1 / 3 + dr - db, 2 / 3 + dg - dr))
    h = np.where(h < 0, h + 1, h)
    h = np.where(h > 1, h - 1, h)
    h = np.where(delta != 0, h, 0)
    return h, s, mx


def _hsv_to_rgb(h, s, v):
    vh = h * 6.0
    vi = np.floor(vh)
    f = vh - vi
    v1, v2, v3 = v * (1 - s), v * (1 - s * f), v * (1 - s * (1 - f))
    out = np.stack([v, v3, v1], -1)  # var_i == 0
    for k, trip in ((1, (v2, v, v1)), (2, (v1, v, v3)), (3, (v1, v2, v)), (4, (v3, v1, v))):
        out = np.where((vi == k)[..., None], np.stack(trip, -1), out)
    # "else" du GLSL : var_i 5 ou négatif (teinte décalée sous 0)
    other = (vi < 0) | (vi >= 5)
    out = np.where(other[..., None], np.stack([v, v1, v2], -1), out)
    return np.where((s == 0)[..., None], np.stack([v, v, v], -1), out)


def render_coolbg(t, w=192, h=108, seed=0):
    """shaders/coolbg.frag : anneaux brisés en rotation, lueur violette, vignette."""
    yy, xx = np.mgrid[0:h, 0:w]
    uv = np.stack([(xx + 0.5) / w - 0.5, (yy + 0.5) / h - 0.5], -1)  # texcoord0 (y vers le bas, comme OpenFL)
    eye, look, up = np.array([7.0, 8, 9]), np.array([0.0, -10, 0]), np.array([0.2, 1, 0]) / np.hypot(0.2, 1)
    negz = (look - eye) / np.linalg.norm(look - eye)
    u = np.cross(negz, up)
    v = np.cross(u, negz)
    dirs = negz + uv[..., :1] * u * (16 / 9) + uv[..., 1:] * v
    dirs /= np.linalg.norm(dirs, axis=-1, keepdims=True)
    jit = uv + 0.01 * _fract(t)  # nrand(texcoord0 + 0.01 * fract(iTime))
    rnd = _fract(np.sin(jit[..., 0] * 12.9898 + jit[..., 1] * 78.233) * 43758.5453)
    p = eye + 2.8 * dirs - 2.0 * dirs * rnd[..., None]
    n = w * h
    p, d3 = p.reshape(n, 3), dirs.reshape(n, 3)
    iters = np.full(n, 128.0)
    hit = np.zeros(n)
    mat = np.zeros(n)
    rdt = np.zeros(n)
    act = np.arange(n)
    for i in range(128):
        if act.size == 0:
            break
        d, m = _scene_coolbg(p[act], t)
        stop = (d < 0) | (rdt[act] > 30.0)
        done = act[stop]
        iters[done] = i + 1
        hit[done] = (d[stop] < 0)
        mat[done] = m[stop]
        go = ~stop
        a2 = act[go]
        dt = 0.01 + 0.25 * d[go]
        p[a2] += d3[a2] * dt[:, None]
        rdt[a2] += dt
        act = a2
    iters, hit, mat = iters.reshape(h, w), hit.reshape(h, w), mat.reshape(h, w)
    col = np.repeat((np.maximum(mat, 0) * hit)[..., None], 3, -1)
    it = (iters + 0.5 - rnd) / 128.0
    glow = 10.0 * np.power(np.maximum(it, 0), 2.3)
    gc0 = np.array([104, 79, 255]) / 255.0
    col = col + 3.0 * glow[..., None] * (0.5 * gc0)
    vign = 1.5 - np.hypot(uv[..., 0] * 2.35, uv[..., 1])
    col *= np.clip(np.maximum(vign, 0) ** 2, 0, 1)[..., None]
    hh, ss, vv = _rgb_to_hsv(col)
    hh = hh - 0.1 * (0.9 - (yy + 0.5) / h) ** 2
    col = _hsv_to_rgb(hh, ss, vv)
    return Image.fromarray((np.clip(col, 0, 1) * 255 + 0.5).astype(np.uint8), "RGB")


RENDERERS = {"coolbg": render_coolbg}
