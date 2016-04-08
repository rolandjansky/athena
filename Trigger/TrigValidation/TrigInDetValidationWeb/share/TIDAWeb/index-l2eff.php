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
  tableTitle( "L2eff", "Efficiencies: Level 2", lowptLink( $l2eff_dir_lowpt, $lowpt ) );    
  tableRow( $l2eff_dir, HLT_eta_eff, "L2 eta efficiency", HLT_pT_eff, "L2 PT efficiency"); 
  tableRow( $l2eff_dir, HLT_phi_eff, "L2 phi efficiency", HLT_a0_eff, "L2 d0 efficiency"); 
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
