<?php
/**
 * @file dcube.php 
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @date 30/05/2007
 * @brief The PHP part of dcube. "fri:d:cub"
 *
 *  This is a part of dcube package - offline monitoring of root histograms.
 *  Copylefts (C) 2007  Krzysztof Ciba
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

// start gzip compression
ob_start("ob_gzhandler");

// set headers
header("Content-type: text/html; charset=utf-8");
header("Cache-Control: must-revalidate");
$offset = 60 * 60 * 12 ;
$ExpStr = "Expires: " . 
gmdate("D, d M Y H:i:s",
time() + $offset) . " GMT";
header($ExpStr);

/**
 * @class dcube_header
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @date 30/05/2007
 * 
 * @brief This class produces HTML header for DCubeServer page.
 * 
 */
class dcube_header {

  /**
   * @param $project string with project name
   * @param $version string with project version number
   * @param $author  string with my name :)
   * @param $css     path (absolute or relative) to CSS file  
   * @param $js      path (absoulte or relative to JS file
   * @param $where   relative path to dcube.php file
   */
  function dcube_header( $project, $version, $author, $css, $js, $where ) {
    

    echo "\n<head>\n";
    echo "<title>".$version.$project." by ".$author." </title>\n";
    echo "<meta http-equiv='content-type' content='text/html; charset=utf-8'>\n";
    echo "<meta name ='description' content='DCubeServer page'>\n";
    echo "<meta name='keywords' content='webbased root histogram displaying ATLAS CERN DCube'>\n";
    echo "<meta http-equiv='content-language' content='en'>\n";
    echo "<meta name='generator' content='GNU Emacs 22.3.2'>\n";
    echo "<meta name='author' content='Krzysztof Daniel Ciba'>\n";
    echo "<link rel='stylesheet' href='./".$where."/".$css."' type='text/css'>\n";
    echo "<link rel='shortcut icon' href='./".$where."/images/favicon.gif' type='image/gif'>\n";
    echo "<link rel='icon' type='image/gif' href='./".$where."/images/favicon.gif'>\n"; 
    echo "<script type='text/javascript' src='./".$where."/preLoading.js'></script>\n";
    echo "<script type='text/javascript' src='./".$where."/".$js."'></script>\n";
    echo "</head>\n";
  
  }
  
}

/**
 * @class dcube_xml_parser
 * @author Krzysztof Ciba <Krzysztof.Ciba@NOSPAMagh.edu.pl>
 *
 * @brief An awful XML parser.
 */
class dcube_xml_parser {
  
  var $parser;
  var $data;
  var $file;
  var $currTag;
  var $tagStack;
  
  /**
   * @param $xml_file path (absoulte or relative) to XML file
   */
  function dcube_xml_parser( $xml_file ) {
    
    $this->parser = xml_parser_create();
    $this->file = $xml_file;
    $this->data = array();
    $this->currTag =& $this->data;
    $this->tagStack = array();
    
  }
  
  /** main worker here */
  function parse() {
    
    xml_set_object($this->parser, $this);
    xml_parser_set_option($this->parser, XML_OPTION_SKIP_WHITE, 1);
    xml_set_element_handler($this->parser, 'tag_open', 'tag_close');
    xml_set_character_data_handler($this->parser, 'cdata');
    if ( ! ( $fp = fopen($this->file, "r") ) ) {
      die( "Cannot open XML file" . $this->file . "\n" );
      return false;
    }
    
    while ($data = fread($fp, 4096)) {
      if ( ! xml_parse( $this->parser, $data, feof( $fp ) ) ) {
				die( sprintf( 'Error parsing XML file. %s. Line %d', 
					      xml_error_string( xml_get_error_code($this->parser) ), 
					      xml_get_current_line_number( $this->parser ) ) );
				return false;
      }
    }
    
    fclose($fp);
    xml_parser_free($this->parser);
    return true;
  }
  
  /** function to run if opening tag is found */
  function tag_open( $parser, $name, $attribs ) {
    
    $name = strtolower($name);
    $tagname = $name;
    /* keep all plots together in one list, so we maintain the order in the xml */
    if ( $name == "hist1d" || $name == "hist2d" || $name == "graph" ) $name = "plot";
    
    if( ! isset( $this->currTag[$name] ) )
      $this->currTag[$name] = array();      
    $newTag = array();
    if( ! empty($attribs) ) {
      /* give default icons for TH1 or TH2 or TGraph */
      $newTag['attr']['type']='TH1';
      if ( strstr($tagname, "hist2d") ) {
	$newTag['attr']['type']='TH2';
      }
      if ( strstr($tagname, "graph") ) {
	$newTag['attr']['type']='TGraph';
      }
      /* end fix */
      foreach( $attribs as $attrname => $attrvalue ) {
				$newTag['attr'][strtolower($attrname)] = $attrvalue;
      }
    }
    array_push( $this->currTag[$name], $newTag );      
    $t =& $this->currTag[$name];
    $this->currTag =& $t[count($t)-1];
    array_push($this->tagStack, $name);
    
  }
  
  /* function to store CDATA into array  */
  function cdata( $parser, $cdata ) {
    $cdata = trim($cdata);
    
    if( ! empty($cdata) ) {
      if(isset($this->currTag['cdata']))
	$this->currTag['cdata'] .= $cdata;
      else
	$this->currTag['cdata'] = $cdata;
    }
  }
  
  /* function to run if closing tag is found */
  function tag_close( $parser, $name ) {
    $this->currTag =& $this->data;
    array_pop($this->tagStack);
    
    for( $i = 0; $i < count($this->tagStack); $i++ ) {
      $t =& $this->currTag[$this->tagStack[$i]];
      $this->currTag =& $t[count($t)-1];
    }
  }
  
}

/**
 * @class dcube_body
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 *
 * @brief A class for displaying body part of DCubeServer page.
 */
class dcube_body {

	/** reference to XML parser */
	var $xml_parser;

	/** string with generated HTML */
	var $body;
 
	/** ??? */
	var $xml;
	
	/** default p-value limit for warning */
	var $pwarn = 0.95;

	/** default p-value limit for failed tests */
	var $pfail = 0.7;

	/** counter for histograms */
	var $id = 0;

	/** counter for directories */
	var $did = 0;

	/** ??? */
	var $plotDir = "";

	/** histogram counter */
	var $hnum = 0;

	/** relative path from this file to the xml output file */
	var $where = "";

	/** DOM script  */
	var $script = "";

	/** JS DOM table for img src */
	var $picTableJS = "";

	/** JS DOM table for img info*/
	var $picInfoTableJS = "";

	var $picNum = 0;

	var $numPicToShow = 20;

