
from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc;
svcMgr+=LArFlatConditionSvc();
svcMgr.ProxyProviderSvc.ProviderNames+=["LArFlatConditionSvc"];
svcMgr.LArFlatConditionSvc.DoSuperCells=True;
svcMgr.LArFlatConditionSvc.DoRegularCells=True;

from CaloTools.CaloLumiBCIDSCToolDefault import CaloLumiBCIDSCToolDefault;
theTool=CaloLumiBCIDSCToolDefault();
svcMgr.ToolSvc+=theTool;

from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SuperCellBCIDAlg
algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.SuperCellBCIDAlg()

#StreamAOD.ItemList += ["CaloCellContainer#SCellBCID"]
