# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Electron Slice single electron signatures"

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.egamma.generateElectronChainDefs' )
log.info("Importing %s",__name__)

from TriggerMenu.egamma.ElectronDef import L2EFChain_e as L2EFChain_e

#from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import splitChainDict, mergeChainDefs, setupTopoStartFrom
Electrons = []

from TriggerMenu.egamma.EgammaDef import EgammaSequence
##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        electron_seq = EgammaSequence(subChainDict)
        log.debug('Egamma Sequence: %s', electron_seq)
        Electron = L2EFChain_e(subChainDict,electron_seq)
            
        listOfChainDefs += [Electron.generateHLTChainDef()]
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

    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None

    if "Jpsiee" in chainDict["topo"]:
        if topoStartFrom:
            EFChainName = EFChainName+'_tsf'

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName, topo_start_from = topoStartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    elif "Zeg" in chainDict["topo"]:
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zeg, TrigEFDielectronMassHypo_Zeg

        EFFex = TrigEFDielectronMassFex_Zeg()
        EFHypo = TrigEFDielectronMassHypo_Zeg()

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    elif "Zee" in chainDict["topo"]:

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zee, TrigEFDielectronMassHypo_ZeeTight

        if 'etcut' in chainDict['chainName']:
            from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFexElectronCluster_Zee, TrigEFDielectronMassHypoElectronCluster_Zee
            EFFex = TrigEFDielectronMassFexElectronCluster_Zee()
            EFHypo = TrigEFDielectronMassHypoElectronCluster_Zee()
        else:
            EFFex = TrigEFDielectronMassFex_Zee()
            EFHypo = TrigEFDielectronMassHypo_ZeeTight()

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

        if 'etcut' in chainDict['chainName']:
             from TrigIDTPMonitor.TrigIDTPMonitorConfig import IDTPMonitorElectron                                                               
             IDTP = IDTPMonitorElectron()
             myInputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
             theChainDef.addSequence([IDTP],myInputTEsEF,EFChainName+"_monit")
             theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1,[EFChainName+"_monit"])
    
    
    else:
        pass 
    
    return theChainDef





