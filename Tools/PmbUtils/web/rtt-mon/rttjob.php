<?

require('../common.php');

//TODO: Merge with getVAR in common.php:
function getVarRTTJOB($key,$default='!',$allowedvalues='') {
  if (!array_key_exists($key,$_GET)) {
    if ($default=='!') {
      print "Missing required argument $key\n";
      exit;
    }
    return $default;
  }
  $val=$_GET[$key];
  if ($allowedvalues!='') {
    if (!in_array($val,$allowedvalues)) {
      print "ERROR: $key=$val not recognised.\n";
      exit(1);
    }
  }
  return $val;
};

function formatBytes($bytes, $precision = 2) {
  $units = array('B', 'KB', 'MB', 'GB', 'TB');
  $bytes = max($bytes, 0);
  $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
  $pow = min($pow, count($units) - 1);
  $bytes /= pow(1024, $pow);
  return round($bytes, $precision) . ' ' . $units[$pow];
}

function listfiles($files,$dirname,$urlopts='',$highlightitems=Array(),$sort='size') {
  if (!is_dir($dirname)) {
    print "Error: Not directory: $dirname";
    return;
  }
  $keys=array_keys($files);
  $sortval=Array();
  $sortdir = ($sort=='name'||$sort=='type'?SORT_ASC:SORT_DESC);
  foreach($files as $key => $val) {
    if ($sort=='name') {
      array_push($sortval,strtolower($key));//mtime+SORT_DESC, size+SORT_DESC,name
    } else if ($sort=='type') {
      $parts=explode( ".", basename($key) );
      $n=count($parts);
      array_push($sortval,strtolower($n>1?$parts[$n-1].$key:'aaaa'.$key));//mtime+SORT_DESC, size+SORT_DESC,name
    } else {
      array_push($sortval,$val[$sort]);//mtime+SORT_DESC, size+SORT_DESC,name
    }
  }
  array_multisort($sortval,$sortdir,$keys);
  //sort($keys,SORT_STRING);
  if (count($files)==0) {
    print "No files...\n";
    return;
  }
  $style='font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;white-space:nowrap;';
  print "<table style=\"$style\">\n";
//   foreach ($files as $file => $info) {
  $s='style="background-color: rgb(200, 200, 255);"';
  if ($urlopts=='') $urlopts=''; else $urlopts="$urlopts&amp;";
  //$urlopts.='sort=';
  print "<tr><th $s><a href=\"?${urlopts}sort=name\">File</a></th><th $s><a href=\"?${urlopts}sort=size\">Size</a></th><th $s><a href=\"?${urlopts}sort=mtime\">Last modified</a></th></tr>\n";
  $highlightkeys = array_keys($highlightitems);
  foreach ($keys as $file) {
    $info=$files[$file];
    $bn=basename($file);
    $col='';
    foreach ($highlightkeys as $hlkey) {
      if ($bn==$hlkey) {
	$col=$highlightitems[$hlkey];
	break;
      }
      if ($hlkey[0]=='/'&&preg_match($hlkey,$file)) {
	$col=$highlightitems[$hlkey];
	break;
      }
    }
    //$col=in_array(basename($file),$highlightkeys) ? $highlightitems[basename($file)] : '';
    $fs = formatBytes($info['size']);
    if ($urlopts=='') $url=''; else $url="$urlopts";
    //$url.='sort='.$sort.'&amp;';
    //print preg_match('/.*\.png$/',$file);
    $url.='file='.$file;
    print "<tr style=\"margin:0;padding:0;$col\"><td><a style=\"$col\" href=\"?$url\">$file</a></td><td style=\"text-align:right\">$fs</td><td>".date("Y-m-d H:i:s", $info['mtime'])."</td></tr>\n";
  }
  print "</table>\n";
};

