<?

require('locations.php');
////////////////////////////////////////////////////////////////
//                                                            //
//  Common php methods for the RTT-based pmb performance      //
//  monitoring pages.                                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

function startswith($Haystack, $Needle){
    return strpos($Haystack, $Needle) === 0;
}

function endswith($Haystack, $Needle){
    return strrpos($Haystack, $Needle) === strlen($Haystack)-strlen($Needle);
}

function strcontains($Haystack, $Needle){
    return !(strpos($Haystack, $Needle) === false);
}

if (!function_exists('scandir')) {
  function scandir($dir = './', $sort = 0)
    {
      $dir_open = @ opendir($dir);
      if (! $dir_open)
        return false;
      while (($dir_content = readdir($dir_open)) !== false)
	$files[] = $dir_content;
      if ($sort == 1) rsort($files, SORT_STRING);
      else sort($files, SORT_STRING);
      return $files;
    }
}

function nicetime($date)
{
  //Found on php.net (by yasmary at gmail dot com)
  if(empty($date))
    return "No date provided";
  $periods         = array("second", "minute", "hour", "day", "week", "month", "year", "decade");
  $lengths         = array("60","60","24","7","4.35","12","10");
  $now             = time();
  $unix_date       = strtotime($date);
  // check validity of date
  if(empty($unix_date))
    return "Bad date";
  // is it future date or past date
  if($now > $unix_date) {
    $difference     = $now - $unix_date;
    $tense         = "ago";
  } else {
    $difference     = $unix_date - $now;
    $tense         = "from now";
  }
  for($j = 0; $difference >= $lengths[$j] && $j < count($lengths)-1; $j++)
    $difference /= $lengths[$j];
  $difference = round($difference);
  if($difference != 1)
    $periods[$j].= "s";
  return "$difference $periods[$j] {$tense}";
}

function getContents($filename) {
  if( false == ($str=file_get_contents( $filename )))
    return false;
  return $str;
}

function buildTitle($opts) {
  return $opts['build'].'/'.$opts['cache'].'/'.$opts['cmtconfig'];
  //  return $opts['build'].' / '.$opts['cache'].' / '.$opts['cmtconfig'];
}

function crondirBase($jobset,$build,$cache,$cmtconfig) {
  return $GLOBALS['location_rttmondata_linkname'].'/'.$jobset.'-'.$build.'-'.$cache.'-'.$cmtconfig;
}

function tableFile($jobset,$build,$cache,$cmtconfig) {
  return $GLOBALS['location_rttmondata'].'/tables/table_'.$jobset.'_'.$build.'_'.$cache.'_'.$cmtconfig.'.html.snippet';
}

function readMenuInfo($filename) {#='data/menu.txt'
  $cont=getContents($filename);
  if (!$cont) {
    print "ERROR: Menu information missing.";
    exit(1);
  }
  $b=Array();
  $t=Array();
  $q=Array();
  $cfg_builds=Array();
  $cfg_caches=Array();
  $cfg_cmtconfigs=Array();
  $lines=explode("\n",$cont);
  foreach($lines as $line) {
    $parts=explode(';',$line);
    $c=count($parts);
    if ($c<2)
      continue;
    if ($c==4 && $parts[0]=='B') {
      array_push($b,Array('build'=>$parts[1],'cache'=>$parts[2],'cmtconfig'=>$parts[3]));
      array_push($cfg_builds,$parts[1]);
      array_push($cfg_caches,$parts[2]);
      array_push($cfg_cmtconfigs,$parts[3]);
    } else if ($c==2 && $parts[0]=='T') {
      array_push($t,$parts[1]);
    } else if ($c==2 && $parts[0]=='V') {
      array_push($q,$parts[1]);
    }
  }
  return Array('builds'=>$b,'cfg_jobsets'=>$t,'cfg_vars'=>$q,
	       'cfg_builds'=>array_unique($cfg_builds),
	       'cfg_caches'=>array_unique($cfg_caches),
	       'cfg_cmtconfigs'=>array_unique($cfg_cmtconfigs));
}

function readFirstLineFromFile($filename) {
  $line='';
  if($fh = fopen($filename,"r")){
    if (!feof($fh))
      $line=fgets($fh,100);
    fclose($fh);
  }
  return $line;
};

function getDateFromLog($filename) {
  $logheader=readFirstLineFromFile($filename);
  if (strlen($logheader)<10||substr($logheader,0,5)!="DATE:")
    return "(unknown date)";
  return chop(substr($logheader,6));
}

function s_link($href,$content) { return '<a href="'.$href.'">'.$content.'</a>'; }
function putLink($href,$content) {
  print s_link($href,$content)."\n";
}

function s_img($filebase) {
  $filename_th=$filebase.'_small.jpg';
  $filename=$filebase.'.png';
  //$size = getimagesize($filename);
  return s_link($filename,'<img alt="The image '.$filename.'" src="'.$filename_th.'" />');
}

