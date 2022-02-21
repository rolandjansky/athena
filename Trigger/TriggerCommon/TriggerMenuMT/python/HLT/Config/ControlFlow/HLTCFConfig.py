
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

from TriggerMenuMT.HLT.Config.ControlFlow.HLTCFDot import stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLT.Config.Validation.CFValidation import testHLTTree
from TriggerMenuMT.HLT.Config.MenuComponents import CFSequence, RoRSequenceFilterNode, PassFilterNode
from TriggerMenuMT.HLT.Config.ControlFlow.MenuComponentsNaming import CFNaming

from AthenaCommon.CFElements import parOR, seqAND, getSequenceChildren, isSequence, compName, findSubSequence,findAlgorithm
from AthenaCommon.AlgSequence import AlgSequence, dumpSequence
from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaCommon.Logging import logging

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena

from DecisionHandling.DecisionHandlingConfig import TriggerSummaryAlg
from HLTSeeding.HLTSeedingConfig import mapThresholdToL1DecisionCollection
from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectViewMakers, collectDecisionObjects, \
     triggerMonitoringCfg, triggerSummaryCfg, triggerMergeViewsAndAddMissingEDMCfg, collectHypoDecisionObjects
from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import getTrigNavSlimmingMTOnlineConfig
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm


from builtins import map, range, str, zip
from collections import OrderedDict, defaultdict
import re

log = logging.getLogger( __name__ )


#### Functions to create the CF tree from CF configuration objects
def makeSummary(name, flatDecisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""

    summary = TriggerSummaryAlg( CFNaming.stepSummaryName(name) )
    summary.InputDecision = "HLTSeedingSummary"
    summary.FinalDecisions = list(OrderedDict.fromkeys(flatDecisions))
    return summary


def createStepRecoNode(name, seq_list, dump=False):
    """ Elementary HLT reco step, contianing all sequences of the step """

    log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name + CFNaming.RECO_POSTFIX)
    for seq in seq_list:
        stepCF += createCFTree(seq)

    if dump:
        dumpSequence (stepCF, indent=0)
    return stepCF


def createStepFilterNode(name, seq_list, dump=False):
    """ Elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """

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
    
    #empty step: add the PassSequence, one instance only is appended to the tree
    if len(CFseq.step.sequences)==0:  
        seqAndWithFilter=filterAlg       
        return seqAndWithFilter

    stepReco = parOR(CFseq.step.name + CFNaming.RECO_POSTFIX)  # all reco algorithms from all the sequences in a parallel sequence
    seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg, stepReco])

    recoSeqSet=set()
    hypoSet=set()
    for menuseq in CFseq.step.sequences:
        menuseq.addToSequencer(recoSeqSet,hypoSet)
  
    stepReco   += sorted(list(recoSeqSet), key=lambda t: t.name())
    seqAndWithFilter += sorted(list(hypoSet), key=lambda t: t.name()) 
    if CFseq.step.combo is not None:         
        seqAndWithFilter += CFseq.step.combo.Alg

    return seqAndWithFilter


#######################################
## CORE of Decision Handling
#######################################

