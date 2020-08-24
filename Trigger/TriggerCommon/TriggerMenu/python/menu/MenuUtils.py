# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import Counter
from copy import deepcopy
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.MenuUtils' )


def splitInterSignatureChainDict(chainDict):
    listOfSplitChainDicts = []
    for chainPart in chainDict['chainParts']:
        thisSignature = chainPart['signature']
        chainPartAdded = False        
        for splitChainDict in listOfSplitChainDicts:
            if thisSignature == splitChainDict['chainParts'][0]['signature']:            
                splitChainDict['chainParts'] += [chainPart]
                chainPartAdded = True
                break
        if not chainPartAdded:
            newSplitChainDict = deepcopy(chainDict)
            newSplitChainDict['chainParts'] = [chainPart]
            newSplitChainDict['signature'] = chainPart['signature']
            listOfSplitChainDicts += [newSplitChainDict]
            
    #special code to handle chains with "AND" in the name
    #jet,ht and bjet jet chains belong to the same signature
    #so an extra key is needed to make sure the part are treated separately
    for splitChainDict in listOfSplitChainDicts:
        if 'AND' in [part['extra'] for part in  splitChainDict['chainParts']]:
            log.info("Implement extra splitting, triggered by AND key (%s.)", chainDict['chainName'])
                
            listOfSplitChainDicts=[]        
            addNewSplit= False      
            for chainPart in chainDict['chainParts']:
                thisSignature = chainPart['signature']
                chainPartAdded = False        
                
                #chainPart coming after the AND, change temporaly the Signature name, 
                #so it does not get added to existing dictionary
                if addNewSplit:                    
                    thisSignature = 'extraSplit'            
                    addNewSplit = False        

                #chainPart containing at AND,
                # make sure next chainPart is added as a new elemented to the listOfSplitChainDicts
                if 'AND' in chainPart['extra']:
                    addNewSplit = True
                    #log.info("AND key found, next chainPart will be added as a new element of listOfSplitChainDicts")
                    
                for splitChainDict in listOfSplitChainDicts:
                    #if AND present in an elemenent of listOfSplitChainDicts, don't add anything to it
                    if 'AND' in [part['extra'] for part in splitChainDict['chainParts']]:
                        continue
                    if thisSignature == splitChainDict['chainParts'][0]['signature']:            
                        #log.info("Adding %s to existing element %s.", chainPart['chainPartName'], splitChainDict)
                        splitChainDict['chainParts'] += [chainPart]
                        chainPartAdded = True
                        break
                if not chainPartAdded:
                    newSplitChainDict = deepcopy(chainDict)
                    newSplitChainDict['chainParts'] = [chainPart]
                    newSplitChainDict['signature'] = chainPart['signature']
                    listOfSplitChainDicts += [newSplitChainDict]
                    #log.info("Adding %s as a new element of listOfSplitChainDicts", chainPart['chainPartName'])
                    
    #log.info("ListOfSplitChainDicts", chainPart['chainPartName'])
    
    #oder the splitted dicts
    orderedListOfSplitChainDicts = []
    if "mergingOrder" not in chainDict:
        log.debug("No merging order given for chain %s.", chainDict['chainName'])
    elif chainDict["mergingOrder"] == []:
        log.info("No merging order given for chain %s.", chainDict['chainName'])
    else:
        for chainPartName in chainDict["mergingOrder"]:
            for splitChainDict in listOfSplitChainDicts:                
                if splitChainDict['chainParts'][0]['chainPartName'] == chainPartName:
                    orderedListOfSplitChainDicts += [splitChainDict]

        if not len(orderedListOfSplitChainDicts) == len(listOfSplitChainDicts):
            for chainPartName in chainDict["mergingOrder"]:
                log.error("Ordering of split chain dicts failed. Please check that orderedListOfSplitChainDicts and listOfSplitChainDicts contain the same elements!!")                
            log.info(chainDict)
            
        return orderedListOfSplitChainDicts

    return listOfSplitChainDicts


def splitChainDict(chainDict):
    listOfChainDicts = []
    for chainPart in chainDict['chainParts']:
        newChainDict = deepcopy(chainDict)
        newChainDict['chainParts'] = chainPart
        listOfChainDicts += [newChainDict]
    return listOfChainDicts


def _addSequence(chainDef,sequenceToAdd):
    """
    Check if sequence is already in chainDef, if not add it
    """
    for chainDefSequence in chainDef.sequenceList:
        if chainDefSequence['output'] == sequenceToAdd['output']:
            return chainDef
    chainDef.sequenceList += [sequenceToAdd]
    return chainDef        