	/**
	 * @param $project string with name of project
	 * @param $version string with project version
	 * @param $author  my name :)
	 * @param $xml_file XML file to parse and display
	 * @param $log_file path to log file from python processing 
	 * @param $pDir directory with plots to display
	 * @param $lDir directory with log file
	 * @param $where relative path to dcube.php file
	 */
	function dcube_body( $project, $version, $author, $xml_file, $pDir, $log_file, $lDir, $where) {
		
	  // set the directory with plot images
	  $this->plotDir = $pDir;
		
	  $this->log_file = $log_file;
	  $this->log_dir  = $lDir;

	  $this->where = $where;

	  // parse xml file
	  $this->xml_parser =& new dcube_xml_parser( $xml_file );
	  $this->xml_parser->parse();
	  $this->xml =& $this->xml_parser->data;

	  // take values for prob limits
	  $this->pwarn = $this->xml["dcube"][0]["plimit"][0]["attr"]["warn"];
	  // set to default if absent
	  if ( $this->pwarn == 0) $this->pwarn = 0.95;
	  $this->pfail = $this->xml["dcube"][0]["plimit"][0]["attr"]["fail"];
	  // set to deafult if absent
	  if ( $this->pfail == 0) $this->pfail = 0.75;
	  
	  if ( isset($_COOKIE['DCubeServerPlotsPerPage'] ) ) {
	    $this->plotsPerPage = $_COOKIE['DCubeServerPlotsPerPage'];
	  } else {
	    $this->plotsPerPage = 20;
	  }

	  if ( isset($_COOKIE['DCubeServerFirstPane'] ) ) {
	    $this->firstPane = $_COOKIE['DCubeServerFirstPane'];
	  } else {
	    $this->firstPane= "1";
	  }

	  echo "<body onload='jsfix();'>\n";
	  echo "<div class='wrapper'>\n";		
	  echo "<div class='header'><b>".$version."</b>".$project." by ".
	    "<a href='mailto:Krzysztof.Ciba@NOSPAMagh.edu.pl?subject=dcube'>".$author."</a></div>\n";
	 

	  echo "<div class='container'>\n";
	  
	  // configuration panel
	  echo "<div class='mid_panel'>";
		$this->configurationPanel();
		echo "</div>\n";
	  
	  // static legend display
	  echo "<div class='legend' title='Show/hide legend window'>";
		$this->staticLegend();
		echo "</div>";
	  
	  // main panel content
	  echo "<div class='content'>\n";
	  $this->mainPanel();	  
	  echo "</div>\n";
	  
	  // end of container 
	  echo "</div>\n";			
	  
		// footer content
	  echo "<div class='footer'>";
		$this->footer();
		echo "</div>\n";
	  
	  // end of wrapper 
	  echo "</div>\n";
	  
	  // display statistics tests legend
	  $this->testLegend();
	  
	  /// moving out of body  
	  echo "<script type='text/javascript'>\n";
	  echo "<!--\n";
	  echo "function jsfix() {\n";
	  echo $this->script;
	  echo "document.getElementById('loading').style.display = 'none';\n";
	  echo "}\n";
	  // set the where variable
	  echo "var where ='".$this->where."';\n";
	  echo "var loading = new Image();\n";
	  echo "loading.src =  where + './images/loading_plot.gif';\n";
	  echo "var selBkg = 'url('+where+'/../images/sel_tabpane_bkg.gif)';\n";
	  echo "var selLeft = 'url('+where+'/../images/sel_tabpane_left.gif)';\n";
	  echo "var selRight = 'url('+where+'/../images/sel_tabpane_right.gif)';\n"; 
	  echo "var pBkg = 'url('+where+'/../images/tabpane_bkg.gif)';\n";
	  echo "var pLeft = 'url('+where+'/../images/tabpane_left.gif)';\n";
	  echo "var pRight = 'url('+where+'/../images/tabpane_right.gif)';\n";
	  echo "var picTable = new Array();\n";
	  echo "var picInfoTable = new Array();\n";
	  echo $this->picTableJS;
	  echo $this->picInfoTableJS;
	  echo "gGetBeg();\n";
	  echo "selPane('".$this->firstPane."');\n";
	  // allow for picture to be selected by url
	  if (isset($_GET["rid"])) {
	    $rid=$_GET["rid"];
	    echo "selPic('".$rid."');\n";
	  }
	  echo "-->\n";
	  echo "</script>\n";
	  echo "</body>\n";
	}
	

	function staticLegend() {
	 
	  echo "<fieldset onclick='toggleLegend();'>\n";
	  echo "<legend>Legend</legend>\n";
	  echo "<table class='pval_table' title='Click here to show/hide legend'>";
	  echo "<tr>\n";
	  echo "<th>stats tests</th>\n";
	  echo "<td class='ok' style='height: 32px;'>OK&nbsp;(p-value&nbsp;&gt;".$this->pwarn.")</td>";
	  echo "<td class='warn' style='height: 32px;'>WARN&nbsp;(".$this->pfail;
	  echo "&lt;p-value&lt;".$this->pwarn.")</td>\n";
	  echo "<td class='fail' style='height: 32px;'>FAIL&nbsp;(p-value&lt;".$this->pfail.")</td>\n";
	  echo "<td class='absent' style='height: 32px;'>&nbsp;&nbsp;ABSENT&nbsp;(no test)</td>\n";
	  echo "</tr>\n";
	  echo "<tr>\n";
	  echo "<td class='absent'>DCube status</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/green.gif' alt='Green'> OK</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/yellow.gif' alt='Yellow'> WARN</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/red.gif' alt='Red'> FAIL</td>";
		echo "<td class='absent'> ABSENT (no status)</td>";
	  echo "</tr></table>\n";
	  echo "</fieldset>\n";
	  
	}
	