def makeHLTTree(newJO=False, triggerConfigHLT = None):
    """ Creates the full HLT tree, main function called from GenerateMenu.py"""

    # Check if triggerConfigHLT exits, if yes, derive information from this
    if triggerConfigHLT is None:
        raise Exception("[makeHLTTree] triggerConfigHLT is set to None, please check!")

    # get topSequnece
    topSequence = AlgSequence()

    # find main HLT top sequence (already set up in runHLT_standalone)
    hltSeeding = findAlgorithm(topSequence, "HLTSeeding")

    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop = findSubSequence(topSequence, "HLTTop")
    hltTop += steps

    hltEndSeq = parOR("HLTEndSeq")
    hltTop += hltEndSeq

    hltFinalizeSeq = seqAND("HLTFinalizeSeq")

    log.debug("[makeHLTTree] will now make the DF and CF tree from chains")

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

    log.debug("[makeHLTTree] created the final summary tree")
    # TODO - check we are not running things twice. Once here and once in TriggerConfig.py
    

    # Collections required to configure the algs below
    hypos = collectHypos(steps)
    filters = collectFilters(steps)
    viewMakers = collectViewMakers(steps)

    viewMakerMap = {compName(vm):vm for vm in viewMakers}
    for vmname, vm in viewMakerMap.items():
        log.debug(f"{vmname} InputMakerOutputDecisions: {vm.InputMakerOutputDecisions}")
        if vmname.endswith("_probe"):
            try:
                log.debug(f"Setting InputCachedViews on {vmname} to read decisions from tag leg {vmname[:-6]}: {vm.InputMakerOutputDecisions}")
                vm.InputCachedViews = viewMakerMap[vmname[:-6]].InputMakerOutputDecisions
            except KeyError: # We may be using a probe leg that has different reco from the tag
                log.debug(f"Tag leg does not match probe: '{vmname[:-6]}', will not use cached views")

    with ConfigurableRun3Behavior():
        summaryAcc, summaryAlg = triggerSummaryCfg( ConfigFlags, hypos )

    # Schedule the DecisionSummaryMakerAlg
    hltFinalizeSeq += conf2toConfigurable( summaryAlg )
    appendCAtoAthena( summaryAcc )

    # Add end-of-event sequences executed conditionally on the DecisionSummaryMakerAlg filter status
    acceptedEventChainDicts = [cd for cd in triggerConfigHLT.dictsList() \
                               if 'Calib' in cd['signatures'] \
                               and 'acceptedevts' in cd['chainParts'][0]['purpose']]
    if ConfigFlags.Trigger.endOfEventProcessing.Enabled and acceptedEventChainDicts:
        from TrigGenericAlgs.TrigGenericAlgsConfig import EndOfEventROIConfirmerAlgCfg, EndOfEventPrescaleCheckAlgCfg
        endOfEventRoIMaker = conf2toConfigurable(EndOfEventROIConfirmerAlgCfg('EndOfEventROIConfirmerAlg'))
        hltFinalizeSeq += endOfEventRoIMaker
        acceptedEventTopSeq = parOR("acceptedEventTopSeq")
        acceptedEventTopSeq.IgnoreFilterPassed=True
        hltFinalizeSeq += conf2toConfigurable(acceptedEventTopSeq)
        for acceptedEventChainDict in acceptedEventChainDicts:
            # Common config for each chain
            prescaleChain = acceptedEventChainDict['chainName']
            seqLabel = prescaleChain.replace('HLT_acceptedevts','')
            acceptedEventSeq = seqAND('acceptedEventSeq'+seqLabel)
            endOfEventPrescaleAlg = EndOfEventPrescaleCheckAlgCfg('EndOfEventPrescaleCheckAlg'+seqLabel, chainName=prescaleChain)
            acceptedEventSeq += conf2toConfigurable(endOfEventPrescaleAlg)
            # Now add chain-specific end-of-event sequences executed conditionally on the prescale

            # The LAr Noise Burst end-of-event sequence
            if 'larnoiseburst' in acceptedEventChainDict['chainParts'][0]['purpose']:
                from TriggerMenuMT.HLT.CalibCosmicMon.CalibChainConfiguration import getLArNoiseBurstEndOfEvent
                recoSeq, LArNBRoIs = getLArNoiseBurstEndOfEvent()
                endOfEventRoIMaker.RoIs += [LArNBRoIs]
                acceptedEventSeq += conf2toConfigurable(recoSeq)
            # elif ... add other end of event sequences (with the corresponding chain) here if needed

            acceptedEventTopSeq += conf2toConfigurable(acceptedEventSeq)
    
    # More collections required to configure the algs below
    decObj = collectDecisionObjects( hypos, filters, hltSeeding, summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)

    with ConfigurableRun3Behavior():
        monAcc, monAlg = triggerMonitoringCfg( ConfigFlags, hypos, filters, hltSeeding )

    hltEndSeq += conf2toConfigurable( monAlg )
    appendCAtoAthena( monAcc )
        
    with ConfigurableRun3Behavior():
        edmAlg = triggerMergeViewsAndAddMissingEDMCfg(ConfigFlags, ['AOD', 'ESD'], hypos, viewMakers, decObj, decObjHypoOut)

    # C) Finally, we create the EDM output
    hltFinalizeSeq += conf2toConfigurable(edmAlg)

    if ConfigFlags.Trigger.doOnlineNavigationCompactification:
        onlineSlimAlg = getTrigNavSlimmingMTOnlineConfig()
        hltFinalizeSeq += conf2toConfigurable(onlineSlimAlg)

    hltEndSeq += hltFinalizeSeq

    # Test the configuration
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


    # Switch on  DEBUG ouput in some algorithms
    #debugDecisions(hypos, summary, monAlg, summaryAlg)

    
