# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import dumpSequence
from TrigUpgradeTest.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot

import sys
import copy
from AthenaCommon.Constants import VERBOSE,INFO,DEBUG
log = logging.getLogger('HLTCFConfig')
log.setLevel( VERBOSE )

#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, flatDecisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summary = TriggerSummaryAlg( name, OutputLevel = 2 )
    summary.InputDecision = "HLTChains"
  
    summary.FinalDecisions = flatDecisions
    summary.HLTSummary = "MonitoringSummary"+name
    return summary

def makeMonitor(name, decisions, EnabledChainNames):
    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
    mon = TrigSignatureMoniMT(name, OutputLevel = 2)
    flatDecisions=[]
    for step in decisions: flatDecisions.extend (step)
    mon.FinalDecisions = flatDecisions
    mon.L1Decisions = "HLTChainsResult" # connection with L1Decoder
    from TrigUpgradeTest.TestUtils import MenuTest
    mon.ChainsList = EnabledChainNames
    tools=[]
    for step in range (0, len(decisions)):
        print "adding collector ",step
        print decisions[step]
        collect = DecisionCollectorTool("StepCollector%d"%step)
        collect.Decisions = decisions[step]
        print collect
        tools.append(collect)
    mon.CollectorTools=tools
    print mon
    return mon

def makeStreamESD(name, flatDecisions):
    import AthenaPoolCnvSvc.WriteAthenaPool
    from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
    StreamESD=createOutputStream(name,"myESD.pool.root",True)
    StreamESD.OutputLevel=3
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.remove( StreamESD )
    def addTC(name):   
        StreamESD.ItemList += [ "xAOD::TrigCompositeContainer#"+name, "xAOD::TrigCompositeAuxContainer#"+name+"Aux." ]

    for tc in flatDecisions:
        addTC( tc )

    addTC("HLTSummary")
    if log.isEnabledFor(logging.DEBUG):
        log.debug("ESD file content: ")
        log.debug( StreamESD.ItemList  )
    return StreamESD


