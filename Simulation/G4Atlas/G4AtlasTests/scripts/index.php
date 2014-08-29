<?php
?>
<html  xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <meta http-equiv="Content-type" content="text/html;charset=UTF-8" />
    <meta http-equiv="Content-Script-Type" content="text/javascript" />
    <meta http-equiv="Content-Style-Type" content="text/css" />
    <title>List of profiling runs in RTT</title>
    <style type="text/css">
   th {
          background-color:#aaf;   
 }
        td {
        text-align: left;
        padding: 5px;
        }
        tr > td td td td td td {
        text-align: left ;
        padding: 5px;
        }
        tr:nth-of-type(odd) {
          background-color:#ccc;
          }
          
    </style>
  </head>
<body>
<h1>Profiling run</h1>
<h2>Bookmarks</h2>
<ul>
    <li><a href="?filter=runs">select run</a></li>
    <li><a href="?filter=machine">select execution host</a></li>
	  <li><a href="?filter=global&rid=39">AtlasG4_muons (dev, rel_1,2012-05-07)</a></li>
	  <li><a href="?filter=global&rid=40">AtlasG4_electrons (devval, rel_1,2012-05-07)</a></li>
	  <li><a href="?filter=global&rid=41">AtlasG4_pions (dev, rel_1,2012-05-07)</a></li>
	  <li><a href="?filter=global&rid=36">TTbarSim (devval, rel_1,2012-05-07)</a></li>
	  <li><a href="?filter=fn&fn=LArWheelSolid::DistanceToOut&rid=36">LArWheelSolid::DistanceToOut (TTbarSim, devval, rel_1,2012-05-07)</a></li>
</ul>
<?php
    function __autoload($class_name) {
      include $class_name . '.php';
    }

$runs=false;
$global=false;
$funsel=false;
$machine=false;
$rid=0;
$extrauri='';

$filter='';
$fn='LArWheelSolid::DistanceToOut';
if (isset($_GET["rid"])) 
  $rid=$_GET["rid"];

