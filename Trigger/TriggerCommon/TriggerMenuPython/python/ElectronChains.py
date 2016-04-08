# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__doc__=""
__version__="Implementation of Electron Slice single electron signatures"

from TriggerMenuPython.ElectronDef import *

##########################################################################################
#
# Create chain objects for all configured chains
#
##########################################################################################


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
seeds = ",".join([ x for x in TriggerFlags.Lvl1.items() if "EM" in x ])


Electrons = [        
    ##################################
    # Chains using IDSCAN 
    ##################################
    
    #L2EFChain_e_NoIDTrkCut('e5_NoCut_R1', 2340, 
    #                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(), L2ElectronFex_1(), L2ElectronHypo_e5_NoCut()), 
    #                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e5_NoCut()  ))),
    #
    L2EFChain_e_NoIDTrkCut('e5_NoCut', 2341, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(), L2ElectronFex_1(), L2ElectronHypo_e5_NoCut()), 
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e5_NoCut()  ))),
    
        
    L2EFChain_e('e5_NoCut_firstempty', 961, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(), L2ElectronFex_1(), L2ElectronHypo_e5_NoCut()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e5_NoCut()  ))),
    
    L2EFChain_e('e10_NoCut', 998, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_NoCut(), L2ElectronFex_1(), L2ElectronHypo_e10_NoCut()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_NoCut(), TrigEFElectronHypo_e10_NoCut()  ))),
    
    L2EFChain_e_IdScan_HI('e5_loose_L1VTE50', 930,
                          L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_loose(), L2ElectronFex_IdScan(), L2ElectronHypo_e5_loose()),
                                              L2EFChain_e.EFConfig('',EFTrackHypo_e5_loose(), TrigEFElectronHypo_e5_loose() ))),
    
    L2EFChain_e('e10_loose', 4513,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e10_loose()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose(), TrigEFElectronHypo_e10_loose()  ))),
    
    L2EFChain_e('e10_loose1', 854,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e10_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose1(), TrigEFElectronHypo_e10_loose1()  ))),

    L2EFChain_e('e10_medium1', 149,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e10_medium1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium1(), TrigEFElectronHypo_e10_medium1()  ))),

    L2EFChain_e('e10_loose1_EM3', 5,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e10_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose1(), TrigEFElectronHypo_e10_loose1()  ))),
    
    L2EFChain_e('e15_loose', 4543, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose(), TrigEFElectronHypo_e15_loose()  ))),
    
    L2EFChain_e('e15_loose0', 520,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose0()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose0(), TrigEFElectronHypo_e15_loose0()  ))),

    L2EFChain_e('e15_loose1', 523, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose1(), TrigEFElectronHypo_e15_loose1()  ))),

    L2EFChain_e('e15_medium1', 532,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium1(), TrigEFElectronHypo_e15_medium1()  ))),

    L2EFChain_e('e15_loose_EM10VH', 777, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose(), TrigEFElectronHypo_e15_loose()  ))),
    
    L2EFChain_e('e15_loose1_EM10VH', 778, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose1(), TrigEFElectronHypo_e15_loose1()  ))),
    
    L2EFChain_e('e14_tight', 2114,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e14_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e14_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e14_tight(), TrigEFElectronHypo_e14_tight()  ))),
    
    L2EFChain_e('e14_tight1', 960,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e14_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e14_tight1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e14_tight1(), TrigEFElectronHypo_e14_tight1()  ))),
    
    #medium uses only SiTrk
    L2EFChain_e('e15_tight', 114,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_tight(), TrigEFElectronHypo_e15_tight()  ))),
    
    L2EFChain_e('e15_HLTtighter', 115,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_HLTtighter(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_HLTtighter()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_tight(), TrigEFElectronHypo_e15_HLTtighter() ))),
    
    L2EFChain_e_NoIDTrkCut('e15_NoCut', 531,
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_NoCut(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_NoCut()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e15_NoCut(), TrigEFElectronHypo_e15_NoCut() ))),

    L2EFChain_e_NoIDTrkCut('e15_NoCut_R1', 2342,
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_NoCut(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_NoCut()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e15_NoCut(), TrigEFElectronHypo_e15_NoCut() ))),

    L2EFChain_e('e18_tight', 24, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e18_tight(), TrigEFElectronHypo_e18_tight() ))),
    
##L2EFChain_e('e18T_medium1', 29, 
    ##                   L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e18_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e18_medium1()), 
    ##                                       L2EFChain_e.EFConfig('',EFTrackHypo_e18_medium1(), TrigEFElectronHypo_e18_medium1() ))),
    ##
    
    L2EFChain_e('e20_tight', 25, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_tight(), TrigEFElectronHypo_e20_tight() ))),
    
    L2EFChain_e('e22_tight', 46, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_tight(), TrigEFElectronHypo_e22_tight() ))),
    
    L2EFChain_e('e15_tight1', 2600,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_tight(), TrigEFElectronHypo_e15_tight1()  ))),
    
    L2EFChain_e('e25_loose2', 5171, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25_loose1(), TrigEFElectronHypo_e25_loose2() ))),
    
    L2EFChain_e('e25_medium', 299, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25_medium(), TrigEFElectronHypo_e25_medium() ))),
    
