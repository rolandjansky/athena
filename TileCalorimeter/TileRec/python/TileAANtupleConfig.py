#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

'''
@file TileAANtupleConfig.py
@brief Python configuration of TileAANtuple algorithm for the Run III
'''
def TileAANtupleCfg(flags, outputFile='', saveTMDB=True, **kwargs):
    ''' Function to configure TileAANtuple algorithm.'''

    cisRun      = flags.Tile.RunType == 'CIS'
    laserRun    = flags.Tile.RunType == 'LAS'
    pedestalRun = flags.Tile.RunType == 'PED'
    physicsRun  = flags.Tile.RunType == 'PHY'

    readDigits = flags.Tile.readDigits

    kwargs.setdefault('TileDigitsContainer', 'TileDigitsCnt' if readDigits else "")
    kwargs.setdefault('TileDigitsContainerFlt', 'TileDigitsFlt' if not readDigits else "")
    kwargs.setdefault('TileRawChannelContainer', flags.Tile.RawChannelContainer)
    kwargs.setdefault('TileRawChannelContainerFit', 'TileRawChannelFit' if flags.Tile.doFit else "")
    kwargs.setdefault('TileRawChannelContainerFitCool', 'TileRawChannelFitCool' if flags.Tile.doFitCOOL else "")
    kwargs.setdefault('TileRawChannelContainerOpt', "")
    kwargs.setdefault('TileRawChannelContainerQIE', 'TileRawChannelQIE' if flags.Tile.doQIE else "")
    kwargs.setdefault('TileRawChannelContainerOF1', 'TileRawChannelOF1' if flags.Tile.doOF1 else "")
    kwargs.setdefault('TileRawChannelContainerMF', 'TileRawChannelMF' if flags.Tile.doMF else "")
    kwargs.setdefault('TileRawChannelContainerWiener', "")
    kwargs.setdefault('TileRawChannelContainerDsp', "")
    kwargs.setdefault('TileLaserObject', 'TileLaserObj' if laserRun else "")
    kwargs.setdefault('TileBeamElemContainer', 'TileBeamElemCnt' if not physicsRun else "")

    kwargs.setdefault('TileMuRcvRawChannelContainer', 'MuRcvRawChCnt' if saveTMDB else "")
    kwargs.setdefault('TileMuRcvDigitsContainer', 'MuRcvDigitsCnt' if saveTMDB else "")
    kwargs.setdefault('TileMuRcvContainer', "TileMuRcvCnt" if saveTMDB else "")

    kwargs.setdefault('CheckDCS', flags.Tile.useDCS)
    kwargs.setdefault('BSInput', flags.Input.Format is Format.BS and not physicsRun)
    kwargs.setdefault('CalibMode', pedestalRun or cisRun)
    kwargs.setdefault('CalibrateEnergy', flags.Input.isMC or not (cisRun or physicsRun))

    acc = ComponentAccumulator()

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
        acc.merge( TileBadChannelsCondAlgCfg(flags) )

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    if kwargs['CheckDCS']:
        from TileConditions.TileDCSConfig import TileDCSCondAlgCfg
        acc.merge( TileDCSCondAlgCfg(flags) )

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    if not outputFile:
        run = str(flags.Input.RunNumber[0])
        outputFile = 'tile_{}.aan.root'.format(run)
    histsvc = CompFactory.THistSvc()
    histsvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % ('AANT', outputFile)]
    acc.addService(histsvc)

    TileAANtuple = CompFactory.TileAANtuple
    acc.addEventAlgo(TileAANtuple('TileNtuple', **kwargs), primary = True)

    return acc


if __name__=='__main__':

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    parser = ConfigFlags.getArgumentParser()
    parser.add_argument('--postExec', help='Code to execute after setup')
    parser.add_argument('--no-tmdb', dest='tmdb', action='store_false', help='Do not save TMDB information into ntuple')
    args, _ = parser.parse_known_args()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs(parser=parser)

    log.info('FINAL CONFIG FLAGS SETTINGS FOLLOW')
    ConfigFlags.dump()

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    rawChannelContainer = 'TileRawChannelCnt'

    if ConfigFlags.Input.Format is Format.BS:

        cfg.addPublicTool(CompFactory.TileROD_Decoder(fullTileMode=ConfigFlags.Input.RunNumber[0]))

        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        tileTypeNames = ['TileDigitsContainer/TileDigitsCnt',
                         'TileRawChannelContainer/TileRawChannelCnt',]
        if args.tmdb:
            tileTypeNames += ['TileRawChannelContainer/MuRcvRawChCnt',
                              'TileDigitsContainer/MuRcvDigitsCnt',
                              'TileMuonReceiverContainer/TileMuRcvCnt',]

        if ConfigFlags.Tile.RunType == 'LAS':
            tileTypeNames += ['TileLaserObject/TileLaserObj']
        if ConfigFlags.Tile.RunType != 'PHY':
            tileTypeNames += ['TileBeamElemContainer/TileBeamElemCnt']

        cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )
        cfg.getEventAlgo('SGInputLoader').FailIfNoProxy=False
    else:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        cfg.merge(PoolReadCfg(ConfigFlags))

        inputCollections = ConfigFlags.Input.Collections
        if rawChannelContainer not in inputCollections:
            rawChannelContainer = 'TileRawChannelFlt' if 'TileRawChannelFlt' in inputCollections else ""

    cfg.merge( TileAANtupleCfg(ConfigFlags, TileRawChannelContainer=rawChannelContainer, saveTMDB = args.tmdb) )

    # Any last things to do?
    if args.postExec:
        log.info('Executing postExec: %s', args.postExec)
        exec(args.postExec)

    cfg.printConfig(withDetails = True, summariseProps = True)

    cfg.store( open('TileAANtuple.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(0 if sc.isSuccess() else 1)
