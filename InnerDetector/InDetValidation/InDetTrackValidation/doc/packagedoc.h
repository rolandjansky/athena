/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackValidation_page The InDetTrackValidation package
Validation tools for tracking in the inner detector.

@author Andreas.Salzburger@cern.ch, Sebastian.Fleischmann@cern.ch

@section InDetTrackValidation_introductionInDetTrackValidation Introduction

This package contains tools to validate the tracking in the inner detector.
This includes tools to fill validation ntuples e.g. with measurement data and a tool
for the calculation of SCT residuals and pulls.

@section InDetTrackValidation_InDetTrackValidationOverview Class Overview
  The InDetTrackValidation package contains the following classes:

  - InDet::SCT_ClusterValidationNtupleWriter : Algorithm to write SCT cluster (PRD) data to ntuple


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


@section InDetTrackValidation_ExtrasInDetTrackValidation Extra Pages

*/
