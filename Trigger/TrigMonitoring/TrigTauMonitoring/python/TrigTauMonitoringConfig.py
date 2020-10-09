# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def TrigTauMonitoringTool():
        full_tau = [
                # put some chains in addition to the ones already in the TrigTauMonitCategory.py lists                     
        ]

        # Setup emulation
        emul_l1_tau = [
                #'L1_EM15HI_2TAU12IM',
                'L1_EM15HI_2TAU12IM_J25_3J12',
                'L1_EM15HI_2TAU12IM_XE35',
                #'L1_MU10_TAU12IM',
                'L1_MU10_TAU12IM_J25_2J12',
                'L1_MU10_TAU12IM_XE35',
                'L1_TAU20IM_2J20_XE45',
                #'L1_TAU20IM_2TAU12IM',
                'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
                'L1_TAU20IM_2TAU12IM_XE35',
                'L1_TAU60',
                'L1_DR-TAU20ITAU12I',
                'L1_DR-TAU20ITAU12I-J25',
                'L1_DR-EM15TAU12I-J25',
                'L1_DR-MU10TAU12I_TAU12I-J25'
        ]

        emul_hlt_tau = [
#               'HLT_tau25_perf_ptonly',
        ]

#      tau_topo_chains = [
#               'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
#              'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
#               'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I',
#               'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
#              'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30',
#              'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I',
#              'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
#      ]
        





        MVA_chains = [
                        'tau25_idperf_tracktwoMVA_L1TAU12IM',
                        'tau25_perf_tracktwoMVA_L1TAU12IM',
                        'tau25_verylooseRNN_tracktwoMVA_L1TAU12IM',
                        'tau25_looseRNN_tracktwoMVA_L1TAU12IM',
                        'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
                        'tau25_tightRNN_tracktwoMVA_L1TAU12IM'
        ]

        RNN_chains = [
                        'tau25_looseRNN_tracktwo_L1TAU12IM',
                        'tau25_mediumRNN_tracktwo_L1TAU12IM',
                        'tau25_tightRNN_tracktwo_L1TAU12IM',
                        'tau25_looseRNN_tracktwoMVA_L1TAU12IM',
                        'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
                        'tau25_tightRNN_tracktwoMVA_L1TAU12IM',
                        'tau35_mediumRNN_tracktwoMVA_L1TAU12IM',
                        'tau160_mediumRNN_tracktwoMVA_L1TAU100'
        ]

        BDTRNN_chains = [
                'tau25_idperf_tracktwoMVA',
                'tau25_perf_tracktwoMVA',
                'tau0_idperf_tracktwoMVA_L1eTAU12',
                'tau0_idperf_tracktwoMVA_L1eTAU5',
                'tau25_verylooseRNN_tracktwo_L1TAU12IM',
                'tau25_looseRNN_tracktwo_L1TAU12IM',
                'tau25_mediumRNN_tracktwo_L1TAU12IM',
                'tau25_tightRNN_tracktwo_L1TAU12IM'

        ]

        tau_topo_support_chains = [

        ]

#       tau_track_test = [
#               'tau25_perf_track_emulate',
#               'tau25_perf_tracktwo_emulate',
#               'tau25_perf_track_ptmin_emulate',
#               'tau25_perf_tracktwo_ptmin_emulate',
#               'tau25_medium1_track_ptmin_emulate',
#               'tau25_medium1_tracktwo_ptmin_emulate',
#               'tau25_perf_tracktwo_ptmin_no0reject_emulate',
#               'tau25_medium1_tracktwo_ptmin_no0reject_emulate'
#       ]

        ztt_RNN_tau = [
                'tau25_mediumRNN_tracktwoMVA',
        ]
        ztt_BDT_tau = [
                'tau25_medium1_tracktwoEF',
        ]
        # removed ztt_tau chains
        # 'tau25_idperf_tracktwo',
        # 'tau25_perf_tracktwo',

        highpt_tau = [
                'tau80_medium1_tracktwo_L1TAU60',
                'tau160_idperf_tracktwo',
                'tau160_medium1_tracktwo',
                'tau160_medium1HighptL_tracktwo',
                'tau160_medium1HighptM_tracktwo',
                'tau160_medium1HighptH_tracktwo'
        ]

        # get the Level1 Emulation tool from the emulation python config
        from TrigTauEmulation.TrigTauEmulationConfig import get_level1_emulator
        Level1Emulator = get_level1_emulator('Level1Emulator', emul_l1_tau)
        
        # Add the Level1 emulation tool to the tool service
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += Level1Emulator

        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
        LuminosityCondAlgOnlineDefault (suffix = 'Online')



        # get the HLT emulation tool from the emulation python config
