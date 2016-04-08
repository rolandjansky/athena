# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuPython.HltConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging

#from TriggerMenuPython.TriggerPythonConfig import HLTChain
#empty = HLTChain("AllAcceptingChain", "0", "", "L2")
#empty.addStreamTag("nohlt")

###########################################################################
#
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus():
    eb_physics_empty_seed = 'L1_EM5_EMPTY,L1_MU0_EMPTY,L1_MU11_EMPTY,L1_2MU6_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_FJ10_EMPTY'
    eb_physics_firstempty_seed = 'L1_EM3_FIRSTEMPTY,L1_MU0_FIRSTEMPTY,L1_2MU0_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_2MU6_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_J10_FIRSTEMPTY,L1_J30_FIRSTEMPTY,L1_FJ10_FIRSTEMPTY'
    eb_physics_noL1PS_seed = 'L1_EM14,L1_EM16,L1_2EM10,L1_3EM5,L1_MU10,L1_MU11,L1_2MU0,L1_TAU30,L1_2TAU11,L1_J75,L1_FJ75,L1_2FJ30,L1_XE60,L1_J50_XE20,L1_J50_XE35,L1_2J20_XE20,L1_MU0_J20_XE20,L1_TAU15_XE10_3J10,L1_TAU11_XE20,L1_TAU15_XS35,L1_EM10_XS45,L1_EM10_XE30,L1_EM5_MU6,L1_2TAU8_EM10,L1_TAU11_MU10'
    eb_physics_unpaired_seed = 'L1_EM3_UNPAIRED_ISO,L1_MU0_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU0_J15_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_FJ10_UNPAIRED_ISO'
    eb_physics_unpaired_noniso_seed = 'L1_EM3_UNPAIRED_NONISO,L1_MU0_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO'
    eb_physics = 'L1_EM5','L1_MU0','L1_TAU6','L1_J10','L1_FJ30','L1_TE400','L1_XE10','L1_XS15','L1_2EM3'
    eb_high_physics = 'L1_EM10','L1_MU6','L1_2MU0','L1_TAU20','L1_J30','L1_FJ30','L1_TE400','L1_XE30','L1_XS30','L1_2TAU11','L1_3J10','L1_TAU11_XS35','L1_2FJ10','L1_3EM5'
else:
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        eb_physics_empty_seed = 'L1_EM5_EMPTY,L1_MU4_EMPTY,L1_MU11_EMPTY,L1_2MU6_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_FJ10_EMPTY'
        eb_physics_firstempty_seed = 'L1_EM3_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_2MU6_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_J10_FIRSTEMPTY,L1_J30_FIRSTEMPTY,L1_FJ10_FIRSTEMPTY'
        eb_physics_noL1PS_seed = 'L1_EM16VH,L1_EM30,L1_MU11,L1_2MU4,L1_TAU30,L1_2TAU11,L1_J75,L1_3J20,L1_FJ75,L1_2FJ30,L1_XE60,L1_J50_XE35,L1_2J20_XE20,L1_MU4_J20_XE20,L1_TAU15_XE10_3J10,L1_TAU11_XE20,L1_TAU11_2J10_J20_XE20,L1_EM10VH_XE30,L1_EM5_MU6,L1_2TAU8_EM10VH,L1_TAU8_MU10'
        eb_physics_unpaired_seed = 'L1_EM3_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_J15_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_FJ10_UNPAIRED_ISO'
        eb_physics_unpaired_noniso_seed = 'L1_EM3_UNPAIRED_NONISO,L1_MU4_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO'
        eb_physics = ['L1_EM5','L1_MU4','L1_TAU8','L1_J10','L1_FJ30','L1_TE500','L1_XE10','L1_XS25','L1_2EM3']
        eb_high_physics = ['L1_EM10VH','L1_MU6','L1_2MU4','L1_TAU15','L1_J30','L1_FJ30','L1_TE700','L1_XE30','L1_XS30','L1_2TAU11','L1_3J10','L1_2FJ10','L1_3EM5']

    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
