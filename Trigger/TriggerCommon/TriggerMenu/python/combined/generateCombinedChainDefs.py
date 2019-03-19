# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Combined Chains, used to implement TOPOS
###########################################################################
__author__  = 'E. Pianori'
__doc__="Definition of topological cuts for combined chains"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

import re

from copy import deepcopy

###########################################################################
###########################################################################

def _addTopoInfo(theChainDef,chainDicts,listOfChainDefs,doAtL2AndEF=True): 

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
            
    topoAlgs = chainDicts[0]["topo"]

    if any("Heg" in alg for alg in topoAlgs):
        hegInputTEsEF=[]
        # First TE: electron merged
        for cD in listOfChainDefs:
            if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_e' in x]:
                hegInputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                break

        # Second TE: electron
        for cD in listOfChainDefs:
            if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_g' in x]:
                hegInputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                break

        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Heg, TrigEFDielectronMassHypo_Heg

        EFFex = TrigEFDielectronMassFex_Heg()
        EFHypo = TrigEFDielectronMassHypo_Heg()
        
        EFChainName = "EF_" + chainDicts[0]['chainName']
        if (len(EFChainName) > 99):
            EFChainName = EFChainName[:-(len(EFChainName)-99)]
        theChainDef.addSequence([EFFex, EFHypo],hegInputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
   
    if any("dphi" in alg for alg in topoAlgs):
        ##Check that we only have a MET and JET chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'MET' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            log.warning("Need a Met and a JET chain to run DPhi Topo cut")        
        else:
            theChainDef=_addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs)

    if any("mt" in alg for alg in topoAlgs):
        ##Check that we only have a MET and Electron chain
        inputChains=[]
        for ChainPart in chainDicts: 
            if 'MET' in ChainPart['signature'] or 'XS' in ChainPart['signature'] or 'Electron' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2: 
            log.warning("Need a MET/XS and an Electron chain to run MT cut")
        else:
            theChainDef=_addTransverseMass(theChainDef,chainDicts,listOfChainDefs)
            
    if any("razor" in alg for alg in topoAlgs):
        ##Check that we only have a MET and JET chain
        inputChains=[]
        for ChainPart in chainDicts: 
            if 'MET' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2: 
            log.warning("Need a Met and a JET chain to run Razor cut")
        else:
            theChainDef=_addRazor(theChainDef,chainDicts,listOfChainDefs)
            
    elif any("taumass" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Muon chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Photon' in ChainPart['signature']  or 'Muon' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            log.warning("Need a Photon and a Muon chain to run TauMass Topo cut")        
        else:
            theChainDef=_addTauMass(theChainDef,chainDicts,listOfChainDefs)   

    elif any("bXemu" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Muon chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Electron' in ChainPart['signature']  or 'Muon' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            log.warning("Need a Electron and a Muon chain to run bXemu Topo algo")        
        else:
            theChainDef=_addXemu(theChainDef,chainDicts,listOfChainDefs)   

    elif any("dr" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Muon chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Muon' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            log.warning("Need a Jet and a Muon chain to run the matching algorithm")        
        else:
            theChainDef=_addMatching(theChainDef,chainDicts,listOfChainDefs)   

    elif any("mVis" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Tau chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Photon' in ChainPart['signature']  or 'Tau' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            log.warning("Need a Photon and a Tau chain to run the matching algorithm")
        else:
            theChainDef=_addMVis(theChainDef,chainDicts,listOfChainDefs)

    elif any("dRtt" in alg for alg in topoAlgs):
        # tau-lep/tau chains with dRtt cut
        inputChains=[]
        for ChainPart in chainDicts:
           if 'Electron' in ChainPart['signature'] or 'Muon' in ChainPart['signature'] or 'Tau' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)==0 or len(inputChains)>2:
            log.warning("dRtt: Need Electron or Muon and a Tau chain to run the matching algorithm")
        else:
            log.debug("Configuring dRtt combo hypo")
            theChainDef=_adddR(theChainDef,chainDicts,listOfChainDefs)

    # elif any("deta" in alg for alg in topoAlgs):
    #     ##Check that we only have a Jet and Muon chain
    #     inputChains=[]
    #     for ChainPart in chainDicts:
    #         if 'Jet' in ChainPart['signature']:
    #             inputChains.append(ChainPart['signature'])
    #     if len(inputChains)<2:
    #         log.warning("Need a Photon and a Muon chain to run TauMass Topo cut")        
    #     else:
    #         theChainDef=_addDEtaJetJet(theChainDef,chainDicts,listOfChainDefs)   

    #else:
    #    log.error("do nothing, intra-signature topological cut not implemented yet")

        
    return theChainDef



##############################################################################
#Helper

def isJetTE(myString):
#    print "Nils:Executing isJetTE: "+myString
    if re.match("EF_[_0-9]+_HLThypo",myString) or re.match("EF_[_0-9]+_jetrec",myString) or ('HLThypo' in myString and re.match("EF_[_0-9]+",myString)) :
#        print "BETTA Pass :",myString
        return True
    else:
#        print "BETTA Fail :",myString
        return False

##############################################################################
def _addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs): 

    maxJets=-1
    DPhiCut=-1
    for topo_item in chainDicts[0]['topo']:
        if 'dphi' in topo_item:
            maxJets=int(topo_item.split('dphi')[0])
            DPhiCut=float(topo_item.split('dphi')[1])/10.

    if DPhiCut==-1 or maxJets==-1:
        log.error("No dphi chain part found in DPhiMetJet Topo cut")

    JetThr=-1
    for ChainPart in chainDicts:
        if 'Jet' in ChainPart['signature']:
            JetThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if JetThr==-1:
        log.error("No JET chain part found in DPhiMetJet Topo cut")

    from TrigHLTJetHypo.TrigEFDPhiMetJetAllTEConfig import EFDPhiMetJet_Generic

    DPhiMetJet_Hypo = EFDPhiMetJet_Generic("EFDPhiMetJet_J"+str(JetThr).replace(".","")+"_"+str(maxJets)+"DPhi"+str(DPhiCut).replace(".",""),
                                           dPhiCut=DPhiCut, minJetEt=JetThr*1000,maxDPhiJets=maxJets)

    ##Get only the last MET TE
    inputTEsEF = [] 

    # First TE: MET
    for cD in listOfChainDefs:
        if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_xe' in x]:
            inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
            break

    if inputTEsEF == []:
        for cD in listOfChainDefs:
            if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_xs' in x]:
                inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                break

    # Second TE: jets
    for cD in listOfChainDefs:
        if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if isJetTE(x)]:
            #print "BETTA: found ", x
            inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])]
            print "Added the following to inputTEsEF:"
            print cD.signatureList[-1]['listOfTriggerElements']
            print ""
            break


    #for cD in listOfChainDefs: 
    #    inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
 		         
    #inputTEsEF.reverse() # need first met then jet input TE           

    log.debug("Input TEs to DPhi algorithm: %s", inputTEsEF)

    EFChainName = "EF_dphi_" + chainDicts[0]['chainName']
    if (len(EFChainName) > 99):
        EFChainName = EFChainName[:-(len(EFChainName)-99)]
    #if '_L1' in EFChainName:
        #EFChainName = EFChainName.split("_L1")[0]
    
    theChainDef.addSequence([DPhiMetJet_Hypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef


##############################################################################
def _addTransverseMass(theChainDef,chainDicts,listOfChainDefs): 

    MTCut=-1
    for topo_item in chainDicts[0]['topo']:
        if 'mt' in topo_item:
            MTCut=int(topo_item.split('mt')[1])

    if MTCut==-1:
        log.error("No mt threshold in topo definition")
    
    ElectronThr=-1
    for ChainPart in chainDicts:
        if 'Electron' in ChainPart['signature']:
            ElectronThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if ElectronThr==-1:
        log.error("No Electron chain part found in Transverse Mass Topo cut")

    from TrigEgammaHypo.TrigEFMtAllTEConfig import TrigEFMtAllTE_Generic

    MT_Hypo = TrigEFMtAllTE_Generic("TrigEFMtAllTE_"+str(ElectronThr)+"Mt"+str(MTCut).replace(".",""),
                                    minMtCut=MTCut,maxNbElectrons=10,minElectronEt=ElectronThr)
    
    ##Get only the last MET TE
    inputTEsEF = [] 

    # First TE: MET
    for cD in listOfChainDefs:
        if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_xe' in x]:
            inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
            break

    if inputTEsEF == []:
        for cD in listOfChainDefs:
            if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_xs' in x]:
                inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                break

    # Second TE: electron
    for cD in listOfChainDefs:
        if [x for x in cD.signatureList[-1]['listOfTriggerElements'] if 'EF_e' in x]:
            inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
            break

    #for cD in listOfChainDefs: 
    #    inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                         
    #inputTEsEF.reverse() # need first met then jet input TE           

    log.debug("Input TEs to Transverse Mass algorithm: %s", inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']
    if (len(EFChainName) > 99):
        EFChainName = EFChainName[:-(len(EFChainName)-99)]
    #if '_L1' in EFChainName:
        #EFChainName = EFChainName.split("_L1")[0]
    
    theChainDef.addSequence([MT_Hypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef


##############################################################################
def _addRazor(theChainDef,chainDicts,listOfChainDefs): 

    for topo_item in chainDicts[0]['topo']:
        RazorCut=int(topo_item.split('razor')[1]) if 'razor' in topo_item else log.error("No Razor threshold in topo definition")
    
    JetThr=-1
    for ChainPart in chainDicts:
        if 'Jet' in ChainPart['signature']:
            JetThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if JetThr==-1:
        log.error("No JET chain part found in Razor cut")

    from TrigHLTJetHypo.TrigEFRazorAllTEConfig import EFRazor

    Razor_Hypo = EFRazor("EFRazor_J"+str(JetThr).replace(".","")+"_Razor"+str(RazorCut).replace(".",""),
                                           Razor_cut=RazorCut)

    inputTEsEFMET = []
    inputTEsEFJet = []

    for sig in theChainDef.signatureList: 
        if "EF" in sig['listOfTriggerElements'][-1] and 'xe' in sig['listOfTriggerElements'][-1] and not( 'step' in sig['listOfTriggerElements'][-1]  ) and not( 'razor' in sig['listOfTriggerElements'][-1]  ):
            inputTEsEFMET.append( sig['listOfTriggerElements'][-1])

    signatureListCopy = deepcopy(theChainDef.signatureList)
    signatureListCopy.reverse()
    for sig in signatureListCopy:
        if not "xe" in sig['listOfTriggerElements'][-1]:
            inputTEsEFJet.append(sig['listOfTriggerElements'][-1])
            break
        
    if inputTEsEFJet == []:
        log.error("Could not identify jet input TE for hemnisphere sequence.")
        
    log.debug("Input TEs to Razor algorithm: %s", inputTEsEFMET)

    from TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecConfig import TrigHLTJetHemisphereRec_Builder
    theTrigHLTJetHemisphereRec = TrigHLTJetHemisphereRec_Builder(jetPtCut=40000.)

    EFChainName = "EF_" + chainDicts[0]['chainName']

    for i,thisSequence in enumerate(theChainDef.sequenceList):
        if "EF_jets_" in thisSequence['output']:
            theChainDef.sequenceList.pop( i )
            break

    for i,thisSignature in enumerate(theChainDef.signatureList):
        if "EF_jets_" in thisSignature['listOfTriggerElements'][0]:
            theChainDef.signatureList.pop( i )
            break

    for j in xrange(i,len(theChainDef.signatureList) ):
        theChainDef.signatureList[j]['signature_counter'] = theChainDef.signatureList[j]['signature_counter']  - 1

    outputTE = inputTEsEFJet[0]+'_hemisphRec'
    theChainDef.addSequence([theTrigHLTJetHemisphereRec], inputTEsEFJet,outputTE)
    theChainDef.addSequence([Razor_Hypo],inputTEsEFMET+[outputTE],EFChainName)

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [outputTE])    
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef


##############################################################################
def _addTauMass(theChainDef,chainDicts,listOfChainDefs): 

    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
    #inputTEsEF.reverse() # need first met then jet input TE           


    from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf       import TrigEFPhotonMuonAngleFexAlgo
    from TrigEgammaMuonCombHypo.TrigEFPhotonMuonAngleHypoConfig  import TrigEFPhotonMuonAngleHypo_tau
    EFFex  =  TrigEFPhotonMuonAngleFexAlgo()
    EFHypo =  TrigEFPhotonMuonAngleHypo_tau()

    log.debug("Input TEs to TauMass algorithm: %s", inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']
    
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef

##############################################################################
def _addXemu(theChainDef,chainDicts,listOfChainDefs): 

    # determine which thresholds are needed for L2 MultiTrk algo
    tracksThr = []
    fexNameExt = ""

    
    for chainDict in chainDicts:
      for dictpart in chainDict['chainParts']:
        if 'mu' in dictpart['trigType'] or  'e' in dictpart['trigType'] :
            for x in range(0,int(dictpart['multiplicity'])):
                if dictpart['threshold']!='0':
                    dthr = float(dictpart['threshold'] ) 
                    thr= dthr * 1000.  # in MeV; 
                    if dthr < 9.5 :
                        thr = thr - 350.
                    elif dthr < 11.5 :
                        thr = thr - 550. 
                    elif dthr < 21.5  :
                        thr = thr - 750.                         
                    else :
                        thr = thr -1000.
                else :
                    thr = 900.
                tracksThr.append(thr)
                fexNameExt = fexNameExt + "_"+str(int(dictpart['threshold']))

    if len(tracksThr) < 2 :
        log.error(" something is wrong with chain, expect at least 2 thresholds, received "+str(tracksThr) )
    from TrigBphysHypo.TrigMultiTrkFexConfig import TrigMultiTrkFex_EMu
    L2Fex = TrigMultiTrkFex_EMu("TrigMultiTrkFex_EMu"+fexNameExt)  # has chi2 < 20 , OS and 0.1< m(2trk) < 7 GeV
    L2Fex.setTrackThresholds( tracksThr )

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements']
    L2TEname = "L2_" +  chainDicts[0]['chainName']+'_'+"bXemu"
    theChainDef.addSequence([L2Fex], inputTEsL2, L2TEname)
    theChainDef.addSignatureL2([L2TEname])

    
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
    #inputTEsEF.reverse() # need first met then jet input TE           


    from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf       import TrigEFElectronMuonAngleFexRun2Algo
    from TrigEgammaMuonCombHypo.TrigEFElectronMuonAngleHypoConfig  import TrigEFElectronMuonAngleHypo_bXemu
    EFFex  =  TrigEFElectronMuonAngleFexRun2Algo()
    EFHypo =  TrigEFElectronMuonAngleHypo_bXemu()

    log.debug("Input TEs to bXemu algorithm: %s", inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']
    
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef

##############################################################################

def _addMVis(theChainDef,chainDicts,listOfChainDefs):

    ## that was it previously, just grabbing the last outputTE:
    #inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']           

    # rather get the last outputTE named EF_g and EF_tau
    inputTEsEF=[]
    inputTE_tau=''
    inputTE_photon=''

    for iS in range(0, len(theChainDef.signatureList) ):
        for TE in theChainDef.signatureList[iS]['listOfTriggerElements'] :
            if 'EF_g' in TE: inputTE_photon=TE
            if 'EF_tau' in TE: inputTE_tau=TE
    inputTEsEF=[inputTE_tau,inputTE_photon ]
        
    maxMvis=-1
    minMvis=-1
    for topo_item in chainDicts[0]['topo']:
        if 'mVis' in topo_item:
            minMvis=float(topo_item.split('mVis')[0])
            maxMvis=float(topo_item.split('mVis')[1])
    log.debug("mVis cuts at [GeV]: %d and %d", minMvis, maxMvis)
    if minMvis==-1 or maxMvis==-1:
        log.error("No mVis chain part found in Photon-Tau mVis Topo cut")

    from TrigTauHypo.TrigTauHypoConfig2012 import EFPhotonTauHypo
    from TrigTauHypo.TrigTauHypoConf       import EFPhotonTauFex 

    EFFex  =  EFPhotonTauFex()
    theVars    = ['MvisMin','MvisMax']
    theThresh  = [minMvis*1000.,maxMvis*1000.]
    PhotonTaumVis_Hypo = EFPhotonTauHypo("EFPhotonTau_"+str(minMvis).replace(".","")+"mVis"+str(maxMvis).replace(".",""),
                                           theVars, theThresh)

    log.debug("Input TEs to mVis algorithm: %s", inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']

    theChainDef.addSequence([EFFex, PhotonTaumVis_Hypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    return theChainDef

##############################################################################
def _adddR(theChainDef,chainDicts,listOfChainDefs):
    # dR between lep/tau+tau chains
    inputTEsEF=[]
    inputTE_lepton=''
    inputTE_taus=[]
    chain_type = "ditau"

    for iS in range(0, len(theChainDef.signatureList) ):
        inputTE_taus_tmp=[]
        for TE in theChainDef.signatureList[iS]['listOfTriggerElements'] :
            el_TE  = re.compile('_e[0-9]')
            mu_TE  = re.compile('_mu[0-9]')
            tau_TE = re.compile('_tau[0-9]')
            if 'EF_' in TE and ('_e' in TE or '_mu' in TE): 
               # last lepton TE
               if len(re.findall(el_TE, TE)): 
                  inputTE_lepton=TE
                  chain_type = "eltau"
               elif len(re.findall(mu_TE, TE)): 
                  inputTE_lepton=TE	
                  chain_type = "mutau"
            if 'EF_' in TE and len(re.findall(tau_TE, TE)): 
               inputTE_taus_tmp+=[TE]
        if len(inputTE_taus_tmp)>0:
            # last set of tau TEs
            inputTE_taus = inputTE_taus_tmp

    if len(inputTE_taus)==0:
       log.error("dRtt: no tau TE found")
    if inputTE_lepton!='':
       inputTEsEF+=[inputTE_lepton]
    inputTEsEF+=inputTE_taus

    if len(inputTEsEF)!=2:
       log.error("dRtt: found %d TEs instead of 2", len(inputTEsEF))
       log.error(inputTEsEF)

    maxdR=-1
    mindR=-1
    topoAlgs = chainDicts[0]["topo"]
    for topo_item in topoAlgs:
            if 'dRtt' in topo_item:
              if(topo_item.split('dRtt')[0]!=''):
                 mindR=float(topo_item.split('dRtt')[0])
              else:
                 mindR=0.
              if(topo_item.split('dRtt')[1]!=''):
                 maxdR=float(topo_item.split('dRtt')[1])
              else:
                 maxdR=100.
    log.debug("dRtt cuts at: %f and %f", mindR*0.1, maxdR*0.1)
    if mindR == -1 or maxdR == -1:
           log.error("No dRtt chain part found in tau-tau / tau-mu / tau-e dR Topo cut")

    from TrigTauHypo.TrigTauHypoConfig2012 import EFTauTopoHypo
    from TrigTauHypo.TrigTauHypoConf       import EFTauTopoFex

    log.info('Topo picks up:')
    log.info(chainDicts[0])
    log.info("Chain {} is of type {}".format(chainDicts[0]['chainName'], chain_type))

    EFFex  =  EFTauTopoFex(comb=chain_type)
    theVars    = ['DRMin', 'DRMax']
    theThresh  = [mindR * 0.1, maxdR * 0.1] # treshold specified as integers -> change them to e.g. 0.3, 3.0
    TauTaudR_Hypo = EFTauTopoHypo(
            'EFTauTopo_{0}dRtt{1}_{2}'.format(
                str(mindR).replace('.', ''), str(maxdR).replace('.', ''), chain_type),
            theVars, theThresh)
    log.debug("Input TEs to dRtt algorithm: %s", inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']

    theChainDef.addSequence([EFFex,  TauTaudR_Hypo], inputTEsEF, EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    return theChainDef

##############################################################################

def _addMatching(theChainDef,chainDicts,listOfChainDefs):

    allInputTEsEF = [] 
    for cD in listOfChainDefs: 
        allInputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'][0])] 

    maxSigCounter = theChainDef.signatureList[-1]['signature_counter']
    # =========================================================
    # muon input TE to the hypo
    muonTE = theChainDef.signatureList[5]['listOfTriggerElements']

    # =========================================================
    #check if jet or bjet to be matched & find hypothreshold
    chnameToMatch = chainDicts[0]['chainName'].split("_dr")[0]
    chnameAddPart = chainDicts[0]['chainName'].split("_dr")[1]
    hypoThresh = [part.split("j")[1] for part in chnameToMatch.split("_") if "j" in part][0]+'GeV'
    if hypoThresh == '': log.error("No HypoThreshold could be extracted!")
    if ('_b') in chnameToMatch and ('_b') in chnameAddPart: 
        log.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))


    # =========================================================
    # configure Hypo and jet/bjetTE
    from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import getLeptonJetMatchAllTEInstance
    from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import LeptonJetMatchAllTE
 
    dzmatching = False
    for topo in chainDicts[0]['topo']:
        if "dz" in topo: dzmatching = True 
 
    # obtain deltaR for Hypo configuration
    deltaR = -1
    for topo_item in chainDicts[0]['topo']:
        if 'dr' in topo_item:
            deltaR=float(topo_item.split('dr')[1])/10.
        if deltaR == -1: log.error("No DeltaR cut could be extracted!")
 
 
    if dzmatching: # it's a bjet chain
        # obtain deltaZ for Hypo configuration
        deltaZ = -1
        for topo_item in chainDicts[0]['topo']:
            if 'dz' in topo_item:
                deltaZ=float(topo_item.split('dz')[1]) # Need to modify this to be able to handle dZ values > 9...
 
        jetTE = theChainDef.signatureList[-1]['listOfTriggerElements']
        pos_sigCounter = -1
 
        if deltaZ==99:
            LeptonJetFexAllTE = LeptonJetMatchAllTE("CloseBy","RZ", hypoThresh, name="LeptonJetMatchAllTE_CloseBy_RZ_"+str(hypoThresh)+'_dz'+str(int(deltaZ)) )
        else:
            LeptonJetFexAllTE = getLeptonJetMatchAllTEInstance("CloseBy","RZ", hypoThresh)
 
 
        LeptonJetFexAllTE.JetKey = "SplitJet"
        LeptonJetFexAllTE.DeltaRCut = deltaR
        LeptonJetFexAllTE.DeltaZCut = deltaZ
        if ('_anti') in chnameAddPart:
            log.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))

        
    else: # dealing with jets to match
        pos_sigCounter = 9999
        pos_sigCounterlist = []
        sigCounter = 9999
        sigCounterlist =[]
        jetTE = ''
        jetTElist = []

        for i, mydict in enumerate(theChainDef.signatureList):
            for tes in mydict['listOfTriggerElements']:
#                if isJetTE(tes):
                if 'noCleaning' in tes or ('HLThypo' in tes and re.match("EF_[_0-9]+",tes)):
                    #print "WOOF found my TE", tes
                    #print "WOOF belongs to sign pos", mydict['signature_counter']
                    jetTElist.append(tes)
                    sigCounterlist.append(mydict['signature_counter'])
                    pos_sigCounterlist.append(i)
#                else:
#                    print "This is not a jet TE?"+ str(tes)
                    
        if ('_b') in chnameToMatch: 
            jetTE = theChainDef.signatureList[-1]['listOfTriggerElements']
            pos_sigCounter = -1
            sigCounter =  theChainDef.signatureList[-1]['signature_counter']
            if ('_anti') in chnameAddPart:
                log.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))
        else:
            # import pdb;pdb.set_trace()
            # log.error('PS chain_name ' + theChainDef.chain_name)
            # log.error('PS jetTElist ' + str(jetTElist))
            # log.error('PS sig list ' + str(theChainDef.signatureList))
            jetTE = jetTElist[0]
            pos_sigCounter = pos_sigCounterlist[0]
            sigCounter = sigCounterlist[0]


        if pos_sigCounter > 20: log.error("Cannot determine the correct signature counter for the last jet sequence TE!")
        if jetTE == '': log.error("Could not find the last jet TE, maybe due to a change in the jet sequence naming convention?")

        LeptonJetFexAllTE = getLeptonJetMatchAllTEInstance("CloseBy","R", hypoThresh)
        LeptonJetFexAllTE.JetKey = ""
        LeptonJetFexAllTE.DeltaRCut = deltaR

    # =========================================================
    # Topo start from settings
    EFChainName = "EF_" + chainDicts[0]['chainName']
    from TriggerMenu.menu.MenuUtils import setupTopoStartFrom
    topoThresh = chainDicts[0]['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None
    if topoStartFrom:
        EFChainName = EFChainName + '_tsf'

    # =========================================================
    # matching sequence of the chain
    log.debug("Input TEs to LeptonJet algorithm: %s %s", muonTE, jetTE)
    theChainDef.addSequence([LeptonJetFexAllTE], [muonTE, jetTE], EFChainName, topo_start_from = topoStartFrom)

    if pos_sigCounter == -1:
        theChainDef.addSignature(theChainDef.signatureList[pos_sigCounter]['signature_counter']+1, [EFChainName])
    elif sigCounter == maxSigCounter:
        #print "WOOOOOOF"
        #print "WOOF: theChainDef.signatureList[pos_sigCounter]['signature_counter']+1", theChainDef.signatureList[pos_sigCounter]['signature_counter']+1
        #print "WOOF: theChainDef.signatureList[pos_sigCounter-1]['signature_counter']+1", theChainDef.signatureList[pos_sigCounter-1]['signature_counter']+1
        #print "WOOF: pos_sigCounter" , pos_sigCounter
        #print "WOOF: sigCounter" , sigCounter
        theChainDef.addSignature(theChainDef.signatureList[pos_sigCounter]['signature_counter']+1, [EFChainName])
    else:
        theChainDef.insertSignature(sigCounter+1, [EFChainName])
        #print "WOOF: theChainDef.signatureList[pos_sigCounter]['signature_counter']+1", theChainDef.signatureList[pos_sigCounter]['signature_counter']+1
        #print "WOOF: theChainDef.signatureList[pos_sigCounter-1]['signature_counter']+1", theChainDef.signatureList[pos_sigCounter-1]['signature_counter']+1
        #print "WOOF: pos_sigCounter" , pos_sigCounter
        #print "WOOF: sigCounter" , sigCounter


    #print "MEOW pos sig counter", pos_sigCounter
    #print "MEOW chain def", theChainDef.signatureList

    return theChainDef

##############################################################################
# def _addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs): 

#     for topo_item in chainDicts[0]['topo']:
#         DEtaCut=float(topo_item.split('deta')[1]) if 'deta' in topo_item else log.error("No DEta threshold in topo definition")
    
#     JetThr=-1
#     for ChainPart in chainDicts:
#         if 'Jet' in ChainPart['signature']:
#             JetThr=int(ChainPart['chainParts'][0]['threshold'])
#             break

#     if JetThr==-1:
#         log.error("No JET chain part found in DPhiMetJet Topo cut")

#     from TrigJetHypo.TrigEFJetMassDEtaConfig import *

#     DEtaMjjJet_Hypo = EFJetMassDEta("EFJetMassDEta_J"+str(JetThr).replace(".","")+"_Mjj"+str(mjjCut).replace(".",""),
#                                            MjjCut=mjjCut, DEtaCut=detaCut)

    
#     ##Get only the last MET TE
#     inputTEsEF = [] 
#     for cD in listOfChainDefs: 
#         inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
 		         
#     #inputTEsEF.reverse() # need first met then jet input TE           

#     log.debug("Input TEs to DEta algorithm: %s", inputTEsEF)

#     EFChainName = "EF_" + chainDicts[0]['chainName']
    
#     theChainDef.addSequence([DEtaMjjJet_Hypo],inputTEsEF,EFChainName)
#     theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

#     return theChainDef


##############################################################################
