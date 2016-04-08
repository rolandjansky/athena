<?
ob_start("ob_gzhandler");
header("Content-type: text/html; charset=utf-8");
?>

<html>
<head>
<style>
body{font-family:Trebuchet,sans-serif;background-color:#eeeeff;text-align:center;margin: 0 5 5 5;color:black}
.pageHeader{font-size:1.8em;font-weight:bold;}
.sectionHeader{font-size:1em;font-weight:bold;}
.statusOK{color:green}
.statusBad{color:red}
.statusRunning{color:brown}
#admin{font-size:0.7em;font-weight:bold;letter-spacing:2px;background-color:#8888df;color:white;font-family:verdana, sans-serif}
a.pageLink{font-family:arial;font-size:0.8em;color:#777777;text-decoration:underline}
a.pageLink:hover{font-family:arial;font-size:0.8em;color:black;text-decoration:none}
a.normalKeepFile{font-family:arial;font-size:0.8em;color:black;text-decoration:underline;font-weight:bold}
a.normalKeepFile:hover{font-family:arial;font-size:0.8em;color:orange;text-decoration:none}
a.linkExpired{font-family:arial;font-size:0.8em;color:#999999;text-decoration:underline;font-style:italic}
span.linkWillExpire{font-family:arial;font-size:0.7em;color:#777777;text-decoration:none;font-style:italic}
table{background-color:#ddddff}
.header{font-weight:bold;background-color:#aaaaff}
.entry{font-size:0.85em;font-weight:normal}
ul{list-style-type:none;padding:2px}
#mainTable{padding:0;margin:auto;}
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

<!------------------------------------------------------------->
<!-- START OF THE PHP SECTION                                -->
<!------------------------------------------------------------->

<?
include("tools.php");

$xmlFile = $_GET['xml'];

if (!file_exists($xmlFile)){
  echo "No data available yet.";
  return;
}

$packageName = $_GET['package'];
$jobName = $_GET['job'];
$jobID = $_GET['id'];

$rootNode = getRootNodeFromXMLfile($xmlFile);
$overviewNodes = $rootNode->get_elements_by_tagname('overview');
$overviewNode = $overviewNodes[sizeOf($overviewNodes)-1];

// Find the corresponding minder node 
$minder = getSingleTagWithChildTag($rootNode->get_elements_by_tagname('minder'), 'identifiedName', $jobName.$jobID);
if (sizeOf($minder)==0){echo "No information available.";return;}

$displayName = getTagContent($minder, 'jobDisplayName');
if ($displayName == ''){$displayName = $jobName;}

$hash   = getTagContent($minder, 'hashString');
if ($hash==''){$hash = 'n/a'; }

$jobGroup = getTagContent($minder, 'jobGroup');

// Output the top of page overview info
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
echo "</div>";

$allKeepFiles = $minder->get_elements_by_tagname('deliveredkeepfiles');

$allKeepFilesTag  = $allKeepFiles[0];
if (isEmptyTag($allKeepFilesTag)){
  $keepFiles = array("None available");
}
else{
  $keepFiles = $allKeepFilesTag->get_elements_by_tagname('keepfile_newstyle');
}

echo "<br />";

//Is this an error minder with content in <errors> tag?
$errors = getTagContent($minder, 'errors');
if($errors==''){
  // store for later the job tests
  $allTests = $minder->get_elements_by_tagname('testResults');
  $allTestsTag  = $allTests[0];
  $jobTests = $allTestsTag->get_elements_by_tagname('testResult');
}

?>

<!------------------------------------------------------------->
<!-- END OF THE PHP SECTION                                  -->
<!------------------------------------------------------------->

<table id='mainTable' cellpadding="0" cellspacing="0">
   <tr>
     <td valign="top">
        <table>
           <tr class='header'><td>Overview</td></tr>
           <tr>
              <td>
                  <table>
                     <tr class='entry'><td align="right">Package: </td><? echo "<td class='entry'>".$packageName."</td>";?></tr>
                     <tr class='entry'><td align="right">Job name: </td><? echo "<td class='entry'>".$displayName."</td>";?></tr>
                     <tr class='entry'><td align="right">Jobgroup: </td><? echo "<td class='entry'>".$jobGroup."</td>";?></tr>
                     <tr class='entry'><td align="right">Job ID: </td><? echo "<td class='entry'>".$jobID."</td>";?></tr>
                     <tr class='entry'><td align="right">Hash ID: </td><? echo "<td class='entry'>".$hash."</td>";?></tr>
                  </table>
              </td>
           </tr>

           <tr class='header'><td>Results</td></tr>
           <tr>
              <td>
                  <table>
		     <?
                      $jobStat = trim(getTagContent($minder, 'status'));
                      if ($jobStat == 'success'){$jobStatClass = 'statusOK';}
                      else if ($jobStat == 'running'){$jobStatClass = 'statusRunning';}
                      else {$jobStatClass = 'statusBad';}
                     ?>
                     <tr class='entry'><td align="right">Job Status: </td><? echo "<td class='".$jobStatClass."'>".$jobStat."</td>";?></tr>

		     <?
		     if (!isset($jobTests)){
		       echo "<tr class='entry'><td align='right'>Tests: </td><td>n/a</td></tr>";
		     } 
                     else if (sizeOf($jobTests) == 0){
		       echo "<tr class='entry'><td align='right'>Tests: </td><td>No tests</td></tr>";
		     }
                     else{ 
		       foreach ($jobTests as $test){
			 echo "<tr>";
			 if(trim(getTagContent($test, 'testStatus')) == 0){
			   $testStatus = "success";
			   $testClass  = "statusOK";
			 }
			 else{
			   $testStatus = "error";
			   $testClass  = "statusBad";
			 }
			 echo "<td class='entry'>".trim(getTagContent($test, 'testName')).": </td>";
			 echo "<td class='entry ".$testClass."'>".$testStatus."</td>";
			 echo "</tr>";
		       }
		     }
                     ?>
                  </table>
              </td>
           </tr>

           <tr class='header'><td>Classification</td></tr>
           <tr>
              <td>
		     <table class='entry'>
		     <? 
		     $displayClassList = $minder->get_elements_by_tagname('displayClass');
                     if (sizeOf($displayClassList) == 0){
		       echo "<tr><td>None available</td></tr>";
                     }
                     else{
                        $displayClass = $displayClassList[0];
			$displayProcessList = $minder->get_elements_by_tagname('displayProcess');
			//$displayProcess = $displayProcessList[0];
			$displayCompList = $minder->get_elements_by_tagname('displayComponent');
			//$displayComp = $displayCompList[0];
                     
			if (isEmptyTag($displayClass)){
			  echo "<tr><td>None available</td></tr>";
			}
			else{
			  echo "<tr><td align='right'>Class: </td><td>".trim($displayClass->get_content())."</td></tr>";
			  foreach($displayProcessList as $dp){
			    echo "<tr><td align='right'>Process: </td><td>".trim($dp->get_content())."</td></tr>";
			  }
			  foreach($displayCompList as $dc){
			    echo "<tr><td align='right'>Component: </td><td>".trim($dc->get_content())."</td></tr>";
			  }
			}
		     }
                     ?>
                   </table>
              </td>
           </tr>
           <tr class='header'><td>Documentation</td></tr>
           <tr>
              <td>
		   <table class='entry'>
		   <?
		   $docList = $minder->get_elements_by_tagname('jobDocString');
                   if(sizeOf($docList)==0){
		     echo "<tr><td style='font-style:italic'>[No doc available]</td></tr>";
		   }
                   else{
		     $doc = $docList[0];
		     if (isEmptyTag($doc)){
		       echo "<tr><td style='font-style:italic'>[No doc available]</td></tr>";
		     }
		     else{
		       $docContent = trim($doc->get_content());
		       echo "<tr><td align='left' style='width:300px'>".$docContent."</td></tr>";
		     }
		   }

		   $docLinkList = $minder->get_elements_by_tagname('jobDocURL');
                   if(sizeOf($docLinkList)==0){
		     echo "<tr><td style='font-style:italic'>[No doclink available]</td></tr>";
		   }
                   else{
		     $doclink = $docLinkList[0];
		     if (isEmptyTag($doclink)){
		       echo "<tr><td style='font-style:italic'>[No doclink available]</td></tr>";
		     }
		     else{
		       echo "<tr><td align='left' style='width:300px'><a class='pageLink' href='".trim($doclink->get_content())."'>More info</a></td></tr>";
		     }
		   }
		   ?>
		   </table>
              </td>
           </tr>
           <tr class='header'><td>Auditor Stats</td></tr>
           <tr>
               <td>
                  <table class='entry'>
                     <tr><td align="right">CPU time (s): </td><td><?echo trim(getTagContent($minder, 'cpuTime'));?></td></tr>
                     <tr><td align="right">CPU time 2000 (s): </td><td><?echo trim(getTagContent($minder, 'cpuTime2000'));?></td></tr>
                     <tr><td align="right">Walltime (s): </td><td><?echo trim(getTagContent($minder, 'wallTime'));?></td></tr>
                     <tr><td align="right">Memory: </td><td><?echo trim(getTagContent($minder, 'mem'));?></td></tr>
                     <tr><td align="right">V-Memory: </td><td><?echo trim(getTagContent($minder, 'vmem'));?></td></tr>
                     <tr><td align="right">Batch Status: </td><td><?echo trim(getTagContent($minder, 'batchStatus'));?></td></tr>
                  </table>
               </td>
           </tr>
        </table>
     </td>
     <td valign="top">
        <ul>
          <?

		     //$base = getTagContent($minder,'resultsPath');
if(trim($jobStat)=='batchError'){
  echo "Job had batch error.";
}
else if ($errors != ''){
  echo "RTT rejected this job. Here's the traceback:<br />";
  echo "<font color='red'><pre>".$errors."</pre></font>";
}
else{
  usort($keepFiles, "sortKeepFiles");
  if (sizeOf($keepFiles)==1 && $keepFiles[0]=='None available'){
    echo "<li>None available</li>\n";
  }
  else{
    foreach($keepFiles as $keepFile){
      $linkColor    = getAttributeValue($keepFile, 'displayColor');
      $md5sum       = getAttributeValue($keepFile, 'md5sum');
      $link_info    = getAttributeValue($keepFile, 'info');
      $keepFileName = trim($keepFile->get_content());
      $linkTgt      = $keepFileName;

      if ($md5sum != ''){
	if (file_exists($linkTgt)){
	  $md5sumNow = md5_file($linkTgt);
	  if ($md5sum != $md5sumNow){
	    echo "<li><a class='linkExpired'>".basename($keepFileName)." [LINK EXPIRED]</a></li>\n";
	    continue;
	  }
	  else{
	    echo "<li><a class='normalKeepFile' title='".$link_info."' style='color:".$linkColor."' href='".$linkTgt."'>".basename($keepFileName)."</a><span class='linkWillExpire'>&nbsp;[LINK WILL EXPIRE]</span></li>\n";
	    continue;
	  }
	}
	else{
	  echo "<li><a class='linkExpired'>".basename($keepFileName)." [LINK EXPIRED]</a></li>\n";
	  continue;
	}
      }

      echo "<li><a class='normalKeepFile' title='".$link_info."' style='color:".$linkColor."' href='".$linkTgt."'>".basename($keepFileName)."</a></li>\n";
    }
  }
}
?>
          </ul>
     </td>
   </tr>
</table>

<br />
<a class="pageLink" href="page0.php">Back to home page</a>
<br />
</body>
</html>