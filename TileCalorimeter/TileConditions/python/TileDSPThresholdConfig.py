# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile DSP threshold conditions tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TileDSP_ThresholdCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile DSP threshold conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile auto correlation conditions (COOL, FILE). Defaults to COOL.
        TileDSP_Threshold -- name of Tile auto correlation conditions object. Defaults to TileDSP_Threshold.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    dspThreshold = kwargs.get('TileDSP_Threshold', 'TileDSP_Threshold')

    name = dspThreshold + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
            dspThresholdFolder = folders.addSplitOnline('/TILE/ONL01/DSP/THRESHOLD', '/TILE/OFL02/DSP/THRESHOLD')
        else:
            raise(Exception("No Tile DSP threshold folder in %s" % flags.IOVDb.DatabaseInstancea))

        TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        dspThresholdProxy = TileCondProxyCoolFlt('TileCondProxyCool_DspThreshold', Source = dspThresholdFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        dspThresholdProxy  = TileCondProxyFileFlt('TileCondProxyFile_DspThreshold', Source = 'TileDefault.dspThreshold')
    else:
        raise(Exception("Invalid source: %s" % source))

    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    dspThresholdCondAlg = TileCalibFltCondAlg( name = name,
                                               ConditionsProxy = dspThresholdProxy,
                                               TileCalibData = dspThreshold)

    acc.addCondAlgo(dspThresholdCondAlg)

    return acc


def TileCondToolDspThresholdCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile DSP threshold tool
    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile DSP threshold conditions (COOL, FILE). Defaults to COOL.
        TileDSP_Threshold -- name of Tile DSP threshold conditions object. Defaults to TileDSP_Threshold.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileDSP_Threshold', 'TileDSP_Threshold')

    dspThreshold = kwargs['TileDSP_Threshold']

    acc.merge( TileDSP_ThresholdCondAlgCfg(flags, **kwargs) )

    TileCondToolDspThreshold=CompFactory.TileCondToolDspThreshold
    acc.setPrivateTools( TileCondToolDspThreshold(TileDSP_Threshold = dspThreshold) )

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    autoCorrelationTool = acc.popToolsAndMerge( TileCondToolDspThresholdCfg(ConfigFlags) )
    print(autoCorrelationTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileDSP_Threshold.pkl','wb') )

    print('All OK')

