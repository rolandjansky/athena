readAOD=True
doMonitoring=False
doWriteTAG=False
doWriteAOD=False
doWriteESD=False
doCBNT=False
doAOD=True
doTruth=False

#PoolAODInput=["/afs/cern.ch/user/y/yming/atcan/Data/MC08/mc08.106050.PythiaZee_1Lepton.recon.AOD.e347_s462_r541/AOD.026360._00626.pool.root.1"]
PoolAODInput=["castor:/castor/cern.ch/user/y/yming/egammaPerformance/mc08_aod_zee.root"]

# number of event to process (can be modified externally)
if not 'EvtMax' in dir():
     EvtMax=2000

OutputLevel = INFO

#-----------------------------------------------------------
#include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

#TriggerFlags.readHLTconfigFromXML=False
#TriggerFlags.readLVL1configFromXML=False

#def electrons():
#    TriggerFlags.Slices_all_setOff()
#    TriggerFlags.EgammaSlice.setAll()
#    TriggerFlags.BphysicsSlice.signatures = []
#    TriggerFlags.BjetSlice.signatures = []
#    #TriggerFlags.EgammaSlice.signatures = []
#    TriggerFlags.JetSlice.signatures = []
#    TriggerFlags.METSlice.signatures = []
#    TriggerFlags.MinBiasSlice.signatures = []
#    TriggerFlags.MuonSlice.signatures = []
#    TriggerFlags.TauSlice.signatures = []
    

#from TriggerMenuPython.GenerateMenu import GenerateMenu
#GenerateMenu.overwriteSignaturesWith(electrons)


####  First switch all slices OFF
#TriggerFlags.Slices_all_setOff()

###### This is a temporary fix ################
#DetFlags.simulateLVL1.Calo_setOn()

# Enable electron slice
#TriggerFlags.EgammaSlice.setAll()
 
#from RecExConfig.RecFlags import rec

#rec.doCalo=True
#rec.doEgamma=True
#rec.doInDet=True
#rec.doMuon=False
#rec.doMuonCombined=False
#rec.doJetMissingETTag=False
#rec.doTau=False

# main jobOption
#-----------------------------------------------------
#set up the trigger
#-----------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList.set_Value_and_Lock( [ "ds" ] )
from RecExConfig.RecFlags import rec
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.readTAG = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter()

include ("RecExCommon/RecExCommon_topOptions.py")
 
 
########Setup egamma monitoring ################
from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc = Service ("THistSvc")
THistSvc.Output += ["egammaMon DATAFILE='egammaMon.root' OPT='RECREATE'"]

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.monManEnvironment         = "tier0ESD"
DQMonFlags.monManFileKey = "egammaMon"
DQMonFlags.useTrigger = True 

include ("egammaPerformance/egammaMonitoring_jobOptions_aod.py")

#include ("egammaPerformance/egamma_MonitorTool.py")

