# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from functools import lru_cache
from AthenaCommon.CFElements import findAllAlgorithms, parOR, seqOR, seqAND, isSequence
from AthenaCommon.Logging import logging
import GaudiConfig2
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainInLegs
from TriggerMenuMT.HLT.Config.MenuComponents import (isComboHypoAlg,
                                                             isFilterAlg,
                                                             isHypoAlg,
                                                             isInputMakerBase,
                                                             EmptyMenuSequence)
from TriggerMenuMT.HLT.Config.ControlFlow.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLT.Config.Utility.HLTMenuConfig import HLTMenuConfig

log = logging.getLogger( __name__ )

def resetDataFlow(acc):
    """
    Find all algorithms involved in HLT decision DF and reset properties to empty (lists), unset
    """
    for alg in acc.getEventAlgos():
        if isInputMakerBase(alg):
            log.verbose("Resetting IO for %s Input Maker", alg.name )
            alg.InputMakerInputDecisions = []
        if isFilterAlg(alg):
            log.verbose("Resetting IO for %s Filter", alg.name )
            alg.Input = []
            alg.Output = []
        if isComboHypoAlg(alg):
            log.verbose("Resetting IO for %s Combo Hypo", alg.name )
            alg.MultiplicitiesMap = {}
            alg.LegToInputCollectionMap = {}
            alg.HypoInputDecisions = []
            alg.HypoOutputDecisions = []


def connectIMAndHypo(im, hypo):
    """Setup Input and Output of Hypo and IM according to the naming convention"""
    im.InputMakerInputDecisions = []
    im.InputMakerOutputDecisions = CFNaming.inputMakerOutName(im.name)
    hypo.HypoInputDecisions = im.InputMakerOutputDecisions # connect the two
    hypo.HypoOutputDecisions =  CFNaming.hypoAlgOutName(hypo.name)

@lru_cache
def legDicts(chainName):
    return splitChainInLegs( chainName )

def chainDictForSequence(chainName, sequences):
    """Returns a list of pairs, sequence, chainDict

    possible cases: 
    A) no sequence (normal wen menu alignment is used)
    B) number of sequences == number of chain parts, e5_mu10 or just e3 type of chain
    ([0, 1], [0, 1]) is the set of indices
    C) number of sequences == 1 && number of chain parts > 1 for single signature asymmetric combined chains e5_e3 type chain
    """
    if len(sequences) == 0:
        return []
    chainDictLegs = legDicts(chainName)
    if len(sequences) == len(chainDictLegs):
        return zip( sequences, chainDictLegs )
    elif len(sequences) == 1 and len(chainDictLegs) > 1:
        return zip( sequences * len(chainDictLegs), chainDictLegs )
    else:
        assert False, "Chain {} has missmatching number of chain legs {} in dictionary to the number of sequences {}".format(chainName, len(chainDictLegs), len(sequences))


def traverseDataFlow(acc, startCollectionName, functor):
    """
    Function to traverse DF collections path starting from an arbitrary one
    For each checked algorithm visited the functor is called with the collection name through which traversing arrived to it, and the algorithm itself.
    Examples are available in the test at the end of the file.

    Note that this traversal has nothing to do with sequences. It only follows DF dependencies.
    """
    outputs = {}
    # assemble all HLT decision DF algorithms in dictionary keyed by the output that they provide
    for alg in acc.getEventAlgos(): # TODO improve CA implementation to obtain all algorithms when no specific sequence is required
        if isHypoAlg(alg):
                outputs[str(alg.HypoOutputDecisions)] = alg
        if isComboHypoAlg(alg):
            for d in alg.HypoOutputDecisions:
                outputs[str(d)] = alg
        if isInputMakerBase(alg):
            outputs[str(alg.InputMakerOutputDecisions)] = alg
        if isFilterAlg(alg):
            for d in alg.Output:
                outputs[str(d)] = alg

    # for a given collection and algorithm returns list of inputs that lead to it
    # all implementations except for ComboHypo and Filter are trivial
    def __jumpToInputs(outputName, alg):
        if isHypoAlg(alg):
            return [alg.HypoInputDecisions.Path]
        if isInputMakerBase(alg):
            return alg.InputMakerInputDecisions
        if isFilterAlg(alg):
            idx = alg.Output.index(outputName)
            if len(alg.IOMapping) == 0: # trivial filter configuration, n-th input -> n-th output
                return [alg.Input[idx]]
            else:
                return [alg.Input[i] for i in alg.IOMapping[idx]]
        if isComboHypoAlg(alg):
            idx = alg.HypoOutputDecisions.index(outputName)
            return [alg.HypoInputDecisions[idx]]

    # recursive traversal implementation
    def __dive(outColl, alg):
        functor(outColl, alg)
        inputs = __jumpToInputs(outColl, alg)
        for inputColl in inputs:
            if inputColl in outputs:
                __dive(inputColl, outputs[inputColl])

    if startCollectionName in outputs:
        __dive(startCollectionName, outputs[startCollectionName])


