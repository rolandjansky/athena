# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AthenaCommon import CfgGetter
#from G4AtlasApps.SimFlags import simFlags
#from AthenaCommon.BeamFlags import jobproperties

from G4AtlasServices.G4AtlasServicesConf import DetectorGeometrySvc, G4AtlasSvc, G4GeometryNotifierSvc
#the physics region tools
from G4AtlasTools.G4PhysicsRegionConfigNew import SX1PhysicsRegionToolCfg, BedrockPhysicsRegionToolCfg, CavernShaftsConcretePhysicsRegionToolCfg, PixelPhysicsRegionToolCfg, SCTPhysicsRegionToolCfg, TRTPhysicsRegionToolCfg, TRT_ArPhysicsRegionToolCfg, BeampipeFwdCutPhysicsRegionToolCfg, FWDBeamLinePhysicsRegionToolCfg, EMBPhysicsRegionToolCfg, EMECPhysicsRegionToolCfg, HECPhysicsRegionToolCfg, FCALPhysicsRegionToolCfg, DriftWallPhysicsRegionToolCfg, DriftWall1PhysicsRegionToolCfg, DriftWall2PhysicsRegionToolCfg, MuonSystemFastPhysicsRegionToolCfg

"""FwdRegionPhysicsRegionToolCfg,  
 TRT_KrPhysicsRegionToolCfg,  
 EMECParaPhysicsRegionToolCfg, 
 FCALParaPhysicsRegionToolCfg, 
 FCAL2ParaPhysicsRegionToolCfg, 
 PreSampLArPhysicsRegionToolCfg, 
 DeadMaterialPhysicsRegionToolCfg, 
 MuonPhysicsRegionToolCfg, 
 SX1PhysicsRegionToolCfg,  
 SCTSiliconPhysicsRegionToolCfg"""

from G4AtlasTools.G4GeometryToolConfig import MaterialDescriptionToolCfg, G4AtlasDetectorConstructionToolCfg, ATLASEnvelopeCfg
from G4AtlasTools.G4FieldConfigNew import ATLASFieldManagerToolCfg, TightMuonsATLASFieldManagerToolCfg, BeamPipeFieldManagerToolCfg, InDetFieldManagerToolCfg, MuonsOnlyInCaloFieldManagerToolCfg, MuonFieldManagerToolCfg, Q1FwdFieldManagerToolCfg, Q2FwdFieldManagerToolCfg, Q3FwdFieldManagerToolCfg, D1FwdFieldManagerToolCfg, D2FwdFieldManagerToolCfg, Q4FwdFieldManagerToolCfg, Q5FwdFieldManagerToolCfg, Q6FwdFieldManagerToolCfg, Q7FwdFieldManagerToolCfg, Q1HKickFwdFieldManagerToolCfg, Q1VKickFwdFieldManagerToolCfg, Q2HKickFwdFieldManagerToolCfg, Q2VKickFwdFieldManagerToolCfg, Q3HKickFwdFieldManagerToolCfg, Q3VKickFwdFieldManagerToolCfg, Q4VKickAFwdFieldManagerToolCfg, Q4HKickFwdFieldManagerToolCfg, Q4VKickBFwdFieldManagerToolCfg, Q5HKickFwdFieldManagerToolCfg,  Q6VKickFwdFieldManagerToolCfg, FwdRegionFieldManagerToolCfg
#import G4AtlasTools.G4FieldConfigNew
"""ClassicFieldManagerToolCfg
BasicDetectorFieldManagerToolCfg
BasicFwdFieldManagerToolCfg"""

