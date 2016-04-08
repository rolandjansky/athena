<!-- emacs: this is -*- html -*- -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<!--
Based on a design by Free CSS Templates
http://www.freecsstemplates.org
Released for free under a Creative Commons Attribution 2.5 License
-->
<html xmlns="http://www.w3.org/1999/xhtml">
<!-- Utilities -->
<?php include "build/utils.php"; ?>
<!-- Header -->
<?php include "build/header.php"; ?>
<body>
<!-- Title -->
<?php pageTitle( $branch, $release, $project ); ?>
<!-- Menubar -->
<?php include "build/menu.php"; ?>
<!-- Content -->
<table class="tabla" style="margin: 0px 0 0 0; padding: 0px 0 0 0;" cellpadding="0px" border="0" frame="void" rules="none" >
<tr class="tabla" style="margin: 0px 0 0 0; padding: 0px 0 0 0" >
<td id="content">

<br /> 
<?php 
  print("<table>\n");  
  tableTitle( "L2diff", "Differential Resolutions: Level 2", lowptLink( $l2diff_dir_lowpt, $lowpt ) );
  tableRow( $l2diff_dir, HLT_reta_vs_eta_sigma, "L2 deta/deta",   HLT_reta_vs_pt_sigma, "L2 deta/dpt"); 
  tableRow( $l2diff_dir, HLT_ript_vs_pt_sigma,  "L2 d(1/pt)/dpt", HLT_rzed_vs_pt_sigma, "L2 dz/dpt"); 

  tableRow( $l2diff_dir, HLT_rzed_vs_eta_sigma, "L2 dz/deta",  HLT_rzed_vs_pt_sigma, "L2 dz/d|pt|"); 
  tableRow( $l2diff_dir, HLT_rd0_vs_eta_sigma,  "L2 dd0/deta", HLT_rd0_vs_pt_sigma,  "L2 dd0/d|pt|"); 

  tableRow( $l2diff_dir, HLT_rd0_vs_signed_pt_sigma,  "L2 dd0/d|pt|", HLT_rzed_vs_signed_pt_sigma,  "L2 dz/d|pt|"); 
  print("</table>\n");  
?>

</td>
<!-- Sidebar -->
<?php include "build/sidebar.php"; ?>
</tr>
</table>
<!--</tr>
<tr>-->
<?php include "build/footer.php"; ?>
<!--</tr>
</table>-->	
</body>
</html>
