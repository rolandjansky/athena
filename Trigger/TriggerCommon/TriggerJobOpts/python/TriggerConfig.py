# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.CFElements import seqAND, seqOR, flatAlgorithmSequences
from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerConfig')

def collectHypos( steps ):
    """
    Method iterating over the CF and picking all the Hypothesis algorithms

    Returned is a map with the step name and list of all instances of hypos in that step.
    Input is top HLT sequencer.
    """
    __log.info("Collecting hypos")
    from collections import defaultdict
    hypos = defaultdict( list )

    for stepSeq in steps.getChildren():
        if "filter" in stepSeq.name():
            __log.info("Skipping filtering steps " +stepSeq.name() )
            continue

        __log.info( "collecting hypos from step " + stepSeq.name() )
#        start = {}
        for seq,algs in flatAlgorithmSequences( stepSeq ).iteritems():

            for alg in algs:
                # will replace by function once dependencies are sorted
                if 'HypoInputDecisions'  in alg.getProperties():
                    __log.info( "found hypo " + alg.name() + " in " +stepSeq.name() )
                    hypos[stepSeq.name()].append( alg )
                else:
                    __log.verbose("Not a hypo" + alg.name())

    return hypos

def __decisionsFromHypo( hypo ):
    """ return all chains served by this hypo and the key of produced decision object """
    from DecisionHandling.TrigCompositeUtils import isLegId
    if hypo.getType() == 'ComboHypo':
        return [key for key in hypo.MultiplicitiesMap.keys() if not isLegId(key)], hypo.HypoOutputDecisions[0]
    else: # regular hypos
        return [ t.name() for t in hypo.HypoTools if not isLegId(t.name())], hypo.HypoOutputDecisions


def collectViewMakers( steps ):
    """ collect all view maker algorithms in the configuration """
    makers = set() # map with name, instance and encompasing recoSequence
    for stepSeq in steps.getChildren():
        for recoSeq in stepSeq.getChildren():
            algsInSeq = flatAlgorithmSequences( recoSeq )
            for seq,algs in algsInSeq.iteritems():
                for alg in algs:
                    if "EventViewCreator" in alg.getFullName(): # TODO base it on checking types of write handles once available
                        makers.add(alg)
    __log.info("Found View Makers: {}".format( ' '.join([ maker.name() for maker in makers ]) ))
    return list(makers)



def collectFilters( steps ):
    """
    Similarly to collectHypos but works for filter algorithms

    The logic is simpler as all filters are grouped in step filter sequences
    Returns map: step name -> list of all filters of that step
    """
    __log.info("Collecting filters")
    from collections import defaultdict
    filters = defaultdict( list )

    for stepSeq in steps.getChildren():
        if "filter" in stepSeq.name():
            filters[stepSeq.name()] = stepSeq.getChildren()
            __log.info("Found Filters in Step {} : {}".format(stepSeq.name(), stepSeq.getChildren()))

    return filters


def collectL1DecoderDecisionObjects(l1decoder):
    decisionObjects = set()
    decisionObjects.update([ d.Decisions for d in l1decoder.roiUnpackers ])
    decisionObjects.update([ d.Decisions for d in l1decoder.rerunRoiUnpackers ])
    from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection
    decisionObjects.add( mapThresholdToL1DecisionCollection("FS") ) # Include also Full Scan
    __log.info("Collecting %i decision objects from L1 decoder instance", len(decisionObjects))
    return decisionObjects

def collectHypoDecisionObjects(hypos, inputs = True, outputs = True):
    decisionObjects = set()
    for step, stepHypos in hypos.iteritems():
        for hypoAlg in stepHypos:
            __log.debug( "Hypo %s with input %s and output %s ",
                         hypoAlg.getName(), hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions )
            if isinstance( hypoAlg.HypoInputDecisions, list):
                if inputs:
                    [ decisionObjects.add( d ) for d in hypoAlg.HypoInputDecisions ]
                if outputs:
                    [ decisionObjects.add( d ) for d in hypoAlg.HypoOutputDecisions ]
            else:
                if inputs:
                    decisionObjects.add( hypoAlg.HypoInputDecisions )
                if outputs:
                    decisionObjects.add( hypoAlg.HypoOutputDecisions )
    __log.info("Collecting %i decision objects from hypos", len(decisionObjects))
    return decisionObjects

