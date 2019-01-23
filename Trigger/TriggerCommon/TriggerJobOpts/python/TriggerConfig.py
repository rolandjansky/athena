# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


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
    return hypos

def __decisionsFromHypo( hypo ):
    """ return all chains served by this hypo and the key of produced decision object """
    if hypo.getType() == 'ComboHypo':
        return hypo.MultiplicitiesMap.keys(), hypo.HypoOutputDecisions[0]            
    else: # regular hypos
        return [ t.name() for t in hypo.HypoTools ], hypo.HypoOutputDecisions



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
    return filters


def collectDecisionObjects(  hypos, filters, l1decoder ):
    """
    Returns the set of all decision objects of HLT
    """
    decisionObjects = set()
    __log.info("Collecting decision obejcts from L1 decoder instance")
    decisionObjects.update([ d.Decisions for d in l1decoder.roiUnpackers ])
    decisionObjects.update([ d.Decisions for d in l1decoder.rerunRoiUnpackers ]) 

    
    __log.info("Collecting decision obejcts from hypos")
    __log.info(hypos)
    for step, stepHypos in hypos.iteritems():
        for hypoAlg in stepHypos:
            __log.debug( "Hypo %s with input %s and output %s " % (hypoAlg.getName(), str(hypoAlg.HypoInputDecisions), str(hypoAlg.HypoOutputDecisions) ) )
            __log.debug( "and hypo tools %s " % (str( [ t.getName() for t in hypoAlg.HypoTools ] ) ) ) 
            decisionObjects.add( hypoAlg.HypoInputDecisions )
            decisionObjects.add( hypoAlg.HypoOutputDecisions )
        
    __log.info("Collecting decision obejcts from filters")
    for step, stepFilters in filters.iteritems():
        for filt in stepFilters:
            decisionObjects.update( filt.Input )
            decisionObjects.update( filt.Output )
    
    return decisionObjects

    
def triggerSummaryCfg(flags, hypos):
    """ 
    Configures an algorithm(s) that should be run after the slection process
    Returns: ca, algorithm
    """
    acc = ComponentAccumulator()
    from TrigOutputHandling.TrigOutputHandlingConf import DecisionSummaryMakerAlg
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
    decisionSummaryAlg.DecisionsSummaryKey = "HLTSummary" # Output
    return acc, decisionSummaryAlg
        


def triggerMonitoringCfg(flags, hypos, l1Decoder):
    """
    Configures components needed for monitoring chains
    """
    acc = ComponentAccumulator()
    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
    mon = TrigSignatureMoniMT()
    mon.L1Decisions = "L1DecoderSummary"
    mon.FinalDecisionKey = "HLTSummary" # Input
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
    return acc, mon

def triggerOutputStreamCfg( flags, decObj, outputType ):
    """ 
    Configure output stream according to the menu setup (decision objects)
    and TrigEDMCOnfig
    """
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemsToRecord = []
    # decision objects and their Aux stores
    def __TCKeys( name ):
        return [ "xAOD::TrigCompositeContainer#%s" % name, "xAOD::TrigCompositeAuxContainer#%sAux." % name]
    [ itemsToRecord.extend( __TCKeys(d) ) for d in decObj ]
    # the rest of triger EDM
    itemsToRecord.extend( __TCKeys( "HLTSummary" ) )

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTList
    EDMCollectionsToRecord=filter( lambda x: outputType in x[1] and "TrigCompositeContainer" not in x[0],  TriggerHLTList )
    itemsToRecord.extend( [ el[0] for el in EDMCollectionsToRecord ] )
    
    # summary objects
    __log.debug( outputType + " trigger content "+str( itemsToRecord ) )
    acc = OutputStreamCfg( flags, outputType, ItemList=itemsToRecord )
    streamAlg = acc.getEventAlgo("OutputStream"+outputType)
    streamAlg.ExtraInputs = [("xAOD::TrigCompositeContainer", "HLTSummary")]

    return acc
    
def triggerAddMissingEDMCfg( flags, decObj ):

    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg    
    EDMFillerAlg = TriggerSummaryAlg( "EDMFillerAlg" )
    EDMFillerAlg.InputDecision  = "L1DecoderSummary"

    from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
    DecisionObjectsFiller = HLTEDMCreator("DecisionObjectsFiller")
    DecisionObjectsFiller.TrigCompositeContainer = list(decObj)
    EDMFillerAlg.OutputTools += [ DecisionObjectsFiller ]

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTList
    collectionsThatNeedMerging = filter( lambda x: len(x) >= 4 and x[3].startswith("inViews:"),  TriggerHLTList )
    for c in collectionsThatNeedMerging:
        tool = HLTEDMCreator(c[0].split("#")[1]+"merger")
        ctype, cname = c[0].split("#")
        ctype = ctype.split(":")[-1]        
        viewsColl = c[3].split(":")[-1]
        setattr(tool, ctype+"Views", [ viewsColl ] )
        setattr(tool, ctype+"InViews", [ cname ] )
        setattr(tool, ctype, [ cname ] )
        EDMFillerAlg.OutputTools += [ tool ]
#egammaViewsMerger.TrigEMClusterContainerViews = [ "EMCaloViews" ]
#egammaViewsMerger.TrigEMClusterContainerInViews = [ clustersKey ]
#egammaViewsMerger.TrigEMClusterContainer = [ clustersKey ]

    return EDMFillerAlg
    
    
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
    
    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, l1DecoderAlg )
    acc.merge( monitoringAcc )

    decObj = collectDecisionObjects( hypos, filters, l1DecoderAlg )
    __log.info( "Number of decision objects found in HLT CF %d" % len( decObj ) )
    __log.info( str( decObj ) )
    
    HLTTop = seqOR( "HLTTop", [ l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg ] )
    acc.addSequence( HLTTop )
        
    acc.merge( menuAcc )

    # output
    # if any output stream is requested, schedule "gap" filling algorithm
    if flags.Output.ESDFileName != "" or flags.Output.AODFileName != "":
        acc.addEventAlgo( triggerAddMissingEDMCfg( flags, decObj ), sequenceName= "HLTTop" )
        
    # configure streams
    if flags.Output.ESDFileName != "":
        acc.merge( triggerOutputStreamCfg( flags, decObj, "ESD" ) )

    if flags.Output.AODFileName != "":        
        acc.merge( triggerOutputStreamCfg( flags, decObj, "AOD" ) )

        
    
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

