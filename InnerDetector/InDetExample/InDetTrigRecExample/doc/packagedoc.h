/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetTrigRecExample_page InDetTrigRecExample Package
@author Patricia.Conde@cern.ch, Iwona.Grabowska@cern.ch
@author Jiri.Masik@cern.ch
@section InDetTrigRecExample_InDetTrigRecExampleIntro Introduction

InDetTrigRecExample is the main configuration package for running EFID
(Inner Detector reconstruction at the Event Filter level). It contains
configuration of tools used by the tracking algorithms and defines
sequences of algorithms for performing the track reconstruction in
various trigger slices

   - InDetTrigRec_jobOptions.py        - the main jobOptions fragment. 
       This sets up the various algorithms used in InDet reconstruction. 


@section InDetTrigRecExample_InDetTrigRecExampleConf Configuration of the EFID

<ul>

<li>
The sequences of algorithms for various slices and reconstruction
options are constructed by python::EFInDetConfig::TrigEFIDSequence
defined in EFInDetConfig.py. The list of algorithms run in 
 python::EFInDetConfig::TrigEFIDSequence::__init__

The sequence is constructed from textual
representation which has greater flexibility over older configuration
classes like python::EFInDetConfig::TrigEFIDCombined_Electron.
python::EFInDetConfig::TrigEFIDSequence allows configurable sequence
name which may be needed in the trigger menu configuration to avoid
undesired caching effects.

<li> The steering of configuration consults python::InDetTrigFlags object
which inherits and extends InDetJobProperties and instantiates them in
a different JobProperty context to avoid conflicts between EFID and
offline settings.

<li> python::InDetTrigSliceSettings::InDetTrigSliceSettingsDB -
defines a slice-dependent settings of pT, RoI half-width or fullScan
mode. The class is found in InDetTrigSliceSettings.py

<li> Instantiation of general tools used by multiple slices is done in
the python::InDetTrigConfigRecLoadTools, cosmics and beam gas specific
tools go in python::InDetTrigConfigRecLoadToolsCosmics and
python::InDetTrigConfigRecLoadToolsBeamGas. Other tools whose
configuration varies slice-by-slice can be found as local to the
algorithm classes mentioned next.

<li> 
InDetTrigConfigRecPreProcessing.py instantiates algorithms for data preparation and spacepoint formation
<ul>
<li>python::InDetTrigConfigRecPreProcessing::PixelClustering_EF 
<li>python::InDetTrigConfigRecPreProcessing::SCTClustering_EF
<li>python::InDetTrigConfigRecPreProcessing::TRTDriftCircleMaker_EF
<li>python::InDetTrigConfigRecPreProcessing::SiTrigSpacePointFinder_EF
</ul>
and a related truth algorithm python::InDetTrigConfigRecPreProcessing::InDetTrigPRD_MultiTruthMaker_EF


<li>
InDetTrigConfigRecNewTracking.py collects algorithms for the inside-out tracking
<ul>
<li>python::InDetTrigConfigRecNewTracking::SiTrigTrackFinder_EF
<li>python::InDetTrigConfigRecNewTracking::TrigAmbiguitySolver_EF
<li>python::InDetTrigConfigRecNewTracking::TRTTrackExtAlg_EF
<li>python::InDetTrigConfigRecNewTracking::TrigExtProcessor_EF
</ul>
and python::InDetTrigConfigRecNewTracking::InDetTrigDetailedTrackTruthMaker_EF

<li>
InDetTrigConfigRecPostProcessing.py configures algorithms run after the main tracking sequence
<ul>
<li>python::InDetTrigConfigRecPostProcessing::InDetTrigTrackSlimmer_EF
<li>python::InDetTrigConfigRecPostProcessing::TrigVxPrimary_EF
<li>python::InDetTrigConfigRecPostProcessing::InDetTrigParticleCreation_EF
<li>python::InDetTrigConfigRecPostProcessing::InDetTrigTrackParticleTruthMaker_EF
<li>python::InDetTrigConfigRecPostProcessing::InDetTrigConversionFinder_EF
</ul>


<li>
InDetTrigConfigRecBackTracking.py - are algorithms specific to backtracking or TRT only tracking
<ul>
<li>python::InDetTrigConfigRecBackTracking::InDetTrigTrackPRD_Association_EF
<li>python::InDetTrigConfigRecBackTracking::TRT_TrigTrackSegmentsFinder_EF
<li>python::InDetTrigConfigRecBackTracking::TRT_TrigSeededTrackFinder_EF
<li>python::InDetTrigConfigRecBackTracking::TRTSeededTrigAmbiguitySolver_EF
<li>python::InDetTrigConfigRecBackTracking::TRT_TrigStandaloneTrackFinder_EF
<li>python::InDetTrigConfigRecBackTracking::TrkTrigTrackCollectionMerger_EF
</ul>


@section InDetTrigRecExample_InDetTrigRecExamplejo End user job options

The package contains jobOptions which can be run by users for testing purposes

   - testEFID_basic.py
   - jobOptionsNewSteering.py     

The second uses CSC datasets and runs the complete EF InDet reconstruction in the trigger.
jobOptionsNewSteering.py is copied from /share to the run/
directory and it is suggested that you modify it there. In general you
should only need to alter the doBlah flags (i.e. set
doTruth=FALSE/TRUE) and the InDetTrigFlags. You may also need to modify
the dataset used. The jobOptions can be steered by general RecExCommon flags.



@section InDetTrigRecExample_InDetTrigRecExampleRTT RTT tests

The global RTT configuration is found in test/InDetTrigRecExample_TestConfiguration.xml. The current configuration runs 
<ul>
<li>
a full chain-test based on official transformations rerunning trigger on the 2008 cosmics data BS followed by Tier0 reconstruction of the output BS file
<li>
tracking in top events with testEFID_RTT_InDetTrigRecExample.py
</ul>

jobOptions of earlier RTT tests are still kept in 

   - jobOptions_RTT_InDetTrigRecExample.py - performs the EF InDet inside-out reconstruction from RDO for 450 top events.
     Three LVL1 RoI sorts are taken into account: electron-, muon- and tau-like. As output monitoring histograms 
qy     (expert-monitoring.root) and CBNTAA ntuple (ntuple.root) are stored.
   - jobOptions_RTT_InDetTrigRecExample_doReadBS.py  - performs the EF InDet reconstruction from byte stream for 50 top events.
     In this case one runs with doTruth=False as no MC truth info is available in byte-stream files. The EF InDet inside-out reconstruction
     is performed in a full-scan mode.
     As output the monitoring histograms (expert-monitoring.root) are stored. They also contain timing measurements (doTimer=True).

   - jobOptions_RTT_InDetTrigRecExample.py - RTT test of the EF InDet reconstruction for 
       450 top events from RDO,
   - jobOptions_RTT_InDetTrigRecExample_doReadBS.py - RTT test of the EF InDet reconstruction for 50 top events from byte stream.
   


@section InDetTrigRecExample_InDetTrigRecExampleLeaks Memory-leak test

There are two jobOptions files to run a memory-leak test. They are put in /TESTS:

	- testEFIDReadBSMemAudit.py - main jobOptions file to read 10k events from byte-stream input and give MemAudit statistics,
	- testEFIDreadBS_Flags.py - auxiliary file to run the EF InDet inside-out reconstruction for the egamma slice.

A recipe how to run the memory-leak test is given in the README file in /TESTS.



*/
