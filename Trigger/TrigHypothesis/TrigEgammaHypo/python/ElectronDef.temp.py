# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'P.Urquijo'
__version__=""
__doc__="Implementation of Electron Slice single electron signatures"

from AthenaCommon.Logging import logging
from TriggerMenuPython.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TriggerMenuPython.EgammaSliceFlags import getEM_ItemFromChain, getEM_InTEFromItem

##########################################################################################
#
# Import and create fexes and hypos
#
########################################################################################## 
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma, T2CaloEgamma_SwSeed, T2CaloEgamma_SwSeed_NoCut
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_SwCluster, T2CaloEgamma_Ringer, T2CaloEgamma_cells

theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
theT2CaloEgamma_SwSeed            = T2CaloEgamma_SwSeed()
theT2CaloEgamma_SwSeed_NoCut      = T2CaloEgamma_SwSeed_NoCut()
theT2CaloEgamma_SwCluster         = T2CaloEgamma_SwCluster()
theT2CaloEgamma_Ringer            = T2CaloEgamma_Ringer()
theT2CaloEgamma_cells_e           = T2CaloEgamma_cells("T2CaloEgamma_cells")

from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_eGamma
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_eGamma, TrigSiTrack_eGamma_robust
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_eGamma

theTrigIDSCAN_eGamma              = TrigIDSCAN_eGamma()
theTrigSiTrack_eGamma             = TrigSiTrack_eGamma()
theTrigSiTrack_eGamma_robust      = TrigSiTrack_eGamma_robust()
theTrigTRTSegFinder_eGamma        = TrigTRTSegFinder_eGamma()


from TrigEgammaHypo.TrigL2CaloHypoConfig     import *
from TrigEgammaHypo.TrigL2ElectronFexConfig  import *
from TrigEgammaHypo.TrigL2ElectronHypoConfig import *
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e10
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e5
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e5
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e10

from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma,   TrigCaloCellMaker_eGamma_cells,  TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()

from InDetTrigRecExample.EFInDetConfig import *
theTrigEFIDInsideOut_Electron           = TrigEFIDInsideOut_Electron("Electron").getSequence()
theTrigEFIDOutsideInTRTOnly_Electron    = TrigEFIDOutsideInTRTOnly_Electron().getSequence()
theTrigEFIDCombined_Electron            = TrigEFIDCombined_Electron().getSequence()

from TrigEgammaHypo.TrigEFTrackHypoConfig    import *
from TrigEgammaHypo.TrigEFElectronHypoConfig import *
from TrigEgammaRec.TrigEgammaRecConfig       import *
theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()

