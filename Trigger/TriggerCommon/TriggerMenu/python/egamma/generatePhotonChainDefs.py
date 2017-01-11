# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Photon Slice single electron signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.egamma.PhotonDef import *

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import *

Photons = []

from TriggerMenu.egamma.EgammaDef import EgammaSequence
##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        photon_seq = EgammaSequence(subChainDict)
        log.info('Egamma Sequence: %s', photon_seq)
        Photon = L2EFChain_g(subChainDict,photon_seq)
            
        listOfChainDefs += [Photon.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    if chainDict["topo"]:
        theChainDef = _addTopoInfo(theChainDef,chainDict)    

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
    
    if "Jpsiee" in chainDict["topo"]:

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi
        from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee

        L2Fex = TrigL2DielectronMassFex_Jpsiee()
        L2Hypo = TrigL2DielectronMassHypo_Jpsiee()        

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()
        
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    if "Zeg" in chainDict["topo"]:

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zeg, TrigEFDielectronMassHypo_Zeg
        from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Zeg, TrigL2DielectronMassHypo_Zeg

        L2Fex = TrigL2DielectronMassFex_Zeg()
        L2Hypo = TrigL2DielectronMassHypo_Zeg()        

        EFFex = TrigEFDielectronMassFex_Zeg()
        EFHypo = TrigEFDielectronMassHypo_Zeg()
        
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    

    return theChainDef