function getStatus($afsdir) {
  $f=$afsdir.'/rttjobinfo.xml';
  $r='unknown';
  if (!file_exists($f))
    return $r;
  $handle = @fopen($f, "r");
  if (!$handle)
    return $r;

  while (!feof($handle)) {
    //not really proper xml parsing...
    $buffer = fgets($handle, 4096);
    if (strpos($buffer, '<status>')===false)
      continue;
    $buffer=fgets($handle, 4096);
    $r=trim($buffer);
    break;
  };
 fclose($handle);
 return $r;
};

function getFilesInDir($dirname) {
  $filesall=scandir($dirname);filesize($dirname.'/'.$file);
  $files=Array();
  foreach ($filesall as $file) {
    if (!is_dir($dirname.'/'.$file)&&$file!=""&&$file[0]!='.') {
      //$fs = formatBytes(filesize($dirname.'/'.$file));
      $files[$file]=Array('name'=>basename($file),'mtime'=>filemtime($dirname.'/'.$file),'size'=>filesize($dirname.'/'.$file));
    }
  }
  return $files;
}

if(!function_exists('mime_content_type')) {

  function mime_content_type($filename) {
    if (endswith($filename,'_log'))
      return 'text/plain';

    $mime_types = array(

			'txt' => 'text/plain',
			'htm' => 'text/html',
			'html' => 'text/html',
			'php' => 'text/html',
			'css' => 'text/css',
			'js' => 'application/javascript',
			'json' => 'application/json',
			//'xml' => 'application/xml',
			'xml' => 'text/plain',
			'swf' => 'application/x-shockwave-flash',
			'flv' => 'video/x-flv',

			// images
			'png' => 'image/png',
			'jpe' => 'image/jpeg',
			'jpeg' => 'image/jpeg',
			'jpg' => 'image/jpeg',
			'gif' => 'image/gif',
			'bmp' => 'image/bmp',
			'ico' => 'image/vnd.microsoft.icon',
			'tiff' => 'image/tiff',
			'tif' => 'image/tiff',
			'svg' => 'image/svg+xml',
			'svgz' => 'image/svg+xml',

			// archives
			'zip' => 'application/zip',
			'rar' => 'application/x-rar-compressed',
			'exe' => 'application/x-msdownload',
			'msi' => 'application/x-msdownload',
			'cab' => 'application/vnd.ms-cab-compressed',

			// audio/video
			'mp3' => 'audio/mpeg',
			'qt' => 'video/quicktime',
			'mov' => 'video/quicktime',

			// adobe
			'pdf' => 'application/pdf',
			'psd' => 'image/vnd.adobe.photoshop',
			'ai' => 'application/postscript',
			'eps' => 'application/postscript',
			'ps' => 'application/postscript',

			// ms office
			'doc' => 'application/msword',
			'rtf' => 'application/rtf',
			'xls' => 'application/vnd.ms-excel',
			'ppt' => 'application/vnd.ms-powerpoint',

			// open office
			'odt' => 'application/vnd.oasis.opendocument.text',
			'ods' => 'application/vnd.oasis.opendocument.spreadsheet',

			// custom
			'log' => 'text/plain',
			'pickle' => 'text/plain',
			'gpickle' => 'text/plain',
			'py' => 'text/plain',
			'sh' => 'text/plain',
			'csv' => 'text/plain',
			'ascii' => 'text/plain',
			'checkfile' => 'text/plain',
			'root' => 'application/octet-stream',

			);

    $p=explode('.',$filename);
    //$ext = strtolower(array_pop(explode('.',$filename)));
    $ext = strtolower($p[count($p)-1]);
    if (array_key_exists($ext, $mime_types)) {
      return $mime_types[$ext];
    }
    elseif (function_exists('finfo_open')) {
      $finfo = finfo_open(FILEINFO_MIME);
      $mimetype = finfo_file($finfo, $filename);
      finfo_close($finfo);
      return $mimetype;
    }
    else {
      return 'application/octet-stream';
    }
  }
}