def collectFilterDecisionObjects(filters, inputs = True, outputs = True):
    decisionObjects = set()
    for step, stepFilters in filters.iteritems():
        for filt in stepFilters:
            if inputs:
                decisionObjects.update( filt.Input )
            if outputs:
                decisionObjects.update( filt.Output )
    __log.info("Collecting %i decision objects from filters", len(decisionObjects))
    return decisionObjects

def collectHLTSummaryDecisionObjects(hltSummary):
    decisionObjects = set()
    decisionObjects.add( hltSummary.DecisionsSummaryKey )
    __log.info("Collecting %i decision objects from hltSummary", len(decisionObjects))
    return decisionObjects

def collectDecisionObjects(  hypos, filters, l1decoder, hltSummary ):
    """
    Returns the set of all decision objects of HLT
    """
    decObjL1 = collectL1DecoderDecisionObjects(l1decoder)
    decObjHypo = collectHypoDecisionObjects(hypos, inputs = True, outputs = True)
    decObjFilter = collectFilterDecisionObjects(filters, inputs = True, outputs = True)
    # InputMaker are not needed explicitly as the Filter Outputs = InputMaker Inputs
    # and InputMaker Outputs = Hypo Inputs
    # Therefore we implicitly collect all navigaiton I/O of all InputMakers
    decObjSummary = collectHLTSummaryDecisionObjects(hltSummary)
    decisionObjects = set()
    decisionObjects.update(decObjL1)
    decisionObjects.update(decObjHypo)
    decisionObjects.update(decObjFilter)
    decisionObjects.update(decObjSummary)
    return decisionObjects

def triggerSummaryCfg(flags, hypos):
    """
    Configures an algorithm(s) that should be run after the selection process
    Returns: ca, algorithm
    """
    acc = ComponentAccumulator()
    DecisionSummaryMakerAlg=CompFactory.DecisionSummaryMakerAlg
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    decisionSummaryAlg = DecisionSummaryMakerAlg()
    allChains = {}


    for stepName, stepHypos in sorted( hypos.items() ):
        for hypo in stepHypos:
            hypoChains,hypoOutputKey = __decisionsFromHypo( hypo )
            allChains.update( dict.fromkeys( hypoChains, hypoOutputKey ) )

    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection
    if len(TriggerConfigHLT.dicts()) == 0:
        __log.warning("No HLT menu, chains w/o algorithms are not handled")
    else:
        for chainName, chainDict in TriggerConfigHLT.dicts().iteritems():
            if chainName not in allChains:
                __log.warn("The chain %s is not mentiond in any step", chainName)
                # TODO once sequences available in the menu we need to crosscheck it here
                assert len(chainDict['chainParts'])  == 1, "Chains w/o the steps can not have mutiple parts in chainDict, it makes no sense: %s"%chainName
                allChains[chainName] = mapThresholdToL1DecisionCollection( chainDict['chainParts'][0]['L1threshold'] )
                __log.info("The chain %s final decisions will be taken from %s", chainName, allChains[chainName] )

    for c, cont in allChains.iteritems():
        __log.info("Final decision of chain  " + c + " will be read from " + cont )
    decisionSummaryAlg.FinalDecisionKeys = list(set(allChains.values()))
    decisionSummaryAlg.FinalStepDecisions = allChains
    decisionSummaryAlg.DecisionsSummaryKey = "HLTNav_Summary" # Output
    decisionSummaryAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decisionSummaryAlg.CostWriteHandleKey = recordable(flags.Trigger.CostMonitoring.outputCollection)
    return acc, decisionSummaryAlg


