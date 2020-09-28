# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from PyUtils.Decorators import memoize
from AthenaCommon.CFElements import findAllAlgorithms, parOR, seqAND
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import (isComboHypoAlg,
                                                             isHypoBase,
                                                             isInputMakerBase)
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

log = logging.getLogger( __name__ )

def printStepsMatrix(matrix):
    print('----- Steps matrix ------') # noqa: ATL901
    for nstep in matrix:
        print('step {}:'.format(nstep)) # noqa: ATL901
        for chainName in matrix[nstep]:
            namesInCell = map(lambda el: el.name, matrix[nstep][chainName])
            print('---- {}: {}'.format(chainName, namesInCell))  # noqa: ATL901
    print('-------------------------')  # noqa: ATL901

# def memoize(f):
#     """ caches call of the helper functions, (copied from the internet) remove when we move to python 3.2 or newer and rplace by functools.lru_cache"""
#     memo = {}
#     def helper(*x):
#         tupledx = tuple(x)
#         if tupledx not in memo:
#             memo[tupledx] = f(*x)
#         return memo[tupledx]
#     return helper

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
    def getComboSequences( stepNumber, stepName ):
        """
        """
        singleMenuSeqName = getSingleMenuSeq( stepNumber, stepName ).name

        stepComboName = "Combo{}{}".format(stepNumber,stepName)
        acc.addSequence( seqAND(stepComboName), parentName=singleMenuSeqName )

        stepComboRecoName ="ComboReco{}{}".format(stepNumber, stepName)
        acc.addSequence( parOR(stepComboRecoName), parentName=stepComboName )
        return acc.getSequence(stepComboName), acc.getSequence(stepComboRecoName)

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
    def findComboHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isComboHypoAlg(alg):
                return alg
        raise Exception("No combo hypo alg in seq "+seq.name)

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
        if prop == "" or prop == toadd:
            return toadd
        if prop != toadd:
            raise Exception("{}, when setting property found conflicting values, existing {} and new {}".format(context, prop, toadd))

    def clearUnderscores(s):
        p = s
        while True:
            n = p.replace("__", "_")
            if p == n:
                return p.rstrip("_")
            p = n

    @memoize
    def prevStepOutput( chain, stepCounter ):
        """
        Returns list of decision collections that are outputs of previous step as well as the hypo alg name that outpus it
        """
        prevHypoAlgName = ""
        if stepCounter == 1: # L1 seed
            out = chain.L1decisions
        else:
            prevCounter = stepCounter-1
            prevName = chain.steps[prevCounter-1].name # counting steps from 1, for indexing need one less
            prevStep = chain.steps[prevCounter-1]
            if prevStep.isCombo:                
                prevHypoAlg = findComboHypoAlg( prevCounter, prevName )
            else:
                prevHypoAlg = findHypoAlg( prevCounter, prevName )
            out = prevHypoAlg.HypoOutputDecisions
            prevHypoAlgName = prevHypoAlg.name

        return [out] if isinstance( out, str) else out, prevHypoAlgName # normalise to list

    # CF construction logic
    # create all sequences and filter algs, merge CAs from signatures (decision CF)
    for chain in chains:
        for stepCounter, step in enumerate( chain.steps, 1 ):
            getFilterAlg( stepCounter, step.name )
            menuSeqName = getSingleMenuSeq( stepCounter, step.name ).name
            log.info("zzz {} mult {} step {} is combo {}".format(chain.name, step.multiplicity, stepCounter, step.isCombo))

            if step.isCombo:
                # add sequences that allows reconstructions to be run in parallel, followed (in sequence) by the combo hypo
                comboSeq, comboRecoSeq = getComboSequences( stepCounter, step.name )
                for sequence in step.sequences:
                    acc.merge( sequence.ca, sequenceName=comboRecoSeq.name)

                comboHypo = CompFactory.ComboHypo( "CH"+step.name )
                acc.addEventAlgo( comboHypo, sequenceName=comboSeq.name )
                pass
            else:
                acc.merge( step.sequences[0].ca, sequenceName=menuSeqName )


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


    # connect all outputs (decision DF) and add chains to filter on
    for chain in chains:

        for stepCounter, step in enumerate( chain.steps, 1 ):
            # Filters linking
            filterAlg = getFilterAlg( stepCounter, step.name )

            def __setup(sequenceCounter, chainDict):
                '''
                Local function to setup filter/input makers/hypo algs  IO
                '''
                # set chain to filter on
                filterAlg.Chains = addAndAssureUniqness( filterAlg.Chains, chainDict["chainName"], "{} filter alg chains".format( filterAlg.name ) )

                filterIn, prevHypoName = prevStepOutput( chain, stepCounter)
                filterAlg.Input = addAndAssureUniqness( filterAlg.Input, filterIn, "{} input".format( filterAlg.name ) )
                filterOut = [ clearUnderscores(CFNaming.filterOutName( filterAlg.name,  s ).replace(prevHypoName, "")) for s in filterIn ]
                filterAlg.Output = addAndAssureUniqness( filterAlg.Output, filterOut, "{} output".format( filterAlg.name ) )

                im = findAllInputMakers( stepCounter, step.name )[sequenceCounter]
                im.InputMakerInputDecisions = addAndAssureUniqness( im.InputMakerInputDecisions,  filterOut[sequenceCounter], "{} input".format( im.name ) )
                imOut = CFNaming.inputMakerOutName( im.name )
                im.InputMakerOutputDecisions = assureUnsetOrTheSame( im.InputMakerOutputDecisions, imOut, "{} IM output".format( im.name ) )

                # Hypo linking
                hypoAlg = findAllHypoAlgs( stepCounter, step.name )[sequenceCounter]
                hypoAlg.HypoInputDecisions = assureUnsetOrTheSame( hypoAlg.HypoInputDecisions, im.InputMakerOutputDecisions,
                    "{} hypo input".format( hypoAlg.name ) )
                hypoOut = CFNaming.hypoAlgOutName( hypoAlg.name )
                hypoAlg.HypoOutputDecisions = assureUnsetOrTheSame( hypoAlg.HypoOutputDecisions, hypoOut,
                    "{} hypo output".format( hypoAlg.name )  )
                print("kkk", chainDict)
                hypoAlg.HypoTools.append( step.sequences[sequenceCounter]._hypoToolConf.confAndCreate( chainDict ) )
                pass

            if step.isCombo:
                for seqCounter in range( len( step.sequences ) ) :
                    chainLegDict = splitChainInDict( chain.name )[seqCounter]
                    # hack for multiplicity chains of the same signature, it should be handled in splitChainInDrict, TODO eliminate it, needs discussion wiht menu
                    if len( step.sequences ) == 1: 
                        from DecisionHandling.TrigCompositeUtils import legName
                        chainLegDict['chainName'] = legName( chainLegDict['chainName'], 0)
                    __setup( seqCounter, chainLegDict )

                    comboHypoAlg = findComboHypoAlg( stepCounter, step.name )
                    comboHypoAlg.MultiplicitiesMap[chain.name] = step.multiplicity

                    elementaryHypos = findAllHypoAlgs( stepCounter, step.name )
                    for hypo in elementaryHypos:
                        if hypo == comboHypoAlg:
                            continue
                        comboHypoAlg.HypoInputDecisions = addAndAssureUniqness( comboHypoAlg.HypoInputDecisions, hypo.HypoOutputDecisions,
                            "{} comboHypo input".format( comboHypoAlg.name ) )

                        comboOut = CFNaming.comboHypoOutputName( comboHypoAlg.name, hypo.name )
                        comboHypoAlg.HypoOutputDecisions = addAndAssureUniqness( comboHypoAlg.HypoOutputDecisions, comboOut,
                            "{} comboHypo output".format( comboHypoAlg.name ) )

                    # Combo Hypo Tools
                    for comboToolConf in step.comboToolConfs:
                        comboHypoAlg.ComboHypoTools.append( comboToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName( chain.name ) ) )

            else:
                assert len( step.sequences ) == 1, "chain {} step {} is not combo bye has number of sequences = {}".format( chain.name, stepCounter, len( step.sequences ) )
                __setup( 0,  TriggerConfigHLT.getChainDictFromChainName( chain.name ) )


    for chain in chains:
        log.info( "CF algorithms for chain {}".format( chain.name ) )
        for stepCounter, step in enumerate( chain.steps, 1 ):
            filterAlg = getFilterAlg( stepCounter, step.name )
            log.info(" FilterAlg {} Inputs {} Outputs {} Chains {}".format( filterAlg.name, filterAlg.Input, filterAlg.Output, filterAlg.Chains ) )

            imAlg = findInputMaker( stepCounter, step.name )
            log.info("  InputMaker {} Inputs {} Outputs {}".format( imAlg.name, imAlg.InputMakerInputDecisions, imAlg.InputMakerOutputDecisions ) )
            if step.isCombo:
                hypoAlgs = findAllHypoAlgs( stepCounter, step.name )
                for hypoAlg in hypoAlgs:
                    if isComboHypoAlg(hypoAlg):
                        continue
                    log.info("   HypoAlg {} Inputs {} Outputs {} Tools {}".format( hypoAlg.name, hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions, [t.name for t in hypoAlg.HypoTools] ) )
                combo = findComboHypoAlg(  stepCounter, step.name )
                log.info("  ComboHypoAlg {} Inputs {} Outputs {} Multiplicities {}".format( combo.name, combo.HypoInputDecisions, combo.HypoOutputDecisions, combo.MultiplicitiesMap ) )

            else:
                hypoAlg = findHypoAlg( stepCounter, step.name )
                log.info("  HypoAlg {} Inputs {} Outputs {} Tools {}".format( hypoAlg.name, hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions, [t.name for t in hypoAlg.HypoTools] ) )
    #kaboom
    return acc



def createControlFlowNewJO(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences in newJO"""
    return
