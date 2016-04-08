# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SliceDef file for met and te chains/signatures

#from TriggerMenuPython.HltConfig import *
from ChainTemplate import *

hypos = FreeStore()
fexes = FreeStore()

from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

####Jet Fexes

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo_calib
fexes.ef_antikt4_tc_had  = TrigJetRec_AntiKt4_topo_calib()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_lctopo
fexes.ef_antikt4_tc_lcw  = TrigJetRec_AntiKt4_lctopo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt10_topo
fexes.ef_antikt10_tc_em  = TrigJetRec_AntiKt10_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt10_lctopo
fexes.ef_antikt10_tc_lcw  = TrigJetRec_AntiKt10_lctopo()

##-----------------------------------------

#L2CaloMissingET_Fex
from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex
fexes.l2fex = L2MissingET_Fex()

## FEB_Fex
from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex
fexes.l2fex_FEB = L2CaloMissingET_Fex() 

# FEB with L2=L1 
from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex_ReadL2L1
fexes.l2fex_FEB_ReadL2L1 = L2CaloMissingET_Fex_ReadL2L1() 

#for HI
##from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex_NoMuSeed
##fexes.l2fex_NoMuSeed = L2MissingET_Fex_NoMuSeed()

#Cell-Based MET, one sided noise cut, used in 2010-2011
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_noiseSupp
fexes.effex = EFMissingET_Fex_noiseSupp()

#Cell-Based MET, two sided noise cut, pp_v4 menus
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_2sidednoiseSupp
fexes.effex_2Sided = EFMissingET_Fex_2sidednoiseSupp()
    
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_FEB 
fexes.effex_FEB = EFMissingET_Fex_FEB()

from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_allCells
fexes.effex_allCells = EFMissingET_Fex_allCells()

from TrigL2PileUp.TrigL2PileupConfig import TrigL2PileUp_Fex
fexes.l2fex_PileUp = TrigL2PileUp_Fex()

from TrigPileUpHypo.TrigPileUpHypoConfig import TrigPileUpHypoBase

#Muon fexes:
from TrigMissingETMuon.TrigMissingETMuonConfig import L2TrigMissingETMuon_Fex
fexes.l2muonfex = L2TrigMissingETMuon_Fex()

from TrigMissingETMuon.TrigMissingETMuonConfig import L2CaloTrigMissingETMuon_Fex
fexes.l2muonfex_FEB = L2CaloTrigMissingETMuon_Fex()

from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex
fexes.efmuonfex = EFTrigMissingETMuon_Fex()

from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_FEB
fexes.efmuonfex_FEB = EFTrigMissingETMuon_Fex_FEB()

from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topocl
fexes.efmuonfex_topocl = EFTrigMissingETMuon_Fex_topocl()
#end muon fexes



#### TopoCluster ####
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClusters 
fexes.effex_cluster = EFMissingET_Fex_topoClusters()

from TrigMissingETHypo.TrigMissingETHypoConfig import EFMissingETHypoBase

##L2 Hypos for v2-v3
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoXE
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoTE
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoXS_2011

##L2 Hypos for FEB MET, v4 
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBXE
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBXE_L1check
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBTE 
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBXS

##EF Hypos for all menus, if using cell-based MET
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXE
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTE
####2011 (v2-v3)
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXS_1sided
####2012 (v2-v3)
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXS_2sided

##EF Hypo for TC MET, v4.
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTCXE 

##EF Hypo for FEB MET, v4.
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoFEBXE 


#L2
thresholds=[5, 12, 15, 17, 20, 22, 25, 27, 30, 32, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 100, 150,10000]
thresholds=[5, 12, 15, 17, 20, 22, 25, 27, 30, 32, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 100, 150,10000]

muon_correction=['', '_noMu','_wMu']
for m in muon_correction:
    for t in thresholds:
        attr = 'l2_xe%d%s' %(t, m)
        h = L2MetHypoXE(name='L2MetHypo_xe%d%s'%(t, m),l2_thr=t*GeV)
        setattr(hypos, attr, h)
        ##Hypo for v4 menu
        attrFEB = 'l2_xe%d_FEB%s' %(t, m) 
        hFEB = L2MetHypoFEBXE(name='L2MetHypo_FEB_xe%d%s'%(t,m),l2_thr=t*GeV) 
        setattr(hypos, attrFEB, hFEB)
        attrFEB = 'l2_xe%d_FEB_L2L1Check%s' %(t, m) 
        hFEB = L2MetHypoFEBXE_L1check(name='L2MetHypo_FEB_xe%d%s_L2L1Check'%(t,m),l2_thr=t*GeV) 
        setattr(hypos, attrFEB, hFEB) 
del thresholds

for m in muon_correction:
    attr = 'l2_noL2%s' %m
    h = L2MetHypoXE('L2MetHypo_xe_noL2%s' %m,l2_thr=t*GeV)
    setattr(hypos, attr, h)

    ##Hypo for v4 menu
    attrFEB = 'l2_noL2_FEB%s' %m 
    hFEB = L2MetHypoFEBXE(name='L2MetHypo_FEB_xe_noL2%s'%m,l2_thr=t*GeV) 
    setattr(hypos, attrFEB, hFEB) 

    attr = 'l2_noL2_noMu_Mon'
    h = L2MetHypoXE('L2MetHypo_xe_noL2_noMu_Mon',l2_thr=-100*GeV)
    setattr(hypos, attr, h)
    attr = 'ef_noEF_noMu_Mon'
    h = EFMetHypoXE('EFMetHypo_xe_noL2_noMu_Mon',ef_thr=-100*GeV)
    setattr(hypos, attr, h)
    attr = 'ef_noEF'
    h = EFMetHypoXE('EFMetHypo_xe_noEF',ef_thr=-100*GeV)
    setattr(hypos, attr, h)

    attr = 'ef_FEB_noEF'
    h = EFMetHypoFEBXE('EFMetHypo_xe_FEB_noEF',ef_thr=-100*GeV)
    setattr(hypos, attr, h)
    
thresholds_mon=[10, 12, 15, 60, 65, 75, 80, 85, 90, 100]
# this is even uglier .. because the constructors of MET derived configurables do not allow passing by keywords
    
for m in muon_correction:
    for t in thresholds_mon:
        attr = 'l2_xe%d%s_Mon'%(t, m)
        h = L2MetHypoXE('L2MetHypo_xe%d%s_Mon'%(t, m),l2_thr=t*GeV)
        setattr(hypos, attr, h)
del thresholds_mon
            
thresholds=[12, 15, 20, 22, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 100, 110, 150]
for m in muon_correction:
    for t in thresholds:
        attr = 'ef_xe%d%s'%(t, m)
        h = EFMetHypoXE('EFMetHypo_xe%d%s'%(t, m),ef_thr=t*GeV)
        setattr(hypos, attr, h)


xe_correction=['_tcem', '_tchad','_tclcw']
for c in xe_correction:
    for m in muon_correction:
        for t in thresholds:
            attrTC = 'ef_xe%d_TC%s%s' %(t,c,m) 
            hTC = EFMetHypoTCXE(name='EFMetHypo_TC_xe%d%s%s'%(t,c,m),ef_thr=t*GeV) 
            setattr(hypos, attrTC, hTC) 
del thresholds



thresholds_mon=[15, 20, 22, 100, 110]
for m in muon_correction:
    for t in thresholds_mon:
        attr = 'ef_xe%d%s_Mon'%(t, m)
        h = EFMetHypoXE('EFMetHypo_xe%d%s_Mon'%(t,m),ef_thr=t*GeV)
        setattr(hypos, attr, h)
del thresholds_mon


#now TE
l2_te_thresholds=[5, 10, 20, 30, 50, 100, 150, 180, 250, 300, 350, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,1400,1600,3000]
for t in l2_te_thresholds:
    attr = 'l2_te%d' %t
    h = L2MetHypoTE('L2MetHypo_te%d'%t,l2_thr=t*GeV)
    setattr(hypos, attr, h)
    attrFEB = 'l2_te%d_FEB' %t 
    hFEB = L2MetHypoFEBTE('L2MetHypo_FEB_te%d'%t,l2_thr=t*GeV) 
    setattr(hypos, attrFEB, hFEB)
del l2_te_thresholds
                
ef_te_thresholds=[20, 90, 150, 170, 200, 250, 300, 350, 400, 450, 500, 550, 600, 700, 800, 900,
                  1000, 1100, 1200,1400, 1600,4000 ]

for t in ef_te_thresholds:
    attr = 'ef_te%d' %t
    h = EFMetHypoTE('EFMetHypo_te%d'%t,ef_thr=t*GeV)
    setattr(hypos, attr, h)
del ef_te_thresholds

# now XS
l2_xs_thresholds=[ 15, 20, 25, 28, 30, 35, 40, 45, 50, 55, 60, 65]
for t in l2_xs_thresholds:
    m = '_noMu' # muon correction
    attr = 'l2_xs%d%s' % (t, m)
    h = L2MetHypoXS_2011('L2MetHypo_xs_2011_%d%s' % (t, m),l2_thr=t*0.1)
    setattr(hypos, attr, h)
    attrFEB = 'l2_xs%d_FEB' %(t) 
    hFEB = L2MetHypoFEBXS(name='L2MetHypo_FEB_xs%d'%(t),l2_thr=t*0.1) 
    setattr(hypos, attrFEB, hFEB) 
del l2_xs_thresholds