#         eb_physics_empty_seed = 'L1_EM6_EMPTY,L1_MU4_EMPTY,L1_MU11_EMPTY,L1_2MU4_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_FJ30_EMPTY'
#         eb_physics_firstempty_seed = 'L1_EM3_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_J10_FIRSTEMPTY,L1_J30_FIRSTEMPTY'
#         eb_physics_noL1PS_seed = 'L1_EM18VH,L1_EM30,L1_2EM10VH,L1_2EM12_EM16V,L1_3EM6_EM14VH,L1_MU15,L1_2MU6,L1_2MU4_BARRELONLY,L1_2EM6_MU6,L1_EM10VH_MU6,L1_EM6_2MU6,L1_TAU40,L1_2TAU20,L1_2TAU11I_TAU15,L1_2TAU11I_EM14VH,L1_2TAU11_TAU20_EM14VH,L1_TAU8_MU10,L1_TAU15I_XE40,L1_TAU20_XE40,L1_J75,L1_4J15,L1_J50_XE40,L1_J30_XE50,L1_3J15_J50,L1_MU10_J20,L1_MU4_J20_XE35,L1_FJ75,L1_2FJ30,L1_XE60,L1_TE800,L1_2MU4_XE40,L1_XE40_BGRP7'
#         eb_physics_unpaired_seed = 'L1_EM3_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_J15_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO'
#         eb_physics_unpaired_noniso_seed = 'L1_EM3_UNPAIRED_NONISO,L1_MU4_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO'
#         eb_physics = ['L1_EM6','L1_MU4','L1_TAU8','L1_J10','L1_FJ50','L1_TE500','L1_XE20','L1_XS25','L1_2EM3']
#         eb_high_physics = ['L1_EM10VH','L1_MU6','L1_2MU4','L1_TAU15','L1_J30','L1_TE700','L1_XE40','L1_XS30','L1_2TAU11','L1_3J10','L1_2FJ30','L1_3EM6']

        #eb_physics_noL1PS_seed = 'L1_2MU6,L1_MU6_2J20,L1_TAU15_XE40,L1_2MU10,L1_3EM6_EM14VH,L1_TAU20_XE35,L1_2FJ30,L1_2MU4_BARRELONLY,L1_XE70,L1_2TAU11I_TAU15,L1_XE40_BGRP7,L1_MU15,L1_2TAU20,L1_2EM12_EM16V,L1_XE60,L1_2MU4_MU6_BARREL,L1_TAU15_XE35,L1_2TAU11I_EM14VH,L1_XE50,L1_J50_XE35,L1_XE35_BGRP7,L1_2EM6_MU6,L1_2EM10VH,L1_2MU4_MU6,L1_XE40,L1_J350,L1_TAU15I_XE35,L1_TE700,L1_EM6_2MU6,L1_FJ75,L1_TAU20_XE40,L1_TAU8_MU10,L1_MU4_J20_XE35,L1_EM10VH_MU6,L1_J30_XE40,L1_2MU4_XE40,L1_2MU4_BARREL,L1_TE800,L1_3MU4,L1_3MU6,L1_TAU11_MU10,L1_EM18VH,L1_2TAU11_TAU20_EM14VH,L1_3J15_J50,L1_EM6_XS55,L1_MU10_J20,L1_EM30,L1_J50_XE40,L1_J30_XE50,L1_TAU15I_XE40,L1_XE50_BGRP7,L1_J75,L1_TAU40,L1_MU20,L1_EM16VH_MU4,L1_4J15'
        #eb_physics = ['L1_2EM3_EM12', 'L1_XS45', 'L1_J50', 'L1_TAU15I', 'L1_MU4_J15', 'L1_FJ30', 'L1_MU10', 'L1_J30', 'L1_EM10VH', 'L1_EM16VH', 'L1_XE30', 'L1_XE20', 'L1_3J15', 'L1_TAU20', 'L1_MU6', 'L1_EM12', 'L1_TE300']
        #eb_high_physics = ['L1_2MU4', 'L1_MU4_J30', 'L1_XS60', 'L1_TE500', 'L1_FJ50', 'L1_EM16V_XE20', 'L1_MU4_J50', 'L1_2EM6_EM16VH', 'L1_XS50']
        #eb_physics_empty_seed = 'L1_MU4_J15_EMPTY,L1_MU11_EMPTY,L1_J30_EMPTY,L1_MU4_EMPTY,L1_FJ30_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_MU10_EMPTY,L1_EM3_EMPTY,L1_EM6_EMPTY,L1_2MU4_EMPTY'
        #eb_physics_firstempty_seed = 'L1_MU20_FIRSTEMPTY,L1_EM3_FIRSTEMPTY,L1_J30_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_FJ30_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_MU10_FIRSTEMPTY,L1_J10_FIRSTEMPTY'
        #eb_physics_unpaired_seed = 'L1_MU4_J15_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO'
        #eb_physics_unpaired_noniso_seed = 'L1_MU4_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO,L1_EM3_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO'

        #New EB configuration 18.10.2012, modified on 29.11.2012

