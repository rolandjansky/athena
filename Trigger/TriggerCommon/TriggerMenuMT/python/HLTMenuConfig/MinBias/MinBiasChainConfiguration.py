# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep
from TrigInDetConfig.InDetSetup import makeInDetAlgs
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from AthenaCommon.GlobalFlags import globalflags
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

                # ----------------------
                # Assemble the chain depending on information from chainName
                # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain for " + self.chainName)
        SpStep = self.getMinBiasSpStep()
        TrkStep = self.getMinBiasTrkStep()

        return self.buildChain([SpStep,TrkStep])
    def getMinBiasSpStep(self):
        """ Use the reco-dict to construct a single MinBias step """
        def generateSPCountHypo(chainDict):
                hypo = SPCountHypoTool(chainDict["chainName"])
                if "hmt" in chainDict["chainName"]:
                    hypo.totNumSctSP = int( chainDict["chainParts"][0]["hypoL2Info"].strip("sp") )
                if "mb_sptrk" in chainDict["chainName"]:
                    hypo.totNumPixSP  = 2
                    hypo.totNumSctSP  = 3
                # will set here thresholds
                return hypo
        SpList = []
        from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT, SPCountHypoAlgMT, SPCountHypoTool

        SPInputMakerAlg = EventViewCreatorAlgorithm("IM_SPEventViewCreator")
        SPInputMakerAlg.ViewFallThrough = True
        SPInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        SPInputMakerAlg.InViewRoIs = "InputRoI"
        SPInputMakerAlg.Views = "SPView"
        idAlgs, verifier = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='', rois=SPInputMakerAlg.InViewRoIs, viewVerifier='SPViewDataVerifier' )
        verifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+InputRoI' ),
                                 ( 'SCT_ID' , 'DetectorStore+SCT_ID' ),
                                 ( 'PixelID' , 'DetectorStore+PixelID' ),
                                 ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

        # Make sure required objects are still available at whole-event level
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence.SGInputLoader.Load += [( 'SCT_ID' , 'DetectorStore+SCT_ID' ),
                                           ( 'PixelID' , 'DetectorStore+PixelID' ),
                                           ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested( '/PIXEL/DCS/FSMSTATE' ):
          verifier.DataObjects += [( 'CondAttrListCollection' , 'ConditionStore+/PIXEL/DCS/FSMSTATE' )]
          topSequence.SGInputLoader.Load += [( 'CondAttrListCollection' , 'ConditionStore+/PIXEL/DCS/FSMSTATE' )]
        if not conddb.folderRequested( '/PIXEL/DCS/FSMSTATUS' ):
          verifier.DataObjects += [( 'CondAttrListCollection' , 'ConditionStore+/PIXEL/DCS/FSMSTATUS' )]
          topSequence.SGInputLoader.Load += [( 'CondAttrListCollection' , 'ConditionStore+/PIXEL/DCS/FSMSTATUS' )]

        SpList = idAlgs[:-2]

        SpCount=TrigCountSpacePointsMT()
        SpCount.SpacePointsKey=recordable("HLT_SpacePointCounts")
        
        from TrigT2MinBias.TrigT2MinBiasMonitoringMT import SpCountMonitoring
        SpCount.MonTool = SpCountMonitoring()

        SPrecoSeq = parOR("SPrecoSeq", SpList + [ SpCount ])
        SPSequence = seqAND("SPSequence", [SPInputMakerAlg, SPrecoSeq])
        SPInputMakerAlg.ViewNodeName = SPrecoSeq.name()


        SpCountHypo =SPCountHypoAlgMT()
        SpCountHypo.SpacePointsKey=recordable("HLT_SpacePointCounts")

        Step1_SPCount = ChainStep( "Step1_SPCount",  [MenuSequence( Sequence    = SPSequence,
                          Maker       = SPInputMakerAlg,
                          Hypo        = SpCountHypo,
                          HypoToolGen = generateSPCountHypo )] )

        return Step1_SPCount

    def getMinBiasTrkStep(self):
        """ Use the reco-dict to construct a single MinBias step """
        def generateTrackCountHypo(chainDict):
                hypo = TrackCountHypoTool(chainDict["chainName"])
                if "hmt" in chainDict["chainName"]:
                    hypo.required_ntrks = int( chainDict["chainParts"][0]["hypoEFInfo"].strip("trk") )
                if "mb_sptrk" in chainDict["chainName"]:
                    hypo.min_pt  = 0.2
                    hypo.max_z0  = 401

                # will set here cuts
                return hypo
        from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT, TrackCountHypoTool

        TrkInputMakerAlg = EventViewCreatorAlgorithm("IM_TrkEventViewCreator")
        TrkInputMakerAlg.ViewFallThrough = True
        TrkInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        TrkInputMakerAlg.InViewRoIs = "InputRoI" # contract with the consumer
        TrkInputMakerAlg.Views = "TrkView"
        TrkInputMakerAlg.RequireParentView = True
        TrkInputMakerAlg.ViewNodeName = "TrkCountHypoAlgMTNode"

        # prepare algorithms to run in views, first, inform scheduler that input data is available in parent view (has to be done by hand)
        idAlgs, verifier = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='', rois=TrkInputMakerAlg.InViewRoIs, viewVerifier='TrkrecoSeqDataVerifier')
        verifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+InputRoI' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'InDet::SCT_ClusterContainer' , 'StoreGateSvc+SCT_TrigClusters' ),
                                 ( 'SpacePointContainer' , 'StoreGateSvc+SCT_TrigSpacePoints' ),
                                 ( 'InDet::PixelClusterContainer' , 'StoreGateSvc+PixelTrigClusters' ),
                                 ( 'SpacePointContainer' , 'StoreGateSvc+PixelTrigSpacePoints' )]

        if globalflags.InputFormat.is_bytestream():
          verifier.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                   ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]


        TrkList = idAlgs[-2:] # FTF and Track to xAOD::TrackParticle conversion alg
        TrackCountHypo=TrackCountHypoAlgMT()
        TrackCountHypo.trackCountKey=recordable("HLT_TrackCount")
        TrackCountHypo.tracksKey=recordable("HLT_IDTrack_MinBias_FTF")

        from TrigMinBias.TrackCountMonitoringMT import TrackCountMonitoring
        TrackCountHypo.MonTool = TrackCountMonitoring()

        TrkrecoSeq = parOR("TrkrecoSeq", [verifier]+TrkList)
        TrkSequence = seqAND("TrkSequence", [TrkInputMakerAlg, TrkrecoSeq])
        TrkInputMakerAlg.ViewNodeName = TrkrecoSeq.name()

        Step2_TrkCount = ChainStep( "Step2_TrkCount",  [MenuSequence( Sequence    = TrkSequence,
                            Maker       = TrkInputMakerAlg,
                            Hypo        = TrackCountHypo,
                            HypoToolGen = generateTrackCountHypo )] )
        return Step2_TrkCount
