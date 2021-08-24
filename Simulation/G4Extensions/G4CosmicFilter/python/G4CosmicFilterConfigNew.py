# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def configCosmicFilterVolumeNames(ConfigFlags):
    """returns a list with volume names. Can be merged with
    athena/Simulation/ISF/ISF_Core/ISF_Tools/python/ISF_ToolsConfigNew.py#0103
    """

    volmap = {
        "Muon": ["MuonExitLayer"],
        "Calo": ["MuonEntryLayer"],
        "InnerDetector": ["CaloEntryLayer"],
        "TRT_Barrel": ["TRTBarrelEntryLayer"],
        "TRT_EC": ["TRTECAEntryLayer","TRTECBEntryLayer"],
        "SCT_Barrel": ["SCTBarrelEntryLayer"],
        "Pixel": ["PixelEntryLayer"],
        "NONE": [],
     }
    volumeNames = []
    for vol in ConfigFlags.Sim.CosmicFilterVolumeNames:
         volumeNames += volmap[vol]

    return volumeNames

def CosmicFilterToolCfg(ConfigFlags, name="G4UA::G4CosmicFilterTool", **kwargs):
    result = ComponentAccumulator()
    volumes=configCosmicFilterVolumeNames(ConfigFlags)

    # use simple cosmic filter
    if len(volumes)==1:

        if ConfigFlags.Sim.CosmicFilterID:
            kwargs.setdefault("PDGId", ConfigFlags.Sim.CosmicFilterID)
        if ConfigFlags.Sim.CosmicFilterPTmin:
            kwargs.setdefault("PtMin", ConfigFlags.Sim.CosmicFilterPTmin)
        if ConfigFlags.Sim.CosmicFilterPTmax:
            kwargs.setdefault("PtMax", ConfigFlags.Sim.CosmicFilterPTmax)
        kwargs.setdefault("CollectionName",volumes[0])

        print ('G4CosmicFilter: Filter volume is %s' % volumes[0])

        result.setPrivateTools(CompFactory.G4UA.G4CosmicFilterTool(name, **kwargs))

    elif len(volumes)==2:
        # need a cosmic AND filter
        kwargs.setdefault("CollectionName",volumes[0])
        kwargs.setdefault("CollectionName2",volumes[1])
        result.setPrivateTools(CompFactory.G4UA.G4UA__G4CosmicAndFilterTool(name, **kwargs))

    else:
        # need a cosmic OR filter
        kwargs.setdefault("CollectionName",volumes[0])
        kwargs.setdefault("CollectionName2",volumes[1])
        kwargs.setdefault("CollectionName3",volumes[2])
        result.setPrivateTools(CompFactory.G4UA.G4UA__G4CosmicOrFilterTool(name, **kwargs))
    return result

# Note - is an ISF one migrated, but todo the G4UA one
# def getStoppedParticleFilterTool(name="G4UA::StoppedParticleFilterTool", **kwargs):
#     kwargs.setdefault("CollectionName",'StoppingPositions')
#     return  CfgMgr.G4UA__G4CosmicFilterTool(name, **kwargs)
