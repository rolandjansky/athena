# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### ********************************************************************
### COMMENTS on MultiJets:
### A comment on L2 and EF naming:
### EF has the number of jets in the configuration
### Eg. EF_j75_j30_a4_EFFS --> 2 jets
### L2 follows the L1 naming
### Eg. L2_j70_2j25 --> 2 jets, of 25GeV out of which one of 70.
### IMPORTANT NOTE:
### The configuration is made by merging *one* L2_j25 and *one* L2_j70.
### Another IMPORTANT NOTE:
### At EF, for FS chains, merging doesn't work,
### a hypo that requires the exact number of jets should be made.
### ********************************************************************


# definition of multijet signatures, using single jets from JetDef.py
from TriggerMenuPython.JetDef_v2v3 import JetTemplate
from TriggerMenuPython.ChainTemplate import *


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags


fexes = FreeStore()
hypos = FreeStore()

from string import atoi
from AthenaCommon.Logging import logging        

#from TriggerMenuPython.Combined import CombinedChain2
from TriggerMenuPython.JetDef_v2v3 import fexes as single_fexes


# di-jet delta eta 
from AthenaCommon.SystemOfUnits import GeV
from TrigJetHypo.TrigJetDEtaFexConfig  import *
from TrigJetHypo.TrigJetDEtaHypoConfig import *
from TrigT2CaloJet.TrigT2CaloJetConfig import T2AllRoiUnpacking_test
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_CC



fexes.l2_j15_deta3p5 = L2DEtaFex('L2DEtaFex_j15_deta3p5', 3.5)
fexes.l2_j15_deta5   = L2DEtaFex('L2DEtaFex_j15_deta5', 5)
fexes.l2_j25_deta3p2 = L2DEtaFex('L2DEtaFex_j25_deta3p2', 3.2)
fexes.l2_j25_deta3p5 = L2DEtaFex('L2DEtaFex_j25_deta3p5', 3.5)
fexes.l2_j25_deta4p6 = L2DEtaFex('L2DEtaFex_j25_deta4p6', 4.6)
fexes.l2_j30_deta3p5 = L2DEtaFex('L2DEtaFex_j30_deta3p5', 3.5)
fexes.l2_2j25_j50_deta3p2 = L2DEtaFex('L2DEtaFex_2j25_j50_deta3p2', 3.2)
fexes.l2_2j25_j50_deta4p6 = L2DEtaFex('L2DEtaFex_2j25_j50_deta4p6', 4.6)
fexes.l2_2j25_j70_deta3p2 = L2DEtaFex('L2DEtaFex_2j25_j70_deta3p2', 3.2)
fexes.l2_2j25_j70_deta4p6 = L2DEtaFex('L2DEtaFex_2j25_j70_deta4p6', 4.6)
fexes.l2_2j25_j95_deta3p2 = L2DEtaFex('L2DEtaFex_2j25_j95_deta3p2', 3.2)
fexes.l2_2j25_j95_deta4p6 = L2DEtaFex('L2DEtaFex_2j25_j95_deta4p6', 4.6)
fexes.l2fex_unpacking_allroi = T2AllRoiUnpacking_test()
fexes.t2_fastjet_cc_fex      = T2CaloFastJet_CC()


hypos.l2_j15_deta3p5 = L2DEtaHypo('L2DEtaHypo_j15_deta3p5', 1) 
hypos.l2_j15_deta5   = L2DEtaHypo('L2DEtaHypo_j15_deta5', 1)  
hypos.l2_j25_deta3p2 = L2DEtaHypo('L2DEtaHypo_j25_deta3p2', 1)  
hypos.l2_j25_deta3p5 = L2DEtaHypo('L2DEtaHypo_j25_deta3p5', 1)  
hypos.l2_j25_deta4p6 = L2DEtaHypo('L2DEtaHypo_j25_deta4p6', 1)  
hypos.l2_j30_deta3p5 = L2DEtaHypo('L2DEtaHypo_j30_deta3p5', 1)  
hypos.l2_2j25_j50_deta3p2 = L2DEtaHypo('L2DEtaHypo_2j25_j50_deta3p2', 1)  
hypos.l2_2j25_j50_deta4p6 = L2DEtaHypo('L2DEtaHypo_2j25_j50_deta4p6', 1)  
hypos.l2_2j25_j70_deta3p2 = L2DEtaHypo('L2DEtaHypo_2j25_j70_deta3p2', 1)  
hypos.l2_2j25_j70_deta4p6 = L2DEtaHypo('L2DEtaHypo_2j25_j70_deta4p6', 1)  
hypos.l2_2j25_j95_deta3p2 = L2DEtaHypo('L2DEtaHypo_2j25_j95_deta3p2', 1)  
hypos.l2_2j25_j95_deta4p6 = L2DEtaHypo('L2DEtaHypo_2j25_j95_deta4p6', 1)  

fexes.ef_j20_deta3p5 = EFDEtaFex('EFDEtaFex_j20_deta3p5', 3.5)
fexes.ef_j20_deta5   = EFDEtaFex('EFDEtaFex_j20_deta5', 5)
fexes.ef_j25_deta3p5 = EFDEtaFex('EFDEtaFex_j25_deta3p5', 3.5)
fexes.ef_j30_deta3p5 = EFDEtaFex('EFDEtaFex_j30_deta3p5', 3.5)
fexes.ef_j30_deta5   = EFDEtaFex('EFDEtaFex_j30_deta5', 5)
fexes.ef_j35_deta3p5 = EFDEtaFex('EFDEtaFex_j35_deta3p5', 3.5)
fexes.ef_j55_j30_deta3p5 = EFDEtaFex('EFDEtaFex_j55_j30_deta3p5', 3.5)
fexes.ef_j55_j30_deta5   = EFDEtaFex('EFDEtaFex_j55_j30_deta5', 5)
fexes.ef_j75_j30_deta3p5 = EFDEtaFex('EFDEtaFex_j75_j30_deta3p5', 3.5)
fexes.ef_j75_j30_deta5   = EFDEtaFex('EFDEtaFex_j75_j30_deta5', 5)
fexes.ef_j100_j30_deta3p5 = EFDEtaFex('EFDEtaFex_j100_j30_deta3p5', 3.5)
fexes.ef_j100_j30_deta5   = EFDEtaFex('EFDEtaFex_j100_j30_deta5', 5)

fexes.ef_fj30_deta5   = EFDEtaFex('EFDEtaFex_fj30_deta5', 5)
fexes.ef_j30_fj30_deta5   = EFDEtaFex('EFDEtaFex_j30_fj30_deta5', 5)
fexes.ef_fj55_deta5   = EFDEtaFex('EFDEtaFex_fj55_deta5', 5)
fexes.ef_j55_fj55_deta5   = EFDEtaFex('EFDEtaFex_j55_fj55_deta5', 5)
fexes.ef_fj75_deta5   = EFDEtaFex('EFDEtaFex_fj75_deta5', 5)
fexes.ef_j75_fj75_deta5   = EFDEtaFex('EFDEtaFex_j75_fj75_deta5', 5)
fexes.ef_fj100_deta5   = EFDEtaFex('EFDEtaFex_fj100_deta5', 5)
fexes.ef_j100_deta3p5 = EFDEtaFex('EFDEtaFex_j100_deta3p5', 3.5)
fexes.ef_j135_deta3p5 = EFDEtaFex('EFDEtaFex_j135_deta3p5', 3.5)
fexes.ef_j180_deta3p5 = EFDEtaFex('EFDEtaFex_j180_deta3p5', 3.5)
fexes.ef_j240_deta3p5 = EFDEtaFex('EFDEtaFex_j240_deta3p5', 3.5)


