# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg, HLTConfigSvcCfg, L1PrescaleCondAlgCfg, HLTPrescaleCondAlgCfg
from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerRecoConfig')


def TriggerRecoCfg(flags):
    """ Configures trigger data decoding
    Run 3 data:
    HLTResultMTByteStreamDecoderAlg -> TriggerEDMDeserialiserAlg

    Run 2 data:
    TrigBSExtraction -> TrigDecisionMaker -> DecisionConv to xAOD -> NavigationConv to xAOD

    Run 1 data:
    as for Run 2 + Run 1 EDM to xAOD conversion
    """
    acc = ComponentAccumulator()
    acc.merge( ByteStreamReadCfg(flags) )


    acc.merge( L1TriggerByteStreamDecoderCfg(flags) )
    acc.merge( L1ConfigSvcCfg(flags) )
    acc.merge( HLTConfigSvcCfg(flags) )
    acc.merge( L1PrescaleCondAlgCfg(flags) )
    acc.merge( HLTPrescaleCondAlgCfg(flags) )

    if flags.Trigger.EDMVersion >= 3:
        acc.merge(Run3TriggerBSUnpackingCfg(flags))
        from TrigDecisionMaker.TrigDecisionMakerConfig import Run3DecisionMakerCfg
        acc.merge(Run3DecisionMakerCfg(flags))
        if flags.Trigger.doNavigationSlimming:
            from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import TrigNavSlimmingMTCfg
            acc.merge(TrigNavSlimmingMTCfg(flags))
    elif flags.Trigger.EDMVersion in [1, 2]:
        if flags.Trigger.EDMVersion == 1:
            acc.merge(Run1xAODConversionCfg(flags))
        acc.merge( Run1Run2BSExtractionCfg(flags) )
        from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
        xRoIBResultAcc, _ = RoIBResultToxAODCfg(flags)
        acc.merge( xRoIBResultAcc )
        from TrigDecisionMaker.TrigDecisionMakerConfig import Run1Run2DecisionMakerCfg
        acc.merge (Run1Run2DecisionMakerCfg(flags) )
        menuwriter = CompFactory.TrigConf.xAODMenuWriterMT()
        menuwriter.KeyWriterTool = CompFactory.TrigConf.KeyWriterTool("KeyWriterToolOffline")
        acc.addEventAlgo( menuwriter )
        if flags.Trigger.doNavigationSlimming:
            acc.merge(Run2Run1NavigationSlimingCfg(flags))

    else:
        raise RuntimeError("Invalid EDMVersion=%s " % flags.Trigger.EDMVersion)
    if flags.Output.doWriteESD  or flags.Output.doWriteAOD:
        acc.merge(TriggerEDMCfg(flags))

    return acc


def TriggerEDMCfg(flags):
    """Configures which trigger collections are recorded"""
    acc = ComponentAccumulator()
    # standard collections & metadata
    # TODO consider unifying with TriggerConfig.triggerPOOLOutputCfg - there the assumption is that Run3 
    # metadata
    menuMetadata = [ "xAOD::TriggerMenuJsonContainer#*", 
                                                "xAOD::TriggerMenuJsonAuxContainer#*" ]
    if flags.Trigger.EDMVersion in [1,2]:
        menuMetadata += ['xAOD::TriggerMenuAuxContainer#*', 'xAOD::TriggerMenuContainer#*']

    # EDM
    def _asList(edm):
        return [ f"{type}#{name}" for type, names in edm.items() for name in names ]
    _TriggerESDList = getTriggerEDMList(flags.Trigger.ESDEDMSet,  flags.Trigger.EDMVersion)
    _TriggerAODList = getTriggerEDMList(flags.Trigger.AODEDMSet,  flags.Trigger.EDMVersion)
    log.debug("ESD EDM list: %s", _TriggerESDList)
    log.debug("AOD EDM list: %s", _TriggerAODList)
    
    # Highlight what is in AOD list but not in ESD list, as this can cause
    # the "different number of entries in branch" problem, when it is in the
    # AOD list but the empty container per event is not created
    # Just compares keys of dicts, which are the class names, not their string keys in StoreGate
    not_in = [ element for element in  _TriggerAODList if element not in _TriggerESDList ]
    if (len(not_in)>0):
        log.warning("In AOD list but not in ESD list: ")
        log.warning(not_in)
    else:
        log.info("AOD list is subset of ESD list - good.")

    # there is internal gating  in addTo* if AOD or ESD do not need to be written out
    acc.merge(addToESD(flags, _asList(_TriggerESDList), MetadataItemList=menuMetadata))
    acc.merge(addToAOD(flags, _asList(_TriggerAODList), MetadataItemList=menuMetadata))
    
    log.info("AOD content set according to the AODEDMSet flag: %s and EDM version %d", flags.Trigger.AODEDMSet, flags.Trigger.EDMVersion)
    # navigation for Run 3
    if flags.Trigger.EDMVersion == 3 and not flags.Trigger.doOnlineNavigationCompactification and not flags.Trigger.doNavigationSlimming:
        nav = ['xAOD::TrigCompositeContainer#HLTNav*', 'xAOD::TrigCompositeAuxContainer#HLTNav*']
        acc.merge(addToAOD(flags, nav))
        acc.merge(addToESD(flags, nav))
    # extra jet keys
    jetSpecials = ["JetKeyDescriptor#JetKeyMap", "JetMomentMap#TrigJetRecMomentMap"]
    acc.merge(addToESD(flags, jetSpecials))
    acc.merge(addToAOD(flags, jetSpecials))

    # RoIs
    if flags.Output.doWriteAOD and flags.Trigger.EDMVersion == 2:
        roiWriter = CompFactory.RoiWriter( ExtraInputs = [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]) 
        acc.addEventAlgo(roiWriter)

        from TrigEDMConfig.TriggerEDMRun2 import TriggerRoiList
        acc.merge(addToAOD(flags, TriggerRoiList))

    return acc

