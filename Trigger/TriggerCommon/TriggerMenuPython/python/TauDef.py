# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
# SliceDef file for Tau chains
###########################################################################
__author__  = 'O.Igonkina, P.Bell'
__version__=""
__doc__="Implementation of tau trigger sequence "

from AthenaCommon.Logging import logging

def logger():
    return logging.getLogger("TauDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from ChainTemplate import *

fexes = FreeStore()

from AthenaCommon.SystemOfUnits import GeV


#
## L2Calo - one and only FEX for everyone - or none
#
from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau, T2CaloTau_Tau_Med, T2CaloTau_cells
fexes.t2calo       = T2CaloTau_Tau()
fexes.t2calo_2012  = T2CaloTau_Tau_Med()
fexes.t2calo_cells = T2CaloTau_cells()

#
## L2 Trk - possibility : none, SiTrack, TRT
#
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Tau_robust
fexes.sitrk_robust = TrigSiTrack_Tau_robust()

from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Tau
fexes.trtfinder = TrigTRTSegFinder_Tau()

from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Tau
fexes.idscan = TrigIDSCAN_Tau()

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import L2HistoPrmVtx_TauIsoB
fexes.t2pvb = L2HistoPrmVtx_TauIsoB()

## L2 tests: STAR
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauA, TrigL2SiTrackFinder_TauB,TrigL2SiTrackFinder_TauC,TrigL2SiTrackFinder_TauF 
fexes.l2sitrkfinder_tauA = TrigL2SiTrackFinder_TauA()
fexes.l2sitrkfinder_tauB = TrigL2SiTrackFinder_TauB()
fexes.l2sitrkfinder_tauC = TrigL2SiTrackFinder_TauC()
fexes.l2sitrkfinder_tauF = TrigL2SiTrackFinder_TauF()

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauBC
    fexes.l2sitrkfinder_tauBC = TrigL2SiTrackFinder_TauBC()
except:
    fexes.l2sitrkfinder_tauBC = None 


## L2 Traking: 2 steps
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauCoreB
    fexes.sitrk_core = TrigL2SiTrackFinder_TauCoreB()
except:
    #print 'ccuenca: no TrigL2SiTrackFinder_TauCoreB(), using TrigL2SiTrackFinder_TauB()'
    fexes.sitrk_core = TrigL2SiTrackFinder_TauB()
    
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauIsoB
    fexes.sitrk_iso = TrigL2SiTrackFinder_TauIsoB()
except:
    #print 'ccuenca: no TrigL2SiTrackFinder_TauIsoB(), using TrigL2SiTrackFinder_TauB()'
    fexes.sitrk_iso = TrigL2SiTrackFinder_TauB()

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Tau
theTrigFastTrackFinder_Tau = TrigFastTrackFinder_Tau()
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
theTrigEFIDInsideOut_MergedDP_Tau = TrigEFIDSequence("Tau","tau","DataPrep").getSequence()
theTrigEFIDInsideOutMerged_Tau = TrigEFIDSequence("Tau","tau","InsideOutMerged").getSequence()



#
## T2IDTau : default(2011), new(2012) and 2stTest instances
#
from TrigT2IDTau.T2IDTauConfig import T2IDTau_Tau, T2IDTau_Tau_1GeV_dZ02_dR0103, T2IDTau_Tau_core, T2IDTau_Tau_iso
fexes.t2id      = T2IDTau_Tau()
fexes.t2id_2012 = T2IDTau_Tau_1GeV_dZ02_dR0103()
fexes.t2id_core = T2IDTau_Tau_core()
fexes.t2id_iso  = T2IDTau_Tau_iso()


#
## T2Tau : default(2011), new(2012)
#
from TrigT2Tau.T2TauFinalConfig import T2TauFinal_Tau, T2TauFinal_Tau_dR03_1GeV_dZ02
fexes.t2final      = T2TauFinal_Tau()
fexes.t2final_2012 = T2TauFinal_Tau_dR03_1GeV_dZ02()


#
## EF: cells, clusters and tracking
#
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau, TrigCaloCellMaker_tau_cells
fexes.cellmaker       = TrigCaloCellMaker_tau()
fexes.cellmaker_cells = TrigCaloCellMaker_tau_cells()

from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
fexes.clustermaker_topo = TrigCaloClusterMaker_topo()

from InDetTrigRecExample.EFInDetConfig import  TrigEFIDInsideOut_Tau, TrigEFIDOutsideInTRTOnly_Tau
fexes.efidinsideout         = TrigEFIDInsideOut_Tau().getSequence()
fexes.efidoutsideintrtonly  = TrigEFIDOutsideInTRTOnly_Tau().getSequence()


#
## EFTau: default (2011), new(2012)  
#
from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau, TrigTauRecMerged_Tau2012
fexes.recmerged       = TrigTauRecMerged_Tau()
fexes.recmerged_2012  = TrigTauRecMerged_Tau2012()

from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter
fexes.efmv            = TrigTauDiscriGetter()

from TrigTauRec.TrigTauRecCosmicsConfig import TrigTauRecCosmics_Tau, TrigTauRecCosmics_Tau2012
fexes.reccosmics      = TrigTauRecCosmics_Tau()
fexes.reccosmics_2012 = TrigTauRecCosmics_Tau2012()


#
# import all the hypo configurations - used in the sequence definitions
#
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() ):
    from TrigTauHypo.TrigTauHypoConfig import *
else:
    from TrigTauHypo.TrigTauHypoConfig2012 import *