if (isset($_GET["filter"]) ) {
  $filter=htmlentities($_GET["filter"]);
  //  echo "select filter =".$filter;
  if ($filter=="fn") {
    $fn=$_GET["fn"];
    if ($rid==0)
      echo "Error: no run id provided<br/>";
    $funsel=true;
  }
  elseif ($filter=="runs") {
    $runs=true;
  }
  elseif ($filter=="global") {
    $global=true;
  }
  elseif ($filter=="machine") {
    $machine=true;
  }
}
else {
  $runs=true;
}

    class ProfDataBase 
    {
      public $db_name="sqlite:/afs/cern.ch/atlas/groups/Simulation/rtt/rttProfile.sqlite";
      //public $db_name="sqlite:/Users/dreas/atlas/rtt/test.sqlite";
      //public $db_name="sqlite:/Users/dreas/Sites/prof/rttProfile.sqlite";
      public $db_initialized=false;
      public $db_error="";

      public function init()
      {
	try {
	  /*** connect to SQLite database ***/
	  $this->db = new PDO($this->db_name);
	  $db_initialized=true;
	  //	  echo "connected to database.";
	}
	catch(PDOException $e) {
	  $this->db_error= $e->getMessage();
	  echo $this->db_error;
	}
	
      }

      /* print an array $result as web table, normalize costs with $norm, and use $rid for links needing a run id */
      public function printTable($result,$norm,$rid)
      {
	$header = array();
	$sums = array();
	$table = array();
	for ($i = 0; $i < $result->columnCount(); $i++) {
	  $meta=$result->getColumnMeta($i);
	  $header[]=$meta['name'];
	  if (isset($meta['table']))
	    $table[]=$meta['table'];
	  else
	    $table[]='';
	  $sums[$i]=0;
	}
	//var_dump($table);
	//var_dump($header);
	//var_dump($sums);

	//now output the data to a simple html table...
	print "<table>";
	
	print "<tr>";
	foreach($header as $h) {
	  print "<th>".$h."</th>";
	}
	print "</tr>";

	global $filter,$extrauri;
	$lastrow=0;
	foreach($result as $row) {
	  $lastrow=$row;
	  print "<tr>";
	  foreach($header as $i=>$h) {
	    if (in_array($h,array('counts','selfcounts', 'totalcounts'))) {
	      printf("<td>%01.2f</td>",$row[$i]*$norm);
	    }
	    elseif ($h=="acc") {
	      $sums[$i]+=$row[$i];
	      printf("<td>%01.2f</td>",$sums[$i]*$norm);
	    }
	    elseif ($h=="fn_name") {
	      printf('<td><a href="?filter=fn&fn=%s&rid=%d%s">%s</a></td>',$row[$i],$rid,$extrauri,$row[$i]);

	    }
	    elseif ($h=="id" && ( $table[$i]=="t_run" || $norm==-1. ) ) {
	      printf('<td><a href="?filter=global&rid=%d%s">%d</a></td>',$row[$i],$extrauri,$row[$i]);
	    }
	    elseif ($h=="machine" || $h=="hostname") {
	      printf('<td><a href="?filter=runs&hostname=%s">%s</a></td>',$row[$i],$row[$i]);
	    }
	    elseif ($h=="model") {
	      printf('<td><a href="?filter=runs&model=%s">%s</a></td>',$row[$i],$row[$i]);
	    }
	    elseif ($h=="job") {
	      printf('<td><a href="?filter=runs&job=%s">%s</a></td>',$row[$i],$row[$i]);
	    }
	    elseif ($h=="package") {
	      printf('<td><a href="?filter=runs&package=%s">%s</a></td>',$row[$i],$row[$i]);
	    }
	    elseif ($h=="cpuf" || $h=="KSI2K_fac") {
	      printf('<td><a href="?filter=runs&cpuf=%s">%s</a></td>',$row[$i],$row[$i]);
	    }
	    else
		print "<td>".$row[$i]."</td>";
	  }
	  print "</tr>";
	}
	print "</table>";
	return $lastrow;
      }

      public function query($cmd)
      {
	try {
	  $this->result = $this->db->query($cmd);
	  return $this->result;
	}
	catch(PDOException $e) {
	  $this->db_error= $e->getMessage();
	  echo $this->db_error;
	}
      }

      public function prepare($cmd)
      {
	try {
	  //var_dump($this->db);
	  return $this->db->prepare($cmd);
	}
	catch(PDOException $e) {
	  $this->db_error= $e->getMessage();
	  echo $this->db_error;
	}
      }
    }

    ///  *** connect to SQLite database 
    //$db = new PDO("sqlite: ");
$db = new ProfDataBase();

if (isset($_GET["db"]) ) {
  $db->db_name="sqlite:".$_GET["db"];
  $extrauri='&db='.$_GET["db"];
  echo '<b>'.$extrauri.'</b>';
}

$db->init();
// echo "loading data ...<br/>";


if ($global) {
  if ($rid==0) {
    echo "Error: no run id provided<br/>";
    $rid=1;
  }
  echo "<h2>Global function list</h2>";
  $runs=true;
}

//echo "rid=".$rid."<br/>";

if ($runs || $rid!=0) {
  if ($rid!=0) {
    //var_dump($db->db);
    $sth=$db->prepare('SELECT * FROM t_run where id=?');
    $sth->execute(array($rid));
    //    $sth->fetchAll();
    $result = $sth; 
  }
  elseif (isset($_GET["model"])) {
    $result=$db->prepare('SELECT a.id,branch,rel,cmt,rdate,rtime,machine,KSI2K_fac,package,job FROM t_run a join t_hosts b on a.machine=b.hostname where b.model=? order by a.id desc');
    $result->execute(array($_GET["model"]));
    $machine=true;
  }
  elseif (isset($_GET["cpuf"])) {
    $result=$db->prepare('SELECT * FROM t_run where KSI2K_fac=? order by id desc');
    $result->execute(array($_GET["cpuf"]));
    $machine=true;
  }
  elseif (isset($_GET["hostname"])) {
    $result=$db->prepare('SELECT * FROM t_run where machine=? order by id desc');
    $result->execute(array($_GET["hostname"]));
    $machine=true;
  }
  elseif (isset($_GET["job"])) {
    $result=$db->prepare('SELECT * FROM t_run where job=?  order by id desc');
    $result->execute(array($_GET["job"]));
    //$machine=true;
  }
  elseif (isset($_GET["package"])) {
    $result=$db->prepare('SELECT * FROM t_run where package=?  order by id desc');
    $result->execute(array($_GET["package"]));
    //$machine=true;
  }
  else {
    echo "<h2>Select run</h2>";
    $result = $db->query('SELECT * FROM t_run  order by id desc');
  }
  echo "<h3>run</h3>";

  $db->printTable($result,-1.,$rid);
  $sth=0;
}

