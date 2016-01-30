# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Jet signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logJet = logging.getLogger("TriggerMenu.jet.generateJetChainDefs")

from AthenaCommon.SystemOfUnits import GeV

from TriggerMenu.jet.JetDef import generateHLTChainDef
from TriggerMenu.jet.JetDef_HT import L2EFChain_HT

from TriggerMenu.menu.MenuUtils import *

from JetDef import dump_chaindef
from exc2string import exc2string2
from TriggerMenu.menu.ChainDef import ErrorChainDef
import os, inspect

TrigEFJetMassDEta_Config = __import__("TrigJetHypo.TrigEFJetMassDEtaConfig",fromlist=[""])


from  __builtin__ import any as b_any

#############################################################################
#############################################################################

def generateChainDefs(chainDict):
    """Delegate the creation of ChainDef instnaces to JetDEf,
    then add in  the top information."""
    
    jetgroup_chain = True
    chainName = chainDict['chainName']
    print 'processing chain part 1 start', chainName
    theChainDef = generateHLTChainDef(chainDict)
    print 'processing chain part 2 end', chainName

    listOfChainDicts = splitChainDict(chainDict)

    topoAlgs = []
    for subCD in listOfChainDicts:  
        allTopoAlgs = subCD['chainParts']['topo']
        for ta in allTopoAlgs:
            topoAlgs.append(ta)
    
    if ('muvtx' in topoAlgs) or \
            ('llp' in topoAlgs) or \
            (b_any(('invm' or 'deta' in x) for x in topoAlgs)):

        logJet.info("Adding topo to jet chain")
        try:
            theChainDef = _addTopoInfo(theChainDef, chainDict, topoAlgs)
        except Exception, e:
            tb = exc2string2()
            theChainDef = process_exception(e, tb, chainName)

        jetgroup_chain = False

    no_instantiation_flag = 'JETDEF_NO_INSTANTIATION' in os.environ
    dump_chain = 'JETDEF_DEBUG2' in os.environ
    if (not jetgroup_chain) and dump_chain:
        dump_chaindef(chainDict, theChainDef, no_instantiation_flag)

    return theChainDef


##########################################################################################
def _addTopoInfo(theChainDef,chainDict, topoAlgs, doAtL2AndEF=True):

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   
    #topoAlgs = chainDict["topo"]

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    if ('muvtx' in topoAlgs):
       # import pdb;pdb.set_trace()
        theChainDef = generateMuonClusterLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs)
    elif ('llp' in topoAlgs):
        theChainDef = generateCaloRatioLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs)
    elif b_any(('invm' or 'deta' in x) for x in topoAlgs):
        theChainDef = addDetaInvmTopo(theChainDef,chainDict,inputTEsL2, inputTEsEF, topoAlgs)
    else:
        logJet.error('Your favourite topo configuration is missing.')

    return theChainDef

##########################################################################################
def generateMuonClusterLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs):
    HLTChainName = "HLT_" + chainDict['chainName']   


    if 'EMPTY' in HLTChainName:
        l1item = 'MU4'
    elif 'UNPAIRED' in HLTChainName:
        l1item = 'MU4'
    else:
        l1item = 'MU10'

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkcore, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", "2step").getSequence()
    
    # muon cluster 
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
    fexes_l2_MuonCluster = MuonClusterConfig()

    # muon cluster hypo
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoConfig
    if ('noiso' not in topoAlgs):
        hypos_l2_MuonCluster = MuonClusterHypoConfig("MuonClusterHypo_primary",maxEta=2.5, numJet=0, numTrk=0)
    else:
        hypos_l2_MuonCluster = MuonClusterHypoConfig("MuonClusterHypo_background",maxEta=2.5, numJet=-1, numTrk=-1)

    TEmuonIsoB = HLTChainName+'_muIsoB'
    TEmuonClusterFex = HLTChainName+'_muClusFex'
    TEmuonClusterHypo = HLTChainName+'_muClusHypo'

    # adding muonIso sequence
    theChainDef.addSequence(trkiso+trkprec,l1item, TEmuonIsoB)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonIsoB])
    
    # adding seq using jetTE and TE from seq above (MuonCluster fex)
#    theChainDef.addSequence([fexes_l2_MuonCluster], [TEmuonIsoB, inputTEsEF], TEmuonClusterFex)
    theChainDef.addSequence([fexes_l2_MuonCluster,hypos_l2_MuonCluster], [TEmuonIsoB, inputTEsEF], TEmuonClusterFex)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonClusterFex])

