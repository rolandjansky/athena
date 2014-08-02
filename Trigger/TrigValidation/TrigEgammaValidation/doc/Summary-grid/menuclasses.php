<?php

class Menu {
  var $MenuItem;  // List of items
  var $name;      // Identifies this menu uniquely
  var $title;     // Title to be shown
  var $menuTitle; // Title on menu bar
  var $alias;     // Alias for referencing this item from outside
  var $job;
  var $group;

  function Menu($name, $menuTitle, $title, $alias) {
    $this->MenuItem = array();
    $this->name = $name;
    $this->title = $title;
    $this->menuTitle = $menuTitle;
    $this->alias = $alias;
    $this->job = "";
  }

  function addMenu($menu) {
    array_push($this->MenuItem, $menu);
  }

  function generateHeader() {
    ?><br><b><? echo $this->title ?></b><br><?
  }
 
  // To be over-written
  function generateContent($father) {
  }
}

class Graph {
  var $src;
  var $desc;

  function Graph($src, $desc) {
    $this->src = $src;
    $this->desc = $desc;
  }
}

class GraphSection {
  var $title;
  var $graphItem;
  function GraphSection($title) {
    $this->title = $title;
    $this->graphItem = array();
  }

  function addGraphItem($src, $desc) {
    array_push($this->graphItem, new Graph($src, $desc));
  }
}

class GraphMenu extends Menu {
  var $graphSection;

  function GraphMenu($name, $menuTitle, $title, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->graphSection = array();
  }

  function addGraphSection($gs) {
    array_push($this->graphSection, $gs);
  }

  function generateContent($father) {
    global $group;
    if ($this->group == "") {
      $this->group = $group;
    }
    foreach($this->graphSection as $key => $value) {
      ?><br><b><? echo $value->title ?></b><br><table><?
      foreach($value->graphItem as $keyItem => $valueItem) {
        $theFile = $valueItem->src;
        if ($father->pattern != "") {
          $theFile = $father->pattern."-".$valueItem->src;
        }
        if (!file_exists(calcfilegroup($this->group, $theFile, false))) {
          ?><tr><td>N/A <?echo "<!-- Looking for ".calcfilegroup($this->group, $theFile,true)."-->"?> </td><td><? echo $valueItem->desc ?></td></tr><?
        } else {
          ?><tr><td><img src="<? echo calcfilegroup($this->group, $theFile, true) ?>"></td><td><? echo $valueItem->desc ?></td></tr><?
        }
      }
      ?></table><?
    }
  }
}

class RawMenu extends Menu {
  var $url;
  function RawMenu($name, $menuTitle, $title, $url, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->url = $url;
  }

  function generateContent($father) {
    include($this->url);
  }

}

class EmptyMenu extends Menu {
  function EmptyMenu($name, $menuTitle, $title, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
  }
  function generateContent($father) {
    include("select.php");
  }
}

class PerfmonMenu extends Menu {
  function PerfmonMenu($name, $menuTitle, $title, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
  }
  function generateContent($father) {
?>
<table>
  <tr><th>Virtual Memory</th></tr>
  <tr><td><img src="<? echo calcfile("vmem_PerfMonSlice.000.gif", true, $this->job) ?>"/></td></tr>
  <tr><td>
  <?
$fitFile = calcfile("out.perfmon.easy.txt", false, $this->job);
if (!file_exists($fitFile)) {
  ?><h4>Fit results unavailable</h4><?
} else {
  $fitHdl = fopen($fitFile, "r");
  ?>
<table>
<tr><th colspan=2>Fit parameters</th></tr>
<tr><td>Linear coefficient</td><td><? echo fgets($fitHdl)." ".fgets($fitHdl); ?></td></tr>
<tr><td>Angular coefficient</td><td><? echo fgets($fitHdl)." ".fgets($fitHdl); ?></td></tr>
</table>
<?
}
?>
  </td></tr>
</table>
<?
  }
}

class JobMenu extends Menu {
  var $pattern;

