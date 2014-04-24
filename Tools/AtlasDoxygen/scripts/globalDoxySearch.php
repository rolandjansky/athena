<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<title>Doxygen Global Search</title>
<script type="text/javascript">
function toggleBox(szDivID, iState) // 1 visible, 0 hidden
{
  var obj = document.layers ? document.layers[szDivID] :
    document.getElementById ?  document.getElementById(szDivID).style :
    document.all[szDivID].style;
  
  obj.visibility = document.layers ? ( (obj.visibility == "show") ? "hide":"show" ) : ( (obj.visibility == "visible") ? "hidden":"visible" );
}
</script>
<style type="text/css">
.hidden {color:#909090;
	 position:absolute;
	 top:35px;
	 left:73%;
	 width:300px;
	 height:100%;
	 z-index:99;
	 visibility:hidden;
	 font-size:11px;
}
.help {color:#909090;
       font-size:11px;
}

</style>

<link href="AtlasAnalysisRelease/html/doxygen.css" rel="stylesheet" type="text/css">
<link href="AtlasAnalysisRelease/html/tabs.css" rel="stylesheet" type="text/css">

</head><body>
<?php
echo "<p style='text-align: right;'><a href=\"#\" onclick=\"toggleBox('help',1);\"></a>
<small><FONT COLOR=BBBBBB><i>Search index generated on ".date("D M d H:i:s Y.",filemtime('globsearch.idx'))."</i></FONT></small></address>\n";
?>

<h1><FONT COLOR=#BB3333>Doxygen Global Search</FONT></h1>

<form action="globalDoxySearch.php" method="get">
  <table cellspacing="0" cellpadding="0" border="0" width=100%>
    <tr>
      <td width=20%></td>
      <td align=right><label><FONT SIZE=+1>&nbsp;S<u>e</u>arch&nbsp;for&nbsp;</FONT></label>
      </td>

<?php set_time_limit(0); 

function search_results()
{
  return "Search Results";
}

function matches_text($num)
{
  if ($num==0)
  {
    return "Sorry, no documents matching your query.";
  }
  else if ($num==1)
  {
    return "Found <b>1</b> document matching your query.";
  }
  else // $num>1
  {
    return "Found <b>$num</b> documents matching your query. Showing best matches first.";
  }
}

