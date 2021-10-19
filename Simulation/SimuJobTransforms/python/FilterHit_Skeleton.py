# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude

def getStreamHITS_ItemList(ConfigFlags):
    #--------------------------------------------------------------
    # Specify collections for output HIT files, as not all are required.
    #--------------------------------------------------------------
    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent", # mc truth (hepmc)
                "TrackRecordCollection#MuonEntryLayer", # others not used in pileup
                "xAOD::JetContainer#*",
                "xAOD::JetAuxContainer#*",
                "xAOD::TruthParticleContainer#TruthPileupParticles",
                "xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux."]

    #PLR
    if ConfigFlags.Detector.EnablePLR:
        ItemList += ["SiHitCollection#PLRHits"]
    #BCM
    if ConfigFlags.Detector.EnableBCM:
        ItemList += ["SiHitCollection#BCMHits"]
    #Pixels
    if ConfigFlags.Detector.EnablePixel:
        ItemList += ["SiHitCollection#PixelHits"]
    #SCT
    if ConfigFlags.Detector.EnableSCT:
        ItemList += ["SiHitCollection#SCT_Hits"]
    #TRT
    if ConfigFlags.Detector.EnableTRT:
        ItemList += ["TRTUncompressedHitCollection#TRTUncompressedHits"]
    #ITk Pixels
    if ConfigFlags.Detector.EnableITkPixel:
        ItemList += ["SiHitCollection#ITkPixelHits"]
    #ITk Strip
    if ConfigFlags.Detector.EnableITkStrip:
        ItemList += ["SiHitCollection#ITkStripHits"]
    #LAr
    if ConfigFlags.Detector.EnableLAr:
        ItemList += ["LArHitContainer#LArHitEMB"]
        ItemList += ["LArHitContainer#LArHitEMEC"]
        ItemList += ["LArHitContainer#LArHitHEC"]
        ItemList += ["LArHitContainer#LArHitFCAL"]
    #Tile
    if ConfigFlags.Detector.EnableTile:
        ItemList += ["TileHitVector#TileHitVec"]
    # MBTS
    if ConfigFlags.Detector.EnableMBTS:
        ItemList += ["TileHitVector#MBTSHits"]
    # HGTD
    if ConfigFlags.Detector.EnableHGTD:
        ItemList += ["SiHitCollection#HGTD_Hits"]
    #CSC
    if ConfigFlags.Detector.EnableCSC:
        ItemList+=["CSCSimHitCollection#CSC_Hits"]
    #MDT
    if ConfigFlags.Detector.EnableMDT:
        ItemList+=["MDTSimHitCollection#MDT_Hits"]
    #RPC
    if ConfigFlags.Detector.EnableRPC:
        ItemList+=["RPCSimHitCollection#RPC_Hits"]
    #TGC
    if ConfigFlags.Detector.EnableTGC:
        ItemList+=["TGCSimHitCollection#TGC_Hits"]
    #STGC
    if ConfigFlags.Detector.EnablesTGC:
        ItemList+=["sTGCSimHitCollection#sTGCSensitiveDetector"]
    #MM
    if ConfigFlags.Detector.EnableMM:
        ItemList+=["MMSimHitCollection#MicromegasSensitiveDetector"]
    return ItemList


