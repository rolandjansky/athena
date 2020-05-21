# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
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
        acc.addSequence( parOR( name ), parentName = mainSequenceName )
        return seq

    @memoize
    def getSingleMenuSeq( stepNumber, stepName ):
        """
        """
        name = "Menu{}{}".format(stepNumber, stepName)
        seq = parOR( name )
        allRecoSeqName = getRecosStepSeq( stepNumber ).name        
        acc.addSequence( seqAND( name ), parentName = allRecoSeqName )
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

            
    def addIfNotThere( prop, toadd ):
        if toadd not in prop:
            prop.append( toadd )

    @memoize 
    def findInputMaker( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        print("HERE "+seq.name)
        algs = findAllAlgorithms( seq )
        print("HERE "+str(len(algs)))
        for alg in algs:
            print("HERE"+alg.name)
            if isInputMakerBase(alg):
                return alg
        raise "No input maker in seq "+seq.name

    @memoize
    def findHypoAlg( stepCounter, stepName ):
        seq = getSingleMenuSeq( stepCounter, stepName )
        algs = findAllAlgorithms( seq )
        for alg in algs:
            print("HERE"+alg.name)
            if isHypoBase(alg):
                return alg
        raise "No hypo alg in seq "+seq.name

    
    #create all sequences and filter algs, add Hypo algs and tools (decision CF)
    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            for sequence in step.sequences:
                getFilterAlg(stepCounter, step.name)
                recoSeqName = getSingleMenuSeq(stepCounter, step.name).name
                acc.merge( sequence.ca, sequenceName=recoSeqName )
    acc.printConfig()
                
    # connect all outputs (decision DF)
    for chain in chains:
        for stepCounter, step in enumerate(chain.steps, 1):
            for sequence in step.sequences:
                filterAlg = getFilterAlg(stepCounter, step.name)
                addIfNotThere( filterAlg.Chains, chain.name )
                if stepCounter == 1:
                    addIfNotThere( filterAlg.Input, chain.L1decisions )
                else: # looks to the previous step
                    hypoOutput = findSingleRecoSeqOutput( chain.steps[chainCounter-1] )
                    addIfNotThere( filterAlg.Input, hypoOutput )
                    
                im = findInputMaker( stepCounter, step.name )
                for i in filterAlg.Input:
                    filterOutputName = CFNaming.filterOutName( i )
                    addIfNotThere( filterAlg.Ouput, filterOutName )
                    addIfNotThere( im.InputMakerInpuDecisions,  filterOutName )
                    
                hypoAlg = findHypo( stepConter, step )
                for i in im.InputMakerInpuDecisions:
                    imOutputName = CFNaming.inputMakerOutName( im.name, i )
                    addIfNotThere( hypoAlg.InputDecisions, imOutputName )
                for i in hypoAlg.InputDecisions:
                    hypoOutName = CFNaming.hypoAlgOutNameOld( hypoAlg.name, i)
                    addIfNotThere( hypoAlg.DecisonOutput, hypoOutName )

                hypoAlg.HypoTools.append( sequence._hypoToolConf.create() )
                            
    acc.printConfig()

    kaboom
    return acc



def createControlFlowNewJO(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences in newJO"""     
    return
