# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile digits filter algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileDigitsFilterCfg(flags, **kwargs):
    """Return component accumulator with configured Tile digits filter algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    acc.merge(DetDescrCnvSvcCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    TileDigitsFilter=CompFactory.TileDigitsFilter
    acc.addEventAlgo(TileDigitsFilter(**kwargs), primary = True)

    return acc



def TileDigitsFilterOutputCfg(flags, streamName = 'ESD', **kwargs):

    acc = TileDigitsFilterCfg(flags)
    tileDigitsFilter = acc.getPrimary()

    outputItemList = []

    if 'OutputDigitsContainer' in tileDigitsFilter._properties:
        digitsContainer = tileDigitsFilter._properties['OutputDigitsContainer']
    else:
        digitsContainer = tileDigitsFilter._descriptors['OutputDigitsContainer'].default

    if digitsContainer != '':
        outputItemList += ['TileDigitsContainer#' + digitsContainer]

    if 'OutputRawChannelContainer' in tileDigitsFilter._properties:
        rawChannelContainer = tileDigitsFilter._properties['OutputRawChannelContainer']
    else:
        rawChannelContainer = tileDigitsFilter._descriptors['OutputRawChannelContainer'].default

    if rawChannelContainer != '':
        outputItemList += ['TileRawChannelContainer#' + rawChannelContainer]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(  OutputStreamCfg(flags, streamName, ItemList = outputItemList) )

    return acc


if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.ESDFileName = "myESD.pool.root"
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    acc.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    acc.merge( TileDigitsFilterOutputCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDigitsFilter.pkl','wb') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

