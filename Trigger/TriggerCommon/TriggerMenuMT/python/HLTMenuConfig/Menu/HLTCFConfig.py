# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from builtins import zip
from builtins import str
from builtins import map
from builtins import range
from collections import OrderedDict
# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.AlgSequence import dumpSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot, create_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from AthenaCommon.Configurable import Configurable
from AthenaCommon.CFElements import getSequenceChildren, isSequence, compName
import re


from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, flatDecisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConfig import TriggerSummaryAlg    
    summary = TriggerSummaryAlg( CFNaming.stepSummaryName(name) )
    summary.InputDecision = "L1DecoderSummary"
    summary.FinalDecisions = list(OrderedDict.fromkeys(flatDecisions))
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
        log.debug("createStepFilterNode: Add  %s to filter node %s", filterAlg.name(), name)
        if filterAlg not in filter_list:
            filter_list.append(filterAlg)


    stepCF = parOR(name + CFNaming.FILTER_POSTFIX, subs=filter_list)

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

    stepReco = parOR(CFseq.step.name + CFNaming.RECO_POSTFIX)  # all reco algorithms from all the sequences in a parallel sequence
    seqAndView = seqAND(CFseq.step.name + CFNaming.VIEW_POSTFIX, [stepReco])  # include in seq:And to run in views: add here the Hypo
    seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg, seqAndView])  # add to the main step+filter

    recoSeqSet=set()
    hypoSet=set()
    for menuseq in CFseq.step.sequences:
        menuseq.addToSequencer(recoSeqSet,hypoSet)
  
    stepReco   += sorted(list(recoSeqSet), key=lambda t: t.name())
    seqAndView += sorted(list(hypoSet), key=lambda t: t.name()) 
    if CFseq.step.combo is not None: 
        seqAndView += CFseq.step.combo.Alg

    return seqAndWithFilter


#######################################
## CORE of Decision Handling
#######################################

def makeHLTTree(newJO=False, triggerConfigHLT = None):
    """ creates the full HLT tree"""

    # Check if triggerConfigHLT exits, if yes, derive information from this
    # this will be in use once TrigUpgrade test has migrated to TriggerMenuMT completely

    # get topSequnece
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()


    # find main HLT top sequence (already set up in runHLT_standalone)
    from AthenaCommon.CFElements import findSubSequence,findAlgorithm
    l1decoder = findAlgorithm(topSequence, "L1Decoder")

    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop = findSubSequence(topSequence, "HLTTop")
    hltTop +=  steps

    hltEndSeq = parOR("HLTEndSeq")
    hltTop += hltEndSeq

    hltFinalizeSeq = seqAND("HLTFinalizeSeq")

    log.info("[makeHLTTree] will now make the DF and CF tree from chains")

    # make DF and CF tree from chains
    finalDecisions = decisionTreeFromChains(steps, triggerConfigHLT.configsList(), triggerConfigHLT.dictsList(), newJO)

    successful_scan = sequenceScanner( steps )
    
    if not successful_scan:
        raise Exception("[makeHLTTree] At least one sequence is expected in more than one step. Check error messages and fix!")    

    flatDecisions=[]
    for step in finalDecisions:
        flatDecisions.extend (step)

    summary = makeSummary("Final", flatDecisions)
    hltEndSeq += summary

    log.info("[makeHLTTree] created the final summary tree")
    # TODO - check we are not running things twice. Once here and once in TriggerConfig.py

    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectViewMakers, collectDecisionObjects,\
        triggerMonitoringCfg, triggerSummaryCfg, triggerMergeViewsAndAddMissingEDMCfg, collectHypoDecisionObjects
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena

    # Collections required to configure the algs below
    hypos = collectHypos(steps)
    filters = collectFilters(steps)
    viewMakers = collectViewMakers(steps)

    Configurable.configurableRun3Behavior=1
    summaryAcc, summaryAlg = triggerSummaryCfg( ConfigFlags, hypos )
    Configurable.configurableRun3Behavior=0
    # A) First we check if any chain accepted the event
    hltFinalizeSeq += conf2toConfigurable( summaryAlg )
    appendCAtoAthena( summaryAcc )

    # B) Then (if true), we run the accepted event algorithms.
    # Add any required algs to hltFinalizeSeq here

    # More collections required to configure the algs below
    decObj = collectDecisionObjects( hypos, filters, l1decoder, summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)

    Configurable.configurableRun3Behavior=1
    monAcc, monAlg = triggerMonitoringCfg( ConfigFlags, hypos, filters, l1decoder )
    Configurable.configurableRun3Behavior=0
    hltEndSeq += conf2toConfigurable( monAlg )
    appendCAtoAthena( monAcc )

    Configurable.configurableRun3Behavior=1
    edmAlg = triggerMergeViewsAndAddMissingEDMCfg(ConfigFlags, ['AOD', 'ESD'], hypos, viewMakers, decObj, decObjHypoOut)
    Configurable.configurableRun3Behavior=0
    # C) Finally, we create the EDM output
    hltFinalizeSeq += conf2toConfigurable(edmAlg)

    hltEndSeq += hltFinalizeSeq

    # Test the configuration
    from TriggerMenuMT.HLTMenuConfig.Menu.CFValidation import testHLTTree
    testHLTTree( hltTop )

    def debugDecisions(hypos, summary, mon, summaryAlg):
        """ set DEBUG flag to the hypos of all the sequences (used to debug), other debug functions can be added here """
        from GaudiKernel.Constants import DEBUG # noqa: ATL900
        for step, stepHypos in sorted(hypos.items()):
            for hypo in stepHypos:
                hypo.OutputLevel=DEBUG   # noqa:  ATL900
        summary.OutputLevel = DEBUG # noqa: ATL900
        mon.OutputLevel = DEBUG # noqa: ATL900
        summaryAlg.OutputLevel = DEBUG # noqa: ATL900



    # switch on  DEBUG ouput in some algorithms
    #debugDecisions(hypos, summary, monAlg, summaryAlg)


