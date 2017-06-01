# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getEnvelopeMap():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.BeamFlags import jobproperties as beamProps

    # Map of volume name to output collection name
    envelopeMap = dict()
    if ( hasattr(simFlags, "LArFarUpstreamMaterial") and
         simFlags.LArFarUpstreamMaterial.statusOn and
         simFlags.LArFarUpstreamMaterial.get_Value() ):
        envelopeMap.update({'LARFARUPSTREAMMATERIAL::LARFARUPSTREAMMATERIAL':
                            'LArFarUpstreamMaterialExitLayer'})
    if beamProps.Beam.beamType() == 'cosmics':
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
                envelopeMap['Pixel::Pixel'] = 'PixelEntryLayer'
    if not simFlags.ISFRun:
        if DetFlags.ID_on():
            envelopeMap['IDET::IDET'] = 'CaloEntryLayer'
        if DetFlags.Calo_on():
            envelopeMap['CALO::CALO'] = 'MuonEntryLayer'
        if DetFlags.geometry.Muon_on():
            envelopeMap['MUONQ02::MUONQ02'] = 'MuonExitLayer'
    return envelopeMap


def getMCTruthSteppingActionTool(name='G4UA::MCTruthSteppingActionTool', **kwargs):
    """Retrieve the MCTruthSteppingActionTool"""
    kwargs.setdefault("VolumeCollectionMap", getEnvelopeMap())
    return CfgMgr.G4UA__MCTruthSteppingActionTool(name, **kwargs)
