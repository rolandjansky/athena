<html>
<head>
<meta http-equiv="refresh" content="20" />

<title>Framework tests: overview of current suites</title>
<style>
body{font-family:tahoma,sans-serif;font-size:1em;color:black;}
tr.header{background-color:#8866ff;font-weight:bold;}
tr.entry{background-color:#ddf;}
</style>
</head>
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

$current = getRootNodeFromXMLfile('current.testsuites.xml');
$testsuites = $current->get_elements_by_tagname('testsuite');

if (sizeOf($testsuites)==0){echo "No testsuite runs ongoing."; return;}
?>

<table>
  <tr class="header"><td>Testsuite Name</td><td>Status</td><td>Testsuite location</td><td></td></tr>
<?
foreach($testsuites as $testsuite){
  $name = getTagContent($testsuite, 'testsuitename');
  $path = getTagContent($testsuite, 'testsuitepath');
  $status = getTagContent($testsuite, 'testsuitestatus');
  echo "<tr class='entry'><td>".$name."</td><td>".$status."</td><td>".$path."</td><td><a href='testsuite.php?name=".$name."'>Show me</a></td></tr>";
  echo "";
}

?>

</table>
</body>
</html>