function report_matches()
{
  return "Matches: ";
}
function end_form($value,$case,$whole,$filters)
{
  $cacheck=$case?"checked":"";
  $wcheck=$whole?"checked":"";
  $pcheck=$filters["package"]?"checked":"";
  $ccheck=$filters["class"]?"checked":"";
  $mcheck=$filters["method"]?"checked":"";
  $tcheck=$filters["title"]?"checked":"";
  echo "<td width=340><input type=\"text\" name=\"query\" value=\"$value\" size=\"40\" accesskey=\"e\"/>
            <input type=submit accesskey=\"a\" value=\"Search\"> </td>
        <td width=15%><!--<B>Filters:</B><BR>-->
            <input type=checkbox name=fpackage accesskey=P $pcheck><u>P</u>ackage only<BR>
            <input type=checkbox name=fclass   accesskey=C $ccheck><u>C</u>lass only<BR>
            <input type=checkbox name=fmethod  accesskey=M $mcheck><u>M</u>ethod/Attribute only<BR>
            <input type=checkbox name=ftitle   accesskey=T $tcheck>Search <u>T</u>itles only<BR>
            <input type=checkbox name=matchwholeword accesskey=W $wcheck>Match <u>w</u>hole word
            <!--<input type=checkbox name=casesensitive accesskey= $cacheck><u>C</u>ase Sensitive-->
        </td>
        <td width=20%><table><tr>
            <td class=help>
            <LI>Search is <b>not</b> case sensitive.
            <LI>Wildecards/Regular Expressions <b>not allowed</b>.
            <LI>Multiple terms will search for either of the terms (OR).
            <LI>Search filters are cumulative (\"Package only\" & \"Class only\" will give both).
            </td>
            </tr></table>\n
        </td>
    </tr>
</table>\n      </form>\n


";
}

function readInt($file)
{
  $b1 = ord(fgetc($file)); $b2 = ord(fgetc($file));
  $b3 = ord(fgetc($file)); $b4 = ord(fgetc($file));
  return ($b1<<24)|($b2<<16)|($b3<<8)|$b4;
}

function readString($file)
{
  $result="";
  while (ord($c=fgetc($file))) $result.=$c;
  return $result;
}

function readHeader($file)
{
  $header =fgetc($file); $header.=fgetc($file);
  $header.=fgetc($file); $header.=fgetc($file);
  return $header;
}

function computeIndex($word)
{
  // Fast string hashing
  //$lword = strtolower($word);
  //$l = strlen($lword);
  //for ($i=0;$i<$l;$i++)
  //{
  //  $c = ord($lword{$i});
  //  $v = (($v & 0xfc00) ^ ($v << 6) ^ $c) & 0xffff;
  //}
  //return $v;

  // Simple hashing that allows for substring search
  if (strlen($word)<2) return -1;
  // high char of the index
  $hi = ord($word{0});
  if ($hi==0) return -1;
  // low char of the index
  $lo = ord($word{1});
  if ($lo==0) return -1;
  // return index
  return $hi*256+$lo;
}

function printIndex($file)
{
    $count=0;
    for ($index = 0; $index<256*256; $index++)
    {
        fseek($file, $index*4+4);
        $idx = readInt($file);

//        echo "<FONT COLOR=RED><B>index=</B>$index <B>idx</B>=$idx</FONT><BR>";
        if ($idx)
        {
            echo "<FONT COLOR=RED>idx=$idx</FONT><BR>";
            fseek($file,$idx);
            $w = readString($file);
            while ($w)
            {
                 $statIdx = readInt($file);
                 $statsList[$count++]=array(
					    "word"=>$word,
					    "match"=>$w,
					    "index"=>$statIdx,
					    "full"=>strlen($w)==strlen($word),
					    "docs"=>array()
					    );
                 echo "w=$w statIdex=$statIdx<BR>";
                 $w = readString($file);
            }
        }
    }

    for ($count=0;$count<sizeof($statsList);$count++)
    {
         $statInfo = &$statsList[$count];
         fseek($file,$statInfo["index"]); 
         $numDocs = readInt($file);
         $docInfo = array();
         echo "<FONT COLOR=GREEN>";
//         echo "<B>word=</B>",$statInfo["word"], " ";
         echo "<B>w=</B>", $statInfo["match"], " ";
         echo "<B>statIdex=</B>", $statInfo["index"], " ";
         echo "<B>numDocs=</B>$numDocs";
         echo "</FONT><BR>";
         // read docs info + occurrence frequency of the word
         for ($i=0;$i<$numDocs;$i++)
         {
              $docidx=readInt($file); 
              $freq=readInt($file);
              $docInfo[$i]=array("idx"  => $docidx,
                                 "freq" => $freq>>1,
                                 "rank" => 0.0,
                                 "hi"   => $freq&1
                                 );
         }
         // read name and url info for the doc
         echo "<TABLE BORDER=1>";
         for ($i=0;$i<$numDocs;$i++)
         {
             fseek($file,$docInfo[$i]["idx"]);
             $docInfo[$i]["name"]=readString($file);
             $docInfo[$i]["url"]=readString($file);
             echo "<TR>";
             echo "<TD><FONT COLOR=BLUE>", $docInfo[$i]["name"], "</FONT></TD>";
             echo "<TD><FONT COLOR=BLUE>", $docInfo[$i]["url"], "</FONT></TD>";
             echo "<TD><FONT COLOR=BLUE>", $docInfo[$i]["freq"], "</FONT></TD>";
             echo "<TD><FONT COLOR=BLUE>", $docInfo[$i]["hi"], "</FONT></TD>";
             echo "</TR>";
         }
         echo "</TABLE>";
         $statInfo["docs"]=$docInfo;
    }
}


function search($file,$word,$whole,&$statsList)
{
  $index = computeIndex($word);
  if ($index!=-1) // found a valid index
  {
    fseek($file,$index*4+4); // 4 bytes per entry, skip header
    $index = readInt($file);

    if ($index) // found words matching the hash key
    {
      $start=sizeof($statsList);
      $count=$start;
      fseek($file,$index);
      $w = readString($file);
      while ($w)
      {
        $statIdx = readInt($file);
//	$statIdxHex=dechex( $statIdx );
//        $match = preg_match("($word)",$w,$matches);
	$match=($whole)?($word==$w):($word==substr($w,0,strlen($word)));
        if ($match)
        { // found word that matches (as substring)
          $statsList[$count++]=array(
				     "word"=>$word,
				     "match"=>$w,
				     "index"=>$statIdx,
				     "full"=>strlen($w)==strlen($word),
				     "docs"=>array()
				     );
//              echo "<FONT SIZE=+2 COLOR=RED>FOUND MATCH!</FONT><BR>";
//              echo "w=$w statIdex=$statIdx (0x$statIdxHex)<BR>";
        }
        $w = readString($file);
      }
      $totalHi=0;
      $totalFreqHi=0;
      $totalFreqLo=0;
      for ($count=$start;$count<sizeof($statsList);$count++)
      {
        $statInfo = &$statsList[$count];
        $multiplier = 1;
        // whole word matches have a double weight
        if ($statInfo["full"]) $multiplier=2;
        fseek($file,$statInfo["index"]);
        $numDocs = readInt($file);
//        $numDocsHex=dechex($numDocs);
//        echo "numDocs=$numDocs ( 0x$numDocsHex )";
        $docInfo = array();
        // read docs info + occurrence frequency of the word
        for ($i=0;$i<$numDocs;$i++)
        {
          $idx=readInt($file);
          $freq=readInt($file);
          $docInfo[$i]=array("idx"  => $idx,
                             "freq" => $freq>>1,
                             "rank" => 0.0,
                             "hi"   => $freq&1
                            );
          if ($freq&1) // word occurs in high priority doc
          {
            $totalHi++;
            $totalFreqHi+=$freq*$multiplier;
          }
          else // word occurs in low priority doc
          {
            $totalFreqLo+=$freq*$multiplier;
          }
        }
        // read name and url info for the doc
        for ($i=0;$i<$numDocs;$i++)
        {
          fseek($file,$docInfo[$i]["idx"]);
          $docInfo[$i]["name"]=readString($file);
          $docInfo[$i]["url"]=readString($file);
        }
        $statInfo["docs"]=$docInfo;
      }
      $totalFreq=($totalHi+1)*$totalFreqLo + $totalFreqHi;
      for ($count=$start;$count<sizeof($statsList);$count++)
      {
        $statInfo = &$statsList[$count];
        $multiplier = 1;
        // whole word matches have a double weight
        if ($statInfo["full"]) $multiplier=2;
        for ($i=0;$i<sizeof($statInfo["docs"]);$i++)
        {
          $docInfo = &$statInfo["docs"];
          // compute frequency rank of the word in each doc
          $freq=$docInfo[$i]["freq"];
          if ($docInfo[$i]["hi"])
          {
            $statInfo["docs"][$i]["rank"]=
              (float)($freq*$multiplier+$totalFreqLo)/$totalFreq;
          }
          else
          {
            $statInfo["docs"][$i]["rank"]=
              (float)($freq*$multiplier)/$totalFreq;
          }
        }
      }
    }
  }
  return $statsList;
}

function combine_results($results,&$docs)
{
  foreach ($results as $wordInfo)
  {
    $docsList = &$wordInfo["docs"];
    foreach ($docsList as $di)
    {
      $key=$di["url"];
      $rank=$di["rank"];
      $freq=$di["freq"];
      if (in_array($key, array_keys($docs)))
      {
        $docs[$key]["rank"]+=$rank;
      }
      else
      {
        $docs[$key] = array("url"=>$key,
			    "name"=>$di["name"],
			    "rank"=>$rank,
			    );
      }
      $docs[$key]["words"][] = array(
				     "word"=>$wordInfo["word"],
				     "match"=>$wordInfo["match"],
				     "freq"=>$di["freq"],
				     "hi"=>$di["hi"]
				     );
    }
  }
  return $docs;
}

function filter_results($docs,&$requiredWords,&$forbiddenWords,&$filters)
{
  $filteredDocs=array();
  while (list ($key, $val) = each ($docs))
  {
    $words = &$docs[$key]["words"];
    $copy=1; // copy entry by default
    if (sizeof($requiredWords)>0)
    {
      foreach ($requiredWords as $reqWord)
      {
        $found=0;
        foreach ($words as $wordInfo)
        {
          $found = $wordInfo["word"]==$reqWord;
          if ($found) break;
        }
        if (!$found)
        {
          $copy=0; // document contains none of the required words
          break;
        }
      }
    }
    if (sizeof($forbiddenWords)>0)
    {
      foreach ($words as $wordInfo)
      {
        if (in_array($wordInfo["word"],$forbiddenWords))
        {
          $copy=0; // document contains a forbidden word
          break;
        }
      }
    }
    //// THIS PART ADDED 20080311 :
    $copy2=1; //Copy by default unless a filter is active
    if ($filters["package"] || $filters["class"] || $filters["method"] || $filters["title"])
    {
      $copy2= 0;
      $url  = &$docs[$key]["url"]; //The url for this document
      $name = strtolower($docs[$key]["name"]); //The name of this url
      $word = &$docs[$key]["words"][0]["word"]; //The word that was searched for
      if ($filters["package"] && preg_match("/\/html\/index\.html$/",$url))
	$copy2=1; // document is a package
      //if ($filters["class"] && ! preg_match("/Class Reference$/",$name))
      if ($filters["class"] && preg_match("/\html\/class.*\.html$/",$url))
 	$copy2=1; //document is a class
      if ($filters["method"] && strpos($name,"::".$word)!==FALSE)//!== is the right form
 	$copy2=1; //document is a method or attribute
      if ($filters["title"] && strpos($name,$word)!==FALSE)//!== is the right form
 	$copy2=1; //document is a method or attribute
    }
    //// END ADDED PART
    if ($copy && $copy2) $filteredDocs[$key]=$docs[$key];
  }
  return $filteredDocs;
}

function compare_rank($a,$b)
{
  if ($a["rank"] == $b["rank"])
  {
    return 0;
  }
  return ($a["rank"]>$b["rank"]) ? -1 : 1;
}

function sort_results($docs,&$sorted)
{
  $sorted = $docs;
  usort($sorted,"compare_rank");
  return $sorted;
}

function report_results(&$docs, $time)
{
  $runPath=getcwd();
  $initURL=(strpos($runPath,"/nir/"))?"http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDocDirectory/":"";
  echo "<TABLE BORDER=0 cellpadding=0 cellspacing=0 width=100% STYLE=\"border-top:1px solid #36c;background:#d5ddf3;color:#000;padding:5px 1px 4px\">";
  echo "<TR>\n";
  echo "  <TD><H2>".search_results()."</td>\n";
  printf("  <TD ALIGN=RIGHT NOWRAP>%1.2f Seconds</TD>\n",$time);
  echo "</TR></TABLE>\n";

  echo "<table cellspacing=\"2\">\n";
//  echo "  <tr>\n";
//  echo "    <td colspan=\"2\"><h2>".search_results()."</h2></td>\n";
//  echo "  </tr>\n";
  $numDocs = sizeof($docs);
  if ($numDocs==0)
  {
    echo "  <tr>\n";
    echo "    <td colspan=\"2\">".matches_text(0)."</td>\n";
    echo "  </tr>\n";
  }
  else
  {
    echo "  <tr>\n";
    echo "    <td colspan=\"2\">".matches_text($numDocs);
    echo "\n";
    echo "    </td>\n";
    echo "  </tr>\n";
    $num=1;
    foreach ($docs as $doc)
    {
      if (!strcmp($doc["name"],"Page index")){//Add the package name to any URL whose name is "Page index"
        $pos = strpos($doc["url"],"/",0);
        $package_name = substr($doc["url"],0,$pos);
        $doc["name"]="$package_name " . $doc["name"];
      }
      echo "  <tr>\n";
      echo "    <td align=\"right\">$num.</td>";
      echo     "<td><a class=\"el\" href=\"".$initURL.$doc["url"]."\">".$doc["name"]."</a></td>\n";
      echo "  <tr>\n";
      echo "    <td></td><td class=\"tiny\">".report_matches()." ";
      foreach ($doc["words"] as $wordInfo)
      {
        $word = $wordInfo["word"];
        $matchRight = substr($wordInfo["match"],strlen($word));
        echo "<b><font color=red>$word</font></b>$matchRight(" . $wordInfo["freq"];
	if ($wordInfo["hi"]) echo "<font color=blue><b>&diams;</b></font>";
	echo ")\n";
	  //        break; //Check why so many identical words!
      }
      echo "    </td>\n";
      echo "  </tr>\n";
      $num++;
    }
  }
  echo "</table>\n";
}

function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}


