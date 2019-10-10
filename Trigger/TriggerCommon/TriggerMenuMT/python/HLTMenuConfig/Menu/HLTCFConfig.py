# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
    ------ Documentation on HLT Tree creation -----

++ Filter creation strategy

++ Connections between InputMaker/HypoAlg/Filter

++ Seeds

++ Combined chain strategy

- The combined chains use duplicates of the single-object-HypoAlg, called HypoAlgName_for_stepName.
  These duplicates are connected to a dedicated ComboHypoAlg (added by the framework), able to count object multiplicity
     -- This is needed for two reasons:
           -- the HypoAlg is designed to have only one input TC (that is already for the single object)
           -- otherwise the HypoAlg would be equipped with differnt HypoTools with the same name (see for example e3_e8)
     -- If the combined chain is symmetric (with multiplicity >1), the Hypo is duplicated only once,
        equipped with a HypoTool configured as single object and followed by one ComboHypoAlg





"""

# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import dumpSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep

import copy
log = logging.getLogger('HLTCFConfig')


#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, flatDecisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summary = TriggerSummaryAlg( name )
    summary.InputDecision = "L1DecoderSummary"
    summary.FinalDecisions = flatDecisions
    return summary


def createStepRecoNode(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """

    log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name + CFNaming.RECO_POSTFIX)
    for seq in seq_list:
        stepCF += createCFTree(seq)

    if dump:
        dumpSequence (stepCF, indent=0)
    return stepCF


