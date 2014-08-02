<?

//$BaseAFS="/afs/cern.ch/user/a/andreza/www/Results/";
//$BaseURL="http://andreza.web.cern.ch/andreza/Results/";
$BaseAFS="/afs/cern.ch/atlas/project/RTT/Results/";
$BaseURL="http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/";

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
      if (isDir($BaseAFS.$entryA) && ($entryA != ".") && ($entryA != "..")) {
        $dB = opendir($BaseAFS . $entryA);
        while(false != ($entryB = readdir($dB))) {
          if ( (!ereg("devval", $entryB)) && (!ereg("dev", $entryB)) )
            continue;
          if (isDir($BaseAFS.$entryA."/".$entryB) && ($entryB != ".") && ($entryB != "..")) {
            $dC = opendir($BaseAFS.$entryA."/".$entryB);
            while (false != ($entryC = readdir($dC))) {
              if (isDir($BaseAFS.$entryA."/".$entryB."/".$entryC) && ($entryC != ".") && ($entryC != "..")) {
                $dD = opendir($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC);
                while (false != ($entryD = readdir($dD))) {
                  if (isDir($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC.'/'.$entryD) && ($entryD != ".") && ($entryD != "..")) {
                    if (ereg("rel_[0-9]", $entryA)) {
                      $dE = opendir($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC.'/'.$entryD);
                      while (false != ($entryE = readdir($dE))) {
                        if (isDir($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC.'/'.$entryD.'/'.$entryE) && ($entryE != ".") && ($entryE != "..")) {
                          // Now list!
                          $a = array_merge($a, $entryA."/".$entryB."/".$entryC."/".$entryD."/".$entryE." (".date ("d/m/Y", filemtime($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC.'/'.$entryD.'/'.$entryE)).")");
                        }
                      }
                      closedir($dE);
                    } else {
                      $a = array_merge($a, $entryA."/".$entryB."/".$entryC."/".$entryD." (".date ("d/m/Y", filemtime($BaseAFS.$entryA.'/'.$entryB.'/'.$entryC.'/'.$entryD)).")");
                    }
                  }
                }
                closedir($dD);
              }
            }
            closedir($dC);
          }
        }
        closedir($dB);
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
