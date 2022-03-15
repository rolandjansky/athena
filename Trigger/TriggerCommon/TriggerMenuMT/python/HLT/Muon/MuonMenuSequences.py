#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from ..Config.MenuComponents import MenuSequence, RecoFragmentsPool, algorithmCAToGlobalWrapper
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

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
muonCombinedRecFlags.doSiAssocForwardMuons = False
muonCombinedRecFlags.doStatisticalCombination = False
muonCombinedRecFlags.doCombinedFit = True
muonRecFlags.enableErrorTuning = False

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool, ViewCreatorNamedROITool, \
  ViewCreatorFSROITool, ViewCreatorCentredOnIParticleROITool, ViewCreatorFetchFromViewROITool

#muon container names (for RoI based sequences)
from .MuonRecoSequences import muonNames
muNames = muonNames().getNames('RoI')
from TrigEDMConfig.TriggerEDMRun3 import recordable

#-----------------------------------------------------#
### ************* Step1  ************* ###
#-----------------------------------------------------#
def muFastAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2MuViewsMaker = EventViewCreatorAlgorithm("IMl2Mu")
    #
    l2MuViewsMaker.RoIsLink = "initialRoI" # ROI is from L1
    l2MuViewsMaker.RoITool = ViewCreatorInitialROITool() # ROI is from L1
    #
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.InViewRoIs = "MURoIs"
    #
    l2MuViewsMaker.ViewFallThrough = True

    ### get muFast reco sequence ###    
    from .MuonRecoSequences import muFastRecoSequence, muonDecodeCfg
    viewAlgs_MuonPRD = algorithmCAToGlobalWrapper(muonDecodeCfg,ConfigFlags,RoIs=l2MuViewsMaker.InViewRoIs.path())
    
    ##### L2 mutli-track mode #####
    from TrigMuonEF.TrigMuonEFConf import MuonChainFilterAlg
    from TriggerMenuMT.HLT.Config.ControlFlow.MenuComponentsNaming import CFNaming
    MultiTrackChainFilter = MuonChainFilterAlg("SAFilterMultiTrackChains")
    MultiTrackChains = getMultiTrackChainNames()
    MultiTrackChainFilter.ChainsToFilter = MultiTrackChains
    MultiTrackChainFilter.InputDecisions = [ CFNaming.inputMakerOutName(l2MuViewsMaker.name()) ]
    MultiTrackChainFilter.L2MuFastContainer = muNames.L2SAName+"l2mtmode"
    MultiTrackChainFilter.L2MuCombContainer = muNames.L2CBName+"l2mtmode"
    MultiTrackChainFilter.WriteMuFast = True
    MultiTrackChainFilter.NotGate = True

    extraLoads = []

    for decision in MultiTrackChainFilter.InputDecisions:
      extraLoads += [( 'xAOD::TrigCompositeContainer', 'StoreGateSvc+%s' % decision )]

    from .MuonRecoSequences  import  isCosmic
    muFastRecoSeq, sequenceOut = muFastRecoSequence( l2MuViewsMaker.InViewRoIs, doFullScanID= isCosmic(), extraLoads=extraLoads )

    muFastl2mtRecoSeq, sequenceOutL2mtSA = muFastRecoSequence( l2MuViewsMaker.InViewRoIs, doFullScanID= isCosmic(), l2mtmode=True )
    muFastl2mtFilterSequence = seqAND("muFastl2mtFilterSequence", [MultiTrackChainFilter, muFastl2mtRecoSeq])

    #muFastSequence = parOR("muFastRecoSequence", [viewAlgs_MuonPRD, muFastRecoSequence])
    muFastSequence = parOR("muFastRecoSequence", [viewAlgs_MuonPRD, muFastRecoSeq, muFastl2mtFilterSequence])
    l2MuViewsMaker.ViewNodeName = muFastSequence.name()

    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, muFastSequence ])
    return (l2muFastSequence, l2MuViewsMaker, sequenceOut)

def muFastSequence(is_probe_leg=False):

    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoToolFromDict

    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolFromDict,
                         IsProbe     = is_probe_leg)


def muFastOvlpRmSequence(is_probe_leg=False):

    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoToolwORFromDict

    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolwORFromDict,
                         IsProbe     = is_probe_leg )