##########################################################################################
#
# L2 & EF Chain configuration helper classes
#
##########################################################################################        
class L2EFChain_e(L2EFChainDef):
    class L2Config:
        def __init__(self, suffix, 
                     TrigL2CaloHypo_1=None, TrigL2ElectronFex_1=None, TrigL2ElectronHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_'+suffix)
            self.TrigL2ElectronFex_1  = TrigL2ElectronFex_1  or TrigL2ElectronFex('TrigL2ElectronFex_'+suffix)
            self.TrigL2ElectronHypo_1 = TrigL2ElectronHypo_1 or TrigL2ElectronHypo('TrigL2ElectronHypo_'+suffix)
            pass
        
    class EFConfig:
        def __init__(self, suffix,  
                     TrigEFTrackHypo_1=None, TrigEFEGammaHypo_1=None):
            self.suffix = suffix
            self.TrigEFTrackHypo_1  = TrigEFTrackHypo_1  or TrigEFTrackHypo('TrigEFTrackHypo_'+suffix)
            self.TrigEFEGammaHypo_1 = TrigEFEGammaHypo_1 or TrigEFElectronHypo('TrigEFElectronHypo_'+suffix)
            pass
        
    class Config:
        def __init__(self, l2config, efconfig):
            self.suffix = ''
            self.L2Config = l2config
            self.EFConfig = efconfig
            pass

    # New simplified constructor for electron chains
    def __init__(self, sig_id, chain_counter, config):

        l1_item_name = getEM_ItemFromChain(sig_id)
        l2_input_tes = getEM_InTEFromItem(l1_item_name)
        
        l2_name = 'L2_'+sig_id
        ef_name = 'EF_'+sig_id
            
        L2EFChainDef.__init__(self, sig_id,
                              l2_name, chain_counter, l1_item_name, ef_name, chain_counter, l2_input_tes, config)

    def defineSequences(self, config):
        ###### L2 ######
            # Step 1: Calo Fex and Hypo
            self.addL2Sequence(self.inputTEs,
                               [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                               'L2_e_step1')
            # Step 2: ID Fex
            self.addL2Sequence(['L2_e_step1'],
                               [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma],
                               'L2_e_step2')
            # Step 3: ID-Calo Fex and Hypo
            self.addL2Sequence(['L2_e_step2'],
                               [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                               'L2_e_step3')
            
        
        ###### EF ######
            # Step 1: Calo Fex
            self.addEFSequence('L2_e_step3', 
                               [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                               'EF_e_step1')
            # Step 2: ID Fex and Hypo
            self.addEFSequence('EF_e_step1', 
                               theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                               'EF_e_step2')
            # Step 3: ID-Calo Fex and Hypo
            self.addEFSequence('EF_e_step2', 
                               [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                               'EF_e_step3')
            
    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addL2Signature(['L2_e_step2'])
        self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['egamma']
        self.calib_streams = []
        self.groups = ['Exclusive_Electrons', 'Inclusive_Electrons']
        self.trigger_type = []

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }

class L2EFChain_e_robust(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma_robust],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')
        
class L2EFChain_e_cells(L2EFChain_e):
 
    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma_robust],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')


        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3',
                           [theTrigCaloCellMaker_eGamma_cells, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1',
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2',
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')




class L2EFChain_e_TRT(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigTRTSegFinder_eGamma ],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDOutsideInTRTOnly_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')


class L2EFChain_e_IdScan(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

class L2EFChain_e_IdScanFex(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')


class L2EFChain_e_SiTrack(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

class L2EFChain_e_SiTrack_robust(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')


class L2EFChain_e_fwdBackTrk(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma, theTrigTRTSegFinder_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDCombined_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

class L2EFChain_e_nocut(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

class L2EFChain_e_Ringer(L2EFChain_e):

    class L2Config:
        def __init__(self, suffix, 
                     TrigL2CaloHypo_1=None, TrigL2CaloHypo_2=None, TrigL2ElectronFex_1=None, TrigL2ElectronHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_1_'+suffix)
            self.TrigL2CaloHypo_2   = TrigL2CaloHypo_2   or TrigL2CaloHypo('TrigL2CaloHypo_2_'+suffix)
            self.TrigL2ElectronFex_1  = TrigL2ElectronFex_1  or TrigL2ElectronFex('TrigL2ElectronFex_'+suffix)
            self.TrigL2ElectronHypo_1 = TrigL2ElectronHypo_1 or TrigL2ElectronHypo('TrigL2ElectronHypo_'+suffix)
            pass
            
    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_Ringer, config.L2Config.TrigL2CaloHypo_1, config.L2Config.TrigL2CaloHypo_2],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')


        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3',
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')    
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1',    
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2',
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

class L2EFChain_e_L2SW(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1a: Calo Fex part 1
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_SwSeed],
                           'L2_e_step1a')
        # Step 1b: Calo Fex part 2 and Hypo
        self.addL2Sequence(['L2_e_step1a'], 
                           [theT2CaloEgamma_SwCluster, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1b')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1b'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1a'])
        self.addL2Signature(['L2_e_step1b'])
        self.addL2Signature(['L2_e_step2'])
        self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1a': mergeRemovingOverlap('L2_', self.sig_id+'cla'),
            'L2_e_step1b': mergeRemovingOverlap('L2_', self.sig_id+'clb'),
            'L2_e_step2':  mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step3':  mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1':  mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2':  mergeRemovingOverlap('EF_', self.sig_id+'id'),
            'EF_e_step3':  mergeRemovingOverlap('EF_', self.sig_id),
            }


class L2EFChain_e_L2SW_NoCut(L2EFChain_e_L2SW):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1a: Calo Fex part 1
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_SwSeed_NoCut],
                           'L2_e_step1a')
        # Step 1b: Calo Fex part 2 and Hypo
        self.addL2Sequence(['L2_e_step1a'], 
                           [theT2CaloEgamma_SwCluster, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1b')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1b'],
                           [theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma_robust],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')


##########################################################################################
#
# Create chain objects for all configured chains
#
##########################################################################################

Electrons = [        
##     ##################################
##     # Chains using all ID Fex Algos
##     ##################################
    L2EFChain_e_robust('e5_NoCut',   484,
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(),  L2ElectronFex_1(), L2ElectronHypo_e5_NoCut()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(),  TrigEFElectronHypo_e5_NoCut()  ))), 

    L2EFChain_e_robust('e5_NoCut_cosmic', 719, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(),  L2ElectronFex_1(), L2ElectronHypo_e5_NoCut()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(),  TrigEFElectronHypo_e5_NoCut()  ))), 


    L2EFChain_e_robust('e10_NoCut', 998, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_NoCut(),  L2ElectronFex_1(), L2ElectronHypo_e10_NoCut()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e10_NoCut(),  TrigEFElectronHypo_e10_NoCut()  ))), 
    
    L2EFChain_e_cells('e5_NoCut_cells', 489, 
                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(),  L2ElectronFex_all(), L2ElectronHypo_e5_NoCut()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(),  TrigEFElectronHypo_e5_NoCut()  ))),

    L2EFChain_e_cells('e5_NoCut_cells_cosmic', 455,
                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut(),  L2ElectronFex_all(), L2ElectronHypo_e5_NoCut()),
                                          L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut(),  TrigEFElectronHypo_e5_NoCut()  ))),

    L2EFChain_e('e5_medium1', 473, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium1(), L2ElectronFex_all(), L2ElectronHypo_e5_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium1(), TrigEFElectronHypo_e5_medium1() ))),
    
    L2EFChain_e('e6_medium1', 475, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e6_medium1(),  L2ElectronFex_all(), L2ElectronHypo_e6_medium1()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e6_medium1(), TrigEFElectronHypo_e6_medium1() ))),
    
    L2EFChain_e('e7_medium', 29, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e7_medium(),   L2ElectronFex_all(), L2ElectronHypo_e7_medium()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e7_medium(),  TrigEFElectronHypo_e7_medium()  ))),
    
    L2EFChain_e('e10_loose', 1, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose(),  L2ElectronFex_all(),  L2ElectronHypo_e10_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose(),  TrigEFElectronHypo_e10_loose()  ))),
    
    L2EFChain_e('e10_loose_cosmic', 725, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose(),  L2ElectronFex_all(),  L2ElectronHypo_e10_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose(),  TrigEFElectronHypo_e10_loose()  ))),
        
    L2EFChain_e('e10_medium_cosmic', 728, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e10_medium()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium(),  TrigEFElectronHypo_e10_medium()  ))),
    
    L2EFChain_e('e10i_loose', 2, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e10i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10i_loose(),  TrigEFElectronHypo_e10i_loose()  ))),
    
    L2EFChain_e('e10i_medium', 378, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10i_medium(),  L2ElectronFex_all(), L2ElectronHypo_e10i_medium()), 
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10i_medium(), TrigEFElectronHypo_e10i_medium() ))),
        
    L2EFChain_e('e15_loose', 4, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose(),   L2ElectronFex_all(), L2ElectronHypo_e15_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose(),  TrigEFElectronHypo_e15_loose()  ))),
    
    L2EFChain_e('e15i_loose', 6, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e15i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15i_loose(),  TrigEFElectronHypo_e15i_loose()  ))),
    
    L2EFChain_e('e20i_loose', 8, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e20i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20i_loose(),  TrigEFElectronHypo_e20i_loose()  ))),
    
    L2EFChain_e('e22i_tight', 12, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e22i_tight(),   L2ElectronFex_all(), L2ElectronHypo_e22i_tight()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e22i_tight(),  TrigEFElectronHypo_e22i_tight()  ))),
    
     L2EFChain_e('e25i_loose', 9, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e25i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25i_loose(),  TrigEFElectronHypo_e25i_loose()  ))), 

    #####################################################
    # Same Chains, with L2 and EF Hypos set to Accept All
    #####################################################

    L2EFChain_e('e10_loose_passL2',  14, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_passL2(),   L2ElectronFex_all(), L2ElectronHypo_e10_loose_passL2()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_passL2(),  TrigEFElectronHypo_e10_loose_passL2()  ))),
    
    L2EFChain_e('e10_loose_passL2_cosmic', 726, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_passL2(),   L2ElectronFex_all(), L2ElectronHypo_e10_loose_passL2()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_passL2(),  TrigEFElectronHypo_e10_loose_passL2()  ))),
    
    L2EFChain_e('e15_loose_passL2', 18, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose_passL2(),   L2ElectronFex_all(), L2ElectronHypo_e15_loose_passL2()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose_passL2(),  TrigEFElectronHypo_e15_loose_passL2()  ))),
    
    L2EFChain_e('e20i_loose_passL2', 24, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_loose_passL2(),   L2ElectronFex_all(), L2ElectronHypo_e20i_loose_passL2()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20i_loose_passL2(),  TrigEFElectronHypo_e20i_loose_passL2()  ))),
    
    L2EFChain_e('e25i_loose_passL2', 26, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_loose_passL2(),   L2ElectronFex_all(), L2ElectronHypo_e25i_loose_passL2()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25i_loose_passL2(),  TrigEFElectronHypo_e25i_loose_passL2()  ))), 
				    
    L2EFChain_e('e10_loose_passEF', 15, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_passEF(),  L2ElectronFex_all(), L2ElectronHypo_e10_loose_passEF()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_passEF(),  TrigEFElectronHypo_e10_loose_passEF()  ))),
    
    L2EFChain_e('e10_loose_passEF_cosmic', 727, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_passEF(),  L2ElectronFex_all(), L2ElectronHypo_e10_loose_passEF()),
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_passEF(),  TrigEFElectronHypo_e10_loose_passEF()  ))),
    
    L2EFChain_e('e15_loose_passEF', 19, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_loose(),   L2ElectronFex_all(), L2ElectronHypo_e15_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e15_loose_passEF(),  TrigEFElectronHypo_e15_loose_passEF()  ))),
    

    L2EFChain_e('e20i_loose_passEF', 25, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e20i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20i_loose_passEF(),  TrigEFElectronHypo_e20i_loose_passEF()  ))),
    
    L2EFChain_e('e25i_loose_passEF', 27, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_loose(),   L2ElectronFex_all(), L2ElectronHypo_e25i_loose()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e25i_loose_passEF(),  TrigEFElectronHypo_e25i_loose_passEF()  ))), 

				     
