#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
import AthenaCommon.SystemOfUnits as Units


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AccumulatorCache import AccumulatorCache
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


########
# to move into TrigMinBiasHypoConfigMT?

def SPCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import SPCountHypoTool
    hypo = SPCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.sctSP = int(chainDict["chainParts"][0]["hypoSPInfo"].strip("sp"))
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.pixCL = 2
        hypo.sctSP = 3
    if "mb_excl" in chainDict["chainName"]:
        hypo.pixCLMax = 150 # TODO revisit tightening those
        hypo.sctSPMax = 150 # as above
    return hypo



def TrackCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import TrackCountHypoTool
    hypo = TrackCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.minNtrks = int(chainDict["chainParts"][0]["hypoTrkInfo"].strip("trk"))
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.minPt = 200
        hypo.maxZ0 = 401
    if "mb_sptrk_pt" in chainDict["chainName"]:
        hypo.minPt = int(chainDict["chainParts"][0]["hypoPtInfo"].strip("pt"))*Units.GeV
        hypo.maxZ0 = 401
    if "mb_excl" in chainDict["chainName"]:
        hypo.exclusive = True
        hypo.minPt = int(chainDict["chainParts"][0]["hypoPtInfo"].strip("pt"))*Units.GeV
        trk = chainDict["chainParts"][0]["hypoTrkInfo"]
        # this is string of the form 4trk6 - 'number'trk'number'
        limits =  trk[0:trk.index("trk")], trk[trk.index("trk")+3:]
        hypo.minNtrks = int(limits[0])
        hypo.maxNtrks = int(limits[1])

        # will set here cuts
    return hypo

def MbtsHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import MbtsHypoTool
    hypo = MbtsHypoTool(chainDict["chainName"]) # to now no additional settings
    if chainDict["chainParts"][0]["extra"] in ["vetombts2in", "vetospmbts2in"]:
        hypo.MbtsCounters=2
        hypo.MBTSMode=1
        hypo.Veto=True
    else:  #default, one counter on each side
        hypo.MbtsCounters=1
    return hypo

    

def TrigZVertexHypoToolGen(chainDict):
    hypo = CompFactory.TrigZVertexHypoTool(chainDict["chainName"])
    if "pusup" in chainDict["chainName"]:
        hypo.minWeight = int(chainDict["chainParts"][0]["pileupInfo"].strip("pusup"))
    else:
        raise RuntimeError("Chain {} w/o pileup suppression required to configure z vertex hypo".format(chainDict["chainName"]))
    return hypo

@AccumulatorCache
def SPCounterRecoAlgCfg(flags):
    acc = ComponentAccumulator()
    from TrigMinBias.TrigMinBiasMonitoring import SpCountMonitoring
    alg = CompFactory.TrigCountSpacePoints( SpacePointsKey = recordable("HLT_SpacePointCounts"), 
                                            MonTool = SpCountMonitoring() ) 
    acc.addEventAlgo(alg)
    return acc
    


### Now the sequences

def MinBiasSPSequence():
    spAlgsList = []
    from TrigMinBias.TrigMinBiasConf import SPCountHypoAlg

    spInputMakerAlg = EventViewCreatorAlgorithm("IM_SPEventViewCreator")
    spInputMakerAlg.ViewFallThrough = True
    spInputMakerAlg.RoITool = ViewCreatorInitialROITool()
    spInputMakerAlg.InViewRoIs = "InputRoI"
    spInputMakerAlg.Views = "SPView"

    idTrigConfig = getInDetTrigConfig('minBias')

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    idAlgs, verifier = makeInDetTrigFastTracking(config=idTrigConfig, 
                                     rois=spInputMakerAlg.InViewRoIs, 
                                     viewVerifier='SPViewDataVerifier', 
                                     doFTF=False)
    verifier.DataObjects += [('TrigRoiDescriptorCollection', 'StoreGateSvc+InputRoI'),
                             ('SCT_ID', 'DetectorStore+SCT_ID'),
                             ('PixelID', 'DetectorStore+PixelID'),
                             ('TagInfo', 'DetectorStore+ProcessingTags')]

    # Make sure required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [('SCT_ID', 'DetectorStore+SCT_ID'),
                                       ('PixelID', 'DetectorStore+PixelID'),
                                       ('TagInfo', 'DetectorStore+ProcessingTags')]

#    spAlgsList = idAlgs[:-2]
    spAlgsList = idAlgs

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags# this will disappear once the flags are transported down here
    from ..Menu.MenuComponents import algorithmCAToGlobalWrapper # this will disappear once whole sequence would be configured at once
    spCount = algorithmCAToGlobalWrapper(SPCounterRecoAlgCfg, flags)[0]

    spRecoSeq = parOR("spRecoSeq", spAlgsList + [spCount])
    spSequence = seqAND("spSequence", [spInputMakerAlg, spRecoSeq])
    spInputMakerAlg.ViewNodeName = spRecoSeq.name()


    spCountHypo =SPCountHypoAlg()
    spCountHypo.SpacePointsKey=recordable("HLT_SpacePointCounts")

    return MenuSequence(Sequence    = spSequence,
                        Maker       = spInputMakerAlg,
                        Hypo        = spCountHypo,
                        HypoToolGen = SPCountHypoToolGen )

