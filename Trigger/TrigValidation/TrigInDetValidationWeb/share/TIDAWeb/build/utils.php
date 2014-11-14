<?php

/// is this a low pt page?
$lowpt=$_GET['lowpt'];
$object_type=$_GET['type']; 

  
function curPageURL() {
  $pageURL = 'http';
  if ($_SERVER["HTTPS"] == "on") {$pageURL .= "s";}
  $pageURL .= "://";
  //  $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
  $server = $_SERVER["HTTP_X_SERVER_HOST"];
  if ( "$server" === "" ) { 
    /// if not forwarded
    $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
  }
  else {
    $pageURL .= $server.$_SERVER["REQUEST_URI"];
  }


  return $pageURL;
}


function findPdfPngFileRoots($files) {
  $fileroots = array();
  foreach ($files as &$pngfile) {
    if (substr($pngfile, -4) === ".png") {
      $plotfile = substr($pngfile, 0, strlen($pngfile)-4);
      foreach ($files as &$pdffile) {
        if ($pdffile === "$plotfile.pdf") {
          $fileroots[] = $plotfile;
        }
      }
    }
  }
  sort($fileroots);
  return $fileroots;
}



function is_dir_empty($dir) {
  if (!is_readable($dir)) return NULL; 
  return ( count(scandir($dir)) == 2 );
  $handle = opendir($dir);
  while (false !== ($entry = readdir($handle))) {
    if ($entry != "." && $entry != "..") {
      return FALSE;
    }
  }
  return TRUE;
}


function lowptLink( $directory, $lowpt ) {
//  if ( $object_type != "" ) $directory = "$directory-$object_type"; 
  if ( file_exists ( $directory ) ) { 
     if ( !is_dir_empty( $directory ) ) { 
       if ( $lowpt != "true" ) return " - high pt <a href=\"$rawpage?lowpt=true\"> (switch to low pt) </a>\n";
       else                    return " - low pt  <a href=\"$rawpage?lowpt=false\"> (switch to high pt) </a>\n";  
     }
  }
}

function pageTitle( $branch, $release, $project ) { 
   print("<div id=\"header\">\n");
   print("  <div id=\"logo\">\n");
   print("    <h1>  id rtt : $branch-$release $project - " ); 
   print(       str_replace( "_", "  ", str_replace( "TrigInDetValidation_", "", basename(dirname(getcwd()))) ) );
   print("    </h1>\n");
   print("  </div>\n");
   print("</div>\n");  
}



function tableTitle( $label, $title, $extra="" ) {
  global $object_type;
  // print("<tr>\n<a id=\"$label\"></a>\n");
  print("<tr>\n");
  if ( $object_type == "" ) print("<td colspan=\"3\"><span class=\"tablaTitle\">$title $extra</span></td>\n");
  else                      print("<td colspan=\"3\"><span class=\"tablaTitle\">$title $extra ($object_type)</span></td>\n"); 
  print("</tr>\n");
}


function tableImageEntry( $href, $alt ) { 
  print("<td><a href=\"$href.pdf\"><img src=\"$href.png\" alt=\"$alt\" height=\"325\" /></a></td>\n");
} 


function tableRawEntry( $href ) { 
  $raw=basename($href);
  print("<td align=\"center\"><a href=\"$href.pdf\"> $raw.pdf </a><br /><br/></td>\n");
} 


$found_plots = TRUE;

$plot_directory = "";


function tableRow( $hdir, $hrefa, $alta, $hrefb, $altb ) {

   global $found_plots;
   global $plot_directory;
   global $object_type;

   if ( !$found_plots ) return;

   $plot_directory = $hdir;	 

   if ( $hdir == ".." ) { 
      /// if directory is "..", then all *-plots* 
      /// directories must be empty
      print( "<tr><td> no plots found </td></tr><br/>\n" );
      $found_plots = FALSE;
      return;
   }
 
   if ( $object_type != "" ) $hdir = "$hdir-$object_type";

   //      print( "tableRow type : $object_type :  $hdir : <br/>\n"); 

   print("<tr>\n");
   tableImageEntry( "$hdir/$hrefa", $alta );  print("<td></td>\n"); // print("<td width=\"30\"></td>\n");		  		  
   tableImageEntry( "$hdir/$hrefb", $altb );
   print("</tr>\n");
   print("<tr>\n");
   tableRawEntry( "$hdir/$hrefa" );  print("<td></td>\n"); //  print("<td width=\"30\"></td>\n");		  
   tableRawEntry( "$hdir/$hrefb" );
   print("</tr>\n");
   
}