ef_xs_thresholds=[ 15, 20, 22, 25, 30, 34, 35, 40, 45, 50, 60, 65, 70, 75, 80, 90, 100, 120 ]
for m in muon_correction:
    for t in ef_xs_thresholds:
        #m = '_noMu' # muon correction
        attr = 'ef_xs%d%s' % (t, m)
        if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
            h = EFMetHypoXS_2sided('EFMetHypo_xs_2sided_%d%s' % (t, m),ef_thr=t*0.1)
            setattr(hypos, attr, h)
        else:
            h = EFMetHypoXS_1sided('EFMetHypo_xs_1sided_%d%s' % (t, m),ef_thr=t*0.1)
            setattr(hypos, attr, h)
del ef_xs_thresholds
        

# #this is needed 
# #from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
# from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
# #DummyFEX_xe = DummyFEX()


# # L2muonSeed='L2_mu6l'  # muComb
# #L2muonSeed='L2_mu_standalone_mu4_MSonly'  # muFast

# # EFmuonSeed='EF_mu6l'  # combined track
# # EFmuonSeed='EF_TB_mu6l' # spectrometer track (does not work)
# #EFmuonSeed='EF_MS_mu4_MSonly' # TrigMuonEFSegmentFinder
# #EFmuonSeed='L2_mu_standalone_mu4_MSonly' # TrigMuonEFSegmentFinder

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    L2muonSeed='L2_mu_standalone_mu4_MSonly'  # muFast
    EFmuonSeed='L2_mu_standalone_mu4_MSonly' # TrigMuonEFSegmentFinder
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    L2muonSeed='L2_mu4'
    EFmuonSeed='L2_mu4'
else:
    L2muonSeed='L2_mu4T'
    EFmuonSeed='L2_mu4T'

    
l2_thresholds_vfj=[15, 17, 20, 22]
for m in muon_correction:
    for t in l2_thresholds_vfj:
        attr = 'l2_xe%d_vfj%s' %(t, m)
        #print attr
        h = TrigPileUpHypoBase('TrigPileUpHypo_xe%d_vfj%s'%(t, m))
        setattr(hypos, attr, h)
#del l2_met
del l2_thresholds_vfj

L2FJseed='JF10'


#new stuff
#############################################################################
class METTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig):
        """
        Base template for all met chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        
            
        #print 'ef name', ef.name, sig
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every Jet chain should have

        if 'delayed' in sig:
            self.addStreams('haddelayed')
        else:
            self.addStreams('tauEtmiss') 
        

#############################################################################
def hypo_from_chain_name(name):
    level,cutval = name.split('_')[0:2]

    if('_vfj' in name):
        cutval = cutval+'_vfj'

#    if('_xeL2Mon' in name):
#        cutval = '_xe150'
        
    if('_noMu' in name):
        cutval = cutval+'_noMu'

    if level == 'L2' and '_L2FEB' in name: 
        cutval = cutval+'_FEB' 
            
    if level == 'EF':
        assert hasattr(hypos, 'ef_%s'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'ef_%s'%cutval), cutval

        
    if level == 'L2':
        assert hasattr(hypos, 'l2_%s'%cutval), 'impossible to deduce the L2 hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'l2_%s'%cutval), cutval


#helpers = FreeStore()
#helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']

#############################################################################
#Used from pp_v4 on, since default L2 hypos uses FEB
############################################################################
def hypo_from_chain_name_FEB(name):
    level,cutval0 = name.split('_')[0:2]

    #seeded by XE*_BGRP7
    if 'T' in name:
        cutval=cutval0.split('T')[0] 
    else:
        cutval=cutval0

    #normally, use FEB hypo 
    if  level == 'L2' : 
        cutval = cutval+'_FEB' 

    #Special chains
    #L2_noL2 chains: run only L1MET and use hypos.l2_noL2 
    if 'noL2' in name and 'EF' not in name:    
        cutval='noL2'

    #Rerun chains: run only L1MET and use hypos.l2_noL2 
    if  'L2_xe_NoCut_allL1' in name:
        cutval='noL2'
    
    #Rerun chains: run FEB at EF
    if level == 'EF' and 'FEB' in name:
        cutval = "FEB"

    #Rerun chains: no EF cut applied
    if level == 'EF' and 'NoCut' in name:
        cutval = cutval + "_noEF"
        
        
    if level == 'EF':
        assert hasattr(hypos, 'ef_%s'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'ef_%s'%cutval), cutval
    
    if level == 'L2':
        assert hasattr(hypos, 'l2_%s'%cutval), 'impossible to deduce the L2 hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
        return getattr(hypos, 'l2_%s'%cutval), cutval


def EFfex_from_chain_name(name):


    fexes_name     = "effex_2Sided"
    muonfexes_name = "efmuonfex"
    
    if 'FEB' in name:
        fexes_name     = "effex_FEB"
        muonfexes_name = "efmuonfex_FEB"

    if 'tc' in name:
        fexes_name     = "effex_cluster"
        muonfexes_name = "efmuonfex_topocl"
    
    assert hasattr(fexes, '%s'%fexes_name), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing %s)' % (name, fexes_name)
    assert hasattr(fexes, '%s'%muonfexes_name), 'impossible to deduce the EF muon hypothesis algorithm from chain name: %s' % (name)
    return getattr(fexes, '%s'%fexes_name), getattr(fexes, '%s' %muonfexes_name), 
    


#############################################################################
#Used from pp_v4 on,  for TopoCluster 
############################################################################
def hypo_from_chain_name_TC(name):
    level,cutval0 = name.split('_')[0:2]

    if 'T' in name:
        cutval=cutval0.split('T')[0] 
    else:
        cutval=cutval0
    
    if level == 'EF':
        cutval = cutval+'_TC' 
        
    if('_tcem' in name):
        cutval = cutval+'_tcem'
        
    if('_tchad' in name):
        cutval = cutval+'_tchad'
            
    if('_tclcw' in name):
        cutval = cutval+'_tclcw'


    assert hasattr(hypos, 'ef_%s'%cutval), 'impossible to deduce the EF hypothesis algorithm from chain name: %s (guessing the cut: %s)' % (name, cutval)
    return getattr(hypos, 'ef_%s'%cutval), cutval
    
        

helpers = FreeStore()
helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']


#############################################################################        
class L2EFChain_met(METTemplate):
    """
    Most regular MET Chains. Their configuration is very trivial.

    Deduction rules:
    see: METTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are mis
sing.
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, effex=None,  efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)
        
        # guess L1 threshold
        ##if l1threshold == None:
        ##    if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
        ##        l1threshold = [L2muonSeed]
        ##    elif('XE' in self.l2.lower_chain_name):
        ##        l1threshold = ['XE10', L2muonSeed]  
        ##    elif('XS' in self.l2.lower_chain_name):
        ##        if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
        ##            l1threshold = ['XS15', L2muonSeed]
        ##        else:
        ##            l1threshold = ['XE10', L2muonSeed]
        ##    else:
        ##        l1threshold = [self.l2.lower_chain_name.split('_')[1], L2muonSeed]

        if l1threshold == None:
            if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
                l1threshold = ['']
            elif('XE' in self.l2.lower_chain_name):
                l1threshold = ['XE10']
            elif('XS' in self.l2.lower_chain_name):
                if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
                    l1threshold = ['XS15']
                else:
                    l1threshold = ['XE10']
            else:
                l1threshold = [self.l2.lower_chain_name.split('_')[1]]


        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        if effex == None:
            effex= fexes.effex

        # now is time to construct sequences
        ##l2te = self.l2.chain_name                                
        ##self.addSequence(l1threshold, [fexes.l2fex, l2hypo], l2te).addSignature()

        l2te_step1 = self.l2.chain_name+'_step1'
        self.addSequence(l1threshold, [fexes.l2fex], l2te_step1).addSignature()
        input_step2 = [l2te_step1, L2muonSeed]
        l2te = self.l2.chain_name
        self.addSequence(input_step2, [fexes.l2muonfex, l2hypo], l2te).addSignature()


        # ef sequence (do not have to specify explicitly this are ef algs)
        efte = self.ef.chain_name

        l2seeds=['']
        self.addSequence(l2seeds, [effex, efhypo], efte).addSignature()
        ##efte_step1 = self.ef.chain_name+'_step1'
        ##self.addSequence(l2seeds,[effex], efte_step1).addSignature()
        ##ef_input_step2 = [efte_step1, EFmuonSeed]
        ##self.addSequence(ef_input_step2, [fexes.efmuonfex, efhypo], efte).addSignature()

        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')
        
        # that's it!
        pass

###--------------------

#############################################################################        
class L2EFChain_met_xeL2Mon(METTemplate):
    """
    Copy of L2EFChain_met with a few changes to run a l2fex
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, effex=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)
        

        if l1threshold == None:
            if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
                l1threshold = ['']
            elif('XE' in self.l2.lower_chain_name):
                l1threshold = ['XE10']
            elif('XS' in self.l2.lower_chain_name):
                if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
                    l1threshold = ['XS15']
                else:
                    l1threshold = ['XE10']
            else:
                l1threshold = [self.l2.lower_chain_name.split('_')[1]]


        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        if effex == None:
            effex= fexes.effex
        if l2fex == None:
            l2fex = fexes.l2fex 

        #l2te = self.l2.chain_name 
        #self.addSequence(l1threshold, [l2fex, l2hypo], l2te).addSignature()

        l2te_step1 = self.l2.chain_name+'_step1'
        self.addSequence(l1threshold, [fexes.l2fex], l2te_step1).addSignature()
        input_step2 = [l2te_step1, L2muonSeed]
        l2te = self.l2.chain_name
        self.addSequence(input_step2, [fexes.l2muonfex, l2hypo], l2te).addSignature()


        efte = self.ef.chain_name

        #if l1threshold == ['']:
        #    l2seeds=['']
        #else:
        #    l2seeds=[EFmuonSeed]
        #
        #if not ('NoAlg' in self.ef.chain_name):
        #    self.addSequence(l2seeds,[effex, efhypo], efte).addSignature()

        l2seeds=['']
        
        if not ('NoAlg' in self.ef.chain_name):
            self.addSequence(l2seeds,[effex, efhypo], efte).addSignature()
            #efte_step1 = self.ef.chain_name+'_step1'
            #self.addSequence(l2seeds,[effex], efte_step1).addSignature()
            #ef_input_step2 = [efte_step1, EFmuonSeed]
            #self.addSequence(ef_input_step2, [fexes.efmuonfex, efhypo], efte).addSignature()

        self.addStreams('JetTauEtmiss') 
        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')
        

        #        self.addGroups('RATE:JetTauEtmiss')
        
        pass



###--------------------    
class L2EFChain_met_hi(METTemplate):
    """
    Most regular MET Chains. Their configuration is very trivial.
    
    Deduction rules:
    see: METTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are mis
