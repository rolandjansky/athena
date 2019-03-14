# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import buildFilter, makeSummary
from AthenaCommon.CFElements import parOR, seqAND, seqOR, findOwningSequence
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import VERBOSE

log = logging.getLogger('HLTCFConfig_newJO')
log.setLevel( VERBOSE )


def connectStepToFilter(chainStep, filterNode):
    filter_output = filterNode.getOutputList()
    if len(filter_output) == 0:
        raise ValueError('ERROR: no filter outputs are set')

    if len(filter_output) != len(chainStep.sequences):
        msg = 'ERROR: found {} filter outputs and {} MenuSequences in step {}'.format(len(filter_output),
            len(chainStep.sequences), chainStep.name)
        raise ValueError(msg)

    for nseq, sequence in enumerate(chainStep.sequences):
        output = filter_output[nseq]
        log.debug("Found input %s to sequence::%s from Filter::%s (from seed %s)", output,
                  sequence.name, filterNode.Alg.name(), sequence.seed)
        sequence.connectToFilter(output)


def generateDecisionTree(chains, allChainDicts):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from collections import defaultdict
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
    from AthenaCommon.CFElements import findOwningSequence

    acc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    acc.addSequence( seqAND(mainSequenceName) )

    log.debug('Generating decision tree with main sequence: {}'.format(mainSequenceName))

    chainStepsMatrix = defaultdict(lambda: defaultdict(list))

    ## Fill chain steps matrix
    for chain in chains:
        for stepNumber, chainStep in enumerate(chain.steps):
            chainName = chainStep.name.split('_')[0]
            chainStepsMatrix[stepNumber][chainName].append(chain)

    allSequences = []

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in chainStepsMatrix:
        stepDecisions = []

        stepName = 'Step{}'.format(nstep)

        stepFilterNodeName = '{}{}'.format(stepName, CFNaming.FILTER_POSTFIX)
        filterAcc = ComponentAccumulator()
        filterAcc.addSequence( parOR(stepFilterNodeName) )

        stepRecoNodeName = '{}_{}'.format(mainSequenceName, stepName)
        recoAcc = ComponentAccumulator()
        recoAcc.addSequence( parOR(stepRecoNodeName) )

        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                # If cell is empty, there is nothing to do
                continue

            firstChain = chainsInCell[0]
            if nstep == 0:
                filter_input = firstChain.group_seed
            else:
                filter_input = [output for sequence in firstChain.steps[nstep - 1].sequences for output in sequence.outputs]

            # One aggregated filter per chain (one per column in matrix)
            filterName = 'Filter_{}'.format( firstChain.steps[nstep].name )
            sfilter = buildFilter(filterName, filter_input)
            filterAcc.addEventAlgo(sfilter.Alg, sequenceName = stepFilterNodeName)

            chainStep = firstChain.steps[nstep]
            stepReco = parOR('{}{}'.format(chainStep.name, CFNaming.RECO_POSTFIX))
            stepView = seqAND('{}{}'.format(chainStep.name, CFNaming.VIEW_POSTFIX), [stepReco])
            viewWithFilter = seqAND(chainStep.name, [sfilter.Alg, stepView])
            recoAcc.addSequence(viewWithFilter, parentName = stepRecoNodeName)

            stepsAcc = ComponentAccumulator()

            for chain in chainsInCell:
                for seq in chain.steps[nstep].sequences:
                    if seq.ca is None:
                        raise ValueError('ComponentAccumulator missing in sequence {} in chain {}'.format(seq.name, chain.name))
                    stepsAcc.merge( seq.ca )
                    recoAcc.addEventAlgo( seq.hypo.Alg, sequenceName = stepView.getName() )
                sfilter.setChains(chain.name)

            recoAcc.merge(stepsAcc, sequenceName = stepReco.getName())

            connectStepToFilter(chainStep, sfilter)

            for sequence in chainStep.sequences:
                stepDecisions += sequence.outputs

        acc.merge(filterAcc, sequenceName = mainSequenceName)
        acc.merge(recoAcc, sequenceName = mainSequenceName)

        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)
        acc.addSequence(summary, parentName = mainSequenceName)

    return acc