def _getSigList(chainDef,sigType="HLT"):
    """
    Little helper function to get the list of signatures associated to a trigger level (L2/EF/HLT)
    """
    sigList = []
    for signature in chainDef.signatureList:
        if signature['listOfTriggerElements'][0][0:len(sigType)] == sigType:
            sigList += [signature]
    return sigList


def _replicateMissingSignatures(listOfChainDefs,unevenSigs,level):
    """
    Loops over the listOfChainDefs, and fills up those ChainDefs that have a shorter signature list at a given level than the largest one with dummy signatures
    """

    for chainDef in listOfChainDefs:

        # if this is already the chainDef with the longest signatureList nothing has to be done
        if len(_getSigList(chainDef,level)) == max(unevenSigs):
            continue
        else:
            maxSignatureCounter = -1
            numberOfSignatures = 0      
            maxSignatureIndex = -1
            for signatureIndex,signature in enumerate(chainDef.signatureList):
                if signature['listOfTriggerElements'][0][0:len(level)] == level:
                    maxSignatureCounter = max(maxSignatureCounter,signature['signature_counter'])
                    numberOfSignatures += 1
                    maxSignatureIndex = signatureIndex
                    
            if maxSignatureIndex == -1:
                log.error('maxSignatureIndex has not been set. Maybe there is no sequence at this level? CHECK IN CODE - fix in work. ')
                
            listOfLastTEs = chainDef.signatureList[maxSignatureIndex]['listOfTriggerElements']   

            for i in range(maxSignatureCounter+1, maxSignatureCounter+1+max(unevenSigs)-numberOfSignatures):  
                if level in ["EF","HLT"]:
                    chainDef.signatureList += deepcopy([{'signature_counter': i, 'listOfTriggerElements': listOfLastTEs}])
                elif level == "L2":
                    chainDef.addSignatureL2(deepcopy(listOfLastTEs))
                log.info("Added %s signature with TEs %s to ChainDef of chain %s in preparation for parallel merging." % (level,str(listOfLastTEs),chainDef.chain_name))
                log.debug("ChainDef with replicated signatures at %s: %s", level, chainDef)
                



def mergeChainDefs(listOfChainDefs, strategy="parallel", offset=-1,
                   preserveL2EFOrder=True, removeDuplicateTEs=False, doTopo=True,
                   chainDicts='', noTEreplication=False, noMergeBJet=False):

    log.debug("Combine using %s  merging", strategy)

    if strategy=="parallel":
        return _mergeChainDefsParallel(listOfChainDefs,offset,removeDuplicateTEs,noMergeBJet=noMergeBJet)
    elif strategy=="serial":
        return _mergeChainDefsSerial(chainDicts,listOfChainDefs,offset,preserveL2EFOrder=preserveL2EFOrder,doTopo=doTopo,noTEreplication=noTEreplication)
    else:
        log.error("Merging failed for %s. Merging strategy '%s' not known.", (listOfChainDefs, strategy))
        return -1



