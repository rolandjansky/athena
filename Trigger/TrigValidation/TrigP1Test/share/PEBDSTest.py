#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
This test defines its own version of the Dev_pp_run3_v1 menu and the corresponding PEB/DS configuration,
and executes several chains testing various types of Partial Event Building and Data Scouting
'''

from TrigEDMConfig import DataScoutingInfo, TriggerEDMRun3
from TriggerMenuMT.HLT.Menu import Dev_pp_run3_v1, EventBuildingInfo, StreamInfo
from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLT.CommonSequences import EventBuildingSequences
from TrigPartialEventBuilding.TrigPartialEventBuildingConfig import StaticPEBInfoWriterToolCfg, RoIPEBInfoWriterToolCfg
from libpyeformat_helper import SubDetector
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
log = logging.getLogger('dataScoutingTest')

# Add new allowed event building identifiers
EventBuildingInfo.PartialEventBuildingIdentifiers.append('TestPEBOne')
EventBuildingInfo.PartialEventBuildingIdentifiers.append('TestPEBTwo')
EventBuildingInfo.PartialEventBuildingIdentifiers.append('TestPEBThree')
EventBuildingInfo.PartialEventBuildingIdentifiers.append('TestPEBFour')
DataScoutingInfo.DataScoutingIdentifiers['ElectronDSTest'] = 3
DataScoutingInfo.DataScoutingIdentifiers['ElectronDSPEBTest'] = 3
DataScoutingInfo.TruncationThresholds[3] = 5*(1024**2) # 5 MB

# Override the setupMenu function from Dev_pp_run3_v1
def myMenu():
    log.debug('Executing myMenu')

    from TriggerMenuMT.HLT.Menu.SignatureDicts import ChainStore
    chains = ChainStore()
    chains['Egamma'] = [
        # DS+PEB chain (special HLT result and subset of detector data saved)
        ChainProp(name='HLT_e3_etcut_ElectronDSPEBTest_L1EM3', stream=['ElectronDSPEBTest'], groups=['RATE:Test','BW:Other']),

        # Pure DS chain (only special HLT result saved and no detector data saved)
        ChainProp(name='HLT_e5_etcut_ElectronDSTest_L1EM3', stream=['ElectronDSTest'], groups=['RATE:Test','BW:Other']),

        # PEB chain (full HLT result and fixed subset of detector data saved)
        ChainProp(name='HLT_e7_etcut_TestPEBOne_L1EM3', stream=['TestPEBOne'], groups=['RATE:Test','BW:Other']),

        # PEB chain (full HLT result and RoI-based subset of detector data saved)
        ChainProp(name='HLT_e10_etcut_TestPEBThree_L1EM3', stream=['TestPEBThree'], groups=['RATE:Test','BW:Other']),

        # Standard chain (full HLT result and full detector data saved)
        ChainProp(name='HLT_e12_etcut_L1EM3', stream=['Main'], groups=['RATE:SingleElectron', 'BW:Electron']),
    ]

    chains['Muon'] = [
        # PEB chain (fixed subset of detector data saved and no HLT result)
        ChainProp(name='HLT_mu6_TestPEBTwo_L1MU5VF', stream=['TestPEBTwo'], groups=['RATE:Test','BW:Other']),

        # PEB chain (RoI-based subset of detector data saved and no HLT result)
        ChainProp(name='HLT_mu6_TestPEBFour_L1MU5VF', stream=['TestPEBFour'], groups=['RATE:Test','BW:Other']),

        # Standard chain (full HLT result and full detector data saved)
        ChainProp(name='HLT_2mu6_L12MU5VF', stream=['Main'], groups=['RATE:SingleMuon', 'BW:Muon']),
    ]
    return chains

Dev_pp_run3_v1.setupMenu = myMenu

# Override the pebInfoWriterTool function from EventBuildingSequences
def myPebInfoWriterTool(name, eventBuildType):
    log.debug('Executing myPebInfoWriterTool')
    tool = None
    if 'TestPEBOne' in eventBuildType:
        # TestPEBOne is an example which saves a few detector ROBs
        # and the full HLT result (typically saved in physics streams)
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.ROBList = [0x42002e, 0x420060, 0x420064] # a few example LAr ROBs
        tool.addHLTResultToROBList() # add the main (full) HLT result to the list
    elif 'TestPEBTwo' in eventBuildType:
        # TestPEBTwo is an example which saves some detector data,
        # but no HLT result (not needed in detector calibration streams)
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.SubDetList = [
            int(SubDetector.MUON_RPC_BARREL_A_SIDE),
            int(SubDetector.MUON_RPC_BARREL_C_SIDE)] # example: RPC side A and C
    elif 'TestPEBThree' in eventBuildType:
        # TestPEBThree is an example using RoIPEBInfoWriterTool which writes
        # all ROBs within a given RoI, and also the main (full) HLT result
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.EtaEdge = 5.0
        tool.EtaWidth = 0.1
        tool.PhiWidth = 0.1
        tool.MaxRoIs = 3
        tool.addRegSelDets(['All'])
        tool.ExtraROBs = []
        tool.ExtraSubDets = []
        tool.addHLTResultToROBList() # add the main (full) HLT result to the list
    elif 'TestPEBFour' in eventBuildType:
        # TestPEBFour is similar to TestPEBThree, but saves only muon detector
        # ROBs within a larger RoI and no HLT result
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.EtaWidth = 0.5
        tool.PhiWidth = 0.5
        tool.addRegSelDets(['MDT', 'CSC', 'RPC', 'TGC', 'MM', 'sTGC']) # all muon detectors
        tool.ExtraROBs = []
    elif 'ElectronDSTest' in eventBuildType:
        # ElectronDSTest is an example of pure Data Scouting,
        # where only the special HLT result is saved and nothing else
        tool = StaticPEBInfoWriterToolCfg(name)
        moduleId = DataScoutingInfo.getDataScoutingResultID(eventBuildType)
        tool.addHLTResultToROBList(moduleId)
    elif 'ElectronDSPEBTest' in eventBuildType:
        # ElectronDSPEBTest is an example of Data Scouting with PEB,
        # where a special HLT result and some detector data are saved
        # (ID + LAr data within an RoI)
        tool = RoIPEBInfoWriterToolCfg(name)
        moduleId = DataScoutingInfo.getDataScoutingResultID(eventBuildType)
        tool.addHLTResultToROBList(moduleId)
        tool.EtaWidth = 0.3
        tool.PhiWidth = 0.3
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD'])
        tool.ExtraROBs = []
        tool.ExtraSubDets = []

    # Name not matched
    if not tool:
        log.error('PEBInfoWriterTool configuration is missing for event building identifier \'%s\'', eventBuildType)
        return None

    # Add Data Scouting HLT result ROB
    if eventBuildType in DataScoutingInfo.getAllDataScoutingResultIDs():
        moduleId = DataScoutingInfo.getAllDataScoutingResultIDs()[eventBuildType]
        tool.addHLTResultToROBList(moduleId)

    return tool

EventBuildingSequences.pebInfoWriterTool = myPebInfoWriterTool

# Define streams and override StreamInfo
myAllStreams = [
    # [name, type, obeysLumiBlock, forceFullEventBuilding]
    StreamInfo.StreamInfo('Main',               'physics',      True, True),
    StreamInfo.StreamInfo('TestPEBOne',         'physics',      True, False),
    StreamInfo.StreamInfo('TestPEBTwo',         'calibration',  True, False),
    StreamInfo.StreamInfo('TestPEBThree',       'physics',      True, False),
    StreamInfo.StreamInfo('TestPEBFour',        'calibration',  True, False),
    StreamInfo.StreamInfo('ElectronDSTest',     'physics',      True, False),
    StreamInfo.StreamInfo('ElectronDSPEBTest',  'physics',      True, False),
]

StreamInfo._all_streams = myAllStreams

# Modify EDM list to add collections to DataScouting results
myTriggerHLTListRun3 = []
for collectionConfig in TriggerEDMRun3.TriggerHLTListRun3:
    if 'Electron' in collectionConfig[0]:
        modConfig = list(collectionConfig)
        modConfig[1] += ' ElectronDSTest ElectronDSPEBTest'
        myTriggerHLTListRun3.append(tuple(modConfig))
    else:
        myTriggerHLTListRun3.append(collectionConfig)
TriggerEDMRun3.TriggerHLTListRun3 = myTriggerHLTListRun3

# Set menu flag and slice options for runHLT_standalone
ConfigFlags.Trigger.triggerMenuSetup = 'Dev_pp_run3_v1'
doEmptyMenu = True
doEgammaSlice = True
doMuonSlice = True

# Set up everything to run HLT
include('TriggerJobOpts/runHLT_standalone.py')  # noqa: F821