@AccumulatorCache
def MinBiasZVertexFinderSequenceCfg(flags):
    from ..Menu.MenuComponents import InViewRecoCA, SelectionCA, MenuSequenceCA
    recoAcc = InViewRecoCA(name="ZVertFinderReco", InViewRoIs="InputRoI", RequireParentView=True)
    vdv = CompFactory.AthViews.ViewDataVerifier( "VDVZFinderInputs",
                                                  DataObjects = [( 'SpacePointContainer' , 'StoreGateSvc+PixelTrigSpacePoints'), 
                                                                 ( 'PixelID' , 'DetectorStore+PixelID' ) ])

    recoAcc.addRecoAlgo(vdv)
    from IDScanZFinder.ZFinderAlgConfig import  MinBiasZFinderCfg
    recoAcc.mergeReco( MinBiasZFinderCfg(flags) )
    selAcc = SelectionCA("ZVertexFinderSel")    
    selAcc.mergeReco(recoAcc)
    selAcc.addHypoAlgo( CompFactory.TrigZVertexHypoAlg("TrigZVertexHypoAlg", ZVertexKey=recordable("HLT_vtx_z")))
    return MenuSequenceCA(selAcc, HypoToolGen = TrigZVertexHypoToolGen)


def MinBiasTrkSequence():
        from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlg

        trkInputMakerAlg = EventViewCreatorAlgorithm("IM_TrkEventViewCreator")
        trkInputMakerAlg.ViewFallThrough = True
        trkInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        trkInputMakerAlg.InViewRoIs = "InputRoI" # contract with the consumer
        trkInputMakerAlg.Views = "TrkView"
        trkInputMakerAlg.RequireParentView = True

        # prepare algorithms to run in views, first,
        # inform scheduler that input data is available in parent view (has to be done by hand)
        idTrigConfig = getInDetTrigConfig('minBias')

        from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
        algs,_ = makeInDetPatternRecognition(idTrigConfig, verifier='VDVMinBiasIDTracking')
        trackCountHypo = TrackCountHypoAlg()
        trackCountHypo.trackCountKey = recordable("HLT_TrackCount")
        trackCountHypo.tracksKey = recordable("HLT_IDTrack_MinBias_IDTrig")

        #TODO move a complete configuration of the algs to TrigMinBias package
        from TrigMinBias.TrigMinBiasMonitoring import TrackCountMonitoring
        trackCountHypo.MonTool = TrackCountMonitoring(trackCountHypo) # monitoring tool configures itself using config of the hypo alg

        trkRecoSeq = parOR("TrkRecoSeq", algs)
        trkSequence = seqAND("TrkSequence", [trkInputMakerAlg, trkRecoSeq])
        trkInputMakerAlg.ViewNodeName = trkRecoSeq.name()

        return MenuSequence(Sequence    = trkSequence,
                            Maker       = trkInputMakerAlg,
                            Hypo        = trackCountHypo,
                            HypoToolGen = TrackCountHypoToolGen)

def MinBiasMbtsSequence():
    from TrigMinBias.TrigMinBiasConf import MbtsHypoAlg
    from TrigMinBias.MbtsConfig import MbtsFexCfg
    fex = MbtsFexCfg(MbtsBitsKey=recordable("HLT_MbtsBitsContainer"))
    MbtsRecoSeq = parOR("MbtsRecoSeq", [fex])

    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
    MbtsInputMakerAlg = InputMakerForRoI("IM_Mbts", 
                                        RoIsLink="initialRoI", 
                                        RoITool = ViewCreatorInitialROITool(),
                                        RoIs='MbtsRoI', # not used in fact
                                        )

    MbtsSequence = seqAND("MbtsSequence", [MbtsInputMakerAlg, MbtsRecoSeq])

    hypo = MbtsHypoAlg("MbtsHypoAlg", MbtsBitsKey=fex.MbtsBitsKey)


    return MenuSequence(Sequence    = MbtsSequence,
                        Maker       = MbtsInputMakerAlg,
                        Hypo        = hypo,
                        HypoToolGen = MbtsHypoToolGen)


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.lock()
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    ca = MinBiasZVertexFinderSequenceCfg(flags)    
    ca.ca.printConfig(withDetails=True)

    from ..Menu.MenuComponents import menuSequenceCAToGlobalWrapper
    ms = menuSequenceCAToGlobalWrapper(MinBiasZVertexFinderSequenceCfg, flags)
    spca = SPCounterRecoAlgCfg(flags)
    spca.printConfig(withDetails=True)
    spca.wasMerged()