def triggerMonitoringCfg(flags, hypos, filters, l1Decoder):
    """
    Configures components needed for monitoring chains
    """
    acc = ComponentAccumulator()
    TrigSignatureMoniMT, DecisionCollectorTool=CompFactory.getComps("TrigSignatureMoniMT","DecisionCollectorTool",)
    mon = TrigSignatureMoniMT()
    mon.L1Decisions = "L1DecoderSummary"
    mon.FinalDecisionKey = "HLTNav_Summary" # Input
    if len(hypos) == 0:
        __log.warning("Menu is not configured")
        return acc, mon
    allChains = set() # collects the last decision obj for each chain

    for stepName, stepHypos in sorted( hypos.items() ):
        stepDecisionKeys = []
        for hypo in stepHypos:
            hypoChains, hypoOutputKey  = __decisionsFromHypo( hypo )
            stepDecisionKeys.append( hypoOutputKey )
            allChains.update( hypoChains )

        dcTool = DecisionCollectorTool( "DecisionCollector" + stepName, Decisions=list(set(stepDecisionKeys)) )
        __log.info( "The step monitoring decisions in " + dcTool.name() + " " +str( dcTool.Decisions ) )
        mon.CollectorTools += [ dcTool ]


    #mon.FinalChainStep = allChains
    mon.L1Decisions  = l1Decoder.getProperties()['L1DecoderSummaryKey'] if l1Decoder.getProperties()['L1DecoderSummaryKey'] != '<no value>' else l1Decoder.getDefaultProperty('L1DecoderSummary')

    from DecisionHandling.DecisionHandlingConfig import setupFilterMonitoring
    [ [ setupFilterMonitoring( alg ) for alg in algs ]  for algs in filters.values() ]


    return acc, mon



def triggerOutputCfg(flags, decObj, decObjHypoOut, summaryAlg):
    # Following cases are considered:
    # 1) Running in partition or athenaHLT - configure BS output written by the HLT framework
    # 2) Running offline athena and writing BS - configure BS output written by OutputStream alg
    # 3) Running offline athena with POOL output - configure POOL output written by OutputStream alg
    onlineWriteBS = False
    offlineWriteBS = False
    writePOOL = False

    isPartition = len(flags.Trigger.Online.partitionName) > 0
    if flags.Trigger.writeBS:
        if isPartition:
            onlineWriteBS = True
        else:
            offlineWriteBS = True
    if flags.Output.doWriteRDO or flags.Output.doWriteESD or flags.Output.doWriteAOD:
        writePOOL = True

    # Consistency checks
    if offlineWriteBS and not flags.Output.doWriteBS:
        __log.error('flags.Trigger.writeBS is True but flags.Output.doWriteBS is False')
        return None, ''
    if writePOOL and onlineWriteBS:
        __log.error("POOL HLT output writing is configured online")
        return None, ''
    if writePOOL and offlineWriteBS:
        __log.error("Writing HLT output to both BS and POOL in one job is not supported at the moment")
        return None, ''

    # Determine EDM set name
    edmSet = ''
    if writePOOL:
        edmSet = flags.Trigger.AODEDMSet if flags.Output.doWriteAOD else flags.Trigger.ESDEDMSet
    elif onlineWriteBS or offlineWriteBS:
        edmSet = 'BS'

    # Create the configuration
    if onlineWriteBS:
        __log.info("Configuring online ByteStream HLT output")
        acc = triggerBSOutputCfg(flags, decObj, decObjHypoOut, summaryAlg)
        # Configure the online HLT result maker to use the above tools
        # For now use old svcMgr interface as this service is not available from acc.getService()
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        hltEventLoopMgr = svcMgr.HltEventLoopMgr
        hltEventLoopMgr.ResultMaker.MakerTools = acc.popPrivateTools()
    elif offlineWriteBS:
        __log.info("Configuring offline ByteStream HLT output")
        acc = triggerBSOutputCfg(flags, decObj, decObjHypoOut, summaryAlg, offline=True)
    elif writePOOL:
        __log.info("Configuring POOL HLT output")
        acc = triggerPOOLOutputCfg(flags, decObj, decObjHypoOut, edmSet)
    else:
        __log.info("No HLT output writing is configured")
        acc = ComponentAccumulator()

    return acc, edmSet


