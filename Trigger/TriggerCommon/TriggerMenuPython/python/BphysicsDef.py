# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for BPhysics chains/signatures
###########################################################################

from TriggerMenuPython.HltConfig import *

from string import atoi
from AthenaCommon.Logging import logging
from AthenaCommon import CfgGetter

from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Bphysics, TrigIDSCAN_Muon, TrigIDSCAN_FullScan
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Bphysics,  TrigSiTrack_FullScan, TrigSiTrack_Muon
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import  TrigL2SiTrackFinder_BphysicsA,TrigL2SiTrackFinder_BphysicsB,TrigL2SiTrackFinder_FullScanA, TrigL2SiTrackFinder_FullScanF
from TrigmuFast.TrigmuFastConfig import *
from TrigmuComb.TrigmuCombConfig import *
from TrigDiMuon.TrigDiMuonConfig import *
from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_Jpsi
from TrigMuonEF.TrigMuonEFConfig import *
from TrigMuonHypo.TrigMuonHypoConfig import *
from InDetTrigRecExample.EFInDetConfig import *
from TrigMuGirl.TrigMuGirlConfig import *
from TrigmuRoI.TrigmuRoIConfig import *

from TrigBphysHypo.TrigL2DsPhiPiFexConfig import L2DsPhiPiFex_1, L2DsPhiPiFex_FS
from TrigBphysHypo.TrigL2DsPhiPiHypoConfig import L2DsPhiPiHypo_1, L2DsPhiPiHypo_FS
from TrigBphysHypo.TrigL2JpsieeFexConfig import L2JpsieeFex_1, L2JpsieeFex_FS
from TrigBphysHypo.TrigL2JpsieeHypoConfig import L2JpsieeHypo_1,L2JpsieeHypo_FS
from TrigBphysHypo.TrigL2BgammaXFexConfig import L2BgammaXFex_1, L2BgammaXFex_FS
from TrigBphysHypo.TrigL2BgammaXHypoConfig import L2BgammaXHypo_1,L2BgammaXHypo_FS
from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_DiMu,L2BMuMuFex_DiMu_DY,L2BMuMuFex_DiMu_DY20, L2BMuMuFex_DiMu_DY25,L2BMuMuFex_DiMu_noOS,L2BMuMuFex_DiMu_DY_noOS, L2BMuMuFex_DiMu_DY14_noOS, L2BMuMuFex_Jpsi, L2BMuMuFex_Upsi, L2BMuMuFex_B, L2BMuMuFex_noId, L2BMuMuFex_DiMu_passL2,L2BMuMuFex_DiMu_noOS_passL2, L2BMuMuFex_Jpsi_passL2, L2BMuMuFex_Upsi_passL2, L2BMuMuFex_B_passL2, L2BMuMuFex_noId_passL2, L2BMuMuFex_noId_Z,L2BMuMuFex_noId_Z_passL2
from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_DiMu, L2BMuMuHypo_DiMu_DY, L2BMuMuHypo_DiMu_DY20, L2BMuMuHypo_DiMu_DY25, L2BMuMuHypo_Jpsi, L2BMuMuHypo_Jpsi_noId, L2BMuMuHypo_Upsi, L2BMuMuHypo_B, L2BMuMuHypo_DiMu_noVtx, L2BMuMuHypo_DiMu_DY_noVtx, L2BMuMuHypo_DiMu_DY14_noVtx, L2BMuMuHypo_DiMu_passL2, L2BMuMuHypo_Jpsi_passL2, L2BMuMuHypo_Upsi_passL2, L2BMuMuHypo_B_passL2, L2BMuMuHypo_DiMu_noVtx_passL2, L2BMuMuHypo_Z,L2BMuMuHypo_Z_passL2            
from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1, L2BMuMuXFex_noVtx, L2BMuMuXFex_noVtx_noOS, L2BMuMuXFex_BcMuMuDs, L2BMuMuXFex_BcMuMuDs_tight, L2BMuMuXFex_allModes, L2BMuMuXFex_allModes_tightBc
from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
from TrigBphysHypo.TrigL2DiMuHypoConfig import  TrigL2DiMuHypo_Jpsi_tight, TrigL2DiMuHypo_Upsi_tight, TrigL2DiMuHypo_Upsi, TrigL2DiMuHypo_Jpsi, TrigL2DiMuHypo_B, TrigL2DiMuHypo_DiMu,TrigL2DiMuHypo_Upsi_passL2, TrigL2DiMuHypo_Jpsi_passL2, TrigL2DiMuHypo_B_passL2, TrigL2DiMuHypo_DiMu_passL2, TrigL2DiMuHypo_DiMu_noOS, TrigL2DiMuHypo_DiMu_noOS_passL2

from TrigBphysHypo.TrigL2DiMuXFexConfig import   TrigL2DiMuXFex_1
from TrigBphysHypo.TrigL2DiMuXHypoConfig import   TrigL2DiMuXHypo_1
from TrigBphysHypo.TrigL2TrkMassFexConfig import   L2TrkMassFex_Jpsimumu, L2TrkMassFex_Jpsimumu_FS,L2TrkMassFex_Jpsimumu_loose,L2TrkMassFex_Jpsimumu_looser, L2TrkMassFex_Jpsimumu_loose_FS,L2TrkMassFex_Upsimumu_FS, L2TrkMassFex_Upsimumu_loose_FS, L2TrkMassFex_Jpsimumu_loose_passL2,L2TrkMassFex_Jpsimumu_looser_passL2, L2TrkMassFex_Jpsimumu_passL2, L2TrkMassFex_Jpsimumu_FS_passL2, L2TrkMassFex_Upsimumu_FS_passL2, L2TrkMassFex_Jpsimumu_loose_FS_passL2, L2TrkMassFex_Upsimumu_loose_FS_passL2
from TrigBphysHypo.TrigL2TrkMassHypoConfig import   L2TrkMassHypo_Jpsimumu, L2TrkMassHypo_Jpsimumu_FS,L2TrkMassHypo_Jpsimumu_loose, L2TrkMassHypo_Jpsimumu_looser, L2TrkMassHypo_Jpsimumu_loose_FS, L2TrkMassHypo_Upsimumu_FS,L2TrkMassHypo_Upsimumu_loose_FS, L2TrkMassHypo_Jpsimumu_loose_passL2, L2TrkMassHypo_Jpsimumu_looser_passL2, L2TrkMassHypo_Jpsimumu_passL2, L2TrkMassHypo_Jpsimumu_FS_passL2, L2TrkMassHypo_Upsimumu_FS_passL2, L2TrkMassHypo_Jpsimumu_loose_FS_passL2, L2TrkMassHypo_Upsimumu_loose_FS_passL2

from TrigBphysHypo.TrigL2MultiMuFexConfig import L2MultiMuFex_DiMu,L2MultiMuFex_Jpsi,L2MultiMuFex_Upsi,L2MultiMuFex_Tau
from TrigBphysHypo.TrigL2MultiMuHypoConfig import L2MultiMuHypo_DiMu,L2MultiMuHypo_Jpsi,L2MultiMuHypo_Upsi,L2MultiMuHypo_Tau

from TrigBphysHypo.TrigEFDsPhiPiFexConfig import EFDsPhiPiFex_1, EFDsPhiPiFex_FS
from TrigBphysHypo.TrigEFDsPhiPiHypoConfig import EFDsPhiPiHypo_1, EFDsPhiPiHypo_FS
from TrigBphysHypo.TrigEFJpsieeFexConfig import EFJpsieeFex_1, EFJpsieeFex_FS
from TrigBphysHypo.TrigEFJpsieeHypoConfig import EFJpsieeHypo_1, EFJpsieeHypo_FS
from TrigBphysHypo.TrigEFBgammaXFexConfig import EFBgammaXFex_1, EFBgammaXFex_FS
from TrigBphysHypo.TrigEFBgammaXHypoConfig import EFBgammaXHypo_1, EFBgammaXHypo_FS
from TrigBphysHypo.TrigEFBMuMuFexConfig import EFBMuMuFex_DiMu,EFBMuMuFex_DiMu_DY, EFBMuMuFex_DiMu_DY20, EFBMuMuFex_DiMu_DY25, EFBMuMuFex_Jpsi, EFBMuMuFex_Upsi, EFBMuMuFex_B, EFBMuMuFex_muEF,  EFBMuMuFex_DiMu_noOS,EFBMuMuFex_DiMu_DY_noOS, EFBMuMuFex_DiMu_DY14_noOS, EFBMuMuFex_Z, EFBMuMuFex_noId
from TrigBphysHypo.TrigEFBMuMuHypoConfig import EFBMuMuHypo_DiMu,EFBMuMuHypo_DiMu_DY, EFBMuMuHypo_DiMu_DY20, EFBMuMuHypo_DiMu_DY25, EFBMuMuHypo_Jpsi, EFBMuMuHypo_Upsi, EFBMuMuHypo_B, EFBMuMuHypo_B_Lxy, EFBMuMuHypo_DiMu_noVtx, EFBMuMuHypo_DiMu_DY_noVtx, EFBMuMuHypo_DiMu_DY14_noVtx, EFBMuMuHypo_Z
from TrigBphysHypo.TrigEFBMuMuXFexConfig import EFBMuMuXFex_1, EFBMuMuXFex_BcMuMuDs, EFBMuMuXFex_BcMuMuDs_tight, EFBMuMuXFex_allModes, EFBMuMuXFex_allModes_tightBc
from TrigBphysHypo.TrigEFBMuMuXHypoConfig import EFBMuMuXHypo_1, EFBMuMuXHypo_BcMuMuDs, EFBMuMuXHypo_BcMuMuDs_tight, EFBMuMuXHypo_allModes, EFBMuMuXHypo_allModes_tightBc
from TrigBphysHypo.TrigEFDiMuFexConfig import  TrigEFDiMuFexConfig
from TrigBphysHypo.TrigEFDiMuHypoConfig import  TrigEFDiMuHypo_Upsi, TrigEFDiMuHypo_Jpsi, TrigEFDiMuHypo_B, TrigEFDiMuHypo_DiMu, TrigEFDiMuHypo_DiMu_noOS
from TrigBphysHypo.TrigEFDiMuXFexConfig import TrigEFDiMuXFex_1
from TrigBphysHypo.TrigEFDiMuXHypoConfig import TrigEFDiMuXHypo_1
from TrigBphysHypo.TrigEFTrkMassFexConfig import   EFTrkMassFex_Jpsimumu, EFTrkMassFex_Jpsimumu_FS, EFTrkMassFex_Jpsimumu_loose,EFTrkMassFex_Jpsimumu_looser, EFTrkMassFex_Jpsimumu_loose_FS, EFTrkMassFex_Upsimumu_FS, EFTrkMassFex_Upsimumu_loose_FS
from TrigBphysHypo.TrigEFTrkMassHypoConfig import   EFTrkMassHypo_Jpsimumu, EFTrkMassHypo_Jpsimumu_FS, EFTrkMassHypo_Jpsimumu_loose,EFTrkMassHypo_Jpsimumu_looser, EFTrkMassHypo_Jpsimumu_loose_FS, EFTrkMassHypo_Upsimumu_FS,  EFTrkMassHypo_Upsimumu_loose_FS

from TrigBphysHypo.TrigEFMultiMuFexConfig import EFMultiMuFex_DiMu,EFMultiMuFex_Jpsi,EFMultiMuFex_Upsi,EFMultiMuFex_Tau
from TrigBphysHypo.TrigEFMultiMuHypoConfig import EFMultiMuHypo_DiMu,EFMultiMuHypo_Jpsi,EFMultiMuHypo_Upsi,EFMultiMuHypo_Tau


from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyCombineAlgo
from TrigGenericAlgs.TrigGenericAlgsConf import  PESA__DummyUnseededAllTEAlgo

#for pp_v2 menu the L2_mu6 TE seeds from MU6
mu6Seed='L1_MU6'
mu6Thr='MU6'
mu4mu6Seed='L1_2MU0_MU6' #really needs MU0_MU6
mu4Tmu6Seed='L1_2MU4_MU6'
mu6em3Seed='L1_MU6_EM3'
_2mu4TBarrelSeed='L1_2MU4_BARREL'
_2mu4TBarrelOnlySeed='L1_2MU4_BARRELONLY'
_mu4Tmu6BarrelSeed='L1_2MU4_MU6_BARREL'

mu4sequence="EF_mu4T"
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    mu4sequence="EF_mu4"


def TrigMuSuperEF_Bphys():
    return CfgGetter.getAlgorithmClone("TrigMuSuperEF_Bphys","TrigMuSuperEF",
                                       deltaEtaRoI = 0.75,
                                       deltaPhiRoI = 0.75,
                                       IdTracks         = "InDetTrigTrackSlimmer_Bphysics_EFID",
                                       IdTrackParticles = "InDetTrigParticleCreation_Bphysics_EFID",
                                       doInsideOut = False,
                                       doOutsideIn = True)
    

###########################################################################
#Set common properties
#
class L2EFChain_Bphysics(L2EFChainDef):

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
        def __init__(self, suffix, confirmmuon=None):
            self.suffix = suffix
            self.l2confirmmuon = confirmmuon
            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.L2_Dummy = DummyFEX("L2Dummy%s" % suffix)

    class EFConfig:
        def __init__(self, suffix, confirmmuon=None, TEs=None):
            self.suffix = suffix
            self.efconfirmmuon = confirmmuon
            self.efTEs = TEs

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig

Config   = L2EFChain_Bphysics.Config
L2create = L2EFChain_Bphysics.L2Config
EFcreate = L2EFChain_Bphysics.EFConfig
###########################################################################
Bphysics = [
    ]
