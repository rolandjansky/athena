##############################################################
# testHLT_standalone.py, updated for 13.X.0
#
#   jobOption file to run HLT standalone (independant of RecExCommon)
#   with Modern python configuration 
#
# Input could be Pool or BS
#
# Input file:
#   Pool: set by athenaCommonFlags.PoolRDOInput
#   BS:   set by athenaCommonFlags.BSRDOInput
#
# L1 configuration is fixed
#   set by TriggerFlags.inputLVL1configFile
#
# HLT configuration uses Python configuration
#
# Usage:
#   run with BS input (default): athena.py -s testHLT_standalone.py &>! testReadBS.log  &
#   run with Pool input: athena.py -s -c ReadBS=False testHLT_standalone.py &>! testTransBS.log  &
#   run LVL2 only: athena.py -s -c doEF=False testHLT_standalone.py &>! testReadBS_LVL2.log  &
#  
#   run with athenaMT:
#      ln -sf /afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.May31.Rel14.2.0.rel6.LB0000.Athena._0001.data Raw.data
#      athenaMT -s -l DEBUG -f Raw.data -n 10 testHLT_standalone.py &>! testMT.log  &
#      or, for writing BS output:
#      athenaMT -s -l DEBUG -f Raw.data -n 10 -o Electron testHLT_standalone.py&>! testMT.log &
#      or, with online THistSvc
#      athenaMT -M -c isOnline=True -l DEBUG -n 10 testHLT_standalone.py &>! testMT.log  &
#
#   run with athenaPT:
#      ln -sf Electron_0001.data athenaPT.input.data
#      athenaPT -s -l DEBUG -f athenaPT.input.data -n 10 testHLT_standalone.py &>! testPT.log  &
#
#   run with EF only with athena:
#      ln -sf Electron_0001.data doEFOnly.input.data
#      athena.py -s -c doLVL2=False testHLT_standalone.py &>! testReadBS_EF.log  &
#
# Select slice(s) to test:
#   set one or more of the following flags to True in the jo:
#      testElectron, testPhoton, testMuon, testTau, testJet, testBjet, testMET, testBPhysics
#
# Contact: Xin.Wu@cern,ch
#===========================================================================================
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if (not "useLumi1E32" in dir()):
    useLumi1E32=False
if not "useLumi1E31" in dir():
    if not useLumi1E32:
        useLumi1E31=True
    else:
        useLumi1E31=False

if (not "isOnline" in dir()):
    acf.isOnline=False
   
if not ('wideTiming' in dir()):
    wideTiming=False

if (not "alwaysAccept" in dir()):
    alwaysAccept=False

if (not "enableFPE" in dir()):
    enableFPE=False


if not ('EvtMax' in dir()):
    acf.EvtMax=10
if not ('SkipEvents' in dir()):
    acf.SkipEvents=0
if not ('OutputLevel' in dir()):
    rec.OutputLevel=DEBUG
if not ('HLTOutputLevel' in dir()):
    HLTOutputLevel=DEBUG

if not ('ReadBS' in dir()):
    ReadBS=True

if not ('noBPhysics' in dir()):
    noBPhysics=False

if not ('doSerialization' in dir()):
    doSerialization=True

if not ('FakeLVL1' in dir()):
    FakeLVL1=False

#-------------------------------------------------------------
# Transfer flags into TriggerFlags
#-------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags

# To turn off Lvl2/EF for athena running
if 'doLVL2' in dir():
    TriggerFlags.doLVL2 = bool(doLVL2)
if 'doEF' in dir():
    TriggerFlags.doEF = bool(doEF)
    
# To extract the Trigger configuration
if "doDBConfig" in dir():
    TriggerFlags.Online.doDBConfig = bool(doDBConfig)
    del doDBConfig
if "trigBase" in dir():
    TriggerFlags.Online.doDBConfigBaseName = trigBase
    del trigBase

    
# Short hand notation (only use them in *this* file)
doLVL2 = TriggerFlags.doLVL2()
doEF = TriggerFlags.doEF()

