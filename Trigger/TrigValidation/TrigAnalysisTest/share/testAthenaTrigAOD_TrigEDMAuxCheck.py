
#PoolAODInput=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/data130003002/AOD.015446._00012.pool.root.2']

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags  import recAlgs
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


####NOTE this still needs to updated for the DC14 menu!!! check what the TrigEDMChecker supports!!! rm L2 etc .. 
from TrigValAlgs.TrigValAlgsConfig import TrigEDMAuxChecker
TrigEDMAuxChecker = TrigEDMAuxChecker("TrigEDMAuxChecker")

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigEDMAuxChecker
