<?

$BaseAFS="/afs/cern.ch/user/d/dferreir/webStableTrigEgammaValidation/Results/";
$BaseURL="http://info-StableTrigEgammaValidation.web.cern.ch/info-StableTrigEgammaValidation/";

//$BaseAFS="/afs/cern.ch/user/a/andreza/www/Results/";
//$BaseURL="http://andreza.web.cern.ch/andreza/Results/";
//$BaseAFS="/afs/cern.ch/atlas/project/RTT/Results/";
//$BaseURL="http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/";

$job = "";

// Let's find out what we are suposed to do:
if (isset($_GET["release"])) {
  $release = $_GET["release"];
} else {
  $release = "";
}

if (isset($_GET["item"])) {
  $item = $_GET["item"];
} else {
  $item = "";
}

if (isset($_GET["alias"])) {
  $alias = $_GET["alias"];
} else {
  $alias = "";
}

if (isset($_GET["file"])) {
  $content = $_GET["file"];
} else {
  $content = "content.xml";
}

include 'functions.php';
include 'menuclasses.php';
include 'parser.php';

$xmlp = new XMLParser($content);
$xmlp->parse();

$package = $xmlp->data['validation'][0]['package'][0]['cdata'];
$group = $xmlp->data['validation'][0]['group'][0]['cdata'];

?>

<html>
  <head>
    <title>TrigEgammaValidation Menu</title>
    <link rel="stylesheet" type="text/css" href="style.css" />
    <script language="javascript">
    <!--

    function showhide(what, img_what) {
      if (what.style.display == 'block') {
        what.style.display = 'none';
        img_what.src = "plus.gif";
      } else {
        what.style.display = 'block'
        img_what.src = "minus.gif";
      }
    }

    -->
    </script>
  </head>
  <body id = "MenuBody">
          <!-- Menu -->
          <div id="verticalMenu">
          <ul><div id="TopLevel"><li><span><img src="none.gif"></span><a target="MenuFrame" href="menu.php?file=<? echo $content ?>">List all releases</a></li></div>
            
          <!-- List releases? -->
            <?
function isDir($d) {
  $cwd = getcwd();
  $retV = false;
  if (@chdir($d)) {
    chdir($cwd);
    $retV = true;
  }
  return $retV;
}

if ($release == "") {
  $a = array();
  $dhA = opendir($BaseAFS);
  while ( ($entryA = readdir($dhA)) != false) {
    if ( (!ereg("rel_[0-9]", $entryA)) && (!ereg("^[0-9]{1,2}\.[0-9]{1,2}\.[0-9]{1,2}", $entryA)) ) {
      continue;
    } else {
      if (isDir($BaseAFS.$entryA)) {
        // Now list!
        $a = array_merge($a, $entryA." (".date ("d/m/Y", filemtime($BaseAFS.$entryA)).")");
      } else {
        $a = array_merge($a, $entryA." (".date ("d/m/Y", filemtime($BaseAFS.$entryA)).")");
      }
    }
  }
  closedir($dhA);

  foreach ($a as $r) {
    $iParen = strpos($r, '(');
    if ($iParen == FALSE) {
      $rNoParen = trim($r);
    } else {
      $rNoParen = trim(substr($r, 0, $iParen));
    }
    ?>
            <ul><div id="TopLevel"><li><span><img src="none.gif"></span><a href="<? echo calcget("menu.php", $rNoParen, "", "") ?>" target="MenuFrame"><? echo $r ?></a></li></div></ul>
      <?
  }

} else {
  $xmlp->DoMenu();

  $lvl = 0;
?>
<?
  $xmlp->ShowMenu(array(&$xmlp->menu), $lvl);
?>
</ul>
<?
}

?>
          </div>
  </body>
</html>
