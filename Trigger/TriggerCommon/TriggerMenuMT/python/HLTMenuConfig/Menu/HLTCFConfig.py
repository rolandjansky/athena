# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import dumpSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming

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
    summary.InputDecision = "L1DecoderSummary"  
    summary.FinalDecisions = flatDecisions
    return summary


def makeStreamESD(name, flatDecisions):
    """ Creates output stream for given decisions """
    import AthenaPoolCnvSvc.WriteAthenaPool
    from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
    StreamESD = createOutputStream(name,"myESD.pool.root",True)
    StreamESD.OutputLevel=3
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.remove( StreamESD )
    def addTC(name):   
        StreamESD.ItemList += [ "xAOD::TrigCompositeContainer#"+name, "xAOD::TrigCompositeAuxContainer#"+name+"Aux." ]

    for tc in flatDecisions:
        addTC( tc )

    addTC("HLTSummary")
    log.debug("ESD file content: ")
    log.debug( StreamESD.ItemList  )
    return StreamESD


def createStepRecoNode(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """

    log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name + CFNaming.RECO_POSTFIX)
    for seq in seq_list:        
        stepCF += createCFTree(seq)
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def createStepFilterNode(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """

    log.debug("Create filter step %s with %d filters", name, len(seq_list))
    stepCF = parOR(name + CFNaming.FILTER_POSTFIX)
    for seq in seq_list:
        filterAlg = seq.filter.Alg
        log.info("Add  %s to filter node %s", filterAlg.name(), name)
        stepCF += filterAlg
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def createCFTree(CFseq):
    """ Creates AthSequencer nodes with sequences attached """

    log.debug(" *** Create CF Tree for CFSequence %s", CFseq.step.name)

    filterAlg = CFseq.filter.Alg
    stepReco = parOR(CFseq.step.name + CFNaming.RECO_POSTFIX)  # all reco algoritms from al lthe sequences in a parallel sequence
    seqAndView = seqAND(CFseq.step.name + CFNaming.VIEW_POSTFIX, [stepReco])  # include in seq:And to run in views: add here the Hypo
    seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg, seqAndView])  # add to the main step+filter

    already_connected = []
    for menuseq in CFseq.step.sequences:
        ath_sequence = menuseq.sequence.Alg
        name = ath_sequence.name()
        if name in already_connected:
            log.debug("AthSequencer %s already in the Tree, not added again",name)
        else:
            already_connected.append(name)
            stepReco += ath_sequence
        seqAndView += menuseq.hypo.Alg

    if CFseq.step.isCombo:
        seqAndView += CFseq.step.combo.Alg

    return seqAndWithFilter



#######################################
## CORE of Decision Handling
#######################################



def makeHLTTree(HLTChains):
    """ creates the full HLT tree"""

    # lock flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()

    # get topSequnece
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    #add the L1Upcacking
    #    TopHLTRootSeq += L1UnpackingSeq

    # connect to L1Decoder
    l1decoder = [ d for d in topSequence.getChildren() if d.getType() == "L1Decoder" ]
    if len(l1decoder)  != 1 :
        raise RuntimeError(" Can't find 1 instance of L1Decoder in topSequence, instead found this in topSequence "+str(topSequence.getChildren()) )


    # set CTP chains before creating the full tree (and the monitor)
    EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in HLTChains])
    l1decoder[0].ChainToCTPMapping = EnabledChainNamesToCTP

    # main HLT top sequence
    hltTop = seqOR("hltTop")
 
    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop +=  steps
    
    # make CF tree
    finalDecisions = decisionTree_From_Chains(steps, HLTChains)
    
    # make Final Summary
    flatDecisions=[]
    for step in finalDecisions: flatDecisions.extend (step)
    summary= makeSummary("TriggerSummaryFinal", flatDecisions)
    #from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
    #edmCreator = HLTEDMCreator()    
    #edmCreator.TrigCompositeContainer = flatDecisions
    #summary.OutputTools= [ edmCreator ]
    hltTop += summary

    # add signature monitor
    from TriggerJobOpts.TriggerConfig import collectHypos, triggerMonitoringCfg, triggerSummaryCfg
    hypos = collectHypos(steps)
    summaryAcc, summaryAlg = triggerSummaryCfg( ConfigFlags, hypos )
    hltTop += summaryAlg
    summaryAcc.appendToGlobals()
    
    monAcc, monAlg = triggerMonitoringCfg( ConfigFlags, hypos, l1decoder[0] )
    monAcc.appendToGlobals()    
    hltTop += monAlg
    #hltTop += makeStreamESD("StreamESD", flatDecisions)
    
    topSequence += hltTop


