<!-- emacs: this is -*- html -*- -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<!--
Based on a design by Free CSS Templates
http://www.freecsstemplates.org
Released for free under a Creative Commons Attribution 2.5 License
-->
<html xmlns="http://www.w3.org/1999/xhtml">
<!-- Utilities -->
<?php include_once "build/utils.php"; ?>
<!-- Header -->
<?php include_once "build/header.php"; ?>
<body>
<?php pageTitle( $branch, $release, $project ); ?>

<!-- Menubar -->
<?php include "build/menu.php"; ?>
<!-- Content -->
<table class="tabla" style="margin: 0px 0 0 0; padding: 0px 0 0 0;" cellpadding="0px" border="0" frame="void" rules="none" >
<tr class="tabla" style="margin: 0px 0 0 0; padding: 0px 0 0 0" > 
<td id="content">

<br />


<!-- <h2>full tests for  <?php print( "$branch-$release  $project" ) ?> </h2> -->

<?php 

     $job_types = array( "bjet_", "el_", "mu_", "tau_", "beamspot_", "minBias", "cosmic" );

     echo "<table>\n";
     tableTitle("", "&nbsp;full tests for $branch-$release  $project" );
     echo "<tr>\n";
     echo "<td>\n";
     echo "<ul>\n";

     $dirs = scandir("../../");
 
     ///   $first_letters = "";

     $counter = 0;

     foreach ($job_types as &$job_type) { 

      foreach ($dirs as &$dir) { 
  
        if ( strpos($dir,$job_type) != true  ) continue;
 
        if ( $dir != "." && $dir != ".." ) {  
          // $long_dir = "$package_dir/$dir/TIDAWeb";
          $long_dir = "../../$dir/TIDAWeb";
          $shortish_jobname = str_replace( "TrigInDetValidation_", "", $dir );
          $short_jobname    = str_replace( "TrigInDetValidationLight_", "", $shortish_jobname );

          //  $new_letters = substr( $short_jobname, 0, 2 );

          //  if ( $first_letters != "" && $new_letters != $first_letters ) { 
          //    echo "<br />\n";
          //    $counter++;
          //  }

          //  $first_letters = $new_letters;

          if ( $counter >= 14 ) { 
            echo "</ul></td><td>\n<ul>\n";
            $counter = 0;  
          }

          if ( file_exists( "../../$dir/TIDAWeb" ) ) { 
             // print( "\t<li><a href=\"$long_dir/$page\"><span>  $short_jobname </span></a> </li>\n" );
             print( "\t<li><a href=\"$long_dir/$page\"><span>  $short_jobname </span></a> </li>\n" );
          }
          else { 
             print( "\t<li><span>  $short_jobname </span></li>\n" );
          }

          $counter++;
        }

       }
          
       print( "\t<li style=\"list-style: none\">&nbsp;</li>\n");   //    echo "<br />\n";
//     print( "\t<li style=\"margin-bottom:-3em\">&nbsp;</li>\n");   //    echo "<br />\n";
       $counter++;
     }
 
     echo "</ul>\n";
     echo "</td>\n";
     echo "</tr>\n";
     echo "</table>\n";

?>


<br />
<?php include "build/releases.php"; ?>




</td>
<!-- Sidebar -->
<?php include "build/indexsidebar.php"; ?>
</tr>
</table>
<!--</tr>
<tr>-->
<?php include "build/footer.php"; ?>
<!--</tr>
</table>-->	
</body>
</html>
