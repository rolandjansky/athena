# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for MinBias chains/signatures

__author__  = 'R. Kwee, B. Demirkoz, T.Bold, T. Kohno, W. H. Bell, P. Bell'
__version__="$Revision: 2.0 $"
__doc__="Minimum Bias HLT menu generation"

#
# This file should not be modified without consulting
# the Minimum Bias trigger slice coordinator.
#
# The Minimum Bias trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/MinbiasTrigger
#
# Cleaned up by P Bell as part of https://savannah.cern.ch/bugs/?88289 Nov 2011
#
###########################################################################

from string import atoi
from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from ChainTemplate import *

from TriggerMenuPython.Lvl1Flags import Lvl1Flags

#FEXES
fexes = FreeStore()
from TrigT2MinBias.TrigT2MinBiasConfig import fexes as l2fexes
fexes.update(l2fexes)
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()
from TrigMinBias.TrigMinBiasConfig import fexes as effexes
fexes.update(effexes)

print fexes.__dict__

#HYPOS
hypos = FreeStore()
from TrigT2MinBias.TrigT2MinBiasConfig import hypos as l2hypos
hypos.update(l2hypos)
from TrigMinBias.TrigMinBiasConfig import hypos as efhypos
hypos.update(efhypos)

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
fexes.dummyRoI=DummyRoI(name='MinBiasDummyRoI', createRoIDescriptors = True, NumberOfOutputTEs=1)
fexes.terminateAlgo = PrescaleAlgo('terminateAlgo')

#L2 pileup suppression
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScan_ZF_OnlyA  #TrigL2SiTrackFinder_FullScanA_ZF_OnlyA
fexes.theTrigL2SiTrackFinder_FullScan_ZF_OnlyA = TrigL2SiTrackFinder_FullScan_ZF_OnlyA()

# for HI
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
hypos.atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

from TrigHIHypo.TrigHIHypoConfig import hypos as hi_hypos
hypos.update(hi_hypos)
# hypos.oneL2VertexHypo = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex')
# hypos.oneL2VertexHypo600 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_600')
# hypos.oneL2VertexHypo700 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_700')
# hypos.oneL2VertexHypo800 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_800')
# hypos.oneL2VertexHypo900 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_900')
# hypos.oneL2VertexHypo1100 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_1100')
# hypos.oneL2VertexHypo1200 = HIL2VtxMultHypo(name='HIL2VtxMultHypo_OneL2Vertex_1200')

######################################################################
#
#Basic template:
#
class MinBiasTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig):
        ChainTemplate.__init__(self, sig, l2, ef)
        
        self.addStreams('minbias') # send event of this chains to minbias stream
        if 'ALFA' in self.sig_id:
            self.addGroups('RATE:ALFAPhysics','BW:Detector')
        elif('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_Minbias','BW:MinBias')
        elif('UNPAIRED' in l2.seed):
            self.addGroups('RATE:Unpaired_Minbias','BW:MinBias')
        elif('zerobias_Overlay' in sig):
            self.addGroups('RATE:Minbias','BW:Detector')
        elif('mbSpTrkVtxMh' in sig):
            self.addGroups('RATE:HMT','BW:MinBias')
        else:
            self.addGroups('RATE:Minbias','BW:MinBias')
            
        #if (self.sig_id=='mbSpBg_unpaired_iso' or self.sig_id=='mbSpTrk_unpaired_iso'
        if (self.sig_id=='mbSpBg_unpaired_iso' 
            or self.sig_id=='mbSpBg_unpaired_iso_HI' or self.sig_id=='mbSp_unpaired_iso_HI'):
            self.addStreams('background')

mb_algs = FixedClassFactory("mb_algs", {'l2spfex': fexes.L2MbSpFex, 'l2trtfex': fexes.L2MbTrtFex, 'l2mbtsfex': fexes.L2MbMbtsFex,
                                        'efid': fexes.efid, 'effex': fexes.EFMbTrkFex, 'efvfex': None})

######################################################################
#
# _MB template
#

from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

ef_VdMSubDetListWriter = TrigSubDetListWriter("VdMSubDetListWriter")
ef_VdMSubDetListWriter.SubdetId = ['TDAQ_LVL2','TDAQ_CTP','FORWARD_LUCID','FORWARD_ZDC','FORWARD_BCM','SiOnly']
ef_VdMSubDetListWriter.MaxRoIsPerEvent=1

class L2EFChain_MB(MinBiasTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, # by default there is no customization at EF
                 l2hypo=hypos.L2MbSpHypo,             # if nothing is given this L2 hypo is used
                 l2hypo2=None,
                 efhypo=hypos.EFMbTrkHypo,            # if nothing is given this EF hypo is used
                 algs=mb_algs()):                     # if nothing is given default set of algs is used
        MinBiasTemplate.__init__(self, sig, l2, ef)


        self.robWriter = []

        # define first sequence but do not add output TE to chain steps            
        self.addSequence("", [fexes.dummyRoI], "L2_mbDummyRoI")


        if ('_pileupSup') in self.sig_id:
            # define second sequence (continue it) and add it to the chain (as chain signature)
            self.continueSequence([algs.l2spfex, l2hypo], self.l2.chain_name+'_sp' ).addSignature() 

            self.continueSequence([fexes.theTrigL2SiTrackFinder_FullScan_ZF_OnlyA, l2hypo2], self.l2.chain_name ).addSignature()
        else:
            # define second sequence (continue it) and add it to the chain (as chain signature)
            self.continueSequence([algs.l2spfex, l2hypo], self.l2.chain_name ).addSignature() 
            
        # define EF sequence and add it's output to EF chain signature list
        if efhypo:
            if ('VdM_') in self.sig_id:
                efte1 =  'EF_%s_beforePEB' % self.sig_id
                self.continueSequence( algs.efid +
                                       [algs.effex,algs.efvfex,efhypo], efte1).addSignature()
                self.robWriter = [ef_VdMSubDetListWriter]
                self.addSequence(efte1, self.robWriter, self.ef.chain_name)
#                print "robWriter, ", self.robWriter
#                print "efte1 ", efte1

            else:
                self.continueSequence( algs.efid +
                                       [algs.effex,algs.efvfex,efhypo], self.ef.chain_name).addSignature()

            

        if 'VdM_' in self.sig_id:
            self.calib_streams = ['VdM']
            self.groups = [ 'RATE:VdMCalibration','BW:Detector']


            
