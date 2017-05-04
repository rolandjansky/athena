# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

import Monitoring_pp_v5_rules

tag = '../python/Cosmic_pp_v5'

EBrun=False

rules = {
                                                 'L1_MBTS_1' : { 10000 : TriggerRule(PS=1.0, comment = 'L1', rerun = 0.0) }, 
                                                 'L1_MBTS_2' : { 10000 : TriggerRule(PS=1.0, comment = 'L1', rerun = 0.0) }, 
                                                'L1_MBTSA12' : { 10000 : TriggerRule(PS=1.0, comment = 'L1', rerun = 0.0) }, 
                                                'L1_MBTSC12' : { 10000 : TriggerRule(PS=1.0, comment = 'L1', rerun = 0.0) }, 
                                            'HLT_2mu4_bDimu' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                 'HLT_2mu4_bDimu_novtx_noos' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_2mu6_bDimu' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                 'HLT_2mu6_bDimu_novtx_noos' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                   'HLT_beamspot_allTE_L2StarB_L1RDO_FILLED' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                          'HLT_cscmon_L1EM3' : { 10000 : TriggerRule(PS=-1.0, comment = 'CSC', rerun = 0.0) }, 
                                          'HLT_cscmon_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'CSC', rerun = 0.0) }, 
                                         'HLT_cscmon_L1MU10' : { 10000 : TriggerRule(PS=-1.0, comment = 'CSC', rerun = 0.0) }, 
                                  'HLT_e0_L2Star_perf_L1EM3' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                         'HLT_e0_perf_L1EM3' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_e0_perf_L1EM3HLIL' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                              'HLT_e3_etcut' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                  'HLT_e3_etcut_L1EM3_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_e3_lhloose' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                              'HLT_e3_loose' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                  'HLT_eb_empty_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'EnhancedBias', rerun = 0.0) }, 
                                         'HLT_g0_perf_L1EM3' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_g0_perf_L1EM3HLIL' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                              'HLT_g3_etcut' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                  'HLT_g3_etcut_L1EM3_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                             'HLT_ht0_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                        'HLT_ht0_perf_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                          'HLT_id_cosmicid' : { 10000 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                    'HLT_id_cosmicid_L1MU11' : { 10000 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0, ESValue = 1.0) }, 
                                     'HLT_id_cosmicid_L1MU4' : { 10000 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0, ESValue = 1.0) }, 
                                     'HLT_id_cosmicid_trtxk' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicID', rerun = 0.0) }, 
                             'HLT_id_cosmicid_trtxk_central' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicID', rerun = 0.0) }, 
                        'HLT_noalg_cosmicmuons_L1MU11_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0, ESValue = 1.0) }, 
                         'HLT_noalg_cosmicmuons_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=25.0, comment = 'express', rerun = 0.0, ESValue = 50.0) }, 
                                              'HLT_j0_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                           'HLT_j0_PS_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                  'HLT_j0_a10_lcw_sub_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                 'HLT_j0_a10_nojcalib_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                           'HLT_j0_a10_nojcalib_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                 'HLT_j0_a10r_lcw_sub_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                           'HLT_j0_a10r_lcw_sub_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                          'HLT_j0_jes_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                      'HLT_j0_lcw_jes_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_j0_nojcalib_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                         'HLT_j0_perf_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                        'HLT_j0_perf_L1MU10' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                   'HLT_j0_perf_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                   'HLT_j0_perf_bperf_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                  'HLT_j0_perf_bperf_L1MU10' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                             'HLT_j0_perf_bperf_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_j0_perf_ds1_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'DataScouting_05_Jets', rerun = 0.0) }, 
                                          'HLT_j0_sub_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                             'HLT_j15_L1J12' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                           'HLT_l1calocalib' : { 10000 : TriggerRule(PS=-1.0, comment = 'L1CaloCalib', rerun = 0.0) }, 
                                   'HLT_l1calocalib_L1BGRP7' : { 10000 : TriggerRule(PS=-1.0, comment = 'L1CaloCalib', rerun = 0.0) }, 
                                     'HLT_mb_mbts_L1MBTS_2' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                       'HLT_mb_perf_L1LUCID' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                 'HLT_mb_perf_L1LUCID_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                          'HLT_mb_perf_L1LUCID_UNPAIRED_ISO' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                      'HLT_mb_perf_L1MBTS_2' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                              'HLT_mb_sptrk' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                  'HLT_mb_sptrk_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                           'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_mb_sptrk_costr' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_mb_sptrk_costr_L1RD0_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                     'HLT_mb_sptrk_noisesup' : { 10000 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                         'HLT_mu0_muoncalib' : { 10000 : TriggerRule(PS=-1.0, comment = 'Muon_Calibration', rerun = 0.0) }, 
                             'HLT_mu4_cosmicEF_L1MU11_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                              'HLT_mu4_cosmicEF_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                               'HLT_mu4_cosmic_L1MU11_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                                'HLT_mu4_cosmic_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=25.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                      'HLT_mu4_msonly_cosmicEF_L1MU11_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                       'HLT_mu4_msonly_cosmicEF_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                        'HLT_mu4_msonly_cosmic_L1MU11_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'CosmicMuons', rerun = 0.0) }, 
                       'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                         'HLT_tau1_cosmic_track_L1MU4_EMPTY' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                    'HLT_tau8_cosmic_ptonly' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_tau8_cosmic_track' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                                  'HLT_te20' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                           'HLT_te20_tc_lcw' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                             'HLT_xe0_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_xe0_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                        'HLT_xe0_L2FS_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                       'HLT_xe0_L2FS_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                     'HLT_xe0_l2fsperf_wEFMuFEB_wEFMu_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                    'HLT_xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                       'HLT_xe0_tc_em_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                      'HLT_xe0_tc_em_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                      'HLT_xe0_tc_lcw_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_xe0_tc_lcw_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                      'HLT_xe0_tc_mht_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                     'HLT_xe0_tc_mht_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                    'HLT_xe0_tc_pueta_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                   'HLT_xe0_tc_pueta_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                    'HLT_xe0_tc_pufit_L1All' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                   'HLT_xe0_tc_pufit_L1XE35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                                  'HLT_xe35' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                             'HLT_xe35_L2FS' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                          'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                              'HLT_xe35_mht' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_xe35_pueta' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_xe35_pufit' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                            'HLT_xe35_tc_em' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                           'HLT_xe35_tc_lcw' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                                  'HLT_xs30' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
                                           'HLT_xs30_tc_lcw' : { 10000 : TriggerRule(PS=-1.0, comment = 'Main', rerun = 0.0) }, 
}


Monitoring_pp_v5_rules.physics_rules.update(rules)
rules = Monitoring_pp_v5_rules.physics_rules

if __name__ == "__main__":
    print "tag:   %s" % tag
    for item in sorted(rules.keys()):
        print "\n", item
        for point in rules[item]:
            print "    %20s %s" % (point, rules[item][point])
            


