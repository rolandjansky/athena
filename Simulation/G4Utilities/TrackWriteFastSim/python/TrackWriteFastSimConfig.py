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
    return CfgMgr.NeutronFastSimTool(name, **kwargs)
