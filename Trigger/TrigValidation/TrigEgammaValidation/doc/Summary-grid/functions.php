<?

function calcget($phpfile, $r, $i, $useAlias = false) {
  global $content;

  $nr = str_replace("/", "%2F", $r);

  $str = $phpfile."?";
  $sep = 0;

  if ($nr != "") {
    $str = $str."release=".$nr;
    $sep = 1;
  }

  if ($i != "") {
    $name = "item";
    if ($useAlias) {
      $name = "alias";
    }

    if ($sep == 1)
      $str = $str . "&";
    $str = $str.$name."=".$i;
    $sep = 1;
  }

  if ($content != "content.xml") {
    if ($sep == 1)
      $str = $str."&";
    $str = $str."file=".$content;
    $sep = 1;
  }

  return $str;
}

function calcfile($file, $useURL = true, $theJob = "") {
  global $BaseAFS;
  global $BaseURL;
  global $release;
  global $job;
  global $group;
  global $package;

  if ($theJob != "") {
    $jobToUse = $theJob;
  } else {
    $jobToUse = $job;
  }

  $almostURL = $BaseURL."/".$release."/".$jobToUse;
  $almost = $BaseAFS."/".$release."/".$jobToUse;

  if ($useURL == true)
    $retV = $almostURL."/".$file;
  else
    $retV = $almost."/".$file;

  return $retV;
}

function calcfilegroup($group, $file, $useURL = true, $theJob = "") {
  global $BaseAFS;
  global $BaseURL;
  global $release;
  global $job;
  global $package;

  if ($theJob != "") {
    $jobToUse = $theJob;
  } else {
    $jobToUse = $job;
  }

  $almostURL = $BaseURL."/".$release."/".$jobToUse;
  $almost = $BaseAFS."/".$release."/".$jobToUse;

  if ($useURL == true)
    $retV = $almostURL."/".$file;
  else
    $retV = $almost."/".$file;

  return $retV;
}
?>