def mul2mtSAOvlpRmSequence(is_probe_leg=False):

    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2mtMufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = muNames.L2SAName+"l2mtmode"

    from TrigMuonHypo.TrigMuonHypoConfig import Trigl2mtSAHypoToolwORFromDict

    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = Trigl2mtSAHypoToolwORFromDict,
                         IsProbe     = is_probe_leg )


#-----------------------------------------------------#
### ************* Step2  ************* ###
#-----------------------------------------------------#
def muCombAlgSequence(ConfigFlags):
    ### set the EVCreator ###
    l2muCombViewsMaker = EventViewCreatorAlgorithm("IMl2muComb")
    newRoITool = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_L2SAMuon") #RoI collection recorded to EDM
    newRoITool.InViewRoIs = muNames.L2forIDName #input RoIs from L2 SA views

    ### get ID tracking and muComb reco sequences ###
    from .MuonRecoSequences  import muFastRecoSequence, muCombRecoSequence, muonIDFastTrackingSequence, muonIDCosmicTrackingSequence, isCosmic
    #
    l2muCombViewsMaker.RoIsLink = "initialRoI" # ROI for merging is still from L1, we get exactly one L2 SA muon per L1 ROI
    l2muCombViewsMaker.RoITool = newRoITool # Create a new ROI centred on the L2 SA muon from Step 1
    #
    l2muCombViewsMaker.Views = "MUCombViewRoIs" if not isCosmic() else "CosmicViewRoIs" #output of the views maker (key in "storegate")
    l2muCombViewsMaker.InViewRoIs = "MUIDRoIs" # Name of the RoI collection inside of the view, holds the single ROI used to seed the View.
    #
    l2muCombViewsMaker.RequireParentView = True
    l2muCombViewsMaker.ViewFallThrough = True #if this needs to access anything from the previous step, from within the view

    muCombRecoSeq, sequenceOut = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs, "FTF", l2mtmode=False )

    # for L2 multi-track SA
    from TrigMuonEF.TrigMuonEFConf import MuonChainFilterAlg
    from TriggerMenuMT.HLT.Config.ControlFlow.MenuComponentsNaming import CFNaming
    MultiTrackChainFilter = MuonChainFilterAlg("CBFilterMultiTrackChains")
    MultiTrackChains = getMultiTrackChainNames()
    MultiTrackChainFilter.ChainsToFilter = MultiTrackChains
    MultiTrackChainFilter.InputDecisions = [ CFNaming.inputMakerOutName(l2muCombViewsMaker.name()) ]
    MultiTrackChainFilter.L2MuFastContainer = muNames.L2SAName+"l2mtmode"
    MultiTrackChainFilter.L2MuCombContainer = muNames.L2CBName+"l2mtmode"
    MultiTrackChainFilter.WriteMuComb = True
    MultiTrackChainFilter.NotGate = True

    extraLoadsForl2mtmode = []

    for decision in MultiTrackChainFilter.InputDecisions:
      extraLoadsForl2mtmode += [( 'xAOD::TrigCompositeContainer', 'StoreGateSvc+%s' % decision )]

    muCombl2mtRecoSeq, sequenceOutL2mtCB = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs, "FTF", l2mtmode=True )
    muCombl2mtFilterSequence = seqAND("l2mtmuCombFilterSequence", [MultiTrackChainFilter, muCombl2mtRecoSeq])

    #Filter algorithm to run muComb only if non-Bphysics muon chains are active
    muonChainFilter = MuonChainFilterAlg("FilterBphysChains")
    muonChainFilter.ChainsToFilter = getBphysChainNames()
    muonChainFilter.InputDecisions = [ CFNaming.inputMakerOutName(l2muCombViewsMaker.name()) ]
    muonChainFilter.L2MuCombContainer = sequenceOut
    muonChainFilter.WriteMuFast = False
    muonChainFilter.WriteMuComb = True

    # for nominal muComb
    muCombFilterSequence = seqAND("l2muCombFilterSequence", [muonChainFilter, muCombRecoSeq])

    extraLoads = []

    for decision in muonChainFilter.InputDecisions:
      extraLoads += [( 'xAOD::TrigCompositeContainer' , 'StoreGateSvc+%s' % decision )]

    if isCosmic():
        muTrigIDRecoSequence = muonIDCosmicTrackingSequence( l2muCombViewsMaker.InViewRoIs , "", extraLoads )
    else:
        muTrigIDRecoSequence = muonIDFastTrackingSequence( l2muCombViewsMaker.InViewRoIs , "", extraLoads, extraLoadsForl2mtmode )


    # for Inside-out L2SA
    from .MuonRecoSequences  import isCosmic
    muFastIORecoSequence, sequenceOutL2SAIO = muFastRecoSequence( l2muCombViewsMaker.InViewRoIs, doFullScanID= isCosmic() , InsideOutMode=True )
    insideoutMuonChainFilter = MuonChainFilterAlg("FilterInsideOutMuonChains")
    insideoutMuonChains = getInsideOutMuonChainNames()
    insideoutMuonChainFilter.ChainsToFilter = insideoutMuonChains
    insideoutMuonChainFilter.InputDecisions = [ CFNaming.inputMakerOutName(l2muCombViewsMaker.name()) ]
    insideoutMuonChainFilter.L2MuFastContainer = sequenceOutL2SAIO
    insideoutMuonChainFilter.L2MuCombContainer = muNames.L2CBName+"IOmode"
    insideoutMuonChainFilter.WriteMuFast = True
    insideoutMuonChainFilter.WriteMuComb = True
    insideoutMuonChainFilter.NotGate = True

    muFastIOFilterSequence = seqAND("l2muFastIOFilterSequence", [insideoutMuonChainFilter, muFastIORecoSequence])

    muCombIDSequence = parOR("l2muCombIDSequence", [muTrigIDRecoSequence, muCombFilterSequence, muFastIOFilterSequence, muCombl2mtFilterSequence])

    l2muCombViewsMaker.ViewNodeName = muCombIDSequence.name()

    l2muCombSequence = seqAND("l2muCombSequence", [l2muCombViewsMaker, muCombIDSequence] )

    return (l2muCombSequence, l2muCombViewsMaker, sequenceOut)



