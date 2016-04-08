<?

////////////////////////////////////////////////////////////////
//                                                            //
//  PHP file used to create the RTT-based pmb performance     //
//  monitoring pages.                                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2009                           //
//                                                            //
////////////////////////////////////////////////////////////////

require_once('../common.php');

/////////////////////////////////////
// Parse arguments and show menus  //
/////////////////////////////////////

$menuinfo=readMenuInfo($GLOBALS['location_rttmondata'].'/menu.txt');

$cfg_builds=$menuinfo['cfg_builds'];
$cfg_caches=$menuinfo['cfg_caches'];
$cfg_cmtconfigs=$menuinfo['cfg_cmtconfigs'];
$cfg_jobsets=$menuinfo['cfg_jobsets'];
$cfg_vars=$menuinfo['cfg_vars'];

$cfg_build=getVar('build','dev',$cfg_builds);
$cfg_cache=getVar('cache','prod',$cfg_caches);
$cfg_cmtconfig=getVar('cmtconfig','i686-slc5-gcc43-opt',$cfg_cmtconfigs);
$cfg_jobset=getVar('jobset','rawtoesd_bs',$cfg_jobsets);
$cfg_thevar=getVar('var','vmem',$cfg_vars);

$crondir_base=crondirBase($cfg_jobset,$cfg_build,$cfg_cache,$cfg_cmtconfig);
$resdir=$crondir_base.'/results/';
$logdir=$crondir_base.'/logs/';
$tablefile=tableFile($cfg_jobset,$cfg_build,$cfg_cache,$cfg_cmtconfig);

$log_check=$logdir.'cron.log';
$log_update=$logdir.'cron.actualupdate.log';
$rttsvninfo_file=$resdir.'rttsvninfo.txt';
$lock_file=$crondir_base.'/cron.lock';

$has_logs=(file_exists($log_check)&&file_exists($log_update));
$has_jobs=file_exists($resdir.'histplot_jobs_'.filevar($cfg_thevar).'.png');
$has_domains=file_exists($resdir.'histplot_domains_'.filevar($cfg_thevar).'.png');

//$nicos= $has_jobs ? '(<a href="'.nicosLink($cfg_build,$cfg_cache).'">nicos</a>)' : '';
$buildtitle='<a href="'.nicosLink($cfg_build,$cfg_cache).'">Build</a>';
$title=$cfg_jobset.'/'.$cfg_thevar.'/'.$cfg_jobset.'/'.$cfg_build.'/'.$cfg_cache.'/'.$cfg_cmtconfig;


$opts=array('build'=>$cfg_build,'cmtconfig'=>$cfg_cmtconfig,'jobset'=>$cfg_jobset,'cache'=>$cfg_cache,'var'=>$cfg_thevar);
$s_jobset=menustring('jobset',$cfg_jobsets,$cfg_jobset,$opts);
//Uncomment to revert from drop-down box:
//$s_build=menustring('build',$cfg_builds,$cfg_build,$opts);
//$s_cache=menustring('cache',$cfg_caches,$cfg_cache,$opts);
//$s_cmtconfig=menustring('cmtconfig',$cfg_cmtconfigs,$cfg_cmtconfig,$opts);
$s_thevar=menustring('var',$cfg_vars,$cfg_thevar,$opts);

$build_links=Array();
foreach($menuinfo['builds'] as $buildinfo) {
  if (isActivePage($cfg_jobset,$buildinfo['build'],$buildinfo['cache'],$buildinfo['cmtconfig']))
    $build_links[ buildTitle($buildinfo) ] = '?'.menuURLOpts($buildinfo,$opts);
}

//Uncomment to get drop-down box:
// $jobset_links=Array();
// foreach($cfg_jobsets as $jobset) {
//   $jobset_links[ $jobset ] = '?'.menuURLOpts(Array('jobset'=>$jobset),$opts);
// }
// $var_links=Array();
// foreach($cfg_vars as $var) {
//   $var_links[ $var ] = '?'.menuURLOpts(Array('var'=>$var),$opts);
// }


$menus=Array(makeformmenu($buildtitle,buildTitle($opts),$build_links),
	     //"Build: $s_build &times; $s_cmtconfig &times; $s_cache &nbsp; $nicos",
	     //drop-down box: makeformmenu('Test',$cfg_jobset,$jobset_links),
	     "Test: $s_jobset",
	     //drop-down box: makeformmenu('Quantity',$cfg_thevar,$var_links),
	     "Quantity: $s_thevar");

pmbheader('',$title,$menus,false,false,'..');

function printUpdateInfo($has_logs,$log_check,$log_update,$lock_file) {
  $currently_updating=file_exists($lock_file);
  if ($currently_updating) {
    print "<span style=\"color:red;font-weight:bold;\">This page is currently updating!</span><p/>\n";
    if (file_exists($log_check)) {
      $str_dateupdatestart=nicetime(getDateFromLog($log_check));
      print "<em>Update started: $str_dateupdatestart</em><br/><br/>\n";
    }
  } else {
    if ($has_logs) {
      $str_datecheck=nicetime(getDateFromLog($log_check));
      $str_dateupdate=nicetime(getDateFromLog($log_update));
      print "<em style=\"font-size:90%\">Last updated <a href=\"$log_update\">$str_dateupdate</a> ".
	"(checked <a href=\"$log_check\">$str_datecheck</a>).</em><br/>\n";
    }
  }
}

function makeplots($long,$pie,$thevar,$resdir,$has_domains,$has_jobs) {
  if (!$has_jobs&&!$has_domains)
    return;
  print "<p/>\n";
  $suf=filevar($thevar).($long?'_long':'');
  $images=Array();
  if ($has_domains) {
    $h_dom=$resdir.'histplot_domains_'.$suf;
    $t_dom=$resdir.'table_domains_'.$suf;
    $textdom='per-domain '.$thevar.' ('.s_link($t_dom.'.txt','table').')';
    $images[$h_dom]=$textdom;
  }
  if ($has_jobs) {
    $h_job=$resdir.'histplot_jobs_'.$suf;
    $t_job=$resdir.'table_jobs_'.$suf;
    $textjob='per-job '.$thevar.' ('.s_link($t_job.'.txt','table').')';
    $images[$h_job]=$textjob;
  }
  if ($has_domains&&$pie) {
    $images[$resdir.'pie_domains_'.$suf]='relative '.$cfg_thevar.' contribution';
  }
  putmultiimages($images);
}

printUpdateInfo($has_logs,$log_check,$log_update,$lock_file);
print '<p/><em>NB: Complementary info available via <a href="../'.$location_archmon_subdir.'/">archive-based monitoring</a>.</em>'."\n";
$dontshowplots=(!$has_logs)||(!$has_jobs)&&!($has_domains);
if ($dontshowplots) {
  print "   <p/>No <strong>$cfg_thevar</strong> data found for the configuration <strong>$cfg_jobset/$cfg_build/$cfg_cache/$cfg_cmtconfig</strong><p/>\n";
} else {
  if (!$has_domains)
    print "  <p/><em>NB: No domain data available for this test. Showing direct job data only.</em>\n";
  makeplots(false,false,$cfg_thevar,$resdir,$has_domains,$has_jobs);
}
if (file_exists($tablefile)) {
  print '<p/>';
  printTable($tablefile);
} else {
  print "<p/>ERROR: Status table not found!<p/>";
}
if (!$dontshowplots) {
  makeplots(true,true,$cfg_thevar,$resdir,$has_domains,$has_jobs);
};
pmbfooter();
?>
