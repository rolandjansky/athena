#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMuonFitterConfig.py
@brief Python configuration of Tile Muon Fitter algorithm for the Run III
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileMuonFitterCfg(flags, **kwargs):

    acc = ComponentAccumulator()

    kwargs.setdefault('DoHoughTransform', True)
    kwargs.setdefault('EThreshold', 250.0)
    kwargs.setdefault('BeamType', flags.Beam.Type)
    kwargs.setdefault('CaloCellContainer', 'AllCalo')

    if kwargs['DoHoughTransform']:
        kwargs.setdefault('name', 'TileMuonFitter')
        kwargs.setdefault('ComTimeKey', 'ComTimeTileMuon')
        kwargs.setdefault('TileCosmicMuonKey', 'TileCosmicMuonHT')
    else:
        kwargs.setdefault('name', 'TileMuonFitterMF')
        kwargs.setdefault('ComTimeKey', 'ComTimeTileMuonMF')
        kwargs.setdefault('TileCosmicMuonKey', 'TileCosmicMuonMF')

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    TileMuonFitter=CompFactory.TileMuonFitter
    acc.addEventAlgo(TileMuonFitter(**kwargs), primary = True)

    return acc


def TileMuonFitterOutputCfg(flags, streamName = 'ESD', **kwargs):

    acc = TileMuonFitterCfg(flags, **kwargs)
    muonFitter = acc.getPrimary()

    cosmiMuonContainer = muonFitter.TileCosmicMuonKey
    cosmiMuonContainer = cosmiMuonContainer.split('+').pop()
    outputItemList = ['TileCosmicMuonContainer#' + cosmiMuonContainer]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge( OutputStreamCfg(flags, streamName, ItemList = outputItemList) )

    return acc


if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileMuonFitterCfg(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileMuonFitter.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
