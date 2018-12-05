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

from AthenaCommon import CfgMgr


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False

from InDetRecExample.InDetJobProperties import InDetFlags
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

### Output data name ###
muFastInfo = "MuonL2SAInfo"
muCombInfo = "MuonL2CBInfo"
muEFSAInfo = "Muons"
muL2ISInfo = "MuonL2ISInfo"
# get Track container name
TrackParticlesName = ""
from TrigUpgradeTest.InDetSetup import makeInDetAlgs
(nameAlgs, evAlgs) = makeInDetAlgs()
for nameAlg in nameAlgs:
    if nameAlg.name() == "InDetTrigTrackParticleCreatorAlg":
        TrackParticlesName = nameAlg.TrackParticlesName
 

### ************* Step1  ************* ###

def muFastStep():

    ### set the EVCreator ###
    l2MuViewNode = parOR("l2MuViewNode")
    
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
    l2MuViewsMaker.ViewFallThrough = True
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.ViewNodeName = l2MuViewNode.name()

     ### set up muFast algs ###    
    from TrigUpgradeTest.MuonSetup import makeMuFastAlgs
    muFastAlg = makeMuFastAlgs()
    muFastAlg.OutputLevel = DEBUG
    muFastAlg.RecMuonRoI = "RecMURoIs"
    muFastAlg.MuRoIs = l2MuViewsMaker.InViewRoIs 
    muFastAlg.MuonL2SAInfo = muFastInfo 
    muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
    muFastAlg.forID = "forID"
    muFastAlg.forMS = "forMS"
    
    l2MuViewNode += muFastAlg
    
    ### set up MuFastHypo ###
    from TrigMuonHypo.TrigMuonHypoConf import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.OutputLevel = DEBUG
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = muFastInfo
    
    
    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, l2MuViewNode ])
    
    
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMufastHypoToolFromName
  
    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolFromName )


### ************* Step2  ************* ###

def muCombStep():

    ### set the EVCreator ###
    l2muCombViewNode = parOR("l2muCombViewNode")
    
    l2muCombViewsMaker = EventViewCreatorAlgorithm("l2muCombViewsMaker", OutputLevel=DEBUG)
    l2muCombViewsMaker.ViewFallThrough = True
    l2muCombViewsMaker.RoIsLink = "roi" # -||-
    l2muCombViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the consumer
    l2muCombViewsMaker.Views = "EMCombViewRoIs"
    l2muCombViewsMaker.ViewNodeName = l2muCombViewNode.name()
    
    ### Define input data of Inner Detector algorithms  ###
    ### and Define EventViewNodes to run the algorithms ###
    from TrigUpgradeTest.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs()
    
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theFTF_Muon = TrigFastTrackFinder_Muon()
    theFTF_Muon.OutputLevel = DEBUG
    theFTF_Muon.isRoI_Seeded = True
    viewAlgs.append(theFTF_Muon)
    
    ### A simple algorithm to confirm that data has been inherited from parent view ###
    ### Required to satisfy data dependencies                                       ###
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+'+muFastInfo)]
    viewAlgs.append(ViewVerify)
    
    for viewAlg in viewAlgs:
        l2muCombViewNode += viewAlg
        if viewAlg.properties().has_key("RoIs"):
            viewAlg.RoIs = l2muCombViewsMaker.InViewRoIs
        if viewAlg.properties().has_key("roiCollectionName"):
            viewAlg.roiCollectionName = l2muCombViewsMaker.InViewRoIs
    
    ### please read out TrigmuCombMTConfig file ###
    ### and set up to run muCombMT algorithm    ###
    from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
    muCombAlg = TrigmuCombMTConfig("Muon", theFTF_Muon.getName())
    muCombAlg.OutputLevel = DEBUG
    muCombAlg.L2StandAloneMuonContainerName = muFastInfo
    muCombAlg.TrackParticlesContainerName = TrackParticlesName
    muCombAlg.L2CombinedMuonContainerName = muCombInfo
    
    l2muCombViewNode += muCombAlg
    
    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConf import TrigmuCombHypoAlg
    #trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py. 
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.OutputLevel = DEBUG
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = muCombInfo 
    
    l2muCombSequence = seqAND("l2muCombSequence", eventAlgs + [l2muCombViewsMaker, l2muCombViewNode ] )
    
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigmuCombHypoToolFromName
    
    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromName )
  

### ************* Step3  ************* ###

