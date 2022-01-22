#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, conf2toConfigurable
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from AthenaCommon.CFElements import seqOR, parOR
from AthenaCommon.Logging import logging
log = logging.getLogger('MTCalibPebConfig.py')

_menu_name = 'MTCalibPeb'
_menu_file_name = 'HLTMenu_{:s}.json'.format(_menu_name)

# LAR-EMB* ROBs from the file
# /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/ATLASros2rob2018-r22format.py
rob_list = [
  0x420024, 0x420025, 0x420026, 0x420027, 0x420034, 0x420035, 0x420036, 0x420037,
  0x42005c, 0x42005d, 0x42005e, 0x42005f, 0x42006c, 0x42006d, 0x42006e, 0x42006f,
  0x42000c, 0x42000d, 0x420001, 0x42000f, 0x420014, 0x420015, 0x420016, 0x420017,
  0x42001c, 0x42001d, 0x420030, 0x420031, 0x420002, 0x420003, 0x420011, 0x420013,
  0x420018, 0x420019, 0x42001a, 0x42001b, 0x42001e, 0x42001f, 0x420032, 0x420033,
  0x420004, 0x420005, 0x420006, 0x420007, 0x420000, 0x42000e, 0x420020, 0x420021,
  0x420028, 0x420029, 0x42002c, 0x42002d, 0x420008, 0x420009, 0x42000a, 0x42000b,
  0x420010, 0x420012, 0x420022, 0x420023, 0x42002a, 0x42002b, 0x42002e, 0x42002f,
  0x41003c, 0x41003d, 0x41003e, 0x41003f, 0x410038, 0x410046, 0x410058, 0x410059,
  0x410060, 0x410061, 0x410064, 0x410065, 0x410040, 0x410041, 0x410042, 0x410043,
  0x410048, 0x41004a, 0x41005a, 0x41005b, 0x410062, 0x410063, 0x410066, 0x410067,
  0x41007c, 0x41007d, 0x410071, 0x41007f, 0x410084, 0x410085, 0x410086, 0x410087,
  0x41008c, 0x41008d, 0x4100a0, 0x4100a1, 0x410072, 0x410073, 0x410081, 0x410083,
  0x410088, 0x410089, 0x41008a, 0x41008b, 0x41008e, 0x41008f, 0x4100a2, 0x4100a3,
  0x410074, 0x410075, 0x410076, 0x410077, 0x410070, 0x41007e, 0x410090, 0x410091,
  0x410098, 0x410099, 0x41009c, 0x41009d, 0x410078, 0x410079, 0x41007a, 0x41007b,
  0x410080, 0x410082, 0x410092, 0x410093, 0x41009a, 0x41009b, 0x41009e, 0x41009f,
  0x410094, 0x410095, 0x410096, 0x410097, 0x4100a4, 0x4100a5, 0x4100a6, 0x4100a7,
  0x4100cc, 0x4100cd, 0x4100ce, 0x4100cf, 0x4100dc, 0x4100dd, 0x4100de, 0x4100df,
  0x41000c, 0x41000d, 0x410001, 0x41000f, 0x410014, 0x410015, 0x410016, 0x410017,
  0x41001c, 0x41001d, 0x410030, 0x410031, 0x410002, 0x410003, 0x410011, 0x410013,
  0x410018, 0x410019, 0x41001a, 0x41001b, 0x41001e, 0x41001f, 0x410032, 0x410033,
  0x410004, 0x410005, 0x410006, 0x410007, 0x410000, 0x41000e, 0x410020, 0x410021,
  0x410028, 0x410029, 0x41002c, 0x41002d, 0x410008, 0x410009, 0x41000a, 0x41000b,
  0x410010, 0x410012, 0x410022, 0x410023, 0x41002a, 0x41002b, 0x41002e, 0x41002f,
  0x410024, 0x410025, 0x410026, 0x410027, 0x410034, 0x410035, 0x410036, 0x410037,
  0x41005c, 0x41005d, 0x41005e, 0x41005f, 0x41006c, 0x41006d, 0x41006e, 0x41006f,
  0x410044, 0x410045, 0x410039, 0x410047, 0x41004c, 0x41004d, 0x41004e, 0x41004f,
  0x410054, 0x410055, 0x410068, 0x410069, 0x41003a, 0x41003b, 0x410049, 0x41004b,
  0x410050, 0x410051, 0x410052, 0x410053, 0x410056, 0x410057, 0x41006a, 0x41006b,
  0x4100b4, 0x4100b5, 0x4100a9, 0x4100b7, 0x4100bc, 0x4100bd, 0x4100be, 0x4100bf,
  0x4100c4, 0x4100c5, 0x4100d8, 0x4100d9, 0x4100aa, 0x4100ab, 0x4100b9, 0x4100bb,
  0x4100c0, 0x4100c1, 0x4100c2, 0x4100c3, 0x4100c6, 0x4100c7, 0x4100da, 0x4100db,
  0x4100ac, 0x4100ad, 0x4100ae, 0x4100af, 0x4100a8, 0x4100b6, 0x4100c8, 0x4100c9,
  0x4100d0, 0x4100d1, 0x4100d4, 0x4100d5, 0x4100b0, 0x4100b1, 0x4100b2, 0x4100b3,
  0x4100b8, 0x4100ba, 0x4100ca, 0x4100cb, 0x4100d2, 0x4100d3, 0x4100d6, 0x4100d7,
  0x420044, 0x420045, 0x420039, 0x420047, 0x42004c, 0x42004d, 0x42004e, 0x42004f,
  0x420054, 0x420055, 0x420068, 0x420069, 0x42003a, 0x42003b, 0x420049, 0x42004b,
  0x420050, 0x420051, 0x420052, 0x420053, 0x420056, 0x420057, 0x42006a, 0x42006b,
  0x420074, 0x420075, 0x420076, 0x420077, 0x420070, 0x42007e, 0x420090, 0x420091,
  0x420098, 0x420099, 0x42009c, 0x42009d, 0x420078, 0x420079, 0x42007a, 0x42007b,
  0x420080, 0x420082, 0x420092, 0x420093, 0x42009a, 0x42009b, 0x42009e, 0x42009f,
  0x420094, 0x420095, 0x420096, 0x420097, 0x4200a4, 0x4200a5, 0x4200a6, 0x4200a7,
  0x4200cc, 0x4200cd, 0x4200ce, 0x4200cf, 0x4200dc, 0x4200dd, 0x4200de, 0x4200df,
  0x42003c, 0x42003d, 0x42003e, 0x42003f, 0x420038, 0x420046, 0x420058, 0x420059,
  0x420060, 0x420061, 0x420064, 0x420065, 0x420040, 0x420041, 0x420042, 0x420043,
  0x420048, 0x42004a, 0x42005a, 0x42005b, 0x420062, 0x420063, 0x420066, 0x420067,
  0x42007c, 0x42007d, 0x420071, 0x42007f, 0x420084, 0x420085, 0x420086, 0x420087,
  0x42008c, 0x42008d, 0x4200a0, 0x4200a1, 0x420072, 0x420073, 0x420081, 0x420083,
  0x420088, 0x420089, 0x42008a, 0x42008b, 0x42008e, 0x42008f, 0x4200a2, 0x4200a3,
  0x4200b4, 0x4200b5, 0x4200a9, 0x4200b7, 0x4200bc, 0x4200bd, 0x4200be, 0x4200bf,
  0x4200c4, 0x4200c5, 0x4200d8, 0x4200d9, 0x4200aa, 0x4200ab, 0x4200b9, 0x4200bb,
  0x4200c0, 0x4200c1, 0x4200c2, 0x4200c3, 0x4200c6, 0x4200c7, 0x4200da, 0x4200db,
  0x4200ac, 0x4200ad, 0x4200ae, 0x4200af, 0x4200a8, 0x4200b6, 0x4200c8, 0x4200c9,
  0x4200d0, 0x4200d1, 0x4200d4, 0x4200d5, 0x4200b0, 0x4200b1, 0x4200b2, 0x4200b3,
  0x4200b8, 0x4200ba, 0x4200ca, 0x4200cb, 0x4200d2, 0x4200d3, 0x4200d6, 0x4200d7,
]