#    # adding MuonCluster hypo
#    theChainDef.addSequence([hypos_l2_MuonCluster], TEmuonClusterFex, TEmuonClusterHypo)
#    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonClusterHypo])

    return theChainDef


##########################################################################################
def generateCaloRatioLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs):
    HLTChainName = "HLT_" + chainDict['chainName']   

    # need to chang the inputTE for the first seq. If set to L1Topoo -> change to TAU30 (HA30 threshold)
    currentInput = theChainDef.sequenceList[0]['input']
    if "-" in currentInput:
        theChainDef.sequenceList[0]['input'] = "HA30"
    
    # jet splitting
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import getJetSplitterInstance
    theJetSplit=getJetSplitterInstance()

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkcore, trkiso, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig", "2step").getSequence()

    # calo-ratio
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import CaloRatioHypo
    fex_llp_jet_hypo = CaloRatioHypo('TrigCaloRatioHypo_j30', threshold=30*GeV, logratio=1.2)

    # beam-halo removal
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import TrigNewLoFHypoConfig
    hypo_LoF = TrigNewLoFHypoConfig()

    TE_SplitJets = HLTChainName+'_SplitJetTool'
    TE_TrackMuonIsoB = HLTChainName+'_TrkMuIsoB'
    TE_LogRatioCut = HLTChainName+'_LogRatioCut'
    TE_BeamHaloRemoval = HLTChainName+'_BeamHaloRemoval'

    # split into several trigger elements
    theChainDef.addSequence(theJetSplit, inputTEsEF, TE_SplitJets)

    # adding tracking sequence
    theChainDef.addSequence(trkiso+trkprec,TE_SplitJets,TE_TrackMuonIsoB)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_TrackMuonIsoB])

    # adding calo-ratio sequence
    theChainDef.addSequence(fex_llp_jet_hypo,TE_TrackMuonIsoB,TE_LogRatioCut)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_LogRatioCut])

    # adding LoF sequence
    if ('noiso' not in topoAlgs):
        theChainDef.addSequence([hypo_LoF],TE_LogRatioCut,TE_BeamHaloRemoval)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_BeamHaloRemoval])

    return theChainDef



##########################################################################################
def addDetaInvmTopo(theChainDef,chainDicts,inputTEsL2, inputTEsEF,topoAlgs):
        
    
    jet_thresholds=[]
    #Check what's the lowest jet threhsold used in the combined chains with deta
    for ChainPart in chainDicts['chainParts']:
        if  'Jet' in ChainPart['signature']:
            jet_thresholds.append( int(ChainPart['threshold']))
    if not jet_thresholds:
        logJet.warning('Error in idenfifying the lowest threshold jet, all jets used for topo algorithm')    

    for topo_item in topoAlgs:
        if 'invm' in topo_item:
            invmCut=float(topo_item.split('invm')[1]) 
        else:
            logJet.debug("No invm threshold in topo definition, using default invm = 0.")
            invmCut = 0.

        if 'deta' in topo_item:
            detaCut=(float(topo_item.split('deta')[1])/10.) 
        else:
            logJet.debug("No deta threshold in topo definition, using default deta=99.")
            detaCut = -99.
                

    hypo='EFJetMassDEta'
    for topo_item in topoAlgs:
        algoName="jet"
        if 'deta' in topo_item:
            try:
                min_thr= min(40,min(jet_thresholds))
                hypo=('EFJetMassDEta2J%i' %(min_thr))
                algoName="2jet%s" %(min_thr)
            except:
                hypo='EFJetMassDEta'
        algoName +="_"+topo_item    
    
    logJet.debug("Configuration of EFJetMassDeta hypo for chain %s: Hypo %s, AlgoName: %s " %(chainDicts['chainName'],hypo,algoName))
    try:
        detamjjet_hypo = getattr(TrigEFJetMassDEta_Config,hypo ) 
    except:  
        logJet.error("Hypo %s does not exists" %(hypo))

    DEtaMjjJet_Hypo = detamjjet_hypo(algoName, mjj_cut=invmCut*GeV, deta_cut=detaCut)
    
    chainName = "HLT_" + inputTEsEF[0] + "_"+ algoName
    theChainDef.addSequence([DEtaMjjJet_Hypo], inputTEsEF, chainName)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [chainName])    

    return theChainDef


def  process_exception(e, tb, chain_name):
    msg = 'JetDef Instantiator error: error: %s\n%s' % (str(e), tb)
    cd = ErrorChainDef(msg, chain_name)
    return cd
