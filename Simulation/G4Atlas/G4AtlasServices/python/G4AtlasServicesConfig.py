# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


def getATLAS_RegionCreatorList():
    regionCreatorList = []
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    isUpgrade = commonGeoFlags.Run()=="RUN4" or (commonGeoFlags.Run()=="UNDEFINED" and geoFlags.isSLHC())
    isRUN2 = (commonGeoFlags.Run() in ["RUN2", "RUN3"]) or (commonGeoFlags.Run()=="UNDEFINED" and geoFlags.isIBL())

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    if simFlags.SimulateCavern.get_Value():
        regionCreatorList += ['SX1PhysicsRegionTool', 'BedrockPhysicsRegionTool', 'CavernShaftsConcretePhysicsRegionTool']
        #regionCreatorList += ['CavernShaftsAirPhysicsRegionTool'] # Not used currently
    if DetFlags.ID_on():
        if DetFlags.pixel_on():
            regionCreatorList += ['PixelPhysicsRegionTool']
        if DetFlags.SCT_on():
            regionCreatorList += ['SCTPhysicsRegionTool']
        if DetFlags.TRT_on() and not isUpgrade:
            regionCreatorList += ['TRTPhysicsRegionTool']
            if isRUN2:
                regionCreatorList += ['TRT_ArPhysicsRegionTool'] #'TRT_KrPhysicsRegionTool'
        # FIXME dislike the ordering here, but try to maintain the same ordering as in the old configuration.
        if DetFlags.bpipe_on():
            if simFlags.BeamPipeSimMode.statusOn and simFlags.BeamPipeSimMode() != "Normal":
                regionCreatorList += ['BeampipeFwdCutPhysicsRegionTool']
            if simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
                regionCreatorList += ['FWDBeamLinePhysicsRegionTool']
    if DetFlags.Calo_on():
        if DetFlags.geometry.LAr_on():
            ## Shower parameterization overrides the calibration hit flag
            if simFlags.LArParameterization.statusOn and simFlags.LArParameterization() > 0 \
                    and simFlags.CalibrationRun.statusOn and simFlags.CalibrationRun.get_Value() in ['LAr','LAr+Tile','DeadLAr']:
                print ('You requested both calibration hits and frozen showers / parameterization in the LAr.')
                print ('  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.')
                print ('  Please try again with a different value of either simFlags.LArParameterization (' + str(simFlags.LArParameterization()) + ') or simFlags.CalibrationRun ('+str(simFlags.CalibrationRun.get_Value())+')')
                raise RuntimeError('Configuration not allowed')
            if simFlags.LArParameterization() > 0:
                regionCreatorList += ['EMBPhysicsRegionTool', 'EMECPhysicsRegionTool',
                                      'HECPhysicsRegionTool', 'FCALPhysicsRegionTool']
                # FIXME 'EMBPhysicsRegionTool' used for parametrization also - do we need a second instance??
                regionCreatorList += ['EMECParaPhysicsRegionTool',
                                      'FCALParaPhysicsRegionTool', 'FCAL2ParaPhysicsRegionTool']
                if simFlags.LArParameterization.get_Value() > 1:
                    regionCreatorList += ['PreSampLArPhysicsRegionTool', 'DeadMaterialPhysicsRegionTool']
            elif simFlags.LArParameterization() is None or simFlags.LArParameterization() == 0:
                regionCreatorList += ['EMBPhysicsRegionTool', 'EMECPhysicsRegionTool',
                                      'HECPhysicsRegionTool', 'FCALPhysicsRegionTool']
    ## FIXME _initPR never called for FwdRegion??
    #if simFlags.ForwardDetectors.statusOn:
    #    if DetFlags.geometry.FwdRegion_on():
    #        regionCreatorList += ['FwdRegionPhysicsRegionTool']
    if DetFlags.Muon_on():
        regionCreatorList += ['DriftWallPhysicsRegionTool', 'DriftWall1PhysicsRegionTool', 'DriftWall2PhysicsRegionTool']
        if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read' and not (simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
            regionCreatorList += ['MuonSystemFastPhysicsRegionTool']
    return regionCreatorList

def getCTB_RegionCreatorList():
    regionCreatorList = []
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    ## FIXME _initPR never called for SCT??
    #if DetFlags.ID_on():
    #    if DetFlags.geometry.SCT_on():
    #        regionCreatorList += ['SCTSiliconPhysicsRegionTool']
    if DetFlags.Calo_on():
        eta=simFlags.Eta.get_Value()
        if eta>=0 and eta<1.201:
            if DetFlags.em_on():
                regionCreatorList += ['EMBPhysicsRegionTool']
    if DetFlags.Muon_on():
        regionCreatorList += ['DriftWallPhysicsRegionTool', 'DriftWall1PhysicsRegionTool', 'DriftWall2PhysicsRegionTool']
    return regionCreatorList

