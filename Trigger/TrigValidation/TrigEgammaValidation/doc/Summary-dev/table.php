<?

$data = array();
$map = array();

$listStuff = array(
             'TrigEgammaValidation_RTT_testOptions_e' => array('ZeeValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_se' => array('SingleElectronValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_h' => array('HggValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_j' => array('JetValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_bs_j' => array('JetValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_bs_off_se' => array('SingleElectronOfflineValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_bs_off_e' => array('ZeeOfflineValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_bs_off_h' => array('HggOfflineValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_off_se' => array('SingleElectronOfflineValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_off_e' => array('ZeeOfflineValidationEff.txt'),
             'TrigEgammaValidation_RTT_testOptions_off_h' => array('HggOfflineValidationEff.txt')
             );

foreach ($listStuff as $key => $value) {
  if (!isset($map[$key])) {
    $map[$key] = array();
  }
  foreach ($value as $file) {
    $map[$key][$file] = array();
    $strTmp = calcfile($file, false, $key);
    if (file_exists($strTmp)) {
      $handle = fopen($strTmp, "r");
      if (isset($handle)) {
        for ($i = 1; !feof($handle); $i++) {
          $tmp = fgets($handle);
          $listMe = sscanf($tmp, "%s %s");
          $map[$key][$file][$listMe[0]] = floatval($listMe[1]);
        }
        fclose($handle);
      }
    }
  } 
}


?>

<div></div>
<div style="float: right; border: 1px solid #666; margin-left: 0.5em; background-color: #AAAACC; width: 20%;">
<b>
<p>Trigger Cuts</p>
</b>
<div style="font-size: 80%;">
<p style="font-size: 120%; font-weight: bold;">Level 1</p>
<p>Em Core &gt; 18 GeV</p>
<p>Em Isolation &lt;= 3 GeV</p>
<p>Had Core &lt;= 2 GeV</p>
<p>Had Isolation &lt;= 1 GeV</p>
<p style="font-size: 120%; font-weight: bold;">Level 2</p>
<p>Rcore &gt; 0.895</p>
<p>Rstrip &gt; 0.730</p>
<p>Et &gt; 20GeV</p>
<p>Had Et &lt; 3.8 GeV</p>
<p style="font-size: 120%; font-weight: bold;">Level 2 Inner Detector</p>
<p>Pt &gt; 5 GeV</p>
<p>|Delta Eta| &lt; 0.018</p>
<p>|Delta Phi| &lt; 0.06</p>
<p>Et/Pt &lt; 5.5</p>
<p style="font-size: 120%; font-weight: bold;">Event Filter Calo</p>
<p>|Delta Eta| &lt; 0.1</p>
<p>|Delta Phi| &lt; 0.1</p>
<p>Et &lt; 22 GeV</p>
<p></p>
</div>
</div>
<div></div>

<div style="width: 79%; text-align: center;">

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>Zee RDOs, Truth Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"]["Total"]) ?> events)
    </th>
    <th colspan=4>Zee RDOs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Truth
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L1", "Total", "L1_Zee"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L1", "Total", "L1_Off_Zee"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2", "Total", "L2_Zee"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2", "L1", "L2_Zee"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2", "Total", "L2_Off_Zee"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2", "L1", "L2_Off_Zee"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_Off_Zee"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_Off_Zee"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_e"]["ZeeValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_Off_Zee"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_Off_Zee"); ?>
    </td>

  </tr>

</table>
</center>

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>Single Electron RDOs, Truth Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"]["Total"]) ?> events)
    </th>
    <th colspan=4>Single Electron RDOs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Truth
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L1", "Total", "L1_SE"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L1", "Total", "L1_Off_SE"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2", "Total", "L2_SE"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2", "L1", "L2_SE"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2", "Total", "L2_Off_SE"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2", "L1", "L2_Off_SE"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_Off_SE"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_Off_SE"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_se"]["SingleElectronValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_Off_SE"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_Off_SE"); ?>
    </td>

  </tr>

</table>
</center>

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>H -&gt; gamma + gamma RDOs, Truth Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"]["Total"]) ?> events)
    </th>
    <th colspan=4>H -&gt; gamma + gamma RDOs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Truth
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L1", "Total", "L1_Hgg"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L1", "Total", "L1_Off_Hgg"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2", "Total", "L2_Hgg"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2", "L1", "L2_Hgg"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2", "Total", "L2_Off_Hgg"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2", "L1", "L2_Off_Hgg"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_Off_Hgg"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_Off_Hgg"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_h"]["HggValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_Off_Hgg"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_off_h"]["HggOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_Off_Hgg"); ?>
    </td>

  </tr>

</table>
</center>

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>Jets RDOs (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"]) ?> events)
    </th>
    <th colspan=4>Jets BSs (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Rates
    </td>
    <td colspan=2>
      Rates relative to previous trigger step
    </td>
    <td colspan=2>
      Rates
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L1"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "L1_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td colspan=2>
      ***
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L1"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "L1_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "L2_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L1"], "L2_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "L2_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td colspan=2>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L1"], "L2_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDScan"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "L2IDScan_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDSi"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "L2IDSi_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDScan"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2"], "L2IDScan_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDSi"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2"], "L2IDSi_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDScan"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "L2IDScan_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDSi"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "L2IDSi_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDScan"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2"], "L2IDScan_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDSi"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2"], "L2IDSi_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFScanCalo"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "EFScanCalo_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFSiCalo"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "EFSiCalo_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFScanCalo"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDScan"], "EFScanCalo_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFSiCalo"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["L2IDSi"], "EFSiCalo_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFScanCalo"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "EFScanCalo_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFSiCalo"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "EFSiCalo_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFScanCalo"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDScan"], "EFScanCalo_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFSiCalo"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["L2IDSi"], "EFSiCalo_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFScanTrack"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "EFScanTrack_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFSiTrack"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["Total"], "EFSiTrack_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFScanTrack"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFScanCalo"], "EFScanTrack_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFSiTrack"], $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["EFSiCalo"], "EFSiTrack_Jet", $map["TrigEgammaValidation_RTT_testOptions_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFScanTrack"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "EFScanTrack_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFSiTrack"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["Total"], "EFSiTrack_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFScanTrack"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFScanCalo"], "EFScanTrack_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>
    <td>
      <? showfreq($map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFSiTrack"], $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["EFSiCalo"], "EFSiTrack_JetBS", $map["TrigEgammaValidation_RTT_testOptions_bs_j"]["JetValidationEff.txt"]["sL"]); ?>
    </td>

  </tr>

</table>
</center>

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>Zee BSs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L1", "Total", "L1_ZeeBS"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2", "Total", "L2_ZeeBS"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2", "L1", "L2_ZeeBS"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_ZeeBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_ZeeBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_ZeeBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_e"]["ZeeOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_ZeeBS"); ?>
    </td>

  </tr>

</table>
</center>


<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>25 GeV Single Electron BSs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L1", "Total", "L1_SEBS"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2", "Total", "L2_SEBS"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2", "L1", "L2_SEBS"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_SEBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_SEBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_SEBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_se"]["SingleElectronOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_SEBS"); ?>
    </td>

  </tr>

</table>
</center>

<h4><?  ?></h4>
<center>
<table border=1>
  <tr>
    <th></th>
    <th colspan=4>Hgg BSs, Offline Algorithm (
<? printf("%d", $map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"]["Total"]) ?> events)
    </th>
  </tr>
  <tr>
    <td></td>
    <td colspan=2>
      Efficiency w.r.t. Offline
    </td>
    <td colspan=2>
      Efficiency relative to previous trigger step
    </td>
  </tr>
  <tr> 
    <td>
Level 1 E25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L1", "Total", "L1_HggBS"); ?>
    </td>
    <td colspan=2>
      ***
    </td>
  </tr>
  <tr>
    <td>
    Level 2 Calo e25i
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2", "Total", "L2_HggBS"); ?>
    </td>
    <td colspan=2>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2", "L1", "L2_HggBS"); ?>
    </td>

  </tr>

  <tr>
    <td></td>
    <td>ID Scan</td>
    <td>SiTrack</td>
    <td>ID Scan</td>
    <td>SiTrack</td>
  </tr>

  <tr>
    <td>
      Level 2 ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2IDScan", "Total", "L2IDScan_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2IDSi", "Total", "L2IDSi_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2IDScan", "L2", "L2IDScan_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "L2IDSi", "L2", "L2IDSi_HggBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter Calo e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFScanCalo", "Total", "EFScanCalo_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFSiCalo", "Total", "EFSiCalo_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFScanCalo", "L2IDScan", "EFScanCalo_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFSiCalo", "L2IDSi", "EFSiCalo_HggBS"); ?>
    </td>
  </tr>

  <tr>
    <td>
      Event Filter ID e25i
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFScanTrack", "Total", "EFScanTrack_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFSiTrack", "Total", "EFSiTrack_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFScanTrack", "EFScanCalo", "EFScanTrack_HggBS"); ?>
    </td>
    <td>
      <? show_file_perc($map["TrigEgammaValidation_RTT_testOptions_bs_off_h"]["HggOfflineValidationEff.txt"], "EFSiTrack", "EFSiCalo", "EFSiTrack_HggBS"); ?>
    </td>

  </tr>

</table>
</center>


</div>
<?

function show_file_perc($item, $a, $b, $i) {
  showperc($item[$a], $item[$b], $i);
}

function showperc($a, $b, $i)
{
  global $release;

  $str = calcget("summary.php", $release, $i, true);
  $str = str_replace("%", "%%", $str);

  if ( ($b != 0) && ($a != 0) ) {
    printf("<a href=\"".$str."\">%0.1f +- %0.1f %%</a>", 100*$a/$b, (100*$a/$b)*sqrt((1/$a)+(1/$b)));
  } else {
    printf("<a href=\"".$str."\">"."0.0 +- 0.0 %%</a>");
  }
}

function showfreq($a, $b, $i, $sL)
{
  global $release;

  $str = calcget("summary.php", $release, $i, true);
  $str = str_replace("%", "%%", $str);

  if ( ($b != 0) && ($a != 0) ) {
    $rate = ($sL*$a/$b);
    $rate_err = ($sL*$a/$b)*sqrt((1/$a)+(1/$b));
    if ($rate < 1000.) {
      printf("<a href=\"".$str."\">%0.1f +- %0.1f Hz</a>", $rate, $rate_err);
    } else {
      printf("<a href=\"".$str."\">%0.1f +- %0.1f KHz</a>", $rate/1000., $rate_err/1000);
    }
  } else {
    printf("<a href=\"".$str."\">0.0 +- 0.0 Hz</a>");
  }
}


?>