##     ##################################
##     # Chains using IDSCAN 
##     ##################################

    L2EFChain_e_IdScan('e5_NoCut_IdScan', 485, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_IdScan(),   L2ElectronFex_IdScan_NoCut(), L2ElectronHypo_e5_NoCut_IdScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_IdScan(),  TrigEFElectronHypo_e5_NoCut_IdScan()  ))), 

    L2EFChain_e_IdScan('e5_NoCut_IdScan_cosmic', 721, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_IdScan(),   L2ElectronFex_IdScan_NoCut(), L2ElectronHypo_e5_NoCut_IdScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_IdScan(),  TrigEFElectronHypo_e5_NoCut_IdScan()  ))), 

    L2EFChain_e_IdScan('e5_medium_IdScan', 999, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium(),          L2ElectronFex_IdScan(), L2ElectronHypo_e5_medium_IDScan()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium_IDScan(),  TrigEFElectronHypo_e5_medium_IDScan()  ))),

    L2EFChain_e_IdScan('e6_medium_IdScan', 999, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e6_medium(),          L2ElectronFex_IdScan(), L2ElectronHypo_e6_medium_IDScan()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e6_medium_IDScan(),  TrigEFElectronHypo_e6_medium_IDScan()  ))),
    
    L2EFChain_e_IdScan('e10_loose_IdScan', 556, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_IDScan(),   L2ElectronFex_IdScan(), L2ElectronHypo_e10_loose_IDScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_IDScan(),  TrigEFElectronHypo_e10_loose_IDScan()  ))), 

    L2EFChain_e_IdScan('e12_medium_IdScan', 999, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e12_medium_IDScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium_IDScan(),  TrigEFElectronHypo_e12_medium_IDScan()  ))), 

    L2EFChain_e_IdScan('e15_medium_IdScan', 999, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e15_medium_IDScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium_IDScan(),  TrigEFElectronHypo_e15_medium_IDScan()  ))), 

    L2EFChain_e_IdScan('e20_loose_IdScan', 888, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose(),   L2ElectronFex_IdScan(), L2ElectronHypo_e20_loose_IDScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_IDScan(),  TrigEFElectronHypo_e20_loose_IDScan()  ))), 

    L2EFChain_e_IdScan('e25_loose_IdScan', 999, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_loose(),   L2ElectronFex_IdScan(), L2ElectronHypo_e25_loose_IDScan()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e25_loose_IDScan(),  TrigEFElectronHypo_e25_loose_IDScan()  ))), 

    L2EFChain_e_IdScanFex('e12_tight', 11, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_tight(),    L2ElectronFex_IdScan(), L2ElectronHypo_e12_tight()),   
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e12_tight(),   TrigEFElectronHypo_e12_tight()   ))),
    
    L2EFChain_e_IdScanFex('e15i_medium', 5, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15i_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e15i_medium()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e15i_medium(),  TrigEFElectronHypo_e15i_medium()  ))),
    
    L2EFChain_e_IdScanFex('e17i_medium', 452, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e17i_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e17i_medium()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e17i_medium(),  TrigEFElectronHypo_e17i_medium()  ))),
    
    L2EFChain_e_IdScanFex('e20i_medium', 298, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e20i_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e20i_medium(), TrigEFElectronHypo_e20i_medium() ))),
    
    L2EFChain_e_IdScanFex('e25_medium', 299, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e25_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e25_medium(), TrigEFElectronHypo_e25_medium() ))),
    
    L2EFChain_e_IdScanFex('e25i_medium', 300, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e25i_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e25i_medium(), TrigEFElectronHypo_e25i_medium() ))),
    
        L2EFChain_e_IdScanFex('e24_medium', 299, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e24_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e24_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e24_medium(), TrigEFElectronHypo_e24_medium() ))),
    				   
    L2EFChain_e_IdScanFex('e40_loose1', 453, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e40_loose1(),   L2ElectronFex_IdScan(), L2ElectronHypo_e40_loose1()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e40_loose1(),  TrigEFElectronHypo_e40_loose1()  ))),
    
    L2EFChain_e_IdScanFex('e55_loose1', 456, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e55_loose1(),   L2ElectronFex_IdScan(), L2ElectronHypo_e55_loose1()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e55_loose1(),  TrigEFElectronHypo_e55_loose1()  ))),
    
    L2EFChain_e_IdScanFex('e55_medium2', 13, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e55_medium2(),  L2ElectronFex_IdScan(), L2ElectronHypo_e55_medium2()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e55_medium2(), TrigEFElectronHypo_e55_medium2() ))),
    
    L2EFChain_e_IdScanFex('e105_loose1', 10, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e105_loose1(),  L2ElectronFex_IdScan(), L2ElectronHypo_e105_loose1()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e105_loose1(), TrigEFElectronHypo_e105_loose1() ))),
    
    L2EFChain_e_IdScanFex('e140_loose1', 454, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e140_loose1(),  L2ElectronFex_IdScan(), L2ElectronHypo_e140_loose1()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e140_loose1(), TrigEFElectronHypo_e140_loose1() ))),
                
    #####################################################
    # Same Chains, with L2 and EF Hypos set to Accept All
    #####################################################

    L2EFChain_e_IdScanFex('e15i_medium_passL2', 20, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15i_medium_passL2(),  L2ElectronFex_IdScan(), L2ElectronHypo_e15i_medium_passL2()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e15i_medium_passL2(),  TrigEFElectronHypo_e15i_medium_passL2()  ))),
    
    
    L2EFChain_e_IdScanFex('e20i_medium_passL2', 303, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_medium_passL2(),  L2ElectronFex_IdScan(), L2ElectronHypo_e20i_medium_passL2()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e20i_medium_passL2(), TrigEFElectronHypo_e20i_medium_passL2() ))),
    
    L2EFChain_e_IdScanFex('e25i_medium_passL2', 305, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_medium_passL2(),  L2ElectronFex_IdScan(), L2ElectronHypo_e25i_medium_passL2()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e25i_medium_passL2(), TrigEFElectronHypo_e25i_medium_passL2() ))), 				  
    
    L2EFChain_e_IdScanFex('e15i_medium_passEF', 21, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15i_medium(),   L2ElectronFex_IdScan(), L2ElectronHypo_e15i_medium()),  
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e15i_medium_passEF(), TrigEFElectronHypo_e15i_medium_passEF() ))),
    
    L2EFChain_e_IdScanFex('e20_medium_passEF',  302, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e20_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium_passEF(), TrigEFElectronHypo_e20_medium_passEF() ))),

        
    L2EFChain_e_IdScanFex('e25i_medium_passEF', 306, 
                       L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25i_medium(),  L2ElectronFex_IdScan(), L2ElectronHypo_e25i_medium()), 
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e25i_medium_passEF(), TrigEFElectronHypo_e25i_medium_passEF() ))), 
					   
