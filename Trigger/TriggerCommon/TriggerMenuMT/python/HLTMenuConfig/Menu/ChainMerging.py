# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, EmptyMenuSequence, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuAlignmentTools import get_alignment_group_ordering as getAlignmentGroupOrdering
from DecisionHandling.DecisionHandlingConfig import ComboHypoCfg
from collections import OrderedDict
from copy import deepcopy
import re

def mergeChainDefs(listOfChainDefs, chainDict):

    #chainDefList is a list of Chain() objects
    #one for each part in the chain

    strategy = chainDict["mergingStrategy"]
    offset = chainDict["mergingOffset"]
    log.info("[mergeChainDefs] %s: Combine by using %s merging", chainDict['chainName'], strategy)

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
                log.error("[mergeChainDefs] Alignment group %s can't be auto-merged because it's not in the grouping list!",chain_ag)
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
        log.error("[mergeChainDefs] Merging failed for %s. Merging strategy '%s' not known.", (listOfChainDefs, strategy))
        return -1



def mergeParallel(chainDefList, offset):

    if offset != -1:
        log.error("[mergeParallel] Offset for parallel merging not implemented.")
        raise Exception("[mergeParallel] Cannot merge this chain, exiting.")

    allSteps = []
    nSteps = []
    chainName = ''
    l1Thresholds = []
    alignmentGroups = []
   
    for cConfig in chainDefList:
        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("[mergeParallel] Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
            raise Exception("[mergeParallel] Cannot merge this chain, exiting.")
        allSteps.append(cConfig.steps)
        nSteps.append(len(cConfig.steps))
        l1Thresholds.extend(cConfig.vseeds)
        if len(cConfig.alignmentGroups) > 1:
            log.error("[mergeParallel] Parallel merging an already merged chain? This is odd! %s",cConfig.alignmentGroups)
            raise Exception("[mergeParallel] Complicated situation currently unimplemented. exiting.")
        elif len(cConfig.alignmentGroups) == 1:
            alignmentGroups.append(cConfig.alignmentGroups[0])
        else: 
            log.info("[mergeParallel] Alignment groups are empty for this combined chain - if this is not _newJO, this is not ok!")
    import itertools
    if 'zip_longest' in dir(itertools):
        from itertools import zip_longest
    else:
        from itertools import izip_longest as zip_longest
    # Use zip_longest so that we get None in case one chain has more steps than the other
    orderedSteps = list(zip_longest(*allSteps))

    combChainSteps =[]
    log.debug("[mergeParallel] len(orderedSteps): %d", len(orderedSteps))
    for chain_index in range(len(chainDefList)):
        log.debug('[mergeParallel] Chain object to merge (i.e. chainDef) %s', chainDefList[chain_index])

    for step_index, steps in enumerate(orderedSteps):
        mySteps = list(steps)
        log.debug("[mergeParallel] Merging step counter %d", step_index+1)
        combStep = makeCombinedStep(mySteps, step_index+1, chainDefList, orderedSteps, combChainSteps)
        combChainSteps.append(combStep)
                                  
    combinedChainDef = Chain(chainName, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds, 
                                nSteps = nSteps, alignmentGroups = alignmentGroups)

    log.debug("[mergeParallel] Parallel merged chain %s with these steps:", chainName)
    for step in combinedChainDef.steps:
        log.debug('\n   %s', step)

    return combinedChainDef

def getEmptySeqName(stepName, chain_index, step_number, alignGroup):
    #remove redundant instances of StepN
    if re.search('^Step[0-9]_',stepName):
        stepName = stepName[6:]

    seqName = 'Empty'+ alignGroup +'Seq'+str(step_number)+ '_'+ stepName
    return seqName


def getEmptyMenuSequence(flags, name, mergeUsingFeature = False):
    return EmptyMenuSequence(name, mergeUsingFeature = mergeUsingFeature)

def getMultiplicityPerLeg(multiplicities):
    mult_per_leg = []
    for mult in multiplicities:
        if mult == 1: 
            mult_per_leg += ['1']
        elif mult > 1: 
            mult_per_leg += ['N']
        else: 
            raise Exception("[serial_zip] multiplicity not an expected value: %s",mult) 
    return mult_per_leg

def isFullScanRoI(inputL1Nav):
    fsRoIList = ['HLTNav_L1FSNOSEED','HLTNav_L1MET','HLTNav_L1J']
    if inputL1Nav in fsRoIList:
        return True
    else:
        return False

def noPrecedingStepsPreMerge(newsteps,chain_index,ileg):
    for step in newsteps:
        seq = step[chain_index].sequences[ileg]
        if type(seq).__name__ == 'EmptyMenuSequence':
            continue
        else:
            #if there's a non-empty sequence in a step before, there is clearly a
            #preceding step in this chain.
            return False
    return True

def noPrecedingStepsPostMerge(newsteps, ileg):
    for step in newsteps:
        seq = step.sequences[ileg]
        if type(seq).__name__ == 'EmptyMenuSequence':
            continue
        else:
            #if there's a non-empty sequence in a step before, there is clearly a
            #preceding step in this chain.
            return False
    return True
        

def serial_zip(allSteps, chainName, chainDefList):

    legs_per_part = [len(cd.steps[0].multiplicity) for cd in chainDefList]
    n_parts = len(allSteps)
    log.debug('[serial_zip] configuring chain with %d parts with multiplicities %s', n_parts, legs_per_part)
    newsteps = []

    doBonusDebug = False

    for chain_index, chainSteps in enumerate(allSteps): #per-part (horizontal) iteration
        for step_index, step in enumerate(chainSteps):  #serial step iteration
            log.debug('[serial_zip] chain_index: %s step_index: %s', chain_index, step_index)
            # create list of correct length (chainSteps in parallel)
            stepList = [None]*n_parts

            # put the step from the current sub-chain into the right place
            stepList[chain_index] = step
            log.debug('[serial_zip] Put step: %s', step.name)

            # all other chain parts' steps should contain an empty sequence
            for chain_index2, (emptyStep, nLegs) in enumerate(zip(stepList,legs_per_part)): #more per-leg iteration
                if emptyStep is None:
                    mult_per_leg = getMultiplicityPerLeg(chainDefList[chain_index2].steps[0].multiplicity)

                    #this WILL NOT work for jets!
                    step_mult = []
                    emptyChainDicts = []
                    if chain_index2 < chain_index:
                        emptyChainDicts = allSteps[chain_index2][-1].stepDicts
                    else:
                        emptyChainDicts = allSteps[chain_index2][0].stepDicts

                    sigNames = []
                    for emptyChainDict in emptyChainDicts:
                        if isFullScanRoI(chainDefList[chain_index2].L1decisions[0]):
                            sigNames +=[emptyChainDict['chainParts'][0]['signature']+'FS']
                        else:
                            sigNames +=[emptyChainDict['chainParts'][0]['signature']]

                    seqMultName = '_'.join([mult+sigName for mult, sigName in zip(mult_per_leg,sigNames)])
                    seqStepName = 'Empty' + chainDefList[chain_index].alignmentGroups[0]+'Align'+str(step_index+1)+'_'+seqMultName

                    seqNames = [getEmptySeqName(emptyChainDicts[iSeq]['signature'], chain_index, step_index+1, chainDefList[chain_index].alignmentGroups[0]) for iSeq in range(nLegs)]
                    if doBonusDebug:                        
                        log.debug("[serial_zip] step name for this leg: %s", seqStepName)
                        log.debug("[serial_zip] created empty sequence(s): %s", seqNames)
                        log.debug("[serial_zip] L1decisions %s ", chainDefList[chain_index2].L1decisions)

                    emptySequences = []
                    for ileg in range(nLegs):
                        if isFullScanRoI(chainDefList[chain_index2].L1decisions[0]) and noPrecedingStepsPreMerge(newsteps,chain_index2, ileg):
                            log.debug("[serial_zip] adding FS empty sequence with mergeUsingFeature = False ")
                            emptySequences += [RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqNames[ileg]+"FS", mergeUsingFeature = False)]
                        elif isFullScanRoI(chainDefList[chain_index2].L1decisions[0]):
                            log.debug("[serial_zip] adding FS empty sequence with mergeUsingFeature = True ")
                            emptySequences += [RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqNames[ileg]+"FS", mergeUsingFeature = True)]
                        else:
                            log.debug("[serial_zip] adding non-FS empty sequence")
                            emptySequences += [RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqNames[ileg])]

                    #this WILL NOT work for jets!
                    step_mult = []
                    emptyChainDicts = []
                    if chain_index2 < chain_index:
                        emptyChainDicts = allSteps[chain_index2][-1].stepDicts
                    else:
                        emptyChainDicts = allSteps[chain_index2][0].stepDicts

                    if doBonusDebug:
                        log.debug("[serial_zip] emptyChainDicts %s",emptyChainDicts)

                    if len(emptySequences) != len(emptyChainDicts):
                        log.error("[serial_zip] different number of empty sequences/legs %d to stepDicts %d", len(emptySequences), len(emptyChainDicts))
                        raise Exception("[serial_zip] Cannot create this chain step, exiting.")

                    for sd in emptyChainDicts:
                        if len(sd['chainParts']) != 1:
                            log.error("[serial_zip] stepDict chainParts has length != 1 within a leg! %s",sd)
                            raise Exception("[serial_zip] Cannot create this chain step, exiting.")
                        step_mult += [int(sd['chainParts'][0]['multiplicity'])] 

                    if len(emptySequences) != len(step_mult):
                        log.error("[serial_zip] different number of empty sequences/legs %d to multiplicities %d", len(emptySequences), len(step_mult))
                        raise Exception("[serial_zip] Cannot create this chain step, exiting.")

                    if doBonusDebug:
                        log.debug('[serial_zip] step multiplicity %s',step_mult)

                    stepList[chain_index2] = ChainStep( seqStepName, Sequences=emptySequences,
                                                  multiplicity = step_mult, chainDicts=emptyChainDicts,
                                                  isEmpty = True)

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
    log.debug('[mergeSerial] Merge chainDefList:')
    log.debug(chainDefList)

    for cConfig in chainDefList:

        if chainName == '':
            chainName = cConfig.name
        elif chainName != cConfig.name:
            log.error("[mergeSerial] Something is wrong with the combined chain name: cConfig.name = %s while chainName = %s", cConfig.name, chainName)
            raise Exception("[mergeSerial] Cannot merge this chain, exiting.")

        allSteps.append(cConfig.steps)
        nSteps.append(len(cConfig.steps))
        l1Thresholds.extend(cConfig.vseeds)
        alignmentGroups.extend(cConfig.alignmentGroups)

    serialSteps = serial_zip(allSteps, chainName, chainDefList)
    combChainSteps =[]
    for chain_index in range(len(chainDefList)):
        log.debug('[mergeSerial] Chain object to merge (i.e. chainDef) %s', chainDefList[chain_index])
    for step_index, steps in enumerate(serialSteps):
        mySteps = list(steps)
        combStep = makeCombinedStep(mySteps, step_index+1, chainDefList)
        combChainSteps.append(combStep)

    # check if all chain parts have the same number of steps
    sameNSteps = all(x==nSteps[0] for x in nSteps) 
    if sameNSteps is True:
        log.info("[mergeSerial] All chain parts have the same number of steps")
    else:
        log.info("[mergeSerial] Have to deal with uneven number of chain steps, there might be none's appearing in sequence list => to be fixed")
                                  
    combinedChainDef = Chain(chainName, ChainSteps=combChainSteps, L1Thresholds=l1Thresholds,
                               nSteps = nSteps, alignmentGroups = alignmentGroups)

    log.debug("[mergeSerial] Serial merged chain %s with these steps:", chainName)
    for step in combinedChainDef.steps:
        log.debug('   %s', step)

    return combinedChainDef

