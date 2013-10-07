/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The InDetTrackValidation package
Validation tools for tracking in the inner detector.

@author Andreas.Salzburger@cern.ch, Sebastian.Fleischmann@cern.ch

@section introductionInDetTrackValidation Introduction

This package contains tools to validate the tracking in the inner detector.
This includes tools to fill validation ntuples e.g. with measurement data and a tool
for the calculation of SCT residuals and pulls.

@section InDetTrackValidationOverview Class Overview
  The InDetTrackValidation package contains the following classes:

  - InDet::PixelClusterValidationNtupleWriter : Algorithm to write Pixel cluster (PRD) data to ntuple
  - InDet::SCT_ClusterValidationNtupleWriter : Algorithm to write SCT cluster (PRD) data to ntuple
  - InDet::TRT_DriftCircleValidationNtupleWriter : Algorithm to write TRT drift circle (PRD) data to ntuple
  - InDet::SiResidualValidationNtupleHelper: Helper tool for Trk::BasicValidationNtupleTool 
        which writes measurement data for Pixels and SCT (PRD data, digi, holes, etc)
  - InDet::TRT_DriftTimeNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool 
        which writes measurement data for the TRT (i.e. data from InDet::TRT_DriftCircle and
        InDet::TRT_DriftCircleOnTrack). See the documentation of the TrkValTools package
        on how to use this helper tool.
  - InDet::SCT_ResidualPullCalculator : Sub-detector specific tool for the Trk::ResidualPullCalculator.
        For "artificially" two-dimensional SCT measurements (SCT endcap modules) the coordinates and
        covariance matrices are rotated to return the residual perpendicular to the strip.

@section WhichVariablesInDetTrackValidation List of the Ntuple variables
This paragraph gives a short overview of the ntuple variables provided by the different tools

