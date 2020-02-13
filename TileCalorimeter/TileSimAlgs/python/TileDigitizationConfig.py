"""Combined Tile Digitization functions

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

def TileTriggerDigitizationCfg(flags):
    """Return ComponentAccumulator with standard Tile Digitization configuration"""

    from TileSimAlgs.TileHitToTTL1Config import TileHitToTTL1OutputCfg
    acc = TileHitToTTL1OutputCfg(flags)

    from TileSimAlgs.TileMuonReceiverConfig import TilePulseForTileMuonReceiverOutputCfg
    acc.merge( TilePulseForTileMuonReceiverOutputCfg(flags) )

    if not flags.Digitization.PileUpPremixing:
        from TileSimAlgs.TileMuonReceiverDecisionConfig import TileMuonReceiverDecisionOutputCfg
        acc.merge( TileMuonReceiverDecisionOutputCfg(flags) )

        from TileL2Algs.TileL2Config import TileRawChannelToL2OutputCfg
        acc.merge( TileRawChannelToL2OutputCfg(flags, streamName = 'RDO') )

    return acc

def TileDigitizationCfg(flags):
    """Return ComponentAccumulator with standard Tile Digitization configuration"""

    from TileSimAlgs.TileDigitsMakerConfig import TileDigitsMakerOutputCfg
    acc = TileDigitsMakerOutputCfg(flags)

    if not flags.Digitization.PileUpPremixing:
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerOutputCfg
        acc.merge( TileRawChannelMakerOutputCfg(flags, streamName = 'RDO') )

        if flags.Digitization.DoDigiTruth:
            from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerDigiHSTruthOutputCfg
            acc.merge( TileRawChannelMakerDigiHSTruthOutputCfg(flags, streamName = 'RDO') )

    return acc



if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.Output.RDOFileName = 'myRDO.pool.root'
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.Pileup = False

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()
    ConfigFlags.dump()

    # Construct our accumulator to run
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
    acc = MainServicesThreadedCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    acc.merge( TileDigitizationCfg(ConfigFlags) )
    acc.merge( TileTriggerDigitizationCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDigitization.pkl','wb') )

    sc = acc.run(maxEvents=3)
    # Success should be 0
    import sys
    sys.exit(not sc.isSuccess())