function putmultiimages($filebase_to_text) {
  $imagetablestyle='style="font-size:90%;font-style:italic;color:rgb(100,100,100);"';//same as above
  $anytext=false;
  print "<table $imagetablestyle>\n";
  print "  <tr>\n";
  foreach($filebase_to_text as $fb => $t) {
    if ($t!='')
      $anytext=true;
    print "<th>".s_img($fb)."</th>\n";
  }
  print "  </tr>\n";
  if ($anytext) {
    print "  <tr>\n";
    foreach($filebase_to_text as $fb => $t)
      print "<th>$t</th>\n";
    print "  </tr>\n";
  }
  print "</table>\n";
};

function pmbheader($title,$titleprop,$menus=Array(),$togglediv=false,$stylesheet=false,$baseurl='.') {
  print '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">'."\n";
  print '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">'."\n";
  print '<head>'."\n";
  print '  <title>'."$titleprop".'</title>'."\n";
  print '  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />'."\n";
  if ($stylesheet)
    print '  <link rel="stylesheet" type="text/css" href="'.$stylesheet.'" />'."\n";
  if (!endswith($baseurl,'/'))
    $baseurl.='/';
  print '  <link rel="icon" href="'.$baseurl.'favicon.png" type="image/x-icon" />'."\n";
  print '  <link rel="shortcut icon" href="'.$baseurl.'favicon.png" type="image/x-icon" />'."\n";
  print '  <meta name="expires" content="0" />'."\n";
  if (count($menus)>1) {
    print '  <script type="text/javascript">'."\n";
    print '     function jumpMenu(el){'."\n";
    print '       popup = el.options[el.selectedIndex].value;'."\n";
    print '       if(popup == \'#\') return false; else location.href=popup;'."\n";
    print '     }'."\n";
    print '  </script>'."\n";
  }
  if ($togglediv) {
    print '<script type="text/javascript">'."\n";
    print '  function toggleDiv(divid){'."\n";
    print '    if(document.getElementById(divid).style.display == \'none\'){'."\n";
    print '      document.getElementById(divid).style.display = \'block\';'."\n";
    print '    }else{'."\n";
    print '      document.getElementById(divid).style.display = \'none\';'."\n";
    print '    }'."\n";
    print '  }'."\n";
    print '</script>'."\n";
  }
  print '</head>'."\n";
  print '<body>'."\n";
  if (count($menus)>0) {
    print '  <table style="margin-top:-2pt;margin-bottom:5pt;width:100%">'."\n";
    $menustyle='style="text-align:left;background-color:rgb(200,200,255);margin:0pt;margin-top:400pt;padding-left:5pt;padding-right:5pt;"';
    foreach($menus as $menu)
      print "    <tr><th $menustyle>$menu</th></tr>\n";
    print '  </table>'."\n";
  }
  print '  <div>'."\n";
  if ($title!='')
    print '  <h2>'."$title".'</h2>'."\n";
};

