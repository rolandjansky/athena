# 
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
# 

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaConfiguration.AllConfigFlags import ConfigFlags


#-----------------------------------------------------#
### Used the algorithms as Step1 "muFast step" ###
### Load data from Muon detectors ###
#-----------------------------------------------------#
#import MuonRecExample.MuonRecStandaloneOnlySetup
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonCombinedRecFlags.doCaloTrkMuId = False
muonCombinedRecFlags.printSummary = False

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

#muon container names (for RoI based sequences)
from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muonNames
muNames = muonNames().getNames('RoI')
#-----------------------------------------------------#
### ************* Step1  ************* ###
#-----------------------------------------------------#
def muFastAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2MuViewsMaker = EventViewCreatorAlgorithm("IMl2Mu")
    l2MuViewsMaker.ViewFallThrough = True
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"

    ### get muFast reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muFastRecoSequence
    muFastRecoSequence, sequenceOut = muFastRecoSequence( l2MuViewsMaker.InViewRoIs )

    l2MuViewsMaker.ViewNodeName = muFastRecoSequence.name() 
    
    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, muFastRecoSequence ])
    return (l2muFastSequence, l2MuViewsMaker, sequenceOut)

def muFastSequence():
    
    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolFromDict  )


def muFastOvlpRmSequence():
    
    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

    ### set up MuSAOverlapRemoval ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMufastAlg
    trigL2MuonOverlapRemover = TrigL2MuonOverlapRemoverMufastAlg("TrigL2MuonOverlapRemoverMufastAlg")
    trigL2MuonOverlapRemover.L2MuonOverlapInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMufastToolFromDict
  
    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = [trigMufastHypo, trigL2MuonOverlapRemover],
                         HypoToolGen = [TrigMufastHypoToolFromDict, TrigL2MuonOverlapRemoverMufastToolFromDict] )


#-----------------------------------------------------#
### ************* Step2  ************* ###
#-----------------------------------------------------#
def muCombAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2muCombViewsMaker = EventViewCreatorAlgorithm("IMl2muComb")
    l2muCombViewsMaker.ViewFallThrough = True #if this needs to access anything from the previous step, from within the view
    l2muCombViewsMaker.RoIsLink = "initialRoI" # setting to initial RoI, since RoIs are not updated in HLT processing
    l2muCombViewsMaker.InViewRoIs = "EMIDRoIs" #name of the RoIS inside of the view, because in principle we can have more than one RoI/view
    l2muCombViewsMaker.Views = "EMCombViewRoIs" #output of the views maker (key in "storegate")
    l2muCombViewsMaker.RequireParentView = True
    
    ### get ID tracking and muComb reco sequences ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import muCombRecoSequence, muonIDFastTrackingSequence
    muFastIDRecoSequence, eventAlgs = muonIDFastTrackingSequence( l2muCombViewsMaker.InViewRoIs )
    muCombRecoSequence, sequenceOut = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs )
 
    #Filter algorithm to run muComb only if non-Bphysics muon chains are active
    from TrigMuonEF.TrigMuonEFConfig import MuonChainFilterAlg
    muonChainFilter = MuonChainFilterAlg("FilterBphysChains")
    bphysChains =getBphysChainNames()
    muonChainFilter.ChainsToFilter=bphysChains
    muonChainFilter.InputDecisions = l2muCombViewsMaker.InputMakerOutputDecisions
    muonChainFilter.L2MuCombContainer = sequenceOut

    muCombFilterSequence = seqAND("l2muCombFilterSequence", [muonChainFilter, muCombRecoSequence])
    muCombIDSequence = parOR("l2muCombIDSequence", [muFastIDRecoSequence, muCombFilterSequence])

    l2muCombViewsMaker.ViewNodeName = muCombIDSequence.name()

    l2muCombSequence = seqAND("l2muCombSequence", eventAlgs + [l2muCombViewsMaker, muCombIDSequence] )

    return (l2muCombSequence, l2muCombViewsMaker, sequenceOut)



def muCombSequence():
   
    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoAlg
    #trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py. 
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoToolFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromDict )
  


def muCombOvlpRmSequence():
   
    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoToolFromDict
    
    ### set up MuSAOverlapRemoval ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMucombAlg
    trigL2MuonOverlapRemover = TrigL2MuonOverlapRemoverMucombAlg("TrigL2MuonOverlapRemoverMucombAlg")
    trigL2MuonOverlapRemover.L2MuonOverlapInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMucombToolFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = [trigmuCombHypo, trigL2MuonOverlapRemover],
                         HypoToolGen = [TrigmuCombHypoToolFromDict, TrigL2MuonOverlapRemoverMucombToolFromDict] )
 


