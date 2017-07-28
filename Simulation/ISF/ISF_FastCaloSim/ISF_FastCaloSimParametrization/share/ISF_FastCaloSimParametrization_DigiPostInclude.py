streamRDO.ItemList+=["ISF_FCS_Parametrization::FCS_StepInfoCollection#ZHMergedEventSteps","LArHitContainer#*","TileHitVector#*"];
from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("LArPileUpTool").CrossTalk=False
