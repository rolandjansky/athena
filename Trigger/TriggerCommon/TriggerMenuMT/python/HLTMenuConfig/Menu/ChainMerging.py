# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, EmptyMenuSequence, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuAlignmentTools import getAlignmentGroupOrdering
from collections import OrderedDict
from copy import deepcopy
import re

def mergeChainDefs(listOfChainDefs, chainDict):

    #chainDefList is a list of Chain() objects
    #one for each part in the chain

    strategy = chainDict["mergingStrategy"]
    offset = chainDict["mergingOffset"]
    log.info("%s: Combine by using %s merging", chainDict['chainName'], strategy)

    if strategy=="parallel":
        return mergeParallel(listOfChainDefs,  offset)
    elif strategy=="serial":
        return mergeSerial(listOfChainDefs)

    elif strategy=="auto":
        ordering = getAlignmentGroupOrdering()
        merging_dict = OrderedDict()
        for ich,cConfig in enumerate(listOfChainDefs):
            chain_ag = cConfig.alignmentGroups[0]
            if chain_ag not in ordering:
                log.error("Alignment group %s can't be auto-merged because it's not in the grouping list!",chain_ag)
            if chain_ag in merging_dict:
                merging_dict[chain_ag] += [ich]
            else:
                merging_dict[chain_ag] = [ich]
                
        tmp_merged = []
        for ag in ordering:
            if ag not in merging_dict:
                continue
            if len(merging_dict[ag]) > 1:
                tmp_merged += [mergeParallel(list( listOfChainDefs[i] for i in merging_dict[ag] ),offset)]
            else:
                tmp_merged += [listOfChainDefs[merging_dict[ag][0]]]

        # only serial merge if necessary
        if len(tmp_merged) == 1:
            return tmp_merged[0]

        return mergeSerial(tmp_merged)
            
        
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
    alignmentGroups = []

    for cConfig in chainDefList:
        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
        
        allSteps.append(cConfig.steps)
        nSteps.append(len(cConfig.steps))
        l1Thresholds.extend(cConfig.vseeds)
        if len(cConfig.alignmentGroups) > 1:
            log.error("Merging an already merged chain? This is odd! %s",cConfig.alignmentGroups)
        elif len(cConfig.alignmentGroups) == 1:
            alignmentGroups.append(cConfig.alignmentGroups[0])
        else: 
            log.info("Alignment groups are empty for this combined chain - if this is not _newJO, this is not ok!")
    import itertools
    if 'zip_longest' in dir(itertools):
        from itertools import zip_longest
    else:
        from itertools import izip_longest as zip_longest
    # Use zip_longest so that we get None in case one chain has more steps than the other
    orderedSteps = list(zip_longest(*allSteps))

    combChainSteps =[]
    log.debug("len(orderedSteps): %d", len(orderedSteps))
    for step_index, steps in enumerate(orderedSteps):
        mySteps = list(steps)
        log.debug("Merging step counter %d", step_index+1)
        combStep = makeCombinedStep(mySteps, step_index+1, chainDefList)
        combChainSteps.append(combStep)
                                  
    combinedChainDef = Chain(chainName, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds, 
                                nSteps = nSteps, alignmentGroups = alignmentGroups)

    log.debug("Parallel merged chain %s with these steps:", chainName)
    for step in combinedChainDef.steps:
        log.debug('\n   %s', step)

    return combinedChainDef

def getEmptySeqName(stepName, chain_index, step_number):
    #remove redundant instances of StepN
    if re.search('^Step[0-9]_',stepName):
        stepName = stepName[6:]

    seqName = 'EmptySeq'+str(step_number)+ '_'+ stepName + '_leg' + str(chain_index)
    return seqName


def getEmptyMenuSequence(flags, name):
    return EmptyMenuSequence(name)


def serial_zip(allSteps, chainName):
    n_chains = len(allSteps)
    newsteps = []
    for chain_index, chainsteps in enumerate(allSteps):
        for step_index, step in enumerate(chainsteps):
            log.debug('chain_index: ' + str(chain_index) + " step_index: " + str(step_index))
            # create list of correct length
            stepList = [None]*n_chains
            
            # put the step from the current sub-chain into the right place
            stepList[chain_index] = step
            log.debug('Put step: ' + str(step.name))

            # all other steps should contain an empty sequence
            for step_index2, emptyStep in enumerate(stepList):
                if emptyStep is None:
                    seqName = getEmptySeqName(step.name, chain_index, step_index+1)
                    emptySeq =  RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqName)
                    stepList[step_index2] = ChainStep( seqName, Sequences=[emptySeq], chainDicts=step.chainDicts)            
            
            newsteps.append(stepList)
    log.debug('After serial_zip')
    for s in newsteps:
        log.debug( ', '.join(map(str, [step.name for step in s]) ) )
    return newsteps