#        eb_physics_noL1PS_seed ='L1_2MU6,L1_MU6_2J20,L1_TAU15_XE40,L1_EM14VH_FJ15,L1_2MU10,L1_3EM6_EM14VH,L1_MU10_FJ15,L1_TAU20_XE35,L1_2FJ30,L1_2MU4_BARRELONLY,L1_XE70,L1_2TAU11I_TAU15,L1_XE40_BGRP7,L1_MU15,L1_2TAU20,L1_3J15_FJ15,L1_2EM12_EM16V,L1_XE60,L1_2MU4_MU6_BARREL,L1_2FJ15,L1_TAU15_XE35,L1_2TAU11I_EM14VH,L1_XE50,L1_J50_XE35,L1_XE35_BGRP7,L1_2EM6_MU6,L1_2EM10VH,L1_XE40,L1_J350,L1_TAU15I_XE35,L1_2TAU8_TAU15_FJ15,L1_EM6_2MU6,L1_FJ75,L1_2TAU8_TAU11I_EM10VH_FJ15,L1_TAU20_XE40,L1_TAU8_MU10,L1_MU4_J20_XE35,L1_EM10VH_MU6,L1_J30_XE40,L1_2MU4_XE40,L1_TE800,L1_3MU4,L1_3MU6,L1_TAU11_MU10,L1_EM18VH,L1_2TAU11_TAU20_EM14VH,L1_2TAU8_TAU11I_EM10VH_J15CF,L1_3J15_J50,L1_2TAU8_TAU15_J15CF,L1_EM6_XS55,L1_MU10_J20,L1_EM30,L1_J50_XE40,L1_J30_XE50,L1_EM14VH_J15CF,L1_TAU15I_XE40,L1_XE50_BGRP7,L1_J75,L1_TAU40,L1_MU20,L1_MU10_J15CF,L1_EM16VH_MU4,L1_4J15'

        eb_physics_noL1PS_seed ='L1_2MU6,L1_MU6_2J20,L1_TAU15_XE40,L1_EM14VH_FJ15,L1_2MU10,L1_3EM6_EM14VH,L1_MU10_FJ15,L1_TAU20_XE35,L1_2FJ30,L1_2MU4_BARRELONLY,L1_XE70,L1_2TAU11I_TAU15,L1_XE40_BGRP7,L1_MU15,L1_2TAU20,L1_3J15_FJ15,L1_2EM12_EM16V,L1_XE60,L1_2MU4_MU6_BARREL,L1_2FJ15,L1_TAU15_XE35,L1_2TAU11I_EM14VH,L1_XE50,L1_J50_XE35,L1_XE35_BGRP7,L1_2EM6_MU6,L1_2EM10VH,L1_XE40,L1_J350,L1_TAU15I_XE35,L1_2TAU8_TAU15_FJ15,L1_EM6_2MU6,L1_FJ75,L1_2TAU8_TAU11I_EM10VH_FJ15,L1_TAU20_XE40,L1_TAU8_MU10,L1_MU4_J20_XE35,L1_EM10VH_MU6,L1_J30_XE40,L1_2MU4_XE40,L1_TE800,L1_3MU4,L1_3MU6,L1_TAU11_MU10,L1_EM18VH,L1_2TAU11_TAU20_EM14VH,L1_2TAU8_TAU11I_EM10VH_J15CF,L1_3J15_J50,L1_2TAU8_TAU15_J15CF,L1_EM6_XS55,L1_MU10_J20,L1_EM30,L1_J50_XE40,L1_J30_XE50,L1_EM14VH_J15CF,L1_TAU15I_XE40,L1_XE50_BGRP7,L1_J75,L1_TAU40,L1_MU20,L1_MU10_J15CF,L1_EM16VH_MU4,L1_4J15,L1_EM6_MU6,L1_EM10VH_MU6'


        #New chain, 29.11.2012
        eb_physics_L1PS_seed ='L1_2MU6,L1_MU6_2J20,L1_TAU15_XE40,L1_EM14VH_FJ15,L1_2MU10,L1_3EM6_EM14VH,L1_MU10_FJ15,L1_TAU20_XE35,L1_2FJ30,L1_2MU4_BARRELONLY,L1_XE70,L1_2TAU11I_TAU15,L1_XE40_BGRP7,L1_MU15,L1_2TAU20,L1_3J15_FJ15,L1_2EM12_EM16V,L1_XE60,L1_2MU4_MU6_BARREL,L1_2FJ15,L1_TAU15_XE35,L1_2TAU11I_EM14VH,L1_XE50,L1_J50_XE35,L1_XE35_BGRP7,L1_2EM6_MU6,L1_2EM10VH,L1_XE40,L1_J350,L1_TAU15I_XE35,L1_2TAU8_TAU15_FJ15,L1_EM6_2MU6,L1_FJ75,L1_2TAU8_TAU11I_EM10VH_FJ15,L1_TAU20_XE40,L1_TAU8_MU10,L1_MU4_J20_XE35,L1_EM10VH_MU6,L1_J30_XE40,L1_2MU4_XE40,L1_TE800,L1_3MU4,L1_3MU6,L1_TAU11_MU10,L1_EM18VH,L1_2TAU11_TAU20_EM14VH,L1_2TAU8_TAU11I_EM10VH_J15CF,L1_3J15_J50,L1_2TAU8_TAU15_J15CF,L1_EM6_XS55,L1_MU10_J20,L1_EM30,L1_J50_XE40,L1_J30_XE50,L1_EM14VH_J15CF,L1_TAU15I_XE40,L1_XE50_BGRP7,L1_J75,L1_TAU40,L1_MU20,L1_MU10_J15CF,L1_EM16VH_MU4,L1_4J15,L1_EM6_MU6,L1_EM10VH_MU6,L1_2MU4,L1_MU4_J30,L1_XS60,L1_FJ50,L1_EM16V_XE20,L1_MU4_J50,L1_2EM6_EM16VH,L1_XS50,L1_2MU4_MU6,L1_2MU4_BARREL,L1_2MU4_2EM3,L1_EM6_MU6,L1_2EM3_EM12,L1_XS45,L1_J50,L1_MU4_J15,L1_FJ30,L1_MU10,L1_J30,L1_TE500,L1_EM10VH,L1_EM16VH,L1_TE700,L1_XE30,L1_XE20,L1_3J15,L1_TAU20,L1_MU6,L1_EM12'
        

        eb_physics_unpaired_noniso_seed ='L1_MU4_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO,L1_EM3_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO' 
        eb_physics_firstempty_seed ='L1_MU20_FIRSTEMPTY,L1_EM3_FIRSTEMPTY,L1_J30_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_FJ30_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_MU10_FIRSTEMPTY,L1_J10_FIRSTEMPTY' 

