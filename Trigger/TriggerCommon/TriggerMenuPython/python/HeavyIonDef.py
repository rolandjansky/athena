# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Heavy Ion chains
###########################################################################
__author__  = 'T. Bold'
__version__=""
__doc__="The implementation of Heavy Ion trigger sequences and chains "

from TriggerMenuPython.HltConfig import *

from string import atoi
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

from ChainTemplate import FreeStore, Chain, ChainTemplate, DeduceChainConfig

fexes = FreeStore()
hypos = FreeStore()

# definde seed for HI
minbias_hi_seed = 'L2_mbZdc_a_c_NoAlg'
minbias_hi_l1_seed = 'L1_ZDC_A_C_VTE50'

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
from TrigJetHypo.TrigJetHypoConfig import L2JetHypo

from TrigHIHypo.TrigHIHypoConfig import HIJetBaseSubtraction

fexes.l2jet  = T2CaloJet_Jet()
fexes.l2corr = HIJetBaseSubtraction()


hypos.j15 = L2JetHypo('L2JetHypo_j15_Ecorr', 15*GeV)
hypos.j20 = L2JetHypo('L2JetHypo_j20_Ecorr', 20*GeV)
hypos.fj20 = L2JetHypo('L2JetHypo_fj20_Ecorr', 20*GeV) 

#UPC stuff
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
fexes.efid_hi = TrigEFIDSequence("FullScanHI","heavyIonFS","InsideOutNoTRT").getSequence()
fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
#RoiCreator = DummyAlgo("RoiCreator", runOncePerEvent=True)
fexes.roi = DummyAlgo('UPCRoiCreator')
#fexes.metroi = DummyAlgo('METRoiCreator')

from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo

hypos.upc = HIEFTrackHypo(name='HIEFTrackHypo_UPC')

from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_loose1

hypos.upc_loose1 = HIEFTrackHypo_loose1(name='HIEFTrackHypo_loose1_UPC')

from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
hypos.atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

# other UPC approach
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_loose2

hypos.upc_loose2 = HIEFTrackHypo_loose2(name='HIEFTrackHypo_loose2_UPC')

from TrigT2MinBias.TrigT2MinBiasConfig import hypos as l2hypos
hypos.update(l2hypos)

from TrigT2MinBias.TrigT2MinBiasConfig import fexes as l2fexes
fexes.update(l2fexes)

from TrigGenericAlgs.TrigGenericAlgsConf import AcceptWhenInputMissing as VetoMBTS
#RoiCreator = DummyAlgo("RoiCreator", runOncePerEvent=True)
fexes.vetoMBTS = VetoMBTS('VetoMBTS')

# FCAL sum Et
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
fexes.effex_fcal = EFMissingET_Fex()

# MET fexes
from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex
fexes.met_feb = L2CaloMissingET_Fex()

#from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_allCells
#fexes.met_allCells = EFMissingET_Fex_allCells()

from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_2sidednoiseSupp
fexes.met_2Sided = EFMissingET_Fex_2sidednoiseSupp()

# met hypos
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBXE
hypos.met_l2_xe40 = L2MetHypoFEBXE('L2MetHypo_FEB_xe40', l2_thr=40.*GeV)
hypos.met_l2_xe30 = L2MetHypoFEBXE('L2MetHypo_FEB_xe30', l2_thr=30.0*GeV)
hypos.met_l2_xe25 = L2MetHypoFEBXE('L2MetHypo_FEB_xe25', l2_thr=25.0*GeV)
hypos.met_l2_xe20 = L2MetHypoFEBXE('L2MetHypo_FEB_xe20', l2_thr=20.0*GeV)

from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXE
hypos.met_ef_xe40 = EFMetHypoXE('EFMetHypoXE_xe40', ef_thr=40.*GeV)
hypos.met_ef_xe30 = EFMetHypoXE('EFMetHypoXE_xe30', ef_thr=30.*GeV)
hypos.met_ef_xe25 = EFMetHypoXE('EFMetHypoXE_xe25', ef_thr=25.*GeV)
hypos.met_ef_xe20 = EFMetHypoXE('EFMetHypoXE_xe20', ef_thr=20.*GeV)

#EF
from TrigHIHypo.TrigHIHypoConfig import hypos as efhypos
hypos.update(efhypos)

#for t in thresholds:
#    h = ETSumHypo_%d_veto('L2MetHypo_xe%d%s_Mon'%(t, m),l2_thr=t*GeV)
#    setattr(hypos, h)
#del thresholds