###  EFMSonly step ###
def muEFMSStep():

    efmsViewNode = parOR("efmsViewNode")
    
    efmsViewsMaker = EventViewCreatorAlgorithm("efmsViewsMaker", OutputLevel=DEBUG)
    efmsViewsMaker.ViewFallThrough = True
    efmsViewsMaker.RoIsLink = "initialRoI" # -||-
    efmsViewsMaker.InViewRoIs = "MUEFMSRoIs" # contract with the consumer
    efmsViewsMaker.Views = "MUEFMSViewRoIs"
    efmsViewsMaker.ViewNodeName = efmsViewNode.name()

    # setup muEFMsonly algs
    from TrigUpgradeTest.MuonSetup import makeMuEFSAAlgs
    efAlgs = makeMuEFSAAlgs()    
    for efAlg in efAlgs:
        if efAlg.properties().has_key("RoIs"):
            efAlg.RoIs = efmsViewsMaker.InViewRoIs
        efmsViewNode += efAlg
    
    # setup MS-only hypo
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuonEFMSonlyHypoAlg
    trigMuonEFMSHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFMSHypoAlg" )
    trigMuonEFMSHypo.OutputLevel = DEBUG
    trigMuonEFMSHypo.MuonDecisions = muEFSAInfo
    
    muonEFMSonlySequence = seqAND( "muonEFMSonlySequence", [efmsViewsMaker, efmsViewNode] )
    
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFMSonlySequence,
                         Maker       = efmsViewsMaker,
                         Hypo        = trigMuonEFMSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )

###  EFSA step ###
def muEFSAStep():

    efsaViewNode = parOR("efsaViewNode")
    
    efsaViewsMaker = EventViewCreatorAlgorithm("efsaViewsMaker", OutputLevel=DEBUG)
    efsaViewsMaker.ViewFallThrough = True
    #efsaViewsMaker.RoIsLink = "initialRoI" # -||-
    efsaViewsMaker.RoIsLink = "roi" # -||-
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs" # contract with the consumer
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
    efsaViewsMaker.ViewNodeName = efsaViewNode.name()
   
    # setup muEFMsonly algs
    from TrigUpgradeTest.MuonSetup import makeMuEFSAAlgs
    efAlgs = makeMuEFSAAlgs()    
    for efAlg in efAlgs:
        if efAlg.properties().has_key("RoIs"):
            efAlg.RoIs = efsaViewsMaker.InViewRoIs
        efsaViewNode += efAlg
    
    # setup EFSA hypo
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAHypoAlg" )
    trigMuonEFSAHypo.OutputLevel = DEBUG
    trigMuonEFSAHypo.MuonDecisions = muEFSAInfo
    
    muonEFSAonlySequence = seqAND( "muonEFSAonlySequence", [efsaViewsMaker, efsaViewNode] )
    
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )

### l2Muiso step ###
def muIsoStep():

    l2muIsoViewNode = parOR("l2muIsoViewNode")
    
    l2muIsoViewsMaker = EventViewCreatorAlgorithm("l2muIsoViewsMaker", OutputLevel=DEBUG)
    l2muIsoViewsMaker.ViewFallThrough = True
    l2muIsoViewsMaker.RoIsLink = "roi" # -||-
    l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs" # contract with the consumer
    l2muIsoViewsMaker.Views = "MUIsoViewRoIs"
    l2muIsoViewsMaker.ViewNodeName = l2muIsoViewNode.name()

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muCombViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TrackParticleContainer' , 'StoreGateSvc+'+TrackParticlesName),
                              ('xAOD::L2CombinedMuonContainer','StoreGateSvc+'+muCombInfo)]
    l2muIsoViewNode += ViewVerify 
 
    # set up algs    
    from TrigmuIso.TrigmuIsoConfig import TrigmuIsoMTConfig
    trigL2muIso = TrigmuIsoMTConfig("TrigL2muIso")
    trigL2muIso.OutputLevel = DEBUG
    trigL2muIso.MuonL2CBInfoName = muCombInfo
    trigL2muIso.TrackParticlesName = TrackParticlesName
    trigL2muIso.MuonL2ISInfoName = muL2ISInfo
    
    l2muIsoViewNode += trigL2muIso

    # set up hypo    
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuisoHypoAlg
    trigmuIsoHypo = TrigMuisoHypoAlg("L2MuisoHypoAlg")
    trigmuIsoHypo.OutputLevel = DEBUG
    trigmuIsoHypo.MuonL2ISInfoName = muL2ISInfo
    
    ### Define a Sequence to run for muIso ### 
    l2muIsoSequence = seqAND("l2muIsoSequence", [ l2muIsoViewsMaker, l2muIsoViewNode ] )
    
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMuisoHypoToolFromName

    return MenuSequence( Sequence    = l2muIsoSequence,
                         Maker       = l2muIsoViewsMaker,
                         Hypo        = trigmuIsoHypo,
                         HypoToolGen = TrigMuisoHypoToolFromName )
  
  
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