def muCombSequence(is_probe_leg=False):

    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoAlg
    #trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py.
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoToolFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromDict,
                         IsProbe     = is_probe_leg )


def muCombLRTAlgSequence(ConfigFlags):
    ### set the EVCreator ###
    l2muCombLRTViewsMaker = EventViewCreatorAlgorithm("IMl2muCombLRT")
    newRoITool = ViewCreatorCentredOnIParticleROITool()

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDConfig = getInDetTrigConfig("muonLRT")
    newRoITool.RoIEtaWidth=IDConfig.etaHalfWidth
    newRoITool.RoIPhiWidth=IDConfig.phiHalfWidth
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_L2SAMuon_LRT") #RoI collection recorded to EDM

    #
    l2muCombLRTViewsMaker.RoIsLink = "initialRoI" # ROI for merging is still from L1, we get exactly one L2 SA muon per L1 ROI
    l2muCombLRTViewsMaker.RoITool = newRoITool # Create a new ROI centred on the L2 SA muon from Step 1
    #
    l2muCombLRTViewsMaker.Views = "MUCombLRTViewRoIs" #output of the views maker (key in "storegate")
    l2muCombLRTViewsMaker.InViewRoIs = "MUIDRoIs" # Name of the RoI collection inside of the view, holds the single ROI used to seed the View.
    #
    l2muCombLRTViewsMaker.RequireParentView = True
    l2muCombLRTViewsMaker.ViewFallThrough = True #if this needs to access anything from the previous step, from within the view

    ### get ID tracking and muComb reco sequences ###
    from .MuonRecoSequences  import muCombRecoSequence, muonIDFastTrackingSequence

    muCombLRTRecoSequence, sequenceOut = muCombRecoSequence( l2muCombLRTViewsMaker.InViewRoIs, "FTF_LRT" )

    extraLoads = []

    muFastIDRecoSequence = muonIDFastTrackingSequence( l2muCombLRTViewsMaker.InViewRoIs , "LRT", extraLoads, doLRT=True )

    muCombLRTIDSequence = parOR("l2muCombLRTIDSequence", [muFastIDRecoSequence, muCombLRTRecoSequence])

    l2muCombLRTViewsMaker.ViewNodeName = muCombLRTIDSequence.name()

    l2muCombLRTSequence = seqAND("l2muCombLRTSequence", [l2muCombLRTViewsMaker, muCombLRTIDSequence] )

    return (l2muCombLRTSequence, l2muCombLRTViewsMaker, sequenceOut)



