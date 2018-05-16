# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import dumpSequence
from TrigUpgradeTest.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot

import sys

log = logging.getLogger('HLTCFConfig')

#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, decisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summary = TriggerSummaryAlg( name, OutputLevel = 2 )
    summary.InputDecision = "HLTChains"
    summary.FinalDecisions = decisions
    summary.HLTSummary = "MonitoringSummary"+name
    return summary



def create_step_reco_node(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """
    log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name+"_reco")
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_step_filter_node(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """
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
    log.debug("\n * Create CFSequence %s *", CFseq.name)
    filterAlg=CFseq.filter.Alg

    subs=[]
    for seq in CFseq.menuSeq.recoSeqList:
        ath_sequence=seq.sequence.Alg
        subs.append(ath_sequence)

    stepReco = parOR(CFseq.name+"_reco", subs)
    seqAndView = seqAND(CFseq.name+"_view", [stepReco,CFseq.menuSeq.hypo.Alg ])      
    stepAnd = seqAND(CFseq.name, [ filterAlg, seqAndView ])
    return stepAnd


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

def decisionTree_From_Chains(HLTNode, chains):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """
    log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())
    HLTNodeName= HLTNode.name()

    # find nsteps
    NSTEPS=0
    for chain in chains:
        steps =len(chain.steps)
        if NSTEPS < steps:
            NSTEPS = steps
    log.debug("Run on %d steps", NSTEPS)
    
    from TrigUpgradeTest.MenuComponents import CFSequence, RoRSequenceFilterNode
    #loop over chains to configure
    count_steps=0
#    finalDecisions = [] # needed for monitor
    allSeq_list = []
    for nstep in range(0, NSTEPS):
        stepCF_name =  "Step%i"%(nstep+1)
        CFseq_list = []
        step_decisions = []
        for chain in chains:
            if len(chain.steps) <= count_steps:
                continue
            chain_step=chain.steps[count_steps]
            chain_step_name= "%s:%s"%(stepCF_name, chain.name)
            log.debug("\n*******Filling step %s for chain  %s", stepCF_name, chain.name)
      
            sequenceHypoTools=chain_step.sequences
            hypotool=chain.hypoToolName
            countseq=0
            for sequence in sequenceHypoTools:
                cfseq_name= sequence.name
                log.debug("Going through sequence %s with threshold %s", sequence.name, hypotool)

                seeds= [recoSeq.seed for recoSeq in sequence.recoSeqList]
                log.debug("Found these seeds form the sequence: %s", seeds)
                #define sequence input
                if count_steps == 0: # L1 seeding                   
                    filter_input= seeds
                    log.debug("Seeds from this chain: %s", filter_input)
                    previous_sequence="".join(chain.group_seed)
                    previous_seeds=seeds                                        
                else:
                    # from previous step, map the seuqence in the same order?
                    prev=chain.steps[count_steps-1].sequences[countseq]
                    previous_sequence = prev.name #menu sequence
                    filter_input      = prev.outputs
                    log.debug("Connect to previous sequence through these filter inputs:")
                    log.debug(filter_input)
                    previous_seeds = chain.steps[count_steps-1].sequences[countseq].seeds
                    if len(filter_input) != len(previous_seeds):
                        log.error("found %d filter inputs and %d seeds", len(filter_input), len(previous_seeds))
                        sys.exit("ERROR, in size") 

               
                hypoAlg= sequence.hypo
                hypoToolClassName= sequence.hypoToolClassName
                log.debug("Adding HypoTool::%s with name %s to %s", 
                          hypoToolClassName,chain.hypoToolName, hypoAlg.algname)
                hypoAlg.addHypoTool(chain.hypoToolName, hypoToolClassName)
                addChainToHypoAlg(hypoAlg, chain.name) # only for TMP Combo


                #### Build the FILTER
                # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
                # if the previous hypo has more than one output, try to get all of them
                # one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
                filter_name="Filter%s_on_%s"%(stepCF_name,previous_sequence)
                filter_already_exists=False
                findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.algname]        
                if len(findFilter):
                    log.debug("Filter %s already exists", filter_name)
                    filter_already_exists=True
                    sfilter=findFilter[0]
                    log.debug("Adding chain %s to %s", chain.name,sfilter.algname)
                    sfilter.setChains(chain.name)
                    log.debug(sfilter.getChains())
                    continue
                else:
                    sfilter = RoRSequenceFilterNode(name=filter_name)
                    log.debug("Adding these seeds to filter: %s", previous_seeds)
                    for i in previous_seeds: sfilter.addSeed(i)
                    for i in filter_input: sfilter.addInput(i)                        
                    filter_out=["%s_from_%s_out"%(filter_name,i) for i in filter_input]
                    for o in filter_out: sfilter.setOutput(o)            

                    sfilter.setChains(chain.name)
                    log.debug("Filter Done: %s", sfilter.name)
                    
                # Connect the InputMaker
                #loop over RecoSequences of this sequence to add inputs to InputMaker and send decisions to HypoAlg
                for recoSeq in sequence.recoSeqList:
                    seed=recoSeq.seed
                    input_maker_input=[sfilter.getOutputList()[i] for i,fseed in enumerate (sfilter.seeds) if fseed in seed  ]
                    if log.isEnabledFor(logging.DEBUG):
                        log.debug("Adding %d inputs to sequence::%s from Filter::%s (from seed %s)",
                                  len(input_maker_input), recoSeq.name, sfilter.algname, seed)
                        for i in input_maker_input: log.debug(i)

                    if len(input_maker_input) == 0:
                        sys.exit("ERROR, no inputs to sequence are set!") 
                    for i in input_maker_input: recoSeq.addInput(i)
                    input_maker_output=["%s_from_%s_output"%(recoSeq.maker.algname,i)  for i in input_maker_input  ]

                    if len(input_maker_output) == 0:
                        sys.exit("ERROR, no outputs to sequence are set!") 
                    if log.isEnabledFor(logging.DEBUG):
                        log.debug("connecting InputMaker to HypoAlg")
                        log.debug("Adding %d output to InputMaker::%s and sending to HypoAlg::%s",
                                  len(input_maker_output), recoSeq.maker.algname, sequence.hypo.algname)
                        for i in input_maker_output: log.debug(i)
                   
                    for out in input_maker_output:
                          recoSeq.addOutputDecision(out) 
                          sequence.hypo.setPreviousDecision(out)

                #needed for the summary
                step_decisions.extend(sequence.outputs)                
                                    
                CF_seq = CFSequence( cfseq_name, FilterAlg=sfilter, MenuSequence=sequence)
                log.debug(CF_seq)
                CFseq_list.append(CF_seq)
                countseq+=1
           
            #end of sequence/threshold
                
        #end of loop over chains for this step, now implement CF:
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
        
        # update final decisions
#        finalDecisions=step_decisions

        log.debug("Now Draw...")
        stepCF_DataFlow_to_dot("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)
       
        count_steps+=1
        log.info("************* End of step %s", stepCF_name)
        # end of steps



   # summary=makeSummary("TriggerSummaryFinal", finalDecisions)
   # HLTNode += summary
#    dumpSequence (HLTNode, indent=0)
    all_DataFlow_to_dot(HLTNodeName, allSeq_list)
    return
