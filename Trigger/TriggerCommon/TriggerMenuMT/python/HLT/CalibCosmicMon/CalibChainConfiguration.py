# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from ..CommonSequences.FullScanDefs import caloFSRoI
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Config.ChainConfigurationBase import ChainConfigurationBase
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool # ChainStep,
from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
from TrigGenericAlgs.TrigGenericAlgsConfig import TimeBurnerCfg, TimeBurnerHypoToolGen
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.CFElements import seqAND


# --------------------
# LArNoiseBurst end-of-event configuration
# --------------------
def getLArNoiseBurstEndOfEvent():
    from TriggerMenuMT.HLT.CommonSequences.FullScanDefs import caloFSRoI
    from TriggerMenuMT.HLT.CommonSequences.CaloSequences import cellRecoSequence

    cells_sequence, _ = RecoFragmentsPool.retrieve(cellRecoSequence, flags=ConfigFlags, RoIs=caloFSRoI)
    return cells_sequence, caloFSRoI

# --------------------
# LArNoiseBurst configuration
# --------------------

def getLArNoiseBurst(self):

    hypoAlg = CompFactory.TrigLArNoiseBurstAlg("NoiseBurstAlg")
    from TrigCaloHypo.TrigCaloHypoConfig import TrigLArNoiseBurstHypoToolGen
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    from TriggerMenuMT.HLT.CommonSequences.CaloSequences import cellRecoSequence
    noiseBurstInputMakerAlg= conf2toConfigurable(clusterFSInputMaker())

    from AthenaCommon.CFElements import parOR, seqAND
    noiseBurstRecoSeq = parOR( "LArNoiseRecoSeq")
    cells_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=ConfigFlags, RoIs=noiseBurstInputMakerAlg.RoIs)
    noiseBurstRecoSeq += cells_sequence
    hypoAlg.CellContainerKey = cells_name

    noiseBurstMenuSeq =  seqAND("LArNoiseMenuSeq", [noiseBurstInputMakerAlg, noiseBurstRecoSeq])

    return MenuSequence(
            Sequence    = noiseBurstMenuSeq,
            Maker       = noiseBurstInputMakerAlg,
            Hypo        = hypoAlg,
            HypoToolGen = TrigLArNoiseBurstHypoToolGen)

#----------------------------------------------------------------

# --------------------
# LArPS Noise Detection EM configuration
# --------------------
def LArPSAllEMSequence(flags, name="LArPSSequence_AllEM"):

    from TrigT2CaloCommon.CaloDef import fastCalo_AllEM_EVCreator
 
    (LArPSViewsMaker_AllEM, InViewRoIs) = fastCalo_AllEM_EVCreator()
    (LArPSAllEMInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs,doRinger=False, ClustersName="HLT_LArPS_AllCaloEMClusters",doAllEm=True,doAll=False)
    LArPSViewsMaker_AllEM.ViewNodeName = LArPSAllEMInViewSequence.name()
    LArPSAllEMSequence = seqAND(name,[LArPSViewsMaker_AllEM, LArPSAllEMInViewSequence])
    return (LArPSAllEMSequence, LArPSViewsMaker_AllEM, sequenceOut)

def getCaloAllEMLayersPS(self):

    sequence, viewsmaker, sequenceOut =  RecoFragmentsPool.retrieve(LArPSAllEMSequence,flags=ConfigFlags)
    
    from TrigCaloHypo.TrigCaloHypoConfig import TrigL2CaloLayersHypoToolGen
    TrigL2CaloLayersAlg = CompFactory.TrigL2CaloLayersAlg("TrigL2CaloLayersAlg_AllEM")
    TrigL2CaloLayersAlg.TrigClusterContainerKey = sequenceOut

    return MenuSequence(
        Sequence    = sequence,
        Maker       = viewsmaker,
        Hypo        = TrigL2CaloLayersAlg,
        HypoToolGen = TrigL2CaloLayersHypoToolGen)

#----------------------------------------------------------------

# --------------------
# LArPS Noise Detection all configuration
# --------------------

def LArPSAllSequence( flags,  name="LArPSSequence_All"):

    from TrigT2CaloCommon.CaloDef import fastCalo_All_EVCreator
    
    (LArPSViewsMaker_All, InViewRoIs) = fastCalo_All_EVCreator()
    (LArPSAllInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs,doRinger=False, ClustersName="HLT_LArPS_AllCaloClusters",doAllEm=False,doAll=True)
    LArPSViewsMaker_All.ViewNodeName = LArPSAllInViewSequence.name()
    LArPSAllSequence = seqAND(name,[LArPSViewsMaker_All, LArPSAllInViewSequence])
    return (LArPSAllSequence, LArPSViewsMaker_All, sequenceOut)


def getCaloAllLayersPS(self):

    sequence, viewsmaker, sequenceOut =  RecoFragmentsPool.retrieve(LArPSAllSequence,flags=ConfigFlags)

    from TrigCaloHypo.TrigCaloHypoConfig import TrigL2CaloLayersHypoToolGen
    TrigL2CaloLayersAlg = CompFactory.TrigL2CaloLayersAlg("TrigL2CaloLayersAlg_All")
    TrigL2CaloLayersAlg.TrigClusterContainerKey = sequenceOut

    return MenuSequence(
        Sequence    = sequence,
        Maker       = viewsmaker,
        Hypo        = TrigL2CaloLayersAlg,
        HypoToolGen = TrigL2CaloLayersHypoToolGen)

