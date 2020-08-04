from HIRecExample.HIRecExampleFlags import jobproperties


from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
CaloTowerCmbGetter()

from HIGlobal.HIGlobalFlags import jobproperties
shape_key=jobproperties.HIGlobalFlags.EventShapeKey()


from HIGlobal.HIGlobalConf import HIEventShapeMaker
from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
theAlg=HIEventShapeMaker()
theAlg.ReadExistingKey=shape_key
theAlg.OutputContainerKey=shape_key
theAlg.HIEventShapeFillerTool=HIEventShapeFillerTool()
theAlg.OrderOfFlowHarmonics=jobproperties.HIGlobalFlags.NumFlowHarmonics()

#Import the map tool
from HIEventUtils.HIEventUtilsConf import HIEventShapeMapTool
theMapTool=HIEventShapeMapTool()
theAlg.HIEventShapeFillerTool.EventShapeMapTool=theMapTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theAlg

jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeContainer#"+shape_key]
jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeAuxContainer#"+shape_key+"Aux."]

#jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleContainer#MBTSModules"]
#jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleAuxContainer#MBTSModulesAux."]

from xAODForwardCnv.xAODMBTSModuleCreator import xAODMaker__MBTSModuleCnvAlg
topSequence += xAODMaker__MBTSModuleCnvAlg()

jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleContainer#MBTSModules"]
jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleAuxContainer#MBTSModulesAux."]

from xAODForwardCnv.xAODForwardEventInfoCreator import xAODMaker__ForwardEventInfoCnvAlg
topSequence += xAODMaker__ForwardEventInfoCnvAlg()

jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::ForwardEventInfoContainer#MBTSForwardEventInfo"]
jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::ForwardEventInfoAuxContainer#MBTSForwardEventInfoAux."]

if jobproperties.HIGlobalFlags.DoSummary() :
    summary_key=jobproperties.HIGlobalFlags.EventShapeSummaryKey()
    from HIEventUtils.HIEventUtilsConf import HIEventShapeSummaryTool
    SummaryTool=HIEventShapeSummaryTool()
    SummaryTool.SubCalos=jobproperties.HIGlobalFlags.SummarySubCalos()
    SummaryTool.Samplings=jobproperties.HIGlobalFlags.SummarySamplings()
    SummaryTool.DoPositiveNegativeSides=jobproperties.HIGlobalFlags.SummaryPN()
    theAlg.SummaryTool=SummaryTool
    theAlg.SummaryContainerKey=summary_key
    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeContainer#"+summary_key]
    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeAuxContainer#"+summary_key+"Aux."]