MinBias = [
    L2EFChain_MB('mbSpTrk', l2=Chain(counter=318, seed='L1_RD0_FILLED')),

    L2EFChain_MB('VdM_mbSpTrk', l2=Chain(counter=712, seed='L1_BGRP7')),
    
    L2EFChain_MB('mbSpTrk_empty', l2=Chain(counter=319, seed='L1_RD0_EMPTY')),
    L2EFChain_MB('mbSpTrk_noiseSup', l2=Chain(counter=631, seed='L1_RD0_FILLED'),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_SCTNoiseSup), efhypo=hypos.EFMbTrkHypo_PT), 

    L2EFChain_MB('mbSpTrk_unpaired_iso', l2=Chain( counter=879, seed='L1_RD0_UNPAIRED_ISO')),
    L2EFChain_MB('mbSpTrk_noiseSup_unpaired_iso', l2=Chain( counter=632, seed='L1_RD0_UNPAIRED_ISO'),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_SCTNoiseSup), efhypo=hypos.EFMbTrkHypo_PT),

    L2EFChain_MB('mbSpTrk_cosmic', l2=Chain(counter=744, seed='L1_RD0_EMPTY')),

    L2EFChain_MB('mbSpTrk_unpaired', l2=Chain( counter=879, seed='L1_RD0_UNPAIRED')),


    L2EFChain_MB('mbSpTrk_BX0', l2=Chain(counter=896, seed='L1_BPTX0_BGRP0')),

    L2EFChain_MB('mbSpTrk_BX1', l2=Chain(counter=897, seed='L1_BPTX1_BGRP0')),

    #pA running
    L2EFChain_MB('mbSpTrk_L1ZDC',      l2=Chain(counter=420, seed='L1_ZDC'),      efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1ZDC_A',    l2=Chain(counter=396, seed='L1_ZDC_A'),    efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1ZDC_C',    l2=Chain(counter=415, seed='L1_ZDC_C'),    efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1MBTS_1',   l2=Chain(counter=421, seed='L1_MBTS_1'),   efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1MBTS_2',   l2=Chain(counter=422, seed='L1_MBTS_2'),   efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1MBTS_1_1', l2=Chain(counter=423, seed='L1_MBTS_1_1'), efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1TE0',      l2=Chain(counter=424, seed='L1_TE0'),      efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1FJ0',      l2=Chain(counter=425, seed='L1_FJ0'),      efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1FJ0_A',    l2=Chain(counter=426, seed='L1_FJ0_A'),    efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1FJ0_C',    l2=Chain(counter=427, seed='L1_FJ0_C'),    efhypo=hypos.atLeastOneTrack),
    L2EFChain_MB('mbSpTrk_L1LHCF',     l2=Chain(counter=400, seed='L1_LHCF'),     efhypo=hypos.atLeastOneTrack),

    ]


#for high multiplicity (Mh)

