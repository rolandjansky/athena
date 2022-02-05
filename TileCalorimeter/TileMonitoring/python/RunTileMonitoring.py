#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''@file RunTileMonitoring.py
@brief Script to run Tile Reconstrcution/Monitoring with new-style configuration
'''
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format


def ByteStreamEmonReadCfg( inputFlags, type_names=[]):
    """
    Creates accumulator for BS Emon reading
    """

    acc = ComponentAccumulator()

    EventSelectorByteStream = CompFactory.EventSelectorByteStream
    eventSelector = EventSelectorByteStream("EventSelector")
    eventSelector.FileBased = False
    acc.addService( eventSelector )
    acc.setAppProperty( "EvtSel", eventSelector.name )

    ByteStreamEmonInputSvc = CompFactory.ByteStreamEmonInputSvc
    bsInputSvc = ByteStreamEmonInputSvc( "ByteStreamInputSvc" )
    acc.addService( bsInputSvc )

    EvtPersistencySvc = CompFactory.EvtPersistencySvc
    eventPersistencySvc = EvtPersistencySvc( "EventPersistencySvc" )
    acc.addService( eventPersistencySvc )

    ByteStreamCnvSvc = CompFactory.ByteStreamCnvSvc
    bsCnvSvc = ByteStreamCnvSvc()
    eventSelector.ByteStreamInputSvc = bsInputSvc.name
    eventPersistencySvc.CnvServices = [ bsCnvSvc.name ]
    acc.addService( bsCnvSvc )

    ROBDataProviderSvc=CompFactory.ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    ByteStreamAddressProviderSvc = CompFactory.ByteStreamAddressProviderSvc
    bsAddressProviderSvc = ByteStreamAddressProviderSvc(TypeNames=type_names)
    acc.addService( bsAddressProviderSvc )

    ProxyProviderSvc = CompFactory.ProxyProviderSvc
    proxy = ProxyProviderSvc()
    proxy.ProviderNames += [ bsAddressProviderSvc.name ]
    acc.addService( proxy )

    return acc



def _configFlagsFromPartition(flags, partition, log):
    """
    Configure the following flags from partition in online: run number, beam type, and project
    """

    from ipc import IPCPartition
    from ispy import ISObject
    ipcPartition = IPCPartition(partition)
    if not ipcPartition.isValid():
        log.error( 'Partition: ' + ipcPartition.name() + ' is not valid' )
        sys.exit(1)
    try:
        runParams = ISObject(ipcPartition, 'RunParams.SOR_RunParams', 'RunParams')
    except Exception:
        beamType = 'cosmics'
        runNumber = 399999
        projectName = 'data20_calib'
        log.warning("No Run Parameters in IS => Set defaults: partition: %s, beam type: %i, run number: %i, project tag: %s",
                    partition, beamType, runNumber, projectName)
    else:
        runParams.checkout()
        beamType = runParams.beam_type
        beamEnergy = runParams.beam_energy
        runNumber = runParams.run_number
        projectName = runParams.T0_project_tag
        runType = runParams.run_type
        log.info("RUN CONFIG: partition: %s, run type: %s, beam type: %i, beam energy: %i, run number: %i, project tag: %s",
                 partition, runType, beamType, beamEnergy, runNumber, projectName)

        if any([projectName.endswith(_) for _ in ("cos", "test", "calib")]):
            beamType = 'cosmics'
        elif projectName.endswith('1beam'):
            beamType = 'singlebeam'
        else:
            beamType = 'collisions'

        if partition == 'Tile':
            ConfigFlags.Tile.NoiseFilter = 0
            if 'CIS' in runType:
                flags.Tile.RunType = 'MONOCIS' if 'mono' in runType else 'CIS'
            elif 'Laser' in runType:
                flags.Tile.RunType = 'LAS'
            elif 'Pedestals' in runType:
                flags.Tile.RunType = 'PED'

    flags.Beam.Type = beamType
    flags.Input.ProjectName = projectName
    flags.Input.RunNumber = [runNumber]


if __name__=='__main__':
    import sys,os

    def _addBoolArgument(parser, argument, dest=None, help=''):
        group = parser.add_mutually_exclusive_group()
        destination = dest if dest else argument
        group.add_argument('--' + argument, dest=destination, action='store_true', help="Switch on " + help)
        group.add_argument('--no-' + argument, dest=destination, action='store_false', help="Switch off " + help)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    parser = ConfigFlags.getArgumentParser()
    parser.add_argument('--preExec', help='Code to execute before locking configs')
    parser.add_argument('--postExec', help='Code to execute after setup')
    parser.add_argument('--printDetailedConfig', action='store_true', help='Print detailed Athena configuration')
    parser.add_argument('--dumpArguments', action='store_true', help='Print arguments and exit')

    _addBoolArgument(parser, 'laser', help='Tile Laser monitoring')
    _addBoolArgument(parser, 'noise', help='Tile Noise monitoring')
    _addBoolArgument(parser, 'cells', help='Tile Calorimeter Cells monitoring')
    _addBoolArgument(parser, 'towers', help='Tile Calorimeter Towers monitoring')
    _addBoolArgument(parser, 'clusters', help='Tile Calorimeter Clusters monitoring')
    _addBoolArgument(parser, 'muid', help='Tile Calorimeter MuId monitoring')
    _addBoolArgument(parser, 'muonfit', help='Tile Calorimeter MuonFit monitoring')
    _addBoolArgument(parser, 'mbts', help='MBTS monitoring')
    _addBoolArgument(parser, 'rod', help='Tile Calorimeter ROD monitoring')
    _addBoolArgument(parser, 'digi-noise',dest='digiNoise', help='Tile digi noise monitoring')
    _addBoolArgument(parser, 'raw-chan-noise',dest='rawChanNoise', help='Tile raw channel noise monitoring')
    _addBoolArgument(parser, 'tmdb', help='TMDB monitoring')
    _addBoolArgument(parser, 'tmdb-digits', dest='tmdbDigits', help='TMDB digits monitoring')
    _addBoolArgument(parser, 'tmdb-raw-channels', dest='tmdbRawChannels', help='TMDB raw channels monitoring')
    _addBoolArgument(parser, 'online', help='Online environment running')

    parser.add_argument('--stateless', action="store_true", help='Run Online Tile monitoring in partition')
    parser.add_argument('--use-mbts-trigger', action="store_true", dest='useMbtsTrigger', help='Use L1 MBTS triggers')
    parser.add_argument('--partition', default="", help='EMON, Partition name, default taken from $TDAQ_PARTITION if not set')
    parser.add_argument('--key', type=str, default="",
                        help='EMON, Selection key, e.g.: SFI, default: dcm (ATLAS), CompleteEvent (TileMon), ReadoutApplication (Tile)')
    parser.add_argument('--keyValue', default="",
                        help='EMON, Key values, e.g. [SFI-1, SFI-2]; if empty all SFIs; default: "" (*), TileREB-ROS (Tile)')
    parser.add_argument('--keyCount', type=int, default=50,
                        help='EMON, key count, e.g. 5 to get five random SFIs, default: 50 (physics), 1000 (laser)')
    parser.add_argument('--publishName', default='TilePT-stateless-10', help='EMON, Name under which to publish histograms')
    parser.add_argument('--include', default="", help='EMON, Regular expression to select histograms to publish')
    parser.add_argument('--lvl1Items', default=[], help='EMON, A list of L1 bit numbers, default []')
    parser.add_argument('--lvl1Logic', default='Ignore', choices=['And','Or','Ignore'], help='EMON, default: Ignore')
    parser.add_argument('--lvl1Origin', default='TAV', choices=['TBP','TAP','TAV'], help='EMON, default: TAV')
    parser.add_argument('--streamType', default='physics', help='EMON, HLT stream type (e.g. physics or calibration)')
    parser.add_argument('--streamNames', default="", help='EMON, List of HLT stream names')
    parser.add_argument('--streamLogic', default='Or', choices=['And','Or','Ignore'], help='EMON, default: Or')
    parser.add_argument('--triggerType', type=int, default=256, help='EMON, LVL1 8 bit trigger type, default: 256')
    parser.add_argument('--groupName', default="TilePhysMon", help='EMON, Name of the monitoring group')

    update_group = parser.add_mutually_exclusive_group()
    update_group.add_argument('--frequency', type=int, default=0, help='EMON, Frequency (in number of events) of publishing histograms')
    update_group.add_argument('--updatePeriod', type=int, default=60, help='EMON, Frequency (in seconds) of publishing histograms')

    args, _ = parser.parse_known_args()

    # Set up default arguments which can be overriden via command line
    if not any([args.laser, args.noise, args.mbts]):
        mbts = False if args.stateless else True
        parser.set_defaults(cells=True, towers=True, clusters=True, muid=True, muonfit=True, mbts=mbts,
                            rod=True, tmdb=True, tmdbDigits=True, tmdbRawChannels=True)
    elif args.noise:
        parser.set_defaults(digiNoise=True, rawChanNoise=True)

    if args.stateless:
        parser.set_defaults(online=True)
        partition = args.partition if args.partition else os.getenv('TDAQ_PARTITION', 'ATLAS')

        keys = {'ATLAS' : 'dcm', 'TileMon' : 'CompleteEvent', 'Tile' : 'ReadoutApplication'}
        key = args.key if args.key else keys.get(partition, 'dcm')

        keyValues = {'Tile': 'TileREB-ROS'}
        keyValue =  args.keyValue if  args.keyValue else keyValues.get(partition, "")

        # Given frequency, set up updatePeriod to 0, since updatePeriod has higher priority
        updatePeriod = 0 if args.frequency > 0 else args.updatePeriod
        parser.set_defaults(partition=partition, key=key, keyValue=keyValue, updatePeriod=updatePeriod)

        if args.laser:
            parser.set_defaults(streamType='calibration', streamNames=['Tile'], streamLogic='And', keyCount=1000, groupName='TileLasMon')
        elif args.noise:
            publishInclude = ""
            parser.set_defaults(streamType='physics', streamNames=['CosmicCalo'], streamLogic='And', include=publishInclude,
                                triggerType=0x82, frequency=300, updatePeriod=0, keyCount=1000, groupName='TileNoiseMon')
        elif args.mbts:
            parser.set_defaults(lvl1Logic='Or', lvl1Origin='TAV', lvl1Items=[164], streamNames=[], streamLogic='Ignore',
                                groupName='TileMBTSMon', useMbtsTrigger = True)

    args, _ = parser.parse_known_args()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    if args.dumpArguments:
        log.info('FINAL ARGUMENTS FOLLOW')
        log.info(args)
        sys.exit(0)

    # Set the Athena configuration flags to defaults (can be overriden via comand line)
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
    ConfigFlags.Tile.RunType = 'PHY'

    if args.stateless:
        _configFlagsFromPartition(ConfigFlags, args.partition, log)
        ConfigFlags.Input.isMC = False
        ConfigFlags.Input.Format = Format.BS
        if args.mbts and args.useMbtsTrigger:
            from AthenaConfiguration.AutoConfigOnlineRecoFlags import autoConfigOnlineRecoFlags
            autoConfigOnlineRecoFlags(ConfigFlags, args.partition)
    else:
        if args.filesInput:
            ConfigFlags.Input.Files = args.filesInput.split(",")
        elif args.laser:
            inputDirectory = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TileByteStream/TileByteStream-02-00-00"
            inputFile = "data18_tilecomm.00363899.calibration_tile.daq.RAW._lb0000._TileREB-ROS._0005-200ev.data"
            ConfigFlags.Input.Files = [os.path.join(inputDirectory, inputFile)]
            ConfigFlags.Input.RunNumber = [363899]
        elif args.noise:
            inputDirectory = 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-tile/test'
            inputFile = 'data12_8TeV.00201555.physics_ZeroBiasOverlay.merge.RAW._lb0150._SFO-ALL._0001.1'
            ConfigFlags.Input.Files = [os.path.join(inputDirectory, inputFile)]
        else:
            from AthenaConfiguration.TestDefaults import defaultTestFiles
            ConfigFlags.Input.Files = defaultTestFiles.RAW

    runNumber = ConfigFlags.Input.RunNumber[0]
    if not ConfigFlags.Output.HISTFileName:
        ConfigFlags.Output.HISTFileName = 'tilemon_{}.root'.format(runNumber)

    if args.online:
        ConfigFlags.Common.isOnline = True
    if ConfigFlags.Common.isOnline:
        ConfigFlags.IOVDb.GlobalTag = 'CONDBR2-HLTP-2018-01' if runNumber > 232498 else 'COMCOND-HLTP-004-02'
        ConfigFlags.DQ.Environment = 'online'
        ConfigFlags.DQ.FileKey = ''
    else:
        ConfigFlags.IOVDb.GlobalTag = 'CONDBR2-BLKPA-2018-14' if runNumber > 232498 else 'COMCOND-BLKPA-RUN1-06'

    if args.laser:
        ConfigFlags.Tile.RunType = 'LAS'
        ConfigFlags.Tile.TimingType = 'GAP/LAS'
        ConfigFlags.Tile.doFit = True
        ConfigFlags.Tile.correctTime = True
        ConfigFlags.Tile.doOverflowFit = False
        ConfigFlags.Tile.BestPhaseFromCOOL = True
        ConfigFlags.Tile.NoiseFilter = 1

    # Override default configuration flags from command line arguments
    ConfigFlags.fillFromArgs(parser=parser)

    if args.preExec:
        log.info('Executing preExec: %s', args.preExec)
        exec(args.preExec)

    log.info('FINAL CONFIG FLAGS SETTINGS FOLLOW')
    ConfigFlags.dump()

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    typeNames = ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    if any([args.tmdbDigits, args.tmdb]):
        typeNames += ['TileDigitsContainer/MuRcvDigitsCnt']
    if any([args.tmdbRawChannels, args.tmdb]):
        typeNames += ['TileRawChannelContainer/MuRcvRawChCnt']
    if args.mbts:
        typeNames += ['CTP_RDO/CTP_RDO']

    if args.stateless:
        cfg.merge( ByteStreamEmonReadCfg(ConfigFlags, type_names=typeNames) )
        bsEmonInputSvc = cfg.getService( "ByteStreamInputSvc" )
        bsEmonInputSvc.Partition = args.partition
        bsEmonInputSvc.Key = args.key
        bsEmonInputSvc.KeyValue = args.keyValue
        bsEmonInputSvc.KeyCount = args.keyCount
        bsEmonInputSvc.PublishName = args.publishName
        bsEmonInputSvc.ISServer = 'Histogramming'
        bsEmonInputSvc.Include = args.include
        bsEmonInputSvc.UpdatePeriod = args.updatePeriod
        bsEmonInputSvc.Frequency = args.frequency
        bsEmonInputSvc.LVL1Items = args.lvl1Items
        bsEmonInputSvc.LVL1Logic = args.lvl1Logic
        bsEmonInputSvc.LVL1Origin = args.lvl1Origin
        bsEmonInputSvc.StreamType = 'express' if ConfigFlags.Beam.Type == 'singlebeam' else args.streamType
        bsEmonInputSvc.StreamNames = args.streamNames
        bsEmonInputSvc.StreamLogic = args.streamLogic
        bsEmonInputSvc.GroupName = args.groupName
    else:
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = typeNames) )

    cfg.addPublicTool( CompFactory.TileROD_Decoder(fullTileMode = runNumber) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    from TileMonitoring.TileDQFragMonitorAlgorithm import TileDQFragMonitoringConfig
    cfg.merge( TileDQFragMonitoringConfig(ConfigFlags) )

    l1Triggers = ['bit0_RNDM', 'bit1_ZeroBias', 'bit2_L1Cal', 'bit3_Muon',
                  'bit4_RPC', 'bit5_FTK', 'bit6_CTP', 'bit7_Calib', 'AnyPhysTrig']

    if args.laser:
        from TileMonitoring.TileRawChannelTimeMonitorAlgorithm import TileRawChannelTimeMonitoringConfig
        cfg.merge(TileRawChannelTimeMonitoringConfig(ConfigFlags))

    if args.rod:
        from TileMonitoring.TileRODMonitorAlgorithm import TileRODMonitoringConfig
        cfg.merge(TileRODMonitoringConfig(ConfigFlags, fillHistogramsForL1Triggers = l1Triggers))

    if args.tmdbDigits:
        from TileMonitoring.TileTMDBDigitsMonitorAlgorithm import TileTMDBDigitsMonitoringConfig
        cfg.merge(TileTMDBDigitsMonitoringConfig(ConfigFlags))

    if args.tmdbRawChannels:
        from TileMonitoring.TileTMDBRawChannelMonitorAlgorithm import TileTMDBRawChannelMonitoringConfig
        cfg.merge(TileTMDBRawChannelMonitoringConfig(ConfigFlags))

    if args.tmdb:
        from TileMonitoring.TileTMDBMonitorAlgorithm import TileTMDBMonitoringConfig
        cfg.merge(TileTMDBMonitoringConfig(ConfigFlags))

    if any([args.cells, args.towers, args.clusters, args.mbts, args.muid, args.muonfit]):
        from TileRecUtils.TileCellMakerConfig import TileCellMakerCfg
        cfg.merge( TileCellMakerCfg(ConfigFlags) )

    if args.cells:
        from TileMonitoring.TileCellMonitorAlgorithm import TileCellMonitoringConfig
        cfg.merge(TileCellMonitoringConfig(ConfigFlags, fillHistogramsForL1Triggers = l1Triggers))

    if args.towers:
        from TileMonitoring.TileTowerMonitorAlgorithm import TileTowerMonitoringConfig
        cfg.merge(TileTowerMonitoringConfig(ConfigFlags, fillHistogramsForL1Triggers = l1Triggers))

    if args.clusters:
        from TileMonitoring.TileClusterMonitorAlgorithm import TileClusterMonitoringConfig
        cfg.merge(TileClusterMonitoringConfig(ConfigFlags, fillTimingHistograms = True, fillHistogramsForL1Triggers = l1Triggers))

    if args.mbts:
        from TileMonitoring.TileMBTSMonitorAlgorithm import TileMBTSMonitoringConfig
        cfg.merge(TileMBTSMonitoringConfig(ConfigFlags, FillHistogramsPerMBTS = True, useTrigger = args.useMbtsTrigger))

    if args.muid:
        from TileMuId.TileMuIdConfig import TileLookForMuAlgCfg
        cfg.merge(TileLookForMuAlgCfg(ConfigFlags))

        from TileMonitoring.TileMuIdMonitorAlgorithm import TileMuIdMonitoringConfig
        cfg.merge(TileMuIdMonitoringConfig(ConfigFlags, fillHistogramsForL1Triggers = l1Triggers))

    if args.muonfit:
        from TileCosmicAlgs.TileMuonFitterConfig import TileMuonFitterCfg
        cfg.merge(TileMuonFitterCfg(ConfigFlags))

        from TileMonitoring.TileMuonFitMonitorAlgorithm import TileMuonFitMonitoringConfig
        cfg.merge(TileMuonFitMonitoringConfig(ConfigFlags, fillHistogramsForL1Triggers = l1Triggers))

    if args.digiNoise:
        from TileMonitoring.TileDigiNoiseMonitorAlgorithm import TileDigiNoiseMonitoringConfig
        cfg.merge(TileDigiNoiseMonitoringConfig(ConfigFlags))

    if args.rawChanNoise:
        from TileMonitoring.TileRawChannelNoiseMonitorAlgorithm import TileRawChannelNoiseMonitoringConfig
        cfg.merge(TileRawChannelNoiseMonitoringConfig(ConfigFlags))

    # Any last things to do?
    if args.postExec:
        log.info('Executing postExec: %s', args.postExec)
        exec(args.postExec)

    if ConfigFlags.Common.isOnline:
        cfg.getService("THistSvc").Output=["Tile DATAFILE='%s' OPT='RECREATE'" % (ConfigFlags.Output.HISTFileName)]

    cfg.printConfig(withDetails=args.printDetailedConfig)

    sc = cfg.run()
    sys.exit(0 if sc.isSuccess() else 1)