def Run2Run1NavigationSlimingCfg(flags):
    """Configures legacy Run1/2 navigation slimming"""
    acc = ComponentAccumulator()
    def _flatten(edm):
        return list(y.split('-')[0] for x in edm.values() for y in x)
    from TrigNavTools.TrigNavToolsConfig import TrigNavigationThinningSvcCfg
    

    if flags.Output.doWriteAOD:
        _TriggerAODList = getTriggerEDMList(flags.Trigger.AODEDMSet,  flags.Trigger.EDMVersion)
        thinningSvc = acc.getPrimaryAndMerge(TrigNavigationThinningSvcCfg(flags, {'name':'HLTNav_StreamAOD', 'mode':'cleanup_noreload', 
                                          'result':'HLTResult_HLT',
                                          'features':_flatten(_TriggerAODList)}))
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "AOD", trigNavThinningSvc=thinningSvc))

    if flags.Output.doWriteESD:
        _TriggerESDList = getTriggerEDMList(flags.Trigger.ESDEDMSet,  flags.Trigger.EDMVersion)
        thinningSvc = acc.getPrimaryAndMerge(TrigNavigationThinningSvcCfg(flags, {'name':'HLTNav_StreamESD', 'mode':'cleanup_noreload', 
                                          'result':'HLTResult_HLT',
                                          'features':_flatten(_TriggerESDList)}))
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "ESD", trigNavThinningSvc=thinningSvc))

    return acc


def Run1Run2BSExtractionCfg( flags ):
    """Configures Trigger data from BS extraction """
    acc = ComponentAccumulator()
    extr = CompFactory.TrigBSExtraction()

    # Add fictional output to ensure data dependency in AthenaMT
    # Keeping the run 2 workflow, we run this after we have put the full serialised navigation into xAOD
    extr.ExtraInputs += [("xAOD::TrigNavigation", "StoreGateSvc+TrigNavigation")]
    extr.ExtraOutputs += [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]
    if 'HLT' in flags.Trigger.availableRecoMetadata:
        serialiserTool = CompFactory.TrigTSerializer()
        acc.addPublicTool(serialiserTool)
        extr.NavigationForL2 = CompFactory.HLT.Navigation("NavigationForL2", 
                                                          ClassesFromPayloadIgnore = ["TrigPassBits#passbits"]) # Ignore the L2 TrigPassBits to avoid clash with EF (ATR-23411)

        extr.Navigation = CompFactory.HLT.Navigation("Navigation")
        from TrigEDMConfig.TriggerEDM import getEDMLibraries
        extr.Navigation.Dlls = getEDMLibraries()            
        from TrigEDMConfig.TriggerEDM import getPreregistrationList
        extr.Navigation.ClassesToPreregister = getPreregistrationList(flags.Trigger.EDMVersion)
        from eformat import helper as efh
        robIDMap = {}   # map of result keys and their ROB ID
        if flags.Trigger.EDMVersion == 1:  # Run-1 has L2 and EF result
            robIDMap["HLTResult_L2"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_LVL2, 0).code()
            robIDMap["HLTResult_EF"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_EVENT_FILTER, 0).code()
            extr.L2ResultKey = "HLTResult_L2"
            extr.HLTResultKey = "HLTResult_EF"
        else:
            robIDMap["HLTResult_HLT"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_HLT, 0).code()
            extr.L2ResultKey = ""
            extr.HLTResultKey = "HLTResult_HLT"
        # Configure DataScouting
        from PyUtils.MetaReaderPeeker import metadata
        if 'stream' in metadata:
            stream_local = metadata['stream']   # e.g. calibration_DataScouting_05_Jets
            if stream_local.startswith('calibration_DataScouting_'):
                ds_tag = '_'.join(stream_local.split('_')[1:3])   # e.g. DataScouting_05
                ds_id = int(stream_local.split('_')[2])           # e.g. 05
                robIDMap[ds_tag] = efh.SourceIdentifier(efh.SubDetector.TDAQ_HLT, ds_id).code()
                extr.DSResultKeys += [ds_tag]
    else:
        log.info("Will not schedule real HLT bytestream extraction, instead EDM gap filling is running")
        # if data doesn't have HLT info set HLTResult keys as empty strings to avoid warnings
        # but the extraction algorithm must run
        extr.L2ResultKey = ""
        extr.HLTResultKey = ""
        extr.DSResultKeys = []

    HLTResults = [ f"HLT::HLTResult/{k}" for k in robIDMap.keys() ]
    acc.addService( CompFactory.ByteStreamAddressProviderSvc( TypeNames = HLTResults) )


    from TrigEDMConfig.TriggerEDM import getTPList
    acc.addPublicTool( CompFactory.TrigSerTPTool(TPMap = getTPList((flags.Trigger.EDMVersion))) )
    
    acc.addPublicTool( CompFactory.TrigSerializeConvHelper(doTP = True) )

    acc.addPublicTool( CompFactory.HLT.HLTResultByteStreamTool(HLTResultRobIdMap = robIDMap))

    acc.addEventAlgo(extr)

    return acc

