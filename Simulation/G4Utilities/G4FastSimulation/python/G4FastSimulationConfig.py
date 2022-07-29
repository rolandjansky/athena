# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSimpleFastKiller(name="SimpleFastKiller", **kwargs):
    kwargs.setdefault("RegionNames" , ["BeampipeFwdCut"] )
    return CfgMgr.SimpleFastKillerTool(name, **kwargs)

def getDeadMaterialShower(name="DeadMaterialShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["DeadMaterial"])
    return CfgMgr.DeadMaterialShowerTool(name, **kwargs)

def getFastCaloSim(name="FastCaloSim", **kwargs):

    import MagFieldServices.SetupField  # noqa: F401
    from TileConditions.TileCoolMgr import tileCoolMgr # noqa: F401

    from AthenaCommon.Resilience import protectedInclude
    protectedInclude( "TileConditions/TileConditions_jobOptions.py" )
    protectedInclude( "LArDetDescr/LArDetDescr_joboptions.py" ) 

    # CALO region defined for FastCaloSim for triggering fast simulation 
    kwargs.setdefault("RegionNames", ["CALO"])
    
    # Associate a custom SD to write out CaloCellContainer to FastCaloSimTool
    kwargs.setdefault('CaloCellContainerSDName', "ToolSvc.SensitiveDetectorMasterTool.CaloCellContainerSD")
    
    return CfgMgr.FastCaloSimTool(name, **kwargs)