###########################################################################
#Basic chain class
#############################################################################
class TauTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, prescale='1', pass_through='0', rerun='0'):
        """
        Base template for all tau chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)

        #print 'ef name', ef.name, sig
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every Tau chain should have
        self.l2.prescale = prescale
        self.l2.pass_through = pass_through
        self.l2.rerun_prescale = rerun
        self.ef.prescale = prescale
        self.ef.pass_through = pass_through
        self.ef.rerun_prescale = rerun

        self.addStreams('tauEtmiss')
        self.calib_stream = []
        if('EMPTY' in self.l2.lower_chain_name):
            self.addGroups('RATE:Cosmic_Tau','BW:Tau')
        else:
            self.addGroups('RATE:SingleTau','BW:Tau')

#######################################################################################
#The only difference between this and the TauTemplate class is that in this new one
#L2 passtrough is always equal to 1. There is really no need to have
#a new class for this, it would be enough to allow for L2 and EF pass trough flag to
#be independent from each other in the base class
#######################################################################################
class TauTemplate_L2NoCut(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, prescale='1', pass_through='0', rerun='0'):
        """
        Base template for all tau chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)

        #print 'ef name', ef.name, sig
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every Tau chain should have
        self.l2.prescale = prescale
        self.l2.pass_through = '1'
        self.l2.rerun_prescale = rerun
        self.ef.prescale = prescale
        self.ef.pass_through = pass_through
        self.ef.rerun_prescale = rerun

        self.addStreams('tauEtmiss')
        self.calib_stream = []
        if('EMPTY' in self.l2.lower_chain_name):
            self.addGroups('RATE:Cosmic_Tau','BW:Tau')
        else:
            self.addGroups('RATE:SingleTau','BW:Tau')


########################################################
def L1InputTE(name):
    tmpte = name.split('_')[1]
    tmpte = 'HA'+tmpte.strip('TAU')
    logger().debug('L1 input TE : %s'%tmpte)
    return tmpte

def sort_name(name, hypo='None'):
    br  = name.split('_')
    tmp = name.replace(str(br[0]+"_"),'').strip()

    if hypo.find('None') != -1:
        hypo = tmp

    return hypo, tmp

########################################################
# functions to obtain te and hypo from chain name
# affix can be calo, id... for L2 and clf0, tr... for EF
########################################################
def hypo_from_chain_name(name, affix='', hypo='None'):
    lvl = name.split('_')[0]
    suffix, cutval = sort_name(name, hypo)

    if lvl == 'L2':
        assert hasattr(hypos, 'l2%s_%s'%(affix,suffix)), 'unable to find L2 hypothesis algorithm (l2%s_%s)'%(affix,suffix)
        return getattr(hypos, 'l2%s_%s'%(affix,suffix))

    if lvl == 'EF':
        assert hasattr(hypos, 'ef%s_%s'%(affix,suffix)), 'unable to find EF hypothesis algorithm (ef%s_%s)'%(affix,suffix)
        return getattr(hypos, 'ef%s_%s'%(affix,suffix))

def te_from_chain_name(name, affix=''):
    lvl = name.split('_')[0]
    suffix, cutval = sort_name(name, 'None')

    if lvl == 'L2':
        if affix.find('calo') != -1:
            te = 'L2%scl'%cutval
        elif affix.find('idcore') != -1:
            te = 'L2%strcore'%cutval
        elif affix.find('idiso') != -1:
            te = 'L2%striso'%cutval
        elif affix.find('id') != -1:
            te = 'L2%str'%cutval
        else:
            te = 'L2_%s'%cutval

        logger().debug('L2 TE : %s'%te)

    if lvl == 'EF':
        if not affix:
            te = 'EF_%s'%cutval
        else:
            te = 'EF%s%s'%(cutval, affix)

        logger().debug('EF TE : %s'%te)

    return te


###########################################################################
# SiTrk class: Default for 2011
###########################################################################
class L2EFChain_Tau_sitrk(TauTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', efhypo='None', effex=fexes.recmerged, prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo, l2_hypo], l2te).addSignature('L2')

        ## T2ID
        l2te = te_from_chain_name(self.l2.chain_name, 'id')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'id', l2hypo)
        self.continueSequence([fexes.sitrk_robust, fexes.t2id, l2_hypo], l2te).addSignature('L2')

        ## T2 Final
        l2te = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        self.continueSequence([fexes.t2final, l2_hypo], l2te).addSignature('L2')

        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence([effex, ef_hypo], efte).addSignature('EF')

        pass


###########################################################################
# 2012 class
###########################################################################
class L2EFChain_Tau_2012(TauTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', efhypo='None', trkAlgo=fexes.l2sitrkfinder_tauB, effex=fexes.recmerged_2012, prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te    = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo_2012, l2_hypo], l2te).addSignature('L2')

        ## T2ID
        l2te    = te_from_chain_name(self.l2.chain_name, 'id')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'id', l2hypo)
        self.continueSequence([trkAlgo, fexes.t2id_2012, l2_hypo], l2te).addSignature('L2')
        
        ## T2 Final
        l2te    = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        self.continueSequence([fexes.t2final_2012, l2_hypo], l2te).addSignature('L2')
        
        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence([effex, fexes.efmv, ef_hypo], efte).addSignature('EF')

        pass
    