rob_access_dict = {
  '01 :ADDGET:RND20:': rob_list,  # Prefetch+Retrieve  20 random ROBs from rob_list
  '02 :GET:RND10:   ': rob_list,  # Retrieve           10 random ROBs from rob_list
  '03 :GET:RND5:    ': rob_list,  # Retrieve            5 random ROBs from rob_list
  '04 :ADD:RND10:   ': rob_list,  # Prefetch           10 random ROBs from rob_list
  '05 :ADD:RND20:   ': rob_list,  # Prefetch           20 random ROBs from rob_list
  '06 :ADDGET:RND50:': rob_list,  # Prefetch+Retrieve  50 random ROBs from rob_list
  '07 :ADDGET:RND10:': rob_list,  # Prefetch+Retrieve  10 random ROBs from rob_list
  '08 :ADDGET:RND5: ': rob_list,  # Prefetch+Retrieve   5 random ROBs from rob_list
  '09 :GET:RND20:   ': rob_list,  # Retrieve           20 random ROBs from rob_list
  '10 :ADDGET:RND20:': rob_list,  # Prefetch+Retrieve  20 random ROBs from rob_list
  '11 :GET:RND10:   ': rob_list,  # Retrieve           10 random ROBs from rob_list
  '12 :ADDGET:RND10:': rob_list   # Prefetch+Retrieve  10 random ROBs from rob_list
}

