# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'M.Woudstra, Y.Nakahama, K.Nagano'
__doc__="Definition of muon chains as used v4- menu" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

from TriggerMenu.muon.MuonDef import L2EFChain_mu as L2EFChain_mu
try:
    from TriggerMenu.muon.MuonDefIdTest import L2EFChain_mu as L2EFChain_mu_IdTest
except:
    log.error('generateMuonChainDefs: Problems when importing MuonDefIdTest.')
    log.info(traceback.print_exc())


from TriggerMenu.menu.MenuUtils import *

###########################################################################
###########################################################################

##Create a list with the threshold of all the muons in the chain
##it's needed to correctly configure the hypo for the FS muon chains
def GetAllMuonThresholds(chainDict):
    
    muons = []
    for chainpart in chainDict['chainParts']:
      if 'mu' in chainpart['trigType']:        
          for x in range(0,int(chainpart['multiplicity'])):

              if chainpart['threshold']!='0':
                  thr= '%sGeV' %(chainpart['threshold'] )
              else:
                  thr= 'passthrough'

              muons.append(thr)

    return muons

def generateChainDefs(chainDict):
    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)

    listOfChainDefs = []

    asymDiMuonChain = False
    if (len(listOfChainDicts) > 1): asymDiMuonChain = True
    else: asymDiMuonChain = False

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Muon = L2EFChain_mu_IdTest(subChainDict, asymDiMuonChain)
        else:
            #If FS muon, needs total counts of muons in chain
            if 'noL1' in subChainDict["chainParts"]["extra"]:
                AllMuons=GetAllMuonThresholds(chainDict)
            else:
                AllMuons=[]

            Muon = L2EFChain_mu(subChainDict, asymDiMuonChain, AllMuons)

        listOfChainDefs += [Muon.generateHLTChainDef()]

    if len(listOfChainDefs)>1:
        if ('mergingStrategy' in chainDict.keys()):        
            theChainDef = mergeChainDefs(listOfChainDefs,chainDict["mergingStrategy"],chainDict["mergingOffset"])
        else:
            theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    #if chainDict["topo"]:
    #    theChainDef = _addTopoInfo(theChainDef,chainDict)

    return theChainDef


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
    
    if "Jpsimumu" in chainDict["topo"]:
        # to be adapted!!!!!!!!!!!!!!!!!
        from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

        L2Fex = TrigL2DielectronMassFex_Jpsiee()
        L2Hypo = TrigL2DielectronMassHypo_Jpsiee()        

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()


        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    return theChainDef


