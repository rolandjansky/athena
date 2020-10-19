# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from functools import lru_cache
from AthenaCommon.CFElements import findAllAlgorithms, parOR, seqAND, isSequence
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import (isComboHypoAlg,
                                                             isFilterAlg,
                                                             isHypoAlg,
                                                             isInputMakerBase,
                                                             EmptyMenuSequence)
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

log = logging.getLogger( __name__.split(".")[-1] )



def printStepsMatrix(matrix):
    print('----- Steps matrix ------') # noqa: ATL901
    for nstep in matrix:
        print('step {}:'.format(nstep)) # noqa: ATL901
        for chainName in matrix[nstep]:
            namesInCell = map(lambda el: el.name, matrix[nstep][chainName])
            print('---- {}: {}'.format(chainName, namesInCell))  # noqa: ATL901
    print('-------------------------')  # noqa: ATL901

def resetDF(acc):
    """
    Find all algorithms involved in HLT decision DF and reset properites to empty (lists), unset
    """
    for alg in acc.getEventAlgos():
        if isHypoAlg(alg):
            log.verbose("Resetting IO for %s Hypo", alg.name )
            alg.HypoInputDecisions  = ""
            alg.HypoOutputDecisions = ""
        if isInputMakerBase(alg):
            log.verbose("Resetting IO for %s Input Maker", alg.name )
            alg.InputMakerInputDecisions = []
            alg.InputMakerOutputDecisions = ""
        if isFilterAlg(alg):
            log.verbose("Resetting IO for %s Filter", alg.name )
            alg.Input = []
            alg.Output = []
        if isComboHypoAlg(alg):
            log.verbose("Resetting IO for %s Combo Hypo", alg.name )
            alg.MultiplicitiesMap = {}
            alg.HypoInputDecisions = []
            alg.HypoOutputDecisions = []

def setHypoOutputs(acc):
    for alg in acc.getEventAlgos():
        if isHypoAlg(alg):
            alg.HypoOutputDecisions = CFNaming.hypoAlgOutName(alg.name)

def setIMOutputs(acc):
    for alg in acc.getEventAlgos():
        if isInputMakerBase(alg):
            alg.InputMakerOutputDecisions = CFNaming.inputMakerOutName(alg.name)

def traverse(acc, startCollectionName, functor):
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
                outputs[alg.HypoOutputDecisions] = alg
        if isComboHypoAlg(alg):
            for d in alg.HypoOutputDecisions:
                outputs[d] = alg
        if isInputMakerBase(alg):
            outputs[alg.InputMakerOutputDecisions] = alg
        if isFilterAlg(alg):
            for d in alg.Output:
                outputs[d] = alg

    # for a given collection and algorithm returns list of inputs that lead to it
    # all implementations except for ComboHypo and Filter are trivial
    def __jumpToInputs(outputName, alg):
        if isHypoAlg(alg):
            return [alg.HypoInputDecisions]
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


