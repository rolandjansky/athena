# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.Enums import Format
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulationOldStyleCfg, RoiB2TopoInputDataCnv
from TrigConfIO.L1TriggerConfigAccess import L1MenuAccess
from TrigConfigSvc.TrigConfigSvcCfg   import getL1MenuFileName
from libpyeformat_helper import SourceIdentifier, SubDetector

def getL1TopoOnlineMonitorHypo(flags):
    hypo = CompFactory.L1TopoOnlineMonitorHypo()
    return hypo

def L1TopoOnlineMonitorHypoToolGen(chainDict):
    tool = CompFactory.L1TopoOnlineMonitorHypoTool(chainDict['chainName'])

    isLegacy = False
    for chainPart in chainDict['chainParts']:
        if 'isLegacyL1' in chainPart and 'legacy' in chainPart['isLegacyL1']:
            isLegacy = True
            break

    if isLegacy:
        tool.ErrorFlagsKey = 'L1TopoErrorFlags_Legacy'
    else:
        tool.ErrorFlagsKey = 'L1TopoErrorFlags'

    # Select error flags to accept events
    tool.AcceptOnGenericRoiError = True
    tool.AcceptOnGenericDaqError = True
    tool.AcceptOnCrcTobError = True
    tool.AcceptOnCrcFibreError = True
    tool.AcceptOnCrcDaqError = True
    tool.AcceptOnRoibDaqDifference = True
    tool.AcceptOnRoibCtpDifference = True
    tool.AcceptOnDaqCtpDifference = True

    return tool

def getL1TopoPhase1OnlineMonitor(flags):
    # Placeholder for phase-1 implementation
    raise RuntimeError('L1Topo phase-1 online monitoring not yet implemented')

def getL1TopoLegacyOnlineMonitor(flags):
    alg = CompFactory.L1TopoLegacyOnlineMonitor()
    alg.MonTool = GenericMonitoringTool('MonTool')
    configureLegacyHistograms(alg, flags)

    def getAlgProp(prop):
        return getattr(alg, prop) if hasattr(alg, prop) \
               else alg.getDefaultProperty(prop)

    # Commented out because TrigEDMConfig.TriggerEDMRun3.recordable() currently enforces
    # that all names start with HLT_ which doesn't make sense for L1TopoErrorFlags
    # alg.ErrorFlagsKey = recordable(str(getAlgProp('ErrorFlagsKey')))

    if getAlgProp('PrescaleDAQROBAccess') >= 1 and flags.Input.Format is Format.BS:
        # Add BS converter for DAQ L1Topo RDO collection
        from L1TopoByteStream.L1TopoByteStreamConfig import L1TopoRDOCollectionBSCnvCfg
        CAtoGlobalWrapper(L1TopoRDOCollectionBSCnvCfg, ConfigFlags)
        if getAlgProp('doSimMon') and not flags.Trigger.doLVL1:
            # Add BS converter for CTP_RDO
            from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
            CAtoGlobalWrapper(ByteStreamReadCfg, ConfigFlags, type_names=['CTP_RDO/CTP_RDO'])

    return alg

