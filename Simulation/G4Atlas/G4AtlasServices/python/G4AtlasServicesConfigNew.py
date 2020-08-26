# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

DetectorGeometrySvc, G4AtlasSvc, G4GeometryNotifierSvc, PhysicsListSvc=CompFactory.getComps("DetectorGeometrySvc","G4AtlasSvc","G4GeometryNotifierSvc","PhysicsListSvc",)
#the physics region tools
from G4AtlasTools.G4PhysicsRegionConfigNew import SX1PhysicsRegionToolCfg, BedrockPhysicsRegionToolCfg, CavernShaftsConcretePhysicsRegionToolCfg, PixelPhysicsRegionToolCfg, SCTPhysicsRegionToolCfg, TRTPhysicsRegionToolCfg, TRT_ArPhysicsRegionToolCfg,ITkPixelPhysicsRegionToolCfg,ITkStripPhysicsRegionToolCfg,BeampipeFwdCutPhysicsRegionToolCfg, FWDBeamLinePhysicsRegionToolCfg, EMBPhysicsRegionToolCfg, EMECPhysicsRegionToolCfg, HECPhysicsRegionToolCfg, FCALPhysicsRegionToolCfg, FCAL2ParaPhysicsRegionToolCfg, EMECParaPhysicsRegionToolCfg, FCALParaPhysicsRegionToolCfg, PreSampLArPhysicsRegionToolCfg, DeadMaterialPhysicsRegionToolCfg
from G4AtlasTools.G4PhysicsRegionConfigNew import DriftWallPhysicsRegionToolCfg, DriftWall1PhysicsRegionToolCfg, DriftWall2PhysicsRegionToolCfg

#the geometry tools
from G4AtlasTools.G4GeometryToolConfig import MaterialDescriptionToolCfg, G4AtlasDetectorConstructionToolCfg, ATLASEnvelopeCfg
#the field config tools
from G4AtlasTools.G4FieldConfigNew import ATLASFieldManagerToolCfg, TightMuonsATLASFieldManagerToolCfg, BeamPipeFieldManagerToolCfg, InDetFieldManagerToolCfg, MuonsOnlyInCaloFieldManagerToolCfg, MuonFieldManagerToolCfg, Q1FwdFieldManagerToolCfg, Q2FwdFieldManagerToolCfg, Q3FwdFieldManagerToolCfg, D1FwdFieldManagerToolCfg, D2FwdFieldManagerToolCfg, Q4FwdFieldManagerToolCfg, Q5FwdFieldManagerToolCfg, Q6FwdFieldManagerToolCfg, Q7FwdFieldManagerToolCfg, Q1HKickFwdFieldManagerToolCfg, Q1VKickFwdFieldManagerToolCfg, Q2HKickFwdFieldManagerToolCfg, Q2VKickFwdFieldManagerToolCfg, Q3HKickFwdFieldManagerToolCfg, Q3VKickFwdFieldManagerToolCfg, Q4VKickAFwdFieldManagerToolCfg, Q4HKickFwdFieldManagerToolCfg, Q4VKickBFwdFieldManagerToolCfg, Q5HKickFwdFieldManagerToolCfg,  Q6VKickFwdFieldManagerToolCfg, FwdRegionFieldManagerToolCfg
from AthenaCommon import Logging