class MTCalibPebHypoOptions:
    def __init__(self):
        self.UseRandomSeed = False
        self.RandomAcceptRate = -1.0
        self.BurnTimePerCycleMillisec = 0
        self.NumBurnCycles = 0
        self.BurnTimeRandomly = True
        self.Crunch = False
        self.CheckDataConsistency = False
        self.ROBAccessDict = rob_access_dict
        self.TimeBetweenROBReqMillisec = 0
        self.PEBROBList = []
        self.PEBSubDetList = []
        self.CreateRandomData = {}
        self.EnableL1CaloPhase1 = False  # Sets ConfigFlags.Trigger.enableL1CaloPhase1
        self.EnableL1MuonPhase1 = False  # Sets ConfigFlags.Trigger.enableL1MuonPhase1
        self.EnableL1CaloLegacy = True  # Sets ConfigFlags.Trigger.enableL1CaloLegacy


default_options = MTCalibPebHypoOptions()


def set_flags(flags, options=default_options):
    flags.Common.isOnline = True
    flags.Input.Format = Format.BS
    flags.Input.isMC = False
    flags.IOVDb.DatabaseInstance = 'CONDBR2'
    flags.IOVDb.GlobalTag = flags.Trigger.OnlineCondTag
    flags.Trigger.doHLT = True
    flags.Trigger.EDMVersion = 3
    flags.Trigger.triggerMenuSetup = 'Dev_pp_run3_v1'
    flags.Trigger.enableL1MuonPhase1 = options.EnableL1MuonPhase1
    flags.Trigger.enableL1CaloPhase1 = options.EnableL1CaloPhase1
    flags.Trigger.enableL1CaloLegacy = options.EnableL1CaloLegacy
    flags.Trigger.HLTSeeding.forceEnableAllChains = True
    # Disable signature-specific detector configuration parts
    flags.Trigger.doID = False
    flags.Trigger.doCalo = False
    flags.Trigger.doMuon = False


