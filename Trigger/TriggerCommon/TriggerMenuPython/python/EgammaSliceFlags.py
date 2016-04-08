# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList
from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig

__author__  = 'T. Bold, P.Urquijo'
__version__="$Revision: 1.42 $"
__doc__="Egamma slice specific flags  "


_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)


class doTrigEgammaRec(JobProperty):
    """ do or not to do TrigEgammaRec (EF algorithm) algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doTrigEgammaRec)

class signatures(JobProperty):
    """ signatures in Egamma slice """
    statusOn=True
    allowedTypes=['list']

    allowedValues = AllowedList(["eNoCut",
                                 'e12Tvh_loose1_AthSel',
                                 'e60_medium1_AthSel', 
                                 'e24vh_loose0_AthSel',
                                 'g20_medium_AthSel', 
                                 'g30_medium_AthSel', 
                                 'g30_medium_g20_medium_AthSel', 
                                 'g120_loose_AthSel', 

                                 'e24vhi_medium1_AthSel',
                                 'e24vhi_medium1_AthLHSel',
                                 'e24vhi_medium1_L2ET21',
                                 'e24vhi_medium1_L2ET22',
                                 'e24vhi_medium1_L2ET23',
                                 'e24vhi_medium1_L2ET24',                                 
                                 "em3_empty_noAlg","e5_NoCut",
                                 "e5_NoCut_L2SW","e5_NoCut_FwdBackTrk",
                                 "e5_L2Star_NoCut_FwdBackTrk","e5_loose_L1VTE50",
                                 "em105_passHLT", "2e5_medium", "2e5_medium_TRT",
                                 "e5_e7_medium","e5_medium_e10_loose",
                                 "e5_medium", "e5_medium_TRT",
                                 "e10_loose","e10_NoCut","e10i_loose","e10_loose_SiTrk",
                                 "e18_medium",
                                 "e20_loose_TRT",
                                 "e10_loose_IdScan","e10_loose_FwdBackTrk",
                                 "e10_loose_passL2","e10_loose_passEF",
                                 "e10_medium",
                                 "e6T_medium","e11T_medium", "e10_medium_IdScan", "e10_medium_TRT",
                                 "2e10_loose", "2e10_medium", "e12_medium",
                                 "e12vh_medium", "e12_medium_IdScan", "e12_medium_TRT",
                                 "e15_medium", "e15_medium_IdScan", "e15_loose", "e15_loose0", "e15_loose1", "e15_medium1", "e15vh_loose0", "e15i_medium",
                                 "2e15_loose","2e15_medium","3e15_medium", "2e15i_medium", "e17i_medium", "e20_loose",
                                 "e2vh_loose", "e20_loose_passL2",
                                 "e20_etcut_passL2", "e20_loose_passEF",
                                 "e20_medium", "e20_medium_passL2",  "e20_medium_passEF",
                                 "2e20_loose_passL2", "2e20_loose_passEF",
                                 "e20i_loose","e20i_loose_passL2", "e20i_loose_passEF",
                                 "e20i_medium", "e20i_medium_passL2", "e20i_medium_passEF",
                                 "e25_loose","e25_loose_IdScan", "e25_loose_TRT", "e25_medium",
                                 "e25i_loose_passL2", "e25i_loose_passEF",
                                 "e25i_medium", "e25i_medium_passL2", "e25i_medium_passEF",
                                 "2e25i_loose", "e30_loose", "e40_loose1",
                                 "e140_loose1", "2e12_tight", "e22i_tight", "e55_loose1",

                                 "g5", "g5_nocut", "g5_loose","g5_medium", "g5_loose_L1TE50",
                                 "g5_loose_2L1J5", "g5_medium_2L1J5",
                                 "g5_loose_TauMass",                                
                                 "g10_loose", "g10_medium",
				 "g12_loose", "g12_medium",
                                 "g10_loose_TauMass",
                                 "g15_medium",
                                 "g3_nocut","2g17i_tight",
				 "g5_tight", "g7_tight", "g9_tight", "g10_tight", "g12_tight", "g15_tight",
                                 "Zee_monitor", "Zee", "Jpsiee",
                                 
                                 "g12Tvh_loose", "g12Tvh_medium",
                                 "g12Tvh_loose_TauMass",
                                 "g15_loose","g15i_loose",
                                 "g20_loose","g20_loose_cnv",
                                 "g20i_loose", "g20vh_loose", 'g20Tvh_medium',
                                 'g20Tvh_loose_TauMass',
                                 'g20vh_medium', 'g20Tvh_medium',
                                 "g25_loose","g25i_loose", 
                                 "2g10_loose","2g15_loose","2g20_loose","2g40_loose",
                                 "2g10_medium",
                                 "3g10_loose","g25_tight","g25i_tight","g150","2g25_tight",
                                 "e10_medium_Ringer", "e5_NoCut_Ringer", 'g5_loose_EFFS_L1ZDC',
                                 'g0_mbts_a', 'g0_mbts_c','g0_mbts_a_EFfullcalo','g0_mbts_c_EFfullcalo','g5_EFfullcalo',
                                 'e0_mbts_a_EFfullcalo','e0_mbts_c_EFfullcalo',
                                 'e5_EFfullcalo','e10_medium_SiTrk_robust','e10_medium_SiTrk',
                                 'e20_loose_SiTrk_robust','e20_loose_SiTrk','e20_loose_IdScan',
                                 'e10_medium_L2SW',
                                 'e20_loose_L2SW','e20_loose_NoIDTrkCut','e20_loose_IDTrkNoCut',
                                 'e20_medium_IDTrkNoCut', 'e22vh_medium_IDTrkNoCut', 'e20_L2Star_IDTrkNoCut',
                                 'g40_medium', 'e5_NoCut_firstempty', 'e10_loose_cosmic',
                                 'e10_loose_passL2_cosmic', 'e10_loose_passEF_cosmic', 'e10_medium_cosmic',
                                 'g5_nocut_cosmic', 'g5_NoCut_cosmic','g10_NoCut_cosmic',
                                 'g5_nocut_unpaired', 'g5_nocut_firstempty', 'g5_nocut_mv', 'g5_EFfullcalo_cosmic',
                                 'g10_loose_cosmic', 'g0_mbts_a_cosmic',
                                 'g0_mbts_c_cosmic', 'g0_mbts_a_EFfullcalo_cosmic', 'g0_mbts_c_EFfullcalo_cosmic',
                                 'e0_mbts_a_EFfullcalo_cosmic', 'e0_mbts_c_EFfullcalo_cosmic',
                                 "2g20i_loose", 'g10i_loose',
                                 'e5_medium','e5_medium_TRT', 'e5_medium_SiTrk',
                                 'e10_loose','e10_medium', 'e10_medium_IdScan','e10_medium_TRT',
                                 'e15_tight', 'e15_loose_IdScan','e15_loose_TRT',
                                 '2g3_nocut','2e5_loose', '2g5_loose', '2e5_medium',
                                 '2e5_medium_SiTrk', '2e10_nocut',
                                 'g3_loose', '2g3_loose',  '2g10_nocut',
                                 "e10_NoCut_IdScan", "e10_NoCut_TRT",'e15_NoCut',
                                 'e15_NoCut_IdScan', 'e15_NoCut_TRT',
                                 'g7_loose', 'g7_medium', '2g5_medium', '2g7_loose', '2g7_medium',
                                 'g30_loose', 'g30_medium', 'g30_tight',
                                 'g20_tight',
                                 'g40_loose', 'g40_tight', 'g40_loose_v1', 'g40_tight_v1',
                                 'g50_etcut', '2g50_etcut', 'g150_etcut', 'g50_loose',
                                 'e10_medium_NoIDTrkCut', 'e10_medium_IDTrkNoCut', 'e5_medium_cells',
                                 'g6T_etcut', 'g9_etcut','g11_etcut', 'g11_etcut_v1',
                                 'g17_etcut','e20_etcut','e20_etcut_EM12',
                                 'e22_etcut','e22_etcut_EM5','e22_etcut_EM12','e15_loose_NoIDTrkCut',
                                 'g5_loose_cnv', 'e5_TrkCut','2e5_tight','2e5_tight1',
				 '2e5_loose1', '2e5_medium1',
                                 'e5_tight_e5_NoCut', 'e5_tight1_e5_NoCut', 'e9_tight1_e5_etcut',
                                 'e5_tight', 'e5_tight_SiTrk', 'e5_tight_TRT',
                                 'e20_medium_SiTrk', 'e22vh_medium_SiTrk', 'e20_medium_TRT',
                                 'e22vh_medium_TRT', 'g20_etcut', 'g24_etcut', 'g27_etcut',
                                 '3e10_medium', '2e12_medium', 'g60_loose', 'g80_loose',
                                 'g100_loose',
                                 'g120_loose',
                                 'g30_loose_g20_loose',
                                 'g35_loose_g25_loose',
                                 'g20_loose_jetcalibdelayed',
                                 'g40_loose_jetcalibdelayed',
                                 'g60_loose_jetcalibdelayed',
                                 'g35_loose_g30_loose',
                                 'g40_loose_g25_loose',
                                 'g40_loose_g30_loose',
                                 '2g30_loose',
                                 'g30_medium_g20_medium',
                                 'e22_medium',
                                 'e22vh_medium',
                                 'e60_loose',
                                 'g100_etcut_g50_etcut',
                                 'e22vh_loose',
                                 'g200_etcut', 
                                 'e5_tight_e5_NoCut_Jpsi', 'e5_tight_e4_etcut_Jpsi',
                                 '2e5_tight_Jpsi', '2e5_tight1_Jpsi', 'e9_tight_e5_tight_Jpsi', 'e14_tight_e5_tight_Jpsi', 
                                 'e4_etcut', 'e7_etcut', 'e9_etcut','e10_etcut', 'e9_tight', 'e9_tight',
                                 'e5_tight_e4_etcut_Jpsi', 'e5_tight_e7_etcut_Jpsi', 'e5_tight_e9_etcut_Jpsi', 
                                 'e5_tight_e4_etcut_Jpsi_SiTrk', 'e5_tight_e4_etcut_Jpsi_TRT',
                                 'e5_tight_e14_etcut_Jpsi', 'e7_tight_e14_etcut_Jpsi',
                                 'e15_tight1','e18_tight','e20_tight','e22_tight',
                                 'e20_loose1', 'e20_looseTrk',
                                 'e22vh_loose1', 'e22vh_looseTrk',
                                 'e22vh_medium1', 'e22vh_medium2',
                                 'e20_medium1', 'e20_medium2',
                                 'e22_medium1','e22_medium2', 'e22vh_medium2'
                                 '2g10_loose_NL','g100_etcut_g50_etcut',
                                 'e15_HLTtighter',
                                 'e5_medium1', 'e5_loose1', 'e5_medium1_IDTrkNoCut', 'e15_loose1_IDTrkNoCut',
                                 'e11_etcut', 'g13_etcut', 'e13_etcut', 'e13_etcutTRT','e13_etcut_EM7', 
                                 'e30_medium', 'e33_medium', 'e35_medium', 'e40_medium',
                                 'e30vh_medium2',
                                 'e25i5vh_medium2', 'e22i6vh_medium2', 'e25i6vh_medium2', 
                                 'e45_medium', 'e45_loose1', 'e60_loose1', 'e45_medium1','e60_medium1','e60_loose0', 
                                 'e20vhT_medium1' ,'e20vhT_tight1',
                                 'e20_tight_e15_NoCut_Zee',
                                 'e15_medium_e12_medium',
                                 'g5_hiptrt', 'g22_hiptrt', 'g_nocut_hiptrtL2','g_nocut_hiptrtL2_L1EM30',
                                 '2e12T_medium','2e12Tvh_medium','2e11T_medium','e11T_medium_2e6T_medium',
                                 'L1EM3_NoAlg',
                                 'L1EM10_NoAlg', 'L1EM12_NoAlg',
                                 'L1EM14_NoAlg', 'L1EM16_NoAlg',
                                 'e22_loose', 'e22_looseTrk', 'e22_loose1', 'e22vh_loose0', 'e22_loose1_EM16VH',
                                 'e22_medium_SiTrk', 'e22_medium_TRT', 'e22_medium_IDTrkNoCut',
                                 '2e15vh_loose_Zee', '2e15vh_loose1_Zee',
                                 'e20vhT_tight1_g6T_etcut_Upsi', 'e20vhT_medium1_g6T_etcut_Upsi',
                                 'e15vh_loose_e12Tvh_loose_Zee', 'e15vh_loose1_e12Tvh_loose1_Zee',
                                 'e12_loose', 'e12T_loose',
                                 'e15_loose_EM10VH','e15_loose1_EM10VH','e12T_loose_EM10VH',
                                 'e12T_loose1_EM10VH',
                                 'e25_loose2', 
                                 'e25_loose2_xe20_noMu_recoil40',
                                 '2e15vh_medium',
                                 'e15vh_medium',
                                 'e12Tvh_medium_2e6T_medium',
                                 'e12Tvh_medium', # 'e12Tvh_medium1',
                                 'e13_etcutTrk', 'e13_etcutTrk_EM7',
                                 'e20_etcutTrk', 'e20_etcutTrk_EM12',
                                 'e22_etcutTrk', 
                                 'e9_tight_e4_etcut_Jpsi', 'e14_tight_e4_etcut_Jpsi',
                                 'e15_loose_e12T_loose_Zee',
                                 'e45_etcut','e60_etcut',
                                 # di-bosons
                                 'e22vh_loose1_e10_loose1',
                                 #E.P.
                                 'e22vh_medium1_e5_medium',
                                 'e10_loose1', 'e10_medium1',
                                 'e10_loose1_EM3',
                                 'e25_medium1', 'e25vh_medium1', 
                                 'e22_etcut_e10_loose1',
                                 'e22_loose1_e10_etcut',
                                 'e5_medium_L2StarA', 'e5_medium_L2StarB', 'e5_medium_L2StarC',
                                 'e12_medium_L2StarA', 'e12_medium_L2StarB', 'e12_medium_L2StarC', 
                                 'e22vh_medium_L2StarA', 'e22vh_medium_L2StarB', 'e22vh_medium_L2StarC', 
                                 'e22vh_medium_L2Star_IDTrkNoCut',
                                 'e24vhi_medium1',
                                 'e24vh_medium1',
                                 'e24vhi_loose1',
                                 '2e12Tvh_loose1',
                                 'e12Tvh_loose1_FTK',
                                 '2e12Tvh_loose1_FTK',
                                 'e12Tvh_loose1',
                                 'e22vhi_medium1',
                                 'e5_tight1',
                                 'e15vh_medium1',
                                 'e18vh_medium',
                                 'e18vh_medium1',
                                 'e7T_loose1',
                                 ##JPsi trigger
                                 'e5_tight1_e4_etcut_Jpsi',
                                 'e5_tight1_e9_etcut_Jpsi',
                                 'e5_tight1_e14_etcut_Jpsi',
                                 'e7_tight1',
                                 'e9_tight1', 
                                 'e9_tight1_e4_etcut_Jpsi',
                                 'e14_tight1',
                                 'e14_tight1_e4_etcut_Jpsi',
                                 ##Supporting trigger
                                 'e24vh_loose',
                                 'e24vh_loose0',
                                 'e24vh_loose1',
                                 'e24vh_medium1_FTK',
                                 'e24vh_medium1_SiTrk',
                                 'e24vh_medium1_IdScan',
                                 'e24vh_medium1_TRT',
                                 'e24vh_medium1_L2StarB', 'e24vh_medium1_L2StarC',
                                 'e24vh_medium1_IDTrkNoCut',
                                 'e24vh_medium1_IDTrkNoCut_IDT',
                                 'e22vh_medium1_SiTrk',
                                 'e22vh_medium1_IdScan',
                                 'e22vh_medium1_IDTrkNoCut',
                                 'e22vh_medium1_TRT',
                                 'e5_tight1_e4_etcut_Jpsi_SiTrk',
                                 'e5_tight1_e4_etcut_Jpsi_IdScan',
                                 'e5_tight1_e4_etcut_Jpsi_TRT',
                                 'e5_tight1_e4_etcut_Jpsi_L2StarB',
                                 'e5_tight1_e4_etcut_Jpsi_L2StarC',
                                 'e5_tight1_IdScan',
                                 'e5_tight1_SiTrk',
                                 'e5_tight1_TRT',
                                 'e5_etcut',
                                 'e5_loose0',
                                 ###
                                 'e24vh_tight1',
                                 'e24vh_tight1_e15_NoCut_Zee',
                                 ###
                                 '2g15vh_loose_g10_loose',
                                 '3g15vh_loose',
                                 '2g15vh_medium_g10_medium',
                                 '2g15vh_medium',
                                 '3g15vh_medium',
                                 'e24vh_medium1_e7_medium1',
                                 'e18vh_medium1_2e7T_medium1',
                                 '2g20vh_medium',
                                 'e18_loose1',
                                 'e18_medium1',
                                 ## For efficiency measurement studies
                                 'g20_medium', 'g15vh_loose', 'g15vh_medium',
                                 'e7T_medium1', 'e12Tvh_medium1', 'e7_medium1', 'e7_loose1',
                                 'e7_medium1_L2StarB', 'e12Tvh_loose1_L2StarB', '2e12Tvh_loose1_L2StarB',
                                 'e12Tvh_medium1_L2StarB', 'e7T_loose1_L2StarB', 'e7T_medium1_L2StarB',
                                 #for alfa run
                                 'e5_tight1_ALFA_ANY',
                                 'e5_tight1_ALFA_MBTS_C',
                                 'e5_tight1_ALFA_MBTS_A',
                                 'e5_tight1_ALFA_ANY_UNPAIREDISO',
                                 'e5_tight1_ALFA_MBTS_C_UNPAIREDISO',
                                 'e5_tight1_ALFA_MBTS_A_UNPAIREDISO',
                                 # Duplicated L2StarB chains
                                 'e24vh_medium1_e7_medium1_L2StarB',
                                 'e18vh_medium1_2e7T_medium1_L2StarB',
                                 #Brem chain
                                 'e24vhi_medium1_L2StarABrem',
                                 'e24vhi_medium1_noL2ID',
                                 'e24vhi_medium1_noL2IDnoEFID',                                 
                                 'e24vhi_medium1_noL2IDnoEF',                                 
                                 'e24vhi_medium1_noL2noEFID',
                                 'e24vhi_medium1_noL2',
                                 #combined tracking
                                 'e24vhi_medium1_combinedTrk2step',
                                 #'e24vhi_medium1_noL2IDnoEF',
                                 #NoCut chains with IDScan and SiTrack
                                 #"e5_NoCut_R1",
                                 #'e15_NoCut_R1', 
                                 #'e5_medium1_IDTrkNoCut_R1',
                                 #'e15_loose1_IDTrkNoCut_R1',
                                 #'e22_medium_IDTrkNoCut_R1',
                                 #'e20_loose_IDTrkNoCut_R1', 
                                 #'e22vh_medium_IDTrkNoCut_R1', 
                                 'e24vh_medium1_IDTrkNoCut_R1',
                                 #'e22vh_medium1_IDTrkNoCut_R1',
                                 #'e20_medium_IDTrkNoCut_R1', 
                                 ])
    StoredValue   = []


_flags.append(signatures)



# create container
class EgammaSlice(JobPropertyContainer, CommonSliceHelper):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
TriggerFlags.add_Container(EgammaSlice)

# add add common slice flags
TriggerFlags.EgammaSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.EgammaSlice.add_JobProperty(flag)
del _flags

# make an alias
EgammaSliceFlags = TriggerFlags.EgammaSlice

##########################################################################################
#
# Exception class
#
##########################################################################################
class EgammaMapError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)


##########################################################################################
#
# Maps of chain signatures to L1 items
#
##########################################################################################

def getEM_ItemFromChain(sig_id):

    e18meditem = "L1_EM12"
    e25item = "L1_EM16"
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus(): 
        e20tightseed = "L1_EM14"
        e15nocutseed = "L1_EM10"
        e33medseed = "L1_EM16"
        e35medseed = "L1_EM16"
        e40medseed = "L1_EM16"
        e20medseed = "L1_EM14"
    else:
        e20tightseed = "L1_EM12"
        e15nocutseed = "L1_EM10VH"
        e33medseed = "L1_EM18VH"
        e35medseed = "L1_EM18VH"
        e40medseed = "L1_EM30"
        e20medseed = "L1_EM16VH"


    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
        e11etcutseed = "L1_EM6"
        e13etcutseed = "L1_EM6"
        e20etcutseed = "L1_EM16V"
    elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        e11etcutseed = "L1_EM5"
        e13etcutseed = "L1_EM7"
        e20etcutseed = "L1_EM12"
    else:
        e11etcutseed = "L1_EM5"
        e13etcutseed = "L1_EM10" 
        e20etcutseed = "L1_EM14"

    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
        e7_tight1_seed = "L1_EM3"
        e9_tight1_seed = "L1_EM5"
    else:
        e7_tight1_seed = "L1_EM5"
        e9_tight1_seed = "L1_EM6"

    # This dict stores default mapping
    SigL1Map = {
        'DefaultMap': {
        'e5_tight1_ALFA_ANY'        :'L1_EM3_ALFA_ANY',
        'e5_tight1_ALFA_MBTS_C'     :'L1_EM3_ALFA_MBTS_C',
        'e5_tight1_ALFA_MBTS_A'     :'L1_EM3_ALFA_MBTS_A',       
        'e5_tight1_ALFA_ANY_UNPAIREDISO'        :'L1_EM3_ALFA_ANY_UNPAIRED_ISO',
        'e5_tight1_ALFA_MBTS_C_UNPAIREDISO'     :'L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO',
        'e5_tight1_ALFA_MBTS_A_UNPAIREDISO'     :'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO',       
        'em3_empty_noAlg'           :'L1_EM3_EMPTY',
        'e5_NoCut'                  :'L1_EM3',
        'e5_NoCut_R1'               :'L1_EM3',
        'e5_etcut'                  :'L1_EM3',
        'e5_loose0'                  :'L1_EM3',
        'e5_NoCut_firstempty'       :'L1_EM3_FIRSTEMPTY',
        'e10_NoCut'                 :'L1_EM5',
        'e10_NoCut_IdScan'          :'L1_EM5',
        'e10_NoCut_TRT'             :'L1_EM5',
        'e5_medium'                 :'L1_EM3',
        'e5_loose1'                 :'L1_EM3',
        'e5_medium1'                :'L1_EM3',
        'e5_medium1_IDTrkNoCut'     :'L1_EM3',
        'e5_medium1_IDTrkNoCut_R1'  :'L1_EM3',
        'e5_medium_SiTrk'           :'L1_EM3',
        'e5_medium_TRT'             :'L1_EM3',
        'e5_tight'                  :'L1_EM3',
        'e5_tight1'                 :'L1_EM3',
        'e5_tight1_SiTrk'           :'L1_EM3',
        'e5_tight1_IdScan'          :'L1_EM3',
        'e5_tight1_TRT'             :'L1_EM3',
        'e5_tight_SiTrk'            :'L1_EM3',
        'e5_tight_TRT'              :'L1_EM3',
        'e5_loose_L1VTE50'          :'L1_EM3_VTE50',
        'e5_TrkCut'                 :'L1_EM3',
        'e7T_loose1'                :'L1_EM6',
        'e7T_medium1'               :'L1_EM6',
        'e7_loose1'                 :'L1_EM3',
        'e7_medium1'                :'L1_EM3',
        'e7_tight_L1EM7'            :'L1_EM7',
        'e7_tight'                  :'L1_EM5',
        'e9_tight'                  :'L1_EM7',
        'e7_tight1'                 : e7_tight1_seed,
        'e9_tight1'                 : e9_tight1_seed,
        'e10_nocut'                 :'L1_EM5',
        'e10_loose'                 :'L1_EM5',
        'e10_loose1'                :'L1_EM5',
        'e10_medium1'               :'L1_EM5',
        'e10_loose1_EM3'            :'L1_EM3',
        'e10_loose_cosmic'          :'L1_EM5_EMPTY',
        'e10_medium_cosmic'         :'L1_EM5_EMPTY',
        'e10_medium_NoIDTrkCut'     :'L1_EM5',
        'e10i_loose'                :'L1_EM5I',
        'e6T_medium'                :'L1_EM5',
        'e11T_medium'               :'L1_EM10',
        'e12T_medium'               :'L1_EM10',
        'e12Tvh_medium'             :'L1_EM10VH',
        'e12Tvh_medium1'            :'L1_EM10VH',
        'e12Tvh_loose1'             :'L1_EM10VH',
        'e12Tvh_loose1_AthSel'             :'L1_EM10VH',
        'e12Tvh_loose1_FTK'         :'L1_EM10VH',
        'e12_medium'                :'L1_EM7',
        'e12_medium_IdScan'         :'L1_EM7',
        'e12_medium_TRT'            :'L1_EM7',
        'e11_etcut'                 : e11etcutseed,
        'e13_etcut'                 :'L1_EM10',
        'e13_etcutTRT'              :'L1_EM10', 
        'e13_etcut_EM7'             :'L1_EM7', 
        'g13_etcut'                 :'L1_EM10', 
        'e13_etcutTrk'              :e13etcutseed,
        'e13_etcutTrk_EM7'          :'L1_EM7',
        'e20_etcutTrk'              : e20etcutseed,
        'e20_etcutTrk_EM12'         :'L1_EM12',
        'e22_etcutTrk'              :'L1_EM16',
        'e14_etcut'                 :'L1_EM10',
        'e20_etcut'                 :'L1_EM14',
        'e20_etcut_EM12'            :'L1_EM12',
        'e22_etcut'                 :'L1_EM16', 
        'e22_etcut_EM5'             :'L1_EM5',
        'e22_etcut_EM12'            :'L1_EM12',
        'e25_loose2'                :'L1_EM16VH', 
        'e14_tight'                 :'L1_EM12',
        'e14_tight1'                :'L1_EM12',
        'e14_etcut_L1EM7'           :'L1_EM7', 
        'e14_etcut_L1EM12'           :'L1_EM12', 
        'e15_NoCut'                 : e15nocutseed,
        'e15_NoCut_R1'              : e15nocutseed,
        'e15_NoCut_IdScan'          :'L1_EM10',
        'e15_NoCut_TRT'             :'L1_EM10',
        'e15_loose'                 :'L1_EM10',
        'e15_loose0'                :'L1_EM10',
        'e15_loose1'                :'L1_EM10',
        'e15_loose1_IDTrkNoCut'     :'L1_EM10',
        'e15_loose1_IDTrkNoCut_R1'  :'L1_EM10',
        'e15_medium1'               :'L1_EM10',
        'e15vh_loose0'              :'L1_EM10VH',
        'e15_loose_EM10VH'          :'L1_EM10VH',
        'e15_loose1_EM10VH'         :'L1_EM10VH',
        'e15_loose_IdScan'          :'L1_EM10',
        'e15_loose_NoIDTrkCut'      :'L1_EM10',
        'e15_medium'                :'L1_EM10',
        'e15vh_medium'              :'L1_EM10VH',
        'e15vh_medium1'             :'L1_EM10VH',
        'e15_medium_IdScan'         :'L1_EM10',
        'e15_tight'                 :'L1_EM10',
        'e15_HLTtighter'            :'L1_EM10',
        'e15i_loose'                :'L1_EM10I',
        'e18_loose1'                :'L1_EM12',
        'e18_medium1'               :'L1_EM12',
        'e18_medium'                : e18meditem,
        'e18_tight'                 :'L1_EM12',
        #'e18T_medium1'              :'L1_EM14VH',
        'e18vh_medium'              :'L1_EM14VH',
        'e18vh_medium1'             :'L1_EM14VH',
        'e20vhT_medium1'            :'L1_EM16VH',
        'e20vhT_tight1'             :'L1_EM16VH',
        'e20_loose'                 :'L1_EM14',
        'e22_loose'                 :'L1_EM16',
        'e22vh_loose0'              :'L1_EM16VH',
        'e22vh_loose'               :'L1_EM16VH',
        'e22vh_loose1'              :'L1_EM16VH',
        'e20_tight'                 : e20tightseed,
        'e22_tight'                 :'L1_EM14',
        'e60_loose'                 :'L1_EM30',
        'e60_loose1'                :'L1_EM30', 
        'e60_medium1'               :'L1_EM30',
        'e60_medium1_AthSel'        :'L1_EM30',
        'e60_loose0'               :'L1_EM30',
        'e20i_loose'                :'L1_EM14I',
        'e20_medium_SiTrk'          : 'L1_EM14',
        'e22vh_medium_SiTrk'        : 'L1_EM16VH',
        'e20_medium_TRT'            : 'L1_EM14',
        'e22vh_medium_TRT'          : 'L1_EM16VH',
        'e22vh_medium1_SiTrk'       : 'L1_EM16VH',
        'e22vh_medium1_IdScan'      : 'L1_EM16VH',
        'e22vh_medium1_IDTrkNoCut' :'L1_EM16VH',
        'e22vh_medium1_IDTrkNoCut_R1':'L1_EM16VH',
        'e22vh_medium1_TRT'       : 'L1_EM16VH',
        'g27_etcut'                 : 'L1_EM14',
        'e22i_tight'                :'L1_EM14I',
        'e24_medium1'              :'L1_EM18VH',
        'e24vh_loose'              :'L1_EM18VH',
        'e24vh_loose0'             :'L1_EM18VH',
        'e24vh_loose0_AthSel'      :'L1_EM18VH',
        'e24vh_loose1'             :'L1_EM18VH',
        'e24vh_medium1'             :'L1_EM18VH',
        'e24vh_medium1_SiTrk'      :'L1_EM18VH',
        'e24vh_medium1_IdScan'      :'L1_EM18VH',
        'e24vh_medium1_TRT'        :'L1_EM18VH',
        'e24vh_medium1_FTK'        :'L1_EM18VH',        
        'e24vh_medium1_IDTrkNoCut' :'L1_EM18VH',        
        'e24vh_medium1_IDTrkNoCut_R1':'L1_EM18VH',        
        'e24vh_medium1_IDTrkNoCut_IDT':'L1_EM18VH',        
        'e24vhi_medium1'            :'L1_EM18VH',
        'e24vhi_medium1_L2StarABrem':'L1_EM18VH',
        'e24vhi_medium1_combinedTrk2step':'L1_EM18VH',
        'e24vhi_medium1_noL2ID'     :'L1_EM18VH',        
        'e24vhi_medium1_noL2IDnoEFID': 'L1_EM18VH',
        'e24vhi_medium1_noL2IDnoEF': 'L1_EM18VH',
        'e24vhi_medium1_noL2noEFID' : 'L1_EM18VH',
        'e24vhi_medium1_noL2'       :'L1_EM18VH',        
        'e24vhi_medium1_L2ET21'     :'L1_EM18VH',
        'e24vhi_medium1_L2ET22'     :'L1_EM18VH',
        'e24vhi_medium1_L2ET23'     :'L1_EM18VH',
        'e24vhi_medium1_L2ET24'     :'L1_EM18VH',
        'e24vhi_medium1_AthSel'     :'L1_EM18VH',
        'e24vhi_medium1_AthLHSel'   :'L1_EM18VH',                                
        'e24vhi_loose1'             :'L1_EM18VH',
        'e24vh_tight1'             :'L1_EM18VH',
        'e25_loose'                 : e25item,
        'e25_loose_IdScan'          : e25item,
        'e25_loose_TRT'             : e25item,
        'e25i_loose'                :'L1_EM14I',
        'e30_loose'                 :'L1_EM14',
        'e10_loose_IdScan'          :'L1_EM5',
        'e20_loose_IdScan'          :'L1_EM14',
        'e12_loose'                 :'L1_EM7',
        'e12T_loose'                :'L1_EM10',
        'e12T_loose_EM10VH'         :'L1_EM10VH',
        'e12T_loose1_EM10VH'        :'L1_EM10VH',
        'e12_tight'                 :'L1_EM7',
        'e15i_medium'               :'L1_EM10I',
        'e17i_medium'               :'L1_EM10I',
        'e20_medium'                :'L1_EM14',
        'e22i6vh_medium2'           :'L1_EM16VH',
        'e25i5vh_medium2'           :'L1_EM16VH',
        'e25i6vh_medium2'           :'L1_EM16VH',
        'e30_medium'                :'L1_EM16',
        'e30vh_medium2'             :'L1_EM16VH',
        'e33_medium'                : e33medseed,
        'e35_medium'                : e35medseed,
        'e40_medium'                : e40medseed,
        'e45_medium'                :'L1_EM30',
        'e45_medium1'               :'L1_EM30',
        'e45_loose1'               :'L1_EM30',
        'e20_loose1'                :'L1_EM14', 
        'e20_looseTrk'              :'L1_EM14',
        'e22vh_looseTrk'            :'L1_EM16VH',
        'e20_medium1'               : e20medseed,
        'e20_medium2'               : e20medseed,
        'e22_medium1'               :'L1_EM16',
        'e22vh_medium1'             :'L1_EM16VH',
        'e22vhi_medium1'            :'L1_EM16VH',
        'e22_medium2'               :'L1_EM16',
        'e22vh_medium2'             :'L1_EM16VH',
        'e20i_medium'               :'L1_EM14I',
        'e25_medium'                : e25item,
        'e25_medium1'               : 'L1_EM16',
        'e25vh_medium1'             : 'L1_EM16VH',
        'e22_loose'                 :'L1_EM16',
        'e22_loose1'                :'L1_EM16',
        'e22_loose1_EM16VH'         :'L1_EM16VH',
        'e22_looseTrk'              :'L1_EM16',
        'e22_medium'                :'L1_EM16',
        'e22vh_medium'              :'L1_EM16VH',
        'e22_medium_SiTrk'          :'L1_EM16',
        'e22_medium_TRT'            :'L1_EM16',
        'e22_medium_IDTrkNoCut'     :'L1_EM16',
        'e22_medium_IDTrkNoCut_R1'  :'L1_EM16',
        'e25i_medium'               :'L1_EM14I',
        'e40_loose1'                :'L1_EM14',
        'e45_etcut'                 :'L1_EM30',
        'e60_etcut'                 :'L1_EM30',
        'e55_loose1'                :'L1_EM14',
        'e105_loose1'               :'L1_EM85',
        'e140_loose1'               :'L1_EM85',
        'e20_loose_TRT'             :'L1_EM14',
        'e10_loose_SiTrk'           :'L1_EM5',
        'e10_medium_SiTrk_robust'   :'L1_EM5',
        'e10_medium_SiTrk'          :'L1_EM5',
        'e10_medium'                :'L1_EM5',
        'e10_medium_IdScan'         :'L1_EM5',
        'e10_medium_TRT'            :'L1_EM5',
        'e20_loose_SiTrk_robust'    :'L1_EM14',
        'e20_loose_SiTrk'           :'L1_EM14',
        'e5_NoCut_FwdBackTrk'       :'L1_EM3',
        'e5_L2Star_NoCut_FwdBackTrk':'L1_EM3',
        'e10_loose_FwdBackTrk'      :'L1_EM5',
        'e20_loose_NoIDTrkCut'      :'L1_EM14',
        'e20_loose_IDTrkNoCut'      :'L1_EM14',
        'e20_loose_IDTrkNoCut_R1'   :'L1_EM14',
        'e20_medium_IDTrkNoCut'     :'L1_EM14',
        'e20_medium_IDTrkNoCut_R1'  :'L1_EM14',
        'e20_L2Star_IDTrkNoCut'     :'L1_EM14',
        'e22vh_medium_IDTrkNoCut'   :'L1_EM16VH',
        'e22vh_medium_IDTrkNoCut_R1':'L1_EM16VH',
        'e22vh_medium_L2Star_IDTrkNoCut'     :'L1_EM16VH',
        'eNoCut'                    :'L1_EM3',
        'e5_NoCut_Ringer'           :'L1_EM3',
        'xeL2Mon'                   :'L1_EM3',
        'e10_medium_Ringer'         :'L1_EM5',
        'e5_NoCut_L2SW'             :'L1_EM3',
        'e15_tight1'                :'L1_EM10', 
        'e4_etcut'                  :'L1_EM3', 
        'e7_etcut'                  :'L1_EM3', 
        'e9_etcut'                  :'L1_EM3', 
        'e10_etcut'                 :'L1_EM5',
        'e4_etcut_SiTrk'            :'L1_EM3', 
        'e4_etcut_TRT'              :'L1_EM3',
        'e4_etcut_IdScan'           :'L1_EM3', 
        'e5_loose'                  :'L1_EM3',
        'e10_medium_L2SW'           :'L1_EM5',
        'e20_loose_L2SW'            :'L1_EM14'}, # end of DefaultMap
        }

    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        SigL1Map['e13_etcut'] = 'L1_EM7'
        ##SigL1Map['e13_etcutTrk'] = 'L1_EM7'
        SigL1Map['e13_etcutTrk_EM7'] = 'L1_EM7'
        ##SigL1Map['e20_etcutTrk'] = 'L1_EM12',
        SigL1Map['e20_etcutTrk_EM12'] = 'L1_EM12'
        SigL1Map['e22_etcutTrk'] = 'L1_EM12'

    log  = logging.getLogger( "EgammaSliceFlags" )
    
    #
    # First, check for menu specific value
    #
    menu = TriggerFlags.triggerMenuSetup.get_Value()


    if menu in SigL1Map:
        menuDict = SigL1Map[menu]
        if sig_id in menuDict:
            item =  menuMap[sig_id]
            log.debug('Found L1 item: menu, signature, item:'+menu+', '+sig_id+', '+item)
            return item

    #
    # Now look in default map
    #
    deftDict = SigL1Map['DefaultMap']
    if sig_id in deftDict:
        item = deftDict[sig_id]
        log.debug('Found default L1 item: signature, item:'+sig_id+', '+item)
        return item
    
    raise EgammaMapError('Unknown menu, signature: '+menu+', '+sig_id)
    return 'unknown'    

##########################################################################################
#
# Get list of L1 thresholds (L2 input TEs) for L1 item
#
##########################################################################################
def getEM_InTEFromItemConfig(item):
    
    log = logging.getLogger( "EgammaSliceFlags" )
    tpc = TriggerPythonConfig.currentTriggerConfig()

    if item not in tpc.allItems.keys():
        raise EgammaMapError('L1 item is not among all keys: '+item)

    resItem = tpc.allItems[item]
    resTEs  = resItem.thresholdNames()

    log.info('Found TEs for item: '+item+str(resTEs))
    return resTEs

def getEM_InTEFromItem(item):
    
    L1Map = {'L1_EM2':            ['EM2'],
             'L1_EM2_UNPAIRED':   ['EM2'],
             'L1_EM3':            ['EM3'],
             'L1_EM3_VTE50':      ['EM3'],
             'L1_EM3_MV':         ['EM3'],
             'L1_EM3_NL':         ['EM3'],
             'L1_EM3_EMPTY':      ['EM3'],
             'L1_EM3_EMPTY':      ['EM3'],
             'L1_EM3_UNPAIRED':   ['EM3'],
             'L1_EM3_FIRSTEMPTY': ['EM3'],
             'L1_EM3_ALFA_ANY': ['EM3'],
             'L1_EM3_ALFA_MBTS_C': ['EM3'],
             'L1_EM3_ALFA_MBTS_A': ['EM3'],
             'L1_EM3_ALFA_ANY_UNPAIRED_ISO': ['EM3'],
             'L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO': ['EM3'],
             'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO': ['EM3'],
             'L1_EM4':            ['EM4'],
             'L1_EM5':            ['EM5'],
             'L1_EM6':            ['EM6'],
             'L1_EM7':            ['EM7'],
             'L1_EM5_EMPTY':      ['EM5'],
             'L1_EM5I':           ['EM5I'],
             'L1_EM10':           ['EM10'],
             'L1_EM10VH':         ['EM10VH'],
             'L1_EM10_NL':        ['EM10'],
             'L1_EM12':           ['EM12'],
             'L1_EM10I':          ['EM10I'],
             'L1_EM14':           ['EM14'],
             'L1_EM14VH':         ['EM14VH'],
             'L1_EM16':           ['EM16'],
             'L1_EM16V':          ['EM16V'],
             'L1_EM16VH':         ['EM16VH'],
             'L1_EM18VH':         ['EM18VH'],
             'L1_EM14I':          ['EM14I'],
             'L1_EM23I':          ['EM23I'],
             'L1_EM30':           ['EM30'],
             'L1_EM85':           ['EM85']
             }

    if item in L1Map:
        return L1Map[item]

    raise EgammaMapError('Unknown item:'+item)
    return ['unknown']
