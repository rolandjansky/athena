<?php
/* See author information at end of file */

$doc = new_xmldoc("1.0");
include 'libxml.php';
include 'libsql.php';

 ?>
<?php
function error_and_dump($d, $e, $w)
{
	Header("Content-type: text/html");
	print "<h2>" . "SQL Error:<br><font color=red>" 
		. $e['message'] . "</font>"
		. (strlen($w)? "<br> in ".$w: "")
		. "</h2>";
	print "XML output until here:<p><pre>\n";
	print htmlspecialchars(str_replace(">", ">\n", $d->dumpmem()));
	print "\n</pre>\n";
	exit(0);
}
function OciParseExecErrDump($conn, $querytag, $sql, $xml)
{
	$cursor = OCIParse($conn, $sql);
	if (!OCIExecute($cursor, OCI_DEFAULT))
		error_and_dump($xml, OCIError($cursor), $querytag);
	return $cursor;
}
function sys_name($k) 
{
	global $sys_names, $sys;
	return array_key_exists($k, $sys_names)? $sys_names[$k].":": $sys;
}
 ?>
<?php

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
	$ilsys = "ATLPIX_4:";

	/* Limit output to some PCCs */
	if (strlen($_GET[limpcc]))
		$limpcc = $_GET[limpcc];
	else
		$limpcc = -1;

	/* Limit separate output of PP2crates:
	 * 0 = come with PCCs/ROUs tree
	 * >0 = obviously used as limit */
	if (strlen($_GET[limpp2]))
		$limpp2 = $_GET[limpp2];
	else
		$limpp2 = -1;

	/* Stylesheet */
	if (strlen($_GET[stylesheet])) {
		$doc->append_child($doc->create_processing_instruction(
			"xml-stylesheet", "type=\"text/css\" href=\""
			. $_GET[stylesheet] ."\""));
		define("CSS", null);
		}

	/* Oracle schema: COOL / COOLONL */
	if (strlen($_GET[orauid]))
		$orauid=($_GET[orauid]);
	else
		$orauid='ATLAS_COOLONL_PIXEL';


/* Later: Doctype 
 * <!DOCTYPE SIT-Detector_Configuration SYSTEM "DcsConn.dtd">
This does not work:
	$doc->new_child(
		'!DOCTYPE SIT-Detector_Configuration SYSTEM "DcsConn.dtd"');
 */
			

$doc->append_child($doc->create_comment(
	"Generated from ".strtoupper($orauid)." tables "
	. "(".$otp."objectdict et al.)"));
$doc->append_child($doc->create_comment(
	"by " . $_SERVER["SERVER_NAME"] 
	. " for " . getenv("REMOTE_HOST") . " (" . getenv("REMOTE_ADDR") .  ")"
	. " on " . date("r")));
$starttime = time();

if (strlen($_GET[mode])) {
	if ($_GET[mode] = 'empty')
		define("EMPTY", null);
	$doc->append_child($doc->create_comment(
			"Special MODE requested: " . $_GET[mode]));
	}


//$root = $doc->add_root("SIT-Detector_Configuration");
$root = $doc->append_child($doc->create_element("SIT-Detector_Configuration"));

$root->new_child("DBTAG", $tag);
$root->new_child("DBIDTAG", $idtag);

include 'coolr_logon.php';

/* --- Global definition of system names --- */
$sys_names = sys_names_array($idtag);


/* ============================================================ *
 * 
 * PP0 (prim. cool. circuit) oriented connectivity 
 * 
 * ============================================================ */

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

/* === Primary Cooling Circuits ("bistaves") === */
$s_pcc = OCIParse($c, select_pcc("$orauid.$otp", $tag));
if (!OCIExecute($s_pcc, OCI_DEFAULT)) 
		error_and_dump($doc, OCIError($s_pcc), "PCC query");

