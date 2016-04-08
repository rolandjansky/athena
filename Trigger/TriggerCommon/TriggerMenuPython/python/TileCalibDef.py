# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for TileCalib chains
###########################################################################
__author__  = 'O.Igonkina'
__version__=""
__doc__="Implementation of hadronic calibration trigger sequence "

from ChainTemplate import *

fexes = FreeStore()

from AthenaCommon.Logging import logging

def logger():
    return logging.getLogger("TileCalibDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from TrigGenericAlgs.TrigGenericAlgsConf import ReverseRoI, PESA__DummyUnseededAllTEAlgo
fexes.ReverseEtaPhiRoI = ReverseRoI("ReverseEtaPhiRoI")
fexes.ReversePhiRoI = ReverseRoI("ReversePhiRoI")
fexes.ReversePhiRoI.FlipEta=False
fexes.RoICreator = PESA__DummyUnseededAllTEAlgo('RoiCreator')


## L2 Trk - possibility : none, IDSCAN, SiTrack, TRT
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Tau
fexes.idscan = TrigIDSCAN_Tau()
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Tau_robust
fexes.sitrk_robust = TrigSiTrack_Tau_robust()
from TrigT2IDTau.T2IDTauConfig import T2IDTau_Tau
fexes.t2id = T2IDTau_Tau()

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import  TrigL2SiTrackFinder_TauB
fexes.l2sitrkfinder_tauB = TrigL2SiTrackFinder_TauB()


## EF algorithms
from InDetTrigRecExample.EFInDetConfig import  TrigEFIDSequence
fexes.efidinsideout =  TrigEFIDSequence("hadCalib","hadCalib","InsideOut").getSequence()
#fexes.fullscan      =  TrigEFIDSequence("hadCalibFS","fullScan","InsideOut").getSequence()
fexes.fullscan      =  TrigEFIDSequence("hadCalibFS","fullScan500","InsideOut").getSequence()


if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() ):
    from TrigTauHypo.TrigTauHypoConfig import *
else:
    from TrigTauHypo.TrigTauHypoConfig2012 import *


from TauDef import L1InputTE, sort_name, hypo_from_chain_name, te_from_chain_name

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    calibThr = '6'
else:
    calibThr = '8'
    