#already imported - is this correct?
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def getATLAS_RegionCreatorList(ConfigFlags):
    regionCreatorList = []
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags


    isUpgrade = ConfigFlags.GeoModel.Run =="RUN4" 
    isRUN2 = (ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]) or (commonGeoFlags.Run()=="UNDEFINED" and geoFlags.isIBL())

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.BeamFlags import jobproperties
    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal':
        regionCreatorList += [SX1PhysicsRegionToolCfg(ConfigFlags), BedrockPhysicsRegionToolCfg(ConfigFlags), CavernShaftsConcretePhysicsRegionToolCfg(ConfigFlags)]
        #regionCreatorList += ['CavernShaftsAirPhysicsRegionTool'] # Not used currently
    if ConfigFlags.Detector.SimulateID:
        if ConfigFlags.Detector.SimulatePixel:
            regionCreatorList += [PixelPhysicsRegionToolCfg(ConfigFlags)]
        if ConfigFlags.Detector.SimulateSCT:
            regionCreatorList += [SCTPhysicsRegionToolCfg(ConfigFlags)]
        if ConfigFlags.Detector.SimulateTRT and not isUpgrade:
            regionCreatorList += [TRTPhysicsRegionToolCfg(ConfigFlags)]
            if isRUN2:
                regionCreatorList += [TRT_ArPhysicsRegionToolCfg(ConfigFlags)] #'TRT_KrPhysicsRegionTool'
        # FIXME dislike the ordering here, but try to maintain the same ordering as in the old configuration.
        if ConfigFlags.Detector.SimulateBpipe:
            #if simFlags.BeamPipeSimMode.statusOn and simFlags.BeamPipeSimMode() != "Normal":
            if True:
                regionCreatorList += [BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags)]
            #if simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
            if True:
                regionCreatorList += [FWDBeamLinePhysicsRegionToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateCalo:
        if ConfigFlags.Detector.GeometryLAr:
            ## Shower parameterization overrides the calibration hit flag
            if simFlags.LArParameterization.statusOn and simFlags.LArParameterization() > 0 \
                    and ConfigFlags.Sim.CalibrationRun in ['LAr','LAr+Tile','DeadLAr']:
                print 'You requested both calibration hits and frozen showers / parameterization in the LAr.'
                print '  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.'
                print '  Please try again with a different value of either simFlags.LArParameterization (' + str(simFlags.LArParameterization()) + ') or simFlags.CalibrationRun ('+str(ConfigFlags.Sim.CalibrationRun)+')'
                raise RuntimeError('Configuration not allowed')
            if simFlags.LArParameterization() > 0:
                regionCreatorList += [EMBPhysicsRegionTool(ConfigFlags), EMECPhysicsRegionTool(ConfigFlags),
                                      HECPhysicsRegionTool(ConfigFlags), FCALPhysicsRegionTool(ConfigFlags)]
                # FIXME 'EMBPhysicsRegionTool' used for parametrization also - do we need a second instance??
                regionCreatorList += [EMECParaPhysicsRegionTool(ConfigFlags),
                                      FCALParaPhysicsRegionTool(ConfigFlags), FCAL2ParaPhysicsRegionTool(ConfigFlags)]
                if simFlags.LArParameterization.get_Value() > 1:
                    regionCreatorList += [PreSampLArPhysicsRegionTool(ConfigFlags), DeadMaterialPhysicsRegionTool(ConfigFlags)]
            elif simFlags.LArParameterization() is None or simFlags.LArParameterization() == 0:
                regionCreatorList += [EMBPhysicsRegionTool(ConfigFlags), EMECPhysicsRegionTool(ConfigFlags),
                                      HECPhysicsRegionTool(ConfigFlags), FCALPhysicsRegionTool(ConfigFlags)]
    ## FIXME _initPR never called for FwdRegion??
    #if simFlags.ForwardDetectors.statusOn:
    #    if DetFlags.geometry.FwdRegion_on():
    #        regionCreatorList += ['FwdRegionPhysicsRegionTool']
    if ConfigFlags.Detector.GeometryMuon:
        regionCreatorList += [DriftWallPhysicsRegionTool(ConfigFlags), DriftWall1PhysicsRegionTool(ConfigFlags), DriftWall2PhysicsRegionTool(ConfigFlags)]
        if ConfigFlags.Sim.CavernBG != 'Read' and not (simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
            regionCreatorList += [MuonSystemFastPhysicsRegionTool(ConfigFlags)]
    return regionCreatorList

#not called anywhere?
def getCTB_RegionCreatorList(ConfigFlags):
    regionCreatorList = []
    from G4AtlasApps.SimFlags import simFlags
    ## FIXME _initPR never called for SCT??
    #if DetFlags.ID_on():
    #    if DetFlags.geometry.SCT_on():
    #        regionCreatorList += ['SCTSiliconPhysicsRegionTool']
    if ConfigFlags.Detector.SimulateCalo:
        eta=simFlags.Eta.get_Value()
        if eta>=0 and eta<1.201:
            if ConfigFlags.Detector.SimulateLAr:
                regionCreatorList += [EMBPhysicsRegionTool(ConfigFlags)]
    if ConfigFlags.Detector.SimulateMuon:
        regionCreatorList += [DriftWallPhysicsRegionTool(ConfigFlags), DriftWall1PhysicsRegionTool(ConfigFlags), DriftWall2PhysicsRegionTool(ConfigFlags)]
    return regionCreatorList

def getTB_RegionCreatorList(ConfigFlags):
    regionCreatorList = []
    from G4AtlasApps.SimFlags import simFlags

    if (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2003"):
        if (ConfigFlags.Detector.SimulateLAr):
            regionCreatorList += [FCALPhysicsRegionTool(ConfigFlags)]
    elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2002"):
        if (ConfigFlags.Detector.SimulateLAr):
            regionCreatorList += [HECPhysicsRegionTool(ConfigFlags)]
    elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6EC_2002"):
        if (ConfigFlags.Detector.SimulateLAr):
            regionCreatorList += [EMECPhysicsRegionTool(ConfigFlags)]
    elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2004"):
        if (simFlags.LArTB_H6Hec.get_Value()):
            regionCreatorList += [HECPhysicsRegionTool(ConfigFlags)]
        if (simFlags.LArTB_H6Emec.get_Value()):
            regionCreatorList += [EMECPhysicsRegionTool(ConfigFlags)]
        if (simFlags.LArTB_H6Fcal.get_Value()):
            regionCreatorList += [FCALPhysicsRegionTool(ConfigFlags)]
    return regionCreatorList

#########################################################################
def getATLAS_FieldMgrList(ConfigFlags):
    result = ComponentAccumulator()
    fieldMgrList = []
    from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.TightMuonStepping.statusOn or\
    #   not simFlags.TightMuonStepping():
    if False:
        fieldMgrList += [ATLASFieldManagerToolCfg(ConfigFlags)]
    else:
        acc, tool = TightMuonsATLASFieldManagerToolCfg(ConfigFlags)
        #fieldMgrList += [acc.getPublicTool('TightMuonsATLASFieldManager')] # issue with public tools?
        fieldMgrList += [tool]
        result.merge(acc)

    from AthenaCommon.DetFlags import DetFlags
    if ConfigFlags.Detector.SimulateBpipe:
        fieldMgrList += [BeamPipeFieldManagerToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateID:
        fieldMgrList += [InDetFieldManagerToolCfg(ConfigFlags)]
    #if ConfigFlags.Detector.SimulateCalo and simFlags.MuonFieldOnlyInCalo.statusOn and simFlags.MuonFieldOnlyInCalo():
    if False:
        fieldMgrList += [MuonsOnlyInCaloFieldManagerToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateMuon:
        fieldMgrList += [MuonFieldManagerToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateForward: #needed?
        if ConfigFlags.Detector.GeometryFwdRegion: #or forward?
            fieldMgrList += [Q1FwdFieldManagerToolCfg(ConfigFlags),
                             Q2FwdFieldManagerToolCfg(ConfigFlags),
                             Q3FwdFieldManagerToolCfg(ConfigFlags),
                             D1FwdFieldManagerToolCfg(ConfigFlags),
                             D2FwdFieldManagerToolCfg(ConfigFlags),
                             Q4FwdFieldManagerToolCfg(ConfigFlags),
                             Q5FwdFieldManagerToolCfg(ConfigFlags),
                             Q6FwdFieldManagerToolCfg(ConfigFlags),
                             Q7FwdFieldManagerToolCfg(ConfigFlags),
                             Q1HKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q1VKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q2HKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q2VKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q3HKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q3VKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q4VKickAFwdFieldManagerToolCfg(ConfigFlags),
                             Q4HKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q4VKickBFwdFieldManagerToolCfg(ConfigFlags),
                             Q5HKickFwdFieldManagerToolCfg(ConfigFlags),
                             Q6VKickFwdFieldManagerToolCfg(ConfigFlags),
                             FwdRegionFieldManagerToolCfg(ConfigFlags)]
    result.setPrivateTools(fieldMgrList)
    return result, fieldMgrList

#called?
def getCTB_FieldMgrList(ConfigFlags):
    fieldMgrList = []
    return fieldMgrList

def getTB_FieldMgrList(ConfigFlags):
    fieldMgrList = []
    return fieldMgrList

def getGeometryConfigurationTools(ConfigFlags):
    geoConfigToolList = []
    # The methods for these tools should be defined in the
    # package containing each tool, so G4AtlasTools in this case
    geoConfigToolList += [MaterialDescriptionToolCfg(ConfigFlags)]
    return geoConfigToolList

def DetectorGeometrySvcCfg(ConfigFlags, name="DetectorGeometrySvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("DetectorConstruction", G4AtlasDetectorConstructionToolCfg(ConfigFlags))
    ## For now just have the same geometry configurations tools loaded for ATLAS and TestBeam
    kwargs.setdefault("GeometryConfigurationTools", getGeometryConfigurationTools(ConfigFlags))
    from G4AtlasApps.SimFlags import simFlags
    #if hasattr(simFlags,"Eta"): #FIXME ugly hack
    if False:
        kwargs.setdefault("World", 'TileTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList(ConfigFlags))
        kwargs.setdefault("FieldManagers", getTB_FieldMgrList(ConfigFlags))
    #elif hasattr(simFlags,"LArTB_H1TableYPos"): #FIXME ugly hack
    elif False:
        kwargs.setdefault("World", 'LArTB_World')
        kwargs.setdefault("RegionCreators", getTB_RegionCreatorList(ConfigFlags))
        kwargs.setdefault("FieldManagers", getTB_FieldMgrList(ConfigFlags))
    else:
        #if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal':
        if False:
            kwargs.setdefault("World", 'Cavern')
        else:
            accGeo, toolGeo = ATLASEnvelopeCfg(ConfigFlags)
            kwargs.setdefault("World", toolGeo)
            result.merge(accGeo)
        kwargs.setdefault("RegionCreators", getATLAS_RegionCreatorList(ConfigFlags))
        #if hasattr(simFlags, 'MagneticField') and simFlags.MagneticField.statusOn:
        if True:
            acc, fieldMgrList = getATLAS_FieldMgrList(ConfigFlags)
            kwargs.setdefault("FieldManagers", fieldMgrList ) #causing issues...!
            tool = result.popToolsAndMerge(acc)
            result.setPrivateTools(tool)

    result.addService(DetectorGeometrySvc(name, **kwargs))
    return result

def G4AtlasSvcCfg(ConfigFlags, name="G4AtlasSvc", **kwargs):
    if ConfigFlags.Concurrency.NumThreads > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault("isMT", is_hive)
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return G4AtlasSvc(name, **kwargs)

def G4GeometryNotifierSvcCfg(ConfigFlags, name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return G4GeometryNotifierSvc(name, **kwargs)






if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Detector.SimulateBpipe = True
  ConfigFlags.Detector.SimulateID = True
  ConfigFlags.Detector.SimulateCalo = True 
  ConfigFlags.Detector.SimulateMuon = True
  ConfigFlags.Detector.SimulateForward = True
  ConfigFlags.Detector.GeometryFwdRegion = True


  ConfigFlags.Sim.CavernBG = "Signal"  #for it to go via atlas?
  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000
  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesSerialCfg()

  #add the algorithm
  acc = DetectorGeometrySvcCfg(ConfigFlags)

  cfg.addService(acc.getService("DetectorGeometrySvc"))

  tool = cfg.popToolsAndMerge(acc)
  #cfg.setPrivateTools(tool) #need to set private tools?



  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"
