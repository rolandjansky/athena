# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Possible cases:
# 1) inputEVNTFile (normal)
# 2) inputEVNT_TRFile (TrackRecords from pre-simulated events,
# used with TrackRecordGenerator)
# Three common cases here:
# 2a) Cosmics simulation
# 2b) Stopped particle simulation
# 2c) Cavern background simulation
# 3) no input file (on-the-fly generation)
# Common cases
# 3a) ParticleGun
# 3b) CosmicGenerator
# 4) inputHITSFile (re-simulation)

from PyJobTransforms.TransformUtils import executeFromFragment
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def specialConfigPreInclude(ConfigFlags):
    fragment = ConfigFlags.Input.SpecialConfiguration.get("preInclude", None)
    if fragment and fragment != 'NONE':
        executeFromFragment(fragment, ConfigFlags) #FIXME assumes only one fragment?


def specialConfigPostInclude(ConfigFlags, cfg):
    fragment = ConfigFlags.Input.SpecialConfiguration.get("postInclude", None)
    if fragment and fragment != 'NONE':
        executeFromFragment(fragment, ConfigFlags, cfg) #FIXME assumes only one fragment?


def CommonSimulationCfg(ConfigFlags, log):
    # Configure main services and input reading (if required)
    if not ConfigFlags.Input.Files:
        # Cases 3a, 3b
        from AthenaConfiguration.MainServicesConfig import MainEvgenServicesCfg
        cfg = MainEvgenServicesCfg(ConfigFlags)
        # For Simulation we need to override the RunNumber to pick up
        # the right conditions. These next two lines are required for
        # this to work.
        cfg.getService("EventSelector").FirstLB = ConfigFlags.Input.LumiBlockNumber[0]
        cfg.getService("EventSelector").OverrideRunNumber = True
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        cfg.merge(EvtIdModifierSvcCfg(ConfigFlags))
        if ConfigFlags.Beam.Type is BeamType.Cosmics:
            # Case 3b: Configure the cosmic Generator
            from CosmicGenerator.CosmicGeneratorConfig import CosmicGeneratorCfg
            cfg.merge(CosmicGeneratorCfg(ConfigFlags))
        else:
            # Case 3a: Configure ParticleGun
            log.error("On-the-fly generation other than with CosmicGenerator is not supported yet!")
            pass
    else:
        # Cases 1, 2a, 2b, 2c, 4
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        cfg = MainServicesCfg(ConfigFlags)
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        cfg.merge(PoolReadCfg(ConfigFlags))
        if ConfigFlags.Sim.ReadTR or ConfigFlags.Sim.CavernBG == "Read":
            # Cases 2a, 2b, 2c
            from TrackRecordGenerator.TrackRecordGeneratorConfigNew import Input_TrackRecordGeneratorCfg
            cfg.merge(Input_TrackRecordGeneratorCfg(ConfigFlags))
        if ConfigFlags.Sim.ISF.ReSimulation:
            # Case 4
            from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
            cfg.merge(EventInfoCnvAlgCfg(ConfigFlags))
            from McEventCollectionFilter.McEventCollectionFilterConfig import TruthResetAlgCfg
            cfg.merge(TruthResetAlgCfg(ConfigFlags))
            cfg.addSequence(CompFactory.AthSequencer('SimSequence'), parentName='AthAlgSeq')
            cfg.addSequence(CompFactory.AthSequencer('CopyHitSequence'), parentName='AthAlgSeq')

    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolWriteCfg(ConfigFlags))

    if ConfigFlags.Sim.ISF.ReSimulation:
        # Case 4
        from ISF_Algorithms.ISF_AlgorithmsConfigNew import SimEventFilterCfg, InvertedSimEventFilterCfg, RenameHitCollectionsCfg
        cfg.merge(SimEventFilterCfg(ConfigFlags, sequenceName='SimSequence'))
        cfg.merge(InvertedSimEventFilterCfg(ConfigFlags, sequenceName='CopyHitSequence'))
        cfg.merge(RenameHitCollectionsCfg(ConfigFlags, sequenceName='CopyHitSequence'))
    else:
        #Cases 1, 2, 3
        # add BeamEffectsAlg
        from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
        cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    AcceptAlgNames=[]
    if ConfigFlags.Sim.ISFRun:
        # add the ISF_MainConfig
        from ISF_Config.ISF_MainConfigNew import ISF_KernelCfg
        cfg.merge(ISF_KernelCfg(ConfigFlags))
        AcceptAlgNames = ['ISF_Kernel_' + ConfigFlags.Sim.ISF.Simulator]
        if ConfigFlags.Sim.ISF.ReSimulation:
            AcceptAlgNames += ['RenameHitCollections']
    else:
        AcceptAlgNames = ['G4AtlasAlg']
        #add the G4AtlasAlg
        from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
        cfg.merge(G4AtlasAlgCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from SimuJobTransforms.SimOutputConfig import getStreamHITS_ItemList
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )
    cfg.getEventAlgo("OutputStreamHITS").AcceptAlgs=AcceptAlgNames
    if ConfigFlags.Sim.ISF.ReSimulation:
        cfg.getEventAlgo("OutputStreamHITS").TakeItemsFromInput=False

    if len(ConfigFlags.Output.EVNT_TRFileName)>0:
        from SimuJobTransforms.SimOutputConfig import getStreamEVNT_TR_ItemList
        cfg.merge( OutputStreamCfg(ConfigFlags,"EVNT_TR", ItemList=getStreamEVNT_TR_ItemList(ConfigFlags), disableEventTag=True) )
        cfg.getEventAlgo("OutputStreamEVNT_TR").AcceptAlgs=AcceptAlgNames

    # Add MT-safe PerfMon
    if ConfigFlags.PerfMon.doFastMonMT or ConfigFlags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        cfg.merge(PerfMonMTSvcCfg(ConfigFlags))

    return cfg
