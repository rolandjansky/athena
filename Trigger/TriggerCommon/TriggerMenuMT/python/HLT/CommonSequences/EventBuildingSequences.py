#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigEDMConfig import DataScoutingInfo
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLT.Menu import EventBuildingInfo
from TriggerMenuMT.HLT.Config.MenuComponents import ChainStep, MenuSequence
from TrigPartialEventBuilding.TrigPartialEventBuildingConf import PEBInfoWriterAlg
from TrigPartialEventBuilding.TrigPartialEventBuildingConfig import StaticPEBInfoWriterToolCfg, RoIPEBInfoWriterToolCfg
from HLTSeeding.HLTSeedingConfig import mapThresholdToL1DecisionCollection
from libpyeformat_helper import SourceIdentifier, SubDetector
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.CFElements import seqAND, findAlgorithm
from .LATOMESourceIDs import LATOMESourceIDs
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def addEventBuildingSequence(chain, eventBuildType, chainDict):
    '''
    Add an extra ChainStep to a Chain object with a PEBInfoWriter sequence configured for the eventBuildType
    '''
    if not eventBuildType:
        log.error('No eventBuildType specified')
        return
    if eventBuildType not in EventBuildingInfo.getAllEventBuildingIdentifiers():
        log.error('eventBuildType \'%s\' not found in the allowed Event Building identifiers', eventBuildType)
        return

    def pebInfoWriterToolGenerator(chainDict):
        return pebInfoWriterTool(chainDict['chainName'], eventBuildType)

    inputMaker = pebInputMaker(chain, eventBuildType)
    seq = MenuSequence(
        Sequence    = pebSequence(eventBuildType, inputMaker),
        Maker       = inputMaker,
        Hypo        = PEBInfoWriterAlg('PEBInfoWriterAlg_' + eventBuildType),
        HypoToolGen = pebInfoWriterToolGenerator)

    if len(chain.steps)==0:
        # noalg PEB chain
        step_name = 'Step{:d}_PEBInfoWriter_{:s}'.format(len(chain.steps)+1, eventBuildType)
        step = ChainStep(name=step_name,
                         Sequences=[seq],
                         chainDicts=[chainDict])
    else:
        # standard PEB chain
        prevStep = chain.steps[-1]
        step_name = 'Step{:d}_merged{:s}_PEBInfoWriter_{:s}'.format(len(chain.steps)+1,prevStep.name, eventBuildType)
        step = ChainStep(name=step_name,
                         Sequences=[seq for leg in prevStep.legIds],
                         multiplicity=prevStep.multiplicity,
                         chainDicts=prevStep.stepDicts)

    chain.steps.append(step)