def matrixDisplayOld( allCFSeq ):
    from collections import defaultdict
    longestName = 5
    mx = defaultdict(lambda: dict())
    for stepNumber,step in enumerate(allCFSeq, 1):
        for seq in step:
            mx[stepNumber][seq.step.name] = seq # what if ther eare more sequences in one step?
            longestName = max(longestName, len(seq.step.name) )

    longestName = longestName + 1

    def __getHyposOfStep( s ):   
        if s.step.combo is not None:
            return list(s.step.combo.getChains())
        return []
   

    def __nextSteps( index, stepName ):
        nextStepName = "Step%s_"%index + "_".join(stepName.split("_")[1:])
        for sname, seq in mx[index].items():
            if sname == nextStepName:
                return sname.ljust( longestName ) + __nextSteps( index + 1, nextStepName )
        return ""

    log.debug("" )
    log.debug("chains^ vs steps ->")
    log.debug( "="*90 )
    for sname, seq in mx[1].items():
        guessChainName = '_'.join( sname.split( "_" )[1:] )
        log.debug( " Reco chain: %s: %s", guessChainName.rjust(longestName),  __nextSteps( 1, sname ) )
        log.debug( " %s", " ".join( __getHyposOfStep( seq ) ) )
        log.debug( "" )

    log.debug( "%s", "="*90 )
    log.debug( "" )


    
def matrixDisplay( allCFSeq ):

    def __getHyposOfStep( step ):
        if len(step.sequences):
            step.getChainNames()           
        return []
   
    # fill dictionary to cumulate chains on same sequences, in steps (dict with composite keys)
    from collections import defaultdict
    mx = defaultdict(list)

    for stepNumber,cfseq_list in enumerate(allCFSeq, 1):
        for cfseq in cfseq_list:
            chains = __getHyposOfStep(cfseq.step)
            for seq in cfseq.step.sequences:
                if seq.name == "Empty":
                    mx[stepNumber, "Empty"].extend(chains)
                else:
                    mx[stepNumber, seq.sequence.Alg.name()].extend(chains)

    # sort dictionary by fist key=step
    from collections import  OrderedDict
    sorted_mx = OrderedDict(sorted( list(mx.items()), key= lambda k: k[0]))

    log.debug( "" )
    log.debug( "="*90 )
    log.debug( "Cumulative Summary of steps")
    log.debug( "="*90 )
    for (step, seq), chains in list(sorted_mx.items()):
        log.debug( "(step, sequence)  ==> (%d, %s) is in chains: ",  step, seq)
        for chain in chains:
            log.debug( "              %s",chain)

    log.debug( "="*90 )