###########################################################################
#TrkMassRoI
class L2EFChain_TrkMassRoI(L2EFChain_Bphysics):
    def defineSequences(self, config):
        from TriggerMenuPython.BphysicsSliceFlags import BphysicsSliceFlags
        #L2 sequences
        self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_stall')
        if "SiTrk" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_stall',
                               [TrigSiTrack_Bphysics()],
                               'L2_tracks')
        else:
            self.addL2Sequence('L2_stall',
                               [TrigIDSCAN_Bphysics()],
                               'L2_tracks')

        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_tracks',
                           [L2TrkMassFex_Jpsimumu(),
                            L2TrkMassHypo_Jpsimumu()],
                           'L2_')
        else:
            self.addL2Sequence('L2_tracks',
                           [L2TrkMassFex_Jpsimumu_passL2(),
                            L2TrkMassHypo_Jpsimumu_passL2()],
                           'L2_')
            
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
#            self.addEFSequence('L2_',
##            self.addEFSequence(self.config.EFConfig.efTEs,
## JKirk hardcode EF_mu4 here as I can't get the above line to work correctly.
            self.addEFSequence(mu4sequence,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_tracks')
                
            self.addEFSequence('EF_tracks',
                           [EFTrkMassFex_Jpsimumu(),
                            EFTrkMassHypo_Jpsimumu()],
                           'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')


    def defineSignatures(self):
        #L2 signatures
#        if self.config.L2Config.l2confirmmuon is not None:
#            self.addL2Signature([self.config.L2Config.l2confirmmuon])

        self.addL2Signature(['L2_stall'])
        self.addL2Signature(['L2_tracks'])
        self.addL2Signature(['L2_'])
        #EF signatures
#        if self.config.EFConfig.efconfirmmuon is not None:
#            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.L2Config.suffix:
            self.addEFSignature(['EF_tracks'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_stall' : mergeRemovingOverlap('L2_', self.config.L2Config.suffix+'_stall'),
            'L2_tracks': mergeRemovingOverlap('L2_', self.config.L2Config.suffix+'_id'),
            'L2_':       mergeRemovingOverlap('L2_', self.config.L2Config.suffix),
            'EF_tracks': mergeRemovingOverlap('EF_', self.config.EFConfig.suffix+'_id'),
            'EF_':       mergeRemovingOverlap('EF_', self.config.EFConfig.suffix),
            }
        pass

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkMassRoI('mu4_Trk_Jpsi',
                             'L2_mu4_Trk_Jpsi', 563, 'L1_MU0','EF_mu4_Trk_Jpsi', 563, ['L2_mu4'],
                             Config(L2create('_mu4_Trk_Jpsi'),
                                    EFcreate('_mu4_Trk_Jpsi',TEs='EF_mu4'))),
    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkMassRoI('mu4T_Trk_Jpsi',
                             'L2_mu4T_Trk_Jpsi', 563, 'L1_MU4','EF_mu4T_Trk_Jpsi', 563, ['L2_mu4T'],
                             Config(L2create('_mu4T_Trk_Jpsi'),
                                    EFcreate('_mu4T_Trk_Jpsi',TEs='EF_mu4T'))),    
        ]
###########################################################################
#TrkMassRoI
class L2EFChain_TrkMassRoI6(L2EFChain_Bphysics):
    def defineSequences(self, config):
        from TriggerMenuPython.BphysicsSliceFlags import BphysicsSliceFlags
        #L2 sequences
        self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_stall')
        if "SiTrk" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_stall',
                               [TrigSiTrack_Bphysics()],
                               'L2_tracks')

        elif "L2StarA" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_stall',
                               [TrigL2SiTrackFinder_BphysicsA()],
                               'L2_tracks')

        elif "L2StarB" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_stall',
                               [TrigL2SiTrackFinder_BphysicsB()],
                               'L2_tracks')

        else:
            self.addL2Sequence('L2_stall',
                               [TrigIDSCAN_Bphysics()],
                               'L2_tracks')

        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_tracks',
                           [L2TrkMassFex_Jpsimumu(),
                            L2TrkMassHypo_Jpsimumu()],
                           'L2_')
        else:
            self.addL2Sequence('L2_tracks',
                           [L2TrkMassFex_Jpsimumu_passL2(),
                            L2TrkMassHypo_Jpsimumu_passL2()],
                           'L2_')
            
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            #            self.addEFSequence('L2_',
            #            self.addEFSequence(self.config.EFConfig.efTEs,
            # JKirk hardcode EF_mu4 here as I can't get the above line to work correctly.

            if "L2StarB" in self.config.L2Config.suffix:
                self.addEFSequence('EF_mu6_L2StarB',
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_tracks')
            else:
                self.addEFSequence('EF_mu6',
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_tracks')
                
            self.addEFSequence('EF_tracks',
                           [EFTrkMassFex_Jpsimumu(),
                            EFTrkMassHypo_Jpsimumu()],
                           'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')


    def defineSignatures(self):
        #L2 signatures
#        if self.config.L2Config.l2confirmmuon is not None:
#            self.addL2Signature([self.config.L2Config.l2confirmmuon])

        self.addL2Signature(['L2_stall'])
        self.addL2Signature(['L2_tracks'])
        self.addL2Signature(['L2_'])
        #EF signatures
#        if self.config.EFConfig.efconfirmmuon is not None:
#            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.L2Config.suffix:
            self.addEFSignature(['EF_tracks'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_stall' : mergeRemovingOverlap('L2_', self.config.L2Config.suffix+'_stall'),
            'L2_tracks': mergeRemovingOverlap('L2_', self.config.L2Config.suffix+'_id'),
            'L2_':       mergeRemovingOverlap('L2_', self.config.L2Config.suffix),
            'EF_tracks': mergeRemovingOverlap('EF_', self.config.EFConfig.suffix+'_id'),
            'EF_':       mergeRemovingOverlap('EF_', self.config.EFConfig.suffix),
            }
        pass

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']
###########################################################################
#TrkMassRoI Loose mass cuts
class L2EFChain_TrkMassLooseRoI(L2EFChain_TrkMassRoI):

  def defineSequences(self, config):
        L2EFChain_TrkMassRoI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_loose()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_loose()
        else:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_loose_passL2()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_loose_passL2()

        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[1].algos[0] = EFTrkMassFex_Jpsimumu_loose()
            self.efchain.sequences[1].algos[1] = EFTrkMassHypo_Jpsimumu_loose()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkMassLooseRoI('mu4_Trk_Jpsi_loose',
                                  'L2_mu4_Trk_Jpsi_loose', 924, 'L1_MU0','EF_mu4_Trk_Jpsi_loose', 924, ['L2_mu4'],
                                  Config(L2create('_mu4_Trk_Jpsi_loose'),
                                         EFcreate('_mu4_Trk_Jpsi_loose',TEs='EF_mu4'))),
        ]
###########################################################################
#TrkMassRoI Loose mass cuts
class L2EFChain_TrkMassLooseRoI6(L2EFChain_TrkMassRoI6):

  def defineSequences(self, config):
        L2EFChain_TrkMassRoI6.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_loose()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_loose()
        else:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_loose_passL2()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_loose_passL2()

        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[1].algos[0] = EFTrkMassFex_Jpsimumu_loose()
            self.efchain.sequences[1].algos[1] = EFTrkMassHypo_Jpsimumu_loose()
        pass

Bphysics += [
    L2EFChain_TrkMassLooseRoI6('mu6_Trk_Jpsi_loose',
                         'L2_mu6_Trk_Jpsi_loose', 742, 'L1_MU6','EF_mu6_Trk_Jpsi_loose', 742, ['L2_mu6'],
                         Config(L2create('_mu6_Trk_Jpsi_loose'),
                                EFcreate('_mu6_Trk_Jpsi_loose',TEs='EF_mu6'))),

    L2EFChain_TrkMassLooseRoI6('mu6_Trk_Jpsi_loose_L2StarA',
                               'L2_mu6_Trk_Jpsi_loose_L2StarA', 721, 'L1_MU6','EF_mu6_Trk_Jpsi_loose_L2StarA', 721, ['L2_mu6'],
                               Config(L2create('_mu6_Trk_Jpsi_loose_L2StarA'),
                                      EFcreate('_mu6_Trk_Jpsi_loose_L2StarA',TEs='EF_mu6'))),

    L2EFChain_TrkMassLooseRoI6('mu6_Trk_Jpsi_loose_L2StarB',
                               'L2_mu6_Trk_Jpsi_loose_L2StarB', 197, 'L1_MU6','EF_mu6_Trk_Jpsi_loose_L2StarB', 197, ['L2_mu6_L2StarB'],
                               Config(L2create('_mu6_Trk_Jpsi_loose_L2StarB'),
                                      EFcreate('_mu6_Trk_Jpsi_loose_L2StarB',TEs='EF_mu6_L2StarB'))),

    ]
###########################################################################
#TrkMassRoI Looser mass cuts
class L2EFChain_TrkMassLooserRoI(L2EFChain_TrkMassRoI):
  def defineSequences(self, config):
        L2EFChain_TrkMassRoI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_looser()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_looser()
        else:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Jpsimumu_looser_passL2()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Jpsimumu_looser_passL2()

        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[1].algos[0] = EFTrkMassFex_Jpsimumu_looser()
            self.efchain.sequences[1].algos[1] = EFTrkMassHypo_Jpsimumu_looser()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkMassLooserRoI('mu4_Trk_Jpsi_looser',
                                   'L2_mu4_Trk_Jpsi_looser', 722, 'L1_MU0','EF_mu4_Trk_Jpsi_looser', 722, ['L2_mu4'],
                                   Config(L2create('_mu4_Trk_Jpsi_looser'),
                                          EFcreate('_mu4_Trk_Jpsi_looser',TEs='EF_mu4'))),
        ]
###########################################################################
#TrkMassFS
class L2EFChain_TrkMassFS(L2EFChain_Bphysics):
    def defineSequences(self, config):
        from TriggerMenuPython.BphysicsSliceFlags import BphysicsSliceFlags
        #L2 sequences
        if "SiTrk" in self.config.L2Config.suffix:
            self.addL2Sequence("",
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigSiTrack_FullScan()],
                               'L2_FStracks')
        else:
            self.addL2Sequence("",
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigIDSCAN_FullScan()],
                               'L2_FStracks')
            
        self.addL2Sequence(self.inputTEs,
                           [PESA__DummyCombineAlgo("L2_comb_mu_FStracks")],
                           'L2_comb')

        if not "passL2" in self. config.L2Config.suffix:
            self.addL2Sequence('L2_comb',
                           [L2TrkMassFex_Jpsimumu_FS(),
                            L2TrkMassHypo_Jpsimumu_FS()],
                           'L2_')

        else:
            self.addL2Sequence('L2_comb',
                               [L2TrkMassFex_Jpsimumu_FS_passL2(),
                                L2TrkMassHypo_Jpsimumu_FS_passL2()],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSequence("",
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                               TrigEFIDInsideOut_FullScan().getSequence(),
                               'EF_FStracks')

#            if not "passL2" in self. config.L2Config.suffix:
##            self.addEFSequence(['L2_']+['EF_FStracks'],
            self.addEFSequence([mu4sequence]+['EF_FStracks'],
                               [PESA__DummyCombineAlgo("EF_comb_mu_FStracks")],
                               'EF_comb')

            self.addEFSequence('EF_comb',
                           [EFTrkMassFex_Jpsimumu_FS(),
                            EFTrkMassHypo_Jpsimumu_FS()],
                           'EF_')

        else: # we need a dummy seq
            self.addEFSequence("",
                               [],
                               'EF_')


    def defineSignatures(self):
         #L2 signatures
        if self.config.L2Config.l2confirmmuon is not None:
            self.addL2Signature([self.config.L2Config.l2confirmmuon])
#        if not "passL2" in self.config.L2Config.suffix:
        self.addL2Signature(['L2_FStracks'])
        self.addL2Signature(['L2_comb'])
        self.addL2Signature(['L2_'])
        #EF signatures
        if self.config.EFConfig.efconfirmmuon is not None:
            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_comb'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_comb': mergeRemovingOverlap('L2_', self.config.L2Config.suffix+'_id_FS'),
            'L2_':       mergeRemovingOverlap('L2_', self.config.L2Config.suffix),
            'EF_comb': mergeRemovingOverlap('EF_', self.config.EFConfig.suffix+'_id_FS'),
            'EF_':       mergeRemovingOverlap('EF_', self.config.EFConfig.suffix),
            }
        if 'SiTrk' in self.config.L2Config.suffix:
            self.TErenamingMap['L2_FStracks'] = 'L2_FStracks_SiTrk'
        pass

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']