def muCombLRTSequence(is_probe_leg=False):

    (l2muCombLRTSequence, l2muCombLRTViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombLRTAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg_LRT")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut
    trigmuCombHypo.RoILinkName = "l2lrtroi"

    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoToolFromDict

    return MenuSequence( Sequence    = l2muCombLRTSequence,
                         Maker       = l2muCombLRTViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromDict,
                         IsProbe     = is_probe_leg )


def muCombOvlpRmSequence(is_probe_leg=False):

    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoToolwORFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolwORFromDict,
                         IsProbe     = is_probe_leg )



def mul2IOOvlpRmSequence(is_probe_leg=False):

    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBIOHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = muNames.L2CBName+"IOmode"

    # from TrigMuonHypo.TrigMuonHypoConfig import TrigL2MuonOverlapRemoverMucombToolFromDict
    from TrigMuonHypo.TrigMuonHypoConfig import Trigl2IOHypoToolwORFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = Trigl2IOHypoToolwORFromDict,
                         IsProbe     = is_probe_leg )


def mul2mtCBOvlpRmSequence(is_probe_leg=False):

    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2mtMuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = muNames.L2CBName+"l2mtmode"

    from TrigMuonHypo.TrigMuonHypoConfig import Trigl2mtCBHypoToolwORFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = Trigl2mtCBHypoToolwORFromDict,
                         IsProbe     = is_probe_leg )



######################
###  EFSA step ###
######################
def muEFSAAlgSequence(ConfigFlags):

    efsaViewsMaker = EventViewCreatorAlgorithm("IMefsa")
    #
    efsaViewsMaker.RoIsLink = "initialRoI" # Merge based on initial RoI

    newRoITool = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_L2SAMuonForEF") #RoI collection recorded to EDM
    newRoITool.InViewRoIs = "forMS" #input RoIs from L2 SA views
    newRoITool.ViewToFetchFrom = "MUViewRoIs"
    efsaViewsMaker.RoITool = newRoITool # Create a new ROI centred on the L2 SA muon from Step 1
    #
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs"
    #
    efsaViewsMaker.RequireParentView = True
    efsaViewsMaker.ViewFallThrough = True

    from .MuonRecoSequences import muEFSARecoSequence, muonDecodeCfg
    #Run decoding again since we are using updated RoIs
    viewAlgs_MuonPRD = algorithmCAToGlobalWrapper(muonDecodeCfg,ConfigFlags,RoIs=efsaViewsMaker.InViewRoIs.path())
    ### get EF reco sequence ###    
    muEFSARecoSequence, sequenceOut = muEFSARecoSequence( efsaViewsMaker.InViewRoIs, 'RoI' )

    muefSASequence = parOR("muEFSARecoSequence", [viewAlgs_MuonPRD, muEFSARecoSequence])
    efsaViewsMaker.ViewNodeName = muefSASequence.name()

    muonEFSAonlySequence = seqAND( "muonEFSAonlySequence", [efsaViewsMaker, muefSASequence ] )

    return (muonEFSAonlySequence, efsaViewsMaker, sequenceOut)

def muEFSASequence(is_probe_leg=False):

    (muonEFSAonlySequence, efsaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFSAHypo = TrigMuonEFHypoAlg( "TrigMuonEFSAHypoAlg" )
    trigMuonEFSAHypo.MuonDecisions = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromDict

    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict,
                         IsProbe     = is_probe_leg )



######################
###  EFCB seq ###
######################
def muEFCBAlgSequence(ConfigFlags):

    #By default the EFCB sequence will run both outside-in and
    #(if zero muons are found) inside-out reconstruction
    from TrigMuonEF.TrigMuonEFConf import MuonFilterAlg, MergeEFMuonsAlg
    from .MuonRecoSequences import muEFCBRecoSequence, muEFInsideOutRecoSequence

    efcbViewsMaker = EventViewCreatorAlgorithm("IMefcbtotal")
    #
    efcbViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    efcbViewsMaker.RoITool = ViewCreatorNamedROITool(ROILinkName="l2cbroi") # Spawn EventViews based on L2 CB RoIs
    #
    from .MuonRecoSequences import isCosmic
    efcbViewsMaker.Views = "MUEFCBViewRoIs" if not isCosmic() else "CosmicEFCBViewRoIs"
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs"
    #
    efcbViewsMaker.RequireParentView = True
    efcbViewsMaker.ViewFallThrough = True
    efcbViewsMaker.mergeUsingFeature = True

    #outside-in reco sequence
    muEFCBRecoSequence, sequenceOutCB = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, "RoI" )

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