##     ##################################
##     # Chains using TRTSegFinder
##     ##################################
    
    
    L2EFChain_e_TRT('e5_medium_TRT', 2006, 
                    L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e5_medium("L2CaloHypo_e5_medium_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e5_medium_TRT()), 
                                            L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e5_medium("EFTrackHypo_e5_medium_TRT"), TrigEFElectronHypo_e5_medium_TRT()  ))),
    
    L2EFChain_e_TRT('e5_tight_TRT', 2330, 
                    L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e5_tight("L2CaloHypo_e5_tight_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e5_tight_TRT()), 
                                            L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e5_tight("EFTrackHypo_e5_tight_TRT"), TrigEFElectronHypo_e5_tight_TRT()  ))),

    L2EFChain_e_TRT('e5_tight1_TRT', 1009, 
                    L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e5_tight1("L2CaloHypo_e5_tight1_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e5_tight1("L2ElectronHypo_e5_tight1_TRT")), 
                                            L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e5_tight1("EFTrackHypo_e5_tight1_TRT"), TrigEFElectronHypo_e5_tight1("EFElectronHypo_e5_tight1_TRT")  ))),
    
    L2EFChain_e_TRT('e20_loose_TRT', 2016, 
                    L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e20_loose("L2CaloHypo_e20_loose_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e20_loose_TRT()),
                                            L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e20_loose("EFTrackHypo_e20_loose_TRT"), TrigEFElectronHypo_e20_loose_TRT()  ))),

    L2EFChain_e_TRT_Ringer('e20_medium_TRT', 105, 
                    L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e20_medium("L2CaloHypo_e20_medium_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e20_medium_TRT()),
                                            L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e20_medium("EFTrackHypo_e20_medium_TRT"), TrigEFElectronHypo_e20_medium_TRT()  ))),


    ##     ##################################
    ##     # Chains using SiTrack
    ##     ##################################

    L2EFChain_e_SiTrack('e5_medium_SiTrk', 1629, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium("L2CaloHypo_e5_medium_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e5_medium("L2ElectronHypo_e5_medium_SiTrk")),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium("EFTrackHypo_e5_medium_SiTrk"), TrigEFElectronHypo_e5_medium("TrigEFElectronHypo_e5_medium_SiTrk")  ))), 
    
    L2EFChain_e_SiTrack('e5_tight_SiTrk', 2329, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight("L2CaloHypo_e5_tight_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e5_tight("L2ElectronHypo_e5_tight_SiTrk")),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight("EFTrackHypo_e5_tight_SiTrk"), TrigEFElectronHypo_e5_tight("TrigEFElectronHypo_e5_tight_SiTrk")  ))), 


    L2EFChain_e_SiTrack('e5_tight1_SiTrk', 1008, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1("L2CaloHypo_e5_tight1_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e5_tight1("L2ElectronHypo_e5_tight1_SiTrk")),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1("EFTrackHypo_e5_tight1_SiTrk"), TrigEFElectronHypo_e5_tight1("TrigEFElectronHypo_e5_tight1_SiTrk")  ))), 
    
    

    ##     ##################################
    ##     # Chains using IdScan
    ##     ##################################

    L2EFChain_e_IdScan('e5_tight1_IdScan', 1009, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1("L2CaloHypo_e5_tight1_IdScan"), L2ElectronFex_IdScan(), L2ElectronHypo_e5_medium1("L2ElectronHypo_e5_tight1_IdScan")), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1("EFTrackHypo_e5_tight1_IdScan"), TrigEFElectronHypo_e5_tight1("TrigEFElectroHypo_e5_tight1_IdScan") ))),
    
    
    L2EFChain_e_L2Star('e5_tight1_L2StarB', l2=Chain(name='L2_e5_tight1_L2StarB', counter=3374, seed='L1_EM3'),
                       strategy='B', l1threshold='EM3'), 
    L2EFChain_e_L2Star('e5_tight1_L2StarC', l2=Chain(name='L2_e5_tight1_L2StarC', counter=3375, seed='L1_EM3'),
                       strategy='C', l1threshold='EM3'), 
    
    ###########################
    
    L2EFChain_e('e5_medium', 28, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium(), TrigEFElectronHypo_e5_medium()  ))),
    
    L2EFChain_e('e5_tight', 978, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight(), TrigEFElectronHypo_e5_tight()  ))),
    
    L2EFChain_e('e5_tight1', 979, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),

    L2EFChain_e('e7_tight', 5411, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight(), TrigEFElectronHypo_e7_tight()  ))),
    
    L2EFChain_e('e5_loose1', 146,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_loose1(), TrigEFElectronHypo_e5_loose1()  ))),

    L2EFChain_e('e5_medium1', 116, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium1(), TrigEFElectronHypo_e5_medium1()  ))),
    
    L2EFChain_e('e7_loose1', 145,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_loose1(), TrigEFElectronHypo_e7_loose1()  ))),

    L2EFChain_e('e7_medium1', 952, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_medium1(), TrigEFElectronHypo_e7_medium1()  ))),
    
    L2EFChain_e('e7_tight_L1EM7', 5412, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight(), TrigEFElectronHypo_e7_tight()  ))),
    
    L2EFChain_e('e7T_loose1', 535,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_loose1(), TrigEFElectronHypo_e7_loose1()  ))),
    
    L2EFChain_e('e7T_medium1', 48, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_medium1(), TrigEFElectronHypo_e7_medium1()  ))),

    L2EFChain_e('e9_tight', 5410, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e9_tight(), TrigL2ElectronHypo_1=L2ElectronHypo_e9_tight()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight(), TrigEFElectronHypo_e9_tight()  ))),
    
    L2EFChain_e('e7_tight1', 414,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e7_tight1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_tight1(), TrigEFElectronHypo_e7_tight1()  ))),

    L2EFChain_e('e9_tight1', 996, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e9_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e9_tight1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e9_tight1()  ))),

    L2EFChain_e('e10_medium', 30, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e10_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium(), TrigEFElectronHypo_e10_medium()  ))),
    
    L2EFChain_e('e12T_loose', 7819, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e12_loose(), TrigEFElectronHypo_e12_loose()  ))),
    
    L2EFChain_e_NoIDTrkCut('e5_medium1_IDTrkNoCut', 234,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium1("L2CaloHypo_e5_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e5_medium1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium1_NoTrk(), TrigEFElectronHypo_e5_medium1_NoTrk()  ))),
    
    L2EFChain_e_NoIDTrkCut('e15_loose1_IDTrkNoCut', 235,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose1("L2CaloHypo_e15_loose1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e15_loose1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose1_NoTrk(), TrigEFElectronHypo_e15_loose1_NoTrk()  ))),

    
    #L2EFChain_e_NoIDTrkCut('e5_medium1_IDTrkNoCut_R1', 2343,
    #                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium1("L2CaloHypo_e5_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e5_medium1_NoTrk()),
    #                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium1_NoTrk(), TrigEFElectronHypo_e5_medium1_NoTrk()  ))),
    #
    L2EFChain_e_NoIDTrkCut('e15_loose1_IDTrkNoCut_R1', 2344,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose1("L2CaloHypo_e15_loose1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e15_loose1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose1_NoTrk(), TrigEFElectronHypo_e15_loose1_NoTrk()  ))),

    




    L2EFChain_e('e12T_loose1_EM10VH', 7821, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e12_loose1(), TrigEFElectronHypo_e12_loose1()  ))),
    
    L2EFChain_e('e11T_medium', 5421, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e11t_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e11t_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e11t_medium(), TrigEFElectronHypo_e11t_medium()  ))),
    
    L2EFChain_e('e6T_medium', 5422, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e6t_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e6t_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e6t_medium(), TrigEFElectronHypo_e6t_medium()  ))),
    
    L2EFChain_e('e12_medium', 818, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium(), TrigEFElectronHypo_e12_medium()  ))),

    L2EFChain_e('e18_loose1', 608, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e18_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e18_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e18_loose1(), TrigEFElectronHypo_e18_loose1() ))),

    #L2EFChain_e('e15_medium1', 117, 
    #            L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium1()), 
    #                                L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium1(), TrigEFElectronHypo_e15_medium1() ))),

    L2EFChain_e('e18_medium1', 50, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e18_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e18_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e18_medium1(), TrigEFElectronHypo_e18_medium1() ))),
    
    L2EFChain_e('e18_medium', 4523, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e18_medium(), TrigEFElectronHypo_e18_medium() ))),
    
    L2EFChain_e('e20_medium', 297, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e20_medium() ))),
    
    L2EFChain_e('e25_loose', 32, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25_loose(), TrigEFElectronHypo_e25_loose()  ))),
    
    L2EFChain_e('e30_loose', 31, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e30_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e30_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e30_loose(), TrigEFElectronHypo_e30_loose()  ))),
    
    L2EFChain_e('e25i5vh_medium2', 113, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_medium2()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25_medium2(), TrigEFElectronHypo_e25i5_medium2() ))),
    
    L2EFChain_e('e22i6vh_medium2', 114, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium2()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium2(), TrigEFElectronHypo_e22i6_medium2() ))),
    
    L2EFChain_e('e25i6vh_medium2', 3862, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_medium2()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25_medium2(), TrigEFElectronHypo_e25i6_medium2() ))),
    
    L2EFChain_e('e30_medium', 3831, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e30_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e30_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e30_medium()  ))),
    
    L2EFChain_e('e30vh_medium2', 115, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e30_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e30_medium2()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e30_medium(), TrigEFElectronHypo_e30_medium2()  ))),
    
    L2EFChain_e('e33_medium', 248, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e33_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e33_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e33_medium()  ))),
    
    L2EFChain_e('e35_medium', 249, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e35_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e35_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e35_medium()  ))),
    
    L2EFChain_e('e40_medium', 265, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e40_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e40_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e40_medium(), TrigEFElectronHypo_e40_medium()  ))),
    
    L2EFChain_e('e45_medium', 546, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e45_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e45_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e45_medium(), TrigEFElectronHypo_e45_medium() ))),
    
    L2EFChain_e('e45_loose1', 2144, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e45_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e45_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e45_loose1(), TrigEFElectronHypo_e45_loose1() ))),

    L2EFChain_e('e45_medium1', 547, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e45_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e45_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e45_medium1(), TrigEFElectronHypo_e45_medium1() ))),

    L2EFChain_e('e60_loose0', 138, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e60_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e60_loose0()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e60_loose0(), TrigEFElectronHypo_e60_loose0() ))),
    
    L2EFChain_e('e60_loose', 527, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e60_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e60_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e60_loose(), TrigEFElectronHypo_e60_loose()  ))),
    
    L2EFChain_e('e60_loose1', 215, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e60_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e60_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e60_loose1(), TrigEFElectronHypo_e60_loose1()  ))),

    L2EFChain_e('e60_medium1', 676, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e60_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e60_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e60_medium1(), TrigEFElectronHypo_e60_medium1()  ))),

    L2EFChain_e('e4_etcut', 2150,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e4_EtCut()  ))),

    L2EFChain_e('e5_etcut', 117,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e5_EtCut()  ))),

    L2EFChain_e('e5_loose0', 118, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_loose0()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_loose0(), TrigEFElectronHypo_e5_loose0() ))),

    L2EFChain_e('e7_etcut', 2151,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e7_EtCut()  ))),
    
    L2EFChain_e('e9_etcut', 2152,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e9_EtCut()  ))),
    
    L2EFChain_e('e10_etcut', 27,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e10_EtCut()  ))),
    
    L2EFChain_e('e11_etcut', 140,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e11_EtCut()  ))),
    
    L2EFChain_e('e13_etcut', 2253,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e13_EtCut()  ))),
    
    L2EFChain_e('e13_etcut_EM7', 2254,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e13_EtCut()  ))),
    
    L2EFChain_e('e14_etcut_L1EM12', 2285,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e14_EtCut()  ))),
    
    L2EFChain_e('e14_etcut', 2259,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e14_EtCut()  ))),
    