def generateDecisionTree(chains):
    acc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    acc.addSequence( seqAND(mainSequenceName) )

    @lru_cache(None)
    def getFiltersStepSeq( stepNumber ):
        """
        Returns sequence containing all filters for a step
        """
        name = 'Step{}{}'.format(stepNumber, CFNaming.FILTER_POSTFIX)
        if stepNumber > 1:
            getRecosStepSeq( stepNumber -1 ) # make sure steps sequencers are correctly made: Step1_filter, Step1_recos, Step2_filters, Step2_recos ...
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequenceName )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getRecosStepSeq( stepNumber ):
        """
        Returns sequence containing all reconstruction algorithms
        """
        getFiltersStepSeq( stepNumber ) # make sure there is always filters step before reco
        name = 'Step{}{}'.format(stepNumber, CFNaming.RECO_POSTFIX)
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequenceName )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getSingleMenuSeq( stepNumber, stepName ):
        """
        """
        name = "Menu{}_{}".format(stepNumber, stepName)
        seq = seqAND( name )

        allRecoSeqName = getRecosStepSeq( stepNumber ).name
        acc.addSequence(seq, parentName = allRecoSeqName )
        return acc.getSequence(seq.name)

    @lru_cache(None)
    def getComboSequences( stepNumber, stepName ):
        """
        Produces sequnece encompassing reconstruction sequences needed in combined chains
        """
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
        filtersStep = getFiltersStepSeq(stepNumber)
        singleMenuSeq = getSingleMenuSeq(stepNumber, step.name)
        if isEmpty:
            filterAlg = CompFactory.PassFilter(CFNaming.filterName("Pass"))
        else:
            filterAlg = CompFactory.RoRSeqFilter(CFNaming.filterName(stepName))
        acc.addEventAlgo(filterAlg, sequenceName=filtersStep.name)
        acc.addEventAlgo(filterAlg, sequenceName=singleMenuSeq.name)

        log.debug('Creted filter {}'.format(filterAlg.name))
        return acc.getEventAlgo(filterAlg.name)


    def __findAllOf( stepCounter, stepName, predicate):
        """
        Find all algorithms fulfilling predicate, for sequnces that do not look like menu None is placed in the result instead
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


    def addAndAssureUniqness( prop, toadd, context="" ):
        if isinstance(toadd, str):
            toadd = [toadd]
        missing = []
        for t in toadd:
            if t not in prop:
                missing.append( t )
        return list( prop ) + missing

    def assureUnsetOrTheSame(prop, toadd, context):
        """
        Central function setting strnig like proeprties (collection keys). Assures that valid names are not overwritten.
        """
        if prop == "" or prop == [] or prop == toadd:
            return toadd
        if prop != toadd:
            raise Exception("{}, when setting property found conflicting values, existing {} and new {}".format(context, prop, toadd))

    def clearUnderscores(s):
        """
        Humanizes collection name by removing multiple underscroers and stripping trailing ones
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
    def stepHypoOutput(  stepCounter, chain ):
        """
        Return hypo outputs, if missing traverse sequences back to find one
        """
        hypos = findAllHypoAlgs( stepCounter, chain.steps[stepCounter-1].name )
        if any( [h is None for h in hypos] ):
            prevHypos = stepHypoOutput( stepCounter-1, chain )
            return [ _1 if _1 else _2 for (_1, _2) in zip(hypos, prevHypos) ] # select leter hypo if set
        return hypos


    # CF construction
    # creates all sequences and filter algs, merge CAs from signatures
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            getSingleMenuSeq( stepCounter, step.name )
            getFilterAlg(stepCounter, step.name, isEmpty(step))
            # add sequences that allows reconstructions to be run in parallel, followed (in sequence) by the combo hypo
            comboSeq, comboRecoSeq = getComboSequences( stepCounter, step.name )

            needCombo = False
            for sequence in step.sequences:
                if not isinstance(sequence,EmptyMenuSequence): # empty sequence
                    acc.merge( sequence.ca, sequenceName=comboRecoSeq.name)
                    needCombo = True
                else: # empty sequence
                    acc.addEventAlgo( CompFactory.PassFilter("Pass"),  sequenceName=comboRecoSeq.name)
            if needCombo:
                comboHypo = CompFactory.ComboHypo( "CH"+step.name, CheckMultiplicityMap = len(step.sequences) != 1 )
                acc.addEventAlgo( comboHypo, sequenceName=comboSeq.name )

            pass
    acc.printConfig()
    resetDF(acc)
    setHypoOutputs(acc)
    setIMOutputs(acc)
    log.debug("Configured Hypo Alg and IM Outputs")

    # continue DF setting using chains info
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            if isEmpty(step):
                continue

            def __commonWihtOtherOuptut( newInputColl, filterAlg, comboHypo ):
                """
                For a new input collection that is meant to be added to the filter algorithm check
                if any hypo leading to it has the same input as any other input to that filter
                """
                if len(filterAlg.Output) == 0: # there can not be any commonality if there is no output
                    return None
                colls = []
                def __collectFilterInputs( outColl, alg ):
                    if isFilterAlg(alg):
                        idx = alg.Output.index(outColl)
                        colls.append(alg.Input[idx])
                colls = []
                traverse( acc, newInputColl, __collectFilterInputs)
                newInputHistory = set(colls)
                for existingOutput in filterAlg.Output:
                    colls = []
                    traverse( acc, existingOutput, __collectFilterInputs)
                    if len(set(colls).intersection(newInputHistory)) != 0:
                        return existingOutput
                return None


            prevComboOut, prevComboName = prevStepComboHypoOutput(stepCounter, chain)

            def __setup(sequenceCounter, chainDict):
                """
                Local function to setup filter/input makers/hypo algs IO
                It is meant to be used for simple inclusive chains (im this case the sequenceCounter == 0)
                For comined chains when the there is more than one sequence per step this function is called as many times as many sequences there is
                """
                filterAlg = getFilterAlg(stepCounter, step.name, isEmpty(step))

                comboOut = prevComboOut[sequenceCounter]
                if comboOut not in filterAlg.Input: # input is not yet connected to filter
                    commonWihtOutput = __commonWihtOtherOuptut( comboOut, filterAlg, comboHypo )
                    if commonWihtOutput: # this input has commonalities with other input, appropriate output is returned
                        filterOut = commonWihtOutput
                        filterAlg.IOMapping[ filterAlg.Output.index(commonWihtOutput) ].append( len(filterAlg.Input) )
                    else: # entirely new input
                        filterAlg.IOMapping.append( [len(filterAlg.Input)] )
                        filterOut = clearUnderscores(CFNaming.filterOutName( filterAlg.name, comboOut ).replace(prevComboName, ""))
                        filterAlg.Output = addAndAssureUniqness(filterAlg.Output, filterOut, "{} output".format(filterAlg.name))
                    filterAlg.Input.append( comboOut )
                    filterAlg.ChainsPerInput.append([])
                else: # that input is connected to the filter, so it was already wired correctly to the output, we need to find out to which output it was connected
                    inputIndex = filterAlg.Input.index(comboOut)
                    for outIndex, inputIndices in enumerate(filterAlg.IOMapping):
                        if inputIndex in inputIndices:
                            filterOut = filterAlg.Output[outIndex]

                # set chain to filter on
                filterAlg.ChainsPerInput[filterAlg.Input.index(comboOut)] = addAndAssureUniqness( filterAlg.ChainsPerInput[filterAlg.Input.index(comboOut)], chainDict["chainName"],
                                                                                                    "{} filtered cahins".format(filterAlg.name))
                filterAlg.Chains = addAndAssureUniqness( filterAlg.Chains, chainDict["chainName"],
                                                        "{} filtered cahins".format(filterAlg.name))

                imAlg = findAllInputMakers( stepCounter, step.name )[sequenceCounter]
                if imAlg:
                    imAlg.InputMakerInputDecisions = addAndAssureUniqness( imAlg.InputMakerInputDecisions, filterOut, "{} input".format( imAlg.name ) )

                # IM -> Hypo linking
                hypoAlg = findAllHypoAlgs( stepCounter, step.name )[sequenceCounter]
                if hypoAlg:
                    hypoAlg.HypoInputDecisions = assureUnsetOrTheSame( hypoAlg.HypoInputDecisions, imAlg.InputMakerOutputDecisions,
                        "{} hypo input".format( hypoAlg.name ) )
                    # chain selection is setup here
                    hypoAlg.HypoTools.append( step.sequences[sequenceCounter]._hypoToolConf.confAndCreate( chainDict ) )
                pass

            chainDictLegs = splitChainInDict( chain.name )
            # possible cases: A) number of seqeunces == number of chain parts, e5_mu10 or just e3 type of chain
            # ([0, 1], [0, 1]) is the set of indices
            indices = zip( range( len( step.sequences ) ), range( len( chainDictLegs ) ) )# case A
            # B) number of sequences == 1 && number of chain parts > 1 for single signature assymetric combined chains e5_e3 type chain
            if len(step.sequences) == 1 and len(chainDictLegs) > 1:
                indices = zip( [0]*len(chainDictLegs), range( len( chainDictLegs ) ) )

            for seqCounter, chainDictCounter  in indices:
                chainLegDict = chainDictLegs[chainDictCounter]
                __setup( seqCounter, chainLegDict )

            comboHypoAlg = findComboHypoAlg( stepCounter, step.name )
            if comboHypoAlg:

                comboHypoAlg.MultiplicitiesMap[chain.name] = step.multiplicity
                elementaryHyposOutputs = stepHypoOutput( stepCounter, chain )
                for hypo in elementaryHyposOutputs:
                    if hypo:
                        comboHypoAlg.HypoInputDecisions = addAndAssureUniqness( comboHypoAlg.HypoInputDecisions, hypo.HypoOutputDecisions,
                            "{} comboHypo input".format( comboHypoAlg.name ) )
                        comboOut = CFNaming.comboHypoOutputName( comboHypoAlg.name, hypo.name )
                        comboHypoAlg.HypoOutputDecisions = addAndAssureUniqness( comboHypoAlg.HypoOutputDecisions, comboOut,
                            "{} comboHypo output".format( comboHypoAlg.name ) )
                    # if chan requires special combo tools (TODO understand why not only one tool?)
                    for comboToolConf in step.comboToolConfs:
                        comboHypoAlg.ComboHypoTools.append( comboToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName( chain.name ) ) )

    for chain in chains:
        log.info( "CF algorithms for chain {}".format(chain.name))
        for stepCounter, step in enumerate( chain.steps, 1 ):
            filterAlg = getFilterAlg( stepCounter, step.name, isEmpty(step))
            if filterAlg and hasattr(filterAlg, "Input"):
                log.info("{} FilterAlg: {} input: {} output: {} IO mapping: {}".format(stepCounter, filterAlg.name, ", ".join(filterAlg.Input), ", ".join(filterAlg.Output), filterAlg.IOMapping))
                for inIndex, input in enumerate(filterAlg.Input):
                    log.info("{} filered chains from input: {} : {}".format( stepCounter, input,  ", ".join(filterAlg.ChainsPerInput[inIndex]) ))
                assert len(filterAlg.IOMapping) == len(filterAlg.Output), "Not all output will be filled in filter"

            imAlgs = findAllInputMakers( stepCounter, step.name )
            for imAlg in imAlgs:
                if imAlg:
                    log.info("{}  InputMaker: {} input: {} output: {}".format(stepCounter, imAlg.name, ", ".join(imAlg.InputMakerInputDecisions), imAlg.InputMakerOutputDecisions))

            hypoAlgs = findAllHypoAlgs(stepCounter, step.name)
            for hypoAlg in hypoAlgs:
                if hypoAlg:
                    log.info("{}  HypoAlg: {} input: {} output: {}".format(stepCounter, hypoAlg.name, hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions))
                    log.info("{}  hypo tools: {}".format(stepCounter, ",".join([t.name for t in hypoAlg.HypoTools])))
            combo = findComboHypoAlg(stepCounter, step.name)
            if combo:
                log.info("{}  ComboHypoAlg: {} input: {} output: {}".format(stepCounter, combo.name, ". ".join(combo.HypoInputDecisions), ", ".join(combo.HypoOutputDecisions)))
                log.info("{}  multiplicities: {}".format(stepCounter, combo.MultiplicitiesMap))
                assert len(combo.HypoInputDecisions) == len(combo.HypoInputDecisions), "Missconfiguraiton of {} ComboHypo input/output counts differ".format(combo.name)
        log.info("-"*50)
    log.info("")
    return acc


