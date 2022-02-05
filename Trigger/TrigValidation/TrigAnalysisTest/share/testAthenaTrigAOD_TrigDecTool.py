#fileList=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s764_r1295/AOD.134900._000001.pool.root.1']

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from AthenaConfiguration.AllConfigFlags import ConfigFlags

acf.FilesInput = fileList  # noqa: F821 - from dir()
ConfigFlags.Input.Files = acf.FilesInput() or acf.PoolAODInput()

rec.doTrigger=False

rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from TrigValAlgs.TrigValAlgsConfig import TrigDecisionChecker

TrigDecChecker = TrigDecisionChecker("TrigDecChecker")
TrigDecChecker.WriteEventDecision=False
TrigDecChecker.MonitoringBlock=100
TrigDecChecker.MonitoredChains=[]

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior+=1
from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg 
tdtAcc = TrigDecisionToolCfg(ConfigFlags)
Configurable.configurableRun3Behavior-=1

theTDT = tdtAcc.getPrimary()
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena

TrigDecChecker.TrigDecisionTool = conf2toConfigurable(theTDT)

appendCAtoAthena( tdtAcc )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigDecChecker
