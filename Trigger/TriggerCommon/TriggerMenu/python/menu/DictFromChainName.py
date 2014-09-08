# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""

Class to obtain the chain configuration dictionary from the short or long name

Authors: Catrin Bernius, Joerg Stelzer, Moritz Backes
Written in December 2013

"""
__author__  = 'Moritz Backes & Catrin Bernius & Joerg Stelzer'
__version__=""
__doc__="Obtaining Dictionaries from Chain Names"


import re

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logDict = logging.getLogger('TriggerMenu.menu.DictFromChainName')

class DictFromChainName(object):

    def getChainDict(self,chainInfo):
        logDict.debug("chainInfo "+ str(chainInfo))
        
        # ---- Loop over all chains (keys) in dictionary ----
        # ---- Then complete the dict with other info    ----
        # ---- of formate: # chainName :                ----
        # ---- [chainName (str), chainCounter (int), L1item (str), Stream (str), EBstep (str)] ----   
        # ---- chainName = chainInfo[0]
        
        m_chainName = chainInfo[0]
        m_chainCounter = chainInfo[1]
        m_L1item = chainInfo[2]
        m_L1items_chainParts = chainInfo[3]
        m_stream = chainInfo[4]
        m_groups = chainInfo[5]
        m_EBstep = chainInfo[6]
        if len(chainInfo) > 7:
            m_mergingStrategy = chainInfo[7][0]
            if not (m_mergingStrategy == "parallel" or m_mergingStrategy == "serial"):
                logDict.error("Merging strategy %s is not known." % m_mergingStrategy)
            m_mergingOffset = chainInfo[7][1]
            m_mergingOrder = chainInfo[7][2]

        logDict.info("Analysing chain with name: "+ m_chainName)

        chainProp = self.analyseShortName(m_chainName,  m_L1items_chainParts)
        logDict.debug('ChainProperties: '+ str(chainProp))

        chainProp['chainCounter']= m_chainCounter
        chainProp['stream'] = m_stream
        chainProp['EBstep'] = m_EBstep
        chainProp['groups'] = m_groups
        if len(chainInfo) > 7:
            chainProp['mergingStrategy'] = m_mergingStrategy
            chainProp['mergingOffset'] = m_mergingOffset
            chainProp['mergingOrder'] = m_mergingOrder

        if (chainProp['L1item']== ''): 
            chainProp['L1item'] = m_L1item

        import pprint
        pp = pprint.PrettyPrinter(indent=4, depth=8)
        logDict.debug('FINAL dictionary: '+ pp.pformat(chainProp))

        return chainProp



    def analyseShortName(self, chainName, L1items_chainParts):
        """
        Function to obtain the chain configuration dictionay from the short name 
        by parsing its components and finding the corrsponding properties which 
        are defined in SliceDicts
        The naming ocnvention is defined inthis document http://
        """
        chainName_orig = chainName

        # ---- dictionary with all chain properties ----
        from SignatureDicts import ChainDictTemplate
        from copy import deepcopy
        genchainDict = deepcopy(ChainDictTemplate)
        genchainDict['chainName'] = chainName
        

        # ---- check for L1Topo in chain name ----
        L1topoitemFromChainName = ''; L1topoitem = ''; L1topoindex   = -5
        L1topoindex = [n for n in xrange(len(chainName)) if chainName.find('L1', n) == n]
        if (L1topoindex): 
            logDict.debug('MEOW L1topindex: '+ str(L1topoindex))

        if (len(L1topoindex) == 1):
            L1topoitemfromChainName = chainName[L1topoindex[0]:]
            if ('-' in L1topoitemfromChainName):
                if (L1topoitemfromChainName[2]=='_'):
                    raise RuntimeError('NOT FOLLOWING THE NAMING CONVENTION: L1 items in chainNames are specified e.g. L1EM4_MU4, not L1_EM4_MU4')
                else:
                    L1topoitem = L1topoitemfromChainName[:2]+'_'+L1topoitemfromChainName[2:]
                    logDict.debug('L1topoitem: ', str(L1topoitem))
                    chainName = chainName[:L1topoindex[0]-1] # -1 to also remove _
                    # build in a check to compare l1 item given with the one found
                    genchainDict['L1item'] = L1topoitem
            else: 
                logDict.debug('L1 item in name is not a L1Topo item')


        # ---- specific chain part information ----
        allChainProperties=[]
        #logDict.debug("ChainName: ", str(chainName))
        cparts = chainName.split("_") 

           
        # ---- identify the topo algorithm and add to genchainDict -----
        from SignatureDicts import AllowedTopos
        topo = '';topos=[];toposIndexed={}; topoindex = -5
        for cindex, cpart in enumerate(cparts):
            if ('-' in cpart) | (':' in cpart) or cpart in AllowedTopos:
                logDict.debug('"- or : or %s" is in this part of the name %s -> topo alg' % (str(AllowedTopos), str(cpart)))
                topo = cpart
                topoindex = cindex
                toposIndexed.update({topo : topoindex})
                chainName=chainName.replace('_'+cpart, '')
                topos.append(topo)

        genchainDict['topo'] = topos
        
        # replace these lines belwo with cparts = chainName.split("_")
        for t, i in enumerate(toposIndexed):
            if (t in cparts): 
                logDict.debug('topo %s with index' %(t), i)
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
            AllowedCalibChainIdentifiers, AllowedStreamingChainIdentifiers, \
            AllowedMonitorChainIdentifiers, AllowedBeamspotChainIdentifiers

        logDict.debug("cparts: "+ str(cparts))
        for cpart in cparts:
            logDict.debug("Looping over cpart: "+ str(cpart))
            m = pattern.match(cpart)
            
            if m: 
                logDict.debug("Pattern found in this string:"+ str(cpart))
                m_groupdict = m.groupdict()
                multiChainIndices = [i for i in range(len(chainName)) if chainName.startswith(cpart, i)]
                logDict.debug("MultiChainIndices: "+ str(multiChainIndices))
                for theMultiChainIndex in multiChainIndices:
                    # this check is necessary for the bjet chains, example: j45_bloose_3j45
                    # j45 would be found in [0, 13], and 3j45 in [12]
                    # so need to make sure the multiplicities are considered here!
                    if (theMultiChainIndex != 0) & (chainName[theMultiChainIndex-1] != '_'): continue                    
                    if not theMultiChainIndex in multichainindex:
                        multichainindex.append(theMultiChainIndex)

                logDict.debug("ChainName: "+ str(chainName))
                logDict.debug("cpart: "+ str(cpart))
                logDict.debug("m_groupdict: "+ str(m_groupdict))
                        
                sName = getSignatureNameFromToken(cpart)
                m_groupdict['signature'] = sName
                logDict.debug('groupdictionary m_groupdict: '+ str(m_groupdict))
                sigToken = getSignatureNameFromToken(cpart)
                if sigToken not in signatureNames: signatureNames.append(sigToken)
                mdicts.append(m_groupdict)

                    
            elif cpart =='noalg':
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Streaming', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'streamer', 'extra': ''}
                if 'Streaming' not in signatureNames: signatureNames.append('Streaming')
                mdicts.append(m_groupdict)
                break # stop loop here so mb doesn't get picked up from min bias slice as it's streaming info

            elif cpart=='mb': 
                logDict.debug('Doing MB')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'MinBias', 'threshold': '', 'multiplicity': '', 
                               'trigType': 'mb', 'extra': ''}
                if 'MinBias' not in signatureNames:  signatureNames.append('MinBias')
                mdicts.append(m_groupdict)
                
            elif cpart in AllowedCosmicChainIdentifiers:
                logDict.debug('COSMIC CHAIN from CosmicDef.py')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Cosmic', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'cosmic', 'extra': ''}
                if 'Cosmic' not in signatureNames: signatureNames.append('Cosmic')
                logDict.debug(str(signatureNames))
                mdicts.append(m_groupdict)
                
            elif cpart in AllowedCalibChainIdentifiers:
                logDict.debug('CALIB CHAIN from Calibration')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Calibration', 'threshold': '', 'multiplicity': '', 
                               'trigType': 'calib', 'extra': ''}
                if 'Calibration' not in signatureNames:  signatureNames.append('Calibration')
                logDict.debug(str(signatureNames))
                mdicts.append(m_groupdict)

            elif cpart in AllowedMonitorChainIdentifiers:
                logDict.debug('Mon CHAIN from Monitor')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Monitoring', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'calib', 'extra': ''}
                if 'Monitoring' not in signatureNames:  signatureNames.append('Monitoring')
                logDict.debug(str(signatureNames))
                mdicts.append(m_groupdict)

            elif cpart in AllowedBeamspotChainIdentifiers:
                logDict.debug('Beamspot CHAIN from Beamspot')
                multichainindex.append(chainName.index(cpart)) 
                m_groupdict = {'signature': 'Beamspot', 'threshold': '', 'multiplicity': '', 
                                'trigType': 'beamspot', 'extra': ''}
                if 'Beamspot' not in signatureNames:  signatureNames.append('Beamspot')
                logDict.debug(str(signatureNames))
                mdicts.append(m_groupdict)


        # ---- If multiple parts exist, split the string and analyse each  ----
        # ---- part depending on the signature it belongs to ----
        # ----  ----
        multichainparts=[]

        remainder = chainName
        #print "MEOW remainder", remainder
        #print "MEOW multichainindex", multichainindex
        multichainindex = sorted(multichainindex, key=int)
        #print "MEOW multichainindex", multichainindex
        cN = chainName
        for i in reversed(multichainindex):
            if i!=0:
                
                multichainparts.append(chainName[i:len(cN)])
                cN = cN[0:i-1]
            else:
                multichainparts.append(cN)
        logDict.debug("MEOW multichainparts: "+ str(multichainparts))

        # ---- build the chainProperties dictionary for each part of the chain ----
        # ---- add it to a allChainProperties ----
        multichainparts.reverse()
        for chainindex, chainparts in enumerate(multichainparts):
            chainProperties = {} #will contain properties for one part of chain if multiple parts

            logDict.debug('chainparts'+ str(chainparts))

            # ---- check if L1 item is specified in chain Name ----
            L1itemFromChainName = ''; L1item = ''; 
            chainpartsNoL1 = chainparts
            
            L1index = chainparts.find('_L1') 
            logDict.debug('chainparts: '+ str(chainparts))
            logDict.debug('L1index: '+ str(L1index))
            if L1index != -1:
                L1index = L1index+1 # to compensate the _
                L1itemfromChainName = chainparts[L1index:]
                if (L1itemfromChainName[2]=='_'):
                    raise RuntimeError('ERROR IN NAMING CONVENTION: L1 items in chainNames are specified e.g. *_L1EM4, not *_L1_EM4')
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
                    logDict.info("Not enough L1 items for chain parts of chain %s defined => fix in menu please!" %(genchainDict['chainName']))



                chainProperties['L1item'] = L1items_chainParts[chainindex]                
                if (L1item !=  chainProperties['L1item']) & (L1item !=''):
                    logDict.info("L1 item in name (%s) of chain %s does not match with given L1item list (%s)!" % (L1item, chainName, chainProperties['L1item']))

            else:
                logDict.debug('No L1 item specified in the name')


            #print 'MEOW chainpartsNoL1', chainpartsNoL1
            parts=chainpartsNoL1.split('_')
            parts = filter(None,parts)

            #print 'parts after L1 string removal = ',parts
            # ---- start with first pattern and write into dict and remove it afterwards ----
            #print 'MOO chainindex', chainindex
            #print 'MOO mdicts', mdicts
            chainProperties['trigType']=mdicts[chainindex]['trigType']
            chainProperties['extra']=mdicts[chainindex]['extra']
            multiplicity = mdicts[chainindex]['multiplicity'] if not mdicts[chainindex]['multiplicity'] == '' else '1'
            chainProperties['multiplicity'] = multiplicity
            chainProperties['threshold']=mdicts[chainindex]['threshold']
            chainProperties['signature']=mdicts[chainindex]['signature']
            chainProperties['chainPartName'] = chainparts #"_".join(filter(None,parts))
            logDict.debug('Chainparts: '+ str(chainparts))
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
            logDict.debug('SignatureDefaultValues: '+ str(SignatureDefaultValues))
            
            # ---- update chain properties with default properties ----
            result = deepcopy(SignatureDefaultValues)
            result.update(chainProperties)
            chainProperties = result

            # ---- check remaining parts for complete machtes in allowedPropertiesAndValues Dict ----
            # ---- unmatched = list of tokens that are not found in the allowed values as a whole ----
            unmatched = [] 
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
                        
            logDict.debug("matched parts "+ str(matchedparts))
            leftoverparts = set(parts)-set(matchedparts)
            logDict.debug('leftoverparts'+ str(leftoverparts))
            for pindex, part in enumerate(leftoverparts):
                for prop, allowedValues in allowedSignaturePropertiesAndValues.items():
                    if prop in chainProperties.keys():  continue
                    for aV in allowedValues:
                        if (aV in part):
                            if (chainProperties['signature'] in ['Egamma', 'Muon'] )& (prop in ['trkInfo','hypoInfo']):
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
            logDict.debug(str(set(chainProperties.keys()))) 
            logDict.debug(str(set(allowedSignaturePropertiesAndValues.keys())))
            for fb in forbiddenProperties:
                forbiddenValue = chainProperties.pop(fb)
                if forbiddenValue != '':
                    raise RuntimeError("Property %s not allowed for signature '%s', but specified '%s'" % (fb, chainProperties['signature'], forbiddenValue))



            # ---- the info of the general and the specific chain parts dict ----
            allChainProperties.append(chainProperties)


        # ---- depending on if signatures are different in this chain, break up the chainProperties dictionary ----
        # ---- finally also taking care of the signatrue key ----
        #if len(signatureNames) == 1:
        genchainDict['chainParts'] = allChainProperties

        # CB DELETE FROM DICTIONARY???? 
        genchainDict['signature'] = allChainProperties[0]['signature']

        logDict.debug('genchainDict that is passed as Final dict'+ str(genchainDict))
        #for cprop in allChainProperties: del cprop['signature']
             
        return genchainDict







