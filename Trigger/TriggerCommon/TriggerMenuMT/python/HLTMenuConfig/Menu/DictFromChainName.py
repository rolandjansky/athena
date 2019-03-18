# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Class to obtain the chain configuration dictionary from the short or long name

Author: Catrin Bernius
Original code from TriggerMenu with CB, Joerg Stelzer, Moritz Backes

"""
__author__  = 'Catrin Bernius'
__version__=""
__doc__="Decoding of chain name into a dictionary"


from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logDict = logging.getLogger('TriggerMenu.menu.DictFromChainName')


class DictFromChainName(object):

    def getChainDict(self,chainInfo):
        logDict.debug("chainInfo %s", chainInfo)
        
        # ---- Loop over all chains (keys) in dictionary ----
        # ---- Then complete the dict with other info    ----
        # ---- of format: # chainName :                ----
        # ----  chainCounter (int), L1item (str), Stream (str), EBstep (str)] ----   
        # ---- chainName = chainInfo[0]
        
        if type(chainInfo) == str:
            m_chainName = chainInfo
            m_L1item = ''
            m_L1items_chainParts = []
            m_stream = ''
            m_groups = []
            m_EBstep = ''

        elif type(chainInfo) == list:
            m_chainName = chainInfo[0]
            m_L1item = '' 
            m_L1items_chainParts = chainInfo[1]
            m_stream = chainInfo[2]
            m_groups = chainInfo[3]
            m_EBstep = chainInfo[4]
        else:
            logDict.error("Format of chainInfo passed to genChainDict not known")

        m_L1item = self.getOverallL1item(m_chainName)

        logDict.debug("Analysing chain with name: %s", m_chainName)
        chainProp = self.analyseShortName(m_chainName,  m_L1items_chainParts, m_L1item)
        logDict.debug('ChainProperties: %s', chainProp)

        chainProp['stream'] = m_stream
        chainProp['EBstep'] = m_EBstep
        chainProp['groups'] = m_groups

        logDict.debug('Setting chain multiplicities')
        allChainMultiplicities = self.getChainMultFromDict(chainProp)

        chainProp['chainMultiplicities'] = allChainMultiplicities

        # for additional options: mergingStrategy and topoStartFrom
        if len(chainInfo) > 6 and type(chainInfo) == list:
            for i in xrange(6, len(chainInfo)):
                mergingInfoFilled = False
                tsfInfoFilled = False
                typeOfChainInfo = type(chainInfo[i])

                if typeOfChainInfo is list:
                    if mergingInfoFilled is False:
                        m_mergingStrategy = chainInfo[i][0]
                        if not (m_mergingStrategy == "parallel" or m_mergingStrategy == "serial"):
                            logDict.error("Merging strategy %s is not known.", m_mergingStrategy)
                        m_mergingOffset = chainInfo[i][1]
                        m_mergingOrder = chainInfo[i][2]

                        if(len(chainInfo[i]) >3):
                            m_preserveL2EFOrder = chainInfo[i][3]
                        else:
                            m_preserveL2EFOrder = True
                            
                        chainProp['mergingStrategy'] = m_mergingStrategy
                        chainProp['mergingOffset'] = m_mergingOffset
                        chainProp['mergingOrder'] = m_mergingOrder
                        chainProp['mergingPreserveL2EFOrder'] = m_preserveL2EFOrder

                        mergingInfoFilled = True
                    else: 
                        logDict.error("Something went wrong here....topoStartFrom has already been filled!")                  

                elif typeOfChainInfo is bool:
                    if tsfInfoFilled is False: 
                        chainProp['topoStartFrom'] = chainInfo[i]
                        tsfInfoFilled = True
                    else: 
                        logDict.error("Something went wrong here....topoStartFrom has already been filled!")                  
                else: 
                    logDict.error('Input format not recognised for chainInfo[%s]', chainInfo[i])
                
        # setting the L1 item
        if (chainProp['L1item']== ''): 
            chainProp['L1item'] = m_L1item

        if logDict.isEnabledFor(logging.DEBUG):
            import pprint
            pp = pprint.PrettyPrinter(indent=4, depth=8)
            logDict.debug('FINAL dictionary: %s', pp.pformat(chainProp))


        return chainProp


    def checkL1inName(self, m_chainName):
        if '_L1' in m_chainName:
            return True
        else:
            return False

    def getOverallL1item(self, chainName):
        mainL1 = ''

        if not self.checkL1inName(chainName):
            logDict.warning("Chain name not complying with naming convention: L1 item missing! PLEASE FIX THIS!!")
            return mainL1
        # this assumes that the last string of a chain name is the overall L1 item
        cNameParts = chainName.split("_") 
        mainL1 = cNameParts[-1]

        if "L1" not in mainL1:
            logDict.warning("Chain name not complying with naming convention: L1 item missing! PLEASE FIX THIS!!")
            return ''
        mainL1 = mainL1.replace("L1", "L1_")

        return mainL1
        

    def getChainMultFromDict(self, chainDict):
        allMultis = []
        for cpart in chainDict['chainParts']:
            if cpart['multiplicity'] != '':
                allMultis.append(cpart['multiplicity'])
        return allMultis
            
        
    def getChainMultFromName(self, chainName):
        cNameParts = chainName.split("_") 

        from SignatureDicts import getBasePattern
        pattern = getBasePattern()

        allMultis = []
        for cpart in cNameParts:
            m = pattern.match(cpart)
            if m: 
                logDict.debug("In getChainMultFromName: Pattern found in this string: %s", cpart)
                m_groupdict = m.groupdict()
                if m_groupdict['multiplicity'] == '':
                    multiplicity = 1
                allMultis.append(multiplicity)
        return allMultis

    def getChainThresholdFromName(self, chainName, signature):
        cNameParts = chainName.split("_") 

        from SignatureDicts import getBasePattern
        pattern = getBasePattern()
        trigType = []
        thresholdToPass = 0

        allThresh = []
        for cpart in cNameParts:
            m = pattern.match(cpart)
            if m: 
                logDict.debug("In getChainThresholdFromName: Pattern found in this string: %s", cpart)
                m_groupdict = m.groupdict()
                allThresh.append(m_groupdict['threshold'])
                trigType.append(m_groupdict['trigType'])
                if signature == m_groupdict['trigType']:
                    thresholdToPass = m_groupdict['threshold']
                    break
        return thresholdToPass


    def analyseShortName(self, chainName, L1items_chainParts, L1item_main):
        """
        Function to obtain the chain configuration dictionay from the short name 
        by parsing its components and finding the corrsponding properties which 
        are defined in SliceDicts
        The naming ocnvention is defined inthis document http://
        """

        # ---- dictionary with all chain properties ----
        from SignatureDicts import ChainDictTemplate
        from copy import deepcopy
        genchainDict = deepcopy(ChainDictTemplate)
        genchainDict['chainName'] = chainName
        
        # ---- specific chain part information ----
        allChainProperties=[]
        cparts = chainName.split("_") 
        if 'HLT' in chainName:
            cparts.remove('HLT')

           
        # ---- identify the topo algorithm and add to genchainDict -----
        from SignatureDicts import AllowedTopos
        topo = ''
        topos=[]
        toposIndexed={} 
        topoindex = -5
        for cindex, cpart in enumerate(cparts):
            if  cpart in AllowedTopos:
                logDict.debug('" %s" is in this part of the name %s -> topo alg', AllowedTopos, cpart)
                topo = cpart
                topoindex = cindex
                toposIndexed.update({topo : topoindex})
                chainName=chainName.replace('_'+cpart, '')
                topos.append(topo)

        genchainDict['topo'] = topos
        
        # replace these lines belwo with cparts = chainName.split("_")
        for t, i in enumerate(toposIndexed):
            if (t in cparts): 
                logDict.debug('topo %s with index %s', t, i)
                del cparts[i]        
        
        
        # ---- Find the signature defining patterns ----
        # ---- and write them out in dictionary     ----
        # ---- expected format: <Multiplicity(int)><TriggerType(str)>
        #      <Threshold(int)><isolation,...(str|str+int)> ----
        # EXCEPT FOR CHAINS ...
        from SignatureDicts import getBasePattern
        pattern = getBasePattern()
        mdicts=[]
        multichainindex=[]
        signatureNames = []

        # ---- obtain dictionary parts for signature defining patterns ----
        from SignatureDicts import getSignatureNameFromToken, AllowedCosmicChainIdentifiers, \
            AllowedCalibChainIdentifiers, AllowedMonitorChainIdentifiers, AllowedBeamspotChainIdentifiers

        logDict.debug("cparts: %s", cparts)
        for cpart in cparts:

            logDict.debug("Looping over cpart: %s", cpart)
            m = pattern.match(cpart)
            if m: 
                logDict.debug("Pattern found in this string: %s", cpart)
                m_groupdict = m.groupdict()
                # Check whether the extra contains a special keyword
                
                multiChainIndices = [i for i in range(len(chainName)) if ( chainName.startswith(cpart, i) ) ]
                logDict.debug("MultiChainIndices: %s", multiChainIndices)
                for theMultiChainIndex in multiChainIndices:
                    # this check is necessary for the bjet chains, example: j45_bloose_3j45
                    # j45 would be found in [0, 13], and 3j45 in [12]
                    # so need to make sure the multiplicities are considered here!
                    if (theMultiChainIndex != 0) & (chainName[theMultiChainIndex-1] != '_'): 
                        continue

                    if theMultiChainIndex not in multichainindex:
                        multichainindex.append(theMultiChainIndex)

                logDict.debug("ChainName: %s", chainName)
                logDict.debug("cpart: %s", cpart)
                logDict.debug("m_groupdict: %s", m_groupdict)
                logDict.debug("multichainindex: %s", multichainindex)
                                        
                sName = getSignatureNameFromToken(cpart)
                m_groupdict['signature'] = sName
                logDict.debug('groupdictionary m_groupdict: %s', m_groupdict)
                sigToken = getSignatureNameFromToken(cpart)
                if sigToken not in signatureNames: 
                    signatureNames.append(sigToken)
                mdicts.append(m_groupdict)

                    
            elif cpart =='noalg':
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Streaming', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'streamer', 'extra': ''}
                if 'Streaming' not in signatureNames: 
                    signatureNames.append('Streaming')
                mdicts.append(m_groupdict)
                break # stop loop here so mb doesn't get picked up from min bias slice as it's streaming info

            elif cpart=='mb': 
                logDict.debug('Doing MB')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'MinBias', 'threshold': '', 'multiplicity': '', 
                               'trigType': 'mb', 'extra': ''}
                if 'MinBias' not in signatureNames:  
                    signatureNames.append('MinBias')
                mdicts.append(m_groupdict)

            elif cpart=='hi':
                logDict.debug('Doing HI')
                multichainindex.append(chainName.index(cpart))
                m_groupdict = {'signature': 'HeavyIon', 'threshold': '', 'multiplicity': '',
                               'trigType': 'mb', 'extra': ''}
                if 'HeavyIon' not in signatureNames:  
                    signatureNames.append('HeavyIon')
                mdicts.append(m_groupdict)

            elif cpart in AllowedCosmicChainIdentifiers:
                logDict.debug('COSMIC CHAIN from CosmicDef.py')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Cosmic', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'cosmic', 'extra': ''}
                if 'Cosmic' not in signatureNames: 
                    signatureNames.append('Cosmic')
                logDict.debug('%s',signatureNames)
                mdicts.append(m_groupdict)
                
            elif cpart in AllowedCalibChainIdentifiers:
                logDict.debug('CALIB CHAIN from Calibration')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Calibration', 'threshold': '', 'multiplicity': '', 
                               'trigType': 'calib', 'extra': ''}
                if 'Calibration' not in signatureNames:  
                    signatureNames.append('Calibration')
                logDict.debug('%s',signatureNames)
                mdicts.append(m_groupdict)

            elif cpart in AllowedMonitorChainIdentifiers:
                logDict.debug('Mon CHAIN from Monitor')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Monitoring', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'calib', 'extra': ''}
                if 'Monitoring' not in signatureNames:  
                    signatureNames.append('Monitoring')
                logDict.debug('%s',signatureNames)
                mdicts.append(m_groupdict)

            elif cpart in AllowedBeamspotChainIdentifiers:
                logDict.debug('Beamspot CHAIN from Beamspot')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Beamspot', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'beamspot', 'extra': ''}
                if 'Beamspot' not in signatureNames:  
                    signatureNames.append('Beamspot')
                logDict.debug('%s',signatureNames)
                mdicts.append(m_groupdict)

            elif cpart=='eb': 
                logDict.debug('EnhancedBias chain')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'EnhancedBias', 'threshold': '', 'multiplicity': '', 
                               'trigType': 'eb', 'extra': ''}
                if 'EnhancedBias' not in signatureNames:  
                    signatureNames.append('EnhancedBias')
                mdicts.append(m_groupdict)

           
        # ---- If multiple parts exist, split the string and analyse each  ----
        # ---- part depending on the signature it belongs to ----
        # ----  ----
        multichainparts=[]
        multichainindex = sorted(multichainindex, key=int)
        cN = chainName
        for i in reversed(multichainindex):
            if i!=0:
                logDict.debug('Appending to multichainparts (i!=0): %s', chainName[i:len(cN)])

                multichainparts.append(chainName[i:len(cN)])
                cN = cN[0:i-1]
            else:
                logDict.debug('Appending to multichainparts: %s', chainName[i:len(cN)])
                multichainparts.append(cN)
        logDict.debug("multichainparts: %s",multichainparts)

        # ---- build the chainProperties dictionary for each part of the chain ----
        # ---- add it to a allChainProperties ----
        multichainparts.reverse()
        for chainindex, chainparts in enumerate(multichainparts):
            chainProperties = {} #will contain properties for one part of chain if multiple parts

            logDict.debug('chainparts %s', chainparts)

            # ---- check if L1 item is specified in chain Name ----
            L1item = '' 
            chainpartsNoL1 = chainparts
            
            #Checking for L1 item for chain part and overall in the name
            #and the L1 for the overall chain
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
                    logDict.error("Typo in chain name for overall L1 item? Please check again")
                else:#remove the last part from the chain name
                    chainparts = chainparts[:allL1indices[1]]
                    
                if (allL1items[0] not in L1items_chainParts):
                    logDict.error("L1 chain part " +L1items_chainParts+" item does not seem to match the one in the chain name "+allL1items[0])



            #this only find the FIRST instance on L1
            L1index = chainparts.find('_L1') 
                                        
            logDict.debug('chainparts: %s', chainparts)
            logDict.debug('L1index: %s', L1index)
            if L1index != -1:
                L1index = L1index+1 # to compensate the _
                L1itemfromChainName = chainparts[L1index:]
                if (L1itemfromChainName[2]=='_'):
                    raise RuntimeError('ERROR IN CHAIN NAMING: L1 items in chainNames are specified e.g. *_L1EM4, not *_L1_EM4')
                else:
                    L1item = L1itemfromChainName[:2]+'_'+L1itemfromChainName[2:]
                    if (L1item[-1] == '_'): 
                        L1item = L1item[:-1]
                        
                chainpartsNoL1 = chainparts[:L1index-1] # -1 to also remove _

            else: 
                logDict.debug('No L1 item specified in the name')
                

            # ---- assign L1 to chain parts from L1 item list in menu ----
            # ---- check if enough L1 items given for chain parts ----
            if (len(L1items_chainParts) > 0):
                if (len(L1items_chainParts) != len(multichainparts)):
                    logDict.info("Not enough L1 items for chain parts of chain %s defined => fix in menu please!",
                                 genchainDict['chainName'])

                chainProperties['L1item'] = L1items_chainParts[chainindex]                
                if (L1item !=  chainProperties['L1item']) & (L1item !=''):
                    logDict.info("L1 item in name (%s) of chain %s does not match with given L1item list (%s)!",
                                 L1item, chainName, chainProperties['L1item'])

            else:
                logDict.debug('No L1 item specified in the name')

            parts=chainpartsNoL1.split('_')
            parts = filter(None,parts)

            chainProperties['trigType']=mdicts[chainindex]['trigType']
            chainProperties['extra']=mdicts[chainindex]['extra']
            multiplicity = mdicts[chainindex]['multiplicity'] if not mdicts[chainindex]['multiplicity'] == '' else '1'
            chainProperties['multiplicity'] = multiplicity
            chainProperties['threshold']=mdicts[chainindex]['threshold']
            chainProperties['signature']=mdicts[chainindex]['signature']

            # if we have a L1 topo in a multi-chain then we want to remove it from the chain name
            # but only if it's the same as the L1item_main; otherwise it belongs to chain part and we q
            # have to keep it in the name
            chainProperties['chainPartName'] = chainparts
            if ('-' in L1item) and (len(multichainparts) > 1) and (L1item_main == L1item):
                chainProperties['chainPartName'] = chainpartsNoL1                
            ##if we have the overall L1 item in a multi-part chains, then we want to remove it from the chain part name
            if( L1item==L1item_main and (len(multichainparts) > 1) and (L1item.count("_") > 1 )):                
                chainProperties['chainPartName'] = chainpartsNoL1
                                
            logDict.debug('Chainparts: %s', chainparts)
            if (chainProperties['signature'] != 'Cosmic') \
                    & (chainProperties['signature'] != 'Calibration')\
                    & (chainProperties['signature'] != 'Streaming') \
                    & (chainProperties['signature'] != 'Beamspot') \
                    & (chainProperties['signature'] != 'Monitoring') : 
                parts.pop(0)


            #---- Check if topo is a bphsyics topo -> change signature ----            
            from SignatureDicts import AllowedTopos_bphys
            for t in genchainDict['topo']:
                if (t in AllowedTopos_bphys):
                    chainProperties['signature'] = 'Bphysics'


            # ---- import the relevant dictionaries for each part of the chain ---- 
            from SignatureDicts import getSignatureInformation
            SignatureDefaultValues, allowedSignaturePropertiesAndValues = getSignatureInformation(chainProperties['signature'])
            logDict.debug('SignatureDefaultValues: %s', SignatureDefaultValues)
            
            # ---- update chain properties with default properties ----
            result = deepcopy(SignatureDefaultValues)
            result.update(chainProperties)
            chainProperties = result

            # ---- check remaining parts for complete machtes in allowedPropertiesAndValues Dict ----
            # ---- unmatched = list of tokens that are not found in the allowed values as a whole ----
            parts = filter(None, parts)     #removing empty strings from list

            matchedparts = []
            for pindex, part in enumerate(parts):
                origpart = part
                for prop, allowedValues in allowedSignaturePropertiesAndValues.items():
                    if part in allowedValues:
                        if type(chainProperties[prop]) is list:
                            chainProperties[prop] += [part]
                        else:
                            chainProperties[prop] = part
                        matchedparts.append(part)
                        
            logDict.debug("matched parts %s", matchedparts)
            leftoverparts = set(parts)-set(matchedparts)
            logDict.debug('leftoverparts %s', leftoverparts)
            for pindex, part in enumerate(leftoverparts):
                for prop, allowedValues in allowedSignaturePropertiesAndValues.items():
                    if prop in chainProperties.keys():  
                        continue
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
            logDict.debug('%s', set(chainProperties.keys()))
            logDict.debug('%s', set(allowedSignaturePropertiesAndValues.keys()))
            for fb in forbiddenProperties:
                forbiddenValue = chainProperties.pop(fb)
                if forbiddenValue != '':
                    raise RuntimeError("Property %s not allowed for signature '%s', but specified '%s'" % (fb, chainProperties['signature'], forbiddenValue))
                
            # ---- the info of the general and the specific chain parts dict ----
            allChainProperties.append(chainProperties)


        # ---- depending on if signatures are different in this chain, break up the chainProperties dictionary ----
        # ---- finally also taking care of the signatrue key ----
        genchainDict['chainParts'] = allChainProperties
        genchainDict['signature'] = allChainProperties[0]['signature']

        logDict.debug('genchainDict that is passed as Final dict %s', genchainDict)

        return genchainDict







