# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from egammaCaloTools.egammaCaloToolsConf import egammaIso
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault

def EgammaIsoTool():
    from AthenaCommon.AppMgr import ToolSvc

    theCaloNoiseTool=CaloNoiseToolDefault('db')
    if theCaloNoiseTool not in ToolSvc:
        ToolSvc += theCaloNoiseTool
    
    if not hasattr(ToolSvc, "PAU_egammaIso"):
        theegammaiso=egammaIso("PAU_egammaIso",
                               CaloNoiseTool= theCaloNoiseTool)
        ToolSvc += theegammaiso
        return theegammaiso
    else:
        return ToolSvc.PD3PD_egammaIso

