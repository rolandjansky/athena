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

    if any("mt" in alg for alg in topoAlgs):
        ##Check that we only have a MET and Electron chain
        inputChains=[]
        for ChainPart in chainDicts: 
            if 'MET' in ChainPart['signature'] or 'XS' in ChainPart['signature'] or 'Electron' in ChainPart['signature']:
                inputChains.append(ChainPart['signature'])
        if len(inputChains)<2: 
            logCombined.warning("Need a MET/XS and an Electron chain to run MT cut")
        else:
            theChainDef=_addTransverseMass(theChainDef,chainDicts,listOfChainDefs)
            
    if any("razor" in alg for alg in topoAlgs):
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

    #else:
    #    logCombined.error("do nothing, intra-signature topological cut not implemented yet")

        
    return theChainDef


##############################################################################
def _addDPhiMetJet(theChainDef,chainDicts,listOfChainDefs): 

    maxJets=-1
    DPhiCut=-1
    for topo_item in chainDicts[0]['topo']:
        if 'dphi' in topo_item:
            maxJets=int(topo_item.split('dphi')[0])
            DPhiCut=float(topo_item.split('dphi')[1])/10.

    if DPhiCut==-1 or maxJets==-1:
        logCombined.error("No dphi chain part found in DPhiMetJet Topo cut")

    JetThr=-1
    for ChainPart in chainDicts:
        if 'Jet' in ChainPart['signature']:
            JetThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if JetThr==-1:
        logCombined.error("No JET chain part found in DPhiMetJet Topo cut")

    from TrigJetHypo.TrigEFDPhiMetJetAllTEConfig import *

    DPhiMetJet_Hypo = EFDPhiMetJet_Generic("EFDPhiMetJet_J"+str(JetThr).replace(".","")+"_DPhi"+str(DPhiCut).replace(".",""),
                                           dPhiCut=DPhiCut, minJetEt=JetThr*1000,maxDPhiJets=maxJets)
    
    ##Get only the last MET TE
    inputTEsEF = [] 
    for cD in listOfChainDefs: 
        inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
 		         
    inputTEsEF.reverse() # need first met then jet input TE           

    logCombined.debug("Input TEs to DPhi algorithm: %s" % inputTEsEF)

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
        logCombined.error("No mt threshold in topo definition")
    
    ElectronThr=-1
    for ChainPart in chainDicts:
        if 'Electron' in ChainPart['signature']:
            ElectronThr=int(ChainPart['chainParts'][0]['threshold'])
            break

    if ElectronThr==-1:
        logCombined.error("No Electron chain part found in Transverse Mass Topo cut")

    from TrigEgammaHypo.TrigEFMtAllTEConfig import *

    MT_Hypo = TrigEFMtAllTE_Generic("TrigEFMtAllTE_Mt"+str(MTCut).replace(".",""),
                                    minMtCut=MTCut,maxNbElectrons=10,minElectronEt=ElectronThr)
    
    ##Get only the last MET TE
    inputTEsEF = [] 
    for cD in listOfChainDefs: 
        inputTEsEF +=[deepcopy(cD.signatureList[-1]['listOfTriggerElements'])] 
                         
    inputTEsEF.reverse() # need first met then jet input TE           

    logCombined.debug("Input TEs to Transverse Mass algorithm: %s" % inputTEsEF)

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

    maxSigCounter = theChainDef.signatureList[-1]['signature_counter']
    # =========================================================
    # muon input TE to the hypo
    muonTE = theChainDef.signatureList[3]['listOfTriggerElements']

    print "MEOW chain def", theChainDef.signatureList

    # =========================================================
    #check if jet or bjet to be matched & find hypothreshold
    chnameToMatch = chainDicts[0]['chainName'].split("_dr")[0]
    chnameAddPart = chainDicts[0]['chainName'].split("_dr")[1]
    jetPart = chnameToMatch.split("j")[1]
    hypoThresh = [part.split("j")[1] for part in chnameToMatch.split("_") if "j" in part][0]+'GeV'
    if hypoThresh == '': logCombined.error("No HypoThreshold could be extracted!")
    if ('_b') in chnameToMatch and ('_b') in chnameAddPart: 
        logCombined.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))


    # =========================================================
    # configure Hypo and jet/bjetTE    
    from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import getLeptonJetMatchAllTEInstance

    dzmatching = False
    drmatching = False
    for topo in chainDicts[0]['topo']:
        if "dz" in topo: dzmatching == True
        if "dr" in topo: drmatching == True
    
        
    # obtain deltaR for Hypo configuration
    deltaR = -1
    for topo_item in chainDicts[0]['topo']:
        if 'dr' in topo_item:
            deltaR=float(topo_item.split('dr')[1])/10.
        if deltaR == -1: logCombined.error("No DeltaR cut could be extracted!")

    
    if dzmatching: # it's a bjet chain
        # obtain deltaZ for Hypo configuration
        deltaZ = -1
        for topo_item in chainDicts[0]['topo']:
            if 'dz' in topo_item:
                deltaZ=float(topo_item.split('dz')[1]) # Need to modify this to be able to handle dZ values > 9...

        jetTE = theChainDef.signatureList[-1]['listOfTriggerElements']
        pos_sigCounter = -1
        LeptonJetFexAllTE = getLeptonJetMatchAllTEInstance("CloseBy","RZ", hypoThresh)
        LeptonJetFexAllTE.JetKey = "SplitJet"
        LeptonJetFexAllTE.DeltaRCut = deltaR
        LeptonJetFexAllTE.DeltaZCut = deltaZ
        if ('_anti') in chnameAddPart: 
            logCombined.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))

        
    else: # dealing with jets to match
        pos_sigCounter = 9999
        pos_sigCounterlist = []
        sigCounter = 9999
        sigCounterlist =[]
        jetTE = ''
        jetTElist = []

        for i, mydict in enumerate(theChainDef.signatureList):
            for tes in mydict['listOfTriggerElements']:
                if 'noCleaning' in tes:
                    #print "WOOF found my TE", tes
                    #print "WOOF belongs to sign pos", mydict['signature_counter']
                    jetTElist.append(tes)
                    sigCounterlist.append(mydict['signature_counter'])
                    pos_sigCounterlist.append(i)
                    
        if ('_b') in chnameToMatch: 
            jetTE = theChainDef.signatureList[-1]['listOfTriggerElements']
            pos_sigCounter = -1
            sigCounter =  theChainDef.signatureList[-1]['signature_counter']
            if ('_anti') in chnameAddPart:
                logCombined.error("Matching functionality for this chain is not implemented yet: %s " % (chainDicts[0]['chainName']))
        else:
            jetTE = jetTElist[0]
            pos_sigCounter = pos_sigCounterlist[0]
            sigCounter = sigCounterlist[0]


        if pos_sigCounter > 20: logCombined.error("Cannot determine the correct signature counter for the last jet sequence TE!")
        if jetTE == '': logCombined.error("Could not find the last jet TE, maybe due to a change in the jet sequence naming convention?")

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
    logCombined.debug("Input TEs to LeptonJet algorithm: %s %s" % (muonTE, jetTE))
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
