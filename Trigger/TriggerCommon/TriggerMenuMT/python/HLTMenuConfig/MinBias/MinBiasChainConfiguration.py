# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, ViewCreatorInitialROITool
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
                # will set here thresholds
                return hypo
        SpList = []
        from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT, SPCountHypoAlgMT, SPCountHypoTool

        SPInputMakerAlg = EventViewCreatorAlgorithm("IM_SPEventViewCreator")
        SPInputMakerAlg.ViewFallThrough = True
        SPInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        SPInputMakerAlg.InViewRoIs = "InputRoI"
        SPInputMakerAlg.Views = "SPView"
        idAlgs = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='', rois=SPInputMakerAlg.InViewRoIs)
        SpList = idAlgs[:-2]

        SpCount=TrigCountSpacePointsMT()
        SpCount.SpacePointsKey=recordable("HLT_SpacePointCounts")

        SPrecoSeq = parOR("SPrecoSeq", SpList + [ SpCount ])
        SPSequence = seqAND("SPSequence", [SPInputMakerAlg, SPrecoSeq])
        SPInputMakerAlg.ViewNodeName = SPrecoSeq.name()


        SpCountHypo =SPCountHypoAlgMT()
        SpCountHypo.SpacePointsKey=recordable("HLT_SpacePointCounts")

        stepSPCount = ChainStep( "stepSPCount",  [MenuSequence( Sequence    = SPSequence,
                          Maker       = SPInputMakerAlg,
                          Hypo        = SpCountHypo,
                          HypoToolGen = generateSPCountHypo )] )

        return stepSPCount

    def getMinBiasTrkStep(self):
        """ Use the reco-dict to construct a single MinBias step """
        def generateTrackCountHypo(chainDict):
                hypo = TrackCountHypoTool(chainDict["chainName"])
                if "hmt" in chainDict["chainName"]:
                    hypo.required_ntrks = int( chainDict["chainParts"][0]["hypoEFInfo"].strip("trk") )
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
        import AthenaCommon.CfgMgr as CfgMgr
        ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("TrkrecoSeqDataVerifier")
        ViewVerifyTrk.DataObjects = [ ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                     ( 'InDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                     ( 'InDet::SCT_ClusterContainer' , 'StoreGateSvc+SCT_TrigClusters' ),
                                     ( 'SpacePointContainer' , 'StoreGateSvc+SCT_TrigSpacePoints' ),
                                     ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ),
                                     ( 'SCT_FlaggedCondData' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                     ( 'InDet::PixelClusterContainer' , 'StoreGateSvc+PixelTrigClusters' ),
                                     ( 'SpacePointContainer' , 'StoreGateSvc+PixelTrigSpacePoints' )
        ]

        idAlgs = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='', rois=TrkInputMakerAlg.InViewRoIs)
        TrkList = idAlgs[-2:] # FTF and Track to xAOD::TrackParticle conversion alg
        TrackCountHypo=TrackCountHypoAlgMT()
        TrackCountHypo.trackCountKey=recordable("HLT_TrackCount")
        TrackCountHypo.tracksKey=recordable("HLT_IDTrack_MinBias_FTF")

        TrkrecoSeq = parOR("TrkrecoSeq", [ ViewVerifyTrk ] + TrkList)
        TrkSequence = seqAND("TrkSequence", [TrkInputMakerAlg, TrkrecoSeq])
        TrkInputMakerAlg.ViewNodeName = TrkrecoSeq.name()

        stepTrkCount = ChainStep( "stepTrkCount",  [MenuSequence( Sequence    = TrkSequence,
                            Maker       = TrkInputMakerAlg,
                            Hypo        = TrackCountHypo,
                            HypoToolGen = generateTrackCountHypo )] )
        return stepTrkCount
