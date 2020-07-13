"""Combined Tile Digitization functions

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TileTriggerDigitizationCfg(flags):
    """Return ComponentAccumulator with standard Tile Trigger Digitization configuration"""

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


def TileOverlayTriggerDigitizationCfg(flags):
    """Return ComponentAccumulator with Overlay Tile Trigger Digitization configuration"""
    acc = ComponentAccumulator()

    from TileSimAlgs.TileMuonReceiverConfig import TilePulseForTileMuonReceiverOutputCfg
    acc.merge( TilePulseForTileMuonReceiverOutputCfg(flags) )

    from TileSimAlgs.TileMuonReceiverDecisionConfig import TileMuonReceiverDecisionOutputCfg
    acc.merge( TileMuonReceiverDecisionOutputCfg(flags) )

    from TileL2Algs.TileL2Config import TileRawChannelToL2OutputCfg
    acc.merge( TileRawChannelToL2OutputCfg(flags, streamName = 'RDO') )

    return acc


def TileDigitizationCfg(flags):
    """Return ComponentAccumulator with standard Tile Digitization configuration"""

    from TileSimAlgs.TileDigitsMakerConfig import TileDigitsMakerOutputCfg
    acc = TileDigitsMakerOutputCfg(flags)

    if not flags.Digitization.PileUpPremixing and flags.Output.doWriteRDO:
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerOutputCfg
        acc.merge( TileRawChannelMakerOutputCfg(flags, streamName = 'RDO') )
    else:
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
        acc.merge( TileRawChannelMakerCfg(flags) )

    if flags.Digitization.DoDigiTruth:
        if flags.Output.doWriteRDO:
            from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerDigiHSTruthOutputCfg
            acc.merge( TileRawChannelMakerDigiHSTruthOutputCfg(flags, streamName = 'RDO') )
        else:
            from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerDigiHSTruthCfg
            acc.merge( TileRawChannelMakerDigiHSTruthCfg(flags) )

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
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

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
