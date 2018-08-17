# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.CFElements import seqAND, seqOR, flatAlgorithmSequences
from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerConfig')

def collectHypos( steps ):
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

def triggerMonitoringCfg(flags, hypos):
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
    from AthenaCommon.Constants import DEBUG
    allChains = []
    for stepName, stepHypos in hypos.iteritems():
        dcTool = DecisionCollectorTool( "DecisionCollector" + stepName, OutputLevel=DEBUG )
        for hypo in stepHypos:
            dcTool.Decisions += [ hypo.HypoOutputDecisions ]
            for t in hypo.HypoTools:
                allChains.append( t.name() )
        mon.CollectorTools += [ dcTool ]
        __log.info( "The step monitoring decisions in " + dcTool.name() + str( dcTool.Decisions ) )
    
    mon.FinalDecisions = mon.CollectorTools[-1].Decisions
    __log.info( "Final decisions to be monitored are "+ str( mon.FinalDecisions ) )
    mon.ChainsList = list( set(allChains) )

    return acc, mon




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
    acc.merge( l1DecoderAcc)    
            

    # detour to the menu here, (missing now, instead a temporary hack)
    if menu:
        menuAcc, HLTSteps = menu( flags )
        __log.info( "Configured menu with "+ str( len(HLTSteps.getChildren()) ) +" steps" )
        
    # collect hypothesis algorithms from all sequence
    hypos = collectHypos( HLTSteps )           
    
    summaryAcc, summaryAlg = triggerSummaryCfg( flags, hypos )
    acc.merge( summaryAcc ) 
    
    #once menu is included we should configure monitoring here as below
    
    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos )
    acc.merge( monitoringAcc )
    
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

    ConfigFlags.set("Trigger.L1Decoder.forceEnableAllChains",True)
    ConfigFlags.set('global.InputFiles',["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",])
    ConfigFlags.lock()    
    
    acc = triggerRunCfg( ConfigFlags, lambda x: (ComponentAccumulator(), seqAND("whatever") ) )

    f=open("TriggerRunConf.pkl","w")
    acc.store(f)
    f.close()

