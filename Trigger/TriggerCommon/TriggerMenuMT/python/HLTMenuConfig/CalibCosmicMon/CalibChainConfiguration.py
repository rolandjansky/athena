# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool # ChainStep,
from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
from AthenaCommon.CFElements import seqAND


# --------------------
# LArNoiseBurst end-of-event configuration
# --------------------
def getLArNoiseBurstEndOfEvent():
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.FullScanDefs import caloFSRoI
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequences import cellRecoSequence

    cells_sequence, _ = RecoFragmentsPool.retrieve(cellRecoSequence, flags=ConfigFlags, RoIs=caloFSRoI)
    return cells_sequence, caloFSRoI

# --------------------
# LArNoiseBurst configuration
# --------------------

def getLArNoiseBurst(self):

    hypoAlg = CompFactory.TrigLArNoiseBurstAlg("NoiseBurstAlg")
    from TrigCaloHypo.TrigCaloHypoConfig import TrigLArNoiseBurstHypoToolGen
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequences import cellRecoSequence
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
                
        if self.chainPart['purpose'][0] == 'larnoiseburst':
            steps=stepDictionary['LArNoiseBurst']
        elif self.chainPart['purpose'][0] == 'larpsallem':
            steps=stepDictionary['LArPSAllEM']
        elif self.chainPart['purpose'][0] == 'larpsall':
            steps=stepDictionary['LArPSAll']
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
            "LArNoiseBurst": ['getAllTEStep'],
            "LArPSAllEM" : ['getCaloAllEMStep'],
            "LArPSAll" : ['getCaloAllStep'],
        }
        return stepDictionary


    def getAllTEStep(self, i):
        return self.getStep(1, 'LArNoiseBurst', [getLArNoiseBurst])

    def getCaloAllEMStep(self, i):
        return self.getStep(1, 'LArPSALLEM', [getCaloAllEMLayersPS])

    def getCaloAllStep(self, i):
        return self.getStep(1, 'LArPSALL', [getCaloAllLayersPS])