@subsection PixelClusterWriterVariables Ntuple variables of the InDet::PixelClusterValidationNtupleWriter
<table>
<tr><td><b>PixelEventNumber</b></td>
<td>current event number</td>
<td><tt>long</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixelClusNum</b></td>
<td>number of pixel clusters per event</td>
<td><tt>long</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>LVL1TriggerType</b></td>
<td>Level1 Trigger type (stream)</td>
<td><tt>long</tt></td>
<td>8-bit</td></tr>
<tr><td><b>PixClusLocX</b></td>
<td>Local X coordinate of the cluster (digital position)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusLocY</b></td>
<td>Local Y coordinate of the cluster (digital position)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusGloX</b></td>
<td>Global X of surface centre of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusGloY</b></td>
<td>Global Y of surface centre of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusGloZ</b></td>
<td>Global Z of surface centre of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusEta</b></td>
<td>Global eta position of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusPhi</b></td>
<td>Global phi position of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixECBarrel</b></td>
<td>Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixClusLayer</b></td>
<td>Layer information (0, 1 or 2)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixEtaModule</b></td>
<td>Eta index of the module with cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixPhiModule</b></td>
<td>Phi index of the module with cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixClusGroupsize</b></td>
<td>Number of pixels in the clusters</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>cells</td></tr>
<tr><td><b>PixClusRow</b></td>
<td>Row index of the first pixel of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixClusCol</b></td>
<td>Cloumn index of the first pixel of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixDeltaRow</b></td>
<td>Row width of the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>cells</td></tr>
<tr><td><b>PixDeltaCol</b></td>
<td>Column width of the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>cells</td></tr>
<tr><td><b>PixOmegaPhi</b></td>
<td>Charge sharing variable, phi direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixOmegaEta</b></td>
<td>Charge sharing variable, eta direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusToT</b></td>
<td>Time over Threshold of the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusCharge</b></td>
<td>Total charge of the cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>electrons</td></tr>
<tr><td><b>PixClusLvl1</b></td>
<td>Level 1 accept of the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusGanged</b></td>
<td>If the cluster contains ganged pixels (1:yes)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusFake</b></td>
<td>If the cluster is flagged as fake (1:yes)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusAmbiguous</b></td>
<td>If the cluster is ambiguous (1:yes)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixClusLocXana</b></td>
<td>Local X coordinate of the cluster (analog position)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixClusLocYana</b></td>
<td>Local Y coordinate of the cluster (analog position)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b></b></td>
<td>Cluster position converted into pixel column index (eta direction)</td>
<td><tt>std::vector&lt;std::vector<int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b></b></td>
<td>Cluster position converted into pixel row index (phi direction)</td>
<td><tt>std::vector&lt;std::vector<int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixelHitsNum</b></td>
<td>number of Geant4 hits strings simulated for current event</td>
<td><tt>long</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixHitXstartpos</b></td>
<td>Local X position of the starting point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitYstartpos</b></td>
<td>Local Y position of the starting point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitZstartpos</b></td>
<td>Local Z position of the starting point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitXendpos</b></td>
<td>Local X position of the ending point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitYendpos</b></td>
<td>Local Y position of the ending point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitZendpos</b></td>
<td>Local Z position of the ending point of the Geant4 hit</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>PixHitBarrelEndcap</b></td>
<td>Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitLayerDisk</b></td>
<td>Layer information (0, 1 or 2)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitEtaModule</b></td>
<td>Eta index of the module with G4 hit</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitPhiModule</b></td>
<td>Phi index of the module with G4 hit</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitEtaIndex</b></td>
<td>G4 hit position converted into pixel column index (eta direction)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitPhiIndex</b></td>
<td>G4 hit position converted into pixel row index (phi direction)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>PixHitAngle</b></td>
<td>G4 hit incidence angle in phi direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>PixHitAngle2</b></td>
<td>G4 hit incidence angle in eta direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>PixHitEnergyLoss</b></td>
<td>G4 hit total energy loss</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>GeV</td></tr>
<tr><td><b>PixHitTime</b></td>
<td>G4 hit arrival time</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>ns</td></tr>
<tr><td><b>PixHitPDGParticle</b></td>
<td>G4 hit particle PDG code</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>PixHitFlag</b></td>
<td>G4 hit flag</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b></b></td>
<td>G4 hit corrected incidence angle</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>MC_Xpos</b></td>
<td>G4 hit associated to cluster local X position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>MC_Ypos</b></td>
<td>G4 hit associated to cluster local Y position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>MC_IncidentAngle</b></td>
<td>G4 hit associated to cluster incidence angle in phi direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>MC_IncidentAngle2</b></td>
<td>G4 hit associated to cluster incidence angle in eta direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>MC_EnergyLoss</b></td>
<td>G4 hit associated to cluster total energy loss</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>GeV</td></tr>
<tr><td><b>MC_Time</b></td>
<td>G4 hit associated to cluster arrival time</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>ns</td></tr>
<tr><td><b>MC_PDGParticle</b></td>
<td>G4 hit associated to cluster particle PDG code</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>MC_Flag</b></td>
<td>G4 hit associated to cluster flag</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>NotAssociated_Xpos</b></td>
<td>Local X position of G4 hit not associated to any cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>NotAssociated_Ypos</b></td>
<td>Local Y position of G4 hit not associated to any cluster</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>NotAssociated_BarrelEndcap</b></td>
<td>Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>NotAssociated_LayerDisk</b></td>
<td>Layer information (0, 1 or 2)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>NotAssociated_EtaModule</b></td>
<td>Eta index of the module with not associated G4 hit</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>NotAssociated_PhiModule</b></td>
<td>Phi index of the module with not associated G4 hit</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>index</td></tr>
<tr><td><b>NotAssociated_IncidentAngle</b></td>
<td>Not associated G4 hit incidence angle in phi direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>NotAssociated_IncidentAngle2</b></td>
<td>Not associated G4 hit incidence angle in eta direction</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>NotAssociated_EnergyLoss</b></td>
<td>Not associated G4 hit total energy loss</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>GeV</td></tr>
<tr><td><b>NotAssociated_Time</b></td>
<td>Not associated G4 hit arrival time</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>ns</td></tr>
<tr><td><b>NotAssociated_PDGParticle</b></td>
<td>Not associated G4 hit particle PDG code</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>NotAssociated_Flag</b></td>
<td>Not associated G4 hit flag</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
</table>

@subsection SctClusterWriterVariables Ntuple variables of the InDet::SCT_ClusterValidationNtupleWriter

