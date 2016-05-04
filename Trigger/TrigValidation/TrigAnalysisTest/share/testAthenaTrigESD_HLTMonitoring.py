#include("jet_105011_10TeV.py")
#PoolAODInput=["/grid01/SUSY/AODs/mc08.106400.SU4_jimmy_susy.recon.AOD.e352_s462_r541/AOD.026551._00073.pool.root.1"]



from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('fileList' in dir()):
     fileList=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s764_r1295/AOD.134900._000001.pool.root.1']

# allow this to work in RTT
if not fileList == []:
    acf.FilesInput=fileList


if not 'RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromAOD.root"

rec.doCBNT=False
rec.readESD=True
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False
rec.doDPD=False 
rec.doWriteTAG=False 

#doTrigger=True
#SkipEvents=840

if not acf.EvtMax.is_locked():
    acf.EvtMax=250
if not ('OutputLevel' in dir()):
    #OutputLevel=DEBUG
    rec.OutputLevel=INFO

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList.set_Value_and_Lock( [ "ds" ] )

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter()


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

################ Mon Tools #################

#Global HLTMonTool

from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name               = 'HLTMon',
                   histoPathBase      = "/Trigger/HLT");

#HLTMon.TrigDecisionTool = monTrigDecTool

ToolSvc += HLTMon;

HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];


from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
HLTMonManager.AthenaMonTools+=HLTCaloMonitoringTool()

from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonitoringTool
HLTMonManager.AthenaMonTools += TrigMuonMonitoringTool()

from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
HLTMonManager.AthenaMonTools += TrigJetMonitoringTool()

from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()

from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
HLTMonManager.AthenaMonTools += HLTMETMonitoringTool()

from TrigTauMonitoring.TrigTauMonitoringConfig import TrigTauMonitoringTool
HLTMonManager.AthenaMonTools += TrigTauMonitoringTool()

from TrigBjetMonitoring.TrigBjetMonitoringConfig import TrigBjetMonitoringConfig
HLTMonManager.AthenaMonTools += TrigBjetMonitoringConfig()

from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()

from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
HLTMonManager.AthenaMonTools += TrigMinBiasMonitoringTool()

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

#include("HLTMonitoring_topOptions.py")
## ToolSvc.HLTTauMon.OutputLevel=DEBUG
## ToolSvc.HLTTauMon.tauTriggItem = ["tau16i_loose"]
## #ToolSvc.HLTTauMon.tauTriggItem = ["tauNoCut"]                
## doPerfMon=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

