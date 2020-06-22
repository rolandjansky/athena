# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags

## flag to set number of events to be processed
EvtMax = -1

## flag for running in mig5
doMig5 = False

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from MuonRecExample import MuonRecUtils

#Need the beam spot for the TrackParticleCreator
if not ('conddb' in dir()):
    IOVDbSvc = Service("IOVDbSvc")
    from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
   from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
   condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )


#--------------------------------------------------------------------------------
# Input
#--------------------------------------------------------------------------------
if not 'InputRdoFile' in dir():
    InputRdoFile="root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/14/a2/user.zhidong.12100112.EXT0._000001.RDO.pool.root"
athenaCommonFlags.FilesInput = [InputRdoFile]

#--------------------------------------------------------------------------------
# Output
#--------------------------------------------------------------------------------
rec.doTruth=True
rec.doTrigger = False

muonRecFlags.doStandalone.set_Value_and_Lock(True)
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doCSCs                = True
muonRecFlags.doCalibNtuple = False # do not write calibration ntuples

# flags to tweak standalone muon reconstruction
if doMig5:
    muonStandaloneFlags.doSegmentsOnly       = False
    muonStandaloneFlags.patternsOnly         = False
    muonStandaloneFlags.createTrackParticles = False

muonStandaloneFlags.printSummary         = True
muonCombinedRecFlags.doTrackPerformance  = True
muonCombinedRecFlags.doMuGirl            = True
muonCombinedRecFlags.printSummary        = True


##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####

    ###### put any user finetuning before this line #####

##### DO NOT ADD ANYTHING AFTER THIS LINE #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print ('INFO: MuonRec_myTopOptions.py - stack trace:')
    print (traceback.format_exc().rstrip())
    
    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    import sys
    sys.exit(10)
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