def l1_seq_cfg(flags, options=default_options):
    from HLTSeeding.HLTSeedingConfig import HLTSeedingCfg
    acc = HLTSeedingCfg(flags, seqName='l1Seq')
    l1_decoder_alg = acc.getEventAlgo('HLTSeeding')
    l1_decoder_alg.prescaler = CompFactory.PrescalingEmulationTool()

    # Generate L1 menu
    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
    generateL1Menu(flags)

    # Need to set HLT menu file name here to avoid conflict when merging with HLT sequence CA
    acc.getService("HLTConfigSvc").JsonFileName = _menu_file_name

    return acc


def make_hypo_alg(name):
    hypo = CompFactory.MTCalibPebHypoAlg(name)
    hypo.HypoInputDecisions = 'HLTSeedingSummary'
    hypo.HypoOutputDecisions = 'MTCalibPebDecisions_'+name
    return hypo


def make_hypo_tool(name, options=default_options):
    hypo_tool = CompFactory.MTCalibPebHypoTool(name)
    hypo_tool.UseRandomSeed             = options.UseRandomSeed
    hypo_tool.RandomAcceptRate          = options.RandomAcceptRate
    hypo_tool.BurnTimePerCycleMillisec  = options.BurnTimePerCycleMillisec
    hypo_tool.NumBurnCycles             = options.NumBurnCycles
    hypo_tool.BurnTimeRandomly          = options.BurnTimeRandomly
    hypo_tool.Crunch                    = options.Crunch
    hypo_tool.CheckDataConsistency      = options.CheckDataConsistency
    hypo_tool.ROBAccessDict             = options.ROBAccessDict
    hypo_tool.TimeBetweenROBReqMillisec = options.TimeBetweenROBReqMillisec
    hypo_tool.PEBROBList                = options.PEBROBList
    hypo_tool.PEBSubDetList             = options.PEBSubDetList
    hypo_tool.CreateRandomData          = options.CreateRandomData
    return hypo_tool


def make_all_hypo_algs(num_chains, concurrent=False):
    hypo_tools = []
    for chain_index in range(1, 1+num_chains):
        # Default hypo tool for chains 1, 4, 7, 10, ...
        hypoTool = make_hypo_tool('HLT_MTCalibPeb{:d}'.format(chain_index))
        hypoTool.PEBROBList = rob_list
        hypoTool.PEBSubDetList = []
        # Hypo for chains 2, 5, 8, 11, ...
        if chain_index % 3 == 2:
            hypoTool.PEBROBList = [0x420024, 0x420025, 0x420026, 0x420027]  # example LAr EMB ROBs
            hypoTool.PEBSubDetList = [0x53, 0x54]  # TILECAL_EXT A and C
        # Hypo for chains 3, 6, 9, 12, ...
        elif chain_index % 3 == 0:
            hypoTool.CreateRandomData = {
                'ExampleCollection_{:d}_1'.format(chain_index): 5,
                'ExampleCollection_{:d}_2'.format(chain_index): 3,
            }
            hypoTool.PEBROBList = [0x420034, 0x420035, 0x420036, 0x420037]  # example LAr EMB ROBs
            hypoTool.PEBROBList.extend([0x7c0001])  # extra HLT result for data scouting
        hypo_tools.append(hypoTool)

    # Add the hypo tools to algorithm(s)
    hypo_algs = []
    if concurrent:
        for tool in hypo_tools:
            hypo = make_hypo_alg('HypoAlg{:d}'.format(len(hypo_algs)+1))
            hypo.HypoTools += [tool]
            hypo_algs.append(hypo)
    else:
        hypo = make_hypo_alg('HypoAlg1')
        for tool in hypo_tools:
            hypo.HypoTools += [tool]
        hypo_algs.append(hypo)

    return hypo_algs