######################
###  EFSA step ###
######################
def muEFSAAlgSequence(ConfigFlags):

    efsaViewsMaker = EventViewCreatorAlgorithm("IMefsa")
    efsaViewsMaker.ViewFallThrough = True
    efsaViewsMaker.RoIsLink = "initialRoI" # -||-
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs" # contract with the consumer
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
    efsaViewsMaker.RequireParentView = True
   
    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFSARecoSequence
    muEFSARecoSequence, sequenceOut = muEFSARecoSequence( efsaViewsMaker.InViewRoIs, 'RoI' )
 
    efsaViewsMaker.ViewNodeName = muEFSARecoSequence.name()
        
    muonEFSAonlySequence = seqAND( "muonEFSAonlySequence", [efsaViewsMaker, muEFSARecoSequence ] )

    return (muonEFSAonlySequence, efsaViewsMaker, sequenceOut)

def muEFSASequence():
    
    (muonEFSAonlySequence, efsaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAHypoAlg" )
    trigMuonEFSAHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromDict
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict )



def muEFMSSequence():
    
    (muonEFSAonlySequence, efsaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFMSonlyHypoAlg" )
    trigMuonEFSAHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromDict
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict )


######################
###  EFCB seq ###
######################
def muEFCBAlgSequence(ConfigFlags):

    #By default the EFCB sequence will run both outside-in and 
    #(if zero muons are found) inside-out reconstruction
    from TrigMuonEF.TrigMuonEFConfig import MuonFilterAlg, MergeEFMuonsAlg
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFCBRecoSequence, muEFInsideOutRecoSequence
    
    efcbViewsMaker = EventViewCreatorAlgorithm("IMefcbtotal")
    efcbViewsMaker.ViewFallThrough = True
    efcbViewsMaker.RoIsLink = "initialRoI" # -||-
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs" # contract with the consumer
    efcbViewsMaker.Views = "MUEFCBViewRoIs"
    efcbViewsMaker.RequireParentView = True

    #outside-in reco sequence
    muEFCBRecoSequence, eventAlgs, sequenceOutCB = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, "RoI" )

    #Algorithm to filter events with no muons
    muonFilter = MuonFilterAlg("FilterZeroMuons")
    muonFilter.MuonContainerLocation = sequenceOutCB
 
    #inside-out reco sequence - runs only if filter is passed
    muonEFInsideOutRecoSequence, sequenceOutInsideOut = muEFInsideOutRecoSequence( efcbViewsMaker.InViewRoIs, "RoI")
    muonInsideOutSequence = seqAND("muonEFInsideOutSequence", [muonFilter,muonEFInsideOutRecoSequence])

    #combine outside-in and inside-out sequences
    muonRecoSequence = parOR("muonEFCBandInsideOutRecoSequence", [muEFCBRecoSequence, muonInsideOutSequence])

    #Merge muon containers from outside-in and inside-out reco
    muonMerger = MergeEFMuonsAlg("MergeEFMuons")
    muonMerger.MuonCBContainerLocation = sequenceOutCB
    muonMerger.MuonInsideOutContainerLocation = sequenceOutInsideOut
    muonMerger.MuonOutputLocation = muNames.EFCBName
    sequenceOut = muonMerger.MuonOutputLocation

    #Add merging alg in sequence with reco sequences
    mergeSequence = seqOR("muonCBInsideOutMergingSequence", [muonRecoSequence, muonMerger])

    #Final sequence running in view
    efcbViewsMaker.ViewNodeName = mergeSequence.name()    
    muonSequence = seqAND("muonEFCBandInsideOutSequence", [efcbViewsMaker, mergeSequence])

    return (muonSequence, efcbViewsMaker, sequenceOut)

def muEFCBSequence():

    (muonEFCBSequence, efcbViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFCBHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFCombinerHypoAlg" )
    trigMuonEFCBHypo.MuonDecisions = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFCBSequence,
                         Maker       = efcbViewsMaker,
                         Hypo        = trigMuonEFCBHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict )

########################
### EF CB with dimuon ##
### mass cuts         ##
########################
def muEFCBInvMassSequence():

    from AthenaCommon import CfgMgr
    invMassRecoSequence = parOR("muInvMViewNode")
    
    invMViewsMaker = EventViewCreatorAlgorithm("IMmuinvm")
    invMViewsMaker.ViewFallThrough = True
    invMViewsMaker.RoIsLink = "initialRoI" # -||-
    invMViewsMaker.InViewRoIs = "muInvMRoIs" # contract with the consumer
    invMViewsMaker.Views = "muInvMViewRoIs"
    invMViewsMaker.ViewNodeName = invMassRecoSequence.name()
    invMViewsMaker.RequireParentView = True
   
    
    ViewVerifyEFCB = CfgMgr.AthViews__ViewDataVerifier("muInvMViewDataVerifier")
    ViewVerifyEFCB.DataObjects = [( 'xAOD::MuonContainer' , 'StoreGateSvc+'+muNames.EFCBName )]
    invMassRecoSequence += ViewVerifyEFCB
    invMassSequence = seqAND( "muInvMSequence", [invMViewsMaker, invMassRecoSequence] )


    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFInvMassHypoAlg, TrigMuonEFInvMassHypoToolFromDict

    trigMuonEFInvMHypo = TrigMuonEFInvMassHypoAlg( "TrigMuonEFInvMassHypoAlg" )
    trigMuonEFInvMHypo.MuonDecisions = muNames.EFCBName
    return MenuSequence( Sequence    = invMassSequence,
                         Maker       = invMViewsMaker,
                         Hypo        = trigMuonEFInvMHypo,
                         HypoToolGen = TrigMuonEFInvMassHypoToolFromDict )

######################
### EF SA full scan ###
######################
def muEFSAFSAlgSequence(ConfigFlags):

    efsafsInputMaker = EventViewCreatorAlgorithm("IMMuonFS", RoIsLink="initialRoI")
    efsafsInputMaker.InViewRoIs = "MUFSRoIs"
    efsafsInputMaker.Views = "MUFSViewRoI"
    efsafsInputMaker.ViewFallThrough=True

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFSARecoSequence
    muEFSAFSRecoSequence, sequenceOut = muEFSARecoSequence( efsafsInputMaker.InViewRoIs,'FS' )
 
    efsafsInputMaker.ViewNodeName = muEFSAFSRecoSequence.name()

    muonEFSAFSSequence = seqAND( "muonEFSAFSSequence", [efsafsInputMaker, muEFSAFSRecoSequence ] )

    return (muonEFSAFSSequence, efsafsInputMaker, sequenceOut)

