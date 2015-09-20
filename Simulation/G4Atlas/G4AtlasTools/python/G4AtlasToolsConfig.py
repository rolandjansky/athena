# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def generateFastSimulationList():
    FastSimulationList=[]
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.bpipe_on():
        if hasattr(simFlags, 'ForwardDetectors') and simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
            FastSimulationList += ['ForwardTransportModel']
        if hasattr(simFlags, 'BeamPipeSimMode') and simFlags.BeamPipeSimMode.statusOn and simFlags.BeamPipeSimMode() != "Normal":
            FastSimulationList += [ 'SimpleFastKiller' ]
    if DetFlags.geometry.LAr_on():
        ## Shower parameterization overrides the calibration hit flag
        if simFlags.LArParameterization.statusOn and simFlags.LArParameterization() > 0 \
                and simFlags.CalibrationRun.statusOn and simFlags.CalibrationRun.get_Value() in ['LAr','LAr+Tile','DeadLAr']:
            print 'getFastSimulationMasterTool FATAL :: You requested both calibration hits and frozen showers / parameterization in the LAr.'
            print '  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.'
            print '  Please try again with a different value of simFlags.LArParameterization or simFlags.CalibrationRun '
            raise RuntimeError('Configuration not allowed')
        if simFlags.LArParameterization() > 0:
            #FIXME If we're only using Frozen Showers in the FCAL do we really need to set up the EMB and EMEC as well?
            FastSimulationList += ['EMBFastShower', 'EMECFastShower', 'FCALFastShower', 'FCAL2FastShower']
            if simFlags.LArParameterization.get_Value() > 1:
                 FastSimulationList += ['DeadMaterialShower']
        elif simFlags.LArParameterization() is None or simFlags.LArParameterization() == 0:
            print "getFastSimulationMasterTool INFO No Frozen Showers"
    if DetFlags.Muon_on():
        if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read' and\
           not (simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
            FastSimulationList += ['NeutronFastSim']
    return FastSimulationList

def getFastSimulationMasterTool(name="FastSimulationMasterTool", **kwargs):
    kwargs.setdefault("FastSimulations", generateFastSimulationList())
    return CfgMgr.FastSimulationMasterTool(name, **kwargs)

def generateFwdSensitiveDetectorList():
    SensitiveDetectorList=[]
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.Lucid_on():
        SensitiveDetectorList += [ 'LUCID_SensitiveDetector' ]
    if hasattr(simFlags, 'ForwardDetectors') and simFlags.ForwardDetectors.statusOn:
        if DetFlags.simulate.ZDC_on():
            SensitiveDetectorList += [ 'ZDC_PixelSD', 'ZDC_StripSD' ]
        if DetFlags.simulate.ALFA_on():
            SensitiveDetectorList += [ 'ALFA_SensitiveDetector' ]
        if DetFlags.simulate.AFP_on():
            SensitiveDetectorList += [ 'AFP_SensitiveDetector' ]
            #SensitiveDetectorList += [ 'AFP_SiDSensitiveDetector', 'AFP_TDSensitiveDetector' ]

    return SensitiveDetectorList

def generateTrackFastSimSensitiveDetectorList():
    SensitiveDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    from G4AtlasApps.SimFlags import simFlags
    if (DetFlags.Muon_on() and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read' and 'Write' in simFlags.CavernBG.get_Value()) or (hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn):
        SensitiveDetectorList += [ 'TrackFastSimSD' ]
    return SensitiveDetectorList

def generateInDetSensitiveDetectorList():
    SensitiveDetectorList=[]

    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
    isUpgrade = geoFlags.Run()=="RUN3" or geoFlags.Run()=="RUN4" or (geoFlags.Run()=="UNDEFINED" and geoFlags.isSLHC())
    isRUN2 = geoFlags.Run()=="RUN2" or (geoFlags.Run()=="UNDEFINED" and geoFlags.isIBL())
    isRUN1 = not (isRUN2 or isUpgrade)

    from AthenaCommon.DetFlags import DetFlags
    if (isRUN1 or isRUN2) and DetFlags.simulate.BCM_on():
        SensitiveDetectorList += [ 'BCMSensorSD' ]
    #if isRUN2 and DetFlags.simulation.DBM_on():
    #    SensitiveDetectorList += [ 'DBMSensorSD' ]
    if DetFlags.simulate.pixel_on():
        if isRUN1 or isRUN2:
            SensitiveDetectorList += [ 'BLMSensorSD' ]
        SensitiveDetectorList += [ 'PixelSensorSD' ]
    if DetFlags.simulate.SCT_on():
        if isUpgrade:
            SensitiveDetectorList += [ 'SLHC_SctSensorSD' ]
        else:
            SensitiveDetectorList += [ 'SctSensorSD' ]
    if DetFlags.simulate.TRT_on() and not isUpgrade:
        SensitiveDetectorList += [ 'TRTSensitiveDetector' ]
    return SensitiveDetectorList

def generateCaloSensitiveDetectorList():
    SensitiveDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.LAr_on():
        SensitiveDetectorList += [ 'LArEMBSensitiveDetector','LArEMECSensitiveDetector','LArFCALSensitiveDetector',\
                                       'LArHECSensitiveDetector','LArMiniFCALSensitiveDetector']
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.CalibrationRun.get_Value() in ['LAr', 'LAr+Tile']:
            SensitiveDetectorList += [ 'LArDeadSensitiveDetector','LArInactiveSensitiveDetector','LArActiveSensitiveDetector' ]
        elif simFlags.CalibrationRun.get_Value() == 'DeadLAr':
            SensitiveDetectorList += [ 'LArDeadSensitiveDetector' ]

    if DetFlags.simulate.Tile_on():
        from G4AtlasApps.SimFlags import simFlags
        if DetFlags.simulate.LAr_on():
            SensitiveDetectorList += [ 'MinBiasScintillatorSD' ]
        if simFlags.CalibrationRun.statusOn and (simFlags.CalibrationRun.get_Value() in ['Tile', 'LAr+Tile']):
            SensitiveDetectorList += [ 'TileGeoG4CalibSD' ] # mode 1 : With CaloCalibrationHits
        else:
            SensitiveDetectorList += [ 'TileGeoG4SD' ]      # mode 0 : No CaloCalibrationHits
    return SensitiveDetectorList

def generateMuonSensitiveDetectorList():
    SensitiveDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.Muon_on():
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == 'cosmics':
            SensitiveDetectorList += [ 'MDTSensitiveDetectorCosmics' , 'RPCSensitiveDetectorCosmics' , 'TGCSensitiveDetectorCosmics' ]
        else:
            SensitiveDetectorList += [ 'MDTSensitiveDetector' , 'RPCSensitiveDetector' , 'TGCSensitiveDetector' ]
        if simFlags.SimulateNewSmallWheel():
            SensitiveDetectorList += [ 'sTGCSensitiveDetector' , 'MicromegasSensitiveDetector' ]
        else:
            # CSCs built instead of NSW
            if jobproperties.Beam.beamType() == 'cosmics':
                SensitiveDetectorList += [ 'CSCSensitiveDetectorCosmics' ]
            else:
                SensitiveDetectorList += [ 'CSCSensitiveDetector' ]
    return SensitiveDetectorList

def generateEnvelopeSensitiveDetectorList():
    SensitiveDetectorList=[]
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    from AthenaCommon.DetFlags import DetFlags
    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, "ReadTR") and not simFlags.ReadTR.statusOn:
        SensitiveDetectorList+=['CosmicRecord']
    if DetFlags.Truth_on() and not simFlags.ISFRun:
        if DetFlags.ID_on():
            SensitiveDetectorList+=['CaloEntryLayer']
        if DetFlags.Calo_on():
            SensitiveDetectorList+=['MuonEntryLayer']
        if DetFlags.geometry.Muon_on():
            SensitiveDetectorList+=['MuonExitLayer']
    return SensitiveDetectorList