def hlt_result_cfg(flags, hypo_algs):
    from TrigEDMConfig.DataScoutingInfo import getFullHLTResultID
    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, StreamTagMakerToolCfg, TriggerBitsMakerToolCfg

    # Tool serialising EDM objects to fill the HLT result
    serialiser = TriggerEDMSerialiserToolCfg('Serialiser')
    for hypo in hypo_algs:
        serialiser.addCollectionListToMainResult([
            'xAOD::TrigCompositeContainer_v1#%s' % hypo.HypoOutputDecisions,
            'xAOD::TrigCompositeAuxContainer_v2#%sAux.' % hypo.HypoOutputDecisions,
        ])

    # Data scouting example
    resultList = [getFullHLTResultID(), 1]
    collections = set()
    chain_names = []
    for hypo in hypo_algs:
        for hypo_tool in hypo.HypoTools:
            chain_names.append(hypo_tool.name)
            if hasattr(hypo_tool,'CreateRandomData'):
                for coll_name in hypo_tool.CreateRandomData.keys():
                    collections.add(coll_name)
    for coll_name in collections:
        serialiser.addCollectionListToResults([
            'xAOD::TrigCompositeContainer_v1#{:s}'.format(coll_name),
            'xAOD::TrigCompositeAuxContainer_v2#{:s}Aux.'.format(coll_name)
        ], resultList)

    # StreamTag definitions
    streamPhysicsMain = {
        'name': 'Main',
        'type': 'physics',
        'obeyLB': True,
        'forceFullEventBuilding': True
    }
    streamExamplePEB = {
        'name': 'ExamplePEB',
        'type': 'calibration',
        'obeyLB': True,
        'forceFullEventBuilding': False
    }
    streamExampleDataScoutingPEB = {
        'name': 'ExampleDataScoutingPEB',
        'type': 'physics',
        'obeyLB': True,
        'forceFullEventBuilding': False
    }
    chain_to_streams = {}
    for counter, ch in enumerate(chain_names, start=1):
        if counter % 3 == 1:
            chain_to_streams[ch] = [streamPhysicsMain]
        elif counter % 3 == 2:
            chain_to_streams[ch] = [streamExamplePEB]
        elif counter % 3 == 0:
            chain_to_streams[ch] = [streamExampleDataScoutingPEB]

    menu_json = write_dummy_menu_json(flags, chain_to_streams.keys(), chain_to_streams)

    acc = ComponentAccumulator()

    # Give the menu json name to HLTConfigSvc
    from TrigConfigSvc.TrigConfigSvcCfg import HLTConfigSvcCfg
    hltConfigSvcCfg = HLTConfigSvcCfg(flags)
    hltConfigSvcCfg.getService("HLTConfigSvc").JsonFileName = menu_json
    acc.merge(hltConfigSvcCfg)

    # Tool adding stream tags to HLT result
    stmaker = StreamTagMakerToolCfg()
    stmaker.ChainDecisions = 'HLTNav_Summary'
    stmaker.PEBDecisionKeys = [str(hypo.HypoOutputDecisions) for hypo in hypo_algs]

    # Tool adding HLT bits to HLT result
    bitsmaker = TriggerBitsMakerToolCfg()
    bitsmaker.ChainDecisions = 'HLTNav_Summary'

    # Configure the HLT result maker to use the above tools
    from TrigServices.TrigServicesConfig import TrigServicesCfg
    trigServicesCfg = TrigServicesCfg(flags)
    hltEventLoopMgr = trigServicesCfg.getPrimary()
    hltEventLoopMgr.ResultMaker.StreamTagMaker = stmaker
    hltEventLoopMgr.ResultMaker.MakerTools = [bitsmaker, serialiser]
    acc.merge(trigServicesCfg)

    return acc


def make_summary_algs(hypo_algs):
    summary = CompFactory.TriggerSummaryAlg('TriggerSummaryAlg')
    summary.InputDecision = 'HLTSeedingSummary'
    summary.FinalDecisions = [str(hypo.HypoOutputDecisions) for hypo in hypo_algs]

    summMaker = CompFactory.DecisionSummaryMakerAlg()
    summMaker.FinalDecisionKeys = [str(hypo.HypoOutputDecisions) for hypo in hypo_algs]
    summMaker.FinalStepDecisions = {}
    for hypo in hypo_algs:
        for tool in hypo.HypoTools:
            summMaker.FinalStepDecisions[tool.getName()] = [str(hypo.HypoOutputDecisions)]
    log.info('summMaker = %s', summMaker)
    return [summary, summMaker]


