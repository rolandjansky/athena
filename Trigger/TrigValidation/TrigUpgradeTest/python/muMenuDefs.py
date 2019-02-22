# 
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
#

from AthenaCommon.Include import include
from AthenaCommon.Constants import VERBOSE,DEBUG, INFO
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
import AthenaCommon.CfgMgr as CfgMgr
import AthenaCommon.CfgGetter as CfgGetter

# Set InDet Flags
def inDetSetup():
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doCaloSeededBrem = False
    InDetFlags.InDet25nsec = True 
    InDetFlags.doPrimaryVertex3DFinding = False 
    InDetFlags.doPrintConfigurables = False
    InDetFlags.doResolveBackTracks = True 
    InDetFlags.doSiSPSeededTrackFinder = True
    InDetFlags.doTRTPhaseCalculation = True
    InDetFlags.doTRTSeededTrackFinder = True
    InDetFlags.doTruth = False
    InDetFlags.init()

    ### PixelLorentzAngleSvc and SCTLorentzAngleSvc ###
    include("InDetRecExample/InDetRecConditionsAccess.py")

from InDetRecExample.InDetKeys import InDetKeys
from TriggerJobOpts.TriggerFlags import TriggerFlags
from MuonRecExample.MuonRecFlags import muonRecFlags


# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

### for Control Flow ###
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

### Used the algorithms as Step1 "muFast step" ###
### Load data from Muon detectors ###
#import MuonRecExample.MuonRecStandaloneOnlySetup
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doNSWNewThirdChain    = False
muonCombinedRecFlags.doCaloTrkMuId = False
muonCombinedRecFlags.printSummary = False
from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AthSequencer
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

ServiceMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False


### ************* Step1  ************* ###

def muFastStep():

    ### set the EVCreator ###
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
    l2MuViewsMaker.ViewFallThrough = True
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"

    ### get muFast reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muFastRecoSequence
    muFastRecoSequence, sequenceOut = muFastRecoSequence( l2MuViewsMaker.InViewRoIs, OutputLevel=DEBUG )
    
    l2MuViewsMaker.ViewNodeName = muFastRecoSequence.name() 
    
    ### set up MuFastHypo ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.OutputLevel = DEBUG
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut
    
    
    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, muFastRecoSequence ])
    
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromName
  
    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolFromName )


### ************* Step2  ************* ###

def muCombStep():

    ### set the EVCreator ###
    l2muCombViewsMaker = EventViewCreatorAlgorithm("l2muCombViewsMaker", OutputLevel=DEBUG)
    l2muCombViewsMaker.ViewFallThrough = True
    l2muCombViewsMaker.RoIsLink = "roi" # -||-
    l2muCombViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the consumer
    l2muCombViewsMaker.Views = "EMCombViewRoIs"
    
    ### get muComb reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muCombRecoSequence
    muCombRecoSequence, eventAlgs, sequenceOut, TrackParticlesName = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs, OutputLevel=DEBUG )
 
    l2muCombViewsMaker.ViewNodeName = muCombRecoSequence.name()
   
    ### set up muCombHypo algorithm ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoAlg
    #trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py. 
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.OutputLevel = DEBUG
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut
    
    l2muCombSequence = seqAND("l2muCombSequence", eventAlgs + [l2muCombViewsMaker, muCombRecoSequence] )
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoToolFromName
    
    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromName )
  

### ************* Step3  ************* ###

###  EFMSonly step ###
def muEFMSStep():

    efmsViewsMaker = EventViewCreatorAlgorithm("efmsViewsMaker", OutputLevel=DEBUG)
    efmsViewsMaker.ViewFallThrough = True
    efmsViewsMaker.RoIsLink = "initialRoI" # -||-
    efmsViewsMaker.InViewRoIs = "MUEFMSRoIs" # contract with the consumer
    efmsViewsMaker.Views = "MUEFMSViewRoIs"

    ### get EF reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muEFSARecoSequence
    muEFMSRecoSequence, sequenceOut = muEFSARecoSequence( efmsViewsMaker.InViewRoIs, 'RoI', OutputLevel=DEBUG )
 
    efmsViewsMaker.ViewNodeName = muEFMSRecoSequence.name()
    
    # setup MS-only hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFMSHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFMSHypoAlg" )
    trigMuonEFMSHypo.OutputLevel = DEBUG
    trigMuonEFMSHypo.MuonDecisions = sequenceOut
    
    muonEFMSonlySequence = seqAND( "muonEFMSonlySequence", [efmsViewsMaker, muEFMSRecoSequence] )
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFMSonlySequence,
                         Maker       = efmsViewsMaker,
                         Hypo        = trigMuonEFMSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )

###  EFSA step ###
def muEFSAStep():

    efsaViewsMaker = EventViewCreatorAlgorithm("efsaViewsMaker", OutputLevel=DEBUG)
    efsaViewsMaker.ViewFallThrough = True
    efsaViewsMaker.RoIsLink = "roi" # -||-
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs" # contract with the consumer
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
   
    ### get EF reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muEFSARecoSequence
    muEFSARecoSequence, sequenceOut = muEFSARecoSequence( efsaViewsMaker.InViewRoIs, 'RoI', OutputLevel=DEBUG )
 
    efsaViewsMaker.ViewNodeName = muEFSARecoSequence.name()
    
    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAHypoAlg" )
    trigMuonEFSAHypo.OutputLevel = DEBUG
    trigMuonEFSAHypo.MuonDecisions = sequenceOut
    
    muonEFSAonlySequence = seqAND( "muonEFSAonlySequence", [efsaViewsMaker, muEFSARecoSequence ] )
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )

def muEFCBStep():

    efcbViewNode = parOR("efcbViewNode")
    
    efcbViewsMaker = EventViewCreatorAlgorithm("efcbViewsMaker", OutputLevel=DEBUG)
    efcbViewsMaker.ViewFallThrough = True
    efcbViewsMaker.RoIsLink = "roi" # -||-
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs" # contract with the consumer
    efcbViewsMaker.Views = "MUEFCBViewRoIs"
    efcbViewsMaker.ViewNodeName = efcbViewNode.name()
   

    ### get EF reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muEFCBRecoSequence
    muEFCBRecoSequence, sequenceOut = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, OutputLevel=DEBUG )
 
    efcbViewsMaker.ViewNodeName = muEFCBRecoSequence.name()
    
    
    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFCBHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFCombinerHypoAlg" )
    trigMuonEFCBHypo.OutputLevel = DEBUG
    trigMuonEFCBHypo.MuonDecisions = sequenceOut
    
    muonEFCBSequence = seqAND( "muonEFCBSequence", [efcbViewsMaker, efcbViewNode] )

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromName

    return MenuSequence( Sequence    = muonEFCBSequence,
                         Maker       = efcbViewsMaker,
                         Hypo        = trigMuonEFCBHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromName )

### EF SA full scan ###
def muEFSAFSStep():

    efsafsInputMaker = EventViewCreatorAlgorithm("MuonFSInputMaker", RoIsLink="initialRoI",OutputLevel=VERBOSE)
    efsafsInputMaker.InViewRoIs = "MUFSRoIs"
    efsafsInputMaker.Views = "MUFSViewRoI"
    efsafsInputMaker.ViewPerRoI=True
    efsafsInputMaker.ViewFallThrough=True

    ### get EF reco sequence ###    
    from TrigUpgradeTest.MuonSetup import muEFSARecoSequence
    muEFSAFSRecoSequence, sequenceOut = muEFSARecoSequence( efsafsInputMaker.InViewRoIs,'FS', OutputLevel=DEBUG )
 
    efsafsInputMaker.ViewNodeName = muEFSAFSRecoSequence.name()

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAFSHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAFSHypoAlg" )
    trigMuonEFSAFSHypo.OutputLevel = DEBUG
    trigMuonEFSAFSHypo.MuonDecisions = sequenceOut

    muonEFSAFSSequence = seqAND( "muonEFSAFSSequence", [efsafsInputMaker, muEFSAFSRecoSequence ] )
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFSAFSSequence,
                         Maker       = efsafsInputMaker,
                         Hypo        = trigMuonEFSAFSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )


### l2Muiso step ###
def muIsoStep():

    l2muIsoViewsMaker = EventViewCreatorAlgorithm("l2muIsoViewsMaker", OutputLevel=DEBUG)
    l2muIsoViewsMaker.ViewFallThrough = True
    l2muIsoViewsMaker.RoIsLink = "roi" # -||-
    l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs" # contract with the consumer
    l2muIsoViewsMaker.Views = "MUIsoViewRoIs"

    ### get EF reco sequence ###    
    from TrigUpgradeTest.MuonSetup import l2muisoRecoSequence
    l2muisoRecoSequence, sequenceOut = l2muisoRecoSequence( l2muIsoViewsMaker.InViewRoIs, OutputLevel=DEBUG )
 
    l2muIsoViewsMaker.ViewNodeName = l2muisoRecoSequence.name()
 
    # set up hypo    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoAlg
    trigmuIsoHypo = TrigMuisoHypoAlg("L2MuisoHypoAlg")
    trigmuIsoHypo.OutputLevel = DEBUG
    trigmuIsoHypo.MuonL2ISInfoName = sequenceOut
    
    ### Define a Sequence to run for muIso ### 
    l2muIsoSequence = seqAND("l2muIsoSequence", [ l2muIsoViewsMaker, l2muisoRecoSequence ] )
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoToolFromName

    return MenuSequence( Sequence    = l2muIsoSequence,
                         Maker       = l2muIsoViewsMaker,
                         Hypo        = trigmuIsoHypo,
                         HypoToolGen = TrigMuisoHypoToolFromName )
  
  
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
