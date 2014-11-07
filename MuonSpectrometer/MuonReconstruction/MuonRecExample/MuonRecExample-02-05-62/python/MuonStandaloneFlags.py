# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file StandaloneFlags.py Flags for configuring the %Standalone muon reconstruction algorithm

## @namespace python::StandaloneFlags @copydoc StandaloneFlags.py

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from MuonRecExample.MuonRecUtils import fillJobPropertyContainer
from AthenaCommon.GlobalFlags import globalflags

## Print out a summary for each event at each reco stage
class printSummary(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class segmentOrigin(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="Muon"
    allowedValues=['Muon','Muonboy','Moore','TruthTracking']

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
    allowedValues=['Muonboy','Moore','TruthTracking','None']
            
class refinementTool(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="Moore"
    allowedValues=['Muonboy','Moore']

class overwriteMoore(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class overwriteMuonboy(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class materialSourcePatRec(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="TGMat"

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

## stop reconstruction at segment level
class doSegmentsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Container with Configuration flags of %Standalone reconstruction
class MuonStandalone(JobPropertyContainer):
    ##set defaults of the flags depending on type of input (MC, data, cosmics etc.)
    def setDefaults(self):
        from MuonRecUtils import setJobPropertyDefault as setDefault

# add moore flags to container
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

MuonboylikeStrategy=[
     "BarrelCombinedLarge[DoRefinement,DoAmbiSolving]:BML;BOL;BIL" ,
     "BarrelCombinedSmall[DoRefinement,DoAmbiSolving]:BMS;BOS;BIS" ,
     "EndcapCombinedLarge[DoRefinement,DoAmbiSolving]:EML;EOL;EIL;CSL;EEL;BEE" ,
     "EndcapCombinedSmall[DoRefinement,DoAmbiSolving]:EMS;EOS;EIS;CSS;EES;BEE" ,
     "BarrelEndcapLarge[DoRefinement,DoAmbiSolving]:EML;BIL" ,
     "BarrelEndcapSmall[DoRefinement,DoAmbiSolving]:EMS;BIS" ,
     "EndcapBarrelLS[DoRefinement,DoAmbiSolving]:EML;EIS;BIS",
     "EndcapBarrelSL[DoRefinement,DoAmbiSolving]:EMS;EIL;BIL"]
