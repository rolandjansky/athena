# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging' )

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from copy import deepcopy


def mergeChainDefs(listOfChainDefs, chainDict, strategy="parallel", offset=-1):

    log.debug("Combine by using %s merging", strategy)

    if strategy=="parallel":
        return mergeParallel(listOfChainDefs, chainDict['L1item'], offset)
    elif strategy=="serial":
        #return mergeSerial(listOfChainDefs,offset)
        log.error("Serial mergin not yet implemented.")
        return -1
    else:
        log.error("Merging failed for %s. Merging strategy '%s' not known.", (listOfChainDefs, strategy))
        return -1



def mergeParallel(chainDefList, chainL1Item, offset):

    if offset != -1:
        log.error("Offset for parallel merging not implemented.")
        
    allSteps = []
    nSteps = []
    chainName = ''
    #l1Seed = ''
    l1Seed = chainL1Item
    l1Thresholds = []


    for cConfig in chainDefList:
        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
            
        if l1Seed == '':
            l1Seed = cConfig.L1Item
        elif l1Seed != cConfig.L1Item:
            log.debug("Taking the overall L1 item of the chain (%s) and not the indiv set ones for the chain parts (%s)", l1Seed, cConfig.L1Item)

        allSteps.append(cConfig.steps)
        nSteps.append(len(cConfig.steps))
        l1Thresholds.extend(cConfig.vseeds)

    from itertools import izip_longest
    orderedSteps = list(izip_longest(*allSteps))
    myOrderedSteps = deepcopy(orderedSteps)

    combChainSteps =[]
    for steps in myOrderedSteps:
        mySteps = list(steps)
        combStep = makeChainSteps(mySteps)
        combChainSteps.append(combStep)

    # check if all chain parts have the same number of steps
    sameNSteps = all(x==nSteps[0] for x in nSteps) 
    if sameNSteps is True:
        log.debug("All chain parts have the same number of steps")
    else:
        log.debug("Have to deal with uneven number of chain steps, there might be none's appearing in sequence list => to be fixed")
                                  
    combinedChainDef = Chain(chainName, L1Item=l1Seed, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds)

    for step in combinedChainDef.steps:
        log.debug('  Step %s', step)

    return combinedChainDef




def makeChainSteps(steps):
    stepName = ''
    stepSeq = []
    stepMult = 0
    stepNumber = ''
    log.debug(" steps %s ", steps)
    stepName = "merged"
    for step in steps:
        if step is None:
            continue
        if len(step.sequences) > 1:
            log.error("More than one menu sequence found in combined chain!!")
        seq = step.sequences[0]
        log.debug(" step type  %s", type(step.sequences))
        log.debug(" step.name %s", step.name)
        log.debug(" step.seq %s", step.sequences)
        log.debug(" step.mult %s", step.multiplicity)
        currentStep = step.name
        stepNameParts = currentStep.split('_')
        if stepNumber == '':
            stepNumber = stepNameParts[0]

        # the step naming for combined chains needs to be revisted!!
        stepName += '_' +step.name #stepNumber + '_' + stepNameParts[1]
        stepSeq.append(seq)
        stepMult += step.multiplicity
        
    log.debug(" - BB stepName %s", stepName)
    log.debug(" - BB stepSeq %s", stepSeq)
    log.debug(" - BB stepMult %s", stepMult)
    
    theChainStep = ChainStep(stepName, stepSeq, stepMult)
    log.debug(" - BBB the chain step %s", theChainStep)
    
    return theChainStep


