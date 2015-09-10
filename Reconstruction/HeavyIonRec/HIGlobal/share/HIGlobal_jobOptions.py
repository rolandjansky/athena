from HIRecExample.HIRecExampleFlags import jobproperties


from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
CaloTowerCmbGetter()

from HIGlobal.HIGlobalFlags import jobproperties
shape_key=jobproperties.HIGlobalFlags.EventShapeKey()


from HIGlobal.HIGlobalConf import HIEventShapeMaker
from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
theAlg=HIEventShapeMaker()
theAlg.OutputContainerKey=shape_key
theAlg.HIEventShapeFillerTool=HIEventShapeFillerTool()
theAlg.OrderOfFlowHarmonics=jobproperties.HIGlobalFlags.NumFlowHarmonics()
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theAlg

jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeContainer#"+shape_key]
jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeAuxContainer#"+shape_key+"Aux."]

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