hypos.ef_j20_deta3p5 = EFDEtaHypo('EFDEtaHypo_j20_deta3p5', 1)
hypos.ef_j20_deta5   = EFDEtaHypo('EFDEtaHypo_j20_deta5', 1)
hypos.ef_j25_deta3p5 = EFDEtaHypo('EFDEtaHypo_j25_deta3p5', 1)
hypos.ef_j30_deta3p5 = EFDEtaHypo('EFDEtaHypo_j30_deta3p5', 1)
hypos.ef_j30_deta5   = EFDEtaHypo('EFDEtaHypo_j30_deta5', 1)
hypos.ef_j35_deta3p5 = EFDEtaHypo('EFDEtaHypo_j35_deta3p5', 1)
hypos.ef_j55_j30_deta3p5 = EFDEtaHypo('EFDEtaHypo_j55_j30_deta3p5', 1)
hypos.ef_j55_j30_deta5   = EFDEtaHypo('EFDEtaHypo_j55_j30_deta5', 1)
hypos.ef_j75_j30_deta3p5 = EFDEtaHypo('EFDEtaHypo_j75_j30_deta3p5', 1)
hypos.ef_j75_j30_deta5   = EFDEtaHypo('EFDEtaHypo_j75_j30_deta5', 1)
hypos.ef_j100_j30_deta3p5 = EFDEtaHypo('EFDEtaHypo_j100_j30_deta3p5', 1)
hypos.ef_j100_j30_deta5   = EFDEtaHypo('EFDEtaHypo_j100_j30_deta5', 1)

hypos.ef_fj30_deta5   = EFDEtaHypo('EFDEtaHypo_fj30_deta5', 1)
hypos.ef_j30_fj30_deta5   = EFDEtaHypo('EFDEtaHypo_j30_fj30_deta5', 1)
hypos.ef_fj55_deta5   = EFDEtaHypo('EFDEtaHypo_fj55_deta5', 1)
hypos.ef_j55_fj55_deta5   = EFDEtaHypo('EFDEtaHypo_j55_fj55_deta5', 1)
hypos.ef_fj75_deta5   = EFDEtaHypo('EFDEtaHypo_fj75_deta5', 1)
hypos.ef_j75_fj75_deta5   = EFDEtaHypo('EFDEtaHypo_j75_fj75_deta5', 1)
hypos.ef_fj100_deta5   = EFDEtaHypo('EFDEtaHypo_fj100_deta5', 1)
hypos.ef_j100_deta3p5   = EFDEtaHypo('EFDEtaHypo_j100_deta3p5', 1)
hypos.ef_j135_deta3p5   = EFDEtaHypo('EFDEtaHypo_j135_deta3p5', 1)
hypos.ef_j180_deta3p5   = EFDEtaHypo('EFDEtaHypo_j180_deta3p5', 1)
hypos.ef_j240_deta3p5   = EFDEtaHypo('EFDEtaHypo_j240_deta3p5', 1)

hypos.efnocut_j20_deta3p5 = EFDEtaHypo_noJetCut('EFDEtaHypo_j20_deta3p5_noJetCut', 1)
hypos.efnocut_j20_deta5   = EFDEtaHypo_noJetCut('EFDEtaHypo_j20_deta5_noJetCut', 1)
hypos.efnocut_j25_deta3p5 = EFDEtaHypo_noJetCut('EFDEtaHypo_j25_deta3p5_noJetCut', 1)
hypos.efnocut_j30_deta3p5 = EFDEtaHypo_noJetCut('EFDEtaHypo_j30_deta3p5_noJetCut', 1)
hypos.efnocut_j35_deta3p5 = EFDEtaHypo_noJetCut('EFDEtaHypo_j35_deta3p5_noJetCut', 1)


from TrigJetHypo.TrigEFJetMctHypoConfig import *
hypos.EFLeadingMct100 = EFJetLeadingMct100()
hypos.EFLeadingMct150 = EFJetLeadingMct150()
hypos.EFAnyMct150 = EFJetAnyMct150()
hypos.EFAnyMct175 = EFJetAnyMct175()


from TrigJetHypo.TrigEFHTHypoConfig import *
hypos.EFHT200 = EFHT("EFHT200",HT_cut=200*GeV)
hypos.EFHT300 = EFHT("EFHT300",HT_cut=300*GeV)
hypos.EFHT350 = EFHT("EFHT350",HT_cut=350*GeV)
hypos.EFHT400 = EFHT("EFHT400",HT_cut=400*GeV)
hypos.EFHT500 = EFHT("EFHT500",HT_cut=500*GeV)

from TrigJetHypo.TrigEFJetDphiHypoConfig import *
hypos.EFDphi04 = EFJetDphi04()

from math import pi

from TrigJetHypo.TrigDijetComboConfig import L2DijetCombo
from TrigJetHypo.TrigJetDphiHypoConfig import L2JetDphiHypo

thresholds = ['2j25_j45', '2j25_j70', '2j30_j90', '2j25_j95']
fexes.update(dict([ ('l2_dphi_%s'%t, L2DijetCombo('L2JetCombo_%s'%t)) for t in thresholds]))

thresholds = ['2j25_j45', '2j25_j70', '2j30_j90']
hypos.update(dict([ ('l2_dphi_%s_dphi03'%t, L2JetDphiHypo('L2dPhiHypo_%s_dphi03'%t, pi-0.3)) for t in thresholds]))

thresholds = ['2j25_j70', '2j25_j95']
hypos.update(dict([ ('l2_dphi_%s_dphi04'%t, L2JetDphiHypo('L2dPhiHypo_%s_dphi04'%t, pi-0.4)) for t in thresholds]))


from TrigJetHypo.TrigJetMctHypoConfig import L2JetMctHypo
from TrigJetHypo.TrigDijetAllTEConfig import L2DijetAllTE
thresholds = ['2j30_anymct100', '2j25_j45_anymct150', '2j25_j45_anymct100', '2j30_anymct150', '2j30_anymct175', '2j25_j45_anymct175','2j25_j70_anymct100', '2j25_j70_anymct150', '2j25_j70_anymct175', '2j25_j70_anymct200', '2j40_anymct100', '2j50_anymct100']
# last 3 digits are the cut in GeV
hypos.update(dict([ ('l2_%s'%t, L2JetMctHypo('L2MctHypo_%s'%t, int(t[-3:])*GeV )) for t in thresholds]))

thresholds = ['2j30_leadingmct100', '2j25_j45_leadingmct150', '2j25_j45_leadingmct100', '2j30_leadingmct150', '2j30_leadingmct175', '2j25_j45_leadingmct175','2j25_j70_leadingmct100','2j25_j70_leadingmct150', '2j25_j70_leadingmct175', '2j25_j70_leadingmct200', '2j45_leadingmct100', '2j55_leadingmct100']
hypos.update(dict([ ('l2_%s'%t, L2JetMctHypo('L2MctHypo_%s'%t, int(t[-3:])*GeV )) for t in thresholds]))

thresholds = ['2j30', '2j25', '2j40', '2j50', '2j55']
fexes.update(dict([ ('leadingmct_%s'%t, L2DijetAllTE('L2JetLeading_%s'%t)) for t in thresholds]))
fexes.update(dict([ ('anymct_%s'%t, L2DijetCombo('L2JetAny_%s'%t)) for t in thresholds]))


from TrigJetHypo.TrigJetHypoConfig import EFCentFullScanMultiJetHypo,EFFwdFullScanMultiJetHypo
##hypos.ef_2j30_a4_EFFS  = EFFullScanMultiJetHypo("EFjetHypo_2j30_a4_EFFS", ef_thr=30*GeV, multiplicity=2)
##hypos.ef_2j40_a4_EFFS  = EFFullScanMultiJetHypo("EFjetHypo_2j40_a4_EFFS", ef_thr=40*GeV, multiplicity=2)
##hypos.ef_2j45_a4_EFFS  = EFFullScanMultiJetHypo("EFjetHypo_2j45_a4_EFFS", ef_thr=45*GeV, multiplicity=2)
##hypos.ef_2j135_a4_EFFS = EFFullScanMultiJetHypo("EFjetHypo_2j135_a4_EFFS",ef_thr=135*GeV, multiplicity=2)