##     ##################################
##     # Chains using TRTSegFinder
##     ##################################

    L2EFChain_e_TRT('e5_NoCut_TRT', 486, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_TRT(),  L2ElectronFex_TRTSegFinder_NoCut(), L2ElectronHypo_e5_NoCut_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_TRT(),  TrigEFElectronHypo_e5_NoCut_TRT()  ))), 

    L2EFChain_e_TRT('e5_NoCut_TRT_cosmic', 722, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_TRT(),  L2ElectronFex_TRTSegFinder_NoCut(), L2ElectronHypo_e5_NoCut_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_TRT(),  TrigEFElectronHypo_e5_NoCut_TRT()  ))), 

    L2EFChain_e_TRT('e5_medium_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium(),  L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e5_medium_TRT()),  
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium_TRT(),  TrigEFElectronHypo_e5_medium_TRT()  ))),

    L2EFChain_e_TRT('e6_medium_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e6_medium(),  L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e6_medium_TRT()),  
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e6_medium_TRT(),  TrigEFElectronHypo_e6_medium_TRT()  ))),

    L2EFChain_e_TRT('e10_loose_TRT', 557, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_TRT(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e10_loose_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_TRT(),  TrigEFElectronHypo_e10_loose_TRT()  ))),

    L2EFChain_e_TRT('e10_medium_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e10_medium_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium_TRT(),  TrigEFElectronHypo_e10_medium_TRT()  ))),

    L2EFChain_e_TRT('e12_medium_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e12_medium_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium_TRT(),  TrigEFElectronHypo_e12_medium_TRT()  ))),

    L2EFChain_e_TRT('e15_medium_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e15_medium_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium_TRT(),  TrigEFElectronHypo_e15_medium_TRT()  ))),

    L2EFChain_e_TRT('e20_loose_TRT', 579, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e20_loose_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_TRT(),  TrigEFElectronHypo_e20_loose_TRT()  ))),     

    L2EFChain_e_TRT('e25_loose_TRT', 999, 
                    L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_loose(),   L2ElectronFex_TRTSegFinder(), L2ElectronHypo_e25_loose_TRT()),
                                        L2EFChain_e.EFConfig('',EFTrackHypo_e25_loose_TRT(),  TrigEFElectronHypo_e25_loose_TRT()  ))),
					
    ##     ##################################
    ##     # Chains using SiTrack
    ##     ##################################

    L2EFChain_e_SiTrack_robust('e5_NoCut_SiTrk', 487, 
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_SiTrk(),  L2ElectronFex_SiTrack_NoCut(), L2ElectronHypo_e5_NoCut_SiTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_SiTrk(),  TrigEFElectronHypo_e5_NoCut_SiTrk()  ))),

    L2EFChain_e_SiTrack_robust('e5_NoCut_SiTrk_cosmic', 720, 
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_SiTrk(),  L2ElectronFex_SiTrack_NoCut(), L2ElectronHypo_e5_NoCut_SiTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_SiTrk(),  TrigEFElectronHypo_e5_NoCut_SiTrk()  ))),

    L2EFChain_e_SiTrack_robust('e10_loose_SiTrk', 558, 
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_SiTrk(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e10_loose_SiTrk()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_SiTrk(),  TrigEFElectronHypo_e10_loose_SiTrk()  ))), 
    
    L2EFChain_e_SiTrack_robust('e10_medium_SiTrk_robust', 580, 
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium_SiTrk_robust(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e10_medium_SiTrk_robust()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium_SiTrk_robust(),  TrigEFElectronHypo_e10_medium_SiTrk_robust()  ))),

    L2EFChain_e_SiTrack_robust('e20_loose_SiTrk_robust', 582, 
                               L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose_SiTrk_robust(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e20_loose_SiTrk_robust()),
                                                   L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_SiTrk_robust(),  TrigEFElectronHypo_e20_loose_SiTrk_robust()  ))),
    

    L2EFChain_e_SiTrack('e5_medium', 28, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e5_medium()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium(),  TrigEFElectronHypo_e5_medium()  ))),

    L2EFChain_e_SiTrack('e6_medium', 474, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e6_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e6_medium()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e6_medium(),  TrigEFElectronHypo_e6_medium()  ))),
        
    L2EFChain_e_SiTrack('e10_medium', 30, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e10_medium()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium(),  TrigEFElectronHypo_e10_medium()  ))),

    L2EFChain_e_SiTrack('e12_medium', 31, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e12_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e12_medium()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e12_medium(), TrigEFElectronHypo_e12_medium() ))),

    L2EFChain_e_SiTrack('e15_medium', 3, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e15_medium()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium(), TrigEFElectronHypo_e15_medium() ))),

    L2EFChain_e_SiTrack('e20_loose', 7, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e20_loose()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose(),  TrigEFElectronHypo_e20_loose()  ))),

    L2EFChain_e_SiTrack('e20_medium', 297, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e20_medium()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium(), TrigEFElectronHypo_e20_medium() ))),
    
    L2EFChain_e_SiTrack('e25_loose', 32, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e25_loose(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e25_loose()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e25_loose(),  TrigEFElectronHypo_e25_loose()  ))),
					    
    L2EFChain_e_SiTrack('e10_medium_SiTrk', 581, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium_SiTrk(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e10_medium_SiTrk()),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium_SiTrk(),  TrigEFElectronHypo_e10_medium_SiTrk()  ))),
    
    L2EFChain_e_SiTrack('e20_loose_SiTrk', 583, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose_SiTrk(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e20_loose_SiTrk()),
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_SiTrk(),  TrigEFElectronHypo_e20_loose_SiTrk()  ))),

    #####################################################
    # Same Chains, with L2 and EF Hypos set to Accept All
    #####################################################

    L2EFChain_e_SiTrack('e15_medium_passL2', 16, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium_passL2(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e15_medium_passL2()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium_passL2(), TrigEFElectronHypo_e15_medium_passL2()   ))),
    
    L2EFChain_e_SiTrack('e15_medium_passEF', 17, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e15_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e15_medium()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e15_medium_passEF(), TrigEFElectronHypo_e15_medium_passEF()   ))),
    
    L2EFChain_e_SiTrack('e20_loose_passL2', 22, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose_passL2(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e20_loose_passL2()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_passL2(),  TrigEFElectronHypo_e20_loose_passL2()  ))),
    
    L2EFChain_e_SiTrack('e20_loose_passEF', 23, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose(),   L2ElectronFex_SiTrack(), L2ElectronHypo_e20_loose()),  
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_passEF(),  TrigEFElectronHypo_e20_loose_passEF()  ))),

    L2EFChain_e_SiTrack('e20_medium_passL2', 301, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_medium_passL2(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e20_medium_passL2()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20_medium_passL2(), TrigEFElectronHypo_e20_medium_passL2() ))),
    
    L2EFChain_e_SiTrack('e20i_medium_passEF', 304, 
                        L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20i_medium(),  L2ElectronFex_SiTrack(), L2ElectronHypo_e20i_medium()), 
                                            L2EFChain_e.EFConfig('',EFTrackHypo_e20i_medium_passEF(), TrigEFElectronHypo_e20i_medium_passEF() ))),
    
     
    ##     ##################################
    ##     # Chains using Forward then Back Tracking 
    ##     ##################################
    
    L2EFChain_e_fwdBackTrk('e5_NoCut_FwdBackTrk', 488, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_FwdBackTrk(),  L2ElectronFex_FwdBackTracking_NoCut(), L2ElectronHypo_e5_NoCut_FwdBackTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_FwdBackTrk(),  TrigEFElectronHypo_e5_NoCut_FwdBackTrk()  ))),

    L2EFChain_e_fwdBackTrk('e5_NoCut_FwdBackTrk_cosmic', 723, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_FwdBackTrk(),  L2ElectronFex_FwdBackTracking_NoCut(), L2ElectronHypo_e5_NoCut_FwdBackTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_FwdBackTrk(),  TrigEFElectronHypo_e5_NoCut_FwdBackTrk()  ))), 
    
    L2EFChain_e_fwdBackTrk('e10_loose_FwdBackTrk', 559, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_loose_FwdBackTrk(),   L2ElectronFex_FwdBackTracking(), L2ElectronHypo_e10_loose_FwdBackTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e10_loose_FwdBackTrk(),  TrigEFElectronHypo_e10_loose_FwdBackTrk()  ))), 
    
    ##     ###########################################
    ##     # Chains using No track info in hypo algos 
    ##     ##########################################

    L2EFChain_e_fwdBackTrk('e20_loose_NoIDTrkCut', 997, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose_NoTrk(),   L2ElectronFex_all_NoCut(), L2ElectronHypo_e20_loose_NoTrk()),
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_NoTrk(),  TrigEFElectronHypo_e20_loose_NoTrk()  ))),     
    
    ##     ##################################
    ##     # Chains with "NoCut"
    ##     ##################################
    
    L2EFChain_e_nocut('eNoCut', 275, 
                      L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_1(),   L2ElectronFex_1(), L2ElectronHypo_1()),  
                                          L2EFChain_e.EFConfig('',EFTrackHypo_1(), TrigEFElectronHypo_1() ))), 
    
    ##     ##################################
    ##     # Chains with Ringer Algorithm
    ##     ##################################
    
    L2EFChain_e_Ringer('e5_NoCut_Ringer', 290, 
                       L2EFChain_e.Config( L2EFChain_e_Ringer.L2Config('',TrigRingerNeuralFexConfig_e5(),TrigRingerNeuralHypoConfig_e5(), L2ElectronFex_all_Ringer_NoCut(), L2ElectronHypo_e5_NoCut_Ringer()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_Ringer(), TrigEFElectronHypo_e5_NoCut_Ringer()  ))),

    L2EFChain_e_Ringer('e5_NoCut_Ringer_cosmic', 291, 
                       L2EFChain_e.Config( L2EFChain_e_Ringer.L2Config('',TrigRingerNeuralFexConfig_e5(),TrigRingerNeuralHypoConfig_e5(), L2ElectronFex_all_Ringer_NoCut(), L2ElectronHypo_e5_NoCut_Ringer()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_Ringer(), TrigEFElectronHypo_e5_NoCut_Ringer()  ))),

    L2EFChain_e_Ringer('e5_medium_Ringer', 991, 
                       L2EFChain_e.Config( L2EFChain_e_Ringer.L2Config('',TrigRingerNeuralFexConfig_e5(),TrigRingerNeuralHypoConfig_e5(), L2ElectronFex_Ringer(), L2ElectronHypo_e5_medium_Ringer()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium_Ringer(), TrigEFElectronHypo_e5_medium_Ringer()  ))),

    L2EFChain_e_Ringer('e10_medium_Ringer', 286, 
                       L2EFChain_e.Config( L2EFChain_e_Ringer.L2Config('',TrigRingerNeuralFexConfig_e10(),TrigRingerNeuralHypoConfig_e10(), L2ElectronFex_Ringer(), L2ElectronHypo_e10_medium_Ringer()),
                                           L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium_Ringer(), TrigEFElectronHypo_e10_medium_Ringer()  ))),

    ##     ################################################
    ##     # Chains with Sliding window cluster finder @ L2
    ##     ###############################################
    
    L2EFChain_e_L2SW_NoCut('e5_NoCut_L2SW', 514, 
                     L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_L2SW(),  L2ElectronFex_all_L2SW_NoCut(), L2ElectronHypo_e5_NoCut_L2SW()),  
                                         L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_L2SW(),  TrigEFElectronHypo_e5_NoCut_L2SW()  ))),
    
    L2EFChain_e_L2SW_NoCut('e5_NoCut_L2SW_cosmic', 724, 
                           L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_NoCut_L2SW(),  L2ElectronFex_all_L2SW_NoCut(), L2ElectronHypo_e5_NoCut_L2SW()),  
                                               L2EFChain_e.EFConfig('',EFTrackHypo_e5_NoCut_L2SW(),  TrigEFElectronHypo_e5_NoCut_L2SW()  ))), 
    
    L2EFChain_e_L2SW('e5_medium_L2SW', 800, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e5_medium_L2SW(),  L2ElectronFex_all_L2SW(), L2ElectronHypo_e5_medium_L2SW()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e5_medium_L2SW(),  TrigEFElectronHypo_e5_medium_L2SW()  ))), 

    L2EFChain_e_L2SW('e10_medium_L2SW', 801, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e10_medium_L2SW(),  L2ElectronFex_all_L2SW(), L2ElectronHypo_e10_medium_L2SW()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e10_medium_L2SW(),  TrigEFElectronHypo_e10_medium_L2SW()  ))), 

    L2EFChain_e_L2SW('e20_loose_L2SW', 802, 
                L2EFChain_e.Config( L2EFChain_e.L2Config('',L2CaloHypo_e20_loose_L2SW(),  L2ElectronFex_all_L2SW(), L2ElectronHypo_e20_loose_L2SW()),  
                                    L2EFChain_e.EFConfig('',EFTrackHypo_e20_loose_L2SW(),  TrigEFElectronHypo_e20_loose_L2SW()  ))), 
]