sing.

    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, effex=None,  efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)

        # guess L1 threshold
        ##if l1threshold == None:
        ##    if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
        ##        l1threshold = [L2muonSeed]
        ##    elif('XE' in self.l2.lower_chain_name):
        ##        l1threshold = ['XE10', L2muonSeed]  
        ##    elif('XS' in self.l2.lower_chain_name):
        ##        l1threshold = ['XS15', L2muonSeed]
        ##    else:
        ##        l1threshold = [self.l2.lower_chain_name.split('_')[1], L2muonSeed]

        if l1threshold == None:
            if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
                l1threshold = ['']
            elif('XE' in self.l2.lower_chain_name):
                l1threshold = ['XE10']
            elif('XS' in self.l2.lower_chain_name):
                    l1threshold = ['XS15']
            else:
                l1threshold = [self.l2.lower_chain_name.split('_')[1]]

        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name(self.ef.chain_name)

        if effex == None:
            effex= fexes.effex
        # now is time to construct sequences

        # ??? the output TE name shoudl just be the name of the chain .. but that was not followed strictly
        # for backward cmpatibility we try to do the same as was there before. Once it is decided to go along the
        # standard just the chain name should be used

        #br = self.l2.chain_name.split('_')        
        #l2te = 'L2_%s'%''.join(b for b in br[1:] if b not in helpers.te_no_keys)

        l2te = self.l2.chain_name 
                               
        #print "l2",sig, self.l2.chain_name,  mangled
        #self.addSequence(l1threshold, [fexes.l2fex_NoMuSeed, l2hypo], l2te).addSignature()
        self.addSequence(l1threshold, [fexes.l2fex, l2hypo], l2te).addSignature()

        # ef sequence (do not have to specify explicitly this are ef algs)
        #br = self.ef.chain_name.split('_')
        #efte = 'EF_%s'%''.join([ b for b in br[1:] if b not in helpers.te_no_keys])

        efte = self.ef.chain_name

        ##if l1threshold == ['']:
        ##    l2seeds=['']
        ##else:
        ##    l2seeds=[EFmuonSeed]
            
        #self.addSequence(l2seeds,[effex, efhypo], efte).addSignature()
        #self.continueSequence([fexes.effex, efhypo], efte).addSignature()

        l2seeds=['']
        self.addSequence(l2seeds, [effex, efhypo], efte).addSignature()
        #efte_step1 = self.ef.chain_name+'_step1'
        #self.addSequence(l2seeds,[effex], efte_step1).addSignature()
        #ef_input_step2 = [efte_step1, EFmuonSeed]
        #self.addSequence(ef_input_step2, [fexes.efmuonfex, efhypo], efte).addSignature()

        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')
        
        # that's it!
        pass
    
###-------------------
class L2EFChain_met_em(L2EFChain_met):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo=None, effex=None,  efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        L2EFChain_met.__init__(self, sig, l2, ef, l2hypo=l2hypo,
                               effex=effex, efhypo=efhypo, l1threshold=l1threshold)
        self.l2.groups = ['RATE:SingleElectron', 'BW:Egamma']
        self.ef.groups = ['RATE:SingleElectron', 'BW:Egamma']
        self.l2.stream_tag = []
        self.ef.stream_tag = []
        self.addStreams('egamma')