def commonWithOtherOutput(acc, newInputColl, filterAlg ):
    """
    Finds the filter input that can be shared (returns None if not found)
    To be called with combo output collection. 
    """
    if len(filterAlg.Output) == 0: # there can not be any commonality if there is no output
        return None
    colls = []
    # functor to collect filters output (see traverseDataFlow)
    def __collectFilterInputs( outColl, alg ):
        if isFilterAlg(alg):
            idx = alg.Output.index(outColl)
            colls.append(alg.Input[idx])
    
    colls = []
    traverseDataFlow( acc, newInputColl, __collectFilterInputs)
    newInputHistory = set(colls)
    for existingOutput in filterAlg.Output:
        colls = []
        traverseDataFlow( acc, existingOutput, __collectFilterInputs)
        if len(set(colls).intersection(newInputHistory)) != 0:
            return existingOutput
    return None


def generateDecisionTree(flags, chains):
    """Creates CF algorithms
    
    The implementation relies of functions that build a small element of it.
    Each function generates whatever is needed in the decision tree before (potentially in a recursive way). 
    All functions are cached, so each element is created only once.

    The functions check the global boolean theCFisFixed - and if set to True no CF algorithms are further constructed (assertion error is generated).
    """
    acc = ComponentAccumulator()
    mainSequence = seqOR('HLTAllSteps')
    acc.addSequence( mainSequence )
    theCFisFixed = False

    @lru_cache(None)
    def getFiltersStepSeq( stepNumber ):
        """
        Returns sequence to hold filters for a step
        """
        assert stepNumber >= 1, "No filter step <1 should be created"
        assert not theCFisFixed, "Can not create further sequences at this stage"
        name = 'Step{}{}'.format(stepNumber, CFNaming.FILTER_POSTFIX)
        if stepNumber > 1:
            getRecosStepSeq( stepNumber -1 ) # make sure steps sequencers are correctly made: Step1_filter, Step1_recos, Step2_filters, Step2_recos ...
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequence.name )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getRecosStepSeq( stepNumber ):
        """
        Returns sequence to hold reconstruction algorithms
        """
        assert not theCFisFixed, "Can not create further sequences at this stage"
        getFiltersStepSeq( stepNumber ) # make sure there is always filters step before reco
        name = 'Step{}{}'.format(stepNumber, CFNaming.RECO_POSTFIX)
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequence.name )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getDecisionDumper( stepNumber ):
        """Configure DumpDecision algorithm for a step"""
        assert not theCFisFixed, "Can not create further algorithms at this stage"
        seq = getFiltersStepSeq( stepNumber )
        algo = CompFactory.DumpDecisions("DecisionSummaryStep{}".format(stepNumber))
        acc.addEventAlgo( algo, sequenceName=seq.name)
        return algo


    @lru_cache(None)
    def getSingleMenuSeq( stepNumber, stepName ):
        """
        Creates basic decision taking unit, the menu sequence
        """
        assert not theCFisFixed, "Can not create further sequences at this stage"
        assert stepNumber > 0, "Incorrect step number, has to be bigger than 0"
        name = "Step{}_menu_{}".format(stepNumber, stepName)
        seq = seqAND( name )

        allRecoSeqName = getRecosStepSeq( stepNumber ).name
        acc.addSequence(seq, parentName = allRecoSeqName )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getComboSequences( stepNumber, stepName ):
        """
        Produces sequence encompassing reconstruction sequences needed in combined chains
        """
        assert not theCFisFixed, "Can not create further sequences at this stage"

        singleMenuSeqName = getSingleMenuSeq( stepNumber, stepName ).name

        stepComboName = "Combo{}_{}".format(stepNumber,stepName)
        acc.addSequence( seqAND(stepComboName), parentName=singleMenuSeqName )

        stepComboRecoName ="ComboReco{}_{}".format(stepNumber, stepName)
        acc.addSequence( parOR(stepComboRecoName), parentName=stepComboName )
        return acc.getSequence(stepComboName), acc.getSequence(stepComboRecoName)

    def isEmpty( step ):
        return all([isinstance(seq, EmptyMenuSequence) for seq in step.sequences])

    @lru_cache(None)
    def getFilterAlg( stepNumber, stepName, isEmpty ):
        """
        Returns, if need be created, filter for a given step
        """
        assert not theCFisFixed, "Can not create further algorithms at this stage"

        filtersStep = getFiltersStepSeq(stepNumber)
        singleMenuSeq = getSingleMenuSeq(stepNumber, stepName)
        if isEmpty:
            filterAlg = CompFactory.PassFilter(CFNaming.filterName("Pass"))
        else:
            filterAlg = CompFactory.RoRSeqFilter(CFNaming.filterName(stepName))
            acc.addEventAlgo(filterAlg, sequenceName=filtersStep.name)
        acc.addEventAlgo(filterAlg, sequenceName=singleMenuSeq.name)

        log.debug('Created filter %s', filterAlg.name)
        return acc.getEventAlgo(filterAlg.name)


    def __findAllOf( stepCounter, stepName, predicate):
        """
        Find all algorithms fulfilling predicate, for sequences that do not look like menu None is placed in the result instead
        """
        _, comboRecoSeq = getComboSequences( stepCounter, stepName )
        result = []
        for recoSeq in comboRecoSeq.Members:
            if isSequence( recoSeq ):
                algs  = findAllAlgorithms( recoSeq )
                for alg in algs:
                    if predicate(alg):
                        result.append(alg)
                        break
            else:
                result.append(None)
        return result

    @lru_cache(None)
    def findAllInputMakers( stepCounter, stepName ):
        return __findAllOf( stepCounter, stepName, isInputMakerBase )

    @lru_cache(None)
    def findComboHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isComboHypoAlg( alg ):
                return alg
        return None

    @lru_cache(None)
    def findAllHypoAlgs( stepCounter, stepName ):
        r = __findAllOf( stepCounter, stepName, isHypoAlg )
        return r


    def addAndAssureUniqueness( prop, toadd, context="" ):
        """Given list of strings (DF input/outputs of an alg) adds the 'toadd' if not there already. Returns updated list."""
        if isinstance(toadd, str):
            toadd = [toadd]
        missing = []
        for t in toadd:
            if t not in prop:
                missing.append( t )
        return list( prop ) + missing

    def assureUnsetOrTheSame(prop, toadd, context):
        """
        Central function setting string like properties (collection keys). Assures that valid names are not overwritten.        
        """
        if prop == "" or prop == [] or prop == toadd:
            return toadd
        if prop != toadd:
            raise Exception("{}, when setting property found conflicting values, existing {} and new {}".format(context, prop, toadd))

    def clearUnderscores(s):
        """
        Humanizes collection name by removing multiple underscores and stripping trailing ones
        """
        p = s
        while True:
            n = p.replace("__", "_")
            if p == n:
                return p.rstrip("_")
            p = n

    def prevStepComboHypoOutput(stepCounter, chain):
        """
        Returns list of decision collections that are outputs of previous step ComboHypo
        For the outputs of HypoAlgs use stepHypoOutput
        """
        if stepCounter == 1: # L1 seed
            return chain.L1decisions, ""
        else:
            prevCounter = stepCounter-1
            prevName = chain.steps[prevCounter-1].name # counting steps from 1, for indexing need one less
            prevCombo = findComboHypoAlg(prevCounter, prevName)
            if not prevCombo:
                return prevStepComboHypoOutput(stepCounter - 1, chain)
            return  prevCombo.HypoOutputDecisions, prevCombo.name

    @lru_cache(None)
    def stepHypoOutput( stepCounter, chain ):
        """
        Return list of (hypoAlgName, output), if missing traverse sequences back to find one
        If L1 needs to be reached the hypoAlgName is based on the threshold nama e.g. L1MU
        """

        def __nameAndOutput(h):
            return (h.name, h.HypoOutputDecisions.Path) if h else None       

        output = None
        for currentCounter in range(stepCounter, -1, -1):
           
            if currentCounter == 0:
                from HLTSeeding.HLTSeedingConfig import mapThresholdToL1DecisionCollection
                thisStep = [ (''.join([c for c in seed if not c.isnumeric()]), mapThresholdToL1DecisionCollection(seed))  for seed in chain.vseeds ]
            else:
                thisStep = [__nameAndOutput(h) for h in findAllHypoAlgs( currentCounter, chain.steps[currentCounter-1].name )]


            if output is None:
                output = thisStep
            for index,el in enumerate(thisStep):
                if not output[index] and el:
                    output[index] = el
            if all(output):
                return output

        return output

    def printChainsConfig():
        """Info about the chains config printout"""
        for chain in chains:
            log.info("CF algorithms for chain %s", chain.name)
            for stepCounter, step in enumerate( chain.steps, 1 ):
                filterAlg = getFilterAlg( stepCounter, step.name, isEmpty(step))
                if filterAlg and hasattr(filterAlg, "Input"):
                    log.info("%s FilterAlg: %s input: %s output: %s IO mapping: %s",
                            stepCounter, filterAlg.name, ", ".join(filterAlg.Input),
                            ", ".join(filterAlg.Output), filterAlg.IOMapping)
                    for inIndex, input in enumerate(filterAlg.Input):
                        log.info("%s filered chains from input: %s : %s",
                                stepCounter, input,  ", ".join(filterAlg.ChainsPerInput[inIndex]))
                    
                imAlgs = findAllInputMakers( stepCounter, step.name )
                for imAlg in imAlgs:
                    if imAlg:
                        log.info("%s  InputMaker: %s input: %s output: %s", stepCounter, imAlg.name,
                                ", ".join(imAlg.InputMakerInputDecisions), imAlg.InputMakerOutputDecisions)

                hypoAlgs = findAllHypoAlgs(stepCounter, step.name)
                for hypoAlg in hypoAlgs:
                    if hypoAlg:
                        log.info("%s  HypoAlg: %s input: %s output: %s", stepCounter, hypoAlg.name,
                                hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions)
                        log.info("%s  hypo tools: %s", stepCounter, ",".join([t.name for t in hypoAlg.HypoTools]))
                combo = findComboHypoAlg(stepCounter, step.name)
                if combo:
                    log.info("%s  ComboHypoAlg: %s input: %s output: %s", stepCounter, combo.name,
                            ". ".join(combo.HypoInputDecisions), ", ".join(combo.HypoOutputDecisions))
                    log.info("%s  multiplicities: %s, leg index to input index: %s", stepCounter, combo.MultiplicitiesMap, combo.LegToInputCollectionMap)
            log.info("-"*50)
        log.info("")

    def validateDataFlowConnections():
        """Checks basic connections corectness
        
        The implementation is suboptimal at the moment because the checks are done for all chains (that means repetitively for the same algs)
        """
        for chain in chains:
            for stepCounter, step in enumerate( chain.steps, 1 ):
                filterAlg = getFilterAlg( stepCounter, step.name, isEmpty(step))
                if filterAlg and hasattr(filterAlg, "Input"):
                    assert len(filterAlg.IOMapping) == len(filterAlg.Output), "Not all output will be filled in filter, while processing chain {}".format(chain.name)
                combo = findComboHypoAlg(stepCounter, step.name)
                if combo:
                    assert len(combo.HypoInputDecisions) == len(combo.HypoInputDecisions), "Misconfiguration of {} ComboHypo input/output counts differ".format(combo.name)

    
    #########################################################################################################

    # CF construction
    def constructControlFlow():
        """Creates all sequences and filter algs, for all configured chains. Merges CAs from signatures """
        maxStep = 0
        for chain in chains:
            for stepCounter, step in enumerate( chain.steps, 1 ):
                log.debug("DF for chain %s step %d ", chain.name, stepCounter)
                maxStep = max(maxStep, stepCounter)
                getSingleMenuSeq( stepCounter, step.name )
                getFilterAlg(stepCounter, step.name, isEmpty(step))
                # add sequences that allows reconstructions to be run in parallel, followed (in sequence) by the combo hypo
                comboSeq, comboRecoSeq = getComboSequences( stepCounter, step.name )

                needCombo = False
                for sequenceCounter, sequence in enumerate(step.sequences):
                    #log.debug("Sequence %d: IM=%s, Hypo=%s", sequenceCounter, sequence.ca.inputMaker().name,  sequence.ca.hypo().name)
                    if not isinstance(sequence, EmptyMenuSequence): # not an empty sequence
                        connectIMAndHypo(sequence.ca.inputMaker(), sequence.ca.hypo()) # setup basic CF                        
                        acc.merge( sequence.ca, sequenceName=comboRecoSeq.name)
                        log.debug("MERGE: sequence %s, sequenceName=%s", sequence, comboRecoSeq.name)
                        needCombo = True
                    else: # empty sequence
                        acc.addEventAlgo( CompFactory.PassFilter("Pass"),  sequenceName=comboRecoSeq.name)
                if needCombo:
                    comboHypo = CompFactory.ComboHypo( "CH"+step.name, CheckMultiplicityMap = len(step.sequences) != 1 )
                    acc.addEventAlgo( comboHypo, sequenceName=comboSeq.name )
                pass
        if flags.Trigger.doRuntimeNaviVal:
            for stepNumber in range(1, maxStep+1):
                getDecisionDumper(stepNumber)
        if log.getEffectiveLevel() <= logging.DEBUG:
            acc.printConfig()
    
    def setupHypoTools():
        """Configures hypo tools (the actual selection) for all configured chains"""
        for chain in chains:
            for stepCounter, step in enumerate(chain.steps, 1):
                comboHypoAlg = findComboHypoAlg( stepCounter, step.name )
                if comboHypoAlg:
                    # if the chain requires special combo tools
                    for comboToolConf in step.comboToolConfs:
                        comboHypoAlg.ComboHypoTools.append( comboToolConf.confAndCreate( HLTMenuConfig.getChainDictFromChainName( chain.name ) ) )

                for sequence, chainLegDict in chainDictForSequence(chain.name, step.sequences):
                    if not isinstance(sequence, EmptyMenuSequence):
                        hypoTool = sequence._hypoToolConf.confAndCreate( chainLegDict )
                        assert isinstance(hypoTool, GaudiConfig2._configurables.Configurable), "The Hypo Tool for {} is not Configurable2".format(chain.name)
                        sequence.hypo.Alg.HypoTools.append( hypoTool )
                        # # if the chain requires special combo tools
                        # for comboToolConf in step.comboToolConfs:
                        #     comboHypoAlg.ComboHypoTools.append( comboToolConf.confAndCreate( HLTMenuConfig.getChainDictFromChainName( chain.name ) ) )

    def constructDataFlow():
        """ Complete connecting (and naming) Data Flow collections"""
        for chain in chains:
            for stepCounter, step in enumerate( chain.steps, 1 ):
                log.debug("\n************* Start connecting step %d %s for chain %s", stepCounter, step.name, chain.name)           
                if isEmpty(step):
                    continue

                prevComboOut, prevComboName = prevStepComboHypoOutput(stepCounter, chain)
                def __constructDataFlowForLeg(sequence, chainDict):
                    """
                    Local function to setup filter/input makers/hypo algs IO
                    For combined chains when the there is more than one sequence per step this function is called as many times as many combinations of sequence, chainDict for a leg there is
                    """
                    sequenceCounter = step.sequences.index(sequence)
                    filterAlg = getFilterAlg(stepCounter, step.name, isEmpty(step))
                    comboOut = prevComboOut[sequenceCounter]
                    if comboOut not in filterAlg.Input: # input is not yet connected to filter
                        commonWithOutput = commonWithOtherOutput(acc, comboOut, filterAlg )
                        if commonWithOutput: # this input has commonalities with other input, appropriate output is returned
                            filterOut = commonWithOutput
                            filterAlg.IOMapping[ filterAlg.Output.index(commonWithOutput) ].append( len(filterAlg.Input) )
                        else: # entirely new input
                            filterAlg.IOMapping.append( [len(filterAlg.Input)] )
                            filterOut = clearUnderscores(CFNaming.filterOutName( filterAlg.name, comboOut ).replace(prevComboName, ""))
                            filterAlg.Output = addAndAssureUniqueness(filterAlg.Output, filterOut, "{} output".format(filterAlg.name))
                        filterAlg.Input.append( comboOut )
                        filterAlg.ChainsPerInput.append([])
                    else: # that input is connected to the filter, so it was already wired correctly to the output, we need to find out to which output it was connected
                        inputIndex = filterAlg.Input.index(comboOut)
                        for outIndex, inputIndices in enumerate(filterAlg.IOMapping):
                            if inputIndex in inputIndices:
                                filterOut = filterAlg.Output[outIndex]

                    # set chain to filter on
                    filterAlg.ChainsPerInput[filterAlg.Input.index(comboOut)] = addAndAssureUniqueness( filterAlg.ChainsPerInput[filterAlg.Input.index(comboOut)], chainDict["chainName"],
                                                                                                        "{} filtered chains".format(filterAlg.name))
                    filterAlg.Chains = addAndAssureUniqueness( filterAlg.Chains, chainDict["chainName"],
                                                            "{} filtered chains".format(filterAlg.name))
                    
                    log.debug("Set Filter input: %s for sequence %d while setting the chain: %s", filterAlg.Input.index(comboOut), sequenceCounter, chain.name)
                    imAlg = findAllInputMakers( stepCounter, step.name )[sequenceCounter]
                    if imAlg: log.debug("Found maker %s",imAlg.name)
                    if imAlg:
                        imAlg.InputMakerInputDecisions = addAndAssureUniqueness( imAlg.InputMakerInputDecisions, filterOut, "{} input".format( imAlg.name ) )
                    pass

                for sequence, chainLegDict in chainDictForSequence(chain.name, step.sequences):
                    __constructDataFlowForLeg( sequence, chainLegDict )

                comboHypo = findComboHypoAlg( stepCounter, step.name )
                if comboHypo:
                    #FP this must be changes, since the number of legs does not dpends on the number of Hypos
                    # for example asymmetric chians have one Hypo and more than one leg
                    # one has to loop over the number of seuqences
                    
                    
                    elementaryHyposOutputs = stepHypoOutput( stepCounter, chain )
                    rawInputsToCombo = []                    
                    for hypoName, hypoOutput in elementaryHyposOutputs:
                        rawInputsToCombo.append(hypoOutput)
                        comboHypo.HypoInputDecisions = addAndAssureUniqueness( comboHypo.HypoInputDecisions, hypoOutput, "{} comboHypo input".format( comboHypo.name ) )
                        comboOut = CFNaming.comboHypoOutputName( comboHypo.name, hypoName )
                        comboHypo.HypoOutputDecisions = addAndAssureUniqueness( comboHypo.HypoOutputDecisions, comboOut, "{} comboHypo output".format( comboHypo.name ) )
                    ###
                    # Here we map each leg-index to the input DecisionContainer (after the input ReadHandleKeyArray is de-duplicated)
                    # Please note that this has not been extensively tested in newJO as we do not have any jet triggers, or asymmetric leg triggers such as
                    # HLT_e5_etcut_e3_etcut_L12EM3. These are the ones which are most likely to need some more work here. 
                    # See mapRawInputsToInputsIndex in MenuComponents for more context around this outside of the newJO setup.
                    legToInputCollectionMapping = []
                    for rawInput in rawInputsToCombo:
                        try:
                            legToInputCollectionMapping.append( comboHypo.HypoInputDecisions.index(rawInput) )
                        except Exception:
                            raise Exception("[generateDecisionTree] Cannot find {} in {}. See HLTCFConfig_newJO for more details.".format(rawInput, tuple(comboHypo.HypoInputDecisions)))
                    assert len(chainLegDict['chainMultiplicities']) == len(legToInputCollectionMapping), "[generateDecisionTree] Can not get the input index for each leg. See HLTCFConfig_newJO for more details."            
                    #
                    ###
                    comboHypo.MultiplicitiesMap[chain.name] = chainLegDict['chainMultiplicities']
                    comboHypo.LegToInputCollectionMap[chain.name] = legToInputCollectionMapping
                # connect dumper
                if flags.Trigger.doRuntimeNaviVal:
                    dumper = getDecisionDumper(stepCounter)
                    filters = getFiltersStepSeq(stepCounter)
                    for alg in filters.Members:
                        if isFilterAlg(alg):
                            dumper.Decisions = addAndAssureUniqueness( dumper.Decisions, alg.Output, context="Settings of decision dumper" )


    constructControlFlow()
    theCFisFixed=True
    log.debug("CF constructed")
    resetDataFlow(acc)    
    constructDataFlow()
    log.debug("DF connected")
    setupHypoTools()
    log.debug("Configured (Combo) Hypos")


    validateDataFlowConnections()

    printChainsConfig()
    return acc


