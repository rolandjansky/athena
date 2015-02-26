# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Combined Chains, used to implement TOPOS
###########################################################################
__author__  = 'E. Pianori'
__doc__="Definition of topological cuts for combined chains"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logCombined = logging.getLogger("TriggerMenu.combined.CombinedChainsDef")

from copy import deepcopy

###########################################################################
###########################################################################

def _addTopoInfo(theChainDef,chainDicts,listOfChainDefs,doAtL2AndEF=True): 

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
            
    topoAlgs = chainDicts[0]["topo"]

    if any("dphi" in alg for alg in topoAlgs):
        ##Check that we only have a MET and JET chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'MET' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            logCombined.warning("Need a Met and a JET chain to run DPhi Topo cut")        
        else:
            theChainDef=_addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs)
            
    elif any("razor" in alg for alg in topoAlgs):
        ##Check that we only have a MET and JET chain
        inputChains=[]
        for ChainPart in chainDicts: 
            if 'MET' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2: 
            logCombined.warning("Need a Met and a JET chain to run Razor cut")
        else:
            theChainDef=_addRazor(theChainDef,chainDicts,listOfChainDefs)
            
    elif any("taumass" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Muon chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Photon' in ChainPart['signature']  or 'Muon' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            logCombined.warning("Need a Photon and a Muon chain to run TauMass Topo cut")        
        else:
            theChainDef=_addTauMass(theChainDef,chainDicts,listOfChainDefs)   

    elif any("dr" in alg for alg in topoAlgs):
        ##Check that we only have a Photon and Muon chain
        inputChains=[]
        for ChainPart in chainDicts:
            if 'Muon' in ChainPart['signature']  or 'Jet' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2:
            logCombined.warning("Need a Jet and a Muon chain to run the matching algorithm")        
        else:
            theChainDef=_addMatching(theChainDef,chainDicts,listOfChainDefs)   

    # elif any("deta" in alg for alg in topoAlgs):
    #     ##Check that we only have a Jet and Muon chain
    #     inputChains=[]
    #     for ChainPart in chainDicts:
    #         if 'Jet' in ChainPart['signature']:
    #             inputChains.append(ChainPart['signature'])
    #     if len(inputChains)<2:
    #         logCombined.warning("Need a Photon and a Muon chain to run TauMass Topo cut")        
    #     else:
    #         theChainDef=_addDEtaJetJet(theChainDef,chainDicts,listOfChainDefs)   

    else:
        logCombined.error("do nothing, intra-signature topological cut not implemented yet")

        
    return theChainDef


##############################################################################
def _addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs): 

    for topo_item in chainDicts[0]['topo']:
        DPhiCut=float(topo_item.split('dphi')[1]) if 'dphi' in topo_item else logCombined.error("No Dphi threshold in topo definition")
    
    JetThr=-1
    for ChainPart in chainDicts:
        if 'Jet' in ChainPart['signature']:
            JetThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if JetThr==-1:
        logCombined.error("No JET chain part found in DPhiMetJet Topo cut")

    from TrigJetHypo.TrigEFDPhiMetJetAllTEConfig import *

    DPhiMetJet_Hypo = EFDPhiMetJet_Generic("EFDPhiMetJet_J"+str(JetThr).replace(".","")+"_DPhi"+str(DPhiCut).replace(".",""),
                                           dPhiCut=DPhiCut, minJetEt=JetThr,maxDPhiJets=1)
    
    ##Get only the last MET TE
    inputTEsEF = [] 
    for cD in listOfChainDefs: 
        inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
 		         
    inputTEsEF.reverse() # need first met then jet input TE           

    logCombined.debug("Input TEs to DPhi algorithm: %s" % inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']
    if '_L1' in EFChainName:
        EFChainName = EFChainName.split("_L1")[0]
    
    theChainDef.addSequence([DPhiMetJet_Hypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef


##############################################################################
def _addRazor(theChainDef,chainDicts,listOfChainDefs): 

    for topo_item in chainDicts[0]['topo']:
        RazorCut=int(topo_item.split('razor')[1]) if 'razor' in topo_item else logCombined.error("No Razor threshold in topo definition")
    
    JetThr=-1
    for ChainPart in chainDicts:
        if 'Jet' in ChainPart['signature']:
            JetThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if JetThr==-1:
        logCombined.error("No JET chain part found in Razor cut")

    from TrigJetHypo.TrigEFRazorAllTEConfig import *

    Razor_Hypo = EFRazor("EFRazor_J"+str(JetThr).replace(".","")+"_Razor"+str(RazorCut).replace(".",""),
                                           Razor_cut=RazorCut)

    inputTEsEFMET = []
    inputTEsEFJet = []

    for sig in theChainDef.signatureList: 
        if "EF" in sig['listOfTriggerElements'][-1] and 'xe' in sig['listOfTriggerElements'][-1] and not( 'step' in sig['listOfTriggerElements'][-1]  ) and not( 'razor' in sig['listOfTriggerElements'][-1]  ):
            inputTEsEFMET.append( sig['listOfTriggerElements'][-1])

    ### CATRIN COMMENTING OUT DUE TO CHANGED JET DEFAULTSinputTEsEFJet.append( 'EF_full__cluster__antikt4_tc_jes' )

    print "YYYYYYYYYYYYYY"
    signatureListCopy = deepcopy(theChainDef.signatureList)
    signatureListCopy.reverse()
    for sig in signatureListCopy:
        if not "xe" in sig['listOfTriggerElements'][-1]:
            inputTEsEFJet.append(sig['listOfTriggerElements'][-1])
            break
        
    if inputTEsEFJet == []:
        logCombined.error("Could not identify jet input TE for hemnisphere sequence.")
        
    logCombined.debug("Input TEs to Razor algorithm: %s" % inputTEsEFMET)

    from TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecConfig import TrigHLTJetHemisphereRec_Builder
    theTrigHLTJetHemisphereRec = TrigHLTJetHemisphereRec_Builder(jetPtCut=30000.)

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

    logCombined.debug("Input TEs to TauMass algorithm: %s" % inputTEsEF)

    EFChainName = "EF_" + chainDicts[0]['chainName']
    
    theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

    return theChainDef

##############################################################################

def _addMatching(theChainDef,chainDicts,listOfChainDefs): 

    allInputTEsEF = [] 
    for cD in listOfChainDefs: 
        allInputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'][0])] 

    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
    secondlastTEsEF = theChainDef.signatureList[-2]['listOfTriggerElements']
    muonTE = theChainDef.signatureList[3]['listOfTriggerElements']

    from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import getLeptonJetMatchAllTEInstance
    LeptonJetFexAllTE_RZ = getLeptonJetMatchAllTEInstance("CloseBy","RZ")

    logCombined.debug("Input TEs to LeptonJet algorithm: %s" % inputTEsEF)
    EFChainName = "EF_" + chainDicts[0]['chainName']


    #-----------------------------------------------------------------------------------
    # algo imports
    #-----------------------------------------------------------------------------------
    # super ROI building
    from TrigBjetHypo.TrigSuperRoiBuilderAllTEConfig import getSuperRoiBuilderAllTEInstance
    theSuperRoi=getSuperRoiBuilderAllTEInstance()

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence # new
    [trkvtx, trkftf, trkprec] = TrigInDetSequence("Bjet", "bjet", "IDTrig", "2step").getSequence() # new
    # for b-tagging
    theBjetTracks = trkftf+trkprec
    # for vertexing
    theVertexTracks = trkvtx 
    
    # primary vertexing
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEConfig import EFHistoPrmVtxAllTE_Jet
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxComboConfig import EFHistoPrmVtxCombo_Jet

    #jet splitting
    from TrigBjetHypo.TrigJetSplitterAllTEConfig import getJetSplitterAllTEInstance
    theJetSplit=getJetSplitterAllTEInstance()

    # Et hypo (doesn't work in superROI mode)
    hypoThresh ='55GeV'
    from TrigBjetHypo.TrigBjetEtHypoConfig import getBjetEtHypoInstance
    theBjetEtHypo   = getBjetEtHypoInstance("EF", "Btagging", hypoThresh)

    # secondary vertexing
    from InDetTrigVxSecondary.InDetTrigVxSecondary_LoadTools import TrigVxSecondaryCombo_EF
    theVxSecondary = TrigVxSecondaryCombo_EF()

    #lepton jet matching
    from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import getLeptonJetMatchAllTEInstance
    LeptonJetFexAllTE_RZ = getLeptonJetMatchAllTEInstance("CloseBy","RZ")


    #-----------------------------------------------------------------------------------
    # sequence assembling & TE definitions
    #-----------------------------------------------------------------------------------
    superTE = inputTEsEF[0]+'_sRoi'
    superTrackingTE = superTE+'_sTrk'
    prmVertexTE = superTrackingTE+'_prmVtx'
    comboPrmVtxTE = inputTEsEF[0]+'_sRoisTrkVtx'
    jetSplitTE=comboPrmVtxTE+'_jetSplit'
    jetEtHypoTE=jetSplitTE+'_bjetHypo'+hypoThresh
    jetTrackTE=jetEtHypoTE+'bjettrk'
    secVtxTE=comboPrmVtxTE+'bj'+hypoThresh+'Vxsec'
    

    # Vertexing part of the chain
    theChainDef.addSequence(theSuperRoi,                 inputTEsEF,                    superTE        )
    theChainDef.addSequence(theVertexTracks,             superTE,                       superTrackingTE) 
    theChainDef.addSequence([EFHistoPrmVtxAllTE_Jet()],  superTrackingTE,               prmVertexTE    )
    theChainDef.addSequence([EFHistoPrmVtxCombo_Jet()], [superTrackingTE, prmVertexTE], comboPrmVtxTE  )
    
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [comboPrmVtxTE])

    # b-tagging part of the chain (requires PV)
    theChainDef.addSequence(theJetSplit,             [inputTEsEF, comboPrmVtxTE], jetSplitTE )
    theChainDef.addSequence(theBjetEtHypo,            jetSplitTE,                 jetEtHypoTE)  
    theChainDef.addSequence(theBjetTracks,            jetEtHypoTE,                jetTrackTE )   
    theChainDef.addSequence(theVxSecondary,          [jetTrackTE, comboPrmVtxTE], secVtxTE   )

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [secVtxTE])

    # matching part of the chain
    theChainDef.addSequence([LeptonJetFexAllTE_RZ], [muonTE,     secVtxTE],EFChainName)    

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    return theChainDef

##############################################################################
# def _addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs): 

#     for topo_item in chainDicts[0]['topo']:
#         DEtaCut=float(topo_item.split('deta')[1]) if 'deta' in topo_item else logCombined.error("No DEta threshold in topo definition")
    
#     JetThr=-1
#     for ChainPart in chainDicts:
#         if 'Jet' in ChainPart['signature']:
#             JetThr=int(ChainPart['chainParts'][0]['threshold'])
#             break

#     if JetThr==-1:
#         logCombined.error("No JET chain part found in DPhiMetJet Topo cut")

#     from TrigJetHypo.TrigEFJetMassDEtaConfig import *

#     DEtaMjjJet_Hypo = EFJetMassDEta("EFJetMassDEta_J"+str(JetThr).replace(".","")+"_Mjj"+str(mjjCut).replace(".",""),
#                                            MjjCut=mjjCut, DEtaCut=detaCut)

    
#     ##Get only the last MET TE
#     inputTEsEF = [] 
#     for cD in listOfChainDefs: 
#         inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
 		         
#     #inputTEsEF.reverse() # need first met then jet input TE           

#     logCombined.debug("Input TEs to DEta algorithm: %s" % inputTEsEF)

#     EFChainName = "EF_" + chainDicts[0]['chainName']
    
#     theChainDef.addSequence([DEtaMjjJet_Hypo],inputTEsEF,EFChainName)
#     theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])    

#     return theChainDef


##############################################################################
