# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


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
                if 'HypoInputDecisions'  in alg.__class__.__dict__:
                    __log.info( "found hypo " + alg.name() + " in " +stepSeq.name() )
                    hypos[stepSeq.name()].append( alg )
    return hypos


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


def collectDecisionObjects( steps, l1decoder ):
    """
    Returns the set of all decision objects of HLT
    """
    decisionObjects = set()
    __log.info("Collecting decision obejcts from L1 decoder instance")
    decisionObjects.update([ d.Decisions for d in l1decoder.roiUnpackers ])
    decisionObjects.update([ d.Decisions for d in l1decoder.rerunRoiUnpackers ]) 

    
    __log.info("Collecting decision obejcts from hypos")
    hypos = collectHypos( steps )
    __log.info(hypos)
    for s, sh in hypos.iteritems():
        for hypo in sh:
            print hypo
            decisionObjects.add( hypo.HypoInputDecisions )
            decisionObjects.add( hypo.HypoOutputDecisions )
        
    __log.info("Collecting decision obejcts from filters")
    filters = collectFilters( steps )
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
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summaryAlg = TriggerSummaryAlg()
    summaryAlg.InputDecision = "HLTChains"
    
    return acc, summaryAlg

def triggerMonitoringCfg(flags, hypos, l1Decoder):
    """
    Configures components needed for monitoring chains
    """
    acc = ComponentAccumulator()
    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
    mon = TrigSignatureMoniMT()
    mon.L1Decisions="HLTChains"
    if len(hypos) == 0:
        __log.warning("Menu is not configured")
        return acc, mon
    allChains = {} # collects the last decision obj for each chain

    for stepName, stepHypos in sorted( hypos.items() ):
        decisions = []
        for hypo in stepHypos:


            if hypo.getType() == 'ComboHypo':
                decisions.append( hypo.HypoOutputDecisions[0] )
                for chain, m in hypo.MultiplicitiesMap.iteritems():
                    allChains[chain] = hypo.HypoOutputDecisions[0]
            
            else: # regular hypos
                decisions.append( hypo.HypoOutputDecisions )
                for t in hypo.HypoTools:
                    allChains[t.name()] = hypo.HypoOutputDecisions

        dcTool = DecisionCollectorTool( "DecisionCollector" + stepName, Decisions=decisions )
        __log.info( "The step monitoring decisions in " + dcTool.name() + " " +str( dcTool.Decisions ) )
        mon.CollectorTools += [ dcTool ]

    
    mon.FinalChainStep = allChains
    mon.FinalDecisions = list( set( allChains.values() ) )
    mon.L1Decisions  = l1Decoder.getProperties()['Chains'] if l1Decoder.getProperties()['Chains'] != '<no value>' else l1Decoder.getDefaultProperty('Chains')
    __log.info( "Final decisions to be monitored are "+ str( mon.FinalDecisions ) )    
    mon.ChainsList = list( set( allChains.keys() + l1Decoder.ChainToCTPMapping.keys()) )

    
    return acc, mon

def setupL1DecoderFromMenu( flags, l1Decoder ):
    """ Post setup of the L1Decoder, once approved, it should be moved to L1DecoderCfg function """

    from TriggerJobOpts.MenuConfigFlags import MenuUtils
    l1Decoder.ChainToCTPMapping = MenuUtils.toCTPSeedingDict( flags )


def triggerRunCfg(flags, menu=None):
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
    
    summaryAcc, summaryAlg = triggerSummaryCfg( flags, hypos )
    acc.merge( summaryAcc ) 
    
    #once menu is included we should configure monitoring here as below
    
    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, l1DecoderAlg )
    acc.merge( monitoringAcc )

    decObj = collectDecisionObjects( HLTSteps, l1DecoderAlg )
    __log.info( "Number of decision objects found in HLT CF %d" % len( decObj ) )
    __log.info( str( decObj ) )
    
    HLTTop = seqOR( "HLTTop", [ l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg ] )
    acc.addSequence( HLTTop )

    
    
    acc.merge( menuAcc )
    
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

