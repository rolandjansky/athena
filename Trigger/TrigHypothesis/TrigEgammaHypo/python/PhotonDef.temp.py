# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Photon trigger slice signatures  """

__author__  = 'Vu Anh Tuan, P. Urquijo'
__version__=""
__doc__="Implementation of Photon Slice single photon signatures"

from string import atoi
from AthenaCommon.Logging import logging

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.HltConfig import *

if TriggerFlags.doLVL2() or True:
    if TriggerFlags.doCalo or True:
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma             = T2CaloEgamma_eGamma()

    from TrigEgammaHypo.TrigL2CaloHypoConfig import *
    from TrigEgammaHypo.TrigL2PhotonFexConfig import *
    from TrigEgammaHypo.TrigL2PhotonHypoConfig import *
    from TrigDetCalib.TrigDetCalibConfig import *
    theLArL2ROBListWriter                  = LArL2ROBListWriter()


if TriggerFlags.doEF() or True:
    if TriggerFlags.doCalo or True:
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma        = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma       = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw        = TrigCaloClusterMaker_slw()

    if TriggerFlags.doID or True:
        from InDetTrigRecExample.EFInDetConfig import *

    from TrigEgammaRec.TrigEgammaRecConfig import *
    theTrigEgammaRec_PhotonConversions     = TrigEgammaRec_PhotonConversions()
    theTrigEgammaRec_NoIDEF_eGamma         = TrigEgammaRec_NoIDEF_eGamma()
    theTrigEFIDCombined_Photon             = TrigEFIDCombined_Photon().getSequence()

    from TrigEgammaHypo.TrigEFPhotonHypoConfig import *

##################
#
# L2 & EF Chain configuration
#
##################

# default class, no ID at EF level

