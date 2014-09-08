# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Jet signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logJet = logging.getLogger("TriggerMenu.jet.generateJetChainDefs")

from TriggerMenu.jet.JetDef import generateHLTChainDef
from TriggerMenu.jet.JetDef_HT import L2EFChain_HT

from TriggerMenu.menu.MenuUtils import *
##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    cP = chainDict['chainParts']
    #listOfChainDicts = splitChainDict(chainDict)
    #listOfChainDefs = []

    #allTrigTypes = []
    #for subCD in listOfChainDicts:  allTrigTypes.append(subCD['chainParts']['trigType'])
    #htchain = False
    #if ('ht' in allTrigTypes): htchain = True
    
    # ATTENTION: This assumes chains of type jXX_htYY but not jXX_jYY_htZZ
    # In the latter case, the chainDict would have to be split seperately

    if type(cP) == list:
        trigtype = cP[0]['trigType']
    else:
        trigtype = cP['trigType']       
	
    if ('ht' in trigtype):
        HTchains = L2EFChain_HT(chainDict)
        theChainDef = HTchains.generateHLTChainDef()        
    else:           
        theChainDef = generateHLTChainDef(chainDict)

    # if htchain:
    #     for subChainDict in listOfChainDicts:
    #         if ('ht' in subChainDict['chainParts']['trigType']):
    #             HTchain = L2EFChain_HT(subChainDict)
    #             listOfChainDefs += [HTchain.generateHLTChainDef()]    
    #         if ('j' in subChainDict['chainParts']['trigType']):
    #             listOfChainDefs += [generateHLTChainDef(subChainDict)] 

    #     if len(listOfChainDefs)>2:
    #         logJet.error('Chains of type jXX_jYY_htZZ not implemented yet!! Can only handle chains like jXX_htYY.')
    #     elif len(listOfChainDefs)>1:
    #         theChainDef = mergeChainDefs(listOfChainDefs)
    #     else:
    #         theChainDef = listOfChainDefs[0]

    # else:
    #     theChainDef = generateHLTChainDef(chainDict)



    if 'muvtx' not in  chainDict["topo"]:
        logJet.info( "No topo given.")
    else:
        logJet.info("Adding topo to jet chain")
        theChainDef = _addTopoInfo(theChainDef, chainDict)    

    return theChainDef


##########################################################################################
def _addTopoInfo(theChainDef,chainDict,doAtL2AndEF=True):

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    topoAlgs = chainDict["topo"]

    if ('muvtx' in topoAlgs):
        theChainDef = generateHVchain(theChainDef, chainDict, inputTEsL2, inputTEsEF)
    else:
        mJet.error('Currntly only muvtx configured. You have add your topo configuration.')

    return theChainDef

##########################################################################################
def generateHVchain(theChainDef, chainDict, inputTEsL2, inputTEsEF):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    topoAlgs = chainDict["topo"]
    l1item = chainDict["L1item"]
    l1item = l1item[4:]

    
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoB
    fexes_l2_SiTrackFinder_muonIsoB = TrigL2SiTrackFinder_muonIsoB()
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
    fexes_l2_MuonCluster = MuonClusterConfig(IDtracking="STRATEGY_B")
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoConfig
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterAllMSHypoConfig
    hypos_l2_MuonCluster = MuonClusterHypoConfig()
    hypos_l2_MuonCluster_ExtendedEta = MuonClusterAllMSHypoConfig()
     
    TEmuonIsoB = HLTChainName+'_muonIsoB'
    TEmuonClusterFex = HLTChainName+'_MuonClusterFex'
    TEmuonClusterHypo = HLTChainName+'_MuonClusterHypo'

    # adding muonIso sequence
    theChainDef.addSequence([fexes_l2_SiTrackFinder_muonIsoB],l1item, TEmuonIsoB)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonIsoB])
    
    # adding seq using jetTE and TE from seq above (MuonCluster fex)
    theChainDef.addSequence([fexes_l2_MuonCluster], [TEmuonIsoB, inputTEsEF], TEmuonClusterFex)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonClusterFex])

    # adding MuonCluster hypo
    theChainDef.addSequence([hypos_l2_MuonCluster], TEmuonClusterFex, TEmuonClusterHypo)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonClusterHypo])

    return theChainDef
    
