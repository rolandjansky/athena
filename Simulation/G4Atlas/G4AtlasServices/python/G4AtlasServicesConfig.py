# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr, CfgGetter
#from G4AtlasApps.SimFlags import simFlags
#from AthenaCommon.BeamFlags import jobproperties

def getATLAS_RegionCreatorList():
    regionCreatorList = []
    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
    isUpgrade = geoFlags.Run()=="RUN3" or geoFlags.Run()=="RUN4" or (geoFlags.Run()=="UNDEFINED" and geoFlags.isSLHC())
    isRUN2 = geoFlags.Run()=="RUN2" or (geoFlags.Run()=="UNDEFINED" and geoFlags.isIBL())

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or \
           (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
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
                print 'You requested both calibration hits and frozen showers / parameterization in the LAr.'
                print '  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.'
                print '  Please try again with a different value of either simFlags.LArParameterization (' + str(simFlags.LArParameterization()) + ') or simFlags.CalibrationRun ('+str(simFlags.CalibrationRun.get_Value())+')'
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
    import MagFieldServices.SetupField
    kwargs.setdefault("MagneticFieldSvc", "AtlasFieldSvc") # TODO This should probably be based on simFlags.MagneticField?
    #kwargs.setdefault("FieldOn", True)
    return CfgMgr.StandardFieldSvc(name, **kwargs)

def getForwardFieldSvc(name="ForwardField", **kwargs):
    #FIXME Once it exists this version should use the new MagField Service defined in ForwardRegionMgField
    kwargs.setdefault("MagneticFieldSvc", "AtlasFieldSvc")
    #kwargs.setdefault("FieldOn", True)
    return CfgMgr.StandardFieldSvc(name, **kwargs)

def getQ1FwdG4FieldSvc(name='Q1FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ2FwdG4FieldSvc(name='Q2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ3FwdG4FieldSvc(name='Q3FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getD1FwdG4FieldSvc(name='D1FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D1")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getD2FwdG4FieldSvc(name='D2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D2")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ4FwdG4FieldSvc(name='Q4FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ5FwdG4FieldSvc(name='Q5FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ6FwdG4FieldSvc(name='Q6FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ7FwdG4FieldSvc(name='Q7FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q7")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ1HKickFwdG4FieldSvc(name='Q1HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1HKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ1VKickFwdG4FieldSvc(name='Q1VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1VKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ2HKickFwdG4FieldSvc(name='Q2HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2HKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ2VKickFwdG4FieldSvc(name='Q2VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2VKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ3HKickFwdG4FieldSvc(name='Q3HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3HKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ3VKickFwdG4FieldSvc(name='Q3VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3VKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ4VKickAFwdG4FieldSvc(name='Q4VKickAFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickA")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ4HKickFwdG4FieldSvc(name='Q4HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4HKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ4VKickBFwdG4FieldSvc(name='Q4VKickBFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickB")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ5HKickFwdG4FieldSvc(name='Q5HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5HKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)
def getQ6VKickFwdG4FieldSvc(name='Q6VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6VKick")
    return CfgMgr.StandardFieldSvc(name, **kwargs)

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
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == 'cosmics' or \
                (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            kwargs.setdefault("World", 'Cavern')
        else:
            kwargs.setdefault("World", 'Atlas')
        kwargs.setdefault("RegionCreators", getATLAS_RegionCreatorList())
        if hasattr(simFlags, 'MagneticField') and simFlags.MagneticField.statusOn:
            kwargs.setdefault("FieldManagers", getATLAS_FieldMgrList())
    return CfgMgr.DetectorGeometrySvc(name, **kwargs)

def getG4AtlasSvc(name="G4AtlasSvc", **kwargs):
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return CfgMgr.G4AtlasSvc(name, **kwargs)

def getG4GeometryNotifierSvc(name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return CfgMgr.G4GeometryNotifierSvc(name, **kwargs)

#back-compatibility hack
from G4AtlasServices.G4AtlasUserActionConfig import addAction