def _mergeChainDefsSerial(ChainDicts,listOfChainDefs,offset,preserveL2EFOrder=True,doTopo=True,noTEreplication=False):

    """
    serial merging of chain def objects for combined chains
    """

    
    #noTEreplication=False
    #Add few exception to the replication of the TE
    jet_count=0
    met_count=0
    ht_count=0
    other_count=0
    try:
        if 'upc' in ChainDicts[0]['chainName'] :
            noTEreplication=True  

        if 'hi' in ChainDicts[0]['chainName'] and 'v2' in  ChainDicts[1]['chainName'] :
            noTEreplication=True  
        
        for chainDict in ChainDicts:
            if (chainDict["signature"] == "Jet"):
                jet_count+=1
            elif (chainDict["signature"] == "HT"):
                ht_count+=1
            elif (chainDict["signature"] == "MET"):
                for chainpart in chainDict["chainParts"]:
                    if 'xe' in chainpart['trigType'] and 'cell' not in chainpart['EFrecoAlg']:
                        met_count+=1
            else:
                other_count+=1

        #print "BETTA Total count: jet-", jet_count, " HT-", ht_count, " xe-", met_count, "other:-", other_count, " in: ",ChainDicts[0]['chainName']          
        
    except Exception:
        pass


    if jet_count > 0 and met_count > 0:
        noTEreplication=True        
    if jet_count > 1:
        noTEreplication=True        
    if met_count > 1:
        noTEreplication=True        
    if jet_count > 0 and ht_count > 0:
        noTEreplication=True        


    listOfChainDefs = deepcopy(listOfChainDefs) 

    # copy the chaindef into which we want to merge the other chaindefs
    mergedChainDef = deepcopy(listOfChainDefs[0])

    #remove the first chaindef from the list
    listOfChainDefs.pop(0)
    
    # Loop remaining chain defs to be merged
    #doTopo=False
    #preserveL2EFOrder=False

    
    for chainDef in listOfChainDefs:
        currentLastTEs = mergedChainDef.signatureList[-1]['listOfTriggerElements']
        
        for sequence in chainDef.sequenceList:
            mergedChainDef = _addSequence(mergedChainDef,sequence)

        for signatureIdx,signature in enumerate(chainDef.signatureList):
            # if a topo is appended after the chain merging, or in same special cases, the replication of the last TEs is not necessary
            if noTEreplication:
                log.info("Removing replication of the TE for chain: %s", chainDef.chain_name)
                signatureToAdd = signature['listOfTriggerElements'] 
            else:
                signatureToAdd = signature['listOfTriggerElements'] if doTopo else signature['listOfTriggerElements'] + currentLastTEs 
            #signatureToAdd = signature['listOfTriggerElements'] 
            if preserveL2EFOrder:
                if not offset == -1:
                    log.error("L2/EF preserving serial merging with offset not yet implemented.")
                else:
                    if signature['listOfTriggerElements'][0].startswith("L2"):
                        mergedChainDef.addSignatureL2(signatureToAdd)
                    elif signature['listOfTriggerElements'][0].startswith("EF") or signature['listOfTriggerElements'][0].startswith("HLT"):
                        mergedChainDef.appendSignature(signatureToAdd)

                    else:
                        log.error("Unknown TE naming :",str(signature['listOfTriggerElements'][0]))
            else:
                if offset ==-1:
                    mergedChainDef.appendSignature(signatureToAdd)
                else:
                    if offset+signatureIdx > len(mergedChainDef.signatureList):
                        mergedChainDef.signatureList[offset+signatureIdx]['listOfTriggerElements'] += signatureToAdd
                    else:
                        mergedChainDef.appendSignature(signatureToAdd)
                    
    return mergedChainDef
        