# ----
class CorrJet(ChainTemplate):
    def __init__(self, sig, l2, hypo, l1te=None):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        # deduce l1 threshold TE for the item name
        if l1te == None:
            l1te  = l2.seed.split('_')[-1]
        jetth = sig.split('_')[0]+'_preEcorr'
        self.addSequence(l1te, [fexes.l2jet], 'L2_%s' % jetth  ).addSignature() 
        self.continueSequence( [fexes.l2corr, hypo], 'L2_%s' % sig).addSignature() 
        # that's it we do not want to have EF algs here
        
        self.addStreams('jets').addGroups('HeavyIon', 'RATE:HeavyIonJets')



# HI  chains
HeavyIonChains = [# dummy chain to decide how big fraction of events will go to
                  # the prompt stream
                  ChainTemplate('prompt', l2=None, ef=Chain(seed='', counter=900)).addStreams('prompt').addGroups('HeavyIon'),

                  # jets with correction
                  CorrJet('j15_Ecorr', l2=Chain(seed='L1_J10', counter=720), hypo = hypos.j15),
                  CorrJet('j20_Ecorr', l2=Chain(seed='L1_J10', counter=721), hypo = hypos.j20),
                  CorrJet('fj20_Ecorr', l1te='JF10', l2=Chain(seed='L1_FJ10', counter=722), hypo = hypos.fj20),             
                  ]

