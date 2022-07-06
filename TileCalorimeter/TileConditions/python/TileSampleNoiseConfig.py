# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile sample noise tool and conditions algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileSampleNoiseCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile sample noise conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile sample noise conditions (COOL, FILE). Defaults to COOL.
        ForceOnline -- flag to use online sample noise in offline. Defaults to False.
        TileSampleNoise -- name of Tile sample noise conditions object. 
                           Defaults to TileOnlineSampleNoise if ForceOnline else TileSampleNoise.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    sampleNoise = kwargs.get('TileSampleNoise', 'TileSampleNoise')
    forceOnline = kwargs.get('ForceOnline', False)

    name = sampleNoise + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        if flags.IOVDb.DatabaseInstance == 'CONDBR2':
            onlineSampleNoiseFolder = '/TILE/ONL01/NOISE/SAMPLE'
        else:
            onlineSampleNoiseFolder = '/TILE/OFL01/NOISE/SAMPLE'

        if forceOnline:
            sampleNoiseFolder = folders.add(onlineSampleNoiseFolder, 'TILE')
        else:
            sampleNoiseFolder = folders.addSplitOnline(onlineSampleNoiseFolder, '/TILE/OFL02/NOISE/SAMPLE')

        TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        sampleNoiseProxy = TileCondProxyCoolFlt('TileCondProxyCool_NoiseSample', Source = sampleNoiseFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        sampleNoiseProxy = TileCondProxyFileFlt('TileCondProxyFile_NoiseSample', Source = 'TileDefault.ped')

    else:
        raise(Exception("Invalid source: %s" % source))


    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    sampleNoiseCondAlg = TileCalibFltCondAlg( name = name,
                                              ConditionsProxy = sampleNoiseProxy,
                                              TileCalibData = sampleNoise)

    acc.addCondAlgo(sampleNoiseCondAlg)

    return acc



def TileCondToolNoiseSampleCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile sample noise tool
    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile sample noise conditions (COOL, FILE). Defaults to COOL.
        TileSampleNoise -- name of Tile sample noise conditions object. Defaults to TileSampleNoise.
        TileOnlineSampleNoise -- name of Tile online sample noise conditions object. If it is not empty
                                 online sample noise will be accessible in offline also and that allows
                                 to get difference between pedestal used in online and corrected one 
                                 in offline. Proposed name: TileOnlineSampleNoise. Defaults to empty.
    """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileCondToolNoiseSample')

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileSampleNoise', 'TileSampleNoise')

    sampleNoise = kwargs['TileSampleNoise']
    onlineSampleNoise = kwargs.pop('TileOnlineSampleNoise', '')

    name = 'TileCondToolNoiseSample'

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    acc.merge( TileEMScaleCondAlgCfg(flags, **kwargs) )

    acc.merge( TileSampleNoiseCondAlgCfg(flags, **kwargs) )

    if onlineSampleNoise:
        if flags.Common.isOnline:
            msg.warning('Not necessary to configure online sample noise in online especially')
            onlineSampleNoise = sampleNoise
        else:
            kwargs['TileSampleNoise'] = onlineSampleNoise
            acc.merge( TileSampleNoiseCondAlgCfg(flags, ForceOnline = True, **kwargs) )

    TileCondToolNoiseSample=CompFactory.TileCondToolNoiseSample
    acc.setPrivateTools( TileCondToolNoiseSample(name, TileSampleNoise = sampleNoise, 
                                                 TileOnlineSampleNoise = onlineSampleNoise) )

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Common.isOnline = True
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(ConfigFlags) )
    print(sampleNoiseTool)

    accOnlSampleNoise = TileCondToolNoiseSampleCfg(ConfigFlags,
                                                   TileSampleNoise = 'TileSampleNoise',
                                                   TileOnlineSampleNoise = 'TileOnlineSampleNoise')

    sampleNoiseTool = acc.popToolsAndMerge( accOnlSampleNoise )
    print(sampleNoiseTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileSampleNoise.pkl','wb') )

    print('All OK')