def _mergeChainDefsParallel(listOfChainDefs,offset=-1,removeDuplicateTEs=False, noMergeBJet=False):

    """
    merge chain def objects for combined chains
    the order of signatures corresponds to the signature counters in the constituent chainDefs
    """

    listOfChainDefs = deepcopy(listOfChainDefs) 

    for chainDef in listOfChainDefs:
        log.debug("List of ChainDefs to be merged %s ", chainDef)
        
    if offset!=-1:
        log.warning("Parallel merging with offset has been tested only for EF-only chains. If you're doing something different, check it works ok!!")
    
    # check length of signature list in each chain def
    # if not even, take measures as the sig of the short chain
    # needs to be propagated to the end

    nL2sigs = []
    nEFsigs = []
    nHLTsigs = []

    for cD in listOfChainDefs:
        
        nL2sigs += [len(_getSigList(cD,"L2"))]
        nEFsigs += [len(_getSigList(cD,"EF"))]
        nHLTsigs += [len(_getSigList(cD,"HLT"))]

        ##Hardocode that the offset is only applied to the second chain in the list
        if cD==listOfChainDefs[1] and offset!=-1:
            nL2sigs[-1] = nL2sigs[-1]+offset if nL2sigs[-1]!=0 else nL2sigs[-1]
            nEFsigs[-1] = nEFsigs[-1]+offset if nEFsigs[-1]!=0 else nEFsigs[-1]
            nHLTsigs[-1]= nHLTsigs[-1]+offset if nHLTsigs[-1]!=0 else nHLTsigs[-1]

    countsL2 = Counter(nL2sigs)
    unevenL2Sigs = [val for val, count in countsL2.items() if count <= 1]
    countsEF = Counter(nEFsigs)
    unevenEFSigs = [val for val, count in countsEF.items() if count <= 1]
    countsHLT = Counter(nHLTsigs)
    unevenHLTSigs = [val for val, count in countsHLT.items() if count <= 1]

    log.debug("nL2sigs %s", nL2sigs)
    log.debug("countsL2 %s", countsL2)
    log.debug("unevenL1Sigs %s", unevenL2Sigs)
    log.debug("nEFsigs %s", nEFsigs)
    log.debug("countsEF %s", countsEF)
    log.debug("unevenEFSigs %s", unevenEFSigs)
    log.debug("nHLTsigs %s", nHLTsigs)
    log.debug("countsHLT %s", countsHLT)
    log.debug("unevenHLTSigs %s", unevenHLTSigs)

    nL2sigs = []
    nEFsigs = []
    nHLTsigs = []

    if unevenL2Sigs:
        _replicateMissingSignatures(listOfChainDefs,unevenL2Sigs,"L2")
    if unevenEFSigs:
        _replicateMissingSignatures(listOfChainDefs,unevenEFSigs,"EF")
    if unevenHLTSigs:
        _replicateMissingSignatures(listOfChainDefs,unevenHLTSigs,"HLT")

    # Now check if the replication of missing signatures worked and all chains have same length signature list

    for cD in listOfChainDefs:
        nL2sigs += [len(_getSigList(cD,"L2"))]
        nEFsigs += [len(_getSigList(cD,"EF"))]
        nHLTsigs += [len(_getSigList(cD,"HLT"))]

        if cD==listOfChainDefs[1] and offset!=-1:
            #print "Second Chain ",cD.chain_name, _getSigList(cD,"EF")
            nL2sigs[-1] = nL2sigs[-1]+offset if nL2sigs[-1]!=0 else nL2sigs[-1]
            nEFsigs[-1] = nEFsigs[-1]+offset if nEFsigs[-1]!=0 else nEFsigs[-1]
            nHLTsigs[-1]= nHLTsigs[-1]+offset if nHLTsigs[-1]!=0 else nHLTsigs[-1]

    countsL2 = Counter(nL2sigs)
    unevenL2Sigs = [val for val, count in countsL2.items() if count <= 1]
    countsEF = Counter(nEFsigs)
    unevenEFSigs = [val for val, count in countsEF.items() if count <= 1]
    countsHLT = Counter(nHLTsigs)
    unevenHLTSigs = [val for val, count in countsHLT.items() if count <= 1]
    

    if unevenL2Sigs or unevenEFSigs or unevenHLTSigs:
        log.error("Replication of trigger elements failed. There is still an uneven number of signatures in the ChainDefs of this combined chain.")


    # Loop over all chain defs to be merged


    
    mergedChainDef = listOfChainDefs[0]
    listOfChainDefs.pop(0)

    log.debug("mergedChainDef prior to merging: %s", mergedChainDef)


    for chainDef in listOfChainDefs:
        for sequence in chainDef.sequenceList:
            mergedChainDef = _addSequence(mergedChainDef,sequence)

        for chainDefSignature in chainDef.signatureList:
            for mergedChainDefSignature in mergedChainDef.signatureList: 
                if int(chainDefSignature['signature_counter']) == int(mergedChainDefSignature['signature_counter']):
                    log.debug("mergedChainDef signatureList before a step: %s", mergedChainDef.signatureList)
                    mergedChainDefSignature['listOfTriggerElements'] += chainDefSignature['listOfTriggerElements']
                    log.debug("mergedChainDef signatureList after a step: %s", mergedChainDef.signatureList)



    if removeDuplicateTEs:
        for signature in mergedChainDef.signatureList:

            if noMergeBJet: 
                hasBJetTE = False
                for TE in set(signature['listOfTriggerElements']):
                    if not (TE.find("bjet") == -1): hasBJetTE = True

                if not hasBJetTE:
                    signature['listOfTriggerElements'] = list(set(signature['listOfTriggerElements']))
            else:
                signature['listOfTriggerElements'] = list(set(signature['listOfTriggerElements']))

    return mergedChainDef
        


def setupTopoStartFrom(topoThresholds, theChainDef):

    from TrigGenericAlgs.TrigGenericAlgsConf import MergeTopoStarts

    if len(topoThresholds) > 1:
        from TrigGenericAlgs.TrigGenericAlgsLegacyConfig import MergeTopoStartsConfig
        m = MergeTopoStartsConfig("testInstance")
        log.debug(m)


    te0 = None
    te1 = None 
    outTE = None
    
    for i in range(len(topoThresholds)):
        if i == 0:
            te0 = topoThresholds[i]
            continue
        te1 = topoThresholds[i]
        combTes = te0+"_"+te1
        outTE = "L2_merged_"+combTes
        theMergeTopoStarts = MergeTopoStarts("MergeTopoStarts_"+combTes)
        theChainDef.addSequence( theMergeTopoStarts,[te0,te1], outTE)   
        theChainDef.addSignatureL2([outTE])   
        te0=outTE

    return te0


    # if ntopoTes ==1:
    #     topoStartFrom = str(chainDict['topoThreshold'][0])
    # elif ntoppoTes == 2:


    #     te1= str(topoThresholds[0])
    #     te2=str(topoThresholds[1])
    #     from TrigGenericAlgs import MergeTopoStarts
    #     theMergeTopoStarts = MergeTopoStarts("MergeTopoStarts_"+te1+"_"+te2)
    #     theChainDef.addSequence([te1,te2], theMergeTopoStarts, "merged_"+te1+"_"+te2)
    #     topoStartFrom = "merged_"+te1+"_"+te2
    # else:
    #     log.error('3 topo Tes to merge not implemented (can be done though!')                    