def configureLegacyHistograms(alg, flags):
    '''All histogram definitions for L1TopoLegacyOnlineMonitor'''
    # ==========================================================================
    rob_src_id_labels = []
    for module_id in alg.DAQROBModuleIds + alg.RoIBROBModuleIds:
        sid = SourceIdentifier(SubDetector.TDAQ_CALO_TOPO_PROC, module_id)
        rob_src_id_labels.append('{:#010x}'.format(sid.code()))

    alg.MonTool.defineHistogram('ROB_src_IDs_fromROB', path='EXPERT', type='TH1F',
                                title='L1Topo ROB source IDs received direct from ROBs;ROB ID;Entries',
                                xlabels=rob_src_id_labels, xbins=len(rob_src_id_labels),
                                xmin=0, xmax=len(rob_src_id_labels))
    alg.MonTool.defineHistogram('ROB_src_IDs_fromCnv', path='EXPERT', type='TH1F',
                                title='L1Topo ROB source IDs received via converters;ROB ID;Entries',
                                xlabels=rob_src_id_labels, xbins=len(rob_src_id_labels),
                                xmin=0, xmax=len(rob_src_id_labels))
    # ==========================================================================
    tob_type_labels = ['EM','TAU','MU','0x3','JETc1','JETc2','ENERGY','0x7',
                       'L1TOPO','0x9','0xa','0xb','HEADER','FIBRE','STATUS','0xf']
    alg.MonTool.defineHistogram('TOBtype_fromROIROB', path='EXPERT', type='TH1F',
                                title='4-bit TOB type via ROI ROB;TOB type;Entries',
                                xlabels=tob_type_labels, xbins=len(tob_type_labels),
                                xmin=0, xmax=len(tob_type_labels))
    alg.MonTool.defineHistogram('TOBtype_fromDAQROB', path='EXPERT', type='TH1F',
                                title='4-bit TOB type via DAQ ROB;TOB type;Entries',
                                xlabels=tob_type_labels, xbins=len(tob_type_labels),
                                xmin=0, xmax=len(tob_type_labels))
    # ==========================================================================
    alg.MonTool.defineHistogram('ROI_ROB_payload_size', path='EXPERT', type='TH1F',
                                title='L1Topo ROI ROB payload size;number of words;Entries',
                                xbins=300, xmin=0, xmax=300)
    alg.MonTool.defineHistogram('DAQ_ROB_payload_size', path='EXPERT', type='TH1F',
                                title='L1Topo DAQ ROB payload size;number of words;Entries',
                                xbins=300, xmin=0, xmax=300)
    # ==========================================================================
    alg.MonTool.defineHistogram('CTP_sig_part_fromROICnv', path='EXPERT', type='TH1F',
                                title='4-bit CTP signal part from ROI via converter;CTP signal part;Entries',
                                xbins=16, xmin=0, xmax=16)
    # ==========================================================================
    input_link_crc_labels = ['EM', 'Tau', 'Muon', 'Jet', 'Energy']
    alg.MonTool.defineHistogram('InputLinkCRCs', path='EXPERT', type='TH1F',
                                title='CRC flags for input links, from ROI via converter;;Entries',
                                xlabels=input_link_crc_labels, xbins=len(input_link_crc_labels),
                                xmin=0, xmax=len(input_link_crc_labels))
    # ==========================================================================
    topo_trigline_labels = [str(i) for i in range(128)]
    lvl1name = getL1MenuFileName(flags)
    lvl1access  = L1MenuAccess(lvl1name)
    connectors = {0: 'LegacyTopo0', 1: 'LegacyTopo1'}
    for connector_id, connectorKey in connectors.items():
        topo_triglines_dict = lvl1access.connector(connectorKey)['triggerlines']
        if not isinstance(topo_triglines_dict, list):
            for fpga_id in [0,1]:
                topo_fpga = topo_triglines_dict['fpga{:d}'.format(fpga_id)]
                for clock_id in [0,1]:
                    topo_clock = topo_fpga['clock{:d}'.format(clock_id)]
                    for topo_trigline in topo_clock:
                        topo_trigline_name = topo_trigline['name']
                        bit_id = topo_trigline['startbit']
                        topo_trigline_index = 64*connector_id + 32*fpga_id + 2*bit_id + clock_id
                        topo_trigline_labels[topo_trigline_index] = topo_trigline_name
        else:
            for topo_trigline in topo_triglines_dict:
                topo_trigline_name = topo_trigline['name']
                bit_id = topo_trigline['startbit']
                fpga_id = topo_trigline['fpga']
                clock_id = topo_trigline['clock']
                topo_trigline_index = 64*connector_id + 32*fpga_id + 2*bit_id + clock_id
                topo_trigline_labels[topo_trigline_index] = topo_trigline_name

    def defineBitsHistogram(var, title):
        alg.MonTool.defineHistogram(var, path='EXPERT', type='TH1F', title=title+';;Entries',
                                    xlabels=topo_trigline_labels, xbins=len(topo_trigline_labels),
                                    xmin=0, xmax=len(topo_trigline_labels))

    defineBitsHistogram('CTP_Trigger_fromROICnv', 'L1Topo CTP signal trigger bits from ROI via converter')
    defineBitsHistogram('CTP_Overflow_fromROICnv', 'L1Topo CTP signal overflow bits from ROI via converter')
    defineBitsHistogram('SimResults', 'L1Topo simulation accepts, events with no overflows')
    defineBitsHistogram('HdwResults', 'L1Topo hardware accepts, events with no overflows')
    defineBitsHistogram('SimNotHdwResult', 'L1Topo events with simulation accept and hardware fail, events with no overflows')
    defineBitsHistogram('HdwNotSimResult', 'L1Topo events with hardware accept and simulation fail, events with no overflows')
    defineBitsHistogram('Hdw_vs_Sim_Events', 'L1Topo decisions hardware XOR simulation event-by-event differences, events with no overflows')
    defineBitsHistogram('SimDaqRobResults', 'L1Topo simulation accepts, events with no overflows (DAQ ROB)')
    defineBitsHistogram('HdwDaqRobResults', 'L1Topo hardware accepts, events with no overflows (DAQ ROB)')
    defineBitsHistogram('SimNotHdwDaqRobResult', 'L1Topo events with simulation accept and hardware fail, events with no overflows (DAQ ROB)')
    defineBitsHistogram('HdwNotSimDaqRobResult', 'L1Topo events with hardware accept and simulation fail, events with no overflows (DAQ ROB)')
    defineBitsHistogram('SimOverflows', 'L1Topo simulation overflows')
    defineBitsHistogram('HdwOverflows', 'L1Topo hardware overflows')
    defineBitsHistogram('SimNotHdwOverflow', 'L1Topo events with overflow simulation=1 and hardware=0')
    defineBitsHistogram('HdwNotSimOverflow', 'L1Topo events with overflow hardware=1 and simulation=0')
    defineBitsHistogram('Hdw_vs_Sim_EventOverflow', 'L1Topo overflow hardware XOR simulation event-by-event differences')
    defineBitsHistogram('DAQ_ROB_Hdw_vs_L1Topo_Hdw_Events', 'L1Topo decisions hardware XOR DAQ ROB hardware event-by-event differences')
    defineBitsHistogram('DAQ_ROB_Hdw_vs_CTP_Hdw_Events', 'L1Topo DAQ ROB hardware XOR CTP TIP hardware event-by-event differences')
    defineBitsHistogram('CTP_Hdw_vs_L1Topo_Hdw_Events', 'L1Topo decisions hardware (trigger|overflow) XOR CTP TIP hardware event-by-event differences, events with no overflows')
    defineBitsHistogram('CTP_Hdw_vs_Sim_Events', 'L1Topo decisions CTP hardware XOR simulation event-by-event differences, events with no overflows')
    # ==========================================================================
    alg.MonTool.defineHistogram('CRC_fromDAQCnv', path='EXPERT', type='TH1F',
                                title='non zero payload CRCs via converter;Payload CRC;Entries',
                                xbins=256, xmin=0, xmax=256)  # CRC is 8 bits
    # ==========================================================================
    alg.MonTool.defineHistogram('NonZero_FibreStatus_fromDAQCnv', path='EXPERT', type='TH1F',
                                title='L1Topo Non-zero Fibre status flags from DAQ via converter;fibre status flags;Entries',
                                xbins=85, xmin=0, xmax=85, opt='kVec')
    # ==========================================================================
    alg.MonTool.defineHistogram('DAQ_ROB_rel_bx_fromCnv', path='EXPERT', type='TH1F',
                                title='L1Topo DAQ ROB relative bunch crossings sent via converter;relative bunch crossing;Entries',
                                xbins=10, xmin=-5, xmax=5)
    # ==========================================================================
    problem_labels = ['ROI no RDO', 'ROI converter error', 'ROI payload empty', 'ROI invalid TOB type',
                      'DAQ no RDO', 'DAC collection empty', 'DAQ payload empty', 'DAQ invalid TOB type',
                      'Fibre overflow', 'Fibre CRC']
    alg.MonTool.defineHistogram('Problems', path='EXPERT', type='TH1F',
                                title='Counts of various problems;;Entries',
                                xlabels=problem_labels, xbins=len(problem_labels),
                                xmin=0, xmax=len(problem_labels))
    # ==========================================================================
    mon_failure_labels = ['doRawMon', 'doCnvMon', 'doSimMon', 'doOverflowSimMon', 'doSimDaq']
    alg.MonTool.defineHistogram('MonitoringFailures', path='EXPERT', type='TH1F',
                                title='Counts of mon functions returning failure;;Entries',
                                xlabels=mon_failure_labels, xbins=len(mon_failure_labels),
                                xmin=0, xmax=len(mon_failure_labels))
    # ==========================================================================

def getL1TopoPhase1SimForOnlineMonitor(flags):
    return [L1TopoSimulationOldStyleCfg(flags, isLegacy=False)]

def getL1TopoLegacySimForOnlineMonitor(flags):
    return [RoiB2TopoInputDataCnv(),
            L1TopoSimulationOldStyleCfg(flags, isLegacy=True)]