def createStepFilterNode(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """

    log.debug("Create filter step %s with %d filters", name, len(seq_list))
    filter_list=[]
    for seq in seq_list:
        filterAlg = seq.filter.Alg
        log.info("createStepFilterNode: Add  %s to filter node %s", filterAlg.name(), name)
        filter_list.append(filterAlg)


    stepCF = parOR(name + CFNaming.FILTER_POSTFIX, subs=set(filter_list))

    if dump:
        dumpSequence (stepCF, indent=0)
    return stepCF


def createCFTree(CFseq):
    """ Creates AthSequencer nodes with sequences attached """

    log.debug(" *** Create CF Tree for CFSequence %s", CFseq.step.name)
    filterAlg = CFseq.filter.Alg

    #empty step:
    if len(CFseq.step.sequences)==0:
        seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg])
        return seqAndWithFilter

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
        if type(menuseq.hypo) is list:
           for hp in menuseq.hypo:
              seqAndView += hp.Alg
        else:
           seqAndView += menuseq.hypo.Alg

    if CFseq.step.isCombo:
        seqAndView += CFseq.step.combo.Alg

    return seqAndWithFilter



#######################################
## CORE of Decision Handling
#######################################

def makeHLTTree(newJO=False, triggerConfigHLT = None):
    """ creates the full HLT tree"""

    # Check if triggerConfigHLT exits, if yes, derive information from this
    # this will be in use once TrigUpgrade test has migrated to TriggerMenuMT completely

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

    # take L1Decoder out of topSeq
    topSequence.remove( l1decoder )

    # main HLT top sequence
    hltTop = seqOR("HLTTop")

    # put L1Decoder here
    hltTop += l1decoder

    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop +=  steps

    # make DF and CF tree from chains
    finalDecisions = decisionTree_From_Chains(steps, triggerConfigHLT.configsList(), triggerConfigHLT.dictsList(), newJO)

    flatDecisions=[]
    for step in finalDecisions:
        flatDecisions.extend (step)

    summary= makeSummary("TriggerSummaryFinal", flatDecisions)
    hltTop += summary


    # add signature monitor
    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectViewMakers, collectDecisionObjects,\
        triggerMonitoringCfg, triggerSummaryCfg, triggerMergeViewsAndAddMissingEDMCfg
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    hypos = collectHypos(steps)
    filters = collectFilters(steps)
    viewMakers = collectViewMakers(steps)
    decObj = collectDecisionObjects( hypos, filters, l1decoder[0] )
    summaryAcc, summaryAlg = triggerSummaryCfg( ConfigFlags, hypos )
    hltTop += summaryAlg
    summaryAcc.appendToGlobals()

    monAcc, monAlg = triggerMonitoringCfg( ConfigFlags, hypos, filters, l1decoder[0] )
    monAcc.appendToGlobals()
    hltTop += monAlg

    # this is a shotcut for now, we always assume we may be writing ESD & AOD outputs, so all gaps will be filled
    edmAlg = triggerMergeViewsAndAddMissingEDMCfg(['AOD', 'ESD'], hypos, viewMakers, decObj )
    hltTop += edmAlg

    Configurable.configurableRun3Behavior=0

    topSequence += hltTop

    # Test the configuration
    from TriggerMenuMT.HLTMenuConfig.Menu.CFValidation import testHLTTree
    testHLTTree( topSequence )


def matrixDisplay( allCFSeq ):
    from collections import defaultdict
    longestName = 5
    mx = defaultdict(lambda: dict())
    for stepNumber,step in enumerate(allCFSeq, 1):
        for seq in step:
            mx[stepNumber][seq.step.name] = seq # what if ther eare more sequences in one step?

            longestName = max(longestName, len(seq.step.name) )

    longestName = longestName + 1
    def __getHyposOfStep( s ):
        if len(s.step.sequences):
           if type(s.step.sequences[0].hypo) is list:
              return s.step.sequences[0].hypo[0].tools
           else:
              return s.step.sequences[0].hypo.tools
        return []



    def __nextSteps( index, stepName ):
        nextStepName = "Step%s_"%index + "_".join(stepName.split("_")[1:])
        for sname, seq in mx[index].iteritems():
            if sname == nextStepName:
                return sname.ljust( longestName ) + __nextSteps( index + 1, nextStepName )
        return ""

    log.debug("" )
    log.debug("chains^ vs steps ->")
    log.debug( "="*90 )
    for sname, seq in mx[1].iteritems():
        guessChainName = '_'.join( sname.split( "_" )[1:] )
        log.debug( " Reco chain: %s: %s", guessChainName.rjust(longestName),  __nextSteps( 1, sname ) )
        log.debug( " %s", " ".join( __getHyposOfStep( seq ) ) )
        log.debug( "" )

    log.debug( "%s", "="*90 )
    log.debug( "" )



def decisionTree_From_Chains(HLTNode, chains, allDicts, newJO):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """

    log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())
    HLTNodeName= HLTNode.name()
    if len(chains) == 0:
        log.info("Configuring empty decisionTree")
        return []

    # add chains to multiplicity map (new step here, as this was originally in the __init__ of Chain class

    (finalDecisions, CFseq_list) = createDataFlow(chains, allDicts)
    if not newJO:
        createControlFlow(HLTNode, CFseq_list)
    else:
        from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import createControlFlowNewJO
        createControlFlowNewJO(HLTNode, CFseq_list)

    log.debug("finalDecisions: %s", finalDecisions)
    all_DataFlow_to_dot(HLTNodeName, CFseq_list)

    # matrix display
    matrixDisplay( CFseq_list )

    return finalDecisions


