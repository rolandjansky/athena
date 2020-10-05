# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__doc__ = """Flags to steer Muon Combined Reconstruction."""

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.Logging import logging
from AthenaCommon import BeamFlags
from AthenaCommon import BFieldFlags
beamFlags = jobproperties.Beam

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

from MuonRecExample.MuonRecFlags import muonRecFlags,muonStandaloneFlags
from MuonRecExample.MuonRecUtils import logMuon,fillJobPropertyContainer,override_lock_and_set_Value,SummaryJobProperty
from InDetRecExample.InDetJobProperties import InDetFlags

logMuonComb = logging.getLogger("MuonCombinedRec")

#
# Flags for general use
#

class printConfigurables(JobProperty):
    """if this is on the all the print ThisTool lines are activated"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
# Switch on/off algorithms for pre-processing
#
class doCosmicSplitTracks(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doMuGirl(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


#
# Switch on/off algorithms that make Muons for the new chain based on muon standalone tracking
#

class doCombinedFit(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doStatisticalCombination(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonSegmentTagger(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
# 'silicon-associated'muons, or muons which rely on special ID reconstruction because they're outside the usual acceptance.
#
class doSiAssocForwardMuons(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
# Switch on/off algorithms that make Muons for the CaloMuonCollection
#

class doCaloTrkMuId(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True



#
# Switch on/off algorithms that make Muons for the MuGirlLowBetaMuonCollection
#

class doMuGirlLowBeta(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


#
# Switch on/off algorithms that do not end up in any MuonCollection
#

class doLArMuId(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doAOD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doxAOD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# @brief run truth-matching on tracks to evaluate tracking performance
#
# It will enable matching for the Combined Muon algorithms that are running
# It uses MuonTrackPerformanceAlg
class doTrackPerformance(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief run the detailed pixel hole search
#
class useDetailedPixelHoleSearch(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Set the summary level of the TrackPerformance algorithm.
#
# Only active if not doing truth matching.
# Meaning of values:
# 0,1: no summary
# 2: summary per event
class TrackPerfSummaryLevel(JobProperty):
    statusOn=True
    allowedTypes=['int']
    StoredValue=1

# @brief Set the debug level of the TrackPerformance algorithm.
#
# Meaning of values:    
# 1: summary at the end (in finalize)
# 4: summary for event with a problem (in execute)
# 5: summary for all tracks (in execute)
# 6: also print out the hits (in execute)
class TrackPerfDebugLevel(JobProperty):
    statusOn=True
    allowedTypes=['int']
    StoredValue=4

## Print out a summary for each event at each reco stage
class printSummary(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class createTagAndProbeInput(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class createScaleCalibrationInput(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

 

## The container with all the flags to steer MuonCombined reconstruction
class MuonCombinedRec(JobPropertyContainer):
    """The flags to steer muon combined reconstruction"""

    def setDefaults(self):
        global beamFlags,muonRecFlags,muonStandaloneFlags
        # set defaults of the flags that its using
        muonRecFlags.setDefaults()

        muonStandaloneFlags.setDefaults()

        from MuonRecExample.MuonRecUtils import setJobPropertyDefault as setDefault
        
        haveMuonTracks    = muonRecFlags.doStandalone() and self.doAOD
# This logic doesn't work - doAOD seems to be off with Vakho's standard test
#        if not haveMuonTracks:
#            if not muonRecFlags.doStandalone():
#                print "turning off doxAOD because doStandalone is off"
#            if not self.doAOD:
#                print "turning off doxAOD because doAOD is off"
#            setDefault(self.doxAOD,False)

        if beamFlags.beamType()=='cosmics' or beamFlags.beamType()=='singlebeam':
            # Algorithms for CaloMuonCollection
            setDefault(self.doCaloTrkMuId,self.doAOD())
            # Algorithms for MuGirlLowBetaCollection
            setDefault(self.doMuGirlLowBeta,self.doAOD())

        #else: # collisions
            # Algorithms for CaloMuonCollection
            #setDefault(self.doCaloTrkMuId,self.doAOD())
            # Algorithms for MuGirlLowBetaCollection
            #setDefault(self.doMuGirlLowBeta,self.doAOD())
            #setDefault(self.doMuGirl,self.doAOD())    # also for new MuonCollection

        if beamFlags.beamType()=='cosmics':
            setDefault(self.doLArMuId,True)
        else:
            setDefault(self.doLArMuId,False)


        # tracking performance algorithms, take defaults from muonRecFlags
        setDefault(self.doTrackPerformance,    muonRecFlags.doTrackPerformance())
        setDefault(self.TrackPerfDebugLevel,   muonRecFlags.TrackPerfDebugLevel())
        setDefault(self.TrackPerfSummaryLevel, muonRecFlags.TrackPerfSummaryLevel())

        if not self.doMuGirl(): setDefault(self.doMuGirlLowBeta, False)

    def doAnyMuons(self):
        """Are we running any algorithm that produces Analysis::MuonContainer? Read-only."""
        return self.doMuonCollection() or self.doMuGirlLowBetaMuonCollection()


    def doMuonClusters(self):
        """Call InitializeMuonClusters and FinalizeMuonClusters or not.
        Read-only based on other flags"""
        return self.doAnyMuons()

        

    

jobproperties.add_Container(MuonCombinedRec)

muonCombinedRecFlags = jobproperties.MuonCombinedRec


# fill properties in container and store for later use (due to lack of iterator)
_propertiesList = fillJobPropertyContainer(muonCombinedRecFlags,__name__)

#
# Add some summary properties to read/write a set of properties
#



## @brief A summary flag for the algorithms that produce muons that go into the MuGirlLowBetaMuonCollection.
#
# It can enable/disable all the flags of the associated algorithms in one go,
# and reads back the logical OR of all of the flag's StoredValues.
class doMuGirlLowBetaMuonCollection(SummaryJobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

    _properties = [ muonCombinedRecFlags.doMuGirlLowBeta ]

muonCombinedRecFlags.add_JobProperty(doMuGirlLowBetaMuonCollection)

## @brief A summary flag for the new collection
#
# It can enable/disable all the flags of the associated algorithms in one go,
# and reads back the logical OR of all of the flag's StoredValues.
class doMuonCollection(SummaryJobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

    _properties = [ muonCombinedRecFlags.doCombinedFit,
                    muonCombinedRecFlags.doStatisticalCombination,
                    muonCombinedRecFlags.doMuGirl, # can't use it, interferes with doMuidCollection
                    muonCombinedRecFlags.doMuonSegmentTagger ]

muonCombinedRecFlags.add_JobProperty(doMuonCollection)



## @brief Enable or disable all MuonCombined algorithms in one go
class Enabled(SummaryJobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
    _properties = [ muonCombinedRecFlags.doMuonCollection,
                    muonCombinedRecFlags.doMuGirlLowBetaMuonCollection,
                    muonCombinedRecFlags.doLArMuId,
                    muonCombinedRecFlags.doAOD,
                    muonCombinedRecFlags.doxAOD,
                    muonCombinedRecFlags.doTrackPerformance,
                    muonCombinedRecFlags.doSiAssocForwardMuons,
                    ]

muonCombinedRecFlags.add_JobProperty(Enabled)




muonCombinedRecFlags.setDefaults()
#muonCombinedRecFlags.sync_with_RecExCommon()