###########################################################################
#TrkMassFS Upsi mass cuts
class L2EFChain_TrkUpsiMassFS(L2EFChain_TrkMassFS):

  def defineSequences(self, config):
        L2EFChain_TrkMassFS.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Upsimumu_FS()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Upsimumu_FS()
        else :
            self.l2chain.sequences[2].algos[0] = L2TrkMassFex_Upsimumu_FS_passL2()
            self.l2chain.sequences[2].algos[1] = L2TrkMassHypo_Upsimumu_FS_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[2].algos[0] = EFTrkMassFex_Upsimumu_FS()
            self.efchain.sequences[2].algos[1] = EFTrkMassHypo_Upsimumu_FS()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkUpsiMassFS('mu4_Trk_Upsi_FS',
                                'L2_mu4_Trk_Upsi_FS', 710, 'L1_MU0','EF_mu4_Trk_Upsi_FS', 710, ['L2_mu4','L2_FStracks'],
                                Config(L2create('_mu4_Trk_Upsi_FS'),
                                       EFcreate('_mu4_Trk_Upsi_FS'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TrkUpsiMassFS('mu4T_Trk_Upsi_FS',
                                'L2_mu4T_Trk_Upsi_FS', 710, 'L1_MU4','EF_mu4T_Trk_Upsi_FS', 710, ['L2_mu4T','L2_FStracks'],
                                Config(L2create('_mu4T_Trk_Upsi_FS'),
                                       EFcreate('_mu4T_Trk_Upsi_FS'))),    
        
        ]
###########################################################################
#NEXT CLASSES ARE ALL MUON FS TYPE
###########################################################################
#MuonROIFS_JPSI
class L2EFChain_MuonROIFS_JPSI(L2EFChain_Bphysics):

    def defineSequences(self, config):
        #L2 sequences

        if "L2StarA" in self.config.L2Config.suffix:
            self.addL2Sequence('',
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigL2SiTrackFinder_FullScanA()],
                               'L2_FStracks')
            self.addL2Sequence(self.inputTEs+['L2_FStracks'],
                               [PESA__DummyCombineAlgo("L2_comb_mu_FStracks")],
                               'L2_comb')
            
        elif "FTK" in self.config.L2Config.suffix:
            self.addL2Sequence('',
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigL2SiTrackFinder_FullScanF()],
                               'L2_FStracks')

            self.addL2Sequence(self.inputTEs+['L2_FStracks'],
                               [PESA__DummyCombineAlgo("L2_comb_mu_FStracks")],
                               'L2_comb')
        
        else:
            self.addL2Sequence('',
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigIDSCAN_FullScan()],
                               'L2_FStracks')

            self.addL2Sequence(self.inputTEs+['L2_FStracks'],
                               [PESA__DummyCombineAlgo("L2_comb_mu_FStracks")],
                               'L2_comb')
        
        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence('L2_comb',
                               [ TrigDiMuon_FS(),
                                 TrigL2DiMuHypo_Jpsi()],
                               'L2_')

        else: 
            self.addL2Sequence('L2_comb',
                               [ TrigDiMuon_FS(),
                                 TrigL2DiMuHypo_Jpsi_passL2()],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:

            self.addEFSequence("",
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo"),
                                CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                                TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV', '0GeV')],
                               'EF_SA_FS')
            self.addEFSequence('EF_SA_FS',
                               [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                               'EF_SAR_FS')
            self.addEFSequence('EF_SAR_FS',
                               TrigEFIDInsideOut_Muon().getSequence(),
                               'EF_FStracksMuon')
            self.addEFSequence('EF_FStracksMuon',
                               [TrigMuonEFCombinerConfig('TrigMuonEFCombinerPerROI')],
                               #can be added to reduce combinatorics in the aggregator, but makes comparisons to old setup harder
                               #TrigMuonEFCombinerHypoConfig('TrigMuonEFCombinerHypoForFS','8GeV')],
                               'EF_CB_FS_single')
                               # hypo code using the RoiAggregator
            self.addEFSequence('EF_CB_FS_single',
                               [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                TrigMuonEFCombinerHypo()],
                               'EF_CB')

            self.addEFSequence('EF_CB',
                               [TrigEFDiMuFexConfig("FS"),
                                TrigEFDiMuHypo_Jpsi()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence("",
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if self.config.L2Config.l2confirmmuon is not None:
            self.addL2Signature([self.config.L2Config.l2confirmmuon])
        #if not "passL2" in self.config.L2Config.suffix:
        #if "L2StarA" in self.config.L2Config.suffix:
        #    self.addL2Signature(['L2_FStracks_L2StarA'])
        #else:
        self.addL2Signature(['L2_FStracks'])
        self.addL2Signature(['L2_comb'])
        self.addL2Signature(['L2_'])
        #EF signatures
        if self.config.EFConfig.efconfirmmuon is not None:
            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_CB'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = { 
            'L2_FStracks': 'L2_FStracks',
            'L2_comb':     'L2_'+self.config.L2Config.suffix+'_FStracks',
            'L2_':         'L2_'+self.config.L2Config.suffix,
            'EF_FStracks': 'EF_FStracks',
 #           'EF_comb':     'EF_'+self.config.EFConfig.suffix+'_FStracks',
            'EF_MS':       'EF_'+self.config.EFConfig.suffix+'_MS',
            'EF_MT':       'EF_'+self.config.EFConfig.suffix+'_MT',
            'EF_SA':       'EF_'+self.config.EFConfig.suffix+'_SA',
            'EF_CB':       'EF_'+self.config.EFConfig.suffix+'_CB',
            'EF_':         'EF_'+self.config.EFConfig.suffix,
            }
        if 'SiTrk' in self.config.L2Config.suffix: #here we modify the dictionary
            self.TErenamingMap['L2_FStracks'] = 'L2_FStracks_SiTrk'
        elif "L2StarA" in self.config.L2Config.suffix:
            self.TErenamingMap['L2_FStracks'] = 'L2_FStracks_L2StarA'
        elif "FTK" in self.config.L2Config.suffix:
            self.TErenamingMap['L2_FStracks'] = 'L2_FStracks_FTK'
        pass

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']
        
Bphysics += [
    L2EFChain_MuonROIFS_JPSI('mu4_Jpsimumu_FS',
                        'L2_mu4_Jpsimumu_FS', 326, 'L1_MU0', 'EF_mu4_Jpsimumu_FS', 326, ['MU0'],
                         Config(L2create('mu4_Jpsimumu_FS',confirmmuon='L2_mu4'),
                                EFcreate('mu4_Jpsimumu_FS',confirmmuon='EF_mu4'))),
    ]
###################################################################################
#MuonRoIFS_UPSI - inherits from L2EFChain_MuonROIFS_JPSI above - just swap the hypos
class L2EFChain_MuonROIFS_UPSI(L2EFChain_MuonROIFS_JPSI):
  def defineSequences(self, config):
        L2EFChain_MuonROIFS_JPSI.defineSequences(self, config)
        if "tight" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_Upsi_tight()
        elif not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_Upsi()
        else:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_Upsi_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[5].algos[1] = TrigEFDiMuHypo_Upsi()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [    
        L2EFChain_MuonROIFS_UPSI('mu4_Upsimumu_FS',
                                 'L2_mu4_Upsimumu_FS', 328, 'L1_MU0', 'EF_mu4_Upsimumu_FS', 328, ['MU0'],
                                 Config(L2create('mu4_Upsimumu_FS',confirmmuon='L2_mu4'),
                                        EFcreate('mu4_Upsimumu_FS',confirmmuon='EF_mu4'))),
        L2EFChain_MuonROIFS_UPSI('mu4_Upsimumu_tight_FS',
                                 'L2_mu4_Upsimumu_tight_FS', 449, 'L1_MU0', 'EF_mu4_Upsimumu_tight_FS', 449, ['MU0'],
                                Config(L2create('mu4_Upsimumu_tight_FS',confirmmuon='L2_mu4'),
                                        EFcreate('mu4_Upsimumu_tight_FS',confirmmuon='EF_mu4'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROIFS_UPSI('mu4T_Upsimumu_FS',
                                 'L2_mu4T_Upsimumu_FS', 328, 'L1_MU4', 'EF_mu4T_Upsimumu_FS', 328, ['MU4'],
                                 Config(L2create('mu4T_Upsimumu_FS',confirmmuon='L2_mu4T'),
                                        EFcreate('mu4T_Upsimumu_FS',confirmmuon='EF_mu4T'))),    
        
        L2EFChain_MuonROIFS_UPSI('mu4T_Upsimumu_tight_FS',
                                 'L2_mu4T_Upsimumu_tight_FS', 449, 'L1_MU4', 'EF_mu4T_Upsimumu_tight_FS', 449, ['MU4'],
                                 Config(L2create('mu4T_Upsimumu_tight_FS',confirmmuon='L2_mu4T'),
                                        EFcreate('mu4T_Upsimumu_tight_FS',confirmmuon='EF_mu4T'))),    
        ]
Bphysics += [
    L2EFChain_MuonROIFS_UPSI('mu6_Upsimumu_FS',
                        'L2_mu6_Upsimumu_FS', 329, mu6Seed, 'EF_mu6_Upsimumu_FS', 329, [mu6Thr],
                         Config(L2create('mu6_Upsimumu_FS',confirmmuon='L2_mu6'),
                                EFcreate('mu6_Upsimumu_FS',confirmmuon='EF_mu6'))),
    L2EFChain_MuonROIFS_UPSI('mu6_Upsimumu_tight_FS',
                        'L2_mu6_Upsimumu_tight_FS', 448, mu6Seed, 'EF_mu6_Upsimumu_tight_FS', 448, [mu6Thr],
                         Config(L2create('mu6_Upsimumu_tight_FS',confirmmuon='L2_mu6'),
                                EFcreate('mu6_Upsimumu_tight_FS',confirmmuon='EF_mu6'))),
    L2EFChain_MuonROIFS_UPSI('mu10_Upsimumu_FS',
                        'L2_mu10_Upsimumu_FS', 355, 'L1_MU10', 'EF_mu10_Upsimumu_FS', 355, ['MU10'],
                         Config(L2create('mu10_Upsimumu_FS',confirmmuon='L2_mu10'),
                                EFcreate('mu10_Upsimumu_FS',confirmmuon='EF_mu10'))),
    L2EFChain_MuonROIFS_UPSI('mu10_Upsimumu_tight_FS',
                             'L2_mu10_Upsimumu_tight_FS', 437, 'L1_MU10', 'EF_mu10_Upsimumu_tight_FS', 437, ['MU10'],
                             Config(L2create('mu10_Upsimumu_tight_FS',confirmmuon='L2_mu10'),
                                    EFcreate('mu10_Upsimumu_tight_FS',confirmmuon='EF_mu10'))),

    L2EFChain_MuonROIFS_UPSI('mu10_Upsimumu_tight_FS_L2StarA',
                             'L2_mu10_Upsimumu_tight_FS_L2StarA', 36, 'L1_MU10', 'EF_mu10_Upsimumu_tight_FS_L2StarA', 36, ['MU10'],
                             Config(L2create('mu10_Upsimumu_tight_FS_L2StarA',confirmmuon='L2_mu10'),
                                    EFcreate('mu10_Upsimumu_tight_FS_L2StarA',confirmmuon='EF_mu10'))),

]
###################################################################################
#MuonRoIFS_DIMU - inherits from L2EFChain_MuonROIFS_JPSI above - just swap the hypos
class L2EFChain_MuonROIFS_DIMU(L2EFChain_MuonROIFS_JPSI):
  def defineSequences(self, config):
        L2EFChain_MuonROIFS_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_DiMu()
        else:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_DiMu_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[5].algos[1] = TrigEFDiMuHypo_DiMu()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROIFS_DIMU('mu4_DiMu_FS',
                                 'L2_mu4_DiMu_FS', 597, 'L1_MU0', 'EF_mu4_DiMu_FS', 597, ['MU0'],
                                 Config(L2create('mu4_DiMu_FS',confirmmuon='L2_mu4'),
                                        EFcreate('mu4_DiMu_FS',confirmmuon='EF_mu4'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROIFS_DIMU('mu4T_DiMu_FS',
                                 'L2_mu4T_DiMu_FS', 597, 'L1_MU4', 'EF_mu4T_DiMu_FS', 597, ['MU4'],
                                 Config(L2create('mu4T_DiMu_FS',confirmmuon='L2_mu4T'),
                                        EFcreate('mu4T_DiMu_FS',confirmmuon='EF_mu4T'))),
        ]

##FTK Full Scan
Bphysics += [
    L2EFChain_MuonROIFS_DIMU('mu6_DiMu_FS_FTK',
                             'L2_mu6_DiMu_FS_FTK', 7010, mu6Seed, 'EF_mu6_DiMu_FS_FTK', 7010, ['L2_mu6_FTK'],
                             Config(L2create('mu6_DiMu_FS_FTK',confirmmuon='L2_mu6_FTK'),
                                    EFcreate('mu6_DiMu_FS_FTK',confirmmuon='EF_mu6_FTK'))),
    
]

###################################################################################
class L2EFChain_MuonROIFS_UPSI_SITRK(L2EFChain_MuonROIFS_UPSI):
  def defineSequences(self, config):
        L2EFChain_MuonROIFS_UPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[1] = TrigSiTrack_FullScan()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROIFS_UPSI_SITRK('mu4_Upsimumu_SiTrk_FS',
                                       'L2_mu4_Upsimumu_SiTrk_FS', 313, 'L1_MU0', 'EF_mu4_Upsimumu_SiTrk_FS', 313, ['MU0'],
                                       Config(L2create('mu4_Upsimumu_SiTrk_FS',confirmmuon='L2_mu4'),
                                              EFcreate('mu4_Upsimumu_SiTrk_FS',confirmmuon='EF_mu4'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics +=  [
        L2EFChain_MuonROIFS_UPSI_SITRK('mu4T_Upsimumu_SiTrk_FS',
                                       'L2_mu4T_Upsimumu_SiTrk_FS', 313, 'L1_MU4', 'EF_mu4T_Upsimumu_SiTrk_FS', 313, ['MU4'],
                                       Config(L2create('mu4T_Upsimumu_SiTrk_FS',confirmmuon='L2_mu4T'),
                                              EFcreate('mu4T_Upsimumu_SiTrk_FS',confirmmuon='EF_mu4T'))),    
        ]
###################################################################################
#MuonRoIFS_DIMU_noOS - inherits from L2EFChain_MuonROIFS_JPSI above - just swap the hypos
class L2EFChain_MuonROIFS_DIMU_NOOS(L2EFChain_MuonROIFS_JPSI):
  def defineSequences(self, config):
        L2EFChain_MuonROIFS_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_DiMu_noOS()
        else:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_DiMu_noOS_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[5].algos[1] = TrigEFDiMuHypo_DiMu_noOS()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        #mu4_DiMu_FS_noOS
    L2EFChain_MuonROIFS_DIMU_NOOS('mu4_DiMu_FS_noOS',
                        'L2_mu4_DiMu_FS_noOS', 928, 'L1_MU0', 'EF_mu4_DiMu_FS_noOS', 928, ['MU0'],
                         Config(L2create('mu4_DiMu_FS_noOS',confirmmuon='L2_mu4'),
                                EFcreate('mu4_DiMu_FS_noOS',confirmmuon='EF_mu4'))),
    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
    L2EFChain_MuonROIFS_DIMU_NOOS('mu4T_DiMu_FS_noOS',
                        'L2_mu4T_DiMu_FS_noOS', 928, 'L1_MU4', 'EF_mu4T_DiMu_FS_noOS', 928, ['MU4'],
                         Config(L2create('mu4T_DiMu_FS_noOS',confirmmuon='L2_mu4T'),
                                EFcreate('mu4T_DiMu_FS_noOS',confirmmuon='EF_mu4T'))),
    ]
###################################################################################
#MuonRoIFS_DIMU_SITRK - inherits from MuonRoIFS_DIMU above, just swap IDSCAN
class L2EFChain_MuonROIFS_DIMU_SITRK(L2EFChain_MuonROIFS_DIMU):

  def defineSequences(self, config):
        L2EFChain_MuonROIFS_DIMU.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[1] = TrigSiTrack_FullScan()
        pass

Bphysics += [
    L2EFChain_MuonROIFS_DIMU_SITRK('mu4_DiMu_SiTrk_FS',
                        'L2_mu4_DiMu_SiTrk_FS', 910, 'L1_MU0', 'EF_mu4_DiMu_SiTrk_FS', 910, ['MU0'],
                         Config(L2create('mu4_DiMu_SiTrk_FS',confirmmuon='L2_mu4'),
                                EFcreate('mu4_DiMu_SiTrk_FS',confirmmuon='EF_mu4'))),
    ]
###########################################################################
#NEXT CLASSES ARE ALL MUON ROI TYPE
###########################################################################
#MuonROI_JPSI
class L2EFChain_MuonROI_JPSI(L2EFChain_Bphysics):
    def defineSequences(self, config):

        #L2 sequences
        self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_stall_')

        if not "passL2" in self.config.L2Config.suffix:
           if "L2StarA" in self.config.L2Config.suffix:
               self.addL2Sequence('L2_stall_',
                                  [TrigL2SiTrackFinder_BphysicsA(),
                                   TrigDiMuon_RoI(),
                                   TrigL2DiMuHypo_Jpsi_tight()],
                                  'L2_')
               
           elif "L2StarB" in self.config.L2Config.suffix:
               self.addL2Sequence('L2_stall_',
                                  [TrigL2SiTrackFinder_BphysicsB(),
                                   TrigDiMuon_RoI(),
                                   TrigL2DiMuHypo_Jpsi_tight()],
                                  'L2_')
           elif "tight" in self.config.L2Config.suffix:
               self.addL2Sequence('L2_stall_',
                                  [TrigIDSCAN_Bphysics(),
                                   TrigDiMuon_RoI(),
                                   TrigL2DiMuHypo_Jpsi_tight()],
                                  'L2_')
               
           elif "SiTrk" in self.config.L2Config.suffix:
               self.addL2Sequence('L2_stall_',
                                  [TrigSiTrack_Bphysics(),
                                   TrigDiMuon_RoI(),
                                   TrigL2DiMuHypo_Jpsi()],
                                  'L2_')



           else:
               self.addL2Sequence('L2_stall_',
                                  [TrigIDSCAN_Bphysics(),
                                   TrigDiMuon_RoI(),
                                   TrigL2DiMuHypo_Jpsi()],
                                  'L2_')

        else: # we need a dummy seq
            if "SiTrk" in self.config.L2Config.suffix:
                self.addL2Sequence('L2_stall_',                               
                                   [TrigSiTrack_Bphysics(),
                                    TrigDiMuon_RoI(),
                                    TrigL2DiMuHypo_Jpsi_passL2()],
                                   'L2_')

            elif "L2StarA" in self.config.L2Config.suffix:
                self.addL2Sequence('L2_stall_',
                                   [TrigL2SiTrackFinder_BphysicsA(),
                                    TrigDiMuon_RoI(),
                                    TrigL2DiMuHypo_Jpsi_passL2()],
                                   'L2_')
                                
            else:
                self.addL2Sequence('L2_stall_',                               
                                   [TrigIDSCAN_Bphysics(),
                                    TrigDiMuon_RoI(),
                                    TrigL2DiMuHypo_Jpsi_passL2()],
                                   'L2_')

        
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
#            if not "passL2" in self.config.L2Config.suffix:
            self.addEFSequence('L2_',
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_id')

            self.addEFSequence('EF_id',
                               [TrigMuSuperEF_Bphys(),
                                TrigMuonEFCombinerHypo()],
                               'EF_CB')

            self.addEFSequence('EF_CB',
                               [TrigEFDiMuFexConfig("1"),
                                TrigEFDiMuHypo_Jpsi()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if self.config.L2Config.l2confirmmuon is not None:
            self.addL2Signature([self.config.L2Config.l2confirmmuon])
        #if not "passL2" in self.config.L2Config.suffix:
        self.addL2Signature(['L2_stall_'])
        self.addL2Signature(['L2_'])
        #EF signatures
        #if self.config.EFConfig.efconfirmmuon is not None:
        #    self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.L2Config.suffix:
            self.addEFSignature(['EF_id'])
            self.addEFSignature(['EF_CB'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_stall_':   'L2_stall_'+self.config.L2Config.suffix,
            'L2_':   'L2_'+self.config.L2Config.suffix,
            'EF_id': 'EF_'+self.config.EFConfig.suffix+'_id',
            'EF_MS': 'EF_'+self.config.EFConfig.suffix+'_MS',
            'EF_MT': 'EF_'+self.config.EFConfig.suffix+'_MT',
            'EF_SA': 'EF_'+self.config.EFConfig.suffix+'_SA',
            'EF_CB': 'EF_'+self.config.EFConfig.suffix+'_CB',
            'EF_':   'EF_'+self.config.EFConfig.suffix,
            }

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']

Bphysics += [
    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu',
                           'L2_mu6_Jpsimumu', 191, mu6Seed, 'EF_mu6_Jpsimumu', 191, ['L2_mu6'],
                           Config(L2create('mu6_Jpsimumu',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_Jpsimumu',confirmmuon='EF_mu6'))),

    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_L2StarA',
                           'L2_mu6_Jpsimumu_L2StarA', 185, mu6Seed, 'EF_mu6_Jpsimumu_L2StarA', 185, ['L2_mu6'],
                           Config(L2create('mu6_Jpsimumu_L2StarA',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_Jpsimumu_L2StarA',confirmmuon='EF_mu6'))),

    L2EFChain_MuonROI_JPSI('mu4_Jpsimumu_tight',
                           'L2_mu4_Jpsimumu_tight', 450, 'L1_MU0', 'EF_mu4_Jpsimumu_tight', 450, ['L2_mu4'],
                           Config(L2create('mu4_Jpsimumu_tight',confirmmuon='L2_mu4'),
                                  EFcreate('mu4_Jpsimumu_tight',confirmmuon='EF_mu4'))),
    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_tight',
                           'L2_mu6_Jpsimumu_tight', 451, mu6Seed, 'EF_mu6_Jpsimumu_tight', 451, ['L2_mu6'],
                           Config(L2create('mu6_Jpsimumu_tight',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_Jpsimumu_tight',confirmmuon='EF_mu6'))),

    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_tight_L2StarB',
                           'L2_mu6_Jpsimumu_tight_L2StarB', 285, mu6Seed, 'EF_mu6_Jpsimumu_tight_L2StarB', 285, ['L2_mu6_L2StarB'],
                           Config(L2create('mu6_Jpsimumu_tight_L2StarB',confirmmuon='L2_mu6_L2StarB'),
                                  EFcreate('mu6_Jpsimumu_tight_L2StarB',confirmmuon='EF_mu6_L2StarB'))),


    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_SiTrk',
                           'L2_mu6_Jpsimumu_SiTrk', 212, mu6Seed, 'EF_mu6_Jpsimumu_SiTrk', 212, ['L2_mu6'],
                           Config(L2create('mu6_Jpsimumu_SiTrk',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_Jpsimumu_SiTrk',confirmmuon='EF_mu6'))),

    L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_tight_L2StarA',
                           'L2_mu6_Jpsimumu_tight_L2StarA', 720, mu6Seed, 'EF_mu6_Jpsimumu_tight_L2StarA', 720, ['L2_mu6'],
                           Config(L2create('mu6_Jpsimumu_tight_L2StarA',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_Jpsimumu_tight_L2StarA',confirmmuon='EF_mu6'))),

 #   L2EFChain_MuonROI_JPSI('mu6_Jpsimumu_tight_L2StarB',
 #                          'L2_mu6_Jpsimumu_tight_L2StarB', 1720, mu6Seed, 'EF_mu6_Jpsimumu_tight_L2StarB', 1720, ['L2_mu6_L2StarB'],
 #                          Config(L2create('mu6_Jpsimumu_tight_L2StarB',confirmmuon='L2_mu6_L2StarB'),
 #                                 EFcreate('mu6_Jpsimumu_tight_L2StarB',confirmmuon='EF_mu6_L2StarB'))),
    

    L2EFChain_MuonROI_JPSI('mu10_Jpsimumu',
                        'L2_mu10_Jpsimumu', 343, 'L1_MU10', 'EF_mu10_Jpsimumu', 343, ['MU10'],
                         Config(L2create('mu10_Jpsimumu',confirmmuon='L2_mu10'),
                                EFcreate('mu10_Jpsimumu',confirmmuon='EF_mu10'))),

    L2EFChain_MuonROI_JPSI('mu10_Jpsimumu_L2StarA',
                        'L2_mu10_Jpsimumu_L2StarA', 45, 'L1_MU10', 'EF_mu10_Jpsimumu_L2StarA', 45, ['MU10'],
                         Config(L2create('mu10_Jpsimumu_L2StarA',confirmmuon='L2_mu10'),
                                EFcreate('mu10_Jpsimumu_L2StarA',confirmmuon='EF_mu10'))),

    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [    
        #mu4_Jpsimumu
        L2EFChain_MuonROI_JPSI('mu4_Jpsimumu',
                               'L2_mu4_Jpsimumu', 190, 'L1_MU0', 'EF_mu4_Jpsimumu', 190, ['L2_mu4'],
                               Config(L2create('mu4_Jpsimumu',confirmmuon='L2_mu4'),
                                      EFcreate('mu4_Jpsimumu',confirmmuon='EF_mu4'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROI_JPSI('mu4T_Jpsimumu',
                               'L2_mu4T_Jpsimumu', 190, 'L1_MU4', 'EF_mu4T_Jpsimumu', 190, ['L2_mu4T'],
                               Config(L2create('mu4T_Jpsimumu',confirmmuon='L2_mu4T'),
                                      EFcreate('mu4T_Jpsimumu',confirmmuon='EF_mu4T'))),
        
        ]
###################################################################################
#MuonRoI_BMUMU - inherits from L2EFChain_MuonROI_JPSI above - just swap the hypos
class L2EFChain_MuonROI_BMUMU(L2EFChain_MuonROI_JPSI):

  def defineSequences(self, config):
        L2EFChain_MuonROI_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_B()
        else:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_B_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[-1].algos[-1] = TrigEFDiMuHypo_B()
        pass

Bphysics += [
    L2EFChain_MuonROI_BMUMU('mu4_Bmumu',
                            'L2_mu4_Bmumu', 194, 'L1_MU0', 'EF_mu4_Bmumu', 194, ['L2_mu4'],
                            Config(L2create('mu4_Bmumu',confirmmuon='L2_mu4'),
                                   EFcreate('mu4_Bmumu',confirmmuon='EF_mu4'))),
    L2EFChain_MuonROI_BMUMU('mu6_Bmumu',
                            'L2_mu6_Bmumu', 195, mu6Seed, 'EF_mu6_Bmumu', 195, ['L2_mu6'],
                            Config(L2create('mu6_Bmumu',confirmmuon='L2_mu6'),
                                   EFcreate('mu6_Bmumu',confirmmuon='EF_mu6'))),
    ]
###################################################################################
#MuonRoI_DIMU - inherits from L2EFChain_MuonROI_JPSI above - just swap the hypos
class L2EFChain_MuonROI_DIMU(L2EFChain_MuonROI_JPSI):

  def defineSequences(self, config):
        L2EFChain_MuonROI_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_DiMu()
        else:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_DiMu_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[-1].algos[-1] = TrigEFDiMuHypo_DiMu()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        #mu4_DiMu
        L2EFChain_MuonROI_DIMU('mu4_DiMu',
                               'L2_mu4_DiMu', 602, 'L1_MU0', 'EF_mu4_DiMu', 602, ['L2_mu4'],
                               Config(L2create('mu4_DiMu',confirmmuon='L2_mu4'),
                                      EFcreate('mu4_DiMu',confirmmuon='EF_mu4'))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_MuonROI_DIMU('mu4T_DiMu',
                               'L2_mu4T_DiMu', 602, 'L1_MU4', 'EF_mu4T_DiMu', 602, ['L2_mu4T'],
                               Config(L2create('mu4T_DiMu',confirmmuon='L2_mu4T'),
                                      EFcreate('mu4T_DiMu',confirmmuon='EF_mu4T'))),
        ]
Bphysics += [
    L2EFChain_MuonROI_DIMU('mu6_DiMu',
                           'L2_mu6_DiMu', 603, mu6Seed, 'EF_mu6_DiMu', 603, ['L2_mu6'],
                           Config(L2create('mu6_DiMu',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_DiMu',confirmmuon='EF_mu6'))),
]
###################################################################################
#MuonRoI_DIMU_noOS - inherits from L2EFChain_MuonROI_JPSI above - just swap the hypos
class L2EFChain_MuonROI_DIMU_NOOS(L2EFChain_MuonROI_JPSI):

  def defineSequences(self, config):
        L2EFChain_MuonROI_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_DiMu_noOS()
        else:
            self.l2chain.sequences[-1].algos[-1] = TrigL2DiMuHypo_DiMu_noOS_passL2()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[-1].algos[-1] = TrigEFDiMuHypo_DiMu_noOS()
        pass

Bphysics += [
    L2EFChain_MuonROI_DIMU_NOOS('mu4_DiMu_noOS',
                           'L2_mu4_DiMu_noOS', 907, 'L1_MU0', 'EF_mu4_DiMu_noOS', 907, ['L2_mu4'],
                           Config(L2create('mu4_DiMu_noOS',confirmmuon='L2_mu4'),
                                  EFcreate('mu4_DiMu_noOS',confirmmuon='EF_mu4'))),
    L2EFChain_MuonROI_DIMU_NOOS('mu6_DiMu_noOS',
                           'L2_mu6_DiMu_noOS', 908, 'L1_MU6', 'EF_mu6_DiMu_noOS', 908, ['L2_mu6'],
                           Config(L2create('mu6_DiMu_noOS',confirmmuon='L2_mu6'),
                                  EFcreate('mu6_DiMu_noOS',confirmmuon='EF_mu6'))),
]
###################################################################################
#MuonRoI_DIMU_SITRK - inherits from MuonRoI_DIMU above, just swap IDSCAN for SiTrack
class L2EFChain_MuonROI_DIMU_SITRK(L2EFChain_MuonROI_DIMU):

  def defineSequences(self, config):
        L2EFChain_MuonROI_DIMU.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[1].algos[0] = TrigSiTrack_Bphysics()
        pass

Bphysics += [
    L2EFChain_MuonROI_DIMU_SITRK('mu4_DiMu_SiTrk',
                                 'L2_mu4_DiMu_SiTrk', 911, 'L1_MU0', 'EF_mu4_DiMu_SiTrk', 911, ['MU0'],
                                 Config(L2create('mu4_DiMu_SiTrk',confirmmuon='L2_mu4'),
                                        EFcreate('mu4_DiMu_SiTrk',confirmmuon='EF_mu4'))),
    ]
###################################################################################
#MuonRoI_BMUMUX - inherits from L2EFChain_MuonROI_JPSI above, redefine l2 sequences
class L2EFChain_MuonROI_BMUMUX(L2EFChain_MuonROI_JPSI):

    def defineSequences(self, config):
        L2EFChain_MuonROI_JPSI.defineSequences(self, config)

        self.l2chain.sequences = []
        self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_stall_')
        if not "passL2" in self.config.L2Config.suffix:
#            self.addL2Sequence(self.inputTEs+['L2_stall'],
            self.addL2Sequence('L2_stall_',
                               [TrigIDSCAN_Bphysics(),
                                TrigDiMuon_RoI(),
                                TrigL2DiMuXFex_1(),
                                TrigL2DiMuXHypo_1()],
                               'L2_')

        else: # we need a dummy seq
            self.addL2Sequence('L2_stall_',
                               [],
                               'L2_')

#CRIS
        #EF sequences
        self.efchain.sequences = []
        if not "passEF" in self.config.EFConfig.suffix:
            if not "passL2" in self.config.L2Config.suffix:
                self.addEFSequence('L2_',
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_id')
            else: #seed from L1 threshold
                self.addEFSequence(self.inputTEs,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_id')

            self.addEFSequence('EF_id',
                               [TrigMuSuperEF_Bphys(),
                                TrigMuonEFCombinerHypo()],
                               'EF_CB')

            self.addEFSequence('EF_CB',
                               [TrigEFDiMuFexConfig("1"),
                                TrigEFDiMuXFex_1(),
                                TrigEFDiMuXHypo_1()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

        
Bphysics += [
    L2EFChain_MuonROI_BMUMUX('mu4_BmumuX',
                             'L2_mu4_BmumuX', 334, 'L1_MU0', 'EF_mu4_BmumuX', 334, ['L2_mu4'],
                             Config(L2create('mu4_BmumuX',confirmmuon='L2_mu4'),
                                    EFcreate('mu4_BmumuX',confirmmuon='EF_mu4'))),
    L2EFChain_MuonROI_BMUMUX('mu6_BmumuX',
                             'L2_mu6_BmumuX', 335, mu6Seed, 'EF_mu6_BmumuX', 335, ['L2_mu6'],
                             Config(L2create('mu6_BmumuX',confirmmuon='L2_mu6'),
                                    EFcreate('mu6_BmumuX',confirmmuon='EF_mu6'))),
    ]
###########################################################################
#NEXT CLASSES ARE ALL DIMU topological TYPE
#TopoDiMu_JPSI
class L2EFChain_TopoDiMu_JPSI(L2EFChain_Bphysics):

    def defineSequences(self, config):
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence(self.inputTEs,
                               [L2BMuMuFex_Jpsi(),
                                L2BMuMuHypo_Jpsi()],
                               'L2_')

        else: # we need a dummy seq
            self.addL2Sequence(self.inputTEs,
                               [L2BMuMuFex_Jpsi_passL2(),
                                L2BMuMuHypo_Jpsi_passL2()],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            #set some properties if required
            self.addEFSequence(self.config.EFConfig.efTEs,
                               [EFBMuMuFex_Jpsi(),EFBMuMuHypo_Jpsi()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')


    def defineSignatures(self):
        #L2 signatures
#        if not "passL2" in self.config.L2Config.suffix:
        self.addL2Signature(self.inputTEs)
        self.addL2Signature(['L2_'])
        #EF signatures
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSignature(self.config.EFConfig.efTEs)
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':   'L2_'+self.config.L2Config.suffix,
            'EF_':   'EF_'+self.config.EFConfig.suffix,
            }

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_MultiMuon','BW:Bphys']

        self.physics_streams = ['bphysics']            
        self.calib_streams = []
        self.trigger_type = ['hello']

Bphysics += [

    L2EFChain_TopoDiMu_JPSI('2mu6_Jpsimumu',
                             'L2_2mu6_Jpsimumu', 897, 'L1_2MU6', 'EF_2mu6_Jpsimumu', 897, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_Jpsimumu'),
                                    EFcreate('2mu6_Jpsimumu',TEs=['EF_mu6','EF_mu6']))),


    L2EFChain_TopoDiMu_JPSI('2mu6_Jpsimumu_L2StarB',
                             'L2_2mu6_Jpsimumu_L2StarB', 383, 'L1_2MU6', 'EF_2mu6_Jpsimumu_L2StarB', 383, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                             Config(L2create('2mu6_Jpsimumu_L2StarB'),
                                    EFcreate('2mu6_Jpsimumu_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_JPSI('mu4mu6_Jpsimumu',
                            'L2_mu4mu6_Jpsimumu', 380, mu4mu6Seed, 'EF_mu4mu6_Jpsimumu', 380, ['L2_mu4','L2_mu6'],
                            Config(L2create('mu4mu6_Jpsimumu'),
                                   EFcreate('mu4mu6_Jpsimumu',TEs=['EF_mu4','EF_mu6']))),

    L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu',
                            'L2_mu4Tmu6_Jpsimumu', 380, mu4Tmu6Seed, 'EF_mu4Tmu6_Jpsimumu', 380, ['L2_mu4T','L2_mu6'],
                            Config(L2create('mu4Tmu6_Jpsimumu'),
                                   EFcreate('mu4Tmu6_Jpsimumu',TEs=['EF_mu4T','EF_mu6']))),


    L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_L2StarB',
                            'L2_mu4Tmu6_Jpsimumu_L2StarB', 377, mu4Tmu6Seed, 'EF_mu4Tmu6_Jpsimumu_L2StarB', 377, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                            Config(L2create('mu4Tmu6_Jpsimumu_L2StarB'),
                                   EFcreate('mu4Tmu6_Jpsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),


    
    L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_prescaled',
                            'L2_mu4Tmu6_Jpsimumu_prescaled', 838, mu4Tmu6Seed, 'EF_mu4Tmu6_Jpsimumu_prescaled', 838, ['L2_mu4T','L2_mu6'],
                            Config(L2create('mu4Tmu6_Jpsimumu'),
                                   EFcreate('mu4Tmu6_Jpsimumu',TEs=['EF_mu4T','EF_mu6']))),    
    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_JPSI('2mu4_Jpsimumu',
                                'L2_2mu4_Jpsimumu', 196, 'L1_2MU0', 'EF_2mu4_Jpsimumu', 196, ['L2_mu4','L2_mu4'],
                                Config(L2create('2mu4_Jpsimumu'),
                                       EFcreate('2mu4_Jpsimumu',TEs=['EF_mu4','EF_mu4']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_L2StarB',
                                'L2_2mu4T_Jpsimumu_L2StarB', 337, 'L1_2MU4', 'EF_2mu4T_Jpsimumu_L2StarB', 337, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_Barrel_L2StarB',
                                'L2_2mu4T_Jpsimumu_Barrel_L2StarB', 338, _2mu4TBarrelSeed, 'EF_2mu4T_Jpsimumu_Barrel_L2StarB', 338, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_Barrel_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_BarrelOnly_L2StarB',
                                'L2_2mu4T_Jpsimumu_BarrelOnly_L2StarB', 375, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Jpsimumu_BarrelOnly_L2StarB', 375, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_BarrelOnly_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_BarrelOnly_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),



        
        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu',
                                'L2_2mu4T_Jpsimumu', 196, 'L1_2MU4', 'EF_2mu4T_Jpsimumu', 196, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu'),
                                       EFcreate('2mu4T_Jpsimumu',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_Barrel',
                                'L2_2mu4T_Jpsimumu_Barrel', 228, _2mu4TBarrelSeed, 'EF_2mu4T_Jpsimumu_Barrel', 228, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu_Barrel'),
                                       EFcreate('2mu4T_Jpsimumu_Barrel',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_BarrelOnly',
                                'L2_2mu4T_Jpsimumu_BarrelOnly', 210, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Jpsimumu_BarrelOnly', 210, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu_BarrelOnly'),
                                       EFcreate('2mu4T_Jpsimumu_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),


        L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_Barrel',
                                'L2_mu4Tmu6_Jpsimumu_Barrel', 229, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Jpsimumu_Barrel', 229, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_Jpsimumu_Barrel'),
                                       EFcreate('mu4Tmu6_Jpsimumu_Barrel',TEs=['EF_mu4T','EF_mu6']))),



        L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_Barrel_L2StarB',
                                'L2_mu4Tmu6_Jpsimumu_Barrel_L2StarB', 43, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Jpsimumu_Barrel_L2StarB', 43, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                Config(L2create('mu4Tmu6_Jpsimumu_Barrel_L2StarB'),
                                       EFcreate('mu4Tmu6_Jpsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),


        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_prescaled',
                                'L2_2mu4T_Jpsimumu_prescaled', 843, 'L1_2MU4', 'EF_2mu4T_Jpsimumu_prescaled', 843, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu'),
                                       EFcreate('2mu4T_Jpsimumu',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_Barrel_prescaled',
                                'L2_2mu4T_Jpsimumu_Barrel_prescaled', 845, _2mu4TBarrelSeed, 'EF_2mu4T_Jpsimumu_Barrel_prescaled', 845, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu_Barrel'),
                                       EFcreate('2mu4T_Jpsimumu_Barrel',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_BarrelOnly_prescaled',
                                'L2_2mu4T_Jpsimumu_BarrelOnly_prescaled', 846, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Jpsimumu_BarrelOnly_prescaled', 846, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Jpsimumu_BarrelOnly'),
                                       EFcreate('2mu4T_Jpsimumu_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_Barrel_prescaled',
                                'L2_mu4Tmu6_Jpsimumu_Barrel_prescaled', 847, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Jpsimumu_Barrel_prescaled', 847, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_Jpsimumu_Barrel'),
                                       EFcreate('mu4Tmu6_Jpsimumu_Barrel',TEs=['EF_mu4T','EF_mu6']))),




        L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_L2StarB_prescaled',
                                'L2_mu4Tmu6_Jpsimumu_L2StarB_prescaled', 56, mu4Tmu6Seed, 'EF_mu4Tmu6_Jpsimumu_L2StarB_prescaled', 56, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                Config(L2create('mu4Tmu6_Jpsimumu_L2StarB'),
                                       EFcreate('mu4Tmu6_Jpsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),    
        
        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_L2StarB_prescaled',
                                'L2_2mu4T_Jpsimumu_L2StarB_prescaled', 88, 'L1_2MU4', 'EF_2mu4T_Jpsimumu_L2StarB_prescaled', 88, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_Barrel_L2StarB_prescaled',
                                'L2_2mu4T_Jpsimumu_Barrel_L2StarB_prescaled', 90, _2mu4TBarrelSeed, 'EF_2mu4T_Jpsimumu_Barrel_L2StarB_prescaled', 90, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_Barrel_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_JPSI('2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled',
                                'L2_2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled', 99, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled', 99, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Jpsimumu_BarrelOnly_L2StarB'),
                                       EFcreate('2mu4T_Jpsimumu_BarrelOnly_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_JPSI('mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled',
                                'L2_mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled', 184, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled', 184, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                Config(L2create('mu4Tmu6_Jpsimumu_Barrel_L2StarB'),
                                       EFcreate('mu4Tmu6_Jpsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),




        ]
###########################################################################
#TopoDiMu_UPSI inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_UPSI(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_Upsi()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_Upsi()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_Upsi_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_Upsi_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_Upsi()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_Upsi()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_UPSI('2mu6_Upsimumu_L2StarB',
                             'L2_2mu6_Upsimumu_L2StarB', 206, 'L1_2MU6', 'EF_2mu6_Upsimumu_L2StarB', 206, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                             Config(L2create('2mu6_Upsimumu_L2StarB'),
                                    EFcreate('2mu6_Upsimumu_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_UPSI('2mu6_Upsimumu',
                             'L2_2mu6_Upsimumu', 199, 'L1_2MU6', 'EF_2mu6_Upsimumu', 199, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_Upsimumu'),
                                    EFcreate('2mu6_Upsimumu',TEs=['EF_mu6','EF_mu6']))),

    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_UPSI('2mu4_Upsimumu',
                                'L2_2mu4_Upsimumu', 198, 'L1_2MU0', 'EF_2mu4_Upsimumu', 198, ['L2_mu4','L2_mu4'],
                                Config(L2create('2mu4_Upsimumu'),
                                       EFcreate('2mu4_Upsimumu',TEs=['EF_mu4','EF_mu4']))),
        
        L2EFChain_TopoDiMu_UPSI('mu4mu6_Upsimumu',
                                'L2_mu4mu6_Upsimumu', 381, mu4mu6Seed, 'EF_mu4mu6_Upsimumu', 381, ['L2_mu4','L2_mu6'],
                                Config(L2create('mu4mu6_Upsimumu'),
                                       EFcreate('mu4mu6_Upsimumu',TEs=['EF_mu4','EF_mu6']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu',
                                'L2_2mu4T_Upsimumu', 198, 'L1_2MU4', 'EF_2mu4T_Upsimumu', 198, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Upsimumu'),
                                       EFcreate('2mu4T_Upsimumu',TEs=['EF_mu4T','EF_mu4T']))),
        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu_L2StarB',
                                'L2_2mu4T_Upsimumu_L2StarB', 203, 'L1_2MU4', 'EF_2mu4T_Upsimumu_L2StarB', 203, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Upsimumu_L2StarB'),
                                       EFcreate('2mu4T_Upsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    
        
        L2EFChain_TopoDiMu_UPSI('mu4Tmu6_Upsimumu',
                                'L2_mu4Tmu6_Upsimumu', 381, mu4Tmu6Seed, 'EF_mu4Tmu6_Upsimumu', 381, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_Upsimumu'),
                                       EFcreate('mu4Tmu6_Upsimumu',TEs=['EF_mu4T','EF_mu6']))),

        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu_Barrel',
                                'L2_2mu4T_Upsimumu_Barrel', 231, _2mu4TBarrelSeed, 'EF_2mu4T_Upsimumu_Barrel', 231, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Upsimumu_Barrel'),
                                       EFcreate('2mu4T_Upsimumu_Barrel',TEs=['EF_mu4T','EF_mu4T']))),    

        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu_BarrelOnly',
                                'L2_2mu4T_Upsimumu_BarrelOnly', 230, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Upsimumu_BarrelOnly', 230, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_Upsimumu_BarrelOnly'),
                                       EFcreate('2mu4T_Upsimumu_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),    


        L2EFChain_TopoDiMu_UPSI('mu4Tmu6_Upsimumu_Barrel',
                                'L2_mu4Tmu6_Upsimumu_Barrel', 487, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Upsimumu_Barrel', 487, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_Upsimumu_Barrel'),
                                       EFcreate('mu4Tmu6_Upsimumu_Barrel',TEs=['EF_mu4T','EF_mu6']))),
        


        L2EFChain_TopoDiMu_UPSI('mu4Tmu6_Upsimumu_Barrel_L2StarB',
                                'L2_mu4Tmu6_Upsimumu_Barrel_L2StarB', 222, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Upsimumu_Barrel_L2StarB', 223, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                Config(L2create('mu4Tmu6_Upsimumu_Barrel_L2StarB'),
                                       EFcreate('mu4Tmu6_Upsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),
        

        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu_Barrel_L2StarB',
                                'L2_2mu4T_Upsimumu_Barrel_L2StarB', 225, _2mu4TBarrelSeed, 'EF_2mu4T_Upsimumu_Barrel_L2StarB', 225, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Upsimumu_Barrel_L2StarB'),
                                       EFcreate('2mu4T_Upsimumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    

        L2EFChain_TopoDiMu_UPSI('2mu4T_Upsimumu_BarrelOnly_L2StarB',
                                'L2_2mu4T_Upsimumu_BarrelOnly_L2StarB', 863, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Upsimumu_BarrelOnly_L2StarB', 866, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                Config(L2create('2mu4T_Upsimumu_BarrelOnly_L2StarB'),
                                       EFcreate('2mu4T_Upsimumu_BarrelOnly_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    


        L2EFChain_TopoDiMu_UPSI('mu4Tmu6_Upsimumu_L2StarB',
                                'L2_mu4Tmu6_Upsimumu_L2StarB', 892, mu4Tmu6Seed, 'EF_mu4Tmu6_Upsimumu_L2StarB', 868, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                Config(L2create('mu4Tmu6_Upsimumu_L2StarB'),
                                       EFcreate('mu4Tmu6_Upsimumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),


        ]
###########################################################################
#TopoDiMu_BMUMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_BMUMU(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_B()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_B()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_B_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_B_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_B()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_B()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_BMUMU('2mu6_Bmumu',
                             'L2_2mu6_Bmumu', 869, 'L1_2MU6', 'EF_2mu6_Bmumu', 869, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_Bmumu'),
                                    EFcreate('2mu6_Bmumu',TEs=['EF_mu6','EF_mu6']))),

    L2EFChain_TopoDiMu_BMUMU('2mu6_Bmumu_L2StarB',
                             'L2_2mu6_Bmumu_L2StarB', 384, 'L1_2MU6', 'EF_2mu6_Bmumu_L2StarB', 384, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                             Config(L2create('2mu6_Bmumu_L2StarB'),
                                    EFcreate('2mu6_Bmumu_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),
    ]



if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_BMUMU('2mu4_Bmumu',
                                 'L2_2mu4_Bmumu', 164, 'L1_2MU0', 'EF_2mu4_Bmumu', 164, ['L2_mu4','L2_mu4'],
                                 Config(L2create('2mu4_Bmumu'),
                                        EFcreate('2mu4_Bmumu',TEs=['EF_mu4','EF_mu4']))),
        
        L2EFChain_TopoDiMu_BMUMU('mu4mu6_Bmumu',
                                 'L2_mu4mu6_Bmumu', 382, mu4mu6Seed, 'EF_mu4mu6_Bmumu', 382, ['L2_mu4','L2_mu6'],
                                 Config(L2create('mu4mu6_Bmumu'),
                                        EFcreate('mu4mu6_Bmumu',TEs=['EF_mu4','EF_mu6']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu',
                                 'L2_2mu4T_Bmumu', 164, 'L1_2MU4', 'EF_2mu4T_Bmumu', 164, ['L2_mu4T','L2_mu4T'],
                                 Config(L2create('2mu4T_Bmumu'),
                                        EFcreate('2mu4T_Bmumu',TEs=['EF_mu4T','EF_mu4T']))),
        
        L2EFChain_TopoDiMu_BMUMU('mu4Tmu6_Bmumu',
                                 'L2_mu4Tmu6_Bmumu', 382, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumu', 382, ['L2_mu4T','L2_mu6'],
                                 Config(L2create('mu4Tmu6_Bmumu'),
                                        EFcreate('mu4Tmu6_Bmumu',TEs=['EF_mu4T','EF_mu6']))),

        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu_Barrel',
                                 'L2_2mu4T_Bmumu_Barrel', 233, _2mu4TBarrelSeed, 'EF_2mu4T_Bmumu_Barrel', 233, ['L2_mu4T','L2_mu4T'],
                                 Config(L2create('2mu4T_Bmumu_Barrel'),
                                        EFcreate('2mu4T_Bmumu_Barrel',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu_BarrelOnly',
                                 'L2_2mu4T_Bmumu_BarrelOnly', 232, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumu_BarrelOnly', 232, ['L2_mu4T','L2_mu4T'],
                                 Config(L2create('2mu4T_Bmumu_BarrelOnly'),
                                        EFcreate('2mu4T_Bmumu_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),



        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu_L2StarB',
                                 'L2_2mu4T_Bmumu_L2StarB', 386, 'L1_2MU4', 'EF_2mu4T_Bmumu_L2StarB', 386, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                 Config(L2create('2mu4T_Bmumu_L2StarB'),
                                        EFcreate('2mu4T_Bmumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),
        
        L2EFChain_TopoDiMu_BMUMU('mu4Tmu6_Bmumu_L2StarB',
                                 'L2_mu4Tmu6_Bmumu_L2StarB', 391, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumu_L2StarB', 391, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                 Config(L2create('mu4Tmu6_Bmumu_L2StarB'),
                                        EFcreate('mu4Tmu6_Bmumu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),

        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu_Barrel_L2StarB',
                                 'L2_2mu4T_Bmumu_Barrel_L2StarB', 400, _2mu4TBarrelSeed, 'EF_2mu4T_Bmumu_Barrel_L2StarB', 400, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                 Config(L2create('2mu4T_Bmumu_Barrel_L2StarB'),
                                        EFcreate('2mu4T_Bmumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_BMUMU('2mu4T_Bmumu_BarrelOnly_L2StarB',
                                 'L2_2mu4T_Bmumu_BarrelOnly_L2StarB', 410, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumu_BarrelOnly_L2StarB', 410, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                 Config(L2create('2mu4T_Bmumu_BarrelOnly_L2StarB'),
                                        EFcreate('2mu4T_Bmumu_BarrelOnly_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),


        
        L2EFChain_TopoDiMu_BMUMU('mu4Tmu6_Bmumu_Barrel',
                                 'L2_mu4Tmu6_Bmumu_Barrel', 236, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumu_Barrel', 236, ['L2_mu4T','L2_mu6'],
                                 Config(L2create('mu4Tmu6_Bmumu_Barrel'),
                                        EFcreate('mu4Tmu6_Bmumu_Barrel',TEs=['EF_mu4T','EF_mu6']))),

        L2EFChain_TopoDiMu_BMUMU('mu4Tmu6_Bmumu_Barrel_L2StarB',
                                 'L2_mu4Tmu6_Bmumu_Barrel_L2StarB', 41, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumu_Barrel_L2StarB', 41, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                 Config(L2create('mu4Tmu6_Bmumu_Barrel_L2StarB'),
                                        EFcreate('mu4Tmu6_Bmumu_Barrel_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),

        ]

###########################################################################
#TopoDiMu_BMUMU_Lxy inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_BMUMU_Lxy(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_B()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_B()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_B_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_B_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_B()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_B_Lxy()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_BMUMU_Lxy('2mu6_Bmumu_Lxy',
                                 'L2_2mu6_Bmumu_Lxy', 1852, 'L1_2MU6', 'EF_2mu6_Bmumu_Lxy', 1852, ['L2_mu6','L2_mu6'],
                                 Config(L2create('2mu6_Bmumu_Lxy'),
                                        EFcreate('2mu6_Bmumu_Lxy',TEs=['EF_mu6','EF_mu6']))),
    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_BMUMU_Lxy('2mu4T_Bmumu_Lxy',
                                     'L2_2mu4T_Bmumu_Lxy', 1853, 'L1_2MU4', 'EF_2mu4T_Bmumu_Lxy', 1853, ['L2_mu4T','L2_mu4T'],
                                     Config(L2create('2mu4T_Bmumu_Lxy'),
                                            EFcreate('2mu4T_Bmumu_Lxy',TEs=['EF_mu4T','EF_mu4T']))),
        
        L2EFChain_TopoDiMu_BMUMU_Lxy('mu4Tmu6_Bmumu_Lxy',
                                     'L2_mu4Tmu6_Bmumu_Lxy', 1854, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumu_Lxy', 1854, ['L2_mu4T','L2_mu6'],
                                     Config(L2create('mu4Tmu6_Bmumu_Lxy'),
                                            EFcreate('mu4Tmu6_Bmumu_Lxy',TEs=['EF_mu4T','EF_mu6']))),
        
        L2EFChain_TopoDiMu_BMUMU_Lxy('2mu4T_Bmumu_Lxy_Barrel',
                                     'L2_2mu4T_Bmumu_Lxy_Barrel', 1855, _2mu4TBarrelSeed, 'EF_2mu4T_Bmumu_Lxy_Barrel', 1855, ['L2_mu4T','L2_mu4T'],
                                     Config(L2create('2mu4T_Bmumu_Lxy_Barrel'),
                                            EFcreate('2mu4T_Bmumu_Lxy_Barrel',TEs=['EF_mu4T','EF_mu4T']))),
        
        L2EFChain_TopoDiMu_BMUMU_Lxy('2mu4T_Bmumu_Lxy_BarrelOnly',
                                     'L2_2mu4T_Bmumu_Lxy_BarrelOnly', 1857, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumu_Lxy_BarrelOnly', 1857, ['L2_mu4T','L2_mu4T'],
                                     Config(L2create('2mu4T_Bmumu_Lxy_BarrelOnly'),
                                            EFcreate('2mu4T_Bmumu_Lxy_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),
        
        L2EFChain_TopoDiMu_BMUMU_Lxy('mu4Tmu6_Bmumu_Lxy_Barrel',
                                     'L2_mu4Tmu6_Bmumu_Lxy_Barrel', 1858, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumu_Lxy_Barrel', 1858, ['L2_mu4T','L2_mu6'],
                                     Config(L2create('mu4Tmu6_Bmumu_Lxy_Barrel'),
                                            EFcreate('mu4Tmu6_Bmumu_Lxy_Barrel',TEs=['EF_mu4T','EF_mu6']))),
        ]


###########################################################################
#TopoDiMu_DIMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_DIMU(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_DIMU('2mu6_DiMu',
                             'L2_2mu6_DiMu', 389, 'L1_2MU6', 'EF_2mu6_DiMu', 389, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_DiMu'),
                                    EFcreate('2mu6_DiMu',TEs=['EF_mu6','EF_mu6']))),

    L2EFChain_TopoDiMu_DIMU('2mu6_DiMu_L2StarB',
                             'L2_2mu6_DiMu_L2StarB', 811, 'L1_2MU6', 'EF_2mu6_DiMu_L2StarB', 811, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                             Config(L2create('2mu6_DiMu_L2StarB'),
                                    EFcreate('2mu6_DiMu_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),


    L2EFChain_TopoDiMu_DIMU('2mu6_DiMu_FTK',
                            'L2_2mu6_DiMu_FTK', 7015, 'L1_2MU6', 'EF_2mu6_DiMu_FTK', 7015, ['L2_mu6_FTK','L2_mu6_FTK'],
                             Config(L2create('2mu6_DiMu_FTK'),
                                    EFcreate('2mu6_DiMu_FTK',TEs=['EF_mu6_FTK','EF_mu6_FTK']))),



    ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
                       
        L2EFChain_TopoDiMu_DIMU('2mu4_DiMu',
                                'L2_2mu4_DiMu', 388, 'L1_2MU0', 'EF_2mu4_DiMu', 388, ['L2_mu4','L2_mu4'],
                                Config(L2create('2mu4_DiMu'),
                                       EFcreate('2mu4_DiMu',TEs=['EF_mu4','EF_mu4']))),

        L2EFChain_TopoDiMu_DIMU('2mu4_DiMu_L2StarA',
                                'L2_2mu4_DiMu_L2StarA', 51, 'L1_2MU0', 'EF_2mu4_DiMu_L2StarA', 51, ['L2_mu4_L2StarA','L2_mu4_L2StarA'],
                                Config(L2create('2mu4_DiMu_L2StarA'),
                                       EFcreate('2mu4_DiMu_L2StarA',TEs=['EF_mu4_L2StarA','EF_mu4_L2StarA']))),

        L2EFChain_TopoDiMu_DIMU('2mu4_DiMu_L2StarB',
                                 'L2_2mu4_DiMu_L2StarB', 52, 'L1_2MU0', 'EF_2mu4_DiMu_L2StarB', 52, ['L2_mu4_L2StarB','L2_mu4_L2StarB'],
                                 Config(L2create('2mu4_DiMu_L2StarB'),
                                        EFcreate('2mu4_DiMu_L2StarB',TEs=['EF_mu4_L2StarB','EF_mu4_L2StarB']))),

        L2EFChain_TopoDiMu_DIMU('2mu4_DiMu_L2StarC',
                                'L2_2mu4_DiMu_L2StarC', 53, 'L1_2MU0', 'EF_2mu4_DiMu_L2StarC', 53, ['L2_mu4_L2StarC','L2_mu4_L2StarC'],
                                Config(L2create('2mu4_DiMu_L2StarC'),
                                       EFcreate('2mu4_DiMu_L2StarC',TEs=['EF_mu4_L2StarC','EF_mu4_L2StarC']))),
        

        
        L2EFChain_TopoDiMu_DIMU('2mu4_DiMu_SiTrk',
                                'L2_2mu4_DiMu_SiTrk', 407, 'L1_2MU0', 'EF_2mu4_DiMu_SiTrk', 407, ['L2_mu4_SiTrk','L2_mu4_SiTrk'],
                                Config(L2create('2mu4_DiMu_SiTrk'),
                                       EFcreate('2mu4_DiMu_SiTrk',TEs=['EF_mu4_SiTrk','EF_mu4_SiTrk']))),
        
        L2EFChain_TopoDiMu_DIMU('mu4mu6_DiMu',
                                'L2_mu4mu6_DiMu', 408, mu4mu6Seed, 'EF_mu4mu6_DiMu', 408, ['L2_mu4','L2_mu6'],
                                Config(L2create('mu4mu6_DiMu'),
                                       EFcreate('mu4mu6_DiMu',TEs=['EF_mu4','EF_mu6']))),



        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu',
                                'L2_2mu4T_DiMu', 388, 'L1_2MU4', 'EF_2mu4T_DiMu', 388, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_DiMu'),
                                       EFcreate('2mu4T_DiMu',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_l2muonSA',
                                'L2_2mu4T_DiMu_l2muonSA', 77, 'L1_2MU4', 'EF_2mu4T_DiMu_l2muonSA', 77, ['L2_mu4T_l2muonSA','L2_mu4T_l2muonSA'],
                                Config(L2create('2mu4T_DiMu_l2muonSA'),
                                       EFcreate('2mu4T_DiMu_l2muonSA',TEs=['EF_mu4T_l2muonSA','EF_mu4T_l2muonSA']))),    


        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_muFast',
                                'L2_2mu4T_DiMu_muFast', 7077, 'L1_2MU4', 'EF_2mu4T_DiMu_muFast', 7077, ['L2_mu4T_muFast','L2_mu4T_muFast'],
                                Config(L2create('2mu4T_DiMu_muFast'),
                                       EFcreate('2mu4T_DiMu_muFast',TEs=['EF_mu4T_muFast','EF_mu4T_muFast']))),
        
        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_L2StarA',
                                'L2_2mu4T_DiMu_L2StarA', 51, 'L1_2MU4', 'EF_2mu4T_DiMu_L2StarA', 51, ['L2_mu4T_L2StarA','L2_mu4T_L2StarA'],
                                Config(L2create('2mu4T_DiMu_L2StarA'),
                                       EFcreate('2mu4T_DiMu_L2StarA',TEs=['EF_mu4T_L2StarA','EF_mu4T_L2StarA']))),

       L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_L2StarB',
                               'L2_2mu4T_DiMu_L2StarB', 52, 'L1_2MU4', 'EF_2mu4T_DiMu_L2StarB', 52, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                               Config(L2create('2mu4T_DiMu_L2StarB'),
                                      EFcreate('2mu4T_DiMu_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),

        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_L2StarC',
                                'L2_2mu4T_DiMu_L2StarC', 53, 'L1_2MU4', 'EF_2mu4T_DiMu_L2StarC', 53, ['L2_mu4T_L2StarC','L2_mu4T_L2StarC'],
                                Config(L2create('2mu4T_DiMu_L2StarC'),
                                       EFcreate('2mu4T_DiMu_L2StarC',TEs=['EF_mu4T_L2StarC','EF_mu4T_L2StarC']))),
        
        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_SiTrk',
                                'L2_2mu4T_DiMu_SiTrk', 407, 'L1_2MU4', 'EF_2mu4T_DiMu_SiTrk', 407, ['L2_mu4T_SiTrk','L2_mu4T_SiTrk'],
                                Config(L2create('2mu4T_DiMu_SiTrk'),
                                       EFcreate('2mu4T_DiMu_SiTrk',TEs=['EF_mu4T_SiTrk','EF_mu4T_SiTrk']))),    

        L2EFChain_TopoDiMu_DIMU('mu4Tmu6_DiMu',
                                'L2_mu4Tmu6_DiMu', 408, mu4Tmu6Seed, 'EF_mu4Tmu6_DiMu', 408, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_DiMu'),
                                       EFcreate('mu4Tmu6_DiMu',TEs=['EF_mu4T','EF_mu6']))),    

        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_Barrel',
                                'L2_2mu4T_DiMu_Barrel', 2485, _2mu4TBarrelSeed, 'EF_2mu4T_DiMu_Barrel', 2485, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_DiMu_Barrel'),
                                       EFcreate('2mu4T_DiMu_Barrel',TEs=['EF_mu4T','EF_mu4T']))),

        L2EFChain_TopoDiMu_DIMU('2mu4T_DiMu_BarrelOnly',
                                'L2_2mu4T_DiMu_BarrelOnly', 2486, _2mu4TBarrelOnlySeed, 'EF_2mu4T_DiMu_BarrelOnly', 2486, ['L2_mu4T','L2_mu4T'],
                                Config(L2create('2mu4T_DiMu_BarrelOnly'),
                                       EFcreate('2mu4T_DiMu_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),
    
        L2EFChain_TopoDiMu_DIMU('mu4Tmu6_DiMu_Barrel',
                                'L2_mu4Tmu6_DiMu_Barrel', 2496, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_DiMu_Barrel', 2496, ['L2_mu4T','L2_mu6'],
                                Config(L2create('mu4Tmu6_DiMu_Barrel'),
                                       EFcreate('mu4Tmu6_DiMu_Barrel',TEs=['EF_mu4T','EF_mu6']))),    
        ]

############################################################################
#TopoMultiMu_DIMU
class L2EFChain_TopoMultiMu_DIMU(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_DiMu()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_DiMu()
        else:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_DiMu_passL2()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_DiMu_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFMultiMuFex_DiMu()
            self.efchain.sequences[0].algos[1] = EFMultiMuHypo_DiMu()

        pass
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_DIMU('3mu4_DiMu',
                                   'L2_3mu4_DiMu', 258, 'L1_3MU0', 'EF_3mu4_DiMu', 258, ['L2_mu4','L2_mu4', 'L2_mu4'],
                                   Config(L2create('3mu4_DiMu'),
                                          EFcreate('3mu4_DiMu',TEs=['EF_mu4','EF_mu4','EF_mu4']))),
        ]


InputTE_for_3mu4T = 'L2_muon_combined_wOvlpRm_mu4T_wOvlpRm' 
        
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_DIMU('3mu4T_DiMu',
                                   'L2_3mu4T_DiMu', 258, 'L1_3MU4', 'EF_3mu4T_DiMu', 258, [InputTE_for_3mu4T,InputTE_for_3mu4T, InputTE_for_3mu4T],
                                   Config(L2create('3mu4T_DiMu'),
                                          EFcreate('3mu4T_DiMu',TEs=['EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm']))),    
        ]
############################################################################
#TopoMultiMu_DIMU
class L2EFChain_TopoMultiMu_JPSI(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Jpsi()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Jpsi()
        else:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Jpsi_passL2()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Jpsi_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFMultiMuFex_Jpsi()
            self.efchain.sequences[0].algos[1] = EFMultiMuHypo_Jpsi()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_JPSI('3mu4_Jpsi',
                                   'L2_3mu4_Jpsi', 259, 'L1_3MU0', 'EF_3mu4_Jpsi', 259, ['L2_mu4','L2_mu4', 'L2_mu4'],
                                   Config(L2create('3mu4_Jpsi'),
                                          EFcreate('3mu4_Jpsi',TEs=['EF_mu4','EF_mu4','EF_mu4']))),
        ]
    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_JPSI('3mu4T_Jpsi',
                                   'L2_3mu4T_Jpsi', 259, 'L1_3MU4', 'EF_3mu4T_Jpsi', 259, [InputTE_for_3mu4T,InputTE_for_3mu4T, InputTE_for_3mu4T],
                                   Config(L2create('3mu4T_Jpsi'),
                                          EFcreate('3mu4T_Jpsi',TEs=['EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm']))),    
        
        ]
############################################################################
#TopoMultiMu_DIMU
class L2EFChain_TopoMultiMu_UPSI(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Upsi()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Upsi()
        else:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Upsi_passL2()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Upsi_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFMultiMuFex_Upsi()
            self.efchain.sequences[0].algos[1] = EFMultiMuHypo_Upsi()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_UPSI('3mu4_Upsi',
                                   'L2_3mu4_Upsi', 260, 'L1_3MU0', 'EF_3mu4_Upsi', 260, ['L2_mu4','L2_mu4', 'L2_mu4'],
                                   Config(L2create('3mu4_Upsi'),
                                          EFcreate('3mu4_Upsi',TEs=['EF_mu4','EF_mu4','EF_mu4']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_UPSI('3mu4T_Upsi',
                                   'L2_3mu4T_Upsi', 260, 'L1_3MU4', 'EF_3mu4T_Upsi', 260, [InputTE_for_3mu4T,InputTE_for_3mu4T, InputTE_for_3mu4T],
                                   Config(L2create('3mu4T_Upsi'),
                                          EFcreate('3mu4T_Upsi',TEs=['EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm']))),    
    ]
###############################################################################
class L2EFChain_TopoMultiMu_TAU(L2EFChain_TopoDiMu_JPSI):
    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Tau()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Tau()
        else:
            self.l2chain.sequences[0].algos[0] = L2MultiMuFex_Tau_passL2()
            self.l2chain.sequences[0].algos[1] = L2MultiMuHypo_Tau_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFMultiMuFex_Tau()
            self.efchain.sequences[0].algos[1] = EFMultiMuHypo_Tau()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_TAU('3mu4_Tau',
                                  'L2_3mu4_Tau', 261, 'L1_3MU0', 'EF_3mu4_Tau', 261, ['L2_mu4','L2_mu4', 'L2_mu4'],
                                  Config(L2create('3mu4_Tau'),
                                         EFcreate('3mu4_Tau',TEs=['EF_mu4','EF_mu4','EF_mu4']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoMultiMu_TAU('3mu4T_Tau',
                                  'L2_3mu4T_Tau', 261, 'L1_3MU4', 'EF_3mu4T_Tau', 261, [InputTE_for_3mu4T,InputTE_for_3mu4T, InputTE_for_3mu4T],
                                  Config(L2create('3mu4T_Tau'),
                                         EFcreate('3mu4T_Tau',TEs=['EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm','EF_mu4T_wOvlpRm']))),    
        ]
###############################################################################
class L2EFChain_TopoDiMu_DIMU_DY(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_DY()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_DY()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_DY()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_DY()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY('2mu4i_DiMu_DY',
                                   'L2_2mu4i_DiMu_DY', 385, 'L1_2MU0', 'EF_2mu4i_DiMu_DY', 385, ['L2_mu4i','L2_mu4i'],
                                   Config(L2create('2mu4i_DiMu_DY'),
                                          EFcreate('2mu4i_DiMu_DY',TEs=['EF_mu4i','EF_mu4i']))),
        L2EFChain_TopoDiMu_DIMU_DY('2mu4_DiMu_DY',
                                   'L2_2mu4_DiMu_DY', 298, 'L1_2MU0', 'EF_2mu4_DiMu_DY', 298, ['L2_mu4','L2_mu4'],
                                   Config(L2create('2mu4_DiMu_DY'),
                                          EFcreate('2mu4_DiMu_DY',TEs=['EF_mu4','EF_mu4']))),
        L2EFChain_TopoDiMu_DIMU_DY('mu4imu6i_DiMu_DY',
                                   'L2_mu4imu6i_DiMu_DY', 287, mu4mu6Seed, 'EF_mu4imu6i_DiMu_DY', 287, ['L2_mu4i','L2_mu6i'],
                                   Config(L2create('mu4imu6i_DiMu_DY'),
                                          EFcreate('mu4imu6i_DiMu_DY',TEs=['EF_mu4i','EF_mu6i']))),
        L2EFChain_TopoDiMu_DIMU_DY('mu4mu6_DiMu_DY',
                                   'L2_mu4mu6_DiMu_DY', 289, mu4mu6Seed, 'EF_mu4mu6_DiMu_DY', 289, ['L2_mu4','L2_mu6'],
                                   Config(L2create('mu4mu6_DiMu_DY'),
                                          EFcreate('mu4mu6_DiMu_DY',TEs=['EF_mu4','EF_mu6']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY('2mu4Ti_DiMu_DY',
                                   'L2_2mu4Ti_DiMu_DY', 385, 'L1_2MU4', 'EF_2mu4Ti_DiMu_DY', 385, ['L2_mu4Ti','L2_mu4Ti'],
                                   Config(L2create('2mu4Ti_DiMu_DY'),
                                          EFcreate('2mu4Ti_DiMu_DY',TEs=['EF_mu4Ti','EF_mu4Ti']))),    
        L2EFChain_TopoDiMu_DIMU_DY('2mu4T_DiMu_DY',
                                   'L2_2mu4T_DiMu_DY', 298, 'L1_2MU4', 'EF_2mu4T_DiMu_DY', 298, ['L2_mu4T','L2_mu4T'],
                                   Config(L2create('2mu4T_DiMu_DY'),
                                          EFcreate('2mu4T_DiMu_DY',TEs=['EF_mu4T','EF_mu4T']))),    
        L2EFChain_TopoDiMu_DIMU_DY('mu4Timu6i_DiMu_DY',
                                   'L2_mu4Timu6i_DiMu_DY', 287, mu4Tmu6Seed, 'EF_mu4Timu6i_DiMu_DY', 287, ['L2_mu4Ti','L2_mu6i'],
                                   Config(L2create('mu4Timu6i_DiMu_DY'),
                                          EFcreate('mu4Timu6i_DiMu_DY',TEs=['EF_mu4Ti','EF_mu6i']))),    
        L2EFChain_TopoDiMu_DIMU_DY('mu4Tiimu6ii_DiMu_DY',
                                   'L2_mu4Tiimu6ii_DiMu_DY', 1695, mu4Tmu6Seed, 'EF_mu4Tiimu6ii_DiMu_DY', 1695, ['L2_mu4Tii_debug','L2_mu6ii_debug'],
                                   Config(L2create('mu4Tiimu6ii_DiMu_DY'),
                                          EFcreate('mu4Tiimu6ii_DiMu_DY',TEs=['EF_mu4Tii_debug','EF_mu6ii_debug']))),
        L2EFChain_TopoDiMu_DIMU_DY('2mu6i_DiMu_DY',
                                   'L2_2mu6i_DiMu_DY', 627, 'L1_2MU6', 'EF_2mu6i_DiMu_DY', 626, ['L2_mu6','L2_mu6'],
                                   Config(L2create('2mu6i_DiMu_DY'),
                                          EFcreate('2mu6i_DiMu_DY',TEs=['EF_mu6i','EF_mu6i']))),    
        ]

######################################################################################
class L2EFChain_TopoDiMu_DIMU_DY20(L2EFChain_TopoDiMu_JPSI):
    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_DY20()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_DY20()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_DY20()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_DY20()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY20('2mu4_DiMu_DY20',
                                     'L2_2mu4_DiMu_DY20', 626, 'L1_2MU0', 'EF_2mu4_DiMu_DY20', 626, ['L2_mu4','L2_mu4'],
                                     Config(L2create('2mu4_DiMu_DY20'),
                                            EFcreate('2mu4_DiMu_DY20',TEs=['EF_mu4','EF_mu4']))),
        L2EFChain_TopoDiMu_DIMU_DY20('mu4mu6_DiMu_DY20',
                                     'L2_mu4mu6_DiMu_DY20', 628, mu4mu6Seed, 'EF_mu4mu6_DiMu_DY20', 628, ['L2_mu4','L2_mu6'],
                                     Config(L2create('mu4mu6_DiMu_DY20'),
                                            EFcreate('mu4mu6_DiMu_DY20',TEs=['EF_mu4','EF_mu6']))),        
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY20('2mu4T_DiMu_DY20',
                                     'L2_2mu4T_DiMu_DY20', 626, 'L1_2MU4', 'EF_2mu4T_DiMu_DY20', 626, ['L2_mu4T','L2_mu4T'],
                                     Config(L2create('2mu4T_DiMu_DY20'),
                                            EFcreate('2mu4T_DiMu_DY20',TEs=['EF_mu4T','EF_mu4T']))),    
        L2EFChain_TopoDiMu_DIMU_DY20('mu4Tmu6_DiMu_DY20',
                                     'L2_mu4Tmu6_DiMu_DY20', 628, mu4Tmu6Seed, 'EF_mu4Tmu6_DiMu_DY20', 628, ['L2_mu4T','L2_mu6'],
                                     Config(L2create('mu4Tmu6_DiMu_DY20'),
                                            EFcreate('mu4Tmu6_DiMu_DY20',TEs=['EF_mu4T','EF_mu6']))),
        L2EFChain_TopoDiMu_DIMU_DY20('mu4Tiimu6ii_DiMu_DY20',
                                     'L2_mu4Tiimu6ii_DiMu_DY20', 1693, mu4Tmu6Seed, 'EF_mu4Tiimu6ii_DiMu_DY20', 1693, ['L2_mu4Tii_debug','L2_mu6ii_debug'],
                                     Config(L2create('mu4Tiimu6ii_DiMu_DY20'),
                                            EFcreate('mu4Tiimu6ii_DiMu_DY20',TEs=['EF_mu4Tii_debug','EF_mu6ii_debug']))),
        ]
    
Bphysics += [
    L2EFChain_TopoDiMu_DIMU_DY20('2mu6_DiMu_DY20',
                                 'L2_2mu6_DiMu_DY20', 629, 'L1_2MU6', 'EF_2mu6_DiMu_DY20', 629, ['L2_mu6','L2_mu6'],
                                 Config(L2create('2mu6_DiMu_DY20'),
                                        EFcreate('2mu6_DiMu_DY20',TEs=['EF_mu6','EF_mu6']))),
    ]

######################################################################################
class L2EFChain_TopoDiMu_DIMU_DY25(L2EFChain_TopoDiMu_JPSI):
    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_DY25()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_DY25()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_DY25()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_DY25()

        pass
    
Bphysics += [
    L2EFChain_TopoDiMu_DIMU_DY25('2mu6_DiMu_DY25',
                                 'L2_2mu6_DiMu_DY25', 625, 'L1_2MU6', 'EF_2mu6_DiMu_DY25', 625, ['L2_mu6','L2_mu6'],
                                 Config(L2create('2mu6_DiMu_DY25'),
                                        EFcreate('2mu6_DiMu_DY25',TEs=['EF_mu6','EF_mu6']))),
    ]


###########################################################################
#TopoDiMu_NOID inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_NOID(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_noId()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_Jpsi_noId()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_noId_passL2()

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_noId()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_Jpsi()
        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_NOID('2mu4_Jpsimumu_IDTrkNoCut',
                                'L2_2mu4_Jpsimumu_IDTrkNoCut', 500, 'L1_2MU0', 'EF_2mu4_Jpsimumu_IDTrkNoCut', 500, ['L2_mu4','L2_mu4_IDTrkNoCut'],
                                Config(L2create('2mu4_Jpsimumu_IDTrkNoCut'),
                                       EFcreate('2mu4_Jpsimumu_IDTrkNoCut',TEs=['EF_mu4','EF_mu4_IDTrkNoCut']))),   
                
        L2EFChain_TopoDiMu_NOID('mu4mu6_Jpsimumu_IDTrkNoCut',
                            'L2_mu4mu6_Jpsimumu_IDTrkNoCut', 502, mu4mu6Seed, 'EF_mu4mu6_Jpsimumu_IDTrkNoCut', 502, ['L2_mu4','L2_mu6_IDTrkNoCut'],
                            Config(L2create('mu4mu6_Jpsimumu_IDTrkNoCut'),
                                   EFcreate('mu4mu6_Jpsimumu_IDTrkNoCut',TEs=['EF_mu4','EF_mu6_IDTrkNoCut']))),        
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_NOID('2mu4T_Jpsimumu_IDTrkNoCut',
                                'L2_2mu4T_Jpsimumu_IDTrkNoCut', 500, 'L1_2MU4', 'EF_2mu4T_Jpsimumu_IDTrkNoCut', 500, ['L2_mu4T','L2_mu4T_IDTrkNoCut'],
                                Config(L2create('2mu4T_Jpsimumu_IDTrkNoCut'),
                                       EFcreate('2mu4T_Jpsimumu_IDTrkNoCut',TEs=['EF_mu4T','EF_mu4T_IDTrkNoCut']))),    
   
        L2EFChain_TopoDiMu_NOID('2mu4T_Jpsimumu_IDTrkNoCut_R1',
                                'L2_2mu4T_Jpsimumu_IDTrkNoCut_R1', 3486, 'L1_2MU4', 'EF_2mu4T_Jpsimumu_IDTrkNoCut_R1', 3486, ['L2_mu4T','L2_mu4T_IDTrkNoCut_R1'],
                                Config(L2create('2mu4T_Jpsimumu_IDTrkNoCut_R1'),
                                       EFcreate('2mu4T_Jpsimumu_IDTrkNoCut_R1',TEs=['EF_mu4T','EF_mu4T_IDTrkNoCut_R1']))),    
   


        L2EFChain_TopoDiMu_NOID('mu4Tmu6_Jpsimumu_IDTrkNoCut',
                            'L2_mu4Tmu6_Jpsimumu_IDTrkNoCut', 502, mu4Tmu6Seed, 'EF_mu4Tmu6_Jpsimumu_IDTrkNoCut', 502, ['L2_mu4T','L2_mu6_IDTrkNoCut'],
                            Config(L2create('mu4Tmu6_Jpsimumu_IDTrkNoCut'),
                                   EFcreate('mu4Tmu6_Jpsimumu_IDTrkNoCut',TEs=['EF_mu4T','EF_mu6_IDTrkNoCut']))),        
        ]
###########################################################################
#TopoDiMu_NOID inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_NOID_Z(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_noId_Z()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_Z()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_noId_Z_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_Z_passL2()

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_Z()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_Z()
        pass


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_NOID_Z('2mu13_Zmumu_IDTrkNoCut',
                                  'L2_2mu13_Zmumu_IDTrkNoCut', 5183, 'L1_2MU0', 'EF_2mu13_Zmumu_IDTrkNoCut', 5183, ['L2_mu13','L2_mu13_IDTrkNoCut'],
                                  Config(L2create('2mu13_Zmumu_IDTrkNoCut'),
                                         EFcreate('2mu13_Zmumu_IDTrkNoCut',TEs=['EF_mu13','EF_mu13_IDTrkNoCut']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_NOID_Z('2mu13_Zmumu_IDTrkNoCut',
                                  'L2_2mu13_Zmumu_IDTrkNoCut', 183, 'L1_2MU4', 'EF_2mu13_Zmumu_IDTrkNoCut', 183, ['L2_mu13','L2_mu13_IDTrkNoCut'],
                                  Config(L2create('2mu13_Zmumu_IDTrkNoCut'),
                                         EFcreate('2mu13_Zmumu_IDTrkNoCut',TEs=['EF_mu13','EF_mu13_IDTrkNoCut']))),
        ]
###########################################################################
#TopoDiMu_DIMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_DIMU_NOVTX(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_noVtx()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_DIMU_NOVTX('2mu4_DiMu_noVtx',
                             'L2_2mu4_DiMu_noVtx', 856, 'L1_2MU0', 'EF_2mu4_DiMu_noVtx', 856, ['L2_mu4','L2_mu4'],
                             Config(L2create('2mu4_DiMu_noVtx'),
                                    EFcreate('2mu4_DiMu_noVtx',TEs=['EF_mu4','EF_mu4']))),
    ]
###########################################################################
#TopoDiMu_DIMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_noOS()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_noOS_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_noOS()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_noVtx()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('2mu4_DiMu_noVtx_noOS',
                                           'L2_2mu4_DiMu_noVtx_noOS', 915, 'L1_2MU0', 'EF_2mu4_DiMu_noVtx_noOS', 915, ['L2_mu4','L2_mu4'],
                                           Config(L2create('2mu4_DiMu_noVtx_noOS'),
                                                  EFcreate('2mu4_DiMu_noVtx_noOS',TEs=['EF_mu4','EF_mu4']))),
        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('mu4mu6_DiMu_noVtx_noOS',
                                           'L2_mu4mu6_DiMu_noVtx_noOS', 926, 'L1_2MU0_MU6', 'EF_mu4mu6_DiMu_noVtx_noOS', 926, ['L2_mu4','L2_mu6'],
                                           Config(L2create('mu4mu6_DiMu_noVtx_noOS'),
                                                  EFcreate('mu4mu6_DiMu_noVtx_noOS',TEs=['EF_mu4','EF_mu6']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('2mu4T_DiMu_noVtx_noOS',
                                           'L2_2mu4T_DiMu_noVtx_noOS', 915, 'L1_2MU4', 'EF_2mu4T_DiMu_noVtx_noOS', 915, ['L2_mu4T','L2_mu4T'],
                                           Config(L2create('2mu4T_DiMu_noVtx_noOS'),
                                                  EFcreate('2mu4T_DiMu_noVtx_noOS',TEs=['EF_mu4T','EF_mu4T']))),    

        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('2mu4T_DiMu_noVtx_noOS_L2StarB',
                                           'L2_2mu4T_DiMu_noVtx_noOS_L2StarB', 496, 'L1_2MU4', 'EF_2mu4T_DiMu_noVtx_noOS_L2StarB', 496, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                                           Config(L2create('2mu4T_DiMu_noVtx_noOS_L2StarB'),
                                                  EFcreate('2mu4T_DiMu_noVtx_noOS_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    

        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('mu4Tmu6_DiMu_noVtx_noOS',
                                           'L2_mu4Tmu6_DiMu_noVtx_noOS', 4195, mu4Tmu6Seed, 'EF_mu4Tmu6_DiMu_noVtx_noOS', 4195, ['L2_mu4T','L2_mu6'],
                                           Config(L2create('mu4Tmu6_DiMu_noVtx_noOS'),
                                                  EFcreate('mu4Tmu6_DiMu_noVtx_noOS',TEs=['EF_mu4T','EF_mu6']))),    

        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('2mu6_DiMu_noVtx_noOS',
                                           'L2_2mu6_DiMu_noVtx_noOS', 143, 'L1_2MU6', 'EF_2mu6_DiMu_noVtx_noOS', 143, ['L2_mu6','L2_mu6'],
                                           Config(L2create('2mu6_DiMu_noVtx_noOS'),
                                                  EFcreate('2mu6_DiMu_noVtx_noOS',TEs=['EF_mu6','EF_mu6']))),    

        L2EFChain_TopoDiMu_DIMU_NOVTX_NOOS('2mu6_DiMu_noVtx_noOS_L2StarB',
                                           'L2_2mu6_DiMu_noVtx_noOS_L2StarB', 486, 'L1_2MU6', 'EF_2mu6_DiMu_noVtx_noOS_L2StarB', 486, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                                           Config(L2create('2mu6_DiMu_noVtx_noOS_L2StarB'),
                                                  EFcreate('2mu6_DiMu_noVtx_noOS_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),    

        ]
###########################################################################
#TopoDiMu_DIMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_DY_noOS()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_DY_noVtx()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_noOS_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_DY_noOS()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_DY_noVtx()

        pass

Bphysics += [
    L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('2mu4i_DiMu_DY_noVtx_noOS',
                                          'L2_2mu4i_DiMu_DY_noVtx_noOS', 479, 'L1_2MU0', 'EF_2mu4i_DiMu_DY_noVtx_noOS', 479, ['L2_mu4i','L2_mu4i'],
                                          Config(L2create('2mu4i_DiMu_DY_noVtx_noOS'),
                                                 EFcreate('2mu4i_DiMu_DY_noVtx_noOS',TEs=['EF_mu4i','EF_mu4i']))),
    L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('mu4mu6_DiMu_DY_noVtx_noOS',
                                          'L2_mu4mu6_DiMu_DY_noVtx_noOS', 475, mu4mu6Seed, 'EF_mu4mu6_DiMu_DY_noVtx_noOS', 475, ['L2_mu4','L2_mu6'],
                                          Config(L2create('mu4mu6_DiMu_DY_noVtx_noOS'),
                                                 EFcreate('mu4mu6_DiMu_DY_noVtx_noOS',TEs=['EF_mu4','EF_mu6']))),
    L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('mu4imu6i_DiMu_DY_noVtx_noOS',
                                          'L2_mu4imu6i_DiMu_DY_noVtx_noOS', 478, mu4mu6Seed, 'EF_mu4imu6i_DiMu_DY_noVtx_noOS', 478, ['L2_mu4i','L2_mu6i'],
                                          Config(L2create('mu4imu6i_DiMu_DY_noVtx_noOS'),
                                                 EFcreate('mu4imu6i_DiMu_DY_noVtx_noOS',TEs=['EF_mu4i','EF_mu6i']))),
    L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('2mu6i_DiMu_DY_noVtx_noOS',
                                          'L2_2mu6i_DiMu_DY_noVtx_noOS', 478, 'L1_2MU6', 'EF_2mu6i_DiMu_DY_noVtx_noOS', 478, ['L2_mu6','L2_mu6'],
                                          Config(L2create('2mu6i_DiMu_DY_noVtx_noOS'),
                                                 EFcreate('2mu6i_DiMu_DY_noVtx_noOS',TEs=['EF_mu6i','EF_mu6i']))),
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('2mu4_DiMu_DY_noVtx_noOS',
                                              'L2_2mu4_DiMu_DY_noVtx_noOS', 474, 'L1_2MU0', 'EF_2mu4_DiMu_DY_noVtx_noOS', 474, ['L2_mu4','L2_mu4'],
                                              Config(L2create('2mu4_DiMu_DY_noVtx_noOS'),
                                                     EFcreate('2mu4_DiMu_DY_noVtx_noOS',TEs=['EF_mu4','EF_mu4']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY_NOVTX_NOOS('2mu4T_DiMu_DY_noVtx_noOS',
                                              'L2_2mu4T_DiMu_DY_noVtx_noOS', 474, 'L1_2MU4', 'EF_2mu4T_DiMu_DY_noVtx_noOS', 474, ['L2_mu4T','L2_mu4T'],
                                              Config(L2create('2mu4T_DiMu_DY_noVtx_noOS'),
                                                     EFcreate('2mu4T_DiMu_DY_noVtx_noOS',TEs=['EF_mu4T','EF_mu4T']))),
        ]
###########################################################################
#TopoDiMu_DIMU inherits from TopoDiMu_JPSI - just change the Fex and Hypo
class L2EFChain_TopoDiMu_DIMU_DY14_NOVTX_NOOS(L2EFChain_TopoDiMu_JPSI):

    def defineSequences(self, config):
        L2EFChain_TopoDiMu_JPSI.defineSequences(self,config)
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_DY14_noOS()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_DY14_noVtx()
        else:
            self.l2chain.sequences[0].algos[0] = L2BMuMuFex_DiMu_noOS_passL2()
            self.l2chain.sequences[0].algos[1] = L2BMuMuHypo_DiMu_noVtx_passL2()
        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[0].algos[0] = EFBMuMuFex_DiMu_DY14_noOS()
            self.efchain.sequences[0].algos[1] = EFBMuMuHypo_DiMu_DY14_noVtx()

        pass

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY14_NOVTX_NOOS('mu4imu6i_DiMu_DY14_noVtx_noOS',
                                                'L2_mu4imu6i_DiMu_DY14_noVtx_noOS', 477, mu4mu6Seed, 'EF_mu4imu6i_DiMu_DY14_noVtx_noOS', 477, ['L2_mu4i','L2_mu6i'],
                                                Config(L2create('mu4imu6i_DiMu_DY14_noVtx_noOS'),
                                                       EFcreate('mu4imu6i_DiMu_DY14_noVtx_noOS',TEs=['EF_mu4i','EF_mu6i']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_DIMU_DY14_NOVTX_NOOS('mu4Timu6i_DiMu_DY14_noVtx_noOS',
                                                'L2_mu4Timu6i_DiMu_DY14_noVtx_noOS', 477, mu4Tmu6Seed, 'EF_mu4Timu6i_DiMu_DY14_noVtx_noOS', 477, ['L2_mu4Ti','L2_mu6i'],
                                                Config(L2create('mu4Timu6i_DiMu_DY14_noVtx_noOS'),
                                                       EFcreate('mu4Timu6i_DiMu_DY14_noVtx_noOS',TEs=['EF_mu4Ti','EF_mu6i']))),    
        L2EFChain_TopoDiMu_DIMU_DY14_NOVTX_NOOS('mu4Tiimu6ii_DiMu_DY14_noVtx_noOS',
                                                'L2_mu4Tiimu6ii_DiMu_DY14_noVtx_noOS', 1694, mu4Tmu6Seed, 'EF_mu4Tiimu6ii_DiMu_DY14_noVtx_noOS', 1694, ['L2_mu4Tii_debug','L2_mu6ii_debug'],
                                                Config(L2create('mu4Tiimu6ii_DiMu_DY14_noVtx_noOS'),
                                                       EFcreate('mu4Tiimu6ii_DiMu_DY14_noVtx_noOS',TEs=['EF_mu4Tii_debug','EF_mu6ii_debug']))),    
        ]
###########################################################################
#TopoDiMu_BMUMUX
class L2EFChain_TopoDiMu_BMUMUX(L2EFChain_Bphysics):

    def defineSequences(self, config):
        
        #L2 sequences
        TE1=self.inputTEs[0]
        TE2=self.inputTEs[1]

        if not "passL2" in self.config.L2Config.suffix:
            if "2mu4" in self.config.L2Config.suffix:
                self.addL2Sequence(TE1,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu4_id_1')
                self.addL2Sequence(TE2,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu4_id_2')
                if "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu4_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu4_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs_tight(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu4_id_2'],
                                  [L2BMuMuXFex_allModes(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu4_id_2'],
                                  [L2BMuMuXFex_allModes_tightBc(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                else:
                    self.addL2Sequence(['L2_mu4_id_1']+['L2_mu4_id_2'],
                                  [L2BMuMuXFex_1(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
            elif "mu4Tmu6" in self.config.L2Config.suffix:
                self.addL2Sequence(TE1,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu4_id_1')
                self.addL2Sequence(TE2,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu6_id_2')
                if "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs_tight(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_allModes(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu4_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_allModes_tightBc(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                else:
                    self.addL2Sequence(['L2_mu4_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_1(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
            elif "2mu6" in self.config.L2Config.suffix:
                self.addL2Sequence(TE1,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu6_id_1')
                self.addL2Sequence(TE2,
                               [TrigIDSCAN_Bphysics()],
                               'L2_mu6_id_2')
                if "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu6_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu6_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_BcMuMuDs_tight(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addL2Sequence(['L2_mu6_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_allModes(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addL2Sequence(['L2_mu6_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_allModes_tightBc(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')
                else:
                    self.addL2Sequence(['L2_mu6_id_1']+['L2_mu6_id_2'],
                                  [L2BMuMuXFex_1(),
                                    L2BMuMuXHypo_1()],
                                  'L2_')

                
        else: # we need a dummy seq
            self.addL2Sequence(self.inputTEs,
                               [],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            #set some properties if required
            EF_TE1=self.config.EFConfig.efTEs[0]
            EF_TE2=self.config.EFConfig.efTEs[1]
            if "2mu4" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu4_id_1')
                self.addEFSequence(EF_TE2,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu4_id_2')
                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                                      [EFBMuMuXFex_1()],
                                  'EF_dimu44_Bmumux_v2')
                        self.addEFSequence(['EF_dimu44_Bmumux_v2'],
                                  [EFBMuMuXHypo_1()],
                                  'EF_')
                elif "BcMuMuDs" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                        [EFBMuMuXFex_BcMuMuDs()],
                        'EF_dimu44_Bmumux_BcMuMuDs_loose')
                        self.addEFSequence(['EF_dimu44_Bmumux_BcMuMuDs_loose'],
                        [EFBMuMuXHypo_BcMuMuDs()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                        [EFBMuMuXFex_BcMuMuDs_tight()],
                        'EF_dimu44_Bmumux_BcMuMuDs_tight')
                        self.addEFSequence(['EF_dimu44_Bmumux_BcMuMuDs_tight'],
                        [EFBMuMuXHypo_BcMuMuDs_tight()],
                        'EF_')
                elif "v3" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                        [EFBMuMuXFex_allModes()],
                        'EF_dimu44_Bmumux_v3_loose')
                        self.addEFSequence(['EF_dimu44_Bmumux_v3_loose'],
                        [EFBMuMuXHypo_allModes()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                        [EFBMuMuXFex_allModes_tightBc()],
                        'EF_dimu44_Bmumux_v3_tight')
                        self.addEFSequence(['EF_dimu44_Bmumux_v3_tight'],
                        [EFBMuMuXHypo_allModes_tightBc()],
                        'EF_')
                else:
                    self.addEFSequence(['EF_mu4_id_1']+['EF_mu4_id_2'],
                                   [EFBMuMuFex_Jpsi()],
                                   'EF_dimu44_Bmumux')
                    self.addEFSequence(['EF_dimu44_Bmumux'],
                                   [EFBMuMuHypo_Jpsi()],
                                   'EF_')
            elif "mu4Tmu6" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu4_id_1')
                self.addEFSequence(EF_TE2,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu6_id_2')
                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                                      [EFBMuMuXFex_1()],
                                  'EF_dimu46_Bmumux_v2')
                        self.addEFSequence(['EF_dimu46_Bmumux_v2'],
                                  [EFBMuMuXHypo_1()],
                                  'EF_')
                elif "BcMuMuDs" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_BcMuMuDs()],
                        'EF_dimu46_Bmumux_BcMuMuDs_loose')
                        self.addEFSequence(['EF_dimu46_Bmumux_BcMuMuDs_loose'],
                        [EFBMuMuXHypo_BcMuMuDs()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_BcMuMuDs_tight()],
                        'EF_dimu46_Bmumux_BcMuMuDs_tight')
                        self.addEFSequence(['EF_dimu46_Bmumux_BcMuMuDs_tight'],
                        [EFBMuMuXHypo_BcMuMuDs_tight()],
                        'EF_')
                elif "v3" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_allModes()],
                        'EF_dimu46_Bmumux_v3_loose')
                        self.addEFSequence(['EF_dimu46_Bmumux_v3_loose'],
                        [EFBMuMuXHypo_allModes()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_allModes_tightBc()],
                        'EF_dimu46_Bmumux_v3_tight')
                        self.addEFSequence(['EF_dimu46_Bmumux_v3_tight'],
                        [EFBMuMuXHypo_allModes_tightBc()],
                        'EF_')
                else:
                    self.addEFSequence(['EF_mu4_id_1']+['EF_mu6_id_2'],
                                   [EFBMuMuFex_Jpsi()],
                                   'EF_dimu46_Bmumux')
                    self.addEFSequence(['EF_dimu46_Bmumux'],
                                   [EFBMuMuHypo_Jpsi()],
                                   'EF_')
            elif "2mu6" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu6_id_1')
                self.addEFSequence(EF_TE2,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               'EF_mu6_id_2')
                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                                      [EFBMuMuXFex_1()],
                                  'EF_dimu66_Bmumux_v2')
                        self.addEFSequence(['EF_dimu66_Bmumux_v2'],
                                  [EFBMuMuXHypo_1()],
                                  'EF_')
                elif "BcMuMuDs" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_BcMuMuDs()],
                        'EF_dimu66_Bmumux_BcMuMuDs_loose')
                        self.addEFSequence(['EF_dimu66_Bmumux_BcMuMuDs_loose'],
                        [EFBMuMuXHypo_BcMuMuDs()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_BcMuMuDs_tight()],
                        'EF_dimu66_Bmumux_BcMuMuDs_tight')
                        self.addEFSequence(['EF_dimu66_Bmumux_BcMuMuDs_tight'],
                        [EFBMuMuXHypo_BcMuMuDs_tight()],
                        'EF_')
                elif "v3" in self.config.EFConfig.suffix:
                    if "loose" in self.config.EFConfig.suffix:
                        self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_allModes()],
                        'EF_dimu66_Bmumux_v3_loose')
                        self.addEFSequence(['EF_dimu66_Bmumux_v3_loose'],
                        [EFBMuMuXHypo_allModes()],
                        'EF_')
                    #elif "tight" in self.config.EFConfig.suffix:
                    else:
                        self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                        [EFBMuMuXFex_allModes_tightBc()],
                        'EF_dimu66_Bmumux_v3_tight')
                        self.addEFSequence(['EF_dimu66_Bmumux_v3_tight'],
                        [EFBMuMuXHypo_allModes_tightBc()],
                        'EF_')
                else:
                    self.addEFSequence(['EF_mu6_id_1']+['EF_mu6_id_2'],
                                   [EFBMuMuFex_Jpsi()],
                                   'EF_dimu_Bmumux')
                    self.addEFSequence(['EF_dimu_Bmumux'],
                                   [EFBMuMuHypo_Jpsi()],
                                   'EF_')


        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if not "passL2" in self.config.L2Config.suffix:
            if "2mu4" in self.config.L2Config.suffix:
                self.addL2Signature(['L2_mu4_id_1']+['L2_mu4_id_2'])
                self.addL2Signature(['L2_'])
            if "mu4Tmu6" in self.config.L2Config.suffix:
                self.addL2Signature(['L2_mu4_id_1']+['L2_mu6_id_2'])
                self.addL2Signature(['L2_'])
            if "2mu6" in self.config.L2Config.suffix:
                self.addL2Signature(['L2_mu6_id_1']+['L2_mu6_id_2'])
                self.addL2Signature(['L2_'])
        #EF signatures
        if not "passEF" in self.config.EFConfig.suffix:
            if "2mu4" in self.config.EFConfig.suffix:
                self.addEFSignature(['EF_mu4_id_1']+['EF_mu4_id_2'])
                self.addEFSignature(['EF_'])
            if "mu4Tmu6" in self.config.EFConfig.suffix:
                self.addEFSignature(['EF_mu4_id_1']+['EF_mu6_id_2'])
                self.addEFSignature(['EF_'])
            if "2mu6" in self.config.EFConfig.suffix:
                self.addEFSignature(['EF_mu6_id_1']+['EF_mu6_id_2'])
                self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':   'L2_'+self.config.L2Config.suffix,
            'EF_':   'EF_'+self.config.EFConfig.suffix,
            }

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_MultiMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']


###########################################################################
#TopoDiMu_BMUMUX_L2Star
class L2EFChain_TopoDiMu_BMUMUX_L2Star(L2EFChain_Bphysics):

    def defineSequences(self, config):
        
        #L2 sequences
        TE1=self.inputTEs[0]
        TE2=self.inputTEs[1]

        if "L2StarA"  in self.config.L2Config.suffix:
            myTrackfinder = eval('TrigL2SiTrackFinder_BphysicsA()')
            myTEaddon = '_L2StarA'
        elif "L2StarB"  in self.config.L2Config.suffix:
            myTrackfinder = eval('TrigL2SiTrackFinder_BphysicsB()')
            myTEaddon = '_L2StarB'
        else:
            myTrackfinder = eval('TrigIDSCAN_Bphysics()')
            myTEaddon = ''


        if not "passL2" in self.config.L2Config.suffix:

            if "2mu4" in self.config.L2Config.suffix:
                TEout1 = 'L2_mu4_id_1%s' % myTEaddon
                TEout2 = 'L2_mu4_id_2%s' % myTEaddon
                
            elif "mu4Tmu6" in self.config.L2Config.suffix:
                TEout1 = 'L2_mu4_id_1%s' % myTEaddon
                TEout2 = 'L2_mu6_id_2%s' % myTEaddon
               
            elif "2mu6" in self.config.L2Config.suffix:
                TEout1 = 'L2_mu6_id_1%s' % myTEaddon
                TEout2 = 'L2_mu6_id_2%s' % myTEaddon



            self.addL2Sequence(TE1,
                               [myTrackfinder],
                               TEout1)
            self.addL2Sequence(TE2,
                               [myTrackfinder],
                               TEout2)        
            if "BcMuMuDs" in self.config.L2Config.suffix:
                if "loose" in self.config.L2Config.suffix:
                    self.addL2Sequence([TEout1]+[TEout2],
                                  [L2BMuMuXFex_BcMuMuDs(),
                                  L2BMuMuXHypo_1()],
                                  'L2_')
                #elif "tight" in self.config.L2Config.suffix:
                else:
                    self.addL2Sequence([TEout1]+[TEout2],
                                  [L2BMuMuXFex_BcMuMuDs_tight(),
                                  L2BMuMuXHypo_1()],
                                  'L2_')
            if "v3" in self.config.L2Config.suffix:
                if "loose" in self.config.L2Config.suffix:
                    self.addL2Sequence([TEout1]+[TEout2],
                                  [L2BMuMuXFex_allModes(),
                                  L2BMuMuXHypo_1()],
                                  'L2_')
                #elif "tight" in self.config.L2Config.suffix:
                else:
                    self.addL2Sequence([TEout1]+[TEout2],
                                  [L2BMuMuXFex_allModes_tightBc(),
                                  L2BMuMuXHypo_1()],
                                  'L2_')
            else:
                self.addL2Sequence([TEout1]+[TEout2],
                                  [L2BMuMuXFex_1(),
                                  L2BMuMuXHypo_1()],
                                  'L2_')


                
        else: # we need a dummy seq
            self.addL2Sequence(self.inputTEs,
                               [],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            #set some properties if required
            EF_TE1=self.config.EFConfig.efTEs[0]
            EF_TE2=self.config.EFConfig.efTEs[1]

            if "2mu4" in self.config.EFConfig.suffix:
                EFTEout1 = 'EF_mu4_id_1%s' % myTEaddon
                EFTEout2 = 'EF_mu4_id_2%s' % myTEaddon
                
            elif "mu4Tmu6" in self.config.EFConfig.suffix:
                EFTEout1 = 'EF_mu4_id_1%s' % myTEaddon
                EFTEout2 = 'EF_mu6_id_2%s' % myTEaddon
               
            elif "2mu6" in self.config.EFConfig.suffix:
                EFTEout1 = 'EF_mu6_id_1%s' % myTEaddon
                EFTEout2 = 'EF_mu6_id_2%s' % myTEaddon

        
            
            
            if "2mu4" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               EFTEout1)
                self.addEFSequence(EF_TE2,
                               TrigEFIDInsideOut_Bphysics().getSequence(),
                               EFTEout2)
                
                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_1()],
                                          'EF_dimu44_Bmumux_v2%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu44_Bmumux_v2%s' % myTEaddon],
                                          [EFBMuMuXHypo_1()],
                                          'EF_')
                elif "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs()],
                                          'EF_dimu44_Bmumux_BcMuMuDs_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu44_Bmumux_BcMuMuDs_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs_tight()],
                                          'EF_dimu44_Bmumux_BcMuMuDs_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu44_Bmumux_BcMuMuDs_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs_tight()],
                                          'EF_') 
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes()],
                                          'EF_dimu44_Bmumux_v3_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu44_Bmumux_v3_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes_tightBc()],
                                          'EF_dimu44_Bmumux_v3_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu44_Bmumux_v3_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes_tightBc()],
                                          'EF_') 
                else:
                    self.addEFSequence([EFTEout1]+[EFTEout2],
                                       [EFBMuMuFex_Jpsi()],
                                       'EF_dimu44_Bmumux_%s' % myTEaddon)
                    self.addEFSequence(['EF_dimu44_Bmumux_%s' % myTEaddon],
                                       [EFBMuMuHypo_Jpsi()],
                                       'EF_')
                    
            elif "mu4Tmu6" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   EFTEout1)            
                self.addEFSequence(EF_TE2,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   EFTEout2)

                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_1()],
                                          'EF_dimu46_Bmumux_v2%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu46_Bmumux_v2%s' % myTEaddon],
                                          [EFBMuMuXHypo_1()],
                                          'EF_')
                elif "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs()],
                                          'EF_dimu46_Bmumux_BcMuMuDs_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu46_Bmumux_BcMuMuDs_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs_tight()],
                                          'EF_dimu46_Bmumux_BcMuMuDs_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu46_Bmumux_BcMuMuDs_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs_tight()],
                                          'EF_') 
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes()],
                                          'EF_dimu46_Bmumux_v3_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu46_Bmumux_v3_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes_tightBc()],
                                          'EF_dimu46_Bmumux_v3_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu46_Bmumux_v3_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes_tightBc()],
                                          'EF_') 
                else:
                    self.addEFSequence([EFTEout1]+[EFTEout2],
                                       [EFBMuMuFex_Jpsi()],
                                       'EF_dimu46_Bmumux_%s' % myTEaddon)
                    self.addEFSequence(['EF_dimu46_Bmumux_%s' % myTEaddon],
                                       [EFBMuMuHypo_Jpsi()],
                                       'EF_')


            elif "2mu6" in self.config.EFConfig.suffix:
                self.addEFSequence(EF_TE1,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   EFTEout1)            
                self.addEFSequence(EF_TE2,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   EFTEout2)
                

                if "v2" in self.config.EFConfig.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_1()],
                                          'EF_dimu66_Bmumux_v2%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu66_Bmumux_v2%s' % myTEaddon],
                                          [EFBMuMuXHypo_1()],
                                          'EF_')
                elif "BcMuMuDs" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs()],
                                          'EF_dimu66_Bmumux_BcMuMuDs_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu66_Bmumux_BcMuMuDs_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_BcMuMuDs_tight()],
                                          'EF_dimu66_Bmumux_BcMuMuDs_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu66_Bmumux_BcMuMuDs_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_BcMuMuDs_tight()],
                                          'EF_') 
                elif "v3" in self.config.L2Config.suffix:
                    if "loose" in self.config.L2Config.suffix:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes()],
                                          'EF_dimu66_Bmumux_v3_loose%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu66_Bmumux_v3_loose%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes()],
                                          'EF_')
                    #elif "tight" in self.config.L2Config.suffix:
                    else:
                        self.addEFSequence([EFTEout1]+[EFTEout2],
                                          [EFBMuMuXFex_allModes_tightBc()],
                                          'EF_dimu66_Bmumux_v3_tight%s' % myTEaddon)
                        self.addEFSequence(['EF_dimu66_Bmumux_v3_tight%s' % myTEaddon],
                                          [EFBMuMuXHypo_allModes_tightBc()],
                                          'EF_') 
                else:
                    self.addEFSequence([EFTEout1]+[EFTEout2],
                                       [EFBMuMuFex_Jpsi()],
                                       'EF_dimu66_Bmumux_%s' % myTEaddon)
                    self.addEFSequence(['EF_dimu66_Bmumux_%s' % myTEaddon],
                                       [EFBMuMuHypo_Jpsi()],
                                       'EF_')


        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if not "passL2" in self.config.L2Config.suffix:
            if "L2StarA" in self.config.L2Config.suffix:
                if "2mu4" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1_L2StarA']+['L2_mu4_id_2_L2StarA'])
                    self.addL2Signature(['L2_'])
                if "mu4Tmu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1_L2StarA']+['L2_mu6_id_2_L2StarA'])
                    self.addL2Signature(['L2_'])
                if "2mu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu6_id_1_L2StarA']+['L2_mu6_id_2_L2StarA'])
                    self.addL2Signature(['L2_'])
            elif "L2StarB" in self.config.L2Config.suffix:
                if "2mu4" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1_L2StarB']+['L2_mu4_id_2_L2StarB'])
                    self.addL2Signature(['L2_'])
                if "mu4Tmu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1_L2StarB']+['L2_mu6_id_2_L2StarB'])
                    self.addL2Signature(['L2_'])
                if "2mu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu6_id_1_L2StarB']+['L2_mu6_id_2_L2StarB'])
                    self.addL2Signature(['L2_'])
            else:
                if "2mu4" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1']+['L2_mu4_id_2'])
                    self.addL2Signature(['L2_'])
                if "mu4Tmu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu4_id_1']+['L2_mu6_id_2'])
                    self.addL2Signature(['L2_'])
                if "2mu6" in self.config.L2Config.suffix:
                    self.addL2Signature(['L2_mu6_id_1']+['L2_mu6_id_2'])
                    self.addL2Signature(['L2_'])                                                
        #EF signatures
        if not "passEF" in self.config.EFConfig.suffix:
            if "L2StarA" in self.config.EFConfig.suffix:
                if "2mu4" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu4_id_1_L2StarA']+['EF_mu4_id_2_L2StarA'])
                    self.addEFSignature(['EF_'])
                if "mu4Tmu6" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu4_id_1_L2StarA']+['EF_mu6_id_2_L2StarA'])
                    self.addEFSignature(['EF_'])
                if "2mu6" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu6_id_1_L2StarA']+['EF_mu6_id_2_L2StarA'])
                    self.addEFSignature(['EF_'])
            elif "L2StarB" in self.config.EFConfig.suffix:
                if "2mu4" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu4_id_1_L2StarB']+['EF_mu4_id_2_L2StarB'])
                    self.addEFSignature(['EF_'])
                if "mu4Tmu6" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu4_id_1_L2StarB']+['EF_mu6_id_2_L2StarB'])
                    self.addEFSignature(['EF_'])
                if "2mu6" in self.config.EFConfig.suffix:
                    self.addEFSignature(['EF_mu6_id_1_L2StarB']+['EF_mu6_id_2_L2StarB'])
                    self.addEFSignature(['EF_'])



    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':   'L2_'+self.config.L2Config.suffix,
            'EF_':   'EF_'+self.config.EFConfig.suffix,
            }

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_MultiMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']



if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_BMUMUX('2mu4_Bmumux',
                                  'L2_2mu4_Bmumux', 166, 'L1_2MU0', 'EF_2mu4_Bmumux', 166, ['L2_mu4','L2_mu4'],
                                  Config(L2create('2mu4_Bmumux'),
                                         EFcreate('2mu4_Bmumux',TEs=['EF_mu4','EF_mu4']))),
        L2EFChain_TopoDiMu_BMUMUX('mu4mu6_Bmumux',
                                  'L2_mu4mu6_Bmumux', 594, mu4mu6Seed, 'EF_mu4mu6_Bmumux', 594, ['L2_mu4','L2_mu6'],
                                  Config(L2create('mu4mu6_Bmumux'),
                                         EFcreate('mu4mu6_Bmumux',TEs=['EF_mu4','EF_mu6']))),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    Bphysics += [
        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux',
                                  'L2_2mu4T_Bmumux', 166, 'L1_2MU4', 'EF_2mu4T_Bmumux', 166, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux'),
                                         EFcreate('2mu4T_Bmumux',TEs=['EF_mu4T','EF_mu4T']))),    
        L2EFChain_TopoDiMu_BMUMUX('mu4Tmu6_Bmumux',
                                  'L2_mu4Tmu6_Bmumux', 594, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumux', 594, ['L2_mu4T','L2_mu6'],
                                  Config(L2create('mu4Tmu6_Bmumux'),
                                         EFcreate('mu4Tmu6_Bmumux',TEs=['EF_mu4T','EF_mu6']))),

        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux_Barrel',
                                  'L2_2mu4T_Bmumux_Barrel', 237,  _2mu4TBarrelSeed, 'EF_2mu4T_Bmumux_Barrel', 237, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux_Barrel'),
                                         EFcreate('2mu4T_Bmumux_Barrel',TEs=['EF_mu4T','EF_mu4T']))),
        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux_BarrelOnly',
                                  'L2_2mu4T_Bmumux_BarrelOnly', 267, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumux_BarrelOnly', 267, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux_BarrelOnly'),
                                         EFcreate('2mu4T_Bmumux_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),    
        L2EFChain_TopoDiMu_BMUMUX('mu4Tmu6_Bmumux_Barrel',
                                  'L2_mu4Tmu6_Bmumux_Barrel', 597, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumux_Barrel', 597, ['L2_mu4T','L2_mu6'],
                                  Config(L2create('mu4Tmu6_Bmumux_Barrel'),
                                         EFcreate('mu4Tmu6_Bmumux_Barrel',TEs=['EF_mu4T','EF_mu6']))),    

        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux_v2_Barrel',
                                  'L2_2mu4T_Bmumux_v2_Barrel', 3216,  _2mu4TBarrelSeed, 'EF_2mu4T_Bmumux_v2_Barrel', 3216, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux_v2_Barrel'),
                                         EFcreate('2mu4T_Bmumux_v2_Barrel',TEs=['EF_mu4T','EF_mu4T']))),
        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux_v2_BarrelOnly',
                                  'L2_2mu4T_Bmumux_v2_BarrelOnly', 3217, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumux_v2_BarrelOnly', 3217, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux_v2_BarrelOnly'),
                                         EFcreate('2mu4T_Bmumux_v2_BarrelOnly',TEs=['EF_mu4T','EF_mu4T']))),    
        L2EFChain_TopoDiMu_BMUMUX('mu4Tmu6_Bmumux_v2_Barrel',
                                  'L2_mu4Tmu6_Bmumux_v2_Barrel', 3218, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumux_v2_Barrel', 3218, ['L2_mu4T','L2_mu6'],
                                  Config(L2create('mu4Tmu6_Bmumux_v2_Barrel'),
                                         EFcreate('mu4Tmu6_Bmumux_v2_Barrel',TEs=['EF_mu4T','EF_mu6']))),    

        L2EFChain_TopoDiMu_BMUMUX('2mu4T_Bmumux_v2',
                                  'L2_2mu4T_Bmumux_v2', 5176, 'L1_2MU4', 'EF_2mu4T_Bmumux_v2', 5176, ['L2_mu4T','L2_mu4T'],
                                  Config(L2create('2mu4T_Bmumux_v2'),
                                         EFcreate('2mu4T_Bmumux_v2',TEs=['EF_mu4T','EF_mu4T']))),    

        L2EFChain_TopoDiMu_BMUMUX('mu4Tmu6_Bmumux_v2',
                                  'L2_mu4Tmu6_Bmumux_v2', 5177, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumux_v2', 5177, ['L2_mu4T','L2_mu6'],
                                  Config(L2create('mu4Tmu6_Bmumux_v2'),
                                         EFcreate('mu4Tmu6_Bmumux_v2',TEs=['EF_mu4T','EF_mu6']))),

        ]

Bphysics += [
    L2EFChain_TopoDiMu_BMUMUX('2mu6_Bmumux',
                             'L2_2mu6_Bmumux', 165, 'L1_2MU6', 'EF_2mu6_Bmumux', 165, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_Bmumux'),
                                    EFcreate('2mu6_Bmumux',TEs=['EF_mu6','EF_mu6']))),
    L2EFChain_TopoDiMu_BMUMUX('2mu6_Bmumux_v2',
                             'L2_2mu6_Bmumux_v2', 5178, 'L1_2MU6', 'EF_2mu6_Bmumux_v2', 5178, ['L2_mu6','L2_mu6'],
                             Config(L2create('2mu6_Bmumux_v2'),
                                    EFcreate('2mu6_Bmumux_v2',TEs=['EF_mu6','EF_mu6']))),

    ]

Bphysics += [
    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu4T_Bmumux_L2StarB',
                              'L2_2mu4T_Bmumux_L2StarB', 204, 'L1_2MU4', 'EF_2mu4T_Bmumux_L2StarB', 204, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                              Config(L2create('2mu4T_Bmumux_L2StarB'),
                                     EFcreate('2mu4T_Bmumux_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    

    
    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_L2StarA',
                              'L2_2mu6_Bmumux_L2StarA', 152, 'L1_2MU6', 'EF_2mu6_Bmumux_L2StarA', 152, ['L2_mu6','L2_mu6'],
                              Config(L2create('2mu6_Bmumux_L2StarA'),
                                     EFcreate('2mu6_Bmumux_L2StarA',TEs=['EF_mu6','EF_mu6']))),

    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_v2_L2StarA',
                              'L2_2mu6_Bmumux_v2_L2StarA', 3214, 'L1_2MU6', 'EF_2mu6_Bmumux_v2_L2StarA', 3214, ['L2_mu6','L2_mu6'],
                              Config(L2create('2mu6_Bmumux_v2_L2StarA'),
                                     EFcreate('2mu6_Bmumux_v2_L2StarA',TEs=['EF_mu6','EF_mu6']))),



    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_L2StarB',
                              'L2_2mu6_Bmumux_L2StarB', 1521, 'L1_2MU6', 'EF_2mu6_Bmumux_L2StarB', 1521, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_L2StarB'),
                                     EFcreate('2mu6_Bmumux_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),


    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_v2_L2StarB',
                              'L2_2mu6_Bmumux_v2_L2StarB', 926, 'L1_2MU6', 'EF_2mu6_Bmumux_v2_L2StarB', 926, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_v2_L2StarB'),
                                     EFcreate('2mu6_Bmumux_v2_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_BcMuMuDs_tight_L2StarB',
                              'L2_2mu6_Bmumux_BcMuMuDs_tight_L2StarB', 7286, 'L1_2MU6', 'EF_2mu6_Bmumux_BcMuMuDs_tight_L2StarB', 7286, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_BcMuMuDs_tight_L2StarB'),
                                     EFcreate('2mu6_Bmumux_BcMuMuDs_tight_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_v3_tight_L2StarB',
                              'L2_2mu6_Bmumux_v3_tight_L2StarB', 7298, 'L1_2MU6', 'EF_2mu6_Bmumux_v3_tight_L2StarB', 7298, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_v3_tight_L2StarB'),
                                     EFcreate('2mu6_Bmumux_v3_tight_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_BcMuMuDs_loose_L2StarB',
                              'L2_2mu6_Bmumux_BcMuMuDs_loose_L2StarB',24 , 'L1_2MU6', 'EF_2mu6_Bmumux_BcMuMuDs_loose_L2StarB', 24, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_BcMuMuDs_loose_L2StarB'),
                                     EFcreate('2mu6_Bmumux_BcMuMuDs_loose_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),

    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu6_Bmumux_v3_loose_L2StarB',
                              'L2_2mu6_Bmumux_v3_loose_L2StarB', 7304, 'L1_2MU6', 'EF_2mu6_Bmumux_v3_loose_L2StarB', 7304, ['L2_mu6_L2StarB','L2_mu6_L2StarB'],
                              Config(L2create('2mu6_Bmumux_v3_loose_L2StarB'),
                                     EFcreate('2mu6_Bmumux_v3_loose_L2StarB',TEs=['EF_mu6_L2StarB','EF_mu6_L2StarB']))),


    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu4T_Bmumux_Barrel_v2_L2StarB',
                              'L2_2mu4T_Bmumux_Barrel_v2_L2StarB', 587,  _2mu4TBarrelSeed, 'EF_2mu4T_Bmumux_Barrel_v2_L2StarB', 587, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                              Config(L2create('2mu4T_Bmumux_Barrel_v2_L2StarB'),
                                     EFcreate('2mu4T_Bmumux_Barrel_v2_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),
    
    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu4T_Bmumux_BarrelOnly_v2_L2StarB',
                              'L2_2mu4T_Bmumux_BarrelOnly_v2_L2StarB', 588, _2mu4TBarrelOnlySeed, 'EF_2mu4T_Bmumux_BarrelOnly_v2_L2StarB', 588, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                              Config(L2create('2mu4T_Bmumux_BarrelOnly_v2_L2StarB'),
                                     EFcreate('2mu4T_Bmumux_BarrelOnly_v2_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),
    
    L2EFChain_TopoDiMu_BMUMUX_L2Star('mu4Tmu6_Bmumux_v2_L2StarB',
                              'L2_mu4Tmu6_Bmumux_v2_L2StarB', 447, mu4Tmu6Seed, 'EF_mu4Tmu6_Bmumux_v2_L2StarB', 447, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                  Config(L2create('mu4Tmu6_Bmumux_v2_L2StarB'),
                                         EFcreate('mu4Tmu6_Bmumux_v2_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),    

    L2EFChain_TopoDiMu_BMUMUX_L2Star('mu4Tmu6_Bmumux_Barrel_v2_L2StarB',
                              'L2_mu4Tmu6_Bmumux_Barrel_v2_L2StarB', 880, _mu4Tmu6BarrelSeed, 'EF_mu4Tmu6_Bmumux_Barrel_v2_L2StarB', 880, ['L2_mu4T_L2StarB','L2_mu6_L2StarB'],
                                  Config(L2create('mu4Tmu6_Bmumux_Barrel_v2_L2StarB'),
                                         EFcreate('mu4Tmu6_Bmumux_Barrel_v2_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu6_L2StarB']))),    
    
    L2EFChain_TopoDiMu_BMUMUX_L2Star('2mu4T_Bmumux_v2_L2StarB',
                              'L2_2mu4T_Bmumux_v2_L2StarB', 485, 'L1_2MU4', 'EF_2mu4T_Bmumux_v2_L2StarB', 485, ['L2_mu4T_L2StarB','L2_mu4T_L2StarB'],
                              Config(L2create('2mu4T_Bmumux_v2_L2StarB'),
                                     EFcreate('2mu4T_Bmumux_v2_L2StarB',TEs=['EF_mu4T_L2StarB','EF_mu4T_L2StarB']))),    

    
    ]

###########################################################################
#NEXT CLASSES ARE ALL MUON ROI TYPE using MuGirl
###########################################################################
#MuGirlROI_JPSI
class L2EFChain_MuGirlROI_JPSI(L2EFChain_Bphysics):

    def defineSequences(self, config):
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence(self.inputTEs,[ config.L2Config.L2_Dummy ],'L2_stall_')
            self.addL2Sequence('L2_stall_',
                               [TrigIDSCAN_Bphysics(),
                                TrigDiMuon_RoI(),
                                TrigL2DiMuHypo_Jpsi()],
                               'L2_')

        else: # we need a dummy seq
            self.addL2Sequence(self.inputTEs,
                               [],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            if not "passL2" in self.config.L2Config.suffix:
                self.addEFSequence('L2_',
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_id')
            else: #seed from L1 threshold
                self.addEFSequence(self.inputTEs,
                                   TrigEFIDInsideOut_Bphysics().getSequence(),
                                   'EF_id')

            self.addEFSequence('EF_id',
                               [TrigMuGirlConfig('test')],
                               'EF_Fex')

            self.addEFSequence('EF_Fex',
                               [TrigEFDiMuFexConfig("MG"),
                                TrigEFDiMuHypo_Jpsi()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if not "passL2" in self.config.L2Config.suffix:
            if self.config.L2Config.l2confirmmuon is not None:
                self.addL2Signature([self.config.L2Config.l2confirmmuon])
            self.addL2Signature(['L2_stall_'])
            self.addL2Signature(['L2_'])
        #EF signatures
        if self.config.EFConfig.efconfirmmuon is not None:
            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSignature(['EF_id'])
            self.addEFSignature(['EF_Fex'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_stall_':   'L2_stall_'+self.config.L2Config.suffix,
            'L2_':   'L2_'+self.config.L2Config.suffix,
            'EF_id': 'EF_'+self.config.EFConfig.suffix+'_id',
            'EF_Fex': 'EF_'+self.config.EFConfig.suffix+'_Fex',
            'EF_':   'EF_'+self.config.EFConfig.suffix,
            }

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']
        
###################################################################################
#MuGirlRoI_DIMU - inherits from L2EFChain_MuGirlROI_JPSI above - just swap the hypos
#
class L2EFChain_MuGirlROI_DIMU(L2EFChain_MuGirlROI_JPSI):

  def defineSequences(self, config):
        L2EFChain_MuGirlROI_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[1].algos[2] = TrigL2DiMuHypo_DiMu()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[2].algos[1] = TrigEFDiMuHypo_DiMu()
        pass

Bphysics += [
    L2EFChain_MuGirlROI_DIMU('mu4_DiMu_MG',
                           'L2_mu4_DiMu_MG', 490, 'L1_MU0', 'EF_mu4_DiMu_MG', 490, ['MU0'],
                           Config(L2create('mu4_DiMu_MG',confirmmuon='L2_mu4'),
                                  EFcreate('mu4_DiMu_MG',confirmmuon='EF_mu4'))),
]
###########################################################################
#NEXT CLASSES ARE ALL MuGirl FS TYPE
###########################################################################
#MuGirlFS_JPSI
class L2EFChain_MuGirlFS_JPSI(L2EFChain_Bphysics):

    def defineSequences(self, config):
        #L2 sequences
        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Sequence("",
                               [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                                TrigIDSCAN_FullScan()],
                               'L2_FStracks')

            self.addL2Sequence(self.inputTEs+['L2_FStracks'],
                               [PESA__DummyCombineAlgo("L2_comb_mu_FStracks")],
                               'L2_comb')

            self.addL2Sequence('L2_comb',
                               [ TrigDiMuon_FS(),
                                 TrigL2DiMuHypo_Jpsi()],
                               'L2_')

        else: # we need a dummy seq
            self.addL2Sequence("",
                               [],
                               'L2_')

        #EF sequences
        if not "passEF" in self.config.EFConfig.suffix:
            #set some properties if required

            self.addEFSequence("",
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                               TrigEFIDInsideOut_FullScan().getSequence(),
                               'EF_FStracks')

            self.addEFSequence(['L2_']+['EF_FStracks'],
                               [PESA__DummyCombineAlgo("EF_comb_mu_FStracks")],
                               'EF_comb')

            self.addEFSequence('EF_comb',
                               [TrigMuGirlConfig('test')],
                               'EF_Fex')


            self.addEFSequence('EF_Fex',
                               [TrigEFDiMuFexConfig("MG_FS"),
                                TrigEFDiMuHypo_Jpsi()],
                               'EF_')

        else: # we need a dummy seq
            self.addEFSequence('L2_',
                               [],
                               'EF_')

    def defineSignatures(self):
        #L2 signatures
        if self.config.L2Config.l2confirmmuon is not None:
            self.addL2Signature([self.config.L2Config.l2confirmmuon])
        if not "passL2" in self.config.L2Config.suffix:
            self.addL2Signature(['L2_FStracks'])
            self.addL2Signature(['L2_comb'])
            self.addL2Signature(['L2_'])
        #EF signatures
        if self.config.EFConfig.efconfirmmuon is not None:
            self.addEFSignature([self.config.EFConfig.efconfirmmuon])
        if not "passEF" in self.config.EFConfig.suffix:
            self.addEFSignature(['EF_FStracks'])
            self.addEFSignature(['EF_comb'])
            self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_FStracks': 'L2_FStracks',
            'L2_comb':     'L2_'+self.config.L2Config.suffix+'_FStracks',
            'L2_':         'L2_'+self.config.L2Config.suffix,
            'EF_FStracks': 'EF_FStracks',
            'EF_comb':     'EF_'+self.config.EFConfig.suffix+'_FStracks',
            'EF_Fex':      'EF_'+self.config.EFConfig.suffix+'_Fex',
            'EF_':         'EF_'+self.config.EFConfig.suffix,
            }
        if 'SiTrk' in self.config.L2Config.suffix: #here we modify the dictionary
            self.TErenamingMap['L2_FStracks'] = 'L2_FStracks_SiTrk'
        pass

    def defineStreamGroupTriggerType(self):
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Bphysics','BW:Bphys']
        else:
            self.groups = ['RATE:Bphysics_SingleMuon','BW:Bphys']
        self.physics_streams = ['bphysics']
        self.calib_streams = []
        self.trigger_type = ['hello']
        
###################################################################################
#MuGirlFS_DIMU - inherits from L2EFChain_MuGirlFS_JPSI above - just swap the hypos
class L2EFChain_MuGirlFS_DIMU(L2EFChain_MuGirlFS_JPSI):

  def defineSequences(self, config):
        L2EFChain_MuGirlFS_JPSI.defineSequences(self, config)
        if not "passL2" in self.config.L2Config.suffix:
            self.l2chain.sequences[2].algos[1] = TrigL2DiMuHypo_DiMu()
        if not "passEF" in self.config.EFConfig.suffix:
            self.efchain.sequences[3].algos[1] = TrigEFDiMuHypo_DiMu()
        pass

Bphysics += [
    L2EFChain_MuGirlFS_DIMU('mu4_DiMu_MG_FS',
                           'L2_mu4_DiMu_MG_FS', 615, 'L1_MU0', 'EF_mu4_DiMu_MG_FS', 615, ['MU0'],
                           Config(L2create('mu4_DiMu_MG_FS',confirmmuon='L2_mu4'),
                                  EFcreate('mu4_DiMu_MG_FS',confirmmuon='EF_mu4'))),
]
