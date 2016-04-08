# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getTrackFastSimSD(name='TrackFastSimSD', **kwargs):
    kwargs.setdefault ('NoVolumes', True)
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read':
        if 'Write' in simFlags.CavernBG.get_Value():
            kwargs.setdefault ('OutputCollectionNames', ['NeutronBG'])
    elif simFlags.StoppedParticleFile.statusOn:
        kwargs.setdefault ('OutputCollectionNames', ['StoppingPositions'])
    return CfgMgr.TrackFastSimSDTool(name, **kwargs)

def getNeutronFastSim(name='NeutronFastSim', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read':
        kwargs.setdefault('RegionNames', ['MuonSystemFastRegion'])
    # Cannot actually get these from the job options in a normal fashion;
    #  would need to know what they are for a "normal" job and configure
    #  to match that.  Not obvious how to do that!
    kwargs.setdefault('PrimaryEtaCut',6.0)
    kwargs.setdefault('NeutronTimeCut',150.)
    return CfgMgr.NeutronFastSimTool(name, **kwargs)

def getCosmicTRSD(name="CosmicRecord", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['TTR_BARREL::TTR_BARREL'])
    kwargs.setdefault("SD_type", 2)
    kwargs.setdefault ('OutputCollectionNames', ['CosmicRecord'])
    return CfgMgr.TrackRecorderSDTool(name, **kwargs)

def getMuonEntryRecordSD(name="MuonEntryRecord", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['MSEL'])
    kwargs.setdefault("SD_type", 1)
    kwargs.setdefault ('OutputCollectionNames', ['MuonEntryRecord'])
    return getTrackRecorderSD(name, **kwargs)

def getTrackRecorderSD(name="TrackRecorderSD", **kwargs):
    kwargs.setdefault("SD_type", 1)
    kwargs.setdefault ('NoVolumes', True)
    return CfgMgr.TrackRecorderSDTool(name, **kwargs)

def getCaloEntryLayerSD(name="CaloEntryLayer", **kwargs):
    #kwargs.setdefault("LogicalVolumeNames", ['IDET::IDET'])
    kwargs.setdefault ('OutputCollectionNames', ['CaloEntryLayer'])
    return getTrackRecorderSD(name, **kwargs)

def getMuonEntryLayerSD(name="MuonEntryLayer", **kwargs):
    #kwargs.setdefault("LogicalVolumeNames", ['CALO::CALO'])
    kwargs.setdefault ('OutputCollectionNames', ['MuonEntryLayer'])
    return getTrackRecorderSD(name, **kwargs)

def getMuonExitLayerSD(name="MuonExitLayer", **kwargs):
    #kwargs.setdefault("LogicalVolumeNames", ['MUONQ02::MUONQ02'])
    kwargs.setdefault ('OutputCollectionNames', ['MuonExitLayer'])
    return getTrackRecorderSD(name, **kwargs)

def getLArFarUpstreamMaterialExitLayerSD(name="LArFarUpstreamMaterialExitLayer", **kwargs):
    #kwargs.setdefault("LogicalVolumeNames", ['LARFARUPSTREAMMATERIAL::LARFARUPSTREAMMATERIAL'])
    kwargs.setdefault ('OutputCollectionNames', ['LArFarUpstreamMaterialExitLayer'])
    return getTrackRecorderSD(name, **kwargs)