###########################################################################
#Basic chain class
###########################################################################
class TileCalibTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig):
        """
        Base template for all TileCalib chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)

        ChainTemplate.__init__(self, sig, l2, ef)

        self.addStreams('minbias')
        self.calib_stream = []
        self.addGroups('RATE:HadCalibration_Trk','BW:Jets')



###########################################################################
class L2EFChain_TileCalib(TileCalibTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo='None', RoiShiftAlgo=fexes.ReverseEtaPhiRoI, efhypo='None'):
        TileCalibTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        self.addSequence(inputTE, [RoiShiftAlgo], l2te).addSignature('L2')

        ## T2ID/Final
        l2te = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'id', l2hypo)
        #self.continueSequence([fexes.idscan, fexes.t2id, l2_hypo], l2te).addSignature('L2')
        self.continueSequence([fexes.l2sitrkfinder_tauB, fexes.t2id, l2_hypo], l2te).addSignature('L2')
        
        #  !!!! New EF Configuration
        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF ID
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence( ef_hypo, efte).addSignature('EF')

        pass


###########################################################################
class L2EFChain_TileCalib_2011(TileCalibTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, l2hypo='None', RoiShiftAlgo=fexes.ReverseEtaPhiRoI ):

        TileCalibTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## L2 sequence
        logger().debug('L2 hypo signature: %s'%(sort_name(self.l2.chain_name,l2hypo)[0]))

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, 'calo')
        self.addSequence(inputTE, [RoiShiftAlgo], l2te).addSignature('L2')

        ## T2ID/Final
        l2te = te_from_chain_name(self.l2.chain_name, '')
        l2_hypo = hypo_from_chain_name(self.l2.chain_name, 'id', l2hypo)
        self.continueSequence([fexes.idscan, fexes.t2id, l2_hypo], l2te).addSignature('L2')

        pass

###########################################################################
class L2EFChain_TileCalib_L2NoAlg(TileCalibTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, RoiShiftAlgo=fexes.ReverseEtaPhiRoI, efhypo='None'):
        ef.replace_if_default('name', 'EF_%s'%sig)
        TileCalibTemplate.__init__(self, sig, l2, ef=DeduceChainConfig)

        ## L1 Input TE
        inputTE = L1InputTE(self.l2.lower_chain_name)

        ## T2Calo
        l2te = te_from_chain_name(self.l2.chain_name, '')
        self.addSequence(inputTE,[RoiShiftAlgo], l2te).addSignature('L2')

        #  New EF Configuration
        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## EF ID
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence(fexes.efidinsideout, efte).addSignature('EF')

        ## EF Final step
        efte = te_from_chain_name(self.ef.chain_name, '')
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence( ef_hypo, efte).addSignature('EF')

        pass

###########################################################################
class L2EFChain_TileCalib_FullScan(TileCalibTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, efhypo='None'):
        ef.replace_if_default('name', 'EF_%s'%sig)
        TileCalibTemplate.__init__(self, sig, l2, ef)

        #  New EF Configuration
        logger().debug('EF hypo signature: %s'%(sort_name(self.ef.chain_name,efhypo)[0]))

        ## Dummy RoI Creator
        efte = te_from_chain_name(self.ef.chain_name, 'roi')
        self.addSequence('', fexes.RoICreator, efte).addSignature('EF')

        ## EF ID
        efte = te_from_chain_name(self.ef.chain_name, 'tr')
        self.continueSequence( fexes.fullscan, efte).addSignature('EF')

        ## EF Final step
        ef_hypo = hypo_from_chain_name(self.ef.chain_name, '', efhypo)
        self.continueSequence( ef_hypo, self.ef.chain_name).addSignature('EF')

        pass

###########################################################################


###########################################################################
#Chain definitions here:
###########################################################################
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() ):

    TileCalib = [
        L2EFChain_TileCalib_2011('hadCalib_trk9',
                                 l2=Chain(name='L2_hadCalib_trk9', counter=425, seed='L1_TAU%s' % calibThr),
                                 l2hypo='trk9_id'),
        L2EFChain_TileCalib_2011('hadCalib_trk9phi',
                                 l2=Chain(name='L2_hadCalib_trk9phi',  counter=432, seed='L1_TAU%s' % calibThr),
                                 l2hypo='trk9_id', RoiShiftAlgo=fexes.ReversePhiRoI),
        
        ]

else:

    TileCalib = [

        L2EFChain_TileCalib('hadCalib_trk9',
                            l2=Chain(name='L2_hadCalib_trk9', counter=425, seed='L1_TAU%s' % calibThr),
                            l2hypo='trk9_id', efhypo='hadCalibHypo_trk9'),
        L2EFChain_TileCalib('hadCalib_trk18',
                            l2=Chain(name='L2_hadCalib_trk18', counter=424, seed='L1_TAU%s' % calibThr),
                            l2hypo='trk18_id', efhypo='hadCalibHypo_trk18'),
        L2EFChain_TileCalib('hadCalib_trk9phi',
                            l2=Chain(name='L2_hadCalib_trk9phi',  counter=432, seed='L1_TAU%s' % calibThr),
                            l2hypo='trk9_id', RoiShiftAlgo=fexes.ReversePhiRoI,  efhypo='hadCalibHypo_trk9'),


        # L2 no Alg, EF standard 
        L2EFChain_TileCalib_L2NoAlg('hadCalib_trk9_L1HA%s'%calibThr,
                            l2=Chain(name='L2_hadCalib_L1HA%s'%calibThr, counter=144, seed='L1_TAU%s' % calibThr),
                            efhypo='hadCalibHypo_trk9_L1HA%s'%calibThr),
        L2EFChain_TileCalib_L2NoAlg('hadCalib_trk18_L1HA%s'%calibThr,
                            l2=Chain(name='L2_hadCalib_L1HA%s'%calibThr, counter=145, seed='L1_TAU%s' % calibThr),
                            efhypo='hadCalibHypo_trk18_L1HA%s'%calibThr),
        #Full scan chains

        L2EFChain_TileCalib_FullScan('hadCalib_trk9_L1RD0', l2=None,
			    ef=Chain(name='EF_hadCalib_trk9_L1RD0', counter=146, seed='L2_rd0_filled_NoAlg'),
                            efhypo='hadCalibHypo_trk9_L1RD0'),
        L2EFChain_TileCalib_FullScan('hadCalib_trk18_L1RD0', l2=None,
			    ef=Chain(name='EF_hadCalib_trk18_L1RD0', counter=147, seed='L2_rd0_filled_NoAlg'),
                            efhypo='hadCalibHypo_trk18_L1RD0'),


        ]

    



