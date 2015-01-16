# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'M.Backes, C.Bernius, J.Walder'
__doc__="Definition of bphysics chains as used in v4 menu" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logBphysDef = logging.getLogger("TriggerMenu.bphysics.BphysicsDef")

#from TriggerMenu.muon.MuonDefv3 import *
from TriggerMenu.menu.MenuUtils import *


#BPhysics = []

###########################################################################
###########################################################################

def generateChainDefs(chainDict):
    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    from TriggerMenu.muon.generateMuonChainDefs import generateChainDefs as genMuonChainDefs
    theChainDef =     genMuonChainDefs(chainDict)

    # for subChainDict in listOfChainDicts:      
    #     BPhysics = L2EFChain_mu(subChainDict)  
    #     listOfChainDefs += [BPhysics.generateHLTChainDef()]    
    # if len(listOfChainDefs)>1:
    #     theChainDef = mergeChainDefs(listOfChainDefs)
    # else:
    #     theChainDef = listOfChainDefs[0]
    
    if not chainDict["topo"]:
        logBphysDef.error( "No topo given -> not a bphysics chain...")
    else:
        logBphysDef.info("Adding Bphysics Topo...")
        theChainDef = _addTopoInfo(theChainDef,chainDict)    

    return theChainDef


###########################################################################
###########################################################################
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

    SameConfigTopos = ['bJpsi', 'bDimu', 'bTau', 'bBmumu', 'bJpsimumu', 'bUpsimumu', 'Zmumu','bUpsi']
    ntopos = len(topoAlgs)

    if ('bBmumux' in topoAlgs) | ('bBmumuxv2' in topoAlgs):
        theChainDef = bBmumuxTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF)
    elif (ntopos ==1) & (topoAlgs[0] in SameConfigTopos):
        theChainDef = bSingleOptionTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF)
    else:
        theChainDef = bMultipleOptionTopos(theChainDef,chainDict,inputTEsL2, inputTEsEF)


    return theChainDef


###################################################################################
###################################################################################

def bSingleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    topoAlgs = chainDict["topo"]
    mtopo = topoAlgs[0]

    if (mtopo == 'bJpsi'): 
        from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_Jpsi
        from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_Jpsi
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Jpsi
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Jpsi
        L2Fex = L2MultiMuFex_Jpsi()
        L2Hypo = L2MultiMuHypo_Jpsi()
        EFFex = EFMultiMuFex_Jpsi()
        EFHypo = EFMultiMuHypo_Jpsi()
        
    elif (mtopo =='bDimu'):
        ##print "JWW"
        ##print chainDict
        if ('3mu' in chainDict['chainName']):
            ## print "JWW: using mutlimuons for", chainDict['chainName']
            #This is a multi-muon chain
            from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_DiMu
            from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_DiMu
            from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_DiMu
            from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu
            L2Fex = L2MultiMuFex_DiMu()
            L2Hypo = L2MultiMuHypo_DiMu()        
            EFFex = EFMultiMuFex_DiMu()
            EFHypo = EFMultiMuHypo_DiMu()
        else:
            #This is a di-muon chain
            ##print "JWW: using di-muons for", chainDict['chainName']
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
            from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
            L2Fex  = L2BMuMuFex_DiMu()
            L2Hypo = L2BMuMuHypo_DiMu()
            EFFex  = EFBMuMuFex_DiMu()
            EFHypo = EFBMuMuHypo_DiMu()

    elif (mtopo == 'bTau'):
        from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_Tau
        from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_Tau
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Tau
        L2Fex = L2MultiMuFex_Tau()
        L2Hypo = L2MultiMuHypo_Tau()        
        EFFex = EFMultiMuFex_Tau()
        EFHypo = EFMultiMuHypo_Tau()
    
    elif (mtopo == 'bUpsi'):
        from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_Upsi
        from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_Upsi
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Upsi
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Upsi
        L2Fex = L2MultiMuFex_Upsi()
        L2Hypo = L2MultiMuHypo_Upsi()        
        EFFex = EFMultiMuFex_Upsi()
        EFHypo = EFMultiMuHypo_Upsi()
    
    elif (mtopo == 'bBmumu'):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B
        L2Fex  = L2BMuMuFex_B()
        L2Hypo = L2BMuMuHypo_B()
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B()
        
    elif (mtopo == 'bJpsimumu'):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        L2Fex  = L2BMuMuFex_Jpsi()
        L2Hypo = L2BMuMuHypo_Jpsi()
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi()

    elif (mtopo == 'bUpsimumu'):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Upsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Upsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Upsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Upsi
        L2Fex  = L2BMuMuFex_Upsi()
        L2Hypo = L2BMuMuHypo_Upsi()
        EFFex  = EFBMuMuFex_Upsi()
        EFHypo = EFBMuMuHypo_Upsi()

    elif (mtopo == 'Zmumu'):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import  L2BMuMuFex_noId_Z
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Z,L2BMuMuHypo_Z        
        from TrigBphysHypo.TrigEFBMuMuFexConfig import  EFBMuMuFex_Z
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Z
        L2Fex  = L2BMuMuFex_noId_Z() 
        L2Hypo = L2BMuMuHypo_Z()
        EFFex  = EFBMuMuFex_Z()
        EFHypo = EFBMuMuHypo_Z()

    else:
        logBphysDef.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], mtopo ))


    theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
    theChainDef.addSignatureL2([L2ChainName])
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    
    return theChainDef