hypos.ef_2j10_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j10_a4topo_EFFS",ef_thr=10*GeV, multiplicity=2)
hypos.ef_2j20_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j20_a4topo_EFFS",ef_thr=20*GeV, multiplicity=2)
hypos.ef_2j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=2)
hypos.ef_2j40_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j40_a4topo_EFFS",ef_thr=40*GeV, multiplicity=2)
hypos.ef_2j45_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j45_a4topo_EFFS",ef_thr=45*GeV, multiplicity=2)
hypos.ef_2j55_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j55_a4topo_EFFS",ef_thr=55*GeV, multiplicity=2)
hypos.ef_2j75_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j75_a4topo_EFFS",ef_thr=75*GeV, multiplicity=2)
hypos.ef_2j100_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j100_a4topo_EFFS",ef_thr=100*GeV, multiplicity=2)
hypos.ef_2j135_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j135_a4topo_EFFS",ef_thr=135*GeV, multiplicity=2)
hypos.ef_2j180_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j180_a4topo_EFFS",ef_thr=180*GeV, multiplicity=2)
hypos.ef_2j240_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_2j240_a4topo_EFFS",ef_thr=240*GeV, multiplicity=2)

hypos.ef_3j10_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j10_a4topo_EFFS",ef_thr=10*GeV, multiplicity=3)
hypos.ef_3j15_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j15_a4topo_EFFS",ef_thr=15*GeV, multiplicity=3)
hypos.ef_3j20_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j20_a4topo_EFFS",ef_thr=20*GeV, multiplicity=3)
hypos.ef_3j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=3)
hypos.ef_3j35_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j35_a4topo_EFFS",ef_thr=35*GeV, multiplicity=3)
hypos.ef_3j40_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j40_a4topo_EFFS",ef_thr=40*GeV, multiplicity=3)
hypos.ef_3j45_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j45_a4topo_EFFS",ef_thr=45*GeV, multiplicity=3)
hypos.ef_3j75_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_3j75_a4topo_EFFS",ef_thr=75*GeV, multiplicity=3)
hypos.ef_3j100_a4topo_EFFS = EFCentFullScanMultiJetHypo("EFjetHypo_3j100_a4topo_EFFS",ef_thr=100*GeV, multiplicity=3)

hypos.ef_4j10_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j10_a4topo_EFFS",ef_thr=10*GeV, multiplicity=4)
hypos.ef_4j15_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j15_a4topo_EFFS",ef_thr=15*GeV, multiplicity=4)
hypos.ef_4j20_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j20_a4topo_EFFS",ef_thr=20*GeV, multiplicity=4)
hypos.ef_4j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=4)
hypos.ef_4j40_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j40_a4topo_EFFS",ef_thr=40*GeV, multiplicity=4)
hypos.ef_4j45_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j45_a4topo_EFFS",ef_thr=45*GeV, multiplicity=4)
hypos.ef_4j55_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j55_a4topo_EFFS",ef_thr=55*GeV, multiplicity=4)
hypos.ef_4j60_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_4j60_a4topo_EFFS",ef_thr=60*GeV, multiplicity=4)

hypos.ef_5j10_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_5j10_a4topo_EFFS",ef_thr=10*GeV, multiplicity=5)
hypos.ef_5j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_5j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=5)
hypos.ef_5j40_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_5j40_a4topo_EFFS",ef_thr=40*GeV, multiplicity=5)
hypos.ef_5j45_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_5j45_a4topo_EFFS",ef_thr=45*GeV, multiplicity=5)

hypos.ef_6j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_6j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=6)
hypos.ef_6j40_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_6j40_a4topo_EFFS",ef_thr=40*GeV, multiplicity=6)
hypos.ef_6j45_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_6j45_a4topo_EFFS",ef_thr=45*GeV, multiplicity=6)

hypos.ef_7j30_a4topo_EFFS  = EFCentFullScanMultiJetHypo("EFjetHypo_7j30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=7)

##hypos.ef_3j45_a4_EFFS = EFFullScanMultiJetHypo("EFjetHypo_3j45_a4_EFFS",ef_thr=45*GeV, multiplicity=3)
##hypos.ef_3j55_a4_EFFS = EFFullScanMultiJetHypo("EFjetHypo_3j55_a4_EFFS",ef_thr=55*GeV, multiplicity=3)

hypos.ef_3j30_a6topo_EFFS = EFCentFullScanMultiJetHypo("EFjetHypo_3j30_a6topo_EFFS",ef_thr=30*GeV, multiplicity=3)
hypos.ef_4j30_a6topo_EFFS = EFCentFullScanMultiJetHypo("EFjetHypo_4j30_a6topo_EFFS",ef_thr=30*GeV, multiplicity=4)
hypos.ef_3j35_a6topo_EFFS = EFCentFullScanMultiJetHypo("EFjetHypo_3j35_a6topo_EFFS",ef_thr=35*GeV, multiplicity=3)

hypos.ef_j55_j45_2j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j55_j45_2j30_a4topo_EFFS',
                                                       multiplicity=4, ef_thrs=[55.0*GeV, 45.0*GeV, 30.0*GeV, 30.0*GeV])
hypos.ef_j120_j55_j40_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j120_j55_j40_a4topo_EFFS',
                                                       multiplicity=3, ef_thrs=[120.0*GeV, 55.0*GeV, 40.0*GeV])
hypos.ef_j100_j55_j40_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j100_j55_j40_a4topo_EFFS',
                                                       multiplicity=3, ef_thrs=[100.0*GeV, 55.0*GeV, 40.0*GeV])

hypos.ef_j45_3j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j45_3j30_a4topo_EFFS',
                                                  multiplicity=4, ef_thrs=[45.0*GeV, 30.0*GeV, 30.0*GeV, 30.0*GeV])

hypos.ef_j55_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j55_j30_a4topo_EFFS',
                                                  multiplicity=2, ef_thrs=[55.0*GeV, 30.0*GeV])

hypos.ef_j75_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j75_j30_a4topo_EFFS',
                                                  multiplicity=2, ef_thrs=[75.0*GeV, 30.0*GeV])

hypos.ef_j75_j40_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j75_j40_a4topo_EFFS',
                                                  multiplicity=2, ef_thrs=[75.0*GeV, 40.0*GeV])

hypos.ef_j75_j45_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j75_j45_a4topo_EFFS',
                                                  multiplicity=2, ef_thrs=[75.0*GeV, 45.0*GeV])

hypos.ef_j75_j55_2j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j75_j55_2j30_a4topo_EFFS',
                                                  multiplicity=4, ef_thrs=[75.0*GeV, 55.0*GeV, 30.0*GeV, 30.0*GeV])

hypos.ef_j100_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j100_j30_a4topo_EFFS',
                                                   multiplicity=2, ef_thrs=[100.0*GeV, 30.0*GeV])


hypos.ef_j100_2j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j100_2j30_a4topo_EFFS',
                                                            multiplicity=3, ef_thrs=[100.0*GeV,30.0*GeV, 30.0*GeV])


hypos.ef_j135_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j135_j30_a4topo_EFFS',
                                                   multiplicity=2, ef_thrs=[135.0*GeV, 30.0*GeV])

hypos.ef_j180_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j180_j30_a4topo_EFFS',
                                                   multiplicity=2, ef_thrs=[180.0*GeV, 30.0*GeV])

hypos.ef_j100_j40_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j100_j40_a4topo_EFFS',
                                                           multiplicity=2, ef_thrs=[100.0*GeV, 40.0*GeV])

hypos.ef_j75_2j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j75_2j30_a4topo_EFFS',
                                                   multiplicity=3, ef_thrs=[75.0*GeV, 30.0*GeV, 30.0*GeV])

hypos.ef_2j135_j30_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_2j135_j30_a4topo_EFFS',
                                                    multiplicity=3, ef_thrs=[135.0*GeV,135.0*GeV, 30.0*GeV])

hypos.ef_j100_2j40_a4topo_EFFS = EFCentFullScanMultiJetHypo('EFJetHypo_j100_2j40_a4topo_EFFS',
                                                            multiplicity=3, ef_thrs=[100.0*GeV, 40.0*GeV, 40.0*GeV])

hypos.ef_2fj30_a4topo_EFFS  = EFFwdFullScanMultiJetHypo("EFjetHypo_2fj30_a4topo_EFFS",ef_thr=30*GeV, multiplicity=2)
hypos.ef_2fj55_a4topo_EFFS  = EFFwdFullScanMultiJetHypo("EFjetHypo_2fj55_a4topo_EFFS",ef_thr=55*GeV, multiplicity=2)
hypos.ef_2fj75_a4topo_EFFS  = EFFwdFullScanMultiJetHypo("EFjetHypo_2fj75_a4topo_EFFS",ef_thr=75*GeV, multiplicity=2)
hypos.ef_2fj100_a4topo_EFFS  = EFFwdFullScanMultiJetHypo("EFjetHypo_2fj100_a4topo_EFFS",ef_thr=100*GeV, multiplicity=2)


