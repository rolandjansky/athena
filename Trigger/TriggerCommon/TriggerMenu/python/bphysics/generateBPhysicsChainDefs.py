# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Bphysics chains
###########################################################################
__author__  = 'M.Backes, C.Bernius, J.Walder'
__doc__="Definition of bphysics chains" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.MenuUtils import setupTopoStartFrom
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence

###########################################################################
###########################################################################

def generateChainDefs(chainDict):
    
    from TriggerMenu.muon.generateMuonChainDefs import generateChainDefs as genMuonChainDefs
    theChainDef =     genMuonChainDefs(chainDict)

    if not chainDict["topo"]:
        log.error( "No topo given -> not a bphysics chain...")
    else:
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

    topoAlgs = chainDict["topo"]

    SameConfigTopos = ['bJpsi', 'bDimu', 'bTau', 'bBmumu', 'bJpsimumu', 'bUpsimumu', 'Zmumu','bUpsi']
    ntopos = len(topoAlgs)

    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None
        
    if ('bBmumux' in topoAlgs) | ('bBmumuxv2' in topoAlgs) | ('bBmumuxv3' in topoAlgs):
        theChainDef = bBmumuxTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom)
    elif ('Trkloose' in topoAlgs):
        theChainDef = bMuTrack(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom)
    elif (ntopos ==1) & (topoAlgs[0] in SameConfigTopos):
        theChainDef = bSingleOptionTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom)
    else:
        theChainDef = bMultipleOptionTopos(theChainDef,chainDict,inputTEsL2, inputTEsEF, topoStartFrom)

    return theChainDef


###################################################################################
###################################################################################

def bSingleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom):
    topoAlgs = chainDict["topo"]
    mtopo = topoAlgs[0]
    # TE naming deifnition
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    L2TEname = "L2_" + TEname+'_'+mtopo+'_'+chainDict['L1item']
    EFTEname = "EF_" + chainDict['chainName']
    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+'_'+mtopo+'_tsf_'+chainDict['L1item']
        topo2StartFrom = L2TEname

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
        import re
        if ('3mu' in chainDict['chainName'] or re.search('2mu[0-9]+_mu[0-9]+',chainDict['chainName']) or re.search('mu[0-9]+_2mu[0-9]+',chainDict['chainName'])):
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
            from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
            from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
            from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
            L2Fex  = L2BMuMuFex_DiMu()
            L2Hypo = L2BMuMuHypo_DiMu()
            EFFex  = EFBMuMuFex_DiMu()
            EFHypo = EFBMuMuHypo_DiMu()

    elif (mtopo == 'bTau'):
        chainParts = chainDict['chainParts']
        mult = 0
        for part in chainParts :
            mult = mult + int(part['multiplicity'])

        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Tau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Tau2

        if ('nscan' in chainDict['chainName'] ): # do not do L2
            L2Fex = None
            L2Hypo = None
            if mult == 2 :
                EFFex = EFMultiMuFex_Tau2()
                EFHypo = EFMultiMuHypo_Tau2()            
            else :
                EFFex = EFMultiMuFex_Tau()
                EFHypo = EFMultiMuHypo_Tau()
        else :
            L2Fex = None
            L2Hypo = None
            if mult == 2 :
                EFFex = EFMultiMuFex_Tau2()
                EFHypo = EFMultiMuHypo_Tau2()
            else :
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
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Z
        from TrigBphysHypo.TrigEFBMuMuFexConfig import  EFBMuMuFex_Z
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Z
        L2Fex  = L2BMuMuFex_noId_Z() 
        L2Hypo = L2BMuMuHypo_Z()
        EFFex  = EFBMuMuFex_Z()
        EFHypo = EFBMuMuHypo_Z()

    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], mtopo ))
   
    if  L2Fex != None :
        theChainDef.addSequence([L2Fex, L2Hypo], inputTEsL2, L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF, EFTEname, topo_start_from=topo2StartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])    

    if 'idperf' in chainDict['chainName']:
      from TrigIDTPMonitor.TrigIDTPMonitorConfig import IDTPMonitor                                                                          
      IDTP = IDTPMonitor()
      myInputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
      theChainDef.addSequence([IDTP],myInputTEsEF,EFTEname+"_monit")
      theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1,[EFTEname+"_monit"])

    return theChainDef