#        eb_high_physics = ['L1_2MU4', 'L1_MU4_J30', 'L1_XS60', 'L1_FJ50', 'L1_EM16V_XE20', 'L1_MU4_J50', 'L1_2EM6_EM16VH', 'L1_XS50', 'L1_2MU4_MU6', 'L1_2MU4_BARREL', 'L1_2MU4_2EM3']
        eb_high_physics = ['L1_2MU4', 'L1_MU4_J30', 'L1_XS60', 'L1_FJ50', 'L1_EM16V_XE20', 'L1_MU4_J50', 'L1_2EM6_EM16VH', 'L1_XS50', 'L1_2MU4_MU6', 'L1_2MU4_BARREL', 'L1_2MU4_2EM3','L1_EM6_MU6'] 

        eb_physics_random_seed ='L1_TAU15I,L1_2EM3,L1_TAU15,L1_TAU11,L1_J10,L1_J15,L1_TAU11I,L1_EM3,L1_2EM3_EM6,L1_MU4,L1_XS30,L1_TE300,L1_TAU8' 
        eb_physics_empty_seed ='L1_MU4_J15_EMPTY,L1_MU11_EMPTY,L1_J30_EMPTY,L1_MU4_EMPTY,L1_FJ30_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_MU10_EMPTY,L1_EM3_EMPTY,L1_EM6_EMPTY,L1_2MU4_EMPTY' 
        eb_physics = ['L1_2EM3_EM12', 'L1_XS45', 'L1_J50', 'L1_MU4_J15', 'L1_FJ30', 'L1_MU10', 'L1_J30', 'L1_TE500', 'L1_EM10VH', 'L1_EM16VH', 'L1_TE700', 'L1_XE30', 'L1_XE20', 'L1_3J15', 'L1_TAU20', 'L1_MU6', 'L1_EM12'] 
        eb_physics_unpaired_seed ='L1_MU4_J15_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO' 