#############################################################################        
class L2EFChain_met_vfj(METTemplate):
    """
    Most regular MET Chains. Their configuration is very trivial.

    Deduction rules:
    see: METTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are mis
sing.
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo_1=None, l2hypo_2=None, effex=None,  efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)
        
        # guess L1 threshold
        ##if l1threshold == None:
        ##    if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
        ##        l1threshold = [L2muonSeed]
        ##    elif('XE' in self.l2.lower_chain_name):
        ##        l1threshold = ['XE10', L2muonSeed]  
        ##    else:
        ##        l1threshold = [self.l2.lower_chain_name.split('_')[1], L2muonSeed]
        
        if l1threshold == None:
            if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
                l1threshold = ['']
            elif('XE' in self.l2.lower_chain_name):
                l1threshold = ['XE10']
            else:
                l1threshold = [self.l2.lower_chain_name.split('_')[1]]


        # guess hypos
        if l2hypo_1 == None:
            MuonCorr=''
            if '_noMu' in self.l2.chain_name:
                MuonCorr='_noMu' 
            l2_step1_chain_name='L2_'+self.l2.chain_name.split('_')[1]+MuonCorr
            l2hypo_1, cut = hypo_from_chain_name(l2_step1_chain_name)

        if l2hypo_2 == None:
            l2hypo_2, cut = hypo_from_chain_name(self.l2.chain_name)

        if efhypo == None:
            ef_chain_name='EF_'+self.ef.chain_name.split('_')[1]+MuonCorr
            efhypo, cut = hypo_from_chain_name(ef_chain_name)

        if effex == None:
            effex= fexes.effex
        # now is time to construct sequences
        
        # ??? the output TE name shoudl just be the name of the chain .. but that was not followed strictly
        # for backward cmpatibility we try to do the same as was there before. Once it is decided to go along the
        # standard just the chain name should be used

        #br = self.l2.chain_name.split('_')        
        #l2te = 'L2_%s'%''.join(b for b in br[1:] if b not in helpers.te_no_keys)

        #l2te = self.l2.chain_name 
                               
        #print "l2",sig, self.l2.chain_name,  mangled
        #self.addSequence(l1threshold, [fexes.l2fex, l2hypo_1], l2_step1_chain_name).addSignature()

        l2te_step1 = self.l2.chain_name+'_step1'
        self.addSequence(l1threshold, [fexes.l2fex], l2te_step1).addSignature()
        input_step2 = [l2te_step1, L2muonSeed]
        l2te = self.l2.chain_name
        self.addSequence(input_step2, [fexes.l2muonfex, l2hypo], l2te).addSignature()

        self.addSequence([L2FJseed], [fexes.l2fex_PileUp, l2hypo_2], l2te).addSignature()

        # ef sequence (do not have to specify explicitly this are ef algs)
        #br = self.ef.chain_name.split('_')
        #efte = 'EF_%s'%''.join([ b for b in br[1:] if b not in helpers.te_no_keys])

        efte = self.ef.chain_name

        #l2seeds=[EFmuonSeed]
        #self.addSequence(l2seeds,[effex, efhypo], efte).addSignature()
        #self.continueSequence([fexes.effex, efhypo], efte).addSignature()

        l2seeds=['']
        self.addSequence(l2seeds, [effex, efhypo], efte).addSignature()
        #efte_step1 = self.ef.chain_name+'_step1'
        #self.addSequence(l2seeds,[effex], efte_step1).addSignature()
        #ef_input_step2 = [efte_step1, EFmuonSeed]
        #self.addSequence(ef_input_step2, [fexes.efmuonfex, efhypo], efte).addSignature()

        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')
        
        # that's it!
        pass

############################################################################# 
        
l2chains = FreeStore()
#############################################################################
#############################################################################
## 2010-2011 Menus
#############################################################################
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():

    MissingETs = [
        L2EFChain_met('xe20', l2=Chain(name='L2_xe12', counter=792, seed='L1_XE10'), l2hypo=hypos.l2_xe12_Mon, efhypo=hypos.ef_xe20_Mon),
        L2EFChain_met('xe30', l2=Chain(name='L2_xe20', counter=145, seed='L1_XE15')),
        L2EFChain_met('xe55', l2=Chain(name='L2_xe45', counter=309, seed='L1_XE40')),
        #    L2EFChain_met('xe65_loose', l2=Chain(name='L2_xe40_loose', counter=439, seed='L1_XE35')),
        #    L2EFChain_met('xe80_medium', l2=Chain(name='L2_xe60_medium', counter=149, seed='L1_XE50')),
        ]

    #unseeded chains
    MissingETs += [
        L2EFChain_met('xe_NoCut_allL1', l2=Chain(name='L2_xe_NoCut_allL1', counter=269, seed=''), l2hypo=hypos.l2_noL2,efhypo=hypos.ef_noEF),
        L2EFChain_met('xe15_noL2', l2=Chain(name='L2_noL2', counter=836, seed=''), l2hypo=hypos.l2_noL2),
        L2EFChain_met('xe20_noL2', l2=Chain(name='L2_noL2', counter=835, seed=''), l2hypo=hypos.l2_noL2),
        #E.P.
        L2EFChain_met('xe40_noL2', l2=Chain(name='L2_noL2', counter=835, seed=''), l2hypo=hypos.l2_noL2),
        ]
    #no muon correction
    MissingETs += [
        L2EFChain_met('xe20_noMu', l2=Chain(name='L2_xe10_noMu', counter=793, seed='L1_XE10'), l2hypo=hypos.l2_xe10_noMu_Mon, efhypo=hypos.ef_xe20_noMu_Mon),
        L2EFChain_met('xe22_noMu', l2=Chain(name='L2_xe15_noMu', counter=796, seed='L1_XE10'),l2hypo=hypos.l2_xe15_noMu_Mon, efhypo=hypos.ef_xe22_noMu_Mon),
        L2EFChain_met('xe25_noMu', l2=Chain(name='L2_xe15_noMu', counter=205, seed='L1_XE10')),
        L2EFChain_met('xe25_tight_noMu_L1XE10', l2=Chain(name='L2_xe17_tight_noMu_L1XE10', counter=3399, seed='L1_XE10')),
        
        L2EFChain_met('xe30_noMu', l2=Chain(name='L2_xe20_noMu', counter=208, seed='L1_XE20')),
        L2EFChain_met('xe30_noMu_L1MU10XE20', l2=Chain(name='L2_xe20_noMu_L1MU10XE20', counter=82, seed='L1_MU10_XE20')),
        L2EFChain_met('xe30_noMu_L1EM14XE20', l2=Chain(name='L2_xe20_noMu_L1EM14XE20', counter=82, seed='L1_EM14_XE20')),
        L2EFChain_met('xe30_noMu_L1XE10', l2=Chain(name='L2_xe20_noMu_L1XE10', counter=3398, seed='L1_XE10')),
        
        L2EFChain_met('xe35_noMu', l2=Chain(name='L2_xe25_noMu', counter=89, seed='L1_XE20')),
        L2EFChain_met('xe35_noMu_L1MU10XE20', l2=Chain(name='L2_xe20_noMu_L1MU10XE20', counter=82, seed='L1_MU10_XE20')),
        L2EFChain_met('xe35_noMu_L1EM10XE20', l2=Chain(name='L2_xe20_noMu_L1EM10XE20', counter=82, seed='L1_EM10_XE20')),
        L2EFChain_met('xe35_medium_noMu', l2=Chain(name='L2_xe20_medium_noMu', counter=97, seed='L1_XE15')),
        
        L2EFChain_met('xe40_noMu', l2=Chain(name='L2_xe30_noMu', counter=209, seed='L1_XE30')),
        L2EFChain_met('xe40_loose_noMu', l2=Chain(name='L2_xe30_loose_noMu', counter=216, seed='L1_XE20')),
        L2EFChain_met('xe40_medium_noMu', l2=Chain(name='L2_xe25_medium_noMu', counter=99, seed='L1_XE20')),
        L2EFChain_met('xe40_medium_noMu_L1XE25', l2=Chain(name='L2_xe25_medium_noMu_L1XE25', counter=2090, seed='L1_XE25')),
        L2EFChain_met('xe40_tight_noMu', l2=Chain(name='L2_xe35_tight_noMu', counter=741, seed='L1_XE35')),

        L2EFChain_met('xe45_loose_noMu', l2=Chain(name='L2_xe20_loose_noMu', counter=214, seed='L1_XE20')),
        L2EFChain_met('xe45_loose_noMu_L1MU0J20XE20', l2=Chain(name='L2_xe20_loose_noMu_L1MU0J20XE20', counter=2175, seed='L1_MU0_J20_XE20')),
        L2EFChain_met('xe45_loose_noMu_L1MU4J20XE20', l2=Chain(name='L2_xe20_loose_noMu_L1MU4J20XE20', counter=2175, seed='L1_MU4_J20_XE20')),
        L2EFChain_met('xe45_tight_noMu', l2=Chain(name='L2_xe40_tight_noMu', counter=747, seed='L1_XE40')),
    
        L2EFChain_met('xe50_noMu', l2=Chain(name='L2_xe35_noMu', counter=227, seed='L1_XE35')),
        
        L2EFChain_met('xe55_noMu', l2=Chain(name='L2_xe45_noMu', counter=227, seed='L1_XE40')),
        L2EFChain_met('xe55_noMu_L1MU4J20XE35', l2=Chain(name='L2_xe35_noMu_L1MU4J20XE35', counter=576, seed='L1_MU4_J20_XE35')),
        L2EFChain_met('xe55_loose_noMu', l2=Chain(name='L2_xe25_loose_noMu', counter=227, seed='L1_XE25')),
        L2EFChain_met('xe55_medium_noMu', l2=Chain(name='L2_xe35_medium_noMu', counter=227, seed='L1_XE35')),
    
        L2EFChain_met('xe60_noMu', l2=Chain(name='L2_xe40_noMu', counter=307, seed='L1_XE40')),
        L2EFChain_met('xe60_tight_noMu', l2=Chain(name='L2_xe45_noMu', counter=420, seed='L1_XE40')),
        L2EFChain_met('xe60_verytight_noMu', l2=Chain(name='L2_xe55_noMu', counter=28, seed='L1_XE50')),
        
        L2EFChain_met('xe65_noMu', l2=Chain(name='L2_xe45_noMu', counter=218, seed='L1_XE40')),
        
        L2EFChain_met('xe70_noMu', l2=Chain(name='L2_xe50_noMu', counter=431, seed='L1_XE50')),
        L2EFChain_met('xe70_tight_noMu', l2=Chain(name='L2_xe60_noMu', counter=27, seed='L1_XE60')),
        
        L2EFChain_met('xe80_noMu', l2=Chain(name='L2_xe60_noMu', counter=285, seed='L1_XE60')),
        L2EFChain_met('xe80_tight_noMu', l2=Chain(name='L2_xe65_noMu', counter=907, seed='L1_XE60')),
        
        L2EFChain_met('xe90_noMu', l2=Chain(name='L2_xe70_noMu', counter=358, seed='L1_XE60')),
        L2EFChain_met('xe90_tight_noMu', l2=Chain(name='L2_xe75_noMu', counter=904, seed='L1_XE60')),
        
        L2EFChain_met('xe100_noMu', l2=Chain(name='L2_xe80_noMu', counter=945, seed='L1_XE60'), l2hypo=hypos.l2_xe80_noMu_Mon, efhypo=hypos.ef_xe100_noMu_Mon),
        L2EFChain_met('xe100_tight_noMu', l2=Chain(name='L2_xe85_noMu', counter=901, seed='L1_XE60')),
        
        L2EFChain_met('xe110_noMu', l2=Chain(name='L2_xe90_noMu', counter=752, seed='L1_XE60'), l2hypo=hypos.l2_xe90_noMu_Mon, efhypo=hypos.ef_xe110_noMu_Mon),
    
        ]    
    
    #unseeded chains
    MissingETs += [
        L2EFChain_met_hi('xe_NoCut_allL1_noMu_Mon', l2=Chain(name='L2_xe_NoCut_allL1_noMu_Mon', counter=271, seed=''), \
                         l2hypo=hypos.l2_noL2_noMu_Mon, efhypo=hypos.ef_noEF_noMu_Mon, l1threshold=['']),
        
        L2EFChain_met('xe20_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3256, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe25_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3257, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe30_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3258, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe35_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3259, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe40_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe45_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe50_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xe70_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3261, seed=''), l2hypo=hypos.l2_noL2_noMu),    
            
        L2EFChain_met('xe30_noMu_unseeded', l2=Chain(name='L2_xe20_noMu_unseeded', counter=237, seed='')),
        L2EFChain_met('xe40_noMu_unseeded', l2=Chain(name='L2_xe25_noMu_unseeded', counter=230, seed='')),
        L2EFChain_met('xe45_medium_noMu_noseed', l2=Chain(name='L2_xe30_medium_noMu_noseed', counter=215, seed='')),
        L2EFChain_met('xe55_medium_noMu_noseed', l2=Chain(name='L2_xe35_medium_noMu_noseed', counter=227, seed='')),
        ]

    #te chains
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
        MissingETs += [
            L2EFChain_met('te550', l2=Chain(name='L2_te250', counter=144, seed='L1_TE180')),
            L2EFChain_met('te700', l2=Chain(name='L2_te300', counter=145, seed='L1_TE300')),
            L2EFChain_met('te900', l2=Chain(name='L2_te400', counter=146, seed='L1_TE400')),
            L2EFChain_met('te1000', l2=Chain(name='L2_te500', counter=147, seed='L1_TE500')),
            L2EFChain_met('te1100', l2=Chain(name='L2_te600', counter=148, seed='L1_TE500')),
            L2EFChain_met('te1200', l2=Chain(name='L2_te700', counter=149, seed='L1_TE500')),
            ]
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        MissingETs += [
            L2EFChain_met('te550', l2=Chain(name='L2_te350', counter=144, seed='L1_TE300')),
            L2EFChain_met('te700', l2=Chain(name='L2_te500', counter=145, seed='L1_TE500')),
            L2EFChain_met('te900', l2=Chain(name='L2_te700', counter=146, seed='L1_TE700')),
            L2EFChain_met('te1000', l2=Chain(name='L2_te800', counter=147, seed='L1_TE800')),
            L2EFChain_met('te1100', l2=Chain(name='L2_te900', counter=148, seed='L1_TE800')),
            L2EFChain_met('te1200', l2=Chain(name='L2_te1000', counter=149, seed='L1_TE800')),
            ]
        
       #xs chains
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
        MissingETs += [
            L2EFChain_met('xs30_noMu', l2=Chain(name='L2_xs15_noMu', counter=158, seed='L1_XS15')),
            L2EFChain_met('xs90_noMu', l2=Chain(name='L2_xs50_noMu', counter=80, seed='L1_XS50')),
            L2EFChain_met('xs100_noMu', l2=Chain(name='L2_xs50_noMu', counter=65, seed='L1_XS50')),
            ]
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        MissingETs += [
            L2EFChain_met('xs30_noMu', l2=Chain(name='L2_xs15_noMu', counter=158, seed='L1_XE10')),
            L2EFChain_met('xs90_noMu', l2=Chain(name='L2_xs50_noMu', counter=80, seed='L1_XS50')),
            L2EFChain_met('xs100_noMu', l2=Chain(name='L2_xs60_noMu', counter=65, seed='L1_XS60')),
            ]
        

    MissingETs += [

        L2EFChain_met('xs35_noMu', l2=Chain(name='L2_xs25_noMu', counter=3050, seed='L1_XS25')), 
        
        L2EFChain_met('xs45_noMu', l2=Chain(name='L2_xs30_noMu', counter=159, seed='L1_XS30')),
        L2EFChain_met('xs45_medium_noMu', l2=Chain(name='L2_xs25_noMu', counter=43, seed='L1_XS25')),
        L2EFChain_met('xs45_loose_noMu_L1XE10', l2=Chain(name='L2_xs20_noMu_L1XE10', counter=68, seed='L1_XE10')),
        
        L2EFChain_met('xs60_noMu', l2=Chain(name='L2_xs45_noMu', counter=160, seed='L1_XS45')),
        L2EFChain_met('xs60_loose_noMu_L1XE10', l2=Chain(name='L2_xs20_noMu_L1XE10', counter=61, seed='L1_XE10')),
        
        L2EFChain_met('xs65_loose_noMu', l2=Chain(name='L2_xs35_noMu', counter=65, seed='L1_XS35')),
        
        L2EFChain_met('xs70_noMu', l2=Chain(name='L2_xs50_noMu', counter=161, seed='L1_XS50')), 
        L2EFChain_met('xs70_loose_noMu', l2=Chain(name='L2_xs35_noMu', counter=65, seed='L1_XS35')),
        L2EFChain_met('xs70_tight_noMu', l2=Chain(name='L2_xs55_noMu', counter=162, seed='L1_XS50')),
        
        L2EFChain_met('xs75_noMu', l2=Chain(name='L2_xs50_noMu', counter=163, seed='L1_XS50')),
        L2EFChain_met('xs75_loose_noMu', l2=Chain(name='L2_xs35_noMu', counter=65, seed='L1_XS35')),
        
        L2EFChain_met('xs80_noMu', l2=Chain(name='L2_xs50_noMu', counter=69, seed='L1_XS50')),
        L2EFChain_met('xs80_loose_noMu', l2=Chain(name='L2_xs35_noMu', counter=65, seed='L1_XS35')), 
        
        L2EFChain_met('xs120_noMu', l2=Chain(name='L2_xs65_noMu', counter=562, seed='L1_XS65')),
        ]
       #XS EM items
    MissingETs += [
        L2EFChain_met_em('xs45_noMu_unseeded', l2=Chain(name='L2_xs30_noMu_unseeded', counter=3001, seed=''), 
                         l2hypo=hypos.l2_xs30_noMu, efhypo=hypos.ef_xs45_noMu),
        L2EFChain_met_em('xs60_noMu_L1EM10XS45', l2=Chain(name='L2_xs45_noMu_L1EM10XS45', counter=219, seed='L1_EM10_XS45'), 
                         l2hypo=hypos.l2_xs45_noMu, efhypo=hypos.ef_xs60_noMu, l1threshold=['XS45', 'L2_mu4']),
        L2EFChain_met_em('xs60_noMu_unseeded', l2=Chain(name='L2_xs45_noMu_unseeded', counter=3000, seed=''), 
                         l2hypo=hypos.l2_xs45_noMu, efhypo=hypos.ef_xs60_noMu),
        L2EFChain_met_em('xs75_noMu_L1EM10XS50', l2=Chain(name='L2_xs55_noMu_L1EM10XS50', counter=220, seed='L1_EM10_XS50'), 
                         l2hypo=hypos.l2_xs55_noMu, efhypo=hypos.ef_xs75_noMu, l1threshold=['XS50', 'L2_mu4']),
        L2EFChain_met_em('xs70_noMu_L1EM10XS45', l2=Chain(name='L2_xs45_noMu_L1EM10XS45', counter=219, seed='L1_EM10_XS45'), 
                            l2hypo=hypos.l2_xs45_noMu, efhypo=hypos.ef_xs70_noMu, l1threshold=['XS45', 'L2_mu4']),
        ]
    #No L2 XS
    MissingETs += [
           
        L2EFChain_met('xs65_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xs70_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        L2EFChain_met('xs75_noL2_noMu', l2=Chain(name='L2_noL2_noMu', counter=3260, seed=''), l2hypo=hypos.l2_noL2_noMu),
        
        ]
       
#############################################################################
    MissingETs += [
        L2EFChain_met_xeL2Mon('xe60_L2FEB_val', l2=Chain(name='L2_xe40_L2FEB_val', counter=999, seed='L1_XE40'), l2fex=fexes.l2fex_FEB),
        L2EFChain_met_xeL2Mon('xe60_L2FEB_val_NoAlg', l2=Chain(name='L2_xe40_L2FEB_val', counter=999, seed='L1_XE40'), ef=Chain(counter=991), l2fex=fexes.l2fex_FEB, efhypo=hypos.ef_noEF),
        
        L2EFChain_met_xeL2Mon('xe60_tight_L2FEB_val', l2=Chain(name='L2_xe50_L2FEB_val', counter=961, seed='L1_XE50'), l2fex=fexes.l2fex_FEB),
        
        ###L2EFChain_met_xeL2Mon_EFunclean('xe50_LArNoiseBurst', l2=Chain(name='L2_xe50_L2FEB_val', counter=961, seed='L1_XE50'), l2fex=fexes.l2fex_FEB),
           
        
        L2EFChain_met_xeL2Mon('xe80_L2FEB_val', l2=Chain(name='L2_xe60_L2FEB_val', counter=987, seed='L1_XE60'), l2fex=fexes.l2fex_FEB),
        L2EFChain_met_xeL2Mon('xe80_L2FEB_val_NoAlg', l2=Chain(name='L2_xe60_L2FEB_val', counter=987, seed='L1_XE60'), ef=Chain(counter=993), l2fex=fexes.l2fex_FEB, efhypo=hypos.ef_noEF),
        
        L2EFChain_met_xeL2Mon('xeL2Mon_L1XE20', l2=Chain(name='L2_xeL2Mon_L1XE20_noL2', counter=998, seed='L1_XE20'), l2fex=fexes.l2fex_FEB, l2hypo=hypos.l2_noL2, efhypo=hypos.ef_noEF),
           L2EFChain_met_xeL2Mon('xeL2Mon_L1RD0FILLED', l2=Chain(name='L2_xeL2Mon_L1RD0FILLED_noL2', counter=997, seed='L1_RD0_FILLED'), l2fex=fexes.l2fex_FEB, l2hypo=hypos.l2_noL2, efhypo=hypos.ef_noEF),
        ] 
    
#END OF THE 2010-2011 MENU

#############################################################################
class L2EFChain_met_2SidedNoiseSupp(METTemplate):
    """
    Most regular MET Chains from pp_v4 on. Their configuration is very trivial.
    Uses FEB Met at L2,
    at EF use cell-based MET with two sided noise cut.
    In case the EF name has FEB in it, run FEB MET at EF

    Deduction rules:
    see: METTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are mis
