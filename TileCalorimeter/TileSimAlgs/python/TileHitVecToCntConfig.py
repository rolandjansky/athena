# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured private Tile hit vector to container tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

def getTileFirstXing():
    """Return the earliest bunch crossing time for which interactions will be sent to the TileHitVecToCntTool"""
    return -200


def getTileLastXing():
    """Return the latest bunch crossing time for which interactions will be sent to the TileHitVecToCntTool"""
    return 150


def getTileRange(name = 'TileRange', **kwargs):
    """Return a PileUpXingFolder tool for Tile"""
    kwargs.setdefault('FirstXing', getTileFirstXing() )
    kwargs.setdefault('LastXing',  getTileLastXing() )
    kwargs.setdefault('ItemList', ['TileHitVector#TileHitVec', 'TileHitVector#MBTSHits'] )

    PileUpXingFolder=CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def TileHitVecToCntToolCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile hit vector to container tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitVecToCntTool')
    kwargs.setdefault('RndmEvtOverlay', flags.Detector.OverlayTile)
    kwargs.setdefault('OnlyUseContainerName', not flags.Detector.OverlayTile)

    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    kwargs.setdefault('TileHitVectors', ['TileHitVec' , 'MBTSHits'])
    kwargs.setdefault('TileHitContainer', 'TileHitCnt')

    kwargs.setdefault('DoHSTruthReconstruction', flags.Digitization.DoDigiTruth)
    if kwargs['DoHSTruthReconstruction']:
        kwargs.setdefault('TileHitContainer_DigiHSTruth', 'TileHitCnt_DigiHSTruth')
    else:
        kwargs.setdefault('TileHitContainer_DigiHSTruth', '')

    if 'RndmSvc' not in kwargs:
        from RngComps.RandomServices import RNG
        acc.merge( RNG(flags.Random.Engine) )
        kwargs['RndmSvc'] = acc.getService('AthRNGSvc')

    if kwargs['RndmEvtOverlay']:
        kwargs.setdefault('PileUp', False)
    else:
        kwargs.setdefault('PileUp', flags.Digitization.Pileup)

    if kwargs['PileUp']:
        PileUpMergeSvc=CompFactory.PileUpMergeSvc
        acc.addService( PileUpMergeSvc() )

    if flags.Beam.Type == 'cosmics':
        CosmicTriggerTimeTool=CompFactory.CosmicTriggerTimeTool
        kwargs.setdefault('TriggerTimeTool', CosmicTriggerTimeTool())
        kwargs.setdefault('HitTimeFlag', 2)
        kwargs.setdefault('UseTriggerTime', True)

    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", getTileFirstXing() )
        kwargs.setdefault("LastXing",  getTileLastXing() )

    TileHitVecToCntTool=CompFactory.TileHitVecToCntTool
    acc.setPrivateTools(TileHitVecToCntTool(**kwargs))

    return acc


def TileHitVecToCntCfg(flags, **kwargs):
    """Return component accumulator with configured Tile hit vector to container algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitVecToCnt')
    acc = ComponentAccumulator()

    if 'DigitizationTool' not in kwargs:
        tool = acc.popToolsAndMerge( TileHitVecToCntToolCfg(flags) )
        kwargs.setdefault('DigitizationTool', tool)

    # choose which alg to attach to, following PileUpToolsCfg
    if flags.Digitization.DoXingByXingPileUp or flags.Detector.OverlayTile:
        Alg = CompFactory.TileHitVecToCnt
    else:
        Alg = CompFactory.DigitizationAlg
        kwargs["name"] = "StandardPileUpToolsAlg"
        kwargs["PileUpTools"] = [kwargs.pop("DigitizationTool")]

    acc.addEventAlgo(Alg(**kwargs))

    return acc


def TileHitOutputCfg(flags, **kwargs):
    """Return component accumulator with Output Stream configuration for Tile hits

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    if flags.Output.doWriteRDO:
        acc = OutputStreamCfg(flags, 'RDO', ['TileHitContainer#*'])
    else:
        acc = ComponentAccumulator()

    return acc


def TileHitVecToCntOutputCfg(flags, **kwargs):
    """Return component accumulator with configured Tile hit vector to container algorithm and Output Stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """
    
    acc = TileHitVecToCntCfg(flags, **kwargs)
    acc.merge(TileHitOutputCfg(flags))

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
    ConfigFlags.Output.RDOFileName = 'myRDO.pool.root'
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.Pileup = False

    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Construct our accumulator to run
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    if 'EventInfo' not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        acc.merge(EventInfoCnvAlgCfg(ConfigFlags,
                                     inputKey='McEventInfo',
                                     outputKey='EventInfo'))

    acc.merge(TileHitVecToCntOutputCfg(ConfigFlags))

    acc.getService('StoreGateSvc').Dump = True
    acc.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()
    acc.store( open('TileHitVecToCnt.pkl','wb') )

    sc = acc.run(maxEvents=3)
    # Success should be 0
    import sys
    sys.exit(not sc.isSuccess())
