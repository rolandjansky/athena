# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Photon Slice single electron signatures"

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.egamma.generatePhotonChainDefs' )

from TriggerMenu.egamma.PhotonDef import L2EFChain_g
from TriggerMenu.menu.MenuUtils import splitChainDict, mergeChainDefs

Photons = []

from TriggerMenu.egamma.EgammaDef import EgammaSequence
##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        photon_seq = EgammaSequence(subChainDict)
        log.debug('Egamma Sequence: %s', photon_seq)
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
    
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    EFChainName = "EF_" + chainDict['chainName']
    
    if 'dPhi15' in chainDict['topo']:
        EFChainName = 'EF_2g_OvlRem_dPhi15'
        from TrigGenericAlgs.TrigGenericAlgsConfig import OverlapRemovalConfig
        OverlapRemoval_algo = OverlapRemovalConfig('OvlRem', MinPhiDist = 1.5, MinEtaDist = 0)
    
        theChainDef.addSequence([OverlapRemoval_algo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    elif "Jpsiee" in chainDict["topo"]:

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()
        
        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    elif "Zeg" in chainDict["topo"]:

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zeg, TrigEFDielectronMassHypo_Zeg

        EFFex = TrigEFDielectronMassFex_Zeg()
        EFHypo = TrigEFDielectronMassHypo_Zeg()
        
        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    #elif "Heg" in chainDict["topo"]:

    #    from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Heg, TrigEFDielectronMassHypo_Heg

    #    EFFex = TrigEFDielectronMassFex_Heg()
    #    EFHypo = TrigEFDielectronMassHypo_Heg()
        
    #    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    #    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    else:
        pass 

    return theChainDef




