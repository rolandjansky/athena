# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile muon receiver decision algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileMuonReceiverDecisionCfg(flags, **kwargs):
    """Return component accumulator with configured Tile muon receiver decision algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Name -- name of TileMuonReceiverDecision algorithm. Defaults to TileMuonReceiverDecision.
    """

    name = kwargs.get('Name', 'TileMuonReceiverDecision')

    acc = ComponentAccumulator()

    from TileSimAlgs.TileSimAlgsConf import TileMuonReceiverDecision
    muRcvDecisionAlg = TileMuonReceiverDecision(name,
                                                MuonReceiverEneThreshCellD6Low = 500,
                                                MuonReceiverEneThreshCellD6andD5Low = 500,
                                                MuonReceiverEneThreshCellD6High = 600,
                                                MuonReceiverEneThreshCellD6andD5High = 600)


    if flags.Digitization.PileUpPremixing:
        muRcvDecisionAlg.TileMuonReceiverContainer = flags.Overlay.BkgPrefix + 'TileMuRcvCnt'

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
    muRcvDecisionAlg.TileCondToolEmscale = emScaleTool

    acc.addEventAlgo(muRcvDecisionAlg, primary = True)

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

    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.Tile.RunType = 'PHY'

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()
    ConfigFlags.dump()

    acc = ComponentAccumulator()

    print( acc.popToolsAndMerge( TileMuonReceiverDecisionCfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileMuonReceiverDecision.pkl','w') )