def getATLAS_RegionCreatorList(ConfigFlags):
    regionCreatorList = []

    isRUN2 = (ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]) or (ConfigFlags.GeoModel.Run=="UNDEFINED" and ConfigFlags.GeoModel.IBLLayout not in ["noIBL", "UNDEFINED"])

    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal':
        regionCreatorList += [SX1PhysicsRegionToolCfg(ConfigFlags), BedrockPhysicsRegionToolCfg(ConfigFlags), CavernShaftsConcretePhysicsRegionToolCfg(ConfigFlags)]
        #regionCreatorList += ['CavernShaftsAirPhysicsRegionTool'] # Not used currently
    if ConfigFlags.Detector.SimulateID:
        if ConfigFlags.Detector.SimulatePixel:
            regionCreatorList += [PixelPhysicsRegionToolCfg(ConfigFlags)]
        if ConfigFlags.Detector.SimulateSCT:
            regionCreatorList += [SCTPhysicsRegionToolCfg(ConfigFlags)]
        if ConfigFlags.Detector.SimulateTRT:
            regionCreatorList += [TRTPhysicsRegionToolCfg(ConfigFlags)]
            if isRUN2:
                regionCreatorList += [TRT_ArPhysicsRegionToolCfg(ConfigFlags)] #'TRT_KrPhysicsRegionTool'
        # FIXME dislike the ordering here, but try to maintain the same ordering as in the old configuration.
        if ConfigFlags.Detector.SimulateBpipe:
            if ConfigFlags.Sim.BeamPipeSimMode != "Normal":
                regionCreatorList += [BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags)]
            #if simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
            if False:
                regionCreatorList += [FWDBeamLinePhysicsRegionToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateITk:
        if ConfigFlags.Detector.SimulateITkPixel:
            regionCreatorList += [ITkPixelPhysicsRegionToolCfg(ConfigFlags)] #TODO: add dedicated config
        if ConfigFlags.Detector.SimulateITkStrip:
            regionCreatorList += [ITkStripPhysicsRegionToolCfg(ConfigFlags)] #TODO: And here...
        # FIXME dislike the ordering here, but try to maintain the same ordering as in the old configuration.
        if ConfigFlags.Detector.SimulateBpipe:
            if ConfigFlags.Sim.BeamPipeSimMode != "Normal":
                regionCreatorList += [BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags)]
            #if simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
            if False:
                regionCreatorList += [FWDBeamLinePhysicsRegionToolCfg(ConfigFlags)]
    if ConfigFlags.Detector.SimulateCalo:
        if ConfigFlags.Detector.GeometryLAr:
            # Shower parameterization overrides the calibration hit flag
            if ConfigFlags.Sim.LArParameterization > 0 \
               and ConfigFlags.Sim.CalibrationRun in ['LAr','LAr+Tile','DeadLAr']:
                Logging.log.info('You requested both calibration hits and frozen showers / parameterization in the LAr.')
                Logging.log.info('  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.')
                Logging.log.info('  Please try again with a different value of either ConfigFlags.Sim.LArParameterization (' + str(ConfigFlags.Sim.LArParameterization) + ') or ConfigFlags.Sim.CalibrationRun ('+str(ConfigFlags.Sim.CalibrationRun)+')')
                raise RuntimeError('Configuration not allowed')
            regionCreatorList += [EMBPhysicsRegionToolCfg(ConfigFlags),
                                  EMECPhysicsRegionToolCfg(ConfigFlags),
                                  HECPhysicsRegionToolCfg(ConfigFlags),
                                  FCALPhysicsRegionToolCfg(ConfigFlags)]
            if ConfigFlags.Sim.LArParameterization > 0:
                # FIXME 'EMBPhysicsRegionTool' used for parametrization also - do we need a second instance??
                regionCreatorList += [EMECParaPhysicsRegionToolCfg(ConfigFlags),
                                      FCALParaPhysicsRegionToolCfg(ConfigFlags),
                                      FCAL2ParaPhysicsRegionToolCfg(ConfigFlags)]
                if ConfigFlags.Sim.LArParameterization > 1:
                    pass
                    #todo - add the line below
                    regionCreatorList += [PreSampLArPhysicsRegionToolCfg(ConfigFlags), DeadMaterialPhysicsRegionToolCfg(ConfigFlags)]
    ## FIXME _initPR never called for FwdRegion??
    #if simFlags.ForwardDetectors.statusOn:
    #    if DetFlags.geometry.FwdRegion_on():
    #        regionCreatorList += ['FwdRegionPhysicsRegionTool']
    if ConfigFlags.Detector.GeometryMuon:
        #todo - add the line below
        regionCreatorList += [DriftWallPhysicsRegionToolCfg(ConfigFlags), DriftWall1PhysicsRegionToolCfg(ConfigFlags), DriftWall2PhysicsRegionToolCfg(ConfigFlags)]
        #if ConfigFlags.Sim.CavernBG != 'Read' and not (simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
            #pass
            #todo - add the line below
            #regionCreatorList += [MuonSystemFastPhysicsRegionTool(ConfigFlags)]
    return regionCreatorList


