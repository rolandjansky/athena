# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file InDetRecExample/python/InDetKeys.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" InDetContainerKeys
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Wildauer"
__version__= "$Revision: 1.19 $"
__doc__    = "InDetContainerKeys"

__all__    = [ "InDetKeyContainers" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class DBMTrackParticles(JobProperty):
    """StoreGate key for TrackParticles DBM"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleCandidateDBM'

class DBMTrackParticlesTruth(JobProperty):
    """StoreGate key for TrackParticles DBM"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleTruthCollectionDBM'

class DBMDetailedTracksTruth(JobProperty): 
    """StoreGate key for DetailedTrackTruth DBM""" 
    statusOn     = True 
    allowedTypes = ['str'] 
    StoredValue  = 'DBMTracksDetailedTruth' 
     
class DBMTracksTruth(JobProperty): 
    """StoreGate key for TracksTruth DBM""" 
    statusOn     = True 
    allowedTypes = ['str'] 
    StoredValue  = 'DBMTracksTruthCollection' 
     
class SiSpSeededDBMTracks(JobProperty): 
    """ Storegate key for new-tracking SP seeded DBM tracks""" 
    statusOn     = True 
    allowedTypes = ['str'] 
    StoredValue  = 'SiSPSeededDBMTracks'

class DBMTracks(JobProperty): 
    """StoreGate key for DBM tracks """ 
    statusOn     = True 
    allowedTypes = ['str'] 
    StoredValue  = 'ResolvedDBMTracks' 

class xAODDBMTrackParticleContainer(JobProperty): 
    """xAOD DBMTrackParticle""" 
    statusOn = True 
    allowedTypes = ['str'] 
    StoredValue = "InDetDBMTrackParticles" 

class PixelManager(JobProperty):
    """DetectorManager for Pixel"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Pixel'

class SCT_Manager(JobProperty):
    """DetectorManager for SCT"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT'

class TRT_Manager(JobProperty):
    """DetectorManager for TRT"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT'

class PixelRDOs(JobProperty):
    """StoreGate key for pixel raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelRDOs'

class SCT_RDOs(JobProperty):
    """StoreGate key for SCT raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_RDOs'

class TRT_RDOs(JobProperty):
    """StoreGate key for TRT raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_RDOs'

	   
class PixelPURDOs(JobProperty):
    """StoreGate key for PU pixel raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Pixel_PU_RDOs'
	
class SCT_PU_RDOs(JobProperty):
    """StoreGate key for PU SCT raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_PU_RDOs'
	
class TRT_PU_RDOs(JobProperty):
    """StoreGate key for PU TRT raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_PU_RDOs'

class BCM_RDOs(JobProperty):
    """StoreGate key for BCM raw data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'BCM_RDOs'

class BCM_CompactDOs(JobProperty):
    """StoreGate key for zero-suppressed BCM data objects"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'BCM_CompactDOs'        

class PixelByteStreamErrs(JobProperty):
    """StoreGate key for pixel BS errors"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelByteStreamErrs'

class SCT_ByteStreamErrs(JobProperty):
    """StoreGate key for SCT BS errors"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_ByteStreamErrs'

class TRT_ByteStreamErrs(JobProperty):
    """StoreGate key for TRT BS errors"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_ByteStreamErrs'

class TRT_ByteStreamIdErrs(JobProperty):
    """StoreGate key for TRT BS Id errors"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_ByteStreamIdErrs'

class GangedPixelMap(JobProperty):
    """StoreGate key for ganged pixel map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelClusterAmbiguitiesMap'

class SplitClusterAmbiguityMap(JobProperty):
    """StoreGate key for split cluster pixel map from NN clustering"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SplitClusterAmbiguityMap'

class PixelSDOs(JobProperty):
    """StoreGate key for pixel simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelSDO_Map'

class SCT_SDOs(JobProperty):
    """StoreGate key for SCT simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_SDO_Map'

class TRT_SDOs(JobProperty):
    """StoreGate key for TRT simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_SDO_Map'

class PixelPUSDOs(JobProperty):
    """StoreGate key for PU pixel simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Pixel_PU_SDO_Map'

class SCT_PU_SDOs(JobProperty):
    """StoreGate key for PU SCT simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_PU_SDO_Map'

class TRT_PU_SDOs(JobProperty):
    """StoreGate key for PU TRT simulated data object"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_PU_SDO_Map'

class PixelClusters(JobProperty):
    """StoreGate key for PixelClusters prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelClusters'

class SCT_Clusters(JobProperty):
    """StoreGate key for SCT_Clusters prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_Clusters'

class TRT_DriftCircles(JobProperty):
    """StoreGate key for TRT_DriftCircles prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_DriftCircles'

class PixelPUClusters(JobProperty):
    """StoreGate key for PU PixelClusters prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelPUClusters'

class SCT_PU_Clusters(JobProperty):
    """StoreGate key for PU SCT_Clusters prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_PU_Clusters'

class TRT_PU_DriftCircles(JobProperty):
    """StoreGate key for PU TRT_DriftCircles prep raw data"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_PU_DriftCircles'

class TRT_DriftCirclesUncalibrated(JobProperty):
    """StoreGate key for uncalibrated TRT_DriftCircles  prep raw data (only for cosmics)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_DriftCirclesUncalibrated'

class TRT_PU_DriftCirclesUncalibrated(JobProperty):
    """StoreGate key for uncalibrated TRT_PU_DriftCircles  prep raw data (only for cosmics)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRT_PU_DriftCirclesUncalibrated'

class PixelClustersTruth(JobProperty):
    """StoreGate key for PixelClustersTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_MultiTruthPixel'

class SCT_ClustersTruth(JobProperty):
    """StoreGate key for SCT_ClustersTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_MultiTruthSCT'

class TRT_DriftCirclesTruth(JobProperty):
    """StoreGate key for TRT_DriftCirclesTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_MultiTruthTRT'

class PixelPUClustersTruth(JobProperty):
    """StoreGate key for PU PixelClustersTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_PU_MultiTruthPixel'

class SCT_PU_ClustersTruth(JobProperty):
    """StoreGate key for PU SCT_ClustersTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_PU_MultiTruthSCT'

class TRT_PU_DriftCirclesTruth(JobProperty):
    """StoreGate key for PU TRT_DriftCirclesTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PRD_PU_MultiTruthTRT'

class PixelSpacePoints(JobProperty):
    """StoreGate key for PixelSpacePoints"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelSpacePoints'

class SCT_SpacePoints(JobProperty):
    """StoreGate key for SCT_SpacePoints"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SCT_SpacePoints'

class OverlapSpacePoints(JobProperty):
    """StoreGate key for OverlapSpacePoints"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'OverlapSpacePoints'

class TRT_Segments(JobProperty):
    """StoreGate key for TRTSegments"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTSegments'

class TRT_SegmentsTRT(JobProperty):
    """StoreGate key for TRTSegmentsTRT"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTSegmentsTRT'

class TRT_SegmentsTRT_EC(JobProperty):
    """StoreGate key for TRTSegmentsTRT_EC"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTSegmentsTRT_EC'

class TRT_Segments_Phase(JobProperty):
    """StoreGate key for TRTSegmentsTRT_Phase"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTSegments_Phase'

class TRT_Tracks_Phase(JobProperty):
    """StoreGate key for TRTTracks_Phase"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTTracks_Phase'


class TRTSeededTracks(JobProperty):
    """StoreGate key for TRT Seeded Tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTSeededTracks'
    
class ResolvedTRTSeededTracks(JobProperty):
    """StoreGate key for Resolved TRT Seeded Tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedTRTSeededTracks'

class AliasToTracks(JobProperty):
    """StoreGate key for AliasToTracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'none'

class Tracks(JobProperty):
    """StoreGate key for the final track collection"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Tracks'

class DetailedTracksTruth(JobProperty):
    """StoreGate key for DetailedTrackTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'DetailedTrackTruth'

class TracksTruth(JobProperty):
    """StoreGate key for TracksTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackTruthCollection'

class UnslimmedTracks(JobProperty):
    """StoreGate key for unslimmed final track collection"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedInDetTracks'

class UnslimmedDetailedTracksTruth(JobProperty):
    """StoreGate key for detailed track truth of final unslimmed track collection"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedInDetTracksDetailedTruth'

class UnslimmedTracksTruth(JobProperty):
    """StoreGate key for track truth of final unslimmed track collection"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedInDetTracksTruthCollection'

class ExtendedTracksMap(JobProperty):
    """StoreGate key for unslimmed inside out extended tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMap'

class ExtendedTracksMapPhase(JobProperty):
    """StoreGate key for unslimmed inside out extended tracks map (phase)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapPhase'

class ExtendedTracksMapSLHC(JobProperty):
    """ Storegate key for new-tracking SLHC extended tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapSLHC'

class ExtendedTracksMapLowPt(JobProperty):
    """ Storegate key for new-tracking low Pt extended tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapLowPt'

class ExtendedTracksMapBeamGas(JobProperty):
    """ Storegate key for new-tracking beam gas extended tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapBeamGas'

class ExtendedTracksMapLargeD0(JobProperty):
    """ Storegate key for new-tracking high-d0 extended tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapLargeD0'

class ExtendedTracks(JobProperty):
    """StoreGate key for unslimmed inside out tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracks'

class ExtendedTracksPhase(JobProperty):
    """StoreGate key for unslimmed inside out tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksPhase'

class ExtendedSLHCTracks(JobProperty):
    """ Storegate key for new-tracking extended SLHC tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedSLHCTracks'

class ExtendedLowPtTracks(JobProperty):
    """ Storegate key for new-tracking extended low Pt tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedLowPtTracks'

class ExtendedBeamGasTracks(JobProperty):
    """ Storegate key for new-tracking extended beam gas tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedBeamGasTracks'

class ExtendedLargeD0Tracks(JobProperty):
    """ Storegate key for new-tracking extended high-d0 tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedLargeD0Tracks'

class ProcessedESDTracks(JobProperty):
    """ Storegate key for already processed tracks from ESD"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Tracks'


class ExtendedDetailedTracksTruth(JobProperty):
    """StoreGate key for detailed track truth of unslimmed inside out tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksDetailedTruth'

class ExtendedTracksTruth(JobProperty):
    """StoreGate key for track truth of unslimmed inside out tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksTruthCollection'

class RefittedTracks(JobProperty):
    """StoreGate key for refitted tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'RefittedTracks'

class RefittedDetailedTracksTruth(JobProperty):
    """StoreGate key for detailed track truth of refitted tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'RefittedTracksDetailedTruth'

class RefittedTracksTruth(JobProperty):
    """StoreGate key for track truth of refitted tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'RefittedTracksTruthCollection'

class SiSpSeededPixelTracks(JobProperty):
    """StoreGate key for SP seeded pixel standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededPixelTracks'

class SiSpSeededPixelPrdAssociationTracks(JobProperty):
    """StoreGate key for SP seeded pixel tracks run after new tracking on remaining PRDs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededPixelPrdAssociationTracks'

class PixelPrdAssociationTracksTruth(JobProperty):
    """StoreGate key for pixel tracks truth run after New Tracking on remaining PRDs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelPrdAssociationTracksTruthCollection'

class PixelPrdAssociationDetailedTracksTruth(JobProperty):
    """StoreGate key for detailed pixel tracks truth run after New Tracking on remaining PRDs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelPrdAssociationTracksDetailedTruth'

class SiSpSeededSCTTracks(JobProperty):
    """StoreGate key for SP seeded SCT standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededSCTTracks'

class PixelTracks(JobProperty):
    """StoreGate key for resolved pixel standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedPixelTracks'

class PixelDetailedTracksTruth(JobProperty):
    """StoreGate key for DetailedTrackTruth  (PseudoTracking)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedPixelTrackDetailedTruth'

class PixelTracksTruth(JobProperty):
    """StoreGate key for TracksTruth  (PseudoTracking)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedPixelTrackTruthCollection'


class ResolvedPixelPrdAssociationTracks(JobProperty):
    """StoreGate key for resolved pixel tracks run after New Tracking on remaining PRDs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedPixelPrdAssociationTracks'

class ExtendedTracksPixelPrdAssociation(JobProperty):
    """ Storegate key for new-tracking extended pixel tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksPixelPrdAssociation'

class PixelPrdAssociationTracks(JobProperty):
    """StoreGate key for rfinal pixel tracks run after New Tracking on remaining PRDs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PixelPrdAssociationTracks'

class ExtendedTracksMapPixelPrdAssociation(JobProperty):
    """ Storegate key for new-tracking extended pixel tracks map"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedTracksMapPixelPrdAssociation'

class SCTTracks(JobProperty):
    """StoreGate key for resolved SCT standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedSCTTracks'    

class TRTTracks(JobProperty):
    """StoreGate key for subdetector TRT standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'StandaloneTRTTracks'

class TRTTracks_NewT(JobProperty):
    """StoreGate key for new-tracking TRT standalone tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TRTStandaloneTracks'
	
class PseudoTracks(JobProperty):
    """StoreGate key for the final track collection (PseudoTracking)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetPseudoTracks'
 
class PseudoDetailedTracksTruth(JobProperty):
    """StoreGate key for DetailedTrackTruth  (PseudoTracking)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetPseudoTrackDetailedTruth'

class PseudoTracksTruth(JobProperty):
    """StoreGate key for TracksTruth  (PseudoTracking)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetPseudoTrackTruthCollection'

class SiSpSeededTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededTracks'

class SiSPSeedSegments(JobProperty):
    """ Storegate key for SP Seed Segments to studies"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeedSegments'


class SiSpSeededSLHCTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded SLHC tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededSLHCTracks'

class SiSpSeededLowPtTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded low Pt tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededLowPtTracks'

class SiSpSeededVeryLowPtTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded very low Pt tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededVeryLowPtTracks'

class SiSpSeededForwardTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded very forward tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSpSeededForwardTracks'

class SiSpSeededSLHCConversionFindingTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded SLHC conversion finding tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSpSeededSLHCConversionFindingTracks'

class SiSpSeededBeamGasTracks(JobProperty):
    """ Storegate key for new-tracking SP seeded beam gas tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSPSeededBeamGasTracks'    

class SiSpSeededLargeD0Tracks(JobProperty):
    """ Storegate key for new-tracking SP seeded high-d0 tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SiSpSeededLargeD0Tracks'

class ResolvedTracks(JobProperty):
    """ Storegate key for new-tracking resolved tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedTracks'

class ResolvedSLHCTracks(JobProperty):
    """ Storegate key for new-tracking resolved SLHC tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedSLHCTracks'

class ResolvedLowPtTracks(JobProperty):
    """ Storegate key for new-tracking resolved low Pt tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedLowPtTracks'

class ResolvedForwardTracks(JobProperty):
    """ Storegate key for new-tracking resolved very forward tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedForwardTracks'

class ResolvedSLHCConversionFindingTracks(JobProperty):
    """ Storegate key for new-tracking resolved SLHC Conversion tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedSLHCConversionFindingTracks'

class ResolvedForwardTracksTruth(JobProperty):
    """ Storegate key for new-tracking resolved very forward tracks truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedForwardTracksTruthCollection'

class ResolvedForwardTrackParticles(JobProperty):
    """ Storegate key for new-tracking resolved very forward tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedForwardTrackParticles'

class ResolvedForwardTrackParticlesTruth(JobProperty):
    """ Storegate key for new-tracking resolved very forward tracks truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedForwardTrackParticlesTruthCollection'

class ResolvedVeryLowPtTracks(JobProperty):
    """ Storegate key for new-tracking resolved very low Pt tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedVeryLowPtTracks'

class ResolvedBeamGasTracks(JobProperty):
    """ Storegate key for new-tracking resolved beam gas tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedBeamGasTracks'

class ResolvedLargeD0Tracks(JobProperty):
    """ Storegate key for new-tracking resolved high-d0 tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedLargeD0Tracks'

class HaloTracks(JobProperty):
    """StoreGate key for beam halo tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedHaloTracks'

class ExtendedHaloTracks(JobProperty):
    """StoreGate key for extended beam halo tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtendedHaloTracks'

class ResolvedSiCosmicTracks(JobProperty):
    """StoreGate key for resolved Si cosmic tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ResolvedSiCosmicTracks'

class CombinedCosmicTracks(JobProperty):
    """StoreGate key for combined cosmic tracks """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedCosmicTracks'
    
class PrimaryVertices(JobProperty):
    """StoreGate key for PrimaryVertices"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'VxPrimaryCandidate'

class PrimaryVerticesWithoutBeamConstraint(JobProperty):
    """StoreGate key for PrimaryVertices without beamconstraint, for monitoring only"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'VxPrimaryCandidateWithBeamConstraint'

class PrimaryVerticesSplitStream(JobProperty):
    """StoreGate key for PrimaryVertices from the split stream, for monitoring only"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'VxPrimaryCandidateSplitStream'

class TrackParticles(JobProperty):
    """StoreGate key for TrackParticles"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleCandidate'

class TrackParticlesTruth(JobProperty):
    """StoreGate key for TrackParticlesTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleTruthCollection'

class IPatPrimaryVertices(JobProperty):
    """StoreGate key for IPatPrimaryVertices"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'IPatVxPrimaryCandidate'

class IPatParticles(JobProperty):
    """StoreGate key for IPatTrackParticles"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'IPatTrackParticleCandidate'

class IPatParticlesTruth(JobProperty):
    """StoreGate key for IPatTrackParticlesTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'IPatTrackParticleTruthCollection'

class IPatConvertedTracks(JobProperty):
    """StoreGate key for converted IPatRec Tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedIPatTracks'

class IPatConvertedTracksTruth(JobProperty):
    """StoreGate key for converted IPatRec Tracks truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedIPatTracksTruth'

class IPatConvertedTracksDetailedTruth(JobProperty):
    """StoreGate key for converted IPatRec Tracks detailed truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedIPatTracksDetailedTruth'

class XKalPrimaryVertices(JobProperty):
    """StoreGate key for XKalPrimaryVertices"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'XKalVxPrimaryCandidate'

class XKalParticles(JobProperty):
    """StoreGate key for XKalTrackParticles"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'XKalTrackParticleCandidate'

class XKalParticlesTruth(JobProperty):
    """StoreGate key for XKalTrackParticlesTruth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'XKalTrackParticleTruthCollection'

class XKalConvertedTracks(JobProperty):
    """StoreGate key for converted XKalman Tracks"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedXKalmanTracks'

class XKalConvertedTracksTruth(JobProperty):
    """StoreGate key for converted XKalman Tracks truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedXKalmanTracksTruth'

class XKalConvertedTracksDetailedTruth(JobProperty):
    """StoreGate key for converted XKalman Tracks detailed truth"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConvertedXKalmanTracksDetailedTruth'

class V0Candidates(JobProperty):
    """StoreGate key for V0 candidates"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'V0Candidates'

class SimpleV0Candidates(JobProperty):
    """StoreGate key for simple V0 candidates"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SimpleV0Candidates'

class SecVertices(JobProperty):
    """StoreGate key for secondary (V0) vertices"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SecVertices'

class Conversions(JobProperty):
    """StoreGate key for conversions"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ConversionCandidate'

class LowBetaCandidates(JobProperty):
    """StoreGate key for low-beta candidates"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetLowBetaCandidates'

# output file keys
class trkValidationNtupleName(JobProperty):
    """Name of tracking validation ntuple file """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TrkValidation.root"

class trkD3PDFileName(JobProperty):
    """Name of tracking validation ntuple file """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "InDetTrackD3PD.root"

class StandardPlotHistName(JobProperty):
    """Name of Histogram file for making the Standard Performance Plots """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "InDetStandardPlots.root"

class PhysValMonHistName(JobProperty):
    """Name of Histogram file for the Physics Validation Monitoring"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "InDetPhysValMonitoringPlots.root"

class OutputRDOFileName(JobProperty):
    """Name of output RDO file"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "InDetRecRDO.root"

class OutputESDFileName(JobProperty):
    """Name of output ESD file"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "ESD.pool.root"

class OutputAODFileName(JobProperty):
    """Name of output AOD file"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "AOD.pool.root"    


class McEventCollection(JobProperty):
    """DetectorManager for Pixel"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TruthEvent'

class CaloClusterContainer(JobProperty):
    """Brem Reco: Container of EM clusters"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'LArClusterEM'

class CaloCellContainer(JobProperty):
    """Brem Reco: Container of calo cell"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AllCalo'

class CaloClusterROIContainer(JobProperty):
    """Brem Reco: Container of calo cluster ROIs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetCaloClusterROIs'

class HadCaloClusterContainer(JobProperty):
    """Brem Reco: Container of EM clusters"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CaloCalTopoCluster'

class HadCaloCellContainer(JobProperty):
    """Brem Reco: Container of calo cell"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AllCalo'

class HadCaloClusterROIContainer(JobProperty):
    """Brem Reco: Container of calo cluster ROIs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetHadCaloClusterROIs'

class xAODTrackParticleContainer(JobProperty):
    """xAOD TrackParticle"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetTrackParticles"

class xAODVertexContainer(JobProperty):
    """xAOD Vertex"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "PrimaryVertices"

class xAODV0VertexContainer(JobProperty):
    """xAOD Vertex V0"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "V0UnconstrVertices"

class xAODKshortVertexContainer(JobProperty):
    """xAOD Vertex Kshort"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "V0KshortVertices"

class xAODLambdaVertexContainer(JobProperty):
    """xAOD Vertex Lambda"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "V0LambdaVertices"

class xAODLambdabarVertexContainer(JobProperty):
    """xAOD Vertex Lambdabar"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "V0LambdabarVertices"

class xAODForwardTrackParticleContainer(JobProperty):
    """xAOD TrackParticle from Forward Tracking"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetForwardTrackParticles"

class xAODLowBetaTrackParticleContainer(JobProperty):
    """xAOD TrackParticle from Low Beta Finder"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetLowBetaTrackParticles"

class xAODPseudoTrackParticleContainer(JobProperty):
    """xAOD Pseduo TrackParticle"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetPseudoTrackParticles"

class xAODPixelTrackParticleContainer(JobProperty):
    """xAOD PixelTrackParticle"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetPixelTrackParticles"

class xAODSCTTrackParticleContainer(JobProperty):
    """xAOD SCTTrackParticle"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetSCTTrackParticles"

class xAODTRTTrackParticleContainer(JobProperty):
    """xAOD TRTTrackParticle"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetTRTTrackParticles"

class xAODPixelPrdAssociationTrackParticleContainer(JobProperty):
    """xAOD PixelTrackParticle with PRD association"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetPixelPrdAssociationTrackParticles"

class xAODPixelPrdAssociationExtendedTrackParticleContainer(JobProperty):
    """xAOD Extended PixelTrackParticle with PRD association"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "InDetPixelPrdAssociationExtendedTrackParticles"

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class InDetContainerKeys(JobPropertyContainer):
    """Container for the InDet flags
    """

    def lockAllExceptAlias(self):
        # Lock everything except AliasToTracks (do it this way to avoid a warning when unlocking AliasToTracks)
        print "InDetKeys: Locking all keys except aliases"
        # InDetKeys.lock_JobProperties()
        # InDetKeys.AliasToTracks.unlock()
        [k.lock() for k in InDetKeys.__dict__.values() if hasattr(k,'is_locked') and not isinstance(k,type(InDetKeys.AliasToTracks))]
        return 
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(InDetContainerKeys)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetContainerKeys container
jobproperties.InDetContainerKeys.add_JobProperty(PixelManager)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_Manager)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_Manager)
jobproperties.InDetContainerKeys.add_JobProperty(PixelRDOs)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_RDOs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_RDOs)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPURDOs)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_PU_RDOs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_PU_RDOs)
jobproperties.InDetContainerKeys.add_JobProperty(BCM_RDOs)
jobproperties.InDetContainerKeys.add_JobProperty(BCM_CompactDOs)
jobproperties.InDetContainerKeys.add_JobProperty(PixelByteStreamErrs)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_ByteStreamErrs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_ByteStreamErrs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_ByteStreamIdErrs)
jobproperties.InDetContainerKeys.add_JobProperty(GangedPixelMap)
jobproperties.InDetContainerKeys.add_JobProperty(SplitClusterAmbiguityMap)
jobproperties.InDetContainerKeys.add_JobProperty(PixelSDOs)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_SDOs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_SDOs)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPUSDOs)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_PU_SDOs)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_PU_SDOs)
jobproperties.InDetContainerKeys.add_JobProperty(PixelClusters)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_Clusters)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_DriftCircles)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPUClusters)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_PU_Clusters)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_PU_DriftCircles)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_DriftCirclesUncalibrated)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_PU_DriftCirclesUncalibrated)
jobproperties.InDetContainerKeys.add_JobProperty(PixelClustersTruth)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_ClustersTruth)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_DriftCirclesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPUClustersTruth)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_PU_ClustersTruth)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_PU_DriftCirclesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PixelSpacePoints)
jobproperties.InDetContainerKeys.add_JobProperty(SCT_SpacePoints)
jobproperties.InDetContainerKeys.add_JobProperty(OverlapSpacePoints)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_Segments)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_Segments_Phase)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_Tracks_Phase)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_SegmentsTRT)
jobproperties.InDetContainerKeys.add_JobProperty(TRT_SegmentsTRT_EC)
jobproperties.InDetContainerKeys.add_JobProperty(TRTSeededTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedTRTSeededTracks)
jobproperties.InDetContainerKeys.add_JobProperty(AliasToTracks)
jobproperties.InDetContainerKeys.add_JobProperty(Tracks)
jobproperties.InDetContainerKeys.add_JobProperty(DetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(TracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMap)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapPhase)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapSLHC)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapLowPt)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapBeamGas)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapLargeD0)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksPhase)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedSLHCTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedLowPtTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedBeamGasTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedLargeD0Tracks)
jobproperties.InDetContainerKeys.add_JobProperty(ProcessedESDTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(UnslimmedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(UnslimmedDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(UnslimmedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(RefittedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(RefittedDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(RefittedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PseudoTracks)
jobproperties.InDetContainerKeys.add_JobProperty(PseudoDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PseudoTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededPixelTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededPixelPrdAssociationTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededSCTTracks)
jobproperties.InDetContainerKeys.add_JobProperty(PixelTracks)
jobproperties.InDetContainerKeys.add_JobProperty(PixelDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PixelTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPrdAssociationTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedPixelPrdAssociationTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksPixelPrdAssociation)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedTracksMapPixelPrdAssociation)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPrdAssociationTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(PixelPrdAssociationDetailedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(SCTTracks)
jobproperties.InDetContainerKeys.add_JobProperty(TRTTracks)
jobproperties.InDetContainerKeys.add_JobProperty(TRTTracks_NewT)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSPSeedSegments)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededSLHCTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededLowPtTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededVeryLowPtTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededForwardTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededSLHCConversionFindingTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededBeamGasTracks)
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededLargeD0Tracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedSLHCTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedLowPtTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedVeryLowPtTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedForwardTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedForwardTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedForwardTrackParticles)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedSLHCConversionFindingTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedForwardTrackParticlesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedBeamGasTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedLargeD0Tracks)
jobproperties.InDetContainerKeys.add_JobProperty(HaloTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ExtendedHaloTracks)
jobproperties.InDetContainerKeys.add_JobProperty(ResolvedSiCosmicTracks)
jobproperties.InDetContainerKeys.add_JobProperty(CombinedCosmicTracks)
jobproperties.InDetContainerKeys.add_JobProperty(PrimaryVertices)
jobproperties.InDetContainerKeys.add_JobProperty(PrimaryVerticesWithoutBeamConstraint)
jobproperties.InDetContainerKeys.add_JobProperty(PrimaryVerticesSplitStream)
jobproperties.InDetContainerKeys.add_JobProperty(TrackParticles)
jobproperties.InDetContainerKeys.add_JobProperty(TrackParticlesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(IPatPrimaryVertices)
jobproperties.InDetContainerKeys.add_JobProperty(IPatParticles)
jobproperties.InDetContainerKeys.add_JobProperty(IPatParticlesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(IPatConvertedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(IPatConvertedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(IPatConvertedTracksDetailedTruth)
jobproperties.InDetContainerKeys.add_JobProperty(XKalPrimaryVertices)
jobproperties.InDetContainerKeys.add_JobProperty(XKalParticles)
jobproperties.InDetContainerKeys.add_JobProperty(XKalParticlesTruth)
jobproperties.InDetContainerKeys.add_JobProperty(XKalConvertedTracks)
jobproperties.InDetContainerKeys.add_JobProperty(XKalConvertedTracksTruth)
jobproperties.InDetContainerKeys.add_JobProperty(XKalConvertedTracksDetailedTruth)
jobproperties.InDetContainerKeys.add_JobProperty(V0Candidates)
jobproperties.InDetContainerKeys.add_JobProperty(SimpleV0Candidates)
jobproperties.InDetContainerKeys.add_JobProperty(SecVertices)
jobproperties.InDetContainerKeys.add_JobProperty(Conversions)
jobproperties.InDetContainerKeys.add_JobProperty(LowBetaCandidates)
jobproperties.InDetContainerKeys.add_JobProperty(trkValidationNtupleName)
jobproperties.InDetContainerKeys.add_JobProperty(trkD3PDFileName)
jobproperties.InDetContainerKeys.add_JobProperty(StandardPlotHistName)
jobproperties.InDetContainerKeys.add_JobProperty(PhysValMonHistName)
jobproperties.InDetContainerKeys.add_JobProperty(OutputRDOFileName)
jobproperties.InDetContainerKeys.add_JobProperty(OutputESDFileName)
jobproperties.InDetContainerKeys.add_JobProperty(OutputAODFileName)
jobproperties.InDetContainerKeys.add_JobProperty(McEventCollection)
jobproperties.InDetContainerKeys.add_JobProperty(CaloClusterContainer)
jobproperties.InDetContainerKeys.add_JobProperty(CaloCellContainer)
jobproperties.InDetContainerKeys.add_JobProperty(CaloClusterROIContainer)
jobproperties.InDetContainerKeys.add_JobProperty(HadCaloClusterContainer)
jobproperties.InDetContainerKeys.add_JobProperty(HadCaloCellContainer)
jobproperties.InDetContainerKeys.add_JobProperty(HadCaloClusterROIContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODVertexContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODV0VertexContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODKshortVertexContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODLambdaVertexContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODLambdabarVertexContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODForwardTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODLowBetaTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODPseudoTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODPixelTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODSCTTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODTRTTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODPixelPrdAssociationTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(xAODPixelPrdAssociationExtendedTrackParticleContainer)
jobproperties.InDetContainerKeys.add_JobProperty(DBMDetailedTracksTruth) 
jobproperties.InDetContainerKeys.add_JobProperty(DBMTracksTruth) 
jobproperties.InDetContainerKeys.add_JobProperty(SiSpSeededDBMTracks) 
jobproperties.InDetContainerKeys.add_JobProperty(DBMTracks) 
jobproperties.InDetContainerKeys.add_JobProperty(xAODDBMTrackParticleContainer) 
jobproperties.InDetContainerKeys.add_JobProperty(DBMTrackParticles)
jobproperties.InDetContainerKeys.add_JobProperty(DBMTrackParticlesTruth)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select InDetContainerKeys as a short name as well.
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from InDetRecExample.InDetKeys import InDetKeys
InDetKeys = jobproperties.InDetContainerKeys