def triggerBSOutputCfg(flags, decObj, decObjHypoOut, summaryAlg, offline=False):
    """
    Returns CA with algorithms and/or tools required to do the serialisation

    decObj - list of all naviagtaion objects
    decObjHypoOut - list of decisions produced by hypos
    summaryAlg - the instance of algorithm producing final decision
    offline - if true CA contains algorithms that needs to be merged to output stream sequence,
              if false the CA contains a tool that needs to be added to HLT EventLoopMgr
    """
    from TriggerMenuMT.HLTMenuConfig.Menu import EventBuildingInfo
    from TrigEDMConfig.TriggerEDM import getRun3BSList

    # handle the collectiosn defined in the EDM config
    collectionsToBS = getRun3BSList( ["BS"]+ EventBuildingInfo.DataScoutingIdentifiers.keys() )

    
    from collections import OrderedDict
    ItemModuleDict = OrderedDict()
    for typekey, bsfragments in collectionsToBS:
        # translate readable frament names like BS, CostMonDS names to ROB fragment IDs 0 - for the BS, 1,...- for DS fragments
        moduleIDs = [ EventBuildingInfo.getFullHLTResultID() if f == 'BS' else EventBuildingInfo.getDataScoutingResultID(f)
                      for f in bsfragments ]
        ItemModuleDict[typekey] = moduleIDs

    # Add decision containers (navigation)
    for item in decObj:
        dynamic = '.-' # Exclude dynamic
        if item in decObjHypoOut:
            dynamic = '.' # Include dynamic
        typeName = 'xAOD::TrigCompositeContainer#{:s}'.format(item)
        typeNameAux = 'xAOD::TrigCompositeAuxContainer#{:s}Aux{:s}'.format(item, dynamic)
        if typeName not in ItemModuleDict.keys():
            ItemModuleDict[typeName] = [EventBuildingInfo.getFullHLTResultID()]
        if typeNameAux not in ItemModuleDict.keys():
            ItemModuleDict[typeNameAux] = [EventBuildingInfo.getFullHLTResultID()]

    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, StreamTagMakerToolCfg, TriggerBitsMakerToolCfg

    # Tool serialising EDM objects to fill the HLT result
    serialiser = TriggerEDMSerialiserToolCfg('Serialiser')
    for item, modules in ItemModuleDict.iteritems():
        __log.debug('adding to serialiser list: %s, modules: %s', item, modules)
        serialiser.addCollection(item, modules)

    # Tools adding stream tags and trigger bits to HLT result
    stmaker = StreamTagMakerToolCfg()
    bitsmaker = TriggerBitsMakerToolCfg()

    # Map decisions producing PEBInfo from DecisionSummaryMakerAlg.FinalStepDecisions to StreamTagMakerTool.PEBDecisionKeys
    pebDecisionKeys = [key for key in summaryAlg.getProperties()['FinalStepDecisions'].values() if 'PEBInfoWriter' in key]
    stmaker.PEBDecisionKeys = pebDecisionKeys

    acc = ComponentAccumulator(sequenceName="HLTTop")
    if offline:
        # Create HLT result maker and alg
        from TrigOutputHandling.TrigOutputHandlingConfig import HLTResultMTMakerCfg
        HLTResultMTMakerAlg=CompFactory.HLTResultMTMakerAlg
        hltResultMakerTool = HLTResultMTMakerCfg()
        hltResultMakerTool.MakerTools = [bitsmaker, stmaker, serialiser] # TODO: stmaker likely not needed for offline BS writing
        hltResultMakerAlg = HLTResultMTMakerAlg()
        hltResultMakerAlg.ResultMaker = hltResultMakerTool
        acc.addEventAlgo( hltResultMakerAlg )
        # TODO: Decide if stream tags are needed and, if yes, find a way to save updated ones in offline BS saving

        # Transfer trigger bits to xTrigDecision which is read by offline BS writing ByteStreamCnvSvc
        from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMakerMT
        decmaker = TrigDecisionMakerMT('TrigDecMakerMT')
        acc.addEventAlgo( decmaker )

        # Create OutputStream alg
        from ByteStreamCnvSvc import WriteByteStream
        StreamBSFileOutput = WriteByteStream.getStream("EventStorage", "StreamBSFileOutput")
        StreamBSFileOutput.ItemList += [ "HLT::HLTResultMT#HLTResultMT" ]
        StreamBSFileOutput.ExtraInputs = [
            ("HLT::HLTResultMT", "HLTResultMT"),
            ("xAOD::TrigDecision", "xTrigDecision")]
        acc.addEventAlgo( StreamBSFileOutput )

    else:
        acc.setPrivateTools( [bitsmaker, stmaker, serialiser] )
    return acc


