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

    #BCM
    ItemList += ["SiHitCollection#BCMHits"]
    #Pixels
    ItemList += ["SiHitCollection#PixelHits"]
    #SCT
    ItemList += ["SiHitCollection#SCT_Hits"]
    #TRT
    ItemList += ["TRTUncompressedHitCollection#TRTUncompressedHits"]
    #LAr
    ItemList += ["LArHitContainer#LArHitEMB"]
    ItemList += ["LArHitContainer#LArHitEMEC"]
    ItemList += ["LArHitContainer#LArHitHEC"]
    ItemList += ["LArHitContainer#LArHitFCAL"]
    #Tile
    ItemList += ["TileHitVector#TileHitVec"]

    if ConfigFlags.Detector.EnableHGTD:
        ItemList += ["SiHitCollection#LArHitHGTD"]
    if ConfigFlags.Detector.EnableMBTS:
        ItemList += ["TileHitVector#MBTSHits"]
    #CSC
    if ConfigFlags.Detector.EnableCSC:
        ItemList+=["CSCSimHitCollection#CSC_Hits"]
    #MDT
    ItemList+=["MDTSimHitCollection#MDT_Hits"]
    #RPC
    ItemList+=["RPCSimHitCollection#RPC_Hits"]
    #TGC
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

    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
        detectors = getDefaultDetectors(ConfigFlags.GeoModel.AtlasVersion)

    # Support switching on Forward Detectors
    if hasattr(runArgs, 'LucidOn'):
        detectors = detectors+['Lucid']
    if hasattr(runArgs, 'ZDCOn'):
        detectors = detectors+['ZDC']
    if hasattr(runArgs, 'AFPOn'):
        detectors = detectors+['AFP']
    if hasattr(runArgs, 'ALFAOn'):
        detectors = detectors+['ALFA']
    if hasattr(runArgs, 'FwdRegionOn'):
        detectors = detectors+['FwdRegion']

    # Setup common simulation flags
    defaultFilterHitFlags(ConfigFlags, detectors)

    if hasattr(runArgs, 'inputHITSFile'):
        ConfigFlags.Input.Files = runArgs.inputHITSFile
    else:
        raise RuntimeError('No input HITS file defined')

    ## from SimuJobTransforms.HitsFilePeeker import HitsFilePeeker
    ## HitsFilePeeker(runArgs, filterHitLog)

    if hasattr(runArgs, 'outputHITS_FILTFile'):
        if runArgs.outputHITS_FILTFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
            # TODO decide if we need a specific HITS_FILTFileName flag
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITS_FILTFile
    else:
        raise RuntimeError('No outputHITSFile defined')

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
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # add LArHitFilter + AddressRemappingSvc
    from LArDigitization.LArDigitizationConfigNew import LArHitFilterCfg
    cfg.merge(LArHitFilterCfg(ConfigFlags)) # TODO add output configuration here?

    if hasattr(runArgs,'TruthReductionScheme'):
        if runArgs.TruthReductionScheme != 'SingleGenParticle':
            log.warning( 'Unknown TruthReductionScheme (' + runArgs.TruthReductionScheme + '). Currently just a dummy value, but please check.' )
        # add McEventCollectionFtiler + AddressRemappingSvc
        from McEventCollectionFilter.McEventCollectionFilterConfigNew import McEventCollectionFilterCfg
        cfg.merge(McEventCollectionFilterCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    cfg.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes

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
