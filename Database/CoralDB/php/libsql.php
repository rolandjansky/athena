<?php 
/*-- libsql.php, SQL requests function library for DCS connectivity 
 *   Dirk Hoffmann -CPPM-, August 2007 --
 *   $Id: libsql.php,v 2.5 2007-09-06 10:13:04 hoffmann Exp $ */
function select_pcc($otp, $tag) 
{
	return "SELECT DISTINCT SUBSTR(toid, 1, INSTR(toid, '_', 1, 2)-1) pcc
    	FROM ".$otp."connections
		WHERE id='PP0S' AND tag='".$tag."' 
		ORDER BY pcc";
}
function select_rou($otp, $tag, $pcc)
{
	return "SELECT DISTINCT 
		SUBSTR(o.id,
			INSTR(o.id,'_',1,2)+1, 
			INSTR(o.id,'_',-1)-INSTR(o.id,'_',1,2)-1) rou,
		c.id pp0,
		SUBSTR(alias,
			INSTR(alias, '-P')+1, 
			INSTR(alias, '-', -1)-INSTR(alias, '-P')-1) prow
		FROM ".$otp."objectdict o,
			".$otp."connections c
		JOIN ".$otp."aliases a
		ON a.tag = c.tag AND a.id = c.id AND convention = 'GEOID'
		WHERE c.tag='$tag'
			AND SUBSTR(o.id, 1, INSTR(o.id,'_',1,2)-1) = '$pcc'
			AND o.id = c.toid
		ORDER BY rou";
}
function select_tree($otp, $tag, $idtag, $pp0)
{
	return	"SELECT id, slot, ROWNUM n FROM (
				SELECT id, toid, slot 
					FROM ".$otp."connections
					WHERE tag='$tag' AND idtag='$idtag')
			CONNECT BY PRIOR id = toid 
			START WITH toid = '$pp0'";
}
function select_mod($otp, $tag, $idtag, $pp0)
{
	return "SELECT SUBSTR(toid, INSTR(toid, '_', -1)+1) mod
		FROM ".$otp."connections
		WHERE id = '$pp0' AND tag = '$tag' AND idtag = '$idtag'
		ORDER BY mod";
}
function select_mod_el1($otp, $select_tree, $append)
{
	return "SELECT DISTINCT c.id id, slot, type, n
		FROM ($select_tree) c,
			".$otp."objectdict d
		WHERE c.id=d.id $append
		ORDER BY n";
}
 ?>