###################################################################################
###################################################################################
def bMultipleOptionTopos(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoStartFrom):

    topoAlgs = chainDict["topo"]
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    
    myTopoString = ''
    for mtopo in topoAlgs:
        myTopoString =myTopoString+'_'+mtopo
    L2TEname = "L2_" + TEname+myTopoString+'_'+chainDict['L1item']
    EFTEname = "EF_" + chainDict['chainName']

    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+myTopoString+'_tsf_'+chainDict['L1item']
        topo2StartFrom = L2TEname



    if ('7invm9' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_7invm9
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_7invm9_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_7invm9
        L2Fex  = L2BMuMuFex_DiMu_7invm9_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_7invm9()
        EFFex  = EFBMuMuFex_DiMu_7invm9_noOS()
        EFHypo = EFBMuMuHypo_DiMu_7invm9()
    elif ('7invm9' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
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
    elif ('11invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm60
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm60
        L2Fex  = L2BMuMuFex_DiMu_11invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_11invm60()
        EFFex  = EFBMuMuFex_DiMu_11invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm60()                   
    elif ('11invm24' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm24
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm24
        L2Fex  = L2BMuMuFex_DiMu_11invm24_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_11invm24()
        EFFex  = EFBMuMuFex_DiMu_11invm24_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm24()
    elif ('24invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' not in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_24invm60
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_24invm60
        L2Fex  = L2BMuMuFex_DiMu_24invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_24invm60()
        EFFex  = EFBMuMuFex_DiMu_24invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_24invm60()
    elif ('11invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm60_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_11invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_11invm60_noVtx()
        EFFex  = EFBMuMuFex_DiMu_11invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm60_noVtx()                   
    elif ('11invm24' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_11invm24_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_11invm24_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_11invm24_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_11invm24_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_11invm24_noVtx()
        EFFex  = EFBMuMuFex_DiMu_11invm24_noOS()
        EFHypo = EFBMuMuHypo_DiMu_11invm24_noVtx()
    elif ('24invm60' in topoAlgs) & ('noos' in topoAlgs) & ('novtx' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_24invm60_noVtx           
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_24invm60_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_24invm60_noVtx           
        L2Fex  = L2BMuMuFex_DiMu_24invm60_noOS()
        L2Hypo = L2BMuMuHypo_DiMu_24invm60_noVtx()
        EFFex  = EFBMuMuFex_DiMu_24invm60_noOS()
        EFHypo = EFBMuMuHypo_DiMu_24invm60_noVtx()
   
    elif ('bTau' in topoAlgs) :
        
        chainParts = chainDict['chainParts']
        mult = 0
        for part in chainParts :
            mult = mult + int(part['multiplicity'])
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Tau
        from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_Tau2
        from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_Tau2

        if ('nscan' in chainDict['chainName'] or  'noL2' in topoAlgs ): # do not do L2
            L2Fex = None
            L2Hypo = None
            if mult ==  2 :
                EFFex = EFMultiMuFex_Tau2()
                EFHypo = EFMultiMuHypo_Tau2()
            else :
                EFFex = EFMultiMuFex_Tau()
                EFHypo = EFMultiMuHypo_Tau()
        else :
            L2Fex = None
            L2Hypo = None
            if mult ==  2 :
                EFFex = EFMultiMuFex_Tau2()
                EFHypo = EFMultiMuHypo_Tau2()
            else :
                EFFex = EFMultiMuFex_Tau()
                EFHypo = EFMultiMuHypo_Tau()

     
    elif ('bDimu' in topoAlgs) & ('novtx' in topoAlgs) & ('noos' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noVtx
        L2Fex  = L2BMuMuFex_DiMu_passL2() 
        L2Hypo = L2BMuMuHypo_DiMu_passL2()
        EFFex  = EFBMuMuFex_DiMu_noOS()
        EFHypo = EFBMuMuHypo_DiMu_noVtx()        
    elif ('bDimu' in topoAlgs) & ('novtx' in topoAlgs) & ('noos' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_noVtx
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noOS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noVtx
        L2Fex  = L2BMuMuFex_DiMu_noOS() 
        L2Hypo = L2BMuMuHypo_DiMu_noVtx()
        EFFex  = EFBMuMuFex_DiMu_noOS()
        EFHypo = EFBMuMuHypo_DiMu_noVtx()        
    elif ('bDimu' in topoAlgs) & ('noinvm' in topoAlgs) & ('novtx' in topoAlgs) & ('ss' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_noinvm_SS
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_noinvm_noVtx
        from TrigBphysHypo.TrigEFBMuMuFexConfig  import EFBMuMuFex_DiMu_noinvm_SS
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_noinvm_noVtx
        L2Fex  = L2BMuMuFex_DiMu_noinvm_SS() 
        L2Hypo = L2BMuMuHypo_DiMu_noinvm_noVtx()
        EFFex  = EFBMuMuFex_DiMu_noinvm_SS()
        EFHypo = EFBMuMuHypo_DiMu_noinvm_noVtx()	
    elif ('bJpsimumu' in topoAlgs) & ('noid' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_noId
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_noId
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_noId
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        L2Fex  = L2BMuMuFex_noId()
        L2Hypo = L2BMuMuHypo_Jpsi_noId()
        EFFex  = EFBMuMuFex_noId()
        EFHypo = EFBMuMuHypo_Jpsi()
    elif ('bDimu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu
        L2Fex  = L2BMuMuFex_DiMu_passL2() 
        L2Hypo = L2BMuMuHypo_DiMu_passL2()
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu() 
        if ('nscan' in chainDict['chainName'] ): # do not do L2 at all; expect one TE
            L2Fex = None
            L2Hypo = None
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu_oneTE
            EFFex  = EFBMuMuFex_DiMu_oneTE()
    elif ('bJpsimumu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        L2Fex  = L2BMuMuFex_Jpsi_passL2()
        L2Hypo = L2BMuMuHypo_Jpsi_passL2()
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi() 
        if ('nscan' in chainDict['chainName'] ): # do not do L2 at all; expect one TE
            L2Fex = None
            L2Hypo = None
            from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi_oneTE
            EFFex  = EFBMuMuFex_Jpsi_oneTE()
    elif ('bBmumu' in topoAlgs) & ('noL2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B
        L2Fex  = L2BMuMuFex_DiMu_passL2()
        L2Hypo = L2BMuMuHypo_DiMu_passL2()
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B() 
    elif ('bDimu' in topoAlgs) & ('noEFbph' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig  import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_passEF
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_passEF
        L2Fex  = L2BMuMuFex_DiMu() 
        L2Hypo = L2BMuMuHypo_DiMu()
        EFFex  = EFBMuMuFex_passEF()
        EFHypo = EFBMuMuHypo_passEF() 
    elif ('bJpsimumu' in topoAlgs) & ('noEFbph' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_passEF
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_passEF
        L2Fex  = L2BMuMuFex_Jpsi()
        L2Hypo = L2BMuMuHypo_Jpsi()
        EFFex  = EFBMuMuFex_passEF()
        EFHypo = EFBMuMuHypo_passEF() 
    # Tight Chi2 variants
    elif ('bJpsimumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi_tightChi2
        L2Fex  = L2BMuMuFex_Jpsi()
        L2Hypo = L2BMuMuHypo_Jpsi()
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi_tightChi2() 
    elif ('bDimu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_tightChi2
        L2Fex  = L2BMuMuFex_DiMu()
        L2Hypo = L2BMuMuHypo_DiMu()
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu_tightChi2() 
    elif ('bBmumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B_tightChi2
        L2Fex  = L2BMuMuFex_B()
        L2Hypo = L2BMuMuHypo_B()
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B_tightChi2() 
    elif ('bUpsimumu' in topoAlgs) & ('tightChi2' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Upsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Upsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Upsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Upsi_tightChi2
        L2Fex  = L2BMuMuFex_Upsi()
        L2Hypo = L2BMuMuHypo_Upsi()
        EFFex  = EFBMuMuFex_Upsi()
        EFHypo = EFBMuMuHypo_Upsi_tightChi2() 
    # Lxy > 0 variants
    elif ('bJpsimumu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi_Lxy0
        L2Fex  = L2BMuMuFex_Jpsi()
        L2Hypo = L2BMuMuHypo_Jpsi()
        EFFex  = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi_Lxy0() 
    elif ('bDimu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu_Lxy0
        L2Fex  = L2BMuMuFex_DiMu()
        L2Hypo = L2BMuMuHypo_DiMu()
        EFFex  = EFBMuMuFex_DiMu()
        EFHypo = EFBMuMuHypo_DiMu_Lxy0() 
    elif ('bBmumu' in topoAlgs) & ('Lxy0' in topoAlgs):
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_B
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_B
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_B
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_B_Lxy0
        L2Fex  = L2BMuMuFex_B()
        L2Hypo = L2BMuMuHypo_B()
        EFFex  = EFBMuMuFex_B()
        EFHypo = EFBMuMuHypo_B_Lxy0() 
    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], topoAlgs ))

    if L2Fex != None :
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])
 
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF, EFTEname, topo_start_from = topo2StartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])       

    return theChainDef


###################################################################################
###################################################################################
def bMuTrack(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom):

    EFChainName = "EF_" + chainDict['chainName']
    topoAlgs = chainDict["topo"]

    [trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()


    if ('bJpsi' in topoAlgs) & ('Trkloose' in topoAlgs):
        from TrigBphysHypo.TrigEFTrkMassFexConfig  import EFTrkMassFex_Jpsimumu_loose
        from TrigBphysHypo.TrigEFTrkMassHypoConfig import EFTrkMassHypo_Jpsimumu_loose        
        EFFex  = EFTrkMassFex_Jpsimumu_loose()
        EFHypo = EFTrkMassHypo_Jpsimumu_loose()

    else:
        log.error('Bphysics Chain %s can not be constructed, the given topo algs are not known: %s  ' %(chainDict['chainName'], topoAlgs ))

    # Commented out section for L2 and EF sequence - not working with current FTF and prec tracking
    #    L2TEcount = 0; L2outTEs = []
    #    for L2inputTE in inputTEsL2:
    #        L2TEcount = L2TEcount+1
    #        L2outputTE = L2inputTE+'_idl2fast_'+str(L2TEcount)
    #        L2outTEs.append(L2outputTE)
    #        #theChainDef.addSequence([theTrigIdscan],L2inputTE, L2outputTE)            
    #        theChainDef.addSequence(trkfast,L2inputTE, L2outputTE)            
    #    theChainDef.addSignatureL2(L2outTEs)
    #        
    #    theChainDef.addSequence( [L2Fex,  L2Hypo], L2outTEs, L2ChainName)
    ##    theChainDef.addSequence( [L2Fex,  L2Hypo], inputTEsL2, L2ChainName)
    #    theChainDef.addSignatureL2([L2ChainName])
    #
    #
    #
    #    EFTEcount = 0; EFoutTEsprec = []; 
    #    for EFinputTE in inputTEsEF:
    #        EFTEcount = EFTEcount + 1        
    #        EFoutputTEprec = EFinputTE+'_idefprec_'+str(EFTEcount)
    #        EFoutTEsprec.append(EFoutputTEprec)
    #        theChainDef.addSequence(trkprec,EFinputTE, EFoutputTEprec)
    #    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)
    #
    #    theChainDef.addSequence([EFFex, EFHypo],EFoutTEsprec,EFChainName)
    #    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])       


    # Sequence where only the EF mu+track stage is run. FTF+prec tracking run first
    #------- EF Sequences -------
    EFTEcount = 0; EFoutTEsfast = []; 
    for EFinputTE in inputTEsEF:
        EFTEcount = EFTEcount + 1
        EFoutputTEfast = EFinputTE+'_idfast_'+str(EFTEcount)
        EFoutTEsfast.append(EFoutputTEfast)
        theChainDef.addSequence(trkfast,EFinputTE, EFoutputTEfast)

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsfast)
    
    EFTEcount = 0; EFoutTEsprec = []; 
    for EFinputTE in inputTEsEF:
        EFTEcount = EFTEcount + 1        
        EFinputTEprec  = EFinputTE+'_idfast_'+str(EFTEcount)
        EFoutputTEprec = EFinputTE+'_idprec_'+str(EFTEcount)
        EFoutTEsprec.append(EFoutputTEprec)
        theChainDef.addSequence(trkprec,EFinputTEprec, EFoutputTEprec)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)

    theChainDef.addSequence([EFFex, EFHypo], EFoutTEsprec, EFChainName, topo_start_from = topoStartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])


    return theChainDef



###################################################################################
###################################################################################
def bBmumuxTopos(theChainDef,chainDict, inputTEsL2, inputTEsEF, topoStartFrom):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    topoAlgs = chainDict["topo"]
    TEname = findL2teBaseName(chainDict['chainName'],topoAlgs)
    
    myTopoString = ''
    for mtopo in topoAlgs:
        myTopoString =myTopoString+'_'+mtopo
    L2TEname = "L2_" + TEname+myTopoString+'_'+chainDict['L1item']
    EFTEname = "EF_" + chainDict['chainName']

    topo2StartFrom = None
    if topoStartFrom:
        L2TEname = "L2_" + TEname+myTopoString+'_tsf_'+chainDict['L1item']
        topo2StartFrom = L2TEname

    # replace L2 Te (and EF tsf) with bBmumuxv2 naming
    # same underlying sequence is used
    # Warning if ever switch to v3 implementation -- this should be changed / tested 
    if 'BcmumuDsloose' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BcmumuDsloose","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
    if 'BpmumuKp' in topoAlgs:
        L2TEname = L2TEname.replace("bBmumux_BpmumuKp","bBmumuxv2")
        if topo2StartFrom:
            topo2StartFrom = L2TEname
        
    [trkfast, trkprec] = TrigInDetSequence("Bphysics", "bphysics", "IDTrig").getSequence()

#    from InDetTrigRecExample.EFInDetConfig import *
#    theTrigEFIDInsideOut = TrigEFIDInsideOut_Bphysics().getSequence()

    #JWW: Remove Bmumux L2 Fex and Hypo. Use a simple BMuMu Fex and Hypo instead
    #from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
    #L2Hypo = L2BMuMuXHypo_1()

    if 'BcmumuDsloose' in topoAlgs:
        #from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_BcMuMuDs
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BcMuMuDs
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BcMuMuDs
        #L2Fex = L2BMuMuXFex_BcMuMuDs()
        EFFex = EFBMuMuXFex_BcMuMuDs()
        EFHypo = EFBMuMuXHypo_BcMuMuDs()
        
    elif 'BpmumuKp' in topoAlgs:
        #from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_BplusMuMuKplus
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_BplusMuMuKplus
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_BplusMuMuKplus
        #L2Fex = L2BMuMuXFex_BplusMuMuKplus()
        EFFex = EFBMuMuXFex_BplusMuMuKplus()
        EFHypo = EFBMuMuXHypo_BplusMuMuKplus()
        
    elif 'bBmumuxv2' in topoAlgs:
        #from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_1
        #from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_1
        from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_1
        from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_1
        #L2Fex  = L2BMuMuFex_1()
        #L2Hypo = L2BMuMuHypo_1()
        EFFex  =  EFBMuMuXFex_1()
        EFHypo = EFBMuMuXHypo_1()
        
    elif 'bBmumuxv3' in topoAlgs:
        print 'MOOOO in bBmumuxv3'
        from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_EF
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_EF
        EFFex  =  L2BMuMuXFex_EF()
        EFHypo = L2BMuMuXHypo_EF()
        
    else:
        #from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
        from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_Jpsi
        from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_Jpsi
        #L2Fex = L2BMuMuXFex_1()
        EFFex = EFBMuMuFex_Jpsi()
        EFHypo = EFBMuMuHypo_Jpsi()
       

    # Use simple di-muon fex/hypo for L2
    # Note - may need to change oppsign and vtx requirements
    if 'bBmumuxv3' in topoAlgs:
        print 'MOOOO2 in bBmumuxv3'
        from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
        from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
        L2Fex = L2BMuMuXFex_1()
        L2Hypo = L2BMuMuXHypo_1()
    else:
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_1
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_1
        L2Fex  = L2BMuMuFex_1()
        L2Hypo = L2BMuMuHypo_1()
        
    # noL2 option to skip dimuon selection at L2
    if 'noL2' in topoAlgs:
        from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu_passL2
        from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu_passL2
        L2Fex  = L2BMuMuFex_DiMu_passL2()
        L2Hypo = L2BMuMuHypo_DiMu_passL2()

    if 'bBmumuxv3' in topoAlgs:
        L2TEcount = 0; L2outTEs = []
        for L2inputTE in inputTEsL2:
            L2TEcount = L2TEcount+1
            L2outputTE = L2inputTE+'_id_'+str(L2TEcount)
            L2outTEs.append(L2outputTE)
            theChainDef.addSequence(trkfast,L2inputTE, L2outputTE)
        theChainDef.addSignatureL2(L2outTEs)
        theChainDef.addSequence( [L2Fex,  L2Hypo], L2outTEs, L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])    
    #------- EF Sequences -------
        EFTEcount = 0; EFoutTEs = []
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            #EFoutputTE = EFinputTE+'_id_'+str(EFTEcount)
            #EFoutTEs.append(EFoutputTE)
            #theChainDef.addSequence(trkprec,L2outputTE, EFoutputTE)  
            EFoutputTE = "EF_bphysprec_"+L2outputTE+"_" + str(EFTEcount)
            EFoutTEs.append(EFoutputTE)
            theChainDef.addSequence(trkprec,L2outputTE, EFoutputTE)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEs)
        theChainDef.addSequence([EFFex, EFHypo], EFoutTEs, EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    else:
    #------- L2 Sequences -------
    # create the individual outputTEs together with the first sequences that are run
        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2TEname, topo_start_from = topoStartFrom)
        theChainDef.addSignatureL2([L2TEname])
    #------- EF Sequences -------
        EFTEcount = 0; EFoutTEsfast = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1
            EFoutputTEfast = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutTEsfast.append(EFoutputTEfast)
            theChainDef.addSequence(trkfast,EFinputTE, EFoutputTEfast)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsfast)
        EFTEcount = 0; EFoutTEsprec = []; 
        for EFinputTE in inputTEsEF:
            EFTEcount = EFTEcount + 1        
            EFinputTEprec  = EFinputTE+'_idfast_'+str(EFTEcount)
            EFoutputTEprec = EFinputTE+'_idprec_'+str(EFTEcount)
            EFoutTEsprec.append(EFoutputTEprec)
            theChainDef.addSequence(trkprec,EFinputTEprec, EFoutputTEprec)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, EFoutTEsprec)
        theChainDef.addSequence([EFFex, EFHypo], EFoutTEsprec, EFTEname, topo_start_from = topo2StartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])


    return theChainDef

    

###################################################################################
###################################################################################
def findL2teBaseName(cName,topos):
    indices = []
    for topo in topos:
        indices.append(cName.find(topo))

    firstTopoOccurance = sorted(indices,key=int)[0]
    
    return cName[:firstTopoOccurance-1]

###################################################################################