def getTB_RegionCreatorList(ConfigFlags):
    regionCreatorList = []
    #from G4AtlasApps.SimFlags import simFlags

    #TODO - migrate below>>
    #if (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2003"):
    #    if (ConfigFlags.Detector.SimulateLAr):
    #        regionCreatorList += [FCALPhysicsRegionTool(ConfigFlags)]
    #elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2002"):
    #    if (ConfigFlags.Detector.SimulateLAr):
    #        regionCreatorList += [HECPhysicsRegionTool(ConfigFlags)]
    #elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6EC_2002"):
    #    if (ConfigFlags.Detector.SimulateLAr):
    #        regionCreatorList += [EMECPhysicsRegionTool(ConfigFlags)]
    #elif (ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2004"):
    #    if (simFlags.LArTB_H6Hec.get_Value()):
    #        regionCreatorList += [HECPhysicsRegionTool(ConfigFlags)]
    #    if (simFlags.LArTB_H6Emec.get_Value()):
    #        regionCreatorList += [EMECPhysicsRegionTool(ConfigFlags)]
    #    if (simFlags.LArTB_H6Fcal.get_Value()):
    #        regionCreatorList += [FCALPhysicsRegionTool(ConfigFlags)]
    #<<migrate above
    return regionCreatorList


#########################################################################
def ATLAS_FieldMgrListCfg(ConfigFlags):
    result = ComponentAccumulator()
    fieldMgrList = []
    #from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.TightMuonStepping.statusOn or\
    #   not simFlags.TightMuonStepping():
    if False:
        acc   = ATLASFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]
    else:
        acc   = TightMuonsATLASFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.SimulateBpipe:
        acc = BeamPipeFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]
    if ConfigFlags.Detector.SimulateID:
        acc = InDetFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]
    #if ConfigFlags.Detector.SimulateCalo and simFlags.MuonFieldOnlyInCalo.statusOn and simFlags.MuonFieldOnlyInCalo():
    if False:
        acc = MuonsOnlyInCaloFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]
    if ConfigFlags.Detector.SimulateMuon:
        acc = MuonFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    #sort these forward ones later
    if ConfigFlags.Detector.SimulateForward: #needed?
        if ConfigFlags.Detector.GeometryFwdRegion: #or forward?
          accQ1FwdRegionFieldManager = Q1FwdFieldManagerToolCfg(ConfigFlags)
          accQ2FwdRegionFieldManager = Q2FwdFieldManagerToolCfg(ConfigFlags)
          accQ3FwdRegionFieldManager = Q3FwdFieldManagerToolCfg(ConfigFlags)
          accD1FwdRegionFieldManager = D1FwdFieldManagerToolCfg(ConfigFlags)
          accD2FwdRegionFieldManager = D2FwdFieldManagerToolCfg(ConfigFlags)
          accQ4FwdRegionFieldManager = Q4FwdFieldManagerToolCfg(ConfigFlags)
          accQ5FwdRegionFieldManager = Q5FwdFieldManagerToolCfg(ConfigFlags)
          accQ6FwdRegionFieldManager = Q6FwdFieldManagerToolCfg(ConfigFlags)
          accQ7FwdRegionFieldManager = Q7FwdFieldManagerToolCfg(ConfigFlags)
          accQ1HKickFwdRegionFieldManager = Q1HKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ1VKickFwdRegionFieldManager = Q1VKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ2HKickFwdRegionFieldManager = Q2HKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ2VKickFwdRegionFieldManager = Q2VKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ3HKickFwdRegionFieldManager = Q3HKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ3VKickFwdRegionFieldManager = Q3VKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ4VKickAFwdRegionFieldManager = Q4VKickAFwdFieldManagerToolCfg(ConfigFlags)
          accQ4HKickFwdRegionFieldManager = Q4HKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ4VKickBFwdRegionFieldManager = Q4VKickBFwdFieldManagerToolCfg(ConfigFlags)
          accQ5HKickFwdRegionFieldManager = Q5HKickFwdFieldManagerToolCfg(ConfigFlags)
          accQ6VKickFwdRegionFieldManager = Q6VKickFwdFieldManagerToolCfg(ConfigFlags)
          accFwdRegionFieldManager = FwdRegionFieldManagerToolCfg(ConfigFlags)

          toolQ1FwdRegionFieldManager = result.popToolsAndMerge(accQ1FwdRegionFieldManager)
          toolQ2FwdFieldManager = result.popToolsAndMerge(accQ2FwdRegionFieldManager)
          toolQ3FwdFieldManager = result.popToolsAndMerge(accQ3FwdRegionFieldManager)
          toolD1FwdFieldManager = result.popToolsAndMerge(accD1FwdRegionFieldManager)
          toolD2FwdFieldManager = result.popToolsAndMerge(accD2FwdRegionFieldManager)
          toolQ4FwdFieldManager = result.popToolsAndMerge(accQ4FwdRegionFieldManager)
          toolQ5FwdFieldManager = result.popToolsAndMerge(accQ5FwdRegionFieldManager)
          toolQ6FwdFieldManager = result.popToolsAndMerge(accQ6FwdRegionFieldManager)
          toolQ7FwdFieldManager = result.popToolsAndMerge(accQ7FwdRegionFieldManager)
          toolQ1HKickFwdFieldManager = result.popToolsAndMerge(accQ1HKickFwdRegionFieldManager)
          toolQ1VKickFwdFieldManager = result.popToolsAndMerge(accQ1VKickFwdRegionFieldManager)
          toolQ2HKickFwdFieldManager = result.popToolsAndMerge(accQ2HKickFwdRegionFieldManager)
          toolQ2VKickFwdFieldManager = result.popToolsAndMerge(accQ2VKickFwdRegionFieldManager)
          toolQ3HKickFwdFieldManager = result.popToolsAndMerge(accQ3HKickFwdRegionFieldManager)
          toolQ3VKickFwdFieldManager = result.popToolsAndMerge(accQ3VKickFwdRegionFieldManager)
          toolQ4VKickAFwdFieldManager = result.popToolsAndMerge(accQ4VKickAFwdRegionFieldManager)
          toolQ4HKickFwdFieldManager = result.popToolsAndMerge(accQ4HKickFwdRegionFieldManager)
          toolQ4VKickBFwdFieldManager = result.popToolsAndMerge(accQ4VKickBFwdRegionFieldManager)
          toolQ5HKickFwdFieldManager = result.popToolsAndMerge(accQ5HKickFwdRegionFieldManager)
          toolQ6VKickFwdFieldManager = result.popToolsAndMerge(accQ6VKickFwdRegionFieldManager)
          toolFwdRegionFieldManager = result.popToolsAndMerge(accFwdRegionFieldManager)

          fieldMgrList+=[toolQ1FwdRegionFieldManager,
                         toolQ2FwdFieldManager,
                         toolQ3FwdFieldManager,
                         toolD1FwdFieldManager,
                         toolD2FwdFieldManager,
                         toolQ4FwdFieldManager,
                         toolQ5FwdFieldManager,
                         toolQ6FwdFieldManager,
                         toolQ7FwdFieldManager,
                         toolQ1HKickFwdFieldManager,
                         toolQ1VKickFwdFieldManager,
                         toolQ2HKickFwdFieldManager,
                         toolQ2VKickFwdFieldManager,
                         toolQ3HKickFwdFieldManager,
                         toolQ3VKickFwdFieldManager,
                         toolQ4VKickAFwdFieldManager,
                         toolQ4HKickFwdFieldManager,
                         toolQ4VKickBFwdFieldManager,
                         toolQ5HKickFwdFieldManager,
                         toolQ6VKickFwdFieldManager,
                         toolFwdRegionFieldManager]

    result.setPrivateTools(fieldMgrList)
    return result


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

    #if hasattr(simFlags,"Eta"): #FIXME ugly hack
    if False:
        kwargs.setdefault("World", 'TileTB_World') # NEED TO ADD THIS
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
            toolGeo = result.popToolsAndMerge(ATLASEnvelopeCfg(ConfigFlags))
            kwargs.setdefault("World", toolGeo)
        kwargs.setdefault("RegionCreators", getATLAS_RegionCreatorList(ConfigFlags))
        #if hasattr(simFlags, 'MagneticField') and simFlags.MagneticField.statusOn:
        if True:
            acc = ATLAS_FieldMgrListCfg(ConfigFlags)
            fieldMgrList = result.popToolsAndMerge(acc)
            kwargs.setdefault("FieldManagers", fieldMgrList)

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


