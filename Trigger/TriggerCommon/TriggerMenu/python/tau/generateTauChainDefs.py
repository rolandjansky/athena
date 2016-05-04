# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Tau Slice"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from AthenaCommon.SystemOfUnits import mm

from TriggerMenu.tau.TauDef import L2EFChain_tau as L2EFChain_tau
try:
    from TriggerMenu.tau.TauDefIdTest import L2EFChain_tau as L2EFChain_tau_IdTest
except:
    log.error('generateTauChainDefs: Problems when importing TauDefIdTest.')
    import traceback
    log.info(traceback.print_exc())

from TriggerMenu.menu.MenuUtils import splitChainDict, mergeChainDefs

# Try new HypoProvider
from TriggerMenu.tau.TauHypoProvider import TauHypoProvider


##########################################################################################
##########################################################################################


def generateChainDefs(chainDict):          

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    myProvider = TauHypoProvider()

    

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Tau = L2EFChain_tau_IdTest(subChainDict, myProvider)
        else:
            Tau = L2EFChain_tau(subChainDict, myProvider)
 
        listOfChainDefs += [Tau.generateHLTChainDef()]

    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]
        
    #add support for topological algorithms
    if chainDict["topo"]:
        log.info("Adding Tau Topological Algorithm(s) to chain")
        theChainDef = _addTopoInfo(theChainDef,chainDict)    
        
    return theChainDef

L2VertexDistances = {'L' : 3 * mm, 'M' : 2 * mm, 'T' : 1 * mm}

def _addTopoInfo(theChainDef,chainDict):
    if(chainDict["topo"][0][:-1] == 'ditau'):
        #find the last preselection step
        lastPreselectionStep = None
        i = 0
        for signature in theChainDef.signatureList:
            #All preselection steps should start with L2
            if signature['listOfTriggerElements'][0][:2] == "L2":
                lastPreselectionStep = i
            i = i + 1
        
        from TrigTauHypo.TrigTauHypoConf import HLTVertexCombo
        from TrigTauHypo.TrigTauHypoConf import HLTVertexPreSelHypo
        theHLTVertexCombo = HLTVertexCombo()
        theHLTVertexPreSelHypo = HLTVertexPreSelHypo(name = "HLTVertexPreSelHypo_" + chainDict["topo"][0])
        
        #set the allowable distance in z between vertices
        theHLTVertexPreSelHypo.acceptableZ0Distance = L2VertexDistances[chainDict["topo"][0][-1]]
        
        
        preselectionTEName = "L2_" + chainDict['chainName']
        preselectionAlgorithms = [theHLTVertexCombo, theHLTVertexPreSelHypo]
        preselectionTEs = theChainDef.signatureList[lastPreselectionStep]['listOfTriggerElements']
        theChainDef.addSequence(preselectionAlgorithms, preselectionTEs, preselectionTEName, None)
        theChainDef.addSignatureL2([preselectionTEName])
        
        return theChainDef