def create_step_reco_node(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """
    if log.isEnabledFor(logging.DEBUG):
        log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name+"_reco")
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_step_filter_node(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """
    if log.isEnabledFor(logging.DEBUG):
        log.debug("Create filter step %s with %d filters", name, len(seq_list))
    stepCF = parOR(name+"_filter")
    for seq in seq_list:
        filterAlg=seq.filter.Alg                    
        log.info("Add  %s to filter node %s", filterAlg.name(), name)
        stepCF += filterAlg
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF



def create_step_sequence(name, filterAlg, rest,sublist):
    """ elementary HLT step sequencer, filterAlg is gating, rest is anything that needs to happe within the step """
    toadd=rest
    for k, v in sublist.iteritems():
        seq= seqAND(k, v)
        toadd.append(seq)
        
    stepReco = parOR(name+"_reco", toadd)    
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd


def create_CFSequence(CFseq):
    """ Creates AthSequencer nodes with sequences attached """
    if log.isEnabledFor(logging.DEBUG):
        log.debug(" *** Create CFSequence %s with FilterAlg %s *", CFseq.name, CFseq.filter.Alg.name())
    filterAlg=CFseq.filter.Alg


    stepReco = parOR(CFseq.name+"_reco") # all reco algoritms from al lthe sequences in a parallel sequence
    seqAndView = seqAND(CFseq.name+"_view", [stepReco]) #include in seq:And to run in views: add here the Hypo
    seqAndWithFilter = seqAND(CFseq.name, [filterAlg, seqAndView]) # add to the main step+filter
    
    for menuseq in CFseq.step.sequences:
        ath_sequence=menuseq.sequence.Alg
        subs = ath_sequence
        stepReco += subs
        seqAndView += menuseq.hypo.Alg 

    if CFseq.step.isCombo:
        seqAndView += CFseq.step.combo.Alg
        print CFseq.step.combo.Alg
        
    return seqAndWithFilter


def addChainToHypoAlg(hypoAlg, chain):
    """ Tmp functions for Combo Hypo """
    if "Comb" in hypoAlg.algname:
        prop="Chains"
        cval = hypoAlg.Alg.getProperties()[prop]
        try:
            cval.append(chain)
            setattr(hypoAlg.Alg, prop, cval)
        except:
            pass


#######################################
## CORE of Decision Handling
#######################################



def makeHLTTree(HLTChains):
    """ creates the full HLT tree"""
   
    #    TopHLTRootSeq = seqAND("TopHLTRoot") # Root
    # main HLT top sequence
    hltTop = seqOR( "hltTop")

    #add the L1Upcacking
#    TopHLTRootSeq += L1UnpackingSeq

    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop +=  steps
    
    # make CF tree
    finalDecisions = decisionTree_From_Chains(steps, HLTChains)
    EnabledChainNames = [c.name for c in HLTChains]
    

    flatDecisions=[]
    for step in finalDecisions: flatDecisions.extend (step)
    summary= makeSummary("TriggerSummaryFinal", flatDecisions)
    from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
    edmCreator = HLTEDMCreator()
    
    edmCreator.TrigCompositeContainer = flatDecisions
    summary.OutputTools= [ edmCreator ]
    hltTop += summary
    hltTop += makeMonitor("TriggerMonitorFinal", finalDecisions, EnabledChainNames)
    hltTop += makeStreamESD("StreamESD", flatDecisions)
   
    
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += hltTop

        

def decisionTree_From_Chains(HLTNode, chains):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """
    if log.isEnabledFor(logging.DEBUG):
        log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())

    from TrigUpgradeTest.MenuComponents import CFSequence, RoRSequenceFilterNode, ComboMaker
    HLTNodeName= HLTNode.name()

    # find nsteps
    NSTEPS=0
    for chain in chains:
        steps =len(chain.steps)
        if log.isEnabledFor(logging.DEBUG):
            log.debug("Adding chain %s with %d steps"%(chain.name,steps))
        if NSTEPS < steps:
            NSTEPS = steps
            
    if log.isEnabledFor(logging.DEBUG):
        log.debug("Run on %d steps", NSTEPS)
    

    #loop over chains to configure hypotools
    # must be done after all chains are created, to avoid conflicts 
    for chain in chains:
        chain.decodeHypoToolConfs()

    finalDecisions = [] # needed for monitor
    allSeq_list = []
    for nstep in range(0, NSTEPS):
        finalDecisions.append([]) # list of final deciisons per step
        stepCF_name =  "Step%i"%(nstep+1)
        CFseq_list = []
        step_decisions = []
        for chain in chains:
            # skip this step if missing
            if len(chain.steps) <= nstep:               
                continue

            chain_step=chain.steps[nstep]
            if log.isEnabledFor(logging.DEBUG):
                log.debug("\n************* Start step %d %s for chain %s", nstep+1, stepCF_name, chain.name)
            
            # one filter per step_chain
            # one filter input per previous menusequence output (the L1Seed for first step)
            # one filter output per menuSeq
            
            filter_input =[]
            if nstep == 0: # L1 seeding
                previous_sequence="".join(chain.group_seed)              
                seeds=chain.group_seed
                filter_input.extend( seeds )
                if log.isEnabledFor(logging.DEBUG):
                    log.debug("Found these seeds from the sequence: %s", seeds)
                    log.debug("Seeds from this chain: %s", filter_input)
                previous_seeds=seeds  
                                      
            else:
                # from previous step, map the seuqence in the same order?
                previous_sequence = chain_step.name
                prev=chain.steps[nstep-1].sequences
                previous_seeds=[]
                for seq in prev:
                    filter_input.extend(seq.outputs)
                    previous_seeds.append( seq.seed)

                if log.isEnabledFor(logging.DEBUG):    
                    log.debug("Connect to previous sequence through these filter inputs: %s" %str( filter_input) )
                if len(filter_input) != len(previous_seeds):
                    log.error("found %d filter inputs and %d seeds", len(filter_input), len(previous_seeds))
                    sys.exit("ERROR, in size")
                    
          
            (sfilter, alreadyFoundFilter) = buildFilter(stepCF_name,previous_sequence, CFseq_list, chain, filter_input, previous_seeds)
             
            if not alreadyFoundFilter:
                CF_seq = CFSequence( ChainStep=chain_step, FilterAlg=sfilter)
                CFseq_list.append(CF_seq)
                for sequence in chain_step.sequences:                
                    step_decisions.extend(sequence.outputs)                
            
            if len(chain.steps) == nstep+1:  
                if log.isEnabledFor(logging.DEBUG):
                    log.debug("Adding finalDecisions for chain %s at step %d:"%(chain.name, nstep+1))
                for seq in chain_step.sequences:
                    finalDecisions[nstep].extend(seq.outputs)
                    if log.isEnabledFor(logging.DEBUG):
                        log.debug(seq.outputs)
            
            #end of loop over menu sequences
                
        #end of loop over chains for this step, now implement CF:
        if log.isEnabledFor(logging.DEBUG):
            log.debug("\n******** Create CF Tree %s with AthSequencers", stepCF_name)        
        #first make the filter step
        stepFilter = create_step_filter_node(stepCF_name, CFseq_list, dump=False)
        HLTNode += stepFilter
        allSeq_list.append(CFseq_list)

        # then the reco step
        stepCF = create_step_reco_node("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list, dump=False)
        HLTNode += stepCF


        # then the monitor summary
        summary=makeSummary("TriggerSummary"+ stepCF_name, step_decisions)
        HLTNode += summary

        if log.isEnabledFor(logging.DEBUG):
            log.debug("Now Draw...")
        stepCF_DataFlow_to_dot("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)

        if log.isEnabledFor(logging.DEBUG):
            log.info("************* End of step %d, %s", nstep+1, stepCF_name)
        # end of steps


    if log.isEnabledFor(logging.DEBUG):
        log.debug("finalDecisions: %s" %str( finalDecisions) )
    all_DataFlow_to_dot(HLTNodeName, allSeq_list)
    return finalDecisions



## This can be simplified!
def buildFilter(stepCF_name,previous_sequence, CFseq_list, chain, filter_input, previous_seeds):
        from TrigUpgradeTest.MenuComponents import  RoRSequenceFilterNode
#### Build the FILTER
# one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
# if the previous hypo has more than one output, try to get all of them
# one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
        filter_name="Filter%s_on_%s"%(stepCF_name,previous_sequence)
        findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.algname]
        n_filters = len(findFilter)
        if n_filters == 1:
            sfilter=findFilter[0]
            sfilter.setChains(chain.name)
            if log.isEnabledFor(logging.DEBUG):
                log.debug("Filter %s already exists", filter_name)
                log.debug("Adding chain %s to %s", chain.name,sfilter.algname)
                log.debug(sfilter.getChains())


        elif n_filters == 0:
            sfilter = RoRSequenceFilterNode(name=filter_name)
            for i in previous_seeds: sfilter.addSeed(i)
            for i in filter_input: sfilter.addInput(i)                        
            filter_out=["%s_from_%s"%(filter_name,i) for i in filter_input]
            for o in filter_out: sfilter.addOutput(o)            
            sfilter.setChains(chain.name)
            if log.isEnabledFor(logging.DEBUG):
                log.debug("Adding these seeds to filter: %s", previous_seeds)
                log.debug("Filter Done: %s", sfilter.name)
        else:
            log.error("found %d filters  with name %s", n_filters, filter_name)
            sys.exit("ERROR, in filter configuration")

        return (sfilter,n_filters)





    