function main()
{
  if(strcmp('4.1.0', phpversion()) > 0) 
  {
    die("Error: PHP version 4.1.0 or above required!");
  }
  if (!($file=fopen("globsearch.idx","rb"))) 
  {
    die("Error: Search index file could NOT be opened!");
  }
  if (readHeader($file)!="DOXS")
  {
    die("Error: Header of index file is invalid!");
  }
  $query="";
  $queryExists=0;
  $casesensitive=0;
  $matchwholeword=0;
  $filters=array(
		 "package"=>0,
		 "class"=>0,
		 "method"=>0,
		 "title"=>0,
		 );
  
  if (array_key_exists("query", $_GET))
  {
    $query=$_GET["query"];
    $queryExists=1;
  }
  if (array_key_exists("matchwholeword", $_GET)){
    $matchwholeword=$_GET["matchwholeword"];
  }
  if (array_key_exists("fpackage", $_GET)){
    $filters["package"]=$_GET["fpackage"];
  }
  if (array_key_exists("fclass", $_GET)){
    $filters["class"]=$_GET["fclass"];
  }
  if (array_key_exists("fmethod", $_GET)){
    $filters["method"]=$_GET["fmethod"];
  }
  if (array_key_exists("ftitle", $_GET)){
    $filters["title"]=$_GET["ftitle"];
  }
  end_form($query,$casesensitive,$matchwholeword,$filters);
  echo "&nbsp;\n<div class=\"searchresults\">\n";
  $results = array();
  $requiredWords = array();
  $forbiddenWords = array();
  $foundWords = array();
  $word=strtok($query," ");
  // printIndex($file);
  $time_begin = microtime_float();
  while ($word) // for each word in the search query
  {
    if (($word{0}=='+')) { $word=substr($word,1); $requiredWords[]=$word; }
    if (($word{0}=='-')) { $word=substr($word,1); $forbiddenWords[]=$word; }
    if (!in_array($word,$foundWords))
    {
      $foundWords[]=$word;
      $searchword = ($casesensitive)?$word:strtolower($word);
      search($file,$searchword,$matchwholeword,$results);
    }
    $word=strtok(" ");
  }
  $docs = array();
  combine_results($results,$docs);
  // filter out documents with forbidden word or that do not contain
  // required words
  $filteredDocs = filter_results($docs,$requiredWords,$forbiddenWords,$filters);
  // sort the results based on rank
  $sorted = array();
  sort_results($filteredDocs,$sorted);
  // report results to the user
  $time_end = microtime_float();
  $time_exec = $time_end - $time_begin;
  if ($queryExists){
    report_results($sorted,$time_exec);
  }
  echo "</div>\n";
  fclose($file);
}

main();


?>
</body>
</html>
