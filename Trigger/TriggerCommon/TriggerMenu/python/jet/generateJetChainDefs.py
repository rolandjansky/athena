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

from TriggerMenu.menu.MenuUtils import splitChainDict

from JetDef import dump_chaindef
from exc2string import exc2string2
from TriggerMenu.menu.ChainDef import ErrorChainDef
import os

from TriggerMenu.commonUtils.makeCaloSequences import getFullScanCaloSequences
# TrigEFHLTJetMassDEta_Config = __import__("TrigHLTJetHypo.TrigEFHLTJetMassDEtaConfig",fromlist=[""])


from  __builtin__ import any as b_any

#############################################################################
#############################################################################

def generateChainDefs(chainDict):
    """Delegate the creation of ChainDef instnaces to JetDEf,
    then add in  the top information."""
    
    jetgroup_chain = True
    chainName = chainDict['chainName']
    #print 'processing chain part 1 start', chainName
    theChainDef = generateHLTChainDef(chainDict)
    #print 'processing chain part 2 end', chainName

    listOfChainDicts = splitChainDict(chainDict)

    topoAlgs = []
    for subCD in listOfChainDicts:  
        allTopoAlgs = subCD['chainParts']['topo']
        for ta in allTopoAlgs:
            topoAlgs.append(ta)

    # test new jet algorithm and configuration PS 15/9/2016
    # now use new jet algorthm TrigHLTJetHypo2 as standard,
    # and have the test chains use th run 1 hypo for comaprison PS 04/10/2016

    if ('muvtx' in topoAlgs) or ('llp' in topoAlgs):

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
        dump_chaindef(chainDict, theChainDef, None, no_instantiation_flag)

    return theChainDef


##########################################################################################
def _addTopoInfo(theChainDef,chainDict, topoAlgs, doAtL2AndEF=True):

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
 
    if ('muvtx' in topoAlgs):
       # import pdb;pdb.set_trace()
        theChainDef = generateMuonClusterLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs)
    elif ('revllp' in topoAlgs):
        theChainDef = generateReversedCaloRatioLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs)
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
    [trkcore,trkiso,trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()

    # muon cluster 
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
    fexes_l2_MuonCluster = MuonClusterConfig()

    # muon cluster isolation
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterIsolationConfig
    fexes_l2_MuonClusterIsolation = MuonClusterIsolationConfig()

    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoConfig
    hypos_l2_MuonCluster = MuonClusterHypoConfig("MuonClusterHypo_all", maxEta=2.5, midEta=1.0) 

    # muon cluster hypo
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterIsolationHypoConfig
    if ('noiso' not in topoAlgs):
        hypos_l2_MuonClusterIso = MuonClusterIsolationHypoConfig("MuonClusterIsolationHypo_primary", maxEta=2.5, midEta = 1.0, numJet=0, numTrk=0,doIsolation=True)
    else:
        hypos_l2_MuonClusterIso = MuonClusterIsolationHypoConfig("MuonClusterIsolationHypo_background", maxEta=2.5, midEta = 1.0, numJet=-1, numTrk=-1,doIsolation=False)

    TE_muonClusters = HLTChainName+'_muonClusters'
    TEmuonIsoB = HLTChainName+'_muIsoB'
    TEmuonClusterFex = HLTChainName+'_muClusFex'

    # make clusters, then test if they pass the hypo
    theChainDef.addSequence([fexes_l2_MuonCluster,hypos_l2_MuonCluster], l1item, TE_muonClusters)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_muonClusters])

    # adding muonIso tracking sequence, seeded by good RoI clusters
    theChainDef.addSequence(trkiso+trkprec,TE_muonClusters, TEmuonIsoB)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonIsoB])

    # test if clusters pass isolation 
    theChainDef.addSequence([fexes_l2_MuonClusterIsolation,hypos_l2_MuonClusterIso], [TEmuonIsoB, inputTEsEF], TEmuonClusterFex)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TEmuonClusterFex])

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
    if ('pufix' in topoAlgs):
        pufixLR=0.5
    else:
        pufixLR=1.2

    theJetSplit=getJetSplitterInstance("TrigJetSplitter",1.2, pufixLR)

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkcore, trkiso, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig", sequenceFlavour=["2step"]).getSequence()

    # calo-ratio
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import getCaloRatioHypoInstance
    fex_llp_jet_hypo = getCaloRatioHypoInstance("TrigCaloRatioHypo", 30, 1.2, True)


    # beam-halo removal
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import TrigNewLoFHypoConfig
    hypo_LoF = TrigNewLoFHypoConfig()

    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import getBHremovalInstance
    theBHremoval=getBHremovalInstance()

    TE_SplitJets = HLTChainName+'_SplitJetTool'
    TE_TrackIsoB = HLTChainName+'_TrkIsoB'
    TE_LogRatioCut = HLTChainName+'_LogRatioCut'
    TE_LogRatioCut_Fcalo = HLTChainName+'_LogRatioCut_Fcalo'
    TE_BeamHaloRemoval = HLTChainName+'_BeamHaloRemoval'

    # split into several trigger elements
    theChainDef.addSequence(theJetSplit, inputTEsEF, TE_SplitJets)

    # adding tracking sequence
    theChainDef.addSequence(trkiso+trkprec,TE_SplitJets,TE_TrackIsoB)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_TrackIsoB])

    # adding calo-ratio sequence
    theChainDef.addSequence(fex_llp_jet_hypo,TE_TrackIsoB,TE_LogRatioCut)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_LogRatioCut])

    # adding LoF sequence
    if ('noiso' not in topoAlgs):
        # create a dummy roi and get full calo
        fullScanSeqMap = getFullScanCaloSequences()
        
        theChainDef.addSequence(fullScanSeqMap['EF_full'][0],'', 'EF_full')
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, ['EF_full'])

        theChainDef.addSequence(fullScanSeqMap['EF_full_cell'][0],'EF_full', 'EF_full_cell')
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, ['EF_full'])

        # adding beam halo removal sequence
        theChainDef.addSequence(theBHremoval, ['EF_full_cell',TE_LogRatioCut],TE_LogRatioCut_Fcalo)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_LogRatioCut_Fcalo])

        theChainDef.addSequence([hypo_LoF], TE_LogRatioCut_Fcalo,TE_BeamHaloRemoval)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_BeamHaloRemoval])

    return theChainDef