###########################################################################
# 2012 class with a bypass of L2
# same one as the 2012, only inherit from modified class
###########################################################################
class L2EFChain_Tau_L2NoCut(TauTemplate_L2NoCut):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', efhypo='None', trkAlgo=fexes.l2sitrkfinder_tauB, effex=fexes.recmerged_2012, prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate_L2NoCut.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te    = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo_2012, l2_hypo], l2te).addSignature('L2')

        ## T2ID
        l2te    = te_from_chain_name(self.l2.chain_name, 'id')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'id', l2hypo)
        self.continueSequence([trkAlgo, fexes.t2id_2012, l2_hypo], l2te).addSignature('L2')
        
        ## T2 Final
        l2te    = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        self.continueSequence([fexes.t2final_2012, l2_hypo], l2te).addSignature('L2')
        
        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence([effex, fexes.efmv, ef_hypo], efte).addSignature('EF')

        pass
    


###########################################################################
# 2 step tracking tests: based on 2012 class
##########################################################################
class L2EFChain_Tau_2stTest(TauTemplate):    
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', efhypo='None', prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo_2012, l2_hypo], l2te).addSignature('L2')

        ## T2IDCore
        l2te = te_from_chain_name(self.l2.chain_name, 'idcore')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'idcore', l2hypo)
        self.continueSequence([fexes.sitrk_core, fexes.t2id_core, l2_hypo], l2te).addSignature('L2')

        ## T2IDIso
        l2te = te_from_chain_name(self.l2.chain_name, 'idiso')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'idiso', l2hypo)
        self.continueSequence([fexes.t2pvb, fexes.sitrk_iso, fexes.t2id_iso, l2_hypo], l2te).addSignature('L2')
                
        ## T2 Final
        l2te = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        self.continueSequence([fexes.t2final_2012, l2_hypo], l2te).addSignature('L2')

        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence([fexes.recmerged_2012, fexes.efmv, ef_hypo], efte).addSignature('EF')
        pass





###########################################################################
# Define physics chains
###########################################################################
L2EFChain_Tau = L2EFChain_Tau_2012
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    L2EFChain_Tau = L2EFChain_Tau_sitrk
    
lowestL1seed = 'L1_TAU8'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    lowestL1seed = 'L1_TAU6'

effexcosmics = fexes.reccosmics_2012
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    effexcosmics = fexes.reccosmics



#
## tau no cut
#
Taus = [
    L2EFChain_Tau('tauNoCut',
                  l2=Chain(name='L2_tauNoCut', counter=93, seed=lowestL1seed), prescale='1', pass_through='1'),
    
    
    L2EFChain_Tau('tauNoCut_unpaired_iso',
                  l2=Chain(name='L2_tauNoCut_unpaired_iso', counter=368, seed='L1_TAU8_UNPAIRED_ISO'),
                  l2hypo='tauNoCut', prescale='1', pass_through='1'),
    
    L2EFChain_Tau('tauNoCut_unpaired_noniso',
                  l2=Chain(name='L2_tauNoCut_unpaired_noniso', counter=369, seed='L1_TAU8_UNPAIRED_NONISO'),
                  l2hypo='tauNoCut', prescale='1', pass_through='1'),
    
    L2EFChain_Tau('tauNoCut_firstempty',
                  l2=Chain(name='L2_tauNoCut_firstempty', counter=376, seed='L1_TAU8_FIRSTEMPTY'),
                  l2hypo='tauNoCut', prescale='1', pass_through='1'),

    L2EFChain_Tau('tauNoCut_cosmic',
                  l2=Chain(name='L2_tauNoCut_cosmic', counter=739, seed='L1_TAU8_EMPTY'),
                  l2hypo='tauNoCut', effex=effexcosmics, prescale='1', pass_through='1'),
    ]




