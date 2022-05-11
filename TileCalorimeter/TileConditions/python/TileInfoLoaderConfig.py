# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile info loader service"""


from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AccumulatorCache import AccumulatorCache

@AccumulatorCache
def TileInfoLoaderCfg(flags, **kwargs):
    """Return component accumulator with configured Tile info loader service

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileInfoLoaderCfg')

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc = ComponentAccumulator()
    acc.merge (TileCablingSvcCfg(flags))

    if 'NoiseScaleIndex' not in kwargs:
        if flags.Tile.doOpt2 and not flags.Tile.doOptATLAS:
            msg.info("Adjusting TileInfo to return cell noise for Opt.Filter with iterations")
            kwargs['NoiseScaleIndex'] = 2 # Noise for Optimal Filter with iterations
        else:
            msg.info("Adjusting TileInfo to return cell noise for Opt.Filter without iterations")
            kwargs['NoiseScaleIndex'] = 1 # Noise for Optimal Filter without iterations

        kwargs.setdefault('TileNoise', flags.Digitization.DoCaloNoise)
        if kwargs['TileNoise']:
            msg.info("Switching ON noise in Tile Digitization" )
        else:
            msg.info("Switching OFF noise in Tile Digitization" )


    TileInfoLoader=CompFactory.TileInfoLoader
    acc.addService(TileInfoLoader(**kwargs), primary = True, create=True)

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

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    acc.merge( TileInfoLoaderCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('TileInfoLoader'))
    acc.store( open('TileInfoLoader.pkl','wb') )

    print('All OK')
