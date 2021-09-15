# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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

def CommonSimulationCfg(ConfigFlags, log):
    # Configure main services and input reading (if required)
    if ConfigFlags.Input.Files == '':
        # Cases 3a, 3b
        from AthenaConfiguration.MainServicesConfig import MainEvgenServicesCfg
        cfg = MainEvgenServicesCfg(ConfigFlags)
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        cfg.merge(EvtIdModifierSvcCfg(ConfigFlags))
        if ConfigFlags.Beam.Type == 'cosmics':
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
        if ConfigFlags.Sim.ReadTR:
            # Cases 2a, 2b, 2c
            from TrackRecordGenerator.TrackRecordGeneratorConfigNew import Input_TrackRecordGeneratorCfg
            cfg.merge(Input_TrackRecordGeneratorCfg(ConfigFlags))

    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    AcceptAlgName=''
    if ConfigFlags.Sim.ISFRun:
    # add the ISF_MainConfig
        from ISF_Config.ISF_MainConfigNew import ISF_KernelCfg
        cfg.merge(ISF_KernelCfg(ConfigFlags))
        if 'MT' in  ConfigFlags.Sim.ISF.Simulator:
            AcceptAlgName = 'SimKernelMT'
        else:
            AcceptAlgName = 'SimKernel'
    else:
        AcceptAlgName = 'G4AtlasAlg'
        #add the G4AtlasAlg
        from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
        cfg.merge(G4AtlasAlgCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from SimuJobTransforms.SimOutputConfig import getStreamHITS_ItemList
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )
    cfg.getEventAlgo("OutputStreamHITS").AcceptAlgs=[AcceptAlgName]

    if len(ConfigFlags.Output.EVNT_TRFileName)>0:
        from SimuJobTransforms.SimOutputConfig import getStreamEVNT_TR_ItemList
        cfg.merge( OutputStreamCfg(ConfigFlags,"EVNT_TR", ItemList=getStreamEVNT_TR_ItemList(ConfigFlags), disableEventTag=True) )
        cfg.getEventAlgo("OutputStreamEVNT_TR").AcceptAlgs=[AcceptAlgName]

    return cfg