def muEFSAFSSequence():

    (muonEFSAFSSequence, efsafsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAFSAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAFSHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAFSHypoAlg" )
    trigMuonEFSAFSHypo.MuonDecisions = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromDict
    
    return MenuSequence( Sequence    = muonEFSAFSSequence,
                         Maker       = efsafsInputMaker,
                         Hypo        = trigMuonEFSAFSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict )

######################
### EF CB full scan ###
######################
def muEFCBFSAlgSequence(ConfigFlags):
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithMuons
    efcbfsInputMaker = EventViewCreatorAlgorithmWithMuons("IMEFCBFS") #can we call it EFCBFSEventViewCreator?
    efcbfsInputMaker.ViewFallThrough = True
    efcbfsInputMaker.mergeOutputs = False
    efcbfsInputMaker.Views = "MUCBFSViews"
    efcbfsInputMaker.InViewRoIs = "MUCBFSRoIs"
    efcbfsInputMaker.RoIsLink = "initialRoI"
    efcbfsInputMaker.InViewMuons = "InViewMuons"
    efcbfsInputMaker.MuonsLink = "feature"
    efcbfsInputMaker.RequireParentView = True
    efcbfsInputMaker.InViewMuonCandidates = "MuonCandidates_FS"
    efcbfsInputMaker.RoisWriteHandleKey = "MuonCandidates_FS_ROIs"

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFCBRecoSequence
    muEFCBFSRecoSequence, eventAlgs, sequenceOut = muEFCBRecoSequence( efcbfsInputMaker.InViewRoIs, "FS" )
 
    efcbfsInputMaker.ViewNodeName = muEFCBFSRecoSequence.name()
        
    muonEFCBFSSequence = seqAND( "muonEFFSCBSequence", eventAlgs + [efcbfsInputMaker, muEFCBFSRecoSequence] )

    return (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut)

def muEFCBFSSequence():

    (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBFSAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFCBFSHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFFSCombinerHypoAlg" )
    trigMuonEFCBFSHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFCBFSSequence,
                         Maker       = efcbfsInputMaker,
                         Hypo        = trigMuonEFCBFSHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict )




######################
### l2Muiso step ###
######################
def muIsoAlgSequence(ConfigFlags):

    l2muIsoViewsMaker = EventViewCreatorAlgorithm("IMl2muIso")
    l2muIsoViewsMaker.ViewFallThrough = True
    l2muIsoViewsMaker.RoIsLink = "initialRoI" # -||-
    l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs" # contract with the consumer
    l2muIsoViewsMaker.Views = "MUIsoViewRoIs"
    l2muIsoViewsMaker.RequireParentView = True

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import l2muisoRecoSequence
    l2muisoRecoSequence, sequenceOut = l2muisoRecoSequence( l2muIsoViewsMaker.InViewRoIs )
 
    l2muIsoViewsMaker.ViewNodeName = l2muisoRecoSequence.name()
     
    ### Define a Sequence to run for muIso ### 
    l2muIsoSequence = seqAND("l2muIsoSequence", [ l2muIsoViewsMaker, l2muisoRecoSequence ] )

    return (l2muIsoSequence, l2muIsoViewsMaker, sequenceOut)

def muIsoSequence():
 
    (l2muIsoSequence, l2muIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muIsoAlgSequence, ConfigFlags)

    # set up hypo    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoAlg
    trigmuIsoHypo = TrigMuisoHypoAlg("L2MuisoHypoAlg")
    trigmuIsoHypo.MuonL2ISInfoName = sequenceOut
   
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoToolFromDict

    return MenuSequence( Sequence    = l2muIsoSequence,
                         Maker       = l2muIsoViewsMaker,
                         Hypo        = trigmuIsoHypo,
                         HypoToolGen = TrigMuisoHypoToolFromDict )



######################
### efMuiso step ###
######################
def muEFIsoAlgSequence(ConfigFlags):
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithMuons
    efmuIsoViewsMaker = EventViewCreatorAlgorithmWithMuons("IMefmuIso")
    efmuIsoViewsMaker.ViewFallThrough = True
    efmuIsoViewsMaker.RoIsLink = "initialRoI" # -||-
    efmuIsoViewsMaker.InViewRoIs = "MUEFIsoRoIs" # contract with the consumer
    efmuIsoViewsMaker.Views = "MUEFIsoViewRoIs"
    efmuIsoViewsMaker.InViewMuons = "IsoViewMuons"
    efmuIsoViewsMaker.MuonsLink = "feature"
    efmuIsoViewsMaker.RoIEtaWidth=0.15
    efmuIsoViewsMaker.RoIPhiWidth=0.15
    efmuIsoViewsMaker.mergeOutputs = False

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import efmuisoRecoSequence
    efmuisoRecoSequence, sequenceOut = efmuisoRecoSequence( efmuIsoViewsMaker.InViewRoIs, efmuIsoViewsMaker.InViewMuons )
 
    efmuIsoViewsMaker.ViewNodeName = efmuisoRecoSequence.name()
     
    ### Define a Sequence to run for muIso ### 
    efmuIsoSequence = seqAND("efmuIsoSequence", [ efmuIsoViewsMaker, efmuisoRecoSequence ] )

    return (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut)

def muEFIsoSequence():
 
    (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFIsoAlgSequence, ConfigFlags)

    # set up hypo    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFTrackIsolationHypoAlg
    trigmuefIsoHypo = TrigMuonEFTrackIsolationHypoAlg("EFMuisoHypoAlg")
    trigmuefIsoHypo.EFMuonsName = sequenceOut
   
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFTrackIsolationHypoToolFromDict

    return MenuSequence( Sequence    = efmuIsoSequence,
                         Maker       = efmuIsoViewsMaker,
                         Hypo        = trigmuefIsoHypo,
                         HypoToolGen = TrigMuonEFTrackIsolationHypoToolFromDict )
  
  
######################
### TrkMaterialProvider ###
######################
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)

##############################
### Get Bphysics triggers to # 
### filter chains where we   #
### don't want to run muComb #
##############################

def getBphysChainNames():

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    bphysSlice = TriggerFlags.BphysicsSlice.signatures()
    chains =[]
    if bphysSlice:
        for chain in bphysSlice:
            chains.append(chain.name)
    return chains