class L2EFChain_generic(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [],
                           'L2_generic')

        # EF sequences
        self.addEFSequence(['L2_generic'],
                           [],
                           'EF_generic')
        pass

    def defineSignatures(self):
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['EnhancedBias']
        self.calib_streams = []
        self.groups = ['Supporting','Calibration','BW:Detector']
        self.trigger_type = ['hello']
        pass

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_generic': mergeRemovingOverlap('L2_generic', L2suf),
            'EF_generic': mergeRemovingOverlap('EF_generic', EFsuf),
            }
        pass
    
#
Config   = L2EFChain_generic.Config
L2Config = L2EFChain_generic.L2Config
EFConfig = L2EFChain_generic.EFConfig

Generic = [
    #special monitoring chain requested by savannah #50204
    L2EFChain_generic('allL1_NoAlg',
                      'L2_allL1_NoAlg', 1023, '', 'EF_allL1_NoAlg', 1023,
                      [''],
                      Config(L2Config('_allL1_NoAlg'),
                             EFConfig('_allL1_NoAlg') )),

    # To JetEtMiss stream for background study in SUSY #92149
    L2EFChain_generic('L1RD0_firstempty_NoAlg',
                      'L2_L1RD0_firstempty_NoAlg', 1021, 'L1_RD0_FIRSTEMPTY', 'EF_L1RD0_firstempty_NoAlg', 1021,
                      ['RD0_FIRSTEMPTY'],
                      Config(L2Config('_L1RD0_firstempty_NoAlg'),
                             EFConfig('_L1RD0_firstempty_NoAlg') )),

    L2EFChain_generic('L1BGRP7_NoAlg',
                      'L2_L1BGRP7_NoAlg', 7, 'L1_BGRP7', 'EF_L1BGRP7_NoAlg', 7,
                      ['L1_BGRP7'],
                      Config(L2Config('_L1BGRP7_NoAlg'),
                             EFConfig('_L1BGRP7_NoAlg') )), 


    
    ]

