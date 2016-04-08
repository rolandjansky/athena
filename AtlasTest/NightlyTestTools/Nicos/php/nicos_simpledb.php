<?
//--------------------------------------------------------------- CONNECT (required to update_pb i delete_pb)--------------------------------------------------
function connect_pb ($filename)
{
	$fcont =file($filename);
	$elements = split(";",$fcont[0]);

	for($i = 1; $i< (count($fcont)) ;$i++)
		{
		$nelem = split(";",$fcont[$i]);
		for ($ii=0; $ii<(count($nelem)-1); $ii++) 
			{
			$tyt = $elements[$ii];
			$tbl[$tyt][$i-1] = $nelem[$ii]; 
			}
		}

	return $tbl;
}

//---------------------------------------------------- Filter functions required to select_pb and delete_pb-------
function functionx($var,$func)
	{
	eval("\$ind = $func ;");
	return ($ind);
	}

function array_filter_it($array, $callback,$func) 
{ 
$farray = array (); 
while(list($key,$val) = each($array)) 
if ($callback($val,$func))  $farray[$key] = $val; 
return $farray; 
}

function advfiltr($tbl,$clmn,$wrnk)
{
$wrnk = split(" AND ", $wrnk); 
$clmn = split(" AND ", $clmn);
$ile = count($clmn);
for($i=0; $i < $ile; $i++)
	{
	$indx = array_filter_it($tbl[$clmn[$i]],"functionx", $wrnk[$i] );
	if($i>0) $ind = array_intersect($ind,array_keys($indx));
	else $ind=array_keys($indx);
	}
$ind=array_flip($ind);
return $ind;
}
//---------------------------------------------------------------SELECT--------------------------------------------------------------

function select_pb ($tbl, $clm,$func,$sort = "0",$jak="0" )
{
if(count_pb($tbl) == 0 ) return(0);
If ($sort == "0" ) $sort= $clm;
If ($jak == "0" ) $jak= "ASC";
$jak=strtoupper($jak);

$func = eregi_replace("x ==","\$var ==",$func);

if(!eregi(" AND ",$clm)) $indx = array_filter_it($tbl[$clm],"functionx", $func );
else $indx = advfiltr($tbl,$clm,$func);

$keye_indx = array_keys($indx);
$elements = array_keys($tbl);

$ile_ind = count($indx);
$ile_tyt = count($tbl);

$toco ="\$zwrot[$sort],SORT_$jak,";

for($i = 0; $i < $ile_tyt; $i++ )
	{
	$ind = $elements[$i];
	if($ind != $sort)	$toco = "$toco \$zwrot[$ind],";
	for ($ii = 0; $ii < $ile_ind; $ii++)
		{
		$indw = $keye_indx[$ii];
		$zwrot[$ind][$ii] = $tbl[$ind][$indw];
		}
	}

$toco = "$toco SORT_ASC"; 
if(($ile_ind)>0) eval("array_multisort ($toco);");

return $zwrot;
}

//--------------------------------------Count rows (required to delete_pb update_pb and select_pb ---------------------
function count_pb ($indx)
{
if($indx!=0) $keye_indx = array_keys($indx);

$key = $keye_indx[0];
$ile = count($indx[$key]);
return ($ile);
}

//--------------------------------------------------------INSERT---------------------------------------------------------------
function insert_pb ($fcont,$string)
{
$file = fopen ($fcont,"a");
$string = ereg_replace("(\r)|(\n)","",$string);
fputs ($file,"\r\n$string",(strlen($string)+2));
fclose($file);
}

//--------------------------------------------------File operations required to update_pb and delete_pb--------
function writefcont($fcont,$tbl)
{
$elements = array_keys($tbl);
$ile_tyt = count($elements);
$ile_wsz = count($tbl[$elements[0]]);

$file = fopen ($fcont,"w");

for($ii=$ile_tyt-1;$ii>=0;$ii--)
	{
	$ind = $elements[$ii];
	$wart = $ind;
	$toco = "$wart;$toco";
	}
$toco = ereg_replace("(\r)|(\n)","",$toco);
fputs ($file,$toco,(strlen($toco)));

for($i=0;$i < $ile_wsz; $i++)
	{
	$toco ="";
	for($ii=$ile_tyt-1;$ii>= 0;$ii--)
		{
		$ind = $elements[$ii];
		$wart = $tbl[$ind][$i];
		$toco = "$wart;$toco";
		}
	$toco = ereg_replace("(\r)|(\n)","",$toco);
	$toco ="\r\n$toco"; 
	fputs ($file,$toco,(strlen($toco)));
	}
fclose($file);
}

//---------------------------------------------------------UPDATE-------------------------------------------------------------

function update_pb ($dbnm,$clm,$func, $key, $wartosc)
{
$tbl = connect_pb($dbnm);
if(count_pb($tbl) == 0 ) return(0);

$func = eregi_replace("x ==","\$var ==",$func);

if(!eregi(" AND ",$clm)) $indx = array_filter_it($tbl[$clm],"functionx", $func );
else $indx = advfiltr($tbl,$clm,$func);

$keye_indx = array_keys($indx);
$ile_ind = count($indx);

for ($ii = 0; $ii < $ile_ind;$ii++)
	{
	$indw = $keye_indx[$ii];
	$tbl[$key][$indw]=$wartosc; 
	}
writefcont($dbnm,$tbl);
}

//------------------------------------------------------------DELETE------------------------------------------------------------
function delete_pb ($dbnm,$clm,$func)
{
$tbl = connect_pb($dbnm);
if(count_pb($tbl) == 0 ) return(0);

$func = eregi_replace("x ==","\$var ==",$func);

if(!eregi(" AND ",$clm)) $indx = array_filter_it($tbl[$clm],"functionx", $func );
else $indx = advfiltr($tbl,$clm,$func);

$ind = array_keys($indx);
$tab = array_keys($tbl[$clm]);
$indx = array_diff($tab,$ind);
$indx = array_flip($indx);
$indx = array_flip($indx);

$keye_indx = array_keys($indx);
$elements = array_keys($tbl);

$ile_tyt = count($elements);
$ile_ind = count($indx);

if($ile_ind != 0)
	{
	for($i = 0; $i < $ile_tyt; $i++ )
		{
		$ind = $elements[$i];
		for ($ii = 0; $ii < $ile_ind; $ii++)
			{
			$indw = $keye_indx[$ii];
			$zwrot[$ind][$ii] = $tbl[$ind][$indw];
			}
		}
	writefcont($dbnm,$zwrot);
	}	
else
	{
	$file = file($dbnm); 
	for($i = 0; $i < count($elements); $i++) $string .= $elements[$i].";";
	$f = fopen($dbnm,"w");
	fwrite($f, $string);
	}
}


?>