<html>
<head>
<title>DcsConn checking</title>
<head>
<!-- 
     **********************************************************
     * DcsConn.php 
     * XML generation from Pixel ConnectivityDB 
     * Dirk Hoffmann -CPPM-, January 2007  
     * $Id: DcsConnAna.php,v 2.11 2007-12-14 09:57:57 dhoffman Exp $
     **********************************************************
  -->
<script><!--
/* If this function returns false, link is not executed by browser (hidden). */
	function toggleSQL() {
		if (document.getElementsByTagName) {
			var e = document.getElementsByTagName("sql");
			for(var i=0; i<e.length; i++) {
				s = e[i].style;
				if (s.display == "none") {
					s.display = "block"; }
				else 
					s.display = "none"; 
				}
			return false; 
			}
		else
			return true;
		}
//--></script>
<style type='text/css'>
h1, h2, h3, h4, h5
	{ background-color: #EEEEEE; Display: block; padding: 2px;
	margin-top: 4pt; margin-bottom: 1pt; 
	font-weight: bolder; }
h1, h2 {
	text-align: center; }
h3 {
	margin-left: 10px;}
h4 {
	margin-left: 20px;}
h5 {
	margin-left: 30px;}
th {
	background-color: #DDDDEE;}
td {
	font-size: .8em; font-family: arial, sanserif;}
sql {
	font-size: .8em; font-family: monospace;
	display: block; margin-left: 40px;
	padding-top: 5px; padding-bottom: 5px}
sql:before { 
	content: "\A" }
</style>
<body>
<?php
/* Dirk Hoffmann -CPPM-, May 2007 */

$doc = new_xmldoc("1.0");
include 'libxml.php';
include 'libsql.php';

 ?>
<?php

function print_n_OCIParse($conn, $in)
{
	print "<sql>" 
			. str_replace("(SELECT", "<br>&nbsp;<font color=blue>(SELECT", 
			  str_replace(") c", ") c</font><br>", $in))
			."</sql>\n";
	$stmt = OCIParse($conn, $in);
	if (!OCIExecute($stmt, OCI_DEFAULT)) {
		$err = OCIError($stmt);
		print "<p/><b>" . "SQL Error:<br><font color=red>" 
			. $err['message'] . "</font>"
			. "</b><p/>";
		}
	return $stmt;
}
function sys_name($k) 
{
	global $sys_names, $sys;
	return array_key_exists($k, $sys_names)? $sys_names[$k].":": $sys;
}
/* This replaces new_connection for debug printout */
function connection($node, $type, $dpe)
{
	print "<li>$type = <b>$dpe</b>\n";
}
 ?>
<?php

/* === Parsing of HTTP-GET arguments === */

	/* Oracle table prefix */
	if (strlen($_GET[otp]))
		$otp = $_GET[otp];
	else
		$otp = "coraldb2_";

	/* CoralDB tag */
	if (strlen($_GET[tag])){
		if (preg_match("/^(.+)::(.+)$/", $_GET[tag], $r)) {
			$idtag = $r[1];
			$tag = $r[2];
			}
		else {
			$idtag = 'PIT';
			$tag = $_GET[tag];
			}
		}
	else
		$tag = "CT-A34ISP-V3";

	if (strlen($_GET[system]))
		$sys = $_GET[system] . ":";
	else
		$sys = "ATLPIX_3:";

	/* Limit output to some PCCs */
	if (strlen($_GET[limpcc]))
		$limpcc = $_GET[limpcc];
	else
		$limpcc = -1;

	/* Oracle schema: COOL / COOLONL */
	if (strlen($_GET[orauid]))
		$orauid=($_GET[orauid]);
	else
		$orauid='ATLAS_COOLONL_PIXEL';


/* === Output of connectivity data === */

print "<h1>SIT-Detector_Configuration ";
if ($limpcc) 
	print "<i>for $limpcc bi-stave/sectors only!</i>";
print "</h1>\n";
print "<h2>for $idtag::$tag \n\t";
if (strcmp($otp,"coraldb_"))
	print "<i>(DB $otp)</i> \n\t";
print '<a href="" onClick="return toggleSQL()">[toggle SQL on/off]</a>';
print "\n\t</h2>\n";

$root = $doc->add_root("SIT-Detector_Configuration");

include 'coolr_logon.php';

/* --- Global definition of PP2 channel numbering --- */
$lv_channels = array ( 0xf, //Vvdc
	/* Vddd, Vdda   L0-2(T=7)  SysTst */
		1, 4,     // M0        M1
		2, 5,     // M1ac      M2
		3, 6,     // M2ac      M3
		7, 9,     // M3ac      M4
		8, 0xa,   // M4ac      M5
		0xd, 0xb, // M5ac      M6
		0xe, 0xc  // M6ac
		);
/* --- Global definition of system names --- */
$sys_names = sys_names_array($idtag);

/* === Primary Cooling Circuits ("bistaves") === */
$s_pcc = print_n_OCIParse($c, select_pcc("$orauid.$otp", $tag));

while (OCIFetch($s_pcc) && $limpcc-- != 0) {
	$pcc = named_child($root, "ParallelCoolingCircuit", 
			ociresult($s_pcc, "PCC"));
	print "<h3>PCC = " . ociresult($s_pcc, "PCC") . "</h3>\n";
	$n_rou = 0;

	/* === Readout Units, PP0s ("halfstaves/sectors") === */
	$s_rou = print_n_OCIParse($c, select_rou("$orauid.$otp", $tag, 
			ociresult($s_pcc, "PCC")));

	while (OCIFetch($s_rou)) {

		print "<h4>ROU = " . ociresult($s_rou, "ROU") 
				. " <i>(" . ociresult($s_pcc, "PCC") . "::"
				. ociresult($s_rou, "ROU") . ")</i></h4>\n";
		print "<h5>Tree for this ROU:</h5>\n";
		$s_rou1 = print_n_OCIParse($c, "SELECT 
				c.id id, toid, slot, type, lvl FROM 
					(SELECT id, toid, slot, LEVEL lvl, tag
						FROM $orauid.".$otp."connections
						WHERE tag='$tag' "
						.(strlen($idtag)? "AND idtag='$idtag' ": "").
						"CONNECT BY PRIOR id = toid AND tag = PRIOR tag
						START WITH toid = '" . ociresult($s_rou, "PP0") . "') c,
					$orauid.".$otp."objectdict d
					WHERE c.id=d.id AND idtag='$idtag'");
		print "<table align=center><tr><th></th>\n"
			. "\t<th>id</th><th>toid</th><th>slot</th><th>type</th></tr>\n";
		while (OCIFetch($s_rou1)) {
			$mark_id = "";
			$mark_toid = "";
			$mark_slot = "";
			$mark_type = "";
			if (OCIresult($s_rou1, "TYPE") == 'BBIM-CHN') {
				$mark_type = " bgcolor=#FF4040"; 
				$mark_slot = " bgcolor=#FF4040"; 
				$mark_id = " bgcolor=#FFEE40";
				}
			else if (OCIresult($s_rou1, "TYPE") == 'LUMEor'
				|| OCIresult($s_rou1, "TYPE") == 'LUTOpto'
				|| OCIresult($s_rou1, "TYPE") == 'LUTpp2or'
				|| OCIresult($s_rou1, "TYPE") == 'IDBLV' 
				|| OCIresult($s_rou1, "TYPE") == 'IDBHV'
				|| OCIresult($s_rou1, "TYPE") == 'IDBSC'
				|| OCIresult($s_rou1, "TYPE") == 'BOB-CHN'
				|| OCIresult($s_rou1, "TYPE") == 'BOB-BLOCK') {
				$mark_type = " bgcolor=#FF4040"; 
				$mark_slot = " bgcolor=#40FF40"; 
				$mark_id = " bgcolor=#FFEE40";
				}
			else if (OCIresult($s_rou1, "TYPE") == 'LU'
				|| OCIresult($s_rou1, "TYPE") == 'IDB'
				|| OCIresult($s_rou1, "TYPE") == 'HV-PS') {
				$mark_type = " bgcolor=#CC7740"; 
				$mark_toid = " bgcolor=#FFEE40";
				}
			else if (OCIresult($s_rou1, "TYPE") == 'LV-PP2'
				|| OCIresult($s_rou1, "TYPE") == 'LV-PS'
				|| OCIresult($s_rou1, "TYPE") == 'HV-PS-CHN'
				|| OCIresult($s_rou1, "TYPE") == 'SCO-LINK') {
				$mark_type = " bgcolor=#FF4040";
				$mark_id = " bgcolor=#FFEE40";
				}
			else if (OCIresult($s_rou1, "TYPE") == 'CAN-FW-Wiener') {
				$mark_type = " bgcolor=#EFC080";
				$mark_id = " bgcolor=#FFEE40";
				}
			else if (preg_match('/^CAN-FW-/' , OCIresult($s_rou1, "TYPE"))) {
				$mark_type = " bgcolor=#FFB080";
				$mark_id = " bgcolor=#FFEE40";
				}
			print "<tr><td align=right>\\";
			for ($i=OCIresult($s_rou1, "LVL"); $i<8; $i++)
				print "---";
			print "</td>\n";
			print "\t<td$mark_id>" . OCIresult($s_rou1, "ID")
					. "</td><td$mark_toid>" . OCIresult($s_rou1, "TOID")
					. "</td><td$mark_slot>" . OCIresult($s_rou1, "SLOT")
					. "</td><td$mark_type>" . OCIresult($s_rou1, "TYPE")
					. "</b></td></tr>\n";
			}
		print "</table>\n";
		OCIFreeStatement($s_rou1);

		print "<ol>\n";
		$n_dpes = 0;
//		$rou = named_child($pcc, "ReadOutUnit", ociresult($s_rou, "ROU"));
//		Temporary replacement for Tobi's parser:
		$rou = named_child($pcc, "ReadOutUnit", preg_replace(
				'/(S[12]_[AC])([67])/', '$1/$2', ociresult($s_rou, "ROU")));

		/* Reset BBIM pattern for module temperatures 
		 * and PP2 board for low voltages */
		$bbim_pattern="";
		$pp2_board = "";
		$hv_pattern="";

		/* Define common subquery to all requests for this ROU (PP0) */
		$select_tree = select_tree(
				"$orauid.$otp", $tag, $idtag, ociresult($s_rou, "PP0")); 
		/* Yes, we ignore "IDTAG" here on purpose! (It is unique with "TAG".)*/

		/* Find both BBIMs relevant for this half-stave/sector:
		 * === T(opto)/T(mod) === */
		$s_bbietal = print_n_OCIParse($c, 
				select_mod_el1("$orauid.$otp", $select_tree, "AND idtag = '$idtag'
					AND (type IN ('BBIM-CHN', 'LV-PS', 'SCO-LINK',
							'LV-PP2', 'HV-PS-CHN', 'CAN-FW-ELMB')
						OR type LIKE 'IDB%' 
						OR type LIKE 'LU%' AND type NOT LIKE 'LU-%')
					"));

		$n_scolink=0; /* reset to show only one SCO-Link */
		$idblv_cnt = 0; /* reset IDBLV counter */
		while (OCIFetch($s_bbietal)) {
			$id = ociresult($s_bbietal, 'ID');
			$slot = ociresult($s_bbietal, 'SLOT');
			$type = OCIResult($s_bbietal, 'TYPE');

			/* See, if we have information for CANbus on stack */
			if ($type === "CAN-FW-ELMB") {
				/* Then output LU* item */
				if (sizeof($lu_stack)) {
					connection("", $lu_stack['type'], 
							sys_name($lu_stack['type']) 
							. "ELMB/" . $id . "/"
							. $lu_stack['id'] . "/" . $lu_stack['slot']);
					$lu_stack=array();
					}
				continue; /*while(OCIFetch()...)*/
				}
			else if (sizeof($lu_stack)) 
				connection("", $lu_stack['type'],
						sys_name($lu_stack['type'])
						. "<font color=red>NOCANBUS</font>/"
						. $lu_stack['id'] . "/" . $lu_stack['slot']);
			/* Clear the LU stack */	
			$lu_stack=array();

			switch($type) {

				case ("BBIM-CHN"):

				/* Set TOpto */  
				if (preg_match("/^TModule/i", $slot)) {
					$bbim_pattern=$id;
					print "<p>BBIM identified for half-stave: $id\n";
					}
				else {
					connection("", $slot, sys_name($slot)
						. substr($id, 0, -1) . strtolower(substr($id, -1))
						);
					}
				break;
				
				case ("LV-PS"):

				preg_match("/(VDDA*2)/", $slot, $r);
				connection("", $r[1], sys_name($r[1]) . $id);
				break;

				case ("SCO-LINK"):

				if ($n_scolink++<1) {
					connection("", "SCOLink", sys_name("SCOLink") . $id);
					}
				break;

				case ("LV-PP2"):

				$pp2_board = $id;
				print "<p>PP2 board identified for half-stave: $id\n";
				break;


				case ("HV-PS-CHN"):
				/* HV channel modularity? */

				if (preg_match("/\.Ch[0-9a-fA-F]-[0-9a-fA-F]$/", $id)) {
					/* module-wise */
					$hv_pattern = $id;
					}
				else 
					/* half-stave (PP0) */
					connection("", "HV", sys_name("HV") . $id);
					/* $hv_pattern remains empty for check below! */
				break;


				case "IDBLV":
					$type .= chr(ord('a')+$idblv_cnt++);
				case "IDBHV":
				case "IDBSC":

				connection("", $type, sys_name($type) 
							. $id
							. "/<font color=green>$slot</font>");
				break;

				case "LUMEor":
				case "LUTOpto":
				case "LUTpp2or":

				$lu_stack = array (
						'id' => $id,
						'slot' => $slot,
						'type' => $type
						);
				break;


				default: 

				connection("",
						"<font color=red>UNKNOWN</font> $type: $slot", $id);
				break;

				}
			}

		OCIFreeStatement($s_bbietal);

		/* Set VVDC2 */
		if (strlen($pp2_board)) {
			connection("", "VVDC2", 
					sys_name("VVDC2") . $pp2_board . ".Chf");
			}


		/* Find Interlock connections from SR1 table:
		 * === LUMEor, LUTOpto, IDBSC, IDBHV, IDBLVa, IDBLVb === */
		if (preg_match('/^CT/', $idtag)) {

			/* Find Interlock connections from SR1 table */

			$s_il = OCIParse($c, "SELECT DISTINCT 
					SUBSTR(path, 2, INSTR(path, '@', 2) - 2) root,
					ilconn, dcstype
				FROM (
					SELECT DISTINCT LEVEL lvl, toid,
						SYS_CONNECT_BY_PATH(toid, '@') path
						FROM $orauid.".$otp."connections
						WHERE tag='". $tag ."'
						CONNECT BY PRIOR id = toid AND tag = PRIOR tag
						START WITH toid = '". ociresult($s_rou, "PP0") ."'
					) c, (
					SELECT dcstype, id || slot ilconn, toid
						FROM atlas_cool_pixel.conndb_il_connections
						WHERE tag='SR1'
					) ils
				WHERE c.toid = ils.toid
				ORDER BY root, dcstype");
			if (!OCIExecute($s_il, OCI_DEFAULT)) 
				error_and_dump($doc, OCIError($s_il), "ROU::IL query (SR1)");

			while (OCIFetch($s_il)) {
				$dcstype = OCIResult($s_il, "DCSTYPE");
				if ($dcstype == 'IDBLV') 
					$dcstype .= chr(ord('a')+$idblv_cnt++);
				connection("", sys_name($dcstype) .  "ELMB/CAN0/$dcstype",
						$ilsys.OCIResult($s_il, "ILCONN"));
				}

			OCIFreeStatement($s_il);
			}
		/* else: Standard case for IL is handled above */	


		/* Initialize payload tables 
		 * (should be done from DB later, once it's in)... 
		 * using: 
		 *  ociresult($s_pcc, "PCC") = PrimaryCoolingCircuit
		 *  ociresult($s_rou, "ROU") = ReadOutUnit */
		if (preg_match("/[AC]7$/", ociresult($s_rou, "ROU"))) { /* barrel */
			/* [AC]7 = top(T) */
			$module_order = array (0, 1, 2, 3, 4, 5, 6);
			}
		else if (preg_match("/[AC]6$/", ociresult($s_rou, "ROU"))) {
			/* [AC]6 = bottom(B) */
			$module_order = array (5, 4, 3, 2, 1, 0);
			if (preg_match("/^L1/", ociresult($s_pcc, "PCC"))) /* L1 */
				$module_order = array_reverse($module_order);
			}
			/* disks */
		else if (preg_match("/^D1/", ociresult($s_pcc, "PCC")))
			$module_order = array (4, 0, 2, 5, 1, 3);
		else if (preg_match("/^D2/", ociresult($s_pcc, "PCC"))) {
			$module_order = array (1, 5, 3, 0, 4, 2);
			/* D2 sectors need special treatment: odd+R/even+L are shifted! */
			preg_match('/P(\d)([LR])/', ociresult($s_rou, "PROW"), $r);
			$d2_offset = ($r[1] + ($r[2]=='L'? 1: 0))%2;
			if (preg_match('/P[02468]L/', ociresult($s_rou, "PROW")) 
					|| preg_match('/P[13579]R/', ociresult($s_rou, "PROW")))
				foreach ($module_order as $key => $value)
					$module_order[$key]++;
			}
		else if (preg_match("/^D3/", ociresult($s_pcc, "PCC")))
			$module_order = array (2, 6, 4, 1, 5, 3);
		else  /* ERROR! */
			$module_order = array (0, 0, 0, 0, 0, 0, 0);


		/* === Modules === */
		$s_mod = print_n_OCIParse($c, select_mod("$orauid.$otp", $tag, $idtag, 
				ociresult($s_pcc, "PCC") ."_". ociresult($s_rou, "ROU")));
		/* ToDo: Use slot names/order here in case there are empty slots or 
		 * irregularly connected modules ! */
	
		$hv_chnl = 0;
		$tp_chnl = 0;
		$pp4_chnl = 0;
		$pp2_chnl = 0; // 0 is Vvdc!
		while (OCIFetch($s_mod)) {
			print "<h5>Module " . ociresult($s_mod, "MOD") .  "</h5>\n<ol>\n";

			/* === HV === */
			if (preg_match("/(.*\.Ch)([0-9a-fA-F])-([0-9a-fA-F])/",
					$hv_pattern, $r)) {
				connection("", "HV", 
					sys_name("HV") . $r[1] . 
							sprintf("%02d", $r[2]+$module_order[$hv_chnl++]));
				/* Error handling for wrong $hv_pattern needed here? */
				}

			/* === TModule === */
			if (preg_match("/(.*\.Temp)([0-9a-fA-F])-([0-9a-fA-F])/",
					$bbim_pattern, $r)) {
				connection("", "TModule", sys_name("TModule") 
						.  $r[1] . sprintf("%x", 
							$r[2]+$module_order[$tp_chnl++]));
				}

			/* === PP4VDDp/PP4VDDAp === */
			$s_lv = print_n_OCIParse($c, "SELECT DISTINCT c.id id, slot 
					FROM ($select_tree) c, 
						$orauid.".$otp."objectdict d
					WHERE c.id=d.id AND type = 'LV-PP4'");
			while (OCIFetch($s_lv)) {
				connection("", ociresult($s_lv, "SLOT"),
						sys_name(ociresult($s_lv, "SLOT")) 
						.  ociresult($s_lv, "ID") . ".Plus.I" 
						. sprintf("%d", $module_order[$pp4_chnl]+1)
						);
				}
			$pp4_chnl++;

			OCIFreeStatement($s_lv);

			/* === VDD1/VDDA1 === */
			if (strlen($pp2_board)) {
				connection("", "VDD1", sys_name("VDD1") .$pp2_board.
						sprintf(".Ch%x", 
							$lv_channels[1+$module_order[$pp2_chnl]*2]));
				connection("", "VDDA1", sys_name("VDDA1") .$pp2_board.
						sprintf(".Ch%x", 
							$lv_channels[2+$module_order[$pp2_chnl]*2]));
				$pp2_chnl++;
				}

			/* === LUMETModule (tbd!) === */
			connection("", "LUMETModule",   "");

			print "</ol>\n";
			}
		OCIFreeStatement($s_mod);

		$n_rou++;

		print "</ol>\n";

		} /*while (OCIFetch($s_rou))*/

	OCIFreeStatement($s_rou);

	}

OCIFreeStatement($s_pcc);
OCILogoff($c);


 ?>
<?php
	print "<!-- Last modified: " . date("F d Y H:i:s.", getlastmod()) . " -->";
	print "<!-- Code execution finished at " . date("r") . " -->\n";
 ?>

</body>
</html>