def matrixDisplay( allSeq ):
    from collections import defaultdict
    longestName = 5
    mx = defaultdict(lambda: dict())
    for stepNumber,step in enumerate(allSeq, 1):
        for seq in step:
            mx[stepNumber][seq.step.name] = seq
            
            longestName = max(longestName, len(seq.step.name) )
            
    longestName = longestName + 1
    def __getHyposOfStep( s ):
        return s.step.sequences[0].hypo.tools
    

    
    def __nextSteps( index, stepName ):
        nextStepName = "Step%s_"%index + "_".join(stepName.split("_")[1:])
        for sname, seq in mx[index].iteritems():
            if sname == nextStepName:
                return sname.ljust( longestName ) + __nextSteps( index + 1, nextStepName )
        return ""
            
    from pprint import pprint
    log.debug("" )
    log.debug("chains^ vs steps ->")
    log.debug( "="*90 )    
    for sname, seq in mx[1].iteritems():
        guessChainName = '_'.join( sname.split( "_" )[1:] )
        log.debug( " Reco chain: %s: %s" % ( guessChainName.rjust(longestName),  __nextSteps( 1, sname ) ) )
        log.debug( " "+ " ".join( __getHyposOfStep( seq ) ) )
        log.debug( "" )
        
    log.debug( "="*90 )
    log.debug( "" )

        

