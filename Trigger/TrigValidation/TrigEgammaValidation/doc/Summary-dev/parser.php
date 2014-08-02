<?

// Parse XML file
class XMLParser {
  var $parser;
  var $data;
  var $filePath;
  var $currTag;
  var $tagStack;

  var $menu;

  var $chainlists;
  var $joblists;

  function XMLParser($path) {
    $this->parser = xml_parser_create();
    $this->filePath = $path;
    $this->data = array();
    $this->currTag =& $this->data;
    $this->tagStack = array();
    $this->menu = new Menu("validation", "", "", "");
    $this->chainlists = array();
    $this->joblists = array();
  }

  function parse() {
    xml_set_object($this->parser, $this);
    xml_parser_set_option($this->parser, XML_OPTION_SKIP_WHITE, 1);
    xml_set_element_handler($this->parser, 'tag_open', 'tag_close');
    xml_set_character_data_handler($this->parser, 'cdata');
    if (!($fp = fopen($this->filePath, "r"))) {
      die("Cannot open XML file $this->filePath.\n");
      return false;
    }

    while ($data = fread($fp, 4096)) {
      if (!xml_parse($this->parser, $data, feof($fp))) {
        die(sprintf('Error parsing XML file. %s. Line %d', xml_error_string(xml_get_error_code($this->parser)), xml_get_current_line_number($this->parser)));
        return false;
      }
    }
    fclose($fp);
    xml_parser_free($this->parser);
    return true;
  }

  function tag_open($parser, $name, $attribs) {
    $name = strtolower($name);

    if(!isset($this->currTag[$name]))
      $this->currTag[$name] = array();
      
    $newTag = array();
    if(!empty($attribs)) {
      foreach($attribs as $attrname => $attrvalue) {
        $newTag['attr'][strtolower($attrname)] = $attrvalue;
      }
    }
    array_push($this->currTag[$name], $newTag);
      
    $t =& $this->currTag[$name];
    $this->currTag =& $t[count($t)-1];
    array_push($this->tagStack, $name);

  }

  function cdata($parser, $cdata) {
    $cdata = trim($cdata);
      
    if(!empty($cdata)) {
      if(isset($this->currTag['cdata']))
        $this->currTag['cdata'] .= $cdata;
      else
        $this->currTag['cdata'] = $cdata;
    }

  }

  function tag_close($parser, $name) {
    $this->currTag =& $this->data;
    array_pop($this->tagStack);

    for($i = 0; $i < count($this->tagStack); $i++) {
      $t =& $this->currTag[$this->tagStack[$i]];
      $this->currTag =& $t[count($t)-1];
    }
  }

  function ShowMenu($current, &$lvl) {
    global $release;
    foreach ($current[0]->MenuItem as $k => $c) {
      if ($lvl == 0) {
?>
<div id="topLevel">
<?
      }
?>
<li>
<?
      if ( (count($c->MenuItem) > 0)) {
?>
<span onClick="showhide(i<? echo $k."_".$c->name ?>, img<? echo $k."_".$c->name  ?>)">
<img id="img<? echo $k."_".$c->name ?>" src="plus.gif">
</span><?
      } else {
?>
<span><img src="none.gif"></span>
<?
      }
      ?>
<a href="<? echo calcget("summary.php", $release, $c->name) ?>" target="BodyFrame"><? echo $c->menuTitle ?></a>
      <?
      $lvl = $lvl + 1;
      if (count($c->MenuItem) > 0) {
?>
<div id="floatMe">
<ul id="i<? echo $k."_".$c->name ?>">
<?
        $this->ShowMenu(array(&$c), $lvl);
?>
</ul>
</div>
<?
      }
      $lvl = $lvl - 1;
?></li><?
      if ($lvl == 0) {
?>
</div>
<?
      }
    }
  }

  function &GetMenuItem($current, $item, &$job, $isAlias = false) {
    $retItem = false;
    $retFather = false;
    foreach ($current[0]->MenuItem as $k => $c) {
      $varToCompare = $c->name;
      if ($isAlias) {
        $varToCompare = $c->alias;
      }
      if ($varToCompare == $item) {
        $retItem =& $c;
        $retFather =& $current[0];
        $job = $retItem->job;
        return array($retItem, $retFather);
      }

      if (($myArray = $this->GetMenuItem(array(&$c,&$current[0]), $item, $job, $isAlias)) != false) {
        return $myArray;
      }
    }
    return false;
  }

  function DoMenu() {
    global $group;
    $tags = $this->data;
    $id = 1;
    foreach ($tags as $tag => $content) {
      $this->ProcessMenu($tag, $content, &$this->menu, $id, "", "", $group);
    }
  }