#
#define exclusive chains for 2011, v2 and v3
#
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()) :
    Taus += [
        L2EFChain_Tau('tauNoCut_L1TAU50',
                      l2=Chain(name='L2_tauNoCut_L1TAU50', counter=333, seed='L1_TAU50'),
                      l2hypo='tauNoCut',prescale='1', pass_through='1'),
            
        L2EFChain_Tau('tau16_loose',
                      l2=Chain(name='L2_tau16_loose', counter=619, seed=lowestL1seed)),
    
        L2EFChain_Tau('tau16_medium',
                      l2=Chain(name='L2_tau16_medium', counter=720, seed=lowestL1seed), prescale='1'),

        L2EFChain_Tau('tau20i_medium',
                      l2=Chain(name='L2_tau20i_medium', counter=711, seed='L1_TAU8I'),
                      l2hypo='tau20_medium', rerun='1'),

        L2EFChain_Tau('tau20v_medium',
                      l2=Chain(name='L2_tau20v_medium', counter=230, seed='L1_TAU8V'),
                      l2hypo='tau20_medium', rerun='1'),

        L2EFChain_Tau('tau20i_medium1',
                      l2=Chain(name='L2_tau20i_medium1', counter=712, seed='L1_TAU8I'),
                      l2hypo='tau20_medium1', rerun='1'),

        L2EFChain_Tau('tau29_loose',
                      l2=Chain(name='L2_tau29_loose', counter=621, seed='L1_TAU11')),

        L2EFChain_Tau('tau29T_loose',
                      l2=Chain(name='L2_tau29T_loose', counter=4790, seed='L1_TAU15'),
                      l2hypo='tau29_loose'),

        L2EFChain_Tau('tau29_loose1',
                      l2=Chain(name='L2_tau29_loose1', counter=721, seed='L1_TAU11')),

        L2EFChain_Tau('tau38T_medium1',
                      l2=Chain(name='L2_tau38T_medium1', counter=4794, seed='L1_TAU15'),
                      l2hypo='tau38_medium1'),

        
        L2EFChain_Tau('tau50_medium',
                      l2=Chain(name='L2_tau50_medium', counter=366, seed='L1_TAU30'),
                      rerun='1'),

        L2EFChain_Tau('tau100_medium',
                      l2=Chain(name='L2_tau100_medium', counter=977, seed='L1_TAU50')),
        
        L2EFChain_Tau('tau125_medium',
                      l2=Chain(name='L2_tau125_medium', counter=94, seed='L1_TAU50')),

        L2EFChain_Tau('tau125_medium1',
                      l2=Chain(name='L2_tau125_medium1', counter=257, seed='L1_TAU50')),

        
        # only in MC_pp_v2
        L2EFChain_Tau('tau12_loose',
                      l2=Chain(name='L2_tau12_loose', counter=83, seed='L1_TAU5')), 

        L2EFChain_Tau('tau20_loose',
                      l2=Chain(name='L2_tau20_loose', counter=620, seed='L1_TAU8')), 
        
        L2EFChain_Tau('tau38_loose',
                      l2=Chain(name='L2_tau38_loose', counter=6988, seed='L1_TAU11')),

        L2EFChain_Tau('tau38_medium',
                      l2=Chain(name='L2_tau38_medium', counter=364, seed='L1_TAU11')),

        L2EFChain_Tau('tau38_medium1',
                      l2=Chain(name='L2_tau38_medium1', counter=270, seed='L1_TAU11')),

        L2EFChain_Tau('tau50_loose',
                      l2=Chain(name='L2_tau50_loose', counter=6990, seed='L1_TAU20')), 

        L2EFChain_Tau('tau84_loose',
                      l2=Chain(name='L2_tau84_loose', counter=91, seed='L1_TAU20')),

        L2EFChain_Tau('tau84_medium',
                      l2=Chain(name='L2_tau84_medium', counter=4000, seed='L1_TAU20')),
        
        L2EFChain_Tau('tau100_loose',
                       l2=Chain(name='L2_tau100_loose', counter=2360, seed='L1_TAU50')), 

        L2EFChain_Tau('tau125_loose',
                      l2=Chain(name='L2_tau125_loose', counter=360, seed='L1_TAU50')), 

        ]



