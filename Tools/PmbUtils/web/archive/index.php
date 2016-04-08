<?php

require('../common.php');

function getMonthIndexFiles($location_archive_indexdir) {
  $files=scandir($location_archive_indexdir);
  rsort($files);//To get newest (year,month) first
  $out=Array();
  foreach ($files as $file)
    if (startswith($file,'2'))//this will break in about 990 years...
      array_push($out,$file);
  return $out;
};

function passthroughFile($indexfile) {
  //Just read enough to get the number of jobs which ran in a given package on a given day.
  $cont=getContents($indexfile);
  if (!$cont) {
    print "ERROR: Could not read info from $indexfile";
  }
  print $cont;
}

if (array_key_exists('showlog',$_GET)) {
  $showlog=$_GET['showlog'];
  $nslash=substr_count($showlog,'/');
  $is_misc_gz=endswith($showlog,'.gz');
  if ($is_misc_gz) {
    $nslash-=1;
  }

  $is_custom=$nslash==5;

  $base=$is_custom?$location_customarchivebase:$location_archivebase;
  //Show logfile or rundir mode:
  $abspath=realpath($base.'/'.$_GET['showlog']);
  if (!startswith($abspath,$location_archivebase_nosymlinks)) {
    print "Bad input!";
    exit(1);
  }
  if (!$is_misc_gz) {
    if ($is_custom) {
      $logfile=$abspath.'/'.'__log.txt.gz';
    } else {
      $p=explode('/',$abspath);
      $logfiles=glob($abspath.'/'.$p[count($p)-1].'*_log.gz');
      if (!count($logfiles)==1) {
	print "Unable to determine log-file for job";
	exit(1);
      }
      $logfile=$logfiles[0];
    }
  } else {
    $logfile=$abspath;
  }
  $fp = gzopen($logfile, 'rb');
  header("Content-Type: text/plain");
  header('Content-Disposition: filename="'.str_replace('.gz','',basename($logfile)).'"');
  header("Content-Transfer-Encoding: binary\n");
  gzpassthru($fp);
  exit;
} else if (array_key_exists('package',$_GET)&&array_key_exists('date',$_GET)) {
  //Package+date mode

  $package=$_GET['package'];
  $date=$_GET['date'];
  $dateok=preg_match('/^[0-9][0-9]-[0-9][0-9]-[0-9][0-9][0-9][0-9]$/',$date);
  $pkgok=preg_match('/^[a-zA-Z_0-9][a-zA-Z_0-9]*$/',$package);
  if (!$dateok||!$pkgok) {
    print "Bad input!";
    exit(1);
  }
  $dateparts=explode('-',$date);
  $day=$dateparts[0];
  $month=$dateparts[1];
  $year=$dateparts[2];

  $ttl='Archived RTT output for '.$package;
  if ($package=='custom')
    $ttl='Archived output of custom cron jobs';
  pmbheader('','Long-term archival of RTT job output ($package $date)',
	    Array($ttl.' on '.$date.' [<a href="?month='."$year-$month".'">up</a>]'),
	    true,false,'..');

  $is_custom=($package=='custom');
  if ($is_custom)
    $rundirs=glob("$location_customarchivebase/$day/$month/$year/*/*/*");
  else
    $rundirs=glob("$location_archivebase/$day/$month/$year/*/*/*/$package/*/*");
  print 'Found '.count($rundirs).' archived run directories for '.$package.' on '.$date.':'."\n";
  $info=Array();
  //$offset= $is_custom ? 11 : 10;
  foreach($rundirs as $rundir) {
    $parts=explode('/',$rundir);
    //print_r($parts);
    if ($parts[10]!=$year||($parts[14]!=$package&&!$is_custom)) {
      print "Bad rundir! $rundir";
      exit(1);
    }
    if ($is_custom) {
      $buildid=$parts[11].'/'.$parts[12];
      $jobid=$parts[13];
      $jobinfo=Array('name'=>$parts[13],'group'=>'custom','rundir'=>$rundir);

    } else {
      $buildid=$parts[11].'/'.$parts[13].'/'.$parts[12];
      $jobid=$parts[15].'/'.$parts[16];
      $jobinfo=Array('name'=>$parts[16],'group'=>$parts[15],'rundir'=>$rundir);
    }
    if (!array_key_exists($buildid,$info)) {
      $info[$buildid]=Array($jobid=>$jobinfo);
    } else {
      $info[$buildid][$jobid]=$jobinfo;
    }
  }

  foreach($info as $buildid=>$jobs) {
    $id='id_'.str_replace('/','___',$buildid);
    print "<p/><strong><a href=\"javascript:;\" onmousedown=\"toggleDiv('$id');\" >$buildid (".count($jobs).")</a></strong><br/>\n";
    print "<div id=\"$id\" style=\"display:none\">\n";
    print '  <table style="font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;white-space:nowrap;">'."\n";
    print "    <tr style=\"background-color:rgb(215,215,255);\"><th>Jobname</th><th>Jobgroup</th><th>Logfile</th><th>AFS location of all archived files</th></tr>\n";
    foreach($jobs as $job=>$jobinfo) {

      if ($is_custom)
	$jobpath=str_replace("$location_customarchivebase/",'',$jobinfo['rundir']);
      else
	$jobpath=str_replace("$location_archivebase/",'',$jobinfo['rundir']);
      print "    <tr><td>".$jobinfo['name']."</td><td>".$jobinfo['group']."</td><td><a href=\"?showlog=$jobpath\">logfile</a></td><td>".$jobinfo['rundir']."</td></tr>\n";
    }
    print "  </table>\n";
    print "</div>\n";
  }
} else {
  //Month overview mode
  $months=getMonthIndexFiles($location_archive_indexdir);
  $cfg_month=getVar('month',$months[0],$months);
  $opts=array('month'=>$cfg_month);
  $s_month=menustring('month',$months,$cfg_month,$opts);
  pmbheader('','Long-term archival of RTT job output',Array("Month: $s_month"),false,false,'..');
  passthroughFile($location_archive_indexdir.'/'.$cfg_month);
}

pmbfooter();



?>