if __name__ == "__main__":
    # Test of DF collections traversal
    # build hypothetical structure of DF to test back traversing
    # make it resemble the combined chain with one arm that is hollow/empty
    # naming convention: H* - hypo, IM* - InputMaker, CH* - combo hypo, F* - filter
    #                    last digit is step
    #                    small letter a/b branch of the selection
    acc = ComponentAccumulator()
    # step 1
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fmerged1", Input=["Fa1_I","Fb1_I"], Output=["Fa1merged_O", "Fb1merged_O"]))
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fa1", Input=["Fa1_I"], Output=["Fa1_O"]))
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fb1", Input=["Fb1_I"], Output=["FOb1"]))
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMa1", InputMakerInputDecisions=["Fa1_O", "Fa1merged_O"], InputMakerOutputDecisions="IMa1_O"))
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMb1", InputMakerInputDecisions=["FOb1", "Fb1merged_O"], InputMakerOutputDecisions="IMb1_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Ha1", HypoInputDecisions="IMa1_O", HypoOutputDecisions="Ha1_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Hb1", HypoInputDecisions="IMb1_O", HypoOutputDecisions="Hb1_O"))
    acc.addEventAlgo(CompFactory.ComboHypo("CHmerged1", HypoInputDecisions=["Ha1_O","Hb1_O"], HypoOutputDecisions=["CHa1merged_O", "CHb1merged_O"]))
    acc.addEventAlgo(CompFactory.ComboHypo("CHa1", HypoInputDecisions=["Ha1_O"], HypoOutputDecisions=["CHa1_O"]))
    acc.addEventAlgo(CompFactory.ComboHypo("CHb1", HypoInputDecisions=["Hb1_O"], HypoOutputDecisions=["CHb1_O"]))

    # step 2 with alternative merged sequences, when one is empty
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fmerged2", Input=["CHa1merged_O","CHb1merged_O"], Output=["Fa2merged_O", "Fb2merged_O"])) #FOb2maerged dangling/unused
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Faltmerged2", Input=["CHa1merged_O"], Output=["Fa2altmerged_O"])) #
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fa2", Input=["CHa1_O"], Output=["Fa2_O"]))
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fb2", Input=["CHb1_O"], Output=["Fb2_O"]))
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMa2", InputMakerInputDecisions=["Fa2_O", "Fa2merged_O", "Fa2altmerged_O"], InputMakerOutputDecisions="IMa2_O"))
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMb2", InputMakerInputDecisions=["Fb2_O", "Fb2merged_O"], InputMakerOutputDecisions="IMb2_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Ha2", HypoInputDecisions="IMa2_O", HypoOutputDecisions="Ha2_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Hb2", HypoInputDecisions="IMb2_O", HypoOutputDecisions="Hb2_O"))
    acc.addEventAlgo(CompFactory.ComboHypo("CHmerged2", HypoInputDecisions=["Ha2_O","Hb2_O"], HypoOutputDecisions=["CHa2merged_O", "CHb2merged_O"])) # assymetry here, see Hb1_O
    acc.addEventAlgo(CompFactory.ComboHypo("CHaltmerged2", HypoInputDecisions=["Ha2_O","Hb1_O"], HypoOutputDecisions=["CHa2altmerged_O", "CHb2altmerged_O"])) # assymetry here, see Hb1_O
    acc.addEventAlgo(CompFactory.ComboHypo("CHa2", HypoInputDecisions=["Ha2_O"], HypoOutputDecisions=["CHa2_O"]))
    acc.addEventAlgo(CompFactory.ComboHypo("CHb2", HypoInputDecisions=["Hb2_O"], HypoOutputDecisions=["CHb2_O"]))

    # step 3 (wiht missing branch b)
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fmerged3", Input=["CHa2merged_O","CHb2merged_O", "CHa2altmerged_O", "CHb2altmerged_O"], Output=["Fa3merged_O", "Fb3merged_O"], IOMapping=[[0,2],[1,3]]))
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fa3", Input=["CHa2_O"], Output=["Fa3_O"]))
    acc.addEventAlgo(CompFactory.RoRSeqFilter("Fb3", Input=["CHb2_O", "CHb1_O"], Output=["Fb3_O"], IOMapping=[[0,1]])) # reach to step 1, one output
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMa3", InputMakerInputDecisions=["Fa3_O", "Fa3merged_O"], InputMakerOutputDecisions="IMa3_O"))
    acc.addEventAlgo(CompFactory.EventViewCreatorAlgorithm("IMb3", InputMakerInputDecisions=["Fb3_O", "Fb3merged_O"], InputMakerOutputDecisions="IMb3_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Ha3", HypoInputDecisions="IMa3_O", HypoOutputDecisions="Ha3_O"))
    acc.addEventAlgo(CompFactory.HLTTest.TestHypoAlg("Hb3", HypoInputDecisions="IMb3_O", HypoOutputDecisions="Hb3_O"))
    acc.addEventAlgo(CompFactory.ComboHypo("CHmerged3", HypoInputDecisions=["Ha3_O","Hb3_O"], HypoOutputDecisions=["CHOa3merged", "CHOb3merged"]))
    acc.addEventAlgo(CompFactory.ComboHypo("CHa3", HypoInputDecisions=["Ha3_O"], HypoOutputDecisions=["CHa3_O"]))
    acc.addEventAlgo(CompFactory.ComboHypo("CHb3", HypoInputDecisions=["Hb3_O"], HypoOutputDecisions=["CHb3_O"]))

    acc.printConfig()
    acc.wasMerged()

    # test traversing and collection of info (example hypo)
    colls = []
    def hypoInOut(outCol, alg):
        if isHypoAlg(alg):
            colls.append(alg.HypoInputDecisions.Path)
            colls.append(alg.HypoOutputDecisions.Path)

    traverseDataFlow(acc,  "CHa2_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert set(colls) == set(['Ha2_O', 'IMa2_O', 'Ha1_O', 'IMa1_O']), "Traversing could not find hypo outpus & outputs for branch 'a'"

    colls = []
    traverseDataFlow(acc,  "Fb3_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert set(colls) == set(['Hb1_O', 'IMb1_O', 'Hb2_O', 'IMb2_O']), "Traversing could not find hypo output & input for branch 'b'"

    colls = []
    traverseDataFlow(acc,  "CHa2altmerged_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert 'Ha1_O' in colls
    assert 'Hb2_0' not in colls, "This is another arm of selection 'b'"

    colls = []
    traverseDataFlow(acc,  "CHb2altmerged_O", hypoInOut) # start from ComboHypo  output from where 2nd setup hypo is not reachable
    log.info(colls)
    assert 'Hb1_O' in colls
    assert 'Hb2_0' not in colls, "Due to empty step, this should nto be reachable"


    colls = []
    def collectFilterInputs( outColl, alg ):
        if isFilterAlg(alg):
            idx = alg.Output.index(outColl)
            colls.append(alg.Input[idx])

    traverseDataFlow(acc, "CHa2_O", collectFilterInputs)
    assert  set(colls) == set(["CHa1_O", "CHa1merged_O", "Fa1_I"])
    colls = []
    traverseDataFlow(acc, "CHb2merged_O", collectFilterInputs)
    assert  set(colls) == set(["CHb1_O", "CHb1merged_O", "Fb1_I"])
    log.info("All ok")
