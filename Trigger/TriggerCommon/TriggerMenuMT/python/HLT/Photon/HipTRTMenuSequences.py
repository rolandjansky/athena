#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.GlobalFlags import globalflags
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
#from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnClusterROITool
from TrigTRTHighTHitCounter.TrigTRTHTHCounterConfig import TrigTRTHTHCounterFex

def TRTHitGeneratorSequence(ConfigFlags):

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys()

    """ hipTRT step ....."""
    InViewRoIs = "TRTHitGenerator"
    # EVCreator:
    roiTool = ViewCreatorCentredOnClusterROITool()
    roiTool.AllowMultipleClusters = False 
    roiTool.RoisWriteHandleKey = "HLT_Roi_TRTHit"
    roiTool.RoIEtaWidth = 0.10
    roiTool.RoIPhiWidth = 0.10
    trtViewsMaker = EventViewCreatorAlgorithm("IMTRTHitGenerator")
    trtViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    trtViewsMaker.RoITool = roiTool
    trtViewsMaker.InViewRoIs = InViewRoIs
    trtViewsMaker.Views = "TRTHitGeneratorViews"
    trtViewsMaker.ViewFallThrough = True
    trtViewsMaker.RequireParentView = True
   
    # view data verifier
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("TRTHitGeneratorViewDataVerifier")
    ViewVerify.DataObjects = [('TrigRoiDescriptorCollection' , 'StoreGateSvc+TRTHitGenerator'),
                              #('TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs'),        
     ]
    if not globalflags.InputFormat.is_bytestream():
        ViewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    # calling trtRIOMaker
    from TrigInDetConfig.InDetTrigPrecisionTracking import trtRIOMaker_builder
    trtInviewAlgs = trtRIOMaker_builder(signature = "electrontrt", config = None, rois=InViewRoIs)
    
    trtHTHFex = TrigTRTHTHCounterFex("TrigTRTH_fex")
    trtHTHFex.RoIs = trtViewsMaker.InViewRoIs
    trtHTHFex.TRT_DC_ContainerName = "TRT_TrigDriftCircles" 
    trtHTHFex.RNNOutputName = TrigEgammaKeys.TrigTRTHTCountsContainer
    sequenceOut = trtHTHFex.RNNOutputName    
    
    trtInviewAlgs = parOR("trtInviewAlgs", trtInviewAlgs + [ViewVerify,trtHTHFex])
    trtViewsMaker.ViewNodeName = "trtInviewAlgs"


    trtDataSequence = seqAND("trtDataSequence", [trtViewsMaker, trtInviewAlgs ] )
    return (sequenceOut, trtDataSequence, trtViewsMaker)



def hipTRTMenuSequence():
    """ Creates TRTDataGenerator MENU sequence """
    (sequenceOut, trtDataSequence, trtViewsMaker) = RecoFragmentsPool.retrieve(TRTHitGeneratorSequence, ConfigFlags)

    #Hypo
    from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHhypoAlg
    from TrigTRTHighTHitCounter.TrigTRTHTHhypoTool import TrigTRTHTHhypoToolFromDict

    trtHTHhypo = TrigTRTHTHhypoAlg("TrigTRTHTHhypo")
    trtHTHhypo.RNNOutputName = sequenceOut 

    return MenuSequence( Sequence    = trtDataSequence,
                         Maker       = trtViewsMaker, 
                         Hypo        = trtHTHhypo,
                         HypoToolGen = TrigTRTHTHhypoToolFromDict)