$total=100;

if ($global) {
  $result = $db->prepare('select sum(selfcounts) from t_counts where rid=?');
  $result->execute(array($rid));
  //  var_dump($result->fetchAll());
  $lastrow=$db->printTable($result,1.,$rid);
  $total=intval($lastrow[0]);
  //var_dump($total);

  if ($total!=0) {
  $result = $db->prepare('select b.id,selfcounts, selfcounts as acc, totalcounts, fn_name,lib FROM t_counts join t_functions b on t_counts.fid=b.id where rid=? order by selfcounts desc');  
// select round(selfcounts/1013.44,2) limit 200
  $result->execute(array($rid));

  
  $db->printTable($result,100./$total,$rid);
  }
  else {
    echo '<b>no results received</b>';
  }
}
else {
  //echo "<h3>total</h3>";
  $result = $db->prepare('select sum(selfcounts) from t_counts where rid=?');
  $result->execute(array($rid));
  //  var_dump($result->fetchAll());
  $rows=$result->fetchAll();
  $total=intval($rows[0][0]);
  //var_dump($total);

}

if ($funsel) {
  echo "<h3>function</h3>";

  $result = $db->prepare("select id,selfcounts, totalcounts,fn_name,lib from t_functions a join t_counts b on a.id=b.fid where b.rid=? and a.fn_name=?");
  $result->execute(array($rid,$fn));

  $lastrow=$db->printTable($result,100./$total,$rid);
  $fid=intval($lastrow['id']);
  //var_dump($lastrow);

  echo "<h3>calls</h3>";

  $result = $db->prepare('select b.counts,b.counts as acc, c.selfcounts,c.totalcounts,fn_name,lib from t_functions a join t_links b on a.id=b.cfunid join t_counts c on a.id=c.fid and c.rid=b.rid where b.rid=? and b.fid=? order by b.counts DESC');
  $result->execute(array($rid,$fid));

  $db->printTable($result,100./$total,$rid);

  echo "<h3>callers</h3>";
  
  $result = $db->prepare('select b.counts,b.counts as acc, c.selfcounts,c.totalcounts,fn_name,lib from t_functions a join t_links b on a.id=b.fid join t_counts c on a.id=c.fid and c.rid=b.rid where b.rid=? and b.cfunid=? order by b.counts DESC');
  $result->execute(array($rid,$fid));

  $db->printTable($result,100./$total,$rid);

}

if ($machine) {
  echo "<h3>machine</h3>";
  if (isset($_GET["hostname"])) {
    $result = $db->prepare("select * from t_hosts where hostname=?");
    $result->execute(array($_GET["hostname"]));
  }
  elseif (isset($_GET["model"])) {
    $result = $db->prepare("select * from t_hosts where model=? order by hostname");
    $result->execute(array($_GET["model"]));
  }
  elseif (isset($_GET["cpuf"])) {
    $result = $db->prepare("select * from t_hosts where cpuf=? order by hostname");
    $result->execute(array($_GET["cpuf"]));
  }
  else {
    $result = $db->query("select * from t_hosts");
  }

  $db->printTable($result,-2.,0);

}

  // close the database connection
  $db = NULL;

?>

</body>
</html>