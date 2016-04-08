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

//Cfg:
$menuinfo=readMenuInfo($GLOBALS['location_rttmondata'].'/menu.txt');
$cfg_recoshifts=Array('15.6.x','15.x.0','muon');
$cfg_recoshift=getVar('recoshift','15.x.0',$cfg_recoshifts);
$standard_cmtconfig='i686-slc5-gcc43-opt';
$tests = Array('rawtoesd_bs', 'rawtoesd_rdo');
if ($cfg_recoshift=='muon')
     $tests = Array('mcp_bs', 'mcp_zmumu', 'muonreco');

function displayBuild($recoshift,$standard_cmtconfig,$buildinfo) {
//   //This function needs to be updated once in a while when shift
//   //definitions change:
  if ($recoshift=='muon') return true;
  $build=$buildinfo['build'];
  if ($recoshift=='15.6.x') return startswith($build,'15.6.');
  if ($recoshift=='15.x.0') return startswith($build,'dev');
  return false;
//   //This function needs to be updated once in a while when shift
//   //definitions change:
//   $cache_prod = $buildinfo['cache']=='prod';
//   $cache_tier0 = (!$cache_prod && $buildinfo['cache']=='tier0');
//   if (!$cache_prod&&!$cache_tier0)
//     return false;
//   $build=$buildinfo['build'];
//   $is_std_cmtconfig = ($standard_cmtconfig==$buildinfo['cmtconfig']);
//   $isSixteen = ($build=='dev' || $build=='devval');
//   if ($isSixteen) {
//     if (!$cache_prod)
//       return false;
//     if ($recoshift=='16.x.y'&&$is_std_cmtconfig)
//       return true;
//     if ($recoshift=='16.x.y_other_build'&&!$is_std_cmtconfig)
//       return true;
//     return false;
//   }
//   if ($recoshift!='15.x.y'||!$is_std_cmtconfig)
//     return false;
//   $is15 = startswith($build,'15.');
// // ( ($build=='15.5.X.Y'&&$cache_tier0)
// // 	    ||($build=='15.5.X.Y-VAL'&&$cache_tier0)
// // 	    ||($build=='15.5.X.Y'&&$cache_prod)//Needs update to 15.6.X.Y soon
// // 	    ||($build=='15.5.X.Y-VAL'&&$cache_prod) );//Needs update to 15.6.X.Y soon
//   return $is15;
}

$opts=array('recoshift'=>$cfg_recoshift);
$s_rttshift=menustring('recoshift',$cfg_recoshifts,$cfg_recoshift,$opts);

pmbheader('','RTT status overview for '.$cfg_jobset.' jobs',Array("Reco shift: $s_rttshift"));

printTableLegend();

$build_links=Array();
$count=0;
foreach($menuinfo['builds'] as $buildinfo) {
  //print "--------------- recoshift=$cfg_recoshift -----------------------\n";
  //print_r($buildinfo);
  if (!displayBuild($cfg_recoshift,$standard_cmtconfig,$buildinfo))
    continue;
  $nicoslink=nicosLink($buildinfo['build'],$buildinfo['cache']);
  foreach($tests as $test) {
    if (isActivePage($test,$buildinfo['build'],$buildinfo['cache'],$buildinfo['cmtconfig'])) {
      $a=$buildinfo;$a['var']='vmem';$a['jobset']=$test;
      $urlopts=menuURLOpts(Array(),$a);
      $testpagelink='?'.$urlopts;
      print '<h3>'.s_link($nicoslink,buildTitle($buildinfo)).' ['.s_link($testpagelink,$test).']</h3>'."\n";
      $tablefile=tableFile($test,$buildinfo['build'],$buildinfo['cache'],$buildinfo['cmtconfig']);
      printTable($tablefile);
      ++$count;
    }
  }
}

if ($count==0) {
  print "<p/><em>No appropriate tests found.</em>\n";
}

pmbfooter();

?>
