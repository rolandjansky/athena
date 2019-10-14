# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging' )

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from copy import deepcopy


def mergeChainDefs(listOfChainDefs, chainDict, strategy="parallel", offset=-1):

    log.debug("Combine by using %s merging", strategy)

    if strategy=="parallel":
        return mergeParallel(listOfChainDefs,  offset)
    elif strategy=="serial":
        #return mergeSerial(listOfChainDefs,offset)
        log.error("Serial mergin not yet implemented.")
        return -1
    else:
        log.error("Merging failed for %s. Merging strategy '%s' not known.", (listOfChainDefs, strategy))
        return -1



def mergeParallel(chainDefList, offset):

    if offset != -1:
        log.error("Offset for parallel merging not implemented.")
        
    allSteps = []
    nSteps = []
    chainName = ''
    l1Thresholds = []


    for cConfig in chainDefList:
        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
            
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
                                  
    combinedChainDef = Chain(chainName, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds)

    for step in combinedChainDef.steps:
        log.debug('  Step %s', step)

    return combinedChainDef




def makeChainSteps(steps):
    stepName = ''
    stepSeq = []
    stepMult = []
    stepNumber = ''
    log.verbose(" steps %s ", steps)
    stepName = "merged"
    for step in steps:
        if step is None:
            continue
        log.debug("  step %s, multiplicity  = %s", step.name, str(step.multiplicity))
        log.debug("      with sequences = %s", ' '.join(map(str, [seq.name for seq in step.sequences])))

         # this function only works if the input chains are single-object chains (one menu seuqnce)
        if len(step.sequences) > 1:
            log.error("More than one menu sequence found in combined chain!!")
        seq = step.sequences[0]

        currentStep = step.name
        stepNameParts = currentStep.split('_')
        if stepNumber == '':
            stepNumber = stepNameParts[0]

        # the step naming for combined chains needs to be revisted!!
        stepName += '_' +step.name 
        stepSeq.append(seq)
        # set the multiplicity of all the legs 
        stepMult.append(sum(step.multiplicity))
        

    theChainStep = ChainStep(stepName, stepSeq, stepMult) 
    log.debug("Merged step: \n %s", theChainStep)
  
    
    return theChainStep


