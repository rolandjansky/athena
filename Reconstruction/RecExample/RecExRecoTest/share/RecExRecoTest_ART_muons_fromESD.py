#Based on MuonRecExample/MuonRec_myTopOptions.py
from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags

## flag to set number of events to be processed
EvtMax = 10

#--------------------------------------------------------------------------------
# Input
#--------------------------------------------------------------------------------
athenaCommonFlags.FilesInput = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup

#import MuonCombinedRecExample.MuonCombinedRecOnlySetup 
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from MuonRecExample import MuonRecUtils
from MuonRecExample.MuonRecUtils import assertCastorStager,hasJobPropertyBeenSet

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

#ToolSvc += MDTCablingDbTool 
import MuonCnvExample.MuonCablingConfig

#--------------------------------------------------------------------------------
# Output
#--------------------------------------------------------------------------------
muonRecFlags.doCalibNtuple = False # write calibration ntuple?
###

#Set to false, because re-reco of muon truth does not work. Lock it because downstream python alters its value to True otherwise.
rec.doTruth.set_Value_and_Lock(False)

rec.doTrigger = False

muonRecFlags.doStandalone.set_Value_and_Lock(True)
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doCSCs                = True
muonRecFlags.doNSWNewThirdChain    = True

muonStandaloneFlags.printSummary         = True
muonCombinedRecFlags.doTrackPerformance  = True
muonCombinedRecFlags.doMuGirl            = True
muonCombinedRecFlags.printSummary        = True


##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####


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
