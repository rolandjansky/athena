# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from copy import deepcopy

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


def splitInterSignatureChainDict(chainDict):
    listOfSplitChainDicts = []

    move_jets = 'Bjet' in chainDict['signatures']

    #Bjet and jet always go together, and the overall dict needs to have signature 'Bjet'

    for chainPart in chainDict['chainParts']:
        thisSignature = chainPart['signature']
        chainPartAdded = False

        for splitChainDict in listOfSplitChainDicts:
            splitSignature = splitChainDict['chainParts'][0]['signature']
            if thisSignature == splitSignature or \
               (move_jets and thisSignature == 'Jet' and splitSignature == 'Bjet') or \
               (move_jets and thisSignature == 'Bjet' and splitSignature == 'Jet'):
                splitChainDict['chainParts'] += [chainPart]
                chainPartAdded = True
                break
        if not chainPartAdded:
            newSplitChainDict = deepcopy(chainDict)
            newSplitChainDict['chainParts'] = [chainPart]
            newSplitChainDict['signature'] = chainPart['signature']
            listOfSplitChainDicts += [newSplitChainDict]
            
    if move_jets:
        for newDict in listOfSplitChainDicts:
            if newDict['signature'] == 'Jet':
                newDict['signature'] = 'Bjet'

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
    
    #order the splitted dicts
    orderedListOfSplitChainDicts = []
    if "mergingOrder" not in chainDict:
        log.debug("No merging order given for chain %s.", chainDict['chainName'])
    elif chainDict["mergingOrder"] == []:
        log.debug("No merging order given for chain %s.", chainDict['chainName'])
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
        newChainDict['chainParts'] = [chainPart]
        listOfChainDicts += [newChainDict]
    return listOfChainDicts

def splitChainDictInLegs(chainDict):
      from TrigCompositeUtils.TrigCompositeUtils import legName
      if len(chainDict['chainParts']) ==1:
            return [chainDict]

      chainName= chainDict['chainName']
      listOfChainDicts = []     
      for count, chainDictPart in enumerate(chainDict['chainParts']):
            onePartChainDict = deepcopy( chainDict )
            onePartChainDict['chainParts'] = [ chainDictPart ]
            onePartChainDict['chainName'] = legName(chainName, count)            
            listOfChainDicts += [onePartChainDict]
      return listOfChainDicts


def splitChainInLegs(chainName):
      from TriggerMenuMT.HLT.Menu.TriggerConfigHLT import TriggerConfigHLT
      from TrigCompositeUtils.TrigCompositeUtils import legName
      chainDict = TriggerConfigHLT.getChainDictFromChainName(chainName)
      if len(chainDict['chainParts']) ==1:
        return [deepcopy(chainDict)]
                  
      listOfChainDicts = []     
      for count, chainDictPart in enumerate(chainDict['chainParts']):
            onePartChainDict = deepcopy( chainDict )
            onePartChainDict['chainParts'] = [ chainDictPart ]
            onePartChainDict['chainName'] = legName(chainName, count)            
            listOfChainDicts += [onePartChainDict]
      return listOfChainDicts
