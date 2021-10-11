# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaCommon import CfgMgr

def getReducedStepSizeUserLimitsSvc(name="ReducedStepSizeUserLimitsSvc", **kwargs):
    from AthenaCommon.SystemOfUnits import millimeter
    kwargs.setdefault("OutputLevel", 1)
    kwargs.setdefault("VolumeList", [
                                    "Atlas::Atlas",
                                    "BeamPipe::SectionC01",
                                    "BeamPipe::SectionF01",
                                    "CALO::CALO",
                                    "LArMgr::LAr::Barrel::Cryostat::",
                                    "LArMgr::LAr::DM::SectorEnvelopes",
                                    "LArMgr::LAr::EMB::GTENF",
                                    "LArMgr::LAr::Endcap::Cryostat::",
                                    "LArMgr::MBTS1",
                                    "LArMgr::MBTS2",
                                    "LArMgr::Moderator",
                                    "LArMgr::ModeratorTube",
                                    "Tile::GirderIron",
                                    "Tile::GirderMother",
                                    ])
    kwargs.setdefault("MaxStep", 10.*millimeter)
    kwargs.setdefault("MatchType", "contains")
    return CfgMgr.UserLimitsSvc(name, **kwargs)


def getPhysicsListSvc(name="PhysicsListSvc", **kwargs):
    PhysOptionList = ["G4StepLimitationTool"]
    from G4AtlasApps.SimFlags import simFlags
    PhysOptionList += simFlags.PhysicsOptions.get_Value()
    PhysDecaysList = []
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.TRT_on():
        PhysOptionList +=["TRTPhysicsTool"]
    if DetFlags.simulate.Lucid_on() or DetFlags.simulate.AFP_on():
        PhysOptionList +=["LucidPhysicsTool"]
    kwargs.setdefault("PhysOption", PhysOptionList)
    kwargs.setdefault("PhysicsDecay", PhysDecaysList)
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("PhysicsList", simFlags.PhysicsList.get_Value())
    if 'PhysicsList' in kwargs:
        if kwargs['PhysicsList'].endswith('_EMV') or kwargs['PhysicsList'].endswith('_EMX'):
            raise RuntimeError( 'PhysicsList not allowed: '+kwargs['PhysicsList'] )
    kwargs.setdefault("GeneralCut", 1.)
    if hasattr(simFlags, 'NeutronTimeCut') and simFlags.NeutronTimeCut.statusOn:
        kwargs.setdefault("NeutronTimeCut", simFlags.NeutronTimeCut.get_Value())
    if hasattr(simFlags, 'NeutronEnergyCut') and simFlags.NeutronEnergyCut.statusOn:
        kwargs.setdefault("NeutronEnergyCut", simFlags.NeutronEnergyCut.get_Value())
    if hasattr(simFlags, 'ApplyEMCuts') and simFlags.ApplyEMCuts.statusOn:
        kwargs.setdefault("ApplyEMCuts", simFlags.ApplyEMCuts.get_Value())
    ## from AthenaCommon.SystemOfUnits import eV, TeV
    ## kwargs.setdefault("EMMaxEnergy"     , 7*TeV)
    ## kwargs.setdefault("EMMinEnergy"     , 100*eV)
    """ --- ATLASSIM-3967 ---
        these two options are replaced by SetNumberOfBinsPerDecade
        which now controls both values.
    """
    ## kwargs.setdefault("EMDEDXBinning"   , 77)
    ## kwargs.setdefault("EMLambdaBinning" , 77)
    return CfgMgr.PhysicsListSvc(name, **kwargs)


#########################################################################
def getStandardFieldSvc(name="StandardField", **kwargs):
    #kwargs.setdefault("FieldOn", True)
    return CfgMgr.StandardFieldSvc(name, **kwargs)

def getForwardFieldSvc(name="ForwardField", **kwargs):
    kwargs.setdefault("MagneticFieldSvc", "ForwardRegionFieldSvc")
    #kwargs.setdefault("FieldOn", True)
    # Must switch on the use of a field svc to be able to have StandardFieldSvc use ForwardRegionFieldSvc
    kwargs.setdefault("UseMagFieldSvc", True)
    return CfgMgr.StandardFieldSvc(name, **kwargs)

def getQ1FwdG4FieldSvc(name='Q1FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1")
    return getForwardFieldSvc(name, **kwargs)

def getQ2FwdG4FieldSvc(name='Q2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2")
    return getForwardFieldSvc(name, **kwargs)

def getQ3FwdG4FieldSvc(name='Q3FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3")
    return getForwardFieldSvc(name, **kwargs)

def getD1FwdG4FieldSvc(name='D1FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D1")
    return getForwardFieldSvc(name, **kwargs)

def getD2FwdG4FieldSvc(name='D2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D2")
    return getForwardFieldSvc(name, **kwargs)

def getQ4FwdG4FieldSvc(name='Q4FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4")
    return getForwardFieldSvc(name, **kwargs)

def getQ5FwdG4FieldSvc(name='Q5FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5")
    return getForwardFieldSvc(name, **kwargs)

def getQ6FwdG4FieldSvc(name='Q6FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6")
    return getForwardFieldSvc(name, **kwargs)

def getQ7FwdG4FieldSvc(name='Q7FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q7")
    return getForwardFieldSvc(name, **kwargs)

def getQ1HKickFwdG4FieldSvc(name='Q1HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1HKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ1VKickFwdG4FieldSvc(name='Q1VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1VKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ2HKickFwdG4FieldSvc(name='Q2HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2HKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ2VKickFwdG4FieldSvc(name='Q2VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2VKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ3HKickFwdG4FieldSvc(name='Q3HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3HKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ3VKickFwdG4FieldSvc(name='Q3VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3VKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ4VKickAFwdG4FieldSvc(name='Q4VKickAFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickA")
    return getForwardFieldSvc(name, **kwargs)

def getQ4HKickFwdG4FieldSvc(name='Q4HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4HKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ4VKickBFwdG4FieldSvc(name='Q4VKickBFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickB")
    return getForwardFieldSvc(name, **kwargs)

def getQ5HKickFwdG4FieldSvc(name='Q5HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5HKick")
    return getForwardFieldSvc(name, **kwargs)

def getQ6VKickFwdG4FieldSvc(name='Q6VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6VKick")
    return getForwardFieldSvc(name, **kwargs)

def getDetectorGeometrySvc(name="DetectorGeometrySvc", **kwargs):
    kwargs.setdefault("DetectorConstruction", 'G4AtlasDetectorConstructionTool')
    return CfgMgr.DetectorGeometrySvc(name, **kwargs)

def getG4AtlasSvc(name="G4AtlasSvc", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault("isMT", is_hive)
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return CfgMgr.G4AtlasSvc(name, **kwargs)

def getG4GeometryNotifierSvc(name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return CfgMgr.G4GeometryNotifierSvc(name, **kwargs)