##    L2EFChain_e('e15_etcut', 2255,
##                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
##                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e15_EtCut()  ))),

    L2EFChain_e('e15vh_loose0', 139, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_loose0()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose0(), TrigEFElectronHypo_e15_loose0() ))),
    
    L2EFChain_e('e20_etcut', 5200,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e20_EtCut()  ))),
    
    L2EFChain_e('e22_etcut_EM12', 5203,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e22_EtCut()  ))),
    
    L2EFChain_e('e22_etcut', 5201,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e22_EtCut()  ))),
    
    L2EFChain_e('e45_etcut', 5204,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e45_EtCut()  ))),
    
    L2EFChain_e('e60_etcut', 5205,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e60_EtCut()  ))),
    
    L2EFChain_e('e22_etcut_EM5', 5202,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e22_EtCut()  ))),
    
    L2EFChain_e('e13_etcutTRT', 2254,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e13_EtCutTRT()  ))),
    
    L2EFChain_e_SiTrack('e4_etcut_SiTrk', 2153,
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), L2ElectronFex_SiTrack(), L2ElectronHypo_e5_NoCut()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e4_EtCut() ))),
    
    L2EFChain_e_TRT('e4_etcut_TRT', 2154,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e4_EtCut() ))),
    
    L2EFChain_e_IdScan('e4_etcut_IdScan', 2155,
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), L2ElectronFex_IdScan(), L2ElectronHypo_e5_NoCut()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e4_EtCut() ))),
    
    L2EFChain_e_L2Star('e4_etcut_L2StarB', l2=Chain(name='L2_e4_etcut_L2StarB', counter=314, seed='L1_EM3'),
                       strategy='B', l1threshold='EM3'), 
    L2EFChain_e_L2Star('e4_etcut_L2StarC', l2=Chain(name='L2_e4_etcut_L2StarC', counter=315, seed='L1_EM3'),
                       strategy='C', l1threshold='EM3'),
    
    L2EFChain_e('e20_medium1', 529, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium1(), TrigEFElectronHypo_e20_medium1() ))),
    
    L2EFChain_e('e20_medium2', 51, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e20_medium2() ))),
    
    # etcutTrk configuration
    #used in W T&P - etcut at L2
    L2EFChain_e('e13_etcutTrk', 2280,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e13_etcutTrk(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e13_EtCutTrk()  ))),
    
    L2EFChain_e('e13_etcutTrk_EM7', 2281,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e13_EtCutTrk()  ))),

    #used in W T&P - etcut at L2
    L2EFChain_e('e20_etcutTrk', 2282,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_etcutTrk(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e20_EtCutTrk()  ))),
    
    L2EFChain_e('e20_etcutTrk_EM12', 2283,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e20_EtCutTrk()  ))),
    
    L2EFChain_e('e22_etcutTrk', 2284,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1('L2CaloHypo_NoCut'), TrigL2ElectronHypo_1=L2ElectronHypo_e5_NoCut()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(), TrigEFElectronHypo_e22_EtCutTrk()  ))),
    
    L2EFChain_e('e22_loose', 170, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose(), TrigEFElectronHypo_e22_loose() ))),
    
    L2EFChain_e('e22_loose1', 171, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose1(), TrigEFElectronHypo_e22_loose1() ))),
    
    L2EFChain_e('e22_loose1_EM16VH', 174,
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose1()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose1(), TrigEFElectronHypo_e22_loose1() ))),
    
    L2EFChain_e('e22_looseTrk', 172, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_looseTrk(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_looseTrk()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22_looseTrk(), TrigEFElectronHypo_e22_looseTrk() ))),
    
    L2EFChain_e_SiTrack('e22_medium_SiTrk', 173, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22_medium_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e22_medium("L2ElectronHypo_e22_medium_SiTrk")),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium("EFTrackHypo_e22_medium_SiTrk"), TrigEFElectronHypo_e22_medium("TrigEFElectronHypo_e22_medium_SiTrk")  ))),
    
    ##     ##################################
    ##     # Chains using Forward then Back Tracking
    ##     ##################################
    
    L2EFChain_e_fwdBackTrk('e5_NoCut_FwdBackTrk', 2175,
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut("L2CaloHypo_e5_NoCut_FwdBackTrk"),
                                                                    L2ElectronFex_FwdBackTracking_NoCut(), L2ElectronHypo_e5_NoCut_FwdBackTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut("EFTrackHypo_e5_NoCut_FwdBackTrk"),
                                                                    TrigEFElectronHypo_e5_NoCut("TrigEFElectronHypo_e5_NoCut_FwdBackTrk")  ))),
        
    ##     ###########################################
    ##     # Chains using No track info in hypo algos 
    ##     ##########################################

    L2EFChain_e_NoIDTrkCut('e22_medium_IDTrkNoCut', 175,
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium_NoTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium_NoTrk(), TrigEFElectronHypo_e22_medium_NoTrk()  ))),

    L2EFChain_e_NoIDTrkCut('e20_loose_IDTrkNoCut', 997, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose("L2CaloHypo_e20_loose_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_loose_NoTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_NoTrk(), TrigEFElectronHypo_e20_loose_NoTrk()  ))),
    
    L2EFChain_e_NoIDTrkCut('e22_medium_IDTrkNoCut_R1', 2345,
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium_NoTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium_NoTrk(), TrigEFElectronHypo_e22_medium_NoTrk()  ))),

    L2EFChain_e_NoIDTrkCut('e20_loose_IDTrkNoCut_R1', 2346, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose("L2CaloHypo_e20_loose_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_loose_NoTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_NoTrk(), TrigEFElectronHypo_e20_loose_NoTrk()  ))),
    


    ##     ##################################
    ##     # Chains with Ringer Algorithm
    ##     ##################################
    
    L2EFChain_e_Ringer('e5_NoCut_Ringer', 290, 
                       L2EFChain_e.Config( L2EFChain_e_Ringer.L2Config('',TrigRingerNeuralFexConfig_e5(),TrigRingerNeuralHypoConfig_e5_NoCut(), L2ElectronFex_all_Ringer_NoCut(), L2ElectronHypo_e5_NoCut("L2ElectronHypo_e5_NoCut_Ringer")),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut("EFTrackHypo_e5_NoCut_Ringer"), TrigEFElectronHypo_e5_NoCut("TrigEfElectronHypo_e5_NoCut_Ringer")  ))),


    ##     ################################################
    ##     # Chains with Sliding window cluster finder @ L2
    ##     ###############################################
    
    L2EFChain_e_L2SW_NoCut('e5_NoCut_L2SW', 514, 
                     L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut("L2CaloHypo_e5_NoCut_L2SW"), L2ElectronFex_all_L2SW_NoCut(), L2ElectronHypo_e5_NoCut("L2ElectronHypo_e5_NoCut_L2SW")), 
                                         L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut("EFTrackHypo_e5_NoCut_L2SW"), TrigEFElectronHypo_e5_NoCut("TrigElectronHypo_e5_NoCut_L2SM")  ))),

]

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus()\
 or TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):
    
    Electrons += [
        
        L2EFChain_e_TRT_Ringer('e22vh_medium_TRT', 105, 
                               L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22vh_medium_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e22_medium_TRT()),
                                                       L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e22_medium("EFTrackHypo_e22vh_medium_TRT"), TrigEFElectronHypo_e22_medium_TRT()  ))),
        
        L2EFChain_e('e12Tvh_medium', 235, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12t_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e12t_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12t_medium(), TrigEFElectronHypo_e12t_medium()  ))),
        
        L2EFChain_e('e12Tvh_medium1', 235, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium1(), TrigEFElectronHypo_e12_medium1()  ))),
        
        L2EFChain_e('e12Tvh_loose1', 234, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_loose1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12_loose1(), TrigEFElectronHypo_e12_loose1()  ))),


        L2EFChain_e_L2Star('e12Tvh_loose1_L2StarB', l2=Chain(name='L2_e12Tvh_loose1_L2StarB', counter=416, seed='L1_EM10VH'),
                           strategy='B', l1threshold='EM10VH'),

        L2EFChain_e_L2Star('e12Tvh_medium1_L2StarB', l2=Chain(name='L2_e12Tvh_medium1_L2StarB', counter=577, seed='L1_EM10VH'),
                           strategy='B', l1threshold='EM10VH'),
        
        
        L2EFChain_e_SiTrack('e22vh_medium_SiTrk', 104, 
                            L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22vh_medium_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e22_medium("L2ElectronHypo_e22vh_medium_SiTrk")),
                                                L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium("EFTrackHypo_e22vh_medium_SiTrk"), TrigEFElectronHypo_e22_medium("TrigEFElectronHypo_e22vh_medium_SiTrk")  ))),  
        
        L2EFChain_e('e15vh_medium', 3, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium(), TrigEFElectronHypo_e15_medium() ))),  
        
        L2EFChain_e('e15vh_medium1', 10, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium1(), TrigEFElectronHypo_e15_medium1() ))),  
        

        ###
        L2EFChain_e('e18vh_medium', 8, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e18_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e18_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e18_medium(), TrigEFElectronHypo_e18_medium() ))),  
        
        L2EFChain_e('e18vh_medium1', 9, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e18_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e18_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e18_medium1(), TrigEFElectronHypo_e18_medium1() ))),  
        ####
        
        L2EFChain_e('e22vh_medium', 528, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium(), TrigEFElectronHypo_e22_medium() ))),
        
        L2EFChain_e('e22vh_loose0', 628, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose0()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose0(), TrigEFElectronHypo_e22_loose0() ))),
        
        L2EFChain_e('e22vh_loose1', 401, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose1(), TrigEFElectronHypo_e22_loose1() ))),
        
        L2EFChain_e('e22vh_looseTrk', 402, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_looseTrk(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_looseTrk()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_looseTrk(), TrigEFElectronHypo_e22_looseTrk() ))),
        
        L2EFChain_e('e22vh_medium1', 49, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1(), TrigEFElectronHypo_e22_medium1() ))),  
        
        L2EFChain_e('e22vh_medium2', 175, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium2()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium2(), TrigEFElectronHypo_e22_medium2() ))),
        
        L2EFChain_e_NoIDTrkCut('e22vh_medium_IDTrkNoCut', 530,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22vh_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium_NoTrk(), TrigEFElectronHypo_e22_medium_NoTrk()  ))),
        
        L2EFChain_e_NoIDTrkCut('e22vh_medium_IDTrkNoCut_R1', 2347,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22vh_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium_NoTrk(), TrigEFElectronHypo_e22_medium_NoTrk()  ))),
        


        L2EFChain_e('e22vhi_medium1', 180, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1(), TrigEFElectronHypo_e22i5_medium1() ))),
        
        L2EFChain_e('e24vhi_medium1', 178, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),

        L2EFChain_e_CombinedTracking2step('e24vhi_medium1_combinedTrk2step', 3186, 
                                          L2EFChain_e_CombinedTracking2step.Config(
                L2EFChain_e_CombinedTracking2step.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                L2EFChain_e_CombinedTracking2step.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))), 
        

        L2EFChain_e('e24vhi_loose1', 1018, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_loose1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_loose1(), TrigEFElectronHypo_e24i5_loose1() ))),
        
        L2EFChain_e('e24vh_medium1', 179,
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24_medium1() ))),  
        
        L2EFChain_e('e24vh_tight1', 176,
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_tight1(), TrigEFElectronHypo_e24_tight1() ))),  
        
        L2EFChain_e('e24vh_loose', 181, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_loose()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_loose(), TrigEFElectronHypo_e24_loose() ))),  
        
        L2EFChain_e('e24vh_loose0', 632, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_loose0()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_loose0(), TrigEFElectronHypo_e24_loose0() ))),

        
        L2EFChain_e('e24vh_loose1', 182, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_loose1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_loose1(), TrigEFElectronHypo_e24_loose1() ))),  
        
        
        L2EFChain_e('e25vh_medium1', 1349, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e25_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e25_medium1(), TrigEFElectronHypo_e25_medium1() ))),  
        
        #needed for upsi chains 90881
        L2EFChain_e('e20vhT_medium1', 1414, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium1(), TrigEFElectronHypo_e20_medium1() ))),
        
        L2EFChain_e('e20vhT_tight1', 1415, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e20_tight1(), TrigEFElectronHypo_e20_tight1() ))),  


        L2EFChain_e('e24vhi_medium1_L2ET21', 3176, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1_L2ET21(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),

        L2EFChain_e('e24vhi_medium1_L2ET22', 3177, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1_L2ET22(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),

        L2EFChain_e('e24vhi_medium1_L2ET23', 3178, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1_L2ET23(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),

        L2EFChain_e('e24vhi_medium1_L2ET24', 3179, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1_L2ET24(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),


        ]

    ###Test Triggers for new algos
    try:
        from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e24i5_medium1_AthSel
        print 'AthSel code does exist'
        configAthSel=True
    except:
        print 'AthSel code does not exist'
        configAthSel=False


    if  configAthSel:
        Electrons += [

            L2EFChain_e('e60_medium1_AthSel', 5676, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e60_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e60_medium1()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e60_medium1(), TrigEFElectronHypo_e60_medium1_AthSel()  ))),
            
            L2EFChain_e('e12Tvh_loose1_AthSel', 5234, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_loose1()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e12_loose1(), TrigEFElectronHypo_e12_loose1_AthSel()  ))),
            
            L2EFChain_e('e24vh_loose0_AthSel', 5632, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_loose0(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_loose0()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e24_loose0(), TrigEFElectronHypo_e24_loose0_AthSel() ))),

            
            L2EFChain_e( 'e24vhi_medium1_AthSel', 3187, 
                         L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                             L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1_AthSel() ))), 
    
            L2EFChain_e('e24vhi_medium1_AthLHSel', 3188, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1_AthLHSel() ))), 
            ]
        
    ###Supporting triggers for alternative tracking
    Electrons += [
        
        L2EFChain_e_SiTrack('e24vh_medium1_SiTrk', 1001, 
                            L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e24_medium1("L2ElectronHypo_e24_medium1_SiTrk")),
                                                L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1("EFTrackHypo_e24_medium1_SiTrk"), TrigEFElectronHypo_e24_medium1("TrigEFElectronHypo_e24_medium1_SiTrk")  ))), 
        

        L2EFChain_e_IdScan('e24vh_medium1_IdScan', 1002, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_IdScan"), L2ElectronFex_IdScan(), L2ElectronHypo_e24_medium1("L2ElectronHypo_e24_medium1_IdScan")), 
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1("EFTrackHypo_e24_medium1_IdScan"), TrigEFElectronHypo_e24_medium1("TrigEFElectroHypo_e24_medium1_IdScan") ))),  
        
        L2EFChain_e_TRT('e24vh_medium1_TRT', 1013, 
                        L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e24_medium1("L2ElectronHypo_e24_medium1_TRT")), 
                                                L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e24_medium1("EFTrackHypo_e24_medium1_TRT"), TrigEFElectronHypo_e24_medium1("EFElectronHypo_e24_medium1_TRT")  ))),
        
        L2EFChain_e_L2Star('e7_medium1_L2StarB', l2=Chain(name='L2_e7_medium1_L2StarB', counter=414, seed='L1_EM3'),
                           strategy='B', l1threshold='EM3'),

        # Need to override hypos by hand to get medium instead of loose
        #L2EFChain_e_L2Star('e7T_medium1_L2StarB', l2=Chain(name='L2_e7T_medium1_L2StarB', counter=5134, seed='L1_EM6'),
        #                   strategy='B', l1threshold='EM6', l2calohypo=L2CaloHypo_e7_medium1(), l2electronhypo=L2ElectronHypo_e7_medium1_L2StarB(),
        #                   eftrackhypo=EFTrackHypo_e7_medium1(), efelectronhypo=TrigEFElectronHypo_e7_medium1()),
        
        #L2EFChain_e_L2Star('e7T_loose1_L2StarB', l2=Chain(name='L2_e7T_loose1_L2StarB', counter=5133, seed='L1_EM6'),
        #                   strategy='B', l1threshold='EM6', l2calohypo=L2CaloHypo_e7_loose1(), l2electronhypo=L2ElectronHypo_e7_loose1_L2StarB(),
        #                                              eftrackhypo=EFTrackHypo_e7_loose1(), efelectronhypo=TrigEFElectronHypo_e7_loose1()),

        L2EFChain_e_L2Star('e7T_medium1_L2StarB', l2=Chain(name='L2_e7T_medium1_L2StarB', counter=327, seed='L1_EM6'),
                           strategy='B', l1threshold='EM6'),
        
        L2EFChain_e_L2Star('e7T_loose1_L2StarB', l2=Chain(name='L2_e7T_loose1_L2StarB', counter=634, seed='L1_EM6'),
                           strategy='B', l1threshold='EM6'),

        L2EFChain_e_L2Star('e7_loose1_L2StarB', l2=Chain(name='L2_e7_loose1_L2StarB', counter=5123, seed='L1_EM3'),
                           strategy='B', l1threshold='EM3'),
        
        L2EFChain_e_L2Star('e12_medium1_L2StarB', l2=Chain(name='L2_e12_medium1_L2StarB', counter=5124, seed='L1_EM7'),
                           strategy='B', l1threshold='EM3'),
        
        L2EFChain_e_L2Star('e24vh_medium1_L2StarB', l2=Chain(name='L2_e24vh_medium1_L2StarB', counter=316, seed='L1_EM18VH'),
                           strategy='B', l1threshold='EM18VH'), 

        L2EFChain_e_L2Star('e24vh_medium1_L2StarC', l2=Chain(name='L2_e24vh_medium1_L2StarC', counter=317, seed='L1_EM18VH'),
                           strategy='C', l1threshold='EM18VH'), 


        L2EFChain_e_TRT('e22vh_medium1_TRT', 1014, 
                        L2EFChain_e_TRT.Config( L2EFChain_e_TRT.L2Config('',L2CaloHypo_e22_medium1("L2CaloHypo_e22_medium1_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e22_medium1("L2ElectronHypo_e22_medium1_TRT")), 
                                                L2EFChain_e_TRT.EFConfig('',EFTrackHypo_e22_medium1("EFTrackHypo_e22_medium1_TRT"), TrigEFElectronHypo_e22_medium1("EFElectronHypo_e22_medium1_TRT")  ))),
        
        L2EFChain_e_NoIDTrkCut('e24vh_medium1_IDTrkNoCut', 1004,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e24_medium1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1_NoTrk(), TrigEFElectronHypo_e24_medium1_NoTrk()  ))),


        L2EFChain_e_NoIDTrkCut_IDT('e24vh_medium1_IDTrkNoCut_IDT', 2450,
                                          L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e24_medium1_NoTrk()),
                                                              L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1_NoTrk(), TrigEFElectronHypo_e24_medium1_NoTrk()  ))),        
        
        L2EFChain_e_NoIDTrkCut('e22vh_medium1_IDTrkNoCut', 1007,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1("L2CaloHypo_e22_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1_NoTrk(), TrigEFElectronHypo_e22_medium1_NoTrk()  ))),
        
        L2EFChain_e_NoIDTrkCut('e24vh_medium1_IDTrkNoCut_R1', 2348,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e24_medium1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1_NoTrk(), TrigEFElectronHypo_e24_medium1_NoTrk()  ))),
        
        L2EFChain_e_NoIDTrkCut('e22vh_medium1_IDTrkNoCut_R1', 2349,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1("L2CaloHypo_e22_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium1_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1_NoTrk(), TrigEFElectronHypo_e22_medium1_NoTrk()  ))),
        


        L2EFChain_e_SiTrack('e22vh_medium1_SiTrk', 1005, 
                            L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1("L2CaloHypo_e22_medium1_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e22_medium1("L2ElectronHypo_e22_medium1_SiTrk")),
                                                L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1("EFTrackHypo_e22_medium1_SiTrk"), TrigEFElectronHypo_e22_medium1("TrigEFElectronHypo_e22_medium1_SiTrk")  ))), 
        

        L2EFChain_e_IdScan('e22vh_medium1_IdScan', 1006, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1("L2CaloHypo_e22_medium1_IdScan"), L2ElectronFex_IdScan(), L2ElectronHypo_e22_medium1("L2ElectronHypo_e22_medium1_IdScan")), 
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1("EFTrackHypo_e22_medium1_IdScan"), TrigEFElectronHypo_e22_medium1("TrigEFElectroHypo_e22_medium1_IdScan") ))),  

        #FTK FOR MC TEST ONLY
        L2EFChain_e('e12Tvh_loose1_FTK', 2897, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_loose1_FTK()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12_loose1(), TrigEFElectronHypo_e12_loose1()  ))),
        
        L2EFChain_e('e24vh_medium1_FTK', 2898, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24vh_medium1_FTK()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24_medium1() ))),  
        
        ]


    try:
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA_Brem 
        doABrem=True
    except:
        doABrem=False

    if doABrem:
        Electrons += [
            
            ###Brem trigger chains (#101493)
            L2EFChain_e_L2StarABrem('e24vhi_medium1_L2StarABrem', 3180, 
                                    L2EFChain_e_L2StarABrem.Config( L2EFChain_e_L2StarABrem.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                                                    L2EFChain_e_L2StarABrem.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))), 

            ]

    # Electrons += [

    #     L2EFChain_e_CombinedTracking2step('e24vhi_medium1_combinedTrk2step', 3189, 
    #                                       L2EFChain_e_CombinedTracking2step.Config(
    #             L2EFChain_e_CombinedTracking2step.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
    #             L2EFChain_e_CombinedTracking2step.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))), 
        
    #     ]


    Electrons += [
        #EXAMPLE
        #       L2EFChain_e_NoIDTrkCut('e24vh_medium1_IDTrkNoCut', 1004,
        #                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e24_medium1_NoTrk()),
        #                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium1_NoTrk(), TrigEFElectronHypo_e24_medium1_NoTrk()  ))),
 
        #101493 (1)
        L2EFChain_e_noL2ID('e24vhi_medium1_noL2ID', 3181,
                           L2EFChain_e_noL2ID.Config( L2EFChain_e_noL2ID.L2Config('', L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk")),
                                                      L2EFChain_e_noL2ID.EFConfig('', EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))), 
        #101493 (2)
        L2EFChain_e_noL2IDnoEFID('e24vhi_medium1_noL2IDnoEFID', 3182,
                                 L2EFChain_e_noL2IDnoEFID.Config( L2EFChain_e_noL2IDnoEFID.L2Config('', L2CaloHypo_e24_medium1("L2CaloHypo_e24_medium1_NoTrk")),
                                                                  L2EFChain_e_noL2IDnoEFID.EFConfig('', EFTrackHypo_e24_medium1_NoTrk(),TrigEFElectronHypo_e24_medium1_NoTrk() ))), 
        #101493 (3)
        L2EFChain_e_noL2IDnoEF('e24vhi_medium1_noL2IDnoEF', 3183,
                               L2EFChain_e_noL2IDnoEF.Config( L2EFChain_e_noL2IDnoEF.L2Config('', L2CaloHypo_e24_medium1_L2ET24()),
                                                              L2EFChain_e_noL2IDnoEF.EFConfig('', EFTrackHypo_e24_medium1_NoTrk(),TrigEFElectronHypo_e24_medium1_NoTrk() ))),
        #101493 (4)
        L2EFChain_e_noL2noEFID('e24vhi_medium1_noL2noEFID', 3184,
                               L2EFChain_e_noL2noEFID.Config( L2EFChain_e_noL2noEFID.L2Config('', L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                                              L2EFChain_e_noL2noEFID.EFConfig('', EFTrackHypo_e24_medium1_NoTrk(),TrigEFElectronHypo_e24_medium1_NoTrk() ))), 
        #101493 (5)
        L2EFChain_e_noL2('e24vhi_medium1_noL2', 3185, 
                         L2EFChain_e_noL2.Config( L2EFChain_e_noL2.L2Config('',L2CaloHypo_e24_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e24_medium1()), 
                                                  L2EFChain_e_noL2.EFConfig('',EFTrackHypo_e24_medium1(), TrigEFElectronHypo_e24i5_medium1() ))),                      


        ]


if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):
    Electrons += [
        #triggers for 2012 alfa running
        L2EFChain_e('e5_tight1_ALFA_ANY', 777, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),
        
        L2EFChain_e('e5_tight1_ALFA_MBTS_C', 778, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),
        
        L2EFChain_e('e5_tight1_ALFA_MBTS_A', 783, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),

        L2EFChain_e('e5_tight1_ALFA_ANY_UNPAIREDISO', 784, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),
        
        L2EFChain_e('e5_tight1_ALFA_MBTS_C_UNPAIREDISO', 781, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),
        
        L2EFChain_e('e5_tight1_ALFA_MBTS_A_UNPAIREDISO', 782, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_tight1(), TrigL2ElectronHypo_1=L2ElectronHypo_e5_tight1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_tight1(), TrigEFElectronHypo_e5_tight1()  ))),
        
        ]