	/**
	 * footer - Valid CSS, Valid HTML icons + anchors to documentation, download etc. 
	 */
	function footer() {
	  
	  $url = "http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Tools/DCubeClient/";
	  echo "<a href='https://twiki.cern.ch/twiki/bin/view/Sandbox/DCubeDoc'>[&nbsp;Documentation&nbsp;]</a>\n";
	  echo "<a href='http://validator.w3.org/check?uri=referer'>";
	  echo "<img src='".$this->where."./images/valid-html401-blue.png' ";
	  echo "alt='Valid HTML 4.01 Strict' style='vertical-align: middle;' " . 
	    "height='31' width='88'></a>\n";
	  echo "<a href='http://jigsaw.w3.org/css-validator/check/referer'>";
	  echo "<img src='".$this->where."./images/valid-css-blue.png' ";
	  echo "alt='Valid CSS!' style='vertical-align: middle;'></a>\n";
	  echo "<a href='http://www.totalvalidator.com/validator/Revalidate?revalidate=true'>".
	    "<img src='".$this->where."/images/valid_n_html_401.gif' alt='Totally Valid HTML 4.01!' ".
	    "style='vertical-align: middle;'></a>\n";
	}
	

	
	function relpath ( $dest ) {
	  $here = explode ("/", realpath(".") );
	  $to = explode ("/", realpath($dest) );
	  $result = "."; 
	  while ( implode ("/", $to) != implode ("/", $here) ) {
	    if (count($here)>count($to)) {
	      array_pop($here);
	      $result .= "/..";
	    } else {
	      array_pop($to);
	    }
	  }
	  return str_replace("//", 
			     "/", 
			     $result . str_replace( implode ("/", $to), 
						    "", 
						    realpath($dest)).(@is_dir(realpath($dest)) ? "/" : "" ) );
	}

	/**
	 * @brief displays monitored and reference filenames, descriptions, time stamp etc.
	 */
	function configurationPanel() {
	  
	  $mon   = $this->xml["dcube"][0]["monitored"][0]["attr"]["file"];
	  $ref   = $this->xml["dcube"][0]["reference"][0]["attr"]["file"];
	  $test_desc  = $this->xml["dcube"][0]["test_desc"][0]["cdata"];
	  $ref_desc = $this->xml["dcube"][0]["ref_desc"][0]["cdata"];
	  $branch = $this->xml["dcube"][0]["attr"]["branch"];
	  $install = $this->xml["dcube"][0]["attr"]["install"];
	  $cmtconfig = $this->xml["dcube"][0]["attr"]["cmtconfig"];
	  $project = $this->xml["dcube"][0]["attr"]["project"];
	  $jobId = $this->xml["dcube"][0]["attr"]["jobid"];
	  $date = $this->xml["dcube"][0]["date"][0]["cdata"];
	  
	  
	  if ( file_exists( $mon ) ) {
	    $mon = "<a href='".$this->relpath( realpath($mon) )."'>".$mon."</a>";
	  }
	  
	  if ( file_exists( $ref ) ) {
	    $ref = "<a href='".$this->relpath( realpath($ref) )."'>".$ref."</a>";
	  }
	  if ( $test_desc == "PUT YOUR TEST DESCRIPTION HERE" ) 
	    $test_desc = "&mdash;";
	  if ( $ref_desc == "PUT YOUR REFERENCE DESCRIPTION HERE" ) 
	    $ref_desc = "&mdash;";
	  
	  
	  echo "Test desc.:&nbsp;<span class='bold'>".$test_desc."</span><br>\n";
	  echo "Date:&nbsp;<span class='bold'>".$date."</span><br>\n";
	  echo "BRANCH:&nbsp;<span class='bold'>".$branch."</span>&nbsp;";
	  echo "INSTALL:&nbsp;<span class='bold'>".$install."</span>&nbsp;\n";
	  echo "CMTCONFIG:&nbsp;<span class='bold'>".$cmtconfig."</span>&nbsp;\n";
	  echo "PROJECT:&nbsp;<span class='bold'>".$project."</span>&nbsp;\n";
	  echo "JOB ID:&nbsp;<span class='bold'>".$jobId."</span><br>\n";
	  echo "Monitored root file:&nbsp;".$mon."<br>\n";
	  echo "Reference root file:&nbsp;".$ref."<br>\n";
	  echo "Reference file descr.:&nbsp;<span class='bold'>".$ref_desc."</span><br>\n";
	  
	  
	  if ( $this->log_file != "" ) {
	    echo "Log file:&nbsp;<a href=\"".$this->log_file."\">[ as plain text ]</a>";
	    echo "&nbsp;<a href=\"./dcubelog.php\">[ as HTML ]</a><br>";
	  } else {
	    echo "Log file:<i>Log file not available.</i><br>\n";
	  }
	  
	}
	
	/**
	 * @brief produces histograms/directories DIV
	 */
	function mainPanel( ) {
		
	  echo "<br>";
	
	  // tabpanel bar
	  echo "<div class='tabbar'>";

	  // 1st button
	  echo "<div id='t1' class='tabpane' onClick='selPane(\"1\");'>";
	  echo "<div class='sel_tp_left'></div>";
	  echo "<div class='sel_tp_title'>Normal view</div>";
	  echo "<div class='sel_tp_right'></div>";
	  echo "</div>";
	  
	  // 2nd button
	  echo "<div id='t2' class='tabpane' onClick='selPane(\"2\");'>";
	  echo "<div class='tp_left'></div>";
	  echo "<div class='tp_title'>Plots View</div>";
	  echo "<div class='tp_right'></div>";
	  echo "</div>";

	  // 3rd button
	  echo "<div id='t3' class='tabpane' onClick='selPane(\"3\");'>";
	  echo "<div class='tp_left'></div>";
	  echo "<div class='tp_title'>Summary</div>";
	  echo "<div class='tp_right'></div>";
	  echo "</div>";
	  
	  // 4th button
	  echo "<div id='t4' class='tabpane' onClick='selPane(\"4\");'>";
	  echo "<div class='tp_left'></div>";
	  echo "<div class='tp_title'>Settings</div>";
	  echo "<div class='tp_right'></div>";
	  echo "</div>";

	  // end of tabpanel
	  echo "</div>";

	  // content panel
	  echo "<div class='contentPane' >\n"; 

	  // normal, tree-like view
	  echo "<div id='c1' class='tp'>";
	  
	  // header row
	  echo "<div class='tdir'>";
	  echo "<div class='tdir0'><div class='absent'>name</div></div>";
	  echo "<div class='tdir1'><div class='absent'>K-S test</div></div>";
	  echo "<div class='tdir1'><div class='absent'>&chi;&sup2; test</div></div>";
	  echo "<div class='tdir1'><div class='absent'>\"bin-by-bin\" test</div></div>";
	  echo "<div class='tdir1'><div class='absent'>DCube status</div></div>";
	  echo "</div>\n";

	  // directory tree
	  $rootDir = $this->xml["dcube"][0]["tdirectory"][0];
	  if ( is_array($rootDir) ) {
	    $this->id = 0;
	    $this->treeView($rootDir, 0);  
	  } else {
	    echo "<span style='color: red;'> root (/) TDirectory not found in output xml file!</span><br>";
	  }
	  
	  echo "</div>\n"; 	
	  
	  // plots view
	  echo "<div id='c2' class='tp' style='display: none; text-align: left;'>\n";
	  $this->plotView($rootDir, "/", "prow_1_1");
	  echo "</div>\n"; 

	  // summary view
	  echo "<div id='c3' class='tp' style='display: none;'>";
	  $this->summaryViewDIV();
	  echo "</div>\n"; 

	  // settings
	  echo "<div id='c4' class='tp' style='display: none;'>";
	  $this->settingsView();
	  echo "</div>\n"; 

	  echo "</div>";
	  // end of content panel

	  echo "<br>";
	 
	}