<table>
<tr><td><b>SCT_RunNumber</b></td>
<td>run number</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_EventNumber</b></td>
<td>event number for current PRD vector</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_LumiBlock</b></td>
<td>event lumi block information</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_TimeStamp</b></td>
<td>event time stamp</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_BunchCrossing</b></td>
<td>event bunch crossing ID</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_nRIOs</b></td>
<td>number of RIOs, i.e. SCT clusters, in the event. This variable set the size of the vectors with cluster information.</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_DriftRadius</b></td>
<td>local x coordinate of strip</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>SCT_SurfaceX</b></td>
<td>global x coordinate of SCT wafer center</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>SCT_SurfaceY</b></td>
<td>global x coordinate of SCT wafer center</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>SCT_SurfaceZ</b></td>
<td>global x coordinate of SCT wafer center</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>SCT_BarrelEndcap</b></td>
<td>from IdHelper: pos/neg barrel (+/-1) and pos/neg endcap (+/-2)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>1:bar, 2:ec</td></tr>
<tr><td><b>SCT_LayerDisk</b></td>
<td>from IdHelper: layer/disk index</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_EtaModule</b></td>
<td>from IdHelper: module index in eta</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_PhiModule</b></td>
<td>from IdHelper: module index in phi</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_DeltaPhi</b></td>
<td>please provide description!</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SCT_HitErr</b></td>
<td>please provide description!</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_RDO_Group_Size</b></td>
<td>RDO group size</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_Layer</b></td>
<td>index of the SCT-layer in which the SCT wafer is positioned</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_Eta</b></td>
<td>eta of the SCT wafer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_Phi</b></td>
<td>phi of the SCT wafer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_Side</b></td>
<td>side of the SCT wafer (0 or 1) within the SCT module</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_Barrelec</b></td>
<td>barrel/endcap index (barrel: 0, endcap: +/-2)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_FirstStrip</b></td>
<td>index (could be from 0 to 767) of the fired strip within the SCT wafer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_NumHitsInWafer</b></td>
<td>number of hits in the SCT wafer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_WaferHash</b></td>
<td>index (could be from 0 to 8175) of the fired SCT wafer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_TotalNumBSErrs</b></td>
<td>total number of BS errors.</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_BCIDErrs</b></td>
<td>bunch crossing(BC) ID error - the ROD sets a bit in the link header if the bunch crossing ID for the event it gets ba
ck from the link doesn't match its own bunch crossing ID</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_LVL1Errs</b></td>
<td>same as BCID error, but with the LVL1ID</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_TimeOutErrs</b></td>
<td>the ROD sent a trigger to the module but got no data back down the link (perhaps the module is dead)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_PreambleErrs</b></td>
<td>the ROD found something strange in the header from the link, and set a bit to flag it</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_FormatterErrs</b></td>
<td>the ROD found something strange in the header from the link, and set a bit to flag it</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_TrailerErrs</b></td>
<td>the ROD found something strange in the link trailer and sets a bit there to flag it</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_HeaderTrailerLimitErrs</b></td>
<td>at high trigger rates and/or big events, where the module can't send events fast enough, so just sends headers and trailers, with no hits inbetween</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_ABCDErrs</b></td>
<td>one of the ABCD chips on the module flagged an error - these errors take the place of a hit, rather than being in the header or trailer</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_RawErrs</b></td>
<td>the ROD expected a hit, but the word doesn't match the expected format (e.g. due to opto noise?); this type of error 
also takes the place of a hit</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>RDO_SCT_BSParseErrs</b></td>
<td>the ByteStream decoder found something strange - either a strip out of range or something not in the cabling map, or 
data words that don't match up with any expected pattern</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
</table>

@subsection TRT_DriftCircleWriterVariables Ntuple variables of the InDet::TRT_DriftCircleValidationNtupleWriter

<table>
<tr><td><b>EventNumber</b></td>
<td>event number</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>TRT_nRIOs</b></td>
<td>number of driftcircles in current event. This variable set the size of the vectors with driftcircle information.</td>
<td><tt>int</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>TRT_DriftRadius</b></td>
<td>measured drift radius</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>TRT_SurfaceX</b></td>
<td>global X coordinate of straw centre position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>TRT_SurfaceY</b></td>
<td>global Y coordinate of straw centre position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>TRT_SurfaceZ</b></td>
<td>global Z coordinate of straw centre position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
</table>


