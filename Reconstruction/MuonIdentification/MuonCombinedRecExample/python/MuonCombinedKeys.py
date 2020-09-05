# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

##
## @file MuonCombinedRecExample/python/MuonCombinedKeys.py
## @brief Python module to hold uniform collection names for muon combined reconstruction
##

from __future__ import print_function

""" MuonCombinedContainerKeys
    Python module to hold storegate keys of MuonCombined objects.

"""

__author__ = "W. Liebig"
__version__= "$Revision: 0.00 $"
__doc__    = "MuonCombinedContainerKeys"

__all__    = [ "MuonCombinedKeys" ]

# analogue to InDetKeys from A. Wildauer

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

## classes for inner detector inputs
class InDetTrackParticles(JobProperty):
    """StoreGate key for ID track particles"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleCandidate'

class InDetTrackParticlesLargeD0(JobProperty):
    """StoreGate key for ID track particles with large radius tracking"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrackParticleCandidateLRT'

## classes for track collections
class SpectrometerTracks(JobProperty): # clarify with MS if to be replaced by jobPr. in MuonRecExample
    """StoreGate key for tracks from MS standalone tracking expressed only in MS"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonSpectrometerTracks'

class ExtrapolatedMSTracks(JobProperty):
    """StoreGate key for tracks from MS standalone tracking expressed at IP and eventually refined using IP constraint"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtrapolatedMuonTracks'

class CombinedFitTracks(JobProperty):
    """StoreGate key for tracks from combined fit"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedMuonTracks'

class CombinedFitTracksLargeD0(JobProperty):
    """StoreGate key for tracks from combined fit using large d0 inner tracking particles"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedMuonTracksLRT'

class RefittedExtrapolatedMSTracks(JobProperty):
    """StoreGate key for tracks with refitted extrapolation using the MS part of the combined track"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'RefittedExtrapolatedMuonSpectrometerTracks'

class StatCombinedTracks(JobProperty):
    """StoreGate key for track collection from statistical combination"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'StatCombinedMuonTracks'

## classes for track particle collections
class SpectrometerParticles(JobProperty):
    """StoreGate key for trackparticles from MS standalone tracking, expressed only in MS"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonSpectrometerTrackParticles'

class ExtrapolatedMSParticles(JobProperty):
    """StoreGate key for trackparticles from MS standalone tracking, expressed at IP"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ExtrapolatedMuonTrackParticles'

class ExtrapolatedMSOnlyParticles(JobProperty):
    """StoreGate key for trackparticles from MS standalone tracking, expressed at IP"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MSOnlyExtrapolatedMuonTrackParticles'

class ExtrapolatedMSOnlyTracks(JobProperty):
    """StoreGate key for trackparticles from MS standalone tracking, expressed at IP"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MSOnlyExtrapolatedTracks'

class CombinedFitParticles(JobProperty):
    """StoreGate key for trackparticles from combined muon track fit """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedMuonTrackParticles'

class RefittedExtrapolatedMSParticles(JobProperty):
    """StoreGate key for trackparticles with refitted extrapolation using the MS part of the combined track"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'RefittedExtrapolatedMuonSpectrometerParticles'

class StatCombinedParticles(JobProperty):
    """StoreGate key for trackparticles from statistical combination"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'StatCombinedMuonParticles'

## classes for internal helper objects
class MuidInternalMatches(JobProperty):
    """StoreGate key for SG container of MuidMatches"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuidInternalMatches'

## classes for muon collections (per identification alg)
class StandaloneMuons(JobProperty):
    """StoreGate key for muon container from standalone muons"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'BackExtrapolatedMuons'

class CombinedFitMuons(JobProperty):
    """StoreGate key for muon container from combined fit"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedFitMuons'

class StatCombinedMuons(JobProperty):
    """StoreGate key for muon container from statistical combination"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'StatCombinedMuons'

class MuGirlMuons(JobProperty):
    """StoreGate key for muon container from MuGirl algorithm"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuGirlCombinedMuonContainer'


## final muon collections
class ESD_Muons(JobProperty):
    """StoreGate key for detailed muon collection prior to overlap removal"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'DetailedMuonCollection'

class FinalMuons(JobProperty):
    """StoreGate key for final muon collection after overlap removal and dressing"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Muons'

class FinalMuonsLargeD0(JobProperty):
    """StoreGate key for final muon collection with large radius InDetTracks after overlap removal and dressing"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonsLRT'


class FinalCaloMuons(JobProperty):
    """StoreGate key for final muon collection from calorimeter tagging"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CaloMuonCollection'

class LowBetaMuons(JobProperty):
    """StoreGate key for final muon collection from (MuGirl) low beta reconstruction"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuGirlLowBetaCollection'

class SpaceTimePoints(JobProperty):
    """StoreGate key for SpaceTimePoints"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonSpaceTimePoints'

class SegmentTagTracks(JobProperty):
    """StoreGate key for track collection from SegmentTag identification"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SegmentTagTracks'

class SegmentTagTrackParticles(JobProperty):
    """StoreGate key for trackparticles from SegmentTag identification"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SegmentTagTrackParticles'

class SegmentTagMuons(JobProperty):
    """StoreGate key for muon container from SegmentTag identification"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'SegmentTagMuons'

class SegmentTagMuonSegments(JobProperty):
    """StoreGate key for Muon Segments from MS standalone tracking"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonSegments'

class InternalMuTagTracksContainer(JobProperty):
    """StoreGate key for Internal MuTag Candidates"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InternalMuTagTracksContainer'



##-----------------------------------------------------------------------------
## 2nd step
## Definition of the MuonCombined flag container
class MuonCombinedContainerKeys(JobPropertyContainer):
    """Job Properties for the MuonCombined reco StoreGate keys
    """

    def lockAllExceptAlias(self):
        # Lock everything except one thing (do it this way to avoid a warning when unlocking one thing)
        print("MuonCombinedKeys: Locking all keys except one thing")
        # MuonCombinedKeys.lock_JobProperties()
        [k.lock() for k in MuonCombinedKeys.__dict__.values() if hasattr(k,'is_locked') and not isinstance(k,type(MuonCombinedKeys.OneThing))]
        return
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(MuonCombinedContainerKeys)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the MuonCombinedContainerKeys container
jobproperties.MuonCombinedContainerKeys.add_JobProperty(InDetTrackParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SpectrometerTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(ExtrapolatedMSTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(CombinedFitTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(RefittedExtrapolatedMSTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(StatCombinedTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SpectrometerParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(ExtrapolatedMSParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(ExtrapolatedMSOnlyParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(CombinedFitParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(RefittedExtrapolatedMSParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(StatCombinedParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(MuidInternalMatches)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(StandaloneMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(CombinedFitMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(StatCombinedMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(MuGirlMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(ESD_Muons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(FinalMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(FinalCaloMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(LowBetaMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SpaceTimePoints)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SegmentTagTracks)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SegmentTagTrackParticles)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SegmentTagMuons)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(SegmentTagMuonSegments)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(InternalMuTagTracksContainer)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(ExtrapolatedMSOnlyTracks)
# Large d0 tracking muons
jobproperties.MuonCombinedContainerKeys.add_JobProperty(FinalMuonsLargeD0)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(InDetTrackParticlesLargeD0)
jobproperties.MuonCombinedContainerKeys.add_JobProperty(CombinedFitTracksLargeD0)



##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select MuonCombinedContainerKeys as a short name as well.
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys
MuonCombinedKeys = jobproperties.MuonCombinedContainerKeys