def generateSensitiveDetectorList():
    SensitiveDetectorList=[]
    SensitiveDetectorList += generateEnvelopeSensitiveDetectorList()
    SensitiveDetectorList += generateInDetSensitiveDetectorList()
    SensitiveDetectorList += generateCaloSensitiveDetectorList()
    SensitiveDetectorList += generateMuonSensitiveDetectorList()
    SensitiveDetectorList += generateTrackFastSimSensitiveDetectorList()
    SensitiveDetectorList += generateFwdSensitiveDetectorList()
    return SensitiveDetectorList

def generateTestBeamSensitiveDetectorList():
    SensitiveDetectorList=[]
    from G4AtlasApps.SimFlags import simFlags
    if (hasattr(simFlags, "LArFarUpstreamMaterial") and
        simFlags.LArFarUpstreamMaterial.statusOn and
        simFlags.LArFarUpstreamMaterial.get_Value()):
        SensitiveDetectorList += [ 'LArFarUpstreamMaterialExitLayer' ]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.pixel_on():
        SensitiveDetectorList += [ 'PixelSensor_CTB' ]
    if DetFlags.simulate.SCT_on():
        SensitiveDetectorList += [ 'SctSensor_CTB' ]
    if DetFlags.simulate.TRT_on():
        SensitiveDetectorList += [ 'TRTSensitiveDetector_CTB' ]
    if DetFlags.simulate.LAr_on():
        SensitiveDetectorList += [ 'LArEMBSensitiveDetector' ]
        if simFlags.CalibrationRun.statusOn and ('LAr' in simFlags.CalibrationRun.get_Value()):
            SensitiveDetectorList += [ 'LArH8CalibSensitiveDetector' ] # mode 1 : With CaloCalibrationHits
    if DetFlags.simulate.Tile_on():
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.CalibrationRun.statusOn and (simFlags.CalibrationRun.get_Value() in ['Tile', 'LAr+Tile']):
            SensitiveDetectorList += [ 'TileCTBGeoG4CalibSD' ] # mode 1 : With CaloCalibrationHits
        else:
            SensitiveDetectorList += [ 'TileCTBGeoG4SD' ]      # mode 0 : No CaloCalibrationHits
    if DetFlags.geometry.Muon_on():
        SensitiveDetectorList += [ 'MuonEntryRecord' ]
    SensitiveDetectorList += generateMuonSensitiveDetectorList()
    return SensitiveDetectorList

def getSensitiveDetectorMasterTool(name="SensitiveDetectorMasterTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if "ATLAS" in simFlags.SimLayout():
        kwargs.setdefault("SensitiveDetectors", generateSensitiveDetectorList())
    elif "tb_Tile2000_2003" in simFlags.SimLayout():
        kwargs.setdefault("SensitiveDetectors", generateTestBeamSensitiveDetectorList())
    elif "tb_LArH6" in simFlags.SimLayout():
        pass
    elif "ctbh8" in simFlags.SimLayout():
        kwargs.setdefault("SensitiveDetectors", generateTestBeamSensitiveDetectorList())
    return CfgMgr.SensitiveDetectorMasterTool(name, **kwargs)

def getPhysicsListToolBase(name="PhysicsListToolBase", **kwargs):
    return CfgMgr.PhysicsListToolBase(name, **kwargs)