def Run1xAODConversionCfg(flags):
    """Convert Run 1 EDM collections to xAOD classes"""
    acc = ComponentAccumulator()

    log.info("Will configure Run 1 trigger EDM to xAOD conversion")
    from TrigEDMConfig.TriggerEDM import getPreregistrationList
    from TrigEDMConfig.TriggerEDM import getEFRun1BSList,getEFRun2EquivalentList,getL2Run1BSList,getL2Run2EquivalentList

    # define list of HLT xAOD containers to be written to the output root file
    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    edm = getTriggerEDMList(flags.Trigger.ESDEDMSet, 2 )


    navTool = CompFactory.HLT.Navigation("Navigation", 
                                         ClassesToPreregister = getPreregistrationList(flags.Trigger.EDMVersion) )

    bstoxaodTool = CompFactory.TrigBStoxAODTool("BStoxAOD", 
                                                ContainersToConvert = getL2Run1BSList() + getEFRun1BSList(), 
                                                NewContainers = getL2Run2EquivalentList() + getEFRun2EquivalentList() )

    xaodConverter = CompFactory.TrigHLTtoxAODConversion( ExtraInputs = [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")] if flags.Trigger.readBS else [],
                                                         Navigation = navTool,
                                                         BStoxAOD = bstoxaodTool,
                                                         HLTResultKey="HLTResult_EF")

    acc.addEventAlgo(xaodConverter)

    acc.merge(addToESD(flags, edm))
    acc.merge(addToAOD(flags, edm))

    return acc

def Run3TriggerBSUnpackingCfg(flags):
    """Configures conversions BS -> HLTResultMT -> Collections """
    acc = ComponentAccumulator()
    from AthenaCommon.CFElements import seqAND
    decoder = CompFactory.HLTResultMTByteStreamDecoderAlg()
    deserialiser = CompFactory.TriggerEDMDeserialiserAlg("TrigDeserialiser")
    from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
    deserialiser.ExtraOutputs += [('xAOD::TrigCompositeContainer' , 'StoreGateSvc+'+getRun3NavigationContainerFromInput(flags))]
    acc.addSequence(seqAND("HLTDecodingSeq"))
    acc.addEventAlgo( decoder, "HLTDecodingSeq")
    acc.addEventAlgo( deserialiser, "HLTDecodingSeq")
    log.debug("Configured HLT result BS decoding sequence")
    return acc


if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    flags.fillFromArgs()

    log.info('Checking setup for EDMVersion %d', flags.Trigger.EDMVersion)
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW # need to update this depending on EDMversion
    flags.Exec.MaxEvents=5

    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge( TriggerRecoCfg(flags) )
    acc.printConfig(withDetails=True)
    with open("TriggerReco.pkl", "wb") as file:
        acc.store(file)
    # TODO decide if we want to run actually
    # sc = acc.run()
    # if sc.isFailure():
    #     import sys
    #     sys.exit(-1)

