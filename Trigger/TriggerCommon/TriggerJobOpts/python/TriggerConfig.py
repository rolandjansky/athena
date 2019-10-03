# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
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
    if hypo.getType() == 'ComboHypo':
        return hypo.MultiplicitiesMap.keys(), hypo.HypoOutputDecisions[0]
    else: # regular hypos
        return [ t.name() for t in hypo.HypoTools ], hypo.HypoOutputDecisions


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
    __log.info("Collecting hypos")
    from collections import defaultdict
    filters = defaultdict( list )

    for stepSeq in steps.getChildren():
        if "filter" in stepSeq.name():
            filters[stepSeq.name()] = stepSeq.getChildren()
            __log.info("Found Filters in Step {} : {}".format(stepSeq.name(), stepSeq.getChildren()))

    return filters


def collectL1DecoderDecisionObjects(l1decoder):
    decisionObjects = set()
    __log.info("Collecting decision objects from L1 decoder instance")
    decisionObjects.update([ d.Decisions for d in l1decoder.roiUnpackers ])
    decisionObjects.update([ d.Decisions for d in l1decoder.rerunRoiUnpackers ])
    return decisionObjects

def collectHypoDecisionObjects(hypos, inputs = True, outputs = True):
    decisionObjects = set()
    __log.info("Collecting decision objects from hypos")
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
    return decisionObjects

def collectFilterDecisionObjects(filters, inputs = True, outputs = True):
    decisionObjects = set()
    __log.info("Collecting decision objects from filters")
    for step, stepFilters in filters.iteritems():
        for filt in stepFilters:
            if inputs:
                decisionObjects.update( filt.Input )
            if outputs:
                decisionObjects.update( filt.Output )
    return decisionObjects

def collectDecisionObjects(  hypos, filters, l1decoder ):
    """
    Returns the set of all decision objects of HLT
    """
    decObjL1 = collectL1DecoderDecisionObjects(l1decoder)
    decObjHypo = collectHypoDecisionObjects(hypos)
    decObjFilter = collectFilterDecisionObjects(filters)
    decisionObjects = set()
    decisionObjects.update(decObjL1)
    decisionObjects.update(decObjHypo)
    decisionObjects.update(decObjFilter)
    return decisionObjects

def triggerSummaryCfg(flags, hypos):
    """
    Configures an algorithm(s) that should be run after the section process
    Returns: ca, algorithm
    """
    acc = ComponentAccumulator()
    from TrigOutputHandling.TrigOutputHandlingConf import DecisionSummaryMakerAlg
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    decisionSummaryAlg = DecisionSummaryMakerAlg()
    allChains = {}
    for stepName, stepHypos in sorted( hypos.items() ):
        for hypo in stepHypos:
            hypoChains,hypoOutputKey = __decisionsFromHypo( hypo )
            allChains.update( dict.fromkeys( hypoChains, hypoOutputKey ) )

    for c, cont in allChains.iteritems():
        __log.info("Final decision of chain  " + c + " will be red from " + cont )
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
    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
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

        dcTool = DecisionCollectorTool( "DecisionCollector" + stepName, Decisions=stepDecisionKeys )
        __log.info( "The step monitoring decisions in " + dcTool.name() + " " +str( dcTool.Decisions ) )
        mon.CollectorTools += [ dcTool ]


    #mon.FinalChainStep = allChains
    mon.L1Decisions  = l1Decoder.getProperties()['L1DecoderSummaryKey'] if l1Decoder.getProperties()['L1DecoderSummaryKey'] != '<no value>' else l1Decoder.getDefaultProperty('L1DecoderSummary')
    allChains.update( l1Decoder.ChainToCTPMapping.keys() )
    mon.ChainsList = list( allChains )
    
    from DecisionHandling.DecisionHandlingConfig import setupFilterMonitoring
    [ [ setupFilterMonitoring( alg ) for alg in algs ]  for algs in filters.values() ]

    
    return acc, mon

def triggerOutputStreamCfg( flags, decObj, outputType ):
    """
    Configure output stream according to the menu setup (decision objects)
    and TrigEDMConfig
    """
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemsToRecord = []
    # decision objects and their Aux stores
    def __TCKeys( name ):
        return [ "xAOD::TrigCompositeContainer#%s" % name, "xAOD::TrigCompositeAuxContainer#%sAux." % name]
    [ itemsToRecord.extend( __TCKeys(d) ) for d in decObj ]
    # the rest of triger EDM
    itemsToRecord.extend( __TCKeys( "HLTNav_Summary" ) )

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3
    EDMCollectionsToRecord=filter( lambda x: outputType in x[1] and "TrigCompositeContainer" not in x[0],  TriggerHLTListRun3 )
    itemsToRecord.extend( [ el[0] for el in EDMCollectionsToRecord ] )

    # summary objects
    __log.info( outputType + " trigger content "+str( itemsToRecord ) )
    acc = OutputStreamCfg( flags, outputType, ItemList=itemsToRecord )
    streamAlg = acc.getEventAlgo("OutputStream"+outputType)
    streamAlg.ExtraInputs = [("xAOD::TrigCompositeContainer", "HLTNav_Summary")] # OutputStream has a data dependency on HLTNav_Summary

    return acc

