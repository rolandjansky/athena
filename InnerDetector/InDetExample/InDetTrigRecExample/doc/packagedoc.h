/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


*/
