# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Unseeded Egamma trigger slice signatures  """

__author__  = 'R.Goncalo, P.Urquijo'
__version__=""
__doc__="Implementation of unseeded egamma signatures"

from string import atoi
from AthenaCommon.Logging import logging

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.HltConfig import *

# definde seed for HI
minbias_hi_seed = "L1_ZDC_A_C"

# get L2 algorithm configurables
if TriggerFlags.doLVL2() or True:
    # unseeded allTE algo to start chain from MBTS
    from TrigGenericAlgs.TrigGenericAlgsConf import  PESA__DummyUnseededAllTEAlgo
    theDummyUnseededAllTEAlgo =  PESA__DummyUnseededAllTEAlgo("L2DummyAlgo")
    
    # calorimeter
    if TriggerFlags.doCalo or  True:
      from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_MBTSA,T2CaloEgamma_MBTSC
      theT2CaloEgamma_MBTSA = T2CaloEgamma_MBTSA()
      theT2CaloEgamma_MBTSC = T2CaloEgamma_MBTSC()

    # Fex to create TrigElectrons and TrigPhotons
    from TrigEgammaHypo.TrigL2PhotonFexConfig import *
    from TrigEgammaHypo.TrigL2ElectronFexConfig import *

    # hypothesis algorithms
    from TrigEgammaHypo.TrigL2ElectronHypoConfig import *    
    from TrigEgammaHypo.TrigL2PhotonHypoConfig import *    
    from TrigEgammaHypo.TrigL2CaloHypoConfig import *

# get EF algorithm configurables    
if TriggerFlags.doEF() or True:
    if TriggerFlags.doCalo or True:
      from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw, TrigCaloCellMaker_fullcalo, TrigCaloClusterMaker_slw_fullcalo
      theTrigCaloCellMaker_eGamma     = TrigCaloCellMaker_eGamma()
      theTrigCaloCellMaker_fullcalo   = TrigCaloCellMaker_fullcalo()
      theTrigCaloTowerMaker_eGamma    = TrigCaloTowerMaker_eGamma()
      theTrigCaloClusterMaker_slw     = TrigCaloClusterMaker_slw()
      theTrigCaloClusterMaker_slw_fullcalo   = TrigCaloClusterMaker_slw_fullcalo()

    if TriggerFlags.doID or True:
      from InDetTrigRecExample.EFInDetConfig import *
      theTrigEFIDCombined_Photon      = TrigEFIDCombined_Photon().getSequence()

    from TrigEgammaRec.TrigEgammaRecConfig import *
    theTrigEgammaRec_eGamma           = TrigEgammaRec_eGamma()
    theTrigEgammaRec_NoIDEF_eGamma         = TrigEgammaRec_NoIDEF_eGamma()
    
    from TrigEgammaHypo.TrigEFPhotonHypoConfig import *
    from TrigEgammaHypo.TrigEFElectronHypoConfig import *
    from TrigEgammaHypo.TrigEFTrackHypoConfig import *

class L2EFChain_Unseeded_g(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix,
                     TrigL2CaloHypo_1=None, TrigL2PhotonFex_1=None, TrigL2PhotonHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_'+suffix)
            self.TrigL2PhotonFex_1  = TrigL2PhotonFex_1  or TrigL2PhotonFex('TrigL2PhotonFex_'+suffix)
            self.TrigL2PhotonHypo_1 = TrigL2PhotonHypo_1 or TrigL2PhotonHypo('TrigL2PhotonHypo_'+suffix)
            pass
        
    class EFConfig:
        def __init__(self, suffix, TrigEFPhotonHypo_1=None):
            self.suffix = suffix
            self.TrigEFPhotonHypo_1  = TrigEFPhotonHypo_1  or TrigEFPhotonHypo('EFPhotonHypo_'+suffix)
            pass

    class Config:
        def __init__(self, l2config, efconfig):
            self.suffix =''
            self.L2Config = l2config
            self.EFConfig = efconfig

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter, 
                 l2_inputTEs, config):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter, 
                              l2_inputTEs, config)

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['egamma']
        self.calib_streams = []
        self.groups = ['RATE:SinglePhoton']
        self.trigger_type = []