	function settingsView() {
	  echo "<fieldset>";	 
	  echo "<legend>&nbsp;Display&nbsp;</legend>";
	  echo "<div class='setup'> When loading DCube page show ";

	  $arr = array( 1 => "Normal View",
			2 => "Plots View",
			3 => "Summary",
			4 => "Settings" );
		
	  echo "<select id='gSelectPane' onChange='createCookie( \"DCubeServerFirstPane\", this.value, 3650 );'>";
	  
	  foreach ( $arr as $key => $value ) {
	    echo "<option value='".$key."'";
	    if ( $this->firstPane == $key ) echo " selected='selected'";
	    echo ">".$value."</option>";
	  } 

	  echo "</select> tabpane first. </div>";
	  echo "</fieldset>";

	  echo "<fieldset>"; 
	  echo "<legend>&nbsp;Plots view&nbsp;</legend>";
	  echo "<div class='setup'> Show ";
	  echo "<select id='gSelectPlotsPerPage' onChange='setupGallery( this.value );'>";

	  $arr = array(5, 10, 20, 50, 500);
	  foreach ( $arr as $value ) {
	    echo "<option name='".$value."' value='".$value."'";
	    if ( $value == $this->plotsPerPage ) echo " selected='selected'";
	    echo ">".$value."</option>";       
	  } 
	  echo "</select> plots per page <i>(triggers page reloading)</i></div>";
	  echo "</fieldset>";
	}

	/**
	 * @brief displays p-value test legend (transparend box in upper-right corner of the page)
	 */
	function testLegend( ) {

	  echo  "<div title='Click here to hide legend' id='pval_legend' " . 
	    "class='pval_legend' onclick='toggleLegend();' style='display: none; background: #ffffff; z-index: 10000;'>";
	  echo "<table class='pval_table' title='Click here to hide legend'>";
	  echo "<tr>\n";
	  echo "<th>stats tests</th>\n";
	  echo "<td class='ok' style='height: 32px;'>OK&nbsp;(p-value&nbsp;&gt;".$this->pwarn.")</td>";
	  echo "<td class='warn' style='height: 32px;'>WARN&nbsp;(".$this->pfail;
	  echo "&lt;p-value&lt;".$this->pwarn.")</td>\n";
	  echo "<td class='fail' style='height: 32px;'>FAIL&nbsp;(p-value&lt;".$this->pfail.")</td>\n";
	  echo "<td class='absent' style='height: 32px;'>&nbsp;&nbsp;ABSENT&nbsp;(no test)</td>\n";
	  echo "</tr>\n";
	  echo "<tr>\n";
	  echo "<td class='absent'>DCube status</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/green.gif' alt='Green'> OK</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/yellow.gif' alt='Yellow'> WARN</td>";
	  echo "<td class='plot'><img src='".$this->where."/images/red.gif' alt='Red'> FAIL</td>";
	  echo "<td class='absent'> ABSENT (no status)</td>";
	  echo "</tr></table>\n";    
	  echo "</div>\n";
	  
	}

	/**
	 * @brief Summary View content
	 */
	function summaryViewDIV() {

		$summaryNode = $this->xml["dcube"][0]["summary"][0];

		$status = $summaryNode["attr"]["status"];
		if ( $status == "OK" ) 
			$status = "<img src='".$this->where."./images/green.gif' alt='Green'>";
		if ( $status == "WARN" ) 
			$status = "<img src='".$this->where."./images/yellow.gif' alt='Yellow'>";
		if ( $status == "FAIL" ) 
			$status = "<img src='".$this->where."./images/red.gif' alt='Red'>";

	
		echo "<div class='si' style='text-align: center;'>";
		echo " Overall status:&nbsp;".$status."&nbsp;".$summaryNode["attr"]["status"]."<br>";
		echo " Objects processed:&nbsp;".$summaryNode["attr"]["objs"];
		echo "</div><br>";

		echo "<div style='text-align: center;'>";
		echo "<table class='sumTable'>";

		$nodeKS = $summaryNode["table"][0]["tr"][1]["td"];
		$rowKS = "<td>".($nodeKS[1]["cdata"]+0)."</td>".
		  "<td>".($nodeKS[2]["cdata"]+0)."</td>".
		  "<td>".($nodeKS[3]["cdata"]+0)."</td></tr>";

		$nodeChi2 = $summaryNode["table"][0]["tr"][2]["td"];
		$rowChi2 = "<td>".($nodeChi2[1]["cdata"]+0)."</td>".
		  "<td>".($nodeChi2[2]["cdata"]+0)."</td>".
		  "<td>".($nodeChi2[3]["cdata"]+0)."</td></tr>";
		
		$nodeBBB = $summaryNode["table"][0]["tr"][3]["td"];
		$rowBBB = "<td>".($nodeBBB[1]["cdata"]+0)."</td>".
		  "<td>".($nodeBBB[2]["cdata"]+0)."</td>".
		  "<td>".($nodeBBB[3]["cdata"]+0)."</td></tr>";

		$nodeMeanY = $summaryNode["table"][0]["tr"][4]["td"];
		$rowMeanY = "<td>".($nodeMeanY[1]["cdata"]+0)."</td>".
		  "<td>".($nodeMeanY[2]["cdata"]+0)."</td>".
		  "<td>".($nodeMeanY[3]["cdata"]+0)."</td></tr>";
		
		$nodeSUM = $summaryNode["table"][0]["tr"][5]["td"];
		$rowSUM = "<td>".($nodeSUM[1]["cdata"]+0)."</td>".
		  "<td>".($nodeSUM[2]["cdata"]+0)."</td>".
		  "<td>".($nodeSUM[3]["cdata"]+0)."</td></tr>";
		
		$nodeFRAC = $summaryNode["table"][0]["tr"][6]["td"];
		$rowFRAC = "<td>".($nodeFRAC[1]["cdata"]+0)."%</td>".
		  "<td>".($nodeFRAC[2]["cdata"]+0)."%</td>".
		  "<td>".($nodeFRAC[3]["cdata"]+0)."%</td></tr>";
		
		echo "<tr><th>test</th><td class='ok'>OK</td>".
		  "<td class='warn'>WARN</td>".
		  "<td class='fail'>FAIL</td></tr>";
		
		echo "<tr><th>K-S</th>".$rowKS;
		echo "<tr><th>&chi;&sup2;</th>".$rowChi2;
		echo "<tr><th>\"bin-by-bin\"</th>".$rowBBB;
		echo "<tr><th>\"mean y\"</th>".$rowMeanY;
		echo "<tr><th>sum</th>".$rowSUM;
		echo "<tr><th>fractions</th>".$rowFRAC;
		echo "</table>";
		echo "</div><br>";

		$nbErrors = $summaryNode["attr"]["errors"];
		
		echo "<div class='si'>";
		echo "&nbsp;Errors:&nbsp;".($nbErrors+0);
		
	
		$errorsNode = $summaryNode["errors"][0]["error"];

		if ( is_array( $errorsNode ) ) {
		  echo "<ul>";
		  foreach  ( $errorsNode as $key => $error ) {
		    $what = $error["attr"]["what"];
		    $times = $error["attr"]["times"];
		    echo "<li>" . $what ." - occured ". $times . " time(s)"; 
		  } 
		  echo "</ul>";
		}
	
		echo "</div>";
	}
	
	
	
