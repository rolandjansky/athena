# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Tau Slice"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.MenuUtils import setupTopoStartFrom
from AthenaCommon.SystemOfUnits import mm

from TriggerMenu.tau.TauDef import L2EFChain_tau as L2EFChain_tau

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


def _addTopoInfo(theChainDef,chainDict,doAtL2AndEF=False):
    
    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)

    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 

    L2ChainName = "L2_" + chainDict['chainName']

    topoThresh = chainDict['topoThreshold']
    topoStartFrom = None # setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None

    topoAlgs = chainDict["topo"]

    if(('tautsf' in topoAlgs) or ('notautsf' in topoAlgs)):
        #log.info("Adding Tau tsf Topological Algorithm(s) to chain")
        #topoName = 'tsf'
        #if topoStartFrom:
        #    L2ChainName = L2ChainName+topoName

        #import topo hypos at L2 
        from TrigTauHypo.TrigTauHypoConf import L2TauTopoFex 
        from TrigTauHypo.TrigTauHypoConfig2012 import L2TauTopoHypo

        L2Fex = L2TauTopoFex()
        hypo_name = 'l2topo_'+L2ChainName
        theL2Hypo = L2TauTopoHypo(hypo_name)

        theChainDef.addSequence([L2Fex, theL2Hypo],inputTEsL2,L2ChainName, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2ChainName])

    if( ('ditauL' in topoAlgs) or ('ditauM' in topoAlgs) or ('ditauT' in topoAlgs) ):
        
        ditau_hypo = 'ditauL'
        if('ditauM' in topoAlgs):
           ditau_hypo = 'ditauM'
        if('ditauT' in topoAlgs):
           ditau_hypo = 'ditauT'

        from TrigTauHypo.TrigTauHypoConfig2012 import HLTVertexPreSelHypo
        from TrigTauHypo.TrigTauHypoConf import HLTVertexCombo

        theHLTVertexCombo = HLTVertexCombo()
        hypo_name = 'l2topo_tau_'+ditau_hypo
        theVars = ['acceptableZ0Distance']
        theThresh = L2VertexDistances[(ditau_hypo)]
        theHLTVertexPreSelHypo = HLTVertexPreSelHypo(hypo_name,theVars,theThresh)        
        
        preselectionTEName = "L2_" + chainDict['chainName']+'_ditau'
        preselectionAlgorithms = [theHLTVertexCombo, theHLTVertexPreSelHypo] 
        theChainDef.addSequence(preselectionAlgorithms, inputTEsL2, preselectionTEName, None)
        theChainDef.addSignatureL2([preselectionTEName])


    if any("dR" in alg for alg in topoAlgs):
       inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

       maxdR=-1
       mindR=-1
       for topo_item in chainDict['topo']:
           if 'dR' in topo_item:
              mindR=float(topo_item.split('dR')[0])
              maxdR=float(topo_item.split('dR')[1])
       log.debug("dR cuts at: %d and %d", mindR, maxdR)
       if mindR==-1 or maxdR==-1:
          log.error("No dR chain part found in tau-tau dR Topo cut")

       from TrigTauHypo.TrigTauHypoConfig2012 import EFTauTopoHypo
       from TrigTauHypo.TrigTauHypoConf       import EFTauTopoFex

       EFFex  =  EFTauTopoFex()
       theVars    = ['DRMin','DRMax']
       theThresh  = [mindR*0.1,maxdR*0.1]
       TauTaudR_Hypo = EFTauTopoHypo("EFTauTopo_"+str(mindR).replace(".","")+"dR"+str(maxdR).replace(".",""),
                                           theVars, theThresh)
       log.debug("Input TEs to dR algorithm: %s", inputTEsEF)

       EFChainName = "EF_" + chainDict['chainName']

       theChainDef.addSequence([EFFex, TauTaudR_Hypo],inputTEsEF,EFChainName)
       theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])         
        
    return theChainDef



L2VertexDistances = {
      ('ditauL') : [3 * mm], 
      ('ditauM') : [2 * mm], 
      ('ditauT') : [1 * mm]
}