##########################################################################################
#
# Helper class for writing python code, eg HLT to L1 maps
#
##########################################################################################
class ElectronCodeHelper:
    def __init__(self):
        print 'Running ElectronCodeHelper:'
        
        global Electrons

        iwidth = int(0)
        for echain in Electrons:
            if iwidth < len(echain.l2chain.sig_id):
                iwidth = len(echain.l2chain.sig_id) 
            
        print 'Printing chain to item map:'
        for echain in Electrons:
            l2name = '\''+echain.l2chain.sig_id.rstrip()+'\''
            l1name = '\''+echain.l2chain.lower_chain_name.rstrip()+'\''
            lempty = ''
            for istep in range(len(echain.l2chain.sig_id), iwidth):
                lempty += ' '
            
            print l2name+lempty+':'+l1name+','
            
        print 'Printing item to theshold map:'
        l1map = {}
        for echain in Electrons:
            if echain.l2chain.lower_chain_name not in l1map:
                l1map[echain.l2chain.lower_chain_name] = echain.l2chain.inputTEs
                print '\''+echain.l2chain.lower_chain_name+'\':',echain.l2chain.inputTEs,'\','

        print 'Comparing L2 and EF counters:'
        l1map = {}
        for echain in Electrons:
            if echain.l2chain.chain_counter != echain.efchain.chain_counter:
                print echain.l2chain.sig_id+':',echain.l2chain.chain_counter,'!=',echain.efchain.chain_counter
                
        print 'Comparing L2 and EF names:'
        l1map = {}
        for echain in Electrons:
            if echain.l2chain.chain_name != 'L2_'+echain.sig_id:
                print echain.l2chain.sig_id+':',echain.l2chain.chain_name
            if echain.efchain.chain_name != 'EF_'+echain.sig_id:
                print echain.efchain.sig_id+':',echain.efchain.chain_name
