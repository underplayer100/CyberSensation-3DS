// PC_REF : pilote ajouté au global.hx du mod par tools/pc_ref.py (retiré par "pc_ref.py remove").
// Lit claude_ref.txt (dossier du jeu) : une ligne "dossier|difficulté|id" par chanson.
// Joue chaque chanson en botplay et enregistre claude_ref/<id>_<ms>.png aux mêmes positions que l'autotest 3DS.
var refList:Array<String> = null;
var refIndex:Int = -1;
var refShot:Int = 0;
var refInSong:Bool = false;
var refWait:Float = 0;
var REF_AT:Array<Int> = [5000, 15000, 30000, 45000, 60000, 90000, 120000, 140000, 150000];

function refLog(m:String) {
    var f = "claude_ref/log.txt";
    File.saveContent(f, (FileSystem.exists(f) ? File.getContent(f) : "") + m + "\n");
}

function refNext() {
    refIndex++;
    refShot = 0;
    refInSong = false;
    refWait = 0;
    if (refIndex >= refList.length) {
        refLog("FINI");
        Sys.exit(0);
        return;
    }
    var p = refList[refIndex].split("|");
    refLog("chanson " + refList[refIndex]);
    try {
        PlayState.loadSong(p[0], p[1], false, false);
        PlayState.isStoryMode = false;
        PlayState.seenCutscene = true;
        FlxG.switchState(new PlayState());
    } catch (e:Dynamic) {
        refLog("  erreur chargement : " + e);
        refNext();
    }
}

function refCapture(name:String) {
    // BitmapData.draw fait planter ce build (rendu OpenGL) : lecture directe de l'image de la fenêtre
    // lue juste après le dessin de l'image (avant, le tampon est vide)
    refLog("  capture " + name);
    FlxG.signals.postDraw.addOnce(function() {
        var img = FlxG.stage.window.readPixels();
        if (img == null) { refLog("  readPixels : rien"); return; }
        File.saveBytes("claude_ref/" + name + ".png", img.encode("png"));
    });
}

function postUpdate(elapsed:Float) {
    FlxG.autoPause = false;
    if (refList == null) {
        if (!FileSystem.exists("claude_ref.txt")) { refList = []; return; }
        refList = [for (l in File.getContent("claude_ref.txt").split("\n")) if (StringTools.trim(l) != "") StringTools.trim(l)];
        if (!FileSystem.exists("claude_ref")) FileSystem.createDirectory("claude_ref");
        refWait = -3;
        // fenêtre hors des écrans : elle doit rester affichée (une fenêtre réduite n'est plus dessinée)
        var w = FlxG.stage.window;
        w.x = -4000;
        w.y = 0;
        w.minimized = false;
    }
    if (refList.length == 0) return;
    if (refIndex < 0) {  // laisse le jeu démarrer
        refWait += elapsed;
        if (refWait > 0) refNext();
        return;
    }
    var ps = Std.isOfType(FlxG.state, PlayState) ? PlayState.instance : null;
    if (ps == null) {
        // fin de chanson (retour au menu) ou état inattendu : chanson suivante
        refWait += elapsed;
        if (refInSong || refWait > 20) refNext();
        return;
    }
    refInSong = true;
    if (ps.subState != null) ps.closeSubState();
    for (s in ps.strumLines.members) s.cpu = true;
    ps.health = 1;
    var id = refList[refIndex].split("|")[2];
    if (refShot < REF_AT.length && Conductor.songPosition >= REF_AT[refShot]) {
        try refCapture(id + "_" + REF_AT[refShot]) catch (e:Dynamic) refLog("  erreur capture : " + e);
        refShot++;
        if (refShot >= REF_AT.length) refNext();
    }
}
// PC_REF FIN