def checkStepContent(parallel_steps):
  
    for step in parallel_steps:
        if step is None:
            continue
        for seq in step.sequences:
            if type(seq).__name__ != 'EmptyMenuSequence':
                return True    
    return False   

def makeCombinedStep(parallel_steps, stepNumber, chainDefList, allSteps = [], currentChainSteps = []):
    from TrigCompositeUtils.TrigCompositeUtils import legName
    stepName = 'merged' #we will renumber all steps after chains are aligned #Step' + str(stepNumber)
    stepSeq = []
    stepMult = []
    log.verbose("[makeCombinedStep] steps %s ", parallel_steps)
    stepDicts = []
    comboHypoTools = []
    comboHypo = None

    # this function only makes sense if we are merging steps corresponding to the chains in the chainDefList
    assert len(chainDefList)==len(parallel_steps), "[makeCombinedStep] makeCombinedStep: Length of chain defs %d does not match length of steps to merge %d" % (len(chainDefList), len(allSteps))
    
    leg_counter = 0
    currentStepName = ''
    # if *all* the steps we're trying to merge are either empty sequences or empty steps
    # we need to create a single empty step instead. 
    hasNonEmptyStep = checkStepContent(parallel_steps)
  
    if not hasNonEmptyStep:
        for chain_index, step in enumerate(parallel_steps): 
            # every step is empty but some might have empty sequences and some might not
            if len(step.sequences) == 0:
                new_stepDict = deepcopy(chainDefList[chain_index].steps[-1].stepDicts[-1])
                currentStepName = 'Empty' + chainDefList[chain_index].alignmentGroups[0]+'Align'+str(stepNumber)+'_'+new_stepDict['chainParts'][0]['multiplicity']+new_stepDict['signature']

                # we need a chain dict here, use the one corresponding to this leg of the chain
                oldLegName = new_stepDict['chainName']
                if re.search('^leg[0-9]{3}_',oldLegName):
                    oldLegName = oldLegName[7:]
                new_stepDict['chainName'] = legName(oldLegName,leg_counter)
                stepDicts.append(new_stepDict)
                leg_counter += 1
            else:
                # Standard step with empty sequence(s)
                currentStepName = step.name
                #remove redundant instances of StepN_ and merged_ (happens when merging already merged chains)
                if re.search('^Step[0-9]_',currentStepName):
                    currentStepName = currentStepName[6:]
                if re.search('^merged_',currentStepName):
                    currentStepName = currentStepName[7:]

                # update the chain dict list for the combined step with the chain dict from this step
                log.debug('[makeCombinedStep] adding step dictionaries %s',step.stepDicts)

                for new_stepDict in deepcopy(step.stepDicts):
                    oldLegName = new_stepDict['chainName']
                    if re.search('^leg[0-9]{3}_',oldLegName):
                        oldLegName = oldLegName[7:]
                    new_stepDict['chainName'] = legName(oldLegName,leg_counter)
                    log.debug("[makeCombinedStep] stepDict naming old: %s, new: %s", oldLegName, new_stepDict['chainName'])
                    stepDicts.append(new_stepDict)
                    leg_counter += 1

            stepName += '_' + currentStepName

        theChainStep = ChainStep(stepName, Sequences=[], multiplicity=[], chainDicts=stepDicts, comboHypoCfg=ComboHypoCfg) 
        log.info("[makeCombinedStep] Merged empty step: \n %s", theChainStep)
        return theChainStep

    for chain_index, step in enumerate(parallel_steps): #this is a horizontal merge!

        if step is None or (hasNonEmptyStep and len(step.sequences) == 0):
            # this happens for merging chains with different numbers of steps, we need to "pad" out with empty sequences to propogate the decisions
            # all other chain parts' steps should contain an empty sequence

            new_stepDict = deepcopy(chainDefList[chain_index].steps[-1].stepDicts[-1])
            seqName = getEmptySeqName(new_stepDict['signature'], chain_index, stepNumber, chainDefList[0].alignmentGroups[0])

            if isFullScanRoI(chainDefList[chain_index].L1decisions[0]):
                if noPrecedingStepsPostMerge(currentChainSteps, chain_index):
                    stepSeq.append(RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqName+"FS", mergeUsingFeature = False))
                else:
                    stepSeq.append(RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqName+'FS', mergeUsingFeature = True))
                currentStepName = 'Empty' + chainDefList[chain_index].alignmentGroups[0]+'Align'+str(stepNumber)+'_'+new_stepDict['chainParts'][0]['multiplicity']+new_stepDict['signature']+'FS'
            else:
                stepSeq.append(RecoFragmentsPool.retrieve(getEmptyMenuSequence, flags=None, name=seqName))
                currentStepName = 'Empty' + chainDefList[chain_index].alignmentGroups[0]+'Align'+str(stepNumber)+'_'+new_stepDict['chainParts'][0]['multiplicity']+new_stepDict['signature']

            log.debug("[makeCombinedStep]  step %s,  empty sequence %s", currentStepName, seqName)

            #stepNumber is indexed from 1, need the previous step indexed from 0, so do - 2
            prev_step_mult = int(currentChainSteps[stepNumber-2].multiplicity[chain_index])
            stepMult.append(prev_step_mult)
            # we need a chain dict here, use the one corresponding to this leg of the chain
            oldLegName = new_stepDict['chainName']
            if re.search('^leg[0-9]{3}_',oldLegName):
                oldLegName = oldLegName[7:]
            new_stepDict['chainName'] = legName(oldLegName,leg_counter)
            stepDicts.append(new_stepDict)
            leg_counter += 1
        else:
            # Standard step, append it to the combined step
            log.debug("[makeCombinedStep]  step %s, multiplicity  = %s", step.name, str(step.multiplicity))
            if len(step.sequences):
                log.debug("[makeCombinedStep]    with sequences = %s", ' '.join(map(str, [seq.name for seq in step.sequences])))

            # this function only works if the input chains are single-object chains (one menu seuqnce)
            if len(step.sequences) > 1:
                log.debug("[makeCombinedStep] combining in an already combined chain")

            comboHypo = step.comboHypoCfg
            currentStepName = step.name
            #remove redundant instances of StepN_ and merged_ (happens when merging already merged chains)
            if re.search('^Step[0-9]_',currentStepName):
                currentStepName = currentStepName[6:]
            if re.search('^merged_',currentStepName):
                currentStepName = currentStepName[7:]
            stepSeq.extend(step.sequences)
            # set the multiplicity of all the legs 
            if len(step.multiplicity) == 0:
                stepMult.append(0)
            else:
                stepMult.extend(step.multiplicity)
            comboHypoTools.extend(step.comboToolConfs)
            # update the chain dict list for the combined step with the chain dict from this step
            log.debug('[makeCombinedStep] adding step dictionaries %s',step.stepDicts)
            
            for new_stepDict in deepcopy(step.stepDicts):
                oldLegName = new_stepDict['chainName']
                if re.search('^leg[0-9]{3}_',oldLegName):
                    oldLegName = oldLegName[7:]
                new_stepDict['chainName'] = legName(oldLegName,leg_counter)
                log.debug("[makeCombinedStep] stepDict naming old: %s, new: %s", oldLegName, new_stepDict['chainName'])
                stepDicts.append(new_stepDict)
                leg_counter += 1


        # the step naming for combined chains needs to be revisted!!
        stepName += '_' + currentStepName
        log.debug('[makeCombinedStep] current step name %s',stepName)
        # for merged steps, we need to update the name to add the leg name
    
    comboHypoTools = list(set(comboHypoTools))
    theChainStep = ChainStep(stepName, Sequences=stepSeq, multiplicity=stepMult, chainDicts=stepDicts, comboHypoCfg=comboHypo, comboToolConfs=comboHypoTools) 
    log.info("[makeCombinedStep] Merged step: \n %s", theChainStep)
  
    
    return theChainStep

