<?

////////////////////////////////////////////////////////////////
//                                                            //
//  PHP file used to create the RTT overview pages for the    //
//  pmb performance monitoring pages.                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

require_once('../common.php');

$menuinfo=readMenuInfo($GLOBALS['location_rttmondata'].'/menu.txt');

$cfg_groups=Array('all','15.5.X','15.6.X','15.X.0','others');
$cfg_group=getVar('group','15.X.0',$cfg_groups);
$cfg_jobsets=$menuinfo['cfg_jobsets'];
$cfg_jobset=getVar('jobset','rawtoesd_bs',$cfg_jobsets);

function inGroup($group,$build) {
  if ($group=='all') return true;
  if ($build=='dev' or $build=='devval') return $group=='15.X.0';
  if ($build=='val' or $build=='bugfix' or substr($build,0,4)=='15.6') return $group=='15.6.X';
  if (substr($build,0,4)=='15.5') return $group=='15.5.X';
  return $group=='others';
}

$opts=array('jobset'=>$cfg_jobset,'group'=>$cfg_group);
$s_jobset=menustring('jobset',$cfg_jobsets,$cfg_jobset,$opts);
$s_group=menustring('group',$cfg_groups,$cfg_group,$opts);

pmbheader('','RTT status overview for '.$cfg_jobset.' jobs',Array("Builds: $s_group", "Test: $s_jobset"));

printTableLegend();

$build_links=Array();
foreach($menuinfo['builds'] as $buildinfo) {
  if (!inGroup($cfg_group,$buildinfo['build']))
    continue;
  $tablefile=tableFile($cfg_jobset,$buildinfo['build'],$buildinfo['cache'],$buildinfo['cmtconfig']);
  if (isActivePage($cfg_jobset,$buildinfo['build'],$buildinfo['cache'],$buildinfo['cmtconfig'])) {
    $nicoslink=nicosLink($buildinfo['build'],$buildinfo['cache']);
    $a=$buildinfo;$a['var']='vmem';$a['jobset']=$cfg_jobset;
    $urlopts=menuURLOpts(Array(),$a);
    $testpagelink='?'.$urlopts;
    print '<h3>'.s_link($nicoslink,buildTitle($buildinfo)).' ['.s_link($testpagelink,'perf').']</h3>'."\n";
    printTable($tablefile);
  }
}

pmbfooter();

?>
