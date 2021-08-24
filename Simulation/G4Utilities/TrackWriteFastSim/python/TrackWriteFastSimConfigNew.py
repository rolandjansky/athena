# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TrackFastSimSDCfg(flags, name='TrackFastSimSD', **kwargs):
    kwargs.setdefault ('NoVolumes', True)
    if flags.Sim.CavernBG not in ['Off', 'Read']:
        if 'Write' in flags.Sim.CavernBG:
            kwargs.setdefault ('OutputCollectionNames', ['NeutronBG'])
    elif len(flags.Sim.StoppedParticleFile)>0:
        kwargs.setdefault ('OutputCollectionNames', ['StoppingPositions'])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.TrackFastSimSDTool(name, **kwargs))
    return result

def NeutronFastSimCfg(flags, name='NeutronFastSim', **kwargs):
    if flags.Sim.CavernBG.get_Value() not in ['Off', 'Read']:
        kwargs.setdefault('RegionNames', ['MuonSystemFastRegion'])
    # Cannot actually get these from the job options in a normal fashion;
    #  would need to know what they are for a "normal" job and configure
    #  to match that.  Not obvious how to do that!

    kwargs.setdefault('TrackFastSimSDName', "ToolSvc.SensitiveDetectorMasterTool.TrackFastSimSD")
    kwargs.setdefault('PrimaryEtaCut',6.0)
    kwargs.setdefault('NeutronTimeCut',150.)
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.NeutronFastSimTool(name, **kwargs))
    return result

def CosmicTRSDCfg(flags, name="CosmicRecord", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['TTR_BARREL::TTR_BARREL'])
    kwargs.setdefault("SD_type", 2)
    kwargs.setdefault ('OutputCollectionNames', ['CosmicRecord'])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.TrackRecorderSDTool(name, **kwargs))
    return result

def MuonEntryRecordSDCfg(flags, name="MuonEntryRecord", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['MSEL'])
    kwargs.setdefault("SD_type", 1)
    kwargs.setdefault ('OutputCollectionNames', ['MuonEntryRecord'])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.TrackRecorderSDTool(name, **kwargs))
    return result
