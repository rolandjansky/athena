<?php
require_once('../common.php');

$devel=endswith($_SERVER['PHP_SELF'],'_devel.php');
$test2info=Array();
$cfg_tests=Array();
$menu=Array();
foreach (glob( 'pages_*/*/*/html.snippet') as $d) {
  if (!(strpos($d, '__tmp__')===false))
    continue;//ignore temporary directories
  $s=split('/',$d);
  //$dirname=substr($s[0],6);
  $testname=$s[1];
  $pageset=split('_',$s[0]);
  if ((count($pageset)==3&&$pageset[2]=='devel')!=$devel)
      continue;
  $testsubname=$s[2];
  $pretty="${testname} (".str_replace('__','/',$testsubname).')';
  $testkey="${testname}/${testsubname}";
  $test2info[$testkey]=Array('snippet'=>$d,'pretty'=>$pretty);
  array_push($cfg_tests,$testkey);
  if ($testname=='oldschool') { array_push($cfg_tests,"perftests/${testsubname}"); }//backwards compat

  //for new menu:
  if (!array_key_exists($testname,$menu)) {
    $menu[$testname]=Array();
  }
  if (!array_key_exists($testsubname,$menu[$testname])) {
    $menu[$testname][$testsubname]=Array();
  }
  $menu[$testname][$testsubname]=$testkey;
}

function prettyTitle($test2nfo,$testkey) {
  assert(array_key_exists($testkey,$test2nfo));
  //assert(array_key_exists($test2nfo,$testkey));
  return $test2nfo[$testkey]['pretty'];
}

$cfg_test=getVar('test','all/overview',$cfg_tests);
//$cfg_test=getVar('test','oldschool_perftests/dev__i686-slc5-gcc43-opt__offline',$cfg_tests);

if (startswith($cfg_test, 'perftests/')) { $cfg_test=str_replace('perftests/','oldschool/',$cfg_test); }//backwards compat

$the_opts=Array('test'=>$cfg_test);

$build_links=Array();
foreach($test2info as $testkey=>$testnfo) {
  assert(is_array($the_opts));
  $build_links[ $testnfo['pretty'] ] = '?'.menuURLOpts(Array('test'=>$testkey),$the_opts);
  //print $testkey.'---'.$testnfo."<br/>\n";
}

function print_menu($menu,$indent='') {
  $the_opts=$GLOBALS['the_opts'];
  print $indent.'<ul id="nav">'."\n";
  foreach($menu as $menuheader=>$submenu) {
    if ($menuheader=='all') {
      //fixme: this is really a hack...
      $testkey=array_pop(array_values($submenu));
      assert(is_array($the_opts));
      $thelink='?'.menuURLOpts(Array('test'=>$testkey),$the_opts);
      print $indent.'  <li>'."\n";
      print $indent.'    <a href="'.$thelink.'">home</a>'."\n";
      print $indent.'  </li>'."\n";
      continue;
    }
    print $indent.'  <li>'."\n";
    print $indent.'    <a href="#">'.$menuheader.'</a>'."\n";
    print $indent.'    <ul>'."\n";
    foreach($submenu as $menuitem=>$menuitem_testkey) {
      $thelink='?'.menuURLOpts(Array('test'=>$menuitem_testkey),$the_opts);
      print $indent.'      <li><a href="'.$thelink.'">'.str_replace('__','/',$menuitem).'</a></li>'."\n";
    }
    print $indent.'    </ul>'."\n";
    print $indent.'  </li>'."\n";
  }
  print $indent.'</ul>'."\n";
}

$title=prettyTitle($test2info,$cfg_test);
pmbheader('','PMB performance monitoring page: '.$title,Array(),false,'menustyle.css','..');
print_menu($menu);
$htmlsnippet=$test2info[$cfg_test]['snippet'];
printTable($htmlsnippet,true,dirname($htmlsnippet));
print '    <span style="font-size:70%">Trend data based on the <a href="../'.$location_archive_subdir.'/">archive</a>. Alternatively, look at Dozer based trends <a href="../'.$location_rttmon_subdir.'/">here</a>.</span>'."\n";
pmbfooter();

?>