def matrixDisplay( allCFSeq ):

    def __getHyposOfStep( step ):
        if len(step.sequences):
            step.getChainNames()           
        return []
   
    # fill dictionary to cumulate chains on same sequences, in steps (dict with composite keys)
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

    _seqMapInStep = defaultdict(set)
    _status = True
    def _mapSequencesInSteps(seq, stepIndex, childInView):
        """ Recursively finds the steps in which sequences are used"""
        if not isSequence(seq):
            return stepIndex
        name = compName(seq)                
        match=re.search('^Step([0-9]+)_filter',name)
        if match:
            stepIndex = match.group(1)
            log.debug("sequenceScanner: This is another step: %s %s", name, stepIndex)
        inViewSequence = ""
        inView = False
        for c in getSequenceChildren( seq ):
            if isSequence(c):
                # Detect whether this is the view sequence pointed to
                # by the EV creator alg, or if it is in such a sequence
                inView = c.name()==inViewSequence or childInView
                stepIndex = _mapSequencesInSteps(c, stepIndex, childInView=inView)
                _seqMapInStep[compName(c)].add((stepIndex,inView))
                log.verbose("sequenceScanner: Child %s of sequence %s is in view? %s --> '%s'", compName(c), name, inView, inViewSequence)
            else:
                if isinstance(c,EventViewCreatorAlgorithm):
                    inViewSequence = c.ViewNodeName
                    log.verbose("sequenceScanner: EventViewCreatorAlg %s is child of sequence %s with ViewNodeName %s", compName(c), name, c.ViewNodeName)
        log.debug("sequenceScanner: Sequence %s is in view? %s --> '%s'", name, inView, inViewSequence)
        return stepIndex

    # do the job:
    final_step=_mapSequencesInSteps(HLTNode, 0, childInView=False)

    for alg, steps in _seqMapInStep.items():
        if 'PassSequence' in alg: # do not count PassSequences, which is used many times
            continue
        # Sequences in views can be in multiple steps
        nonViewSteps = sum([0 if isInViews else 1 for (stepIndex,isInViews) in steps])
        if nonViewSteps > 1:
            steplist = [stepIndex for stepIndex,inViewSequence in steps]
            log.error("sequenceScanner: Sequence %s is expected outside of a view in more than one step: %s", alg, steplist)
            match=re.search('Step([0-9]+)',alg)
            if match:
                candidateStep=match.group(1)
                log.error("sequenceScanner:         ---> candidate good step is %s", candidateStep)
            _status=False
            raise RuntimeError(f"Duplicated event-scope sequence {alg} in steps {steplist}")

    log.debug("sequenceScanner: scanned %s steps with status %d", final_step, _status)
    return _status
   

def decisionTreeFromChains(HLTNode, chains, allDicts, newJO):
    """ Creates the decision tree, given the starting node and the chains containing the sequences  """

    log.info("[decisionTreeFromChains] Run decisionTreeFromChains on %s", HLTNode.name())
    HLTNodeName= HLTNode.name()
    if len(chains) == 0:
        log.info("[decisionTreeFromChains] Configuring empty decisionTree")
        return []

    (finalDecisions, CFseq_list) = createDataFlow(chains, allDicts)
    if not newJO:
        createControlFlow(HLTNode, CFseq_list)
    else:
        raise RuntimeError('createControlFlowNewJO not implemented')
        # createControlFlowNewJO(HLTNode, CFseq_list)

    
    # decode and attach HypoTools:
    for chain in chains:
        chain.createHypoTools()

    # create dot graphs
    log.debug("finalDecisions: %s", finalDecisions)

    if ConfigFlags.Trigger.generateMenuDiagnostics:
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
            if nstep == 0:
                if chainStep.stepDicts:
                    filterInput = [ mapThresholdToL1DecisionCollection(p["chainParts"][0]["L1threshold"]) for p in chainStep.stepDicts]
                else:
                    filterInput = chain.L1decisions
            else:
                filterInput = lastDecisions
            if len(filterInput) == 0 :
                log.error("[createDataFlow] Filter for step %s has %d inputs! At least one is expected", chainStep.name, len(filterInput))
                raise Exception("[createDataFlow] Cannot proceed, exiting.")
            
            log.debug("Set Filter input: %s while setting the chain: %s", filterInput, chain.name)

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
                log.error("[createDataFlow] lengths of chainlegs = %s differ from inputs=%s", str(chainLegs), str(filterInput))
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

        if ConfigFlags.Trigger.generateMenuDiagnostics:
            log.debug("Now Draw...")
            stepCF_DataFlow_to_dot(stepRecoNode.name(), CFseqList[nstep])
            stepCF_ControlFlow_to_dot(stepRecoNode)

        log.debug("************* End of step %d, %s", nstep+1, stepSequenceName)

    return




def findCFSequences(filter_name, cfseqList):
      """
      Searches for a filter, with given name, in the CF sequence list of this step
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
