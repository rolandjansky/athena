# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def TrigTauMonitoringTool():
        full_tau = [
                # put some chains in addition to the ones already in the TrigTauMonitCategory.py lists                     
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
              # Run 2 data reprocessing
              'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
              'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
              'tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
              'tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50',
              'tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50',
              'tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',
              #  Run 3
              'tau25_idperf_tracktwoMVA_L1TAU12IM',
              'tau25_idperf_tracktwoMVABDT_L1TAU12IM',
              'tau25_perf_tracktwoMVA_L1TAU12IM',
              'tau25_perf_tracktwoMVABDT_L1TAU12IM',
              'tau25_looseRNN_tracktwoMVA_L1TAU12IM',
              'tau25_looseRNN_tracktwoMVABDT_L1TAU12IM',
              'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
              'tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
              'tau25_tightRNN_tracktwoMVA_L1TAU12IM',
              'tau25_tightRNN_tracktwoMVABDT_L1TAU12IM',
              'tau35_idperf_tracktwoMVA_L1TAU20IM',
              'tau35_idperf_tracktwoMVABDT_L1TAU20IM',
              'tau35_perf_tracktwoMVA_L1TAU20IM',
              'tau35_perf_tracktwoMVABDT_L1TAU20IM',
              'tau35_looseRNN_tracktwoMVA_L1TAU20IM',
              'tau35_looseRNN_tracktwoMVABDT_L1TAU20IM',
              'tau35_mediumRNN_tracktwoMVA_L1TAU20IM',
              'tau35_mediumRNN_tracktwoMVABDT_L1TAU20IM',
              'tau35_tightRNN_tracktwoMVA_L1TAU20IM',
              'tau35_tightRNN_tracktwoMVABDT_L1TAU20IM',     
              'tau160_idperf_tracktwoMVA_L1TAU100',
              'tau160_idperf_tracktwoMVABDT_L1TAU100',
              'tau160_perf_tracktwoMVA_L1TAU100',
              'tau160_perf_tracktwoMVABDT_L1TAU100',
              'tau160_mediumRNN_tracktwoMVA_L1TAU100',
              'tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
              'tau180_mediumRNN_tracktwoLLP_L1TAU100',
              'tau180_tightRNN_tracktwoLLP_L1TAU100',
              'tau200_mediumRNN_tracktwoLLP_L1TAU100',
              'tau200_tightRNN_tracktwoLLP_L1TAU100',
              # Phase-I
              'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12',
              'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12M',
              'tau35_mediumRNN_tracktwoMVABDT_L1eTAU20',
              'tau160_mediumRNN_tracktwoMVABDT_L1eTAU100',
        ]

        RNN_chains = [
             # Run 2 data reprocessing
             'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
             'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
             'tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
             'tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50',
             'tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50',
             'tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',
             #  Run 3
             'tau25_looseRNN_tracktwoMVA_L1TAU12IM',
             'tau25_looseRNN_tracktwoMVABDT_L1TAU12IM',
             'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
             'tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
             'tau25_tightRNN_tracktwoMVA_L1TAU12IM',
             'tau25_tightRNN_tracktwoMVABDT_L1TAU12IM',
             'tau35_looseRNN_tracktwoMVA_L1TAU20IM',
             'tau35_looseRNN_tracktwoMVABDT_L1TAU20IM',
             'tau35_mediumRNN_tracktwoMVA_L1TAU20IM',
             'tau35_mediumRNN_tracktwoMVABDT_L1TAU20IM',
             'tau35_tightRNN_tracktwoMVA_L1TAU20IM',
             'tau35_tightRNN_tracktwoMVABDT_L1TAU20IM',
             'tau160_mediumRNN_tracktwoMVA_L1TAU100',
             'tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
             'tau180_mediumRNN_tracktwoLLP_L1TAU100',
             'tau180_tightRNN_tracktwoLLP_L1TAU100',
             'tau200_mediumRNN_tracktwoLLP_L1TAU100',
             'tau200_tightRNN_tracktwoLLP_L1TAU100',
             # Phase-I
             'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12',
             'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12M',
             'tau35_mediumRNN_tracktwoMVABDT_L1eTAU20',
             'tau160_mediumRNN_tracktwoMVABDT_L1eTAU100',
        ]

        BDTRNN_chains = [
             # Run 2 data reprocessing
             'tau25_idperf_tracktwo',
             'tau25_idperf_tracktwoEF',
             'tau25_idperf_tracktwoMVA',
             'tau25_perf_tracktwo',
             'tau25_perf_tracktwoEF',
             'tau25_perf_tracktwoMVA', 
             # Run 3
             'tau0_idperf_tracktwoMVA_L1eTAU12',
             'tau0_idperf_tracktwoMVA_L1eTAU5',
             'tau0_ptonly_L1TAU8',
             'tau0_ptonly_L1TAU60',
             'tau25_ptonly_L1TAU12IM',
             'tau25_idperf_track_L1TAU12IM',
             'tau25_idperf_tracktwo_L1TAU12IM',
             'tau25_idperf_tracktwoEF_L1TAU12IM',
             'tau25_idperf_tracktwoMVA_L1TAU12IM',
             'tau25_idperf_tracktwoMVABDT_L1TAU12IM',
             'tau25_perf_track_L1TAU12IM',
             'tau25_perf_tracktwo_L1TAU12IM',
             'tau25_perf_tracktwoEF_L1TAU12IM',
             'tau25_perf_tracktwoMVA_L1TAU12IM',
             'tau25_perf_tracktwoMVABDT_L1TAU12IM',
             'tau35_ptonly_L1TAU20IM',
             'tau35_idperf_track_L1TAU20IM',
             'tau35_idperf_tracktwo_L1TAU20IM',
             'tau35_idperf_tracktwoEF_L1TAU20IM',
             'tau35_idperf_tracktwoMVA_L1TAU20IM',
             'tau35_idperf_tracktwoMVABDT_L1TAU20IM',
             'tau35_perf_track_L1TAU20IM',
             'tau35_perf_tracktwo_L1TAU20IM',
             'tau35_perf_tracktwoEF_L1TAU20IM',
             'tau35_perf_tracktwoMVA_L1TAU20IM',
             'tau35_perf_tracktwoMVABDT_L1TAU20IM',            
             'tau160_ptonly_L1TAU100',
             'tau160_idperf_track_L1TAU100',
             'tau160_idperf_tracktwo_L1TAU100',
             'tau160_idperf_tracktwoEF_L1TAU100',
             'tau160_idperf_tracktwoMVA_L1TAU100',
             'tau160_idperf_tracktwoMVABDT_L1TAU100',
             'tau160_perf_track_L1TAU100',
             'tau160_perf_tracktwo_L1TAU100',
             'tau160_perf_tracktwoEF_L1TAU100',
             'tau160_perf_tracktwoMVA_L1TAU100',
             'tau160_perf_tracktwoMVABDT_L1TAU100',
             'tau180_mediumRNN_tracktwoLLP_L1TAU100',
             'tau180_tightRNN_tracktwoLLP_L1TAU100',
             'tau200_mediumRNN_tracktwoLLP_L1TAU100',
             'tau200_tightRNN_tracktwoLLP_L1TAU100',

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
                'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
        ]
        ztt_BDT_tau = [
                'tau25_medium1_tracktwoEF_L1TAU12IM',
        ]
        # removed ztt_tau chains
        # 'tau25_idperf_tracktwo',
        # 'tau25_perf_tracktwo',

        highpt_tau = [
          'tau160_idperf_tracktwo_L1TAU100',
          'tau160_mediumRNN_tracktwoMVA_L1TAU100',
          'tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
        ]

        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
        LuminosityCondAlgOnlineDefault (suffix = 'Online')

        from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
        from TrigTauMonitoring.TrigTauMonitoringConf import HLTTauMonTool
        from AthenaCommon.GlobalFlags import globalflags

        HLTTauMon = HLTTauMonTool(name = 'HLTTauMon',
                                histoPathBase           = "/Trigger/HLT",
                                monitoring_tau          = hltmonList.monitoring_tau,
                                primary_tau             = full_tau, #[]
                                prescaled_tau           = [], #tau_track_test, #[],
                                LowestSingleTauRNN      = "tau25_mediumRNN_tracktwoMVA", #hltmonList.monitoring_singleTau, #"tau25_medium1_tracktwo",
                                LowestSingleTauBDT      = "tau25_medium1_tracktwoEF",
                                Highpt_tau              = highpt_tau,
                                Ztt_RNN_tau             = ztt_RNN_tau,
                                Ztt_BDT_tau             = ztt_BDT_tau,
                                EffOffTauPtCut          = 25000.,  #MeV
                                TurnOnCurves            = True,
                                TurnOnCurvesDenom       = "Reco", # combined string with combination of "Truth", "Reco" and "Presel". For Truth doTruth=True!
                                doTruth                 = True,
                                doRealZtautauEff        = True,
                                dodijetFakeTausEff      = True,
                                doBootstrap             = False,
                                doTrackCurves           = True,
#                               doTestTracking          = False,
#                               topo_chains                     = tau_topo_chains,
                                doL1JetPlots            = False,
                                doEFTProfiles           = True,
                                domuCut40               = False,
                                doL1TopoLeptonsMonitoringWarnings = False,
                                trigMVA_chains          = MVA_chains,
                                trigRNN_chains          = RNN_chains,
                                trigBDTRNN_chains       = BDTRNN_chains,
                                topo_support_chains     = tau_topo_support_chains,
                                doTopoValidation        = False,
                                L1TriggerCondition      = "Physics", #allowResurrectedDecision, alsoDeactivateTEs, Physics [default]
                                HLTTriggerCondition     = "Physics",
                                nTrkMax                 = -1,
                                nTrkMin                 = -1,
                                PtMax                   = -1., #MeV
                                PtMin                   = -1., #MeV
                                AbsEtaMax               = -1.,
                                AbsEtaMin               = -1.,
                                AbsPhiMax               = -1.,
                                AbsPhiMin               = -1.,
                                BDTMedium               = True,
                                isData                  = (globalflags.DataSource == 'data'))

        list = [ HLTTauMon ]
        return list