def triggerPOOLOutputCfg(flags, decObj, decObjHypoOut, edmSet):
    # Get the list from TriggerEDM
    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    edmList = getTriggerEDMList(edmSet, flags.Trigger.EDMDecodingVersion)

    # Build the output ItemList
    itemsToRecord = []
    for edmType, edmKeys in edmList.iteritems():
        itemsToRecord.extend([edmType+'#'+collKey for collKey in edmKeys])

    # Add decision containers (navigation)
    for item in decObj:
        dynamic = '.-' # Exclude dynamic
        if item in decObjHypoOut:
            dynamic = '.' # Include dynamic
        itemsToRecord.append('xAOD::TrigCompositeContainer#{:s}'.format(item))
        itemsToRecord.append('xAOD::TrigCompositeAuxContainer#{:s}Aux{:s}'.format(item, dynamic))

    # Create OutputStream
    outputType = ''
    if flags.Output.doWriteRDO:
        outputType = 'RDO'
    if flags.Output.doWriteESD:
        outputType = 'ESD'
    if flags.Output.doWriteAOD:
        outputType = 'AOD'
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, outputType, ItemList=itemsToRecord)

    # OutputStream has a data dependency on xTrigDecision
    streamAlg = acc.getEventAlgo("OutputStream"+outputType)
    streamAlg.ExtraInputs = [("xAOD::TrigDecision", "xTrigDecision")]

    # Produce the trigger bits
    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerBitsMakerToolCfg
    from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMakerMT

    bitsmaker = TriggerBitsMakerToolCfg()
    decmaker = TrigDecisionMakerMT('TrigDecMakerMT')

    decmaker.BitsMakerTool = bitsmaker
    acc.addEventAlgo( decmaker )

    # Produce trigger metadata
    TrigConf__xAODMenuWriterMT=CompFactory.TrigConf__xAODMenuWriterMT
    menuwriter = TrigConf__xAODMenuWriterMT()
    acc.addEventAlgo( menuwriter )

    return acc


def triggerMergeViewsAndAddMissingEDMCfg( edmSet, hypos, viewMakers, decObj, decObjHypoOut ):

    HLTEDMCreatorAlg, HLTEDMCreator=CompFactory.getComps("HLTEDMCreatorAlg","HLTEDMCreator",)
    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3

    alg = HLTEDMCreatorAlg("EDMCreatorAlg")

    # configure views merging
    needMerging = filter( lambda x: len(x) >= 4 and x[3].startswith("inViews:"),  TriggerHLTListRun3 )
    __log.info("These collections need merging: {}".format( " ".join([ c[0] for c in needMerging ])) )
    # group by the view collection name/(the view maker algorithm in practice)
    from collections import defaultdict
    groupedByView = defaultdict(list)
    [ groupedByView[c[3]].append( c ) for c in needMerging ]

    for view, colls in groupedByView.iteritems():
        viewCollName = view.split(":")[1]
        tool = HLTEDMCreator( "{}Merger".format( viewCollName ) )
        for coll in colls:  # see the content in TrigEDMConfigRun3
            collType, collName = coll[0].split("#")
            collType = collType.split(":")[-1]
            viewsColl = coll[3].split(":")[-1]
            # Get existing property, or return empty list if not set.
            attrView = getattr(tool, collType+"Views", [])
            attrInView = getattr(tool, collType+"InViews", [])
            attrName = getattr(tool, collType, [])
            #
            attrView.append( viewsColl )
            attrInView.append( collName )
            attrName.append( collName )
            #
            setattr(tool, collType+"Views", attrView )
            setattr(tool, collType+"InViews", attrInView )
            setattr(tool, collType, attrName )
            producer = [ maker for maker in viewMakers if maker.Views == viewsColl ]
            if len(producer) == 0:
                __log.warning("The producer of the {} not in the menu, it's outputs won't ever make it out of the HLT".format( viewsColl ) )
                continue
            if len(producer) > 1:
                for pr in producer[1:]:
                    if pr != producer[0]:
                        __log.error("Several View making algorithms produce the same output collection {}: {}".format( viewsColl, ' '.join([p.name() for p in producer ]) ) )
                        continue
        alg.OutputTools += [ tool ]

    tool = HLTEDMCreator( "GapFiller" )
    if len(edmSet) != 0:
        from collections import defaultdict
        groupedByType = defaultdict( list )

        # scan the EDM
        for el in TriggerHLTListRun3:
            if not any([ outputType in el[1].split() for outputType in edmSet ]):
                continue
            collType, collName = el[0].split("#")
            if "Aux" in collType: # the GapFiller crates appropriate Aux obejcts
                continue
            groupedByType[collType].append( collName )

        for collType, collNameList in groupedByType.iteritems():
            propName = collType.split(":")[-1]
            if hasattr( tool, propName ):
                setattr( tool, propName, collNameList )
                __log.info("GapFiller will create EDM collection type '{}' for '{}'".format( collType, collNameList ))
            else:
                __log.info("EDM collections of type {} are not going to be added to StoreGate, if not created by the HLT".format( collType ))

    __log.debug("The GapFiller is ensuring the creation of all the decision object collections: '{}'".format( decObj ) )
    # Append and hence confirm all TrigComposite collections
    # Gap filler is also used to perform re-mapping of the HypoAlg outputs which is a sub-set of decObj
    tool.FixLinks = list(decObjHypoOut)
    tool.TrigCompositeContainer += list(decObj)
    alg.OutputTools += [tool]

    return alg