@subsection TRT_DriftTimeNtupleHelperVariables Ntuple variables of the InDet::TRT_DriftTimeNtupleHelper
<table>
<tr><td>TRT_R[nTRTHits]</td> <td>drift radii of the TRT measurement</td> <td><tt>vector&lt;float&gt;</tt></td></tr>
<tr><td>TRT_R_track[nTRTHits]</td> <td>local track position (Trk::locR) of the track</td> <td><tt>vector&lt;float&gt;</tt></td></tr>
<tr><td>TRT_t[nTRTHits]</td> <td>drift time of the TRT measurement (==-1 if not valid)</td> <td><tt>vector&lt;float&gt;</tt></td></tr>
<tr><td>TRT_x[nTRTHits]</td> <td>x coordinate of the straw center</td> <td><tt>vector&lt;float&gt;</tt></td></tr>
<tr><td>TRT_y[nTRTHits]</td> <td>y coordinate of the straw center</td> <td><tt>vector&lt;float&gt;</tt></td></tr>
<tr><td>TRT_HL[nTRTHits]</td> <td>has the measurement reached the high level threshold?</td> <td><tt>vector&lt;int&gt;</tt></td></tr>
<tr><td>TRT_strawlayer[nTRTHits]</td> <td>index of the straw layer of the measurement</td> <td><tt>vector&lt;int&gt;</tt></td></tr>
<tr><td>TRT_layer[nTRTHits]</td> <td>index of the layer of the measurement</td> <td><tt>vector&lt;int&gt;</tt></td></tr>
<tr><td>TRT_straw[nTRTHits]</td> <td>index of the straw of the measurement</td> <td><tt>vector&lt;int&gt;</tt></td></tr>
<tr><td>TRT_phi[nTRTHits]</td> <td>phi module index of the measurement</td> <td><tt>vector&lt;int&gt;</tt></td></tr>
</table>
              
@subsection SiCalibrationVariablesForPixel Pixel Ntuple variables of the InDet::SiResidualValidationNtupleHelper
<table>
<tr><td>lvl1TriggerType</td><td>   LVL1 trigger type    </td><td><tt>  long</tt></td></tr>
<tr><td>PixDetType</td><td>  Layer information (0, 1, 2 for barrel,  100, 101, 102 for endcap A side, -100, -101, -102 for endcap C side) it is 0 by default    </td><td><tt> vector&lt;int&gt;</tt></td></tr>
<tr><td>PixHasGanged</td><td> 1 if the cluster contains ganged pixels     </td><td><tt> vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixIsFake</td><td> 1 if the cluster is marked as fake     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixLVL1A</td><td> lower level1 accept among the pixel of the cluster     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixToT</td><td>  total ToT of the cluster    </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixCharge</td><td>  total charge of the cluster    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTLorPhi</td><td>  tangent of the lorentz angle (local x direction)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTLorEta</td><td>  tangent of the lorentz angle (local y direction - should be 0!)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixBiasVolt</td><td>  Bias voltage (from conditions service)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTemp</td><td> Temperature (from conditions service)     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDepVolt</td><td>  Depletion voltage (from conditions service)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDigResPhi</td><td>  Residuals obtained using the cluster digital coordinates  </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDigResEta</td><td>  Residuals obtained using the cluster digital coordinates      </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixErrPhi</td><td>  Pixel Cluster position error (local x)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixErrEta</td><td>  Pixel Cluster position error (local y)    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixEta</td><td> Global position of the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixGloX</td><td> Global position of the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixGloY</td><td> Global position of the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixGloZ</td><td> Global position of the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixLocX</td><td> Cluster local coordinates     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixLocY</td><td>  Cluster local coordinates    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixEtaModule</td><td> phi indexe of the module     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixPhiModule</td><td> Eta indexe of the module     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixFirstRow</td><td> lower row of the pixels contained in the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixFirstCol</td><td> lower column of the pixels contained in the cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDeltaPhi</td><td> Pixel Cluster Size, in cell units     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDeltaEta</td><td> Pixel Cluster Size, in cell units     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixDeltaRow</td><td> Pixel cluster size, in mm     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixDeltaCol</td><td> Pixel cluster size, in mm     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
<tr><td>PixOmegaPhi</td><td> Pixel charge sharing variable     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixOmegaEta</td><td> Pixel charge sharing variable     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkAngle</td><td> Track incidence angle on the module (rphi plane) in local reference frame  </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkThetaI</td><td> Track incidence angle on the module (eta-normal plane) in local reference frame  </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkEta</td><td> Track incidence pseudorapidity in local module reference frame     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkPt</td><td> Track momentum     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkQ</td><td>  charge of the associated track    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
<tr><td>PixTrkClusGroupSize</td><td> total size of the cluster   </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
</table>