def getTB_RegionCreatorList():
    regionCreatorList = []
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    if (simFlags.SimLayout.get_Value()=="tb_LArH6_2003"):
        if (DetFlags.FCal_on()):
            regionCreatorList += ['FCALPhysicsRegionTool']
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
        if (DetFlags.HEC_on()):
            regionCreatorList += ['HECPhysicsRegionTool']
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
        if (DetFlags.em_on()):
            regionCreatorList += ['EMECPhysicsRegionTool']
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
        if (simFlags.LArTB_H6Hec.get_Value()):
            regionCreatorList += ['HECPhysicsRegionTool']
        if (simFlags.LArTB_H6Emec.get_Value()):
            regionCreatorList += ['EMECPhysicsRegionTool']
        if (simFlags.LArTB_H6Fcal.get_Value()):
            regionCreatorList += ['FCALPhysicsRegionTool']
    return regionCreatorList

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

def getATLAS_FieldMgrList():
    fieldMgrList = []
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.TightMuonStepping.statusOn or\
       not simFlags.TightMuonStepping():
        fieldMgrList += ['ATLASFieldManager']
    else:
        fieldMgrList += ['TightMuonsATLASFieldManager']

    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.bpipe_on():
        fieldMgrList += ['BeamPipeFieldManager']
    if DetFlags.ID_on():
        fieldMgrList += ['InDetFieldManager']
    if DetFlags.Calo_on() and simFlags.MuonFieldOnlyInCalo.statusOn and simFlags.MuonFieldOnlyInCalo():
        fieldMgrList += ['MuonsOnlyInCaloFieldManager']
    if DetFlags.Muon_on():
        fieldMgrList += ['MuonFieldManager']
    if simFlags.ForwardDetectors.statusOn:
        if DetFlags.geometry.FwdRegion_on():
            fieldMgrList += ['Q1FwdFieldManager',
                             'Q2FwdFieldManager',
                             'Q3FwdFieldManager',
                             'D1FwdFieldManager',
                             'D2FwdFieldManager',
                             'Q4FwdFieldManager',
                             'Q5FwdFieldManager',
                             'Q6FwdFieldManager',
                             'Q7FwdFieldManager',
                             'Q1HKickFwdFieldManager',
                             'Q1VKickFwdFieldManager',
                             'Q2HKickFwdFieldManager',
                             'Q2VKickFwdFieldManager',
                             'Q3HKickFwdFieldManager',
                             'Q3VKickFwdFieldManager',
                             'Q4VKickAFwdFieldManager',
                             'Q4HKickFwdFieldManager',
                             'Q4VKickBFwdFieldManager',
                             'Q5HKickFwdFieldManager',
                             'Q6VKickFwdFieldManager',
                             'FwdRegionFieldManager']
    return fieldMgrList

def getCTB_FieldMgrList():
    fieldMgrList = []
    return fieldMgrList

def getTB_FieldMgrList():
    fieldMgrList = []
    return fieldMgrList

def getGeometryConfigurationTools():
    geoConfigToolList = []
    # CfgGetter methods for these tools should be defined in the
    # package containing each tool, so G4AtlasTools in this case
    geoConfigToolList += ["MaterialDescriptionTool"]
    return geoConfigToolList

def getDetectorGeometrySvc(name="DetectorGeometrySvc", **kwargs):
    kwargs.setdefault("DetectorConstruction", 'G4AtlasDetectorConstructionTool')
    ## For now just have the same geometry configurations tools loaded for ATLAS and TestBeam
    kwargs.setdefault("GeometryConfigurationTools", getGeometryConfigurationTools())
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags,"Eta"): #FIXME ugly hack
        kwargs.setdefault("World", 'TileTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList())
        kwargs.setdefault("FieldManagers", getTB_FieldMgrList())
    elif hasattr(simFlags,"LArTB_H1TableYPos"): #FIXME ugly hack
        kwargs.setdefault("World", 'LArTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList())
        kwargs.setdefault("FieldManagers", getTB_FieldMgrList())
    else:
        if simFlags.SimulateCavern.get_Value():
            kwargs.setdefault("World", 'Cavern')
        else:
            kwargs.setdefault("World", 'Atlas')
        kwargs.setdefault("RegionCreators", getATLAS_RegionCreatorList())
        if hasattr(simFlags, 'MagneticField') and simFlags.MagneticField.statusOn:
            kwargs.setdefault("FieldManagers", getATLAS_FieldMgrList())
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