class MultiJet(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, group='RATE:MultiJet', tomerge=None):
        JetTemplate.__init__(self, sig, l2=l2, ef=ef)
        self.merge(tomerge).addGroups(group)
        self.addGroups('BW:Jets')
        if 'mct' in sig:
            self.addGroups('RATE:SUSY')
        elif 'dphi' in sig:
            self.addGroups('RATE:SUSY')
        elif 'ht' in sig:
            self.addGroups('RATE:SUSY')
        else:
            pass

class MultiJet_Deta(JetTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, group='RATE:MultiJet', l2tomerge=None, eftomerge=None):
        JetTemplate.__init__(self, sig, l2=l2, ef=ef)
        self.merge(l2names=l2tomerge).addGroups(group)
        self.merge(efnames=eftomerge, onlyEF=True).addGroups(group)
        self.addGroups('BW:Jets')
        

class AntiKtFSMultiJet(MultiJet):
    def __init__(self, sig, l2, ef=DeduceChainConfig, group='RATE:MultiJet', tomerge=None, antikt=single_fexes.antikt4, hypo=None):
        JetTemplate.__init__(self, sig, l2=l2, ef=ef)
        self.merge(l2names=tomerge).addGroups(group).addGroups('BW:Jets')


        # EF
        self.addSequence('',[single_fexes.roi,
                             single_fexes.cell_maker_fullcalo, single_fexes.tower_maker], 'EF_FSTowers') # not in a chain signature

        self.continueSequence([antikt, hypo], self.ef.chain_name).addSignature()

class AntiKtFSMultiJet_topo(MultiJet):
    def __init__(self, sig, l2, ef=DeduceChainConfig, group='RATE:MultiJet', tomerge=None, antikt=single_fexes.antikt4_topo, efhypo=None):
        JetTemplate.__init__(self, sig, l2=l2, ef=ef)
        self.merge(l2names=tomerge).addGroups(group).addGroups('BW:Jets')
        if 'mct' in sig:
            self.addGroups('RATE:SUSY')
        elif 'dphi' in sig:
            self.addGroups('RATE:SUSY')
        else:
            pass
        
        # EF
        self.addSequence('',[single_fexes.roi_topo,
                             single_fexes.cell_maker_fullcalo_topo, single_fexes.topocluster_maker_fullcalo], 'EF_FSTopoClusters') # not in a chain signature
        
        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