def triggerBSOutputCfg( flags, decObj ):
    """
    Configure output to be saved in BS
    """
    acc = ComponentAccumulator()

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3, persistent
    from TrigOutputHandling.TrigOutputHandlingConf import HLTResultMTMakerAlg, TriggerBitsMakerTool, StreamTagMakerTool
    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, HLTResultMTMakerCfg
    
    serialiser = TriggerEDMSerialiserToolCfg("Serialiser")
    for coll in decObj:
        serialiser.addCollectionListToMainResult( [ "{}#remap_{}".format( persistent("xAOD::TrigCompositeContainer"), coll ),
                                                    "{}#remap_{}Aux.".format( persistent("xAOD::TrigCompositeAuxContainer"), coll )] )

    # EDM
    EDMCollectionsToRecord=filter( lambda x: "BS" in x[1],  TriggerHLTListRun3 )    
    for item in EDMCollectionsToRecord:
        typeName, collName = item[0].split("#")
        serialisedTypeColl="{}#{}".format(persistent(typeName), collName)
        __log.info( "Serialising {}".format( serialisedTypeColl ) ) 
        serialiser.addCollectionListToMainResult( [ serialisedTypeColl ] )

    stmaker = StreamTagMakerTool()
    stmaker.HLTmenuFile = TriggerFlags.outputHLTmenuJsonFile()
        
    bitsmaker = TriggerBitsMakerTool()
    
    hltResultMakerTool            = HLTResultMTMakerCfg("MakerTool") # want short nme to see in the log
    hltResultMakerTool.MakerTools = [ serialiser, stmaker, bitsmaker ] 
    hltResultMakerAlg             = HLTResultMTMakerAlg()
    hltResultMakerAlg.ResultMaker = hltResultMakerTool
    acc.addEventAlgo( hltResultMakerAlg )

    return acc


def triggerMergeViewsAndAddMissingEDMCfg( edmSet, hypos, viewMakers, decObj ):

    from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreatorAlg, HLTEDMCreator
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
    tool.FixLinks = list(collectHypoDecisionObjects(hypos, inputs=False, outputs=True))
    tool.TrigCompositeContainer += list(decObj)
    alg.OutputTools += [tool]

    return alg


def setupL1DecoderFromMenu( flags, l1Decoder ):
    """ Post setup of the L1Decoder, once approved, it should be moved to L1DecoderCfg function """

    from TriggerJobOpts.MenuConfigFlags import MenuUtils
    l1Decoder.ChainToCTPMapping = MenuUtils.toCTPSeedingDict( flags )


def triggerRunCfg( flags, menu=None ):
    """
    top of the trigger config (for real triggering online or on MC)
    Returns: ca only
    """
    if flags.Trigger.doLVL1:
        # conigure L1 simulation
        pass

    acc = ComponentAccumulator()

    acc.merge( triggerIDCCacheCreatorsCfg( flags ) )

    from L1Decoder.L1DecoderConfig import L1DecoderCfg
    #TODO
    # information about the menu has to be injected into L1 decoder config
    # necessary ingreedient is list of mappings from L1 item to chain
    # and item to threshold (the later can be maybe extracted from L1 config file)
    l1DecoderAcc, l1DecoderAlg = L1DecoderCfg( flags )
    setupL1DecoderFromMenu( flags, l1DecoderAlg )
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

    decObj = collectDecisionObjects( hypos, filters, l1DecoderAlg )
    __log.info( "Number of decision objects found in HLT CF %d", len( decObj ) )
    __log.info( str( decObj ) )

    HLTTop = seqOR( "HLTTop", [ l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg ] )
    acc.addSequence( HLTTop )

    acc.merge( menuAcc )

    
    # configure components need to normalise output before writing out
    viewMakers = collectViewMakers( HLTSteps )
    edmSet = []

    if flags.Output.ESDFileName != "":
        __log.debug( "Setting up trigger EDM output for ESD" )
        acc.merge( triggerOutputStreamCfg( flags, decObj, "ESD" ) )
        edmSet.append('ESD')

    if flags.Output.AODFileName != "":
        __log.debug( "Setting up trigger EDM output for AOD" )
        acc.merge( triggerOutputStreamCfg( flags, decObj, "AOD" ) )
        edmSet.append('AOD')
        
    if any( (flags.Output.ESDFileName != "" , flags.Output.AODFileName != "", flags.Trigger.writeBS) ):
        mergingAlg = triggerMergeViewsAndAddMissingEDMCfg( edmSet , hypos, viewMakers, decObj )
        acc.addEventAlgo( mergingAlg, sequenceName="HLTTop" )

    # configure actual streams
    if flags.Trigger.writeBS:
        __log.debug( "Setting up trigger output for ByteStream" )
        acc.merge( triggerBSOutputCfg( flags, decObj ) )

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

    f=open("TriggerRunConf.pkl","w")
    acc.store(f)
    f.close()
