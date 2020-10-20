# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo, L1InfoHypoTool

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, MenuSequence, RecoFragmentsPool
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.CFElements import seqAND

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.EnhancedBiasDef")

# Low threshold prescaled L1 items - slected at HLT based on TBP bit from L1 in random-seeded events        
# High(er) threshold prescaled L1 items - slected at HLT based on TBP bit from L1 in random-seeded events 
l1seeds = { 'low'  : \
               ['L1_2EM7',\
                'L1_EM10VH',\
                #'L1_EM12_2EM3',\
                'L1_EM12_XS20',\
                'L1_J15.31ETA49',\
                'L1_J30',\
                'L1_J30.0ETA49_2J20.0ETA49',\
                'L1_JPSI-1M5-EM7',\
                'L1_MU10',\
                #'L1_TAU20',\
                'L1_ZB'],\
             'medium' : \
               [#'L1_2EM10VH',\
                'L1_2EM15',\
                #'L1_2EM8I_J50',\
                'L1_2MU4',\
                #'L1_2TAU20IM_3J20',\
                #'L1_3J15',\
                #'L1_AFP_A_AND_C_SPECTOF_J50',\
                #'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',\
                #'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',\
                #'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',\
                #'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',\
                #'L1_BTAG-MU4J15',\
                #'L1_DR-MU10TAU12I',\
                'L1_DR-TAU20ITAU12I',\
                #'L1_DR25-TAU20ITAU12I',\
                'L1_DY-BOX-2MU6',\
                #'L1_DY-DR-2MU4',\
                #'L1_EM15I_MU4',\
                'L1_EM15VHI_2TAU12IM_J25_3J12',\
                #'L1_EM15VH_3EM7',\
                'L1_EM15_XS30',\
                'L1_EM18VHI',\
                'L1_EM20VH',\
                'L1_EM7_MU10',\
                #'L1_HT190-J15.ETA21',\
                'L1_J30.31ETA49',\
                #'L1_J4-MATCH',\
                'L1_J40.0ETA25_2J15.31ETA49',\
                #'L1_J40_DPHI-J20XE50',\
                'L1_J50',\
                'L1_J50_DETA20-J50J',\
                'L1_JPSI-1M5-EM12',\
                'L1_LFV-EM15I',\
                'L1_LFV-EM8I',\
                'L1_LFV-MU6',\
                #'L1_MJJ-400',\
                'L1_MU10_TAU12IM',\
                'L1_MU6_J20',\
                'L1_MU6_J30.0ETA49_2J20.0ETA49',\
                #'L1_MU6_MJJ-200',\
                'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',\
                'L1_TAU20IM_2TAU12IM_XE35',\
                #'L1_TAU20_2TAU12',\
                'L1_TAU40',\
                'L1_XE35',
                #'L1_BPH-0DR3-EM7J15_2MU4',
                #'L1_BPH-0DR3-EM7J15_MU6',
                #'L1_BPH-0M9-EM7-EM5_MU6'
            ] }

def enhancedBiasAthSequence(ConfigFlags):
        inputMakerAlg = InputMakerForRoI("IM_enhancedBias")
        inputMakerAlg.RoITool = ViewCreatorInitialROITool()
        inputMakerAlg.RoIs="enhancedBiasInputRoIs"
        enhancedBiasSequence = seqAND("enhancedBiasSequence", [inputMakerAlg])

        return (enhancedBiasAthSequence, inputMakerAlg, enhancedBiasSequence)

class EnhancedBiasChainConfiguration(ChainConfigurationBase):
    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self, chainDict)


    def assembleChain(self):
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)

        ebSeq = self.enhancedBiasMenuSequence()
        chainSteps.append( ChainStep(name='Step1_EnhancedBias', Sequences=[ebSeq]) )

        return self.buildChain(chainSteps)


    def enhancedBiasMenuSequence(self):
        # InputMaker and sequence
        (_, inputMakerAlg, enhancedBiasSequence) = RecoFragmentsPool.retrieve(enhancedBiasAthSequence, None)

        # Hypo
        hypoAlg = L1InfoHypo("EnhancedBiasHypo")

        # HypoToolGen
        def EnhancedBiasHypoToolGen(chainDict):
            tool = L1InfoHypoTool(chainDict['chainName'])
            tool.CTPUnpackingTool.UseTBPBits = True

            key = chainDict['chainParts'][0]['algType']
            if key not in l1seeds:
                log.error("No configuration exist for EB chain: ", key)
            else:
                tool.L1ItemNames = l1seeds[key]

            return tool

        return MenuSequence( Sequence   = enhancedBiasSequence,
                            Maker       = inputMakerAlg,
                            Hypo        = hypoAlg,
                            HypoToolGen = EnhancedBiasHypoToolGen )