function passthrough($absfilename) {
  $fp = fopen("$absfilename", 'rb');
  header("Content-Type: ".mime_content_type($absfilename));
  header("Content-Length: " . filesize($absfilename));
  //header('Content-Disposition: attachment; filename="downloaded.pdf"');
  header('Content-Disposition: filename="'.basename($absfilename).'"');
  header("Content-Transfer-Encoding: binary\n");
  fpassthru($fp);
  exit;
}

function displayContent($dirname,$filename='',$urlopts='',$highlightitems=Array(),$sort='size') {
  $files=getFilesInDir($dirname);
  if ($filename=='') {
    listfiles($files,$dirname,$urlopts,$highlightitems,$sort);
  } else {
    if (!in_array($filename,array_keys($files))) {
      print "ERROR: Not allowed to display $filename";
    } else {
      passthrough("$dirname/$filename");
    }
  }
}

function getAfsDir($nightly,$build,$cache,$cmtconfig,$package,$jobname,$jobid) {
  if ($cache=='prod')
    $cache='offline';
  $location_rttbase=$GLOBALS['location_rttbase'];
  $a=$location_rttbase."/$nightly/$build/build/$cmtconfig/$cache/$package/$jobname/";
  return file_exists($a) ? $a : '';
}

$cmtconfig = getVarRTTJOB('cmtconfig');
$cache = getVarRTTJOB('cache');
$build = getVarRTTJOB('build');
$nightly = getVarRTTJOB('nightly');
$package = getVarRTTJOB('package');
$jobname = getVarRTTJOB('jobname');
$jobid = getVarRTTJOB('jobid','*');
$sort = getVarRTTJOB('sort','size',Array('name','size','mtime'));

$file = getVarRTTJOB('file','');
$afsdir=getAfsDir($nightly,$build,$cache,$cmtconfig,$package,$jobname,$jobid);
if ($afsdir=='') {
  print "Problems finding run directory!";
  exit;
}

$afsdir_parts = explode('/',$afsdir);
if ($jobid=='*')
     $jobid=$afsdir_parts[count($afsdir_parts)-2];
//$oldrtturl = 'http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/page2.php';
//$cache2 = ($cache=='prod' ? 'offline' : $cache);
//$oldrtturl.="?xml=${nightly}/$build/build/${cmtconfig}/$cache2/RTTSummary.xml&amp;package=${package}&amp;job=${jobname}&amp;id=$jobid";

if ($file=='') {
  $title="$package#$jobname [ $build/$cache/$cmtconfig/$nightly ]";
  print "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
  print "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n";
  print "<head>\n";
  print "  <title>$title</title>\n";
  print "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
  print "  <meta name=\"expires\" content=\"0\" />\n";
  print "  <style type=\"text/css\">\n";
  print "  A:link {text-decoration: none; color: blue}\n";
  print "  A:visited {text-decoration: none; color: blue}\n";
  print "  A:active {text-decoration: none}\n";
  print "  A:hover {text-decoration: underline; color: red;}\n";
  print "  </style>\n";
  print "</head>\n";
  print "<body>\n";
  print "<h3>$title</h3>\n";
  //  print "<p>RTT job status: ".getStatus($afsdir)."<p/>\n";

}

$highlight=Array($package.'_TestConfiguration.xml'=>'color:red;',
		 "${jobname}${jobid}_log"=>'color:red;',
		 '/.*\.png$/'=>'color:green;');


displayContent($afsdir,$file,"nightly=$nightly&amp;build=$build&amp;cache=$cache&amp;cmtconfig=$cmtconfig&amp;package=$package&amp;jobname=$jobname&amp;jobid=$jobid",$highlight,$sort);

if ($file=='') {
  print "<ul>\n";
  //print "<li><a href=\"missing\">RTT status page</a> (<a href=\"$oldrtturl\">old</a>)</li>\n";
  print "<li>AFS location: $afsdir</li>\n";
  print "</ul>\n";
  print "</body>\n";
  print "</html>\n";
}

?>