def muEFCBSequence(is_probe_leg=False):

    (muonEFCBSequence, efcbViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFCBHypo = TrigMuonEFHypoAlg( "TrigMuonEFCombinerHypoAlg" )
    trigMuonEFCBHypo.MuonDecisions = sequenceOut
    trigMuonEFCBHypo.MapToPreviousDecisions=True

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFCBSequence,
                         Maker       = efcbViewsMaker,
                         Hypo        = trigMuonEFCBHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict,
                         IsProbe     = is_probe_leg )



def muEFCBLRTAlgSequence(ConfigFlags):

    from .MuonRecoSequences import muEFCBRecoSequence

    efcbViewsMaker = EventViewCreatorAlgorithm("IMefcblrttotal")
    #
    efcbViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    efcbViewsMaker.RoITool = ViewCreatorNamedROITool(ROILinkName="l2lrtroi") # Spawn EventViews based on L2 CB RoIs
    #
    efcbViewsMaker.Views = "MUEFCBLRTViewRoIs"
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs"
    #
    efcbViewsMaker.RequireParentView = True
    efcbViewsMaker.ViewFallThrough = True
    efcbViewsMaker.mergeUsingFeature = True

    #outside-in reco sequence
    muEFCBRecoSequence, sequenceOut = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, "LRT" )

    #Final sequence running in view
    efcbViewsMaker.ViewNodeName = muEFCBRecoSequence.name()
    muonSequence = seqAND("muonEFCBLRTSequence", [efcbViewsMaker, muEFCBRecoSequence])

    return (muonSequence, efcbViewsMaker, sequenceOut)

def muEFCBLRTSequence(is_probe_leg=False):

    (muonEFCBLRTSequence, efcbViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBLRTAlgSequence, ConfigFlags)

    # setup EFCBLRT hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFCBLRTHypo = TrigMuonEFHypoAlg( "TrigMuonEFCombinerHypoAlgLRT" )
    trigMuonEFCBLRTHypo.MuonDecisions = sequenceOut
    trigMuonEFCBLRTHypo.MapToPreviousDecisions=True

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFCBLRTSequence,
                         Maker       = efcbViewsMaker,
                         Hypo        = trigMuonEFCBLRTHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict,
                         IsProbe     = is_probe_leg )


######################
### EF SA full scan ###
######################
def muEFSAFSAlgSequence(ConfigFlags):

    efsafsInputMaker = EventViewCreatorAlgorithm("IMMuonFS")
    fsRoiTool = ViewCreatorFSROITool()
    fsRoiTool.RoisWriteHandleKey = "MuonFS_RoIs"
    #
    efsafsInputMaker.RoIsLink = "initialRoI" # Only expect to get in one FS RI
    efsafsInputMaker.RoITool = fsRoiTool # Use new FS roi (note: the ViewCreatorInitialROITool should work excactly the same here)
    #
    efsafsInputMaker.Views = "MUFSViewRoI"
    efsafsInputMaker.InViewRoIs = "MUFSRoIs"
    #
    efsafsInputMaker.ViewFallThrough=True

    ### get EF reco sequence ###    
    from .MuonRecoSequences import muEFSARecoSequence, muonDecodeCfg
    viewAlgs_MuonPRD = algorithmCAToGlobalWrapper(muonDecodeCfg,ConfigFlags,RoIs=efsafsInputMaker.InViewRoIs.path())
    muEFSAFSRecoSequence, sequenceOut = muEFSARecoSequence( efsafsInputMaker.InViewRoIs,'FS' )

    muEFFSRecoSequence = parOR("muEFSAFSRecoSequence",[viewAlgs_MuonPRD, muEFSAFSRecoSequence])
    efsafsInputMaker.ViewNodeName = muEFFSRecoSequence.name()

    muonEFSAFSSequence = seqAND( "muonEFSAFSSequence", [efsafsInputMaker, muEFFSRecoSequence ] )

    return (muonEFSAFSSequence, efsafsInputMaker, sequenceOut)