# List of all slices
allSlices = ['testElectron','testPhoton','testMuon','testTau','testJet',
             'testBjet','testMET','testBPhysics','testMinbias','testCombined','testAll',
	     'test_e10','test_g10','test_mu','test_e10_ID','test_mu_noID_noCalo',
	     'test_mu_noCalo', 'test_mu_IDTRT_noCalo','test_mu_noIdScan', 
	     'test_mu_noIdScan_noCalo', 'testCosmicEgamma']

# List of default slices if no flags given
#defaultSlices = ['testCosmicEgamma']
defaultSlices = ['testAll']
#defaultSlices = ['testElectron','testPhoton','testTau','testJet','testBjet','testMinbias']
#defaultSlices = ['testMET']
#defaultSlices = ['testJet']
#defaultSlices = ['testTau']
#defaultSlices = ['testMuon']
#defaultSlices = ['testElectron']
#defaultSlices = ['testPhoton']
#defaultSlices = ['test_mu_noIdScan_noCalo']
#defaultSlices = ['test_mu_noID_noCalo']
#defaultSlices = ['test_mu']
#defaultSlices = ['test_e10_ID']
#defaultSlices = ['test_e10']
#defaultSlices = ['test_g10']

# if no slice flags given set default slices
if set(dir()).intersection(set(allSlices))==set([]):
    for s in allSlices:
        # This will add a variables called testXXX to the global namespace
        globals()[s] = (s in defaultSlices)

# If slice flags are given set corresponding global variables
else:
    for s in allSlices:
        globals()[s] = (s in dir()) and (globals()[s]==True)
            
# Print active slices
from AthenaCommon.Logging import logging
log = logging.getLogger('testHLT_standalone.py')
log.info("=============== Running slices: ===============")
for s in allSlices:
    if globals()[s]: log.info(s)
log.info("===============================================")


#-------------------------------------------------------------
# Setting TriggerFlags
#-------------------------------------------------------------
include.block("RecExCond/RecExCommon_flags.py")

TriggerFlags.doHLTpersistency=False
if (isOnline):
    TriggerFlags.enableMonitoring = ['Online','Time']
else:
    TriggerFlags.enableMonitoring = ['Online','Time','Log']

if doSerialization:
   TriggerFlags.writeBS = True

if useLumi1E31 and testAll:
    TriggerFlags.triggerMenuSetup = 'lumi1E31'
    TriggerFlags.L1PrescaleSet = 'None'
    TriggerFlags.HLTPrescaleSet = 'None'

#turn down rate of g10_calib:
    from TriggerMenuPython.Lumi1E31 import Prescales
    Prescales.HLTPrescales['g10_calib']=[100,1]


elif useLumi1E32 and testAll:
    TriggerFlags.triggerMenuSetup = 'lumi1E32'
    TriggerFlags.L1PrescaleSet = "L1Prescales100_Physics_lumi1E32"
    TriggerFlags.HLTPrescaleSet = "HLTPrescales100_Physics_lumi1E32"    
else:
    TriggerFlags.triggerMenuSetup = 'full'

# Lvl1 config
TriggerFlags.readLVL1configFromXML=True
TriggerFlags.inputLVL1configFile="../AthenaP1RDO/outputLVL1config.xml"
if useLumi1E32:
    #to be changed to rel 14.1.0 version
    TriggerFlags.inputLVL1configFile="TriggerRelease/Rel14_0_10rel3_Apr16_lumi1E32_outputLVL1config.xml"
if testCosmicEgamma:
    TriggerFlags.inputLVL1configFile="TrigL2CosmicMuonHypo/L1MenuM5.xml"
else:
    TriggerFlags.CosmicSlice.testCosmic=False

TriggerFlags.outputLVL1configFile=None

# HLT config
TriggerFlags.readHLTconfigFromXML=False
if isOnline:
    # define a unique HLT configuration xml
    import os
    pid = os.getpid()
    TriggerFlags.outputHLTconfigFile ="OutputHLTConfiguration_PID-%d.xml" % (pid)
    TriggerFlags.inputHLTconfigFile  ="OutputHLTConfiguration_PID-%d.xml" % (pid)

