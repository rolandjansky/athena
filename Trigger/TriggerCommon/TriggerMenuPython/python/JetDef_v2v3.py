# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SliceDef file for jet chains/signatures

#from TriggerMenuPython.HltConfig import *

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from ChainTemplate import *

hypos = FreeStore()
fexes = FreeStore()

from AthenaCommon.SystemOfUnits import GeV
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet

# definde seed for HI
minbias_hi_seed = 'L2_mbZdc_a_c_NoAlg'

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_noise
fexes.l2fex = T2CaloJet_Jet_noise()

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_JESCalib_noiseCut
fexes.l2fex_JESCalib = T2CaloJet_Jet_JESCalib_noiseCut()

from TrigCaloRec.TrigCaloRecConfig import TrigLArNoisyROAlgConfig
fexes.lar_noisy = TrigLArNoisyROAlgConfig()

# seems unused
#from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJetGridFromCells
#fexes.l2fex_fom_cells = T2CaloJetGridFromCells()

from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
from TrigJetHypo.TrigJetHypoConfig import L2JetHypo_doCleaning_HECPS
from TrigJetHypo.TrigJetHypoConfig import L2JetHypo_doCleaning_conservative
from TrigJetHypo.TrigJetHypoConfig import L2JetHypoNoise

# instead of this above here shoudl go to jet hypo package
# from TrigJetHypo.TrigJetHypoConfig import l2_hypos
# hypos.update(l2_hypos)
# for the time beeing we instantiate them here
#

# this certainly can & shoudl be coded in simpler python but I am lazy now
#l2 thresholds
thresholds=[0,7,10,15,20,25,30,35,40,45,50,60,65,70,75,90,95,115,140,200]
hypos.update(dict([ ('l2_j%d'%t,  L2JetHypo('L2JetHypo_j%d'%t, t*GeV)) for t in thresholds]))

