# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file StandaloneFlags.py Flags for configuring the %Standalone muon reconstruction algorithm

## @namespace python::StandaloneFlags @copydoc StandaloneFlags.py

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from MuonRecExample.MuonRecUtils import fillJobPropertyContainer
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

## Print out a summary for each event at each reco stage
class printSummary(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class segmentOrigin(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="Muon"
    allowedValues=['Muon','TruthTracking']

class reconstructionMode(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="collisions"
    allowedValues=['collisions','cosmics']

class strategy(JobProperty):
    """ Allowed options for the strategy currently are:
       CutSeedsOnTracks
       CombineSegInStation
       DynamicSeeding
       PreferOutsideIn
       AllowOneSharedHit
       DoRefinement
       DoAmbiSolving
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

class trackBuilder(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="Moore"
    allowedValues=['Moore','TruthTracking','None']
            
class refinementTool(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="Moore"
    allowedValues=['Moore']

class optimiseMomentumResolutionUsingChi2(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class patternsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class createTrackParticles(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class straightLineFitMomentum(JobProperty):
    statusOn=True
    allowedTypes=['float']
    StoredValue=2000.0

## stop reconstruction at segment level
class doSegmentsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## chi-squared per degree of freedom cut in fitter.
class Chi2NDofCut(JobProperty):
    statusOn=True
    allowedTypes=['float']
    StoredValue=20.

## flag to turn on curved segment finding
class enableCurvedSegmentFinding(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class updateSegmentSecondCoordinate(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class useSegmentMatching(JobProperty) :
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class useTrackSegmentMatching(JobProperty) :
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Container with Configuration flags of %Standalone reconstruction
class MuonStandalone(JobPropertyContainer):
    ##set defaults of the flags depending on type of input (MC, data, cosmics etc.)
    def setDefaults(self):
        from MuonRecUtils import setJobPropertyDefault as setDefault

        global beamFlags
        if beamFlags.beamType()=='cosmics' or beamFlags.beamType()=='singlebeam':
            setDefault(self.reconstructionMode,'cosmics')
            setDefault(self.useSegmentMatching,False)
            setDefault(self.updateSegmentSecondCoordinate,False)
        else:
            setDefault(self.useSegmentMatching,True)
            setDefault(self.updateSegmentSecondCoordinate,True)

# add flags to container
jobproperties.add_Container(MuonStandalone)

## shortcut to access flags
muonStandaloneFlags = jobproperties.MuonStandalone

# add all properties in this module
fillJobPropertyContainer(muonStandaloneFlags,__name__)

# set defaults for container
muonStandaloneFlags.setDefaults()

MoorelikeStrategy=[
     "BarrelCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:BML,BMS;BOL,BOS;BIL,BIS",
     "EndcapCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:EML,EMS;EOL,EOS;EIL,EIS;CSL,CSS;EEL,EES;BEE",
     "BarrelEndcap[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks,BarrelEndcapFilter]:EML,EMS;EEL,EES;BML,BMS;EIL,EIS;BIL,BIS;BOL,BOS"]
