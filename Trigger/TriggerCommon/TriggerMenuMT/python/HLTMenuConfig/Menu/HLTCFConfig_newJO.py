# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeSummary
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import stepCF_DataFlow_to_dot, \
    stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import isInputMakerBase, isHypoBase
from AthenaCommon.CFElements import parOR, seqAND, findAlgorithm, findAllAlgorithms
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
        Returns, if need be create, filter for a given step
        """
        
        filtersStep = getFiltersStepSeq( stepNumber )
        singleRecSeq = getSingleMenuSeq( stepNumber, stepName )
        
        filterName = CFNaming.filterName( stepName )
        filterAlg = CompFactory.RoRSeqFilter( filterName )
        
        acc.addEventAlgo( filterAlg, sequenceName=filtersStep.name )
        acc.addEventAlgo( filterAlg, sequenceName=singleRecSeq.name )
                             
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
    def findHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            if isHypoBase(alg):
                return alg
        raise Exception("No hypo alg in seq "+seq.name)
        
    def addIfNotThere( prop, toadd, context="" ):
        if toadd not in prop:
            log.info("{} adding value {} to property".format(context, toadd))
            prop.append( toadd )
        else:
            log.info("{} value {} already there".format(context, toadd))

    def setOrCheckIfTheSame( prop, toadd, context):
        if prop == "":
            return toadd
        if prop != toadd:
            raise Exception("{}, when setting property found conflicting values, existing {} and new {}".format(context, prop, toadd))


    #create all sequences and filter algs, merge CAs from signatures (decision CF)
    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            for sequence in step.sequences:
                getFilterAlg(stepCounter, step.name)
                recoSeqName = getSingleMenuSeq(stepCounter, step.name).name
                acc.merge( sequence.ca, sequenceName=recoSeqName )

    # cleanup settings made by Chain object (will be removed in the future)
    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            filterAlg = getFilterAlg(stepCounter, step.name)
            filterAlg.Input = []
            filterAlg.Output = []

            imAlg = findInputMaker( stepCounter, step.name )
            imAlg.InputMakerInputDecisions = []
            imAlg.InputMakerOutputDecisions = ""

            hypoAlg = findHypoAlg( stepCounter, step.name )
            hypoAlg.HypoInputDecisions  = ""
            hypoAlg.HypoOutputDecisions = ""
            
    # connect all outputs (decision DF)
    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            for sequence in step.sequences:

                filterAlg = getFilterAlg(stepCounter, step.name)
                addIfNotThere( filterAlg.Chains, chain.name, "{} filter alg chains".format(filterAlg.name) )
                if stepCounter == 1:
                    addIfNotThere( filterAlg.Input, chain.L1decisions[0], "{} L1 input".format(filterAlg.name) )
                else: # look into the previous step, index -2 is because we count steps from 1
                    hypoOutput = findHypoAlg( stepCounter-1, chain.steps[chain.steps.index(step)-1].name ).HypoOutputDecisions
                    addIfNotThere( filterAlg.Input, hypoOutput, "{} input".format(filterAlg.name) )

                im = findInputMaker( stepCounter, step.name )
                for i in filterAlg.Input:
                    filterOutputName = CFNaming.filterOutName( filterAlg.name, i )
                    addIfNotThere( filterAlg.Output, filterOutputName, "{} output".format(filterAlg.name) )
                    addIfNotThere( im.InputMakerInputDecisions,  filterOutputName, "{} input".format(im.name))
                    
                hypoAlg = findHypoAlg( stepCounter, step.name )
                for i in im.InputMakerInputDecisions:
                    imOutputName = CFNaming.inputMakerOutName( im.name, i )
                    setOrCheckIfTheSame( im.InputMakerOutputDecisions, imOutputName, "{} output".format( im.name ) )
                    hypoAlg.HypoInputDecisions = setOrCheckIfTheSame( hypoAlg.HypoInputDecisions, imOutputName, "{} hypo input".format(hypoAlg.name) )
                for i in hypoAlg.HypoInputDecisions:
                    hypoOutName = CFNaming.hypoAlgOutNameOld( hypoAlg.name, i)
                    hypoAlg.HypoOutputDecisions = setOrCheckIfTheSame( hypoAlg.HypoOutputDecisions, hypoOutName, "{} hypo output".format(hypoAlg.name) )
                    
                hypoAlg.HypoTools.append( sequence._hypoToolConf.confAndCreate( TriggerConfigHLT.getChainDictFromChainName(chain.name ) ) )                                                

    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            filterAlg = getFilterAlg(stepCounter, step.name)
            log.info("FilterAlg {} Inputs {} Outputs ".format(filterAlg.name, filterAlg.Input, filterAlg.Output ))

            imAlg = findInputMaker( stepCounter, step.name )
            log.info("InputMaker {} Inputs {} Outputs ".format(imAlg.name, imAlg.InputMakerInputDecisions, imAlg.InputMakerOutputDecisions ))

            hypoAlg = findHypoAlg( stepCounter, step.name )
            log.info("HypoAlg {} Inputs {} Outputs ".format(hypoAlg.name, hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions ))

    #kaboom
    return acc



def createControlFlowNewJO(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences in newJO"""     
    return