#----------------------------------------------------------------
class CalibChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):       
                         
        chainSteps = []
        log.debug("Assembling chain for %s", self.chainName)

        stepDictionary = self.getStepDictionary()
                
        if 'acceptedevts' in self.chainPart['purpose']:
            steps=stepDictionary['AcceptedEvents']
        elif self.chainPart['purpose'][0] == 'larnoiseburst':
            steps=stepDictionary['LArNoiseBurst']
        elif self.chainPart['purpose'][0] == 'larpsallem':
            steps=stepDictionary['LArPSAllEM']
        elif self.chainPart['purpose'][0] == 'larpsall':
            steps=stepDictionary['LArPSAll']
        elif self.chainPart['purpose'][0] == 'idcalib':
            steps=stepDictionary['IDCalib']
        for i, step in enumerate(steps): 
            chainstep = getattr(self, step)(i)
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps)
        return myChain


    def getStepDictionary(self):
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "AcceptedEvents": ['getAcceptedEventsStep'],
            "LArNoiseBurst": ['getAllTEStep'],
            "LArPSAllEM" : ['getCaloAllEMStep'],
            "LArPSAll" : ['getCaloAllStep'],
            "IDCalib"  : ['getIDCalibEmpty','getIDCalibFTFReco','getIDCalibTrigger']
        }
        return stepDictionary


    def getAcceptedEventsStep(self, i):
        return self.getStep(1, 'AcceptedEvents', [acceptedEventsSequence])

    def getAllTEStep(self, i):
        return self.getStep(1, 'LArNoiseBurst', [getLArNoiseBurst])

    def getCaloAllEMStep(self, i):
        return self.getStep(1, 'LArPSALLEM', [getCaloAllEMLayersPS])

    def getCaloAllStep(self, i):
        return self.getStep(1, 'LArPSALL', [getCaloAllLayersPS])

    def getIDCalibEmpty(self,i):
        return self.getEmptyStep(1, 'IDCalibEmptyStep')

    def getIDCalibFTFReco(self,i):
        return self.getStep(2,'IDCalibFTFCfg',[IDCalibFTFCfg])

    def getIDCalibTrigger(self,i):
        return self.getStep(3,'IDCalibTriggerCfg',[IDCalibTriggerCfg])

#----------------------------------------------------------------

# --------------------
# IDCalib trigger configurations
# --------------------

def IDCalibTriggerCfg(flags):

    from TrigTrackingHypo.IDCalibHypoConfig import IDCalibHypoToolFromDict
    from TrigTrackingHypo.IDCalibHypoConfig import createIDCalibHypoAlg
    theHypoAlg = createIDCalibHypoAlg("IDCalibHypo")

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, InputMakerAlg, sequenceOut ) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    theHypoAlg.tracksKey = recordable(sequenceOut)

    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_IDCalib_HypoOnlyStep" ))
    DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    return MenuSequence( Sequence    = seqAND("IDCalibEmptySeq",[DummyInputMakerAlg]),
                         Maker       = DummyInputMakerAlg,
                         Hypo        = theHypoAlg,
                         HypoToolGen = IDCalibHypoToolFromDict,
    )

# --------------------

def FTFTrackSequence(ConfigFlags):

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker("ftf")

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    TrkInputNoViewAlg = makeInDetTrigFastTrackingNoView( config=IDTrigConfig, rois=caloFSRoI )

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    
    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet)
    prmVtx = vtxAlgs[-1]

    TrkSeq =  [InputMakerAlg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.tracks_FTF()

    return (TrkSeq, InputMakerAlg, sequenceOut)

# --------------------

def IDCalibFTFCfg(flags):

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, InputMakerAlg, sequenceOut ) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)

    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
    from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator
    HypoAlg = TrigStreamerHypoAlg("IDCalibTrkDummyStream")

    return MenuSequence( Sequence    = seqAND("IDCalibTrkrecoSeq", TrkSeq),
                         Maker       = InputMakerAlg,
                         Hypo        = HypoAlg,
                         HypoToolGen = StreamerHypoToolGenerator )

#----------------------------------------------------------------

# --------------------
# HLT step for the AcceptedEvents chains
# --------------------
def acceptedEventsSequence(flags):
    '''
    Return MenuSequence for an HLT step used by the AcceptedEvents chains. This step is a trivial
    always-reject hypo with no reco. The step itself should be noop as only the HLTSeeding and the
    end-of-event sequence parts of AcceptedEvents chains are actually used.
    '''
    # Implementation identical to the timeburner chain but with zero sleep time
    inputMaker = InputMakerForRoI("IM_AcceptedEvents")
    inputMaker.RoITool = ViewCreatorInitialROITool()
    inputMaker.RoIs="AcceptedEventsRoIs"
    inputMakerSeq = seqAND("AcceptedEventsSequence", [inputMaker])

    # TimeBurner alg works as a reject-all hypo
    hypoAlg = conf2toConfigurable(TimeBurnerCfg(name="AcceptedEventsHypo"))
    hypoAlg.SleepTimeMillisec = 0

    return MenuSequence(
        Sequence    = inputMakerSeq,
        Maker       = inputMaker,
        Hypo        = hypoAlg,
        HypoToolGen = TimeBurnerHypoToolGen)