def sequenceScanner( HLTNode ):
    """ Checks the alignement of sequences and steps in the tree"""
    # +-- AthSequencer/HLTAllSteps
    #   +-- AthSequencer/Step1_filter
    #   +-- AthSequencer/Step1_reco

    from collections import defaultdict
    _seqMapInStep = defaultdict(set)
    _status = True

    def _mapSequencesInSteps(seq, stepIndex):
        """ Recursively finds the steps in which sequences are used"""
        if not isSequence(seq):
            return stepIndex
        name = compName(seq)                
        match=re.search('^Step([0-9])_filter',name)
        if match:
            stepIndex = match.group(1)
            log.debug("sequenceScanner: This is another step: %s %s", name, stepIndex)            
        for c in getSequenceChildren( seq ):
            if isSequence(c):
                stepIndex = _mapSequencesInSteps(c, stepIndex)
                _seqMapInStep[compName(c)].add(stepIndex)
        return stepIndex

    # do the job:
    final_step=_mapSequencesInSteps(HLTNode, 0)

    for alg, steps in _seqMapInStep.items():
        if len(steps)> 1:
            log.error("sequenceScanner: Sequence %s is expected in more than one step: %s", alg, steps)
            match=re.search('Step([0-9])',alg)
            if match:
                candidateStep=match.group(1)
                log.error("sequenceScanner:         ---> candidate good step is %s", candidateStep)
            _status=False     

    log.debug("sequenceScanner: scanned %s steps with status %d", final_step, _status)
    return _status
   