while (OCIFetch($s_pcc)) {
//	if (!preg_match('/^L0/', ociresult($s_pcc, "PCC"))) continue;
	if ($limpcc-- == 0) break;

	$pcc = named_child($root, "ParallelCoolingCircuit", 
			ociresult($s_pcc, "PCC"));
	$n_rou = 0;

/*
	$pcc->new_child("DEBUG", select_rou($otp, $tag, 
			ociresult($s_pcc, "PCC")));
*/

	/* === Readout Units, PP0s ("halfstaves/sectors") === */
	$s_rou = OCIParse($c, select_rou("$orauid.$otp", $tag, 
			ociresult($s_pcc, "PCC")));
	if (!OCIExecute($s_rou, OCI_DEFAULT))
		error_and_dump($doc, OCIError($s_rou), "ROU query");

	while (OCIFetch($s_rou)) {
		$n_dpes = 0;
		$rou = named_child($pcc, "ReadOutUnit", ociresult($s_rou, "ROU"));

		if (defined ("CSS")) {
			$dbg = $rou->new_child("H1", ociresult($s_pcc, "PCC") . "::"
					. ociresult($s_rou, "ROU") );
			$dbg->set_attribute("type", "PCC::ROU");
			}

		/* Reset BBIM pattern for module temperatures 
		 * and PP2 board for low voltages */
		$bbim_pattern="";
		$pp2_board = "";
		$hv_pattern="";

		if (defined ("EMPTY")) {
			foreach ( array(
					"TOpto", 
//					"HV", 
					"VDD2",
					"VDDA2",
					"VVDC2",
					"SCOLink",
					"LUME",
					"LUTOpto",
					"IDBHV",
					"IDBVDD",
					"IDBVDDA",
					"IDBSC",
					"BOB"
					) as $element ) 
				new_connection($rou, $element,   "");
			}
		else {
			/* Define common subquery to all requests for this ROU (PP0) */
			$select_tree = select_tree(
					"$orauid.$otp", $tag, $idtag, ociresult($s_rou, "PP0")); 

			/* Find BBIMs (et al.) relevant for this half-stave/sector:
			 * === T(opto)/T(mod) === */
			$s_bbietal = OCIParse($c, 
					select_mod_el1("$orauid.$otp", $select_tree, "AND idtag='$idtag'
						AND (type IN ('BBIM-CHN', 'BOB-BLOCK',
								'LV-PS', 'SCO-LINK', 'LV-PP2', 'HV-PS-CHN')
							OR type LIKE 'CAN-FW-%' 
							OR type LIKE 'IDB%' 
							OR type LIKE 'PVSS-SYSTEM-%' 
							OR type LIKE 'LU%' AND type NOT LIKE 'LU-%')
							"));
			if (!OCIExecute($s_bbietal, OCI_DEFAULT))
				error_and_dump($doc, OCIError($s_bbietal), "ROU::BBIM++ query");

			$n_scolink=0; /* reset to show only one SCO-Link */
			$idb_cnt = array( /* reset IDB** counters */
					'IDBHV' => 1,
					/* tricky! */ 'IDBLV' => 2, 'IDBLVa' => 1, 'IDBLVb' => 1, 
					'IDBSC' => 1,
					); 
			while (OCIFetch($s_bbietal)) {
				$id = ociresult($s_bbietal, "ID");
				$slot = ociresult($s_bbietal, "SLOT");
				$type = OCIResult($s_bbietal, 'TYPE');

				if ($limpp2!=0 && preg_match('/Tpp2/', $type.$slot)) {
					//$rou->new_child("DEBUG", "Tpp2 skipped: ". $type);
					continue;}

				/* See, if we have information for PVSS-SYSTEM on ps_stack */
				if (substr($type,0,12) === "PVSS-SYSTEM-") {
					if (sizeof($ps_stack))
						new_connection($rou, 
								$ps_stack['type'], 
								"$id:ELMB/" 
								. $ps_stack['id'] . "/" . $ps_stack['slot']);
					/* Clear the PVSS-SYSTEM stack */
					$ps_stack = array();
					continue; /*while(OCIFetch()...)*/
					}

				/* See, if we have information for CANbus on stack */
				else if (preg_match("/CAN-FW-(.*)/", $type, $r)) {
					/* Then output LU* item */
					if (sizeof($lu_stack)) {
						new_connection($rou, $lu_stack['type'], 
								sys_name($lu_stack['type'])
								. "$r[1]/$id/"
								. $lu_stack['id'] . "/" . $lu_stack['slot']);
						$lu_stack=array();
						}
					continue; /*while(OCIFetch()...)*/
					}
				else if (sizeof($lu_stack)) 
					/* Notify missing canbus */
					new_connection($rou, $lu_stack['type'],
							sys_name($lu_stack['type'])
							. "NOCANBUS/"
							. $lu_stack['id'] . "/" . $lu_stack['slot']);
				/* Clear the LU stack */	
				$lu_stack=array();

				switch($type) {

					case ("BBIM-CHN"):

					/* Set TOpto */
					if (preg_match("/^TModule/i", $slot))
						$bbim_pattern=$id;
					else  {
						new_connection($rou, $slot, sys_name($slot)
							. substr($id, 0, -1) . strtolower(substr($id, -1))
							);
						$n_dpes++;
						}
					break;

					case ("BOB-BLOCK"):

					//new_connection($rou, "BOB", sys_name("BOB").  $id);
					$ps_stack = array (
							'type' => 'BOB',
							'id' => $id,
							'slot' => $slot
							);
					break;

					case ("LV-PS"):

					preg_match("/(VDDA*2)/", $slot, $r);
					new_connection($rou, $r[1], sys_name($r[1]) . $id);
					$n_dpes++;
					break;

					case ("SCO-LINK"):

					if ($n_scolink++<1) {
						new_connection($rou, 
								"SCOLink", sys_name("SCOLink") . $id);
						$n_dpes++;
					}
					break;

					case ("LV-PP2"):
					/* PP2-RS for this half-stave/sector and modules */

					$pp2_board = $id;
					break;

					case ("HV-PS-CHN"):
					/* HV channel modularity? */

					if (preg_match("/\.Ch[0-9a-fA-F]-[0-9a-fA-F]$/", $id)) {
						/* module-wise */
						$hv_pattern = $id;
						}
					else 
						/* half-stave (PP0) */
						new_connection($rou, "HV", sys_name("HV") . $id);
						/* $hv_pattern remains empty for check below! */
					break;


					case "IDBLV":
					$type .= chr(ord('a')-1+$idb_cnt{$type}--);
					case "IDBHV":
					case "IDBSC":

					if (!($idb_cnt{$type}--))
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


					default: /* For debugging. Remove later? */

					new_connection($rou, "UNKNOWN::$slot"."[$type]", $id);
					$n_dpes++;
					break;

					}
				}

			OCIFreeStatement($s_bbietal);


			/* Set VVDC2 ($pp2_board is later used once per modules) */
			if (strlen($pp2_board)) {
				new_connection($rou, "VVDC2", 
						sys_name("VVDC2") . $pp2_board . ".Chf");
				$n_dpes++;
				}


			/* Find Interlock connections from SR1 table:
			 * === LUMEor, LUTOpto, IDBSC, IDBHV, IDBLVa, IDBLVb === */
			$idblv_cnt = 0;
			if (preg_match('/^CT/', $idtag)) {
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
					new_connection($rou, 
							sys_name($dcstype) .  "ELMB/CAN0/$dcstype",
							$ilsys.OCIResult($s_il, "ILCONN"));
					}
				}

				/* common finishing */
				OCIFreeStatement($s_il);

			}

		

		/* Initialize payload tables 
		 * (should be done from DB later, once it is in)... 
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
/*
			$rou->new_child("prow", ociresult($s_rou, "PROW") 
				. "=" .  $d2_offset
				. "(" . $r[1] . "," . $r[2] . ")");
*/
			if (preg_match('/P[02468]L/', ociresult($s_rou, "PROW")) 
					|| preg_match('/P[13579]R/', ociresult($s_rou, "PROW")))
				foreach ($module_order as $key => $value)
					$module_order[$key]++;
			}
		else if (preg_match("/^D3/", ociresult($s_pcc, "PCC")))
			$module_order = array (2, 6, 4, 1, 5, 3);
		else  /* ERROR! */
			$module_order = array (0, 0, 0, 0, 0, 0, 0);

/*
		$rou->new_child("module_order", implode(",", $module_order));
*/
/*
		$rou->new_child("DEBUG", "SELECT DISTINCT id3 mod, m.id mod_id, c.id sqp
			FROM $orauid.".$otp."modules m,
				$orauid.".$otp."connections c
			WHERE id1 = '" . ociresult($s_pcc, "PCC") ."'
				AND id2 = '" . ociresult($s_rou, "ROU") ."'
				AND m.id = c.toid
			ORDER BY mod");
*/

		/* === Modules === */
		$s_mod = OCIParse($c, select_mod("$orauid.$otp", $tag, $idtag, 
				ociresult($s_pcc, "PCC") ."_". ociresult($s_rou, "ROU")));
		/* ToDo: Use slot names/order here in case there are empty slots or 
		 * irregularly connected modules ! */
		if (!OCIExecute($s_mod, OCI_DEFAULT)) 
			error_and_dump($doc, OCIError($s_mod), "MOD query");
	
		/* Initialize counters before module loop */
		$hv_chnl = 0;
		$tp_chnl = 0;
		$pp2_chnl = 0; // 0 is Vvdc!

		/* === Prepare: PP4VDDp/PP4VDDAp === */
		$pp4_chnl = 0;
		$pp4_lv = array();
		$s_lv = OCIParse($c, "SELECT DISTINCT c.id id, slot 
			FROM ($select_tree) c,
				$orauid.".$otp."objectdict d
			WHERE c.id=d.id AND type = 'LV-PP4'");
		if (!OCIExecute($s_lv, OCI_DEFAULT))
			error_and_dump($doc, OCIError($s_lv), "ROU::PP4 query");
		while (OCIFetch($s_lv)) {
			array_push($pp4_lv, array(
					'slot' => ociresult($s_lv, "SLOT"),
					'id' =>   ociresult($s_lv, "ID") ));
			}

		OCIFreeStatement($s_lv);

		/* Now loop over modules */
		while (OCIFetch($s_mod)) {
			$mod = new_module($rou, ociresult($s_mod, "MOD"));

			if (defined ("CSS")) {
				$dbg = $mod->new_child("H2", ociresult($s_mod, "MOD"));
				$dbg->set_attribute("type", "Module");
				}

			if (defined ("EMPTY"))
				foreach ( array(
						"HV", 
						"TModule", 
						"VDD1",    
						"VDDA1",   
						"PP4VDDp", 
						"PP4VDDAp",
						"LUMETModule"
						) as $element ) 
					new_connection($mod, $element,   "");
			else {
				/* === HV === */
				if (preg_match("/^(.*\.Ch)([0-9a-fA-F])-([0-9a-fA-F])$/",
						$hv_pattern, $r) ) {
					new_connection($mod, "HV", 
							sys_name("HV") . $r[1] . 
							sprintf("%02d", $r[2]+$module_order[$hv_chnl++]));
					$n_dpes++;
					/* Error handling for wrong $hv_pattern needed here? */
					}

				/* === TModule === */
				if (preg_match("/(.*\.Temp)([0-9a-fA-F])-([0-9a-fA-F])/",
						$bbim_pattern, $r)) {
					new_connection($mod, "TModule", sys_name("TModule") 
							.  $r[1] . sprintf("%x", 
								$r[2]+$module_order[$tp_chnl++]));
					$n_dpes++;
					}

				/* === PP4VDDp/PP4VDDAp === */
				foreach ($pp4_lv as $i) {
					new_connection($mod, $i['slot'],
							sys_name($i['slot']) 
							.  ($i['id']) . ".Plus.I" 
							. sprintf("%d", $module_order[$pp4_chnl]+1)
							);
					$n_dpes++;
					}
				$pp4_chnl++;

				/* === VDD1/VDDA1 === */
				if (strlen($pp2_board)) {
					new_connection($mod, "VDD1", sys_name("VDD1") .$pp2_board.
							sprintf(".Ch%x", 
								$lv_channels[1+$module_order[$pp2_chnl]*2]));
					new_connection($mod, "VDDA1", sys_name("VDDA1") .$pp2_board.
							sprintf(".Ch%x", 
								$lv_channels[2+$module_order[$pp2_chnl]*2]));
					$n_dpes+=2;
					$pp2_chnl++;
					}

				/* === LUMETModule (tbd!) === */
				new_connection($mod, "LUMETModule",   "");

				}/*if(defined(EMPTY))::else*/

			}
		OCIFreeStatement($s_mod);

		/* If no elements have been defined, remove all children of this ROU */
		if ($n_dpes == 0) {
			foreach ($rou->child_nodes() as $child) 
				$rou->remove_child($child);
			}

		$n_rou++;

		} /*while (OCIFetch($s_rou))*/

	OCIFreeStatement($s_rou);

	}

OCIFreeStatement($s_pcc);

/* ============================================================ *
 * 
 * PP2 crate oriented connectivity 
 * 
 * ============================================================ */

	$s_pp2 = OciParseExecErrDump($c, "PP2Crate query",
			"SELECT c.id id, slot, type, lvl,
				SUBSTR(path, 2, INSTR(path||'$', '$', 1, 2)-2) crate
			 FROM (
			 SELECT id, slot, LEVEL lvl, ROWNUM n, 
				SYS_CONNECT_BY_PATH(toid, '$') path
			 FROM $orauid.coraldb2_connections
			  WHERE idtag='$idtag' AND tag = '$tag'
			  CONNECT BY toid = PRIOR id 
			   AND tag = PRIOR tag AND idtag = PRIOR idtag
			  START WITH toid IN (
			    SELECT id FROM $orauid.coraldb2_objectdict
			    WHERE idtag='$idtag' AND type='PP2-CRATE') 
			 ) c,
			 (
			 SELECT id, type FROM $orauid.coraldb2_objectdict
			  WHERE idtag='$idtag' 
			 ) d
			WHERE c.id = d.id AND
			(lvl=1 OR type IN 
					('BBIM-CHN', 'LUTpp2or', 
					'PVSS-SYSTEM-IDBLU', 'PVSS-SYSTEM-AUXPS')
			)
			ORDER BY n DESC",
			$doc);

	$last_lvl = 0;
	while (OCIFetch($s_pp2)) {
		$lvl = ociresult($s_pp2, "LVL");
		if (strcmp($pp2_cratename, ociresult($s_pp2, "CRATE"))) {
			if ($limpp2-- != 0) {
				$pp2_cratename = ociresult($s_pp2, "CRATE");
				$pp2_crate = named_child($root, "PP2Crate", $pp2_cratename);
				$pp2vaux_cnt = 0;
				}
			else
				break;
			}
		if ($lvl > $last_lvl) {
			$last_lvl = $lvl;
			$sysname = "NOSYSTEM";
			}

		$type = ociresult($s_pp2, "TYPE");
		$id = $sysname.":".ociresult($s_pp2, "ID");

//		$pp2_crate->new_child("DEBUG", $id);

		switch($type) {

			case "PVSS-SYSTEM-IDBLU":
			case "PVSS-SYSTEM-AUXPS":
				$sysname = ociresult($s_pp2, "ID");
				break;

			case ("BBIM-CHN"):
				new_connection($pp2_crate, 
						ociresult($s_pp2, "SLOT"),
						$id);
				break;

			case ("LUTpp2or"):
				new_connection($pp2_crate,
						$type, 
						$id);
				break;

			case ("AUX-PS-CHN"):
				new_connection($pp2_crate, 
						ociresult($s_pp2, "SLOT")
							. chr(ord('1') + $pp2vaux_cnt++),
						$id);
				break;

			}
		}

	OCIFreeStatement($s_pp2);


/* ============================================================ *
 * 
 * VME crate oriented connectivity 
 * 
 * ============================================================ */

	$s_vme = OciParseExecErrDump($c, "RODcrate query",
			"SELECT c.toid crate, c1.id ||'/'|| c1.slot dpe
				FROM $orauid.coraldb2_connections c,
					$orauid.coraldb2_connections c1,
					$orauid.coraldb2_objectdict d,
					$orauid.coraldb2_objectdict d1
				WHERE d.idtag='$idtag' AND d.type = 'VME-PS'
					AND d1.idtag='$idtag' AND d1.type = 'CAN-FW-Wiener'
					AND c.idtag='$idtag' AND c.tag = '$tag'
					AND c1.idtag='$idtag' AND c1.tag = '$tag'
					AND d.id = c.id
					AND d1.id = c1.id
					AND c1.toid = c.id");

	$rodcanbus = array();
	while (OCIFetch($s_vme)) 
		$rodcanbus[ociresult($s_vme, 'CRATE')] 
				= "ATLPIXVME:Wiener/".ociresult($s_vme, 'DPE');

	OCIFreeStatement($s_vme);

	$vme_list = $root->new_child("VmeCrateList", "");
	//$root->new_child("DEBUG", sizeof($rodcanbus));

	$s_vme = OciParseExecErrDump($c, "VmeCrate query",
			"SELECT r.id crate, c.id id, slot, c.toid toid, lvl, type FROM
			(
			SELECT id, toid, slot, LEVEL lvl, ROWNUM n
			 FROM $orauid.coraldb2_connections
			  CONNECT BY PRIOR id = toid AND idtag='$idtag' AND tag = '$tag'
			  START WITH idtag='$idtag' AND tag = '$tag'
			   AND id IN (
			    SELECT id FROM $orauid.coraldb2_objectdict
			     WHERE idtag='$idtag' AND type = 'BOCMON-DP-ELT'
			     ) ) c,
			  (SELECT id, type FROM $orauid.coraldb2_objectdict
			   WHERE idtag='$idtag'
			   ) d,
			 (
			  SELECT d.id id, toid
			    FROM $orauid.coraldb2_objectdict d,
			     $orauid.coraldb2_connections c
			   WHERE d.idtag='$idtag' AND type = 'RODCRATE'
			    AND c.idtag='$idtag' AND c.tag = '$tag' AND c.id = d.id
			    AND c.toid = c.toid
			 ) r
			 WHERE  c.id = d.id AND r.toid(+) = c.toid
			 ORDER BY n",
/*
			"SELECT id, toid, slot
			 FROM $orauid.coraldb2_connections
			 WHERE idtag='$idtag' AND tag = '$tag'
			 AND id IN (
			  SELECT id FROM atlas_coolonl_pixel.coraldb2_objectdict
			   WHERE idtag = '$idtag' AND type = 'RODCRATE')",
*/
			$doc);

	while (OCIFetch($s_vme)) {
		if (ociresult($s_vme, 'LVL')==1 && sizeof($stack)) {
			/* This catchup will be missed on the last statement! */
			new_connection($vme_crate, 
					$stack['slot'], 'NOSYSTEM:'.$stack['dpe']);
			$stack=array();}
		if (strlen($crate = ociresult($s_vme, "CRATE"))) {
			/* Do we have a new crate ? */
			if (strcmp($vme_cratename, $crate)) {
				$vme_cratename = $crate;
				$vme_crate = named_child($vme_list, "VmeCrate", $vme_cratename);
				new_connection($vme_crate, 'VME', $rodcanbus[$vme_cratename]); 
				}
			$stack['dpe'] = ociresult($s_vme, "ID");
			$stack['slot'] = ociresult($s_vme, "SLOT");
			}
		if (preg_match('/^PVSS-SYSTEM/', ociresult($s_vme, 'TYPE'))) {
			/* Found PVSS system: output and flush stack */
			new_connection($vme_crate, 
					$stack['slot'], 
					ociresult($s_vme, "ID") . ":" . $stack['dpe']);
			$stack=array();}
		}
		/* Cleanup */
		if (sizeof($stack)) {
			/* This catchup will be missed on the last statement! */
			new_connection($vme_crate, 
					$stack['slot'], 'NOSYSTEM:'.$stack['dpe']);
			$stack=array();}

	OCIFreeStatement($s_vme);



/* ============================================================ *
 * 
 * OptoHeater Interlock connectivity 
 * 
 * ============================================================ */


	$s_ohi = OciParseExecErrDump($c, "OHIbox query",
		"SELECT id, toid FROM $orauid.coraldb2_connections
			WHERE idtag='$idtag' AND tag='$tag'
			AND id IN (
				SELECT id FROM $orauid.coraldb2_objectdict
				WHERE idtag='$idtag' AND type='OHI-BOX-CHN'
				)
			");

	$ohibox_list = $root->new_child("OHIboxList", "");

	while (OCIFetch($s_ohi)) {
		$ohibox = $ohibox_list->new_child("OHIbox", "");
		$ohibox->set_attribute("name", ociresult($s_ohi, "TOID"));
		new_connection($ohibox, "OHIbox",
				sys_name("OHIbox") . "ELMB/" . ociresult($s_ohi, "ID"));
		}

	OCIFreeStatement($s_ohi);

OCILogoff($c);

$querytime=time()-$starttime;

Header("Content-type: text/xml");
print preg_replace("/(<[a-zA-Z])/", "\n$1", $doc->dumpmem());

 ?>
<!-- 
     **********************************************************
     * DcsConn.php 
     * XML generation from Pixel ConnectivityDB 
     * Dirk Hoffmann -CPPM-, January 2007  
     * $Id: DcsConn.php,v 2.16 2008-02-15 15:03:45 dhoffman Exp $
     **********************************************************
  -->
<?php
	print "<!-- Last modified: " . date("F d Y H:i:s.", getlastmod()) . " -->";
	print "<!-- Code execution finished at " . date("r") . 
			" ($querytime/" . (time()-$starttime) . "s) -->\n";
 ?>