def defaultFilterHitFlags(ConfigFlags, detectors):
    """Fill default flags for hit filtering"""
    # Nothing special other than setup of detector flags
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('FilterHit_tf')
    log.info('****************** STARTING HIT FILTERING *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'inputHITSFile'):
        ConfigFlags.Input.Files = runArgs.inputHITSFile
    else:
        raise RuntimeError('No input HITS file defined')

    # Generate detector list and setup detector flags
    from SimuJobTransforms.SimulationHelpers import getDetectorsFromRunArgs
    detectors = getDetectorsFromRunArgs(ConfigFlags, runArgs)
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

    ## from SimuJobTransforms.HitsFilePeeker import HitsFilePeeker
    ## HitsFilePeeker(runArgs, filterHitLog)

    if hasattr(runArgs, 'outputHITS_FILTFile'):
        if runArgs.outputHITS_FILTFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
            # TODO decide if we need a specific HITS_FILTFileName flag
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITS_FILTFile
    else:
        raise RuntimeError('No outputHITS_FILTFile defined')

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    # force TreeAutoFlush=1 as events will be accessed randomly
    cfg.merge(PoolWriteCfg(ConfigFlags, forceTreeAutoFlush=1))

    # add LArHitFilter + AddressRemappingSvc
    if ConfigFlags.Detector.EnableLAr:
        from LArDigitization.LArDigitizationConfigNew import LArHitFilterCfg
        cfg.merge(LArHitFilterCfg(ConfigFlags)) # TODO add output configuration here?

    if hasattr(runArgs, 'TruthReductionScheme'):
        if runArgs.TruthReductionScheme != 'SingleGenParticle':
            log.warning(f'Unknown TruthReductionScheme ({runArgs.TruthReductionScheme}). Currently just a dummy value, but please check.')

        # add McEventCollectionFtiler + AddressRemappingSvc
        from McEventCollectionFilter.McEventCollectionFilterConfig import McEventCollectionFilterCfg
        cfg.merge(McEventCollectionFilterCfg(ConfigFlags))
        # Check for Truth Containers
        for entry in ConfigFlags.Input.Collections:
            if 'AntiKt4TruthJets' == entry:
                from McEventCollectionFilter.McEventCollectionFilterConfig import DecoratePileupAntiKt4TruthJetsCfg
                cfg.merge(DecoratePileupAntiKt4TruthJetsCfg(ConfigFlags))
            if 'AntiKt6TruthJets' == entry:
                from McEventCollectionFilter.McEventCollectionFilterConfig import DecoratePileupAntiKt6TruthJetsCfg
                cfg.merge(DecoratePileupAntiKt6TruthJetsCfg(ConfigFlags))
            if 'TruthPileupParticles' == entry:
                from McEventCollectionFilter.McEventCollectionFilterConfig import DecorateTruthPileupParticlesCfg
                cfg.merge(DecorateTruthPileupParticlesCfg(ConfigFlags))

        # ID
        if ConfigFlags.Detector.EnableBCM:
            from McEventCollectionFilter.McEventCollectionFilterConfig import BCM_HitsTruthRelinkCfg
            cfg.merge(BCM_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnablePixel:
            from McEventCollectionFilter.McEventCollectionFilterConfig import PixelHitsTruthRelinkCfg
            cfg.merge(PixelHitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableSCT:
            from McEventCollectionFilter.McEventCollectionFilterConfig import SCT_HitsTruthRelinkCfg
            cfg.merge(SCT_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableTRT:
            from McEventCollectionFilter.McEventCollectionFilterConfig import TRT_HitsTruthRelinkCfg
            cfg.merge(TRT_HitsTruthRelinkCfg(ConfigFlags))
        # ITk
        if ConfigFlags.Detector.EnableITkPixel:
            from McEventCollectionFilter.McEventCollectionFilterConfig import ITkPixelHitsTruthRelinkCfg
            cfg.merge(ITkPixelHitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableITkPixel:
            from McEventCollectionFilter.McEventCollectionFilterConfig import ITkPixelHitsTruthRelinkCfg
            cfg.merge(ITkPixelHitsTruthRelinkCfg(ConfigFlags))
        # HGTD
        if ConfigFlags.Detector.EnableHGTD:
            from McEventCollectionFilter.McEventCollectionFilterConfig import HGTD_HitsTruthRelinkCfg
            cfg.merge(HGTD_HitsTruthRelinkCfg(ConfigFlags))
        # Muons
        if ConfigFlags.Detector.EnableCSC:
            from McEventCollectionFilter.McEventCollectionFilterConfig import CSC_HitsTruthRelinkCfg
            cfg.merge(CSC_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableMDT:
            from McEventCollectionFilter.McEventCollectionFilterConfig import MDT_HitsTruthRelinkCfg
            cfg.merge(MDT_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableMM:
            from McEventCollectionFilter.McEventCollectionFilterConfig import MM_HitsTruthRelinkCfg
            cfg.merge(MM_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableRPC:
            from McEventCollectionFilter.McEventCollectionFilterConfig import RPC_HitsTruthRelinkCfg
            cfg.merge(RPC_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableTGC:
            from McEventCollectionFilter.McEventCollectionFilterConfig import TGC_HitsTruthRelinkCfg
            cfg.merge(TGC_HitsTruthRelinkCfg(ConfigFlags))
        if ConfigFlags.Detector.EnablesTGC:
            from McEventCollectionFilter.McEventCollectionFilterConfig import sTGC_HitsTruthRelinkCfg
            cfg.merge(sTGC_HitsTruthRelinkCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run()
    log.info("Ran FilterHit_tf in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