def decisionTreeFromChains(HLTNode, chains, allDicts, newJO):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """

    log.info("[decisionTreeFromChains] Run decisionTreeFromChains on %s", HLTNode.name())
    HLTNodeName= HLTNode.name()
    if len(chains) == 0:
        log.info("[decisionTreeFromChains] Configuring empty decisionTree")
        return []

    (finalDecisions, CFseq_list) = createDataFlow(chains, allDicts)
    if not newJO:
        createControlFlow(HLTNode, CFseq_list)
    else:
        from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import createControlFlowNewJO
        createControlFlowNewJO(HLTNode, CFseq_list)

    
    # decode and attach HypoTools:
    for chain in chains:
        chain.createHypoTools()

    # create dot graphs
    log.debug("finalDecisions: %s", finalDecisions)
    if create_dot():
        all_DataFlow_to_dot(HLTNodeName, CFseq_list)

    # matrix display
    # uncomment for serious debugging
    # matrixDisplay( CFseq_list )

    return finalDecisions


def createDataFlow(chains, allDicts):
    """ Creates the filters and connect them to the menu sequences"""

    # find tot nsteps
    chainWithMaxSteps = max(chains, key=lambda chain: len(chain.steps))
    NSTEPS = len(chainWithMaxSteps.steps)

    log.info("[createDataFlow] creating DF for %d chains and total %d steps", len(chains), NSTEPS)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
    # initialize arrays for monitor
    finalDecisions = [ [] for n in range(NSTEPS) ]
    CFseqList = [ [] for n in range(NSTEPS) ]

    # loop over chains
    for chain in chains:
        log.debug("\n Configuring chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))

        lastCFseq = None
        lastDecisions = []
        for nstep, chainStep in enumerate( chain.steps ):
            log.debug("\n************* Start connecting step %d %s for chain %s", nstep+1, chainStep.name, chain.name)           

            filterInput = chain.L1decisions if nstep == 0 else lastDecisions
            log.debug("Seeds added; having in the filter now: %s", filterInput)

            if len(filterInput) == 0 :
                log.error("[createDataFlow] Filter for step %s has %d inputs! At least one is expected", chainStep.name, len(filterInput))
                raise Exception("[createDataFlow] Cannot proceed, exiting.")

            # make one filter per step:
            sequenceFilter= None
            filterName = CFNaming.filterName(chainStep.name)
            if chainStep.isEmpty:
                filterOutput= filterInput
            else:
                filterOutput =[ CFNaming.filterOutName(filterName, inputName) for inputName in filterInput ]
       
            (foundFilter, foundCFSeq) = findCFSequences(filterName, CFseqList[nstep])
            log.debug("Found %d CF sequences with filter name %s", foundFilter, filterName)        
             # add error if more than one
            if not foundFilter:
                sequenceFilter = buildFilter(filterName, filterInput, chainStep.isEmpty)
                CFseq = CFSequence( ChainStep=chainStep, FilterAlg=sequenceFilter)
                CFseq.connect(filterOutput)
                CFseqList[nstep].append(CFseq)
                lastDecisions=CFseq.decisions
                lastCFseq=CFseq
            else:
                if len(foundCFSeq) > 1:
                    log.error("Found more than one seuqences containig this filter %s", filterName)
                lastCFseq=foundCFSeq[0]
                sequenceFilter=lastCFseq.filter
                [ sequenceFilter.addInput(inputName) for inputName in filterInput ]
                [ sequenceFilter.addOutput(outputName) for outputName in  filterOutput ]
                lastCFseq.connect(filterOutput)
                if chainStep.isEmpty:
                    lastDecisions=filterOutput
                else:
                    lastDecisions=lastCFseq.decisions

            # add chains to the filter:
            chainLegs = chainStep.getChainLegs()
            if len(chainLegs) != len(filterInput):
                log.error("[createDataFlow] chainlegs = %i differ from inputs=%i", len(chainLegs), len(filterInput))
                raise Exception("[createDataFlow] Cannot proceed, exiting.")
            for finput, leg in zip(filterInput, chainLegs):
                sequenceFilter.addChain(leg, finput)
                log.debug("Adding chain %s to input %s of %s", leg, finput,sequenceFilter.Alg.name())
            log.debug("Now Filter has chains: %s", sequenceFilter.getChains())
            log.debug("Now Filter has chains/input: %s", sequenceFilter.getChainsPerInput())

            if chainStep.combo is not None:
                chainStep.combo.addChain( [d for d in allDicts if d['chainName'] == chain.name ][0])
                log.debug("Added chains to ComboHypo: %s",chainStep.combo.getChains())
            else:
                log.debug("Combo not implemented if it's empty step")

            if len(chain.steps) == nstep+1:
                log.debug("Adding finalDecisions for chain %s at step %d:", chain.name, nstep+1)
                for dec in lastDecisions:
                    finalDecisions[nstep].append(dec)
                    log.debug(dec)
                    
        #end of loop over steps
        log.debug("\n Built CF for chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))
    #end of loop over chains


    log.debug("End of createDataFlow for %d chains and total %d steps", len(chains), NSTEPS)
    return (finalDecisions, CFseqList)


def createControlFlow(HLTNode, CFseqList):
    """ Creates Control Flow Tree starting from the CFSequences"""

    HLTNodeName= HLTNode.name()
    log.debug("createControlFlow on node %s",HLTNodeName)

    for nstep in range(len(CFseqList)):
        stepSequenceName =  CFNaming.stepName(nstep)
        log.debug("\n******** Create CF Tree %s with AthSequencers", stepSequenceName)

        #first make the filter step
        stepFilterNode = createStepFilterNode(stepSequenceName, CFseqList[nstep], dump=False)
        HLTNode += stepFilterNode

        # then the reco step
        stepRecoNode = createStepRecoNode(stepSequenceName, CFseqList[nstep], dump=False)
        HLTNode += stepRecoNode


        # then the monitor summary
        stepDecisions = []
        for CFseq in CFseqList[nstep]:
            stepDecisions.extend(CFseq.decisions)

        summary=makeSummary( stepSequenceName, stepDecisions )

        HLTNode += summary

        if create_dot():
            log.debug("Now Draw...")
            stepCF_DataFlow_to_dot(stepRecoNode.name(), CFseqList[nstep])
            stepCF_ControlFlow_to_dot(stepRecoNode)

        log.debug("************* End of step %d, %s", nstep+1, stepSequenceName)

    return






def findCFSequences(filter_name, cfseqList):
      """
      searches for a filter, with given name, in the CF sequence list of this step
      """
      log.debug( "findCFSequences: filter base name %s", filter_name )

      foundFilters = [cfseq for cfseq in cfseqList if filter_name == cfseq.filter.Alg.name()]
      log.debug("found %d filters with base name %s", len( foundFilters ), filter_name)

      found=len(foundFilters)
      if found:
          return (found, foundFilters)
      return (found, None)


def buildFilter(filter_name,  filter_input, empty):
    """
     Build the FILTER
     one filter per previous sequence at the start of the sequence: always create a new one
     if the previous hypo has more than one output, try to get all of them
     one filter per previous sequence: 1 input/previous seq, 1 output/next seq
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import  RoRSequenceFilterNode, PassFilterNode
    if empty:
        sfilter = PassFilterNode(name=filter_name)
        for i in filter_input:
            sfilter.addInput(i)
            sfilter.addOutput(i)
    else:
        sfilter = RoRSequenceFilterNode(name=filter_name)
        for i in filter_input:
            sfilter.addInput(i)
            sfilter.addOutput(CFNaming.filterOutName(filter_name, i))

    log.debug("Added inputs to filter: %s", sfilter.getInputList())
    log.debug("Added outputs to filter: %s", sfilter.getOutputList())
    log.debug("Filter Done: %s", sfilter.Alg.name())

    return (sfilter)