@subsection SiCalibrationVariablesForHoles Pixel Holes Ntuple variables of the InDet::SiResidualValidationNtupleHelper
<table>
 <tr><td>PixHoleGloX</td><td>  Global position of the hole    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleGloY</td><td>  Global position of the hole    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleGloZ</td><td>  Global position of the hole    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleLocX</td><td>  Local position of the hole    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleLocY</td><td>  Local position of the hole    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleEtaModule</td><td> Eta indexes of the module that contains the hole     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
 <tr><td>PixHolePhiModule </td><td> Phi indexes of the module that contains the hole      </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
 <tr><td>PixHoleLayer</td><td> Layer (0, 1 or 2) of the hole     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleECBarrel </td><td>  -2 for EC, 2 for EA, 0 for barrel    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleClNearstSize</td><td> total size of the nearest cluster to the hole     </td><td><tt>vector&lt;int&gt;</tt></td></tr>   
 <tr><td>PixHoleLocXClNearst </td><td> nearest cluster local X     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleLocYClNearst </td><td>  nearest cluster local Y    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleClRowClNearst  </td><td> row of the nearest cluster     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleClColClNearst </td><td>  Column of the nearest cluster    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleGloZNearst </td><td>  global z of the nearest cluster    </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleDeltaRowNearst </td><td> nearest Cluster Size, in cell units     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
 <tr><td>PixHoleDeltaColNearst </td><td> nearest Cluster Size, in cell units     </td><td><tt>vector&lt;float&gt;</tt></td></tr> 
</table>


@subsection SiCalibrationVariablesForSCT SCT Ntuple variables of the InDet::SiResidualValidationNtupleHelper

<table>
<tr><td><b>SctDetType</b></td>
<td>layer information (1 for barrel, 2 for disks)</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctLayerDisk</b></td>
<td>index of layer or disk containing the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctEtaModule</b></td>
<td>eta index of module containing the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctPhiModule</b></td>
<td>phi index of module containing the cluster</td>
<td><tt>std::vector&lt;int&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctTLorPhi</b></td>
<td>tangent of the lorentz angle (local x direction)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>1</td></tr>
<tr><td><b>SctTLorEta</b></td>
<td>tangent of the lorentz angle (local y direction, should be 0)</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>1</td></tr>
<tr><td><b>SctEta</b></td>
<td>pseudorapidity calculated from module position</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctDeltaPhi</b></td>
<td>SCT cluster size in strip units</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>strips</td></tr>
<tr><td><b>SctTrkAngle</b></td>
<td>track incidence angle on the module in local reference frame</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>rad</td></tr>
<tr><td><b>SctTrkQ</b></td>
<td>reconstructed track charge</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
<tr><td><b>SctErr</b></td>
<td>ClusterOnTrack's error in local X coordinate</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>mm</td></tr>
<tr><td><b>SctTrkClusGroupSize</b></td>
<td>SCT total cluster size</td>
<td><tt>std::vector&lt;float&gt;</tt></td>
<td>&nbsp;</td></tr>
</table>


@section ExtrasInDetTrackValidation Extra Pages

 - @ref UsedInDetTrackValidation
 - @ref requirementsInDetTrackValidation
*/

/**
@page UsedInDetTrackValidation Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirementsInDetTrackValidation Requirements
@include requirements
*/