def pebInfoWriterTool(name, eventBuildType):
    '''
    Create PEBInfoWriterTool configuration for the eventBuildType
    '''
    tool = None
    if 'BeamSpotPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([
            SubDetector.PIXEL_BARREL,
            SubDetector.PIXEL_DISK_SIDE, # note different name in C++, ADHI-4753
            SubDetector.PIXEL_B_LAYER,
            SubDetector.PIXEL_IBL,
            SubDetector.SCT_BARREL_A_SIDE,
            SubDetector.SCT_BARREL_C_SIDE,
            SubDetector.SCT_ENDCAP_A_SIDE,
            SubDetector.SCT_ENDCAP_C_SIDE,
            SubDetector.TDAQ_CTP
        ])
    elif 'MuonTrkPEB' == eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'MDT', 'RPC', 'TGC', 'CSC', 'MM', 'sTGC'])
        tool.MaxRoIs = 99
        tool.EtaWidth= 0.75#values from run2 check later
        tool.PhiWidth= 0.75#values from run2 check later
        tool.addHLTResultToROBList()  # add the main (full) HLT result to the output
        tool.addSubDets([SubDetector.TDAQ_CTP]) # add full CTP data to the output
    elif 'IDCalibPEB' == eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT'])
        tool.EtaWidth = 0.1
        tool.PhiWidth = 0.1
        tool.addSubDets([SubDetector.TDAQ_CTP]) # add full CTP data to the output
    elif 'LArPEBCalib' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.LAR_EM_BARREL_A_SIDE,
                         SubDetector.LAR_EM_BARREL_C_SIDE,
                         SubDetector.LAR_EM_ENDCAP_A_SIDE,
                         SubDetector.LAR_EM_ENDCAP_C_SIDE,
                         SubDetector.LAR_HAD_ENDCAP_A_SIDE,
                         SubDetector.LAR_HAD_ENDCAP_C_SIDE,
                         SubDetector.LAR_FCAL_A_SIDE,
                         SubDetector.LAR_FCAL_C_SIDE,
                         SubDetector.LAR_EM_BARREL_ENDCAP_A_SIDE,
                         SubDetector.LAR_EM_BARREL_ENDCAP_C_SIDE,
                         SubDetector.LAR_EM_HAD_ENDCAP_A_SIDE,
                         SubDetector.LAR_EM_HAD_ENDCAP_C_SIDE,
                         SubDetector.TDAQ_CTP
        ])
    elif 'LArPEBHLT' == eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD'])
        tool.MaxRoIs = 5
        tool.addHLTResultToROBList()  # add the main (full) HLT result to the output
        tool.addSubDets([SubDetector.TDAQ_CTP]) # add full CTP data to the output
        tool.addROBs(LATOMESourceIDs) # add full-scan LATOME data
    elif 'LArPEB' == eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD'])
        tool.MaxRoIs = 5
        tool.addSubDets([SubDetector.TDAQ_CTP]) # add full CTP data to the output
        tool.addROBs(LATOMESourceIDs) # add full-scan LATOME data
    elif 'LATOMEPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addROBs(LATOMESourceIDs) # add full-scan LATOME data
    elif 'RPCPEBSecondaryReadout' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addROBs([0x610080, 0x620080])
    elif 'SCTPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.SCT_BARREL_A_SIDE,
                         SubDetector.SCT_BARREL_C_SIDE,
                         SubDetector.SCT_ENDCAP_A_SIDE,
                         SubDetector.SCT_ENDCAP_C_SIDE
        ])
    elif 'TilePEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.TILECAL_LASER_CRATE,
                         SubDetector.TILECAL_BARREL_A_SIDE,
                         SubDetector.TILECAL_BARREL_C_SIDE,
                         SubDetector.TILECAL_EXT_A_SIDE,
                         SubDetector.TILECAL_EXT_C_SIDE,
                         SubDetector.TDAQ_CTP,
                         SubDetector.TDAQ_CALO_PREPROC, # = 0x71
                         SubDetector.TDAQ_CALO_CLUSTER_PROC_DAQ, # = 0x72
                         SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI, # = 0x73
                         SubDetector.TDAQ_CALO_JET_PROC_DAQ, # = 0x74
                         SubDetector.TDAQ_CALO_JET_PROC_ROI # = 0x75
        ])
    elif 'AlfaPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.FORWARD_ALPHA,
                         SubDetector.TDAQ_CTP
        ])
    elif 'LArPEBNoise' == eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD'])
        tool.MaxRoIs = 5
        tool.addHLTResultToROBList()  # add the main (full) HLT result to the output
        tool.addSubDets([SubDetector.TDAQ_CTP]) # add full CTP data to the output
        tool.addROBs(LATOMESourceIDs) # add full-scan LATOME data
        tool.addSubDets([
            SubDetector.MUON_MMEGA_ENDCAP_A_SIDE,            
            SubDetector.MUON_MMEGA_ENDCAP_C_SIDE,            
            SubDetector.MUON_STGC_ENDCAP_A_SIDE,            
            SubDetector.MUON_STGC_ENDCAP_C_SIDE            
         ])
    elif 'ZDCPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.FORWARD_ZDC,
                         SubDetector.TDAQ_CTP
        ])
    elif 'AFPPEB' == eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.FORWARD_AFP,
                         SubDetector.TDAQ_CTP
        ])

    elif eventBuildType in DataScoutingInfo.getAllDataScoutingIdentifiers():
        # Pure DataScouting configuration
        tool = StaticPEBInfoWriterToolCfg(name)
        moduleId = DataScoutingInfo.getDataScoutingResultID(eventBuildType)
        tool.addHLTResultToROBList(moduleId)

    # Name not matched
    if not tool:
        log.error('PEBInfoWriterTool configuration is missing for event building identifier \'%s\'', eventBuildType)
        return None

    return tool


def pebInputMaker(chain, eventBuildType):
    # Check if we are configuring a chain with at least one full-scan leg
    isFullscan = (mapThresholdToL1DecisionCollection('FSNOSEED') in chain.L1decisions)

    # Check if we are configuring RoI-based PEB
    _tmpTool = pebInfoWriterTool('tmpTool_'+eventBuildType, eventBuildType)
    _isRoIBasedPEB = isinstance(_tmpTool, CompFactory.RoIPEBInfoWriterTool)
    _isStaticPEB = isinstance(_tmpTool, CompFactory.StaticPEBInfoWriterTool)
    if not _isRoIBasedPEB and not _isStaticPEB:
        raise RuntimeError('Cannot determine whether ' + eventBuildType +
                           ' is static or RoI-based PEB from a tool of type ' + type(_tmpTool))
    del _tmpTool

    _isNoalg = (len(chain.steps) == 0)

    # Configure the InputMaker
    maker = CompFactory.InputMakerForRoI("IMpeb_"+eventBuildType)
    maker.RoIs = "pebInputRoI_" + eventBuildType
    # Allow more than one feature per input RoI if we care about RoIs, and have at least one Step
    maker.mergeUsingFeature = _isRoIBasedPEB and not _isNoalg

    # Configure the InputMaker RoI tool
    if _isNoalg or _isStaticPEB:
        # Streamers or static PEB: use initial RoI
        maker.RoITool = CompFactory.ViewCreatorInitialROITool()
    elif isFullscan and _isRoIBasedPEB:
        # Full-scan chains with RoI-based PEB: create RoI around feature IParticle
        maker.RoITool = CompFactory.ViewCreatorCentredOnIParticleROITool()
        maker.RoITool.RoisWriteHandleKey = recordable("HLT_Roi_" + eventBuildType)
    else:
        # Other chains: use previous RoI
        maker.RoITool = CompFactory.ViewCreatorPreviousROITool()

    return maker