  function JobMenu($name, $menuTitle, $title, $job, $alias, $pattern="") {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->job = $job;
    $this->pattern = $pattern;
  }

  function generateContent($father) {
    global $group;
    if ($this->group == "") {
      $this->group = $group;
    }
    // Calculate Kolmogorov-Smirnov page
    $fileStr = calcfilegroup($this->group, 'results.txt', false, $this->job);
    if (!file_exists($fileStr)) {
      ?><center><h4>The results of the Kolmogorov-Smirnov test aren't available (yet?!).</h4></center><?
    } else {
      $fileHdl = fopen($fileStr, "r");
      ?>
      <center>
<div style="float:left; padding-left: 3em;">
<table border=1 cellspacing=0 cellpadding=0 style="text-align: left">
  <tr>
    <th>Histogram name</th>
    <th>KS test</th>
    <th>JS test</th>
    <th>KL metric</th>
    <th>QN</th>
  </tr>

  <?
  while (!feof($fileHdl)) {
    $line = fgets($fileHdl);
    if (strpos($line, "omparision") != 0) {
      $data = sscanf($line, "Comparision of histogram named %s with reference: %f%% %f%% %f %f");

      list($histogram, $simks, $simjs, $kl_metric, $qn) = $data;
      $similarity = $simjs;
      if ($simks < $simjs)
        $similarity = $simks;

      if ( ($similarity < 100) && ($similarity > 90) ) {
        echo "<tr bgcolor=\"#00FF00\">";
      } else if ( ($similarity <= 90) && ($similarity > 70) ) {
        echo "<tr bgcolor=\"#FFFF00\">";
      } else if ($similarity <= 70) {
	echo "<tr bgcolor=\"#FF0000\">";
      }
      ?>
     <td>
       <?
         $histogramFile = "";
         list($dir1, $dir2, $hist) = split('/', $histogram);
         if ($dir2 == "Dump") {
           $histogramFile = $dir1.'_'.$dir2.'_'.$hist;
         }
         if ($histogramFile != "") {
       ?>
         <a href="<? printf("%s.gif", calcfilegroup($this->group, $histogramFile, true, $this->job)) ?>" target="_blank"><? printf("%s", $histogram) ?></a>
       <?
         } else {
       ?>
         <? printf("%s", $histogram) ?></a>
       <?
         }
       ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.2f%%", $simks) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.2f%%", $simjs) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.5f", $kl_metric) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.5f", $qn) ?>
     </td>
   </tr>
   
      <?
    }
  }
   ?>
</table>
<?
   fclose($fileHdl);
?>
</div>

<div style="text-align: right; vertical-align: top; width: 30%; float: right;">
<table border=1 cellpadding=0 cellspacing=0 style="text-align: center;">
<tr><td colspan=2>Color code</td></tr>
<tr><td bgcolor="#FFFFFF" width="10%">&nbsp;</td><td>100% similarity</td></tr>
<tr><td bgcolor="#00FF00" width="10%">&nbsp;</td><td>90%-100% similarity</td></tr>
<tr><td bgcolor="#FFFF00" width="10%">&nbsp;</td><td>70%-90% similarity</td></tr>
<tr><td bgcolor="#FF0000" width="10%">&nbsp;</td><td>less than 70% similarity</td></tr>
</table>
</div>

<div style="text-align: right; vertical-align: top; width: 30%; float: right;">
<table border=1 cellpadding=0 cellspacing=0 style="text-align: center;">
<tr><td>Symbol</td><td>Measure</td><td>Description</td></tr>
<tr><td>KS</td><td>Kolmogorov-Smirnov test</td><td>Similarity is calculated based on the maximum distance between the cumulative distribution functions of the random variables.</td></tr>
<tr><td>KL</td><td>Kullback-Leibler Symmetrized metric</td><td>A distance between the random variable is calculated based on information theory's Entropy concept (this is a value greater than zero and unbounded).</td></tr>
<tr><td>JS</td><td>Jensen-Shannon test</td><td>A "normalized version" of the Kullback-Leibler Symmetrized metric, giving a value between 0% and 100%, related to the similarity of the random variable's probability density function.</td></tr>
<tr><td>QN</td><td>Quadratic Negentropy</td><td>Measure of the distance between histograms using L2 metric: QN = sum_x ((p1(x) - p2(x))^2).</td></tr>
</table>
</div>