log.info("Trigger xml files: %s %s %s" % (TriggerFlags.outputHLTconfigFile,TriggerFlags.outputLVL1configFile,TriggerFlags.inputHLTconfigFile))

### detector flags
TriggerFlags.doID=True    #ID always gets setup in the menu, so have to it on always
TriggerFlags.doMuon=False
TriggerFlags.doCalo=False
if testElectron or testPhoton or testTau or testJet or testBjet or testMET or testMuon or testBPhysics or testCombined or testAll or testCosmicEgamma:
    TriggerFlags.doCalo=True
#if testElectron or testMuon or testTau or testBjet or testBPhysics or testMET or testMinbias or testCombined or testAll or testCosmicEgamma:
#    TriggerFlags.doID=True
if testMuon or testBPhysics or testBjet or testMET or testCombined or testAll:
    TriggerFlags.doMuon=True

#special tests
if test_g10 or test_e10 or test_mu or test_mu_noIdScan:
    TriggerFlags.doCalo=True
### Fixme Now photon also need ID because of egammaGetter!
if test_g10 or test_e10 or test_e10_ID or test_mu or test_mu_noCalo or test_mu_IDTRT_noCalo or test_e10_ID:
    TriggerFlags.doID=True
    TriggerFlags.MuonSlice.doSiTrack=False
if test_mu_IDTRT_noCalo:
    TriggerFlags.MuonSlice.doTRTProp=True
if test_mu_noIdScan_noCalo or test_mu_noIdScan:
    TriggerFlags.doID=True
    TriggerFlags.MuonSlice.doIdScan=False
if test_mu or test_mu_noID_noCalo or test_mu_noIdScan or test_mu_noIdScan_noCalo or test_mu_noCalo or test_mu_IDTRT_noCalo:
    TriggerFlags.doMuon=True

TriggerFlags.MuonSlice.doL2ID   = TriggerFlags.doID()
TriggerFlags.MuonSlice.doL2Calo = TriggerFlags.doCalo()
TriggerFlags.MuonSlice.doEFID   = TriggerFlags.doID()
TriggerFlags.MuonSlice.doMuonCalibrationStream = isOnline

if alwaysAccept:
    def addAlwaysAcceptChain():
        TriggerFlags.MinBiasSlice.enableSignature("always")
    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)

if noBPhysics:  #temporary method - incompatible with alwaysAccept
    def removeBPhysicsSlice():
        TriggerFlags.BphysicsSlice.signatures=[]
    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(removeBPhysicsSlice)

###helper fuction
#-----------------------------
def resetAllSignaturesExcept(slice):
    slices = ['MuonSlice','EgammaSlice','TauSlice','JetSlice','BjetSlice','BphysicsSlice','METSlice','CombinedSlice','MinBiasSlice', 'CosmicSlice','CalibSlice']
    for s in slices:
        if s not in slice:
            exec 'TriggerFlags.%s.signatures = []' % s
#-----------------------------

if useLumi1E31 and not testAll:
    from TriggerMenuPython.Lumi1E31 import setupLumi1E31
    setupLumi1E31()
if useLumi1E32 and not testAll:
    from TriggerMenuPython.Lumi1E31 import setupLumi1E32
    setupLumi1E32()

signaturesToKeep=[]
    
# Enable electron slice
if testElectron and not testPhoton :
    current = TriggerFlags.EgammaSlice.signatures.get_Value()
    TriggerFlags.EgammaSlice.signatures =  [ x for x in current if not ("g" == x[0] or "g" == x[1]) ]

# Enable photon slice
if testPhoton and not testElectron:
    current = TriggerFlags.EgammaSlice.signatures.get_Value()
    TriggerFlags.EgammaSlice.signatures =  [ x for x in current if "g" == x[0] or "g" == x[1] ]

# Enable both electron and/ photon (egamma) slice
if testPhoton or testElectron:
    signaturesToKeep+=['EgammaSlice']

