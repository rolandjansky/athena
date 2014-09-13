<?php 
/*-- libxml.php, XML format DCS connectivity function library      
 *   Dirk Hoffmann -CPPM-, January 2007 --
 *   $Id: */
function named_child($node, $tag, $name) 
{
	$child = $node->new_child($tag, "");
	$child->set_attribute("name", $name);
	return $child;
}
function new_connection($node, $type, $dpe)
{
	$conn = $node->new_child("Connection", "");
	$conn->set_attribute("type", $type);
	if (is_a($dpe, "DomComment")) {
		$dom_dpe = $conn->new_child("DataPointElement", "");
		$dom_dpe->append_child($dpe);
		}
	else {
		$dom_dpe = $conn->new_child("DataPointElement", $dpe);
		if (!strlen($dpe))
			$dom_dpe->new_child("undefined");
		}
	return $dom_dpe;
}
function new_module($node, $place)
{
	if (is_numeric($place)) 
		$module_name = sprintf("M%d%s", abs($place), 
				$place>0? "A": ($place<0? "C": ""));
	else
		$module_name = $place;
	$module = $node->new_child("Module", "");
	$module->set_attribute("name", $module_name);
	return $module;
}

function sys_names_array($idtag)
{
	if (preg_match('/^PIT/', $idtag) || (preg_match('/^PIXEL/', $idtag)))
		return array (
			"TOpto"      => "ATLPIXELMB1",
			"TModule"    => "ATLPIXELMB1",
			"Tpp2"       => "ATLPIXILOCK",
			"SCOLink"    => "ATLPIXELMB1",
			"PP4VDDp"    => "ATLPIXELMB2",
			"PP4VDDAp"   => "ATLPIXELMB2",
			"HV"         => "ATLPIXHV",
			"VDD2"       => "ATLPIXLV",
			"VDDA2"      => "ATLPIXLV",
			"VVDC2"      => "ATLPIXPP2",
			"VDD1"       => "ATLPIXPP2",
			"VDDA1"      => "ATLPIXPP2",
			"PP2Vaux1"   => "ATLPIXPP2AUX",
			"PP2Vaux2"   => "ATLPIXPP2AUX",
			"LUMEor"     => "ATLPIXILOCK",
			"LUMETModule"=> "ATLPIXILOCK",
			"LUTOpto"    => "ATLPIXILOCK",
			"LUTpp2or"   => "ATLPIXILOCK",
			"IDBHV"      => "ATLPIXILOCK",
			"IDBLVa"     => "ATLPIXILOCK",
			"IDBLVb"     => "ATLPIXILOCK",
			"IDBVDD"     => "ATLPIXILOCK",
			"IDBVDDA"    => "ATLPIXILOCK",
			"IDBSC"      => "ATLPIXILOCK",
			"BOB"        => "ATLPIXILOCK",
			"OHIbox"     => "ATLPIXILOCK"
			);
	else /* SR1 */
		return array (
			"TOpto"      => "ATLPIXSR1ELMB",
			"TModule"    => "ATLPIXSR1ELMB",
			"SCOLink"    => "ATLPIXSR1ELMB",
			"PP4VDDp"    => "ATLPIXSR1ELMB",
			"PP4VDDAp"   => "ATLPIXSR1ELMB",
			"HV"         => "ATLPIXSR1HV",
			"VDD2"       => "ATLPIXSR1LV",
			"VDDA2"      => "ATLPIXSR1LV",
			"VVDC2"      => "ATLPIXSR1ELMB",
			"VDD1"       => "ATLPIXSR1ELMB",
			"VDDA1"      => "ATLPIXSR1ELMB",
			"LUMEor"     => "ATLPIXSR1ILOCK",
			"LUMETModule"=> "ATLPIXSR1ILOCK",
			"LUTOpto"    => "ATLPIXSR1ILOCK",
			"IDBHV"      => "ATLPIXSR1ILOCK",
			"IDBLVa"     => "ATLPIXSR1ILOCK",
			"IDBLVb"     => "ATLPIXSR1ILOCK",
			"IDBVDD"     => "ATLPIXSR1ILOCK",
			"IDBVDDA"    => "ATLPIXSR1ILOCK",
			"IDBSC"      => "ATLPIXSR1ILOCK"
			);
}

 ?>
