<html>

<head>
<title>Inspect CoralDB for PixelDcsConnectivity V3</title>
<STYLE TYPE="TEXT/CSS">
<!--
 .seltable {
	font-size:.8em; font-family: arial,helvetica,sans-serif;
	background-color:white; }
 .rowheader {
	font-size:1em;}
 h1 {font-family: arial,helvetica,sans-serif}
 div.newtag {background-color: white; color: #AA5522;
	font-family: arial,helvetica,sans-serif; font-weight: bold}
 li {margin-right: 100px}
 div.smaller {font-size:.8em;
	margin: 10px}
 a {color: #114444; text-decoration: none;}
 a:hover {
	color: #DD8844; background-color: #444488;}
-->
</STYLE>
<SCRIPT><!--
	/** Toggle visibility of HTML elements with JavaScript.
	 *  Dirk Hoffmann -CPPM-, February 2008 */
	function toggle(idtag) {
		if (document.getElementsByName) {
			elems = document.getElementsByName(idtag)
			for(i=0; i<elems.length; i++) {
				newSrc = "+.gif";
				if (elems[i].src) {
					thisImg = elems[i];
					}
				else {
					thisStyle = elems[i].style;
					if (thisStyle.display == "none") {
						thisStyle.display = ""; 
						newSrc="-.gif";
						}
					else 
						thisStyle.display = "none"; 
					}
				thisImg.src = newSrc;
				}
			}
		else
			alert ("Cannot use Expand/Collapse with this version of JS.");
		return false;
		}
//--></SCRIPT>
</head>

<body bgcolor="#FFFFC0">

<table width="100%">
 <tr>
  <td valign=top>
  <h1 align=center>PixelDcsConnectivity Portal V3</h1>

  This page is used 
  <ol>
   <li>to guide through the use of the 
    <code><a href=DcsConn.php>DcsConn</a></code> application
   for XML setup extraction and 
   <li>perform some basic checks on DB integrity and consistency:<br>
    <a href="CoralDbErr.php">double aliases</a> --
    <a href="pp0-mod.php">PP0 channels for modules</a> --
    <a href="cool_reader.php">COOL reader account Overview</a>
    (list of tables, columns etc.) --
    ...
   </ol>
  There are some cross-references to the CoralDB(2) browser as well.
   </td>
  <td align=center> <img src=PixelConnPortal.jpg width=250px>
   <p/>
<!--
   <div class=newtag align=center>
    <img align=middle src="http://www.buttonpool.de/new_w_005.gif">
    Using CoralDb2
    </div>
  -->
   </td>
  </tr>
 </table>


<p/>

Click on tag name to use it for DcsConn extraction.
<br>
<b>This page is referencing only CoralDb2-ONLINE tables.</b> 
<p/>
Colours are changed back to 
<span style="background-color: #EEDD80">standard</span>, no 
<span style="background-color: #99CC80">green</span> /
<span style="background-color: #FFAA50">red</span>
any more!

<h2>Available (Connectivity) Tags</h2>

<table bgcolor="EEDD80">
<tr>
 <td colspan=2><b>Version Tags / DCS XML</b> (#conn.)</td>
 <td colspan=4><i>further options...</i></td></tr>
<?php

include 'coolr_logon.php';

	$extract = "DcsConn";

	/* Find all valid tags in CoralDB2 */
	$s = OCIParse($c, "SELECT idtag, tag, COUNT(*) n FROM
			atlas_coolonl_pixel.coraldb2_connections
			GROUP BY idtag, tag
			ORDER BY idtag, tag");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		$HTML = $HTML
		.  "<p><b>".font_red("SQL Error: " . $erra['message'])."</b></p>";
		}
	else {
		if (file_exists("$extract+.php"))
			define("DEVEL", null);
		while (OCIFetch($s)) {
			if (!strcmp(ociresult($s, "IDTAG"), $idtag)) 
				print "\t<tr $name><td $name class=rowheader>"; 
			else {
				$idtag = ociresult($s, "IDTAG");
				$name = "name=\"$idtag\"";  
				print "\t<tr><td class=rowheader bgcolor=#FFFFD0>"
					. "<a onClick=\"javascript:return toggle('$idtag');\" "
					. "href=\".\">"
					. "<img $name border=0 align=bottom "
					. "alt=\"-\" src=\"-.gif\">&nbsp;$idtag</a>";
				}
			$tag = $idtag . "::" . ociresult($s, "TAG");
			print "</td>\n";
			print "\t<td $name bgcolor=#FFFFD0 class=rowheader>"
				. "<a href=\"$extract.php?tag=$tag"
				. "\">"
				. ociresult($s, "TAG") . "</a>";
			print " (" . (
					ociresult($s, "N")>1000?  
					(int)(ociresult($s, "N")/1000) . "k":  
					ociresult($s, "N")
					);
			print ")</td>\n";
			print "\t<td $name class=seltable><a href=\"$extract.php?tag=$tag"
				. "&limpcc=1\">1 bs</a></td>\n";
			print "\t<td $name class=seltable><a href=\"$extract.php?tag=$tag"
				. "&stylesheet=dcsconn1.css\">"
				. "nice style</a></td>\n";
			if (defined("DEVEL"))
				print "\t<td $name class=seltable>"
					. "<a href=\"$extract+.php?tag=$tag\">devel*</a> " 
					. "(<a href=\"$extract+.php?tag=$tag&limpcc=1\">1 bs</a>)"
					. "</td>\n";
			print "\t<td $name class=seltable>"
				. "<a href=\"DcsConnAna.php?tag=$tag\">browse</a> ";
			print "\t\t(<a href=\"DcsConnAna.php?tag=$tag&limpcc=1\">"
				. "1bs</a>)</td>\n";
			$tag = ociresult($s, "TAG");
			print "\t<td $name class=seltable><a href=\""
				. "http://atlsr1.cern.ch/web-db/coraldbbrowser.cgi?"
				. "what=PP0S&where=any+name&convention=ID&"
				. "idTag=$idtag"
				. "&connTag=$tag"
				. "&dataTag=$tag"
				. "&aliasTag=$tag"
				. "&dograph=1\">"
				. "PP0s in CoralDb2</a></td>\n";
			print "</tr>\n";
			}
		}

 ?>
	</table>

<p/>
<div class=smaller>
Explanations:
 <b>Default</b> (click on tag name) is XML format output.
 <b>1&nbsp;bs</b> = limit output to one bi-stave/sector (for quick tests).
 <b>nice style</b> = human readable, coloured format of the SIT setup file 
  (XML format enhanced with CSS).
 <b>browse</b> = detailed overview and debugging tool for SIT setup extraction.
 <b>PP0s&nbsp;...</b> = link to CoralDb2 browser for same data.
 
<?php
	OCIFreeStatement($s);

	if (defined("DEVEL"))
		print "<br><i>*Use development extraction code with care!!!</i><p>\n";

	OCILogoff($c);

 ?>

</div>


</body>

<address><hr width=35% align=left>
<i><font size=-1>
$Id: DcsConnInspect.php,v 2.10 2008-02-15 15:00:45 dhoffman Exp $;
<?php
	print "Last modified: " . date("F d Y H:i:s.", getlastmod()) . "\n";
 ?>
</font></i>
</address>
</html>