      </center> 
      <?
    }
  }
}

class TextMenu extends Menu {
  var $file;

  function TextMenu($name, $menuTitle, $title, $file, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->file = $file;
  }

  function generateContent($father) {
    global $BaseAFS;
    global $BaseURL;
    global $run;
    // Find file from pattern
    $foundfile = false;
    if (is_dir($BaseAFS.$run)) {
      if ($dh = opendir($BaseAFS.$run)) {
        while (($file = readdir($dh)) != false) {
          if (ereg($this->file, $file)) {
            $foundfile = $file;
            break;
          }
        }
      }
    }
    if (!$foundfile) {
      ?><b> Couldn't find any file matching pattern <? echo $BaseURL.$run."/".$this->file ?></b><?
    } else {
    ?><center><form><textarea rows="30" cols="140"><?
    $f = fopen($BaseAFS.$run."/".$foundfile, "r");
    while (!feof($f)) {
      $line = fgets($f);
      echo $line."\n";
    }
    ?></textarea></form></center><?
    }
  }
}

class DumpMenu extends Menu {
  var $file;

  function DumpMenu($name, $menuTitle, $title, $file, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->file = $file;
  }

  function generateContent($father) {
    global $BaseAFS;
    global $BaseURL;
    global $run;
    // Find file from pattern
    $foundfile = false;
    if (is_dir($BaseAFS.$run)) {
      if ($dh = opendir($BaseAFS.$run)) {
        while (($file = readdir($dh)) != false) {
          if (ereg($this->file, $file)) {
            $foundfile = $file;
            break;
          }
        }
      }
    }
    if (!$foundfile) {
      ?><b> Couldn't find any file matching pattern <? echo $BaseURL.$run."/".$this->file ?></b><?
    } else {
    ?><b>Dump of file <a href="<? echo $BaseURL.$run."/".$foundfile ?>"><? echo $BaseURL.$run."/".$foundfile ?></a> last modified at <? echo date("d/m/Y", filemtime($BaseAFS.$run."/".$foundfile)) ?></b><br><?
    }
  }
}

class CutsMenu extends Menu {
  var $pattern;

  function CutsMenu($name, $menuTitle, $title, $pattern, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
   $this->pattern = $pattern;
  }

  function dumpGraph($file, $cut) {
?>
    <table>
    <tr><td><img src="<? echo calcfile($file, true) ?>"></td><td><center>Chain <b><? echo $cut ?></b></center></td></tr>
    </table>
<?
  }

  function generateContent($father) {
    global $BaseAFS;
    global $BaseURL;
    global $run;
    // Find file from pattern
    if (is_dir($BaseAFS.$run)) {
      if ($dh = opendir($BaseAFS.$run)) {
        while (($file = readdir($dh)) != false) {
          if (ereg($this->pattern.".*gif$", $file)) {
            // Pattern matched!
            $endingpos = strlen($this->pattern);
            $ending = substr($file, $endingpos);
            $cutpos = strpos($ending, ".gif");
            $cut = substr($ending, 0, $cutpos);
            $this->dumpGraph($file, $cut);
          }
        }
      }
    }
  }
}

class ChainMenu extends Menu {
  var $pattern;

  function ChainMenu($name, $menuTitle, $title, $pattern, $alias) {
    parent::Menu($name, $menuTitle, $title, $alias);
    $this->pattern = $pattern;
  }