##########################################################################################
def generateReversedCaloRatioLLPchain(theChainDef, chainDict, inputTEsL2, inputTEsEF, topoAlgs):
    HLTChainName = "HLT_" + chainDict['chainName']   

    # jet splitting
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import getJetSplitterInstance_LowLogRatio
    theJetSplit=getJetSplitterInstance_LowLogRatio()

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkcore, trkiso, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig", sequenceFlavour=["2step"]).getSequence()

    # reversed calo-ratio
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import getCaloRatioHypoInstance
    if ('trkiso' in topoAlgs):
        fex_llp_jet_hypo = getCaloRatioHypoInstance("TrigCaloRatioHypo", 200, -1.7, True)
    elif ('230' in HLTChainName):
        fex_llp_jet_hypo = getCaloRatioHypoInstance("TrigCaloRatioHypo", 230, -1.7, False)
    elif ('250' in HLTChainName):
        fex_llp_jet_hypo = getCaloRatioHypoInstance("TrigCaloRatioHypo", 250, -1.7, False)

    TE_SplitJets = HLTChainName+'_SplitJetTool'
    TE_TrackMuonIsoB = HLTChainName+'_TrkMuIsoB'
    TE_LogRatioCut = HLTChainName+'_LogRatioCut'

    # split into several trigger elements
    theChainDef.addSequence(theJetSplit, inputTEsEF, TE_SplitJets)

    if ('trkiso' in topoAlgs):
        # adding tracking sequence
        theChainDef.addSequence(trkiso+trkprec,TE_SplitJets,TE_TrackMuonIsoB)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_TrackMuonIsoB])
        # adding reversed calo-ratio sequence
        theChainDef.addSequence(fex_llp_jet_hypo,TE_TrackMuonIsoB,TE_LogRatioCut)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_LogRatioCut])
    else:
        # adding reversed calo-ratio sequence
        theChainDef.addSequence(fex_llp_jet_hypo,TE_SplitJets,TE_LogRatioCut)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [TE_LogRatioCut])

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
                

    hypo='EFHLTJetMassDEta'
    for topo_item in topoAlgs:
        algoName="jet"
        if 'deta' in topo_item:
            try:
                min_thr= min(40,min(jet_thresholds))
                hypo=('EFHLTJetMassDEta2J%i' %(min_thr))
                algoName="2jet%s" %(min_thr)
            except:
                hypo='EFHLTJetMassDEta'
        algoName +="_"+topo_item    
    
    logJet.debug("Configuration of EFHLTJetMassDeta hypo for chain %s: Hypo %s, AlgoName: %s " %(chainDicts['chainName'],hypo,algoName))
    # import pdb;pdb.set_trace()
    TrigEFHLTJetMassDEta_Config = None
    try:
        detamjjet_hypo = getattr(TrigEFHLTJetMassDEta_Config,hypo ) 
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