def pebSequence(eventBuildType, inputMaker):
    # If a Configurable with the same name already exists, the below call
    # returns the existing one. We add the inputMaker to the sequence only if
    # it's not already there (i.e. if the sequence didn't exist before)
    seq = seqAND("pebSequence_"+eventBuildType)
    if findAlgorithm(seq, inputMaker.name()) != inputMaker:
        seq += inputMaker
    return seq


def findEventBuildingStep(chainConfig):
    pebSteps = [s for s in chainConfig.steps if 'PEBInfoWriter' in s.name and 'EmptyPEBAlign' not in s.name]
    if len(pebSteps) == 0:
        return None
    elif len(pebSteps) > 1:
        raise RuntimeError('Multiple Event Building steps in one chain are not supported but found in chain ' + chainConfig.name)
    return pebSteps[0]


def alignEventBuildingSteps(chain_configs, chain_dicts):
    def is_peb_dict(chainNameAndDict):
        return len(chainNameAndDict[1]['eventBuildType']) > 0

    all_peb_chain_dicts = dict(filter(is_peb_dict, chain_dicts.items()))
    all_peb_chain_names = list(all_peb_chain_dicts.keys())

    def is_peb_config(chainNameAndConfig):
        return chainNameAndConfig[0] in all_peb_chain_names

    all_peb_chain_configs = dict(filter(is_peb_config, chain_configs.items()))

    maxPebStepPosition = {} # {eventBuildType: N}

    def getPebStepPosition(chainConfig):
        pebStep = findEventBuildingStep(chainConfig)
        return chainConfig.steps.index(pebStep) + 1

    # First loop to find the maximal PEB step positions to which we need to align
    for chainName, chainConfig in all_peb_chain_configs.items():
        pebStepPosition = getPebStepPosition(chainConfig)
        ebt = all_peb_chain_dicts[chainName]['eventBuildType']
        if ebt not in maxPebStepPosition or pebStepPosition > maxPebStepPosition[ebt]:
            maxPebStepPosition[ebt] = pebStepPosition

    # Second loop to insert empty steps before the PEB steps where needed
    for chainName, chainConfig in all_peb_chain_configs.items():
        pebStepPosition = getPebStepPosition(chainConfig)
        ebt = all_peb_chain_dicts[chainName]['eventBuildType']
        if pebStepPosition < maxPebStepPosition[ebt]:
            numStepsNeeded = maxPebStepPosition[ebt] - pebStepPosition
            log.debug('Aligning PEB step for chain %s by adding %d empty steps', chainName, numStepsNeeded)
            chainConfig.insertEmptySteps('EmptyPEBAlign', numStepsNeeded, pebStepPosition-1)


def isRoIBasedPEB(eventBuildType):
    '''Helper function to determine if eventBuildType corresponds to RoI-based PEB'''
    if not eventBuildType:
        return False
    _tmpTool = pebInfoWriterTool('tmpTool_'+eventBuildType, eventBuildType)
    result = isinstance(_tmpTool, CompFactory.RoIPEBInfoWriterTool)
    del _tmpTool
    return result


# Unit test
if __name__ == "__main__":
    failures = 0
    for eb_identifier in EventBuildingInfo.getAllEventBuildingIdentifiers():
        tool = None
        try:
            tool = pebInfoWriterTool('TestTool_'+eb_identifier, eb_identifier)
        except Exception as ex:
            failures += 1
            log.error('Caught exception while configuring PEBInfoWriterTool for %s: %s', eb_identifier, ex)
            continue

        if not tool:
            failures += 1
            log.error('No tool created for %s', eb_identifier)
            continue

        if tool.getType() not in ['StaticPEBInfoWriterTool', 'RoIPEBInfoWriterTool']:
            failures += 1
            log.error('Unexpected tool type for %s: %s', eb_identifier, tool.getType())
            continue

        robs = tool.ROBList if tool.getType() == 'StaticPEBInfoWriterTool' else tool.ExtraROBs
        dets = tool.SubDetList if tool.getType() == 'StaticPEBInfoWriterTool' else tool.ExtraSubDets
        robs_check_passed = True
        for rob_id in robs:
            rob_sid = SourceIdentifier(rob_id)
            rob_det_id = rob_sid.subdetector_id()
            if int(rob_det_id) in dets:
                robs_check_passed = False
                log.error('Redundant configuration for %s: ROB %s added to the ROB list while full SubDetector '
                          '%s is already in the SubDets list', eb_identifier, rob_sid.human(), str(rob_det_id))

        if not robs_check_passed:
            failures += 1
            continue

        log.info('%s correctly configured', tool.name)

    import sys
    sys.exit(failures)
