################################################################################
#
#  minimal jO to run EFID with a little overhead from other subsystems
#         (in terms of reconstruction and detectors involved)
#  based on RecExCommon like jO to run the trigger and functionality of
#         the former InDetTrigRecExample/jobOptionsNewSteering.py 
#
#  by default only EF ID is executed on FakeLVL1 RoIs (LVL2 is dummy)
#
#  25/01/2008     Jiri Masik
#  
################################################################################

#these are the defaults, can be modified by RTT
if not ('EvtMax' in dir()):
    EvtMax=10
if not ('OutputLevel' in dir()):
    OutputLevel=INFO
if not ('doReadBS' in dir()):
    doReadBS=False
if not ('minimalSetup' in dir ()):
    minimalSetup=True

#the performance package may modify
# if not ('useROOTNtuple' in dir()): 
#     useROOTNtuple=True  
# if not ('RootNtupleOutput' in dir()): 
#     RootNtupleOutput="ntupleEFIDAA.root"
# if not ('doWriteESD' in dir()): 
#     doWriteESD=False
# if not ('doESD' in dir()): 
#     doESD=False


#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"] 
#BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.May7.Rel14.1.0.LB0000.Athena._0001.data"] 




###############################
doTruth = True
if doReadBS:
  ReadBS=True
  doTruth = False
  #BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.May7.Rel14.1.0.LB0000.Athena._0001.data"]
  #BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.May7.Rel14.1.0.LB0000.Athena._0001.data"]


doTrigger=True

TriggerModernConfig=True

doWriteAOD=False
doWriteTAG=False
doAOD=False 
doTAG=False


#modify standard menu
def clearsigs():
    print 'jmasik clearsigs'
    TriggerFlags.EgammaSlice.signatures = ['e10_passL2']

def resetLowerChain(triggerPythonConfig):
    print 'jmasik - resetLowerChain'
    for chain in triggerPythonConfig.theEFHLTChains:
        print 'chain: ', chain.chain_name
        if chain.chain_name == 'EF_e10':
            chain.lower_chain_name=''


from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
if 'doReadBS' in dir() and doReadBS:
    globalflags.InputFormat = 'bytestream'
else:
    globalflags.InputFormat = 'pool'
            

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#override menu
def ElectronOnly():
  TriggerFlags.Slices_all_setOff()

  # Enable electron slice TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.unsetCalo()
  TriggerFlags.EgammaSlice.signatures =  ["e5_NoCut","e20_medium_IDTrkNoCut"]
    
    
from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(ElectronOnly)


#
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
#InDetTrigFlags.doTruth = TriggerFlags.doTruth()
from RecExConfig.RecFlags import rec 
InDetTrigFlags.doTruth = rec.doTruth()
InDetTrigFlags.doNewTracking = True
InDetTrigFlags.doBackTracking = False
InDetTrigFlags.doiPatRec = False
InDetTrigFlags.trackFitterType = 'GlobalChi2Fitter'
#InDetTrigFlags.trackFitterType = 'GaussianSumFilter'
#InDetTrigFlags.trackFitterType = 'KalmanFitter'
print InDetTrigFlags
#




#TriggerFlags.readHLTconfigFromXML=True
#TriggerFlags.readLVL1configFromXML=True
#TriggerFlags.inputHLTconfigFile = 'newMenu.xml'
#TriggerFlags.inputLVL1configFile = 'lvl1Menu.xml'

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]


# #menu


##########

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
if 'doReadBS' in dir() and doReadBS:
    globalflags.InputFormat = 'bytestream'
else:
    globalflags.InputFormat = 'pool'
             

from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
if minimalSetup:
    TriggerFlags.doMuon=False
    DetFlags.Calo_setOff()
    DetFlags.Muon_setOff()

DetFlags.digitize.all_setOff() #to avoid TrigEgammaRec_NoIDEF_eGamma:ERROR could not 
                               #                  get handle to CaloNoiseToolDefault

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default'

#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
TriggerFlags.abortOnConfigurationError=True
#-------------end of temporary fix-------------------

#from TriggerJobOpts.TriggerGetter import TriggerGetter



#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from InDetTrigRecExample.EFInDetConfig import *


#no truth ->no fakeRoIs in BS
if not doReadBS:
  electronAlgs = TrigEFIDInsideOut_Electron()
else:
  electronAlgs = TrigEFIDInsideOut_FullScan()

##########

##########

jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

print job

steeringL2 = job.TrigSteer_L2

if rec.doTruth():
  from TrigFake.TrigFakeConf import FakeLvl1RoIfromKine
  steeringL2.LvlConverterTool = FakeLvl1RoIfromKine("FakeLvl1RoIfromKine")
  steeringL2.LvlConverterTool.FakeEmTauRoiParticleIDs=[11,-11]
  steeringL2.LvlConverterTool.FakeEmTauRoiPtMin=25*GeV
  steeringL2.LvlConverterTool.FakeEmRoiLabel="EM7"
  steeringL2.LvlConverterTool.McEventCollectionName = "TruthEvent"
  steeringL2.LvlConverterTool.OutputLevel = DEBUG
  #steeringL2.OutputLevel = VERBOSE
else:
  from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
  steeringL2.LvlConverterTool = FakeLvl1RoIatFixedEtaPhi("FakeLvl1RoIatFixedEtaPhi")
  steeringL2.LvlConverterTool.FakeEmRoiPhi=0.0
  steeringL2.LvlConverterTool.FakeEmRoiEta=0.0
  steeringL2.LvlConverterTool.FakeEmRoiLabel="EM7"
  if not True:
    steeringL2.LvlConverterTool.OutputLevel = DEBUG
    #steeringL2.OutputLevel = VERBOSE
  else:
    steeringL2.LvlConverterTool.OutputLevel = ERROR
    #steeringL2.OutputLevel = ERROR	

  
steeringEF = job.TrigSteer_EF


#
if 0 and doReadBS:
    ServiceMgr.ByteStreamCnvSvc.IsSimulation = True

    # hack to force MC calbing loading for FDR2
    if ServiceMgr.ByteStreamCnvSvc.IsSimulation:
        from InDetCabling.InDetCablingConf import SCT_CablingSelector
        SCT_CablingSelector = SCT_CablingSelector(Method = "MANUAL", Layout ="FromTextFile", Filename = "SCT_MC_FullCabling.dat")
        ToolSvc            += SCT_CablingSelector
        


