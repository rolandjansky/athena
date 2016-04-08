<?
ob_start("ob_gzhandler"); 
header("Content-type: text/html; charset=utf-8");
?>

<html>
<head>
<style>
body{font-family:Trebuchet,sans-serif;background-color:#eeeeff;text-align:center;margin:0;color:black}
.pageHeader{font-size:1.5em;font-weight:bold;}
.sectionHeader{font-size:1em;font-weight:bold;}
.statusOK{color:green}
.statusBad{color:red}
.statusNeutral{color:black}
.statusRunning{color:brown}
#viewTitle{font-size:1.2em;font-weight:bold;letter-spacing:3px;font-family:verdana, sans-serif}
.statHeader{font-weight:bold;background-color:#bbbbff;text-align:left;font-size:0.9em}
.statTitle{font-weight:bold;background-color:#ccccff;font-size:0.8em;text-align:right}
.statEntry{font-weight:normal;font-size:0.8em;text-align:left}
#admin{font-size:0.7em;font-weight:bold;letter-spacing:2px;background-color:#8888df;color:white;font-family:verdana, sans-serif}
a.pageLink{font-family:arial;font-size:10pt;color:#777777;text-decoration:underline}
a.pageLink:hover{font-family:arial;font-size:10pt;color:black;text-decoration:none}
a.switchView{font-family:arial;font-size:0.7em;color:black;text-decoration:underline}
a.switchView:hover{font-family:arial;color:black;text-decoration:none}
span.stats{font-family:arial;font-size:0.8em;color:black;text-decoration:none}
#tableHeader td {background-color:#bbbbff;padding:3px;text-align:center;font-weight:bold}
.packageTableHeader{background-color:#bbbbbb;padding:3px;text-align:left;font-weight:bold;font-size:1.3em;color:#666666}
.frameworkHeader{background-color:#ddddff;padding:3px;text-align:center;font-size:1.0em;font-weight:bold}
.cpcTableHeader{background-color:#ddddff;text-align:right;font-weight:normal;}
.cpcTableColHeader{background-color:#aaaaff;text-align:center;font-weight:bold}
.minderEntry{background-color:#eeeeff;padding:3px;text-align:left;font-size:0.8em;font-weight:normal}
.stats{font-weight:normal;font-size:0.8em}
.tableRow td {background-color:white;font-weight:normal;font-size:0.7em;padding:5px;text-align:center}
.tableRow td a:link{text-decoration:underline; color:black;}   
.tableRow td a:visited{text-decoration:underline; color:black;}   
.tableRow td a:hover{text-decoration:none; color:orange;}
.tableRow td a:active{text-decoration:underline; color:black;}
.tableRow td span{background-color:white;font-weight:normal;font-size:0.8em;padding:5px;text-align:center}
.tableRow2 td {background-color:#dddddd;font-weight:bold;font-size:0.7em;padding:5px;text-align:center}
.packageRow td {background-color:#ddddff;font-weight:normal;font-size:0.7em;padding:5px;text-align:center}
.packageRow td a:link{text-decoration:underline; color:black;}   
.packageRow td a:visited{text-decoration:underline; color:black;}   
.packageRow td a:hover{text-decoration:none; color:orange;}
.packageRow td a:active{text-decoration:underline; color:black;}
.showJobs{font-weight:normal;font-size:0.8em;text-decoration:underline;cursor:pointer;}
#tableHeader td a:link{text-decoration:underline; color:black;}   
#tableHeader td a:visited{text-decoration:underline; color:black;}   
#tableHeader td a:hover{text-decoration:none; color:orange;}
#tableHeader td a:active{text-decoration:underline; color:black;} 
</style>
<script src="prototype.js" type="text/javascript"></script>
<script src="effects.js" type="text/javascript"></script>
<script type="text/javascript">
<!--
function toggleThis(srcID, tgtID){
  var el = document.getElementById(srcID);
  if(document.getElementById(tgtID).style.display == 'none'){
    new Effect.Appear(tgtID);
    el.innerHTML = "Click to Hide";
  }
  else{
     new Effect.DropOut(tgtID);
     el.innerHTML = "Click to Show";
  }
}

function toggleStuff(buttonID, tgtID){
  var abutton = document.getElementById(buttonID);

  if (document.getElementById(tgtID).style.display == 'none'){
    new Effect.Appear(tgtID);

    abutton.innerHTML = '<font color="red">[Hide]</font>';

  }
  else{
    new Effect.DropOut(tgtID);
    abutton.innerHTML = '[Show]';
  }
}
// -->
</script>
</head>
<body>
<div id='admin' onclick="toggleThis('admin','topFrame');">Click to Show</div>
<div id='topFrame' style="display:none">
<script src="RTTpage1_LinksAtTopOfPage.js" type="text/JavaScript"></script>
<script src="messageOfTheDay.js" type="text/JavaScript"></script>
</div>
<br />

<?
include("tools.php");

// Grab the GET parameters in the URL
$xmlFile = $_GET['xml'];

if (!file_exists($xmlFile)){
  echo "<br /><b><center>No data available yet.</center></b>";
  return;
}

//Select specific packages to view
$view = $_GET['viewBy'];
$packagesToView = trim($_GET['package']);

//Select specific states to view
$packageStatesToView = trim($_GET['status']);

//Open up the XML file
$rootNode = getRootNodeFromXMLfile($xmlFile);
$overviewNodes = $rootNode->get_elements_by_tagname('overview');
$overviewNode = $overviewNodes[sizeOf($overviewNodes)-1]; //0th one is ATN summary, ignore

//////////////////////////////////////////////////////////////////////////
// SECTION 1: INFORMATION AT TOP OF THE PAGE
//////////////////////////////////////////////////////////////////////////

// Link to RTTSummary.txt
$diagLink = explode('.xml', $xmlFile);
$diagLink = $diagLink[0].".txt";
echo "<a class='pageLink' href='".$diagLink."'>RTT developer run diagnostics</a>\n";
echo "<br /><br />\n\n";


$releaseName = getTagContent($overviewNode, 'releaseName');
$runType     = getTagContent($overviewNode, 'runType');
$platform    = getTagContent($overviewNode, 'targetCMTCONFIG');
$topProj     = getTagContent($overviewNode, 'topProject');
$otherProj   = getTagContent($overviewNode, 'otherProject');
echo "<div class='pageHeader'>";
if($otherProj==''){
  echo $releaseName."/".$runType." --- ".$platform." --- ".$topProj;
}
else{
  echo $releaseName."/".$runType." --- ".$platform." --- ".$topProj.'/'.$otherProj;
}
echo "</div>\n\n";
echo "<br />\n\n";

// Now the RTT versions at top of the page
$rttSrc = getTagContent($overviewNode, 'rttSrcVersion');
$rttLib = getTagContent($overviewNode, 'rttLibVersion');
echo "<div class='sectionHeader'>";
echo "RTT src code: ".$rttSrc;
echo "<br />";
echo "RTT library tools: ".$rttLib;
echo "</div>\n\n";
echo "<br />\n\n";

// Find the package nodes, and count them
$packages = $rootNode->get_elements_by_tagname('package');

///////////////////////////////////////////////////////////
////   FILTER THE PACKAGES
///////////////////////////////////////////////////////////

function filter($packages, $what, $allowedValues){
  $acceptedPackages = array();
  foreach($packages as $package){
    $value = getTagContent($package, $what);
    if (in_array($value, $allowedValues)){$acceptedPackages[]=$package;}
  }
  return $acceptedPackages;
}

// Does the user wish to see all or some packages?
if ($packagesToView != ''){
  $userPackages = split('::', $packagesToView);
  $packages = filter($packages, 'packageName', $userPackages);
 }


// Does the user wish to see some packages statii or all?
if ($packageStatesToView != ''){
  if ($packageStatesToView!='all'){
    $states = split('::',$packageStatesToView);
    $packages = filter($packages, 'packageStatus', $states);
  }
}
else{ // default is to show only running and done states
  $packages = filter($packages, 'packageStatus', array('running', 'done'));
}

//////////////////////////////////////////////////////////

echo "<span class='pageHeader'>Packages found by the RTT: ".sizeOf($packages)."</span>\n";
$currentURL = $_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"];
$currentURLtoks = explode('page1.php', $currentURL);
$currentURL = 'page1.php'.$currentURLtoks[1];
if($packageStatesToView=='all'){
  echo "<a class='switchView' href='".$currentURL."&status=running::done'>[Show only running/done packages]</a>\n\n";
}
else{
  echo "<a class='switchView' href='".$currentURL."&status=all'>[Show all packages]</a>\n\n";
}
echo "<br /><br />\n\n";

$sText  = getTagContent($overviewNode, 'statusText');
$jobsOK = extractFromStatusText($sText, 'ok');
$done   = extractFromStatusText($sText, 'done');
$total  = extractFromStatusText($sText, 'total');

echo "<span class='stats'>[Jobs OK/Done/Total: ".$jobsOK."/".$done."/".$total."--- Pkgs with all jobs OK: ".getTagContent($overviewNode, 'nPackagesAllJobsOK')."/".getTagContent($overviewNode, 'nPackages')."]</span>";
echo "<br /><br />";

//////////////////////////////////////////////////////////////////////////
// SECTION 2: LOAD THE VIEW
//////////////////////////////////////////////////////////////////////////

//set as default, if not set
if ($view == '') $view = 'package'; 

if ($view == 'cpcByPackage'){
  // Show the package view options
  echo "<div id='viewTitle'>CPC-by-Package View</div>";
  echo " [<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=package'>Switch to package view</a>]";
  echo "[<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=cpc'>Switch to CPC view</a>]<br /><br />";

  // Show the package states options
  echo "<div id='viewTitle'>View</div>";

  doByCPCPackageView($packages);
}
else if($view == 'package'){
  echo "<div id='viewTitle'>Package View</div>";
  echo "[<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=cpc'>Switch to CPC view</a>]";
  echo " [<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=cpcByPackage'>Switch to CPC-by-package view</a>]<br /><br />";
  doByPackageView($packages);
}
else if($view == 'cpc'){
  echo "<div id='viewTitle'>CPC View</div>\n";
  echo "[<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=package'>Switch to package view</a>]\n";
  echo "[<a class='switchView' href='page1.php?xml=".$xmlFile."&viewBy=cpcByPackage'>Switch to CPC-by-package view</a>]\n\n<br /><br />\n\n";
  doByCPCView($packages);
}

///////////////////////////////////////////////////////////////////
// VIEW 1: DO THE TRUE BY-CPC VIEW
///////////////////////////////////////////////////////////////////
function doByCPCView($packages){
  global $xmlFile;
  $sortBy = $_GET['sortBy'];;

  function sortByCPC($packages){
    $byCPC = array();
    foreach($packages as $package){
      $packageName = getTagContent($package, 'packageName');
      $minders = $package->get_elements_by_tagname('minder');

      foreach($minders as $minder){
	$classList     = getMultiTagContent($minder, 'displayClass');
	$processList   = getMultiTagContent($minder, 'displayProcess');
	$componentList = getMultiTagContent($minder, 'displayComponent');

	foreach($classList as $class){
	  foreach($processList as $process){
	    foreach($componentList as $component){

	      $key = "[".$class."] [".$process."] [".$component."]";
	      if (array_key_exists($key, $byCPC)){
		$dict = $byCPC[$key]; // dictionary keyed on package name
		if (array_key_exists($packageName, $dict)){
		  $minderList = $dict[$packageName];
		  $minderList[] = $minder;
		  $dict[$packageName] = $minderList;
		  $byCPC[$key] = $dict;
		}
		else{
		  $dict[$packageName] = array($minder);
		  $byCPC[$key] = $dict;
		}
	      }
	      else{
		$val = array($packageName => array($minder));
		$byCPC[$key] = $val;
	      }
	    }
	  }
	}
      }
    }
    
    return $byCPC;
  }

  // --------------------------------------------------

  function arrayOnlyHasSuccesses($theArray){
    foreach($theArray as $entry){if (trim($entry)!='success') return false;}
    return true;
  }
  
  // --------------------------------------------------

  function cpcStatus($dict){ // dict = {packageName:list of minders}
    $totNumbJobs  = 0;
    $totNumbPacks = 0;
    $totNotOKJobs  = 0;
    $statii = array();
    foreach($dict as $pName => $minders){
      $totNumbPacks++;
      foreach($minders as $minder){
	$status = getTagContent($minder, 'status');
	$statii[] = $status;
	$totNumbJobs++;
	if ($status != 'success'){$totNotOKJobs++;}
      }
    }

    if(in_array('running', $statii)) return array('status' => 'ongoing', 'jobsInCPC' => $totNumbJobs, 'packagesInCPC' => $totNumbPacks, 'notOKjobs' => $totNotOKJobs);
    else if (arrayOnlyHasSuccesses($statii)) return array('status' => 'OK', 'jobsInCPC' => $totNumbJobs, 'packagesInCPC' => $totNumbPacks, 'notOKjobs' => 0);
    return array('status' => 'FAIL', 'jobsInCPC' => $totNumbJobs, 'packagesInCPC' => $totNumbPacks, 'notOKjobs' => $totNotOKJobs);
  }

  // --------------------------------------------------

  function colorizeCPCStatus($statusText){
    if (trim($statusText)=='OK') return '<span style="color:#00ff00;font-weight:bold">OK</span>';
    if (trim($statusText)=='FAIL') return '<span style="color:#ff0000;font-weight:bold">FAIL</span>';
    return '<font color="black">ongoing</font>';
  }

  // --------------------------------------------------

  $cpcCategories = sortByCPC($packages);

  // Remove non-CPC jobs
  $nonCPCKey = '[] [] []';
  if(array_key_exists($nonCPCKey, $cpcCategories)){
    unset($cpcCategories[$nonCPCKey]);
  }


  if (sizeOf(array_keys($cpcCategories)) == 0){
    echo "<div style='font-weight:bold;'>There are no jobs with CPC values.</div>";
    return;
  }


  // Now show the table of CPC values
  echo "<table width='60%' border='0' align='center'>\n\n";

  $colLink = "page1.php?xml=".$xmlFile."&viewBy=cpc&sortBy=";

  echo "<tr id='tableHeader' align='center'>\n";
  echo "   <td></td>\n";
  echo "   <td><a href='".$colLink."class'>Class</a></td>\n";
  echo "   <td><a href='".$colLink."process'>Process</a></td>\n";
  echo "   <td><a href='".$colLink."component'>Component</a></td>\n";
  echo "   <td>Status</td>\n";
  echo "   <td>Summary</td>\n";
  echo "</tr>\n\n";
  
  // Table Column sorting
  $sortMap = array('class'     => 'sortCPCByClass',
		   'process'   => 'sortCPCByProcess',
		   'component' => 'sortCPCByComponent');
  
  //default is sort by release
  if ($sortBy == ''){$sortBy='class';}
  $cpcNames = array_keys($cpcCategories);
  usort($cpcNames, $sortMap[$sortBy]);

  // Now loop over the CPC categories (rows in table)
  foreach($cpcNames as $cpcName){
    $dict = $cpcCategories[$cpcName]; // dict = {packageName:list of minders}
    //foreach($cpcCategories as $cpcName => $dict){ // dict = {packageName:list of minders}

    $cpcToks = explode(' ', $cpcName);
    $val = cpcStatus($dict);

    $buttonID = $cpcName;
    $jobsID   = "jobs_".$cpcName;

    $show_Them = "<span style='text-decoration:underline' id='".$buttonID."' onclick='toggleStuff(\"".$buttonID."\",\"".$jobsID."\");'>[Show]</span>";
    echo "<tr class='cpcTableHeader'>\n";
    echo "   <td align='center'>".$show_Them."</td>\n";
    echo "   <td>".$cpcToks[0]."</td>\n";
    echo "   <td>".$cpcToks[1]."</td>\n";
    echo "   <td>".$cpcToks[2]."</td>\n";
    echo "   <td align='center'>".colorizeCPCStatus($val['status'])."</td>";

    if($val['status'] == 'ongoing'){
      echo "   <td class='statEntry'>".$val['jobsInCPC']." jobs total, ".$val['packagesInCPC']." packages total</td>\n";
    }
    else if($val['status'] == 'OK'){
      echo "   <td class='statEntry'>".$val['jobsInCPC']."/".$val['jobsInCPC']." jobs OK; ".$val['packagesInCPC']." packages total</td>\n";
    }
    else{
      echo "   <td class='statEntry'>".$val['notOKjobs']."/".$val['jobsInCPC']." jobs not OK; ".$val['packagesInCPC']." packages total</td>\n";
    }

    echo "</tr>\n\n";

    echo "<tr>\n";
    echo "   <td colspan='3'>\n";
    echo "      <div  align='center' style='display:none' id='".$jobsID."'>\n";

    echo "         <table width='80%'>\n";

    foreach($dict as $pName => $minders){

      echo "            <tr class='packageTableHeader'><td colspan='10'>".$pName."</td></tr>\n";
      echo "            <tr class='tableRow2'><td>Name</td><td>JobGroup</td><td>Job status</td><td>Tests</td><td>Datasets</td><td>Hash</td></tr>";
      foreach($minders as $minder){

	$jobName = getTagContent($minder, 'jobName');
	$escapedJobName = rawurlencode($jobName);
	$displayName = getTagContent($minder, 'jobDisplayName');
	if ($displayName == ''){$displayName = $jobName;}
	$jobID   = getTagContent($minder, 'jobID');
	$group   = getTagContent($minder, 'jobGroup');

	// ==================================================
	// Job doc/link stuff
	// ==================================================
	
	/*
	$jobDocString = getTagContent($minder, 'jobDocString');
	$jobDocURL    = getTagContent($minder, 'jobDocURL');

	if(sizeOf($jobDocString)!=0){
	  $docString ='';
	  if (!isEmptyTag($jobDocString)){
	    $docString = trim($doc->get_content());
	  }
	}

	if(sizeOf($jobDocURL)!=0){
	  $doclink = $jobDocURL;
	  $docURL = '';
	  if (!isEmptyTag($doclink)){
	    $docURL = "<a class='pageLink' href='".trim($doclink->get_content())."'>More info</a>";
	  }
	}
	*/
	// ==================================================
	// Hash stuff
	// ==================================================

	$hash   = getTagContent($minder, 'hashString');
	if ($hash==''){$hash = 'n/a';}
	// Now add the datasets
	$datasetsList = $minder->get_elements_by_tagname('datasets');
	$datasetsTag  = $datasetsList[0];
	$datasets_holder = '';
	if (isEmptyTag($datasetsTag)){
	  $datasets = "n/a";
	}
	else{
	  $datasets = "";
	  foreach($datasetsTag->get_elements_by_tagname('dataset') as $dataset){
	    $datasets .= trim($dataset->get_content())."\n";
	  }

	  $datasetsButtonID = $jobID."_datasetsButtonID";
	  $datasetsID = $jobID."_datasetsID";
	  $datasets_holder = "<span style='text-decoration:underline' id='".$datasetsButtonID."' onclick='toggleStuff(\"".$datasetsButtonID."\",\"".$datasetsID."\");'>[Show]</span>";
	}
	
	// Job status
	$status = getTagContent($minder, 'status');
	//$jobsStatii[] = $status; //append to array

	$statusClass = 'statusBad';
	if ($status == 'success') $statusClass = 'statusOK';
	if ($status == 'running') $statusClass =  'statusRunning';
	
	// Post processing combined status
	$postProcRes = getTagContent($minder, 'postProcessingResult');
	$postProcClass = 'statusBad';
	if ($postProcRes == 'success') $postProcClass = 'statusOK';
	if ($postProcRes == 'running') $postProcClass =  'statusRunning';
	if ($postProcRes == 'no tests') $postProcClass =  'statusNeutral';
      
	// Ref. release used
	$refRel = getTagContent($minder, 'refRelease');

	// =====================================================
	// Now create the table row
	// =====================================================

	echo "\n<tr class='tableRow'>\n";

	echo "<td valign='top'><a href='page2.php?xml=".$xmlFile."&package=".$pName."&job=".$escapedJobName."&id=".$jobID."'>".$displayName."</a><br /><br /><span>[id:".$jobID."]</span><br /><span>".$docString."</span></td>\n";
	echo "<td valign='top'>".$group."</td>\n";
	echo "<td valign='top' class='".$statusClass."'>".$status."</td>\n";
	echo "<td valign='top' class='".$postProcClass."'>".$postProcRes."</td>\n";

	if ($datasets_holder==''){
	  echo "<td valign='top'>n/a</td>";
	}
	else{
	  echo "<td valign='top'>";
	  echo $datasets_holder;
	  echo "<div align='center' style='display:none' id='".$datasetsID."'>".$datasets."</div>";
	  echo "</td>\n";
	}

	echo "<td valign='top'>".$hash."</td>\n";
	//echo "<td valign='top'>".$refRel."</td>\n";
	echo "</tr>";
      }
      
    }

    echo "</table>\n"; // end of minder table      
    echo "</div></td></tr>";

  }

  echo "</table>";

}

///////////////////////////////////////////////////////////////////
// VIEW 2: DO THE BY-CPC (PACKAGE) VIEW
///////////////////////////////////////////////////////////////////
function doByCPCPackageView($packages){
  global $xmlFile;

  function sortByCPC($minders){
    //Sort minders into CPC categories for this package
    $cpcHash = array();
    foreach($minders as $minder){
      $class     = getTagContent($minder, 'displayClass');
      $process   = getTagContent($minder, 'displayProcess');
      $component = getTagContent($minder, 'displayComponent');
      $key = "[".$class."] [".$process."] [".$component."]";
      if (array_key_exists($key, $cpcHash)){
	$val = $cpcHash[$key];
	$val[] = $minder;
	$cpcHash[$key] = $val;
      }
      else{
	$cpcHash[$key] = array($minder);
      }
    }

    return $cpcHash;
  }

  function sortByCPC2($minders){
    //Sort minders into CPC categories for this package
    $cpcHash = array();
    foreach($minders as $minder){
      $classTagsCont     = getMultiTagContent($minder, 'displayClass');
      $processTagsCont   = getMultiTagContent($minder, 'displayProcess');
      $componentTagsCont = getMultiTagContent($minder, 'displayComponent');
      
      foreach($classTagsCont as $class){
	foreach($processTagsCont as $process){
	  foreach($componentTagsCont as $component){
	    $key = "[".$class."] [".$process."] [".$component."]";
	    if (array_key_exists($key, $cpcHash)){
	      $val = $cpcHash[$key];
	      $val[] = $minder;
	      $cpcHash[$key] = $val;
	    }
	    else{
	      $cpcHash[$key] = array($minder);
	    }
	  } 
	}
      }
    }

    return $cpcHash;
  }

  function arrayOnlyHasSuccesses($theArray){
    foreach($theArray as $entry){if (trim($entry)!='success') return false;}
    return true;
  }
  
  function cpcCategoryStatus($minders){
    $statii = array();
    foreach($minders as $minder){
      $statii[] = getTagContent($minder, 'status');
    }

    if(in_array('running', $statii)) return 'ongoing';
    else if (arrayOnlyHasSuccesses($statii)){ return 'OK';}
    return 'FAIL';
  }

  function colorizeCPCStatus($statusText){
    if (trim($statusText)=='OK') return '<span style="color:#00ff00;font-weight:bold">OK</span>';
    if (trim($statusText)=='FAIL') return '<span style="color:#ff0000;font-weight:bold">FAIL</span>';
    return '<font color="black">ongoing</font>';
  }

  function colorizePackageStat($stat){
    if(trim($stat)!='running' && trim($stat)!='done'){
      return '<span style="color:red">'.$stat.'</span>';
    }
    return $stat;
  }

  function countNotOKJobs($minders){
    $tot = 0;
    foreach($minders as $minder){
      if(getTagContent($minder, 'status') != 'success'){$tot++;}
    }
    return $tot;
  }

  function countNotOKTests($minders){
    $tot=0;
    foreach($minders as $minder){
      if(getTagContent($minder, 'postProcessingResult') != 'success'){$tot++;}
    }
    return $tot;
  }

  // Let's make the table
  echo "\n<table align='center' border='0' cellspacing='3' cellpadding='3' width='60%'>";

  //Sort and loop through the packages
  usort($packages, "sortPackagesByName");

  foreach($packages as $package){

    $packageName = getTagContent($package, 'packageName');
    $packageTag   = getTagContent($package, 'packageTag');
    $packageStat  = getTagContent($package, 'packageStatus');

    $tag = explode($packageName,$packageTag);
    echo "\n<tr class='packageTableHeader'>";
    echo "<td colspan='12'>".$packageName;
    echo "<span style='font-weight:normal;font-size:0.9em'>[".$tag[1]."]</span>";
    echo "<div style='font-weight:normal;font-size:0.8em;'>[".colorizePackageStat($packageStat)."]</div>";
    echo "</td></tr>";

    $minders = $package->get_elements_by_tagname('minder');

    # $cpcCategories = sortByCPC($minders);
    $cpcCategories = sortByCPC2($minders);

    //Remove non-CPC jobs
    $nonCPCKey = '[] [] []';
    if(array_key_exists($nonCPCKey, $cpcCategories)){
      unset($cpcCategories[$nonCPCKey]);
    }

    // Loop through the CPC categories
    foreach($cpcCategories as $cpcName=>$mindersInCPC){ //cpcValue is an array of minder nodes

      $buttonID = getTagContent($package, 'packageName')."_".$cpcName;
      $jobsID   = "jobs_".getTagContent($package, 'packageName')."_".$cpcName;

      // Print out the name of this CPC category
      echo "<tr class='cpcTableHeader'>\n";
      echo "<td style='text-decoration:underline' id='".$buttonID."' onclick='toggleStuff(\"".$buttonID."\",\"".$jobsID."\");'>[Show]</td>";
      echo "<td>".$cpcName."</td>";

      // Print out the status of this CPC category
      $cpcStatus = cpcCategoryStatus($mindersInCPC);
      echo "<td align='center'>".colorizeCPCStatus($cpcStatus)."</td>";

      // Print out the number of jobs in this CPC category
      $jobText = 'jobs';
      if (sizeOf($mindersInCPC) == 1) $jobText = 'job';

      if($cpcStatus == 'ongoing'){
	echo "<td align='center'>ongoing</td>";
      }
      else if($cpcStatus == 'FAIL'){
	$failedJobsInCPC = countNotOKJobs($mindersInCPC);	
	echo "<td align='center'>".$failedJobsInCPC."/".sizeOf($mindersInCPC)." ".$jobText." not OK</td>";
      }
      else{
	echo "<td align='center'>".sizeOf($mindersInCPC)."/".sizeOf($mindersInCPC)." ".$jobText." OK</td>";
      }
      
      echo "</tr>";

      // Print out a table of jobs in this CPC category
      echo "<tr><td colspan='3'><div  align='center' style='display:none' id='".$jobsID."'>";

      $jobsStatii = array();

      echo "<table>\n";

      echo "<tr class='tableRow2'><td>Name</td><td>JobGroup</td><td>Job status</td><td>Tests</td><td>Datasets</td><td>Hash</td></tr>";
      foreach($mindersInCPC as $minder){
	$jobName = getTagContent($minder, 'jobName');
	$escapedJobName = rawurlencode($jobName);
	$displayName = getTagContent($minder, 'jobDisplayName');
	if ($displayName == ''){$displayName = $jobName;}
	$jobID   = getTagContent($minder, 'jobID');
	$group   = getTagContent($minder, 'jobGroup');
	$hash   = getTagContent($minder, 'hashString');
	if ($hash==''){$hash = 'n/a';}
	// Now add the datasets
	$datasetsList = $minder->get_elements_by_tagname('datasets');
	$datasetsTag  = $datasetsList[0];
	$datasets_holder = '';

	if (isEmptyTag($datasetsTag)){
	  $datasets = "n/a";
	}
	else{
	  $datasets = "";
	  foreach($datasetsTag->get_elements_by_tagname('dataset') as $dataset){
	    $datasets .= trim($dataset->get_content())."\n";
	  }

	  $datasetsButtonID = $jobID."_datasetsButtonID";
	  $datasetsID = $jobID."_datasetsID";
	  $datasets_holder = "<span style='text-decoration:underline' id='".$datasetsButtonID."' onclick='toggleStuff(\"".$datasetsButtonID."\",\"".$datasetsID."\");'>[Show]</span>";
	}

	// Job status
	$status = getTagContent($minder, 'status');
	$jobsStatii[] = $status; //append to array

	$statusClass = 'statusBad';
	if ($status == 'success') $statusClass = 'statusOK';
	if ($status == 'running') $statusClass =  'statusRunning';

	// Post processing combined status
	$postProcRes = getTagContent($minder, 'postProcessingResult');
	$postProcClass = 'statusBad';
	if ($postProcRes == 'success') $postProcClass = 'statusOK';
	if ($postProcRes == 'running') $postProcClass =  'statusRunning';
	if ($postProcRes == 'no tests') $postProcClass =  'statusNeutral';

	// Ref. release used
	$refRel = getTagContent($minder, 'refRelease');

	// Now create the table row
	echo "\n<tr class='tableRow'>\n";
	//echo "<td valign='top'><a href='page2.php?xml=".$xmlFile."&package=".$packageName."&job=".$jobName."&id=".$jobID."'>".$displayName."</td>\n";
	echo "<td valign='top'><a href='page2.php?xml=".$xmlFile."&package=".$packageName."&job=".$escapedJobName."&id=".$jobID."'>".$displayName."</a><br /><br /><span>[id:".$jobID."]</span></td>\n";

	//echo "<td valign='top'>".$jobID."</td>\n";
	echo "<td valign='top'>".$group."</td>\n";
	echo "<td valign='top' class='".$statusClass."'>".$status."</td>\n";
	echo "<td valign='top' class='".$postProcClass."'>".$postProcRes."</td>\n";

	if ($datasets_holder == ''){
	  echo "<td valign='top'>n/a</td>";
	}
	else{
	  echo "<td valign='top'>";
	  echo $datasets_holder;
	  echo "<div align='center' style='display:none' id='".$datasetsID."'>".$datasets."</div>";
	  echo "</td>\n";
	}

	echo "<td valign='top'>".$hash."</td>\n";
	//echo "<td valign='top'>".$refRel."</td>\n";
	echo "</tr>";

      }

      echo "</table>\n"; // end of minder table      
      echo "</div></td></tr>";
    }
  }

  echo "</table>";
  
}

///////////////////////////////////////////////////////////////////
// VIEW 3: DO THE BY-PACKAGE VIEW
///////////////////////////////////////////////////////////////////
function doByPackageView($packages){
  global $xmlFile;

  function sortByFramework($minders){
    $atnMinders = array();
    $rttMinders = array();
    foreach($minders as $minder){
      $group = getTagContent($minder, 'jobGroup');
      if ($group=='AthenaATN'){$atnMinders[] = $minder;}
      else {$rttMinders[] = $minder;}
    }
    return array($rttMinders, $atnMinders);
  }

  function countDoneJobs($minders){
    $tot = 0;
    foreach($minders as $minder){
      if(hasChildTag($minder, 'done')){$tot++;}
    }
    return $tot;
  }

  function countOKJobs($minders){
    $tot = 0;
    foreach($minders as $minder){
      if(getTagContent($minder, 'status') == 'success'){$tot++;}
    }
    return $tot;
  }

  function countBadJobs($minders){
    $tot = 0;
    foreach($minders as $minder){
      if(getTagContent($minder, 'status') == 'error'){$tot++;}
    }
    return $tot;
  }

  function countOKTests($minders){
    $tot = 0;
    foreach($minders as $minder){
      if(getTagContent($minder, 'postProcessingResult') == 'success'){$tot++;}
    }
    return $tot;
  }

  function colorizePackageStat($stat){
    if(trim($stat)!='running' && trim($stat)!='done'){
      return '<span style="color:#880000">'.$stat.'</span>';
    }
    return $stat;
  }

  echo "<table align='center' width='60%'>";

  // Loop over each package
  usort($packages, "sortPackagesByName");
  foreach($packages as $package){
    $packageName = getTagContent($package, 'packageName');
    $packageCont = getTagContent($package, 'containerPackage');
    $packageTag   = getTagContent($package, 'packageTag');
    $packageStat  = getTagContent($package, 'packageStatus');

    $cvsURL = cvsReposURL($packageCont, $packageTag);
    $tag = explode($packageName,$packageTag);
    echo "\n<tr class='packageTableHeader'><td colspan='12'>".$packageName;
    echo " <span style='font-weight:normal;font-size:0.8em'>[".$tag[1]."]</span>";
    echo "<span><a style='font-weight:normal;font-size:0.6em' href='".$cvsURL."'>[CVS]</a></span>";
    echo " <div style='font-weight:normal;font-size:0.8em;'>[".colorizePackageStat($packageStat)."]</div></td></tr>";

    // Get the jobs in this package
    $minders = $package->get_elements_by_tagname('minder');
  
    // Split these jobs into ATN and RTT jobs
    $sortedMinders = sortByFramework($minders);
    $rttMinders = $sortedMinders[0];
    $atnMinders = $sortedMinders[1];

    $rttButtonID = "rttButton_".$packageName;
    $atnButtonID = "atnButton_".$packageName;
    $rttJobsID   = "rttJobs_".$packageName;
    $atnJobsID   = "atnJobs_".$packageName;

    $totRTTjobs  = sizeOf($rttMinders);
    $doneRTTjobs = countDoneJobs($rttMinders);
    $okRTTjobs   = countOKJobs($rttMinders);
    $badRTTjobs  = countBadJobs($rttMinders);
    $doneAthJobs = $okRTTjobs+$badRTTjobs;
    $okRTTtests  = countOKTests($rttMinders);
    $totRTTtests = $okRTTjobs;
    echo "<tr class='frameworkHeader'>";
    echo "<td>RTT jobs<br /><span id='".$rttButtonID."' onclick='toggleStuff(\"".$rttButtonID."\",\"".$rttJobsID."\");' class='showJobs'>[Show]</span></td>";
    echo "<td class='stats'>";
    echo "<table width='100%' border='0'>";
    echo "<tr>";
    echo "<td class='statHeader'>Run by RTT</td>";
    echo "<td class='statTitle'>Jobs OK:&nbsp;</td><td class='statEntry'>".$okRTTjobs."/".$doneAthJobs."</td>";
    echo "<td class='statTitle'>Jobs Done:&nbsp;</td><td class='statEntry'>".$doneRTTjobs."/".$totRTTjobs."</td>";
    echo "<td class='statTitle'>Tests OK:&nbsp;</td><td class='statEntry'>".$okRTTtests."/".$totRTTtests."</td>";
    echo "</tr>";
    echo "</table>";
    echo "</td>";
    echo "</tr>";

    // Print out a table of jobs in this CPC category
    echo "<tr><td colspan='3'><div  align='center' style='display:none' id='".$rttJobsID."'>";

    $jobsStatii = array();

    echo "<table>\n";
    echo "<tr class='tableRow2'><td>Name</td><td>JobGroup</td><td>Job status</td><td>PostProcessing status</td><td>Datasets</td><td>Hash</td></tr>";

    foreach($rttMinders as $minder){
      $jobName = getTagContent($minder, 'jobName');
      $escapedJobName = rawurlencode($jobName);

      $displayName = getTagContent($minder, 'jobDisplayName');
      if ($displayName == ''){$displayName = $jobName;}
      $jobID   = getTagContent($minder, 'jobID');
      $group   = getTagContent($minder, 'jobGroup');
      $hash    = getTagContent($minder, 'hashString');
      if ($hash==''){$hash = 'n/a';}

      $stateHistory = getTagContent($minder, 'stateHistory');
      $currentState = getTagContent($minder, 'state');


      // Now add the datasets
      $datasetsList = $minder->get_elements_by_tagname('datasets');
      $datasetsTag  = $datasetsList[0];
      $datasets_holder = '';

      if (isEmptyTag($datasetsTag)){
	$datasets = "n/a";
      }
      else{
	$datasets = "";
	foreach($datasetsTag->get_elements_by_tagname('dataset') as $dataset){
	  $datasets .= trim($dataset->get_content())."\n";
	}

	$datasetsButtonID = $jobID."_datasetsButtonID";
	$datasetsID = $jobID."_datasetsID";
	$datasets_holder = "<span style='text-decoration:underline' id='".$datasetsButtonID."' onclick='toggleStuff(\"".$datasetsButtonID."\",\"".$datasetsID."\");'>[Show]</span>";
      }

      // Job status
      $status = getTagContent($minder, 'status');
      $jobsStatii[] = $status; //append to array

      $statusClass = 'statusBad';
      if ($status == 'success') $statusClass = 'statusOK';
      if ($status == 'running') $statusClass =  'statusRunning';

      // Post processing combined status
      $postProcRes = getTagContent($minder, 'postProcessingResult');
      $postProcClass = 'statusBad';
      if ($postProcRes == 'success') $postProcClass = 'statusOK';
      if ($postProcRes == 'running') $postProcClass =  'statusRunning';
      if ($postProcRes == 'no tests') $postProcClass =  'statusNeutral';
      
      // Ref. release used
      $refRel = getTagContent($minder, 'refRelease');
      
      // Now create the table row
      echo "\n<tr class='tableRow'>\n";
      echo "<td valign='top'><a href='page2.php?xml=".$xmlFile."&package=".$packageName."&job=".$escapedJobName."&id=".$jobID."'>".$displayName."</a><br /><br /><span>[id:".$jobID."]</span></td>\n";
      //echo "<td valign='top'>".$jobID."</td>\n";
      echo "<td valign='top'>".$group."</td>\n";
      echo "<td valign='top' class='".$statusClass."'>".$status."</td>\n";
      echo "<td valign='top' class='".$postProcClass."'>".$postProcRes."</td>\n";

      if($datasets_holder==''){
	echo "<td valign='top'>n/a</td>\n";
      }
      else{
	echo "<td valign='top'>";
	echo $datasets_holder;
	echo "<div align='center' style='display:none' id='".$datasetsID."'>".$datasets."</div>";
	echo "</td>\n";
      }

      //echo "<td valign='top'>".$datasets."</td>\n";
      echo "<td valign='top'>".$hash."</td>\n";
      //echo "<td valign='top'>".$refRel."</td>\n";
      echo "</tr>";
      
    }

    echo "</table>\n"; // end of minder table      
    echo "</div></td></tr>";

    $totATNjobs  = sizeOf($atnMinders);
    $doneATNjobs = countDoneJobs($atnMinders);
    $okATNjobs   = countOKJobs($atnMinders);
    if($totATNjobs != 0){
      echo "<tr class='frameworkHeader'>";
      echo "<td>ATN jobs<br /><span class='showJobs' id='".$atnButtonID."' onclick='toggleStuff(\"".$atnButtonID."\",\"".$atnJobsID."\");'>[Show]</span></td>";
      echo "<td class='stats'>";
      echo "<table width='100%' border='0'>";
      echo "<tr>";
      echo "<td class='statHeader'>Run by RTT</td>";
      echo "<td class='statTitle'>Jobs OK:&nbsp;</td><td class='statEntry'>".$okATNjobs."/".$doneATNjobs."</td>";
      echo "<td class='statTitle'>Jobs Done:&nbsp;</td><td class='statEntry'>".$doneATNjobs."/".$totATNjobs."</td>";
      echo "</tr>";
      echo "<tr>";
      echo "<td class='statHeader'>Run by ATN</td>";
      echo "<td class='statTitle'>Jobs OK:&nbsp;</td><td class='statEntry'>---</td>";
      echo "<td class='statTitle'>Jobs Done:&nbsp;</td><td class='statEntry'>---</td>";
      echo "</tr>";
      echo "</table>";
      echo "</td>";
      echo "</tr>";

      // Print out a table of jobs in this CPC category
      echo "<tr><td colspan='3'><div  align='center' style='display:none' id='".$atnJobsID."'>";

      $jobsStatii = array();

      echo "<table>\n";

      //echo "<tr class='tableRow2'><td>Name</td><td>ID</td><td>Hash</td><td>JobGroup</td><td>Datasets</td><td>Job status</td><td>Tests</td><td>Ref.Release</td></tr>";
      echo "<tr class='tableRow2'><td>Name</td><td>JobGroup</td><td>Job status</td><td>Tests</td><td>Datasets</td><td>Hash</td></tr>";
      foreach($atnMinders as $minder){
	$jobName = getTagContent($minder, 'jobName');
	$displayName = getTagContent($minder, 'jobDisplayName');
	if ($displayName == ''){$displayName = $jobName;}
	$jobID   = getTagContent($minder, 'jobID');
	$hash   = getTagContent($minder, 'hashString');
	if ($hash==''){$hash = 'n/a';}
	$group   = getTagContent($minder, 'jobGroup');
	$chainLength = getTagContent($minder, 'chainLength');
	$chainSuccesses = getTagContent($minder, 'chainSuccesses');

	// Now add the datasets
	$datasetsList = $minder->get_elements_by_tagname('datasets');
	$datasetsTag  = $datasetsList[0];
	if (isEmptyTag($datasetsTag)){
	  $datasets = "n/a";
	}
	else{
	  $datasets = "";
	  foreach($datasetsTag->get_elements_by_tagname('dataset') as $dataset){
	    $datasets .= trim($dataset->get_content())."\n";
	  }
	}

	// Job status
	$status = getTagContent($minder, 'status');
	$jobsStatii[] = $status; //append to array

	$statusClass = 'statusBad';
	if ($status == 'success') $statusClass = 'statusOK';
	if ($status == 'running') $statusClass =  'statusRunning';

	// Post processing combined status
	$postProcRes = getTagContent($minder, 'postProcessingResult');
	$postProcClass = 'statusBad';
	if ($postProcRes == 'success') $postProcClass = 'statusOK';
	if ($postProcRes == 'running') $postProcClass =  'statusRunning';
	if ($postProcRes == 'no tests') $postProcClass =  'statusNeutral';
      
	// Ref. release used
	$refRel = getTagContent($minder, 'refRelease');
      
	// Now create the table row
	echo "\n<tr class='tableRow'>\n";
	echo "<td valign='top'><a href='page2.php?xml=".$xmlFile."&package=".$packageName."&job=".$jobName."&id=".$jobID."'>".$displayName."</a><br /><br /><span>[id:".$jobID."]</span></td>\n";
	//echo "<td>".$jobID."</td>\n";
	echo "<td valign='top'>".$group."</td>\n";
	echo "<td valign='top' class='".$statusClass."'>".$status."</td>\n";
	echo "<td valign='top' class='".$postProcClass."'>".$postProcRes."</td>\n";
	echo "<td>".$datasets."</td>\n";
	echo "<td>".$hash."</td>\n";
	//echo "<td valign='top'>".$refRel."</td>\n";
	echo "</tr>";
      }

      echo "</table>\n"; // end of minder table      
      echo "</div></td></tr>";

    }

  }

  echo "</table>";
}

?>

<br />
<a class="pageLink" href="page0.php">Back to home page</a>
<br />
</body>
</html>