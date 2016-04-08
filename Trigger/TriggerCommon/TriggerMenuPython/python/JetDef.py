# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SliceDef file for jet chains/signatures

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from ChainTemplate import *
from Combined import CombinedChain2

hypos = FreeStore()
fexes = FreeStore()
l2chains = FreeStore()

# this is added here as a quick hack, te real solution would be to move all the MBTS jet chains to the Combined signatures
# and combine the botj L2 mbMbts and EF jet
# The definition must match Mbts config from MnBiasDef.py
# TB


from AthenaCommon.SystemOfUnits import GeV

import re

class ChainTemplate_jet(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig,
                 l15te='', l15unpackingfex=None, l15algo=None, l15hypo=None,
                 l2te='', l2unpackingfex=None, l2algo=None, l2hypo=None, l2tomerge='',
                 efinputobjectfexes=[], efalgo=None, efhypo=None, efinputtype='', efobjecttype='',
                 groups=None, streams=None):
        """
        Base template for all jet chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        if ef!=None:            
            ef.replace_if_default('name', 'EF_%s'%sig)
        
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every Jet chain should have
        if streams == None:
            if 'jetcalib' in sig:
                self.addStreams('jetcalibdelayed')
            elif 'delayed' in sig:
                self.addStreams('haddelayed')
            else:    
                self.addStreams('jets')
        else:
            self.addStreams(streams)

        # This is a hardcoded input for j10000_c4cchad_L1RD0
        if 'j10000' in l2.name:
            self.l1threshold = 'J10'
        elif ('LOF' in l2.name):
            self.l1threshold = 'HA40'
        elif ('2L1MU' in l2.name):
            self.l1threshold = 'J15'            
            
        elif not l15te:
            self.deduceL1threshold(l2.seed)

        else:
            self.l1threshold=''

        self.l15te = l15te
        self.l15unpackingfex = l15unpackingfex
        self.l15algofex = l15algo
        self.l15hypo = l15hypo
        self.l2te = l2te
        self.l2unpackingfex = l2unpackingfex
        self.l2algofex = l2algo
        self.l2hypo = l2hypo
        self.efinputobjectfexes = efinputobjectfexes
        self.efinputtype = efinputtype
        self.efobjecttype = efobjecttype
        self.efalgo = efalgo
        self.efhypo = efhypo


        if not hasattr(l2chains,self.l2.chain_name):
            l2chains.update(dict([ (l2.name, l2), ]))


        if self.l15te:                                                                         ##
            self.defineJetL15Sequence()                                                        ##

        ##
        if self.l2te:
            if len(l2tomerge)==1 or (self.l2unpackingfex is not None) or self.l15te: 
                self.defineJetL2Sequence()                                                     ##
            else:                                                                              ##  TO BE MOVED IN IF ABOVE ONCE ReuseChain() is available
                self.merge(l2names=l2tomerge)                                                  ##

        if ef is not None:
            if 'NoAlg' not in ef.name:
                self.defineJetEFSequence()
                
        if groups == None:
            groups = ['RATE:SingleJet', 'BW:Jets']

        self.addGroups(*groups)
        pass

    def deduceL1threshold(self,l1_name):
        re1='(\\d*)'# jet multiplicity (default is empty string=1) 
        re2='([FJ]+)'# jet type (J or FJ)                          
        re3='(\\d+)'# threshold (integer)

        multypethresh_re = re.compile('(_)'+re1+re2+re3,re.DOTALL)
        multypethresh_list = multypethresh_re.finditer(l1_name)

        self.l1threshold=''
        for item in multypethresh_list:
            mul=item.group(2)
            type=item.group(3)
            thresh=item.group(4)
            if 'FJ' in type:
                type='JF'

            prefix=''    

            if self.l1threshold:
                prefix='_'+prefix
                mul='_'+mul

            self.l1threshold+='%s%s%s' % (prefix,type,thresh)
            # Multiplicity is not part of the L1 threshold names
            #self.l1threshold+='%s%s%s' % (mul,type,thresh)

    def defineJetL15Sequence(self):
        l15unpackingseq = 'L2_%s'%self.l15unpackingfex.getName()
        l15algoseq = 'L2_%s'%self.l15algofex.getName()
        self.addSequence('',[self.l15unpackingfex],l15unpackingseq)
        self.continueSequence([self.l15algofex],l15algoseq)
        self.continueSequence([self.l15hypo],self.l15te).addSignature()

    def defineJetL2Sequence(self):
        input_te = self.l15te or self.l1threshold
        unpack_te = ''
        
        if self.l2unpackingfex is not None:
            if self.l15te : input_te = 'L2_%s'%self.l15algofex.getName()
            unpack_te = 'L2_AllRoiUnpacking_%s' % input_te
            self.addSequence(input_te, [self.l2unpackingfex], unpack_te)
        l2algoseq=self.l2te+'_Jets'
        self.addSequence(unpack_te or input_te, [self.l2algofex], l2algoseq)
        self.continueSequence([self.l2hypo],self.l2te).addSignature()    
        
    def defineJetEFSequence(self):
        self.addSequence(self.efinputtype, self.efinputobjectfexes, self.efobjecttype) # not in a chain signature
        self.continueSequence([self.efalgo, self.efhypo], self.ef.chain_name).addSignature()


class ChainTemplate_jetTopo(ChainTemplate_jet):
    """
    This template defines topological jet triggers
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig,
                 l15te='', l15unpackingfex=None, l15algo=None, l15hypo=None,
                 l2te='', l2unpackingfex=None, l2algo=None, l2hypo=None, l2tomerge='', l2topohypo=None,
                 efinputobjectfexes=[], efalgo=None, efhypo=None, efinputtype='', efobjecttype='', eftopohypo=None,
                 groups=None, streams=None):

        self.l2topohypo = l2topohypo
        self.eftopohypo = eftopohypo

        ChainTemplate_jet.__init__(self, sig, l2, ef,
                                   l15te, l15unpackingfex, l15algo, l15hypo,
                                   l2te, l2unpackingfex, l2algo, l2hypo, l2tomerge,
                                   efinputobjectfexes, efalgo, efhypo,
                                   efinputtype, efobjecttype,
                                   groups, streams)
        
    def defineJetL2Sequence(self):
        ChainTemplate_jet.defineJetL2Sequence(self)
        if self.l2topohypo:
            self.addSequence(self.l15te,[self.l2topohypo],self.l2.chain_name).addSignature()

    def defineJetEFSequence(self):
        self.addSequence(self.efinputtype, self.efinputobjectfexes, self.efobjecttype) # not in a chain signature
        self.continueSequence([self.efalgo, self.efhypo, self.eftopohypo], self.ef.chain_name).addSignature()


