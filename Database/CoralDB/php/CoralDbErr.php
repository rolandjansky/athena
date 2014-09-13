<html>

<head>
<title>Check CoralDb for Errors</title>
<STYLE TYPE="TEXT/CSS">
<!--
 table {
	margin-left:50px}
 td {
	font-size:.8em; font-family: arial,helvetica,sans-serif;
	background-color:white; }
 th {
	font-family: arial,helvetica,sans-serif;
	background-color:#80CCCC; }
 .error {
	background-color:#FF4040}
 .rowheader {
	font-size:1em;
	background-color:#edf4f9}
 h1 {font-family: arial,helvetica,sans-serif}
-->
</STYLE>
</head>

<body bgcolor="#FFFFE0">
<h1>Check CoralDb for Errors</h1>

<h2>Contents</h2>
<ul>
 <li><a href="#Multiply_defined_aliases">Multiply defined aliases</a>
 <li><a href="#Aliases_in_CoralDb_partitions">Aliases in CoralDb partitions</a>
 <li><a href="#Connections_per_Tag">Connections per Tag and Object Type</a>
 </ul>

<a name="Multiply_defined_aliases">
<h3>Multiply defined aliases</h3>
</a>
<table>
<tr><th>Version Tag</th><th>Item</th><th>Occurences</th></tr>
<?php

include 'coolr_logon.php';

	/* Find all valid (connectivity) tags in CoralDB */
	$s = OCIParse($c, "SELECT 
			tag, convention || '::' || id item, COUNT(*) count
		FROM atlas_cool_pixel.coraldb_aliases a
			GROUP BY tag, convention, id
			HAVING COUNT(*) > 1
		UNION
		SELECT tag, '<i>(all)</i>' item, COUNT(*) count
		FROM atlas_cool_pixel.coraldb_aliases a
			GROUP BY tag
			ORDER BY tag, item");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		print "<p><b><font color=red>SQL Error: " . $erra['message']
				."</font></b></p>\n";
		}
	else {
		while (OCIFetch($s)) {
			if ( $tag != ociresult($s, "TAG") ) {
				$cl="class=rowheader";
				$tag = ociresult($s, "TAG");
				print "<tr><td $c>$tag</td>\n";
				}
			else {
				$cl="";
				print "<tr><td></td>\n";}
			print "\t<td $cl>"
				. ociresult($s, "ITEM") . "</td>\n";
			print "\t<td align=right " . (strlen($cl)? $cl: "class=error") . ">"
				. ociresult($s, "COUNT") . "</td>\n";
			print "</tr>\n";
			}
		}

	flush();
 ?>
	</table>
<?php
	OCIFreeStatement($s);
 ?>

<a name="Aliases_in_CoralDb_partitions">
<h3>Aliases in CoralDb partitions</h3>
</a>
<table>
<tr><th>Version Tag</th><th>connections</th><th>aliases</th></tr>

<?php

	/*  */
	$s = OCIParse($c, "SELECT a.tag, count_aliases, count_connections FROM (
				SELECT tag, COUNT(*) count_aliases
						FROM atlas_cool_pixel.coraldb_aliases
						GROUP BY tag ) a
				LEFT JOIN (
					SELECT tag, COUNT(*) count_connections
						FROM atlas_cool_pixel.coraldb_connections
						GROUP BY tag ) c
				ON a.tag = c.tag
			UNION SELECT c.tag, count_aliases, count_connections FROM (
				SELECT tag, COUNT(*) count_aliases
						FROM atlas_cool_pixel.coraldb_aliases
						GROUP BY tag ) a
				RIGHT JOIN (
					SELECT tag, COUNT(*) count_connections
						FROM atlas_cool_pixel.coraldb_connections
						GROUP BY tag ) c
				ON a.tag = c.tag
				ORDER BY tag");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		print "<p><b><font color=red>SQL Error: " . $erra['message']
				."</font></b></p>\n";
		}
	else {
		while (OCIFetch($s)) {
			if (!strlen(ociresult($s, "COUNT_ALIASES"))
				|| !strlen(ociresult($s, "COUNT_CONNECTIONS")))
				$cl="class=error";
			else
				$cl="";
			print "\t"
				. (strlen(ociresult($s, "TAG"))? 
					"<td>" . ociresult($s, "TAG"):
					"<td class=error><i>(no tag!)</i>")
				. "</td>\n";
			print "\t<td $cl>"
				. ociresult($s, "COUNT_CONNECTIONS") . "</td>\n";
			print "\t<td $cl>"
				. ociresult($s, "COUNT_ALIASES") . "</td>\n";
			print "</tr>\n";
	flush();
			}
		}
 ?>
	</table>
<?php

	OCIFreeStatement($s);

 ?>


<a name="Connections_per_Tag">
<h3>Connections per Tag and Object Type</h3>
</a>
<table>
<tr><th rowspan=2>Version Tag</th>
 <th colspan=50>connections</th>

<?php

	/*  */

	$s = OCIParse($c, "SELECT a.tag, a.type, count FROM
		(SELECT tag, type
			FROM 
			(SELECT DISTINCT tag FROM atlas_cool_pixel.coraldb_connections
			UNION SELECT '-title-' tag FROM DUAL) c,
			(SELECT DISTINCT type FROM atlas_cool_pixel.coraldb_objectdict) o) a
		LEFT OUTER JOIN
		(SELECT tag, type, COUNT(*) count
			FROM atlas_cool_pixel.coraldb_connections c,
				atlas_cool_pixel.coraldb_objectdict o
			WHERE (o.id = c.id OR o.id = c.toid)
			GROUP BY tag, type) b
		ON a.tag = b.tag AND a.type = b.type
		ORDER BY tag, type");
	if (!OCIExecute($s, OCI_DEFAULT)) {
		$erra=OCIError($s);
		print "<p><b><font color=red>SQL Error: " . $erra['message']
				."</font></b></p>\n";
		}
	else {
		while (OCIFetch($s)) {
			if ( $tag != ociresult($s, "TAG") ) {
				$tag = ociresult($s, "TAG");
				print "</tr>\n";
				if ($tag != '-title-')
					print "<tr><td>$tag</td>\n";
				}
			print ($tag == '-title-'? 
				"\t<td>" . ociresult($s, "TYPE") . "</td>\n":
				(strlen(ociresult($s, "COUNT"))?
				"\t<td align=right>" . ociresult($s, "COUNT") . "</td>\n":
				"\t<td class=error>" . ociresult($s, "TYPE") . "</td>\n")
				);
			}
		}

	flush();

 ?>
	</tr>
	</table>
<?php

	OCIFreeStatement($s);

	OCILogoff($c);

 ?>

<a href="DcsConnInspect.php">&lt;&lt;Back</a>

</body>

<address><hr width=35% align=left>
<i><font size=-1>
$Id: CoralDbErr.php,v 1.3 2007-07-11 09:17:37 hoffmann Exp $;
<?php
	print "Last modified: " . date("F d Y H:i:s.", getlastmod()) . "\n";
 ?>
</font></i>
</address>
</html>