function tableRowSingle( $hdir, $hrefa ) {
  print("<tr>\n");
    tableImageEntry( "$hdir/$hrefa", $alta );  print("<td></td>\n");		  
  print("</tr>\n");
  print("<tr>\n");
    tableRawEntry( "$hdir/$hrefa" );  print("<td></td>\n");		  
  print("</tr>\n");
}






/// http://localhost/~sutt/prod/rtt/rel_2/devval/build/x86_64-slc6-gcc47-opt/offline/TrigInDetValidation/TrigInDetValidation_el_Zee_pu46_run2_oldSeeding/TIDAWeb/




/// useful navigation stuff  
$rawpage     = curPageURL();

$tags = explode("/", curPageURL() );
$N = count( $tags );

$page       = $tags[$N-1];
$short_page = str_replace( "?lowpt=true", "", str_replace( "?lowpt=false", "", str_replace( "?type=tau", "", str_replace( "?type=muon", "", str_replace( "?type=electron", "", str_replace( "?type=bjet", "", $page ) ) ) ) ) );
$arch       = $tags[$N-6];
$branch     = $tags[$N-8];

// print( "tags: $N<br />" );
// for ( $i=0 ; $i<$N ; $i++ )  print( "_____ $i $tags[$i]<br />" );

$job_p     = $tags[$N-3];
$package_p = $tags[$N-4];
$project_p = $tags[$N-5];
$arch_p    = $tags[$N-6];
$branch_p  = $tags[$N-8];
$release   = $tags[$N-9];

$rtt_base = "";
$big_tags = explode( "prod/rtt", curPageURL() );
for ( $idir=0 ; $idir<9 ; $idir++ ) { 
  // $rtt_base = "$big_tags[0]/prod/rtt";
  $rtt_base += "../";
} 

// print( "_____ page:    $page    $page_p <br />" );
// print( "_____ job:     $job     $job_p <br />" );
// print( "_____ package: $package $package_p <br />" );
// print( "_____ branch:  $branch  $branch_p <br />" );
// print( "_____ release: $release $release_p <br />" );

// print( "rawpage: $rawpage<br />" );

if ( $page == "" ) $job_dir = dirname($rawpage);            // directory for this job
else               $job_dir = dirname(dirname($rawpage));   // directory for this job         

$job         = basename($job_dir);       // name of this job

$package_dir = dirname($job_dir);        // package - ie TrigInDetAnalysis root directory   
$package     = basename($package_dir);   // ie TriginDetAnalysis

$project_dir = dirname($package_dir);    // project, ie offline, P1HLT etc  
$project     = basename($project_dir);


$release_dir = dirname(dirname(dirname(dirname($project_dir))));
$release     = basename($release_dir);

$rtt_base    = dirname($release_dir);

/// $rtt_base = dirname(dirname(dirname(dirname(dirname($project_dir)))));

if ( $project === "offline" ) $real_project="AtlasProduction";
else                          $real_project=$project;

/// put in here some base page that can be easily changed to be
/// an RTT page, or a local page ... 

/// if we want to redirect to the plots with some url instead of 
/// absolute paths, then if the files and directories are still
/// on the same filesystem we can use the absolute file access 
/// to test whether the files or directories exist
/// and a corresponding url for the hrefs  
// $real_base    = "https://atlas-rtt.cern.ch/prod/rtt/$release/$branch/build/$arch/$project/$package/$job";
// $real_dirbase = "..";

// $real_base    = "http://localhost/~sutt/RTT";
// $real_dirbase = "/Users/sutt/Sites/RTT";

/// otherwise simply use absolute paths
$real_base    = "..";
$real_dirbase = "..";
  
