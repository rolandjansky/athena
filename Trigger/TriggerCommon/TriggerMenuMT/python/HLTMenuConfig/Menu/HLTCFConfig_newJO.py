# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import isInputMakerBase, isHypoBase, isComboHypoAlg
from AthenaCommon.CFElements import parOR, seqAND, findAllAlgorithms
from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

def printStepsMatrix(matrix):
    print('----- Steps matrix ------') # noqa: ATL901
    for nstep in matrix:
        print('step {}:'.format(nstep)) # noqa: ATL901
        for chainName in matrix[nstep]:
            namesInCell = map(lambda el: el.name, matrix[nstep][chainName])
            print('---- {}: {}'.format(chainName, namesInCell))  # noqa: ATL901
    print('-------------------------')  # noqa: ATL901

def memoize(f):
    """ caches call of the helper functions, (copied from the internet) remove when we move to python 3.2 or newer and rplace by functools.lru_cache"""
    memo = {}
    def helper(*x):
        tupledx = tuple(x)
        if tupledx not in memo:
            memo[tupledx] = f(*x)
        return memo[tupledx]
    return helper

def generateDecisionTree(chains):
    acc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    acc.addSequence( seqAND(mainSequenceName) )

    @memoize
    def getFiltersStepSeq( stepNumber ):
        """
        Returns sequence containing all filters for a step
        """
        name = 'Step{}_{}'.format(stepNumber, CFNaming.FILTER_POSTFIX)
        if stepNumber > 1:
            getRecosStepSeq( stepNumber -1 ) # make sure steps sequencers are correctly made: Step1_filter, Step1_recos, Step2_filters, Step2_recos ...
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequenceName )
        return seq

    @memoize
    def getRecosStepSeq( stepNumber ):
        """
        """
        getFiltersStepSeq( stepNumber ) # make sure there is filters step before recos
        name = 'Step{}{}'.format(stepNumber, CFNaming.RECO_POSTFIX)
        seq = parOR( name )
        acc.addSequence( seq, parentName = mainSequenceName )
        return seq

    @memoize
    def getSingleMenuSeq( stepNumber, stepName ):
        """
        """
        name = "Menu{}{}".format(stepNumber, stepName)
        seq = seqAND( name )
        allRecoSeqName = getRecosStepSeq( stepNumber ).name
        acc.addSequence(seq, parentName = allRecoSeqName )
        return seq


    @memoize
    def getFilterAlg( stepNumber, stepName ):
        """
        Returns, if need be created, filter for a given step
        """

        filtersStep = getFiltersStepSeq( stepNumber )
        singleMenuSeq = getSingleMenuSeq( stepNumber, stepName )

        filterName = CFNaming.filterName( stepName )
        filterAlg = CompFactory.RoRSeqFilter( filterName )

        acc.addEventAlgo( filterAlg, sequenceName=filtersStep.name )
        acc.addEventAlgo( filterAlg, sequenceName=singleMenuSeq.name )

        log.debug('Creted filter {}'.format(filterName))
        return filterAlg
            
    @memoize
    def findInputMaker( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isInputMakerBase(alg):
                return alg
        raise Exception("No input maker in seq "+seq.name)

    @memoize
    def findAllInputMakers( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        result = []
        for alg in algs:
            if isInputMakerBase(alg):
                result.append(alg)

        if result:
            return result
        else:
            raise Exception("No input maker in seq "+seq.name)

    @memoize
    def findHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isHypoBase(alg):
                return alg
        raise Exception("No hypo alg in seq "+seq.name)

    @memoize
    def findAllHypoAlgs( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        result = []
        for alg in algs:
            if isHypoBase(alg):
                result.append(alg)

        if result:
            return result
        else:
            raise Exception("No hypo alg in seq "+seq.name)

    @memoize
    def findComboHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isComboHypoAlg(alg):
                return alg
        raise Exception("No combo hypo alg in seq "+seq.name)

    def addAndAssureUniqness( prop, toadd, context="" ):
        if toadd not in prop:
            log.info("{} value {} not there".format(context, toadd))
            return list( prop ) + [ toadd ]
        else:
            log.info("{} value {} already there".format(context, toadd))
            return list( prop )

    def assureUnsetOrTheSame(prop, toadd, context):
        """
        Central function setting strnig like proeprties (collection keys). Assures that valid names are not overwritten.
        """
        if prop == "" or prop == toadd:
            return toadd
        if prop != toadd:
            raise Exception("{}, when setting property found conflicting values, existing {} and new {}".format(context, prop, toadd))


    # create all sequences and filter algs, merge CAs from signatures (decision CF)
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            getFilterAlg( stepCounter, step.name )
            recoSeqName = getSingleMenuSeq( stepCounter, step.name ).name

            if step.isCombo:
                # add merged reco sequence
                stepRecoName = step.name + CFNaming.RECO_POSTFIX
                stepViewName = step.name + CFNaming.VIEW_POSTFIX

                acc.addSequence( seqAND(stepViewName), parentName=recoSeqName )
                acc.addSequence( parOR(stepRecoName), parentName=stepViewName )

                for sequence in step.sequences:
                    for stepView in sequence.ca.getSequence().Members:
                        for viewMember in stepView.Members:
                            if isHypoBase(viewMember):
                                # add hypo alg to view sequence
                                acc.addEventAlgo( viewMember, sequenceName=stepViewName )
                            else:
                                # add reco sequence to merged _reco
                                for recoAlg in viewMember.Members:
                                    acc.addSequence( recoAlg, parentName=stepRecoName )

                    # elements from ca were moved above to the appropriate sequences
                    # so sequence and algorithms are considered as merged
                    sequence.ca._algorithms = {}
                    sequence.ca._sequence.Members = []
                    acc.merge(sequence.ca, sequenceName=recoSeqName)

                # create combo hypo
                comboHypo = CompFactory.ComboHypo( step.combo.Alg.getName() )
                acc.addEventAlgo( comboHypo, sequenceName=stepViewName )

            else:
                acc.merge( step.sequences[0].ca, sequenceName=recoSeqName )


    # cleanup settings made by Chain & related objects (can be removed in the future)
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            filterAlg = getFilterAlg( stepCounter, step.name )
            filterAlg.Input = []
            filterAlg.Output = []

            imAlgs = findAllInputMakers( stepCounter, step.name )
            for imAlg in imAlgs:
                imAlg.InputMakerInputDecisions = []
                imAlg.InputMakerOutputDecisions = ""

            hypoAlgs = findAllHypoAlgs( stepCounter, step.name )
            for hypoAlg in hypoAlgs:
                hypoAlg.HypoInputDecisions  = ""
                hypoAlg.HypoOutputDecisions = ""

            if step.isCombo:
                comboHypoAlg = findComboHypoAlg( stepCounter, step.name )
                comboHypoAlg.MultiplicitiesMap = {}
                comboHypoAlg.HypoInputDecisions = []
                comboHypoAlg.HypoOutputDecisions = []


    # connect all outputs (decision DF)
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            for seqCounter, sequence in enumerate( step.sequences ):

                # Filters linking
                filterAlg = getFilterAlg( stepCounter, step.name )
                if step.isCombo:
                    chainDictLegs = ' '.join(map(str, [dic['chainName'] for dic in step.chainDicts]))
                    filterAlg.Chains = addAndAssureUniqness( filterAlg.Chains, chainDictLegs, "{} filter alg chains".format( filterAlg.name ) )
                else:
                    filterAlg.Chains = addAndAssureUniqness( filterAlg.Chains, chain.name, "{} filter alg chains".format( filterAlg.name ) )

                if stepCounter == 1:
                    filterAlg.Input = addAndAssureUniqness( filterAlg.Input, chain.L1decisions[0], "{} L1 input".format( filterAlg.name ) )
                else: # look into the previous step
                    hypoOutput = findHypoAlg( stepCounter-1, chain.steps[chain.steps.index( step )-1].name ).HypoOutputDecisions
                    filterAlg.Input = addAndAssureUniqness( filterAlg.Input, hypoOutput, "{} input".format( filterAlg.name ) )

                # Input Maker linking
                im = findAllInputMakers( stepCounter, step.name )[seqCounter]
                for i in filterAlg.Input:
                    filterOutputName = CFNaming.filterOutName( filterAlg.name, i )
                    filterAlg.Output = addAndAssureUniqness( filterAlg.Output, filterOutputName, "{} output".format( filterAlg.name ) )
                    im.InputMakerInputDecisions = addAndAssureUniqness( im.InputMakerInputDecisions,  filterOutputName, "{} input".format( im.name ) )

                imOutputName = CFNaming.inputMakerOutName( im.name )
                im.InputMakerOutputDecisions = assureUnsetOrTheSame( im.InputMakerOutputDecisions, imOutputName, "{} IM output".format( im.name ) )
                
                # Hypo linking
                hypoAlg = findAllHypoAlgs( stepCounter, step.name )[seqCounter]
                hypoAlg.HypoInputDecisions = assureUnsetOrTheSame( hypoAlg.HypoInputDecisions, im.InputMakerOutputDecisions,
                    "{} hypo input".format( hypoAlg.name ) )
                hypoOutName = CFNaming.hypoAlgOutName( hypoAlg.name )
                hypoAlg.HypoOutputDecisions = assureUnsetOrTheSame( hypoAlg.HypoOutputDecisions, hypoOutName,
                    "{} hypo output".format( hypoAlg.name )  )

                # Hypo Tools
                if step.isCombo:
                    from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
                    chainDictLeg = splitChainInDict(chain.name)[seqCounter]
                    hypoAlg.HypoTools.append( sequence._hypoToolConf.confAndCreate( chainDictLeg ) )

                    # to be deleted after ComboHypos will be properly configured and included in DF
                    hypoAlg.HypoTools.append( sequence._hypoToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName( chain.name ) ) )
                else:
                    hypoAlg.HypoTools.append( sequence._hypoToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName( chain.name ) ) )

            # Combo Hypo linking
            if step.isCombo:
                comboHypoAlg = findComboHypoAlg( stepCounter, step.name )
                comboHypoAlg.MultiplicitiesMap[chain.name] = step.multiplicity

                comboInputList = findAllHypoAlgs( stepCounter, step.name )
                for comboInput in comboInputList:
                    comboHypoAlg.HypoInputDecisions = addAndAssureUniqness( comboHypoAlg.HypoInputDecisions, comboInput.name, 
                        "{} comboHypo input".format( comboHypoAlg.name ) )
                    
                    comboOutName = CFNaming.comboHypoOutputName( comboHypoAlg.name, comboInput.name )
                    comboHypoAlg.HypoOutputDecisions = addAndAssureUniqness( comboHypoAlg.HypoOutputDecisions, comboOutName, 
                        "{} comboHypo output".format( comboHypoAlg.name ) )

                # Combo Hypo Tools
                for comboToolConf in step.comboToolConfs:
                    comboHypoAlg.ComboHypoTools.append( comboToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName( chain.name ) ) )


    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            filterAlg = getFilterAlg( stepCounter, step.name )
            log.info("FilterAlg {} Inputs {} Outputs {}".format( filterAlg.name, filterAlg.Input, filterAlg.Output ) )

            imAlg = findInputMaker( stepCounter, step.name )
            log.info("InputMaker {} Inputs {} Outputs {}".format( imAlg.name, imAlg.InputMakerInputDecisions, imAlg.InputMakerOutputDecisions ) )

            hypoAlg = findHypoAlg( stepCounter, step.name )
            log.info("HypoAlg {} Inputs {} Outputs {}".format( hypoAlg.name, hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions ) )

    return acc



def createControlFlowNewJO(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences in newJO"""
    return