sing.
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, effex=None,  efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)
        
             
        if l1threshold == None:
            l1threshold = ['']

        # always guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name_FEB(self.l2.chain_name)
                    
        if efhypo == None:
            efhypo, cut = hypo_from_chain_name_FEB(self.ef.chain_name)

        #if l2fex == None:
        #    l2fex    = fexes.l2fex_FEB
        #    l2muonfex= fexes.l2muonfex_FEB
        #else:
        #    l2muonfex= fexes.l2muonfex
            
        if effex == None:
            effex,efmuonfex = EFfex_from_chain_name(self.ef.chain_name)
            

        #l2te_step1 = self.l2.chain_name+'_step1'
        l2te_step1 = self.l2.chain_name+'_step1'            

        # To prevent warning about multiple chains definition 
        #if "xe" in self.l2.chain_name:
        #    l2te_step1 = 'L2_xe25_step1'
        #    if "unseeded" in self.l2.chain_name:
        #        l2te_step1 = 'L2_xe35_unseeded_step1'
        #    if "NoCut" in self.l2.chain_name:
        #        l2te_step1 = 'L2_noL2_step1'


        #L1 MET
        self.addSequence(l1threshold, [fexes.l2fex],l2te_step1).addSignature()

        #L1 Muons
        input_step2 = [l2te_step1, L2muonSeed]
        l2te_step2 = self.l2.chain_name+'_step2'

        l2te = self.l2.chain_name

        #noL2 chains, make sure not to run FEB
        if 'noL2' in self.l2.chain_name or "allL1" in self.l2.chain_name:
            #print "Betta1 ",self.l2.chain_name
            self.addSequence(input_step2, [fexes.l2muonfex,l2hypo],l2te).addSignature()

        else:
            #print "Betta2 ",self.l2.chain_name
            self.addSequence(input_step2, [fexes.l2muonfex],l2te_step2).addSignature()
            #FEB MET
            input_step3 = [l2te_step2]
            l2te_step3 = self.l2.chain_name+'_step3'
            self.addSequence(input_step3, [fexes.l2fex_FEB_ReadL2L1],l2te_step3).addSignature()

            print "here ",l2te_step2," ",input_step2

            
            #FEB Muons
            input_step4 = [l2te_step3, L2muonSeed]            
            self.addSequence(input_step4, [fexes.l2muonfex_FEB,l2hypo], l2te).addSignature()        
             
        # ef sequence (do not have to specify explicitly this are ef algs)
        efte = self.ef.chain_name

        l2seeds=['']
        self.addSequence(l2seeds,[effex, efhypo], efte).addSignature(sigcounter=3)

        ##efte_step1 = self.ef.chain_name+'_step1'
        ##self.addSequence(l2seeds,[effex], efte_step1).addSignature(sigcounter=3)
        ##ef_input_step2 = [efte_step1, EFmuonSeed]
        ##self.addSequence(ef_input_step2, [efmuonfex, efhypo], efte).addSignature(sigcounter=4)


        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')

                
        # that's it!
        pass