###################################################################################
###################################################################################
def bMultipleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    topoAlgs = chainDict["topo"]

    if ('7invm9' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_7invm9_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_7invm9_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_7invm9_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_7invm9_noVtx()
        EFFex  = EFBMuMuFex_DiMu_7invm9_noOS()
        EFHypo = EFBMuMuHypo_DiMu_7invm9_noVtx()
    elif ('18invm60' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_18invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_18invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_18invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_18invm60_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_18invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_18invm60_noVtx()
        EFFex  = EFBMuMuFex_DiMu_18invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_18invm60_noVtx()
    elif ('11invm60' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm60_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_11invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_11invm60_noVtx()
        EFFex  = EFBMuMuFex_DiMu_11invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm60_noVtx()
        
        
     
    elif ('bDimu' in topoAlgs) & ('novtx' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_noVtx
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noVtx
        L2Fex  = L2BMuMuFex_DiMu_noOS() 
        L2Hypo = L2BMuMuHypo_DiMu_noVtx()
        EFFex  = EFBMuMuFex_DiMu_noOS()
        EFHypo = EFBMuMuHypo_DiMu_noVtx()
	

    elif ('bJpsimumu' in topoAlgs) & ('noid' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_noId
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_noId
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_noId
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        L2Fex  = L2BMuMuFex_noId()
        L2Hypo = L2BMuMuHypo_Jpsi_noId()
        EFFex  = EFBMuMuFex_noId()
        EFHypo = EFBMuMuHypo_Jpsi()
        
    elif ('bJpsi' in topoAlgs) & ('Trkloose' in topoAlgs):
        from TrigBphysHypo.TrigL2TrkMassFexConfig  import L2TrkMassFex_Jpsimumu_loose
        from TrigBphysHypo.TrigL2TrkMassHypoConfig import L2TrkMassHypo_Jpsimumu_loose
        from TrigBphysHypo.TrigEFTrkMassFexConfig  import EFTrkMassFex_Jpsimumu_loose
        from TrigBphysHypo.TrigEFTrkMassHypoConfig import EFTrkMassHypo_Jpsimumu_loose        
        L2Fex  = L2TrkMassFex_Jpsimumu_loose()
        L2Hypo = L2TrkMassHypo_Jpsimumu_loose()
        EFFex  = EFTrkMassFex_Jpsimumu_loose()
        EFHypo = EFTrkMassHypo_Jpsimumu_loose()

    else:
        logBphysDef.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], topoAlgs ))


    theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
    theChainDef.addSignatureL2([L2ChainName])
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])       
    return theChainDef







###################################################################################
###################################################################################
def bBmumuxTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    topoAlgs = chainDict["topo"]
    
    from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Bphysics
    theTrigIdscan = TrigIDSCAN_Bphysics()
    
    from InDetTrigRecExample.EFInDetConfig import *
    theTrigEFIDInsideOut = TrigEFIDInsideOut_Bphysics().getSequence()
    
    from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
    L2Hypo = L2BMuMuXHypo_1()

    if 'BcmumuDsloose' in topoAlgs:
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_BcMuMuDs
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BcMuMuDs
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BcMuMuDs
        L2Fex = L2BMuMuXFex_BcMuMuDs()
        EFFex = EFBMuMuXFex_BcMuMuDs()
        EFHypo = EFBMuMuXHypo_BcMuMuDs()
        
    elif 'bBmumuxv2' in topoAlgs:
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_1
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_1
        L2Fex = L2BMuMuXFex_1()
        EFFex  =  EFBMuMuXFex_1()
        EFHypo = EFBMuMuXHypo_1()
        
    else:
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        L2Fex = L2BMuMuXFex_1()
        EFFex = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi()
        
    #------- L2 Sequences -------
    # create the individual outputTEs together with the first sequences that are run
    L2TEcount = 0; L2outTEs = []
    for L2inputTE in inputTEsL2:
        L2TEcount = L2TEcount+1
        L2outputTE = L2inputTE+'_id_'+str(L2TEcount)
        L2outTEs.append(L2outputTE)
        theChainDef.addSequence([theTrigIdscan],L2inputTE, L2outputTE)            

    theChainDef.addSignatureL2(L2outTEs)
        
    theChainDef.addSequence( [L2Fex,  L2Hypo], L2outTEs, L2ChainName)
    theChainDef.addSignatureL2([L2ChainName])
    
    #------- EF Sequences -------
    EFTEcount = 0; EFoutTEs = []
    for EFinputTE in inputTEsEF:
        EFTEcount = EFTEcount + 1
        EFoutputTE = EFinputTE+'_id_'+str(EFTEcount)
        EFoutTEs.append(EFoutputTE)
        ##print 'MOOOO 1 inputTE', EFinputTE
        ##print 'MOOOO 1outputTE', EFoutputTE           
        theChainDef.addSequence(theTrigEFIDInsideOut,EFinputTE, EFoutputTE)  
    ##print 'MOOOO 2 outTEs', EFoutTEs          
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEs)

    theChainDef.addSequence([EFFex, EFHypo], EFoutTEs, EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    return theChainDef
    