#       from TrigTauEmulation.TrigTauEmulationConfig import get_hlt_emulator
#       HltEmulator = get_hlt_emulator('HltEmulator', emul_hlt_tau, Level1Emulator)
        # Add the HLT emulation tool to the tool service
#       ToolSvc += HltEmulator


        from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
        from TrigTauMonitoring.TrigTauMonitoringConf import HLTTauMonTool
        from AthenaCommon.GlobalFlags import globalflags

        HLTTauMon = HLTTauMonTool(name = 'HLTTauMon',
                                histoPathBase           = "/Trigger/HLT",
                                monitoring_tau          = hltmonList.monitoring_tau,
                                primary_tau             = full_tau, #[]
                                prescaled_tau           = [], #tau_track_test, #[],
                                LowestSingleTauRNN              = "tau25_mediumRNN_tracktwoMVA", #hltmonList.monitoring_singleTau, #"tau25_medium1_tracktwo",
                                LowestSingleTauBDT      = "tau25_medium1_tracktwoEF",
                                Highpt_tau              = highpt_tau,
                                Ztt_RNN_tau                     = ztt_RNN_tau,
                                Ztt_BDT_tau                     = ztt_BDT_tau,
                                EffOffTauPtCut          = 25000.,  #MeV
                                TurnOnCurves            = True,
                                TurnOnCurvesDenom       = "Reco", # combined string with combination of "Truth", "Reco" and "Presel". For Truth doTruth=True!
                                doTruth                 = True,
                                doRealZtautauEff        = True,
                                dodijetFakeTausEff      = True,
                                doBootstrap             = False,
                                doEmulation             = True,
                                emulation_l1_tau        = emul_l1_tau,
                                emulation_hlt_tau       = emul_hlt_tau,
                                L1EmulationTool         = Level1Emulator,
#                               HltEmulationTool        = HltEmulator,
                                doTrackCurves           = True,
#                               doTestTracking          = False,
#                               topo_chains                     = tau_topo_chains,
                                doL1JetPlots            = False,
                                doEFTProfiles           = True,
                                domuCut40               = False,
                                doEfficiencyRatioPlots = True, #False
                                doL1TopoLeptonsMonitoringWarnings = False,
                                trigMVA_chains  = MVA_chains,
                                trigRNN_chains  = RNN_chains,
                                trigBDTRNN_chains       = BDTRNN_chains,
                                topo_support_chains     = tau_topo_support_chains,
                                doTopoValidation        = False,
                                L1TriggerCondition      = "Physics", #allowResurrectedDecision, alsoDeactivateTEs, Physics [default]
                                HLTTriggerCondition     = "Physics",
                                nTrkMax                         = -1,
                                nTrkMin                         = -1,
                                PtMax                           = -1., #MeV
                                PtMin                           = -1., #MeV
                                AbsEtaMax                       = -1.,
                                AbsEtaMin                       = -1.,
                                AbsPhiMax                       = -1.,
                                AbsPhiMin                       = -1.,
                                BDTMedium                       = True,
                                isData                          = (globalflags.DataSource == 'data'))

        print (ToolSvc)
        list = [ HLTTauMon ]
        return list
