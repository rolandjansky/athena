<!-- emacs: this is -*- html -*- -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<!--
Based on a design by Free CSS Templates
http://www.freecsstemplates.org
Released for free under a Creative Commons Attribution 2.5 License
-->
<html xmlns="http://www.w3.org/1999/xhtml">
<!-- Utilities -->
<?php include_once "build/utils.php"; ?>
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
  tableTitle( "EFeff", "Efficiencies: Event Filter", lowptLink( $efeff_dir_lowpt, $lowpt ) );    
  tableRow( $efeff_dir, HLT_eta_eff, "EF eta efficiency", HLT_pT_eff, "EF PT efficiency"); 
  tableRow( $efeff_dir, HLT_phi_eff, "EF phi efficiency", HLT_a0_eff, "EF d0 efficiency"); 
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