MinBias += [
    
    #pPb chains
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk100_L1TE10',            l2=Chain(counter=795, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk110_L1TE10',            l2=Chain(counter=794, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_110),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk120_L1TE10',            l2=Chain(counter=791, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk130_L1TE10',            l2=Chain(counter=786, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_130),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk140_L1TE20',            l2=Chain(counter=257, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_140),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk150_L1TE20',            l2=Chain(counter=245, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_150),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk155_L1TE20',            l2=Chain(counter=244, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_155),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk160_L1TE20',            l2=Chain(counter=238, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_160),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk170_L1TE35',            l2=Chain(counter=784, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_170),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk175_L1TE35',            l2=Chain(counter=783, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_175),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk180_L1TE35',            l2=Chain(counter=782, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_180),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk190_L1TE35',            l2=Chain(counter=781, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_190),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk140_L1TE50',            l2=Chain(counter=236, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_140),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk150_L1TE50',            l2=Chain(counter=694, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_150),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk155_L1TE50',            l2=Chain(counter=695, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_155),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk160_L1TE50',            l2=Chain(counter=698, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700, efhypo=hypos.EFMbVxHypoMh_hip_160),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk170_L1TE50',            l2=Chain(counter=805, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_170),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk175_L1TE50',            l2=Chain(counter=719, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_175),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk180_L1TE50',            l2=Chain(counter=724, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_180),
                 
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk190_L1TE50',            l2=Chain(counter=706, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, efhypo=hypos.EFMbVxHypoMh_hip_190),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk180_L1TE65',            l2=Chain(counter=278, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_180),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk190_L1TE65',            l2=Chain(counter=279, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_190),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk200_L1TE65',            l2=Chain(counter=280, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_200),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk210_L1TE65',            l2=Chain(counter=283, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_210),
  
   L2EFChain_MB('mbSpTrkVtxMh_hip_trk220_L1TE65',            l2=Chain(counter=865, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_220),
   
   L2EFChain_MB('mbSpTrkVtxMh_hip_trk225_L1TE65',            l2=Chain(counter=866, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_225),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk230_L1TE65',            l2=Chain(counter=881, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_230),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk240_L1TE65',            l2=Chain(counter=882, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_240),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk245_L1TE65',            l2=Chain(counter=898, seed="L1_TE65"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_245),
   #TE90
   L2EFChain_MB('mbSpTrkVtxMh_hip_trk200_L1TE90',            l2=Chain(counter=80, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_200),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk210_L1TE90',            l2=Chain(counter=84, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_210),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk220_L1TE90',            l2=Chain(counter=85, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_220),
  
   L2EFChain_MB('mbSpTrkVtxMh_hip_trk230_L1TE90',            l2=Chain(counter=88, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_230),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk240_L1TE90',            l2=Chain(counter=697, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_240),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk260_L1TE90',            l2=Chain(counter=674, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_260),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk175_L1MBTS',            l2=Chain(counter=360, seed="L1_MBTS_4_4"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, efhypo=hypos.EFMbVxHypoMh_hip_175),

    #pile up suppressed HMT
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk140_L1TE20_pileupSup',            l2=Chain(counter=168, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700,  l2hypo2=hypos.HIL2VtxMultHyp_600, efhypo=hypos.EFMbVxHypoMh_hip_140),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk160_L1TE50_pileupSup',            l2=Chain(counter=806, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2700,  l2hypo2=hypos.HIL2VtxMultHyp_700, efhypo=hypos.EFMbVxHypoMh_hip_160),
    
    L2EFChain_MB('mbSpTrkVtxMh_hip_trk180_L1TE50_pileupSup',            l2=Chain(counter=872, seed="L1_TE50"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_3800, l2hypo2=hypos.HIL2VtxMultHyp_800, efhypo=hypos.EFMbVxHypoMh_hip_180),

   L2EFChain_MB('mbSpTrkVtxMh_hip_trk200_L1TE65_pileupSup',            l2=Chain(counter=873, seed="L1_TE65"),
                algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, l2hypo2=hypos.HIL2VtxMultHyp_900, efhypo=hypos.EFMbVxHypoMh_hip_200),

    L2EFChain_MB('mbSpTrkVtxMh_hip_trk240_L1TE90_pileupSup',            l2=Chain(counter=677, seed="L1_TE90"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, l2hypo2=hypos.HIL2VtxMultHyp_1100, efhypo=hypos.EFMbVxHypoMh_hip_240),
                 
   L2EFChain_MB('mbSpTrkVtxMh_hip_trk260_L1TE90_pileupSup',            l2=Chain(counter=673, seed="L1_TE90"),
                algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_4400, l2hypo2=hypos.HIL2VtxMultHyp_1200, efhypo=hypos.EFMbVxHypoMh_hip_260),

    #pp HMT chains
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk30_L1TE0_pileupSup',            l2=Chain(counter=1451, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_800, l2hypo2=hypos.HIL2VtxMultHyp_100, efhypo=hypos.EFMbVxHypoMh_hip_30),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk30_L1TE10_pileupSup',            l2=Chain(counter=1452, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_800, l2hypo2=hypos.HIL2VtxMultHyp_100, efhypo=hypos.EFMbVxHypoMh_hip_30),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk30_L1TE20_pileupSup',            l2=Chain(counter=1453, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_800, l2hypo2=hypos.HIL2VtxMultHyp_100, efhypo=hypos.EFMbVxHypoMh_hip_30),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1TE10',            l2=Chain(counter=1128, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, efhypo=hypos.EFMbVxHypoMh_hip_50),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1TE10_pileupSup',            l2=Chain(counter=1101, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, l2hypo2=hypos.HIL2VtxMultHyp_150, efhypo=hypos.EFMbVxHypoMh_hip_50),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1MBTS',            l2=Chain(counter=1102, seed="L1_MBTS_1_1"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, efhypo=hypos.EFMbVxHypoMh_hip_50),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1MBTS_pileupSup',            l2=Chain(counter=1103, seed="L1_MBTS_1_1"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, l2hypo2=hypos.HIL2VtxMultHyp_150, efhypo=hypos.EFMbVxHypoMh_hip_50),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1TE0',            l2=Chain(counter=1145, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, efhypo=hypos.EFMbVxHypoMh_hip_50),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk50_L1TE0_pileupSup',            l2=Chain(counter=1146, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1000, l2hypo2=hypos.HIL2VtxMultHyp_150, efhypo=hypos.EFMbVxHypoMh_hip_50),

    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1TE10',            l2=Chain(counter=1104, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, efhypo=hypos.EFMbVxHypoMh_hip_60),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1TE10_pileupSup',            l2=Chain(counter=1105, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_60),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1TE0',            l2=Chain(counter=1147, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, efhypo=hypos.EFMbVxHypoMh_hip_60),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1TE0_pileupSup',            l2=Chain(counter=1148, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_60),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1MBTS',            l2=Chain(counter=1106, seed="L1_MBTS_1_1"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, efhypo=hypos.EFMbVxHypoMh_hip_60),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk60_L1MBTS_pileupSup',            l2=Chain(counter=1107, seed="L1_MBTS_1_1"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1150, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_60),

    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE0',            l2=Chain(counter=1149, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, efhypo=hypos.EFMbVxHypoMh_hip_70),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE0_pileupSup',            l2=Chain(counter=1150, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_70),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE10',            l2=Chain(counter=1108, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, efhypo=hypos.EFMbVxHypoMh_hip_70),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE10_pileupSup',            l2=Chain(counter=1109, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_70),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE20',            l2=Chain(counter=1110, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, efhypo=hypos.EFMbVxHypoMh_hip_70),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE20_pileupSup',            l2=Chain(counter=1111, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1300, l2hypo2=hypos.HIL2VtxMultHyp_200, efhypo=hypos.EFMbVxHypoMh_hip_70),
    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE0',            l2=Chain(counter=1151, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, efhypo=hypos.EFMbVxHypoMh_hip_80),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE0_pileupSup',            l2=Chain(counter=1152, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, l2hypo2=hypos.HIL2VtxMultHyp_300, efhypo=hypos.EFMbVxHypoMh_hip_80),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE10',            l2=Chain(counter=1112, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, efhypo=hypos.EFMbVxHypoMh_hip_80),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE10_pileupSup',            l2=Chain(counter=1113, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, l2hypo2=hypos.HIL2VtxMultHyp_300, efhypo=hypos.EFMbVxHypoMh_hip_80),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE20',            l2=Chain(counter=1114, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, efhypo=hypos.EFMbVxHypoMh_hip_80),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk80_L1TE20_pileupSup',            l2=Chain(counter=1115, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1450, l2hypo2=hypos.HIL2VtxMultHyp_300, efhypo=hypos.EFMbVxHypoMh_hip_80),
    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE0',            l2=Chain(counter=1153, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, efhypo=hypos.EFMbVxHypoMh_hip_90),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE0_pileupSup',            l2=Chain(counter=1154, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, l2hypo2=hypos.HIL2VtxMultHyp_350, efhypo=hypos.EFMbVxHypoMh_hip_90),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE10',            l2=Chain(counter=1116, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, efhypo=hypos.EFMbVxHypoMh_hip_90),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE10_pileupSup',            l2=Chain(counter=1117, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, l2hypo2=hypos.HIL2VtxMultHyp_350, efhypo=hypos.EFMbVxHypoMh_hip_90),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE20',            l2=Chain(counter=1118, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, efhypo=hypos.EFMbVxHypoMh_hip_90),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk90_L1TE20_pileupSup',            l2=Chain(counter=1119, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1600, l2hypo2=hypos.HIL2VtxMultHyp_350, efhypo=hypos.EFMbVxHypoMh_hip_90),
    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE0',            l2=Chain(counter=1155, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE0_pileupSup',            l2=Chain(counter=1156, seed="L1_TE0"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, l2hypo2=hypos.HIL2VtxMultHyp_400, efhypo=hypos.EFMbVxHypoMh_hip_100),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE10',            l2=Chain(counter=1144, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE10_pileupSup',            l2=Chain(counter=1137, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, l2hypo2=hypos.HIL2VtxMultHyp_400, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE10',            l2=Chain(counter=1138, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_110),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE10_pileupSup',            l2=Chain(counter=1139, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, l2hypo2=hypos.HIL2VtxMultHyp_500, efhypo=hypos.EFMbVxHypoMh_hip_110),
    
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE10',            l2=Chain(counter=1140, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE10_pileupSup',            l2=Chain(counter=1141, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, l2hypo2=hypos.HIL2VtxMultHyp_550, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE10',            l2=Chain(counter=1142, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, efhypo=hypos.EFMbVxHypoMh_hip_130),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE10_pileupSup',            l2=Chain(counter=1143, seed="L1_TE10"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, l2hypo2=hypos.HIL2VtxMultHyp_600, efhypo=hypos.EFMbVxHypoMh_hip_130),
    
    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE35',            l2=Chain(counter=1120, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE35_pileupSup',            l2=Chain(counter=1121, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, l2hypo2=hypos.HIL2VtxMultHyp_400, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE20',            l2=Chain(counter=1122, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, efhypo=hypos.EFMbVxHypoMh_hip_100),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk100_L1TE20_pileupSup',            l2=Chain(counter=1123, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_1800, l2hypo2=hypos.HIL2VtxMultHyp_400, efhypo=hypos.EFMbVxHypoMh_hip_100),
    #

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE35',            l2=Chain(counter=1124, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_110),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE35_pileupSup',            l2=Chain(counter=1125, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, l2hypo2=hypos.HIL2VtxMultHyp_500, efhypo=hypos.EFMbVxHypoMh_hip_110),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE20',            l2=Chain(counter=1126, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, efhypo=hypos.EFMbVxHypoMh_hip_110),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk110_L1TE20_pileupSup',            l2=Chain(counter=1127, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2000, l2hypo2=hypos.HIL2VtxMultHyp_500, efhypo=hypos.EFMbVxHypoMh_hip_110),
    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE35',            l2=Chain(counter=1136, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE35_pileupSup',            l2=Chain(counter=1129, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, l2hypo2=hypos.HIL2VtxMultHyp_550, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE20',            l2=Chain(counter=1130, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, efhypo=hypos.EFMbVxHypoMh_hip_120),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk120_L1TE20_pileupSup',            l2=Chain(counter=1131, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2200, l2hypo2=hypos.HIL2VtxMultHyp_550, efhypo=hypos.EFMbVxHypoMh_hip_120),

    #
    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE35',            l2=Chain(counter=1132, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, efhypo=hypos.EFMbVxHypoMh_hip_130),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE35_pileupSup',            l2=Chain(counter=1133, seed="L1_TE35"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, l2hypo2=hypos.HIL2VtxMultHyp_600, efhypo=hypos.EFMbVxHypoMh_hip_130),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE20',            l2=Chain(counter=1134, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, efhypo=hypos.EFMbVxHypoMh_hip_130),

    L2EFChain_MB('mbSpTrkVtxMh_pp_trk130_L1TE20_pileupSup',            l2=Chain(counter=1135, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
                 l2hypo=hypos.L2MbSpMhNoPixHypo_hip_2400, l2hypo2=hypos.HIL2VtxMultHyp_600, efhypo=hypos.EFMbVxHypoMh_hip_130),
    
        
#     L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE10',            l2=Chain(counter=948, seed="L1_TE10"),
#                  algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),  
#                  l2hypo=hypos.L2MbSpMhNoPixHypo, efhypo=hypos.EFMbVxHypo),

#     L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE20',            l2=Chain(counter=946, seed="L1_TE20"),
#                  algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
#                  l2hypo=hypos.L2MbSpMhNoPixHypo, efhypo=hypos.EFMbVxHypo), 

#     L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE10_pileupSup',            l2=Chain(counter=936, seed="L1_TE10"),
#                  algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
#                  l2hypo=hypos.L2MbSpMhNoPixHypo, l2hypo2=hypos.HIL2VtxMultHyp_500, efhypo=hypos.EFMbVxHypo),          
    
#     L2EFChain_MB('mbSpTrkVtxMh_pp_trk70_L1TE20_pileupSup',            l2=Chain(counter=906, seed="L1_TE20"),
#                  algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
#                  l2hypo=hypos.L2MbSpMhNoPixHypo, l2hypo2=hypos.HIL2VtxMultHyp_500, efhypo=hypos.EFMbVxHypo),

    L2EFChain_MB('mbSpTrkVtxMh',            l2=Chain(counter=521, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo, efhypo=hypos.EFMbVxHypo),

    L2EFChain_MB('mbSpTrkVtxMh_eff',        l2=Chain(counter=255, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_eff, efhypo=hypos.EFMbVxHypo_PT),

    L2EFChain_MB('mbSpTrkVtxMh_medium',     l2=Chain(counter=442, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_medium, efhypo=hypos.EFMbVxHypo),
    
    L2EFChain_MB('mbSpTrkVtxMh_medium_eff', l2=Chain(counter=443, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_medium_eff, efhypo=hypos.EFMbVxHypo_PT),

    L2EFChain_MB('mbSpTrkVtxMh_tight',      l2=Chain(counter=488, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_tight, efhypo=hypos.EFMbVxHypo),
    
    L2EFChain_MB('mbSpTrkVtxMh_tight_eff',  l2=Chain(counter=489, seed="L1_TE20"),
                 algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                 l2hypo=hypos.L2MbSpMhNoPixHypo_tight_eff, efhypo=hypos.EFMbVxHypo_PT),


    ]

# for HI
MinBias += [
    L2EFChain_MB('mbSp_HI',                l2=Chain(name='L2_mbSp', counter=318, seed='L1_RD0_FILLED'),
                 efhypo=None),
    L2EFChain_MB('mbSp_HI_L1BGRP1',        l2=Chain(name='L2_mbSp_L1BGRP1', counter=375, seed='L1_BGRP1'),
                 efhypo=None),
    L2EFChain_MB('mbSp_unpaired_iso_HI',   l2=Chain(name='L2_mbSp_unpaired_iso', counter=879, seed='L1_RD0_UNPAIRED_ISO'),
                 efhypo=None),
            ]

# #no EF 
MinBias += [L2EFChain_MB('mbSpTrk_L1J20_MV', l2=Chain(counter=624, seed='L1_J20_MV'),
                         efhypo=None)]

MinBias += [L2EFChain_MB('mbSpBg_unpaired', l2=Chain(counter=984, seed='L1_RD0_UNPAIRED'),
                         l2hypo=hypos.L2MbSpHypoBg),
            L2EFChain_MB('mbSpBg_unpaired_iso', l2=Chain(counter=984, seed='L1_RD0_UNPAIRED_ISO'),
                         l2hypo=hypos.L2MbSpHypoBg),
            L2EFChain_MB('mbSpBg_noiseSup_unpaired_iso', l2=Chain(counter=633, seed='L1_RD0_UNPAIRED_ISO'),
                         l2hypo=hypos.L2MbSpHypoBg,
                         algs=mb_algs(l2spfex=fexes.L2MbSpFex_SCTNoiseSup)),
            L2EFChain_MB('mbSpBg_unpaired_iso_HI', l2=Chain(name='L2_mbSpBg_unpaired_iso',counter=984, seed='L1_RD0_UNPAIRED_ISO'),
                         l2hypo=hypos.L2MbSpHypoBg, efhypo=hypos.EFMbTrkHypo_PT), 
            ]

###########################################################################
#
#for efficiency chains redefine sequences (like calib, no Trt)                                        
#
class L2EFChain_MB_eff(MinBiasTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, # by default there is no customization at EF
                 l2sphypo=hypos.L2MbSpHypo_PT,        # if nothing is given this L2 hypo is used for space points                 
                 l2trthypo=hypos.L2MbTrtHypo_PT,
                 l2mbtshypo=hypos.L2MbMbtsHypo_PT,
                 efhypo=hypos.EFMbTrkHypo_PT,         # if nothing is given this EF hypo is used
                 algs=mb_algs()):                     # if nothing is given default set of algs is used
        MinBiasTemplate.__init__(self, sig, l2, ef)
                
        self.addSequence("", [fexes.dummyRoI], "L2_mbDummyRoI")
        
        self.continueSequence([algs.l2spfex, l2sphypo], "L2_mbSpHypo_PT_%s"%(self.l2.chain_name.replace('L2_','', 1)) ).addSignature()

        self.continueSequence([algs.l2mbtsfex, l2mbtshypo], self.l2.chain_name ).addSignature() 

        if efhypo:
            self.continueSequence( algs.efid +
                                   [algs.effex, algs.efvfex, efhypo], self.ef.chain_name).addSignature() 
            
MinBias += [
    L2EFChain_MB_eff('mbRd0_eff_unpaired',        l2=Chain(counter=1600,  seed='L1_RD0_UNPAIRED')),
    L2EFChain_MB_eff('mbRd0_eff_unpaired_iso',    l2=Chain(counter=922,   seed='L1_RD0_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbRd0_eff_unpaired_iso_HI', l2=Chain(name='L2_mbRd0_eff_unpaired_iso', counter=1600, seed='L1_RD0_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbMbts_2_eff', l2=Chain(name='L2_mbMbts_2_eff',counter=300, seed='L1_MBTS_2'), efhypo=None),

    L2EFChain_MB_eff('mbMbts_1_1_eff_HI',         l2=Chain(name='L2_mbMbts_1_1_eff',counter=999, seed='L1_MBTS_1_1')),
    L2EFChain_MB_eff('mbMbts_1_eff', l2=Chain(name='L2_mbMbts_1_eff',counter=15, seed='L1_MBTS_1'), efhypo=None),
    
    L2EFChain_MB_eff('mbMbts_2_2_eff_HI',         l2=Chain(name='L2_mbMbts_2_2_eff',counter=998, seed='L1_MBTS_2_2')),
    L2EFChain_MB_eff('mbMbts_3_3_eff_HI',         l2=Chain(name='L2_mbMbts_3_3_eff',counter=997, seed='L1_MBTS_3_3')),
    L2EFChain_MB_eff('mbMbts_4_4_eff_HI',         l2=Chain(name='L2_mbMbts_4_4_eff',counter=983, seed='L1_MBTS_4_4')),
    L2EFChain_MB_eff('mbMbts_1_1_eff_unpaired_iso_HI', l2=Chain(name='L2_mbMbts_1_1_eff_unpaired_iso',counter=982,  seed='L1_MBTS_1_1_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbMbts_2_2_eff_unpaired_iso_HI', l2=Chain(name='L2_mbMbts_2_2_eff_unpaired_iso',counter=994,  seed='L1_MBTS_2_2_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbMbts_3_3_eff_unpaired_iso_HI', l2=Chain(name='L2_mbMbts_3_3_eff_unpaired_iso',counter=993,  seed='L1_MBTS_3_3_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbMbts_4_4_eff_unpaired_iso_HI', l2=Chain(name='L2_mbMbts_4_4_eff_unpaired_iso',counter=992,  seed='L1_MBTS_4_4_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbRd0_eff_empty',           l2=Chain(counter=239,   seed='L1_RD0_EMPTY')),
    L2EFChain_MB_eff('mbMbts_2_eff_unpaired_iso', l2=Chain(counter=1000,  seed='L1_MBTS_2_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbRd1_eff',                 l2=Chain(counter=831,   seed='L1_RD1_FILLED')),
    L2EFChain_MB_eff('mb_ZC_eff',                 l2=Chain(counter=1611,  seed='L1_ZDC')),
    L2EFChain_MB_eff('mbZdc_eff',                 l2=Chain(counter=923,   seed='L1_ZDC')),
    L2EFChain_MB_eff('mbZdc_eff_HI',              l2=Chain(name='L2_mbZdc_eff',counter=1611, seed='L1_ZDC')),
    L2EFChain_MB_eff('mbZdc_and_eff_HI',          l2=Chain(name='L2_mbZdc_and_eff',counter=975, seed='L1_ZDC_AND')),
    L2EFChain_MB_eff('mbZdc_a_c_eff_HI',          l2=Chain(name='L2_mbZdc_a_c_eff',counter=965, seed='L1_ZDC_A_C')),
    L2EFChain_MB_eff('mb_BCM_Wide_eff',           l2=Chain(counter=811,   seed='L1_BCM_Wide_BGRP0')),
    L2EFChain_MB_eff('mb_BCM_HT_eff',             l2=Chain(counter=1334,  seed='L1_BCM_HT_BGRP0')),
    L2EFChain_MB_eff('mb_BCM_AC_CA_eff',          l2=Chain(counter=1335,  seed='L1_BCM_AC_CA_BGRP0')),
    L2EFChain_MB_eff('mbLucid_eff',               l2=Chain(counter=821,   seed='L1_LUCID')),
    L2EFChain_MB_eff('mbLucid_eff_empty',               l2=Chain(counter=172,   seed='L1_LUCID_EMPTY')),
    L2EFChain_MB_eff('mbLucid_eff_unpaired_iso',               l2=Chain(counter=173,   seed='L1_LUCID_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbLucid_eff_HI',            l2=Chain(name='L2_mbLucid_eff',counter=978, seed='L1_LUCID')),
    L2EFChain_MB_eff('mbLucid_a_c_eff_HI',        l2=Chain(name='L2_mbLucid_a_c_eff',counter=977, seed='L1_LUCID_A_C')),
    L2EFChain_MB_eff('mbLucid_a_c_eff_unpaired_iso_HI', l2=Chain(name='L2_mbLucid_a_c_eff_unpaired_iso',counter=967, seed='L1_LUCID_A_C_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbZdc_eff_unpaired',        l2=Chain(counter=1616,  seed='L1_ZDC_UNPAIRED')),
    L2EFChain_MB_eff('mbZdc_eff_unpaired_iso',    l2=Chain(counter=1616,  seed='L1_ZDC_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbZdc_eff_unpaired_HI',     l2=Chain(name='L2_mbZdc_eff_unpaired',counter=1616, seed='L1_ZDC_UNPAIRED')),
    L2EFChain_MB_eff('mbZdc_and_eff_unpaired_iso_HI',   l2=Chain(name='L2_mbZdc_and_eff_unpaired_iso',counter=974, seed='L1_ZDC_AND_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbZdc_a_c_eff_unpaired_iso_HI',   l2=Chain(name='L2_mbZdc_a_c_eff_unpaired_iso',counter=966, seed='L1_ZDC_A_C_UNPAIRED_ISO')),
    L2EFChain_MB_eff('mbTE20_eff',                l2=Chain(counter=14, seed='L1_TE20'),
                     algs=mb_algs(l2spfex=fexes.L2MbSpFex_noPix, effex=fexes.EFMbTrkFex,efvfex=fexes.EFMbVxFex),
                     efhypo=hypos.EFMbVxHypo_PT),
    ]

# # for HI
MinBias += [
    L2EFChain_MB_eff('mbRd1_eff_HI',  l2=Chain(name='L2_mbRd1_eff', counter=1609, seed='L1_RD1_FILLED')),
    ]


# ###########################################################################
class L2EFChain_MB_extended_eff(MinBiasTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, # by default there is no customization at EF
                 l2sphypo=hypos.L2MbSpHypo_PT, # if nothng is given this L2 hypo is used for space points                 
                 l2trthypo=hypos.L2MbTrtHypo_PT,
                 l2mbtshypo=hypos.L2MbMbtsHypo_PT,
                 l2zdchypo=hypos.L2MbZdcHypo_PT,
                 efhypo=hypos.EFMbTrkHypo_PT,  # if nothingis given this EF hypo is used
                 algs=mb_algs()): # if nothing is given default set of algs is used
        MinBiasTemplate.__init__(self, sig, l2, ef)


        self.addSequence("", [fexes.dummyRoI], "L2_mbDummyRoI")

        self.continueSequence([algs.l2spfex, l2sphypo], "L2_mbSpHypo_PT_%s"%(self.l2.chain_name.replace('L2_','', 1)) ).addSignature()
        
        self.continueSequence([fexes.L2MbZdcFex, l2zdchypo], "L2_mbZdcHypo_PT_%s"%(self.l2.chain_name.replace('L2_','', 1)) ).addSignature()

        self.continueSequence([algs.l2mbtsfex, l2mbtshypo], self.l2.chain_name ).addSignature() 

        if efhypo:
            self.continueSequence( algs.efid +
                                   [algs.effex, algs.efvfex, efhypo], self.ef.chain_name).addSignature() 

MinBias += [
    L2EFChain_MB_extended_eff('mbZdc_a_c_L2Zdc_eff',  l2=Chain(name='L2_mbZdc_a_c_L2Zdc_eff', counter=832, seed='L1_ZDC_A_C'),
                              efhypo=None),
    L2EFChain_MB_extended_eff('mbTe10_L2Zdc_eff',  l2=Chain(name='L2_mbTe10_L2Zdc_eff', counter=833, seed='L1_TE10'),
                              efhypo=None),
    ]

###########################################################################
#
# ZDC assymetric chains for HI p+Pb
#
class L2EFChain_MB_ZDC(MinBiasTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, # default empty EF chain
                 zdcfex=None, zdchypo=None, doMbSp=False):
        MinBiasTemplate.__init__(self, sig, l2, ef)
        assert zdcfex != None,  'L2 fex not given'
        assert zdchypo != None, 'L2 hypo not given'
        
        self.addSequence('', [fexes.dummyRoI], 'L2_mbDummyRoI')
        if doMbSp:
            self.continueSequence( [mb_algs.l2spfex, hypos.L2MbSpHypo], 'L2_'+sig+'_MbSp').addSignature()
        self.continueSequence( [zdcfex, zdchypo], 'L2_'+sig).addSignature()

MinBias += [
    L2EFChain_MB_ZDC('mbZdc_LG_eff', l2=Chain(name='L2_mbZdc_LG_eff', counter=5400, seed='L1_RD0_FILLED'),
                     zdcfex=fexes.L2MbZdcFex_LG , zdchypo=hypos.L2MbZdcHypo_PT, doMbSp=True),
    L2EFChain_MB_ZDC('mbZdc_HG_eff', l2=Chain(name='L2_mbZdc_HG_eff', counter=5401, seed='L1_RD0_FILLED'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_PT, doMbSp=True),
    L2EFChain_MB_ZDC('mbZdc_HG_eff_L1MBTS2', l2=Chain(name='L2_mbZdc_HG_eff_L1MBTS2', counter=5402, seed='L1_MBTS_2'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_PT),
    
    L2EFChain_MB_ZDC('mbZdc_HG_sideA_L1MBTS2', l2=Chain(name='L2_mbZdc_HG_sideA_L1MBTS2', counter=787, seed='L1_MBTS_2'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_hip_low_sideA),
    L2EFChain_MB_ZDC('mbZdc_HG_sideC_L1MBTS2', l2=Chain(name='L2_mbZdc_HG_sideC_L1MBTS2', counter=788, seed='L1_MBTS_2'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_hip_low_sideC),

    L2EFChain_MB_ZDC('mbZdc_HG_sideA', l2=Chain(name='L2_mbZdc_HG_sideA', counter=789, seed='L1_ZDC_A'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_hip_hi_sideA),
    L2EFChain_MB_ZDC('mbZdc_HG_sideC', l2=Chain(name='L2_mbZdc_HG_sideC', counter=790, seed='L1_ZDC_C'),
                     zdcfex=fexes.L2MbZdcFex_HG , zdchypo=hypos.L2MbZdcHypo_hip_hi_sideC),

    L2EFChain_MB_ZDC('mbZdc_LG_sideA_L1MBTS2', l2=Chain(name='L2_mbZdc_LG_sideA_L1MBTS2', counter=5403, seed='L1_MBTS_2'),
                     zdcfex=fexes.L2MbZdcFex_LG , zdchypo=hypos.L2MbZdcHypo_hip_low_sideA),
    L2EFChain_MB_ZDC('mbZdc_LG_sideC_L1MBTS2', l2=Chain(name='L2_mbZdc_LG_sideC_L1MBTS2', counter=5404, seed='L1_MBTS_2'),
                     zdcfex=fexes.L2MbZdcFex_LG , zdchypo=hypos.L2MbZdcHypo_hip_low_sideC),

    L2EFChain_MB_ZDC('mbZdc_LG_sideA', l2=Chain(name='L2_mbZdc_LG_sideA', counter=5405, seed='L1_ZDC_A'),
                     zdcfex=fexes.L2MbZdcFex_LG , zdchypo=hypos.L2MbZdcHypo_hip_hi_sideA),
    L2EFChain_MB_ZDC('mbZdc_LG_sideC', l2=Chain(name='L2_mbZdc_LG_sideC', counter=5406, seed='L1_ZDC_C'),
                     zdcfex=fexes.L2MbZdcFex_LG , zdchypo=hypos.L2MbZdcHypo_hip_hi_sideC),
    
    
    ]
###########################################################################
#
#for mbts chains
#        
class L2EFChain_MB_MBTS(MinBiasTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, # by default there is no customization at EF
                 l2hypo=None,
                 terminator=None):                    # if nothing is given default set of algs is used
        MinBiasTemplate.__init__(self, sig, l2, ef)
        assert l2hypo != None, 'L2 hypo not given'
                
        self.addSequence("", [fexes.L2MbMbtsFex, l2hypo], self.l2.chain_name ).addSignature() 
        if terminator:            
            self.continueSequence(terminator, "L2_%s_Terminate"%sig ).addSignature() 


MinBias += [

    L2EFChain_MB_MBTS('mbRmMbts_1_NoCut_Time', l2=Chain(counter=895,  seed='L1_RD0_FILLED'),
                      l2hypo=hypos.L2MbMbtsHypo_1_NTime),

    ]


#MBTS2 No cut time with Terminate Algo
MinBias += [
    L2EFChain_MB_MBTS('VdM_Monitoring',l2=Chain(counter=5016, seed='L1_MBTS_2_BGRP7,L1_BGRP7'), ef=None,
                      l2hypo=hypos.L2MbMbtsHypo_2_NTime, terminator = fexes.terminateAlgo),
    L2EFChain_MB_MBTS('mbMbts_2_Monitoring',l2=Chain(counter=5180, seed='L1_MBTS_2'), ef=None,
                      l2hypo=hypos.L2MbMbtsHypo_2_NTime, terminator = fexes.terminateAlgo),
    L2EFChain_MB_MBTS('mbRd0_Monitoring',l2=Chain(counter=5181, seed='L1_RD0_FILLED'), ef=None,
                      l2hypo=hypos.L2MbMbtsHypo_2_NTime, terminator = fexes.terminateAlgo),
    
    ]


#MBTS 1_1 
MinBias += [
    L2EFChain_MB_MBTS('mbMbts_2', l2=Chain(counter=301, seed='L1_MBTS_2'),
                      l2hypo = hypos.L2MbMbtsHypo_2),

    L2EFChain_MB_MBTS('mbMbts_2_L1BGRP7', l2=Chain(counter=120, seed='L1_MBTS_2_BGRP7'),
                      l2hypo = hypos.L2MbMbtsHypo_2),

    L2EFChain_MB_MBTS('mbMbts_1_1', l2=Chain(counter=507, seed='L1_MBTS_1_1'),
                      l2hypo = hypos.L2MbMbtsHypo_1_1),
    
    L2EFChain_MB_MBTS('mbMbts_1_1_overlay', l2=Chain(counter=1019, seed='L1_MBTS_1_1_OVERLAY'),
                      l2hypo = hypos.L2MbMbtsHypo_1_1),

    L2EFChain_MB_MBTS('mbMbts_2_unpaired_iso', l2=Chain(counter=302,   seed='L1_MBTS_2_UNPAIRED_ISO'),
                      l2hypo = hypos.L2MbMbtsHypo_2),

    L2EFChain_MB_MBTS('mbMbts_1_1_unpaired_iso', l2=Chain(counter=991,   seed='L1_MBTS_1_1_UNPAIRED_ISO'),
                      l2hypo = hypos.L2MbMbtsHypo_1_1),

    L2EFChain_MB_MBTS('mbMbts_2_2_unpaired_iso', l2=Chain(counter=990,   seed='L1_MBTS_2_2_UNPAIRED_ISO'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),

    L2EFChain_MB_MBTS('mbMbts_3_3_unpaired_iso', l2=Chain(counter=989,   seed='L1_MBTS_3_3_UNPAIRED_ISO'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),

    L2EFChain_MB_MBTS('mbMbts_4_4_unpaired_iso', l2=Chain(counter=988,   seed='L1_MBTS_4_4_UNPAIRED_ISO'),
                      l2hypo = hypos.L2MbMbtsHypo_4_4),

    L2EFChain_MB_MBTS('mbMbts_4_4', l2=Chain(counter=987, seed='L1_MBTS_4_4'),
                      l2hypo = hypos.L2MbMbtsHypo_4_4),

    L2EFChain_MB_MBTS('mbMbts_3_3', l2=Chain(counter=986, seed='L1_MBTS_3_3'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),
    
    L2EFChain_MB_MBTS('mbMbts_2_2', l2=Chain(counter=985, seed='L1_MBTS_2_2'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),

    L2EFChain_MB_MBTS('mbZdc_and_Mbts_2_2', l2=Chain(counter=950,   seed='L1_ZDC_AND'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),

    L2EFChain_MB_MBTS('mbZdc_and_Mbts_1_1', l2=Chain(counter=951,   seed='L1_ZDC_AND'),
                      l2hypo = hypos.L2MbMbtsHypo_1_1),
    
    L2EFChain_MB_MBTS('mbZdc_a_c_Mbts_2_2', l2=Chain(counter=973,   seed='L1_ZDC_A_C'),
                      l2hypo = hypos.L2MbMbtsHypo_2_2),
    
    L2EFChain_MB_MBTS('mbZdc_a_c_Mbts_1_1', l2=Chain(counter=954,   seed='L1_ZDC_A_C'),
                      l2hypo = hypos.L2MbMbtsHypo_1_1),  
    ]


###########################################################################
# Triggers with no L2 or EF algorithms.

L2EFChain_MB_noAlg = MinBiasTemplate

MinBias += [
    
    L2EFChain_MB_noAlg('L1TRT_NoAlg',      l2=Chain(counter=1789, seed='L1_TRT')),

    L2EFChain_MB_noAlg('L1LHCF_NoAlg',      l2=Chain(counter=395, seed='L1_LHCF')),

    L2EFChain_MB_noAlg('zerobias_NoAlg',   l2=Chain(counter=646,  seed='L1_ZB')),

    L2EFChain_MB_noAlg('zerobias_Overlay_NoAlg',   l2=Chain(counter=668,  seed='L1_ZB')),
    
    L2EFChain_MB_noAlg('rd1_empty_NoAlg',  l2=Chain(counter=733,  seed='L1_RD1_EMPTY')),

    L2EFChain_MB_noAlg('rd0_empty_NoAlg',  l2=Chain(counter=624,  seed='L1_RD0_EMPTY')),

    L2EFChain_MB_noAlg('rd0_filled_NoAlg', l2=Chain(counter=623,  seed='L1_RD0_FILLED')),

    L2EFChain_MB_noAlg('rd0_unpaired_iso_NoAlg', l2=Chain(counter=171,  seed='L1_RD0_UNPAIRED_ISO')),

    L2EFChain_MB_noAlg('mbZdc',            l2=Chain(counter=511,  seed='L1_ZDC')),

    L2EFChain_MB_noAlg('mbRndm_cosmic',    l2=Chain(counter=785,  seed='L1_RD0_EMPTY')),
   
    L2EFChain_MB_noAlg('mbZdc_and_cosmic_NoAlg',  l2=Chain(counter=976,   seed='L1_ZDC_AND_EMPTY')),
    
    L2EFChain_MB_noAlg('mbZdc_and_NoAlg',  l2=Chain(counter=981,  seed='L1_ZDC_AND')),
    
    L2EFChain_MB_noAlg('mbZdc_a_c_cosmic_NoAlg',  l2=Chain(counter=968,   seed='L1_ZDC_A_C_EMPTY')),

    L2EFChain_MB_noAlg('mbZdc_a_c_NoAlg',  l2=Chain(counter=969,  seed='L1_ZDC_A_C')),

    L2EFChain_MB_noAlg('mbZdc_a_c_overlay_NoAlg', l2=Chain(counter=800,   seed='L1_ZDC_A_C_OVERLAY')),

    L2EFChain_MB_noAlg('L1MBTS_1_1_overlay_NoAlg', l2=Chain(counter=800,   seed='L1_MBTS_1_1_OVERLAY')),

    L2EFChain_MB_noAlg('mbZdc_a_c_L1TE50_NoAlg',  l2=Chain(counter=943,   seed='L1_ZDC_A_C_TE50')),

    L2EFChain_MB_noAlg('mbMbts_1_1_unpaired',     l2=Chain(counter=1580,  seed='L1_MBTS_1_1_UNPAIRED')),

    L2EFChain_MB_noAlg('mbMbts_1_NoAlg', l2=Chain(counter=19,  seed='L1_MBTS_1')), 
    L2EFChain_MB_noAlg('mbMbts_2_NoAlg', l2=Chain(counter=18,  seed='L1_MBTS_2')),
    L2EFChain_MB_noAlg('mbMbts_2_unpaired_iso_NoAlg', l2=Chain(counter=20,  seed='L1_MBTS_2_UNPAIRED_ISO')),
    L2EFChain_MB_noAlg('mbMbts_1_unpaired_iso_NoAlg', l2=Chain(counter=21,  seed='L1_MBTS_1_UNPAIRED_ISO')),

    L2EFChain_MB_noAlg('mbMbts_1_empty_NoAlg', l2=Chain(counter=6,  seed='L1_MBTS_1')),
    L2EFChain_MB_noAlg('mbMbts_2_empty_NoAlg', l2=Chain(counter=7,  seed='L1_MBTS_2')),
    
    #for HI
    L2EFChain_MB_noAlg('L1TE0_NoAlg', l2=Chain(counter=101,   seed='L1_TE0')),
    L2EFChain_MB_noAlg('L1TE10_NoAlg', l2=Chain(counter=90,   seed='L1_TE10')),
    L2EFChain_MB_noAlg('L1TE20_NoAlg', l2=Chain(counter=822,   seed='L1_TE20')),
    L2EFChain_MB_noAlg('L1TE35_NoAlg', l2=Chain(counter=128,   seed='L1_TE35')),
    L2EFChain_MB_noAlg('L1TE50_NoAlg', l2=Chain(counter=750,   seed='L1_TE50')),
    L2EFChain_MB_noAlg('L1TE65_NoAlg', l2=Chain(counter=184,   seed='L1_TE65')),
    L2EFChain_MB_noAlg('L1TE90_NoAlg', l2=Chain(counter=189,   seed='L1_TE90')),
    L2EFChain_MB_noAlg('L1TE100_NoAlg', l2=Chain(counter=486,   seed='L1_TE100')),
    L2EFChain_MB_noAlg('L1TE115_NoAlg', l2=Chain(counter=473,   seed='L1_TE115')),
    L2EFChain_MB_noAlg('L1MBTS_2_NoAlg', l2=Chain(counter=874,   seed='L1_MBTS_2')),
    L2EFChain_MB_noAlg('L1MBTS_1_1_NoAlg', l2=Chain(counter=439,   seed='L1_MBTS_1_1')),
    L2EFChain_MB_noAlg('L1MBTS_3_3_NoAlg', l2=Chain(counter=440,   seed='L1_MBTS_3_3')),
    L2EFChain_MB_noAlg('mbZdc_NoAlg', l2=Chain(counter=826,   seed='L1_ZDC')),
    L2EFChain_MB_noAlg('L1ZDC_NoAlg', l2=Chain(counter=826,   seed='L1_ZDC')),
    L2EFChain_MB_noAlg('L1ZDC_A_NoAlg', l2=Chain(counter=444,   seed='L1_ZDC_A')),
    L2EFChain_MB_noAlg('L1ZDC_C_NoAlg', l2=Chain(counter=470,   seed='L1_ZDC_C')),
    L2EFChain_MB_noAlg('L1ZDC_A_C_BGRP7_NoAlg', l2=Chain(counter=835,   seed='L1_ZDC_A_C_BGRP7')),
    L2EFChain_MB_noAlg('L1FJ0_NoAlg', l2=Chain(counter=870,   seed='L1_FJ0')),
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus():
    #alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'
    alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_ELAST1,L1_ALFA_ELAST2,L1_ALFA_ELAST11,L1_ALFA_ELAST12,L1_ALFA_ELAST13,L1_ALFA_ELAST14,L1_ALFA_ELAST15,L1_ALFA_ELAST16,L1_ALFA_ELAST17,L1_ALFA_ELAST18,L1_ALFA_SDIFF5,L1_ALFA_SDIFF6,L1_ALFA_SDIFF7,L1_ALFA_SDIFF8,L1_ALFA_SHOW1,L1_ALFA_SHOW2,L1_ALFA_SHOW3,L1_ALFA_SHOW4,L1_ALFA_SYST1,L1_ALFA_SYST2,L1_ALFA_SYST3,L1_ALFA_SYST4,L1_ALFA_SHOWSYST1,L1_ALFA_SHOWSYST2,L1_ALFA_SHOWSYST3,L1_ALFA_SHOWSYST4,L1_ALFA_SYST9,L1_ALFA_SYST10,L1_ALFA_SYST11,L1_ALFA_SYST12,L1_ALFA_SYST17,L1_ALFA_SYST18,L1_ALFA_B7L1U,L1_ALFA_B7L1L,L1_ALFA_A7L1U,L1_ALFA_A7L1L,L1_ALFA_A7R1U,L1_ALFA_A7R1L,L1_ALFA_B7R1U,L1_ALFA_B7R1L,L1_ALFA_B1_EMPTY,L1_ALFA_B2_EMPTY,L1_MBTS_2_A_ALFA_C,L1_MBTS_2_C_ALFA_A,L1_MBTS_2_ALFA,L1_LUCID_A_ALFA_C,L1_LUCID_C_ALFA_A,L1_LUCID_ALFA,L1_ZDC_A_ALFA_C,L1_ZDC_C_ALFA_A,L1_ZDC_ALFA,L1_MBTS_1_A_ALFA_C,L1_MBTS_1_C_ALFA_A,L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO,L1_LUCID_A_ALFA_C_UNPAIRED_ISO,L1_LUCID_C_ALFA_A_UNPAIRED_ISO,L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO,L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO'

    alfa_seed_any = 'L1_ALFA_BGRP7,L1_ALFA_ANY,L1_ALFA_ANY_EMPTY,L1_ALFA_ANY_FIRSTEMPTY,L1_ALFA_ANY_UNPAIRED_ISO,L1_ALFA_ANY_UNPAIRED_NONISO'
    alfa_seed_sdiff ='L1_ALFA_SDIFF1,L1_ALFA_SDIFF2,L1_ALFA_SDIFF3,L1_ALFA_SDIFF4'
else:
    alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'
    alfa_seed_any = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'
    alfa_seed_sdiff = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'

MinBias += [
    L2EFChain_MB_noAlg('ALFA_Phys', l2=Chain(counter=5113,   seed=alfa_seed)),
    L2EFChain_MB_noAlg('ALFA_Phys_Any', l2=Chain(counter=5117,   seed=alfa_seed_any)),
    L2EFChain_MB_noAlg('mbMbts_1_A_ALFA_C_NoAlg', l2=Chain(counter=8,  seed='L1_MBTS_1_A_ALFA_C')), 
    L2EFChain_MB_noAlg('mbMbts_1_C_ALFA_A_NoAlg', l2=Chain(counter=9,  seed='L1_MBTS_1_C_ALFA_A')), 
    L2EFChain_MB_noAlg('mbMbts_1_C_ALFA_A_UNPAIRED_ISO_NoAlg', l2=Chain(counter=10,  seed='L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO')), 
    L2EFChain_MB_noAlg('mbMbts_1_A_ALFA_C_UNPAIRED_ISO_NoAlg', l2=Chain(counter=11,  seed='L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO')),     

    L2EFChain_MB('mbSpTrk_ALFA_any', l2=Chain(counter=295, seed='L1_ALFA_ANY')),
    L2EFChain_MB('mbSpTrk_ALFA_SDIFF', l2=Chain(counter=296, seed=alfa_seed_sdiff)),

    L2EFChain_MB_noAlg('alfa_any_NoAlg', l2=Chain(counter=616,   seed='L1_ALFA_ANY')),
    L2EFChain_MB_noAlg('alfa_sdiff_NoAlg', l2=Chain(counter=617,   seed=alfa_seed_sdiff)),

    ]