##e22vh_loose for previous menu is defined in ElectronDef_v2.py
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    Electrons += [
        L2EFChain_e('e22vh_loose', 534, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_loose(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_loose()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_loose(), TrigEFElectronHypo_e22_loose() ))),
        ]
        



    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus():
    Electrons += [
        
        L2EFChain_e('e12T_medium', 5420, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e12_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium(), TrigEFElectronHypo_e12_medium()  ))),
        
        L2EFChain_e('e15_medium', 3, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e15_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium(), TrigEFElectronHypo_e15_medium() ))),
        
        L2EFChain_e('e20_loose1', 401, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose1(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_loose1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose1(), TrigEFElectronHypo_e20_loose1() ))),  
        
        L2EFChain_e('e20_looseTrk', 402, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_looseTrk(), TrigL2ElectronHypo_1=L2ElectronHypo_e20_looseTrk()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e20_looseTrk(), TrigEFElectronHypo_e20_looseTrk() ))),
        
        L2EFChain_e('e22_medium', 528, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium(), TrigEFElectronHypo_e22_medium() ))),
        
        L2EFChain_e_NoIDTrkCut('e20_medium_IDTrkNoCut', 530,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium("L2CaloHypo_e20_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_medium_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium_NoTrk(), TrigEFElectronHypo_e20_medium_NoTrk()  ))),
        
        L2EFChain_e_NoIDTrkCut('e20_medium_IDTrkNoCut_R1', 2350,
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium("L2CaloHypo_e20_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_medium_NoTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium_NoTrk(), TrigEFElectronHypo_e20_medium_NoTrk()  ))),
        

        L2EFChain_e_SiTrack('e20_medium_SiTrk', 104, 
                            L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium("L2CaloHypo_e20_medium_SiTrk"), L2ElectronFex_SiTrack(), L2ElectronHypo_e20_medium("L2ElectronHypo_e20_medium_SiTrk")),
                                                L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium("EFTrackHypo_e20_medium_SiTrk"), TrigEFElectronHypo_e20_medium("TrigEFElectronHypo_e20_medium_SiTrk")  ))),
        
        L2EFChain_e_TRT('e22_medium_TRT', 174, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22_medium_TRT"), L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e22_medium("L2ElectronHypo_e22_medium_TRT")),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium("EFTrackHypo_e22_medium_TRT"), TrigEFElectronHypo_e22_medium("TrigEFElectronHypo_e22_medium_TRT")  ))),
        
        L2EFChain_e('e22_medium1', 49, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium1(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium1()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium1(), TrigEFElectronHypo_e22_medium1() ))),
        
        L2EFChain_e('e22_medium2', 69, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium2(), TrigL2ElectronHypo_1=L2ElectronHypo_e22_medium2()), 
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium2(), TrigEFElectronHypo_e22_medium2() ))),
        ]
    