  function generateContent($father) {
    global $group;
    if ($this->group == "") {
      $this->group = $group;
    }
    $fileStr = calcfilegroup($this->group, 'results.txt', false, $this->job);
    if (!file_exists($fileStr)) {
      ?><center><h4>The results of the Kolmogorov-Smirnov test aren't available (yet?!).</h4></center><?
    } else {
      $fileHdl = fopen($fileStr, "r");
      ?>
      <center>
<div style="float:left; padding-left: 3em;">
<table border=1 cellspacing=0 cellpadding=0 style="text-align: left">
  <tr>
    <th>Histogram name</th>
    <th>KS test</th>
    <th>JS test</th>
    <th>KL metric</th>
    <th>QN</th>
  </tr>

  <?
  while (!feof($fileHdl)) {
    $line = fgets($fileHdl);
    if (strpos($line, "omparision") != 0) {
      $data = sscanf($line, "Comparision of histogram named %s with reference: %f%% %f%% %f %f");

      list($histogram, $simks, $simjs, $kl_metric, $qn) = $data;
      if (strpos($histogram, "/".$this->pattern."/") == 0) {
        continue;
      }
      $similarity = $simjs;
      if ($simks < $simjs)
        $similarity = $simks;

      if ( ($similarity < 100) && ($similarity > 90) ) {
        echo "<tr bgcolor=\"#00FF00\">";
      } else if ( ($similarity <= 90) && ($similarity > 70) ) {
        echo "<tr bgcolor=\"#FFFF00\">";
      } else if ($similarity <= 70) {
	echo "<tr bgcolor=\"#FF0000\">";
      }
      ?>
     <td>
       <?
         $histogramFile = "";
         list($rootDir, $dir1, $dir2, $hist) = split('/', $histogram);
         if ($dir2 == "Dump") {
           $histogramFile = $dir1.'-'.$dir2.'-'.$hist;
         }
         if ($histogramFile != "") {
       ?>
         <a href="<? printf("%s.gif", calcfilegroup($this->group, $histogramFile, true, $this->job)) ?>" target="_blank"><? printf("%s", $histogram) ?></a>
       <?
         } else {
       ?>
         <? printf("%s", $histogram) ?></a>
       <?
         }
       ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.2f%%", $simks) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.2f%%", $simjs) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.5f", $kl_metric) ?>
     </td>
     <td style="text-align: right">
       <? printf("%0.5f", $qn) ?>
     </td>
   </tr>
   
      <?
    }
  }
   ?>
</table>
<?
   fclose($fileHdl);
?>
</div>

<div style="text-align: right; vertical-align: top; width: 30%; float: right;">
<table border=1 cellpadding=0 cellspacing=0 style="text-align: center;">
<tr><td colspan=2>Color code</td></tr>
<tr><td bgcolor="#FFFFFF" width="10%">&nbsp;</td><td>100% similarity</td></tr>
<tr><td bgcolor="#00FF00" width="10%">&nbsp;</td><td>90%-100% similarity</td></tr>
<tr><td bgcolor="#FFFF00" width="10%">&nbsp;</td><td>70%-90% similarity</td></tr>
<tr><td bgcolor="#FF0000" width="10%">&nbsp;</td><td>less than 70% similarity</td></tr>
</table>
</div>

<div style="text-align: right; vertical-align: top; width: 30%; float: right;">
<table border=1 cellpadding=0 cellspacing=0 style="text-align: center;">
<tr><td>Symbol</td><td>Measure</td><td>Description</td></tr>
<tr><td>KS</td><td>Kolmogorov-Smirnov test</td><td>Similarity is calculated based on the maximum distance between the cumulative distribution functions of the random variables.</td></tr>
<tr><td>KL</td><td>Kullback-Leibler Symmetrized metric</td><td>A distance between the random variable is calculated based on information theory's Entropy concept (this is a value greater than zero and unbounded).</td></tr>
<tr><td>JS</td><td>Jensen-Shannon test</td><td>A "normalized version" of the Kullback-Leibler Symmetrized metric, giving a value between 0% and 100%, related to the similarity of the random variable's probability density function.</td></tr>
<tr><td>QN</td><td>Quadratic Negentropy</td><td>Measure of the distance between histograms using L2 metric: QN = sum_x ((p1(x) - p2(x))^2).</td></tr>
</table>
</div>

      </center> 
      <?
    }
  }
}
?>