	// tree-like view
	function treeView( $dirNode, $level=0 ) {

		$dirName = $dirNode["attr"]["name"];
		$displayName = $this->displayName( $dirName, $level );

		$tab = str_repeat("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", $level);
		$this->id++;

		// set the counters for stat test 
		$dirNode["KS"]   = array( "OK" => 0, "WARN" => 0, "FAIL" => 0 );
		$dirNode["chi2"] = array( "OK" => 0, "WARN" => 0, "FAIL" => 0 );
		$dirNode["bbb"]  = array( "OK" => 0, "WARN" => 0, "FAIL" => 0 );

		echo "<div id='d_".($this->id+0)."' class='dir' ";
		if ( $level <= 1 ) 
			echo "style='display: block' ";
		else 
			echo "style='display: auto' ";
		echo 	" title='".$this->$dirName."' onmouseover=\"statusBar('TDirectory ".$dirName."');\"".
									"onclick=\"showhide('dc_".($this->id+0)."', 'di_".($this->id+0)."');\">";

		echo "<div class='dir0'>".$tab;
		if ( $dirName != "/" ) {
			echo "<img id='di_".($this->id+0)."' src='".$this->where."./images/tdirectory_c.gif' alt='folder icon'>&nbsp;";
		} else {
			echo "<img id='di_".($this->id+0)."' src='".$this->where."./images/tdirectory_o.gif' alt='folder icon'>&nbsp;";
		}
		echo $displayName;							
		echo "</div>";
		
		// summary tables 
		echo "<div class='dir1' id='KS_".($this->id+0)."'>";
		$this->sumDirTABLE( $dirNode, "ks");
		echo "</div>";
		echo "<div class='dir1' id='chi2_".($this->id+0)."'>";
		$this->sumDirTABLE( $dirNode, "chi2");
		echo "</div>";
		echo "<div class='dir1' id='bbb_".($this->id+0)."'>";
		$this->sumDirTABLE( $dirNode, "bbb");
		echo "</div>";
		
		// DCube processing status
		$status = $dirNode["attr"]["status"];
		echo "<div class='dir1'><img src='";
		if ( strstr($status, "OK") ) {
			echo $this->where."./images/green.gif' alt='Green' id='dqb_".($this->id+0)."'>";
		} else if ( strstr($status, "WARN") ) {
			echo $this->where."./images/yellow.gif' alt='yellow' id='dqb_".($this->id+0)."'>";
		} else {
			echo $this->where."./images/red.gif' alt='red' id='dqb_".($this->id+0)."'>";
		}
		echo "</div>";

		echo "</div>\n";
		
		
		if ( $level < 1 ) {
			echo "<div id='dc_".($this->id+0)."' class='dirc' style='display: auto;'>";
		} else {
			echo "<div id='dc_".($this->id+0)."' class='dirc' style='display: none;'>";
		}

		$subDirs = $dirNode["tdirectory"];
		if ( is_array($subDirs) ) {
			foreach ( $subDirs as $nb => $subdir ) {
				$this->treeView( $subdir, $level+1 ); 
			}
		}
			
		$hists = $dirNode["plot"];
		if ( is_array( $hists ) ) {
			foreach ( $hists as $hist ) {
				$this->histDiv( $hist, $level+1, $dirNode);
			}
		}

		echo "</div>\n";
	}

