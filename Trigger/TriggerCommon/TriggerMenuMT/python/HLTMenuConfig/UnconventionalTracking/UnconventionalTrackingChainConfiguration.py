# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.UnconventionalTracking.UnconventionalTrackingDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND
from ..CommonSequences.FullScanDefs import caloFSRoI

def unconventionalTrackingChainParts(chainParts):
    unconvtrkChainParts = []
    for p in chainParts:
        if p['trigType'] == 'unconvtrk':
            unconvtrkChainParts.append(p)
    return unconvtrkChainParts


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class UnconventionalTrackingChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain %s", self.chainName)

        chainSteps = []

        stepDictionary = self.getStepDictionary()


        key = self.chainPart['extra']
        #Work around to run isohpttrack with an alternative step without skip when there is an HLT_MET trigger
        # if key=="isohpttrack" and (re.match(".*_xe\d*_*",(self.chainName))):
        #     steps=stepDictionary["isohpttrackWithHLTMET"]
        # else:
        steps=stepDictionary[key]


        for step_level in steps:
            for step in step_level:
                chainstep = getattr(self, step)()
                chainSteps+=[chainstep]


        # from ..MET.METChainConfiguration import METChainConfiguration
        # METChain =  METChainConfiguration(self.chainDict).assembleChain()

        # myChain = mergeChainDefs([METChain] + [self.buildChain(chainSteps)])
        myChain = self.buildChain(chainSteps)

        return myChain

    def getStepDictionary(self):


        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # each value is a list [ L2, EF ] where L2 = [list of L2 steps] and EF = [ EF steps]
        # this way, Bphys (or others) can insert steps at the end of L2 and end of EF after
        # the muon steps are defined
        # note that bphys chains are by default noL2Comb, even though this is not in the name
        # --------------------

        stepDictionary = {
            "isohpttrack" : [['getSkipStep'],['getFTFTrackReco'],['getIsoHPtTrackTrigger']],
            "isohpttrackWithHLTMET" : [['getFTFTrackReco'],['getIsoHPtTrackTrigger']],
            "fslrt": [['getFSLRTEmpty'], ['getFSLRTTrigger']]
        }

        return stepDictionary


        # --------------------
    def getIsoHPtTrackTrigger(self):
        return self.getStep(7,'IsoHPtTrackTriggerCfg',[IsoHPtTrackTriggerCfg])

    def getFTFTrackReco(self):
        return self.getStep(6,'FTFRecoOnlyCfg',[FTFRecoOnlyCfg])

    def getSkipStep(self):
        return  self.getEmptyStep(1,"EmptyUncTrk")

    def getFSLRTTrigger(self):
        return self.getStep(2,'FSLRTTrigger',[FSLRTTriggerCfg])

    def getFSLRTEmpty(self):
        return self.getEmptyStep(1, 'FSLRTEmptyStep')





def FTFTrackSequence(ConfigFlags):


    # from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    # InputMakerAlg= clusterFSInputMaker()

    from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker()


    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )




    from TrigEDMConfig.TriggerEDMRun3 import recordable
    from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
    TrkInputNoViewAlg = makeInDetAlgsNoView( config = IDTrigConfig, rois=caloFSRoI)



    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices

    verticesname = recordable("HLT_IDVertex_FS")
    vtxAlgs = makeVertices( "jet", IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable ) , verticesname, IDTrigConfig )
    prmVtx = vtxAlgs[-1]


    TrkSeq =  [InputMakerAlg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable )


    # from .JetTrackingConfig import JetTrackingSequence
    # (jettrkseq, trkcolls) = RecoFragmentsPool.retrieve( JetTrackingSequence, configFlags, trkopt=jetRecoDict["ftf"], RoIs=RoIs)
    # recoSeq += jettrkseq



    return (TrkSeq, InputMakerAlg, sequenceOut)





def IsoHPtTrackTriggerHypoSequence():

        from TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool import TrigIsoHPtTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigIsoHPtTrackTriggerHypoAlgMT)


        # from AthenaConfiguration.AllConfigFlags import ConfigFlags
        # ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)


        # Get sequence name
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig( 'jet' )
        sequenceOut = IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable )

        #Setup the hypothesis algorithm
        theIsoHPtTrackTriggerHypo = TrigIsoHPtTrackTriggerHypoAlgMT("L2IsoHPtTrack")
        theIsoHPtTrackTriggerHypo.trackKey =  sequenceOut

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_IsoHPtTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        log.info("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = seqAND("UncTrkEmptySeq",[DummyInputMakerAlg]),
                            Maker       = DummyInputMakerAlg,
                            Hypo        = theIsoHPtTrackTriggerHypo,
                            HypoToolGen = TrigIsoHPtTrackTriggerHypoToolFromDict,
                            )

        # log.info("Building the Step dictinary for IsoHPt!")
        # return MenuSequence( Sequence    = seqAND("UncTrkEmptySeq",TrkSeq), #,parOR("UncTrkEmptySeq",[InputMakerAlg]),
        #                     Maker       = InputMakerAlg,# DummyInputMakerAlg,
        #                     Hypo        = theIsoHPtTrackTriggerHypo,
        #                     HypoToolGen = TrigIsoHPtTrackTriggerHypoToolFromDict,
        #                     )





def FTFRecoOnlySequence():
        from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT
        from TrigStreamerHypo.TrigStreamerHypoConfigMT import StreamerHypoToolMTgenerator

        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)

        HypoAlg = TrigStreamerHypoAlgMT("UncTrkDummyStream")


        log.info("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = seqAND("UncTrkrecoSeq", TrkSeq),
                            Maker       = InputMakerAlg,
                            Hypo        = HypoAlg,
                            HypoToolGen = StreamerHypoToolMTgenerator
                            )







def IsoHPtTrackTriggerCfg(flags):
    return IsoHPtTrackTriggerHypoSequence()
def FTFRecoOnlyCfg(flags):
    return FTFRecoOnlySequence()

def FSLRTTriggerCfg(flags):
    from TriggerMenuMT.HLTMenuConfig.UnconventionalTracking.FullScanLRTTrackingConfiguration import FullScanLRTTriggerMenuSequence
    return FullScanLRTTriggerMenuSequence()