#-----------------------------------
class L2EFChain_g(L2EFChainDef):
#-----------------------------------
    
    class L2Config:
        def __init__(self, suffix,
                     TrigL2CaloHypo_1=None, TrigL2PhotonFex_1=None, TrigL2PhotonHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_'+suffix)
            self.TrigL2PhotonFex_1  = TrigL2PhotonFex_1  or TrigL2PhotonFex('TrigL2PhotonFex_' + suffix)
            self.TrigL2PhotonHypo_1 = TrigL2PhotonHypo_1 or TrigL2PhotonHypo('TrigL2PhotonHypo_' + suffix)
            pass
        
    class EFConfig:
        def __init__(self, suffix, TrigEFPhotonHypo_1=None):
            self.suffix = suffix
            self.TrigEFPhotonHypo_1 = TrigEFPhotonHypo_1 or TrigEFPhotonHypo('EFPhotonHypo_' + suffix)
            pass
        
    class Config:
        def __init__(self, l2config, efconfig):
            self.suffix = ''
            self.L2Config = l2config
            self.EFConfig = efconfig
            pass

    def __init__(self, sig_id, 
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter, 
                 l2_inputTEs, config):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter, 
                              l2_inputTEs, config)

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_g_step1')

        # Step 2: Photon Fex and Hypo
        self.addL2Sequence('L2_g_step1',
                           [config.L2Config.TrigL2PhotonFex_1, config.L2Config.TrigL2PhotonHypo_1],
                           'L2_g_step2')
        
        ###### EF ######
        # Step 1: Calo cluster
        self.addEFSequence('L2_g_step2', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_g_step1')
        
        # Step 3: Calo cluster and Hypo
        self.addEFSequence('EF_g_step1', 
                           [theTrigEgammaRec_NoIDEF_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_g_step2')
        
    def defineSignatures(self):
        self.addL2Signature(['L2_g_step1'])
        self.addL2Signature(['L2_g_step2'])
        self.addEFSignature(['EF_g_step1'])
        self.addEFSignature(['EF_g_step2'])

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['egamma']
        self.calib_streams = []
        self.groups = ['Exclusive_Photons', 'Inclusive_Photons']
        self.trigger_type = []

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.sig_id+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.sig_id),
            }


# derived class, ID algos run at EF level

#-----------------------------------
class L2EFChain_g_EFID(L2EFChain_g):
#-----------------------------------

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_g_step1')

        # Step 2: Photon Fex and Hypo
        self.addL2Sequence('L2_g_step1',
                           [config.L2Config.TrigL2PhotonFex_1, config.L2Config.TrigL2PhotonHypo_1],
                           'L2_g_step2')
        
        ###### EF ######
        # Step 1: Calo cluster
        self.addEFSequence('L2_g_step2', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_g_step1')
        
        # Step 2: ID
        self.addEFSequence('EF_g_step1',
                           theTrigEFIDCombined_Photon,
                           'EF_g_step2')
        
        # Step 3: Calo cluster and Hypo
        self.addEFSequence('EF_g_step2', 
                           [theTrigEgammaRec_PhotonConversions, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_g_step3')
        
    def defineSignatures(self):
        self.addL2Signature(['L2_g_step1'])
        self.addL2Signature(['L2_g_step2'])
        self.addEFSignature(['EF_g_step1'])
        self.addEFSignature(['EF_g_step2'])
        self.addEFSignature(['EF_g_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.sig_id+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.sig_id+'_id'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }


# derived class, ID algos run at EF level and L2 calorimeter calibration

#-----------------------------------
class L2EFChain_g_L2calib(L2EFChain_g):
#-----------------------------------

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_g_step1')

        # Step 2: Photon Fex and Hypo
        self.addL2Sequence('L2_g_step1',
                           [config.L2Config.TrigL2PhotonFex_1, config.L2Config.TrigL2PhotonHypo_1],
                           'L2_g_step2')
        
        # Step 3: Calo calibration
        self.addL2Sequence('L2_g_step2',
                           [theLArL2ROBListWriter],
                           'L2_g_step3')
            
        ###### EF ######
        # Step 1: Calo cluster
        self.addEFSequence('L2_g_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_g_step1')
        
        # Step 2: e/gamma reconstruction and Hypo
        self.addEFSequence('EF_g_step1', 
                           [theTrigEgammaRec_NoIDEF_eGamma, config.EFConfig.TrigEFPhotonHypo_1],
                           'EF_g_step2')

    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = []
        self.L2physics_streams = ['egamma']
        self.L2calib_streams = []
        self.EFphysics_streams = []
        self.EFcalib_streams = ['LArCells']
        self.groups = ['Exclusive_Photons', 'Inclusive_Photons']
        self.trigger_type = []

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.sig_id+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.sig_id),
            'L2_g_step3': mergeRemovingOverlap('L2_', self.sig_id+'_calib'),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.sig_id),
            }



Photons = [
    L2EFChain_g('g3_nocut', 'L2_g3_nocut', 609, 'L1_EM3', 'EF_g53nocut', 609, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g3_nocut()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g3_nocut()))),
    
    L2EFChain_g('g5_nocut', 'L2_g5_nocut', 572, 'L1_EM3', 'EF_g5_nocut', 572, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),

    L2EFChain_g('g5_nocut_cosmic', 'L2_g5_nocut_cosmic', 729, 'L1_EM3_EMPTY', 'EF_g5_nocut_cosmic', 729, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),

    L2EFChain_g('g10_nocut', 'L2_g10_nocut', 994, 'L1_EM7', 'EF_g10_nocut', 994, ['EM7'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g10_nocut()))),

    L2EFChain_g('g5', 'L2_g5', 577, 'L1_EM3', 'EF_g5', 577, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5()))),
    
    L2EFChain_g('g10_loose', 'L2_g10_loose', 36, 'L1_EM7', 'EF_g10_loose', 36, ['EM7'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose()))), # validated

    L2EFChain_g('g10_loose_cosmic', 'L2_g10_loose_cosmic', 731, 'L1_EM7_EMPTY', 'EF_g10_loose_cosmic', 731, ['EM7'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose()))),
        
    L2EFChain_g('g15_loose', 'L2_g15_loose', 37, 'L1_EM13', 'EF_g15_loose', 37, ['EM13'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g15_loose()))), # validated
    
    L2EFChain_g('g15i_loose', 'L2_g15i_loose', 38, 'L1_EM13I', 'EF_g15i_loose', 38, ['EM13I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15i_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g15i_loose()))), # validated
    
    L2EFChain_g('g17i_tight', 'L2_g17i_tight', 34, 'L1_EM13I', 'EF_g17i_tight', 34, ['EM13I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g17i_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g17i_tight()))),
    
    L2EFChain_g('g20_loose', 'L2_g20_loose', 39, 'L1_EM18', 'EF_g20_loose', 39, ['EM18'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose()))), # validated
    
    L2EFChain_g('g20i_loose', 'L2_g20i_loose', 40, 'L1_EM18I', 'EF_g20i_loose', 40, ['EM18I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20i_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20i_loose()))), # validated
    
    L2EFChain_g('g25_loose', 'L2_g25_loose', 42, 'L1_EM18', 'EF_g25_loose', 42, ['EM18'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25_loose()))), # validated
    
    L2EFChain_g('g25i_loose', 'L2_g25i_loose', 41, 'L1_EM18I', 'EF_g25i_loose', 41, ['EM18I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25i_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25i_loose()))), # validated
    
    L2EFChain_g('g25_tight', 'L2_g25_tight', 294, 'L1_EM18', 'EF_g25_tight', 294, ['EM18'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25_tight()))), # validated
    
    L2EFChain_g('g25i_tight', 'L2_g25i_tight', 293, 'L1_EM18I', 'EF_g25i_tight', 293, ['EM18I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25i_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25i_tight()))), # validated
    

    L2EFChain_g('g40_medium', 'L2_g40_medium', 993, 'L1_EM18', 'EF_g40_medium', 993, ['EM18'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g40_medium()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g40_medium()))), 
    

    L2EFChain_g('g55_tight', 'L2_g55_tight', 35, 'L1_EM18', 'EF_g55_tight', 35, ['EM18'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g55_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g55_tight()))), # validated
    
    L2EFChain_g('g105', 'L2_g105', 43, 'L1_EM100', 'EF_g105', 43, ['EM100'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g105()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g105()))), # validated
    
    L2EFChain_g('g150', 'L2_g150', 295, 'L1_EM100', 'EF_g150', 295, ['EM100'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g150()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g150()))), # validated

    ##     ######################################################
    ##     # Chains with photon conversion reconstruction at EF
    ##     ######################################################

    L2EFChain_g_EFID('g20_loose_cnv', 'L2_g20_loose_cnv', 999, 'L1_EM18', 'EF_g20_loose_cnv', 999, ['EM18'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose_cnv()))), # validated
    
    
#    ##     ##################################
#    ##     # L2LAr Calibration chains
#    ##     ##################################
#
#    
#    L2EFChain_g_L2calib('g10_calib', 'L2_g10_calib', 500, 'L1_EM7', 'EF_g10_calib', 500, ['EM7'],
#                             L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_calib()),
#                                                L2EFChain_g.EFConfig('', EFPhotonHypo_g10_calib()))), # validated
#    
#    L2EFChain_g_L2calib('g20i_calib', 'L2_g20i_calib', 501, 'L1_EM18I', 'EF_g20i_calib', 501, ['EM18I'],
#                             L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20i_calib()),
#                                                L2EFChain_g.EFConfig('', EFPhotonHypo_g20i_calib()))), # validated
#    
#    L2EFChain_g_L2calib('g25i_calib', 'L2_g25i_calib', 502, 'L1_EM18I', 'EF_g25i_calib', 502, ['EM18I'],
#                             L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25i_calib()),
#                                                L2EFChain_g.EFConfig('', EFPhotonHypo_g25i_calib()))), # validated
#
    ]#end of Photons chain
        