if __name__ == "__main__":
    # Test of DF collections traversal
    # build hypothetical structure of DF to test back traversing
    # make it ressemble the combined chain with one arm that is hollow/empty
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
            colls.append(alg.HypoInputDecisions)
            colls.append(alg.HypoOutputDecisions)

    traverse(acc,  "CHa2_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert set(colls) == set(['Ha2_O', 'IMa2_O', 'Ha1_O', 'IMa1_O']), "Traversing could not find hypo outpus & outputs for branch 'a'"

    colls = []
    traverse(acc,  "Fb3_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert set(colls) == set(['Hb1_O', 'IMb1_O', 'Hb2_O', 'IMb2_O']), "Traversing cound not find hypo output & input for branch 'b'"

    colls = []
    traverse(acc,  "CHa2altmerged_O", hypoInOut) # start from ComboHypo  output
    log.info(colls)
    assert 'Ha1_O' in colls
    assert 'Hb2_0' not in colls, "This is another arm of selection 'b'"

    colls = []
    traverse(acc,  "CHb2altmerged_O", hypoInOut) # start from ComboHypo  output from where 2nd setep hypo is not reachable
    log.info(colls)
    assert 'Hb1_O' in colls
    assert 'Hb2_0' not in colls, "Due to empty step, this shoudl nto be reachable"


    colls = []
    def collectFilterInputs( outColl, alg ):
        if isFilterAlg(alg):
            idx = alg.Output.index(outColl)
            colls.append(alg.Input[idx])

    traverse(acc, "CHa2_O", collectFilterInputs)
    assert  set(colls) == set(["CHa1_O", "CHa1merged_O", "Fa1_I"])
    colls = []
    traverse(acc, "CHb2merged_O", collectFilterInputs)
    assert  set(colls) == set(["CHb1_O", "CHb1merged_O", "Fb1_I"])
    log.info("All ok")