###--------------------
class L2EFChain_met_TopoCluster(METTemplate):
    """
    MET chains with FEB Met at L2, and topo-cluster MET at EF

    Deduction rules:
    see: METTemplate
    hypo instances --- from the cut value expressed in the chain name. There is exception if they are mis
sing.
    
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, effex=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)
        

        l1threshold = ['']

        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name_FEB(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name_TC(self.ef.chain_name)
            
            
        if effex == None:
            effex,efmuonfex = EFfex_from_chain_name(self.ef.chain_name)
            


        l2te_step1 = self.l2.chain_name+'_step1'            

        #L1 MET
        self.addSequence(l1threshold, [fexes.l2fex],l2te_step1).addSignature()

        #L1 Muons
        input_step2 = [l2te_step1, L2muonSeed]
        l2te_step2 = self.l2.chain_name+'_step2'


        l2te = self.l2.chain_name
        #noL2 chains, make sure not to run FEB
        if 'noL2' in self.l2.chain_name or "allL1" in self.l2.chain_name:
            self.addSequence(input_step2, [fexes.l2muonfex,l2hypo],l2te).addSignature()
        else:
            self.addSequence(input_step2, [fexes.l2muonfex],l2te_step2).addSignature()
            #FEB MET
            input_step3 = [l2te_step2]
            l2te_step3 = self.l2.chain_name+'_step3'
            self.addSequence(input_step3, [fexes.l2fex_FEB_ReadL2L1],l2te_step3).addSignature()
            
            #FEB Muons
            input_step4 = [l2te_step3, L2muonSeed]
            self.addSequence(input_step4, [fexes.l2muonfex_FEB,l2hypo], l2te).addSignature()        
        
        # ef sequence (do not have to specify explicitly this are ef algs)
        efte = self.ef.chain_name

        if "tclcw_allL1" in self.ef.chain_name:
            efte_step1 = self.ef.chain_name+'_step1'
            self.addSequence(['EF_FSTopoClusters'],[effex,efhypo], efte_step1).addSignature(sigcounter=3)
            jetfexes= [fexes.ef_antikt4_tc_had,fexes.ef_antikt4_tc_lcw,fexes.ef_antikt10_tc_em,fexes.ef_antikt10_tc_lcw]
            self.addSequence([efte_step1],jetfexes , efte).addSignature(sigcounter=4)

        else:
            self.addSequence(['EF_FSTopoClusters'],[effex, efhypo], efte).addSignature(sigcounter=3)

                          
        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')
        
        # that's it!
        pass


class L2EFChain_met_xeL2Mon_EFunclean(METTemplate):
    """
    Copy of L2EFChain_met with a few changes to run a l2fex

    """
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2fex=None, l2hypo=None, effex=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (but only if not given)
        METTemplate.__init__(self, sig, l2, ef)

        #if l1threshold == None:
        #    if(self.l2.lower_chain_name is '' or 'RD' in self.l2.lower_chain_name or 'MBTS' in self.l2.lower_chain_name):
        #        l1threshold = ['']
        #    elif('XE' in self.l2.lower_chain_name):
        #        l1threshold = ['XE20']
        #    elif('XS' in self.l2.lower_chain_name):
        #        l1threshold = ['XE20']
        #    else:
        #        l1threshold = [self.l2.lower_chain_name.split('_')[1]]
        
        if l1threshold == None:
            l1threshold = ['']

        # guess hypos
        if l2hypo == None:
            l2hypo, cut = hypo_from_chain_name_FEB(self.l2.chain_name)

        if efhypo == None:
            efhypo, cut = hypo_from_chain_name_FEB(self.ef.chain_name)

            
       # if l2fex == None:
       #     l2fex =    fexes.l2fex_FEB_ReadL2L1    
       #     l2muonfex= fe
       #     xes.l2muonfex_FEB
       # else:
       #     l2muonfex= fexes.l2muonfex


        if effex == None:
                effex= fexes.effex_2Sided
        
        #l2te_step1 = self.l2.chain_name+'_step1'
        #self.addSequence(l1threshold, [l2fex], l2te_step1).addSignature()
        #input_step2 = [l2te_step1, L2muonSeed]
        #l2te = self.l2.chain_name
        #self.addSequence(input_step2, [l2muonfex, l2hypo], l2te).addSignature()
        
        l2te_step1 = self.l2.chain_name+'_step1'            

        #L1 MET
        self.addSequence(l1threshold, [fexes.l2fex],l2te_step1).addSignature()

        #L1 Muons
        input_step2 = [l2te_step1, L2muonSeed]
        l2te_step2 = self.l2.chain_name+'_step2'

        #l2hypofeb = l2hypo
        #    l2hypo, cut = hypo_from_chain_name(self.l2.chain_name)
        #if l2hypo != None:

        l2te = self.l2.chain_name
        #noL2 chains, make sure not to run FEB
        if 'noL2' in self.l2.chain_name or "allL1" in self.l2.chain_name:
            l2te = self.l2.chain_name
            self.addSequence(input_step2, [fexes.l2muonfex,l2hypo],l2te_step2).addSignature()

        else:
            self.addSequence(input_step2, [fexes.l2muonfex],l2te_step2).addSignature()
            #FEB MET
            input_step3 = [l2te_step2]
            l2te_step3 = self.l2.chain_name+'_step3'
            self.addSequence(input_step3, [fexes.l2fex_FEB_ReadL2L1],l2te_step3).addSignature()
            
            #FEB Muons
            input_step4 = [l2te_step3, L2muonSeed]
                    
            self.addSequence(input_step4, [fexes.l2muonfex_FEB,l2hypo], l2te).addSignature()        
        


        efte = self.ef.chain_name

        if l1threshold == ['']:
            l2seeds=['']
        else:
            l2seeds=[EFmuonSeed]

        # For full scan finding
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
        roi_topo = DummyAlgo('RoiCreator_Topo')
        from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo
        cell_maker_fullcalo_topo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo",doNoise=0, AbsE=True, doPers=True)

        if not ('NoAlg' in self.ef.chain_name):
            self.addSequence('',[roi_topo, cell_maker_fullcalo_topo], 'EF_FSUncleanMET').addSignature()

        from TrigCaloRec.TrigCaloRecConfig import TrigLArNoisyROAlgConfig
        lar_noisy = TrigLArNoisyROAlgConfig()

        from TrigJetHypo.TrigJetHypoConfig import EFJetHypoNoiseConfig
        efhypo = EFJetHypoNoiseConfig('EFJetHypoNoise_j30')

        self.continueSequence([lar_noisy, efhypo], self.ef.chain_name).addSignature()

        self.addStreams('JetTauEtmiss')
        self.addGroups('BW:MET')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_MET')
        else:
            if ('_xe' in self.ef.chain_name):
                self.addGroups('RATE:MET')
            else:
                self.addGroups('RATE:TotalEnergy')


        #        self.addGroups('RATE:JetTauEtmiss')

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():

    MissingETs = [


        L2EFChain_met_2SidedNoiseSupp('xe10000_L1RD1',
                                      l2=Chain(name='L2_xe10000_L1RD1', counter=1019, seed='L1_RD1_FILLED')
                                      ,l2hypo=hypos.l2_xe10000_FEB,efhypo=hypos.ef_noEF), 
        

        L2EFChain_met_2SidedNoiseSupp('xe30', l2=Chain(name='L2_xe25', counter=647, seed='L1_XE20')),
        L2EFChain_met_2SidedNoiseSupp('xe30_noL2', l2=Chain(name='L2_noL2', counter=662, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 

        L2EFChain_met_2SidedNoiseSupp('xe40', l2=Chain(name='L2_xe35', counter=660, seed='L1_XE30')),
        L2EFChain_met_2SidedNoiseSupp('xe40_noL2', l2=Chain(name='L2_noL2', counter=662, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 



        ##
        ##L2EFChain_met_2SidedNoiseSupp('xe40_tight', l2=Chain(name='L2_xe35', counter=2080, seed='L1_XE35')),
        L2EFChain_met_2SidedNoiseSupp('xe40_tight', l2=Chain(name='L2_xe35_tight', counter=2080, seed='L1_XE35')),
        L2EFChain_met_2SidedNoiseSupp('xe45_tight', l2=Chain(name='L2_xe40_tight', counter=661, seed='L1_XE40')),
        
        L2EFChain_met_2SidedNoiseSupp('xe50'         , l2=Chain(name='L2_xe35'         , counter=649, seed='L1_XE30')),
        L2EFChain_met_2SidedNoiseSupp('xe50_unseeded', l2=Chain(name='L2_xe35_unseeded', counter=645, seed='')),
        L2EFChain_met_2SidedNoiseSupp('xe50_noL2'    , l2=Chain(name='L2_noL2'         , counter=663, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 
        
        L2EFChain_met_2SidedNoiseSupp('xe60'         , l2=Chain(name='L2_xe45'         , counter=650, seed='L1_XE40')),
        L2EFChain_met_2SidedNoiseSupp('xe60_unseeded', l2=Chain(name='L2_xe45_unseeded', counter=644, seed='')),

        L2EFChain_met_2SidedNoiseSupp('xe70', l2=Chain(name='L2_xe55', counter=651, seed='L1_XE50')),
        L2EFChain_met_2SidedNoiseSupp('xe70_tight', l2=Chain(name='L2_xe65_tight', counter=652, seed='L1_XE60')),

        L2EFChain_met_2SidedNoiseSupp('xe80', l2=Chain(name='L2_xe65', counter=653, seed='L1_XE60')),
        L2EFChain_met_2SidedNoiseSupp('xe80_tight', l2=Chain(name='L2_xe65', counter=654, seed='L1_XE60')),

        L2EFChain_met_2SidedNoiseSupp('xe90', l2=Chain(name='L2_xe75', counter=655, seed='L1_XE70')),
        L2EFChain_met_2SidedNoiseSupp('xe90_tight', l2=Chain(name='L2_xe65', counter=656, seed='L1_XE60')),

        L2EFChain_met_2SidedNoiseSupp('xe100', l2=Chain(name='L2_xe65', counter=657, seed='L1_XE60')),
        L2EFChain_met_2SidedNoiseSupp('xe100_tight', l2=Chain(name='L2_xe90_tight', counter=195, seed='L1_XE70')),        

        L2EFChain_met_2SidedNoiseSupp('xe110', l2=Chain(name='L2_xe90', counter=659, seed='L1_XE60')),


        L2EFChain_met_2SidedNoiseSupp('xe60T', l2=Chain(name='L2_xe45T', counter=791, seed='L1_XE40_BGRP7')),        
        L2EFChain_met_2SidedNoiseSupp('xe80T_loose', l2=Chain(name='L2_xe45T', counter=792, seed='L1_XE40_BGRP7')),
        L2EFChain_met_2SidedNoiseSupp('xe80T', l2=Chain(name='L2_xe55T', counter=793, seed='L1_XE50_BGRP7')),

        ##L2EFChain_met_2SidedNoiseSupp('te550',  l2=Chain(name='L2_te350',  counter=664, seed='L1_TE300')),
        ##L2EFChain_met_2SidedNoiseSupp('te700',  l2=Chain(name='L2_te500',  counter=665, seed='L1_TE500')),
        ##L2EFChain_met_2SidedNoiseSupp('te900',  l2=Chain(name='L2_te700',  counter=666, seed='L1_TE700')),
        ##L2EFChain_met_2SidedNoiseSupp('te1000', l2=Chain(name='L2_te800',  counter=667, seed='L1_TE800')),
        ##L2EFChain_met_2SidedNoiseSupp('te1100', l2=Chain(name='L2_te900',  counter=668, seed='L1_TE800')),
        ##L2EFChain_met_2SidedNoiseSupp('te1200', l2=Chain(name='L2_te1000', counter=669, seed='L1_TE800')),

        #L2EFChain_met_2SidedNoiseSupp('te600',  l2=Chain(name='L2_te600',  counter=664, seed='L1_TE300')),
        #L2EFChain_met_2SidedNoiseSupp('te1000', l2=Chain(name='L2_te1000',  counter=665, seed='L1_TE500')),
        #L2EFChain_met_2SidedNoiseSupp('te1400', l2=Chain(name='L2_te1400',  counter=666, seed='L1_TE700')),
        #L2EFChain_met_2SidedNoiseSupp('te1600', l2=Chain(name='L2_te1600',  counter=667, seed='L1_TE800')),        

        L2EFChain_met_2SidedNoiseSupp('te600',  l2=Chain(name='L2_te400',  counter=664, seed='L1_TE300')),
        L2EFChain_met_2SidedNoiseSupp('te1000', l2=Chain(name='L2_te600',  counter=665, seed='L1_TE500')),
        L2EFChain_met_2SidedNoiseSupp('te1400', l2=Chain(name='L2_te900',  counter=666, seed='L1_TE700')),
        L2EFChain_met_2SidedNoiseSupp('te1600', l2=Chain(name='L2_te1200',  counter=667, seed='L1_TE800')),        


        L2EFChain_met_2SidedNoiseSupp('xs30', l2=Chain(name='L2_xs15',  counter=670, seed='L1_XE20')),
        L2EFChain_met_2SidedNoiseSupp('xs45',  l2=Chain(name='L2_xs30', counter=671, seed='L1_XS30')),
        L2EFChain_met_2SidedNoiseSupp('xs60',  l2=Chain(name='L2_xs45', counter=672, seed='L1_XS45')),
        L2EFChain_met_2SidedNoiseSupp('xs45_L1XS45',  l2=Chain(name='L2_xs45', counter=1671, seed='L1_XS45')),
        L2EFChain_met_2SidedNoiseSupp('xs45_L1XS50',  l2=Chain(name='L2_xs50', counter=1672, seed='L1_XS50')),
        L2EFChain_met_2SidedNoiseSupp('xs45_L1XS55',  l2=Chain(name='L2_xs55', counter=1673, seed='L1_XS55')),
        L2EFChain_met_2SidedNoiseSupp('xs45_L1XS60',  l2=Chain(name='L2_xs60', counter=1674, seed='L1_XS60')),
        L2EFChain_met_2SidedNoiseSupp('xs50_L1XS50',  l2=Chain(name='L2_xs50', counter=1675, seed='L1_XS50')),
        L2EFChain_met_2SidedNoiseSupp('xs75',  l2=Chain(name='L2_xs50', counter=673, seed='L1_XS50')),
        L2EFChain_met_2SidedNoiseSupp('xs100', l2=Chain(name='L2_xs60', counter=674, seed='L1_XS60')),
        L2EFChain_met_2SidedNoiseSupp('xs120', l2=Chain(name='L2_xs65', counter=675, seed='L1_XS65')),


        #L2: Run L1 MET, no cut on it; EF: run 2SidedNoise MET, no cut
        L2EFChain_met_2SidedNoiseSupp('xe_NoCut_allL1', l2=Chain(name='L2_xe_NoCut_allL1', counter=269, seed=''),
                                      l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2,efhypo=hypos.ef_noEF),

        #L2: Run L1 MET, no cut on it; EF: run FEB, no cut
        L2EFChain_met_2SidedNoiseSupp('xe_NoCut_FEB_allL1', l2=Chain(name='L2_xe_NoCut_allL1', counter=1285, seed=''),
                                      l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2,efhypo=hypos.ef_FEB_noEF),


        
        ]


    ###TopoCluster MET
    MissingETs += [

        L2EFChain_met_TopoCluster('xe30_tclcw'      ,  l2=Chain(name='L2_xe25', counter=798, seed='L1_XE20')),
        L2EFChain_met_TopoCluster('xe30_tclcw_noL2' ,  l2=Chain(name='L2_noL2', counter=2112, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 

        L2EFChain_met_TopoCluster('xe35_tclcw_noL2' ,  l2=Chain(name='L2_noL2', counter=796, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 

        L2EFChain_met_TopoCluster('xe60_tclcw_tight_unseeded'      ,  l2=Chain(name='L2_xe50_tight_unseeded', counter=806, seed='')),
        #L2EFChain_met_TopoCluster('xe75_tclcw'      ,  l2=Chain(name='L2_xe45', counter=504, seed='L1_XE40')),
        
        L2EFChain_met_TopoCluster('xe65_tclcw'      ,  l2=Chain(name='L2_xe45', counter=2088, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe70_tclcw'      ,  l2=Chain(name='L2_xe45', counter=2082, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe85_tclcw'      ,  l2=Chain(name='L2_xe55', counter=3801, seed='L1_XE50')),
        L2EFChain_met_TopoCluster('xe75_tight1_tclcw',  l2=Chain(name='L2_xe55', counter=2085, seed='L1_XE50')),

        L2EFChain_met_TopoCluster('xe60_tclcw_unseeded' ,  l2=Chain(name='L2_xe45_unseeded', counter=2089, seed='')),
        L2EFChain_met_TopoCluster('xe70_tclcw_unseeded' ,  l2=Chain(name='L2_xe45_unseeded', counter=807, seed='')),
        L2EFChain_met_TopoCluster('xe70_tclcw_loose_unseeded' ,  l2=Chain(name='L2_xe40_unseeded', counter=6567, seed='')),
        L2EFChain_met_TopoCluster('xe80_tclcw_unseeded' ,  l2=Chain(name='L2_xe45_unseeded', counter=4765, seed='')),
        L2EFChain_met_TopoCluster('xe90_tclcw_unseeded' ,  l2=Chain(name='L2_xe45_unseeded', counter=807, seed='')),


        L2EFChain_met_TopoCluster('xe40_tclcw_noL2' , l2=Chain(name='L2_noL2', counter=808, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2),
        L2EFChain_met_TopoCluster('xe50_tclcw_noL2' , l2=Chain(name='L2_noL2', counter=803, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2),
        L2EFChain_met_TopoCluster('xe60_tclcw_noL2' , l2=Chain(name='L2_noL2', counter=505, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 

        L2EFChain_met_TopoCluster('xe70_tclcw_noL2' , l2=Chain(name='L2_noL2', counter=5500, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2),
        L2EFChain_met_TopoCluster('xe80_tclcw_noL2' , l2=Chain(name='L2_noL2', counter=5501, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2),

        L2EFChain_met_TopoCluster('xe80_tclcw_L2L1Check' , l2=Chain(name='L2_xe45_L2L1Check', counter=881, seed='L1_XE40'),l2hypo=hypos.l2_xe45_FEB_L2L1Check),


        L2EFChain_met_TopoCluster('xe70_tight_tclcw', l2=Chain(name='L2_xe65', counter=2086, seed='L1_XE60')),

        #L2EFChain_met_TopoCluster('xe100_tight_tclcw', l2=Chain(name='L2_xe90_tight', counter=640, seed='L1_XE70')),
        #L2EFChain_met_TopoCluster('xe100_loose_tclcw',  l2=Chain(name='L2_xe45', counter=501, seed='L1_XE40')),


        ### -New TopoCluster chains
        L2EFChain_met_TopoCluster('xe80_tclcw'          , l2=Chain(name='L2_xe55'      , counter=658, seed='L1_XE50')),
        L2EFChain_met_TopoCluster('xe80_tclcw_tight'    , l2=Chain(name='L2_xe65'      , counter=635, seed='L1_XE60')),
        L2EFChain_met_TopoCluster('xe80_tclcw_verytight', l2=Chain(name='L2_xe75'      , counter=636, seed='L1_XE70')),

        ##96354
        L2EFChain_met_TopoCluster('xe80_tclcw_veryloose', l2=Chain(name='L2_xe40'      , counter=100, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe80T_tclcw_veryloose', l2=Chain(name='L2_xe40T' , counter=95, seed='L1_XE35_BGRP7')),
        L2EFChain_met_TopoCluster('xe80_tclcw_veryloose_v2', l2=Chain(name='L2_xe45_v2'      , counter=3012, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe80T_tclcw_veryloose_v2', l2=Chain(name='L2_xe45T_v2'      , counter=3013, seed='L1_XE35_BGRP7')),
        

        L2EFChain_met_TopoCluster('xe90_tclcw'           , l2=Chain(name='L2_xe55'      , counter=641, seed='L1_XE50')),
        L2EFChain_met_TopoCluster('xe90_tclcw_loose'     , l2=Chain(name='L2_xe45'      , counter=4763, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe90_tclcw_tight'     , l2=Chain(name='L2_xe65'      , counter=637, seed='L1_XE60')),
        L2EFChain_met_TopoCluster('xe90_tclcw_verytight' , l2=Chain(name='L2_xe75'      , counter=638, seed='L1_XE70')),

        L2EFChain_met_TopoCluster('xe100_tclcw_loose'    , l2=Chain(name='L2_xe55'      , counter=642, seed='L1_XE50')),
        L2EFChain_met_TopoCluster('xe100_tclcw'          , l2=Chain(name='L2_xe65'      , counter=639, seed='L1_XE60')),
        L2EFChain_met_TopoCluster('xe100_tclcw_verytight', l2=Chain(name='L2_xe75'      , counter=805, seed='L1_XE70')),

        L2EFChain_met_TopoCluster('xe50_tclcw'      ,  l2=Chain(name='L2_xe35', counter=824, seed='L1_XE30')),
        
        L2EFChain_met_TopoCluster('xe50_tclcw_tight' ,       l2=Chain(name='L2_xe35_tight', counter=7000, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe55_tclcw'      ,        l2=Chain(name='L2_xe40', counter=799, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe55_tclcw_tight',        l2=Chain(name='L2_xe40_tight', counter=814, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe60_tclcw_loose_delayed',        l2=Chain(name='L2_xe40',  counter=121, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe60_tclcw_loose',        l2=Chain(name='L2_xe40',  counter=634, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe60_tclcw_veryloose',    l2=Chain(name='L2_xe35',  counter=827, seed='L1_XE30')),
        L2EFChain_met_TopoCluster('xe70_tclcw_veryloose',    l2=Chain(name='L2_xe40',  counter=648, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe70_tclcw_veryloose_v2', l2=Chain(name='L2_xe45_v2',  counter=8053, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe70T_tclcw_veryloose',   l2=Chain(name='L2_xe40T', counter=8056, seed='L1_XE35_BGRP7')),
        L2EFChain_met_TopoCluster('xe90_tclcw_veryloose',    l2=Chain(name='L2_xe40',  counter=801, seed='L1_XE35')),

        L2EFChain_met_TopoCluster('xe60_tclcw'           ,  l2=Chain(name='L2_xe45',  counter=812, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe60T_tclcw'          ,  l2=Chain(name='L2_xe45T', counter=8050, seed='L1_XE40_BGRP7')),
        L2EFChain_met_TopoCluster('xe60_tclcw_loose_v2'  ,  l2=Chain(name='L2_xe45_v2',  counter=8051, seed='L1_XE35')),
        L2EFChain_met_TopoCluster('xe60T_tclcw_loose_delayed'  ,  l2=Chain(name='L2_xe40T', counter=120, seed='L1_XE35_BGRP7')),
        L2EFChain_met_TopoCluster('xe60T_tclcw_loose'  ,      l2=Chain(name='L2_xe40T', counter=8062, seed='L1_XE35_BGRP7')),        
        L2EFChain_met_TopoCluster('xe60T_tclcw_loose_v2'  ,  l2=Chain(name='L2_xe45T_v2',  counter=8060, seed='L1_XE35_BGRP7')),

        L2EFChain_met_TopoCluster('xe70_tclcw_loose'     ,  l2=Chain(name='L2_xe45', counter=808, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe80_tclcw_loose'     ,  l2=Chain(name='L2_xe45', counter=865, seed='L1_XE40')),
        L2EFChain_met_TopoCluster('xe100_tclcw_veryloose',  l2=Chain(name='L2_xe45', counter=6505, seed='L1_XE40')),

        L2EFChain_met_TopoCluster('xe75_tclcw'      ,  l2=Chain(name='L2_xe55', counter=504, seed='L1_XE50')),
        
        
        #L2: Run L1 MET, no cut on it; EF: run TopoCluster MET, no cut
        L2EFChain_met_TopoCluster('xe_NoCut_tclcw_allL1', l2=Chain(name='L2_xe_NoCut_allL1', counter=1197, seed=''),
                                  l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2,efhypo=hypos.ef_noEF),

        
        #L2EFChain_met_TopoCluster('xs100_tclcw', l2=Chain(name='L2_xs60', counter=639, seed='L1_XS60')),
        #L2EFChain_met_TopoCluster('xs120_tclcw', l2=Chain(name='L2_xs65', counter=640, seed='L1_XS65')),

        
        L2EFChain_met_TopoCluster('xe35_tcem_noL2' ,  l2=Chain(name='L2_noL2', counter=2007, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 
        L2EFChain_met_TopoCluster('xe30_tcem_noL2' ,  l2=Chain(name='L2_noL2', counter=2008, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2), 

        L2EFChain_met_TopoCluster('xe80T_tclcw'       , l2=Chain(name='L2_xe55T', counter=795, seed='L1_XE50_BGRP7')),
                                                                                 
        L2EFChain_met_TopoCluster('xe100T_tclcw_loose', l2=Chain(name='L2_xe45T', counter=852, seed='L1_XE40_BGRP7')),
        L2EFChain_met_TopoCluster('xe100T_tclcw'      , l2=Chain(name='L2_xe55T', counter=839, seed='L1_XE50_BGRP7')),
        L2EFChain_met_TopoCluster('xe80T_tclcw_loose' , l2=Chain(name='L2_xe45T', counter=794, seed='L1_XE40_BGRP7')),
        
        ]
    MissingETs += [
        L2EFChain_met_TopoCluster('xe60_tclcw_noL2_wMu' , l2=Chain(name='L2_noL2', counter=357, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2, efhypo=hypos.ef_xe60_TC_tclcw_wMu),
        L2EFChain_met_TopoCluster('xe40_tclcw_noL2_wMu' , l2=Chain(name='L2_noL2', counter=358, seed=''),l2fex=fexes.l2fex,l2hypo=hypos.l2_noL2, efhypo=hypos.ef_xe40_TC_tclcw_wMu),
        ]

    MissingETs += [
        
        #L2EFChain_met_xeL2Mon_EFunclean('xe55_LArNoiseBurst', l2=Chain(name='L2_xe55_LArNoiseBurst', counter=532, seed='L1_XE50')),

        L2EFChain_met_xeL2Mon_EFunclean('xe45_LArNoiseBurst', l2=Chain(name='L2_xe45_LArNoiseBurst', counter=537, seed='L1_XE40')),
        
        #L2EFChain_met_xeL2Mon_EFunclean('xe55_LArNoiseBurst', l2=Chain(name='L2_xe55_LArNoiseBurst', counter=532, seed='L1_XE50_BGRP7')),
        L2EFChain_met_xeL2Mon_EFunclean('xe55_LArNoiseBurst', l2=Chain(name='L2_xe55_LArNoiseBurst', counter=532, seed='L1_XE50')),
        L2EFChain_met_xeL2Mon_EFunclean('xe60_LArNoiseBurst', l2=Chain(name='L2_xe60_LArNoiseBurst', counter=751, seed='L1_XE70')),
        
        ]


    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus():

    MissingETs = [   
        L2EFChain_met_xeL2Mon_EFunclean('xe45_LArNoiseBurst', l2=Chain(name='L2_xe45_LArNoiseBurst', counter=537, seed='L1_XE40')),
        L2EFChain_met_2SidedNoiseSupp('xe30', l2=Chain(name='L2_xe25', counter=647, seed='L1_XE20')),
        L2EFChain_met_2SidedNoiseSupp('xe80', l2=Chain(name='L2_xe65', counter=653, seed='L1_XE60')),
        ]