/// rtt base pages for this release, of this project
$job_base      = "https://atlas-rtt.cern.ch/index.php?q=%28release=$release;branch=$branch;cmtconfig=$arch;project=$real_project;packagename=$package;jobname=$job%29&amp;page=job";
$base_page     = "https://atlas-rtt.cern.ch/index.php?q=%28release=$release;branch=$branch;cmtconfig=$arch;project=$real_project;packagename=$package;verbosity=vvv;viewby=packagename%29";
$big_base_page = "https://atlas-rtt.cern.ch/index.php?q=%28release=;branch=$branch;cmtconfig=$arch;project=$real_project;packagename=$package;verbosity=vvv;viewby=packagename%29";

/// base directories for the different plots 
$efeff_dir  = $real_base;  
$l2eff_dir  = $real_base;  
$efres_dir  = $real_base;  
$l2res_dir  = $real_base;  
$efdiff_dir = $real_base;  
$l2diff_dir = $real_base;  
$time_dir   = "$real_base/times";


if ( file_exists( "$real_dirbase/HLTEF-plots" ) && !is_dir_empty( "$real_dirbase/HLTEF-plots" ) ) { 
    $efeff_dir  = "$efeff_dir/HLTEF-plots";
    $efres_dir  = "$efres_dir/HLTEF-plots";
    $efdiff_dir = "$efdiff_dir/HLTEF-plots";
}
else { 
  if ( file_exists( "$real_dirbase/EF-plots" ) && !is_dir_empty( "$real_dirbase/EF-plots" ) ) { 
     $efeff_dir  = "$efeff_dir/EF-plots";
     $efres_dir  = "$efres_dir/EF-plots";
     $efdiff_dir = "$efdiff_dir/EF-plots";

  }
}


if ( file_exists( "$real_dirbase/HLTL2-plots" ) && !is_dir_empty( "$real_dirbase/HLTL2-plots" ) ) { 
     $l2eff_dir  = "$l2eff_dir/HLTL2-plots";
     $l2res_dir  = "$l2res_dir/HLTL2-plots";
     $l2diff_dir = "$l2diff_dir/HLTL2-plots";
}
else { 
  if ( file_exists( "$real_dirbase/L2-plots" ) && !is_dir_empty( "$real_dirbase/L2-plots" ) ) { 
      $l2eff_dir  = "$l2eff_dir/L2-plots";
      $l2res_dir  = "$l2res_dir/L2-plots";
      $l2diff_dir = "$l2diff_dir/L2-plots";
  }
}

$efeff_dir_lowpt  = "$efeff_dir-lowpt";
$efres_dir_lowpt  = "$efres_dir-lowpt";
$efdiff_dir_lowpt = "$efdiff_dir-lowpt";
$l2eff_dir_lowpt  = "$l2eff_dir-lowpt";
$l2res_dir_lowpt  = "$l2res_dir-lowpt";
$l2diff_dir_lowpt = "$l2diff_dir-lowpt";

if ( $lowpt == "true" ) { 
    $efeff_dir  = $efeff_dir_lowpt;
    $efres_dir  = $efres_dir_lowpt;
    $efdiff_dir = $efdiff_dir_lowpt;
    $l2eff_dir  = $l2eff_dir_lowpt;
    $l2res_dir  = $l2res_dir_lowpt;
    $l2diff_dir = $l2diff_dir_lowpt;
}


/**
print( "ef_eff_dir:   $efeff_dir <br />\n" );
print( "ef_res_dir:   $efres_dir <br />\n" );
print( "ef_diff_dir:  $efdiff_dir <br />\n" );

print( "l2_eff_dir:   $l2eff_dir <br />\n" );
print( "l2_res_dir:   $l2res_dir <br />\n" );
print( "l2_diff_dir:  $l2diff_dir <br />\n" );
**/


function checkPlotType() { 
   global $plot_directory;
   global $short_page;
   global $object_type;

   $object_types = array( "electron", "muon", "tau", "bjet" );

   $first = TRUE;
   foreach ($object_types as &$otype) {
      if ( file_exists( "$plot_directory-$otype" )  ) { 
            if ( $first ) print("<li><span>\n");
	    $first = FALSE;
	    if ( $otype === $object_type ) print ( "$otype\n");
            else {
               if ( $lowpt == "" ) print ( "<a href=\"$short_page?type=$otype\">$otype</a>\n");
               else                print ( "<a href=\"$short_page?lowpt=$lowpt&type=$otype\">$otype</a>\n");
            }
      }	       
   }
   if ( !$first ) print("</span></li>\n");
}


?>