	function sumDirTABLE( $dirNode, $test ) {

		echo "<table class='st'>";
	  echo "<tr>";

		$testOK = $dirNode["attr"][$test . "ok"];
	  if ( $testOK != 0 ) { 
			echo "<td class='ok'>" . $testOK . "</td>";
		} else 
			echo "<td class='eok'>&mdash;</td>";
		
		$testWARN = $dirNode["attr"][$test . "warn"];
		if ( $testWARN != 0 ) { 
			echo "<td class='warn'>" .$testWARN . "</td>";
		} else 
			echo "<td class='ewarn'>&mdash;</td>";
	    
		$testFAIL = $dirNode["attr"][$test . "fail"];
		if ( $testFAIL != 0 ) { 
			echo "<td class='fail'>" . $testFAIL . "</td>";
		} else 
			echo "<td class='efail'>&mdash;</td>";
		
	  echo "</tr></table>";
	} 

	
	function histDiv( $hist, $level, $parDir) {

		$this->id++;
		$tab = str_repeat("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", $level);
		
		$histName = $hist["attr"]["name"];
		$displayName = $this->displayName( $histName, $level );
		$type = $hist["attr"]["type"];


		// DCube status 
		$tests = $hist["attr"]["stest"];
		$plots = $hist["attr"]["plots"];
		$stat  = $hist["attr"]["status"];

		$regSrc = $hist["img"][0]["attr"]["src"];

		$picInfo = "";
		if ( strstr($stat, "FAIL") ) {
			$status = explode(";", $stat );
			$picInfo .= "STATUS: ". $status[0] . " &mdash; " . $status[1];
		} else {
			$picInfo .= "TESTS: " . $tests;
		}
		
		$this->picTableJS .= "picTable[".($this->picNum+0)."]='".$regSrc."';\n";	
		$this->picInfoTableJS .= "picInfoTable[".($this->picNum+0)."]='<span>&#35;".($this->picNum+1)." ".$picInfo."</span>';\n";
		$this->picNum++;

		$showhide = " showhide('hc_".($this->id+0)."', null, 'plot_".$this->id."', '".$regSrc."');"; 
		
		// allow for picture to be selected by url
		$rid=("r_".($this->id));

		if ( strstr($type, "TH1") ) $icon = "h1";
		if ( strstr($type, "TH2") ) $icon = "h2";
		if ( strstr($type, "TGraph") || strstr($type, "TEfficiency") ) $icon = "graph";
		if ( strstr($type, "TProfile") ) $icon = "profile";

		if ( $level == 1 ) {
			echo "<div id='".$rid."' class='his' style='display: block;' title='".$histName."' ".
				"onmouseover=\"statusBar('".$type." ".$histName."');\" onclick=\"".$showhide."\">";
		}  else {
			echo "<div id='".$rid."' class='his' style='display: auto;' title='".$histName."' ".
				"onmouseover=\"statusBar('".$type." ".$histName."');\" onclick=\"".$showhide."\">";
		}
		
		echo "<div class='his0'>".$tab.
			"<img src='".$this->where."./images/".$icon.".gif' alt='icon'>&nbsp;".$displayName;
		echo "</div>";

				
		$pvalNode = $hist["stat"][0]["pvalue"];

		$test0 = $pvalNode[0]["attr"]["test"];
		$test1 = $pvalNode[1]["attr"]["test"];
		$test2 = $pvalNode[2]["attr"]["test"];
			
		$pval0 = $pvalNode[0]["cdata"];
		$pval1 = $pvalNode[1]["cdata"];
		$pval2 = $pvalNode[2]["cdata"];
		
		$stat0 = $pvalNode[0]["attr"]["status"];
		$stat1 = $pvalNode[1]["attr"]["status"];
		$stat2 = $pvalNode[2]["attr"]["status"];
		
		$pvals = array( $test0 => $pval0, $test1 => $pval1, $test2 => $pval2 );
		$stats = array( $test0 => $stat0, $test1 => $stat1, $test2 => $stat2 );
		
		if ( strstr($type, "TProfile") )  {
		  
		  echo "<div class='his1' style='width: 48.5%;'>";
		  $pval = "<span style='font-size: 8pt;'>|&mu;<sup>eff</sup>(mon) - &mu;<sup>eff</sup>(ref)| = ".$pvals["meanY"]."</span>";
		  if ( $stats["meanY"] == "OK" ) { 
		    echo "<div class='ok' style='margin: auto auto;'>".$pval."</div>";
		  } else if ( $stats["meanY"] == "WARN" ) {
		    echo "<div class='warn' style='margin: auto auto;'>".$pval."</div>";
		  } else if ( $stats["meanY"] == "FAIL" ) {
		    echo "<div class='fail' style='margin: auto auto;'>".$pval."</div>";
		  } else {
		    echo "<div class='absent' style='margin: auto auto;'>&mdash;</div>";
		  } 
		  echo "</div>";
		 
		} else {
		  echo $this->pvalueDIV( $pvals["KS"], $stats["KS"] );
		  echo $this->pvalueDIV( $pvals["chi2"], $stats["chi2"] );
		  echo $this->pvalueDIV( $pvals["bbb"], $stats["bbb"] );
		}
		
	

		// DCube status 
		$tests = $hist["attr"]["stest"];
		$plots = $hist["attr"]["plots"];
		$stat = $hist["attr"]["status"];

		$status = $stat . $tests . $plots;

		echo "<div title='DCube status' class='his1'>";
		if ( strstr($status, "FAIL") ) {
			echo "<img src='".$this->where."./images/red.gif' alt='Red'>";
		} else if ( strstr($status, "WARN") ) {
			echo "<img src='".$this->where."./images/yellow.gif' alt='Yellow'>";
		} else {
			echo "<img src='".$this->where."./images/green.gif' alt='Green'>";
		}
		echo "</div>";
		echo "</div>";

		$this->histContentsDIV( $hist, $this->id );
	}


	/**
	 * @brief display DIV with histogram/plot or progfile contents
	 */
	function histContentsDIV( $histNode, $id ) {
		
		$type = $histNode["attr"]["type"];
		$name = $histNode["attr"]["name"];
		$status = $histNode["attr"]["status"];

		echo "\n<div id='hc_".$id."' class='hc' style='display: none;'>";
		echo "<span style='font-size: 14pt;'>Name: <tt>".$name."</tt>&nbsp;&nbsp;Type: <tt>".$type."</tt></span><br><br>";

		if ( strlen($status) ) {
			$status = explode(";", $status);
			echo "<span style='color: red'>" . $status[0] . " &mdash; " . $status[1]  . "</span><br>";
			
		} else {
					
			$imgNodes = $histNode["img"];
		
			$this->statbox( $histNode, $type );

			
			echo "<div class='tabbar'>";
			$pid = "plot_" . $this->id;
			
			$reg = "";
			foreach ( $imgNodes as $key => $val ) {
				
				$src = $val["attr"]["src"];
				$type = $val["attr"]["type"];
				if ( $reg == "" ) $reg = $src;
				
				$id = $type . "_" . $this->id; 
				
				echo "<div id='".$id."' class='tabpane' onClick='selPlot(\"".$id."\", \"".$src."\");'>";
				
				if ( strstr($type, "reg") ) {
					echo "<div class='sel_tp_left'></div>";
					echo "<div class='sel_tp_title'>Normal plot</div>";
					echo "<div class='sel_tp_right'></div>";
					echo "</div>";
				} else {
					echo "<div class='tp_left'></div>";
					if ( $type == "log" ) $type = "Log";
					if ( $type == "dif" ) $type = "Diff";
					if ( $type == "prx" ) $type = "Proj X";
					if ( $type == "pry" ) $type = "Proj Y";
					$type .= " plot";
					echo "<div class='tp_title'>".$type."</div>";
					echo "<div class='tp_right'></div>";
					echo "</div>";
				}
			}
			
			echo "</div>";
			
			echo "<div class='contentPane'>\n"; 
			
			echo "<div class='tp'>";
			// provide a link to this picture
			echo "<a href='?rid=r_".$this->id."'><img class='plot' alt='plot' id='".$pid."' src='".$this->where."./images/loading_plot.gif'></a>";
			echo "</div>";		

			echo "</div>";
					
		}
		echo "</div>";
	}
	/**
	 * @param $pval pvalue
	 * @param $status test status
	 */
	function pvalueDIV( $pval, $status ) {
		
		echo "<div class='his1'>";
		
		if ( $status == "OK" ) { 
			echo "<div class='ok'>".$pval."</div>";
		} else if ( $status == "WARN" ) {
			echo "<div class='warn'>".$pval."</div>";
		} else if ( $status == "FAIL" ) {
			echo "<div class='fail'>".$pval."</div>";
		} else {
			echo "<div class='absent'>&mdash;</div>";
		} 
		echo "</div>";
	}

	
	// change the display name if longer than 35 characters
	function displayName( $name, $level ) {
		$where = ($level-1)*5; 

		if ( ( strlen($name) + $where ) > 35 ) {
			
			$where = 35 - $where + ($level-1)*3 -1;
			
			if ( $where + 8 < strlen($name) ) {
					$beg = substr( $name, 0, $where);
					$end = substr( $name, -3);
					$name =  $beg . "(...)" .$end;
			}
		}
		return $name;
	}

