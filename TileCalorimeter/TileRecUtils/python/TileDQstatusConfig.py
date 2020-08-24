# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile DQ status tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileDQstatusToolCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile DQ status tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        SimulateTrips - flag to simulate drawer trips. Defaults to False.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('SimulateTrips', False)

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChanTool

    TileDQstatusTool=CompFactory.TileDQstatusTool
    acc.setPrivateTools( TileDQstatusTool(**kwargs) )

    return acc


def TileDQstatusAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile DQ status algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        TileDQstatus - name of Tile DQ status produced
        TileDigitsContainer - name of Tile digits container, provided it will be used,
                              otherwise it will be determined automatically depending on flags.
        TileRawChannelContainer - name of Tile raw channel container, provided it will be used,
                                  otherwise it will be determined automatically depending on flags.
        TileBeamElemContainer - name of Tile beam elements container, provided it will be used,
                                otherwise it will be determined automatically depending on flags.
    """


    acc = ComponentAccumulator()
    kwargs.setdefault('TileDQstatus', 'TileDQstatus')

    name = kwargs['TileDQstatus'] + 'Alg'
    kwargs.setdefault('name', name)

    if not (flags.Input.isMC or flags.Overlay.DataOverlay or flags.Input.Format.lower() == 'pool'):
        if flags.Tile.RunType == 'PHY' or flags.Tile.TimingType == 'GAP/LAS':
            beamElemContainer = ""
        else:
            beamElemContainer = 'TileBeamElemCnt'

        if flags.Tile.readDigits:
            digitsContainer = 'TileDigitsCnt'
        else:
            digitsContainer = ""

        rawChannelContainer = 'TileRawChannelCnt'

    elif flags.Overlay.DataOverlay and flags.Detector.OverlayTile:
        beamElemContainer = ''
        digitsContainer = flags.Overlay.BkgPrefix + 'TileDigitsCnt'
        rawChannelContainer = flags.Overlay.BkgPrefix + 'TileRawChannelCnt'

    else:
        beamElemContainer = ""
        digitsContainer = ""
        rawChannelContainer = ""

    kwargs.setdefault('TileBeamElemContainer', beamElemContainer)
    kwargs.setdefault('TileDigitsContainer', digitsContainer)
    kwargs.setdefault('TileRawChannelContainer', rawChannelContainer)

    if 'TileDQstatusTool' not in kwargs:
        tileDQstatusTool = acc.popToolsAndMerge( TileDQstatusToolCfg(flags) )
        kwargs['TileDQstatusTool'] = tileDQstatusTool

    TileDQstatusAlg=CompFactory.TileDQstatusAlg
    acc.addEventAlgo(TileDQstatusAlg(**kwargs), primary = True)

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
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg(ConfigFlags, ['TileDigitsContainer/TileDigitsCnt']) )

    acc.merge( TileDQstatusAlgCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDQstatus.pkl','wb') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