MultiJets = [
    #antikT-chains, fullscan

    AntiKtFSMultiJet_topo('2j10_a4tc_EFFS', l2=None, ef=Chain(counter=644), efhypo=hypos.ef_2j10_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2j20_a4tc_EFFS', l2=None, ef=Chain(counter=6422), efhypo=hypos.ef_2j20_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2j45_a4tc_EFFS_L12J20XE20', l2=Chain(name='L2_2j40', counter=651, seed='L1_2J20_XE20'), ef=Chain(counter=651), tomerge=['L2_j40']*2, efhypo=hypos.ef_2j45_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2j55_a4tc_EFFS_L12J30XE20', l2=Chain(name='L2_2j50', counter=650, seed='L1_2J30_XE20'), ef=Chain(counter=650), tomerge=['L2_j50']*2, efhypo=hypos.ef_2j55_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2j55_a4tc_EFFS', l2=Chain(name='L2_2j50', counter=643, seed='L1_2J30_XE20'), ef=Chain(counter=650), tomerge=['L2_j50']*2, efhypo=hypos.ef_2j55_a4topo_EFFS),
    
    AntiKtFSMultiJet_topo('3j10_a4tc_EFFS', l2=None, ef=Chain(counter=647, seed='L2_rd0_filled_NoAlg'), efhypo=hypos.ef_3j10_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('3j15_a4tc_EFFS', l2=None, ef=Chain(counter=648, seed='L2_rd0_filled_NoAlg'), efhypo=hypos.ef_3j15_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j20_a4tc_EFFS', l2=Chain(name='L2_3j15_L13J10', counter=6011, seed='L1_3J10'), ef=Chain(counter=6011), tomerge=['L2_j15_L1J10']*3, efhypo=hypos.ef_3j20_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j30_a4tc_EFFS', l2=Chain(name='L2_3j25', counter=640, seed='L1_3J10'), ef=Chain(counter=640), tomerge=['L2_j25']*3, efhypo=hypos.ef_3j30_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j40_a4tc_EFFS', l2=Chain(name='L2_3j35', counter=661, seed='L1_3J15'), ef=Chain(counter=661), tomerge=['L2_j35']*3, efhypo=hypos.ef_3j40_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j45_a4tc_EFFS', l2=Chain(name='L2_3j40', counter=664, seed='L1_3J20'), ef=Chain(counter=664), tomerge=['L2_j40']*3, efhypo=hypos.ef_3j45_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j75_a4tc_EFFS', l2=Chain(name='L2_3j70', counter=649, seed='L1_3J50'), ef=Chain(counter=649), tomerge=['L2_j70']*3, efhypo=hypos.ef_3j75_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j100_a4tc_EFFS', l2=Chain(name='L2_3j95', counter=650, seed='L1_3J75'), ef=Chain(counter=367), tomerge=['L2_j95']*3, efhypo=hypos.ef_3j100_a4topo_EFFS),    

    AntiKtFSMultiJet_topo('4j10_a4tc_EFFS', l2=None, ef=Chain(counter=648, seed='L2_rd0_filled_NoAlg'), efhypo=hypos.ef_4j10_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('4j15_a4tc_EFFS', l2=None, ef=Chain(counter=648, seed='L2_rd0_filled_NoAlg'), efhypo=hypos.ef_4j15_a4topo_EFFS),
    AntiKtFSMultiJet_topo('4j20_a4tc_EFFS', l2=None, ef=Chain(counter=6480), efhypo=hypos.ef_4j20_a4topo_EFFS),
    AntiKtFSMultiJet_topo('4j30_a4tc_EFFS', l2=Chain(name='L2_4j25', counter=645, seed='L1_4J10'), ef=Chain(counter=645), tomerge=['L2_j25']*4, efhypo=hypos.ef_4j30_a4topo_EFFS),
    AntiKtFSMultiJet_topo('4j40_a4tc_EFFS', l2=Chain(name='L2_4j35', counter=662, seed='L1_4J15'), ef=Chain(counter=662), tomerge=['L2_j35']*4, efhypo=hypos.ef_4j40_a4topo_EFFS),
    AntiKtFSMultiJet_topo('4j45_a4tc_EFFS', l2=Chain(name='L2_4j40', counter=669, seed='L1_4J20'), ef=Chain(counter=669), tomerge=['L2_j40']*4, efhypo=hypos.ef_4j45_a4topo_EFFS),
    
    #E.P.
    AntiKtFSMultiJet_topo('4j45_a4tc_EFFS_4L1J15', l2=Chain(name='L2_4j40_4L1J15', counter=671, seed='L1_4J15'), ef=Chain(counter=671), tomerge=['L2_j40']*4, efhypo=hypos.ef_4j45_a4topo_EFFS),
    
    AntiKtFSMultiJet_topo('4j55_a4tc_EFFS', l2=Chain(name='L2_4j50', counter=651, seed='L1_4J30'), ef=Chain(counter=651), tomerge=['L2_j50']*4, efhypo=hypos.ef_4j55_a4topo_EFFS),
    AntiKtFSMultiJet_topo('4j60_a4tc_EFFS', l2=Chain(name='L2_4j50', counter=651, seed='L1_4J30'), ef=Chain(counter=650), tomerge=['L2_j50']*4, efhypo=hypos.ef_4j60_a4topo_EFFS),

    AntiKtFSMultiJet_topo('5j10_a4tc_EFFS', l2=None, ef=Chain(counter=688, seed='L2_rd0_filled_NoAlg'), efhypo=hypos.ef_5j10_a4topo_EFFS),
    AntiKtFSMultiJet_topo('5j30_a4tc_EFFS', l2=Chain(name='L2_5j25', counter=652, seed='L1_5J10'), ef=Chain(counter=652), tomerge=['L2_j25']*5, efhypo=hypos.ef_5j30_a4topo_EFFS),
    AntiKtFSMultiJet_topo('5j40_a4tc_EFFS', l2=Chain(name='L2_5j35', counter=663, seed='L1_4J15'), ef=Chain(counter=663), tomerge=['L2_j35']*5, efhypo=hypos.ef_5j40_a4topo_EFFS),
    AntiKtFSMultiJet_topo('5j45_a4tc_EFFS', l2=Chain(name='L2_5j40', counter=665, seed='L1_5J20'), ef=Chain(counter=665), tomerge=['L2_j40']*5, efhypo=hypos.ef_5j45_a4topo_EFFS),#

    AntiKtFSMultiJet_topo('6j40_a4tc_EFFS', l2=Chain(name='L2_5j35', counter=663, seed='L1_4J15'), ef=Chain(counter=670), tomerge=['L2_j35']*5, efhypo=hypos.ef_6j40_a4topo_EFFS),
    AntiKtFSMultiJet_topo('6j45_a4tc_EFFS', l2=Chain(name='L2_5j40', counter=665, seed='L1_5J20'), ef=Chain(counter=666), tomerge=['L2_j40']*5, efhypo=hypos.ef_6j45_a4topo_EFFS),#
    AntiKtFSMultiJet_topo('6j30_a4tc_EFFS', l2=Chain(name='L2_6j25', counter=653, seed='L1_6J10'), ef=Chain(counter=653), tomerge=['L2_j25']*6, efhypo=hypos.ef_6j30_a4topo_EFFS),#
    AntiKtFSMultiJet_topo('6j30_a4tc_EFFS_L15J10', l2=Chain(name='L2_5j25', counter=652, seed='L1_5J10'), ef=Chain(counter=659), tomerge=['L2_j25']*5, efhypo=hypos.ef_6j30_a4topo_EFFS),#
    
    AntiKtFSMultiJet_topo('7j30_a4tc_EFFS_L16J10', l2=Chain(name='L2_6j25', counter=121, seed='L1_6J10'), ef=Chain(counter=121), tomerge=['L2_j25']*6, efhypo=hypos.ef_7j30_a4topo_EFFS), 
    AntiKtFSMultiJet_topo('7j30_a4tc_EFFS_L15J10', l2=Chain(name='L2_5j25', counter=122, seed='L1_5J10'), ef=Chain(counter=122), tomerge=['L2_j25']*5, efhypo=hypos.ef_7j30_a4topo_EFFS), 
    
    AntiKtFSMultiJet_topo('2j100_a4tc_EFFS_L1J75', l2=Chain(name='L2_j95', counter=119, seed='L1_J75'), ef=Chain(counter=587), tomerge=['L2_j95'], efhypo=hypos.ef_2j100_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('2j135_a4tc_EFFS_L1J75', l2=Chain(name='L2_j95', counter=119, seed='L1_J75'), ef=Chain(counter=588), tomerge=['L2_j95'], efhypo=hypos.ef_2j135_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('2j180_a4tc_EFFS_L1J75', l2=Chain(name='L2_j95', counter=119, seed='L1_J75'), ef=Chain(counter=585), tomerge=['L2_j95'], efhypo=hypos.ef_2j180_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('2j240_a4tc_EFFS_L1J75', l2=Chain(name='L2_j95', counter=119, seed='L1_J75'), ef=Chain(counter=586), tomerge=['L2_j95'], efhypo=hypos.ef_2j240_a4topo_EFFS),
    AntiKtFSMultiJet_topo('3j100_a4tc_EFFS_L1J75', l2=Chain(name='L2_j95', counter=119, seed='L1_J75'), ef=Chain(counter=583), tomerge=['L2_j95'], efhypo=hypos.ef_3j100_a4topo_EFFS),    

    AntiKtFSMultiJet_topo("j45_3j30_a4tc_EFFS",   l2=Chain(name = 'L2_j40_4j25',  counter = 102,  seed = 'L1_4J10'), tomerge = ['L2_j40']+['L2_j25']*3, efhypo=hypos.ef_j45_3j30_a4topo_EFFS  ),

    AntiKtFSMultiJet_topo('j55_j45_2j30_a4tc_EFFS', l2=Chain(name='L2_j50_2j40_4j25', counter=727, seed='L1_4J10'),
                          ef=Chain(counter=3630), efhypo=hypos.ef_j55_j45_2j30_a4topo_EFFS,
                          tomerge=['L2_j50', 'L2_j40']+['L2_j25']*2), 

    AntiKtFSMultiJet_topo("j75_j30_a4tc_EFFS",   l2=Chain(name = 'L2_j70_2j25',  counter = 3396, seed = 'L1_2J10_J50'), tomerge = [ 'L2_j70', 'L2_j25'], efhypo=hypos.ef_j75_j30_a4topo_EFFS  ),
    AntiKtFSMultiJet_topo("j75_2j30_a4tc_EFFS",   l2=Chain(name = 'L2_j70_3j25',  counter = 396,  seed = 'L1_3J10_J50'), tomerge = ['L2_j70']+['L2_j25']*2, efhypo=hypos.ef_j75_2j30_a4topo_EFFS  ),
    AntiKtFSMultiJet_topo("j75_j40_a4tc_EFFS",   l2=Chain(name = 'L2_j70_2j35',  counter = 4531, seed = 'L1_2J15_J50'), tomerge = [ 'L2_j70', 'L2_j35'], efhypo=hypos.ef_j75_j40_a4topo_EFFS  ),

    #E.P.
    AntiKtFSMultiJet_topo("j75_j40_a4tc_EFFS_2L1J10",   l2=Chain(name = 'L2_j70_2j35_2L1J10',  counter = 4533, seed = 'L1_2J10_J50'), tomerge = [ 'L2_j70', 'L2_j35'], efhypo=hypos.ef_j75_j40_a4topo_EFFS  ),

    AntiKtFSMultiJet_topo("j75_j40_a4tc_EFFS_L12J15J50",   l2=Chain(name = 'L2_j70_j35_L12J15J50',  counter = 4534, seed = 'L1_2J15_J50'), tomerge = [ 'L2_j70', 'L2_j35'], efhypo=hypos.ef_j75_j40_a4topo_EFFS  ),
    
    AntiKtFSMultiJet_topo("j75_j45_a4tc_EFFS",   l2=Chain(name = 'L2_j70_2j40',  counter = 4532, seed = 'L1_2J15_J50'), tomerge = [ 'L2_j70', 'L2_j40'], efhypo=hypos.ef_j75_j45_a4topo_EFFS  ),
    AntiKtFSMultiJet_topo("j75_j55_2j30_a4tc_EFFS",  l2=Chain(name = 'L2_j70_2j50_4j25',  counter = 4533, seed = 'L1_4J10'), tomerge = [ 'L2_j70', 'L2_j50']+['L2_j25']*2, efhypo=hypos.ef_j75_j55_2j30_a4topo_EFFS  ),

    AntiKtFSMultiJet_topo("j100_j30_a4tc_EFFS",  l2=Chain(name = 'L2_j95_2j25',  counter = 3396, seed = 'L1_2J10_J75'), tomerge = ['L2_j95', 'L2_j25'], efhypo=hypos.ef_j100_j30_a4topo_EFFS ),

    AntiKtFSMultiJet_topo("j100_j40_a4tc_EFFS",   l2=Chain(name = 'L2_j95_j35',  counter = 3130, seed = 'L1_J75'), tomerge = [ 'L2_j95', 'L2_j35'], efhypo=hypos.ef_j100_j40_a4topo_EFFS  ),

    AntiKtFSMultiJet_topo("j100_2j40_a4tc_EFFS",   l2=Chain(name = 'L2_j95_2j35',  counter = 3131, seed = 'L1_J75'), tomerge = [ 'L2_j95', 'L2_j35','L2_j35'], efhypo=hypos.ef_j100_2j40_a4topo_EFFS  ),
    
    #E.P.
    AntiKtFSMultiJet_topo("j100_2j30_a4tc_EFFS",  l2=Chain(name = 'L2_j95_3j25',  counter = 3396, seed = 'L1_2J10_J75'), tomerge = ['L2_j95']+['L2_j25']*2, efhypo=hypos.ef_j100_2j30_a4topo_EFFS ),        
    

    AntiKtFSMultiJet_topo("j135_j30_a4tc_EFFS",   l2=Chain(name = 'L2_j95_2j25',  counter = 3396, seed = 'L1_2J10_J75'), tomerge = ['L2_j95', 'L2_j25'  ], efhypo=hypos.ef_j135_j30_a4topo_EFFS  ),
    AntiKtFSMultiJet_topo("j180_j30_a4tc_EFFS",   l2=Chain(name = 'L2_j95_2j25',  counter = 3397, seed = 'L1_2J10_J75'), tomerge = [ 'L2_j95', 'L2_j25'], efhypo=hypos.ef_j180_j30_a4topo_EFFS  ),
    
    ]

###central - forward
MultiJets += [

    AntiKtFSMultiJet_topo('j30_fj30_a4tc_EFFS', l2=Chain(name='L2_j25_fj25', counter=72, seed='L1_J10_FJ10'), ef=Chain(counter=72), tomerge = ['L2_j25','L2_fj25'], efhypo=hypos.ef_2j30_a4topo_EFFS),    
    AntiKtFSMultiJet_topo('j40_fj40_a4tc_EFFS', l2=Chain(name='L2_j35_fj35', counter=73, seed='L1_J10_FJ10'), ef=Chain(counter=73), tomerge = ['L2_j35_loose','L2_fj35'], efhypo=hypos.ef_2j40_a4topo_EFFS),    

    AntiKtFSMultiJet_topo('j55_fj55_a4tc_EFFS', l2=Chain(name='L2_j50_fj50', counter=74, seed='L1_J30_FJ30'), ef=Chain(counter=74), tomerge = ['L2_j50','L2_fj50'], efhypo=hypos.ef_2j55_a4topo_EFFS),
    
    AntiKtFSMultiJet_topo('j75_fj75_a4tc_EFFS', l2=Chain(name='L2_j70_fj70', counter=75, seed='L1_J50_FJ50'), ef=Chain(counter=75), tomerge = ['L2_j70','L2_fj70'], efhypo=hypos.ef_2j75_a4topo_EFFS),    

    ]

#### Forward-Forward
MultiJets += [
    AntiKtFSMultiJet_topo('2fj30_a4tc_EFFS', l2=Chain(name='L2_2fj25', counter=120, seed='L1_2FJ10'), tomerge=['L2_fj25']*2, efhypo=hypos.ef_2fj30_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2fj55_a4tc_EFFS', l2=Chain(name='L2_2fj50', counter=686, seed='L1_2FJ30'), tomerge=['L2_fj50']*2, efhypo=hypos.ef_2fj55_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2fj75_a4tc_EFFS', l2=Chain(name='L2_2fj70', counter=283, seed='L1_2FJ50'), tomerge=['L2_fj70']*2, efhypo=hypos.ef_2fj75_a4topo_EFFS),
    AntiKtFSMultiJet_topo('2fj100_a4tc_EFFS_L1FJ75', l2=Chain(name='L2_fj95', counter=687, seed='L1_FJ75'), tomerge=['L2_fj95'], efhypo=hypos.ef_2fj100_a4topo_EFFS),
    
    ]


# anti-kt versions
MultiJets +=[
    MultiJet_Deta("2fj30_a4tc_EFFS_deta50_FB", l2=Chain(name = 'L2_2fj25',  counter = 120, seed = 'L1_2FJ10'), ef=Chain(counter=675), l2tomerge=['L2_fj25']*2,eftomerge = ['EF_2fj30_a4tc_EFFS']).\
    addSequence('EF_2fj30_a4tc_EFFS', [fexes.ef_fj30_deta5, hypos.ef_fj30_deta5], 'EF_fj30deta5').addSignature(),
    
    MultiJet_Deta("2fj30_a4tc_EFFS_deta50_FC", l2=Chain(name = 'L2_j25_fj25',  counter = 72, seed = 'L1_J10_FJ10'), ef=Chain(counter=676), l2tomerge=['L2_j25','L2_fj25'],eftomerge = ['EF_j30_fj30_a4tc_EFFS']).\
    addSequence('EF_j30_fj30_a4tc_EFFS', [fexes.ef_j30_fj30_deta5, hypos.ef_j30_fj30_deta5], 'EF_j30_fj30deta5').addSignature(),

    MultiJet_Deta("2fj55_a4tc_EFFS_deta50_FB", l2=Chain(name = 'L2_2fj50',  counter = 686, seed = 'L1_2FJ30'), ef=Chain(counter=686), l2tomerge=['L2_fj50']*2, eftomerge = ['EF_2fj55_a4tc_EFFS']).\
    addSequence('EF_2fj55_a4tc_EFFS', [fexes.ef_fj55_deta5, hypos.ef_fj55_deta5], 'EF_fj55deta5').addSignature(),

    MultiJet_Deta("2fj55_a4tc_EFFS_deta50_FC", l2=Chain(name = 'L2_j50_fj50',  counter = 74, seed = 'L1_J30_FJ30'), ef=Chain(counter=678), l2tomerge = ['L2_j50','L2_fj50'], eftomerge = ['EF_j55_fj55_a4tc_EFFS']).\
    addSequence('EF_j55_fj55_a4tc_EFFS', [fexes.ef_j55_fj55_deta5, hypos.ef_j55_fj55_deta5], 'EF_j55_fj55deta5').addSignature(),
    
    MultiJet_Deta("2fj75_a4tc_EFFS_deta50_FB", l2=Chain(name = 'L2_2fj70',  counter = 283, seed = 'L1_2FJ50'), ef=Chain(counter=679), l2tomerge=['L2_fj70']*2, eftomerge = ['EF_2fj75_a4tc_EFFS']).\
    addSequence('EF_2fj75_a4tc_EFFS', [fexes.ef_fj75_deta5, hypos.ef_fj75_deta5], 'EF_fj75deta5').addSignature(),
    
    MultiJet_Deta("2fj75_a4tc_EFFS_deta50_FC", l2=Chain(name = 'L2_j70_fj70',  counter = 75, seed = 'L1_J50_FJ50'), ef=Chain(counter=680), l2tomerge=['L2_j70', 'L2_fj70'], eftomerge = ['EF_j75_fj75_a4tc_EFFS']).\
    addSequence('EF_j75_fj75_a4tc_EFFS', [fexes.ef_j75_fj75_deta5, hypos.ef_j75_fj75_deta5], 'EF_j75_fj75deta5').addSignature(),

    MultiJet_Deta("2fj100_a4tc_EFFS_deta50_FB", l2=Chain(name = 'L2_fj95',  counter = 228, seed = 'L1_FJ75'), ef=Chain(counter=674), l2tomerge=['L2_fj95'], eftomerge = ['EF_2fj100_a4tc_EFFS_L1FJ75']).\
    addSequence('EF_2fj100_a4tc_EFFS_L1FJ75', [fexes.ef_fj100_deta5, hypos.ef_fj100_deta5], 'EF_fj100deta5').addSignature(),
    
    MultiJet_Deta("2j100_a4tc_EFFS_deta35_FC", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=681), l2tomerge=['L2_j95'], eftomerge = ['EF_2j100_a4tc_EFFS_L1J75']).\
    addSequence('EF_2j100_a4tc_EFFS_L1J75', [fexes.ef_j100_deta3p5, hypos.ef_j100_deta3p5], 'EF_j100deta3p5').addSignature(),

    MultiJet_Deta("2j135_a4tc_EFFS_deta35_FC", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=682), l2tomerge=['L2_j95'], eftomerge = ['EF_2j135_a4tc_EFFS_L1J75']).\
    addSequence('EF_2j135_a4tc_EFFS_L1J75', [fexes.ef_j135_deta3p5, hypos.ef_j135_deta3p5], 'EF_j135deta3p5').addSignature(),

    MultiJet_Deta("2j180_a4tc_EFFS_deta35_FC", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=683), l2tomerge=['L2_j95'], eftomerge = ['EF_2j180_a4tc_EFFS_L1J75']).\
    addSequence('EF_2j180_a4tc_EFFS_L1J75', [fexes.ef_j180_deta3p5, hypos.ef_j180_deta3p5], 'EF_j180deta3p5').addSignature(),

    MultiJet_Deta("2j240_a4tc_EFFS_deta35_FC", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=687), l2tomerge=['L2_j95'], eftomerge = ['EF_2j240_a4tc_EFFS_L1J75']).\
    addSequence('EF_2j240_a4tc_EFFS_L1J75', [fexes.ef_j240_deta3p5, hypos.ef_j240_deta3p5], 'EF_j240deta3p5').addSignature(),
]


MultiJets += [
    MultiJet('j70_j25_dphi03_NoEF', l2=Chain(name='L2_2j25_j70_dphi03', counter=1146, seed='L1_2J10_J50')).merge(l2names=['L2_j25']+['L2_j70']).\
    addSequence(['L2_j70','L2_j25'], [fexes.l2_dphi_2j25_j70, hypos.l2_dphi_2j25_j70_dphi03], 'L2_2j25j70dphi03').addSignature(),
    ]

#dphi with anti-kt
MultiJets += [
    AntiKtFSMultiJet_topo("j75_j30_a4tc_EFFS_L2dphi04",   l2=Chain(name = 'L2_j70_2j25_dphi04',  counter = 1149, seed = 'L1_2J10_J50'),   tomerge = ['L2_j70',  'L2_j25'],  efhypo=hypos.ef_j75_j30_a4topo_EFFS  ).\
    addSequence(['L2_j70', 'L2_j25'], [fexes.l2_dphi_2j25_j70, hypos.l2_dphi_2j25_j70_dphi04], 'L2_j70_2j25dphi04').addSignature(),

    AntiKtFSMultiJet_topo("j100_j30_a4tc_EFFS_L2dphi04",   l2=Chain(name = 'L2_j95_2j25_dphi04',  counter = 1150, seed = 'L1_2J10_J75'),  tomerge = ['L2_j95', 'L2_j25'],  efhypo=hypos.ef_j100_j30_a4topo_EFFS  ).\
    addSequence(['L2_j95', 'L2_j25'], [fexes.l2_dphi_2j25_j95, hypos.l2_dphi_2j25_j95_dphi04], 'L2_j95_2j25dphi04').addSignature(),

    AntiKtFSMultiJet_topo("j135_j30_a4tc_EFFS_L2dphi04",   l2=Chain(name = 'L2_j95_2j25_dphi04',  counter = 1151, seed = 'L1_2J10_J75'), tomerge = ['L2_j95',  'L2_j25'], efhypo=hypos.ef_j135_j30_a4topo_EFFS  ).\
    addSequence(['L2_j95', 'L2_j25'], [fexes.l2_dphi_2j25_j95, hypos.l2_dphi_2j25_j95_dphi04], 'L2_j95_2j25dphi04').addSignature(),

    ]

## EF dphi
MultiJets += [
    
    MultiJet("j135_j30_a4tc_EFFS_dphi04", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=543), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j135_j30_a4tc_EFFS'], [hypos.EFDphi04], 'EF_j135_j30dphi04').addSignature(),

    MultiJet("j180_j30_a4tc_EFFS_dphi04", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=559), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j180_j30_a4tc_EFFS'], [hypos.EFDphi04], 'EF_j180_j30dphi04').addSignature(),


    ]


##mct with a4_EFFS
   
MultiJets += [

    AntiKtFSMultiJet_topo("j75_j30_a4tc_EFFS_L2anymct100",   l2=Chain(name = 'L2_j70_2j25_anymct100',  counter = 1302, seed = 'L1_2J10_J50'), tomerge = ['L2_j70',  'L2_j25'], efhypo=hypos.ef_j75_j30_a4topo_EFFS  ).\
    addSequence(['L2_j70', 'L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct100], 'L2_j70_2j25anymct100').addSignature(),

    AntiKtFSMultiJet_topo("j75_j30_a4tc_EFFS_L2anymct150",   l2=Chain(name = 'L2_j70_2j25_anymct150',  counter = 302, seed = 'L1_2J10_J50'), tomerge = ['L2_j70',  'L2_j25'], efhypo=hypos.ef_j75_j30_a4topo_EFFS  ).\
    addSequence(['L2_j70', 'L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct150], 'L2_j70_2j25anymct150').addSignature(),

    AntiKtFSMultiJet_topo("j75_j30_a4tc_EFFS_L2anymct175",   l2=Chain(name = 'L2_j70_2j25_anymct175',  counter = 303, seed = 'L1_2J10_J50'), tomerge = ['L2_j70',  'L2_j25'], efhypo=hypos.ef_j75_j30_a4topo_EFFS  ).\
    addSequence(['L2_j70', 'L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct175], 'L2_j70_2j25anymct175').addSignature(),
    ]



MultiJets += [
	
    MultiJet("j75_j30_a4tc_EFFS_anymct150", l2=Chain(name = 'L2_j70_2j25_L2anymct100',  counter = 813, seed = 'L1_2J10_J50'), tomerge = ['EF_j75_j30_a4tc_EFFS']).\
    addSequence(['L2_j70','L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct100], 'L2_j70_2j25L2anymct100').addSignature().\
    addSequence(['EF_j75_j30_a4tc_EFFS'], [hypos.EFAnyMct150], 'EF_j75j30anymct150').addSignature(),

    MultiJet("j75_j30_a4tc_EFFS_anymct175", l2=Chain(name = 'L2_j70_2j25_L2anymct100',  counter = 814, seed = 'L1_2J10_J50'), tomerge = ['EF_j75_j30_a4tc_EFFS']).\
    addSequence(['L2_j70','L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct100], 'L2_j70_2j25L2anymct100').addSignature().\
    addSequence(['EF_j75_j30_a4tc_EFFS'], [hypos.EFAnyMct175], 'EF_j75j30anymct175').addSignature(),


    MultiJet("j75_j30_a4tc_EFFS_leadingmct150", l2=Chain(name = 'L2_j70_2j25_L2anymct100',  counter = 815, seed = 'L1_2J10_J50'), tomerge = ['EF_j75_j30_a4tc_EFFS']).\
    addSequence(['L2_j70','L2_j25'], [fexes.anymct_2j25, hypos.l2_2j25_j70_anymct100], 'L2_j70_2j25L2anymct100').addSignature().\
    addSequence(['EF_j75_j30_a4tc_EFFS'], [hypos.EFLeadingMct150], 'EF_j75j30leadingmct150').addSignature(),

    MultiJet("2j45_a4tc_EFFS_leadingmct100", l2=Chain(name = 'L2_2j40_anymct100',  counter = 4816, seed = 'L1_2J20_XE20'), tomerge = ['EF_2j45_a4tc_EFFS_L12J20XE20']).\
    addSequence(['L2_j40','L2_j40'], [fexes.anymct_2j40, hypos.l2_2j40_anymct100], 'L2_2j40L2anymct100').addSignature().\
    addSequence(['EF_2j45_a4tc_EFFS_L12J20XE20'], [hypos.EFLeadingMct100], 'EF_2j45leadingmct100').addSignature(),	

    MultiJet("2j55_a4tc_EFFS_leadingmct100", l2=Chain(name = 'L2_2j50_anymct100',  counter = 4815, seed = 'L1_2J30_XE20'), tomerge = ['EF_2j55_a4tc_EFFS_L12J30XE20']).\
    addSequence(['L2_j50','L2_j50'], [fexes.anymct_2j50, hypos.l2_2j50_anymct100], 'L2_2j50L2anymct100').addSignature().\
    addSequence(['EF_2j55_a4tc_EFFS_L12J30XE20'], [hypos.EFLeadingMct100], 'EF_2j55leadingmct100').addSignature(),	

    ]


## HT
MultiJets += [
    
    MultiJet("j75_2j30_a4tc_EFFS_ht350", l2=Chain(name = 'L2_j70_3j25',  counter = 396, seed = 'L1_3J10_J50'), ef=Chain(counter=455), tomerge = ['EF_j75_2j30_a4tc_EFFS']).\
    addSequence(['EF_j75_2j30_a4tc_EFFS'], [hypos.EFHT350], 'EF_j75_2j30HT350').addSignature(),

    #E.P.
    MultiJet("j75_j40_a4tc_EFFS_ht350", l2=Chain(name = 'L2_j70_2j35_2L1J10',  counter = 453, seed = 'L1_2J10_J50'), ef=Chain(counter=454), tomerge = ['EF_j75_j40_a4tc_EFFS_2L1J10']).\
    addSequence(['EF_j75_j40_a4tc_EFFS_2L1J10'], [hypos.EFHT350], 'EF_j75_j40HT350').addSignature(),
    
    ###E.P. TMP
    ##MultiJet("EF_j75_j40_a4tc_EFFS_ht350", l2=Chain(name = 'L2_j70_2j35',  counter = 453, seed = 'L1_2J15_J50'), ef=Chain(counter=454), tomerge = ['EF_j75_j40_a4tc_EFFS']).\
    ##addSequence(['EF_j75_j40_a4tc_EFFS'], [hypos.EFHT350], 'EF_j75_j40HT350').addSignature(),
    
    MultiJet("j100_a4tc_EFFS_ht200", l2=Chain(name = 'L2_j95',  counter = 1121, seed = 'L1_J75'), ef=Chain(counter=1459), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j100_a4tc_EFFS'], [hypos.EFHT200], 'EF_j100HT200').addSignature(),

    MultiJet("j100_a4tc_EFFS_ht300", l2=Chain(name = 'L2_j95',  counter = 1121, seed = 'L1_J75'), ef=Chain(counter=1457), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j100_a4tc_EFFS'], [hypos.EFHT300], 'EF_j100HT300').addSignature(),
    
    MultiJet("j100_a4tc_EFFS_ht350", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=456), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j100_a4tc_EFFS'], [hypos.EFHT350], 'EF_j100HT350').addSignature(),

    MultiJet("j135_a4tc_EFFS_ht500", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=476), tomerge = ['EF_j135_a4tc_EFFS']).\
    addSequence(['EF_j135_a4tc_EFFS'], [hypos.EFHT500], 'EF_j135HT500').addSignature(),

    MultiJet("j100_a4tc_EFFS_ht500", l2=Chain(name = 'L2_j95',  counter = 119, seed = 'L1_J75'), ef=Chain(counter=534), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j100_a4tc_EFFS'], [hypos.EFHT500], 'EF_j100HT500').addSignature(),

    MultiJet("j100_j40_a4tc_EFFS_ht300", l2=Chain(name = 'L2_j95_j35',  counter = 1119, seed = 'L1_J75'), ef=Chain(counter=1456), tomerge = ['EF_j100_j40_a4tc_EFFS']).\
    addSequence(['EF_j100_j40_a4tc_EFFS'], [hypos.EFHT300], 'EF_j100_j40HT300').addSignature(),
    
    MultiJet("j100_2j40_a4tc_EFFS_ht350", l2=Chain(name = 'L2_j95_2j35',  counter = 1122, seed = 'L1_J75'), ef=Chain(counter=1458), tomerge = ['EF_j100_2j40_a4tc_EFFS']).\
    addSequence(['EF_j100_2j40_a4tc_EFFS'], [hypos.EFHT350], 'EF_j100_2j40HT350').addSignature(),

    MultiJet("4j40_a4tc_EFFS_ht400", l2=Chain(name = 'L2_4j35',  counter = 662, seed = 'L1_4J15'), ef=Chain(counter=823), tomerge = ['EF_4j40_a4tc_EFFS']).\
    addSequence(['EF_4j40_a4tc_EFFS'], [hypos.EFHT400], 'EF_4j40HT400').addSignature(),

    MultiJet("4j40_a4tc_EFFS_ht350", l2=Chain(name = 'L2_4j35',  counter = 662, seed = 'L1_4J15'), ef=Chain(counter=596), tomerge = ['EF_4j40_a4tc_EFFS']).\
    addSequence(['EF_4j40_a4tc_EFFS'], [hypos.EFHT350], 'EF_4j40HT350').addSignature(),

    MultiJet("j100_a4tc_EFFS_ht400", l2=Chain(name = 'L2_j95',  counter = 107, seed = 'L1_J75'), ef=Chain(counter=458), tomerge = ['EF_j100_a4tc_EFFS']).\
    addSequence(['EF_j100_a4tc_EFFS'], [hypos.EFHT400], 'EF_j100HT400').addSignature(),

    MultiJet('ht350_a4tc_EFFS_L2je255',l2=Chain(name='L2_je255', counter=256, seed='L1_JE140'), ef=Chain(counter=473), tomerge = ['EF_je255_NoEF']).\
    addSequence(['EF_FSTopoClusters'], [single_fexes.antikt4_topo,hypos.EFHT350], 'EF_ht350L2je255').addSignature(),


    ]

class MultiJet_mct(MultiJet):
    def __init__(self, sig, l2, ef=DeduceChainConfig, tomerge=None, fex=None, hypo=None):
        MultiJet.__init__(self, sig, l2=l2, ef=ef, tomerge=None)
        # ??? this again shoudl be just chain name
        out_te_name = 'L2_'+self.l2.chain_name.replace('L2_', '').replace('_','')
        in_te_name = tomerge
        # we do not call base class (MultiJet merge because this ine does it by the EF chain names)
        self.merge(l2names=tomerge)
        
        assert fex, 'Can not create chain %s w/o anymct fex'% self.l2.chain_name
        assert hypo,'Can not create chain %s w/o anymct hypo'% self.l2.chain_name                
        self.addSequence(in_te_name, [fex, hypo], out_te_name).addSignature()



class L2EFChain_jet_full_pseudoscan(MultiJet):
    """
    Jet chain with L2 full pseudo-scan and with a virtual l1 threshold.
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        JetTemplate.__init__(self, sig, l2, ef)
        # Check we've been given the l1threshold
        assert l1threshold != None, "l1 threshold not given, this cannont be assumed from the signature name at the moment"
        # guess hypos (the l2 cut will be the 2nd word, and be something like jXX)
        if l2hypo == None:
            from TrigJetHypo.TrigJetHypoConfig import L2FullScanMultiJetHypo
            l2cut = int(l2.name.split('_')[1].split('j')[1])
            # the assumed multiplicity is 1, of the name is of the form XjY, then multiplicty is = X
            l2multi = l2.name.split('_')[1].split('j')[0]
            if l2multi == "":
                hypos.l2fullscanhypo =  L2FullScanMultiJetHypo('L2JetHypo_FullScan_j%d'%(l2cut), l2_thr=l2cut*GeV, multiplicity=1, l2_thrs=[])
            else:
                l2multi = l2multi
                hypos.l2fullscanhypo =  L2FullScanMultiJetHypo('L2JetHypo_FullScan_%sj%d'%(l2multi,l2cut), l2_thr=l2cut*GeV, multiplicity=int(l2multi), l2_thrs=[])

            l2hypo = hypos.l2fullscanhypo
            print " inferred l2 hypo:        ",l2hypo.getName()
        # now construct sequences        
        self.addSequence(l1threshold, [fexes.l2fex_unpacking_allroi,], "L2_AllRoiUnpacking")
        self.continueSequence([fexes.t2_fastjet_cc_fex,l2hypo],
                              self.l2.chain_name).addSignature()
        # EF
        self.addSequence('',[single_fexes.roi_topo,
                             single_fexes.cell_maker_fullcalo_topo,
                             single_fexes.topocluster_maker_fullcalo],
                         'EF_FSTopoClusters')
        antikt = single_fexes.antikt4_topo
        self.continueSequence([antikt, efhypo], self.ef.chain_name).addSignature()

        self.addGroups('BW:Jets')

        # that's it!
        print " done\n\n"
        #assert False
        pass


MultiJets +=[
    L2EFChain_jet_full_pseudoscan('6j30_a4tcem_L2a4cc',
                                  l2=Chain(name='L2_6j25_a4ccem', counter=6006, seed='L1_6J10'),
                                  # l2hypo is deduced from the chain name (6j25)
                                  efhypo=hypos.ef_6j30_a4topo_EFFS,
                                  l1threshold="J10",
                                  ),
    ]