	/**
	 * function to display "plot view"
	 */
	function plotView( $object, $dirname , $pid ) {
		
	  $steps = $this->picNum / $this->plotsPerPage;
	  $steps = (int)$steps;
	  $rest =  $this->picNum % $this->plotsPerPage;
	  echo "<div class='gal'>";

	  // top bar
	  echo "<div class='gbar' id='gtop'>";
	  echo "<span id='gtt'></span>";
	  echo "<div class='gnav' id='gsbt' onClick='gGetBeg();' title='Begin'><span>&lt;&lt;&lt;</span></div>";
	  echo "<div class='gnav' id='gspt' onClick='gGetPrev();' title='Previous'><span>&lt;</span></div>";
	  echo "<div class='gnav' id='gjt' title='Jump to...'>Jump: ";
	  echo "<select id='gst' onchange='gOnSelect(this.value);'>";
	  if ( $steps > 0 ) {
	    for ($i = 0; $i < $steps; $i++ ) {
	      $val = $i*$this->plotsPerPage;
	      $beg = $val+1;
	      $end = ($i+1)*$this->plotsPerPage;
	      echo "<option value='".$val."'>".$beg." - ".$end."</option>";
	    }
	  }
	  if ( $rest > 0 ) {
	    $val = $steps*$this->plotsPerPage;
	    $beg = $steps*$this->plotsPerPage+1;
	    $end = $beg + $rest-1;
	    echo "<option value='".$val."'>".$beg." - ".$end."</option>";	   
	  }
	  echo "</select>";
	  echo "</div>";
	  echo "<div class='gnav' id='gsnt' onClick='gGetNext();' title='Next'><span>&gt;</span></div>";
	  echo "<div class='gnav' id='gset' onClick='gGetEnd();' title='End'><span>&gt;&gt;&gt;</span></div>";
	  echo "</div>";

	  // galery content
	  echo "<div class='gc'>";
      
	  for ($i = 0; $i < $this->plotsPerPage; $i++ ) {
	    echo "\n<div class='gp' id='gp_".($i+0)."'><div class='gs' id='gs_".($i+0)."'></div>";
	    echo "<img class='gi' id='gi_".($i+0)."' src='' alt='plot'></div>";
	  }
	  echo "</div>\n";

	  // bottom bar
	  echo "<div class='gbar' id='gbottom'>";
	  echo "<span id='gbt'></span>";
	  echo "<div class='gnav' id='gsbb' onClick='gGetBeg();' title='Begin'><span>&lt;&lt;&lt;</span></div>";
	  echo "<div class='gnav' id='gspb' onClick='gGetPrev();' title='Previuos'><span>&lt;</span></div>";
	  echo "<div class='gnav' id='gjb' title='Jump to...'>Jump: ";
	  echo "<select id='gsb' onchange='gOnSelect(this.value);'>";
	  if ( $steps > 0 ) {
	    for ($i = 0; $i < $steps; $i++ ) {
	      $val = $i*$this->plotsPerPage;
	      $beg = $val+1;
	      $end = ($i+1)*$this->plotsPerPage;
	      echo "<option value='".$val."'>".$beg." - ".$end."</option>";
	    }
	  }
	  if ( $rest > 0 ) {
	    $val = $steps*$this->plotsPerPage;
	    $beg = $steps*$this->plotsPerPage+1;
	    $end = $beg + $rest-1;
	    echo "<option value='".$val."'>".$beg." - ".$end."</option>";	   
	  }

	  echo "</select>";
	  echo "</div>\n";
	  echo "<div class='gnav' id='gsnb' onClick='gGetNext();' title='Next'><span>&gt;</span></div>";
	  echo "<div class='gnav' id='gseb' onClick='gGetEnd();' title='End'><span>&gt;&gt;&gt;</span></div>";
	  echo "</div>";

	  // end of gallery
	  echo "</div>\n";
	}


