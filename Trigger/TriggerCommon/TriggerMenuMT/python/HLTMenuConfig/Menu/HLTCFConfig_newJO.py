# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeSummary
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import stepCF_DataFlow_to_dot, \
    stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence, createStepView, SequenceFilterNode
from AthenaCommon.CFElements import parOR, seqAND, findAlgorithm
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


def generateDecisionTree(chains):
    acc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    acc.addSequence( seqAND(mainSequenceName) )

    log.debug('Generating decision tree with main sequence: {}'.format(mainSequenceName))

    chainStepsMatrix = defaultdict(lambda: defaultdict(list))

    ## Fill chain steps matrix
    for index, chain in enumerate(chains):
        for stepNumber, chainStep in enumerate(chain.steps):
            chainStepsMatrix[stepNumber][chainStep.name].append(chain)

    printStepsMatrix(chainStepsMatrix)

    allCFSequences = []
    

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in sorted(chainStepsMatrix.keys()):
        stepDecisions = []

        stepName = CFNaming.stepName(nstep)

        stepFilterNodeName = '{}{}'.format(stepName, CFNaming.FILTER_POSTFIX)
        filterAcc = ComponentAccumulator()
        filterAcc.addSequence( parOR(stepFilterNodeName) )

        stepRecoNodeName = CFNaming.stepRecoNodeName(mainSequenceName, stepName)
        stepRecoNode = parOR(stepRecoNodeName)
        recoAcc = ComponentAccumulator()
        recoAcc.addSequence(stepRecoNode)

        CFSequences = []

        chainCounter=0
        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                # If cell is empty, there is nothing to do
                continue

            firstChain = chainsInCell[0]
            if nstep == 0:
                filter_input = firstChain.L1decisions
            else:
                #tmp FP replacement, but it should not work properly
                #filter_input = [output for sequence in firstChain.steps[nstep - 1].sequences for output in sequence.outputs]
                filter_input = [output for output in allCFSequences[nstep - 1][chainCounter].decisions]

            chainStep = firstChain.steps[nstep]

            # One aggregated filter per chain (one per column in matrix)
            filterName = CFNaming.filterName(chainStep.name)
            #filter = buildFilter(filterName, filter_input)

            # append input to filter if it exists
            filterAlg = findAlgorithm( filterAcc.getSequence(), filterName )
            if filterAlg:
                filterAlg.Input += filter_input
            else:
                filterAlg = CompFactory.RoRSeqFilter(filterName, Input=filter_input )
                filterAcc.addEventAlgo(filterAlg, sequenceName = stepFilterNodeName)

            stepReco, stepView = createStepView(chainStep.name)
            viewWithFilter = seqAND(chainStep.name, [filterAlg, stepView])

            sfilter = SequenceFilterNode(filterAlg, 'Input', 'Output')
            recoAcc.addSequence(viewWithFilter, parentName = stepRecoNodeName)

            stepsAcc = ComponentAccumulator()

            CFSequenceAdded = False
            filter_output =[]
            for i in filter_input: 
                filter_output.append( CFNaming.filterOutName(filterName, i))

            for chain in chainsInCell:
                step = chain.steps[nstep]
                CFSeq = CFSequence(step, sfilter)
                CFSeq.connect(filter_output)
                if not CFSequenceAdded:
                    CFSequences.append(CFSeq)
                    CFSequenceAdded = True
                for seq in step.sequences:
                    if seq.ca is None:
                        raise ValueError('ComponentAccumulator missing in sequence {} in chain {}'.format(seq.name, chain.name))
                    stepsAcc.merge( seq.ca )
                if step.isCombo:
                    if step.combo is not None:
                        stepsAcc.addEventAlgo(step.combo.Alg, sequenceName = stepView.getName())
                sfilter.addChain(chain.name)

            recoAcc.merge(stepsAcc, sequenceName = viewWithFilter.getName())
            chainCounter+=1
            
        for sequence in CFSequences:
            stepDecisions += sequence.decisions



        acc.merge(filterAcc, sequenceName = mainSequenceName)
        acc.merge(recoAcc, sequenceName = mainSequenceName)

        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)
        acc.addEventAlgo(summary, sequenceName = mainSequenceName)

        allCFSequences.append(CFSequences)

        stepCF_DataFlow_to_dot(stepRecoNodeName, CFSequences)
        stepCF_ControlFlow_to_dot(stepRecoNode)

    acc.printConfig()

    all_DataFlow_to_dot(mainSequenceName, allCFSequences)

    return acc



def createControlFlowNewJO(HLTNode, CFseq_list):
    """ Creates Control Flow Tree starting from the CFSequences in newJO"""
    
 
    return
