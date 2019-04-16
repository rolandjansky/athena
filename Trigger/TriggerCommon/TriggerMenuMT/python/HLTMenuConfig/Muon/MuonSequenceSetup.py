# 
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
# 

from AthenaCommon.AppMgr import ServiceMgr

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
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
muonRecFlags.doNSWNewThirdChain    = False
muonCombinedRecFlags.doCaloTrkMuId = False
muonCombinedRecFlags.printSummary = False

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

ServiceMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False


#-----------------------------------------------------#
### ************* Step1  ************* ###
#-----------------------------------------------------#
def muFastAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker")
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
                         HypoToolGen = TrigMufastHypoToolFromDict )


#-----------------------------------------------------#
### ************* Step2  ************* ###
#-----------------------------------------------------#
def muCombAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2muCombViewsMaker = EventViewCreatorAlgorithm("l2muCombViewsMaker")
    l2muCombViewsMaker.ViewFallThrough = True
    l2muCombViewsMaker.RoIsLink = "roi" # -||-
    l2muCombViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the consumer
    l2muCombViewsMaker.Views = "EMCombViewRoIs"
    
    ### get muComb reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import muCombRecoSequence
    muCombRecoSequence, eventAlgs, sequenceOut, TrackParticlesName = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs )
 
    l2muCombViewsMaker.ViewNodeName = muCombRecoSequence.name()
       
    l2muCombSequence = seqAND("l2muCombSequence", eventAlgs + [l2muCombViewsMaker, muCombRecoSequence] )

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
  


######################
###  EFSA step ###
######################
def muEFSAAlgSequence(ConfigFlags):

    efsaViewsMaker = EventViewCreatorAlgorithm("efsaViewsMaker")
    efsaViewsMaker.ViewFallThrough = True
    efsaViewsMaker.RoIsLink = "roi" # -||-
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs" # contract with the consumer
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
   
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
    efcbViewNode = parOR("efcbViewNode")
    
    efcbViewsMaker = EventViewCreatorAlgorithm("efcbViewsMaker")
    efcbViewsMaker.ViewFallThrough = True
    efcbViewsMaker.RoIsLink = "roi" # -||-
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs" # contract with the consumer
    efcbViewsMaker.Views = "MUEFCBViewRoIs"
    efcbViewsMaker.ViewNodeName = efcbViewNode.name()
   

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFCBRecoSequence
    muEFCBRecoSequence, eventAlgs, sequenceOut = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, "RoI" )
 
    efcbViewsMaker.ViewNodeName = muEFCBRecoSequence.name()    
    
    muonEFCBSequence = seqAND( "muonEFCBSequence", [efcbViewsMaker, efcbViewNode] )
    return (muonEFCBSequence, efcbViewsMaker, sequenceOut)

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

######################
### EF SA full scan ###
######################
def muEFSAFSAlgSequence(ConfigFlags):

    efsafsInputMaker = EventViewCreatorAlgorithm("MuonFSInputMaker", RoIsLink="initialRoI")
    efsafsInputMaker.InViewRoIs = "MUFSRoIs"
    efsafsInputMaker.Views = "MUFSViewRoI"
    efsafsInputMaker.ViewPerRoI=True
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
    efcbfsInputMaker = EventViewCreatorAlgorithmWithMuons("EFCBFSInputMaker")
    efcbfsInputMaker.ViewFallThrough = True
    efcbfsInputMaker.ViewPerRoI = True
    efcbfsInputMaker.Views = "MUCBFSViews"
    efcbfsInputMaker.InViewRoIs = "MUCBFSRoIs"
    efcbfsInputMaker.RoIsLink = "roi"
    efcbfsInputMaker.InViewMuons = "InViewMuons"
    efcbfsInputMaker.MuonsLink = "feature"

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

    l2muIsoViewsMaker = EventViewCreatorAlgorithm("l2muIsoViewsMaker")
    l2muIsoViewsMaker.ViewFallThrough = True
    l2muIsoViewsMaker.RoIsLink = "roi" # -||-
    l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs" # contract with the consumer
    l2muIsoViewsMaker.Views = "MUIsoViewRoIs"

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
### TrkMaterialProvider ###
######################
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