hypos.update(dict([ ('l2cleanph_j%d'%t,  L2JetHypo_doCleaning_HECPS('L2JetHypo_doCleaning_HECPSj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('l2cleancons_j%d'%t,  L2JetHypo_doCleaning_conservative('L2JetHypo_doCleaning_conservativej%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('l2unclean_j%d'%t,  L2JetHypoNoise('L2JetHypoNoise_j%d'%t, t*GeV)) for t in thresholds]))

from TrigJetHypo.TrigJetHypoConfig import L2JetHypo_noJetCut
hypos.update(dict([ ('l2nocut_j%d'%t,  L2JetHypo_noJetCut('L2JetHypo_noJetCutj%d'%t, t*GeV)) for t in thresholds]))

#ef thresholds
thresholds=[5,10,15,20,25,30,35,40,45,50,55,65,70,75,80,90,95,100,115,120,135,145,150,175,180,200,240,250,320,425]
from TrigJetHypo.TrigJetHypoConfig import EFCentJetHypo, EFFwdJetHypo, EFJetHypo_doCleaning, EFJetHypo_doCleaningInverse
from TrigJetHypo.TrigJetHypoConfig import EFJetHypoNoiseConfig
hypos.update(dict([ ('ef_j%d'%t,  EFCentJetHypo('EFJetHypo_j%d'%t, t*GeV)) for t in thresholds]))
# antikt
hypos.update(dict([ ('ef_j%da4'%t,  EFCentJetHypo('EFJetHypo_j%da4'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%da6'%t,  EFCentJetHypo('EFJetHypo_j%da6'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a4_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a4_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a6_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a6_EFFS'%t, t*GeV)) for t in thresholds]))
# topoclusters
hypos.update(dict([ ('ef_j%d_a4topo_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a4topo_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a6topo_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a6topo_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a10topo_EFFS'%t, EFCentJetHypo('EFJetHypo_j%d_a10topo_EFFS'%t, t*GeV)) for t in thresholds]))

#topotowers
hypos.update(dict([ ('ef_j%d_a4topotower_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a4topotower_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a6topotower_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a6topotower_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a10topotower_EFFS'%t, EFCentJetHypo('EFJetHypo_j%d_a10topotower_EFFS'%t, t*GeV)) for t in thresholds]))

#ef cleaning
hypos.update(dict([ ('ef_j%d_a4topo_EFFS_clean'%t,  EFJetHypo_doCleaning('EFJetHypo_j%d_a4topo_EFFS_clean'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a4topo_EFFS_cleaninv'%t,  EFJetHypo_doCleaningInverse('EFJetHypo_j%d_a4topo_EFFS_cleaninv'%t, t*GeV)) for t in thresholds]))

# Central-jets for empty, firstempty
hypos.update(dict([ ('ef_j30_eta13_a4topo_EFFS',  EFCentJetHypo('EFJetHypo_j30_eta13_a4topo_EFFS', ef_thr=30*GeV, etaMax=1.3)) ]))
hypos.update(dict([ ('ef_j50_eta13_a4topo_EFFS',  EFCentJetHypo('EFJetHypo_j50_eta13_a4topo_EFFS', ef_thr=50*GeV, etaMax=1.3)) ]))
hypos.update(dict([ ('ef_j50_eta25_a4topo_EFFS',  EFCentJetHypo('EFJetHypo_j50_eta25_a4topo_EFFS', ef_thr=50*GeV, etaMax=2.5)) ]))

#HI jets
hypos.update(dict([ ('ef_j%d_a4hi_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a4hi_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a2hi_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a2hi_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a4tc_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a4tc_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_j%d_a2tc_EFFS'%t,  EFCentJetHypo('EFJetHypo_j%d_a2tc_EFFS'%t, t*GeV)) for t in thresholds]))

from TrigJetHypo.TrigJetHypoConfig import EFJetHypo_noJetCut
#??? should preserve the _ as in L2 between noJetCut ... also jetNoCut seems more consistent with chains naming
hypos.update(dict([ ('efnocut_j%d'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%da4'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%da4'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%da6'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%da6'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%d_a4_EFFS'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d_a4_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%d_a6_EFFS'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d_a6_EFFS'%t, t*GeV)) for t in thresholds]))
# topoclusters
hypos.update(dict([ ('efnocut_j%d_a4topo_EFFS'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d_a4topo_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%d_a6topo_EFFS'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d_a6topo_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_j%d_a10topo_EFFS'%t, EFJetHypo_noJetCut('EFJetHypo_noJetCutj%d_a10topo_EFFS'%t, t*GeV)) for t in thresholds]))


# Unclean
EFJetHypoNoiseConfig
hypos.update(dict([ ('efunclean_j%d'%t,  EFJetHypoNoiseConfig('EFJetHypoNoise_j%d'%t, t*GeV)) for t in thresholds]))

#############################################################################
# forward jets 
#l2 thresholds
thresholds=[25,35,45,50,65,70,95,130]
hypos.update(dict([ ('l2_fj%d'%t,  L2JetHypo('L2JetHypo_fj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('l2cleanph_fj%d'%t,  L2JetHypo_doCleaning_HECPS('L2JetHypo_doCleaning_HECPSfj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('l2nocut_fj%d'%t,  L2JetHypo_noJetCut('L2JetHypo_noJetCut_fj%d'%t, t*GeV)) for t in thresholds]))

#ef thresholds
thresholds=[10,15,20,30,40,50,55,70,75,95,100,135,175,250]
hypos.update(dict([ ('ef_fj%d'%t,  EFFwdJetHypo('EFJetHypo_fj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('efnocut_fj%d'%t,  EFJetHypo_noJetCut('EFJetHypo_noJetCutfj%d'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_fj%da4'%t,  EFFwdJetHypo('EFJetHypo_fj%da4'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_fj%da6'%t,  EFFwdJetHypo('EFJetHypo_fj%da6'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_fj%d_a4_EFFS'%t,  EFFwdJetHypo('EFJetHypo_fj%d_a4_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_fj%d_a6_EFFS'%t,  EFFwdJetHypo('EFJetHypo_fj%d_a6_EFFS'%t, t*GeV)) for t in thresholds]))
# topoclusters
hypos.update(dict([ ('ef_fj%d_a4topo_EFFS'%t,  EFFwdJetHypo('EFJetHypo_fj%d_a4topo_EFFS'%t, t*GeV)) for t in thresholds]))
hypos.update(dict([ ('ef_fj%d_a6topo_EFFS'%t,  EFFwdJetHypo('EFJetHypo_fj%d_a6topo_EFFS'%t, t*GeV)) for t in thresholds]))
#topotowers
hypos.update(dict([ ('ef_fj%d_a4topotower_EFFS'%t,  EFFwdJetHypo('EFJetHypo_fj%d_a4topotower_EFFS'%t, t*GeV)) for t in thresholds]))

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    hvThr = "MU0"
else:
    hvThr = "MU4"
    
    
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet
fexes.cell_maker = TrigCaloCellMaker_jet()

from TrigCaloRec.TrigCaloRecConfig import TrigCaloTowerMaker_jet
fexes.tower_maker = TrigCaloTowerMaker_jet()

# TopoTowers
from TrigCaloRec.TrigCaloRecConfig import TrigCaloTopoTowerMaker_jet
fexes.topoTower_maker_fullcalo = TrigCaloTopoTowerMaker_jet('TrigCaloTopoTowerMaker_jet_fullscan')

# TopoTowers
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCell2ClusterMapperBase
fexes.cell_cluster_mapper = TrigCaloCell2ClusterMapperBase('TrigCaloCell2ClusterMapper_topoTower_fullscan')

# TopoTowers
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topotower
fexes.antikt4_topotower  = TrigJetRec_AntiKt4_topotower()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_Cone
fexes.cone = TrigJetRec_Cone()

# For full scan finding
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
#RoiCreator = DummyAlgo("RoiCreator", runOncePerEvent=True)
fexes.roi = DummyAlgo('RoiCreator')

# for fullscan with topoclusters
fexes.roi_topo = DummyAlgo('RoiCreator_Topo')


# fullscan CaloCellMaker and 4pi RoiCreator for fullscan jet finding
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo
fexes.cell_maker_fullcalo = TrigCaloCellMaker_jet_fullcalo()
# topoclusters
fexes.cell_maker_fullcalo_topo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo",doNoise=0, AbsE=True, doPers=True)
# TopoCluster Maker
from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
fexes.topocluster_maker_fullcalo = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_fullscan',doMoments=False,doLC=False)

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4
fexes.antikt4 = TrigJetRec_AntiKt4()

# topoclusters
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo
fexes.antikt4_topo  = TrigJetRec_AntiKt4_topo()
#EF JES calibration
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo_calib
fexes.antikt4_topo_had  = TrigJetRec_AntiKt4_topo_calib()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt2_topo
fexes.antikt2_topo  = TrigJetRec_AntiKt2_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt6_topo
fexes.antikt6_topo  = TrigJetRec_AntiKt6_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt10_topo
fexes.antikt10_topo = TrigJetRec_AntiKt10_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt6
fexes.antikt6 = TrigJetRec_AntiKt6()

#HI
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt2HI
fexes.antikt2hi = TrigJetRec_AntiKt2HI()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4HI
fexes.antikt4hi = TrigJetRec_AntiKt4HI()


#Hidden Valley
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import L2HVJetHypoAllCuts, L2HVJetHypoTrk, L2HVJetHypo
hypos.l2_hvallcuts_j30 = L2HVJetHypoAllCuts(name='L2HVJetHypoAllCuts_j30', l2_thr=30*GeV, l2_lrat=1.)
hypos.l2_hvtrk_j30 = L2HVJetHypoTrk(name='L2HVJetHypoTrk_j30')
hypos.l2_hv_j30 = L2HVJetHypo(name='L2HVJetHypo_j30', l2_thr=30*GeV, l2_lrat=1.)

from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet
fexes.sitrack = TrigSiTrack_Jet()

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig  import L2MuonJetFex
fexes.l2_mu_jet = L2MuonJetFex()
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import *
hypos.l2_mu_jet = getTrigMuonJetHypoInstance("L2","2011") 

from TrigEFLongLivedParticles.TrigEFLongLivedParticlesConfig import TrigLoFRemovalConfig
fexes.LoF = TrigLoFRemovalConfig()
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import TrigLoFRemovalHypoConfig
hypos.EF_LoF = TrigLoFRemovalHypoConfig()

#end Hidden Valley
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

# for JetEtSum triggers #
from TrigT2MissingHT.TrigT2MissingHTConfig import T2MissingHT_Fex
from TrigT2MissingHT.TrigT2MissingHTConfig import EFMissingHT_Fex

from TrigMissingETHypo.TrigMissingETHypoConfig import L2JetEtSumHypo
from TrigMissingETHypo.TrigMissingETHypoConfig import EFJetEtSumHypo
l2thresholds=[135, 195, 255, 300, 350, 420, 500 ]
efthresholds=[135, 195, 255, 300, 350, 420, 500 ]

for t in l2thresholds:
    attr = 'l2_je%s'%t
    f = T2MissingHT_Fex('L2JetEtSumFex_%d'%t )
    f.METChecks = False
    f.METLabel = 'L2JetEtSum'
    setattr(fexes, attr, f)
    h = L2JetEtSumHypo(name='L2JetEtSumHypo_%d'%t, l2_thr=t*GeV)
    setattr(hypos, attr, h)

for t in efthresholds:
    attr = 'ef_je%s'%t
    f = EFMissingHT_Fex('EFJetEtSumFex_%d'%t )
    f.METLabel = 'EFJetEtSum'
    setattr(fexes, attr, f)
    h = EFJetEtSumHypo(name='EFJetEtSumHypo_%d'%t,ef_thr=t*GeV)
    setattr(hypos, attr, h)

#del jeh
del l2thresholds
del efthresholds

from TrigMinBias.TrigMinBiasConfig import fexes as minBiasFexes
fexes.OneVxFex = minBiasFexes["EFMbVxFex"]
from TrigMinBias.TrigMinBiasConfig import hypos as minBiasHypos 
hypos.OneVxHypo = minBiasHypos["EFMbOneVxHypo"]

# L1.5 jets
from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_TT
from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_TT_TowerCalib
from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_JE
from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_JE_TowerCalib
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4JE
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4JE_TowerCalib
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4JE_JESCalib
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4JE_MultipleOutput_TEs
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4JE_PileupSubtraction
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT_TowerCalib
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT_JESCalib
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT_MultipleOutput_TEs
#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT_PileupSubtraction

fexes.t2l1_tt_unpacking_fex    = T2L1Unpacking_TT()
fexes.t2l1_tt_unpacking_tc_fex = T2L1Unpacking_TT_TowerCalib()
fexes.t2l1_je_unpacking_fex    = T2L1Unpacking_JE()
fexes.t2l1_je_unpacking_tc_fex = T2L1Unpacking_JE_TowerCalib()

#fexes.t2_fastjet_je_fex        = T2CaloFastJet_a4JE()
#fexes.t2_fastjet_je_tc_fex     = T2CaloFastJet_a4JE_TowerCalib()
#fexes.t2_fastjet_je_jes_fex    = T2CaloFastJet_a4JE_JESCalib()
#fexes.t2_fastjet_je_multiple_output_tes_fex = T2CaloFastJet_a4JE_MultipleOutput_TEs()
#fexes.t2_fastjet_je_pileup_subtraction_fex = T2CaloFastJet_a4JE_PileupSubtraction()

#fexes.t2_fastjet_tt_fex        = T2CaloFastJet_a4TT()
#fexes.t2_fastjet_tt_tc_fex     = T2CaloFastJet_a4TT_TowerCalib()
#fexes.t2_fastjet_tt_jes_fex    = T2CaloFastJet_a4TT_JESCalib()
#fexes.t2_fastjet_tt_multiple_output_tes_fex = T2CaloFastJet_a4TT_MultipleOutput_TEs()
#fexes.t2_fastjet_tt_pileup_subtraction_fex = T2CaloFastJet_a4TT_PileupSubtraction()

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_L15
fexes.l2fex_l15 = T2CaloJet_Jet_L15()

from TrigJetHypo.TrigJetHypoConfig import L2FullScanMultiJetHypo, L2FullScanMultiJetNoCutHypo

from TrigJetHypo.TrigJetHypoConfig import L2FullScanMultiJetHypo, L2FullScanMultiJetNoCutHypo,  FullScanL2HTHypo

## no cut thresholds
## no cut thresholds
full_scan_thresholds = [0]
hypos.update(dict([ ('l2_fullscan_j%d'%t,  L2FullScanMultiJetHypo('L2JetHypo_FullScan_j%d'%t, l2_thr=t*GeV, multiplicity=1, l2_thrs=[])) for t in full_scan_thresholds]))

hypos.update(dict([ ('l2_fullscan_NoCut',  L2FullScanMultiJetNoCutHypo('L2JetHypo_FullScan_NoCut'))]))

l2htthresholds=[150]
hypos.update(dict([ ('l2_fullscan_ht%d'%t,  FullScanL2HTHypo('L2HTL1_FullScan_ht%d'%t,t*GeV)) for t in l2htthresholds]))

#############################################################################
class JetTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig):
        """
        Base template for all jet chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        
            
        #print 'ef name', ef.name, sig
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every Jet chain should have

        if 'LArNoiseBurst' in sig:
            self.addStreams('minbias')
        else:    
            self.addStreams('jets') 


#############################################################################
def hypo_from_chain_name(name):
    level,cutval = name.split('_')[0:2]

    if level == 'EF':
        assert hasattr(hypos, 'ef_%s'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'ef_%s'%cutval), cutval
        
    if level == 'L2':
        assert hasattr(hypos, 'l2_%s'%cutval), 'impossible to deduce the L2 hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'l2_%s'%cutval), cutval


helpers = FreeStore()
helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']

        
#############################################################################        
class L2EFChain_jet(JetTemplate):
    """
    Most regular Jet Chains. Their configuration is very trivial.

    Deduction rules:
    see: JetTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are missing.
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        JetTemplate.__init__(self, sig, l2, ef)
        
        # guess L1 threshold
        if l1threshold == None:
            l1threshold = self.l2.lower_chain_name.split('_')[1]

        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        # now is time to construct sequences
        
        # ??? the output TE name shoudl just be the name of the chain .. but that was not followed strictly
        # for backward cmpatibility we try to do the same as was there before. Once it is decided to go along the
        # standard just the chain name should be used

        br = self.l2.chain_name.split('_')        
        l2te = 'L2_%s'%''.join(b for b in br[1:] if b not in helpers.te_no_keys)
        
        #print "l2",sig, self.l2.chain_name,  mangled
        #self.addSequence(l1threshold, [fexes.l2fex, l2hypo], self.l2.chain_name).addSignature()
        self.addSequence(l1threshold, [fexes.l2fex, l2hypo], l2te).addSignature()

        # ef sequence (do not have to specify explicitly this are ef algs)
        #self.continueSequence([fexes.cell_maker, fexes.tower_maker, fexes.cone, efhypo], self.ef.chain_name).addSignature()
        br = self.ef.chain_name.split('_')
        efte = 'EF_%s'%''.join([ b for b in br[1:] if b not in helpers.te_no_keys])
            
        self.continueSequence([fexes.cell_maker, fexes.tower_maker, fexes.cone, efhypo], efte).addSignature()

        self.addGroups('BW:Jets')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_Jet')
        else:
            if 'JF' in l2.seed:
                self.addGroups('RATE:SingleForwardJet')
            else:
                self.addGroups('RATE:SingleJet')
                
        
        # that's it!
        pass 

#############################################################################

# since the L2 chains are shared many times let's make them here
# it would not be a big deal to repeat them also
l2chains = FreeStore()
l2chains.j15 = Chain(name='L2_j15', counter=103, seed='L1_J5')
l2chains.j15_L1J10 = Chain(name='L2_j15_L1J10', counter=103, seed='L1_J10') # test trigger
l2chains.j25 = Chain(name='L2_j25', counter=106, seed='L1_J10')
l2chains.j30 = Chain(name='L2_j30', counter=109, seed='L1_J15')
l2chains.j35 = Chain(name='L2_j35', counter=57, seed='L1_J15')
l2chains.j35_loose = Chain(name='L2_j35_loose', counter=58, seed='L1_J10')
l2chains.j40 = Chain(name='L2_j40', counter=117, seed='L1_J20')
l2chains.j40_loose = Chain(name='L2_j40_loose', counter=117, seed='L1_J15')
l2chains.j45 = Chain(name='L2_j45', counter=118, seed='L1_J30')
l2chains.j50 = Chain(name='L2_j50', counter=156, seed='L1_J30')
l2chains.j60 = Chain(name='L2_j60', counter=1156, seed='L1_J30')    
l2chains.j65 = Chain(name='L2_j65', counter=191, seed='L1_J30')
l2chains.j70 = Chain(name='L2_j70', counter=112, seed='L1_J50')
l2chains.j75 = Chain(name='L2_j75', counter=111, seed='L1_J50')    
l2chains.j90 = Chain(name='L2_j90', counter=113, seed='L1_J75')
l2chains.j95 = Chain(name='L2_j95', counter=119, seed='L1_J75')
l2chains.j200 = Chain(name='L2_j200', counter=1190, seed='L1_J175')
l2chains.fj25 = Chain(name='L2_fj25', counter=116, seed='L1_FJ10')
l2chains.fj35 = Chain(name='L2_fj35', counter=116, seed='L1_FJ10')
l2chains.fj50 = Chain(name='L2_fj50', counter=125, seed='L1_FJ30')
l2chains.fj70 = Chain(name='L2_fj70', counter=126, seed='L1_FJ50')
l2chains.fj95 = Chain(name='L2_fj95', counter=128, seed='L1_FJ75')

l2chains.rd0_filled = Chain(name='L2_rd0_filled_NoAlg', counter=623, seed='L1_RD0_FILLED')

l2chains.L1J30_empty = Chain(name='L2_L1J30_empty_NoAlg', counter=629,seed='L1_J30_EMPTY')

#l2chains.j = Chain(name=)
#l2chains.j = Chain(name=)
#l2chains.j = Chain(name=)

#Single jets
Jets = [

    L2EFChain_jet('j20', l2=l2chains.j15, ef=Chain(counter=631)),
    L2EFChain_jet('j30', l2=l2chains.j25, ef=Chain( counter=774)),
    L2EFChain_jet('j35', l2=l2chains.j30, ef=Chain(counter=633)),
    L2EFChain_jet('j40_loose', l2=l2chains.j35_loose, ef=Chain(counter=633)),
    L2EFChain_jet('j45_loose', l2=l2chains.j40_loose, ef=Chain(counter=633)),
#    L2EFChain_jet('j40', l2=l2chains.j35, ef=Chain(counter=633)),
    L2EFChain_jet('j50', l2=l2chains.j45, ef=Chain(counter=634)),
    L2EFChain_jet('j55', l2=l2chains.j50, ef=Chain(counter=634)),
    L2EFChain_jet('j75', l2=l2chains.j70, ef=Chain(counter=635)),
    L2EFChain_jet('j95', l2=l2chains.j90, ef=Chain(counter=636)),
    L2EFChain_jet('j100', l2=l2chains.j95, ef=Chain(counter=636)),
    L2EFChain_jet('j200', l2=l2chains.j200, ef=Chain(counter=3000)),
    ]


    
Jets += [
    #     #chains for non-collid BCIDs
    #     #using standard jet thresholds
    
    L2EFChain_jet('j30_cosmic', l2=Chain(name='L2_j25_cosmic', counter=1012, seed='L1_J10_EMPTY')),
    L2EFChain_jet('j50_cosmic', l2=Chain(name='L2_j45_cosmic', counter=1013, seed='L1_J30_EMPTY')),
    L2EFChain_jet('j30_firstempty', l2=Chain(name='L2_j25_firstempty', counter=1016, seed='L1_J10_FIRSTEMPTY')),
    L2EFChain_jet('j50_firstempty', l2=Chain(name='L2_j45_firstempty', counter=1017, seed='L1_J30_FIRSTEMPTY')),
    
    ]

#Single forward jets
Jets += [
    #      #forward jets
    L2EFChain_jet('fj30', l2=l2chains.fj25,
                  l1threshold='JF10', l2hypo=hypos.l2_fj25, efhypo=hypos.ef_fj30).addGroups('RATE:SingleForwardJet').removeGroups('RATE:SingleJet'),
    L2EFChain_jet('fj40', l2=l2chains.fj35,
                  l1threshold='JF10', l2hypo=hypos.l2_fj35, efhypo=hypos.ef_fj40).addGroups('RATE:SingleForwardJet').removeGroups('RATE:SingleJet'),
    L2EFChain_jet('fj50', l2=Chain(name='L2_fj45', counter=278, seed='L1_FJ30'),
                  l1threshold='JF30', l2hypo=hypos.l2_fj45, efhypo=hypos.ef_fj50).addGroups('RATE:SingleForwardJet').removeGroups('RATE::SingleJet'),
    L2EFChain_jet('fj55', l2=Chain(name='L2_fj50', counter=278, seed='L1_FJ30'),
                  l1threshold='JF30', l2hypo=hypos.l2_fj50, efhypo=hypos.ef_fj55).addGroups('RATE:SingleForwardJet').removeGroups('RATE::SingleJet'),

    L2EFChain_jet('fj75',l2=Chain(name='L2_fj70', counter=608, seed='L1_FJ50'),
                  l1threshold='JF50', l2hypo=hypos.l2_fj70, efhypo=hypos.ef_fj75).addGroups('RATE:SingleForwardJet').removeGroups('RATE::SingleJet'),        
    ]

#Single forward jets - jetNoCut
Jets += [
    
    L2EFChain_jet('fj30_jetNoCut', l2=Chain(name='L2_fj25_jetNoCut', counter=117, seed='L1_FJ10'),
                  l1threshold='JF10', l2hypo=hypos.l2nocut_fj25 , efhypo=hypos.efnocut_fj30 ).addGroups('RATE:SingleForwardJet').removeGroups('RATE::SingleJet'),
    ]


class L2EFChain_L1TAU_HV(JetTemplate):
    """
    Most regular Jet Chains. Their configuration is very trivial.

    This are deduced if not given:
    EF chain name  --- from signature name i.e. EF_+sig
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are missing.    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, hvhypoallcuts=None, efhypo=None, l1threshold='J15'):
        # generate EF chain name to be identical as the signature name (but only if not given)
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef=ef)


        # guess hypos (but also remember the l2 cut (we stick the same into the HV hypo)
        l2cut = l2.name.split('_')[1]
        if l2hypo==None:
            l2hypo = getattr(hypos, 'l2_%s'%l2cut)
            assert l2hypo, 'impossible to deduce the L2 HV hypothesis algorithm'

        if hvhypoallcuts==None:
            hvhypoallcuts = getattr(hypos, 'l2_hvallcuts_%s'%l2cut)
            assert hvhypoallcuts, 'impossible to deduce the L2 HV hypothesis algorithm'


            
        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(ef.name)

        br = self.l2.chain_name.split('_')
        l2te = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)

        # now is time to construct sequences
        # ??? the TE name shoudl folow the standard i.e. should be chain name +suffix
        self.addSequence(l1threshold, [fexes.l2fex, l2hypo], 'L2_jhvallcuts_%s'%l2te).addSignature()
        
        self.continueSequence([fexes.sitrack, hvhypoallcuts], 'L2_%s'%l2te).addSignature()

        br = self.l2.chain_name.split('_')
        efte = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)
        # if we use continue we do not have to worry about matchin the names
        #        self.continueSequence([fexes.cell_maker, fexes.tower_maker, fexes.cone, efhypo], 'EF_%s'%efte).addSignature()
        self.addSequence('',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters')
        self.continueSequence([fexes.antikt4_topo, efhypo], 'EF_%s'%efte).addSignature()
        self.addGroups('RATE:SingleTau', 'BW:Tau')
        # that's it!

        pass

class L2EFChain_L1TAU_HVtrk_LOF(JetTemplate):
    """
    Most regular Jet Chains. Their configuration is very trivial.

    This are deduced if not given:
    EF chain name  --- from signature name i.e. EF_+sig
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are missing.    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, hvhypo=None, hvhypotrk=None, efhypo=None, l1threshold='J15'):
        # generate EF chain name to be identical as the signature name (but only if not given)
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef=ef)


        # guess hypos (but also remember the l2 cut (we stick the same into the HV hypo), remove l2hypo because cut on jet energy are now done in hvhypo before sitrack 
        l2cut = l2.name.split('_')[1]
        if hvhypo==None:
            hvhypo = getattr(hypos, 'l2_hv_%s'%l2cut)
            assert hvhypo, 'impossible to deduce the L2 HV hypothesis algorithm'

        if hvhypotrk==None:
            hvhypotrk = getattr(hypos, 'l2_hvtrk_%s'%l2cut)
            assert hvhypotrk, 'impossible to deduce the L2 HV hypothesis algorithm'


            
        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(ef.name)

        br = self.l2.chain_name.split('_')
        l2te = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)

        # now is time to construct sequences
        # ??? the TE name shoudl folow the standard i.e. should be chain name +suffix
        self.addSequence('L2_%s'%l2cut,[ hvhypo], 'L2_jhv_%s'%l2cut)
        self.continueSequence([fexes.sitrack, hypos.l2_hvtrk_j30], 'L2_%s'%l2te).addSignature()

        br = self.l2.chain_name.split('_')
        efte = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)
        # if we use continue we do not have to worry about matchin the names
        #        self.continueSequence([fexes.cell_maker, fexes.tower_maker, fexes.cone, efhypo], 'EF_%s'%efte).addSignature()
#        self.addSequence('',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters')
#        self.continueSequence([fexes.antikt4_topo, efhypo], 'EF_%s'%efte).addSignature()

        efcut = self.ef.chain_name.split('_')[1]
        self.addSequence('EF_%s'%efcut + '_a4tc_EFFS', [fexes.LoF, hypos.EF_LoF], self.ef.chain_name).addSignature()

        self.addGroups('RATE:SingleTau', 'BW:Tau')
        # that's it!

        pass 


class L2EFChain_L1TAU_HVtrk(JetTemplate):
    """
    Most regular Jet Chains. Their configuration is very trivial.

    This are deduced if not given:
    EF chain name  --- from signature name i.e. EF_+sig
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are missing.    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, hvhypo=None, hvhypotrk=None, efhypo=None, l1threshold='J15'):
        # generate EF chain name to be identical as the signature name (but only if not given)
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef=ef)


        # guess hypos (but also remember the l2 cut (we stick the same into the HV hypo), remove l2hypo because cut on jet energy are now done in hvhypo before sitrack 
        l2cut = l2.name.split('_')[1]
        if hvhypo==None:
            hvhypo = getattr(hypos, 'l2_hv_%s'%l2cut)
            assert hvhypo, 'impossible to deduce the L2 HV hypothesis algorithm'

        if hvhypotrk==None:
            hvhypotrk = getattr(hypos, 'l2_hvtrk_%s'%l2cut)
            assert hvhypotrk, 'impossible to deduce the L2 HV hypothesis algorithm'


            
        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(ef.name)

        br = self.l2.chain_name.split('_')
        l2te = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)

        # now is time to construct sequences
        # ??? the TE name shoudl folow the standard i.e. should be chain name +suffix
        self.addSequence(l1threshold, [fexes.l2fex, hvhypo], 'L2_jhv_%s'%l2te).addSignature()

        self.continueSequence([fexes.sitrack, hvhypotrk], 'L2_%s'%l2te).addSignature()

        br = self.l2.chain_name.split('_')
        efte = ''.join(b for b in br[1:] if b not in helpers.te_no_keys)
        # if we use continue we do not have to worry about matchin the names
        #        self.continueSequence([fexes.cell_maker, fexes.tower_maker, fexes.cone, efhypo], 'EF_%s'%efte).addSignature()
        self.addSequence('',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters')
        self.continueSequence([fexes.antikt4_topo, efhypo], 'EF_%s'%efte).addSignature()
        self.addGroups('RATE:SingleTau', 'BW:Tau')
        # that's it!

        pass 


Jets += [

    L2EFChain_L1TAU_HV('j35_a4tc_EFFS_L1TAU_HV', l2=Chain(name='L2_j30_L1TAU_HV', counter=982, seed='L1_TAU30'), efhypo = hypos.ef_j35_a4topo_EFFS),

    L2EFChain_L1TAU_HV('j35_a4tc_EFFS_L1TAU_HV_firstempty', l2=Chain(name='L2_j30_L1TAU_HV_firstempty', counter=940, seed='L1_TAU8_FIRSTEMPTY'), efhypo = hypos.ef_j35_a4topo_EFFS),

    L2EFChain_L1TAU_HV('j35_a4tc_EFFS_L1TAU_HV_unpaired_iso', l2=Chain(name='L2_j30_L1TAU_HV_unpaired_iso', counter=939, seed='L1_TAU8_UNPAIRED_ISO'), efhypo = hypos.ef_j35_a4topo_EFFS),

    L2EFChain_L1TAU_HV('j35_a4tc_EFFS_L1TAU_HV_unpaired_noniso', l2=Chain(name='L2_j30_L1TAU_HV_unpaired_noniso', counter=904, seed='L1_TAU8_UNPAIRED_NONISO'), efhypo = hypos.ef_j35_a4topo_EFFS),

    L2EFChain_L1TAU_HV('j35_a4tc_EFFS_L1TAU_HV_cosmic', l2=Chain(name='L2_j30_L1TAU_HV_cosmic', counter=878, seed='L1_TAU8_EMPTY'), efhypo = hypos.ef_j35_a4topo_EFFS),

                       
    ]

Jets += [
    L2EFChain_L1TAU_HVtrk_LOF('j35_a4tc_EFFS_L1TAU_HVtrk_LOF', l2=Chain(name='L2_j30_L1TAU_HVtrk_LOF', counter=876, seed='L1_TAU30'), efhypo = hypos.ef_j35_a4topo_EFFS),
    ]


Jets += [
    L2EFChain_L1TAU_HVtrk('j35_a4tc_EFFS_L1TAU_HVtrk', l2=Chain(name='L2_j30_L1TAU_HVtrk', counter=903, seed='L1_TAU30'), efhypo = hypos.ef_j35_a4topo_EFFS) ,
    L2EFChain_L1TAU_HVtrk('j35_a4tc_EFFS_L1TAU_HVtrk_firstempty'      , l2=Chain(name='L2_j30_L1TAU_HVtrk_firstempty'      , counter=905  , seed='L1_TAU8_FIRSTEMPTY')      , efhypo = hypos.ef_j35_a4topo_EFFS) ,
    L2EFChain_L1TAU_HVtrk('j35_a4tc_EFFS_L1TAU_HVtrk_unpaired_iso'    , l2=Chain(name='L2_j30_L1TAU_HVtrk_unpaired_iso'    , counter=906   , seed='L1_TAU8_UNPAIRED_ISO')    , efhypo = hypos.ef_j35_a4topo_EFFS) ,
    L2EFChain_L1TAU_HVtrk('j35_a4tc_EFFS_L1TAU_HVtrk_unpaired_noniso' , l2=Chain(name='L2_j30_L1TAU_HVtrk_unpaired_noniso' , counter=909   , seed='L1_TAU8_UNPAIRED_NONISO') , efhypo = hypos.ef_j35_a4topo_EFFS) ,
    L2EFChain_L1TAU_HVtrk('j35_a4tc_EFFS_L1TAU_HVtrk_cosmic'          , l2=Chain(name='L2_j30_L1TAU_HVtrk_cosmic'          , counter=919   , seed='L1_TAU8_EMPTY')           , efhypo = hypos.ef_j35_a4topo_EFFS) ,
    ]

class  L2EFChain_tracklessHV(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l1threshold=None):
        JetTemplate.__init__(self, sig, l2, ef)

        self.addSequence(l1threshold, [fexes.l2fex, hypos.l2_j30], '%s_jet'%l2.name).addSignature()
        self.continueSequence([fexes.sitrack], '%s_hv'%l2.name).addSignature()
        self.addSequence( ['%s_hv'%l2.name, 'MU6'], [fexes.l2_mu_jet, hypos.l2_mu_jet], l2.name).addSignature()
        # nothing going on at EF

        self.addGroups('BW:Jets', 'RATE:SingleJet')
        

Jets += [

    L2EFChain_tracklessHV('l2j30_Trackless_HV', l2=Chain(name='L2_j30_Trackless_HV', counter=981, seed='L1_%s_J15' % hvThr),
                          l1threshold='J15'),

    L2EFChain_tracklessHV('l2j30_Trackless_HV_L1MU10', l2=Chain(name='L2_j30_Trackless_HV_L1MU10', counter=398, seed='L1_MU10_J10'),
                          l1threshold='J15'),

    L2EFChain_tracklessHV('l2j30_Trackless_HV_cosmic', l2=Chain(name='L2_j30_Trackless_HV_cosmic', counter=988,
                                                                seed='L1_%s_J15_EMPTY' % hvThr), l1threshold='J15'),

    L2EFChain_tracklessHV('l2j30_Trackless_HV_firstempty', l2=Chain(name='L2_j30_Trackless_HV_firstempty', counter=983,
                                                                seed='L1_%s_J15_FIRSTEMPTY' % hvThr), l1threshold='J15'),

    L2EFChain_tracklessHV('l2j30_Trackless_HV_unpaired_iso', l2=Chain(name='L2_j30_Trackless_HV_unpaired_iso', counter=985,
                                                                seed='L1_%s_J15_UNPAIRED_ISO' % hvThr), l1threshold='J15'),
    L2EFChain_tracklessHV('l2j30_Trackless_HV_unpaired_noniso', l2=Chain(name='L2_j30_Trackless_HV_unpaired_noniso', counter=11,
                                                                seed='L1_%s_J15_UNPAIRED_NONISO' % hvThr), l1threshold='J15'),

    ]


# in fact this chains do not woth template now. But in future maybe there will be some cutting here.
class L2EFChain_jet_je(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, l1threshold=None, effex=None,efhypo=None,
                 empty_sigs=False):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        self.addGroups('BW:Jets')
        self.addGroups('RATE:TotalJetEnergy')
        
        if l1threshold == None:
            l1threshold = self.l2.lower_chain_name.split('_')[1]

        if empty_sigs:
            # This option was added to configure the chain as empty chain
            # since L2_je255 was configured so in 2011 running
            return
        
        self.addSequence('J10', [fexes.l2fex, hypos.l2_j15], 'L2_j25').addSignature()
        #self.addSequence('L2_j25', [fexes.effex, hypos.j30_a4_EFFS], 'j30_a4_EFFS').addSignature()
        self.continueSequence([l2fex,l2hypo], l2.name).addSignature()


Jets +=[

    L2EFChain_jet_je('je195_NoEF',l2=Chain(name='L2_je195', counter=4255, seed='L1_JE100'), l2fex=fexes.l2_je195, l2hypo=hypos.l2_je195),
    #L2EFChain_jet_je('je255_NoEF',l2=Chain(name='L2_je255', counter=4256, seed='L1_JE140'), l2fex=fexes.l2_je255, l2hypo=hypos.l2_je255),
    # Configuring je255_NoEF in the wrong way deliberately as it was done so in 2011 running (v2,v3)
    L2EFChain_jet_je('je255_NoEF',l2=Chain(name='L2_je255', counter=4256, seed='L1_JE140'), l2fex=None, l2hypo=None, empty_sigs=True),
    L2EFChain_jet_je('je300_NoEF',l2=Chain(name='L2_je300', counter=4249, seed='L1_JE140'), l2fex=fexes.l2_je300, l2hypo=hypos.l2_je300),
    L2EFChain_jet_je('je350_NoEF',l2=Chain(name='L2_je350', counter=4257, seed='L1_JE200'), l2fex=fexes.l2_je350, l2hypo=hypos.l2_je350),
    L2EFChain_jet_je('je420_NoEF',l2=Chain(name='L2_je420', counter=4488, seed='L1_JE140'), l2fex=fexes.l2_je420, l2hypo=hypos.l2_je420),
    L2EFChain_jet_je('je500_NoEF',l2=Chain(name='L2_je500', counter=4489, seed='L1_JE140'), l2fex=fexes.l2_je500, l2hypo=hypos.l2_je500),
    
]

class L2EFChain_NoAlg(JetTemplate):
    def __init__(self, sig, l2):
        JetTemplate.__init__(self, sig, l2)
        self.addGroups('BW:Jets')

Jets +=[

    L2EFChain_NoAlg('L1FJ0_empty_NoAlg', l2=Chain(counter=307, seed='L1_FJ0_EMPTY')).addGroups('RATE:Cosmic_ForwardJet'),
    L2EFChain_NoAlg('L1FJ1_empty_NoAlg', l2=Chain(counter=308, seed='L1_FJ1_EMPTY')).addGroups('RATE:Cosmic_ForwardJet'),
    L2EFChain_NoAlg('L1FJ10_empty_NoAlg', l2=Chain(counter=314, seed='L1_FJ10_EMPTY')).addGroups('RATE:Cosmic_ForwardJet'),
    L2EFChain_NoAlg('L1J10_empty_NoAlg',l2=Chain(counter=566, seed='L1_J10_EMPTY')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J30_empty_NoAlg',l2=Chain(counter=567, seed='L1_J30_EMPTY')).addGroups('RATE:SingleJet'),

    L2EFChain_NoAlg('L1J5_NoAlg',l2=Chain(counter=321, seed='L1_J5')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J5_empty_NoAlg',l2=Chain(counter=305, seed='L1_J5_EMPTY')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J5_firstempty_NoAlg',l2=Chain(counter=306, seed='L1_J5_FIRSTEMPTY')).addGroups('RATE:SingleJet'),


    L2EFChain_NoAlg('L1J10_firstempty_NoAlg',l2=Chain(counter=568, seed='L1_J10_FIRSTEMPTY')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J30_firstempty_NoAlg',l2=Chain(counter=569, seed='L1_J30_FIRSTEMPTY')).addGroups('RATE:SingleJet'),

    L2EFChain_NoAlg('L1J10_unpaired_iso_NoAlg',l2=Chain(counter=570, seed='L1_J10_UNPAIRED_ISO')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J30_unpaired_iso_NoAlg',l2=Chain(counter=581, seed='L1_J30_UNPAIRED_ISO')).addGroups('RATE:SingleJet'),

    L2EFChain_NoAlg('L1J10_unpaired_noniso_NoAlg',l2=Chain(counter=571, seed='L1_J10_UNPAIRED_NONISO')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J30_unpaired_noniso_NoAlg',l2=Chain(counter=759, seed='L1_J30_UNPAIRED_NONISO')).addGroups('RATE:SingleJet'),

    
    #These two L1 items don't exist yet
    L2EFChain_NoAlg('L1J10_NoAlg',l2=Chain(counter=732, seed='L1_J10')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J75_NoAlg',l2=Chain(counter=936, seed='L1_J75')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J175_NoAlg',l2=Chain(counter=5, seed='L1_J175')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J350_NoAlg',l2=Chain(counter=5, seed='L1_J350')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1J250_NoAlg',l2=Chain(counter=6, seed='L1_J250')).addGroups('RATE:SingleJet'),
    L2EFChain_NoAlg('L1FJ75_NoAlg',l2=Chain(counter=279, seed='L1_FJ75')).addGroups('RATE:SingleForwardJet'),
    L2EFChain_NoAlg('L1FJ100_NoAlg',l2=Chain(counter=180, seed='L1_FJ100')).addGroups('RATE:SingleForwardJet'),
    L2EFChain_NoAlg('L1JE500_NoAlg',l2=Chain(counter=181, seed='L1_JE500')),
    L2EFChain_NoAlg('L1JE200_NoAlg',l2=Chain(counter=182, seed='L1_JE200')),
    L2EFChain_NoAlg('L1JE140_NoAlg',l2=Chain(counter=183, seed='L1_JE140')),
    L2EFChain_NoAlg('L1JE350_NoAlg',l2=Chain(counter=184, seed='L1_JE350')),
    ]

Jets +=[
    L2EFChain_jet('j30_jetNoCut',l2=Chain(name='L2_j25_jetNoCut', counter=106, seed='L1_J10'),
                  l2hypo = hypos.l2nocut_j25, efhypo = hypos.efnocut_j30),

    L2EFChain_jet('j30_jetNoEF',l2=Chain(name='L2_j25', counter=106, seed='L1_J10'),
                  efhypo = hypos.efnocut_j30),

    ]

class L2EFChain_jet_antikt(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4, efhypo=None, groups=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # guess threshold
        threshold = l2.seed.split('_')[1] 

        # guess hypos
        l2hypo, cut = hypo_from_chain_name(l2.name)        
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex, l2hypo], l2.name).addSignature()

        # EF
        self.continueSequence([fexes.cell_maker, fexes.tower_maker, antikt, efhypo], ef.name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']
        self.addGroups(*groups)
        pass 
        

# # AntiKt fullscan chains

# #--------------------------
# # AntiKt R=0.4 fullscan
# #--------------------------

class L2EFChain_jet_antikt_fullscan(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4, efhypo=None, threshold=None, groups=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # guess threshold
        if threshold == None:
            threshold = l2.seed.split('_')[1] 

        # guess hypos
        if l2hypo == None:
            cut = l2.name.split('_')[1]
            l2hypo = getattr(hypos, 'l2_%s'%cut)
            assert l2hypo, 'impossible to deduce the L2 hypothesis algorithm'
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex, l2hypo], self.l2.chain_name).addSignature()

        # EF
        self.addSequence('',[fexes.roi,
                             fexes.cell_maker_fullcalo, fexes.tower_maker], 'EF_FSTowers') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']
            
        self.addGroups(*groups)
        pass

class L2EFChain_jet_antikt_fullscan_rndm(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, antikt=fexes.antikt4, efhypo=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # EF
        self.addSequence('',[fexes.roi,
                             fexes.cell_maker_fullcalo, fexes.tower_maker], 'EF_FSTowers') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()
        
        self.addGroups('RATE:SingleJet', 'BW:Jets')
        pass    

class L2EFChain_jet_antikt_fullscan_hi(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, antikt=fexes.antikt2hi, efhypo=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # EF
        self.addSequence('',[fexes.roi,
                             fexes.cell_maker_fullcalo_topo, fexes.tower_maker], 'EF_FSTowersHI') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()
        
        self.addGroups('RATE:SingleJet', 'BW:Jets')
        pass
    
class L2EFChain_jet_antikt_fullscan_topo(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4_topo, efhypo=None, threshold=None, groups=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # guess threshold
        if threshold == None:
            threshold = l2.seed.split('_')[1]

        # guess hypos
        if l2hypo == None:
            cut = l2.name.split('_')[1]
            l2hypo = getattr(hypos, 'l2_%s'%cut)
            assert l2hypo, 'impossible to deduce the L2 hypothesis algorithm'
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex, l2hypo], self.l2.chain_name).addSignature()

        # EF
        self.addSequence('',[fexes.roi_topo,
                             fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']

        self.addGroups(*groups)
        pass


class L2EFChain_jet_unclean_fullscan(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4_topo, efhypo=None, threshold=None, groups=None):
        print "starting unclean"
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # guess threshold
        if threshold == None:
            threshold = l2.seed.split('_')[1]

        # guess hypos
        if l2hypo == None:
            cut = l2.name.split('_')[1]
            l2hypo = getattr(hypos, 'l2_%s'%cut)
            assert l2hypo, 'impossible to deduce the L2 hypothesis algorithm'
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex, l2hypo], self.l2.chain_name).addSignature()

        # EF
        print fexes.lar_noisy
        self.addSequence('',[fexes.roi_topo,
                             fexes.cell_maker_fullcalo_topo], 'EF_FSUnclean') # not in a chain signature

        self.continueSequence([fexes.lar_noisy, efhypo], self.ef.chain_name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']





        self.addGroups(*groups)
        pass



class L2EFChain_jet_unclean_fullscan_NoAlg(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, antikt=fexes.antikt4_topo, efhypo=None):
        print "starting unclean_NoAlg"
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        #EF
        print fexes.lar_noisy
        self.addSequence('',[fexes.roi_topo,
                             fexes.cell_maker_fullcalo_topo], 'EF_FSUnclean') # not in a chain signature

        self.continueSequence([fexes.lar_noisy, efhypo], self.ef.chain_name).addSignature()

        groups = ['RATE:SingleJet', 'BW:Jets']
        self.addGroups(*groups)
        
        
        pass


class L2EFChain_jet_antikt_fullscan_topo_rndm(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, antikt=fexes.antikt4_topo, efhypo=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # EF
        self.addSequence('',[fexes.roi_topo,
                             fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        self.addGroups('RATE:SingleJet', 'BW:Jets')
        pass


#     MultiJet(, l2=Chain(name=, counter=, seed=), efname=['']*),

Jets += [
#     #---------------------------------------------------
#     # AntiKt R=0.4 fullscan
#     #---------------------------------------------------
    L2EFChain_jet_antikt_fullscan_topo_rndm('j10_a4tc_EFFS',
                                            l2=None,
                                            ef=Chain(counter=1001, seed='L2_rd0_filled_NoAlg'),
                                            efhypo=hypos.ef_j10_a4topo_EFFS
                                            ),

    L2EFChain_jet_antikt_fullscan_topo_rndm('j15_a4tc_EFFS',
                                            l2=None,
                                            ef=Chain(counter=1002, seed='L2_rd0_filled_NoAlg'),
                                            efhypo=hypos.ef_j15_a4topo_EFFS
                                            ),

    L2EFChain_jet_antikt_fullscan_topo_rndm('j20_a4tc_EFFS', l2=None,
                                            ef=Chain(counter=1003, seed='L2_rd0_filled_NoAlg'),
                                            efhypo = hypos.ef_j20_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j20_a4tc_EFFS_L1J10', l2=l2chains.j15_L1J10, ef=Chain(counter=6012),
                                  efhypo = hypos.ef_j20_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j30_a4tc_EFFS', l2=l2chains.j25, ef=Chain(counter=1004),
                                  efhypo = hypos.ef_j30_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j30_a4tc_EFFS_efclean', l2=l2chains.j25, ef=Chain(counter=850),
                                  efhypo = hypos.ef_j30_a4topo_EFFS_clean),     
    
    L2EFChain_jet_antikt_fullscan_topo('j30_a4tc_EFFS_efcleaninv', l2=l2chains.j25, ef=Chain(counter=851),
                                  efhypo = hypos.ef_j30_a4topo_EFFS_cleaninv),     

    L2EFChain_jet_antikt_fullscan_topo('j35_a4tc_EFFS', l2=l2chains.j30, ef=Chain(counter=1005),
                                  efhypo = hypos.ef_j35_a4topo_EFFS),  

    L2EFChain_jet_antikt_fullscan_topo('j40_a4tc_EFFS', l2=l2chains.j35, ef=Chain(counter=1006),
                                  efhypo = hypos.ef_j40_a4topo_EFFS),  

    L2EFChain_jet_antikt_fullscan_topo('j45_a4tc_EFFS', l2=l2chains.j40, ef=Chain(counter=955),
                                  efhypo = hypos.ef_j45_a4topo_EFFS),  

    L2EFChain_jet_antikt_fullscan_topo('j50_a4tc_EFFS',l2=l2chains.j45, ef=Chain(counter=957),
                                  efhypo = hypos.ef_j50_a4topo_EFFS), 

    L2EFChain_jet_antikt_fullscan_topo('j55_a4tc_EFFS',l2=l2chains.j50, ef=Chain(counter=958),
                                  efhypo = hypos.ef_j55_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j65_a4tc_EFFS',l2=l2chains.j60, ef=Chain(counter=1958),
                                  efhypo = hypos.ef_j65_a4topo_EFFS), 

    L2EFChain_jet_antikt_fullscan_topo('j70_a4tc_EFFS',l2=l2chains.j65, ef=Chain(counter=3598),
                                   efhypo = hypos.ef_j70_a4topo_EFFS),  

    L2EFChain_jet_antikt_fullscan_topo('j75_a4tc_EFFS',l2=l2chains.j70, ef=Chain(counter=951),
                                   efhypo = hypos.ef_j75_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j80_a4tc_EFFS',l2=l2chains.j75, ef=Chain(counter=775),
                                   efhypo = hypos.ef_j80_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j90_a4tc_EFFS',l2=l2chains.j70, ef=Chain(counter=3597),
                                  efhypo = hypos.ef_j90_a4topo_EFFS),  

    L2EFChain_jet_antikt_fullscan_topo('j100_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=758),
                                  efhypo = hypos.ef_j100_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j135_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=760),
                                  efhypo = hypos.ef_j135_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j180_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=762),
                                  efhypo = hypos.ef_j180_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j240_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=764),
                                  efhypo = hypos.ef_j240_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j320_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=763),
                                  efhypo = hypos.ef_j320_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j425_a4tc_EFFS', l2=l2chains.j95, ef=Chain(counter=765),
                                  efhypo = hypos.ef_j425_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo_rndm('fj10_a4tc_EFFS',
                                       l2=None,
                                       ef=Chain(counter=864, seed='L2_rd0_filled_NoAlg'),
                                       efhypo=hypos.ef_fj10_a4topo_EFFS
                                       ),

    L2EFChain_jet_antikt_fullscan_topo_rndm('fj15_a4tc_EFFS',
                                       l2=None,
                                       ef=Chain(counter=865, seed='L2_rd0_filled_NoAlg'),
                                       efhypo=hypos.ef_fj15_a4topo_EFFS
                                       ),

    L2EFChain_jet_antikt_fullscan_topo_rndm('fj20_a4tc_EFFS',
                                       l2=None,
                                       ef=Chain(counter=866, seed='L2_rd0_filled_NoAlg'),
                                       efhypo=hypos.ef_fj20_a4topo_EFFS
                                       ),

    L2EFChain_jet_antikt_fullscan_topo('fj30_a4tc_EFFS', l2=l2chains.fj25, ef=Chain(counter=766),
                                       efhypo = hypos.ef_fj30_a4topo_EFFS,
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"]),
    
    L2EFChain_jet_antikt_fullscan_topo('fj55_a4tc_EFFS', l2=l2chains.fj50, ef=Chain(counter=767),
                                       efhypo = hypos.ef_fj55_a4topo_EFFS,
                                       threshold='JF30',
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"]),
 
    L2EFChain_jet_antikt_fullscan_topo('fj75_a4tc_EFFS', l2=l2chains.fj70, ef=Chain(counter=768),
                                       efhypo = hypos.ef_fj75_a4topo_EFFS,
                                       threshold='JF50',
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"]),   

    L2EFChain_jet_antikt_fullscan_topo('fj100_a4tc_EFFS', l2=l2chains.fj95, ef=Chain(counter=769),
                                       efhypo = hypos.ef_fj100_a4topo_EFFS,
                                       threshold='JF75',
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"]),   

    L2EFChain_jet_antikt_fullscan_topo('fj135_a4tc_EFFS', l2=l2chains.fj95, ef=Chain(counter=728),
                                       efhypo = hypos.ef_fj135_a4topo_EFFS,
                                       threshold='JF75',
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"]),

    # empty, firstempty triggers 
    L2EFChain_jet_antikt_fullscan_topo('j30_eta13_a4tc_EFFS_empty', l2=Chain(name='L2_j30_jetNoCut', counter=772, seed='L1_J10_EMPTY'),
                                       ef=Chain(counter=772), l2hypo=hypos.l2nocut_j30,
                                       efhypo = hypos.ef_j30_eta13_a4topo_EFFS),
    L2EFChain_jet_antikt_fullscan_topo('j30_eta13_a4tc_EFFS_firstempty', l2=Chain(name='L2_j30_jetNoCut', counter=772, seed='L1_J10_FIRSTEMPTY'),
                                       ef=Chain(counter=773), l2hypo=hypos.l2nocut_j30,
                                       efhypo = hypos.ef_j30_eta13_a4topo_EFFS),
    L2EFChain_jet_antikt_fullscan_topo('j50_eta13_a4tc_EFFS_empty', l2=Chain(name='L2_j50_jetNoCut', counter=774, seed='L1_J30_EMPTY'),
                                       ef=Chain(counter=774), l2hypo=hypos.l2nocut_j45,
                                       efhypo = hypos.ef_j50_eta13_a4topo_EFFS),
    L2EFChain_jet_antikt_fullscan_topo('j50_eta13_a4tc_EFFS_firstempty', l2=Chain(name='L2_j50_jetNoCut', counter=774, seed='L1_J30_FIRSTEMPTY'),
                                       ef=Chain(counter=775), l2hypo=hypos.l2nocut_j45,
                                       efhypo = hypos.ef_j50_eta13_a4topo_EFFS),
    L2EFChain_jet_antikt_fullscan_topo('j50_eta25_a4tc_EFFS_empty', l2=Chain(name='L2_j50_jetNoCut', counter=774, seed='L1_J30_EMPTY'),
                                       ef=Chain(counter=776), l2hypo=hypos.l2nocut_j45,
                                       efhypo = hypos.ef_j50_eta25_a4topo_EFFS),
    L2EFChain_jet_antikt_fullscan_topo('j50_eta25_a4tc_EFFS_firstempty', l2=Chain(name='L2_j50_jetNoCut', counter=774, seed='L1_J30_FIRSTEMPTY'),
                                       ef=Chain(counter=777), l2hypo=hypos.l2nocut_j45,
                                       efhypo = hypos.ef_j50_eta25_a4topo_EFFS),
    
    ]


class L2EFChain_jet_antikt_fullscan_topo_1vtx(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, group='RATE:SingleJet', tomerge=None):
        JetTemplate.__init__(self, sig, l2=l2, ef=ef)
        self.merge(tomerge).addGroups(group)
        self.addSequence('EF_FStracks', [fexes.OneVxFex, hypos.OneVxHypo], self.ef.chain_name).addSignature()
        self.addGroups('BW:Jets')

Jets += [
    L2EFChain_jet_antikt_fullscan_topo_1vtx('j10_a4tc_EFFS_1vx',
                                            l2=Chain(name='L2_rd0_filled_NoAlg', counter=623, seed='L1_RD0_FILLED'),
                                            ef=Chain(counter=927), tomerge=['EF_j10_a4tc_EFFS']
                                            ),
    
    L2EFChain_jet_antikt_fullscan_topo_1vtx('fj10_a4tc_EFFS_1vx',
                                            l2=Chain(name='L2_rd0_filled_NoAlg', counter=623, seed='L1_RD0_FILLED'),
                                            ef=Chain(counter=929), tomerge=['EF_fj10_a4tc_EFFS']
                                            ),
    ]

Jets += [

    L2EFChain_jet_antikt_fullscan_topo('j20_a4tc_EFFS_jetNoCut', l2=l2chains.j15, ef=Chain(counter=3551),
                                       l2hypo=hypos.l2nocut_j15, efhypo = hypos.efnocut_j20_a4topo_EFFS),  
    ]


#---------------------------------------------------
# AntiKt R=1.0 fullscan Topo
#---------------------------------------------------
Jets += [
    L2EFChain_jet_antikt_fullscan_topo('j240_a10tc_EFFS',
                                       l2=l2chains.j95,
                                       ef=Chain(counter=58),
                                       antikt=fexes.antikt10_topo, efhypo = hypos.ef_j240_a10topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j320_a10tc_EFFS',
                                       l2=l2chains.j95,
                                       ef=Chain(counter=59),
                                       antikt=fexes.antikt10_topo, efhypo = hypos.ef_j320_a10topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo('j425_a10tc_EFFS',
                                       l2=l2chains.j95,
                                       ef=Chain(counter=66),
                                       antikt=fexes.antikt10_topo, efhypo = hypos.ef_j425_a10topo_EFFS),
]

Jets += [

    L2EFChain_jet_antikt_fullscan_topo('j30_a4tc_EFFS_l2cleanph', l2=Chain(name='L2_j25_l2cleanph', counter=1031, seed='L1_J10'),
                                       ef=Chain(counter=1031), l2hypo=hypos.l2cleanph_j25,
                                       efhypo = hypos.ef_j30_a4topo_EFFS),


    L2EFChain_jet_unclean_fullscan('j100_LArNoiseBurst', l2=Chain(name='L2_j95_l2unclean', counter=459, seed='L1_J75'),
                                       ef=Chain(counter=459), l2hypo=hypos.l2unclean_j95,
                                       efhypo = hypos.efunclean_j100),

    
    L2EFChain_jet_unclean_fullscan('j30_LArNoiseBurst_empty', l2=Chain(name='L2_j25_l2unclean', counter=460, seed='L1_J10_EMPTY'),
                                       ef=Chain(counter=460), l2hypo=hypos.l2unclean_j25,
                                       efhypo = hypos.efunclean_j30),

    
    L2EFChain_jet_unclean_fullscan('j55_LArNoiseBurst_firstempty', l2=Chain(name='L2_j50_l2unclean', counter=461, seed='L1_J30_FIRSTEMPTY'),
                                       ef=Chain(counter=461), l2hypo=hypos.l2unclean_j50,
                                   efhypo = hypos.efunclean_j55),


    
    L2EFChain_jet_unclean_fullscan_NoAlg('j55_LArNoiseBurst_empty',l2=None,
                                         ef=Chain(counter=462, seed='L2_L1J30_empty_NoAlg'),
                                         efhypo = hypos.efunclean_j55),



    #new for HI run, pilot run
    L2EFChain_jet_unclean_fullscan_NoAlg('j10_u0uchad_LArNoiseBurst',l2=None,
                                         ef=Chain(counter=320, seed='L2_L1J5_NoAlg'),
                                         efhypo = hypos.efunclean_j10),

    L2EFChain_jet_unclean_fullscan_NoAlg('j10_u0uchad_firstempty_LArNoiseBurst',l2=None,
                                         ef=Chain(counter=322, seed='L2_L1J5_firstempty_NoAlg'),
                                         efhypo = hypos.efunclean_j10),
    
    L2EFChain_jet_unclean_fullscan_NoAlg('j10_u0uchad_empty_LArNoiseBurst',l2=None,
                                         ef=Chain(counter=323, seed='L2_L1J5_empty_NoAlg'),
                                         efhypo = hypos.efunclean_j10),


#    #new for HI run, pilot run
#    L2EFChain_jet_unclean_fullscan_NoAlg('j20_u0uchad_LArNoiseBurst',l2=None,
#                                         ef=Chain(counter=324, seed='L2_L1J10_NoAlg'),
#                                         efhypo = hypos.efunclean_j20),
#
#    L2EFChain_jet_unclean_fullscan_NoAlg('j20_u0uchad_firstempty_LArNoiseBurst',l2=None,
#                                         ef=Chain(counter=325, seed='L2_L1J10_firstempty_NoAlg'),
#                                         efhypo = hypos.efunclean_j20),
#    
#    L2EFChain_jet_unclean_fullscan_NoAlg('j20_u0uchad_empty_LArNoiseBurst',l2=None,
#                                         ef=Chain(counter=326, seed='L2_L1J10_empty_NoAlg'),
#                                         efhypo = hypos.efunclean_j20),
#



    L2EFChain_jet_antikt_fullscan_topo('j55_a4tc_EFFS_l2cleancons',l2=Chain(name='L2_j50_l2cleancons', counter=1034, seed='L1_J30'),
                                       ef=Chain(counter=1034), l2hypo=hypos.l2cleancons_j50,
                                       efhypo = hypos.ef_j55_a4topo_EFFS),  
    

    L2EFChain_jet_antikt_fullscan_topo('j75_a4tc_EFFS_l2cleancons',l2=Chain(name='L2_j70_l2cleancons', counter=1033, seed='L1_J50'),
                                       ef=Chain(counter=1033), l2hypo=hypos.l2cleancons_j70,
                                       efhypo = hypos.ef_j75_a4topo_EFFS),  


    L2EFChain_jet_antikt_fullscan_topo('j240_a4tc_EFFS_l2cleanph', l2=Chain(name='L2_j95_l2cleanph', counter=1035, seed='L1_J75'),
                                       ef=Chain(counter=1035), l2hypo=hypos.l2cleanph_j95,
                                       efhypo = hypos.ef_j240_a4topo_EFFS),
    
    
    L2EFChain_jet_antikt_fullscan_topo('fj30_a4tc_EFFS_l2cleanph', l2=Chain(name='L2_fj25_l2cleanph', counter=1032, seed='L1_FJ10'),
                                       ef=Chain(counter=1032), l2hypo=hypos.l2cleanph_fj25,
                                       efhypo = hypos.ef_fj30_a4topo_EFFS,
                                       groups = ["RATE:SingleForwardJet", "BW:Jets"], threshold='JF10'),
    
    ]


#############################################################################
# cosmics stuff
#HI jets
Jets += [
    #tower jets        
    L2EFChain_jet_antikt_fullscan_rndm('j15_a4_EFFS_L1ZDC', l2=None,
                                       ef=Chain(counter=972, seed=minbias_hi_seed),
                                       efhypo = hypos.ef_j15_a4_EFFS),
        
    L2EFChain_jet_antikt_fullscan_rndm('j20_a4_EFFS_L1ZDC', l2=None,
                                       ef=Chain(counter=1001, seed=minbias_hi_seed),
                                       efhypo = hypos.ef_j20_a4_EFFS),
    
    L2EFChain_jet_antikt_fullscan_rndm('j40_a4_EFFS_L1ZDC', l2=None,
                                       ef=Chain(counter=1002, seed=minbias_hi_seed),
                                       efhypo = hypos.ef_j40_a4_EFFS),
    
    L2EFChain_jet_antikt_fullscan_rndm('j75_a4_EFFS_L1ZDC', l2=None,
                                       ef=Chain(counter=1012, seed=minbias_hi_seed),
                                       efhypo = hypos.ef_j75_a4_EFFS),
    
    #Hi 0.2 cone jets
    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=971, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS_L1TE10', l2=None,
                                     ef=Chain(counter=805, seed='L2_L1TE10_NoAlg'),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS_L1TE20', l2=None,
                                     ef=Chain(counter=806, seed='L2_L1TE20_NoAlg'),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS_L1J10', l2=None,
                                     ef=Chain(counter=734, seed='L2_L1J10_NoAlg'),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1003, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS_L1TE10', l2=None,
                                     ef=Chain(counter=807, seed='L2_L1TE10_NoAlg'),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS_L1TE20', l2=None,
                                     ef=Chain(counter=808, seed='L2_L1TE20_NoAlg'),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS_L1J10', l2=None,
                                     ef=Chain(counter=735, seed='L2_L1J10_NoAlg'),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j25_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1004, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j25_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j25_a2hi_EFFS_L1TE10', l2=None,
                                     ef=Chain(counter=823, seed='L2_L1TE10_NoAlg'),
                                     efhypo = hypos.ef_j25_a2hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j25_a2hi_EFFS_L1TE20', l2=None,
                                     ef=Chain(counter=824, seed='L2_L1TE20_NoAlg'),
                                     efhypo = hypos.ef_j25_a2hi_EFFS),   

    L2EFChain_jet_antikt_fullscan_hi('j25_a2hi_EFFS_L1J10', l2=None,
                                     ef=Chain(counter=820, seed='L2_L1J10_NoAlg'),
                                     efhypo = hypos.ef_j25_a2hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j30_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1005, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j30_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j40_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1006, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j40_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j50_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1007, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j50_a2hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j75_a2hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1008, seed=minbias_hi_seed),
                                     efhypo = hypos.ef_j75_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS', l2=None,
                                     ef=Chain(counter=918, seed='L2_rd0_filled_NoAlg'),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    
    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS', l2=None,
                                     ef=Chain(counter=921, seed='L2_rd0_filled_NoAlg'),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_topo_rndm('j15_a2tc_EFFS', l2=None,
                                            ef=Chain(counter=888, seed='L2_rd0_filled_NoAlg'),
                                            antikt=fexes.antikt2_topo,
                                            efhypo = hypos.ef_j15_a2tc_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topo_rndm('j20_a2tc_EFFS', l2=None,
                                            ef=Chain(counter=889, seed='L2_rd0_filled_NoAlg'),
                                            antikt=fexes.antikt2_topo,
                                            efhypo = hypos.ef_j20_a2tc_EFFS),            
    #pPb 0.2 cone jets
    L2EFChain_jet_antikt_fullscan_hi('j15_a2hi_EFFS_L1MBTS', l2=None,
                                     ef=Chain(counter=870, seed='L2_L1MBTS_2_NoAlg'),
                                     efhypo = hypos.ef_j15_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a2hi_EFFS_L1MBTS', l2=None,
                                     ef=Chain(counter=871, seed='L2_L1MBTS_2_NoAlg'),
                                     efhypo = hypos.ef_j20_a2hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j15_a4hi_EFFS_L1MBTS', l2=None,
                                     ef=Chain(counter=872, seed='L2_L1MBTS_2_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j15_a4hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a4hi_EFFS_L1MBTS', l2=None,
                                     ef=Chain(counter=873, seed='L2_L1MBTS_2_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j20_a4hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j15_a4hi_EFFS_L1TE10', l2=None,
                                     ef=Chain(counter=876, seed='L2_L1TE10_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j15_a4hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j20_a4hi_EFFS_L1TE10', l2=None,
                                     ef=Chain(counter=877, seed='L2_L1TE10_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j20_a4hi_EFFS),
    
    # HI 0.4 cone jets
    L2EFChain_jet_antikt_fullscan_hi('j15_a4hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=970, seed=minbias_hi_seed),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j15_a4hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a4hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1013, seed=minbias_hi_seed),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j20_a4hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j20_a4hi_EFFS', l2=None,
                                     ef=Chain(counter=890, seed='L2_rd0_filled_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j20_a4hi_EFFS),    
    
    L2EFChain_jet_antikt_fullscan_hi('j75_a4hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1009, seed=minbias_hi_seed),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j75_a4hi_EFFS),

    L2EFChain_jet_antikt_fullscan_hi('j100_a4hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1010, seed=minbias_hi_seed),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j100_a4hi_EFFS),
    
    L2EFChain_jet_antikt_fullscan_hi('j150_a4hi_EFFS_L1ZDC', l2=None,
                                     ef=Chain(counter=1011, seed=minbias_hi_seed),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j150_a4hi_EFFS),

    
    L2EFChain_jet_antikt_fullscan_hi('j75_a4hi_EFFS_L1J10', l2=None,
                                     ef=Chain(counter=1019, seed='L2_L1J10_NoAlg'),
                                     antikt=fexes.antikt4hi, efhypo = hypos.ef_j75_a4hi_EFFS),
    
    #topo jets
    L2EFChain_jet_antikt_fullscan_topo_rndm('j20_a4tc_EFFS_L1ZDC', l2=None,
                                            ef=Chain(counter=1003, seed=minbias_hi_seed),
                                            efhypo = hypos.ef_j20_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo_rndm('j40_a4tc_EFFS_L1ZDC', l2=None,
                                            ef=Chain(counter=1004, seed=minbias_hi_seed),
                                            efhypo = hypos.ef_j40_a4topo_EFFS),
    ]




class L2EFChain_jet_antikt_fullscan_topo_had(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4_topo_had, efhypo=None, threshold=None, groups=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)

        # guess threshold
        if threshold == None:
            threshold = l2.seed.split('_')[1]

        # guess hypos
        if l2hypo == None:
            cut = l2.name.split('_')[1]
            l2hypo = getattr(hypos, 'l2_%s'%cut)
            assert l2hypo, 'impossible to deduce the L2 hypothesis algorithm'
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex_JESCalib, l2hypo], self.l2.chain_name).addSignature()

        # EF
        self.addSequence('',[fexes.roi_topo,
                             fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters') # not in a chain signature

        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']

        self.addGroups(*groups)
        pass

Jets += [
    
    L2EFChain_jet_antikt_fullscan_topo_had('j55_a4tchad_EFFS', l2=Chain(name='L2_j50_c4had_ROI', counter=896, seed='L1_J15'),
                                           ef=Chain(counter=896), l2hypo=hypos.l2_j50,
                                           efhypo = hypos.ef_j55_a4topo_EFFS),

    L2EFChain_jet_antikt_fullscan_topo_had('j145_a4tchad_EFFS', l2=Chain(name='L2_j140_c4had_ROI', counter=898, seed='L1_J75'),
                                           ef=Chain(counter=898), l2hypo=hypos.l2_j140,
                                           efhypo = hypos.ef_j145_a4topo_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topo_had('fj70_a4tchad_EFFS', l2=Chain(name='L2_fj65_c4had_ROI', counter=899, seed='L1_FJ30'),
                                           ef=Chain(counter=899), l2hypo=hypos.l2_fj65, threshold='JF30',
                                           efhypo = hypos.ef_fj70_a4topo_EFFS),
    ]


#############
# Fullscan TopoTowers
#############
class L2EFChain_jet_antikt_fullscan_topoTower(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, antikt=fexes.antikt4_topotower, efhypo=None, threshold=None, groups=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)
        
        # guess threshold
        if threshold == None:
            threshold = l2.seed.split('_')[1]
            
        # guess hypos
        if l2hypo == None:
            cut = l2.name.split('_')[1]
            l2hypo = getattr(hypos, 'l2_%s'%cut)
            assert l2hypo, 'impossible to deduce the L2 hypothesis algorithm'
        # build the sequences
        # L2
        self.addSequence(threshold, [fexes.l2fex, l2hypo], self.l2.chain_name).addSignature()
        
        # EF
        ##    self.addSequence('',[fexes.roi_topo,
        ##                         fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo, fexes.cell_cluster_mapper, fexes.tower_maker, fexes.topoTower_maker_fullcalo], 'EF_FSTopoTowerClusters') # not in a chain signature
        
        self.addSequence('EF_FSTopoClusters', [fexes.cell_cluster_mapper, fexes.tower_maker, fexes.topoTower_maker_fullcalo], 'EF_FSTopoTowerClusters')
        
        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']
            
        self.addGroups(*groups)
        pass

class L2EFChain_jet_antikt_fullscan_topoTower_rndm(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, antikt=fexes.antikt4_topotower, efhypo=None):
        ef.replace_if_default('name', 'EF_%s'%sig)
        JetTemplate.__init__(self, sig, l2, ef)
        
        # EF
        self.addSequence('EF_FSTopoClusters', [fexes.cell_cluster_mapper, fexes.tower_maker, fexes.topoTower_maker_fullcalo], 'EF_FSTopoTowerClusters')
        
        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()
        
        self.addGroups('RATE:SingleJet', 'BW:Jets')
        pass


Jets += [

    L2EFChain_jet_antikt_fullscan_topoTower_rndm('j15_a4tt_EFFS',
                                                 l2=None,
                                                 ef=Chain(counter=8002, seed='L2_rd0_filled_NoAlg'),
                                                 efhypo=hypos.ef_j15_a4topotower_EFFS
                                                 ),
    
    L2EFChain_jet_antikt_fullscan_topoTower('j30_a4tt_EFFS', l2=l2chains.j25, ef=Chain(counter=8004),
                                            efhypo = hypos.ef_j30_a4topotower_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topoTower('j100_a4tt_EFFS', l2=l2chains.j95, ef=Chain(counter=8005),
                                            efhypo = hypos.ef_j100_a4topotower_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topoTower('j240_a4tt_EFFS', l2=l2chains.j95, ef=Chain(counter=8006),
                                            efhypo = hypos.ef_j240_a4topotower_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topoTower('fj30_a4tt_EFFS', l2=l2chains.fj25, ef=Chain(counter=8007),
                                            efhypo = hypos.ef_fj30_a4topotower_EFFS),
    
    L2EFChain_jet_antikt_fullscan_topoTower('fj100_a4tt_EFFS', l2=l2chains.fj95, ef=Chain(counter=8008),
                                            efhypo = hypos.ef_fj100_a4topotower_EFFS),
    ]


############# L1.5
class L15L2EFChain_jet_full_scan(JetTemplate):
    """
    full scan Jet Chains including L1.5 stages
    
    M.Tamsett
    
    """
    def __init__(self, sig, l15, l15hypo, l2fex=None, l2hypo=None, ef=DeduceChainConfig, efhypo=None, l1threshold=None, grid_type="FastJet_JE"):
        # generate EF chain name to be identical as the signature name (but only if not given)
        JetTemplate.__init__(self, sig, l15, ef)
        print "\n====================="
        print "   L1.5 full scan jet trigger "
        print "======================\n"
        
        print " In L15L2EFChain_jet_full_scan"
        print " signature:      ",sig
        print " l15 chain:      ",l15.name
        print " l15 hypo:       ",l15hypo.getName()
        if l2fex:  print " l2 fex:         ",l2fex.name
        if l2hypo: print " l2 hypo:        ",l2hypo.getName()
        if ef:     print " ef fex:         ",ef.name
        if efhypo: print " ef hypo:        ",efhypo.getName()
        print " l1 threshold: ",l1threshold
        
        grid_type_fex_dictionary = {"FastJet_JE":       [fexes.t2l1_je_unpacking_fex,    fexes.t2_fastjet_je_fex],
                                    "FastJet_JE_TC":    [fexes.t2l1_je_unpacking_tc_fex, fexes.t2_fastjet_je_tc_fex],
                                    "FastJet_JE_JES":   [fexes.t2l1_je_unpacking_fex,    fexes.t2_fastjet_je_jes_fex],
                                    "FastJet_JE_perJet":[fexes.t2l1_je_unpacking_fex,    fexes.t2_fastjet_je_multiple_output_tes_fex],
                                    "FastJet_JE_pu_sub":[fexes.t2l1_je_unpacking_fex,    fexes.t2_fastjet_je_pileup_subtraction_fex],
                                    "FastJet_TT":       [fexes.t2l1_tt_unpacking_fex,    fexes.t2_fastjet_tt_fex],
                                    "FastJet_TT_TC":    [fexes.t2l1_tt_unpacking_tc_fex, fexes.t2_fastjet_tt_tc_fex],
                                    "FastJet_TT_JES":   [fexes.t2l1_tt_unpacking_fex,    fexes.t2_fastjet_tt_jes_fex],
                                    "FastJet_TT_perJet":[fexes.t2l1_tt_unpacking_fex,    fexes.t2_fastjet_tt_multiple_output_tes_fex],
                                    "FastJet_TT_pu_sub":[fexes.t2l1_tt_unpacking_fex,    fexes.t2_fastjet_tt_pileup_subtraction_fex],
                                    }
        # Check we've got a valid grid type
        assert grid_type in grid_type_fex_dictionary.keys(), "Failure due to bad configuration, unknown jet type %s given"%grid_type
        
        #print " l15 unpacking:   %s"%grid_type_fex_dictionary[grid_type][0].getName()
        #print " l15 jet finding: %s"%grid_type_fex_dictionary[grid_type][1].getName()
        
        # Check we've been given the l1threshold and the l15 hypothesis
        assert l1threshold != None, "l1 threshold not given, this cannont be assumed from the signature name at the moment"
        assert l15hypo, "l1.5 hypo not given, this cannot be assumed from signature names at the moment"
        
        # now it is time to construct sequences
        br = self.l2.chain_name.split('_')
        l15te = 'L2_L15_%s'%''.join(b for b in br[1:] if b not in helpers.te_no_keys)
        print " l15te:          ",l15te

        l15unpacking = 'L2_%s'%grid_type_fex_dictionary[grid_type][0].getName()
        print "l15unpacking:    ",l15unpacking
        l15jetfinding = 'L2_%s'%grid_type_fex_dictionary[grid_type][1].getName()
        print "l15jetfinding    ",l15jetfinding
        
        # to avoid repeating the same chains over and over we use an output trigger element whose name is given by the
        # name of the FEX. These should be configured such that each type of chain has a unique name
        
        self.addSequence(l1threshold, [grid_type_fex_dictionary[grid_type][0]], l15unpacking) # unpacking
        self.continueSequence([grid_type_fex_dictionary[grid_type][1]], l15jetfinding)    # jet finding
        self.continueSequence([l15hypo],l15te).addSignature()                                                                # hypothesis
        print ' - l15 sequence added'
        
        if l2fex and l2hypo:
            l2te = 'L2_%s'%''.join(b for b in br[1:] if b not in helpers.te_no_keys)
            print " l2te:           ",l2te
            self.continueSequence([l2fex,l2hypo],l2te).addSignature()
            print ' - l2 sequence added'
            
        self.addGroups('BW:Jets')
        
        # that's it!
        print " done\n\n"
        pass

Jets +=[
#    L15L2EFChain_jet_full_scan('NoCut_a4JE_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4JE_L15FS_NoEF', counter=715, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4JEcal_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4JEcal_L15FS_NoEF', counter=6790, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE_TC"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4JEhad_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4JEhad_L15FS_NoEF', counter=6789, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE_JES"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4JE_L15FS_L14J10_NoEF',
#                               l15=Chain(name='L2_NoCut_a4JE_L15FS_L14J10_NoEF', counter=720, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="J10",
#                               grid_type="FastJet_JE"
#                               ),
       
#    L15L2EFChain_jet_full_scan('j0_a4JE_L15FS_j0_NoEF',
#                               l15=Chain(name='L2_j0_a4JE_L15FS_j0_NoEF', counter=7260, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_j0,
#                               l2fex=fexes.l2fex_l15,
#                               l2hypo=hypos.l2_j0,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE_perJet"
#                               ),
    
#    L15L2EFChain_jet_full_scan('j0_a4JE_L15FS_j0_L14J10_NoEF',
#                               l15=Chain(name='L2_j0_a4JE_L15FS_j0_L14J10_NoEF', counter=7240, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_j0,
#                               l2fex=fexes.l2fex_l15,
#                               l2hypo=hypos.l2_j0,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="J10",
#                               grid_type="FastJet_JE_perJet"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4JE_pu_sub_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4JE_pu_sub_L15FS_NoEF', counter=7270, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE_pu_sub"
#                               ),

    #----------------
    
#    L15L2EFChain_jet_full_scan('NoCut_a4TT_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4TT_L15FS_NoEF', counter=724, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4TTcal_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4TTcal_L15FS_NoEF', counter=7022, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT_TC"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4TThad_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4TThad_L15FS_NoEF', counter=8029, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT_JES"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4TT_L15FS_L14J10_NoEF',
#                               l15=Chain(name='L2_NoCut_a4TT_L15FS_L14J10_NoEF', counter=722, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="J10",
#                               grid_type="FastJet_TT"
#                               ),
    
#    L15L2EFChain_jet_full_scan('j0_a4TT_L15FS_j0_NoEF',
#                               l15=Chain(name='L2_j0_a4TT_L15FS_j0_NoEF', counter=7026, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_j0,
#                               l2fex=fexes.l2fex_l15,
#                               l2hypo=hypos.l2_j0,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT_perJet"
#                               ),
    
#    L15L2EFChain_jet_full_scan('j0_a4TT_L15FS_j0_L14J10_NoEF',
#                               l15=Chain(name='L2_j0_a4TT_L15FS_j0_L14J10_NoEF', counter=7024, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_j0,
#                               l2fex=fexes.l2fex_l15,
#                               l2hypo=hypos.l2_j0,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="J10",
#                               grid_type="FastJet_TT_perJet"
#                               ),
    
#    L15L2EFChain_jet_full_scan('NoCut_a4TT_pu_sub_L15FS_NoEF',
#                               l15=Chain(name='L2_NoCut_a4TT_pu_sub_L15FS_NoEF', counter=7027, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_NoCut,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT_pu_sub"
#                               ),


        # HT
#    L15L2EFChain_jet_full_scan('L2_ht150_a4JE_L15FS_NoEF',
#                               l15=Chain(name='L2_ht150_a4JE_L15FS_NoEF', counter=7028, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_ht150,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE"
#                               ),

#    L15L2EFChain_jet_full_scan('L2_ht150_a4TT_L15FS_NoEF',
#                               l15=Chain(name='L2_ht150_a4TT_L15FS_NoEF', counter=7029, seed='L1_RD0_FILLED'),
#                               l15hypo=hypos.l2_fullscan_ht150,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT"
#                               ),

#    L15L2EFChain_jet_full_scan('L2_ht150_a4JE_L15FS_L14J10_NoEF',
#                               l15=Chain(name='L2_ht150_a4JE_L15FS_L14J10_NoEF', counter=7030, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_ht150,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_JE"
#                               ),

#    L15L2EFChain_jet_full_scan('L2_ht150_a4TT_L15FS_L14J10_NoEF',
#                               l15=Chain(name='L2_ht150_a4TT_L15FS_L14J10_NoEF', counter=7031, seed='L1_4J10'),
#                               l15hypo=hypos.l2_fullscan_ht150,
#                               efhypo=hypos.efnocut_j5,
#                               l1threshold="",
#                               grid_type="FastJet_TT"
#                               ),

    ]
  ##### end of L1.5


