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

  $almostURL = $BaseURL."/".$release."/".$package."/".$group."/".$jobToUse;
  $almost = $BaseAFS."/".$release."/".$package."/".$group."/".$jobToUse;

  $last = "0"; // Bad workaround!
  if (isDirValid($almost)) {
    $d = opendir($almost);
    while (false != ($entry = readdir($d))) {
      if (($entry != ".") && ($entry != "..")) {
        $last = $entry;
      }
    }
  }
  if ($useURL == true)
    $retV = $almostURL."/".$last."/".$file;
  else
    $retV = $almost."/".$last."/".$file;

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

  $almostURL = $BaseURL."/".$release."/".$package."/".$group."/".$jobToUse;
  $almost = $BaseAFS."/".$release."/".$package."/".$group."/".$jobToUse;

  $d = opendir($almost);
  $last = "0"; // Bad workaround!
  while (false != ($entry = readdir($d))) {
    if (($entry != ".") && ($entry != "..")) {
      $last = $entry;
    }
  }

  if ($useURL == true)
    $retV = $almostURL."/".$last."/".$file;
  else
    $retV = $almost."/".$last."/".$file;

  return $retV;
}

function isDirValid($d) {
  $cwd = getcwd(); 
  $retV = false;
  if (@chdir($d)) {
    chdir($cwd);
    $retV = true;
  } 
  return $retV;
} 

?>