# --- the new L2Star code present in release 17 and above, configure the chain only there
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
    print 'igb L2Star for electrons code exists'
    configL2Star=True
except:
    configL2Star=False
    print 'igb L2Star for electrons does not exist'

if configL2Star :
    
    Electrons += [
        L2EFChain_e_L2Star_NoIDTrkCut('e20_L2Star_IDTrkNoCut', 1320,
                                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium("L2CaloHypo_e20_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_medium_NoTrk()),
                                                          L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium_NoTrk(), TrigEFElectronHypo_e20_medium_NoTrk()  ))),
        
        L2EFChain_e_L2Star_fwdBackTrk('e5_L2Star_NoCut_FwdBackTrk', 1332, 
                                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut("L2CaloHypo_e5_L2Star_NoCut_FwdBackTrk"), L2ElectronFex_FwdBackTracking_NoCut(), L2ElectronHypo_e5_NoCut_FwdBackTrk()),
                                                          L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut("EFTrackHypo_e5_L2Star_NoCut_FwdBackTrk"), TrigEFElectronHypo_e5_NoCut("TrigEFElectronHypo_e5_L2Star_NoCut_FwdBackTrk")  ))),
        
        L2EFChain_e_L2Star('e5_medium_L2StarA', l2=Chain(name='L2_e5_medium_L2StarA', counter=3370, seed='L1_EM3'),
                           strategy='A', l1threshold='EM3'), 
        L2EFChain_e_L2Star('e5_medium_L2StarB', l2=Chain(name='L2_e5_medium_L2StarB', counter=3371, seed='L1_EM3'),
                           strategy='B', l1threshold='EM3'), 
        L2EFChain_e_L2Star('e5_medium_L2StarC', l2=Chain(name='L2_e5_medium_L2StarC', counter=3372, seed='L1_EM3'),
                           strategy='C', l1threshold='EM3'), 
        L2EFChain_e_L2Star('e12_medium_L2StarA', l2=Chain(name='L2_e12_medium_L2StarA', counter=3373, seed='L1_EM7'),
                           strategy='A', l1threshold='EM7'), 
        L2EFChain_e_L2Star('e12_medium_L2StarB', l2=Chain(name='L2_e12_medium_L2StarB', counter=3374, seed='L1_EM7'),
                           strategy='B', l1threshold='EM7'), 
        L2EFChain_e_L2Star('e12_medium_L2StarC', l2=Chain(name='L2_e12_medium_L2StarC', counter=3375, seed='L1_EM7'),
                           strategy='C', l1threshold='EM7'), 
        L2EFChain_e_L2Star('e22vh_medium_L2StarA', l2=Chain(name='L2_e22vh_medium_L2StarA', counter=3376, seed='L1_EM16VH'),
                           strategy='A', l1threshold='EM16VH'), 
        L2EFChain_e_L2Star('e22vh_medium_L2StarB', l2=Chain(name='L2_e22vh_medium_L2StarB', counter=3377, seed='L1_EM16VH'),
                           strategy='B', l1threshold='EM16VH'), 
        L2EFChain_e_L2Star('e22vh_medium_L2StarC', l2=Chain(name='L2_e22vh_medium_L2StarC', counter=3378, seed='L1_EM16VH'),
                           strategy='C', l1threshold='EM16VH'), 

        L2EFChain_e_L2Star_NoIDTrkCut('e22vh_medium_L2Star_IDTrkNoCut', 1345,
                                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22_medium("L2CaloHypo_e22vh_medium_NoTrk"), L2ElectronFex_all_NoCut(), L2ElectronHypo_e22_medium_NoTrk()),
                                                          L2EFChain_e.EFConfig('',EFTrackHypo_e22_medium_NoTrk(), TrigEFElectronHypo_e22_medium_NoTrk()  ))),
        


        ]


#  LocalWords:  IdScan
