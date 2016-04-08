<?

function extractFromStatusText($statusText, $whatToExtract){
  if ($statusText == '---'){return "---";}

  $toks = split(':', $statusText);

  $numbers = $toks[3];
  $nToks = split('/', $numbers);
  if ($whatToExtract=='ok') {
    return $nToks[0];
  }
  if ($whatToExtract=='done') {
    return $nToks[1];
  } 
  if ($whatToExtract=='total') {
    return $nToks[2];
  } 
}

function cvsReposURL($fullPackagePath, $packageTag){
  // fullpackagePath is path from offline/
  $baseURL = "http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/";
  $URL = $baseURL.$fullPackagePath.'/?pathrev='.$packageTag;
  if ($packageTag == ''){
    $URL = $baseURL.$fullPackagePath;
  }

  return $URL;
}

function openAndReadSummaryFilePaths($pathToFile){
  if(!file_exists($pathToFile)){return NULL;}
  $handle = fopen($pathToFile, 'r');
  $lines = array();
  while (!feof($handle)){
    $line = trim(fgets($handle));
    if (!feof($handle) && sizeOf($line) > 0){
      if (!in_array($line, $lines)){
	$lines[] = $line;
      }
    }
  }

  $listOfSummFiles = array();
  foreach($lines as $line){
    if(!file_exists($line) || filesize($line)==0){continue;}
    $listOfSummFiles[] = $line;
  }

  fclose($handle);

  return $listOfSummFiles;
}

function getTagContent($parentNode, $tagName){
  $thing = $parentNode->get_elements_by_tagname($tagName);
  if (sizeOf($thing)==0) return "";
  $thing = $thing[0]->get_content();
  return trim($thing);
}

function getMultiTagContent($parentNode, $tagName){
  $things = $parentNode->get_elements_by_tagname($tagName);
  if (sizeOf($things)==0) return array("");

  $contents = array();
  foreach($things as $thing){
    $contents[] = trim($thing->get_content());
  }
  
  return $contents;
}

function getRootNodeFromXMLfile($theFile){
  //$xmlDoc = xmldocfile($theFile);
  $xmlDoc = domxml_open_file($theFile);
  $rootNode = $xmlDoc->document_element();
  $xmlDoc->unlink();
  unset($xmlDoc);
  return $rootNode;
}

function getAttributeValue($element, $attributeName){
  if ($element->has_attributes()){
    foreach ($element->attributes() as $attr){
      if($attr->name == $attributeName) return trim($attr->value());
    }
  }

  return '';
}

function hasChildTag($parentNode, $childTagName){
  return sizeOf($parentNode->get_elements_by_tagname($childTagName)) > 0;
}

function isEmptyTag($element){
  return ($element->is_blank_node() || strlen(trim($element->get_content())) == 0);
}

function getSingleTagWithChildTag($parentNodes, $childName, $childValue){
  $tags = getTagsWithChildTags($parentNodes, $childName, $childValue);
  return $tags[0];
}

function getTagsWithChildTags($parentNodes, $childTagName, $childTagValue, $inverse = FALSE){
  $index = 0;
  $matches = array();
  foreach ($parentNodes as $parentNode){
    $val = getTagContent($parentNode, $childTagName);
    if ($val == $childTagValue){
      $matches[$index] = $parentNode;
      $index++;
    }
  }

  return $matches;
}

////////////////////////////////////////////////////////////
// SORT SUMMARY FILES
////////////////////////////////////////////////////////////

function getOverviewNode($summaryFilePath){
  $rootNode = getRootNodeFromXMLfile($summaryFilePath);
  $overviewNodes = $rootNode->get_elements_by_tagname('overview');
  $oNode = $overviewNodes[sizeOf($overviewNodes)-1]; //0th one is ATN summary, ignore if present
  return $oNode;
}

function sortSummaryFilesByRelease($a, $b){
  $A = $a['release'];
  $B = $b['release'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortSummaryFilesByBranch($a, $b){
  $A = $a['branch'];
  $B = $b['branch'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortSummaryFilesByPlatform($a, $b){
  $A = $a['platform'];
  $B = $b['platform'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortSummaryFilesByProject($a, $b){
  $A = $a['topProject'];
  $B = $b['topProject'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortSummaryFilesByStarted($a, $b){
  $A = $a['started'];
  $B = $b['started'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortSummaryFilesByCompleted($a, $b){
  $A = $a['completed'];
  $B = $b['completed'];
  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

// ==================================================================

function sortPackagesByName($a, $b){
  $nameA    = strtolower(trim(getTagContent($a, 'packageName')));
  $nameB    = strtolower(trim(getTagContent($b, 'packageName')));
  if ($nameA == $nameB) {
    return 0;
  }
  return ($nameA < $nameB) ? -1 : 1;
}

function sortPackagesByStatus($a, $b){
  $nameA    = getTagContent($a, 'packageStatus');
  $nameB    = getTagContent($b, 'packageStatus');
  if ($nameA == $nameB) {
    return 0;
  }
  return ($nameA < $nameB) ? -1 : 1;
}

function sortJobsByJob($a, $b){
  $nameA    = getTagContent($a, 'identifiedName');
  $nameB    = getTagContent($b, 'identifiedName');
  if ($nameA == $nameB) {
    return 0;
  }
  return ($nameA < $nameB) ? -1 : 1;
}

function sortJobsByGroup($a, $b){
  $nameA    = getTagContent($a, 'jobGroup');
  $nameB    = getTagContent($b, 'jobGroup');
  if ($nameA == $nameB) {
    return 0;
  }
  return ($nameA < $nameB) ? -1 : 1;
}

function sortKeepFiles($a, $b){
  $keepA    = strtolower(trim($a->get_content()));
  $keepB    = strtolower(trim($b->get_content()));
  if ($keepA == $keepB) {
    return 0;
  }
  return ($keepA < $keepB) ? -1 : 1;
}

//=====================================================
// CPC sorting for the CPC view
//=====================================================
function sortCPCByClass($cpcKeyA, $cpcKeyB){
  $A = explode(' ', $cpcKeyA);
  $A = trim($A[0]);
  $A = substr($A, 1, -1);

  $B = explode(' ', $cpcKeyB);
  $B = trim($B[0]);
  $B = substr($B, 1, -1);

  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortCPCByProcess($cpcKeyA, $cpcKeyB){
  $A = explode(' ', $cpcKeyA);
  $A = trim($A[1]);
  $A = substr($A, 1, -1);

  $B = explode(' ', $cpcKeyB);
  $B = trim($B[1]);
  $B = substr($B, 1, -1);

  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

function sortCPCByComponent($cpcKeyA, $cpcKeyB){
  $A = explode(' ', $cpcKeyA);
  $A = trim($A[2]);
  $A = substr($A, 1, -1);

  $B = explode(' ', $cpcKeyB);
  $B = trim($B[2]);
  $B = substr($B, 1, -1);

  if ($A == $B) {
    return 0;
  }
  return ($A < $B) ? -1 : 1;
}

?>