def muEFSAFSSequence(is_probe_leg=False):

    (muonEFSAFSSequence, efsafsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAFSAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFSAFSHypo = TrigMuonEFHypoAlg( "TrigMuonEFSAFSHypoAlg" )
    trigMuonEFSAFSHypo.MuonDecisions = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromName

    return MenuSequence( Sequence    = muonEFSAFSSequence,
                         Maker       = efsafsInputMaker,
                         Hypo        = trigMuonEFSAFSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName,
                         IsProbe     = is_probe_leg )

######################
### EF CB full scan ###
######################
def muEFCBFSAlgSequence(ConfigFlags):
    efcbfsInputMaker = EventViewCreatorAlgorithm("IMEFCBFS")
    newRoITool = ViewCreatorCentredOnIParticleROITool()
    newRoITool.RoisWriteHandleKey = "MuonCandidates_FS_ROIs"
    #
    efcbfsInputMaker.mergeUsingFeature = True
    efcbfsInputMaker.RoITool = newRoITool
    #
    efcbfsInputMaker.Views = "MUCBFSViews"
    efcbfsInputMaker.InViewRoIs = "MUCBFSRoIs"
    #
    efcbfsInputMaker.RequireParentView = True
    efcbfsInputMaker.ViewFallThrough = True
    # Muon specific
    efcbfsInputMaker.PlaceMuonInView = True
    efcbfsInputMaker.InViewMuons = "InViewMuons"
    efcbfsInputMaker.InViewMuonCandidates = "MuonCandidates_FS"

    from .MuonRecoSequences import muEFCBRecoSequence
    muEFCBFSRecoSequence, sequenceOut = muEFCBRecoSequence( efcbfsInputMaker.InViewRoIs, "FS" )

    efcbfsInputMaker.ViewNodeName = muEFCBFSRecoSequence.name()

    muonEFCBFSSequence = seqAND( "muonEFFSCBSequence", [efcbfsInputMaker, muEFCBFSRecoSequence] )

    return (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut)

def muEFCBFSSequence(is_probe_leg=False):

    (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBFSAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFCBFSHypo = TrigMuonEFHypoAlg( "TrigMuonEFFSCombinerHypoAlg" )
    trigMuonEFCBFSHypo.MuonDecisions = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoToolFromName

    return MenuSequence( Sequence    = muonEFCBFSSequence,
                         Maker       = efcbfsInputMaker,
                         Hypo        = trigMuonEFCBFSHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromName,
                         IsProbe     = is_probe_leg )

def efLateMuRoIAlgSequence(ConfigFlags):

    from .MuonRecoSequences import efLateMuRoISequence

    eflateViewsMaker = EventViewCreatorAlgorithm("IMeflatemuroi")
    #
    eflateViewsMaker.RoIsLink = "initialRoI"
    eflateViewsMaker.RoITool = ViewCreatorInitialROITool()
    #
    eflateViewsMaker.Views = "MULATEViewRoIs"
    eflateViewsMaker.InViewRoIs = "MULATERoIs"
    #
    eflateViewsMaker.ViewFallThrough = True

    #Get Late Muon RoIs
    efLateMuRoISequence, sequenceOut = efLateMuRoISequence()

    #Final sequence running in view
    eflateViewsMaker.ViewNodeName = efLateMuRoISequence.name()
    muonSequence = seqAND("lateMuonRoISequence", [eflateViewsMaker, efLateMuRoISequence])

    return (muonSequence, eflateViewsMaker, sequenceOut)

def efLateMuRoISequence():

    (muonEFLateSequence, eflateViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(efLateMuRoIAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonLateMuRoIHypoAlg
    trigMuonEFLateHypo = TrigMuonLateMuRoIHypoAlg( "TrigMuonLateMuRoIHypoAlg" )
    trigMuonEFLateHypo.LateRoIs = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonLateMuRoIHypoToolFromDict

    return MenuSequence( Sequence    = muonEFLateSequence,
                         Maker       = eflateViewsMaker,
                         Hypo        = trigMuonEFLateHypo,
                         HypoToolGen = TrigMuonLateMuRoIHypoToolFromDict )


def efLateMuAlgSequence(ConfigFlags):

    from .MuonRecoSequences import muEFInsideOutRecoSequence, muonDecodeCfg, muonIDFastTrackingSequence
    eflateViewsMaker = EventViewCreatorAlgorithm("IMeflatemu")
    roiTool = ViewCreatorNamedROITool() # Use an existing ROI which is linked to the navigation with a custom name.
    roiTool.ROILinkName = "feature" # The ROI is actually linked as Step 1's feature. So the custom name is "feature".
    #
    eflateViewsMaker.mergeUsingFeature = True # Expect to have efLateMuRoIAlgSequence produce one Decision Object per lateROI, keep these distinct in the merging
    eflateViewsMaker.RoITool = roiTool
    #
    eflateViewsMaker.Views = "MUEFLATEViewRoIs"
    eflateViewsMaker.InViewRoIs = "MUEFLATERoIs"
    #
    eflateViewsMaker.ViewFallThrough = True

    #decode data in these RoIs
    viewAlgs_MuonPRD = algorithmCAToGlobalWrapper(muonDecodeCfg,ConfigFlags,RoIs=eflateViewsMaker.InViewRoIs.path())
    #ID fast tracking
    muFastIDRecoSequence = muonIDFastTrackingSequence( eflateViewsMaker.InViewRoIs,"Late" )
    #inside-out reco sequence
    muonEFInsideOutRecoSequence, sequenceOut = muEFInsideOutRecoSequence( eflateViewsMaker.InViewRoIs, "LateMu")

    lateMuRecoSequence = parOR("lateMuonRecoSequence", [viewAlgs_MuonPRD, muFastIDRecoSequence, muonEFInsideOutRecoSequence])

    #Final sequence running in view
    eflateViewsMaker.ViewNodeName = lateMuRecoSequence.name()
    muonSequence = seqAND("lateMuonOutSequence", [eflateViewsMaker, lateMuRecoSequence])

    return (muonSequence, eflateViewsMaker, sequenceOut)

def efLateMuSequence():

    (muonEFLateSequence, eflateViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(efLateMuAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFHypoAlg
    trigMuonEFLateHypo = TrigMuonEFHypoAlg( "TrigMuonEFCombinerLateMuHypoAlg" )
    trigMuonEFLateHypo.MuonDecisions = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFLateSequence,
                         Maker       = eflateViewsMaker,
                         Hypo        = trigMuonEFLateHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict )



######################
### efMuiso step ###
######################
def muEFIsoAlgSequence(ConfigFlags, doMSiso=False):
    name = ""
    if doMSiso:
        name = "MS"
    efmuIsoViewsMaker = EventViewCreatorAlgorithm("IMefmuiso"+name)
    newRoITool = ViewCreatorCentredOnIParticleROITool()
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDConfig = getInDetTrigConfig("muonIso")
    newRoITool.RoIEtaWidth=IDConfig.etaHalfWidth
    newRoITool.RoIPhiWidth=IDConfig.phiHalfWidth
    newRoITool.RoIZedWidth=IDConfig.zedHalfWidth
    if doMSiso:
        newRoITool.RoisWriteHandleKey = "Roi_MuonIsoMS"
    else:
        newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_MuonIso")
    #
    efmuIsoViewsMaker.mergeUsingFeature = True
    efmuIsoViewsMaker.RoITool = newRoITool
    #
    efmuIsoViewsMaker.Views = "MUEFIsoViewRoIs"+name
    efmuIsoViewsMaker.InViewRoIs = "MUEFIsoRoIs"+name
    #
    efmuIsoViewsMaker.ViewFallThrough = True
    # Muon specific
    # TODO - this should be deprecated here and removed in the future, now that we mergeUsingFeature, each parent View should only have one muon.
    # therefore the xAOD::Muon should be got via ViewFallThrough, rather than being copied in here as "IsoViewMuons"
    efmuIsoViewsMaker.PlaceMuonInView = True
    efmuIsoViewsMaker.InViewMuonCandidates = "IsoMuonCandidates"+name
    efmuIsoViewsMaker.InViewMuons = "IsoViewMuons"+name

    ### get EF reco sequence ###
    from .MuonRecoSequences  import efmuisoRecoSequence
    efmuisoRecoSequence, sequenceOut = efmuisoRecoSequence( efmuIsoViewsMaker.InViewRoIs, efmuIsoViewsMaker.InViewMuons, doMSiso )

    efmuIsoViewsMaker.ViewNodeName = efmuisoRecoSequence.name()

    ### Define a Sequence to run for muIso ###
    efmuIsoSequence = seqAND("efmuIsoSequence"+name, [ efmuIsoViewsMaker, efmuisoRecoSequence ] )

    return (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut)

def muEFIsoSequence(is_probe_leg=False):

    (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFIsoAlgSequence, ConfigFlags)

    # set up hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFTrackIsolationHypoAlg
    trigmuefIsoHypo = TrigMuonEFTrackIsolationHypoAlg("EFMuisoHypoAlg")
    trigmuefIsoHypo.EFMuonsName = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFTrackIsolationHypoToolFromDict

    return MenuSequence( Sequence    = efmuIsoSequence,
                         Maker       = efmuIsoViewsMaker,
                         Hypo        = trigmuefIsoHypo,
                         HypoToolGen = TrigMuonEFTrackIsolationHypoToolFromDict,
                         IsProbe     = is_probe_leg )

def muEFMSIsoSequence(is_probe_leg=False):

    (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFIsoAlgSequence, ConfigFlags, doMSiso=True)

    # set up hypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFTrackIsolationHypoAlg
    trigmuefIsoHypo = TrigMuonEFTrackIsolationHypoAlg("EFMuMSisoHypoAlg")
    trigmuefIsoHypo.EFMuonsName = sequenceOut

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFTrackIsolationHypoToolFromDict

    return MenuSequence( Sequence    = efmuIsoSequence,
                         Maker       = efmuIsoViewsMaker,
                         Hypo        = trigmuefIsoHypo,
                         HypoToolGen = TrigMuonEFTrackIsolationHypoToolFromDict,
                         IsProbe     = is_probe_leg )


####################################################
##  Muon RoI Cluster Trigger for MS LLP Searches  ##
####################################################

def muRoiClusterSequence(ChainFlags):

    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
    from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
    from TrigLongLivedParticlesHypo.TrigL2LongLivedParticlesHypoConfig import MuonClusterHypoAlgConfig, TrigL2LongLivedParticlesHypoToolFromDict

    muvtxMuonClusterSequence = parOR('muvtxMuonClusterSequence', [MuonClusterConfig('muvtxMuonCluster')])




    inputMaker = InputMakerForRoI(
                name = 'IM_muvtxMuonCluster',
                mergeUsingFeature = False,
                RoITool = ViewCreatorInitialROITool(),
                RoIs = 'HLT_muVtxCluster_RoIs'
                )

    muvtx_sequence = seqAND('muvtxSequence', [inputMaker, muvtxMuonClusterSequence])

    hypo        = MuonClusterHypoAlgConfig('MuRoiClusterHypoAlg')

    return MenuSequence( Sequence    = muvtx_sequence,
                         Maker       = inputMaker,
                         Hypo        = hypo,
                         HypoToolGen = TrigL2LongLivedParticlesHypoToolFromDict )


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
    from ..Config.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()  # get menu singleton
    chains = [chain.name for chain in menu.chainsInMenu['Bphysics']]
    return chains

############################################################
### Get muon triggers except L2 inside-out trigger
### to filter chains where we don't want to run L2SA IO mode
############################################################

def getInsideOutMuonChainNames():
    from ..Config.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()  # get menu singleton
    chains = [chain.name for chain in menu.chainsInMenu['Muon'] if "l2io" in chain.name]
    chains += [chain.name for chain in menu.chainsInMenu['Bphysics'] if not any(key in chain.name for key in ['noL2Comb','l2mt'])]
    return chains

############################################################
### Get muon triggers except L2 multi-track trigger
### to filter chains where we don't want to run L2SA multi-track mode
############################################################

def getMultiTrackChainNames():
    from ..Config.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()  # get menu singleton
    chains = [chain.name for chain in menu.chainsInMenu['Muon'] if "l2mt" in chain.name]
    chains += [chain.name for chain in menu.chainsInMenu['Bphysics'] if "l2mt" in chain.name]
    return chains
