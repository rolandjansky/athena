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

#import MuonCombinedRecExample.MuonCombinedRecOnlySetup 
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from MuonRecExample import MuonRecUtils
from MuonRecExample.MuonRecUtils import assertCastorStager,hasJobPropertyBeenSet
#--------------------------------------------------------------------------------
# Input
#--------------------------------------------------------------------------------
athenaCommonFlags.FilesInput = [
"root://eosatlas.cern.ch///eos/atlas/atlasdatadisk/rucio/valid1/18/60/RDO_TRIG.10625506._000001.pool.root.1"
]

if not hasJobPropertyBeenSet(athenaCommonFlags.FilesInput):
    athenaCommonFlags.FilesInput = MuonRecUtils.FileList.readDirectory("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/Zmumu_15616/")
#    assertCastorStager("castoratlast3","atlascerngroupdisk")
#    athenaCommonFlags.FilesInput = MuonRecUtils.FileList.readDirectory("rfio:/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/Zmumu_15616/")

#--------------------------------------------------------------------------------
# Output
#--------------------------------------------------------------------------------
#rec.doPerfMon = True
#rec.doDetailedAuditor = True
#rec.doWriteESD = True
muonRecFlags.doCalibNtuple = False # write calibration ntuple?
#muonRecFlags.calibNtupleSegments = False # write segments to ntuple?
#muonRecFlags.calibNtupleTracks = False # write tracks to ntuple?
#muonRecFlags.calibNtupleTrigger = False # write trigger info to ntuple?
###

#rec.doNameAuditor = True
#muonRecFlags.doVP1 = True    # Decide whether to run Virtual Point1 graphical event display
rec.doTruth=True

#rec.doTrigger = True

# Read geometry alignment corrections from DB
#muonRecFlags.useAlignmentCorrections = True
rec.doTrigger = False
#recFlags.doTruth.set_Value_and_Lock(False)
muonRecFlags.doStandalone.set_Value_and_Lock(True)
# Read geometry alignment corrections from DB
#muonRecFlags.useAlignmentCorrections = True
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doCSCs                = True
muonRecFlags.doNSWNewThirdChain    = True

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

    #from MuonTestEDM.MuonTestEDMConf import MuonTestEDM
    #MyEDMTester = MuonTestEDM(DoDumpPRDs=True, DoDumpTracks=False, DoDumpRDOs=True, DoDumpSegments=False)
    #topSequence += MyEDMTester
        
    #if not 'DumpFileName' in dir():
    #  DumpFileName="ReadBS"
    
    #MyEDMTester.RdoDumpFileName    = DumpFileName+".rdo.log"
    #MyEDMTester.PrdDumpFileName    = DumpFileName+".prd.log"
    #MyEDMTester.TrackDumpFileName  = DumpFileName+".track.log"
    #MyEDMTester.TrackParticleDumpFileName  = DumpFileName+".trackParticle.log"
    #MyEDMTester.SegmentDumpFileName= DumpFileName+".segment.log"
    #MyEDMTester.SummaryDumpFileName= DumpFileName+".summary.log" 

    ###### put any user finetuning before this line #####

##### DO NOT ADD ANYTHING AFTER THIS LINE #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print traceback.format_exc().rstrip()
    
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
