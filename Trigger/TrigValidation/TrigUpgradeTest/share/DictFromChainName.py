# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""

Class to obtain the chain configuration dictionary from the short or long name

Authors: Catrin Bernius, Joerg Stelzer, Moritz Backes
Written in December 2013
Re-written for AthenaMT starting in July 2017

"""
__author__  = 'Moritz Backes & Catrin Bernius & Joerg Stelzer'
__version__=""
__doc__="Obtaining Dictionaries from Chain Names"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger('TriggerMenu.menu.DictFromChainName')

include("TrigUpgradeTest/SignatureDicts.py")


class DictFromChainName(object):

    def getChainDict(self,chainInfo):
        log.debug("chainInfo %s", chainInfo)
        
        m_chainName = chainInfo[0]
        m_L1item = chainInfo[1]
        m_L1items_chainParts=[]

        log.debug("Analysing chain with name: %s", m_chainName)
        chainProp = self.analyseShortName(m_chainName,  m_L1items_chainParts, m_L1item)
        log.debug('ChainProperties: %s', chainProp)
               
        if (chainProp['L1item']== ''): chainProp['L1item'] = m_L1item

        import pprint
        pp = pprint.PrettyPrinter(indent=4, depth=8)
        log.info('FINAL dictionary: %s', pp.pformat(chainProp))

        return chainProp

    def analyseShortName(self, chainName, L1items_chainParts, L1item_main):
        """
        Function to obtain the chain configuration dictionay from the short name 
        by parsing its components and finding the corrsponding properties which 
        are defined in SliceDicts
        """        
        from copy import deepcopy

        # ---- dictionary with all chain properties ----
        finalChainDict = deepcopy(ChainDictTemplate)
        finalChainDict['chainName'] = chainName
        
        # ---- specific chain part information ----
        cparts = chainName.split("_") 
        log.debug("cparts: %s", cparts)
                   
        # ---- Find the signature defining patterns as BasePattern ----
        # ---- <Multiplicity(int)><TriggerType(str)><Threshold(int)><etra,...(str|str+int)> ----
        pattern = getBasePattern()

        # ---- define some dictionaries ----
        chainPartDicts=[] # stores the list of dictionaries in case of multiple chain parts
        mdicts=[] # generated when checking for the BasePattern
        multichainindex=[]
        signatureNames = []

        # ---- Loop over all chain name parts ----
        for cpart in cparts:
            log.debug("Looping over cpart: %s", cpart)

            m = pattern.match(cpart)
            if m: 
                log.debug("Pattern found in this string: %s", cpart)
                m_groupdict = m.groupdict()
                multiChainIndices = [i for i in range(len(chainName)) if ( chainName.startswith(cpart, i) ) ]
                log.debug("MultiChainIndices: %s", multiChainIndices)
                for theMultiChainIndex in multiChainIndices:
                    # this check is necessary for the bjet chains, example: j45_bloose_3j45
                    if (theMultiChainIndex != 0) & (chainName[theMultiChainIndex-1] != '_'): continue
                    if not theMultiChainIndex in multichainindex:
                        multichainindex.append(theMultiChainIndex)
                log.debug("multichainindex: %s", multichainindex)
                                        
                m_groupdict['signature'] = getSignatureNameFromToken(cpart)
                sigToken = getSignatureNameFromToken(cpart)
                if sigToken not in signatureNames: signatureNames.append(sigToken)
                mdicts.append(m_groupdict)
                    
            else: 
                log.info('not a BasePattern, continue with analysis')
           
        # ---- If multiple parts exist, split the string and analyse each  ----
        # ---- part depending on the signature it belongs to ----
        multichainparts=[]
        multichainindex = sorted(multichainindex, key=int)
        cN = chainName
        for i in reversed(multichainindex):
            if i!=0:
                log.debug('Appending to multichainparts (i!=0): %s', chainName[i:len(cN)])
                multichainparts.append(chainName[i:len(cN)])
                cN = cN[0:i-1]
            else:
                log.debug('Appending to multichainparts: %s', chainName[i:len(cN)])
                multichainparts.append(cN)
        log.debug("multichainparts: %s",multichainparts)

        # ---- build the chainProperties dictionary for each part of the chain ----
        # ---- add it to a chainPartDicts ----
        multichainparts.reverse()
        for chainindex, chainparts in enumerate(multichainparts):
            chainProperties = {} #will contain properties for one part of chain if multiple parts
            log.debug('chainparts %s', chainparts)

            # ---- check if L1 item is specified in chain Name ----
            L1itemfromChainName = ''; L1item = ''; 
            chainpartsNoL1 = chainparts
            
            # Checking for L1 item for chain part and overall in the name and the L1 for the overall chain
            if (chainparts.count("_L1") > 1):
                # get position of all L1 items
                import re
                allL1indices = [match.start() for match in re.finditer(re.escape("_L1"), chainparts)]
                allL1items = []
                m_chainparts = deepcopy(chainparts)
                for index in reversed(allL1indices):
                    newindex = index+1
                    m_newitem = m_chainparts[newindex:]  
                    m_chainparts = m_chainparts[:newindex-1]
                    newitem = m_newitem[:2]+'_'+m_newitem[2:]
                    allL1items.append(newitem)
                    
                allL1items.reverse()            
                if (allL1items[1] != L1item_main):
                    log.error("Typo in chain name for overall L1 item? Please check again")
                else:  # remove the L1 item from the chain name
                    chainparts = chainparts[:allL1indices[1]]
                    
                if (allL1items[0] not in L1items_chainParts):
                    log.error("L1 chain part " +L1items_chainParts+" does not match the one in the chain name "+allL1items[0])

            #this only finds the FIRST instance on L1
            L1index = chainparts.find('_L1')                                         
            log.debug('chainparts: %s', chainparts)
            log.debug('L1index: %s', L1index)
            if L1index != -1:
                L1index = L1index+1 # to compensate the _
                L1itemfromChainName = chainparts[L1index:]
                if (L1itemfromChainName[2]=='_'):
                    raise RuntimeError('ERROR IN CHAIN NAME: L1 items are specified as e.g. *_L1EM4, not *_L1_EM4')
                else:
                    L1item = L1itemfromChainName[:2]+'_'+L1itemfromChainName[2:]
                    if (L1item[-1] == '_'): L1item = L1item[:-1]
                        
                chainpartsNoL1 = chainparts[:L1index-1] # -1 to also remove _
 
            else: 
                log.debug('No L1 item specified in the name')
                
            # ---- assign L1 to chain parts from L1 item list in menu ----
            # ---- check if enough L1 items given for chain parts ----
            if (len(L1items_chainParts) > 0):
                if (len(L1items_chainParts) != len(multichainparts)):
                    log.info("Not enough L1 items for chain parts of chain %s defined", finalChainDict['chainName'])

                chainProperties['L1item'] = L1items_chainParts[chainindex]                
                if (L1item !=  chainProperties['L1item']) & (L1item !=''):
                    log.info("L1 item in name (%s) of chain %s does not match with given L1item list (%s)!",
                                 L1item, chainName, chainProperties['L1item'])
            else:
                log.debug('No L1 item specified in the name')

            parts = chainpartsNoL1.split('_')
            parts = filter(None,parts)

            # ---- start writing first base pattern info into dict and remove it afterwards ----
            chainProperties['trigType']=mdicts[chainindex]['trigType']
            chainProperties['extra']=mdicts[chainindex]['extra']
            multiplicity = mdicts[chainindex]['multiplicity'] if not mdicts[chainindex]['multiplicity'] == '' else '1'
            chainProperties['multiplicity'] = multiplicity
            chainProperties['threshold']=mdicts[chainindex]['threshold']
            chainProperties['signature']=mdicts[chainindex]['signature']

            # ---- if we have a L1 topo in a multi-chain then we want to remove it from the chain name
            # but only if it's the same as the L1item_main; otherwise it belongs to chain part and we keep it ----
            chainProperties['chainPartName'] = chainparts
            if ('-' in L1item) and (len(multichainparts) > 1) and (L1item_main == L1item):
                chainProperties['chainPartName'] = chainpartsNoL1                
            ## if we have the overall L1 item in a multi-part chains, then we want to remove it from the chain part name
            if( L1item==L1item_main and (len(multichainparts) > 1) and (L1item.count("_") > 1 )):                
                chainProperties['chainPartName'] = chainpartsNoL1
                                
            log.debug('Chainparts: %s', chainparts)
            parts.pop(0)

            # ---- import the relevant dictionaries for each part of the chain ---- 
            SignatureDefaultValues, allowedSignaturePropertiesAndValues = getSignatureInformation(chainProperties['signature'])
            log.debug('SignatureDefaultValues: %s', SignatureDefaultValues)
            
            # ---- update chain properties with default properties ----
            sigDefault = deepcopy(SignatureDefaultValues)
            sigDefault.update(chainProperties)
            chainProperties = sigDefault

            # ---- check remaining parts for complete machtes in allowedPropertiesAndValues Dict ----
            parts = filter(None, parts)     #removing empty strings from list

            matchedparts = []
            for pindex, part in enumerate(parts):
                origpart = part
                for prop, allowedValues in allowedSignaturePropertiesAndValues.items():
                    if part in allowedValues:
                        if type(chainProperties[prop]) == list:
                            chainProperties[prop] += [part]
                        else:
                            chainProperties[prop] = part
                        matchedparts.append(part)
                        
            log.debug("matched parts %s", matchedparts)
            leftoverparts = set(parts)-set(matchedparts)
            log.debug('leftoverparts %s', leftoverparts)
            for pindex, part in enumerate(leftoverparts):
                for prop, allowedValues in allowedSignaturePropertiesAndValues.items():
                    if prop in chainProperties.keys():  continue
                    for aV in allowedValues:
                        if (aV in part):
                            if (chainProperties['signature'] in ['Egamma', 'Muon'] )& (prop in ['trkInfo','hypoInfo']):
                                chainProperties[prop] = part
                                part = part.replace(part,'')
                            elif (chainProperties['signature'] in ['Jet'] )& (prop in ['gscThreshold']):
                                chainProperties[prop] = part
                                part = part.replace(part,'')
                            else:                                    
                                chainProperties[prop] = aV
                                part = part.replace(aV,'')
                                break # done with allowed values for that property
                            
                if len(part.split())>0:
                    raise RuntimeError("These parts of the chain name %s are not understood %r" % (origpart,part))

            # ---- remove properties that aren't allowed in the chain properties for a given siganture ----
            forbiddenProperties = set(chainProperties.keys()) - set(allowedSignaturePropertiesAndValues.keys())
            log.debug('%s', set(chainProperties.keys()))
            log.debug('%s', set(allowedSignaturePropertiesAndValues.keys()))
            for fb in forbiddenProperties:
                forbiddenValue = chainProperties.pop(fb)
                if forbiddenValue != '':
                    raise RuntimeError("Property %s not allowed for signature '%s', but specified '%s'" % (fb, chainProperties['signature'], forbiddenValue))
                
            # ---- the info of the general and the specific chain parts dict ----
            chainPartDicts.append(chainProperties)

        # ---- depending on if signatures are different in this chain, break up the chainProperties dictionary ----
        # ---- finally also taking care of the signatrue key ----
        finalChainDict['chainParts'] = chainPartDicts
        finalChainDict['signature'] = chainPartDicts[0]['signature']
        log.debug('finalChainDict that is passed as Final dict %s', finalChainDict)
             
        return finalChainDict
