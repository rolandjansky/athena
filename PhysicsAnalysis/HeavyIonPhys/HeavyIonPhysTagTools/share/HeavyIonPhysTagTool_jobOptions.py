include.block ("HeavyIonPhysTagTools/HeavyIonPhysTagTool_jobOptions.py")

from HeavyIonPhysTagTools.HeavyIonPhysTagToolsConf import \
     HeavyIonPhysTagTool as ConfiguredHeavyIonPhysTagTool
HeavyIonPhysTagTool = ConfiguredHeavyIonPhysTagTool(
    EtCut       = 6.0*GeV)
ToolSvc += HeavyIonPhysTagTool
