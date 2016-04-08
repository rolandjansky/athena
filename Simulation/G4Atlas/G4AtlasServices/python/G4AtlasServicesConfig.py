# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
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

def getDetectorGeometrySvc(name="DetectorGeometrySvc", **kwargs):
    kwargs.setdefault("DetectorConstruction", 'G4AtlasDetectorConstructionTool')
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags,"Eta"): #FIXME ugly hack
        kwargs.setdefault("World", 'TileTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList())
    elif hasattr(simFlags,"LArTB_H1TableYPos"): #FIXME ugly hack
        kwargs.setdefault("World", 'LArTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList())
    else:
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == 'cosmics' or \
                (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            kwargs.setdefault("World", 'Cavern')
        else:
            kwargs.setdefault("World", 'Atlas')
        kwargs.setdefault("RegionCreators", getATLAS_RegionCreatorList())
    return CfgMgr.DetectorGeometrySvc(name, **kwargs)

def getG4AtlasSvc(name="G4AtlasSvc", **kwargs):
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return CfgMgr.G4AtlasSvc(name, **kwargs)

def getG4GeometryNotifierSvc(name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return CfgMgr.G4GeometryNotifierSvc(name, **kwargs)