	/**
	 * @brief produces basic statistic div
	 * @param object XML node
	 * @param type root type
	 */
	function statbox( $object, $type ) {

	  $obj = $object["stat"][0];

		if ( strstr($type, "TGraph" ) || strstr($type, "TEfficiency" ) ) {

			$p = $obj["points"][0]["cdata"];
			$rp = $obj["points"][0]["attr"]["ref"];

			$m = $obj["mean"][0]["cdata"];
			$rm = $obj["mean"][0]["attr"]["ref"];
			
			$r = $obj["rms"][0]["cdata"];
			$rr = $obj["rms"][0]["attr"]["ref"];

			echo "<table class='center' style='width: 60%; font-size: 10pt;'>";
			
			echo "<tr><th></th><th>monitored</th><th>reference</th></tr>";
	 
			echo "<tr><td class='bl'>Points</td><td class='bc'>".($p+0)."</td>";
			echo "<td class='bc'>";
			if ( $rp != "" ) {
				echo ($rp+0);
			} else {
				echo "&mdash;";
			}
			echo "</td></tr>";

			echo "<tr><td class='yl'>Mean</td><td class='yc'>".($m+0)."</td>";
			echo "<td class='yc'>";
			if ( $rm != "" ) {
				echo ($rm+0);
			} else {
				echo "&mdash;";
			}
			echo "</td></tr>";

			echo "<tr><td class='bl'>RMS</td><td class='bc'>".($r+0)."</td>";
			echo "<td class='bc'>";
			if ( $rr != "" ) {
				echo ($rr+0);
			} else {
				echo "&mdash;";
			}
			echo "</td></tr>";
		
			echo "</table>\n";

		} else {

			$e  = $obj["entries"][0]["cdata"] + 0;
			$re = $obj["entries"][0]["attr"]["ref"] + 0;
			
			$xbin = $obj["dim"][0]["attr"]["bins"];
			$ybin = $obj["dim"][1]["attr"]["bins"];
			
			$xu = $obj["dim"][0]["underflow"][0]["cdata"] + 0;
			$xo = $obj["dim"][0]["overflow"][0]["cdata"] + 0;
			
			$yu = $obj["dim"][1]["underflow"][0]["cdata"] + 0;
			$yo = $obj["dim"][1]["overflow"][0]["cdata"] + 0;
			
			$rxu = $obj["dim"][0]["underflow"][0]["attr"]["ref"] + 0;
			$rxo = $obj["dim"][0]["overflow"][0]["attr"]["ref"] + 0;
			
			$ryu = $obj["dim"][1]["underflow"][0]["attr"]["ref"] + 0;
			$ryo = $obj["dim"][1]["overflow"][0]["attr"]["ref"] + 0;
			

			$xm = $obj["dim"][0]["mean"][0]["cdata"];
			$xm_unc = $obj["dim"][0]["mean_unc"][0]["cdata"];
			
			$rxm = $obj["dim"][0]["mean"][0]["attr"]["ref"];
			$rxm_unc = $obj["dim"][0]["mean_unc"][0]["attr"]["ref"];
			
			$ym = $obj["dim"][1]["mean"][0]["cdata"];
			$ym_unc = $obj["dim"][1]["mean_unc"][0]["cdata"];
			
			$rym = $obj["dim"][1]["mean"][0]["attr"]["ref"];
			$rym_unc = $obj["dim"][1]["mean_unc"][0]["attr"]["ref"];
			
		
			$xrms = $obj["dim"][0]["rms"][0]["cdata"];
			$xrms_unc = $obj["dim"][0]["rms_unc"][0]["cdata"];
			
			$rxrms = $obj["dim"][0]["rms"][0]["attr"]["ref"];
			$rxrms_unc = $obj["dim"][0]["rms_unc"][0]["attr"]["ref"];

			$yrms = $obj["dim"][1]["rms"][0]["cdata"];
			$yrms_unc = $obj["dim"][1]["rms_unc"][0]["cdata"];
			
			$ryrms = $obj["dim"][1]["rms"][0]["attr"]["ref"];
			$ryrms_unc = $obj["dim"][1]["rms_unc"][0]["attr"]["ref"];
			
	 
			echo "<table class='center' style='width: 60%; font-size: 10pt;'>";
			
			echo "<tr><th></th><th>monitored</th><th>reference</th></tr>";
	 
	  echo "<tr><td class='bl'>Entries</td><td class='bc'>".($e+0)."</td>";
	  echo "<td class='bc'>";
		if ( $re != "" ) {
	    echo ($re+0);
	  } else {
	    echo "&mdash;";
	  }
	  echo "</td></tr>";
	  
	  echo "<tr><td class='yl'>Underflows</td><td class='yc'>".($xu+$yu)."</td>";
		echo "<td class='yc'>";
	  if ( $rxu.$ryu != "" ) {
	    echo ($rxu+$ryu);
	  } else {
	    echo "&mdash;";
	  }
	  echo "</td></tr>";

	  echo "<tr><td class='bl'>Overflows</td><td class='bc'>".($xo+$yo)."</td>";
	  echo "<td class='bc'>";
		if ( $rxo.$ryo != "" ) {
	    echo ($rxo+$ryo);
	  } else {
	    echo "&mdash;";
	  }
	  echo "</td></tr>";
	  
	  // x 
		echo "<tr><td class='yl'>Bins(x)</td><td class='yc' colspan='2'>".$xbin."</td></tr>";

	  echo "<tr><td class='bl'>Mean(x)</td><td class='bc'>".$xm."&plusmn;".$xm_unc."</td>";
	  echo "<td class='bc'>";
		if ( $rxm != "" ) {
	    echo $rxm."&plusmn;".$rxm_unc;
	  } else {
	    echo "&mdash;";
	  }
	  echo "</td></tr>";

	  echo "<tr><td class='yl'>RMS(x)</td><td class='yc'>".$xrms."&plusmn;".$xrms_unc."</td>";
	  echo "<td class='yc'>";
		if ( $rxrms != "" ) {
	    echo $rxrms."&plusmn;".$rxrms_unc;
	  } else {
	    echo "&mdash;";
	  }
	  echo "</td></tr>";

	  
	  if ( strstr($type,"2") || strstr($type, "TProfile" ) ) {
			
	     // y
	    echo "<tr><td class='bl'>Bins(y)</td><td class='bc' colspan='2'>".$ybin."</td></tr>";

	    echo "<tr><td class='yl'>Mean(y)</td><td class=\"yc\">".$ym."&plusmn;".$ym_unc."</td>";
			echo "<td class='yc'>";
			if ( $rym != "" ) {
	      echo $rym."&plusmn;".$rym_unc;
	    } else {
	      echo "&mdash;";
	    }
	    echo "</td></tr>";
	    
	    echo "<tr><td class='bl'>RMS(y)</td><td class='bc'>".$yrms."&plusmn;".$yrms_unc."</td>";
			echo "<td class='bc'>";
	    if ( $ryrms != "" ) {
	      echo $ryrms."&plusmn;".$ryrms_unc;
	    } else {
	      echo "&mdash;";
	    }
	    echo "</td></tr>";

	  }

	  echo "</table>\n";
	  }
	}

}


/**
 * @class dcube
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @date 30/05/2007
 *
 * @brief main class to build up the HTML output.
 */
class dcube {

  var $project = "DCube";
  var $version = "5.0 ";
  var $author  = "Krzysztof Daniel Ciba";
  var $css     = "css/dcube.css.php";
  var $js      = "dcube.js.php";
  
  
  /**
   * @param $xml_file output XML file pythom python part
   * @param $log_file log file from from python part 
   * @param $whereiam relative path from xml file to dcube.php
   */
  function dcube( $xml_file, $log_file = NULL, $whereiam = "" ) {
    
    echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" " .
      "\"http://www.w3.org/TR/html4/strict.dtd\">\n\n";

    $whereiam = $this->relpath( $whereiam );

    echo "<html>";
    $header = new dcube_header( $this->project, 
				$this->version, 
				$this->author, 
				$this->css,
				$this->js, 
				$whereiam );
    
    $body = new dcube_body( $this->project, 
			    $this->version, 
			    $this->author , 
			    $xml_file, 
			    dirname($xml_file),
			    $log_file,
			    dirname($logfile),
			    $whereiam );
    
    

    echo "</html>";
    
    }


  function relpath ($dest) {
    $Ahere = explode ("/", realpath(".") );
    $Adest = explode ("/", realpath($dest) );
    $result = "."; 
    while (implode ("/", $Adest) != implode ("/", $Ahere) ) {
      if (count($Ahere)>count($Adest)) {
	array_pop($Ahere);
	$result .= "/..";
      }
      else {
	array_pop($Adest);
      }
    }
    return str_replace("//", "/", $result.str_replace(implode ("/", $Adest), "", realpath($dest)).(@is_dir(realpath($dest))?"/":""));
  }
  

}

/* end of PHP part */
?>