# Enable Muon slice
if testMuon:
    signaturesToKeep+=['MuonSlice']

# Enable tau slice
if testTau:
    signaturesToKeep+=['TauSlice']

# Enable jet slice
if testJet:
    signaturesToKeep+=['JetSlice']

# Enable bjet slice
if testBjet:
    signaturesToKeep+=['BjetSlice']

# Enable BPhysics slice
if testBPhysics:
    signaturesToKeep+=['BphysicsSlice']

# Enable met slice
if testMET:
    signaturesToKeep+=['METSlice']

# Enable minbias slice
if testMinbias or alwaysAccept:
    signaturesToKeep+=['MinBiasSlice']

# Enable combined slice needs additional signatures
if testCombined:
    signaturesToKeep+=['MuonSlice','EgammaSlice','TauSlice','JetSlice','BjetSlice','METSlice','CombinedSlice']

# Enable cosmic slice
if testCosmicEgamma:
    signaturesToKeep+=['CosmicSlice']
    TriggerFlags.CosmicSlice.doEgamma=True
    #TrigEgammaRec not working 
    TriggerFlags.CosmicSlice.doEgammaRec=False
    TriggerFlags.CosmicSlice.signatures =  ['cosmic_g10']

# Only if we are not running all slices do we remove something
if not testAll:
    resetAllSignaturesExcept(signaturesToKeep)

#BPhysics needs muons
if testBPhysics and not testMuon:
    TriggerFlags.MuonSlice.signatures =  ['mu4', 'mu6']

# special tests
if test_e10 or test_e10_ID:
    TriggerFlags.EgammaSlice.signatures =  ['e10_medium']
    resetAllSignaturesExcept('EgammaSlice')
    if test_e10_ID:
        TriggerFlags.EgammaSlice.unsetCalo()
if test_g10:
    TriggerFlags.EgammaSlice.signatures =  ['g10']
    resetAllSignaturesExcept('EgammaSlice')
    if not doLVL2:
        TriggerFlags.EgammaSlice.unsetL2()
    if not doEF:
        TriggerFlags.EgammaSlice.unsetEF()

if test_mu or test_mu_noID_noCalo or test_mu_noIdScan or test_mu_noIdScan_noCalo or test_mu_noCalo or test_mu_IDTRT_noCalo:
    TriggerFlags.MuonSlice.signatures =  ['mu6','mu20i']
    resetAllSignaturesExcept('MuonSlice')
    if not doLVL2:
        TriggerFlags.MuonSlice.unsetL2()
    if not doEF:
        TriggerFlags.MuonSlice.unsetEF()

#-------------------------------------------------------------
# Setting GlobalFlags
#-------------------------------------------------------------
# GlobalFlags can only be set once. We need to catch the RuntimeError
# so that we can do multiple configure transitions. Needs fix.
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
try:
    GlobalFlags.DetGeo.set_atlas()
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.Luminosity.set_zero()
    if not testCosmicEgamma:
        GlobalFlags.DetDescrVersion = "ATLAS-CSC-01-02-00"
    else:
        GlobalFlags.DetDescrVersion = "ATLAS-CommNF-03-00-00"

    # Input format and file
    if not ReadBS:
        GlobalFlags.InputFormat.set_pool()
        athenaCommonFlags.PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
    else :
        GlobalFlags.InputFormat.set_bytestream()
        athenaCommonFlags.BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.May31.Rel14.2.0.rel6.LB0000.Athena._0001.data"]
        if testCosmicEgamma:
            athenaCommonFlags.BSRDOInput=["/afs/cern.ch/user/x/xin/Athena/13.test/Calo/daq.calo.0032645.physics.cosmics.LB0000.SFO-1._0002.data"]
        if not doLVL2 and doEF:
            athenaCommonFlags.BSRDOInput=["doEFOnly.input.data"]
            
except RuntimeError:
    pass

#-------------------------------------------------------------
# some people start using globalflags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion=GlobalFlags.DetDescrVersion

include("TriggerRelease/Trigger_topOptions_standalone.py")