# ----
class UPC_FullScan(ChainTemplate):
    def __init__(self, sig, l2):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        self.addSequence('',[DummyAlgo("EFDummyAlgo")]+
                         fexes.efid_hi, 'EF_FStracksHI').addSignature()
        self.continueSequence(  [hypos.upc], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('UPC').addGroups('BW:MuonElectronUPC', 'RATE:HeavyIonUPC')

HeavyIonChains += [
    UPC_FullScan('L1EM3_VTE50_upc', l2=Chain(seed='L1_EM3_VTE50', counter=878)),
    UPC_FullScan('L1EM3_MV_VTE50_upc', l2=Chain(seed='L1_EM3_MV_VTE50', counter=875)),
    UPC_FullScan('L1MU0_VTE50_upc', l2=Chain(seed='L1_MU0_VTE50', counter=799)),
    UPC_FullScan('L1MU0_MV_VTE50_upc', l2=Chain(seed='L1_MU0_MV_VTE50', counter=798)),
    UPC_FullScan('L1MU4_VTE50_upc', l2=Chain(seed='L1_MU4_VTE50', counter=765)),
    UPC_FullScan('L1MU4_MV_VTE50_upc', l2=Chain(seed='L1_MU4_MV_VTE50', counter=764)),
    UPC_FullScan('L1ZDC_VTE50_upc', l2=Chain(seed=minbias_hi_l1_seed, counter=797)),
    ]

# ----
class UPC_FullScan_loose(ChainTemplate):
    def __init__(self, sig, l2):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        self.addSequence('EF_FStracksHI', [hypos.upc_loose1], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('UPC').addGroups('BW:MuonElectronUPC', 'RATE:HeavyIonUPC')

HeavyIonChains += [
    UPC_FullScan_loose('L1EM3_VTE50_loose_upc', l2=Chain(seed='L1_EM3_VTE50', counter=736)),
    UPC_FullScan_loose('L1EM3_MV_VTE50_loose_upc', l2=Chain(seed='L1_EM3_MV_VTE50', counter=737)),
    UPC_FullScan_loose('L1MU0_VTE50_loose_upc', l2=Chain(seed='L1_MU0_VTE50', counter=738)),
    UPC_FullScan_loose('L1MU0_MV_VTE50_loose_upc', l2=Chain(seed='L1_MU0_MV_VTE50', counter=740)),
    UPC_FullScan_loose('L1MU4_VTE50_loose_upc', l2=Chain(seed='L1_MU4_VTE50', counter=741)),
    UPC_FullScan_loose('L1MU4_MV_VTE50_loose_upc', l2=Chain(seed='L1_MU4_MV_VTE50', counter=742)),
    UPC_FullScan_loose('L1ZDC_VTE50_loose_upc', l2=Chain(seed=minbias_hi_l1_seed, counter=725)),
    ]

#----
class UPC_FullScan_VetoMBTS_loose(ChainTemplate):
    def __init__(self, sig, l2, l2hypo=None):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        if l2hypo == None:
            l2hypo = hypos.L2MbMbtsHypo_5
            
        self.addSequence('', [fexes.L2MbMbtsFex, l2hypo], self.l2.chain_name+'_mbts'  ) 
        self.continueSequence( [fexes.vetoMBTS], self.l2.chain_name  ).addSignature() 
        self.addSequence( 'EF_FStracksHI',
                          [hypos.upc_loose2], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('UPC').addGroups('BW:MuonElectronUPC', 'RATE:HeavyIonUPC')

HeavyIonChains += [
    UPC_FullScan_VetoMBTS_loose('L1ZDC_VTE50_mv_loose2_upc', l2=Chain(seed=minbias_hi_l1_seed, counter=804)),
    UPC_FullScan_VetoMBTS_loose('L1ZDC_OR_mv_loose2_upc', l2=Chain(seed='L1_ZDC', counter=825)),

    UPC_FullScan_VetoMBTS_loose('L1ZDC_VTE50_mv_medium2_upc', l2=Chain(seed=minbias_hi_l1_seed, counter=890),
                                l2hypo=hypos.L2MbMbtsHypo_3_3_or),
    UPC_FullScan_VetoMBTS_loose('L1ZDC_OR_mv_medium2_upc', l2=Chain(seed='L1_ZDC', counter=891),
                                l2hypo=hypos.L2MbMbtsHypo_3_3_or),
    ]
#----
class UPC_FullScan_hip_VetoMBTS_loose(ChainTemplate):
    def __init__(self, sig, l2, l2hypo1=None, l2hypo2=None):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        if l2hypo1 == None:
            l2hypo1 = hypos.L2MbMbtsHypo_5
        if l2hypo2 == None:
            l2hypo2 = hypos.L2MbSpHypoLow
            
        self.addSequence('', [fexes.L2MbMbtsFex, l2hypo1], self.l2.chain_name+'_mbts'  )
        self.continueSequence( [fexes.vetoMBTS], self.l2.chain_name+'_vmbts' ).addSignature()
        self.continueSequence([fexes.L2MbSpFex, l2hypo2], self.l2.chain_name ).addSignature()
        
        self.continueSequence( fexes.efid+[hypos.upc_loose2], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('UPC').addGroups('BW:MuonElectronUPC', 'RATE:HeavyIonUPC')

HeavyIonChains += [
    UPC_FullScan_hip_VetoMBTS_loose('L1MU0_VTE20_hip_loose2_upc', l2=Chain(seed='L1_MU0_VTE20', counter=804),l2hypo1=hypos.L2MbMbtsHypo_4_4_or),
    UPC_FullScan_hip_VetoMBTS_loose('L1EM3_VTE20_hip_loose2_upc', l2=Chain(seed='L1_EM3_VTE20', counter=504),l2hypo1=hypos.L2MbMbtsHypo_4_4_or),
    UPC_FullScan_hip_VetoMBTS_loose('L1EM3_VTE50_hip_loose2_upc', l2=Chain(seed='L1_EM3_VTE50', counter=505),l2hypo1=hypos.L2MbMbtsHypo_4_4_or),
    UPC_FullScan_hip_VetoMBTS_loose('L1ZDC_A_VTE20_hip_loose2_upc', l2=Chain(seed='L1_ZDC_A_VTE20', counter=510),l2hypo1=hypos.L2MbMbtsHypo_4_4_or),
    UPC_FullScan_hip_VetoMBTS_loose('L1ZDC_C_VTE20_hip_loose2_upc', l2=Chain(seed='L1_ZDC_C_VTE20', counter=519),l2hypo1=hypos.L2MbMbtsHypo_4_4_or),
    ]

#----
class MinBias_FullScan_AtLeastOneTrack(ChainTemplate):
    def __init__(self, sig, l2):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        self.addSequence( 'EF_FStracksHI',
                          [hypos.atLeastOneTrack], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('minbias').addGroups('BW:MinBias', 'RATE:MinBias')

HeavyIonChains += [
    MinBias_FullScan_AtLeastOneTrack('mbZdc_a_c_L1VTE50_trk', l2=Chain(seed=minbias_hi_l1_seed, counter=935)),
    ]

#----
class MinBias_FullScan_MBTS_AtLeastOneTrack(ChainTemplate):
    def __init__(self, sig, l2, l2hypo=None):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        if l2hypo == None:
            l2hypo = hypos.L2MbMbtsHypo_1_1
            
        self.addSequence('', [fexes.L2MbMbtsFex, l2hypo], self.l2.chain_name  ).addSignature() 
        
        self.addSequence( 'EF_FStracksHI',
                                   [hypos.atLeastOneTrack], self.ef.chain_name).addSignature()
        # that's it we do not want to have EF algs here
        
        self.addStreams('minbias').addGroups('BW:MinBias', 'RATE:MinBias')

HeavyIonChains += [
    MinBias_FullScan_MBTS_AtLeastOneTrack('mbMbts_1_1_L1VTE50_trk', l2=Chain(seed='L1_MBTS_1_1_VTE50', counter=830)),
    MinBias_FullScan_MBTS_AtLeastOneTrack('mbMbts_2_2_L1VTE50_trk', l2=Chain(seed='L1_MBTS_2_2_VTE50', counter=831), l2hypo=hypos.L2MbMbtsHypo_2_2),
    ]

# -----
class L2EFChain_FCAL_SumEt(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, effex=None, efhypo=None):
        #ef.replace_if_default('name', 'EF_%s'%sig)
        
        #if ef == DeduceChainConfig:
        #    ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)

        ChainTemplate.__init__(self, sig, l2, ef)

        if effex == None:
            effex= fexes.effex_fcal

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        # EF
        self.addSequence('',[effex, efhypo], self.ef.chain_name).addSignature() # not in a chain signature

        self.addGroups('RATE:TotalEnergy', 'BW:MET')
        self.addStreams('minbias')
        pass

#############################################################################
def hypo_from_chain_name(name):
    level,cutval = name.split('_')[0:2]

    assert hasattr(hypos, 'ETSumHypo_%s_veto'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
    return getattr(hypos, 'ETSumHypo_%s_veto'%cutval), cutval

        
helpers = FreeStore()
helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']

HeavyIonChains += [
    
    L2EFChain_FCAL_SumEt('fte75_veto', l2=None, ef=Chain(counter=953, seed=minbias_hi_seed),),
    L2EFChain_FCAL_SumEt('centrality100_60', l2=None, ef=Chain(counter=840, seed=minbias_hi_seed), efhypo=hypos.CentralityHypo_100_60),
    L2EFChain_FCAL_SumEt('centrality80_60', l2=None, ef=Chain(counter=841, seed=minbias_hi_seed), efhypo=hypos.CentralityHypo_80_60),
    
    ]

#----
class L2EFChain_FEB_MET(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, efhypo=None):
        ChainTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        #self.addSequence('',fexes.met_feb, 'L2_FEB_MET') # not in a chain signature

        self.continueSequence([fexes.met_feb, l2hypo], self.l2.chain_name).addSignature() # in a chain signature

        # note this is unseeded sequence
        self.addSequence('', [fexes.met_2Sided, efhypo], self.ef.chain_name).addSignature()
        #self.continueSequence([fexes.met_2Sided, efhypo], self.ef.chain_name).addSignature()
        self.addGroups('RATE:TotalEnergy', 'BW:MET')
        self.addStreams('met')

HeavyIonChains += [
    L2EFChain_FEB_MET ( 'xe20_FEB_L1TE65', l2=Chain(counter=876, seed='L1_TE65'), l2hypo=hypos.met_l2_xe20 , efhypo=hypos.met_ef_xe20 ),
    L2EFChain_FEB_MET ( 'xe20_FEB_L1TE90', l2=Chain(counter=877, seed='L1_TE90'), l2hypo=hypos.met_l2_xe20 , efhypo=hypos.met_ef_xe20 ),
    L2EFChain_FEB_MET ( 'xe20_FEB_L1EM5', l2=Chain(counter=893, seed='L1_EM5'), l2hypo=hypos.met_l2_xe20 , efhypo=hypos.met_ef_xe20 ),
    L2EFChain_FEB_MET ( 'xe20_FEB_L1J10', l2=Chain(counter=894, seed='L1_J10'), l2hypo=hypos.met_l2_xe20 , efhypo=hypos.met_ef_xe20 ),
    L2EFChain_FEB_MET ( 'xe20_FEB_L1MU0', l2=Chain(counter=905, seed='L1_MU0'), l2hypo=hypos.met_l2_xe20 , efhypo=hypos.met_ef_xe20 ),

    L2EFChain_FEB_MET ( 'xe25_FEB_L1TE65', l2=Chain(counter=141, seed='L1_TE65'), l2hypo=hypos.met_l2_xe25 , efhypo=hypos.met_ef_xe25 ),
    L2EFChain_FEB_MET ( 'xe25_FEB_L1TE90', l2=Chain(counter=142, seed='L1_TE90'), l2hypo=hypos.met_l2_xe25 , efhypo=hypos.met_ef_xe25 ),

    L2EFChain_FEB_MET ( 'xe30_FEB_L1TE65', l2=Chain(counter=912, seed='L1_TE65'), l2hypo=hypos.met_l2_xe30 , efhypo=hypos.met_ef_xe30 ),
    L2EFChain_FEB_MET ( 'xe30_FEB_L1TE90', l2=Chain(counter=914, seed='L1_TE90'), l2hypo=hypos.met_l2_xe30 , efhypo=hypos.met_ef_xe30 ),
    L2EFChain_FEB_MET ( 'xe30_FEB_L1EM5', l2=Chain(counter=932, seed='L1_EM5'), l2hypo=hypos.met_l2_xe30 , efhypo=hypos.met_ef_xe30 ),
    L2EFChain_FEB_MET ( 'xe30_FEB_L1J10', l2=Chain(counter=933, seed='L1_J10'), l2hypo=hypos.met_l2_xe30 , efhypo=hypos.met_ef_xe30 ),
    L2EFChain_FEB_MET ( 'xe30_FEB_L1MU0', l2=Chain(counter=934, seed='L1_MU0'), l2hypo=hypos.met_l2_xe30 , efhypo=hypos.met_ef_xe30 ),
    
    L2EFChain_FEB_MET ( 'xe40_FEB_L1TE65', l2=Chain(counter=845, seed='L1_TE65'), l2hypo=hypos.met_l2_xe40 , efhypo=hypos.met_ef_xe40 ),
    L2EFChain_FEB_MET ( 'xe40_FEB_L1TE90', l2=Chain(counter=846, seed='L1_TE90'), l2hypo=hypos.met_l2_xe40 , efhypo=hypos.met_ef_xe40 ),
    L2EFChain_FEB_MET ( 'xe40_FEB_L1EM5', l2=Chain(counter=847, seed='L1_EM5'), l2hypo=hypos.met_l2_xe40 , efhypo=hypos.met_ef_xe40 ),
    L2EFChain_FEB_MET ( 'xe40_FEB_L1J10', l2=Chain(counter=860, seed='L1_J10'), l2hypo=hypos.met_l2_xe40 , efhypo=hypos.met_ef_xe40 ),
    L2EFChain_FEB_MET ( 'xe40_FEB_L1MU0', l2=Chain(counter=861, seed='L1_MU0'), l2hypo=hypos.met_l2_xe40 , efhypo=hypos.met_ef_xe40 ),
    ]

from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex
fexes.met_feb = L2CaloMissingET_Fex()
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBTE
hypos.met_l2_te10 = L2MetHypoFEBTE('L2MetHypo_FEB_te10', l2_thr=10.*GeV)

HeavyIonChains += [
    ChainTemplate('zerobias_Overlay_L2te', l2=Chain(counter=1501, seed='L1_ZB'), ef=DeduceChainConfig)\
    .addSequence('',[fexes.met_feb, hypos.met_l2_te10], 'L2_e10_zerobias_Overlay').addSignature().addGroups('RATE:Minbias','BW:Detector').addStreams('MinBiasOverlay'),
    ChainTemplate('zerobias_Overlay_L2mbMbts',   l2=Chain(counter=1500, seed='L1_ZB'), ef=DeduceChainConfig)\
    .addSequence('', [fexes.L2MbMbtsFex, hypos.L2MbMbtsHypo_1_1], 'L2_mbMbts_1_1' ).addSignature().addGroups('RATE:Minbias','BW:Detector').addStreams('MinBiasOverlay'),
    ]



class L2EFChain_FCAL_SumEt_mbMbts(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, effex=None, efhypo=None):
        #ef.replace_if_default('name', 'EF_%s'%sig)
        
        #if l2 == DeduceChainConfig:
        #    l2 = Chain()
        #l2.replace_if_default('name', 'L2_%s'%sig)

        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        
        ChainTemplate.__init__(self, sig, l2, ef)


        if l2hypo == None:
            l2hypo = hypos.L2MbMbtsHypo_1_1

        if l2fex == None:
            l2fex= fexes.L2MbMbtsFex

        if effex == None:
            effex= fexes.effex_fcal

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        #L2
        self.addSequence('',[l2fex, l2hypo], self.l2.chain_name).addSignature() # not in a chain signature
        # EF
        self.continueSequence([effex, efhypo], self.ef.chain_name).addSignature() # not in a chain signature

        self.addGroups('RATE:TotalEnergy', 'BW:MET')
        self.addStreams('minbias')
        pass

#############################################################################
def hypo_from_chain_name(name):
    level,cutval = name.split('_')[0:2]

    assert hasattr(hypos, 'ETSumHypo_%s_veto'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
    return getattr(hypos, 'ETSumHypo_%s_veto'%cutval), cutval

        
helpers = FreeStore()
helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']
#############################################################################
HeavyIonChains += [
    
    L2EFChain_FCAL_SumEt_mbMbts('fte75_veto_mbMbts_2_2', l2=Chain(seed='L1_ZDC_A_C', counter=813),
                                l2hypo=hypos.L2MbMbtsHypo_2_2),
    
    ]