###########################################################################
#
#
class L2EFChain_EB(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix   = suffix

            from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo        
            from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
            
            self.dummyAlg = PESA__DummyUnseededAllTEAlgo("L2_DummyFEX"+suffix)
            self.dummyAlg.createRoIDescriptors  = False
            self.dummyAlg.NumberOfOutputTEs     = 1
            
            hypo_name = "L2_Hypo"+suffix

            if suffix == '_eb_physics':
                self.hypoEB = L1InfoHypo(hypo_name)
                self.hypoEB.L1ItemNames = eb_physics
            elif suffix == '_high_eb_physics':
                self.hypoEB = L1InfoHypo(hypo_name)
                self.hypoEB.L1ItemNames = eb_high_physics
                
            elif suffix == '_eb_minbias':
                self.hypoEB = L1InfoHypo(hypo_name)
                self.hypoEB.L1ItemNames = ['L1_MBTS_1',
                                           'L1_ZDC',
                                           'L1_LUCID']
            else:
                self.hypoEB = L1InfoHypo(hypo_name)
                self.hypoEB.L1ItemNames = []

            self.hypoEB.AlwaysPass                = False
            self.hypoEB.InvertSelection           = False
            self.hypoEB.InvertL1ItemNameSelection = False
            self.hypoEB.L1TriggerBitMask          = 0
            self.hypoEB.TriggerTypeBitMask        = 0
            self.hypoEB.UseBeforePrescaleBit      = True

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
            
    def defineSequences(self, config):
        # L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [self.config.L2Config.dummyAlg, self.config.L2Config.hypoEB],
                           'L2_eb')

    def defineSignatures(self):
        self.addL2Signature(['L2_eb'])

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['EnhancedBias']
        self.calib_streams = []
        self.groups = ['Supporting','Calibration','BW:Detector']
        self.trigger_type = ['eb']

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'L2_eb': mergeRemovingOverlap('L2_eb', L2suf),
            'EF_eb': mergeRemovingOverlap('EF_eb', EFsuf),
            }
    
