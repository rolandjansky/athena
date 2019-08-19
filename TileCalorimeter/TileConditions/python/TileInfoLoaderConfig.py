# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile info loader service"""


def TileInfoLoaderCfg(flags):
    """Return component accumulator with configured Tile info loader service

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileInfoLoader')

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    from TileConditions.TileConditionsConf import TileInfoLoader
    tileInfoLoader = TileInfoLoader()

    if flags.Tile.doOpt2 and flags.Tile.doOptATLAS:
        msg.info("Adjusting TileInfo to return cell noise for Opt.Filter with iterations")
        tileInfoLoader.NoiseScaleIndex = 2 # Noise for Optimal Filter with iterations
    else:
        msg.info("Adjusting TileInfo to return cell noise for Opt.Filter without iterations")
        tileInfoLoader.NoiseScaleIndex = 1 # Noise for Optimal Filter without iterations


    acc.addService(tileInfoLoader, primary = True)

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    acc.merge( TileInfoLoaderCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('TileInfoLoader'))
    acc.store( open('TileInfoLoader.pkl','w') )

    print('All OK')
