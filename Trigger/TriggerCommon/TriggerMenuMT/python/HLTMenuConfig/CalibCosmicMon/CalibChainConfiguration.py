# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.CalibChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

#----------------------------------------------------------------
class CalibChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):       
                         
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        if self.chainPartName == 'larnoiseburst':
           chainSteps.append(self.getLArNoiseBurst())
        myChain = self.buildChain(chainSteps)
        return myChain


    # --------------------
    # LArNoiseBurst configuration
    # --------------------
    def getLArNoiseBurst(self):

        from AthenaConfiguration.ComponentFactory import CompFactory
        from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, RecoFragmentsPool
        hypoAlg = CompFactory.TrigLArNoiseBurstAlg("NoiseBurstAlg")
        from TrigCaloHypo.TrigCaloHypoConfig import TrigLArNoiseBurstHypoToolGen
        from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import cellRecoSequence
        noiseBurstInputMakerAlg= conf2toConfigurable(clusterFSInputMaker())

        from AthenaCommon.CFElements import parOR, seqAND
        noiseBurstRecoSeq = parOR( "LArNoiseRecoSeq")
        cells_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=None, RoIs=noiseBurstInputMakerAlg.RoIs)
        noiseBurstRecoSeq += cells_sequence
        hypoAlg.CellContainerKey = cells_name

        noiseBurstMenuSeq =  seqAND("LArNoiseMenuSeq", [noiseBurstInputMakerAlg, noiseBurstRecoSeq])

        seq = MenuSequence(
            Sequence    = noiseBurstMenuSeq,
            Maker       = noiseBurstInputMakerAlg,
            Hypo        = hypoAlg,
            HypoToolGen = TrigLArNoiseBurstHypoToolGen)

        return ChainStep(name='LArNoiseBurst', Sequences=[seq])
