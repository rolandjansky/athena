<html>
<head>
<style>
body{font-family:tahoma,sans-serif;font-size:1em;color:black;text-align:center;background-color:#fff}
div.pageheader{text-align:center;font-weight:bold;}
tr.tableheader{font-weight:bold;}
tr.testname{background-color:#aaa}
td.fail{background-color:red;}
td.pass{background-color:#44aa33;}
</style>
<title></title>
<body>
<?
function getRootNodeFromXMLfile($theFile){
  $xmlDoc = domxml_open_file($theFile);
  $rootNode = $xmlDoc->document_element();
  $xmlDoc->unlink();
  unset($xmlDoc);
  return $rootNode;
}

function getTagContent($parentNode, $tagName){
  $thing = $parentNode->get_elements_by_tagname($tagName);
  if (sizeOf($thing)==0) return "";
  $thing = $thing[0]->get_content();
  return trim($thing);
}

$testsuitename = trim($_GET['name']);
$pathToTestsuiteFile = '../testsuites/'.$testsuitename.'/testsuite.status.xml';
$testsuite = getRootNodeFromXMLfile($pathToTestsuiteFile);

$name   = getTagContent($testsuite, 'testsuitename');
$status = getTagContent($testsuite, 'testsuitestatus');

echo "<div class='pageheader'>".$testsuitename."</div><br />";

$pathToSummaryFile = getTagContent($testsuite, 'pathToSummaryFile');
if ($pathToSummaryFile==""){
  echo "No job to show yet";
  return;
}


echo "<a href='../testsuites/".$testsuitename."/Results/page1.php?xml=".$pathToSummaryFile."'>Show me the job(s)</a><br /><br />";

echo "Tests:<br />";
echo "<table align='center' border='1'>";
foreach($testsuite->get_elements_by_tagname('testsuitetest') as $testsuitetest){
  $testname = getTagContent($testsuitetest, 'testsuitetestname');
  echo "<tr class='testname'><td colspan='3'>".$testname."</tr>";
    foreach($testsuitetest->get_elements_by_tagname('test') as $test){
    $testname    = getTagContent($test, 'testname');
    $teststatus  = getTagContent($test, 'status');
    $teststatus  = $teststatus?'PASS':'FAIL';
    $statusCSS   = $teststatus=='PASS'?'pass':'fail';
    $testmessage = getTagContent($test, 'message');
    if (sizeOf($testmessage)){$testmessage='--';}

    echo "<tr><td>".$testname."</td><td class='".$statusCSS."'>".$teststatus."</td><td>".$testmessage."</td></tr>";
  }
}

echo "</table>";
?>

</body>
</html>