def decisionTree_From_Chains(HLTNode, chains):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """

    log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence, RoRSequenceFilterNode, ComboMaker
    HLTNodeName= HLTNode.name()

    # find nsteps
    chainWithMaxSteps = max(chains, key=lambda chain: len(chain.steps))
    NSTEPS = len(chainWithMaxSteps.steps)

    #loop over chains to configure hypotools
    # must be done after all chains are created, to avoid conflicts 
    log.debug("Loop over chains to decode hypo tools")
    for chain in chains:
        chain.decodeHypoToolConfs()

    finalDecisions = [] # needed for monitor
    allSeq_list = []

    for nstep in range(0, NSTEPS):
        finalDecisions.append([]) # list of final deciisons per step
        stepCF_name =  CFNaming.stepName(nstep)
        CFseq_list = []
        step_decisions = []

        for chain in chains:
            # skip this step if missing
            if len(chain.steps) <= nstep:               
                continue

            chain_step=chain.steps[nstep]
            log.debug("\n************* Start step %d %s for chain %s", nstep+1, stepCF_name, chain.name)
            # one filter per ChainStep - same name indeed
            # one filter input per previous menusequence output (the L1Seed for first step)
            # one filter output per menuSeq
            
            filter_input =[]
            if nstep == 0: # L1 seeding
                seeds = chain.group_seed
                filter_input.extend( seeds )
                log.debug("Found these seeds from the chain: %s", seeds)
                log.debug("Seeds added; having in the filter now: %s", filter_input)
            else:
                prev = chain.steps[nstep-1].sequences
                ## for seq in prev:                   
                ##     filter_input.extend(seq.outputs)
                ## log.debug("Connect to previous sequence through these filter inputs: %s" %str( filter_input) )


                # previous filter name
                pre_filter_name = CFNaming.filterName(chain.steps[nstep-1].name)
                # searching the correct sequence outut to connect to the filter
                for seq in prev:
                    for out in seq.outputs:
                        if pre_filter_name in out:
                            filter_input.append(out)
                            log.debug("Connect to previous sequence through these filter inputs: %s" %str( filter_input) )

            if len(filter_input) == 0 or (len(filter_input) != 1 and not chain_step.isCombo):
                log.error("ERROR: Filter for step %s has %d inputs! One is expected"%(chain_step.name, len(filter_input)))
                sys.exit("ERROR, in configuration of sequence "+seq.name)
                    

            # get the filter:
            filter_name = CFNaming.filterName(chain_step.name)
            (foundFilter, sfilter) = findFilter(filter_name, CFseq_list)
            if not foundFilter:           
                sfilter = buildFilter(filter_name, filter_input)
                CF_seq = CFSequence( ChainStep=chain_step, FilterAlg=sfilter)
                CFseq_list.append(CF_seq)
                for sequence in chain_step.sequences:                
                    step_decisions.extend(sequence.outputs)
            else:
                log.debug("foundFilter %s", filter_name)

            sfilter.setChains(chain.name)
            log.debug("Adding chain %s to %s", chain.name,sfilter.Alg.name())
            log.debug(sfilter.getChains())

            if len(chain.steps) == nstep+1:  
                log.debug("Adding finalDecisions for chain %s at step %d:"%(chain.name, nstep+1))
                for seq in chain_step.sequences:
                    finalDecisions[nstep].extend(seq.outputs)
                    log.debug(seq.outputs)
#            else:
#                log.debug("len(chain.steps) != nstep+1")
            #end of loop over menu sequences
                
        #end of loop over chains for this step, now implement CF:
    
        log.debug("\n******** Create CF Tree %s with AthSequencers", stepCF_name)
        
        #first make the filter step
        stepFilter = createStepFilterNode(stepCF_name, CFseq_list, dump=False)
        HLTNode += stepFilter
        allSeq_list.append(CFseq_list)

        # then the reco step
        recoNodeName = CFNaming.stepRecoNodeName(HLTNodeName, stepCF_name)
        stepCF = createStepRecoNode(recoNodeName, CFseq_list, dump=False)
        HLTNode += stepCF


        # then the monitor summary
        summary=makeSummary(CFNaming.stepSummaryName(stepCF_name), step_decisions)
        HLTNode += summary


        log.debug("Now Draw...")
        stepCF_DataFlow_to_dot(recoNodeName, CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)

        log.info("************* End of step %d, %s", nstep+1, stepCF_name)
        # end of steps



    log.debug("finalDecisions: %s" %str( finalDecisions) )
    all_DataFlow_to_dot(HLTNodeName, allSeq_list)

    # matrix display
    matrixDisplay( allSeq_list )


    return finalDecisions


def generateDecisionTree(HLTNode, chains):
    log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())

    from collections import defaultdict
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence

    chainStepsMatrix = defaultdict(lambda: defaultdict(lambda: list()))

    ## Fill chain steps matrix
    for chain in chains:
        chain.decodeHypoToolConfs()
        for stepNumber, chainStep in enumerate(chain.steps):
            chainName = chainStep.name.split('_')[0]
            chainStepsMatrix[stepNumber][chainName].append(chain)

    allSequences = []

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in chainStepsMatrix:
        CFsequences = []
        stepDecisions = []

        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                continue

            firstChain = chainsInCell[0]

            if nstep == 0:
                filter_input = firstChain.group_seed
#                previous_seeds = firstChain.group_seed

            else:
                filter_input = []
                for sequence in firstChain.steps[nstep - 1].sequences:
                    filter_input += sequence.outputs

            # One aggregated filter per chain (one per column in matrix)
            filterName = 'Filter_{}'.format( firstChain.steps[nstep].name )
            sfilter = buildFilter(filterName,  filter_input)

            chainStep = firstChain.steps[nstep]
            CFseq = CFSequence( ChainStep=chainStep, FilterAlg=sfilter )
            CFsequences.append( CFseq )

            for sequence in chainStep.sequences:
                stepDecisions += sequence.outputs

            for chain in chainsInCell:
                sfilter.setChains(chain.name)

        allSequences.append(CFsequences)

        stepName = 'Step{}'.format(nstep)
        stepFilter = createStepFilterNode(stepName, CFsequences, dump=False)
        stepCF = createStepRecoNode('{}_{}'.format(HLTNode.name(), stepName), CFsequences, dump=False)
        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)

        HLTNode += stepFilter
        HLTNode += stepCF
        HLTNode += summary

        stepCF_DataFlow_to_dot('{}_{}'.format(HLTNode.name(), stepName), CFsequences)
        stepCF_ControlFlow_to_dot(stepCF)

    all_DataFlow_to_dot(HLTNode.name(), allSequences)

    matrixDisplay( allSequences )


def findFilter(filter_name, cfseqList):
      """
      searches for a filter, with given name, in the CF sequence list of this step
      """
      log.debug( "findFilter: filter name %s" % filter_name )
      foundFilters = [cfseq.filter for cfseq in cfseqList if filter_name in cfseq.filter.Alg.name()]
      if len(foundFilters) > 1:
          log.error("found %d filters  with name %s", len( foundFilters ), filter_name)
          sys.exit("ERROR, in filter configuration")

      found = bool(foundFilters)
      if found:          
          log.debug("Filter %s already exists", filter_name)        
          return (found, foundFilters[0])
      return (found, None)



def buildFilter(filter_name,  filter_input):
    """
     Build the FILTER
     one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
     if the previous hypo has more than one output, try to get all of them
     one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import  RoRSequenceFilterNode       
    sfilter = RoRSequenceFilterNode(name=filter_name)
    for i in filter_input: sfilter.addInput(i)
    for i in filter_input: sfilter.addOutput(CFNaming.filterOutName(filter_name, i))
        
    log.debug("Added inputs to filter: %s", sfilter.getInputList())
    log.debug("Added outputs to filter: %s", sfilter.getOutputList())
    log.debug("Filter Done: %s", sfilter.Alg.name())

    return (sfilter)





    
