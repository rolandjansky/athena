<?
//To check for missing symlinks etc.
 require_once('locations.php');
?> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
  <title>ATLAS PMB</title>
  <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
  <link rel="icon" href="favicon.png" type="image/x-icon" />
  <link rel="shortcut icon" href="favicon.png" type="image/x-icon" />
  <meta name="expires" content="0" />
</head>
<body>
  <div style="margin-left:10%;margin-right:10%"  >
  <h2 style="background-color:rgb(200,200,255);padding:5pt;">ATLAS Performance Management Board Pages</h2>
<img style="float:right;margin-left:2%;" width="255" height="667" alt="Chronometer" src="timing.jpg"/>
  Welcome to the ATLAS PMB pages.
  <p/>
  Most PMB-related information is maintained on the <a
  href="https://twiki.cern.ch/twiki/bin/view/Atlas/PerformanceManagementBoard">PMB
wiki</a>, while dynamic content is hosted here.
  <p/>
On this site you will find:
  <ul><? $thisdir=dirname($_SERVER["PHP_SELF"]); ?>
    <li><a href="<? print $thisdir.'/'.$location_rttmon_subdir;?>/">Automatic monitoring of memory and CPU usage per-domain in RTT tests</a></li>
    <li><a href="<? print $thisdir.'/'.$location_archive_subdir;?>/">Long-term archive of output from PMB related RTT jobs</a></li>
    <li><a href="<? print $thisdir.'/'.$location_archmon_subdir;?>/">Detailed automatic monitoring based on archive</a></li>
    <li><a href="<? print $thisdir.'/'.$location_oprofile_linkname;?>/">Profile results of ATLAS jobs</a></li>
  </ul>
<?
//<div style="text-align:center;padding-top:5%">
//  </div>
//    $thisurl="http://".$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"];
//    print "  <a href=\"http://validator.w3.org/check?uri=".rawurlencode($thisurl)."\">"."(check syntax)</a>\n";
?>
  </div>
</body>
</html>
