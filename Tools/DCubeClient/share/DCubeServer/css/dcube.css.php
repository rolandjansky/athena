<?php
ob_start ("ob_gzhandler");
header("Content-type: text/css");
header("Cache-Control: must-revalidate");
$offset = 60 * 60 * 12;
$ExpStr = "Expires: " . 
gmdate("D, d M Y H:i:s",
time() + $offset) . " GMT";
header($ExpStr);
/** 
 * @file dcube.css 
 * @brief Cascade Style Sheet for DCube
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @date 11/04/2007 
 */
?>
body {font-family: Arial,Halvetica,sans-serif; background-color: #ffffff; font-size: 12pt;}
div {text-align: center;}
#loading {width: 193px; height: 70px;  position: absolute; left: 50%; top: 50%; margin-top: -50px; margin-left: -100px; background-color: #ff6600; border: 5px solid #ffff00; z-index: 1000;}
#load {width: 173px; height: 10px; background-image: url(../images/loading.gif); position: relative; top: 10px; left: 10px;}
div.wrapper {text-align: left; margin: 0px auto; padding: 0px; border: 0; width: 99%;}
div.header {margin: 0 0 12px 0; background-color: #cccccc; text-align: right; font-size: 10pt;}
div.mid_panel {padding: 5px 5px; margin: 18px 0 0 0; background-color: #cccccc; width: auto; text-align: left; overflow: auto;}
div.legend { margin: 18px 0 0 0; background-color: transparent; }
legend {background-color: #a0a0a0; border: 1px dotted #000000;}
fieldset {background-color: transparent; font-weight: bold; border: 1px dotted #000000; text-align: center;}
fieldset.fc {text-align: left;}
div.content {margin: 18px 0 0 0; text-align: left; width: 100%;}
div.footer {clear: both; margin: 18px 0 0 0; text-align: right; background-color: #cccccc;}
div.tp {border: 1px dotted #000000; margin: auto auto; padding: 5px 5px;}
div.his {width: 100%; height: 28px; background-color: #ffffff; cursor: pointer; margin: 1px 0px;}
div.his0 {text-align: left; vertical-align: middle; width: 32%; min-width: 15em; height: 28px; float: left; clear: left; overflow: auto; cursor: pointer; position: relative; top: 4px;}
div.his:hover , div.dir:hover {border: 1px #000000 solid;}
div.his1 {width: 16%; float: left; vertical-align: middle; text-align: center; margin-left: 2px; margin-right: 2px; }
div.ok {width: 97%; margin: 2px 1px; border: 2px dotted #00ff00;}
div.warn {width: 97%;  background-color: #ffd700;  margin: 2px 1px; border: 2px solid #ffd700;}
div.fail {width: 97%;  background-color: #ff4500;  margin: 2px 1px; border: 2px solid #ff4500;}
div.absent {width: 97%;  background-color: #cccccc;  margin: 2px 1px; border: 2px solid #cccccc;}
div.hc { margin: auto auto; width: 99%;  background-color: #cccccc; padding: 5px 5px; border: 1px dotted #000000;}
div.dir {text-align: left; width: 100%; height: 28px; vertical-align: middle; font-weight: bold; background-color: #fff8dc; cursor: pointer; margin: 1px 0px;}
div.dir0 {text-align: left; min-width: 15em; width: 32%;  float: left; clear: left; overflow: auto; cursor: pointer; position: relative; top: 4px;}
div.dir1 {width: 16%; float: left; margin-left: 2px; margin-right: 2px;}
div.dirc {clear: left; width: 100%;}
div.tdir {text-align: left; width: 100%; height: 27px; font-weight: bold; background-color: #cccccc;}
div.tdir0 {min-width: 15em; width: 32%; float: left; overflow: auto;}
div.tdir1 {width: 16%; float: left;  margin-left: 2px; margin-right: 2px; }
div.ch {width: 30%; height: 550px; float: left; position: relative; left: 0px; top: 0px; text-align: left; padding: 10px 10px;}
div.si {margin-left: auto; margin-right: auto; border: 2px solid #cccccc; width: 55%; text-align: left;}
table.pval_table {cursor: pointer; border: dotted 1px; width: 100%; background-color: transparent; text-align: left; empty-cells: hide;}
table.st {width: 100%; border-width: 0px; empty-cells: hide; border-collapse: collapse; position: relative; top: 2px;}
table.center {margin-left: auto; margin-right: auto; border-collapse: collapse;}
td.bc {background-color: #6699cc; text-align: center;}
td.bl {background-color: #6699cc; text-align: left;}
td.yc {background-color: #ffcc00; text-align: center;}
td.yl {background-color: #ffcc00; text-align: left;}
div.pval_legend {position: fixed; bottom: 0em; left: 0em;  padding: 1px; background-color: transparent; width: 100%; display: none; }
a {color: #000099; font-weight: bold; }
a:visited {color: #990000; font-weight: bold;}
td {background-color: #ffffff; width: 14%; text-align: left; cursor: pointer;}
th {font-weight: bold; text-align: center; background-color: #cccccc; width: 14%;}
td.sum {cursor: pointer;}
td.warn {font-weight: bold; background-color: #ffd700; text-align: center;}
td.fail {font-weight: bold; background-color: #ff4500; text-align: center;}
td.ok {font-weight: bold; background-color: #ffffff; border: 2px dotted #00ff00; text-align: center;}
td.eok {margin: 2px 1px; background-color: #ffffff; border: 2px dotted #99ff99; text-align: center;}
td.ewarn {background-color: #ffff99;  text-align: center; }
td.efail {background-color: #ffab99;  text-align: center; }
td.basic {font-family: serif; background-color: #cccccc; font-weight: bold; text-align: center;}
td.absent {font-weight: bold; background-color: #cccccc; text-align: center;}
td.missing {font-weight: bold;	background-color: #00ff00; border: 3px dotted #ff0000; text-align: center;}
td.dir {font-weight: bold; background-color: #fff8dc; text-align: left;}
td.plot {font-weight: bold; background-color: #ffffff; border: 2px dotted #0000ff; text-align: center;}
img { vertical-align: middle; }
img.plot { border: 1px dotted #000000; }
.tabpane {cursor: default; margin: 0px 0px; height: 24px; float: left;}
.tabpane .tp_left {position: relative; top: 0px; left: 0px; background: url(../images/tabpane_left.gif) no-repeat; width: 6px; height: 24px; float: left;}
.tabpane .tp_right {position: relative; top: 0px; right: 0px; background: url(../images/tabpane_right.gif) no-repeat; width: 6px; height: 24px; float:left; }
.tabpane .tp_title {background: url(../images/tabpane_bkg.gif) repeat-x; float: left; height: 24px; padding: 4px 5px; }
.tabpane .sel_tp_left {position: relative; top: 0px; left: 0px; background: url(../images/sel_tabpane_left.gif) no-repeat; width: 6px; height: 24px; float:left;}
.tabpane .sel_tp_right {position: relative; top: 0px; right: 0px; background: url(../images/sel_tabpane_right.gif) no-repeat; width: 6px; height: 24px; float:left;}
.tabpane .sel_tp_title {background: url(../images/sel_tabpane_bkg.gif) repeat-x; float: left; height: 24px; padding: 4px 5px; color: #ffffff;}
.contentPane {clear: left; width: 99%; border: 1px solid black; margin: 0px 0px; padding: 5px 5px;}
.sumTable {margin-left: auto; margin-right: auto; text-align: center; border: 1px solid #000000; width: 55%;}
.sumTable td {text-align: center;}
.gal span {position: relative; top: 4px;}
.gbar {width: 100%; height: 24px; background-color: #cccccc; text-align: center;}
#gtop {position: relative; top: 0px; left: 0px;}
#gbottom {position: relative; bottom: 0px; left: 0px; clear: both;}
.gnav {height: 24px; width: 75px; cursor: pointer;}
div.gnav:hover {background-color: #6d6d6d; text-decoration: underline;}
.gc {width: 100%; clear: both; text-align: center; padding: 5px 5px;}
#gsbt, #gsbb {position: absolute; left: 0px; top: 0px;}
#gspt, #gspb {position: absolute; left: 75px; top: 0px;}
#gsnt, #gsnb {position: absolute; right: 75px; top: 0px;}
#gset, #gseb {position: absolute; right: 0px; top: 0px;}
#gjt, #gjb {position: absolute; top: 0px; left: 150px; width: 230px;}
#gjt select , #gjb select, .setup select {width: 160px; height: 22px; text-align: center; background-color: #cccccc; border: 1px solid #6d6d6d;}
.gp {width: 596px; height: 596px; margin: 1px 1px;  border: 1px dotted #000000; display: none; float: left;}
.gs {text-align: left; text-indent: 20px; width: 100%; height: 24px; position: relative; top: 0px; left: 0px; background-color: #cccccc;}
.gi {width: 596px; height: 572px;}
.setup { font-weight: normal; text-align: left; text-indent: 20px; width: 100%; height: 24px; position: relative; top: 0px; left: 0px; background-color: #cccccc;  }
.bold { font-weight: bold; font-family: Courier; } 