#
#
#
try:
    Generic += [
        L2EFChain_EB('eb_physics',
                     'L2_eb_physics', 5550, 'L1_RD0_FILLED', 'EF_eb_physics', 5550,
                     [''],
                     L2EFChain_EB.Config(L2EFChain_EB.L2Config('_eb_physics'),
                                         L2EFChain_EB.EFConfig('_eb_physics'))),

        L2EFChain_EB('high_eb_physics',
                     'L2_high_eb_physics', 5560, 'L1_RD0_FILLED', 'EF_high_eb_physics', 5560,
                     [''],
                     L2EFChain_EB.Config(L2EFChain_EB.L2Config('_high_eb_physics'),
                                         L2EFChain_EB.EFConfig('_high_eb_physics'))),

        L2EFChain_EB('eb_physics_noL1PS',
                     'L2_eb_physics_noL1PS', 5561,
                     eb_physics_noL1PS_seed,
                     'EF_eb_physics_noL1PS', 5561,
                     [''],
                     L2EFChain_EB.Config(L2EFChain_EB.L2Config('_eb_physics_noL1PS'),
                                         L2EFChain_EB.EFConfig('_eb_physics_noL1PS'))),
        
        L2EFChain_EB('eb_minbias',
                     'L2_eb_minbias', 5551, 'L1_RD0_FILLED', 'EF_eb_minbias', 5551,
                     [''],
                     L2EFChain_EB.Config(L2EFChain_EB.L2Config('_eb_minbias'),
                                         L2EFChain_EB.EFConfig('_eb_minbias'))),
        
        L2EFChain_generic('eb_random',
                          'L2_eb_random', 5553, 'L1_RD1_FILLED', 'EF_eb_random', 5553,
                          [''],
                          Config(L2Config('_eb_random'),
                                 EFConfig('_eb_random'))),

        L2EFChain_generic('eb_physics_unpaired',
                          'L2_eb_physics_unpaired', 5554,
                          'L1_EM2_UNPAIRED,L1_MU0_UNPAIRED,L1_MU0_COMM_UNPAIRED,L1_TAU5_UNPAIRED,L1_J5_UNPAIRED,L1_FJ10_UNPAIRED',
                          'EF_eb_physics_unpaired', 5554,
                          [''],
                          Config(L2Config('_eb_random_unpaired'),
                                 EFConfig('_eb_random_unpaired'))),           

        L2EFChain_generic('eb_physics_unpaired_iso',
                          'L2_eb_physics_unpaired_iso', 5554,
                          eb_physics_unpaired_seed,
                          'EF_eb_physics_unpaired_iso', 5554,
                          [''],
                          Config(L2Config('_eb_random_unpaired_iso'),
                                 EFConfig('_eb_random_unpaired_iso'))),
        
        L2EFChain_generic('eb_physics_unpaired_noniso',
                          'L2_eb_physics_unpaired_noniso', 5564,
                          eb_physics_unpaired_noniso_seed,
                          'EF_eb_physics_unpaired_noniso', 5564,
                          [''],
                          Config(L2Config('_eb_random_unpaired_noniso'),
                                 EFConfig('_eb_random_unpaired_noniso'))),

        L2EFChain_generic('eb_physics_firstempty',
                          'L2_eb_physics_firstempty', 5555,
                          eb_physics_firstempty_seed,
                          'EF_eb_physics_firstempty', 5555,
                          [''],
                          Config(L2Config('_eb_random_firstempty'),
                                 EFConfig('_eb_random_firstempty'))),           

        L2EFChain_generic('eb_physics_empty',
                          'L2_eb_physics_empty', 5556,
                          eb_physics_empty_seed,
                          'EF_eb_physics_empty', 5556,
                          [''],
                          Config(L2Config('_eb_random_empty'),
                                 EFConfig('_eb_random_empty'))),

        L2EFChain_generic('eb_random_unpaired',
                          'L2_eb_random_unpaired', 5557, 'L1_RD0_UNPAIRED', 'EF_eb_random_unpaired', 5557,
                          [''],
                          Config(L2Config('_eb_random_unpaired'),
                                 EFConfig('_eb_random_unpaired'))),

        L2EFChain_generic('eb_random_unpaired_iso',
                          'L2_eb_random_unpaired_iso', 5557, 'L1_RD0_UNPAIRED_ISO', 'EF_eb_random_unpaired_iso', 5557,
                          [''],
                          Config(L2Config('_eb_random_unpaired_iso'),
                                 EFConfig('_eb_random_unpaired_iso'))),        

        L2EFChain_generic('eb_random_firstempty',
                          'L2_eb_random_firstempty', 5558, 'L1_RD0_FIRSTEMPTY', 'EF_eb_random_firstempty', 5558,
                          [''],
                          Config(L2Config('_eb_random_firstempty'),
                                 EFConfig('_eb_random_firstempty'))),

        L2EFChain_generic('eb_random_empty',
                          'L2_eb_random_empty', 5559, 'L1_RD0_EMPTY', 'EF_eb_random_empty', 5559,
                          [''],
                          Config(L2Config('_eb_random_empty'),
                                 EFConfig('_eb_random_empty'))),
        ]


    
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
        
        Generic += [
            L2EFChain_EB('eb_physics_L1PS',
                         'L2_eb_physics_L1PS', 5570,
                         eb_physics_L1PS_seed,
                          'EF_eb_physics_L1PS', 5570,
                         [''],
                         L2EFChain_EB.Config(L2EFChain_EB.L2Config('_eb_physics_L1PS'),
                                             L2EFChain_EB.EFConfig('_eb_physics_L1PS'))),
            
           ]
        


except:
       log = logging.getLogger( "GenericDef.py" )
       log.warning('Enhanced bias hypo can not be imported... continue without EB chains')
