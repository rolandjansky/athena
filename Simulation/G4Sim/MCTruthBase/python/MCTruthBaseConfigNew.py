# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def getEnvelopeMap(ConfigFlags):
    #from G4AtlasApps.SimFlags import simFlags

    # Map of volume name to output collection name
    envelopeMap = dict()

    #todo - migrate ctb and cosmic config too
    """if ( hasattr(simFlags, "LArFarUpstreamMaterial") and
                     simFlags.LArFarUpstreamMaterial.statusOn and
                     simFlags.LArFarUpstreamMaterial.get_Value() ):
                    envelopeMap.update({'LARFARUPSTREAMMATERIAL::LARFARUPSTREAMMATERIAL':
                                        'LArFarUpstreamMaterialExitLayer'})
                if ConfigFlags.Beam.Type == 'cosmics':
                    ## First filter volume
                    if simFlags.CosmicFilterVolumeName == "TRT_Barrel":
                        envelopeMap['TRT::BarrelOuterSupport'] = 'TRTBarrelEntryLayer'
                    elif simFlags.CosmicFilterVolumeName == "TRT_EC":
                        envelopeMap['TRT::WheelA'] = 'TRTECAEntryLayer'
                        envelopeMap['TRT::WheelB'] = 'TRTECBEntryLayer'
                    elif simFlags.CosmicFilterVolumeName == "SCT_Barrel":
                        envelopeMap['SCT::ThShieldOuterCly'] = 'SCTBarrelEntryLayer'# could be ThShieldInnerCly or Cyl..
                    elif simFlags.CosmicFilterVolumeName == "Pixel":
                        envelopeMap['Pixel::Pixel'] = 'PixelEntryLayer'
                    ## If second volume requested
                    if simFlags.CosmicFilterVolumeName2.statusOn:
                        if simFlags.CosmicFilterVolumeName2 == "TRT_Barrel":
                            envelopeMap['TRT::BarrelOuterSupport'] = 'TRTBarrelEntryLayer'
                        elif simFlags.CosmicFilterVolumeName2 == "TRT_EC":
                            envelopeMap['TRT::WheelA'] = 'TRTECAEntryLayer'
                            envelopeMap['TRT::WheelB'] = 'TRTECBEntryLayer'
                        elif simFlags.CosmicFilterVolumeName2 == "SCT_Barrel":
                            envelopeMap['SCT::ThShieldOuterCly'] = 'SCTBarrelEntryLayer'# could be ThShieldInnerCly or Cyl..
                        elif simFlags.CosmicFilterVolumeName2 == "Pixel":
                            envelopeMap['Pixel::Pixel'] = 'PixelEntryLayer'"""
    if not ConfigFlags.Sim.ISFRun:
        if ConfigFlags.Detector.SimulateID:
            envelopeMap['IDET::IDET'] = 'CaloEntryLayer'
        if ConfigFlags.Detector.SimulateCalo:
            envelopeMap['CALO::CALO'] = 'MuonEntryLayer'
        if ConfigFlags.Detector.GeometryMuon: #was geometry in old style, should it be?
            envelopeMap['MUONQ02::MUONQ02'] = 'MuonExitLayer'
    return envelopeMap


def MCTruthSteppingActionToolCfg(ConfigFlags, name='G4UA::MCTruthSteppingActionTool', **kwargs):
    """Retrieve the MCTruthSteppingActionTool"""
    result = ComponentAccumulator()
    kwargs.setdefault("VolumeCollectionMap", getEnvelopeMap(ConfigFlags))

    result.setPrivateTools( CompFactory.G4UA.MCTruthSteppingActionTool(name, **kwargs) )
    return result
