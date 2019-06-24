# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile DQ status tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileDQstatusToolCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile DQ status tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        SimulateTrips - flag to simulate drawer trips. Defaults to False.
    """

    acc = ComponentAccumulator()

    simulateTrips = kwargs.get('SimulateTrips', False)

    from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
    badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileDQstatusTool
    acc.setPrivateTools( TileDQstatusTool(SimulateTrips = simulateTrips, 
                                          TileBadChanTool = badChanTool) )

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

    tileDQstatus = kwargs.get('TileDQstatus', 'TileDQstatus')
    name = tileDQstatus + 'Alg'

    if not (flags.Input.isMC or flags.Overlay.DataOverlay):
        if flags.Tile.RunType == 'PHY':
            beamElemContainer = ""
        else:
            beamElemContainer = 'TileBeamElemCnt'

        if flags.Tile.readDigits:
            digitsContainer = 'TileDigitsCnt'
        else:
            digitsContainer = ""

        rawChannelContainer = 'TileRawChannelCnt'
    else:
        beamElemContainer = ""
        digitsContainer = ""
        rawChannelContainer = ""

    tileDigitsContainer = kwargs.get('TileDigitsContainer', digitsContainer)
    tileRawChannelContainer = kwargs.get('TileRawChannelContainer', rawChannelContainer)
    tileBeamElemContainer = kwargs.get('TileBeamElemContainer', beamElemContainer)

    tileDQstatusTool = acc.popToolsAndMerge( TileDQstatusToolCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileDQstatusAlg
    dqStatusAlg = TileDQstatusAlg (name,
                                   TileRawChannelContainer = tileRawChannelContainer,
                                   TileDigitsContainer = tileDigitsContainer,
                                   TileBeamElemContainer = tileBeamElemContainer,
                                   TileDQstatus = tileDQstatus,
                                   TileDQstatusTool = tileDQstatusTool)

    acc.addEventAlgo(dqStatusAlg, primary = True)

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

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    acc = MainServicesSerialCfg()

    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    acc.merge( TrigBSReadCfg(ConfigFlags) )
    acc.getService('ByteStreamAddressProviderSvc').TypeNames += ['TileDigitsContainer/TileDigitsCnt']

    acc.merge( TileDQstatusAlgCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDQstatus.pkl','w') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