def triggerRunCfg( flags, menu=None ):
    """
    top of the trigger config (for real triggering online or on MC)
    Returns: ca only
    """
    acc = ComponentAccumulator()

    # L1ConfigSvc needed for L1Decoder
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge( L1ConfigSvcCfg(flags) )

    acc.merge( triggerIDCCacheCreatorsCfg( flags ) )

    from L1Decoder.L1DecoderConfig import L1DecoderCfg
    l1DecoderAcc, l1DecoderAlg = L1DecoderCfg( flags )
    acc.merge( l1DecoderAcc )


    # detour to the menu here, (missing now, instead a temporary hack)
    if menu:
        menuAcc = menu( flags )
        HLTSteps = menuAcc.getSequence( "HLTAllSteps" )
        __log.info( "Configured menu with "+ str( len(HLTSteps.getChildren()) ) +" steps" )


    # collect hypothesis algorithms from all sequence
    hypos = collectHypos( HLTSteps )
    filters = collectFilters( HLTSteps )

    summaryAcc, summaryAlg = triggerSummaryCfg( flags, hypos )
    acc.merge( summaryAcc )

    #once menu is included we should configure monitoring here as below

    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, filters, l1DecoderAlg )
    acc.merge( monitoringAcc )

    from TrigCostMonitorMT.TrigCostMonitorMTConfig import TrigCostMonitorMTCfg
    acc.merge( TrigCostMonitorMTCfg( flags ) )

    decObj = collectDecisionObjects( hypos, filters, l1DecoderAlg, summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
    __log.info( "Number of decision objects found in HLT CF %d", len( decObj ) )
    __log.info( "Of which, %d are the outputs of hypos", len( decObjHypoOut ) )
    __log.info( str( decObj ) )

    HLTTop = seqOR( "HLTTop", [ l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg ] )
    acc.addSequence( HLTTop )

    acc.merge( menuAcc )


    # configure components need to normalise output before writing out
    viewMakers = collectViewMakers( HLTSteps )

    outputAcc, edmSet = triggerOutputCfg( flags, decObj, decObjHypoOut, summaryAlg )
    acc.merge( outputAcc )

    if edmSet:
        mergingAlg = triggerMergeViewsAndAddMissingEDMCfg( [edmSet] , hypos, viewMakers, decObj, decObjHypoOut )
        acc.addEventAlgo( mergingAlg, sequenceName="HLTTop" )

    return acc

def triggerIDCCacheCreatorsCfg(flags):
    """
    Configures IDC cache loading, for now unconditionally, may make it menu dependent in future
    """
    acc = ComponentAccumulator()
    from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheCfg
    acc.merge( MuonCacheCfg() )

    from TrigUpgradeTest.InDetConfig import InDetIDCCacheCreatorCfg
    acc.merge( InDetIDCCacheCreatorCfg() )

    return acc

def triggerPostRunCfg(flags):
    """
    Configures components needed for processing trigger informatin in Raw/ESD step
    Returns: ca only
    """
    acc = ComponentAccumulator()
    # configure in order BS decodnig, EDM gap filling, insertion of trigger metadata to ESD

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Trigger.L1Decoder.forceEnableAllChains = True
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",]
    ConfigFlags.lock()

    def testMenu(flags):
        menuCA = ComponentAccumulator()
        menuCA.addSequence( seqAND("HLTAllSteps") )
        return menuCA

    acc = triggerRunCfg( ConfigFlags, testMenu )

    f=open("TriggerRunConf.pkl","wb")
    acc.store(f)
    f.close()
