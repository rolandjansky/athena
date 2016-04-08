<html>
<head>
<style>
body{font-family:Trebuchet,sans-serif;background-color:#eeeeff;text-align:center;margin:0}
#admin{font-size:0.7em;font-weight:bold;letter-spacing:2px;background-color:#8888df;color:white;font-family:verdana,sans-serif;cursor:pointer}
#resultsTable{font-size:1.2em;font-weight:bold;border:1px gray solid;padding:2px;background-color:#bbbbff;margin:auto;}
#tableHeader td {background-color:#ccccff;padding:3px;text-align:center}
.tableRow td {background-color:#ddddff;font-weight:normal;font-size:0.7em;padding:5px}
.tableRow td a:link{text-decoration:underline; color:black;}   
.tableRow td a:visited{text-decoration:underline; color:black;}   
.tableRow td a:hover{text-decoration:none; color:orange;}
.tableRow td a:active{text-decoration:underline; color:black;}   
#tableHeader td a:link{text-decoration:underline; color:black;}   
#tableHeader td a:visited{text-decoration:underline; color:black;}   
#tableHeader td a:hover{text-decoration:none; color:orange;}
#tableHeader td a:active{text-decoration:underline; color:black;}   
#latestnews{position:fixed;bottom:0px;left:20%;right:20%;font-size:10pt;text-align:center;border:1px solid red;width:60%;font-family:sans-serif;background-color:#ddaaaa}
</style>
<title>ATLAS RunTimeTester Results</title>
<script src="prototype.js" type="text/javascript"></script>
<script src="effects.js" type="text/javascript"></script>
<script type="text/javascript">
<!--
function toggleThis(srcID, tgtID){
  var el = document.getElementById(srcID);
  if(document.getElementById(tgtID).style.display == 'none'){
    new Effect.Appear(tgtID);
    el.innerHTML = "Click me to Hide";
  }
  else{
    new Effect.DropOut(tgtID);
    el.innerHTML = "Click me to Show";
  }
}

function emphasize(){
  setTimeout("toggleThis('admin','topFrame')", 500);
}
// -->
</script>

</head>
<body>
<div id='admin' onclick="toggleThis('admin','topFrame');">Click me to Show</div>

<!------------------------------>
<!-- TOP FRAME WITH THE LINKS -->
<!------------------------------>
<div id='topFrame' style="display:none">
<script src="RTTpage1_LinksAtTopOfPage.js" type="text/JavaScript"></script>
</div>
<center>
<div id="latestnews"><b>Latest News:</b><br />            
8th Jan@09:30: Last night a new RTT version went into production, providing default data access to
the atlasgroupdisk CASTOR area. If your job is failing due to dataset access problems this is probably why.
</div>  
</center>
<br />

<?
$sortBy = $_GET['sort'];
?>

<?
include("tools.php");


$summaryFiles = openAndReadSummaryFilePaths('summaryFilePaths.txt');

if(is_null($summaryFiles) || sizeOf($summaryFiles)==0){
  echo "<h1>No data available</h1>";
  exit;
 }

$runs = array();

foreach($summaryFiles as $pathToSummFile){
  $overviewFile = dirname($pathToSummFile)."/overview.xml";
  if(!file_exists($pathToSummFile) 
     || !file_exists($overviewFile) 
     || filesize($pathToSummFile)==0
     || filesize($overviewFile)==0){continue;}

  $oNode = getRootNodeFromXMLfile($overviewFile);

  $run = array();

  // now grab info from that overview node
  $run['summFile']     = $pathToSummFile;
  $run['release']      = getTagContent($oNode, 'release');
  $run['branch']       = getTagContent($oNode, 'originalBranch');
  $run['platform']     = getTagContent($oNode, 'targetCMTCONFIG');
  $run['topProject']   = getTagContent($oNode, 'topProject');
  $run['otherProject'] = getTagContent($oNode, 'otherProject');
  $run['started']      = getTagContent($oNode, 'startTime1');
  $run['completed']    = getTagContent($oNode, 'endTime1');
  $run['nicos']        = getTagContent($oNode, 'nicosDate1');
  $run['status']       = getTagContent($oNode, 'statusText');

  $runs[] = $run;
  $oNode->unlink();
  unset($oNode);
}

$sortBy = $_GET['sort'];
$sortMap = array('byRelease'   => 'sortSummaryFilesByRelease',
                 'byBranch'    => 'sortSummaryFilesByBranch',
                 'byProject'   => 'sortSummaryFilesByProject',
                 'byPlatform'  => 'sortSummaryFilesByPlatform',
                 'byStarted'   => 'sortSummaryFilesByStarted',
                 'byCompleted' => 'sortSummaryFilesByCompleted');
 
 
if(trim($sortBy) == ''){$sortBy='byRelease';}
usort($runs, $sortMap[$sortBy]);
?>

<table id='resultsTable'>
<tr id='tableHeader'>
   <td>View By</td>
   <td><a href="page0.php?sort=byRelease">Release</a></td>
   <td><a href="page0.php?sort=byBranch">Branch</a></td>
   <td><a href="page0.php?sort=byPlatform">Platform</a></td>
   <td><a href="page0.php?sort=byProject">Project</a></td>
   <td><a href="page0.php?sort=byStarted">Started</a></td>
   <td><a href="page0.php?sort=byCompleted">Completed</a></td>
   <td>NICOS</td>
   <td>Status</td>
</tr>

<?
foreach($runs as $run){
  //Special cases
  if ($run['branch']=='bugfix'){$displayBranch = '14.2.X';}
  else if ($run['branch']=='val'){$displayBranch = '14.2.X-VAL';}
  else{$displayBranch=$run['branch'];}
  $byPackageLink = 'page1.php?xml='.$run['summFile'].'&viewBy=package';
  $byCPCPackageLink = 'page1.php?xml='.$run['summFile'].'&viewBy=cpcByPackage';
  $byCPCLink = 'page1.php?xml='.$run['summFile'].'&viewBy=cpc';
  echo "<tr class='tableRow'>";
  echo "<td><a href='".$byCPCLink."'>[CPC]</a>&nbsp;<a href='".$byCPCPackageLink."'>[CPC-by-package]</a>&nbsp;<a href='".$byPackageLink."'>[package]</a></td>";
  echo "<td>".$run['release']."</td>";
  echo "<td>".$displayBranch."</td>";
  echo "<td>".$run['platform']."</td>";
  if($run['otherProject']!=''){
     echo "<td>".$run['topProject']."/".$run['otherProject']."</td>";
  }
  else{
     echo "<td>".$run['topProject']."</td>";
  }   
  echo "<td>".$run['started']."</td>";
  echo "<td>".$run['completed']."</td>";
  echo "<td>".$run['nicos']."</td>";
  echo "<td>".$run['status']."</td>";
  echo "</tr>";
}

?>

</table>

</body>
</html>