function pmbfooter() {
  $text='Page maintained by <a href="http://consult.cern.ch/xwho/people/555216">Thomas Kittelmann</a>';
  print '<p/>  </div>'."\n";
  $thisurl=rawurlencode("http://".$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"]);
  if ($text!='')
    $text = "<em>$text</em>";

  print "  <div class=\"footer\">\n  $text  (check <a href=\"http://validator.w3.org/check?uri=$thisurl\">"."html</a>/<a href=\"http://jigsaw.w3.org/css-validator/validator?warning=1&amp;uri=$thisurl\">css</a>)<p/>\n</div>\n";
  print '</body>'."\n";
  print '</html>'."\n";

};

function getVar($key,$default,$allowedvalues) {
  if (!array_key_exists($key,$_GET)) {
    if (in_array($default,$allowedvalues))
      return $default;
    return $allowedvalues[0];
  }
  $val=$_GET[$key];
  if (!in_array($val,$allowedvalues)) {
    print "ERROR: $key=$val not recognised. Problem might be temporary so try to reload in a few seconds.\n";
    exit(1);
  }
  return $val;
};

function printTable($tablefile,$correctlastupdate=true,$translatelinks='') {
  $tl=($translatelinks!='');
  $hreftarget='href="'.$translatelinks.'/';
  $srctarget='src="'.$translatelinks.'/';
  if($fh = fopen($tablefile,"r")){
    while(!feof($fh)) {
      $line=fgets($fh);
      if ($tl) {
	if (strstr($line,'href=')) {
	  $line=str_replace('href="http','HREF="http',$line);
	  $line=str_replace('href="!','HREF="',$line);
	  $line=str_replace('href="',$hreftarget,$line);
	  $line=str_replace('HREF="','href="',$line);
	}
	if (strstr($line,'src=')) {
	  $line=str_replace('src="http','SRC="http',$line);
	  $line=str_replace('src="!','SRC="',$line);
	  $line=str_replace('src="',$srctarget,$line);
	  $line=str_replace('SRC="','src="',$line);
	}
      }
      $i=($correctlastupdate&&strpos($line,'LASTUPDATE['));
      if ($i===false) {
	print $line;
      } else {
	$parts=explode('LASTUPDATE[',$line,2);
	$parts2=explode(']',$parts[1],2);
	print $parts[0].nicetime($parts2[0]).$parts2[1];
      }
    }
    fclose($fh);
  }
}

function printTableLegend() {
  //Fixme: Should be produced by RTTUtils.py to ensure consistency
  print '<table style="margin-right:3%;float:right;font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;">'."\n";
  print '  <tr><th style="text-align:center;background-color:rgb(200,200,255);" colspan="2">Table legend</th></tr>'."\n";
  print '  <tr><th><span style="color:green;">reln</span></th><th>Success</th></tr>'."\n";
  print '  <tr><th><span style="color:red;">reln</span></th><th>Error</th></tr>'."\n";
  print '  <tr><th><span style="color:orange;">reln</span></th><th>Unavailable/running</th></tr>'."\n";
  print '  <tr><th><span style="color:gray;">reln</span></th><th>Missing/outdated</th></tr>'."\n";
  print '  <tr><th><span style="color:#8A2BE2;">reln</span></th><th>Success but w/o dozer</th></tr>'."\n";
  print '  <tr><th><span style="color:#483D8B;">reln</span></th><th>Success but perhaps w/o dozer</th></tr>'."\n";
  print '  <tr><th colspan="2">&nbsp;&rarr; More recent entries at the right</th></tr>'."\n";
  print '  <tr><th colspan="2">&nbsp;&rarr; Mouse-over entries for status</th></tr>'."\n";
  print '  <tr><th colspan="2">&nbsp;&rarr; Time since update listed in table</th></tr>'."\n";
  print "</table>\n";
}

function isActivePage($jobset,$build,$cache,$cmtconfig) {
  //NB: This file_exists check makes the page load noticably slower...
  return file_exists(tableFile($jobset,$build,$cache,$cmtconfig));
  //return file_exists(crondirBase($jobset,$build,$cache,$cmtconfig).'/results/table_jobs_vmem.txt');//NB: We simply look at short-term vmem info here...
}

function filevar($varname) { return $varname=='cpu' ? 'cpu_evt' : ($varname=='malloc' ? 'malloc_evt' : $varname); }

function nicosID($build,$cache) {
  //We have to hardcode a bit here:
  $devbranch='17.X.0';
  $bugfixbranch='UNKNOWN';
  if ($cache=='prod') {
    if ($build=='dev') return $devbranch;
    if ($build=='devval') return $devbranch.'-VAL';
    if ($build=='bugfix') return $bugfixbranch;
    if ($build=='val') return $bugfixbranch.'-VAL';
  }
  if (strcontains($build,'.Y'))
    return $build.'-'.( $cache=='tier0' ? 'T0' : ucfirst($cache) );
  if (strcontains($build,'mig'))
    return $devbranch.'-'.strtoupper($build);
  return $build;
}

function nicosLink($build,$cache) {
  return 'http://atlas-computing.web.cern.ch/atlas-computing/links/distDirectory/nightlies/global/index'.nicosID($build,$cache).'.html';
}

function menuURLOpts($targetOpts,$AllCurrentOpts) {
  if (!is_array($AllCurrentOpts)) {
    var_dump(debug_backtrace());
    exit;
  }
  assert(is_array($AllCurrentOpts));
  foreach($AllCurrentOpts as $k=>$v) {
    if(!array_key_exists($k,$targetOpts))
      $targetOpts[$k]=$v;
  }
  $s='';
   foreach($targetOpts as $k=>$v) {
     if ($s!='')
       $s.='&amp;';
     $s.="$k=$v";
   }
  return $s;
}

function menustring($key,$values,$currentvalue,$opts) {
  $s='[';
  $first=true;
  foreach($values as $v) {
    if ($first!=true)
      $s.=', ';
    if ($v==$currentvalue) {
      $s.="<strong>".strtolower($v)."</strong>";
    } else {
      assert(is_array($opts));
      $s.="<a href=\"?".menuURLOpts(Array($key=>$v),$opts)."\">".strtolower($v)."</a>";
    }
    $first=false;
  };
  $s.=']';
  return $s;
}

function makeformmenu($title,$current, $entriesToOpts,$extra='',$specials=Array()) {
  $form='<select style="background-color: rgb(200, 200, 255);"  name="page" onchange="jumpMenu(this)"><option value="#">Select other...</option>';
  foreach($entriesToOpts as $e=>$url)
    $form.='<option '.($current==$e?'style="color:red" ':'').'value="'.$url.'">'.$e.'</option>';
  $form.='</select>';
  foreach($specials as $text=>$url)
    $current.=', <a href="'.$url.'">'.$text.'</a>';
  return $title.': ['.$current.']'.($extra!='' ?' '.$extra : '' ).' '.$form;//.' '.$form;
}

?>