class L2EFChain_Unseeded_g_MBTSA_L2(L2EFChain_Unseeded_g):
    
    def defineSequences(self, config):
        ###### L2 ######
        #Step 1: AllTEAlgo dummy to start the chain
        self.addL2Sequence(self.inputTEs,
                           [theDummyUnseededAllTEAlgo],
                           'L2_e_step1')
        # Step 2: Calo Fex and Hypo
        self.addL2Sequence('L2_e_step1',
                           [theT2CaloEgamma_MBTSA, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step2')
        # Step 3: Photon Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2PhotonFex_1, config.L2Config.TrigL2PhotonHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDCombined_Photon,
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_e_step3')
        

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addL2Signature(['L2_e_step2'])
        self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'_te'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'_calo'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'_id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }

class L2EFChain_Unseeded_g_MBTSC_L2(L2EFChain_Unseeded_g_MBTSA_L2):
    
    def defineSequences(self, config):
        ###### L2 ######
        #Step 1: AllTEAlgo dummy to start the chain
        self.addL2Sequence(self.inputTEs,
                           [theDummyUnseededAllTEAlgo],
                           'L2_e_step1')
        
        # Step 2: Calo Fex and Hypo
        self.addL2Sequence('L2_e_step1',
                           [theT2CaloEgamma_MBTSC, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step2')
        
        # Step 3: Photon Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2PhotonFex_1, config.L2Config.TrigL2PhotonHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDCombined_Photon,
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_e_step3')
        
class L2EFChain_Unseeded_g_EF(L2EFChain_Unseeded_g):
    
    def defineSequences(self, config):
        ###### L2 ######
        #Step 1: AllTEAlgo dummy to start the chain
        self.addL2Sequence(self.inputTEs,
                           [theDummyUnseededAllTEAlgo],
                           'L2_e_step1')
        
    ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step1', 
                           [theTrigCaloCellMaker_fullcalo],
                           'EF_e_step1a')
        self.addEFSequence('EF_e_step1a', 
                           [theTrigCaloTowerMaker_eGamma],
                           'EF_e_step1b')
        self.addEFSequence('EF_e_step1b', 
                           [theTrigCaloClusterMaker_slw_fullcalo],
                           'EF_e_step1c')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1c', 
                           theTrigEFIDCombined_Photon,
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_e_step3')
        

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addEFSignature(['EF_e_step1a'])
        self.addEFSignature(['EF_e_step1b'])
        self.addEFSignature(['EF_e_step1c'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1':  mergeRemovingOverlap('L2_', self.sig_id+'_dummy'),
            'EF_e_step1a': mergeRemovingOverlap('EF_', self.sig_id+'_caloa'),
            'EF_e_step1b': mergeRemovingOverlap('EF_', self.sig_id+'_calob'),
            'EF_e_step1c': mergeRemovingOverlap('EF_', self.sig_id+'_caloc'),
            'EF_e_step2':  mergeRemovingOverlap('EF_', self.sig_id+'_id'),
            'EF_e_step3':  mergeRemovingOverlap('EF_', self.sig_id),
            }

class L2EFChain_Unseeded_g_EF_HI(L2EFChain_Unseeded_g):
    
    def defineSequences(self, config):
        ###### L2 ######
        #Step 1: AllTEAlgo dummy to start the chain
        self.addL2Sequence(self.inputTEs,
                           [theDummyUnseededAllTEAlgo],
                           'L2_e_step1')
        
    ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step1', 
                           [theTrigCaloCellMaker_fullcalo],
                           'EF_e_step1a')
        self.addEFSequence('EF_e_step1a', 
                           [theTrigCaloTowerMaker_eGamma],
                           'EF_e_step1b')
        self.addEFSequence('EF_e_step1b', 
                           [theTrigCaloClusterMaker_slw_fullcalo],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_NoIDEF_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_e_step3')
        

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addEFSignature(['EF_e_step1a'])
        self.addEFSignature(['EF_e_step1b'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1':  mergeRemovingOverlap('L2_', self.sig_id+'_dummy'),
            'EF_e_step1a': mergeRemovingOverlap('EF_', self.sig_id+'_caloa'),
            'EF_e_step1b': mergeRemovingOverlap('EF_', self.sig_id+'_calob'),
            'EF_e_step2':  mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_e_step3':  mergeRemovingOverlap('EF_', self.sig_id),
            }
        

################################################################

UnseededPhotons=[
    ##L2 T2CaloEgamma configured for MBTS
    L2EFChain_Unseeded_g_MBTSA_L2('g0_mbts_a', 'L2_g0_mbts_a', 498, 'L1_MBTS_A', 'EF_g0_mbts_a', 498, ['MBTS_A'],
                                  L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                               L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),
    L2EFChain_Unseeded_g_MBTSC_L2('g0_mbts_c', 'L2_g0_mbts_c', 499, 'L1_MBTS_C', 'EF_g0_mbts_c', 499, ['MBTS_C'],
                                  L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                               L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),

    L2EFChain_Unseeded_g_MBTSA_L2('g0_mbts_a_cosmic', 'L2_g0_mbts_a_cosmic', 732, 'L1_MBTS_A_EMPTY', 'EF_g0_mbts_a_cosmic', 732, ['MBTS_A'],
                                  L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                               L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),
    L2EFChain_Unseeded_g_MBTSC_L2('g0_mbts_c_cosmic', 'L2_g0_mbts_c_cosmic', 733, 'L1_MBTS_C_EMPTY', 'EF_g0_mbts_c_cosmic', 733, ['MBTS_C'],
                                  L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                               L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),


    #HI config
    ##EF full calorimeter scan
    L2EFChain_Unseeded_g_EF_HI('g5_loose_EFFS_L1ZDC', 'L2_g5_loose_EFFS_L1ZDC', 515, minbias_hi_seed, 'EF_g5_loose_EFFS_L1ZDC', 515, ['ZDC_AND'],
                               L2EFChain_Unseeded_g_EF_HI.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                                  L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_loose()))),

    ##EF full calorimeter scan
    L2EFChain_Unseeded_g_EF('g0_mbts_a_EFfullcalo', 'L2_g0_mbts_a_EFfullcalo', 515, 'L1_MBTS_1_1', 'EF_g0_mbts_a_EFfullcalo', 515, ['MBTS_A'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),
    L2EFChain_Unseeded_g_EF('g0_mbts_c_EFfullcalo', 'L2_g0_mbts_c_EFfullcalo', 516, 'L1_MBTS_C', 'EF_g0_mbts_c_EFfullcalo', 516, ['MBTS_C'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),

    L2EFChain_Unseeded_g_EF('g0_mbts_a_EFfullcalo_cosmic', 'L2_g0_mbts_a_EFfullcalo_cosmic', 734, 'L1_MBTS_A_EMPTY', 'EF_g0_mbts_a_EFfullcalo_cosmic', 734, ['MBTS_A'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),
    L2EFChain_Unseeded_g_EF('g0_mbts_c_EFfullcalo_cosmic', 'L2_g0_mbts_c_EFfullcalo_cosmic', 735, 'L1_MBTS_C_EMPTY', 'EF_g0_mbts_c_EFfullcalo_cosmic', 735, ['MBTS_C'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),
    
    L2EFChain_Unseeded_g_EF('g5_EFfullcalo',        'L2_g5_EFfullcalo',        517, 'L1_EM3',           'EF_g5_EFfullcalo',       517, ['EM3'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut()))),

    L2EFChain_Unseeded_g_EF('g5_EFfullcalo_cosmic',        'L2_g5_EFfullcalo_cosmic',        730, 'L1_EM3_EMPTY',           'EF_g5_EFfullcalo_cosmic',       730, ['EM3'],
                            L2EFChain_Unseeded_g.Config( L2EFChain_Unseeded_g.L2Config('',L2CaloHypo_g4(),L2PhotonFex_1(),L2PhotonHypo_g5_nocut()),
                                                         L2EFChain_Unseeded_g.EFConfig('',EFPhotonHypo_g5_nocut())))
    ]

