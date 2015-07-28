# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
import Monitoring_pp_v4_rules

tag = 'Cosmic_pp_v4'


rules = {

    #L1 items
    'L1_MU4_EMPTY' : {
    0 : TriggerRule(PS=40, comment='cosmic'),
    },
    'L1_TRT' : {
    0: TriggerRule(PS=1, comment='primary for cosmic with TRT Fast Or')
    },
    #HLT
    'EF_rd0_empty_NoAlg' : {
    #0: TriggerRule(inputRate=10, comment='streamer for L1_RD0_EMPTY')
    0: TriggerRule(inputRate=100, ESValue=20, comment='streamer for L1_RD0_EMPTY')
    },
    'EF_rd1_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY')
    },
    'EF_L1TRT_NoAlg' : {
    0: TriggerRule(PS=1, comment='streamer for L1_TRT')
    },
    'EF_mu4_L1MU11_cosmic' : {
    0: TriggerRule(PS=1, comment='cosmic chain for muons')
    },
    'EF_mu4_L1MU11_MSonly_cosmic' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for muons')
    },
    'EF_mu4_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for muons')
    },
    'EF_mu11_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for muons')
    },
    'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_CosmicsAllTeEFID' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for InDet studies')
    },
    'EF_g5_NoCut_cosmic' : {
    0: TriggerRule(PS=1, comment='cosmic chain for egamma')
    },
    'EF_tauNoCut_cosmic' : {
    0: TriggerRule(PS=1, ESRate=3, comment='cosmic chain for taus')
    },
    'L2_mu4_l2cal_empty' : {
    0: TriggerRule(PS=1, comment='L2 Calibration for Muons')
    },
    'L2_em3_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    'L2_tau8_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    'L2_j10_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    'L2_fj30_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    'EF_L1J10_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    'EF_L1FJ30_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr')
    },
    }    


Monitoring_pp_v4_rules.physics_rules.update(rules)
rules = Monitoring_pp_v4_rules.physics_rules


if __name__ == "__main__":
    print "tag:   %s" % tag
    for item in sorted(rules.keys()):
        print "\n", item
        for point in rules[item]:
            print "    %20s %s" % (point, rules[item][point])
            