class ChainTemplate_jetHV(ChainTemplate_jet):
    """
    This template defines HV chain
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig,
                 l15te='', l15unpackingfex=None, l15algo=None, l15hypo=None,
                 l2te='', l2unpackingfex=None, l2algo=None, l2hypo=None, l2tomerge='', l2hvhypo1=None, l2hvhypo2=None, thetype='',
                 efinputobjectfexes=[], efalgo=None, efhypo=None, efinputtype='', efobjecttype='',
                 groups=None, streams=None):

        self.l2hvhypo1 = l2hvhypo1
        self.l2hvhypo2 = l2hvhypo2
        self.thetype = thetype

        ChainTemplate_jet.__init__(self, sig, l2, ef,
                                   l15te, l15unpackingfex, l15algo, l15hypo,
                                   l2te, l2unpackingfex, l2algo, l2hypo, l2tomerge,
                                   efinputobjectfexes, efalgo, efhypo,
                                   efinputtype, efobjecttype,
                                   groups, streams)

        
    # L2 is considerably different    
    def defineJetL2Sequence(self):
        # Not needed as it's over-written
        #ChainTemplate_jet.defineJetL2Sequence(self)

        # HV trigger - CalRatio 
        if self.thetype == 'CalRatio':
            self.addSequence(self.l15te or self.l1threshold,[self.l2algofex, self.l2hypo],self.l2.chain_name+"step1")
            self.continueSequence([self.l2hvhypo1],self.l2.chain_name+"step2")
            self.continueSequence([fexes.starB, self.l2hvhypo2], self.l2.chain_name).addSignature()

        # HV trigger - Trackless
        elif self.thetype == 'Trackless':
            self.addSequence(self.l15te or self.l1threshold, [self.l2algofex, self.l2hypo], self.l2.chain_name+"step1")
            self.continueSequence([fexes.starB], self.l2.chain_name+"step2").addSignature()
            self.addSequence('MU10', [TrigmuFastConfig("Muon"), MufastHypoConfig("Muon","10GeV_v11a")], '%s_MU10'%self.l2.chain_name).addSignature()
            self.addSequence( [self.l2.chain_name+"step2", '%s_MU10'%self.l2.chain_name], [fexes.l2_mu_jet, hypos.l2_mu_jet], self.l2.chain_name).addSignature()
        # HV trigger - MuonCluster
        elif 'MuonCluster' in self.thetype:
            if '10' in self.thetype:
                myThresh = 'MU10'
            elif '4' in self.thetype:
                myThresh = 'MU4'
            else:
                myThresh = 'MU6'
                
            if 'ExtendedEta' in self.l2.chain_name:
                muonHypo = hypos.l2_MuonCluster_ExtendedEta
            else:
                muonHypo = hypos.l2_MuonCluster

            self.addSequence(myThresh,[fexes.l2_SiTrackFinder_muonIsoB], self.l2.chain_name+"step1")
            self.addSequence(self.l15te or self.l1threshold, [self.l2algofex, self.l2hypo], self.l2.chain_name+"step2")          
            self.addSequence([self.l2.chain_name+"step1",self.l2.chain_name+"step2"],[fexes.l2_MuonCluster], self.l2.chain_name+"step3")
            self.addSequence(self.l2.chain_name+"step3",[muonHypo], self.l2.chain_name).addSignature() 
            
    def defineJetEFSequence(self):
        # HV trigger - CalRatio         
        if self.thetype == 'CalRatio':
            self.addSequence(self.efinputtype, self.efinputobjectfexes, self.efobjecttype) # not in a chain signature
            self.continueSequence([self.efalgo, self.efhypo], self.ef.chain_name+"step1").addSignature()
            self.continueSequence([fexes.LoF, hypos.EF_LoF], self.ef.chain_name).addSignature()

        # Nothing for the other HV triggers at the EF...
            
class ChainTemplate_jetdEta(ChainTemplate_jet):
    """
    This template defines dEta chain
    """
    def __init__(self, sig, l2, ef=DeduceChainConfig,
                 l15te='', l15unpackingfex=None, l15algo=None, l15hypo=None,
                 l2te='', l2unpackingfex=None, l2algo=None, l2hypo=None, l2tomerge='', l2detahypo=None, l2detafex=None,
                 efinputobjectfexes=[], efalgo=None, efhypo=None, efinputtype='', efobjecttype='', efdetahypo=None, efdetafex=None,
                 groups=None, streams=None):

        self.l2detahypo = l2detahypo
        self.efdetahypo = efdetahypo
        self.l2detafex = l2detafex
        self.efdetafex = efdetafex
        
        ChainTemplate_jet.__init__(self, sig, l2, ef,
                                   l15te, l15unpackingfex, l15algo, l15hypo,
                                   l2te, l2unpackingfex, l2algo, l2hypo, l2tomerge,
                                   efinputobjectfexes, efalgo, efhypo,
                                   efinputtype, efobjecttype,
                                   groups, streams)
        
    def defineJetL15Sequence(self):
        ChainTemplate_jet.defineJetL15Sequence(self)
        if self.l2te == '' and self.l2detahypo:
            self.continueSequence([self.l2detahypo], self.l2.chain_name).addSignature()

    def defineJetL2Sequence(self):
        ChainTemplate_jet.defineJetL2Sequence(self)
        if self.l2detahypo:
            self.addSequence(self.l15te or self.l1threshold,[self.l2detafex, self.l2detahypo],self.l2.chain_name).addSignature()
            
    def defineJetEFSequence(self):
        self.addSequence(self.efinputtype, self.efinputobjectfexes+[self.efdetafex], self.efobjecttype) # not in a chain signature
        self.continueSequence([self.efalgo, self.efhypo, self.efdetahypo], self.ef.chain_name).addSignature()


class ChainTemplate_L2jet(ChainTemplate_jet):
    """
    This template defines only L2 chain
    """
    def __init__(self, sig, l2=None,
                 l15te='', l15unpackingfex=None, l15algo=None, l15hypo=None,
                 l2te='', l2unpackingfex=None, l2algo=None, l2hypo=None, l2tomerge=[]):

        ef = None
        if 'L2_' not in sig:
            ef = DeduceChainConfig
            
        ChainTemplate_jet.__init__(self, sig, l2, ef,
                                   l15te, l15unpackingfex, l15algo, l15hypo,
                                   l2te, l2unpackingfex, l2algo, l2hypo, l2tomerge)


#L1.5 algorithms
#-----------------------------------------------------------------------------------------------
from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_TT
fexes.l15_unpacking_TT    = T2L1Unpacking_TT()

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT
fexes.l15_antikt4_TT_em        = T2CaloFastJet_a4TT()
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a4TT_JESCalib      
fexes.l15_antikt4_TT_had        = T2CaloFastJet_a4TT_JESCalib()
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_a10TT
fexes.l15_antikt10_TT_em        = T2CaloFastJet_a10TT()

from TrigJetHypo.TrigJetHypoConfig import L2FullScanMultiJetAllTE
from TrigJetHypo.TrigJetHypoConfig import L2FullScanMultiJetAllTE_doBasicCleaning
#-----------------------------------------------------------------------------------------------

#L2 algorithms
#-----------------------------------------------------------------------------------------------
from TrigT2CaloJet.TrigT2CaloJetConfig import T2AllRoiUnpacking_test
fexes.l2_unpacking_cc = T2AllRoiUnpacking_test()

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_CC_JESCalib
fexes.l2_antikt4_cc_had = T2CaloFastJet_CC_JESCalib()
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloFastJet_CC_JESCalib_L15, T2CaloFastJet_CC_JESCalib_L15had
fexes.l2_antikt4_cc_had_l15 = T2CaloFastJet_CC_JESCalib_L15()
fexes.l2_antikt4_cc_had_l15had = T2CaloFastJet_CC_JESCalib_L15had()
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_noise
fexes.l2_cone4_cc_em = T2CaloJet_Jet_noise()

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_JESCalib_noiseCut
fexes.l2_cone4_cc_had = T2CaloJet_Jet_JESCalib_noiseCut()
fexes.l2_cone4_cc_had.fillLayerInfo = True

#from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_JESCalib_noiseCut_writeLayers
#fexes.l2_cone4_cc_had = T2CaloJet_Jet_JESCalib_noiseCut_writeLayers()


from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
fexes.l2_cone4_uc_em = T2CaloJet_Jet()
from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_JESCalib
fexes.l2_cone4_uc_had = T2CaloJet_Jet_JESCalib()

from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
from TrigJetHypo.TrigJetHypoConfig import L2JetHypo_doBasicCleaning
from TrigJetHypo.TrigJetHypoConfig import L2JetHypoNoise
from TrigJetHypo.TrigJetHypoConfig import FullScanL2HTHypo
from TrigJetHypo.TrigJetHypoConfig import L2JetHypoNoiseTight

from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import L2HVJetHypoAllCuts, L2HVJetHypoTrk, L2HVJetHypo

from TrigmuFast.TrigmuFastConfig import *
from TrigMuonHypo.TrigMuonHypoConfig import *

#-----------------------------------------------------------------------------------------------

#EF algorithms
#-----------------------------------------------------------------------------------------------
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
fexes.roi_topo = DummyAlgo('RoiCreator_Topo')
fexes.roi = DummyAlgo('RoiCreator')


from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo
fexes.cell_maker_fullcalo_topo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo",doNoise=0, AbsE=True, doPers=True)

from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo
fexes.cell_maker_fullcalo = TrigCaloCellMaker_jet_fullcalo()

from TrigCaloRec.TrigCaloRecConfig import TrigCaloTowerMaker_jet
fexes.tower_maker = TrigCaloTowerMaker_jet()
# TopoTowers
from TrigCaloRec.TrigCaloRecConfig import TrigCaloTopoTowerMaker_jet
fexes.topoTower_maker_fullcalo = TrigCaloTopoTowerMaker_jet('TrigCaloTopoTowerMaker_jet_fullscan')

from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
fexes.topocluster_maker_fullcalo = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_fullscan',doMoments=True,doLC=True)

# TopoTowers
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCell2ClusterMapperBase
fexes.cell_cluster_mapper = TrigCaloCell2ClusterMapperBase('TrigCaloCell2ClusterMapper_topoTower_fullscan')

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt2_topo
fexes.ef_antikt2_tc_em  = TrigJetRec_AntiKt2_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo
fexes.ef_antikt4_tc_em  = TrigJetRec_AntiKt4_topo()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo_calib
fexes.ef_antikt4_tc_had  = TrigJetRec_AntiKt4_topo_calib()
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topo_calib_lowpt
fexes.ef_antikt4_tc_hadloose  = TrigJetRec_AntiKt4_topo_calib_lowpt()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_lctopo
fexes.ef_antikt4_tc_lcw  = TrigJetRec_AntiKt4_lctopo()
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topotower
fexes.ef_antikt4_tt_em  = TrigJetRec_AntiKt4_topotower()
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4_topotower_calib
fexes.ef_antikt4_tt_had  = TrigJetRec_AntiKt4_topotower_calib()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt10_topo
fexes.ef_antikt10_tc_em  = TrigJetRec_AntiKt10_topo()
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt10_lctopo
fexes.ef_antikt10_tc_lcw  = TrigJetRec_AntiKt10_lctopo()

#HI
from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt2HI
fexes.ef_antikt2_hi_em = TrigJetRec_AntiKt2HI()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4HI
fexes.ef_antikt4_hi_em = TrigJetRec_AntiKt4HI()

from TrigJetRec.TrigJetRecConfig import TrigJetRec_AntiKt4
fexes.ef_antikt4_tj_em = TrigJetRec_AntiKt4()

# LarNoiseBurst
from TrigCaloRec.TrigCaloRecConfig import TrigLArNoisyROAlgConfig
fexes.ef_larnoiseburst0_uc_had = TrigLArNoisyROAlgConfig()

from TrigJetHypo.TrigJetHypoConfig import EFCentJetHypo, EFFwdJetHypo
from TrigJetHypo.TrigJetHypoConfig import EFCentFullScanMultiJetHypo
from TrigJetHypo.TrigJetHypoConfig import EFJetHypoNoiseConfig

# jet cleaning
from TrigJetHypo.TrigJetHypoConfig import EFJetHypo_doBasicCleaning
from TrigJetHypo.TrigJetHypoConfig import EFFwdJetHypo_doBasicCleaning

# Hadronic HT
from TrigJetHypo.TrigEFHTHypoConfig import EFHT_HAD

# Dijet Mass
from TrigJetHypo.TrigEFJetMassYConfig import EFJetMassY 

# Dijet Mass dEta
from TrigJetHypo.TrigEFJetMassDEtaConfig import EFJetMassDEta, EFJetMassDEta2J7, EFJetMassDEta2J10,  EFJetMassDEta2J25, EFJetMassDEta2J30, EFJetMassDEta2J35, EFJetMassDEta2J40

# Delta Eta
from TrigJetHypo.TrigJetDEtaFexConfig  import *
from TrigJetHypo.TrigJetDEtaHypoConfig import *

# Fexless Delta Eta
from TrigJetHypo.TrigJetDEtaMjjAllTEConfig import *

# SiTrack
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet
fexes.sitrack = TrigSiTrack_Jet()
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetB
fexes.starB = TrigL2SiTrackFinder_JetB()

# HV triggers - LoF
from TrigEFLongLivedParticles.TrigEFLongLivedParticlesConfig import TrigLoFRemovalConfig
fexes.LoF = TrigLoFRemovalConfig()
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import TrigLoFRemovalHypoConfig
hypos.EF_LoF = TrigLoFRemovalHypoConfig()

# HV triggers - MuJet
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig  import L2MuonJetFex
fexes.l2_mu_jet = L2MuonJetFex()
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import *
hypos.l2_mu_jet = getTrigMuonJetHypoInstance("L2","2012") 

# HV triggers - MuonCluster
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoB
fexes.l2_SiTrackFinder_muonIsoB = TrigL2SiTrackFinder_muonIsoB()
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConfig import MuonClusterConfig
fexes.l2_MuonCluster = MuonClusterConfig(IDtracking="STRATEGY_B")
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoConfig
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterAllMSHypoConfig
hypos.l2_MuonCluster = MuonClusterHypoConfig()
hypos.l2_MuonCluster_ExtendedEta = MuonClusterAllMSHypoConfig() 

# change min Et for HIV2
if 'HI_v2' in TriggerFlags.triggerMenuSetup():
    fexes.ef_antikt4_tc_em.AlgTools['JetFinalPtCut'].MinimumSignal = 5.*GeV
    fexes.ef_antikt4_tc_had.AlgTools['JetFinalPtCut'].MinimumSignal = 5.*GeV
    fexes.ef_antikt4_hi_em.AlgTools['JetFinalPtCut'].MinimumSignal = 5.*GeV

#------------------------------------------------------------------------------------------------

class JetChainFromName:
    def __init__(self):

        # New June 26: 3rd entry in dictionary is for forward jet algorithms
        # Only way to implement a different fj hypo without changing names in the dictionary
        self.non_standard_hypos_dict = {"LArNoiseBurst":['L2JetHypoNoise(\'L2JetHypoNoise_{0}{1}\', {1}*GeV)','EFJetHypoNoiseConfig(\'EFJetHypoNoise_{0}{1}\', {1}*GeV)', 'EFJetHypoNoiseConfig(\'EFJetHypoNoise_{0}{1}\', {1}*GeV)'],
                                        "testCleaning":['L2JetHypo_doBasicCleaning(\'L2JetHypo_doBasicCleaning_{0}{1}\', {1}*GeV)','EFJetHypo_doBasicCleaning(\'EFJetHypo_doBasicCleaning_{0}{1}\', {1}*GeV)', 'EFFwdJetHypo_doBasicCleaning(\'EFFwdJetHypo_doBasicCleaning_{0}{1}\', {1}*GeV)'],
                                        "LArNoiseBurstT":['L2JetHypoNoiseTight(\'L2JetHypoNoiseTight_{0}{1}\', {1}*GeV)','EFJetHypoNoiseConfig(\'EFJetHypoNoise_{0}{1}\', {1}*GeV)', 'EFJetHypoNoiseConfig(\'EFJetHypoNoise_{0}{1}\', {1}*GeV)'],
                                        }


        self.algo_dict = {"a":"antikt",
                          "c":"cone",
                          "u":"larnoiseburst",
                          }
        
        self.objectsequence_dict = {"tc":['',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo],'EF_FSTopoClusters'],
                                    "uc":['',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo],'EF_FSUnclean'],
                                    "tt":['EF_FSTopoClusters', [fexes.cell_cluster_mapper, fexes.tower_maker, fexes.topoTower_maker_fullcalo], 'EF_FSTopoTowerClusters'],
                                    "hi":['',[fexes.roi, fexes.cell_maker_fullcalo_topo, fexes.tower_maker],'EF_FSTowersHI'],
                                    "tj":['',[fexes.roi, fexes.cell_maker_fullcalo, fexes.tower_maker],'EF_FSTowers'],
                                    }


    def createJetChain(self, sig_name, l2_name, l1_name, chain_counter, ef_groups=None, ef_streams=None):

        self.setupStandardChain(sig_name, l2_name, l1_name, chain_counter)

        if ef_groups is None:
            if len(self.ef_jet_mul)>1 or self.ef_jet_mul[0]:
                ef_groups = ['RATE:MultiJet', 'BW:Jets']
                

        return ChainTemplate_jet(sig_name, l2=self.l2chain, ef=self.efchain,
                                 l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                 l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge,
                                 efinputobjectfexes=self.efobjectsequence[1], efalgo=self.efalgofex, efhypo=self.efhypo,
                                 efinputtype=self.efobjectsequence[0], efobjecttype=self.efobjectsequence[2],
                                 groups=ef_groups, streams=ef_streams)


    def createJetTopoChain(self, sig_name, l2_name, l1_name, chain_counter, ef_groups=None, ef_streams=None):

        self.setupStandardChain(sig_name, l2_name, l1_name, chain_counter)

        self.deduceTopohypos(sig_name,l2_name)

        return ChainTemplate_jetTopo(sig_name, l2=self.l2chain, ef=self.efchain,
                                   l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                   l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge, l2topohypo=self.l2topohypo,
                                   efinputobjectfexes=self.efobjectsequence[1], efalgo=self.efalgofex, efhypo=self.efhypo, eftopohypo=self.eftopohypo,
                                   efinputtype=self.efobjectsequence[0], efobjecttype=self.efobjectsequence[2],
                                   groups=ef_groups, streams=ef_streams)


    
    def createJetHVChain(self, sig_name, l2_name, l1_name, chain_counter, ef_groups=None, ef_streams=None):
        
        self.setupStandardChain(sig_name, l2_name, l1_name, chain_counter)

        self.deduceHVhypos(sig_name,l2_name)
        
        return ChainTemplate_jetHV(sig_name, l2=self.l2chain, ef=self.efchain,
                                   l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                   l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge, l2hvhypo1=self.l2hvhypo1,
                                   l2hvhypo2=self.l2hvhypo2, thetype=self.thetype, efinputobjectfexes=self.efobjectsequence[1], efalgo=self.efalgofex, efhypo=self.efhypo,
                                   efinputtype=self.efobjectsequence[0], efobjecttype=self.efobjectsequence[2],
                                   groups=ef_groups, streams=ef_streams)

    def createJetdEtaChain(self, sig_name, l2_name, l1_name, chain_counter, ef_groups=None, ef_streams=None):

        self.setupStandardChain(sig_name, l2_name, l1_name, chain_counter)

        self.deducedEtahypos(sig_name,l2_name)
        
        return ChainTemplate_jetdEta(sig_name, l2=self.l2chain, ef=self.efchain,
                                     l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                     l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge, l2detahypo=self.l2detahypo,
                                     l2detafex=self.l2detafex,
                                     efinputobjectfexes=self.efobjectsequence[1], efalgo=self.efalgofex, efhypo=self.efhypo, efdetahypo=self.efdetahypo, efdetafex=self.efdetafex,
                                     efinputtype=self.efobjectsequence[0], efobjecttype=self.efobjectsequence[2],
                                     groups=ef_groups, streams=ef_streams)    
    
    
    def createJetdEtaMjjChain(self, sig_name, l2_name, l1_name, chain_counter, ef_groups=None, ef_streams=None):

        self.setupStandardChain(sig_name, l2_name, l1_name, chain_counter)

        self.deducedEtaMjjhypos(sig_name,l2_name)

        return ChainTemplate_jetdEta(sig_name, l2=self.l2chain, ef=self.efchain,
                                     l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                     l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge, l2detahypo=self.l2detahypo,
                                     l2detafex=self.l2detafex,
                                     efinputobjectfexes=self.efobjectsequence[1], efalgo=self.efalgofex, efhypo=self.efhypo, efdetahypo=self.efdetahypo, efdetafex=self.efdetafex,
                                     efinputtype=self.efobjectsequence[0], efobjecttype=self.efobjectsequence[2],
                                     groups=ef_groups, streams=ef_streams)    

    def setupStandardChain(self, sig_name, l2_name, l1_name, chain_counter):
        
        self.parseL15chain(l2_name)
        self.parseL2chain(l2_name)
        self.parseEFchain(sig_name)

        if hasattr(l2chains,l2_name):
            self.l2chain = getattr(l2chains,l2_name)
            assert self.l2chain.seed==l1_name, "%s already defined with L1 seed %s" % (l2_name,self.l2chain.seed)
        else:
            self.l2chain = Chain(name=l2_name,counter=chain_counter,seed=l1_name)
            
        self.deduceL15fexhypo() #TO BE MOVED IN THE ELSE: ONCE ReuseChain is available 
        self.deduceL2fexhypo()  #TO BE MOVED IN THE ELSE: ONCE ReuseChain is available 
        
        self.deduceEFfexhypo()
        self.efchain = Chain(counter=chain_counter)

    def createL2JetChain(self, sig_name, l1_name, chain_counter):

        self.parseL15chain(sig_name)
        self.parseL2chain(sig_name)
        if 'L2_' not in sig_name:
            l2_name='L2_'+sig_name
        else:
            l2_name = sig_name

        if hasattr(l2chains,l2_name):
            l2chain = getattr(l2chains,l2_name)
            assert self.l2chain.seed==l1_name, "%s already defined with L1 seed %s" % (l2_name,self.l2chain.seed)
        else:
            l2chain = Chain(name=l2_name,counter=chain_counter,seed=l1_name)

        self.deduceL15fexhypo() #TO BE MOVED IN THE ELSE: ONCE ReuseChain is available 
        self.deduceL2fexhypo()  #TO BE MOVED IN THE ELSE: ONCE ReuseChain is available 

        return ChainTemplate_L2jet(sig_name, l2=l2chain,
                                   l15te=self.l15te, l15unpackingfex=self.l15unpackingfex, l15algo=self.l15algofex, l15hypo=self.l15hypo,
                                   l2te=self.l2te, l2unpackingfex=self.l2unpackingfex, l2algo=self.l2algofex, l2hypo=self.l2hypo, l2tomerge=self.l2_tomerge)


    def createNoAlgJetChain(self, sig_name, l1_name, chain_counter):

        if 'L2_' not in sig_name:
            l2_name='L2_'+sig_name
        else:
            l2_name = sig_name

        if hasattr(l2chains,l2_name):
            l2chain = getattr(l2chains,l2_name)
            assert self.l2chain.seed==l1_name, "%s already defined with L1 seed %s" % (l2_name,self.l2chain.seed)
        else:
            l2chain = Chain(name=l2_name,counter=chain_counter,seed=l1_name)

        return ChainTemplate_L2jet(sig_name, l2=l2chain)
    
        
    def parseEFchain(self,sig_txt):
        # txt='j30_a4tcem'   EXAMPLE
        
        re1='(\\d*)'# jet multiplicity (default is empty string=1) 
        re2='([fj]+)'# jet type (j or fj)                          
        re3='(\\d+)'# threshold (integer)
        re4='(_)'# fixed separator
        re5='([a-z])'# jet algorithm (single character)                                                                                
        re6='(\\d+)'# jet radius (integer)                                                                                             
        re7='([a-z])'# input object: first character (see dictionary self.objectsequence_dict)
        re8='([a-z])'# input object: second character (this cannot be a word of undefined length because calibration word is following)
        re9='((?:[a-z][a-z]+)*)'# calibration word (em or had)  ### hack for HI: default calib can be empty string to allow the same chain names as in _v2v3 
        re10='((_eta\\d+)*)' # pick up eta cut (but not deta)

        rg = re.compile(re1+re2+re3+re4+re5+re6+re7+re8+re9+re10,re.DOTALL)
        m = rg.search(sig_txt)
        assert m is not None, "cannot parse signature name %s" % sig_txt

        self.ef_jet_mul=[]
        self.ef_jet_type=[]
        self.ef_thresh=[]
        self.ef_algo  = ''
        self.ef_radius= ''
        self.ef_object= ''
        self.ef_calib = ''
        self.ef_etamax = ''

        if m:                                              
            self.ef_algo=m.group(5)
            self.ef_radius=m.group(6)
            self.ef_object=m.group(7)+m.group(8)
            if m.group(9):    ## hack for HI
                self.ef_calib=m.group(9)
            else:
                self.ef_calib='em'
            # if eta cut exists
            if m.group(10):
                self.ef_etamax = str(float(m.group(10)[-1])/10 + float(m.group(10)[-2]))
                                     
            multypethresh_re = re.compile(re1+re2+re3+('_'),re.DOTALL)            ## parsing all sequence (jet_mul)(jet_type)(thresh)  (e.g j110_2j55_j20)
            multypethresh_list = multypethresh_re.finditer(sig_txt)               ## N.B thresholds should be in DECREASING order    

            for item in multypethresh_list:
                self.ef_jet_mul.append(item.group(1))
                self.ef_jet_type.append(item.group(2))
                self.ef_thresh.append(item.group(3))

            thresholds = []
            for myThresh in self.ef_thresh:
                thresholds += [int(myThresh)]
                
        self.specialefhypo = ''
        special_hypo_key = sig_txt.split('_')[-1]                                ## check if chain requires non standard l2 hypo
        if special_hypo_key in self.non_standard_hypos_dict:                     ##
            self.specialefhypo = special_hypo_key                                ##


    def deduceEFfexhypo(self):

        object = self.ef_object
        self.efobjectsequence = self.objectsequence_dict[object]

        algofex_name = 'ef_%s%s_%s_%s' % (self.algo_dict[self.ef_algo],self.ef_radius,self.ef_object,self.ef_calib)
        self.efalgofex = getattr(fexes, algofex_name)
        assert self.efalgofex, 'impossible to deduce the EF jet algorithm'

        hypo_name = ''
        hypo_name_hi = ''
        tot_mul = 0

        for mul, type, thresh in zip(self.ef_jet_mul,self.ef_jet_type,self.ef_thresh):
            hypo_name += '_%s%s%s' % (mul,type,thresh)                                       ## deducing hypo name
            hypo_name_hi += '%s%s%s' % (mul,type,thresh)
            tot_mul += int(mul) if mul else 1                                                ## computing total multiplicity

        if object=="hi":
            hypo_name = hypo_name_hi + '_%s%s%s_%s' % (self.ef_algo,self.ef_radius,self.ef_object,"EFFS")
        elif object=="tj":
            hypo_name = hypo_name_hi + '_%s%s_%s' % (self.ef_algo,self.ef_radius,"EFFS")
        else:
            hypo_name = 'ef' + hypo_name + self.specialefhypo

        if self.ef_etamax:
            hypo_name = hypo_name + '_eta' + self.ef_etamax.replace('.', 'p')
            
        if not hasattr(hypos, hypo_name):

            single_type = self.ef_jet_type[0]
            single_thresh = self.ef_thresh[0]

            if self.specialefhypo:
                efhypo = self.non_standard_hypos_dict[self.speciall2hypo][1]
                # Nasty, but check for forward jet hypothesis
                if tot_mul==1 and single_type == 'fj':
                    efhypo = self.non_standard_hypos_dict[self.speciall2hypo][2]
            elif tot_mul==1:
                if 'j'==single_type:
                    efhypo = 'EFCentJetHypo(\'EFJetHypo_%s\', int({1})*GeV)' % hypo_name
                    if self.ef_etamax:
                        efhypo = 'EFCentJetHypo(\'EFJetHypo_%s\', int({1})*GeV, etaMax=float(self.ef_etamax))' % hypo_name
                elif 'fj'==single_type:
                        efhypo = 'EFFwdJetHypo(\'EFJetHypo_%s\', int({1})*GeV)' %hypo_name
            elif self.ef_etamax:
                efhypo = 'EFCentJetHypo(\'EFJetHypo_%s\', int({1})*GeV, etaMax=float(self.ef_etamax))' % hypo_name            
            else:
                efhypo = 'EFCentFullScanMultiJetHypo(\'EFjetHypo_%s\',multiplicity=tot_mul,'\
                         'ef_thrs=[int(thresh)*GeV for thresh,mul in zip(self.ef_thresh,self.ef_jet_mul) '\
                         'for i in range(int(mul) if mul else 1)] )' %hypo_name
                    ## build list of thresholds; for each threshold, int(mul) items. example: [110,55,55,20]

            hypos.update( dict([(hypo_name,  eval(efhypo.format(single_type,single_thresh))),]) )

                
        self.efhypo = getattr(hypos, hypo_name)
        assert self.efhypo, 'impossible to deduce the EF hypothesis algorithm'
        

    def parseL2chain(self,l2_txt):
        #txt='L2_j30_c4ccem'
        
        re1='(\\d*)'#  jet multiplicity (default is empty string=1) 
        re2='([fj]+)'#  jet type (j or fj)                          
        re3='(\\d+)'# threshold (integer)
        re4='(_)'#  fixed separator
        re5='([a-z])'#  jet algorithm (single character)                                                                                
        re6='(\\d+)'#  jet radius (integer)                                                                                             
        re7='([a-z])'#  input object: first character (see dictionary self.objectsequence_dict)
        re8='([a-z])'#  input object: second character (this cannot be a word of undefined length because calibration word is following)
        re9='((?:[a-z][a-z]+))'# calibration word (em or had 

        rg = re.compile(re1+re2+re3+re4+re5+re6+re7+re8+re9,re.DOTALL)
        m = rg.search(l2_txt)

        self.l2_jet_mul  = []
        self.l2_jet_type = []
        self.l2_thresh   = []
        self.l2_algo     = ''
        self.l2_radius   = ''
        self.l2_object   = ''
        self.l2_calib    = ''
              
        self.l2te = ''
        self.l2_tomerge = []

        if m:
            self.l2_algo=m.group(5)
            self.l2_radius=m.group(6)
            self.l2_object=m.group(7)+m.group(8)
            self.l2_calib=m.group(9)

            self.l2te = 'L2'

            l2_txt_only = ''
            l2te_suffix = ''
            if self.l15te:
                l2_txt_only = l2_txt.split(self.l15_object)[1]
                l2te_suffix = '_L2FS'
                
            multypethresh_re = re.compile(re1+re2+re3+'(_)',re.DOTALL)
            multypethresh_list = multypethresh_re.finditer(l2_txt_only or l2_txt)

            l2chain_suffix = self.l2_algo+self.l2_radius+self.l2_object+self.l2_calib
            l2chain_suffix += (l2_txt_only or l2_txt).split(l2chain_suffix)[1]
            
            for item in multypethresh_list:
                self.l2te+='_'
                self.l2te+=item.group(0)
                self.l2_jet_mul.append(item.group(1))
                self.l2_jet_type.append(item.group(2))
                self.l2_thresh.append(item.group(3))
                self.l2_tomerge+=['L2_%s%s_%s' % (item.group(2),item.group(3),l2chain_suffix)]*int(item.group(1) if item.group(1) else 1)
                                                                                                    
            self.l2te+='%s%s%s%s%s' % (self.l2_algo,self.l2_radius,self.l2_object,self.l2_calib,l2te_suffix)

            self.speciall2hypo = ''
            special_hypo_key = l2_txt.split('_')[-1]                                 ## check if chain requires non standard l2 hypo
            if special_hypo_key in self.non_standard_hypos_dict:                     ##
                self.speciall2hypo = special_hypo_key                                ##
                self.l2te += '_%s'%self.speciall2hypo


    def deduceL2fexhypo(self):

        self.l2unpackingfex = None
        self.l2algofex = None
        self.l2hypo = None

        if self.l2te:
            if 'a' in self.l2_algo:
                unpackingfex_name = 'l2_unpacking_%s'%self.l2_object
                self.l2unpackingfex = getattr(fexes,unpackingfex_name)
                assert self.l2unpackingfex, 'impossible to deduce the L2 unpacker'

            algofex_name = 'l2_%s%s_%s_%s' % (self.algo_dict[self.l2_algo],self.l2_radius,self.l2_object,self.l2_calib)

            if ('a' in self.l2_algo) and self.l15te:
                algofex_name += '_l15' if not 'had' in self.l15te else '_l15had'

            self.l2algofex = getattr(fexes, algofex_name)
            assert self.l2algofex, 'impossible to deduce the L2 jet algorithm'
                                    
            hypo_name = ''
            tot_mul = 0

            if not self.l15te and self.l2unpackingfex is None:
                if self.speciall2hypo:
                    l2hypo = self.non_standard_hypos_dict[self.speciall2hypo][0]
                else:
                    l2hypo = 'L2JetHypo(\'L2JetHypo_{0}{1}\', int({1})*GeV)'

                type = self.l2_jet_type[0]
                thresh = self.l2_thresh[0]
                hypo_name = 'l2_%s%s%s' % (type,thresh,self.speciall2hypo)
                    
                if not hasattr(hypos, hypo_name):
                    hypos.update( dict([(hypo_name, eval(l2hypo.format(type,thresh))),]) )
                                
            else:
                for mul, type, thresh in zip(self.l2_jet_mul,self.l2_jet_type,self.l2_thresh):
                    hypo_name += '_%s%s%s' % (mul,type,thresh)
                    tot_mul += int(mul) if mul else 1
                hypo_name = 'l2%s_L2FS'%hypo_name                                     

                if 'fj' not in self.l2_jet_type:
                    eta_min = 0.0
                    eta_max = 3.2
                elif 'j' not in self.l2_jet_type:
                    eta_min = 3.2
                    eta_max = 5.0
                                        
                l2hypo = 'L2FullScanMultiJetAllTE(\'L2JetAllTE_FullScan_%s\','\
                                                 'multiplicity=tot_mul,'\
                                                 'l2_thrs=[int(thresh)*GeV'\
                                                         ' for thresh,mul in zip(self.l2_thresh,self.l2_jet_mul)'\
                                                         ' for i in range(int(mul) if mul else 1)],'\
                                                 'etaMin=%s, etaMax=%s)' % (hypo_name,eta_min,eta_max)

                if not hasattr(hypos, hypo_name):
                    hypos.update( dict([(hypo_name, eval(l2hypo)),]) )

            self.l2hypo = getattr(hypos, hypo_name)
            assert self.l2hypo, 'impossible to deduce the L2 hypothesis algorithm'


    def parseL15chain(self,l2_txt):
        #txt='L2_J30_a4TTem'
        
        re1='(\\d*)'#  jet multiplicity (default is empty string=1) 
        re2='([fj]+)'#  jet type (j or fj)
        re3='(\\d+)'# threshold (integer)
        re4='(_)'#  fixed separator
        re5='([a-z])'#  jet algorithm (single character)                                                                                
        re6='(\\d+)'#  jet radius (integer)                                                                                             
        re7='([A-Z])'#  input object: first character (see dictionary self.objectsequence_dict,capital letters for L1.5)
        re8='([A-Z])'#  input object: second character (this cannot be a word of undefined length because calibration word is following)
        re9='((?:[a-z][a-z]+))'# calibration word (em or had 

        rg = re.compile(re1+re2+re3+re4+re5+re6+re7+re8+re9,re.DOTALL)
        m = rg.search(l2_txt)

        self.l15_jet_mul  = []
        self.l15_jet_type = []
        self.l15_thresh   = []
        self.l15_algo     = ''
        self.l15_radius   = ''
        self.l15_object   = ''
        self.l15_calib    = ''

        self.l15te = ''
        
        if m:
            self.l15_algo=m.group(5)
            self.l15_radius=m.group(6)
            self.l15_object=m.group(7)+m.group(8)
            self.l15_calib=m.group(9)

            self.l15te = 'L2_L15'

            if self.l15te:
                l15_txt = l2_txt.split(self.l15_object)[0]

            multypethresh_re = re.compile(re1+re2+re3+'(_)',re.DOTALL)
            multypethresh_list = multypethresh_re.finditer(l15_txt)
            
            for item in multypethresh_list:
                self.l15te+='_'
                self.l15te+=item.group(0)
                self.l15_jet_mul.append(item.group(1))
                self.l15_jet_type.append(item.group(2))
                self.l15_thresh.append(item.group(3))
                
                self.l15te+='%s%s%s%s' % (self.l15_algo,self.l15_radius,self.l15_object,self.l15_calib)

            special_hypo_key = l2_txt.split('_')[-1]                                 ## check if chain requires non standard l2 hypo
            if special_hypo_key in self.non_standard_hypos_dict:                     ##
                self.l15te += '_%s'%special_hypo_key

    def deduceL15fexhypo(self):

        self.l15unpackingfex = None
        self.l15algofex = None
        self.l15hypo = None

        if self.l15te:
            unpackingfex_name = 'l15_unpacking_%s'%self.l15_object
            self.l15unpackingfex = getattr(fexes,unpackingfex_name)
            assert self.l15unpackingfex, 'impossible to deduce the L1.5 unpacker'

            algofex_name = 'l15_%s%s_%s_%s' % (self.algo_dict[self.l15_algo],self.l15_radius,self.l15_object,self.l15_calib)
            self.l15algofex = getattr(fexes, algofex_name)
            assert self.l15algofex, 'impossible to deduce the L1.5 jet algorithm'

            hypo_name = ''
            tot_mul = 0

            for mul, type, thresh in zip(self.l15_jet_mul,self.l15_jet_type,self.l15_thresh):
                hypo_name += '_%s%s%s' % (mul,type,thresh)
                tot_mul += int(mul) if mul else 1
            hypo_name = 'l15%s'%hypo_name

            #allow deta chains to use all jets
            rg = re.compile('deta\\d+')
            deta_match = rg.search(self.l2chain.name)
            if deta_match :
                hypo_name=hypo_name+'_all_eta'
                self.l15te=self.l15te+'_all_eta'
            
            if not hasattr(hypos, hypo_name):
                
                if 'fj' not in self.l15_jet_type:
                    eta_min = 0.0
                    eta_max = 3.2
                elif 'j' not in self.l15_jet_type:
                    eta_min = 3.2
                    eta_max = 5.0

                if deta_match :
                    eta_min = 0.0
                    eta_max = 5.0                      

                    
                l15hypo = 'L2FullScanMultiJetAllTE(\'L2JetAllTE_FullScan_%s\','\
                                                  'multiplicity=tot_mul,'\
                                                  'l2_thrs=[int(thresh)*GeV'\
                                                           ' for thresh,mul in zip(self.l15_thresh,self.l15_jet_mul)'\
                                                           ' for i in range(int(mul) if mul else 1)],'\
                                                  'etaMin=%s, etaMax=%s)' % (hypo_name,eta_min,eta_max)

                if 'testCleaning' in self.l15te:
                    hypo_name = 'doBasicCleaning_'+hypo_name
                    l15hypo = 'L2FullScanMultiJetAllTE_doBasicCleaning(\'L2JetAllTE_FullScan_%s\','\
                                                  'multiplicity=tot_mul,'\
                                                  'l2_thrs=[int(thresh)*GeV'\
                                                           ' for thresh,mul in zip(self.l15_thresh,self.l15_jet_mul)'\
                                                           ' for i in range(int(mul) if mul else 1)],'\
                                                  'etaMin=%s, etaMax=%s)' % (hypo_name,eta_min,eta_max)

                hypos.update( dict([(hypo_name, eval(l15hypo)),]) )

            self.l15hypo = getattr(hypos, hypo_name)
            assert self.l15hypo, 'impossible to deduce the L1.5 hypothesis algorithm'

    def parseHT(self, sig_name, l2name):

        l2hthypo_name = ''
        efhthypo_name = ''

        rg = re.compile('(_ht)(\\d+)')
        l2_match = rg.search(l2name)
        ef_match = rg.search(sig_name)

        if l2_match:
            l2ht_thresh = l2_match.group(2)
            l2hthypo_name = 'l2_ht%s' % l2ht_thresh

            if not hasattr(hypos, l2hthypo_name):
                hypos.update(dict([ (l2hthypo_name,  FullScanL2HTHypo('L2HTHypo_FullScan_ht%s'%l2ht_thresh,int(l2ht_thresh)*GeV))  ]))

        if ef_match:
            efht_thresh = ef_match.group(2)
            efhthypo_name = 'ef_ht%s' % efht_thresh

            if not hasattr(hypos, efhthypo_name):
                hypos.update(dict([ (efhthypo_name,  EFHT_HAD("EFHT_HAD_ht%s"%efht_thresh,HT_cut=int(efht_thresh)*GeV))  ]))

        return (l2hthypo_name,efhthypo_name)

    def parseDijet(self, sig_name, l2name):

        l2dijethypo_name = ''
        efdijethypo_name = ''

        rg = re.compile('(_m)(\\d+)(_dy)(\\d+)')

##        l2_match = rg.search(l2name)  ##no dijet algo at L2 for now
        ef_match = rg.search(sig_name)

        if ef_match:
            efmjj_thresh = ef_match.group(2)
            efdy_thresh = ef_match.group(4)
            efdijethypo_name = 'ef_mjj%sdy%s' % (efmjj_thresh,efdy_thresh)

            if not hasattr(hypos, efdijethypo_name):
                hypos.update(dict([ (efdijethypo_name,  EFJetMassY("EFJetMassY_mjj%sdy%s" % (efmjj_thresh,efdy_thresh),
                                                                   mjj_cut=int(efmjj_thresh)*GeV, ystar_cut=float(efdy_thresh)/20.))  ]))

        return (l2dijethypo_name,efdijethypo_name)

    def deduceTopohypos(self, sig_name, l2name):

        self.l2topohypo = None
        self.eftopohypo = None

        l2hthypo_name,efhthypo_name = self.parseHT(sig_name,l2name)
        l2dijethypo_name,efdijethypo_name = self.parseDijet(sig_name,l2name)

        l2hypo_name = l2hthypo_name or l2dijethypo_name
        efhypo_name = efhthypo_name or efdijethypo_name
            
        if l2hypo_name:
            self.l2topohypo = getattr(hypos, l2hypo_name)
            assert self.l2topohypo, 'impossible to deduce the L2 Topological hypothesis algorithm'

        if efhypo_name:
            self.eftopohypo = getattr(hypos, efhypo_name)
            assert self.eftopohypo, 'impossible to deduce the EF Topological hypothesis algorithm'

    def deduceHVhypos(self, sig_name, l2name):

        self.l2hvhypo1 = None
        self.l2hvhypo2 = None

        if 'LOF' in l2name:
            self.thetype = 'CalRatio'

        elif 'Trackless' in l2name:
            self.thetype = 'Trackless'

        elif '2L1MU' in l2name:
            self.thetype = 'MuonCluster'
            if '2L1MU10' in l2name:
                self.thetype = 'MuonCluster10'
            if '2L1MU4' in l2name:
                self.thetype = 'MuonCluster4'

        if self.thetype == 'CalRatio':

            l2hvhypo1_name = 'l2_hv_j30'
            l2hvhypo2_name = 'l2_hvtrk_j30'
            
            if not hasattr(hypos, l2hvhypo1_name):
                hypos.update(dict([ (l2hvhypo1_name,  L2HVJetHypo('L2HVJetHypo_j30', l2_thr=30*GeV, l2_lrat=1.2)) ]))

            if not hasattr(hypos, l2hvhypo2_name):
                hypos.update(dict([ (l2hvhypo2_name,  L2HVJetHypoTrk('L2HVJetHypoTrk_j30')) ]))

            self.l2hvhypo1 = getattr(hypos, l2hvhypo1_name)
            assert self.l2hvhypo1, 'impossible to deduce the L2 HV hypothesis algorithm for JetHypo_j30'
            
            self.l2hvhypo2 = getattr(hypos, l2hvhypo2_name)
            assert self.l2hvhypo2, 'impossible to deduce the L2 HV hypothesis algorithm for JetHypoTrk_j30'

    def deducedEtahypos(self, sig_name, l2name):

        rg = re.compile('(_deta)(\\d+)')
        l2_match = rg.search(l2name)
        ef_match = rg.search(sig_name)

        self.l2detahypo = None
        self.efdetahypo = None
        self.l2detafex  = None
        self.efdetafex  = None
        
        if l2_match:
            etacut = l2_match.group(2)
            l2deta_thresh = 'deta%sp%s' %(etacut[0], etacut[1])

            # Construct a string with the jet thresholds
            l2string = ''
            for index in range(len(self.ef_jet_mul)):
                if l2string:
                    l2string += '_'
                l2string += self.l2_jet_mul[index]
                l2string += 'j'
                l2string += self.l2_thresh[index]
                
            l2detahypo_name = 'l2_%s_%s' %(l2string, l2deta_thresh)

            # Look for the desired hypo in the collection
            if not hasattr(hypos, l2detahypo_name):
                hypos.update(dict([ (l2detahypo_name,  L2DEtaHypo('L2DEtaHypo_%s' %l2detahypo_name, 1))]))
                
            self.l2detahypo = getattr(hypos, l2detahypo_name)
            assert self.l2detahypo, 'impossible to deduce the L2 dEta hypothesis algorithm'

            # Same thing, but for the FEX
            cutValue = float(etacut[1])/10 + float(etacut[0])

            if not hasattr(fexes, l2detahypo_name):
                fexes.update(dict([ (l2detahypo_name,  L2DEtaFex('L2DEtaFex_%s' %l2detahypo_name, cutValue))]))

            self.l2detafex = getattr(fexes, l2detahypo_name)
            assert self.l2detafex, 'impossible to deduce the L2 dEta Fex'
                

        if ef_match:
            etacut = ef_match.group(2)
            efdeta_thresh = 'deta%sp%s' %(etacut[0], etacut[1])

            # Construct a string with the jet thresholds
            efstring = ''
            for index in range(len(self.ef_jet_mul)):
                if efstring:
                    efstring += '_'
                efstring += self.ef_jet_mul[index]
                efstring += 'j'
                efstring += self.ef_thresh[index]
            
            efdetahypo_name = 'ef_%s_%s' %(efstring, efdeta_thresh)

            # Look for the desired hypo in the collection
            if not hasattr(hypos, efdetahypo_name):
                hypos.update(dict([ (efdetahypo_name,  EFDEtaHypo('EFDEtaHypo_%s' %efdetahypo_name, 1))]))

            self.efdetahypo = getattr(hypos, efdetahypo_name)
            assert self.efdetahypo, 'impossible to deduce the EF dEta hypothesis algorithm'

            # Same thing, but for the FEX
            cutValue = float(etacut[1])/10 + float(etacut[0])
            if not hasattr(fexes, efdetahypo_name):
                fexes.update(dict([ (efdetahypo_name,  EFDEtaFex('EFDEtaFex_%s' %efdetahypo_name, cutValue))]))

            self.efdetafex = getattr(fexes, efdetahypo_name)
            assert self.efdetafex, 'impossible to deduce the EF dEta Fex'

    def deducedEtaMjjhypos(self, sig_name, l2name):
        has_mjj_l2=True
        has_mjj_ef=True
 
        rg = re.compile('(_mjj)(\\d+)(deta)(\\d+)')
        rg2 = re.compile('(deta)(\\d+)')        

        l2_match = rg.search(l2name)        
        if not l2_match :
            l2_match = rg2.search(l2name)
            has_mjj_l2=False

        ef_match = rg.search(sig_name)
        if not ef_match :
            ef_match = rg2.search(sig_name)
            has_mjj_ef=False
        
        ##rg = re.compile('(_mjj)(\\d+)(deta)(\\d+)')
        ##l2_match = rg.search(l2name)
        ##ef_match = rg.search(sig_name)

        self.l2detahypo = None
        self.efdetahypo = None
        #There should be no fexes for dEta
        self.l2detafex  = None
        self.efdetafex  = None
        
        if l2_match:
            mjjcut='0'
            etacut=''
            if has_mjj_l2 :
                mjjcut = l2_match.group(2)
                etacut = l2_match.group(4)
            else :
                etacut = l2_match.group(2)

            ##mjjcut = l2_match.group(2)
            ##etacut = l2_match.group(4)


            l2deta_thresh = 'deta%sp%s' %(etacut[0], etacut[1])
            l2mjj_thresh = 'Mjj%s' %(mjjcut)

            # Construct a string with the jet thresholds
            l2string = ''
            for index in range(len(self.l15_jet_mul)):
            #for index in range(len(self.l2_jet_mul)):
                if l2string:
                    l2string += '_'
                #l2string += self.l2_jet_mul[index]
                l2string += self.l15_jet_mul[index]
                l2string += 'j'
                #l2string += self.l2_thresh[index]
                l2string += self.l15_thresh[index]
                
            l2detahypo_name = 'l2_%s_%s_%s' %(l2string, l2mjj_thresh, l2deta_thresh)


            L2JetDEtaMjjAllTEType = L2JetDEtaMjjAllTE2J15

            #L2MjjDEtaFexType = L2MjjDEta2J15Fex
            ##One would need to update Trigger/TrigHypothesis/TrigJetHypo/python/TrigJetDEta(Hypo|Fex)Config.py
            ##To add additional 2jXX types
            #if(l2string == '2aj15') :
            #    L2MjjDEtaFexType = L2MjjDEta2J15Fex

            # Look for the desired hypo in the collection
            etaCutValue = float(etacut[1])/10 + float(etacut[0])
            if not hasattr(hypos, l2detahypo_name):
                hypos.update(dict([ (l2detahypo_name, L2JetDEtaMjjAllTEType('L2JetDEtaMjjAllTE_%s' %l2detahypo_name, etaCutValue, 1000*float(mjjcut) ))]))
                                
            self.l2detahypo = getattr(hypos, l2detahypo_name)
            assert self.l2detahypo, 'impossible to deduce the L2 dEta hypothesis algorithm'

            #if not hasattr(fexes, l2detahypo_name):
            #    fexes.update(dict([ (l2detahypo_name,  L2MjjDEtaFexType('L2MjjDEtaFex_%s' %l2detahypo_name, etaCutValue, 1000*float(mjjcut)))]))
            #
            #self.l2detafex = getattr(fexes, l2detahypo_name)
            #assert self.l2detafex, 'impossible to deduce the L2 dEta Fex'
                

        if ef_match:
            mjjcut='0'
            etacut=''
            if has_mjj_ef :
                mjjcut = ef_match.group(2)
                etacut = ef_match.group(4)
            else :
                etacut = ef_match.group(2)

            efdeta_thresh = 'deta%sp%s' %(etacut[0], etacut[1])
            efmjj_thresh = 'Mjj%s' %(mjjcut)

            # Construct a string with the jet thresholds
            efstring = ''
            for index in range(len(self.ef_jet_mul)):
                if efstring:
                    efstring += '_'
                efstring += self.ef_jet_mul[index]
                efstring += 'j'
                efstring += self.ef_thresh[index]
            
            efdetahypo_name = 'ef_%s_%s_%s' %(efstring, efmjj_thresh, efdeta_thresh)

            EFMassDEtaHypoType = EFJetMassDEta2J25

            #One would need to update Trigger/TrigHypothesis/TrigJetHypo/python/TrigJetDEta(Hypo|Fex)Config.py
            #To add additional 2jXX types
            if(efstring == '2j0') :
                EFMassDEtaHypoType = EFJetMassDEta
            if(efstring == '2j7') :
                EFMassDEtaHypoType = EFJetMassDEta2J7
            elif(efstring == '2j10') :
                EFMassDEtaHypoType = EFJetMassDEta2J10                
            elif(efstring == '2j25') :
                EFMassDEtaHypoType = EFJetMassDEta2J25
            elif(efstring == '2j30') :
                EFMassDEtaHypoType = EFJetMassDEta2J30
            elif(efstring == '2j35') :
                EFMassDEtaHypoType = EFJetMassDEta2J35
            elif(efstring == '2j40') :
                EFMassDEtaHypoType = EFJetMassDEta2J40

            # Look for the desired hypo in the collection
            # Same thing, but for the Fex
            etaCutValue = float(etacut[1])/10 + float(etacut[0])
            if not hasattr(hypos, efdetahypo_name):
                hypos.update(dict([ (efdetahypo_name,  EFMassDEtaHypoType('EFJetMassDEta_%s' %(efdetahypo_name), 1000*float(mjjcut), etaCutValue ))]))

            self.efdetahypo = getattr(hypos, efdetahypo_name)
            assert self.efdetahypo, 'impossible to deduce the EF dEta hypothesis algorithm'


#------------------------------------------------------------------------------------------------------------------------------------------------

jet_chains_creator = JetChainFromName()

Jets = [
    
    jet_chains_creator.createJetTopoChain('j180_j145_a4tchad_m2000_dy34','L2_j165_c4cchad','L1_J75',758),

    #test chain
    jet_chains_creator.createJetChain('j30_a4tcem','L2_j25_c4ccem','L1_J10',7004),
    
    #another test chain
    #jet_chains_creator.createL2JetChain('L2_4j15_a4TTem', 'L1_4J15', 332),
    
    # LarNoiseBurst
    jet_chains_creator.createJetChain('j165_u0uchad_LArNoiseBurstT','L2_j165_c4cchad_LArNoiseBurstT','L1_J75',454),
    jet_chains_creator.createJetChain('j165_u0uchad_LArNoiseBurst','L2_j165_c4cchad_LArNoiseBurst','L1_J75',331),
    jet_chains_creator.createJetChain('j100_u0uchad_LArNoiseBurstT','L2_j100_c4cchad_LArNoiseBurstT','L1_J75',282),
    jet_chains_creator.createJetChain('j130_u0uchad_LArNoiseBurstT','L2_j130_c4cchad_LArNoiseBurstT','L1_J75',264),

    jet_chains_creator.createJetChain('j30_u0uchad_empty_LArNoiseBurst','L2_j30_c4cchad_empty_LArNoiseBurst','L1_J10_EMPTY',452),
    jet_chains_creator.createJetChain('j55_u0uchad_firstempty_LArNoiseBurst','L2_j55_c4cchad_firstempty_LArNoiseBurst','L1_J30_FIRSTEMPTY',325), 
    jet_chains_creator.createJetChain('j55_u0uchad_firstempty_LArNoiseBurstT','L2_j55_c4cchad_firstempty_LArNoiseBurstT','L1_J30_FIRSTEMPTY',453),

    jet_chains_creator.createJetChain('j55_u0uchad_empty_LArNoiseBurst','L2_j55_c4cchad_empty_LArNoiseBurst','L1_J30_EMPTY',749), 

    jet_chains_creator.createJetChain('j35_u0uchad_empty_LArNoiseBurst','L2_j35_c4cchad_empty_LArNoiseBurst','L1_J30_EMPTY',1),
    jet_chains_creator.createJetChain('j35_u0uchad_firstempty_LArNoiseBurst','L2_j35_c4cchad_firstempty_LArNoiseBurst','L1_J30_FIRSTEMPTY',2),
    jet_chains_creator.createJetChain('j80_u0uchad_LArNoiseBurstT','L2_j80_c4cchad_LArNoiseBurstT','L1_J75',4),

    ##HI chains
    jet_chains_creator.createNoAlgJetChain('L1J5_NoAlg', 'L1_J5',380),
    jet_chains_creator.createNoAlgJetChain('L1J5_L1TE90_NoAlg', 'L1_J5_TE90',137),
    jet_chains_creator.createNoAlgJetChain('2L1J5_L1TE90_NoAlg', 'L1_2J5_TE90',167),
    jet_chains_creator.createNoAlgJetChain('2L1J5_NoAlg', 'L1_2J5',388),
    jet_chains_creator.createNoAlgJetChain('L1J5_win6_NoAlg', 'L1_J5_win6',644),
    jet_chains_creator.createNoAlgJetChain('L1J5_win8_NoAlg', 'L1_J5_win8',645),
    jet_chains_creator.createNoAlgJetChain('L1TAU3_NoAlg', 'L1_TAU3',317),
    jet_chains_creator.createNoAlgJetChain('L1FJ1_empty_NoAlg', 'L1_FJ1_EMPTY',364),
    jet_chains_creator.createNoAlgJetChain('L1FJ5_empty_NoAlg', 'L1_FJ5_EMPTY',176),
    jet_chains_creator.createNoAlgJetChain('L1J75_NoAlg', 'L1_J75',365),
    jet_chains_creator.createNoAlgJetChain('L1J175_NoAlg', 'L1_J175',366),
    jet_chains_creator.createNoAlgJetChain('L1FJ10_empty_NoAlg', 'L1_FJ10_EMPTY',367),
    jet_chains_creator.createNoAlgJetChain('L1FJ0_empty_NoAlg', 'L1_FJ0_EMPTY',369),
    jet_chains_creator.createNoAlgJetChain('L1J10_NoAlg', 'L1_J10',368),
    jet_chains_creator.createNoAlgJetChain('L1J15_NoAlg', 'L1_J15',210),
    jet_chains_creator.createNoAlgJetChain('L1J20_NoAlg', 'L1_J20',38),
    jet_chains_creator.createNoAlgJetChain('L1J5_empty_NoAlg'     , 'L1_J5_EMPTY'     ,884),
    jet_chains_creator.createNoAlgJetChain('L1J5_firstempty_NoAlg', 'L1_J5_FIRSTEMPTY',883),
    
    jet_chains_creator.createJetChain('j10_u0uchad_LArNoiseBurst','L2_L1J5_NoAlg','L1_J5',325, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j10_u0uchad_empty_LArNoiseBurst','L2_L1J5_empty_NoAlg','L1_J5_EMPTY',453, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j10_u0uchad_firstempty_LArNoiseBurst','L2_L1J5_firstempty_NoAlg','L1_J5_FIRSTEMPTY',749, ef_streams='MinBias'),

    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',335),
    jet_chains_creator.createJetChain('j20_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',336),
    jet_chains_creator.createJetChain('j25_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',337),
    jet_chains_creator.createJetChain('j30_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',338),
    jet_chains_creator.createJetChain('j40_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',339),
    jet_chains_creator.createJetChain('j50_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',340),
    jet_chains_creator.createJetChain('j75_a2hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',341),
    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1TE10','L2_L1TE10_NoAlg','L1_TE10',342),
    jet_chains_creator.createJetChain('j20_a2hi_EFFS_L1TE10','L2_L1TE10_NoAlg','L1_TE10',343),
    jet_chains_creator.createJetChain('j25_a2hi_EFFS_L1TE10','L2_L1TE10_NoAlg','L1_TE10',344),
    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1TE20','L2_L1TE20_NoAlg','L1_TE20',311),
    jet_chains_creator.createJetChain('j20_a2hi_EFFS_L1TE20','L2_L1TE20_NoAlg','L1_TE20',346),
    jet_chains_creator.createJetChain('j25_a2hi_EFFS_L1TE20','L2_L1TE20_NoAlg','L1_TE20',347),
    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1J5','L2_L1J5_NoAlg','L1_J5',606),
    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',348),
    jet_chains_creator.createJetChain('j20_a2hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',349),
    jet_chains_creator.createJetChain('j25_a2hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',350),
    jet_chains_creator.createJetChain('j15_a2hi_EFFS_L1MBTS','L2_L1MBTS_2_NoAlg','L1_MBTS_2',351),
    jet_chains_creator.createJetChain('j20_a2hi_EFFS_L1MBTS','L2_L1MBTS_2_NoAlg','L1_MBTS_2',352),


    jet_chains_creator.createJetChain('j15_a4hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',354),
    jet_chains_creator.createJetChain('j20_a4hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',355),
    jet_chains_creator.createJetChain('j75_a4hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',356),
    jet_chains_creator.createJetChain('j100_a4hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',357),
    jet_chains_creator.createJetChain('j150_a4hi_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',358),
    
#    jet_chains_creator.createJetChain('j7_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',386), 
#    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',403),
#    jet_chains_creator.createJetChain('j15_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',406),
#    jet_chains_creator.createJetChain('j20_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',272),
#    jet_chains_creator.createJetChain('j30_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',291),
#    jet_chains_creator.createJetChain('j40_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',292),
#    jet_chains_creator.createJetChain('j50_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg','L1_MBTS_1_1',284),
# definitions moved to the bottom because of change in L2 seeding

    jet_chains_creator.createJetChain('j7_a4hi_EFFS_L1J5','L2_L1J5_NoAlg','L1_J5',321),
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1J5','L2_L1J5_NoAlg','L1_J5',320),
    jet_chains_creator.createJetChain('j7_a4hi_EFFS_L1EM3','L2_L1EM3_NoAlg','L1_EM3',322),
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1EM3','L2_L1EM3_NoAlg','L1_EM3',323),
    jet_chains_creator.createJetChain('j7_a4hi_EFFS_L1TAU3','L2_L1TAU3_NoAlg','L1_TAU3',326),
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1TAU3','L2_L1TAU3_NoAlg','L1_TAU3',327),

    jet_chains_creator.createJetChain('j7_a4hi_EFFS_L1J5_L1TE90','L2_L1J5_L1TE90_NoAlg','L1_J5_TE90',169),
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1J5_L1TE90','L2_L1J5_L1TE90_NoAlg','L1_J5_TE90',704),
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1TE90','L2_L1TE90_NoAlg','L1_TE90',1005),   
    jet_chains_creator.createJetChain('j10_a4hi_EFFS_L1TE100','L2_L1TE100_NoAlg','L1_TE100',1006),

    jet_chains_creator.createJetChain('j15_a4hi_EFFS_L1J5','L2_L1J5_NoAlg','L1_J5',605),
    jet_chains_creator.createJetChain('j15_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',604),
    jet_chains_creator.createJetChain('j20_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',156),
    jet_chains_creator.createJetChain('j25_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',179),
    jet_chains_creator.createJetChain('j30_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',177),
    jet_chains_creator.createJetChain('j35_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',178),
    jet_chains_creator.createJetChain('j40_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',158),
    jet_chains_creator.createJetChain('j45_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',186),
    jet_chains_creator.createJetChain('j50_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',188),

    #
    jet_chains_creator.createJetChain('j60_a4hi_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 22),
    jet_chains_creator.createJetChain('j75_a4hi_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 23),
    jet_chains_creator.createJetChain('j80_a4hi_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 26),
    jet_chains_creator.createJetChain('j90_a4hi_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 33),
    jet_chains_creator.createJetChain('j100_a4hi_EFFS_L1J20', 'L2_L1J20_NoAlg', 'L1_J20', 35),
    
    jet_chains_creator.createJetChain('j75_a4hi_EFFS_L1J10','L2_L1J10_NoAlg','L1_J10',359),
    jet_chains_creator.createJetChain('j15_a4hi_EFFS_L1TE10','L2_L1TE10_NoAlg','L1_TE10',362),
    jet_chains_creator.createJetChain('j20_a4hi_EFFS_L1TE10','L2_L1TE10_NoAlg','L1_TE10',363),

    jet_chains_creator.createJetChain('j15_a2hi_EFFS','L2_rd0_filled_NoAlg','L1_RD0_FILLED',633),


    # dEta chains - should now work
    jet_chains_creator.createJetdEtaMjjChain('2j0_a4tcem_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 611),
    jet_chains_creator.createJetdEtaMjjChain('2j7_a4tcem_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 612),
    jet_chains_creator.createJetdEtaMjjChain('2j10_a4tcem_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 613),
    jet_chains_creator.createJetdEtaMjjChain('2j15_a4tcem_deta40', 'L2_L1J15_NoAlg', 'L1_J15', 232),

    jet_chains_creator.createJetdEtaMjjChain('2j7_a4hi_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 58), 
    jet_chains_creator.createJetdEtaMjjChain('2j10_a4hi_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 59), 
    jet_chains_creator.createJetdEtaMjjChain('2j15_a4hi_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 100),

    jet_chains_creator.createJetdEtaMjjChain('2j7_a4tcem_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 55),
    jet_chains_creator.createJetdEtaMjjChain('2j10_a4tcem_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 56),
    jet_chains_creator.createJetdEtaMjjChain('2j15_a4tcem_eta50_deta40', 'L2_L1J10_NoAlg', 'L1_J10', 57),

    #tchad jets
#   jet_chains_creator.createJetdEtaMjjChain('2j7_a4tchad_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 538),
#   jet_chains_creator.createJetdEtaMjjChain('2j10_a4tchad_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 539),
#   jet_chains_creator.createJetdEtaMjjChain('2j15_a4tchad_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 542),

#   jet_chains_creator.createJetdEtaMjjChain('2j7_a4tchad_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 543),
#   jet_chains_creator.createJetdEtaMjjChain('2j10_a4tchad_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 545),
#   jet_chains_creator.createJetdEtaMjjChain('2j15_a4tchad_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 550),

    #a4hi jets
#   jet_chains_creator.createJetdEtaMjjChain('2j7_a4hi_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 559),
#   jet_chains_creator.createJetdEtaMjjChain('2j10_a4hi_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 591),
#   jet_chains_creator.createJetdEtaMjjChain('2j15_a4hi_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 665),
    
#   jet_chains_creator.createJetdEtaMjjChain('2j7_a4hi_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 670),
#   jet_chains_creator.createJetdEtaMjjChain('2j10_a4hi_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 671),
#   jet_chains_creator.createJetdEtaMjjChain('2j15_a4hi_eta50_deta40_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 672),

    ##HI chains for LAr calib
    jet_chains_creator.createJetChain('j30_a4tcem_L1J15', 'L2_j25_c4ccem_L1J15', 'L1_J15', 2220),    
    jet_chains_creator.createJetChain('j50_a4tcem_L1J30', 'L2_j45_c4ccem', 'L1_J30', 2221),
    jet_chains_creator.createJetChain('j95_a4tcem_L1J75', 'L2_j90_c4ccem', 'L1_J75', 2225),
    jet_chains_creator.createJetChain('j200_a4tcem_L1J175', 'L2_j200_c4ccem', 'L1_J175', 2226),

    jet_chains_creator.createJetChain('fj35_a4tcem', 'L2_fj30_c4ccem', 'L1_FJ10', 2227),    
    #jet_chains_creator.createJetChain('fj35_a4tchad', 'L2_fj30_c4cchad', 'L1_FJ10', 2227),    

    
    ###HI MC chains
    jet_chains_creator.createJetChain('j15_a4tj_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',3335),
    jet_chains_creator.createJetChain('j20_a4tj_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',3336),
    jet_chains_creator.createJetChain('j40_a4tj_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',3337),
    jet_chains_creator.createJetChain('j75_a4tj_EFFS_L1ZDC','L2_mbZdc_a_c_NoAlg','L1_ZDC_A_C',3338),   

    # HT chain
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht500_L2FS','L2_j75_a4TTem_ht100_j140_c4cchad','L1_J75',7580),
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht600_L2FS','L2_j75_a4TTem_ht300_j140_c4cchad','L1_J75',589),
    #jet_chains_creator.createJetTopoChain('j145_a4tchad_ht700_L2FS','L2_j75_a4TTem_ht200_j140_c4cchad','L1_J75',590),
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht700_L2FS','L2_j75_a4TTem_ht300_j140_c4cchad','L1_J75',590),
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht800_L2FS','L2_j75_a4TTem_ht300_j140_c4cchad','L1_J75',927),
    
    jet_chains_creator.createJetTopoChain('j170_a4tchad_ht500','L2_j165_c4cchad','L1_J75',7583),
    jet_chains_creator.createJetTopoChain('j170_a4tchad_ht600','L2_j165_c4cchad','L1_J75',584),
    jet_chains_creator.createJetTopoChain('j170_a4tchad_ht700','L2_j165_c4cchad','L1_J75',585),
    jet_chains_creator.createJetTopoChain('j170_a4tchad_ht800','L2_j165_c4cchad','L1_J75',925),

    # Not used in the menu, but needed for other items
    jet_chains_creator.createJetTopoChain('j165_a4tchad_ht500','L2_j160_c4cchad','L1_J75',9583),
    jet_chains_creator.createJetTopoChain('j165_a4tchad_ht600','L2_j160_c4cchad','L1_J75',9584),
    jet_chains_creator.createJetTopoChain('j165_a4tchad_ht700','L2_j160_c4cchad','L1_J75',9585),

    #3jet+HT triggers for the delayed stream
    jet_chains_creator.createJetTopoChain('j145_2j45_a4tchad_ht450_L2FS','L2_j75_a4TTem_ht200_j140_c4cchad', 'L1_J75', 937),
    jet_chains_creator.createJetTopoChain('j145_2j45_a4tchad_ht500_L2FS','L2_j75_a4TTem_ht200_j140_c4cchad', 'L1_J75', 6533),
    jet_chains_creator.createJetTopoChain('j145_2j45_a4tchad_ht550_L2FS','L2_j75_a4TTem_ht200_j140_c4cchad', 'L1_J75', 6531),

    # HV chain
    jet_chains_creator.createJetHVChain('j35_a4tcem_L1TAU_LOF_HV', 'L2_j30_c4ccem_L1TAU_LOF_HV', 'L1_TAU40', 766),
    jet_chains_creator.createJetHVChain('l2j25_a4tcem_Trackless_HV', 'L2_j25_c4ccem_Trackless_HV', 'L1_MU10_J20', 767),
    jet_chains_creator.createJetHVChain('l2j30_a4tcem_2L1MU6_HV', 'L2_j30_c4ccem_2L1MU6_HV', 'L1_2MU6', 768),
    jet_chains_creator.createJetHVChain('l2j30_a4tcem_2L1MU10_HV', 'L2_j30_c4ccem_2L1MU10_HV', 'L1_2MU10', 776),
    jet_chains_creator.createJetHVChain('l2j30_a4tcem_2L1MU10_ExtendedEta_HV', 'L2_j30_c4ccem_2L1MU10_ExtendedEta_HV', 'L1_2MU10', 217),

    jet_chains_creator.createJetHVChain('j35_a4tcem_L1TAU_LOF_HV_EMPTY', 'L2_j30_c4ccem_L1TAU_LOF_HV_EMPTY', 'L1_TAU8_EMPTY', 769),
    jet_chains_creator.createJetHVChain('l2j25_a4tcem_Trackless_HV_EMPTY', 'L2_j25_c4ccem_Trackless_HV_EMPTY', 'L1_MU4_J15_EMPTY', 770),
    jet_chains_creator.createJetHVChain('l2j30_a4tcem_2L1MU4_HV_EMPTY', 'L2_j30_c4ccem_2L1MU4_HV_EMPTY', 'L1_2MU4_EMPTY', 771),

    jet_chains_creator.createJetHVChain('j35_a4tcem_L1TAU_LOF_HV_UNPAIRED_ISO', 'L2_j30_c4ccem_L1TAU_LOF_HV_UNPAIRED_ISO', 'L1_TAU8_UNPAIRED_ISO', 772),
    jet_chains_creator.createJetHVChain('l2j25_a4tcem_Trackless_HV_UNPAIRED_ISO', 'L2_j25_c4ccem_Trackless_HV_UNPAIRED_ISO', 'L1_MU4_J15_UNPAIRED_ISO', 773),
    jet_chains_creator.createJetHVChain('l2j30_a4tcem_2L1MU6_HV_UNPAIRED_ISO', 'L2_j30_c4ccem_2L1MU6_HV_UNPAIRED_ISO', 'L1_2MU6_UNPAIRED_ISO', 774),


    # No Alg chains [create also EF chain for this case]
    jet_chains_creator.createNoAlgJetChain('L1J350_NoAlg', 'L1_J350',399),
    jet_chains_creator.createNoAlgJetChain('L1FJ100_NoAlg','L1_FJ100',398),
    jet_chains_creator.createNoAlgJetChain('L1JE140_NoAlg','L1_JE140',6997),
    jet_chains_creator.createNoAlgJetChain('L1JE200_NoAlg','L1_JE200',5396),
    jet_chains_creator.createNoAlgJetChain('L1JE350_NoAlg','L1_JE350',5395),
    jet_chains_creator.createNoAlgJetChain('L1JE500_NoAlg','L1_JE500',394),
    jet_chains_creator.createNoAlgJetChain('L1J10_empty_NoAlg'     , 'L1_J10_EMPTY'     ,885),
    jet_chains_creator.createNoAlgJetChain('L1J10_firstempty_NoAlg', 'L1_J10_FIRSTEMPTY',886),
    jet_chains_creator.createNoAlgJetChain('L1J30_empty_NoAlg'     , 'L1_J30_EMPTY'     ,887),
    jet_chains_creator.createNoAlgJetChain('L1J30_firstempty_NoAlg', 'L1_J30_FIRSTEMPTY',888),
    jet_chains_creator.createNoAlgJetChain('L1FJ30_empty_NoAlg'    , 'L1_FJ30_EMPTY'    ,889),

    # Inclusive chains
    jet_chains_creator.createJetChain('j45_a4tchad','L2_j40_c4cchad', 'L1_J10', 340),
    jet_chains_creator.createJetChain('j55_a4tchad','L2_j50_c4cchad', 'L1_J15', 341),
    jet_chains_creator.createJetChain('j80_a4tchad','L2_j75_c4cchad', 'L1_J30', 342),
    jet_chains_creator.createJetChain('j110_a4tchad','L2_j105_c4cchad', 'L1_J50', 348),
    jet_chains_creator.createJetChain('j145_a4tchad','L2_j140_c4cchad', 'L1_J75', 344),
    jet_chains_creator.createJetChain('j180_a4tchad','L2_j165_c4cchad','L1_J75',350),
    jet_chains_creator.createJetChain('j220_a4tchad','L2_j165_c4cchad','L1_J75',351),
    jet_chains_creator.createJetChain('j280_a4tchad','L2_j165_c4cchad','L1_J75',352),
    jet_chains_creator.createJetChain('j360_a4tchad','L2_j165_c4cchad','L1_J75',353),
    jet_chains_creator.createJetChain('j460_a4tchad','L2_j165_c4cchad','L1_J75',354),

    # topotowers
    jet_chains_creator.createJetChain('j35_a4tthad','L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 491),
    jet_chains_creator.createJetChain('j180_a4tthad','L2_j165_c4cchad', 'L1_J75', 4750),
    jet_chains_creator.createJetChain('j220_a4tthad','L2_j165_c4cchad', 'L1_J75', 6643),
    jet_chains_creator.createJetChain('j260_a4tthad','L2_j165_c4cchad', 'L1_J75', 6944),
    jet_chains_creator.createJetChain('j360_a4tthad','L2_j165_c4cchad', 'L1_J75', 492),
    jet_chains_creator.createJetChain('j380_a4tthad','L2_j165_c4cchad', 'L1_J75', 4435),


    # lc calibration
    jet_chains_creator.createJetChain('j35_a4tclcw','L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 438),
    jet_chains_creator.createJetChain('j55_a4tclcw','L2_j50_c4cchad', 'L1_J15', 436),
    jet_chains_creator.createJetChain('j110_a4tclcw','L2_j105_c4cchad', 'L1_J50', 493),
    jet_chains_creator.createJetChain('j145_a4tclcw','L2_j140_c4cchad', 'L1_J75', 494),
    jet_chains_creator.createJetChain('j180_a4tclcw','L2_j165_c4cchad', 'L1_J75', 3440),
    jet_chains_creator.createJetChain('j360_a4tclcw','L2_j165_c4cchad', 'L1_J75', 495),

    jet_chains_creator.createJetChain('j280_a10tclcw_L2FS','L2_j220_a10TTem', 'L1_J75', 6941),
    jet_chains_creator.createJetChain('j180_a10tclcw','L2_j165_c4cchad', 'L1_J75', 455),
    jet_chains_creator.createJetChain('j240_a10tclcw','L2_j165_c4cchad', 'L1_J75', 460),
    jet_chains_creator.createJetChain('j360_a10tclcw','L2_j165_c4cchad', 'L1_J75', 461),
    jet_chains_creator.createJetChain('j460_a10tclcw','L2_j165_c4cchad', 'L1_J75', 462),
    
    # L1.5 Chains
    jet_chains_creator.createJetChain('j45_a4tchad_L2FS','L2_j10_a4TTem', 'L1_J10', 5320),
    jet_chains_creator.createJetChain('j45_a4tchad_L2FS_L1RD0','L2_j10_a4TTem_L1RD0', 'L1_RD0_FILLED', 3233),
    jet_chains_creator.createJetChain('j45_a4tchad_L2FS_L1J15','L2_j15_a4TTem_j40_c4cchad', 'L1_J15', 333),
    jet_chains_creator.createJetChain('j55_a4tchad_L2FS','L2_j15_a4TTem', 'L1_J15', 5321),
    jet_chains_creator.createJetChain('j65_a4tchad_L2FS','L2_j20_a4TTem', 'L1_J20', 5322),

    # Fat jets
    jet_chains_creator.createJetChain('j180_a10tcem','L2_j165_c4cchad','L1_J75',5355),
    jet_chains_creator.createJetChain('j240_a10tcem','L2_j165_c4cchad','L1_J75',357),
    jet_chains_creator.createJetChain('j360_a10tcem','L2_j165_c4cchad','L1_J75',358),
    jet_chains_creator.createJetChain('j460_a10tcem','L2_j165_c4cchad','L1_J75',359),

    # Needed for a MET chain
    jet_chains_creator.createJetChain('j145_a10tcem','L2_j140_c4cchad', 'L1_J75', 9494),

    # L1.5 Fat jets
    jet_chains_creator.createJetChain('j80_a10tcem_L2FS', 'L2_j30_a10TTem', 'L1_RD0_FILLED', 4420),
    jet_chains_creator.createJetChain('j110_a10tcem_L2FS', 'L2_j75_a10TTem', 'L1_J50', 421),
    jet_chains_creator.createJetChain('j145_a10tcem_L2FS', 'L2_j100_a10TTem', 'L1_J75', 4422),

    #pp at 2.76TeV
#    jet_chains_creator.createJetChain('j7_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 487),
#    jet_chains_creator.createJetChain('j10_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 410),
#    jet_chains_creator.createJetChain('j15_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 496),
#    jet_chains_creator.createJetChain('j20_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 266),
#    jet_chains_creator.createJetChain('j30_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 267),
#    jet_chains_creator.createJetChain('j40_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 268),
#    jet_chains_creator.createJetChain('j50_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 270),
    
    #forward jets at 2.76 TeV
#    jet_chains_creator.createJetChain('fj7_a4tchad_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 820),
    jet_chains_creator.createJetChain('fj7_a4tchad_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 823),
    jet_chains_creator.createJetChain('fj10_a4tchad_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 824),
    jet_chains_creator.createJetChain('fj15_a4tchad_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 892),
#    jet_chains_creator.createJetChain('fj7_a4hi_EFFS_L1MBTS', 'L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 871),
    jet_chains_creator.createJetChain('fj7_a4hi_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 827),
    jet_chains_creator.createJetChain('fj10_a4hi_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 829),
    jet_chains_creator.createJetChain('fj15_a4hi_EFFS_L1FJ0', 'L2_L1FJ0_NoAlg', 'L1_FJ0', 842),

    #pPb jets
    jet_chains_creator.createJetChain('j7_a4tcem_EFFS_L1J5_win6', 'L2_L1J5_win6_NoAlg', 'L1_J5_win6', 640),
    jet_chains_creator.createJetChain('j10_a4tcem_EFFS_L1J5_win6', 'L2_L1J5_win6_NoAlg', 'L1_J5_win6', 641),
    jet_chains_creator.createJetChain('j7_a4tcem_EFFS_L1J5_win8', 'L2_L1J5_win8_NoAlg', 'L1_J5_win8', 642),
    jet_chains_creator.createJetChain('j10_a4tcem_EFFS_L1J5_win8', 'L2_L1J5_win8_NoAlg', 'L1_J5_win8', 643),
    jet_chains_creator.createJetChain('j7_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 304),
    jet_chains_creator.createJetChain('j7_a4tchad_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 228),
    jet_chains_creator.createJetChain('j7_a4tcem_EFFS_L1EM3', 'L2_L1EM3_NoAlg', 'L1_EM3', 305),
    jet_chains_creator.createJetChain('j7_a4tcem_EFFS_L1TAU3', 'L2_L1TAU3_NoAlg', 'L1_TAU3', 310),
    jet_chains_creator.createJetChain('j10_a4tcem_EFFS_L1TAU3', 'L2_L1TAU3_NoAlg', 'L1_TAU3', 306),
    jet_chains_creator.createJetChain('j10_a4tcem_EFFS_L1EM3', 'L2_L1EM3_NoAlg', 'L1_EM3', 308),
    jet_chains_creator.createJetChain('j10_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 316),
    jet_chains_creator.createJetChain('j10_a4tchad_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 229),
    jet_chains_creator.createJetChain('j20_a4tchad_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 660),
    jet_chains_creator.createJetChain('j30_a4tchad_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 661),
    jet_chains_creator.createJetChain('j40_a4tchad_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 662),

    jet_chains_creator.createJetChain('j20_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 418),
    jet_chains_creator.createJetChain('j30_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 419),
    jet_chains_creator.createJetChain('j40_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 435),
    jet_chains_creator.createJetChain('j15_a2tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 607),
    jet_chains_creator.createJetChain('j15_a4tcem_EFFS_L1J5', 'L2_L1J5_NoAlg', 'L1_J5', 608),
    jet_chains_creator.createJetChain('j15_a2tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 609),
    jet_chains_creator.createJetChain('j15_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 610),
    jet_chains_creator.createJetChain('j20_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 150),
    jet_chains_creator.createJetChain('j25_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 180),
    jet_chains_creator.createJetChain('j30_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 181),
    jet_chains_creator.createJetChain('j35_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 182),
    jet_chains_creator.createJetChain('j40_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 151),
    jet_chains_creator.createJetChain('j45_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 153),
    jet_chains_creator.createJetChain('j50_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 154),
    jet_chains_creator.createJetChain('j60_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 193),
    jet_chains_creator.createJetChain('j75_a4tcem_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 200),
    jet_chains_creator.createJetChain('j50_a4tcem_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 202),
    jet_chains_creator.createJetChain('j60_a4tcem_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 203),
    jet_chains_creator.createJetChain('j75_a4tcem_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 207),

    jet_chains_creator.createJetChain('j50_a4tchad_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 564),
    jet_chains_creator.createJetChain('j60_a4tchad_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 566),
    jet_chains_creator.createJetChain('j75_a4tchad_EFFS_L1J10', 'L2_L1J10_NoAlg', 'L1_J10', 568),
    jet_chains_creator.createJetChain('j50_a4tchad_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 570),
    jet_chains_creator.createJetChain('j60_a4tchad_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 592),
    jet_chains_creator.createJetChain('j75_a4tchad_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 593),
    jet_chains_creator.createJetChain('j80_a4tchad_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 598),
    jet_chains_creator.createJetChain('j90_a4tchad_EFFS_L1J15', 'L2_L1J15_NoAlg', 'L1_J15', 614),
    jet_chains_creator.createJetChain('j100_a4tchad_EFFS_L1J20', 'L2_L1J20_NoAlg', 'L1_J20', 17),

    # Narrow jets
    jet_chains_creator.createJetChain('2j100_a2tcem', 'L2_j165_c4cchad', 'L1_J75', 6426),
    jet_chains_creator.createJetChain('2j150_a2tcem', 'L2_j165_c4cchad', 'L1_J75', 6427),
    jet_chains_creator.createJetChain('2j200_a2tcem', 'L2_j165_c4cchad', 'L1_J75', 428),
    jet_chains_creator.createJetChain('2j250_a2tcem', 'L2_j165_c4cchad', 'L1_J75', 429),
    # Boosted W trigger
    jet_chains_creator.createJetTopoChain('2j200_a2tcem_m0_dy04', 'L2_j165_c4cchad', 'L1_J75', 213),

    # Forward jets
    jet_chains_creator.createJetChain('fj80_a4tchad','L2_fj75_c4cchad', 'L1_FJ30', 345),
    jet_chains_creator.createJetChain('fj110_a4tchad','L2_fj105_c4cchad', 'L1_FJ50', 346),
    jet_chains_creator.createJetChain('fj145_a4tchad','L2_fj140_c4cchad', 'L1_FJ75', 347),
    jet_chains_creator.createJetChain('fj180_a4tchad','L2_fj140_c4cchad','L1_FJ75',365),
    jet_chains_creator.createJetChain('fj220_a4tchad','L2_fj140_c4cchad','L1_FJ75',361),

    jet_chains_creator.createJetChain('fj35_a4tclcw','L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 4439),
    jet_chains_creator.createJetChain('fj145_a4tclcw','L2_fj140_c4cchad', 'L1_FJ75', 4479),


    # L2 FS jets
    jet_chains_creator.createJetChain('fj45_a4tchad_L2FS','L2_fj10_a4TTem_fj40_c4cchad','L1_RD0_FILLED',513),
    jet_chains_creator.createJetChain('fj55_a4tchad_L2FS','L2_fj15_a4TTem_fj50_c4cchad','L1_RD0_FILLED',514),

    
    ]

## Jet chains seeded from randoms
Jets += [

    jet_chains_creator.createJetChain('j10_a4tchadloose', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 506),
    jet_chains_creator.createJetChain('fj10_a4tchadloose', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 507),

    jet_chains_creator.createJetChain('j15_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',299),
    jet_chains_creator.createJetChain('j25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',301), 
    jet_chains_creator.createJetChain('j35_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',302), 
    jet_chains_creator.createJetChain('j35_a10tcem', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',303), 

    # Note naming convention since these chains are normally L1Calo based
    jet_chains_creator.createJetChain('j45_a4tchad_L1RD0', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',5304), 
    jet_chains_creator.createJetChain('j45_a10tcem_L1RD0', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',5305), 

    jet_chains_creator.createJetChain('fj15_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',306), 
    jet_chains_creator.createJetChain('fj25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',307), 
    jet_chains_creator.createJetChain('fj35_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',308), 
    jet_chains_creator.createJetChain('fj45_a4tchad_L1RD0', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',3090),

    jet_chains_creator.createJetChain('2j25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',899), 
    jet_chains_creator.createJetChain('3j25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',310), 
    jet_chains_creator.createJetChain('4j25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',311), 
    jet_chains_creator.createJetChain('5j25_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',312), 

    jet_chains_creator.createJetChain('2j35_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 315),
    jet_chains_creator.createJetChain('3j35_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 316),
    jet_chains_creator.createJetChain('3j45_a4tchad', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 3167),
    jet_chains_creator.createJetChain('4j35_a4tchad_L1RD0', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED', 317),

    # the L1 input to this item is hard coded in chainTemplate to be J10
    jet_chains_creator.createL2JetChain('L2_j10000_c4cchad_L1RD0','L1_RD0_FILLED',313),
    jet_chains_creator.createL2JetChain('L2_j10000_a4cchad_L1RD0','L1_RD0_FILLED',223),
    jet_chains_creator.createL2JetChain('L2_fj10_a4TTem_fj10000_c4cchad_L1RD0', 'L1_RD0_FILLED', 213),
    jet_chains_creator.createL2JetChain('L2_j10_a4TThad_j100000_c4cchad_L1RD0', 'L1_RD0_FILLED', 156),

    ]

## Multijets
Jets += [

    # L1.5 jets
    jet_chains_creator.createJetChain('j110_2j55_a4tchad_L2FS','L2_j50_2j15_a4TTem', 'L1_3J15_J50', 4415),

    jet_chains_creator.createJetChain('3j35_a4tchad_L2FS_4L1J15','L2_3j15_a4TTem_4L1J15', 'L1_4J15', 4410),

    jet_chains_creator.createJetChain('3j45_a4tchad_L2FS_L1RD0','L2_3j10_a4TTem_L1RD0', 'L1_RD0_FILLED', 3245),
    jet_chains_creator.createJetChain('3j45_a4tchad_L2FS','L2_3j10_a4TTem', 'L1_3J10', 327),
    jet_chains_creator.createJetChain('3j45_a4tchad_L2FS_4L1J15','L2_3j15_a4TTem_4L1J15', 'L1_4J15', 4411),
    jet_chains_creator.createJetChain('4j45_a4tchad_L2FS_L1RD0','L2_4j10_a4TTem_L1RD0', 'L1_RD0_FILLED', 2325),
    jet_chains_creator.createJetChain('4j45_a4tchad_L2FS_4L1J10','L2_4j10_a4TTem_4L1J10', 'L1_4J10', 5328),
    jet_chains_creator.createJetChain('5j45_a4tchad_L2FS_L1RD0','L2_5j10_a4TTem_L1RD0', 'L1_RD0_FILLED', 5326),
    jet_chains_creator.createJetChain('5j45_a4tchad_L2FS_5L1J10','L2_5j10_a4TTem_5L1J10', 'L1_5J10', 329),
    jet_chains_creator.createJetChain('6j45_a4tchad_L2FS','L2_6j10_a4TTem', 'L1_5J10', 330),
    jet_chains_creator.createJetChain('7j45_a4tchad_L2FS','L2_7j10_a4TTem', 'L1_5J10', 2331),

    jet_chains_creator.createJetChain('3j15_a4tchad_L2FS',      'L2_3j5_a4TTem', 'L1_2J5', 332),
    jet_chains_creator.createJetChain('3j15_a4tchad_L2FS_2L1J5','L2_2j5_a4TTem', 'L1_2J5', 329),
    jet_chains_creator.createJetChain('2j15_a4tchad_L2FS',      'L2_2j5_a4TTem', 'L1_2J5', 330),


    jet_chains_creator.createJetChain('3j55_a4tchad_L2FS','L2_3j15_a4TTem', 'L1_3J15', 412),
    jet_chains_creator.createJetChain('4j55_a4tchad_L2FS','L2_4j15_a4TTem', 'L1_4J15', 332),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FS','L2_5j15_a4TTem', 'L1_4J15', 711),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FS_5L1J15','L2_5j15_a4TTem_5L1J15', 'L1_5J15', 3336),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FS','L2_6j15_a4TTem', 'L1_4J15', 5334),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FS_5L1J15','L2_6j15_a4TTem_5L1J15', 'L1_5J15', 3337),
    jet_chains_creator.createJetChain('7j55_a4tchad_L2FS','L2_7j15_a4TTem', 'L1_4J15', 5335),
    jet_chains_creator.createJetChain('7j55_a4tchad_L2FS_5L1J15','L2_7j15_a4TTem_5L1J15', 'L1_5J15', 3338),

    # Extra multijet L1.5 chains
    jet_chains_creator.createJetChain('5j60_a4tchad_L2FS','L2_5j15_a4TTem', 'L1_4J15', 849),
    jet_chains_creator.createJetChain('5j60_a4tclcw_L2FS','L2_5j15_a4TTem', 'L1_4J15', 853),
    jet_chains_creator.createJetChain('6j50_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 854),
    jet_chains_creator.createJetChain('6j50_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 855),
    jet_chains_creator.createJetChain('7j40_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 857),
    jet_chains_creator.createJetChain('7j40_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 858),
    jet_chains_creator.createJetChain('8j40_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 859),
    jet_chains_creator.createJetChain('8j40_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 862),
    jet_chains_creator.createJetChain('8j35_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 863),
    jet_chains_creator.createJetChain('8j35_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 864),

    jet_chains_creator.createJetChain('4j65_a4tchad_L2FS','L2_4j20_a4TTem', 'L1_4J15', 339),
    jet_chains_creator.createJetChain('5j65_a4tchad_L2FS','L2_5j15_a4TTem', 'L1_4J15', 402),
    jet_chains_creator.createJetChain('6j65_a4tchad_L2FS','L2_6j20_a4TTem', 'L1_4J15', 5349),
    jet_chains_creator.createJetChain('7j65_a4tchad_L2FS','L2_7j20_a4TTem', 'L1_4J15', 5360),
    jet_chains_creator.createJetChain('4j65_a4tchad_L2FS_5L1J15','L2_4j20_a4TTem_5L1J15', 'L1_5J15', 5370),
    jet_chains_creator.createJetChain('5j65_a4tchad_L2FS_5L1J15','L2_5j20_a4TTem_5L1J15', 'L1_5J15', 5371),
    jet_chains_creator.createJetChain('6j65_a4tchad_L2FS_5L1J15','L2_6j20_a4TTem_5L1J15', 'L1_5J15', 5372),
    jet_chains_creator.createJetChain('7j65_a4tchad_L2FS_5L1J15','L2_7j20_a4TTem_5L1J15', 'L1_5J15', 4375),
    jet_chains_creator.createJetChain('4j65_a4tchad_L2FS_4L1J20','L2_4j20_a4TTem_4L1J20', 'L1_4J20', 5376),
    jet_chains_creator.createJetChain('5j65_a4tchad_L2FS_4L1J20','L2_5j20_a4TTem_4L1J20', 'L1_4J20', 4377),

    jet_chains_creator.createJetChain('4j80_a4tchad_L2FS','L2_4j20_a4TTem', 'L1_4J15', 378),
    jet_chains_creator.createJetChain('5j80_a4tchad_L2FS','L2_5j30_a4TTem', 'L1_4J15', 5379),
    jet_chains_creator.createJetChain('4j80_a4tchad_L2FS_5L1J15','L2_4j30_a4TTem_5L1J15', 'L1_5J15', 4383),
    jet_chains_creator.createJetChain('5j80_a4tchad_L2FS_5L1J15','L2_5j30_a4TTem_5L1J15', 'L1_5J15', 4384),
    jet_chains_creator.createJetChain('4j80_a4tchad_L2FS_4L1J20','L2_4j30_a4TTem_4L1J20', 'L1_4J20', 5385),
    jet_chains_creator.createJetChain('5j80_a4tchad_L2FS_4L1J20','L2_5j30_a4TTem_4L1J20', 'L1_4J20', 4386),

    jet_chains_creator.createJetChain('4j110_a4tchad_L2FS','L2_4j20_a4TTem', 'L1_4J15', 387),
    jet_chains_creator.createJetChain('5j110_a4tchad_L2FS','L2_5j50_a4TTem', 'L1_4J15', 6990),
    jet_chains_creator.createJetChain('4j110_a4tchad_L2FS_5L1J15','L2_4j50_a4TTem_5L1J15', 'L1_5J15', 4391),
    jet_chains_creator.createJetChain('5j110_a4tchad_L2FS_5L1J15','L2_5j50_a4TTem_5L1J15', 'L1_5J15', 5393),
    jet_chains_creator.createJetChain('4j110_a4tchad_L2FS_4L1J20','L2_4j50_a4TTem_4L1J20', 'L1_4J20', 4400),
    jet_chains_creator.createJetChain('5j110_a4tchad_L2FS_4L1J20','L2_5j50_a4TTem_4L1J20', 'L1_4J20', 4403),

    # Multi-jet, Level 2 Triggers
    jet_chains_creator.createJetChain('6j35_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 211),
    jet_chains_creator.createJetChain('7j35_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 212),

    jet_chains_creator.createJetChain('4j45_a4tchad_L2FS','L2_4j15_a4TTem', 'L1_4J15', 219),
    jet_chains_creator.createJetChain('5j45_a4tchad_L2FS','L2_5j15_a4TTem', 'L1_4J15', 220),
    jet_chains_creator.createJetChain('6j45_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 221),
    jet_chains_creator.createJetChain('7j45_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 222),
    
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 227),

    #for 1e34
    jet_chains_creator.createJetChain('5j50_a4tchad_L2FS'           ,'L2_5j15_a4TTem' , 'L1_4J15', 320),
    jet_chains_creator.createJetChain('5j50_j45_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem' , 'L1_4J15', 321),
    jet_chains_creator.createJetChain('5j50_2j45_a4tchad_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 322),

    jet_chains_creator.createJetChain('5j50_a4tclcw_L2FS'           ,'L2_5j15_a4TTem' , 'L1_4J15', 304),
    jet_chains_creator.createJetChain('5j50_j45_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem' , 'L1_4J15', 305),
    jet_chains_creator.createJetChain('5j50_2j45_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 309),


    # Level 2 jets
    jet_chains_creator.createJetChain('j110_2j55_a4tchad','L2_j105_2j50_c4cchad', 'L1_3J15_J50', 6916),

    jet_chains_creator.createJetChain('3j170_a4tchad_L1J75','L2_j165_c4cchad', 'L1_J75', 404),
    jet_chains_creator.createJetChain('3j180_a4tchad_L1J75','L2_j165_c4cchad', 'L1_J75', 405),

    #pPb jets
#    jet_chains_creator.createJetChain('2j7_a4tchad_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 556), 
#    jet_chains_creator.createJetChain('2j10_a4tchad_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 557),
#    jet_chains_creator.createJetChain('2j15_a4tchad_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 558),
#    jet_chains_creator.createJetChain('2j20_a4tchad_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 637),

#    jet_chains_creator.createJetChain('2j7_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 638),
#    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 639),
#    jet_chains_creator.createJetChain('2j15_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 653),
#    jet_chains_creator.createJetChain('2j20_a4hi_EFFS_L1MBTS','L2_L1MBTS_1_1_NoAlg', 'L1_MBTS_1_1', 361),

    jet_chains_creator.createJetChain('2j7_a4tcem_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 223),
    jet_chains_creator.createJetChain('2j10_a4tcem_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 183),
    jet_chains_creator.createJetChain('2j15_a4tcem_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 225),
    jet_chains_creator.createJetChain('2j5_a4tcem_EFFS_L1EM3','L2_L1EM3_NoAlg', 'L1_EM3', 372),
    jet_chains_creator.createJetChain('2j7_a4tcem_EFFS_L1EM3','L2_L1EM3_NoAlg', 'L1_EM3', 110),
    jet_chains_creator.createJetChain('2j10_a4tchad_EFFS_L1EM3','L2_L1EM3_NoAlg', 'L1_EM3', 111),
    jet_chains_creator.createJetChain('2j10_a4tchad_EFFS_L1EM5','L2_L1EM5_NoAlg', 'L1_EM5', 112),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_L1EM3','L2_L1EM3_NoAlg', 'L1_EM3', 970),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_L1EM5','L2_L1EM5_NoAlg', 'L1_EM5', 971),

    jet_chains_creator.createJetChain('2j7_a4hi_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 581),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 582),
    jet_chains_creator.createJetChain('2j15_a4hi_EFFS_2L1J5','L2_2L1J5_NoAlg', 'L1_2J5', 583),

    jet_chains_creator.createJetChain('2j7_a4hi_EFFS_2L1J5_L1TE90','L2_2L1J5_L1TE90_NoAlg', 'L1_2J5_TE90', 135),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_2L1J5_L1TE90','L2_2L1J5_L1TE90_NoAlg', 'L1_2J5_TE90', 136),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_L1TE90','L2_L1TE90_NoAlg', 'L1_TE90', 1007),
    jet_chains_creator.createJetChain('2j10_a4hi_EFFS_L1TE100','L2_L1TE100_NoAlg', 'L1_TE100', 1014),

    # L1.5 fat jets
    jet_chains_creator.createJetChain('2j240_a10tcem_L2FS', 'L2_2j100_a10TTem', 'L1_J75', 423),
    jet_chains_creator.createJetChain('2j240_a10tclcw_L2FS', 'L2_2j100_a10TTem', 'L1_J75', 466),

   #E.P.
    jet_chains_creator.createJetChain('2j360_a10tclcw_L2FS', 'L2_2j100_a10TTem', 'L1_J75', 745),
    jet_chains_creator.createJetChain('2j360_a10tcem_L2FS', 'L2_2j100_a10TTem', 'L1_J75', 746),

    # fat jets
    jet_chains_creator.createJetChain('2j240_a10tcem', 'L2_j165_c4cchad', 'L1_J75', 467),
    jet_chains_creator.createJetChain('2j240_a10tclcw', 'L2_j165_c4cchad', 'L1_J75', 468),

    #E.P.
    jet_chains_creator.createJetChain('2j360_a10tcem', 'L2_j165_c4cchad', 'L1_J75', 747),
    jet_chains_creator.createJetChain('2j360_a10tclcw', 'L2_j165_c4cchad', 'L1_J75', 748),

    # topotowers
    jet_chains_creator.createJetChain('3j180_a4tthad','L2_j165_c4cchad', 'L1_J75', 6930),
    jet_chains_creator.createJetChain('4j55_a4tthad_L2FS','L2_4j15_a4TTem', 'L1_4J15', 4431),

    # lc calibration
    jet_chains_creator.createJetChain('3j170_a4tclcw_L1J75','L2_j165_c4cchad', 'L1_J75', 497),
    jet_chains_creator.createJetChain('4j80_a4tclcw_L2FS','L2_4j20_a4TTem', 'L1_4J15', 498),
    jet_chains_creator.createJetChain('4j110_a4tclcw_L2FS','L2_4j20_a4TTem', 'L1_4J15', 499),
    jet_chains_creator.createJetChain('5j45_a4tclcw_L2FS','L2_5j15_a4TTem', 'L1_4J15', 516),
    jet_chains_creator.createJetChain('5j55_a4tclcw_L2FS','L2_5j15_a4TTem', 'L1_4J15', 709),
    jet_chains_creator.createJetChain('6j45_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 572),
    jet_chains_creator.createJetChain('7j35_a4tclcw_L2FS_5L2j15','L2_5j15_a4TTem', 'L1_4J15', 578),


    # L1.5 hadronic scale
    jet_chains_creator.createJetChain('4j55_a4tchad_L2FSj15had','L2_4j15_a4TThad', 'L1_4J15', 4443),
    jet_chains_creator.createJetChain('4j55_a4tchad_L2FSj25had','L2_4j25_a4TThad', 'L1_4J15', 4474),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FSj15had','L2_5j15_a4TThad', 'L1_4J15', 6945),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FSj25had','L2_5j25_a4TThad', 'L1_4J15', 6946),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FSj15had','L2_6j15_a4TThad', 'L1_4J15', 6947),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FSj25had','L2_6j25_a4TThad', 'L1_4J15', 4478),

    jet_chains_creator.createJetChain('5j55_a4tchad_L2PS','L2_5j50_a4cchad', 'L1_4J15', 1923),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2PS','L2_6j50_a4cchad', 'L1_5J15', 6925),

    jet_chains_creator.createJetChain('4j55_a4tchad_L2FSPS','L2_4j15_a4TTem_4j50_a4cchad', 'L1_4J15', 216),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FSPS','L2_4j15_a4TTem_5j50_a4cchad', 'L1_4J15', 224),
    jet_chains_creator.createJetChain('6j55_a4tchad_L2FSPS','L2_4j15_a4TTem_6j50_a4cchad', 'L1_4J15', 226),
    jet_chains_creator.createJetChain('7j55_a4tchad_L2FSPS','L2_4j15_a4TTem_7j50_a4cchad', 'L1_4J15', 218),
    
    jet_chains_creator.createJetChain('4j110_a4tchad_L2FSPS',       'L2_4j15_a4TTem_4j105_a4cchad', 'L1_4J15', 127),
    jet_chains_creator.createJetChain('5j60_a4tchad_L2FSPS',        'L2_4j15_a4TTem_5j55_a4cchad', 'L1_4J15', 119),
    jet_chains_creator.createJetChain('6j50_a4tchad_L2FSPS_5L2j15', 'L2_5j15_a4TTem_6j45_a4cchad', 'L1_4J15', 108),
    jet_chains_creator.createJetChain('7j40_a4tchad_L2FSPS_5L2j15', 'L2_5j15_a4TTem_7j35_a4cchad', 'L1_4J15', 111),
    jet_chains_creator.createJetChain('8j35_a4tchad_L2FSPS_5L2j15', 'L2_5j15_a4TTem_8j30_a4cchad', 'L1_4J15', 112),

    ###L2FShadPS
    jet_chains_creator.createJetChain('4j80_a4tchad_L2FShad', 'L2_4j40_a4TThad', 'L1_4J15', 446),
    jet_chains_creator.createJetChain('4j80_a4tchad_L2FShadPS', 'L2_4j35_a4TThad_4j75_a4cchad', 'L1_4J15', 445),
    jet_chains_creator.createJetChain('5j55_a4tchad_L2FShadPS', 'L2_4j35_a4TThad_5j50_a4cchad', 'L1_4J15', 441),
    jet_chains_creator.createJetChain('6j45_a4tchad_L2FShadPS', 'L2_4j35_a4TThad_6j40_a4cchad', 'L1_4J15', 434),
    jet_chains_creator.createJetChain('fj55_a4tchad_L2FShad', 'L2_fj30_a4TThad', 'L1_RD0_FILLED', 242),

    
    # this one should be implemented as a combined chain
    jet_chains_creator.createJetChain('2j55_a4tchad','L2_2j50_c4cchad', 'L1_J15', 413),
    CombinedChain2('j110_a10tcem_L2FS_2j55_a4tchad_4L1J15', '6514', ['jets'], ['RATE:MultiJet', 'BW:Jets'], ['RATE:MultiJet', 'BW:Jets'],
                   'L2_j75_a10TTem_2j50_c4cchad', ['L2_j75_a10TTem', 'L2_2j50_c4cchad'], 'L1_4J15', 1, 0,
                   'EF_j110_a10tcem_L2FS_2j55_a4tchad', ['EF_j110_a10tcem_L2FS', 'EF_2j55_a4tchad'], 1, 0, doOrderEF=True),


    # Eta Max chains for Stopped Hadrons
    jet_chains_creator.createJetChain('j30_a4tcem_eta13_firstempty','L2_L1J10_firstempty_NoAlg', 'L1_J10_FIRSTEMPTY', 7578),
    jet_chains_creator.createJetChain('j50_a4tcem_eta13_firstempty','L2_L1J30_firstempty_NoAlg', 'L1_J30_FIRSTEMPTY', 7579),
    jet_chains_creator.createJetChain('j50_a4tcem_eta25_firstempty','L2_L1J30_firstempty_NoAlg', 'L1_J30_FIRSTEMPTY', 7580),
    jet_chains_creator.createJetChain('j30_a4tcem_eta13_empty','L2_L1J10_empty_NoAlg', 'L1_J10_EMPTY', 7581),
    jet_chains_creator.createJetChain('j50_a4tcem_eta13_empty','L2_L1J30_empty_NoAlg', 'L1_J30_EMPTY', 7582),
    jet_chains_creator.createJetChain('j50_a4tcem_eta25_empty','L2_L1J30_empty_NoAlg', 'L1_J30_EMPTY', 7583),

    #EF only chain, to be combined with MET
    jet_chains_creator.createJetChain('j40_j20_a4tchad', '', '', 3016),
    jet_chains_creator.createJetChain('j40_a4tchad', '', '', 3019),
    
    ]

## Jet Chains seeded from MBTS or TE20

Jets += [

    # MBTS seeded chains
    jet_chains_creator.createJetChain('j15_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 551, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j25_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 552, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j35_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 553),
    jet_chains_creator.createJetChain('fj15_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 554),
    jet_chains_creator.createJetChain('fj25_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 555),
    jet_chains_creator.createJetChain('fj35_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7556),
    jet_chains_creator.createJetChain('3j25_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7557),
    # May not be needed
    jet_chains_creator.createJetChain('4j25_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7558),
    # Probably not needed
    jet_chains_creator.createJetChain('5j25_a4tchad_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7559),

    # Special Low pT fex chains
    jet_chains_creator.createJetChain('j10_a4tchadloose_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 560, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('fj10_a4tchadloose_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 561),
    jet_chains_creator.createJetChain('3j10_a4tchadloose_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 562),
    jet_chains_creator.createJetChain('4j10_a4tchadloose_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 549),
    jet_chains_creator.createJetChain('5j10_a4tchadloose_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7564),

    # L1.5 Chains
    jet_chains_creator.createJetChain('fj45_a4tchad_L2FS_L1MBTS', 'L2_fj10_a4TTem_fj40_c4cchad_L1MBTS', 'L1_MBTS_2', 565),
    jet_chains_creator.createJetChain('fj55_a4tchad_L2FS_L1MBTS', 'L2_fj15_a4TTem_fj50_c4cchad_L1MBTS', 'L1_MBTS_2', 7566),


    
    jet_chains_creator.createJetdEtaChain('j10_fj10_a4tchadloose_deta50_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 567),
    jet_chains_creator.createJetdEtaChain('j15_fj15_a4tchad_deta50_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7568),
    jet_chains_creator.createJetdEtaChain('j25_fj25_a4tchad_deta50_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 569),
    jet_chains_creator.createJetdEtaChain('j35_fj35_a4tchad_deta50_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 7570),
    jet_chains_creator.createJetdEtaChain('2j10_a4tchadloose_deta35_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 571),
    jet_chains_creator.createJetdEtaChain('2j15_a4tchad_deta35_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 573),
    jet_chains_creator.createJetdEtaChain('2j25_a4tchad_deta35_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 574),
    jet_chains_creator.createJetdEtaChain('2j35_a4tchad_deta35_FC_L1MBTS', 'L2_mbMbts_2_NoAlg', 'L1_MBTS_2', 575),
    jet_chains_creator.createJetdEtaChain('2fj45_a4tchad_deta50_FB_L2FS_L1MBTS', 'L2_2fj10_a4TTem_2fj40_c4cchad_L1MBTS', 'L1_MBTS_2', 576),
    jet_chains_creator.createJetdEtaChain('2fj55_a4tchad_deta50_FB_L2FS_L1MBTS', 'L2_2fj15_a4TTem_2fj50_c4cchad_L1MBTS', 'L1_MBTS_2', 7577),


    
    # TE20 seeded chains
    jet_chains_creator.createJetChain('j15_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 508, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j25_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 509, ef_streams='MinBias'),
    jet_chains_creator.createJetChain('j35_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 7510),
    jet_chains_creator.createJetChain('fj15_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 511),
    jet_chains_creator.createJetChain('fj25_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 512),
    jet_chains_creator.createJetChain('fj35_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 7515),
    jet_chains_creator.createJetChain('3j25_a4tchad_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 516),

    jet_chains_creator.createJetdEtaChain('2j15_a4tchad_deta35_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 517),
    jet_chains_creator.createJetdEtaChain('2j25_a4tchad_deta35_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 518),
    jet_chains_creator.createJetdEtaChain('2j35_a4tchad_deta35_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 7519),
    jet_chains_creator.createJetdEtaChain('j15_fj15_a4tchad_deta50_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 7520),
    jet_chains_creator.createJetdEtaChain('j25_fj25_a4tchad_deta50_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 522),
    jet_chains_creator.createJetdEtaChain('j35_fj35_a4tchad_deta50_FC_L1TE20', 'L2_L1TE20_NoAlg', 'L1_TE20', 7523),

    jet_chains_creator.createJetChain('fj45_a4tchad_L2FS_L1TE20','L2_fj10_a4TTem_fj40_c4cchad_L1TE20','L1_TE20',540),
    jet_chains_creator.createJetChain('fj55_a4tchad_L2FS_L1TE20','L2_fj15_a4TTem_fj50_c4cchad_L1TE20','L1_TE20',541),
    jet_chains_creator.createJetdEtaChain('2fj45_a4tchad_deta50_FB_L2FS_L1TE20', 'L2_2fj10_a4TTem_2fj40_c4cchad_L1TE20', 'L1_TE20', 544),
    jet_chains_creator.createJetdEtaChain('2fj55_a4tchad_deta50_FB_L2FS_L1TE20', 'L2_2fj15_a4TTem_2fj50_c4cchad_L1TE20', 'L1_TE20', 7548),


    ]


Jets += [
    
    #jet chains needed for bjet slice:

    jet_chains_creator.createL2JetChain('L2_j95_c4cchad', 'L1_J75', 3136),

    #4J15 seeded chains
    jet_chains_creator.createJetChain('4j55_a4tchad','L2_4j50_c4cchad','L1_4J15',3465),
    jet_chains_creator.createJetChain('4j45_a4tchad','L2_4j40_c4cchad','L1_4J15', 7651),
    jet_chains_creator.createJetChain('4j35_a4tchad_L2FS', 'L2_4j15_a4TTem', 'L1_4J15', 3569),

    jet_chains_creator.createL2JetChain('L2_j30_c4cchad','L1_J15',3136),
    jet_chains_creator.createJetChain('4j35_a4tchad', 'L2_4j30_c4cchad', 'L1_4J15', 3571),
    
    jet_chains_creator.createL2JetChain('L2_j50_c4cchad_4L1J15','L1_4J15',3133),
    jet_chains_creator.createL2JetChain('L2_j50_a4TTem_4L1J15','L1_4J15',3137),
    jet_chains_creator.createL2JetChain('L2_j40_c4cchad_4L1J15','L1_4J15',3134),
    jet_chains_creator.createL2JetChain('L2_j30_c4cchad_4L1J15','L1_4J15',3135),
    jet_chains_creator.createJetChain('3j55_a4tchad_4L1J15','L2_3j50_c4cchad_4L1J15','L1_4J15',3466),
    jet_chains_creator.createJetChain('3j55_a4tchad_L2FS_4L1J15','L2_3j15_a4TTem_4L1J15','L1_4J15',3468),
    jet_chains_creator.createJetChain('3j45_a4tchad_4L1J15','L2_3j40_c4cchad_4L1J15','L1_4J15',3467),
    jet_chains_creator.createJetChain('3j35_a4tchad_4L1J15','L2_3j30_c4cchad_4L1J15','L1_4J15',3469),


    #4J10 seeded chains
    jet_chains_creator.createL2JetChain('L2_j50_c4cchad_4L1J10','L1_4J10',2131),
    jet_chains_creator.createL2JetChain('L2_j40_c4cchad_4L1J10','L1_4J10',2133),
    jet_chains_creator.createL2JetChain('L2_j30_c4cchad_4L1J10','L1_4J10',2132),
    jet_chains_creator.createL2JetChain('L2_j15_a4TTem_4L1J10','L1_4J10',2131),
    jet_chains_creator.createL2JetChain('L2_j10_a4TTem_4L1J10','L1_4J10',2132),

    jet_chains_creator.createJetChain('4j55_a4tchad_4L1J10','L2_4j50_c4cchad_4L1J10','L1_4J10',4131),
    jet_chains_creator.createJetChain('4j45_a4tchad_4L1J10','L2_4j40_c4cchad_4L1J10','L1_4J10',4132),
    jet_chains_creator.createJetChain('3j35_a4tchad_4L1J10','L2_3j30_c4cchad_4L1J10','L1_4J10',4133),
    jet_chains_creator.createJetChain('4j55_a4tchad_L2FS_4L1J10','L2_4j10_a4TTem_4L1J10','L1_4J10',4334),
    jet_chains_creator.createJetChain('3j35_a4tchad_L2FS_4L1J10','L2_3j10_a4TTem_4L1J10','L1_4J10',4434),

    #5J10 seeded chains
    jet_chains_creator.createL2JetChain('L2_j10_a4TTem_5L1J10','L1_5J10',4835),
    jet_chains_creator.createL2JetChain('L2_j30_c4cchad_5L1J10','L1_5J10',4735),
    jet_chains_creator.createJetChain('4j35_a4tchad_L2FS_5L1J10','L2_4j10_a4TTem_5L1J10','L1_5J10',4135),
    jet_chains_creator.createJetChain('4j35_a4tchad_5L1J10','L2_4j30_c4cchad_5L1J10','L1_5J10',4136),


    #L2FS chains
    jet_chains_creator.createJetChain('j110_a10tcem_L2FS_L1J75','L2_j100_a10TTem_L1J75', 'L1_J75', 3470),
    jet_chains_creator.createJetChain('j180_a10tcem_L2FS','L2_j100_a10TTem', 'L1_J75', 3472),
    jet_chains_creator.createJetChain('j240_a10tcem_L2FS','L2_j100_a10TTem','L1_J75',2487),

    jet_chains_creator.createJetChain('j80_a4tchad_L2FS','L2_j30_a4TTem', 'L1_J30', 3201),
    jet_chains_creator.createJetChain('j110_a4tchad_L2FS','L2_j50_a4TTem', 'L1_J50', 3202),
    jet_chains_creator.createJetChain('j145_a4tchad_L2FS','L2_j75_a4TTem', 'L1_J75', 3203),
    jet_chains_creator.createJetChain('j180_a4tchad_L2FS','L2_j75_a4TTem', 'L1_J75', 3204),
    jet_chains_creator.createJetChain('j220_a4tchad_L2FS','L2_j75_a4TTem', 'L1_J75', 3205),
    jet_chains_creator.createJetChain('j280_a4tchad_L2FS','L2_j75_a4TTem', 'L1_J75', 3206),
    jet_chains_creator.createJetChain('j360_a4tchad_L2FS','L2_j75_a4TTem', 'L1_J75', 3207),


    #L2 140 a10tcem
    jet_chains_creator.createJetChain('j240_a10tcem_L2j140','L2_j140_c4cchad','L1_J75',2479),
    jet_chains_creator.createJetChain('j180_a10tcem_L2j140','L2_j140_c4cchad', 'L1_J75', 3473), 
    jet_chains_creator.createJetChain('j220_a10tcem','L2_j140_c4cchad', 'L1_J75', 3474),
    jet_chains_creator.createJetChain('j280_a10tcem','L2_j140_c4cchad', 'L1_J75', 3475),

    #L2 a4tchad
    jet_chains_creator.createJetChain('j180_a4tchad_L2j140','L2_j140_c4cchad','L1_J75',3575),
    jet_chains_creator.createJetChain('j220_a4tchad_L2j140','L2_j140_c4cchad','L1_J75',3576),
    jet_chains_creator.createJetChain('j280_a4tchad_L2j140','L2_j140_c4cchad','L1_J75',3577),
    jet_chains_creator.createJetChain('j360_a4tchad_L2j140','L2_j140_c4cchad','L1_J75',3578),

    #J75 seeded chains
    jet_chains_creator.createJetChain('j145_2j45_a4tchad','L2_j140_c4cchad', 'L1_J75', 3792),
    
    jet_chains_creator.createJetChain('j145_j55_a4tchad','L2_j140_j50_c4cchad', 'L1_J75', 3492),
    jet_chains_creator.createJetChain('j145_j45_a4tchad','L2_j140_j40_c4cchad', 'L1_J75', 3493),
    jet_chains_creator.createJetChain('j145_j100_j35_a4tchad', 'L2_j140_j95_j30_c4cchad', 'L1_J75', 3573),
    jet_chains_creator.createJetChain('j145_2j35_a4tchad', 'L2_j140_2j30_c4cchad', 'L1_J75', 3139),
    jet_chains_creator.createJetChain('j145_j35_a4tchad', 'L2_j140_j30_c4cchad', 'L1_J75', 3140),

    #jet_chains_creator.createJetChain('j145_j55_a4tchad','L2_j140_j50_c4cchad', 'L1_J75', 3492),
    #jet_chains_creator.createJetChain('j145_j45_a4tchad','L2_j140_j40_c4cchad', 'L1_J75', 3493),

    jet_chains_creator.createJetChain('j100_a4tchad', 'L2_j95_c4cchad', 'L1_J75', 3572),
    jet_chains_creator.createJetChain('j170_a4tchad', 'L2_j165_c4cchad', 'L1_J75', 3574),

    
    #J15, J20 seed
    jet_chains_creator.createJetChain('j35_a4tchad_L1J15','L2_j30_c4cchad_L1J15','L1_J15', 3468),
    jet_chains_creator.createJetChain('j45_a4tchad_L1J15','L2_j40_c4cchad_L1J15', 'L1_J15', 3409),
    #jet_chains_creator.createJetChain('j45_a4tchad_L2FS_L1J15','L2_j40_c4cchad_L2FS_L1J15', 'L1_J15', 3408),
    jet_chains_creator.createJetChain('j65_a4tchad','L2_j60_c4cchad','L1_J20',3476),

    jet_chains_creator.createJetChain('j55_a4tchad_L1J20','L2_j50_c4cchad_L1J20', 'L1_J20', 5341),
    jet_chains_creator.createJetChain('2j55_a4tchad_L1J20','L2_2j50_c4cchad_L1J20', 'L1_J20', 5342),
    jet_chains_creator.createJetChain('2j65_a4tchad','L2_2j60_c4cchad', 'L1_J20', 5343),

    #L1_2J15_J50
    jet_chains_creator.createJetChain('j110_j55_a4tchad','L2_j105_j50_c4cchad', 'L1_2J15_J50', 3479),
    jet_chains_creator.createJetChain('j110_j45_a4tchad','L2_j105_j40_c4cchad', 'L1_2J15_J50', 3491),
    jet_chains_creator.createJetChain('j110_2j35_a4tchad','L2_j105_2j30_c4cchad', 'L1_3J15_J50', 3494),

    # FJ seed
    jet_chains_creator.createJetChain('j35_a4tchad_L2FS_2L1FJ15', 'L2_j30_a4TTem_2L1FJ15', 'L1_2FJ15', 7824),
    jet_chains_creator.createJetChain('j35_a4tchad_L2FS_3L1J15_FJ15', 'L2_j30_a4TTem_3L1J15_FJ15', 'L1_3J15FJ15', 7826),

    jet_chains_creator.createJetChain('j35_a4tchad_2L1FJ15', 'L2_j30_c4cchad_2L1FJ15', 'L1_2FJ15', 7825),
    jet_chains_creator.createJetChain('j35_a4tchad_3L1J15_FJ15', 'L2_j30_c4cchad_3L1J15_FJ15', 'L1_3J15FJ15', 7827),
    
    #ht chains
    jet_chains_creator.createJetTopoChain('j110_j55_a4tchad_ht500','L2_j105_j50_c4cchad', 'L1_2J15_J50', 3480),
    jet_chains_creator.createJetTopoChain('j165_j55_a4tchad_ht500','L2_j160_j50_c4cchad', 'L1_J75', 3481),
    jet_chains_creator.createJetTopoChain('j165_j80_a4tchad_ht500','L2_j160_j75_c4cchad', 'L1_J75', 3482),
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht400','L2_j140_c4cchad','L1_J75',3477),    
    jet_chains_creator.createJetTopoChain('j145_j45_a4tchad_ht400','L2_j140_j40_c4cchad','L1_J75',3579),
    jet_chains_creator.createJetTopoChain('j145_j45_a4tchad_ht500','L2_j140_j40_c4cchad','L1_J75',3580), 
    jet_chains_creator.createJetTopoChain('j165_2j55_a4tchad_ht500','L2_j160_2j50_c4cchad','L1_J75',3478),

    ]

# New Delayed Jet Chains
# handled by parsing: looks for 'delayed' in the chain name

Jets += [
    
    jet_chains_creator.createJetChain('4j65_a4tchad_L2FS_delayed','L2_4j20_a4TTem', 'L1_4J15', 1015),
    jet_chains_creator.createJetChain('5j45_a4tchad_L2FS_delayed','L2_5j15_a4TTem', 'L1_4J15', 1016),
    jet_chains_creator.createJetTopoChain('j145_a4tchad_ht500_L2FS_delayed','L2_j75_a4TTem_ht300_j140_c4cchad','L1_J75',1017),
    jet_chains_creator.createJetChain('j220_a10tcem_delayed','L2_j165_c4cchad', 'L1_J75', 901),
    jet_chains_creator.createJetChain('j240_a10tcem_delayed','L2_j165_c4cchad', 'L1_J75', 909),
    jet_chains_creator.createJetChain('j280_a4tchad_delayed','L2_j165_c4cchad', 'L1_J75', 913),
    jet_chains_creator.createJetChain('j360_a10tclcw_delayed','L2_j165_c4cchad', 'L1_J75', 929),

    jet_chains_creator.createJetChain('j15_a4tchad_jetcalibdelayed', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',2011),
    jet_chains_creator.createJetChain('j25_a4tchad_jetcalibdelayed', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',2012),
    jet_chains_creator.createJetChain('j35_a4tchad_jetcalibdelayed', 'L2_rd0_filled_NoAlg', 'L1_RD0_FILLED',2013),
    jet_chains_creator.createJetChain('j55_a4tchad_jetcalibdelayed', 'L2_j50_c4cchad', 'L1_J15', 2004),
    jet_chains_creator.createJetChain('j80_a4tchad_jetcalibdelayed', 'L2_j75_c4cchad', 'L1_J30', 2005),
    jet_chains_creator.createJetChain('j110_a4tchad_jetcalibdelayed', 'L2_j105_c4cchad', 'L1_J50', 2006),

    ]

# New Jet Cleaning Chains
Jets += [
    jet_chains_creator.createJetChain('j55_a4tchad_testCleaning','L2_j50_c4cchad_testCleaning', 'L1_J15', 2763),
    jet_chains_creator.createJetChain('j360_a4tchad_testCleaning','L2_j165_c4cchad_testCleaning','L1_J75', 2866),
    jet_chains_creator.createJetChain('j45_a4tchad_L2FS_L1J15_testCleaning','L2_j15_a4TTem_j40_c4cchad_testCleaning', 'L1_J15', 2868),
    jet_chains_creator.createJetChain('fj45_a4tchad_L2FS_testCleaning','L2_fj10_a4TTem_fj40_c4cchad_testCleaning', 'L1_RD0_FILLED', 2952),
]


# New L2 VBF jet chains
Jets += [
    
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L1TAU8MU10', 'L2_2j15_a4TTem_deta25_L1TAU8MU10', 'L1_TAU8_MU10', 7811),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11ITAU15', 'L2_2j15_a4TTem_deta25_L12TAU11ITAU15', 'L1_2TAU11I_TAU15', 7812),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11IEM14VH', 'L2_2j15_a4TTem_deta25_L12TAU11IEM14VH', 'L1_2TAU11I_EM14VH', 7813),

    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L1MU10FJ15', 'L2_2j15_a4TTem_deta25_L1MU10FJ15', 'L1_MU10_FJ15', 7818),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L1MU10J15CF', 'L2_2j15_a4TTem_deta25_L1MU10J15CF', 'L1_MU10_J15CF', 7819),


    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU11IEM10VHFJ15','L2_2j15_a4TTem_deta25_L12TAU8TAU11IEM10VHFJ15','L1_2TAU8_TAU11I_EM10VH_FJ15',7816),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU11IEM10VHJ15CF','L2_2j15_a4TTem_deta25_L12TAU8TAU11IEM10VHJ15CF','L1_2TAU8_TAU11I_EM10VH_J15CF',7817),
    
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU15FJ15',  'L2_2j15_a4TTem_deta25_L12TAU8TAU15FJ15', 'L1_2TAU8_TAU15_FJ15', 7814),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU15J15CF', 'L2_2j15_a4TTem_deta25_L12TAU8TAU15J15CF', 'L1_2TAU8_TAU15_J15CF', 7815),

   
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L1EM14VHFJ15', 'L2_2j15_a4TTem_deta25_L1EM14VHFJ15', 'L1_EM14VH_FJ15', 7820),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L1EM14VHJ15CF','L2_2j15_a4TTem_deta25_L1EM14VHJ15CF', 'L1_EM14VH_J15CF', 7821),

    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L13J15FJ15','L2_2j15_a4TTem_deta25_L13J15FJ15', 'L1_3J15_FJ15', 7822),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_L15CF',     'L2_2j15_a4TTem_deta25_L1EM14VHJ15CF','L1_EM14VH_J15CF', 7823),
    jet_chains_creator.createJetdEtaMjjChain('2j25_a4tchad_eta50_mjj400deta28_L2FS_2L1FJ15',   'L2_2j15_a4TTem_deta25_2L1FJ15', 'L1_2FJ15', 7828),

    ]



#chains for alfa v2 menu
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus():
    alfa_seed_sdiff ='L1_ALFA_SDIFF1,L1_ALFA_SDIFF2,L1_ALFA_SDIFF3,L1_ALFA_SDIFF4'
else:
    alfa_seed_sdiff = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'
    
Jets += [
    
    jet_chains_creator.createJetChain('j10_a4tchadloose_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY', 619),
    jet_chains_creator.createJetChain('j15_a4tchad_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY',648),
    jet_chains_creator.createJetChain('j25_a4tchad_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY',649), 
    jet_chains_creator.createJetChain('j35_a4tchad_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY',650),
    
    jet_chains_creator.createJetChain('j10_a4tchadloose_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff, 651),
    jet_chains_creator.createJetChain('j15_a4tchad_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff,652),
    jet_chains_creator.createJetChain('j25_a4tchad_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff,655), 
    jet_chains_creator.createJetChain('j35_a4tchad_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff,654),

    jet_chains_creator.createJetChain('fj10_a4tchadloose_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY', 675),
    jet_chains_creator.createJetChain('fj15_a4tchad_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY',656),
    jet_chains_creator.createJetChain('fj25_a4tchad_L1ALFAANY', 'L2_alfa_any_NoAlg', 'L1_ALFA_ANY',657), 

    jet_chains_creator.createJetChain('fj10_a4tchadloose_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff, 658),
    jet_chains_creator.createJetChain('fj15_a4tchad_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff,659),
    jet_chains_creator.createJetChain('fj25_a4tchad_L1ALFASDIFF', 'L2_alfa_sdiff_NoAlg', alfa_seed_sdiff,660), 

    
    ]

################################################################################################
#
#Dummy L2 chains to require some ROIs (used in combined chains seeded from EMROI using L1.5 jets)
#

from TriggerMenuPython.HltConfig import *
class L2EFChain_Dummy(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config, prescale=1, pass_through=0):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.L2_Dummy = DummyFEX("L2%s" % suffix)

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config,EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig

    def defineSequences(self, config):
        self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_dummy')

    def defineSignatures(self):
        self.addL2Signature(['L2_dummy'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            #'L2_' : mergeRemovingOverlap('L2_st', self.config.L2Config.suffix),
            'L2_dummy' : 'L2_roi'+self.config.L2Config.suffix,
            #'L2_dummy' : mergeRemovingOverlap('L2_dummy', self.config.L2Config.suffix),
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.groups = ["RATE:JetMETJetTauEtmiss"]
        self.physics_streams = ["JetTauEtmiss"]
        
      
Config   = L2EFChain_Dummy.Config
L2create = L2EFChain_Dummy.L2Config
EFcreate = L2EFChain_Dummy.EFConfig

Jets += [
    L2EFChain_Dummy('roi_dummy_J15',
                    'L2_roi_dummy_J15', 1018, 'L1_3J15','EF_roi_dummy_J15', 1018, ['J15'],
                    Config(L2create('_dummy_J15'),
                           EFcreate('_dummy_J15'))),

    CombinedChain2('roi_dummy_3J15', '2019', ['jets'], [], [], 
                   'L2_roi_dummy_3J15', ['L2_roi_dummy_J15', 'L2_roi_dummy_J15', 'L2_roi_dummy_J15'], 'L1_3J15',1, 0, 
                   'EF_roi_dummy_3J15', ['EF_roi_dummy_J15', 'EF_roi_dummy_J15', 'EF_roi_dummy_J15'], 1, 0),
    
    CombinedChain2('roi_dummy_4J15', '2020', ['jets'], [], [], 
                   'L2_roi_dummy_4J15', ['L2_roi_dummy_J15', 'L2_roi_dummy_J15', 'L2_roi_dummy_J15', 'L2_roi_dummy_J15'], 'L1_4J15',1, 0, 
                   'EF_roi_dummy_4J15', ['EF_roi_dummy_J15', 'EF_roi_dummy_J15', 'EF_roi_dummy_J15', 'EF_roi_dummy_J15'], 1, 0),


    ]



hypos.ef_2j20 = EFCentFullScanMultiJetHypo('EFjetHypo_ef_2j20', ef_thr=20*GeV, multiplicity=2)
hypos.ef_3j15 = EFCentFullScanMultiJetHypo('EFjetHypo_ef_3j15', ef_thr=15*GeV, multiplicity=3)
hypos.ef_2j20_a4hi = EFCentFullScanMultiJetHypo('EFjetHypo_2j20_a4hi_EFFS', ef_thr=20*GeV, multiplicity=2, ef_thrs=[20.*GeV, 20.*GeV])


#print "DDD"
#print "Hypos"
#import pprint
#pprint.pprint( hypos.__dict__ )

class JetSeededOffMBTS(ChainTemplate):
    def __init__(self, sig, ef, efseq):
        ChainTemplate.__init__(self, sig, l2=None, ef=ef)
        if "tc" in sig:
            self.addSequence('',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo],'EF_FSTopoClusters')
        elif "hi" in sig:
            self.addSequence('',[fexes.roi, fexes.cell_maker_fullcalo_topo, fexes.tower_maker],'EF_FSTowersHI')
        self.continueSequence(efseq, self.ef.chain_name).addSignature()
        self.addGroups('BW:Jets')
        if '2j' in sig:
            self.addGroups('RATE:MultiJet')
        else:
            self.addGroups('RATE:SingleJet')
        self.addStreams('jets')

# min bias seeded jets for p+Pb



Jets += [
    # HI jets
    JetSeededOffMBTS('j7_a4hi_EFFS_L2mbMbts',  ef=Chain(counter=386, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j7_a4hi_EFFS]),
    JetSeededOffMBTS('j10_a4hi_EFFS_L2mbMbts', ef=Chain(counter=403, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j10_a4hi_EFFS]),
    JetSeededOffMBTS('j15_a4hi_EFFS_L2mbMbts', ef=Chain(counter=406, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j15_a4hi_EFFS]),
    JetSeededOffMBTS('j20_a4hi_EFFS_L2mbMbts', ef=Chain(counter=272, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j20_a4hi_EFFS]),
    JetSeededOffMBTS('j30_a4hi_EFFS_L2mbMbts', ef=Chain(counter=291, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j30_a4hi_EFFS]),    
    JetSeededOffMBTS('j40_a4hi_EFFS_L2mbMbts', ef=Chain(counter=292, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j40_a4hi_EFFS]),
    JetSeededOffMBTS('j50_a4hi_EFFS_L2mbMbts', ef=Chain(counter=284, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.j50_a4hi_EFFS]),

    JetSeededOffMBTS('j7_a4tchad_EFFS_L2mbMbts',  ef=Chain(counter=487, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j7]),
    JetSeededOffMBTS('j10_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=410, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j10]),
    JetSeededOffMBTS('j15_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=496, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j15]),
    JetSeededOffMBTS('j20_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=266, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j20]),
    JetSeededOffMBTS('j30_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=267, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j30]),
    JetSeededOffMBTS('j40_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=268, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j40]),
    JetSeededOffMBTS('j50_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=270, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j50]),    

    #forward jets
    JetSeededOffMBTS('fj7_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=820, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj7] ),
    JetSeededOffMBTS('fj7_a4hi_EFFS_L2mbMbts',    ef=Chain(counter=871, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.fj7_a4hi_EFFS] ),
    JetSeededOffMBTS('fj10_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=72, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj10] ),
    JetSeededOffMBTS('fj10_a4hi_EFFS_L2mbMbts',    ef=Chain(counter=73, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.fj10_a4hi_EFFS] ),
    JetSeededOffMBTS('fj15_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=74, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj15] ),
    JetSeededOffMBTS('fj15_a4hi_EFFS_L2mbMbts',    ef=Chain(counter=75, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.fj15_a4hi_EFFS] ),
    
    
    JetSeededOffMBTS('2j7_a4tchad_deta40_L2mbMbts',  ef=Chain(counter=538, seed='L2_mbMbts_1_1'),  efseq=[fexes.ef_antikt4_tc_had,  hypos.ef_2j7, hypos.ef_2j7_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j10_a4tchad_deta40_L2mbMbts', ef=Chain(counter=539, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10, hypos.ef_2j10_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j15_a4tchad_deta40_L2mbMbts', ef=Chain(counter=542, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j15, hypos.ef_2j15_Mjj0_deta4p0]),

    JetSeededOffMBTS('2j7_a4tchad_eta50_deta40_L2mbMbts',  ef=Chain(counter=543, seed='L2_mbMbts_1_1'),  efseq=[fexes.ef_antikt4_tc_had,  hypos.ef_2j7_eta5p0, hypos.ef_2j7_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j10_a4tchad_eta50_deta40_L2mbMbts', ef=Chain(counter=545, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10_eta5p0, hypos.ef_2j10_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j15_a4tchad_eta50_deta40_L2mbMbts', ef=Chain(counter=550, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j15_eta5p0, hypos.ef_2j15_Mjj0_deta4p0]),

    JetSeededOffMBTS('2j7_a4hi_deta40_L2mbMbts',  ef=Chain(counter=559, seed='L2_mbMbts_1_1'),  efseq=[fexes.ef_antikt4_hi_em,  hypos.__dict__['2j7_a4hi_EFFS'], hypos.ef_2j7_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j10_a4hi_deta40_L2mbMbts', ef=Chain(counter=591, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em,   hypos.__dict__['2j10_a4hi_EFFS'], hypos.ef_2j10_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j15_a4hi_deta40_L2mbMbts', ef=Chain(counter=665, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em,   hypos.__dict__['2j15_a4hi_EFFS'], hypos.ef_2j15_Mjj0_deta4p0]),

    JetSeededOffMBTS('2j7_a4hi_eta50_deta40_L2mbMbts',  ef=Chain(counter=670, seed='L2_mbMbts_1_1'),  efseq=[fexes.ef_antikt4_hi_em,  hypos.__dict__['2j7_a4hi_EFFS_eta5p0'], hypos.ef_2j7_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j10_a4hi_eta50_deta40_L2mbMbts', ef=Chain(counter=671, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em,   hypos.__dict__['2j10_a4hi_EFFS_eta5p0'], hypos.ef_2j10_Mjj0_deta4p0]),
    JetSeededOffMBTS('2j15_a4hi_eta50_deta40_L2mbMbts', ef=Chain(counter=672, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em,   hypos.__dict__['2j15_a4hi_EFFS_eta5p0'], hypos.ef_2j15_Mjj0_deta4p0]),


    JetSeededOffMBTS('2j7_a4tchad_EFFS_L2mbMbts',  ef=Chain(counter=556, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j7]),
    JetSeededOffMBTS('2j10_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=557, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j10]),
    JetSeededOffMBTS('2j15_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=558, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j15]),
    JetSeededOffMBTS('2j20_a4tchad_EFFS_L2mbMbts', ef=Chain(counter=637, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j20]),
    
    JetSeededOffMBTS('2j7_a4hi_EFFS_L2mbMbts',  ef=Chain(counter=638, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.__dict__['2j7_a4hi_EFFS']]),
    JetSeededOffMBTS('2j10_a4hi_EFFS_L2mbMbts', ef=Chain(counter=639, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.__dict__['2j10_a4hi_EFFS']]),
    JetSeededOffMBTS('2j15_a4hi_EFFS_L2mbMbts', ef=Chain(counter=653, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.__dict__['2j15_a4hi_EFFS']]),
    JetSeededOffMBTS('2j20_a4hi_EFFS_L2mbMbts', ef=Chain(counter=361, seed='L2_mbMbts_1_1'), efseq=[fexes.ef_antikt4_hi_em, hypos.ef_2j20_a4hi]),

    # test chains
    ]

from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex
fexes.met_feb = L2CaloMissingET_Fex()
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBTE
hypos.met_l2_te5 = L2MetHypoFEBTE('L2MetHypo_FEB_te5', l2_thr=5.*GeV)
hypos.met_l2_te10 = L2MetHypoFEBTE('L2MetHypo_FEB_te10', l2_thr=10.*GeV)
from TrigT2MinBias.TrigT2MinBiasConfig import fexes as l2fexes
fexes.update(l2fexes)
from TrigT2MinBias.TrigT2MinBiasConfig import hypos as l2hypos
hypos.update(l2hypos)

  
# Min Bias seeded jets for pp
L1random="" # in the data taking we want to run seeded from L1_RD0 but in the reprocessing this seed is useless so we replace it for the moment wiht the "" == OR of all L1
# must become L1_RD0_FILLED
class JetSeededOffRandom(ChainTemplate):
    def __init__(self, sig, ef, l2, efseq):
        ef.name = "EF_"+sig
        ChainTemplate.__init__(self, sig, l2=l2, ef=ef)


        if 'L2te' in sig and self.l2 != None:
            self.continueSequence([fexes.met_feb, hypos.met_l2_te10], self.l2.chain_name).addSignature()

        if 'L2mbMbts' in sig and self.l2 != None:
            self.addSequence("", [fexes.L2MbMbtsFex, hypos.L2MbMbtsHypo_1_1], "L2_mbMbts_1_1" ).addSignature() # this is assuming that the sequence will places in the menu by MinBias chains
        # EF sequences
        if "tc" in sig:
            self.addSequence('',[fexes.roi_topo, fexes.cell_maker_fullcalo_topo, fexes.topocluster_maker_fullcalo],'EF_FSTopoClusters')
        elif "hi" in sig:
            self.addSequence('',[fexes.roi, fexes.cell_maker_fullcalo_topo, fexes.tower_maker],'EF_FSTowersHI')
        self.continueSequence(efseq, self.ef.chain_name).addSignature()

        self.addGroups('BW:Jets')
        if '2j' in sig:
            self.addGroups('RATE:MultiJet')
        else:
            self.addGroups('RATE:SingleJet')
        self.addStreams('jets')

Jets += [
#    JetSeededOffRandom('j10_a4hi_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1200, name="EF_j10_a4hi_EFFS_L2mbMbts_L1RD0"),
#                       l2=Chain(name="L2_mbMbts_RD0", seed=L1random, counter=1203),
#                       efseq=[fexes.ef_antikt4_hi_em, hypos.j10_a4hi_EFFS]),

    JetSeededOffRandom('j10_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1201, seed="L2_mbMbts_RD0"),
                       l2=Chain(name="L2_mbMbts_RD0", seed="L1_RD0_FILLED", counter=1203),
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j10]),

    JetSeededOffRandom('j15_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1202, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j15]),

    JetSeededOffRandom('j20_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1203, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j20]),

    JetSeededOffRandom('j30_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1204, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j30]),
    

    JetSeededOffRandom('2j10_a4tchad_eta50_deta40_L2mbMbts_L1RD0', ef=Chain(counter=1205, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10_eta5p0, hypos.ef_2j10_Mjj0_deta4p0]),

    JetSeededOffRandom('2j10_a4tchad_deta40_L2mbMbts_L1RD0', ef=Chain(counter=1206, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10, hypos.ef_2j10_Mjj0_deta4p0]),

#    JetSeededOffRandom('j10_a4hi_EFFS_L2te_L1RD0', ef=Chain(name="EF_j10_a4hi_EFFS_L2te_L1RD0", counter=1202),
#                       l2=Chain(name="L2_te10_RD0", seed=L1random, counter=1201),
#                       efseq=[fexes.ef_antikt4_hi_em, hypos.j10_a4hi_EFFS]),

    JetSeededOffRandom('j10_a4tchad_EFFS_L2te_L1RD0', ef=Chain( counter=1207, seed="L2_te10_RD0"),
                       l2=Chain(name="L2_te10_RD0", seed='L1_RD0_FILLED', counter=1201),
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j10]),

    JetSeededOffRandom('j15_a4tchad_EFFS_L2te_L1RD0', ef=Chain( counter=1208, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j15]),
    JetSeededOffRandom('j20_a4tchad_EFFS_L2te_L1RD0', ef=Chain( counter=1209, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j20]),

    JetSeededOffRandom('j30_a4tchad_EFFS_L2te_L1RD0', ef=Chain( counter=1210, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j30]),


    JetSeededOffRandom('2j10_a4tchad_eta50_deta40_L2te_L1RD0', ef=Chain(counter=1211, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10_eta5p0, hypos.ef_2j10_Mjj0_deta4p0]),

    JetSeededOffRandom('2j10_a4tchad_deta40_L2te_L1RD0', ef=Chain(counter=1212, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10, hypos.ef_2j10_Mjj0_deta4p0]),


    # this two chains need to be defined using L2_rd0_filled_NoAlg for the running
#    JetSeededOffRandom('j10_a4hi_EFFS_L1RD0', ef=Chain(name="EF_j10_a4hi_EFFS_L1RD0", counter=1204, seed="L2_rd0"), # this seed needs to be replaced by the L1_rd0_filled_NoAlg
#                       l2=Chain(name="L2_rd0", seed=L1random, counter=1202), efseq=[fexes.ef_antikt4_hi_em, hypos.j10_a4hi_EFFS]),
    
    JetSeededOffRandom('j10_a4tchad_EFFS_L1RD0', ef=Chain(name="EF_j10_a4tchad_EFFS_L1RD0", counter=1213, seed="L2_rd0_filled_NoAlg"),
                        l2=None, #Chain(name="L2_rd0", seed=L1random, counter=1204),
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j10]),
    
    JetSeededOffRandom('j15_a4tchad_EFFS_L1RD0', ef=Chain( counter=1214, seed="L2_rd0_filled_NoAlg"),
                        l2=None, efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j15]),

    JetSeededOffRandom('j20_a4tchad_EFFS_L1RD0', ef=Chain( counter=1215, seed="L2_rd0_filled_NoAlg"),
                        l2=None, efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j20]),

    JetSeededOffRandom('j30_a4tchad_EFFS_L1RD0', ef=Chain( counter=1216, seed="L2_rd0_filled_NoAlg"),
                       l2=None, efseq=[fexes.ef_antikt4_tc_had, hypos.ef_j30]),

    JetSeededOffRandom('2j10_a4tchad_eta50_deta40_L1RD0', ef=Chain(counter=1217, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10_eta5p0, hypos.ef_2j10_Mjj0_deta4p0]),
    
    JetSeededOffRandom('2j10_a4tchad_deta40_L1RD0', ef=Chain(counter=1218, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had,   hypos.ef_2j10, hypos.ef_2j10_Mjj0_deta4p0]),

    # 2 jet
    JetSeededOffRandom('2j10_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1219, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j10]),
    JetSeededOffRandom('2j15_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1220, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j15]),

    JetSeededOffRandom('2j10_a4tchad_EFFS_L2te_L1RD0', ef=Chain(counter=1221, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j10]),
    JetSeededOffRandom('2j15_a4tchad_EFFS_L2te_L1RD0', ef=Chain(counter=1222, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j15]),

    JetSeededOffRandom('2j10_a4tchad_EFFS_L1RD0', ef=Chain(counter=1223, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j10]),
    JetSeededOffRandom('2j15_a4tchad_EFFS_L1RD0', ef=Chain(counter=1224, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_2j15]),

    # forward jets
    JetSeededOffRandom('fj10_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1225, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj10] ),
    JetSeededOffRandom('fj15_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1226, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj15] ),
    #
    JetSeededOffRandom('fj10_a4tchad_EFFS_L2te_L1RD0', ef=Chain(counter=1227, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj10] ),
    JetSeededOffRandom('fj15_a4tchad_EFFS_L2te_L1RD0', ef=Chain(counter=1228, seed="L2_te10_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj15] ),
    #
    JetSeededOffRandom('fj10_a4tchad_EFFS_L1RD0', ef=Chain(counter=1229, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj10] ),
    JetSeededOffRandom('fj15_a4tchad_EFFS_L1RD0', ef=Chain(counter=1230, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_fj15] ),

    # 3 jets
    
    JetSeededOffRandom('3j15_a4tchad_EFFS_L1RD0', ef=Chain(counter=1231, seed="L2_rd0_filled_NoAlg"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_3j15]),
    
    JetSeededOffRandom('3j15_a4tchad_EFFS_L2te_L1RD0', ef=Chain(counter=1232, seed="L2_te10_RD0"),
                       l2=None,                       
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_3j15]),

    JetSeededOffRandom('3j15_a4tchad_EFFS_L2mbMbts_L1RD0', ef=Chain(counter=1233, seed="L2_mbMbts_RD0"),
                       l2=None,
                       efseq=[fexes.ef_antikt4_tc_had, hypos.ef_3j15]),

    
    ]
