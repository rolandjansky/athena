# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile conditions tools for tests"""


def TileCondToolsTestCfg(flags):
    """Test Tile conditions tools configuration and return component accumulator"""
    
    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileCondToolsTestCfg')

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator    
    acc = ComponentAccumulator()

    from .TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
    msg.info(emScaleTool)

    from .TileEMScaleConfig import TileExpertToolEmscaleCfg
    emScaleExpertTool = acc.popToolsAndMerge( TileExpertToolEmscaleCfg(flags) )
    msg.info(emScaleExpertTool)

    from .TileBadChannelsConfig import TileBadChanToolCfg
    badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
    msg.info(badChanTool)

    from .TileMuIDConfig import TileCondToolMuIDCfg
    muIDTool = acc.popToolsAndMerge( TileCondToolMuIDCfg(flags) )
    msg.info(muIDTool)

    from .TileIntegratorConfig import TileCondToolIntegratorCfg
    integratorTool = acc.popToolsAndMerge( TileCondToolIntegratorCfg(flags) )
    msg.info(integratorTool)

    from .TileTimingConfig import TileCondToolTimingCfg
    timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
    msg.info(timingTool)

    from .TileTimingConfig import TileCondToolOnlineTimingCfg
    onlineTimingTool = acc.popToolsAndMerge( TileCondToolOnlineTimingCfg(flags) )
    msg.info(onlineTimingTool)

    from .TileOFCConfig import TileCondToolOfcCoolCfg
    ofcCoolTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags) )
    msg.info(ofcCoolTool)

    from .TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
    if flags.Common.isOnline:
        accSampleNoise = TileCondToolNoiseSampleCfg(flags)
    else:
        accSampleNoise = TileCondToolNoiseSampleCfg(flags, 
                                                    TileSampleNoise = 'TileSampleNoise',
                                                    TileOnlineSampleNoise = 'TileOnlineSampleNoise')

    sampleNoiseTool = acc.popToolsAndMerge( accSampleNoise )
    msg.info(sampleNoiseTool)

    if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
        from .TileTMDBConfig import TileCondToolTMDBCfg
        tmdbTool = acc.popToolsAndMerge( TileCondToolTMDBCfg(flags) )
        msg.info(tmdbTool)

        from .TileDSPThresholdConfig import TileCondToolDspThresholdCfg
        dspThrTool = acc.popToolsAndMerge( TileCondToolDspThresholdCfg(flags) )
        msg.info(dspThrTool)

    if not (flags.IOVDb.DatabaseInstance  == 'CONDBR2' and flags.Common.isOnline):
        from .TilePulseShapeConfig import TileCondToolPulseShapeCfg
        pulseShapeTool = acc.popToolsAndMerge( TileCondToolPulseShapeCfg(flags) )
        msg.info(pulseShapeTool)

        from .TilePulseShapeConfig import TileCondToolMuRcvPulseShapeCfg
        muRcvPulseShapeTool = acc.popToolsAndMerge( TileCondToolMuRcvPulseShapeCfg(flags) )
        msg.info(muRcvPulseShapeTool)

        from .TileAutoCorrelationConfig import TileCondToolAutoCrCfg
        autoCorrelationTool = acc.popToolsAndMerge( TileCondToolAutoCrCfg(flags) )
        msg.info(autoCorrelationTool)

        from .TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags) )
        msg.info(ofcTool)


    if not (flags.Input.isMC or flags.Common.isOnline):
        from TileConditions.TileDCSConfig import TileDCSToolCfg
        dcsTool = acc.popToolsAndMerge( TileDCSToolCfg(flags) )
        msg.info(dcsTool)
    
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    acc.merge( TileCondToolsTestCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileCondTools.pkl','wb') )

    print('All OK')