  function ProcessMenu(&$tag, &$content, $curr, &$id, $lastjob, $lastalias = '', $lastgroup = '') {
    foreach($content as $index => $contentSon) {
      if ($tag == "chainlist") {
        $this->chainlists[$contentSon['attr']['id']] = array();
        foreach ($contentSon['chain'] as $idxChain => $oneChain) {
          $newMenu = new ChainMenu($id, $oneChain['attr']['title'], $oneChain['attr']['menu-title'], $oneChain['attr']['pattern'], $oneChain['attr']['alias']);
          $newMenu->group = $lastgroup;
          array_push($this->chainlists[$contentSon['attr']['id']], $newMenu);
          $id = $id + 1;
        }
      } else if ($tag == "joblist") {
        $this->joblists[$contentSon['attr']['id']] = array();
        foreach ($contentSon['job'] as $idxChain => $oneChain) {
          $newMenu = new JobMenu($id, $oneChain['attr']['title'], $oneChain['attr']['menu-title'], $oneChain['attr']['job'], $oneChain['attr']['alias'], $oneChain['attr']['pattern']);
          $newMenu->group = $lastgroup;

          array_push($this->joblists[$contentSon['attr']['id']], $newMenu);
          $id = $id + 1;
        }
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'graph')) {
        $newMenu = new GraphMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        foreach($contentSon['section'] as $idx => $section) {
          $newSection = new GraphSection($section['attr']['title']);
          foreach($section as $idxSec => $subsec) {
            if ($idxSec == 'img') {
              foreach($subsec as $idxImg => $img) {
                $desc = $img['cdata'];
                $newSection->addGraphItem($img['attr']['src'], $desc);
              }
            }
          }
          $newMenu->addGraphSection($newSection);
        }
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'raw')) {
        $newMenu = new RawMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['url'][0]['cdata'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'empty')) {
        $newMenu = new EmptyMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'job')) {
        if ($contentSon['attr']['jobid'] != '') {
          $joblist = $this->joblists[$contentSon['attr']['jobid']];
          foreach ($joblist as $jobKey => $jobValue) {
            $newMenu = $jobValue;
            $id = $id + 1;
            $newMenu->job = $lastjob;
            $newMenu->group = $lastgroup;
            $lastalias = $newMenu->alias;
            foreach($contentSon as $tag1 => $tag1Content) {
              $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
            }
            $curr->addMenu($newMenu);
          }
        } else {
          $newMenu = new JobMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['attr']['job'], $contentSon['attr']['alias'].$lastalias);
          $id = $id + 1;
          $lastjob = $newMenu->job;
          $lastalias = $newMenu->alias;
          $lastgroup = $contentSon['attr']['group'];
          $newMenu->group = $lastgroup;
          foreach($contentSon as $tag1 => $tag1Content) {
            $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
          }
          $curr->addMenu($newMenu);
        }
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'text')) {
        $newMenu = new TextMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['url'][0]['cdata'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'dump')) {
        $newMenu = new DumpMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['url'][0]['cdata'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'cuts')) {
        $newMenu = new CutsMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['pattern'][0]['cdata'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content,&$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
        $newMenu->group = $lastgroup;
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'perfmon')) {
        $newMenu = new PerfmonMenu($id, $contentSon['attr']['title'], $contentSon['attr']['menu-title'], $contentSon['pattern'][0]['cdata'], $contentSon['attr']['alias'].$lastalias);
        $id = $id + 1;
        
        $newMenu->job = $lastjob;
        $newMenu->group = $lastgroup;
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
        }
        $curr->addMenu($newMenu);
      } else if ( ($tag == 'item') && ($contentSon['attr']['mode'] == 'chain')) {
        $chainlist = $this->chainlists[$contentSon['attr']['id']];
        foreach ($chainlist as $chainKey => $chainValue) {
          $newMenu = $chainValue;
          $id = $id + 1;
          $newMenu->job = $lastjob;
          $newMenu->group = $lastgroup;
          foreach($contentSon as $tag1 => $tag1Content) {
            $this->ProcessMenu($tag1, $tag1Content, &$newMenu, $id, $lastjob, $lastalias, $lastgroup);
          }
          $curr->addMenu($newMenu);
        }
      } else if ($tag == 'validation') {
        foreach($contentSon as $tag1 => $tag1Content) {
          $this->ProcessMenu($tag1, $tag1Content, &$curr, $id, $lastjob, $lastalias, $lastgroup);
        }
      }
    }
  }
}


?>

