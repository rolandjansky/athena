doTrigger = True

TriggerModernConfig = True

doTruth=False
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False
doESD=False
doTAG=False
doCBNT=False
useROOTNtuple=True

OutputLevel=WARNING
LVL1OutputLevel=WARNING
HLTOutputLevel=WARNING

EvtMax=3

PoolRDOInput = [
"/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/valid3.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s472_RDO.025026._00004.pool.root.1"]
#"/tmp/klemetti/dijets_J8.RDO.pool.root"]
#"/tmp/klemetti/user.SethCaughron.misal1_mc12.EnhancedBias_1031.digit.RDO.v13004004.RDO._00001.pool.root"]
#"/scratch/dufourma/test_sample/dijets_J8/dijets_J8.RDO.pool.root"]
#DetDescrVersion="ATLAS-CSC-01-02-00"  #old default
DetDescrVersion="ATLAS-GEO-02-00-00"   #new default

include ("RecExCommon/RecExCommon_flags.py")
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.Slices_all_setOff()
TriggerFlags.doLVL1=True
TriggerFlags.doLVL2=True
TriggerFlags.doEF=True
TriggerFlags.disableRandomPrescale=True

#TriggerFlags.triggerMenuSetup = 'lumi1E31'
#TriggerFlags.triggerMenuSetup = 'Physics_lumi1E31'
TriggerFlags.triggerMenuSetup = 'MC_lumi1E31'
TriggerFlags.L1PrescaleSet = 'L1Prescales100_Physics_lumi1E31'
TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_Physics_lumi1E31'

TriggerFlags.Lvl1.RemapThresholdsAsListed = True
def addAlwaysAcceptChain():
    TriggerFlags.MinBiasSlice.enableSignature("always")
from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)

include("RecExCommon/RecExCommon_topOptions.py")
include("TriggerRateTools/TriggerRateTools_common.py")

ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
Service ("StoreGateSvc" ).ActivateHistory=False  #less info is faster

triggerRateTools.OutputLevel = DEBUG
triggerRateTools.xSection = 1.0 # barns
triggerRateTools.Luminosity = 1.0 # barns^-1 sec^-1
#triggerRateTools.collsPerEvent = 5.1 #for pileup
triggerRateTools.doSlicesInfo = True
triggerRateTools.doRawTD = True
triggerRateTools.doTextOutput = True
#triggerRateTools.ignorePassThrough = True
triggerRateTools.doRetrieveItems = True
