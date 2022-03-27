# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile sampling fraction conditions algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileSamplingFractionCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile sampling fraction conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile SamplingFraction conditions (COOL, FILE). Defaults to COOL.
        TileSamplingFraction -- name of Tile sampling fraction conditions object. Defaults to TileSamplingFraction.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    samplingFraction = kwargs.get('TileSamplingFraction', 'TileSamplingFraction')

    name = samplingFraction + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        samplingFractionFolder = '/TILE/OFL02/CALIB/SFR'
        
        TileCondProxyCoolFlt = CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        samplingFractionProxy = TileCondProxyCoolFlt('TileCondProxyCool_SamplingFraction', Source = samplingFractionFolder)

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge( addFolders(flags, samplingFractionFolder + f'<key>{samplingFractionFolder}</key>',
                              detDb='TILE_OFL', className='CondAttrListCollection', tag='TileOfl02CalibSfr-SIM-02', db='OFLP200') )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt = CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        samplingFractionProxy = TileCondProxyFileFlt('TileCondProxyFile_SamplingFraction', Source = 'TileDefault.sfr')
    else:
        raise(Exception("Invalid source: %s" % source))

    TileSamplingFractionCondAlg = CompFactory.getComp("TileCondAlg<TileSamplingFraction,TileCalibDrawerFlt>")
    samplingFractionCondAlg = TileSamplingFractionCondAlg( name = name,
                                                           ConditionsProxy = samplingFractionProxy,
                                                           TileCondData = samplingFraction)

    acc.addCondAlgo(samplingFractionCondAlg)

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

    ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    acc.merge( TileSamplingFractionCondAlgCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileSamplingFraction.pkl','wb') )

    print('All OK')