def PhysicsListSvcCfg(ConfigFlags, name="PhysicsListSvc", **kwargs):
    result = ComponentAccumulator()
    G4StepLimitationTool = CompFactory.G4StepLimitationTool
    PhysOptionList = [G4StepLimitationTool("G4StepLimitationTool")]
    #PhysOptionList += ConfigFlags.Sim.PhysicsOptions # FIXME Missing functionality
    PhysDecaysList = []
    if ConfigFlags.Detector.SimulateTRT:
        TRTPhysicsTool = CompFactory.TRTPhysicsTool
        PhysOptionList +=[TRTPhysicsTool("TRTPhysicsTool")]
    if ConfigFlags.Detector.SimulateLucid or ConfigFlags.Detector.SimulateAFP:
        LucidPhysicsTool = CompFactory.LucidPhysicsTool
        PhysOptionList +=[LucidPhysicsTool("LucidPhysicsTool")]
    kwargs.setdefault("PhysOption", PhysOptionList)
    kwargs.setdefault("PhysicsDecay", PhysDecaysList)
    kwargs.setdefault("PhysicsList", ConfigFlags.Sim.PhysicsList)
    if 'PhysicsList' in kwargs:
        if kwargs['PhysicsList'].endswith('_EMV') or kwargs['PhysicsList'].endswith('_EMX'):
            raise RuntimeError( 'PhysicsList not allowed: '+kwargs['PhysicsList'] )

    kwargs.setdefault("GeneralCut", 1.)
    kwargs.setdefault("NeutronTimeCut", ConfigFlags.Sim.NeutronTimeCut)
    kwargs.setdefault("NeutronEnergyCut", ConfigFlags.Sim.NeutronEnergyCut)
    kwargs.setdefault("ApplyEMCuts", ConfigFlags.Sim.ApplyEMCuts)
    ## from AthenaCommon.SystemOfUnits import eV, TeV
    ## kwargs.setdefault("EMMaxEnergy"     , 7*TeV)
    ## kwargs.setdefault("EMMinEnergy"     , 100*eV)
    """ --- ATLASSIM-3967 ---
        these two options are replaced by SetNumberOfBinsPerDecade
        which now controls both values.
    """
    ## kwargs.setdefault("EMDEDXBinning"   , 77)
    ## kwargs.setdefault("EMLambdaBinning" , 77)
    result.addService(PhysicsListSvc(name, **kwargs))
    return result