def hlt_seq_cfg(flags, num_chains, concurrent=False, hackCA2Global=False, hypo_algs=None):
    acc = ComponentAccumulator()

    # Sequences need to ensure that summary algs run after all hypos
    acc.addSequence(seqOR('hltTop'))
    acc.addSequence(parOR('hltHypoSeq'), parentName='hltTop')
    acc.addSequence(parOR('hltEndSeq'), parentName='hltTop')

    if not hypo_algs:
        hypo_algs = make_all_hypo_algs(num_chains, concurrent)
    summary_algs = make_summary_algs(hypo_algs)
    hlt_result_ca = hlt_result_cfg(flags, hypo_algs)

    acc.addEventAlgo(hypo_algs, sequenceName='hltHypoSeq')
    acc.addEventAlgo(summary_algs, sequenceName='hltEndSeq')
    acc.merge(hlt_result_ca)

    # Hack to work around a shortcoming of CA2GlobalWrapper when a component
    # has an empty ToolHandle and CA adds a tool to the handle
    if hackCA2Global:
        from AthenaCommon.Configurable import Configurable
        prevConfBehav = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior=0
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.HltEventLoopMgr.ResultMaker = conf2toConfigurable(acc.getService('HltEventLoopMgr').ResultMaker)
        Configurable.configurableRun3Behavior=prevConfBehav

    return acc


def write_dummy_menu_json(flags, chains, chain_to_streams):
    import json
    from TrigConfHLTUtils.HLTUtils import string2hash
    from TrigConfigSvc.TrigConfigSvcCfg import getHLTPrescalesSetFileName
    menu_dict = dict([
        ("filetype", "hltmenu"),
        ("name", _menu_name),
        ("chains", dict()),
        ("streams", dict()),
        ("sequencers", dict())
    ])
    hlt_ps_dict = dict([
        ("filetype", "hltprescale"),
        ("name", _menu_name),
        ("prescales", dict())
    ])
    counter = 0
    for chain in chains:
        # Prepare information for stream list and fill separate dictionary
        chain_streams = []
        for stream in chain_to_streams[chain]:
            stream_name = stream['name']
            chain_streams.append(stream_name)
            # If not already listed, add stream details to stream dictionary
            if stream_name not in menu_dict["streams"]:
                menu_dict["streams"][stream_name] = dict([
                    ("name", stream_name),
                    ("type", stream['type']),
                    ("obeyLB", stream['obeyLB']),
                    ("forceFullEventBuilding", stream['forceFullEventBuilding'])
                ])

        menu_dict["chains"][chain] = dict([
            ("counter", counter),
            ("nameHash", string2hash(chain)),
            ("legMultiplicities", [1]),
            ("l1item", ''),
            ("l1thresholds", ['FSNOSEED']),
            ("groups", []),
            ("streams", chain_streams),
            ("sequencers", [] )
        ])
        hlt_ps_dict["prescales"][chain] = dict([
            ("counter", counter),
            ("hash", string2hash(chain)),
            ("prescale", 1),
            ("enabled", True),
        ])

        counter += 1

    log.info('Writing trigger menu to %s', _menu_file_name)
    with open(_menu_file_name, 'w') as json_file:
        json.dump(menu_dict, json_file, indent=4, sort_keys=False)

    hlt_ps_file_name = getHLTPrescalesSetFileName(flags)
    log.info('Writing HLT prescales to %s', hlt_ps_file_name)
    with open(hlt_ps_file_name, 'w') as json_file:
        json.dump(hlt_ps_dict, json_file, indent=4, sort_keys=False)

    return _menu_file_name
