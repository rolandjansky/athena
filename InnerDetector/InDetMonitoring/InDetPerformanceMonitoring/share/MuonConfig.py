from MuonRecExample.MuonRecFlags import muonRecFlags

#--------------------------------------------------------------------------------
# Input
#--------------------------------------------------------------------------------
# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup

#import MuonCombinedRecExample.MuonCombinedRecOnlySetup 
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
rec.doWriteAOD=True

#Disables PFO Thnning - these cotnainers do not exist, unless you run jet finding
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)

muonRecFlags.doStandalone.set_Value_and_Lock(True)
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doCSCs                = True
muonRecFlags.doTGCs                = False

muonStandaloneFlags.printSummary         = True
muonCombinedRecFlags.doTrackPerformance  = True
muonCombinedRecFlags.doMuGirl            = True
muonCombinedRecFlags.printSummary        = True


##### no more flags after this line #####
try:
    print (' == runzmumu == muonconfig == include("MuonRecExample/MuonRec_topOptions.py")')
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####


    ##### DO NOT ADD ANYTHING AFTER THIS LINE #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    traceback.print_exc()
    
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

