# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from copy import deepcopy
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.ChainDictTools' )


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




def setupTopoStartFrom(topoThresholds, theChainDef):

    from TrigGenericAlgs.TrigGenericAlgsConf import MergeTopoStarts

    if len(topoThresholds) > 1:
        from TrigGenericAlgs.TrigGenericAlgsConfig import MergeTopoStartsConfig
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