#
#define exclusive chains for 2012, v4
#
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() ) :
    Taus += [
        L2EFChain_Tau('tauNoCut_L1TAU40',
                      l2=Chain(name='L2_tauNoCut_L1TAU40', counter=96, seed='L1_TAU40'),
                      l2hypo='tauNoCut',prescale='1', pass_through='1'),

        L2EFChain_Tau('tau29Ti_medium',
                      l2=Chain(name='L2_tau29Ti_medium', counter=680, seed='L1_TAU15I'),
                      l2hypo='tau29_medium',rerun='1'),
        
        L2EFChain_Tau('tau29Ti_medium1',
                      l2=Chain(name='L2_tau29Ti_medium1', counter=692, seed='L1_TAU15I'),
                      l2hypo='tau29_medium1',rerun='1'),
        
        L2EFChain_Tau('tau38T_medium',
                      l2=Chain(name='L2_tau38T_medium', counter=6477, seed='L1_TAU20'),
                      l2hypo='tau38_medium'),
        
        L2EFChain_Tau('tau38T_medium1',
                      l2=Chain(name='L2_tau38T_medium1', counter=474, seed='L1_TAU20'),
                      l2hypo='tau38_medium1'),

        #L2EFChain_Tau('tau38T_tight',
        #              l2=Chain(name='L2_tau38T_tight', counter=4795, seed='L1_TAU20'),
        #              l2hypo='tau38_tight'),

        L2EFChain_Tau('tau38T_medium1_llh',
                      l2=Chain(name='L2_tau38T_medium1_llh', counter=4734, seed='L1_TAU20'),
                      l2hypo='tau38_medium1', efhypo='tau38_medium1_llh'),

        
        L2EFChain_Tau('tau50_medium',
                      l2=Chain(name='L2_tau50_medium', counter=366, seed='L1_TAU20'),
                      rerun='1'),

        L2EFChain_Tau('tau50_medium1',
                      l2=Chain(name='L2_tau50_medium1', counter=367, seed='L1_TAU20'),
                      rerun='1'),

       L2EFChain_Tau('tau70_loose1',
                      l2=Chain(name='L2_tau70_loose1', counter=136, seed='L1_TAU20'),
                      rerun='1'),

        L2EFChain_Tau('tau100_loose1',
                      l2=Chain(name='L2_tau100_loose1', counter=63, seed='L1_TAU20'),
                      rerun='1'),

         L2EFChain_Tau('tau115_medium1',
                      l2=Chain(name='L2_tau115_medium1', counter=84, seed='L1_TAU40')),

        L2EFChain_Tau('tau115_medium1_llh',
                      l2=Chain(name='L2_tau115_medium1_llh', counter=85, seed='L1_TAU40'),
                      l2hypo='tau115_medium1', efhypo='tau115_medium1_llh'),
        
        
        L2EFChain_Tau('tau125_medium1',
                      l2=Chain(name='L2_tau125_medium1', counter=257, seed='L1_TAU40')),

        L2EFChain_Tau('tau125_medium1_llh',
                      l2=Chain(name='L2_tau125_medium1_llh', counter=191, seed='L1_TAU40'),
                      l2hypo='tau125_medium1', efhypo='tau125_medium1_llh'),


        #LLH and BDT chains 
        L2EFChain_Tau('tau20_medium_llh',
                      l2=Chain(name='L2_tau20_medium_llh', counter=2959, seed='L1_TAU8'),
                      l2hypo='tau20_medium', efhypo='tau20_medium_llh'),
        L2EFChain_Tau('tau20_medium1_llh',
                      l2=Chain(name='L2_tau20_medium1_llh', counter=194, seed='L1_TAU8'),
                      l2hypo='tau20_medium1', efhypo='tau20_medium1_llh'),
        L2EFChain_Tau('tau29_medium_llh',
                      l2=Chain(name='L2_tau29_medium_llh', counter=2963, seed='L1_TAU11'),
                      l2hypo='tau29_medium', efhypo='tau29_medium_llh'),
        L2EFChain_Tau('tau29_medium1_llh',
                      l2=Chain(name='L2_tau29_medium1_llh', counter=2965, seed='L1_TAU11'),
                      l2hypo='tau29_medium1', efhypo='tau29_medium1_llh'),
        
        
        ## L2EFChain_Tau('tau20_medium_bdt',
##                       l2=Chain(name='L2_tau20_medium_bdt', counter=2960, seed='L1_TAU8'),
##                       l2hypo='tau20_medium', efhypo='tau20_medium_bdt'),
##         L2EFChain_Tau('tau20_medium1_bdt',
##                       l2=Chain(name='L2_tau20_medium1_bdt', counter=2962, seed='L1_TAU8'),
##                       l2hypo='tau20_medium1', efhypo='tau20_medium1_bdt'),
##         L2EFChain_Tau('tau29_medium_bdt',
##                       l2=Chain(name='L2_tau29_medium_bdt', counter=2964, seed='L1_TAU11'),
##                       l2hypo='tau29_medium', efhypo='tau29_medium_bdt'),
##         L2EFChain_Tau('tau29_medium1_bdt',
##                       l2=Chain(name='L2_tau29_medium1_bdt', counter=2966, seed='L1_TAU11'),
##                       l2hypo='tau29_medium1', efhypo='tau29_medium1_bdt'),

        L2EFChain_Tau('tau29Ti_medium1_llh',
                      l2=Chain(name='L2_tau29Ti_medium1_llh', counter=2966, seed='L1_TAU15I'),
                      l2hypo='tau29_medium1', efhypo='tau29_medium1_llh'),
        
        L2EFChain_Tau('tau20Ti_medium1_llh',
                      l2=Chain(name='L2_tau20Ti_medium1_llh', counter=2862, seed='L1_TAU11I'),
                      l2hypo='tau20_medium1', efhypo='tau20_medium1_llh'),
        L2EFChain_Tau('tau29T_medium1_llh',
                      l2=Chain(name='L2_tau29T_medium1_llh', counter=2866, seed='L1_TAU15'),
                      l2hypo='tau29_medium1', efhypo='tau29_medium1_llh'),

        # tight1 versions added for ditau trigger.
        # At L2 these chains should use medium1 while at EF they should use tight1 hypos.
        L2EFChain_Tau('tau20Ti_tight1_llh',
                      l2=Chain(name='L2_tau20Ti_tight1_llh', counter=991, seed='L1_TAU11I'),
                      l2hypo='tau20_medium1', efhypo='tau20_tight1_llh'),

        L2EFChain_Tau('tau29Ti_tight1_llh',
                      l2=Chain(name='L2_tau29Ti_tight1_llh', counter=996, seed='L1_TAU15I'),
                      l2hypo='tau29_medium1', efhypo='tau29_tight1_llh'),
        
        L2EFChain_Tau('tau29T_tight1_llh',
                      l2=Chain(name='L2_tau29T_tight1_llh', counter=2992, seed='L1_TAU15'),
                      l2hypo='tau29_medium1', efhypo='tau29_tight1_llh'),

        L2EFChain_Tau('tau20Ti_tight1',
                      l2=Chain(name='L2_tau20Ti_tight1', counter=993, seed='L1_TAU11I'),
                      l2hypo='tau20_medium1', efhypo='tau20_tight1'),

        L2EFChain_Tau('tau29Ti_tight1',
                      l2=Chain(name='L2_tau29Ti_tight1', counter=997, seed='L1_TAU15I'),
                      l2hypo='tau29_medium1', efhypo='tau29_tight1'),
        L2EFChain_Tau('tau29T_tight1',
                      l2=Chain(name='L2_tau29T_tight1', counter=570, seed='L1_TAU15'),
                      l2hypo='tau29_medium1', efhypo='tau29_tight1'),


        # 2-step tracking test
        L2EFChain_Tau_2stTest('tau29_medium_2stTest',
                              l2=Chain(name='L2_tau29_medium_2stTest', counter=971, seed='L1_TAU11')),   

        # L2Star tests
        L2EFChain_Tau('tau29_medium_L2StarA',
                      l2=Chain(name='L2_tau29_medium_L2StarA', counter=955, seed='L1_TAU11'),
                      l2hypo='tau29_medium', trkAlgo=fexes.l2sitrkfinder_tauA),
        
        L2EFChain_Tau('tau29_medium_L2StarB',
                      l2=Chain(name='L2_tau29_medium_L2StarB', counter=956, seed='L1_TAU11'),
                      l2hypo='tau29_medium', trkAlgo=fexes.l2sitrkfinder_tauB),
        
        L2EFChain_Tau('tau29_medium_L2StarC',
                      l2=Chain(name='L2_tau29_medium_L2StarC', counter=957, seed='L1_TAU11'),
                      l2hypo='tau29_medium', trkAlgo=fexes.l2sitrkfinder_tauC),
        
        L2EFChain_Tau('tau125_medium1_L2StarA',
                      l2=Chain(name='L2_tau125_medium1_L2StarA', counter=958, seed='L1_TAU40'),
                      l2hypo='tau125_medium1', trkAlgo=fexes.l2sitrkfinder_tauA),
        
        L2EFChain_Tau('tau125_medium1_L2StarB',
                      l2=Chain(name='L2_tau125_medium1_L2StarB', counter=959, seed='L1_TAU40'),
                      l2hypo='tau125_medium1', trkAlgo=fexes.l2sitrkfinder_tauB),
        
        L2EFChain_Tau('tau125_medium1_L2StarC',
                      l2=Chain(name='L2_tau125_medium1_L2StarC', counter=962, seed='L1_TAU40'),
                      l2hypo='tau125_medium1', trkAlgo=fexes.l2sitrkfinder_tauC),
        
        L2EFChain_Tau('tauNoCut_FTK',
                      l2=Chain(name='L2_tauNoCut_FTK', counter=7000, seed=lowestL1seed), 
                      l2hypo='tauNoCut',trkAlgo=fexes.l2sitrkfinder_tauF),

        
        L2EFChain_Tau('tau20_medium_FTK',
                      l2=Chain(name='L2_tau20_medium_FTK', counter=7001, seed='L1_TAU8'),
                      l2hypo='tau20_medium',trkAlgo=fexes.l2sitrkfinder_tauF),

        
        L2EFChain_Tau('tau29Ti_medium1_L2loose',
                      l2=Chain(name='L2_tau29Ti_medium1_L2loose', counter=6920, seed='L1_TAU15I'),
                      l2hypo='tau29_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau27Ti_medium1_L2loose',
                      l2=Chain(name='L2_tau27Ti_medium1_L2loose', counter=370, seed='L1_TAU15I'),
                      l2hypo='tau27_medium1_L2loose',rerun='1'),

        L2EFChain_Tau('tau29T_medium1_L2loose',
                      l2=Chain(name='L2_tau29T_medium1_L2loose', counter=6972, seed='L1_TAU15'),
                      l2hypo='tau29_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau27T_medium1_L2loose',
                      l2=Chain(name='L2_tau27T_medium1_L2loose', counter=371, seed='L1_TAU15'),
                      l2hypo='tau27_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau27T_loose3',
                      l2=Chain(name='L2_tau27T_loose3', counter=7713, seed='L1_TAU15'),
                      l2hypo='tau27_loose3',rerun='1'),

        L2EFChain_Tau('tau27T_loose2',
                      l2=Chain(name='L2_tau27T_loose2', counter=7716, seed='L1_TAU15'),
                      l2hypo='tau27_loose2',rerun='1'),

        L2EFChain_Tau('tau27Ti_loose3',
                      l2=Chain(name='L2_tau27Ti_loose3', counter=7714, seed='L1_TAU15I'),
                      l2hypo='tau27_loose3',rerun='1'),

        L2EFChain_Tau('tau27Ti_loose2',
                      l2=Chain(name='L2_tau27Ti_loose2', counter=7715, seed='L1_TAU15I'),
                      l2hypo='tau27_loose2',rerun='1'),

        L2EFChain_Tau('tau20Ti_medium1_L2loose',
                      l2=Chain(name='L2_tau20Ti_medium1_L2loose', counter=6911, seed='L1_TAU11I'),
                      l2hypo='tau20_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau18Ti_medium1_L2loose',
                      l2=Chain(name='L2_tau18Ti_medium1_L2loose', counter=372, seed='L1_TAU11I'),
                      l2hypo='tau18_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau20_medium1_L2loose',
                      l2=Chain(name='L2_tau20_medium1_L2loose', counter=393, seed='L1_TAU8'),
                      l2hypo='tau20_medium1_L2loose',rerun='1'),
        
        L2EFChain_Tau('tau18_medium1_L2loose',
                      l2=Chain(name='L2_tau18T_medium1_L2loose', counter=7914, seed='L1_TAU8'),
                      l2hypo='tau18_medium1_L2loose',rerun='1'),

        L2EFChain_Tau('tau18Ti_loose3',
                      l2=Chain(name='L2_tau18Ti_loose3', counter=7721, seed='L1_TAU11I'),
                      l2hypo='tau18_loose3',rerun='1'),

        L2EFChain_Tau('tau18Ti_loose2',
                      l2=Chain(name='L2_tau18Ti_loose2', counter=7722, seed='L1_TAU11I'),
                      l2hypo='tau18_loose2',rerun='1'),
        
        L2EFChain_Tau('tau20_loose3',
                      l2=Chain(name='L2_tau20_loose3', counter=3732, seed='L1_TAU8'),
                      l2hypo='tau20_loose3',rerun='1'),

        L2EFChain_Tau('tau20_loose2',
                      l2=Chain(name='L2_tau20_loose2', counter=3832, seed='L1_TAU8'),
                      l2hypo='tau20_loose2',rerun='1'),

        ### L2 bypass test chains
        
        L2EFChain_Tau_L2NoCut('tau20T_loose1_L2NoCut',
                              l2=Chain(name='L2_tau20T_loose1_L2NoCut', counter=5530, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_loose1', rerun='1'),        

        L2EFChain_Tau_L2NoCut('tau20T_loose2_L2NoCut',
                              l2=Chain(name='L2_tau20T_loose2_L2NoCut', counter=5531, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_loose2', rerun='1'),        

        L2EFChain_Tau_L2NoCut('tau20T_medium1_L2NoCut',
                              l2=Chain(name='L2_tau20T_medium1_L2NoCut', counter=5532, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_medium1', rerun='1'),        

        L2EFChain_Tau_L2NoCut('tau20Ti_loose1_L2NoCut',
                              l2=Chain(name='L2_tau20Ti_loose1_L2NoCut', counter=5533, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_loose1', rerun='1'),
        
        L2EFChain_Tau_L2NoCut('tau20Ti_loose2_L2NoCut',
                              l2=Chain(name='L2_tau20Ti_loose2_L2NoCut', counter=5534, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_loose2', rerun='1'),
        
        L2EFChain_Tau_L2NoCut('tau20Ti_medium1_L2NoCut',
                              l2=Chain(name='L2_tau20Ti_medium1_L2NoCut', counter=5535, seed='L1_TAU11I'),
                              l2hypo='tauNoCut',efhypo='tau20_medium1', rerun='1'),
        
        L2EFChain_Tau_L2NoCut('tau29Ti_loose1_L2NoCut',
                              l2=Chain(name='L2_tau29Ti_loose1_L2NoCut', counter=5536, seed='L1_TAU15I'),
                              l2hypo='tauNoCut',efhypo='tau29_loose1', rerun='1'),
        
        L2EFChain_Tau_L2NoCut('tau29Ti_loose2_L2NoCut',
                              l2=Chain(name='L2_tau29Ti_loose2_L2NoCut', counter=5537,  seed='L1_TAU15I'),
                              l2hypo='tauNoCut',efhypo='tau29_loose2', rerun='1'),
        
        L2EFChain_Tau_L2NoCut('tau29Ti_medium1_L2NoCut',
                              l2=Chain(name='L2_tau29Ti_medium1_L2NoCut', counter=5538, seed='L1_TAU15I'),
                              l2hypo='tauNoCut',efhypo='tau29_medium1', rerun='1'),
        

        ]


#
#define chains common for 2011 and 2012:
#
Taus += [

    L2EFChain_Tau('tau20_medium',
                  l2=Chain(name='L2_tau20_medium', counter=86, seed='L1_TAU8'),rerun='1'),


    
    L2EFChain_Tau('tau20_medium1',
                  l2=Chain(name='L2_tau20_medium1', counter=976, seed='L1_TAU8'),rerun='1'),
    

    L2EFChain_Tau('tau20T_medium',
                  l2=Chain(name='L2_tau20T_medium', counter=87, seed='L1_TAU11'),
                  l2hypo='tau20_medium', rerun='1'),
    
    L2EFChain_Tau('tau20T_medium1',
                  l2=Chain(name='L2_tau20T_medium1', counter=192, seed='L1_TAU11'),
                  l2hypo='tau20_medium1', rerun='1'),

    
    L2EFChain_Tau('tau20Ti_medium',
                  l2=Chain(name='L2_tau20Ti_medium', counter=690, seed='L1_TAU11I'),
                  l2hypo='tau20_medium',rerun='1'),
    
    L2EFChain_Tau('tau20Ti_medium1',
                  l2=Chain(name='L2_tau20Ti_medium1', counter=691, seed='L1_TAU11I'),
                  l2hypo='tau20_medium1',rerun='1'),


    L2EFChain_Tau('tau29_medium',
                  l2=Chain(name='L2_tau29_medium', counter=92, seed='L1_TAU11'),
                  rerun='1'),
    
    L2EFChain_Tau('tau29i_medium',
                  l2=Chain(name='L2_tau29i_medium', counter=714, seed='L1_TAU11I'),
                  l2hypo='tau29_medium',rerun='1'),

    L2EFChain_Tau('tau29T_medium',
                  l2=Chain(name='L2_tau29T_medium', counter=300, seed='L1_TAU15'),
                  l2hypo='tau29_medium', rerun='1'),
    
    L2EFChain_Tau('tau29_medium1',
                  l2=Chain(name='L2_tau29_medium1', counter=974, seed='L1_TAU11'),
                  rerun='1'),
    
    L2EFChain_Tau('tau29i_medium1',
                  l2=Chain(name='L2_tau29i_medium1', counter=713, seed='L1_TAU11I'),
                  l2hypo='tau29_medium1', rerun='1'),
    
    L2EFChain_Tau('tau29T_medium1',
                  l2=Chain(name='L2_tau29T_medium1', counter=718, seed='L1_TAU15'),
                  l2hypo='tau29_medium1', rerun='1'),


    ]



###########################################################################
# Calo only class - need to redefine the sequences here
###########################################################################
class L2EFChain_Tau_NoIDTrkCut(TauTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', l2trk=[fexes.idscan, fexes.sitrk_robust, fexes.trtfinder],efhypo='None', prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo_cells, l2_hypo], l2te).addSignature('L2')

        ## T2 Final
        l2te = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        self.continueSequence(l2trk, l2te).addSignature('L2')

        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker_cells, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, '')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        pass


class L2EFChain_Tau_NoIDTrkCut_IDT(TauTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', l2trk=[fexes.idscan, fexes.sitrk_robust, fexes.trtfinder],efhypo='None', prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        print 'jmasik IDT'

        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'calo', l2hypo)
        self.addSequence(inputTE, [fexes.t2calo_cells, l2_hypo], l2te).addSignature('L2')

        trkseq = list(theTrigEFIDInsideOut_MergedDP_Tau)
        trkseq.append(theTrigFastTrackFinder_Tau)
        trkseq += theTrigEFIDInsideOutMerged_Tau


        ## T2 Final
        # l2te = te_from_chain_name(self.l2.chain_name, '')
        # l2_hypo = hypo_from_chain_name(self.l2.chain_name, '', l2hypo)
        # self.continueSequence(trkseq, l2te).addSignature('L2')
        # #self.continueSequence(l2trk, l2te).addSignature('L2')

        # EF sequence (do not have to specify explicitly these are ef algs)
        if efhypo.find('None') != -1:
            efhypo = l2hypo

        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF step 1
        efte = te_from_chain_name(self.ef.chain_name, 'clf0')
        self.continueSequence([fexes.cellmaker_cells, fexes.clustermaker_topo], efte).addSignature('EF')

        ## EF step 2
        efte = te_from_chain_name(self.ef.chain_name, '')
        self.continueSequence(trkseq, efte).addSignature('EF')

        pass

if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()) :
    myL2Trk = [fexes.idscan, fexes.sitrk_robust, fexes.trtfinder]

    Taus += [
        L2EFChain_Tau_NoIDTrkCut('tau16_IDTrkNoCut',
                                 l2=Chain(name='L2_tau16_IDTrkNoCut', counter=470, seed=lowestL1seed),
                                 l2hypo='tau16_calo',l2trk=myL2Trk),
        
        L2EFChain_Tau_NoIDTrkCut('tau29_IDTrkNoCut',
                                 l2=Chain(name='L2_tau29_IDTrkNoCut', counter=470, seed='L1_TAU11'),
                                 l2hypo='tau29_calo',l2trk=myL2Trk),

        L2EFChain_Tau_NoIDTrkCut('tau50_IDTrkNoCut',
                                 l2=Chain(name='L2_tau50_IDTrkNoCut', counter=784, seed='L1_TAU30'),##CHECK COUNTER
                                 l2hypo='tau50_calo',l2trk=myL2Trk),

        L2EFChain_Tau_NoIDTrkCut('tau125_IDTrkNoCut',
                                 l2=Chain(name='L2_tau125_IDTrkNoCut', counter=784, seed='L1_TAU50'),##CHECK COUNTER
                                 l2hypo='tau125_calo',l2trk=myL2Trk),
        ]


if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() ) :
    myL2Trk = [fexes.l2sitrkfinder_tauA,fexes.l2sitrkfinder_tauB,fexes.l2sitrkfinder_tauC,
               fexes.l2sitrkfinder_tauBC]
    myL2Trk_old = [fexes.idscan, fexes.sitrk_robust, fexes.trtfinder]

    Taus += [
        L2EFChain_Tau_NoIDTrkCut('tau29_IDTrkNoCut',
                                 l2=Chain(name='L2_tau29_IDTrkNoCut', counter=470, seed='L1_TAU11'),
                                 l2hypo='tau29_calo',l2trk=myL2Trk),


        L2EFChain_Tau_NoIDTrkCut('tau29_IDTrkNoCut_R1',
                                 l2=Chain(name='L2_tau29_IDTrkNoCut_R1', counter=3281, seed='L1_TAU11'),
                                 l2hypo='tau29_calo',l2trk=myL2Trk_old),
       

#        L2EFChain_Tau_NoIDTrkCut('tau50_IDTrkNoCut',
#                                 l2=Chain(name='L2_tau50_IDTrkNoCut', counter=784, seed='L1_TAU20'),##CHECK COUNTER
#                                 l2hypo='tau50_calo',l2trk=myL2Trk),
        
        L2EFChain_Tau_NoIDTrkCut('tau125_IDTrkNoCut',
                                 l2=Chain(name='L2_tau125_IDTrkNoCut', counter=784, seed='L1_TAU40'),##CHECK COUNTER
                                 l2hypo='tau125_calo',l2trk=myL2Trk),

        L2EFChain_Tau_NoIDTrkCut_IDT('tau125_IDTrkNoCut_IDT',
                                     l2=Chain(name='L2_tau125_IDTrkNoCut_IDT', counter=3282, seed='L1_TAU40'),l2hypo='tau125_calo'),


        L2EFChain_Tau_NoIDTrkCut_IDT('tau29_IDTrkNoCut_IDT',
                                     l2=Chain(name='L2_tau29_IDTrkNoCut_IDT', counter=3283, seed='L1_TAU11'),l2hypo='tau29_calo')


        ]




################################################################################
# no Algorithms, for hidden valley
################################################################################
class L2EFChain_Tau_noAlg(TauTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo ='None', efhypo='None', prescale='1', pass_through='0', rerun='0'):
        ## generate EF chain name to be identical as the signature name (but only if not given)
        TauTemplate.__init__(self, sig, l2, ef, prescale, pass_through, rerun)

        pass
    
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()) :
    Taus += [
        L2EFChain_Tau_noAlg('L1TAU30_NoAlg',
                            l2=Chain(name='L2_L1TAU30_NoAlg', counter=417 , seed='L1_TAU30'),
                            l2hypo='trk9_id'),
        ]
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() ) :
    Taus += [
        L2EFChain_Tau_noAlg('L1TAU40_NoAlg',
                            l2=Chain(name='L2_L1TAU40_NoAlg', counter=417 , seed='L1_TAU40'),
                            l2hypo='trk9_id'),
        ]