def createDataFlow(chains, allDicts):
    """ Creates the filters and connect them to the menu sequences"""

    # find tot nsteps
    chainWithMaxSteps = max(chains, key=lambda chain: len(chain.steps))
    NSTEPS = len(chainWithMaxSteps.steps)

    log.debug("createDataFlow for %d chains and total %d steps", len(chains), NSTEPS)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
    # initialize arrays for monitor
    finalDecisions = []
    CFseq_list = []
    for nstep in range(0, NSTEPS):
        finalDecisions.append([]) # list of final deciisons per step
        CFseq_list.append([]) # list of all CFseq per step


    # loop over chains
    for chain in chains:
        log.info("\n Configuring chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))

        lastCFseq = None
        for nstep in range(0,len(chain.steps)):
#            stepCF_name =  CFNaming.stepName(nstep)
            chain_step=chain.steps[nstep]
            log.debug("\n************* Start connecting step %d %s for chain %s", nstep+1, chain_step.name, chain.name)

            filter_input =[]
            if nstep == 0: # L1 seeding
                seeds = chain.group_seed
                filter_input.extend( seeds )
                log.debug("Found these seeds from the chain: %s", seeds)
                log.debug("Seeds added; having in the filter now: %s", filter_input)
            else:
                for out in lastCFseq.decisions:
                    filter_input.append(out)
                    log.debug("Connect to previous sequence through these filter inputs: %s", filter_input)


            if len(filter_input) == 0 or (len(filter_input) != 1 and not chain_step.isCombo):
                log.error("ERROR: Filter for step %s has %d inputs! One is expected", chain_step.name, len(filter_input))


            # make one filter per step:
            sfilter= None
            filter_name = CFNaming.filterName(chain_step.name)
            filter_output =[]
            for i in filter_input:
                filter_output.append( CFNaming.filterOutName(filter_name, i))

            (foundFilter, foundCFSeq) = findCFSequences(filter_name, CFseq_list[nstep])
            log.debug("Found %d CF sequences with base filter name %s", foundFilter, filter_name)
            if not foundFilter:
                sfilter = buildFilter(filter_name, filter_input)
                CF_seq = CFSequence( ChainStep=chain_step, FilterAlg=sfilter, connections=filter_output)
                CFseq_list[nstep].append(CF_seq)
                lastCFseq=CF_seq
            else:
                count_fil=0
                # find correspoding CFsequence
                for cfseq in foundCFSeq: # all the CFseq using the same filter
                    sfilter=cfseq.filter
                    #exactly same filter with the same inputs (same gropu of chains);
                    already_connected = [x for x in filter_output if x in cfseq.connections]
                    if len(already_connected):
                        chain.steps[nstep] = cfseq.step # reuse the same step
                        chain_step=chain.steps[nstep]
                        lastCFseq=cfseq #reuse the CFseq
                        count_fil =0
                        break
                    else:
                        count_fil+=1

                # if we have the same filter, with differnt inputs:
                # add inputs/output to filter
                # deepcopy all the seqeunces
                # duplicate the Hypo with differnt name
                # create new ChainStep and replace in the list
                # create a new CFsequence with different name
                if (count_fil):
                    log.debug("Adding more inputs/outputs to filter %s", filter_name)
                    for i in filter_input:
                        sfilter.addInput(i)
                    for i in filter_output:
                        sfilter.addOutput(i)

                    log.debug("Duplicating the Step %s", chain_step.name)
                    new_sequences = []
                    for sequence in chain_step.sequences:
                        new_sequence=copy.deepcopy(sequence)
                        new_sequence.resetConnections()
                        new_sequence.name = "%s_%d"%(sequence.name, count_fil)
                        if type(sequence.hypo) is list:
                           new_hypoAlg = []
                           for hp in sequence.hypo:
                              oldhypo=hp.Alg
                              newHypoAlgName = "%s_%d"%(oldhypo.name(),count_fil)
                              new_hypoAlg.append( oldhypo.clone(newHypoAlgName) )
                           new_sequence.replaceHypoForDuplication(new_hypoAlg)
                           new_sequences.append(new_sequence)
                        else:
                           oldhypo=sequence.hypo.Alg
                           newHypoAlgName = "%s_%d"%(oldhypo.name(),count_fil)
                           new_hypoAlg=oldhypo.clone(newHypoAlgName)
                           new_sequence.replaceHypoForDuplication(new_hypoAlg)
                           new_sequences.append(new_sequence)

                    new_chain_step_name="%s_%d"%(chain_step.name, count_fil)
                    # making new ChainStep
                    new_chain_step = ChainStep(new_chain_step_name, Sequences=new_sequences,  multiplicity=chain_step.multiplicity)
                    chain.steps[nstep] = new_chain_step # replace chain step
                    chain_step = chain.steps[nstep]

                    new_CF_seq = CFSequence( ChainStep=new_chain_step, FilterAlg=sfilter, connections=filter_output)
                    CFseq_list[nstep].append(new_CF_seq)
                    lastCFseq=new_CF_seq


            sfilter.setChains(chain.name)
            log.debug("Adding chain %s to %s", chain.name,sfilter.Alg.name())
            log.debug("Now Filter has chains: %s", sfilter.getChains())

            if chain_step.isCombo:
                if chain_step.combo is not None:
                    chain_step.combo.addChain( [d for d in allDicts if d['chainName'] == chain.name ][0])
                    log.debug("Added chains to ComboHypo: %s",chain_step.combo.getChains())


            if len(chain.steps) == nstep+1:
                log.debug("Adding finalDecisions for chain %s at step %d:", chain.name, nstep+1)
                for seq in chain_step.sequences:
                    finalDecisions[nstep].extend(seq.outputs)
                    log.debug(seq.outputs)


        #end of loop over steps
        log.info("\n Built CF for chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))
    #end of loop over chains

    # decode and attach HypoTools:
    #must be done after the creation of the CFsequences, since HypoAlgs are duplicated in the previous loop
    for chain in chains:
        chain.decodeHypoToolConfs(allDicts)


    return (finalDecisions, CFseq_list)


def createControlFlow(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences"""

    HLTNodeName= HLTNode.name()
    log.debug("createControlFlow on node %s",HLTNodeName)

    for nstep in range(0, len(CFseq_list)):
        step_decisions = []
        for CFseq in CFseq_list[nstep]:
            step_decisions.extend(CFseq.decisions)

        stepCF_name =  CFNaming.stepName(nstep)
        log.debug("\n******** Create CF Tree %s with AthSequencers", stepCF_name)

        #first make the filter step
        stepFilter = createStepFilterNode(stepCF_name, CFseq_list[nstep], dump=False)
        HLTNode += stepFilter

        # then the reco step
        recoNodeName = CFNaming.stepRecoNodeName(HLTNodeName, stepCF_name)
        stepCF = createStepRecoNode(recoNodeName, CFseq_list[nstep], dump=False)
        HLTNode += stepCF


        # then the monitor summary
        summary=makeSummary(CFNaming.stepSummaryName(stepCF_name), step_decisions)
        HLTNode += summary

        log.debug("Now Draw...")
        stepCF_DataFlow_to_dot(recoNodeName, CFseq_list[nstep])
        stepCF_ControlFlow_to_dot(stepCF)

        log.info("************* End of step %d, %s", nstep+1, stepCF_name)

    return




"""
Not used, kept for reference and testing purposes
To be removed in future
"""
def generateDecisionTreeOld(HLTNode, chains, allChainDicts):
    log.debug("Run decisionTree_From_Chains on %s", HLTNode.name())
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from collections import defaultdict
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence

    chainStepsMatrix = defaultdict(lambda: defaultdict(lambda: list()))

    ## Fill chain steps matrix
    for chain in chains:
        chain.decodeHypoToolConfs(allChainDicts)
        for stepNumber, chainStep in enumerate(chain.steps):
            chainName = chainStep.name.split('_')[0]
            chainStepsMatrix[stepNumber][chainName].append(chain)

    allSequences = []

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in chainStepsMatrix:
        CFsequences = []
        stepDecisions = []
        stepAccs = []
        stepHypos = []

        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                continue


            stepCategoryAcc = ComponentAccumulator()

            stepHypo = None

            for chain in chainsInCell:
                for seq in chain.steps[nstep].sequences:
                    if seq.ca:
                        stepCategoryAcc.merge( seq.ca )

                    alg = seq.hypo.Alg
                    if stepHypo is None:
                        stepHypo = alg
                        stepHypos.append( alg )
                    stepCategoryAcc.addEventAlgo( alg )

            stepAccs.append( stepCategoryAcc )

            stepCategoryAcc.printConfig( True, True )
            firstChain = chainsInCell[0]

            if nstep == 0:
                filter_input = firstChain.group_seed
            else:
                filter_input = []
                for sequence in firstChain.steps[nstep - 1].sequences:
                    filter_input += sequence.outputs

            # One aggregated filter per chain (one per column in matrix)
            filterName = 'Filter_{}'.format( firstChain.steps[nstep].name )
            filter_output =[]
            for i in filter_input:
                filter_output.append( CFNaming.filterOutName(filterName, i))
            sfilter = buildFilter(filterName,  filter_input)

            chainStep = firstChain.steps[nstep]

            CFseq = CFSequence( ChainStep=chainStep, FilterAlg=sfilter, connections=filter_output )
            CFsequences.append( CFseq )


            for sequence in chainStep.sequences:
                stepDecisions += sequence.outputs

            for chain in chainsInCell:
                sfilter.setChains(chain.name)

        allSequences.append(CFsequences)

        stepName = 'Step{}'.format(nstep)
        stepFilter = createStepFilterNode(stepName, CFsequences, dump=False)
        stepCF = createStepRecoNode('{}_{}'.format(HLTNode.name(), stepName), CFsequences, dump=False)

        from AthenaCommon.CFElements import findOwningSequence
        for oneAcc, cfseq, hypo in zip( stepAccs, CFsequences, stepHypos):
            owning = findOwningSequence( stepCF, hypo.getName() )
            acc.addSequence( owning )
            acc.merge( oneAcc, sequenceName = owning.getName() )
        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)

        HLTNode += stepFilter
        HLTNode += stepCF
        HLTNode += summary

        stepCF_DataFlow_to_dot('{}_{}'.format(HLTNode.name(), stepName), CFsequences)
        stepCF_ControlFlow_to_dot(stepCF)

    all_DataFlow_to_dot(HLTNode.name(), allSequences)
    matrixDisplay( allSequences )
    return acc



def findFilter(filter_name, cfseqList):
      """
      searches for a filter, with given name, in the CF sequence list of this step
      """
      log.debug( "findFilter: filter name %s", filter_name )
      foundFilters = [cfseq for cfseq in cfseqList if filter_name in cfseq.filter.Alg.name()]
      #foundFilters = [cfseq.filter for cfseq in cfseqList if filter_name in cfseq.filter.Alg.name()]
      if len(foundFilters) > 1:
          log.error("found %d filters  with name %s", len( foundFilters ), filter_name)

      found = bool(foundFilters)
      if found:
          log.debug("Filter %s already exists", filter_name)
          return (found, foundFilters[0])
      return (found, None)

def findCFSequences(filter_name, cfseqList):
      """
      searches for a filter, with given name, in the CF sequence list of this step
      """
      log.debug( "findCFSequences: filter base name %s", filter_name )
      foundFilters = [cfseq for cfseq in cfseqList if filter_name in cfseq.filter.Alg.name()]
      #foundFilters = [cfseq.filter for cfseq in cfseqList if filter_name in cfseq.filter.Alg.name()]

      log.debug("found %d filters with base name %s", len( foundFilters ), filter_name)

      found=len(foundFilters)
      if found:
          return (found, foundFilters)
      return (found, None)


def buildFilter(filter_name,  filter_input):
    """
     Build the FILTER
     one filter per previous sequence at the start of the sequence: always create a new one
     if the previous hypo has more than one output, try to get all of them
     one filter per previous sequence: 1 input/previous seq, 1 output/next seq
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import  RoRSequenceFilterNode
    sfilter = RoRSequenceFilterNode(name=filter_name)
    for i in filter_input:
        sfilter.addInput(i)
    for i in filter_input:
        sfilter.addOutput(CFNaming.filterOutName(filter_name, i))

    log.debug("Added inputs to filter: %s", sfilter.getInputList())
    log.debug("Added outputs to filter: %s", sfilter.getOutputList())
    log.debug("Filter Done: %s", sfilter.Alg.name())

    return (sfilter)