def mergeSerial(chainDefList):
    allSteps = []
    nSteps = []
    chainName = ''
    l1Thresholds = []
    alignmentGroups = []
    log.debug('Merge chainDefList:')
    log.debug(chainDefList)

    for cConfig in chainDefList:
        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
            
        allSteps.append(cConfig.steps)
        nSteps.append(len(cConfig.steps))
        l1Thresholds.extend(cConfig.vseeds)
        if len(cConfig.alignmentGroups) > 1:
            log.error("Merging an already merged chain? This is odd! %s",cConfig.alignmentGroups)
        alignmentGroups.append(cConfig.alignmentGroups[0])

    serialSteps = serial_zip(allSteps, chainName)
    mySerialSteps = deepcopy(serialSteps)
    combChainSteps =[]
    for step_index, steps in enumerate(mySerialSteps):
        mySteps = list(steps)
        combStep = makeCombinedStep(mySteps, step_index+1, chainDefList)
        combChainSteps.append(combStep)

    # check if all chain parts have the same number of steps
    sameNSteps = all(x==nSteps[0] for x in nSteps) 
    if sameNSteps is True:
        log.info("All chain parts have the same number of steps")
    else:
        log.info("Have to deal with uneven number of chain steps, there might be none's appearing in sequence list => to be fixed")
                                  
    combinedChainDef = Chain(chainName, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds,
                               nSteps = nSteps, alignmentGroups = alignmentGroups)

    log.debug("Serial merged chain %s with these steps:", chainName)
    for step in combinedChainDef.steps:
        log.debug('   %s', step)

    return combinedChainDef


def makeCombinedStep(steps, stepNumber, chainDefList):
    from TrigCompositeUtils.TrigCompositeUtils import legName
    stepName = 'merged' #we will renumber all steps after chains are aligned #Step' + str(stepNumber)
    stepSeq = []
    stepMult = []
    log.verbose(" steps %s ", steps)
    stepDicts = []
    comboHypoTools = []
    comboHypo = None

    # this function only makes sense if we are merging steps corresponding to the chains in the chainDefList
    assert len(chainDefList)==len(steps), "makeCombinedStep: Length of chain defs %d does not match length of steps to merge %d" % (len(chainDefList), len(steps))
    
    for chain_index, step in enumerate(steps):
        if step is None:
            # this happens for merging chains with different numbers of steps, we need to "pad" out with empty sequences to propogate the decisions
            currentStepName = "Step" + str(stepNumber) + "_Empty" + str(chain_index)
            seqName = getEmptySeqName(currentStepName, chain_index, stepNumber)
            log.debug("  step %s,  empty sequence %s", currentStepName, seqName)
            emptySeq = RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqName)

            stepSeq.append(emptySeq)
            stepMult.append(1)
            # we need a chain dict here, use the one corresponding to this leg of the chain
            stepDicts.append(deepcopy(chainDefList[chain_index].steps[-1].chainDicts[-1]))
        else:
            # Standard step, append it to the combined step
            log.debug("  step %s, multiplicity  = %s", step.name, str(step.multiplicity))
            if len(step.sequences):
                log.debug("      with sequences = %s", ' '.join(map(str, [seq.name for seq in step.sequences])))

            # this function only works if the input chains are single-object chains (one menu seuqnce)
            if len(step.sequences) > 1:
                log.error("More than one menu sequence found in combined chain!!")
            comboHypo = step.comboHypoCfg
            currentStepName = step.name
            #remove redundant instances of StepN_
            if re.search('^Step[0-9]_',currentStepName):
                currentStepName = currentStepName[6:]

            if len(step.sequences):
                seq = step.sequences[0]
                stepSeq.append(seq)
            # set the multiplicity of all the legs 
            stepMult.append(sum(step.multiplicity))
            comboHypoTools.extend(step.comboToolConfs)
            # update the chain dict list for the combined step with the chain dict from this step
            stepDicts += deepcopy(step.chainDicts)


        # the step naming for combined chains needs to be revisted!!
        stepName += '_' + currentStepName
        # for merged steps, we need to update the name to add the leg name
        stepDicts[-1]['chainName'] = legName(stepDicts[-1]['chainName'], chain_index)
        
    comboHypoTools = list(set(comboHypoTools))
    theChainStep = ChainStep(stepName, Sequences=stepSeq, multiplicity=stepMult, chainDicts=stepDicts, comboHypoCfg=comboHypo, comboToolConfs=comboHypoTools) 
    log.info("Merged step: \n %s", theChainStep)
  
    
    return theChainStep


