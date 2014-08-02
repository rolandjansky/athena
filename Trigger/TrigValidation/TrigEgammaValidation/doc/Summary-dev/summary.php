<?php

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

    <title>
      <? echo $xmlp->data['validation'][0]['title'][0]['cdata'] ?>
    </title>

    <link rel="stylesheet" type="text/css" href="style.css" />
  </head>

  <body>
    <div id="title"><? echo $xmlp->data['validation'][0]['title'][0]['cdata'] ?><? if ($release != "") echo " for ".$release." (".date ("d/m/Y", filemtime($BaseAFS.$release)).")"; ?></div>
    <div class="content">
	<!-- Data -->
<?
if ( ($release == "") || ( ($item == "") && ($alias == "") ) ) {
  include("select.php");
} else if ( ($item != "") || ($alias != "") ) {
  $xmlp->DoMenu();
  $showItem = false;
  $father = false;

  $varToCompare = $item;
  $isAlias = false;
  if ($item == "") {
    $isAlias = true;
    $varToCompare = $alias;
  }
  $myArray = false;
  if (($myArray = $xmlp->GetMenuItem(array(&$xmlp->menu, ""), $varToCompare, $job, $isAlias)) == false) {
    echo "<h2>Could not load this item</h2>";
  } else {
    $showItem = $myArray[0];
    $father = $myArray[1];
  }

  ?><center><h3><? echo $showItem->title ?></h3></center>
  <br>
<?
  $showItem->generateContent($father);

}
?>
    </div>
  </body>

</html>
<?


?>