#MC files still have old mapping - don't put in topOptions, since that is inconsistent with cosmics
if hasattr(ToolSvc,"PixelFillCablingData_Final"):
    ToolSvc.PixelFillCablingData_Final.MappingFile='PixelCabling/Pixels_Atlas_IdMapping.dat'

if hasattr(ToolSvc,"TRT_FillCablingData_DC3"):
    ToolSvc.TRT_FillCablingData_DC3.RealData=False

if not ReadBS:
    # if file not in catalog put it there
    svcMgr.PoolSvc.AttemptCatalogPatch=True 

if testCosmicEgamma:
    FakeLVL1=True
    FakeLVL1ROIEta=-0.5
    FakeLVL1ROIPhi=-1.5
    CosmicL1Signature = '1MU3'
    CosmicL1Threshold = 'MU3'
    forceLVL2Accept=False
    filterEmptyROB=False
    if doLVL2:
        forceLVL2Accept=True
        filterEmptyROB=True

if (FakeLVL1):
    from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
    fake = FakeLvl1RoIatFixedEtaPhi()
    fake.FakeMuonRoiPhi=FakeLVL1ROIPhi
    fake.FakeMuonRoiEta=FakeLVL1ROIEta
    #fake.FakeMuonRoiEta=0.5
    fake.FakeMuonRoiLabel=CosmicL1Threshold
    if (doLVL2):
        topSequence.TrigSteer_L2.LvlConverterTool =  fake
        if forceLVL2Accept and hasattr(svcMgr,"Lvl2EventLoopMgr"):
            svcMgr.Lvl2EventLoopMgr.ForceLvl2Accept = True
        if (filterEmptyROB):
            ROBDataProviderSvc.filterEmptyROB=True

# ----------------------------------------------------------------
# Number of events to be processed 
# ----------------------------------------------------------------
theApp.EvtMax = EvtMax
if hasattr(svcMgr,"EventSelector"):
    svcMgr.EventSelector.SkipEvents = SkipEvents 

# ----------------------------------------------------------------
# Setting individual OutputLevel
# ----------------------------------------------------------------
# Make a list of the steering objects
trigSteerConf = []
if doLVL2: trigSteerConf += [topSequence.TrigSteer_L2]
if doEF: trigSteerConf += [topSequence.TrigSteer_EF]

for s in trigSteerConf:
    s.OutputLevel = HLTOutputLevel
    for algo in s.getChildren(): algo.OutputLevel = HLTOutputLevel

for svc in ["Lvl2EventLoopMgr","EFEventLoopMgr"]:
    if hasattr(svcMgr,svc): getattr(svcMgr,svc).OutputLevel = HLTOutputLevel
    
if doLVL2:
    svcMgr.ROBDataProviderSvc.OutputLevel=ERROR
    
if not ReadBS:
    topSequence.CTPSimulation.OutputLevel = INFO

# Reduce verbosity of AtRndmGenSvc (used in steering scalers)
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc(OutputLevel = WARNING)

# ----------------------------------------------------------------
# Setting limits for timer histograms
# ----------------------------------------------------------------
#include("TriggerRelease/jobOfragment_trigTimers.py")
#for now just do top histograms
if doLVL2:
    topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].NumberOfHistBins = 200
if doEF:
    topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].NumberOfHistBins = 200
if wideTiming:
    if doLVL2:
        topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].TimerHistLimits  = [0,1500]
    if doEF:
        topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].TimerHistLimits = [0,8000]
else:
    if doLVL2:
        topSequence.TrigSteer_L2.MonTools['L2SteeringTime'].TimerHistLimits  = [0,500]
    if doEF:
        topSequence.TrigSteer_EF.MonTools['EFSteeringTime'].TimerHistLimits = [0,1000]

include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
include("TrigMuonEvent/TrigMuonEventDict_joboptions.py")
include("TrigSteeringEvent/TrigSteeringEventDict_joboptions.py")


if enableFPE:
    theApp.CreateSvc += ["FPEControlSvc"]

    
#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr

