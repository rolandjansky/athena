<html>

<head>
<title>PixelConnectivity: Interlock system in CoralDb</title>
<STYLE TYPE="TEXT/CSS">
<!--
 table {
	margin-left:50px}
 td {
	font-size:.8em; font-family: arial,helvetica,sans-serif;
	background-color:white; }
 th {
	font-family: arial,helvetica,sans-serif;
	background-color:#88CCCC; }
 .rowheader {
	font-size:1em;
	background-color:#CCEEEE; }
 .old {
	font-size:1em;
	background-color:#EE8888; }
 .new {
	font-size:1em;
	background-color:#88EE88; }
 .even {
	background-color:#EEEEEE; }
 h1 {font-family: arial,helvetica,sans-serif}
-->
</STYLE>
</head>

<body bgcolor="#FFFFE0">
<h1>PixelConnectivity: Interlock system in CoralDb</h1>


<?php

include 'coolr_logon.php';

	/* CoralDB tag */
	if (strlen($_GET[tag]))
		$tag = $_GET[tag];
	else
		$tag = "CT-C34ISP-V21";

	print"<h3>from CoralDB, tag='$tag'</h3>\n";

 ?>
<?php

include 'coolr_logon.php';

	/* CoralDB tag */
	if (strlen($_GET[tag]))
		$tag = $_GET[tag];
	else
		$tag = "CT-C34ISP-V21";

	print"<h2>from CoralDB, tag='$tag'</h2>\n";

 ?>

<h2>Full table with comments</h2>
<table>
<tr>
 <th>IL object/crate</th>
 <th>IL slot</th>
 <th>type</th>
 <th>connected to</th>
 <th>DCS XML tag</th>
 </tr>

<?php

	/* Find interlock connections in CoralDB and auxiliary table */
	$s = OCIParse($c, " SELECT DISTINCT c.id id, c.slot slot, 
			type, c.toid toid, ils.id il_id, ils.slot il_slot, dcstype,
			SUBSTR(path, 2, INSTR(path, '@', 2) - 2) root
		FROM (
			SELECT DISTINCT id, slot, LEVEL lvl, toid,
				SYS_CONNECT_BY_PATH(toid, '@') path
				FROM atlas_cool_pixel.coraldb_connections
			WHERE tag='$tag'
			CONNECT BY PRIOR id = toid AND tag = PRIOR tag
			START WITH toid LIKE 'D%S_' OR toid LIKE 'L%S____'
			) c, (
			SELECT id, type
				FROM atlas_cool_pixel.coraldb_objectdict
			WHERE type LIKE 'IDB%' OR TYPE LIKE 'LU%'
			) d, (
			SELECT * FROM atlas_cool_pixel.conndb_il_connections
			WHERE tag='SR1'
			) ils
		WHERE c.id=d.id AND c.toid = ils.toid(+)
		ORDER BY root, type, c.toid");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		print "<p><b><font color=red>" . "SQL Error: " .  $erra['message']
			. "</font></b></p>\n";
		}
	else {
		while (OCIFetch($s)) {
			if ($root != ociresult($s, "ROOT")) {
				$root = ociresult($s, "ROOT");
				print "<td class=rowheader colspan=5><b>$root<b></td></tr>\n"
					. "\t<tr>";
				$idblv = "a"; }
			print "\t<td>" . ociresult($s, "ID") . "</td>\n";
			print "\t<td>" . ociresult($s, "SLOT") . "</td>\n";
			print "\t<td>" . ociresult($s, "TYPE") . "</td>\n";
			print "\t<td>" . ociresult($s, "TOID") . "</td>\n";
			if (strlen(ociresult($s, "IL_ID"))) {
				$dcstype = ociresult($s, "DCSTYPE");
				$dcstype = $dcstype . ($dcstype == "IDBLV"? $idblv++ : "");
				print "\t<td>&lt;$dcstype&gt;"
					. '<font color="#209944">'
					. ociresult($s, "IL_ID") 
					. '</font><font color="#992044">'
					. ociresult($s, "IL_SLOT")
					. "</font>"
					. "&lt;/$dcstype&gt;";
				}
			else 
				print "\t<td>?";
			print "</td>\n\t</tr>\n";
			}
		}

	OCIFreeStatement($s);

 ?>
<tr>
 <th>IL object/crate</th>
 <th>IL slot</th>
 <th>type</th>
 <th>connected to</th>
 <th>DCS XML element</th>
 </tr>
</table>

<h2>Formal table to complement
 <code>atlas_cool_pixel.coraldb_connections</code></h2>
<table>
<tr>
 <td class=old colspan=3><b>OLD</b></td>
 <th><i>UNCHANGED</i></th>
 <td class=new colspan=3><b>NEW</b></td>
 </tr>
<tr>
 <th>id</th>
 <th>slot</th>
 <th>type</th>
 <th>toid (toslot)</th>
 <th>id</th>
 <th>slot</th>
 <th>type</th>
 </tr>

<?php
	/* Find interlock connections in CoralDB and auxiliary table */
	$s = OCIParse($c, " SELECT c.id id, c.slot slot, 
			type, c.toid toid, toslot, ils.id il_id, ils.slot il_slot, dcstype
		FROM (
			SELECT id, slot, toid, toslot
				FROM atlas_cool_pixel.coraldb_connections
			WHERE tag='$tag'
			) c, (
			SELECT id, type
				FROM atlas_cool_pixel.coraldb_objectdict
			WHERE type LIKE 'IDB%' OR TYPE LIKE 'LU%'
			) d, (
			SELECT * FROM atlas_cool_pixel.conndb_il_connections
			WHERE tag='SR1'
			) ils
		WHERE c.id=d.id AND c.toid = ils.toid
		ORDER BY type, c.toid");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		print "<p><b><font color=red>" . "SQL Error: " .  $erra['message']
			. "</font></b></p>\n";
		}
	else {
		while (OCIFetch($s)) {
			if (strlen($toggle)) 
				$toggle="";
			else
				$toggle=" class=even";
			print "\t<td$toggle>" . ociresult($s, "ID") . "</td>\n";
			print "\t<td$toggle>" . ociresult($s, "SLOT") . "</td>\n";
			print "\t<td$toggle>" . ociresult($s, "TYPE") . "</td>\n";
			print "\t<td$toggle>" . ociresult($s, "TOID") . " ("
				. ociresult($s, "TOSLOT") . ")</td>\n";
			print "\t<td$toggle>" . ociresult($s, "IL_ID") . "</td>\n";
			print "\t<td$toggle>" . ociresult($s, "IL_SLOT") . "</td>\n";
			print "\t<td$toggle>" . ociresult($s, "DCSTYPE") . "</td>\n";
			print "\t</tr>\n";
			}
		}

	OCIFreeStatement($s);

	OCILogoff($c);

 ?>


</table>

<a href="DcsConnInspect.php">&lt;&lt; Back</a>

</body>

<address><hr width=35% align=left>
<i><font size=-1>
$Id: ils.php,v 1.4 2007-04-03 10:20:03 hoffmann Exp $;
<?php
	print "Last modified: " . date("F d Y H:i:s.", getlastmod()) . "\n";
 ?>
</font></i>
</address>
</html>

