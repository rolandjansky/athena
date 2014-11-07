# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file MooreFlags.py Flags for configuring the %Moore muon reconstruction algorithm

## @namespace python::MooreFlags @copydoc MooreFlags.py


from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from MuonRecExample.MuonRecUtils import fillJobPropertyContainer
from AthenaCommon import BeamFlags
from AthenaCommon.GlobalFlags import globalflags

# shortCut
beamFlags = jobproperties.Beam

## Print out a summary for each event at each reco stage
class printSummary(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class addEndcapMiddleSingleStationTracks(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class materialSourcePatRec(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="TGMat"

class materialSourceFinalFit(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="TGMat"

class useSegmentMatching(JobProperty) :
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class useTrackSegmentMatching(JobProperty) :
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class optimiseMomentumResolutionUsingChi2(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class straightLineFitMomentum(JobProperty):
    statusOn=True
    allowedTypes=['float']
    StoredValue=5000.0

## stop reconstruction at segment level
class doSegmentsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## stop reconstruction at segment level
class updateSegmentSecondCoordinate(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## stop reconstruction at segment level
class rescaleCscClustersInTrackFinding(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## stop reconstruction at segment level
class useNewSteering(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

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


## flag to enable new Moore
class doNewMoore(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## track building strategy (only for New Moore)
#  Default value copied on (30-01-2012) from MuonStandaloneFlags.MoorelikeStrategy to make it independent from 3rd chain
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
    StoredValue=[
     "BarrelCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:BML,BMS;BOL,BOS;BIL,BIS",
     "EndcapCombined[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:EML,EMS;EOL,EOS;EIL,EIS;CSL,CSS;EEL,EES;BEE",
     "BarrelEndcap[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:EML,EMS;BIL,BIS",
     "EMandEE[CombineSegInStation,DoRefinement,DoAmbiSolving,CutSeedsOnTracks]:EML,EMS;EEL,EES"]
    

## Container with Configuration flags of %Moore reconstruction
class Moore(JobPropertyContainer):
    ##set defaults of the flags depending on type of input (MC, data, cosmics etc.)
    def setDefaults(self):
        from MuonRecUtils import setJobPropertyDefault as setDefault
        global beamFlags
        setDefault(self.Chi2NDofCut,20.0)
        setDefault(self.straightLineFitMomentum,2000.0)
        if beamFlags.beamType()=='cosmics' or beamFlags.beamType()=='singlebeam':
            setDefault(self.addEndcapMiddleSingleStationTracks,False)
            setDefault(self.useSegmentMatching,False)
            setDefault(self.updateSegmentSecondCoordinate,False)
        else:
            setDefault(self.addEndcapMiddleSingleStationTracks,True)
            setDefault(self.straightLineFitMomentum,2000.0)
            setDefault(self.useSegmentMatching,True)
            setDefault(self.updateSegmentSecondCoordinate,True)


            

# add moore flags to container
jobproperties.add_Container(Moore)

## shortcut to access flags
mooreFlags = jobproperties.Moore

# add all properties in this module
fillJobPropertyContainer(mooreFlags,__name__)

# set defaults for container
mooreFlags.setDefaults()
