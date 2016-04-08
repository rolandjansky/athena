# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'P.Urquijo'
__version__=""
__doc__="Implementation of Electron Slice single electron signatures"

#for L2MonTemplate
from ChainTemplate import *


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

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
from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex

theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
theT2CaloEgamma_SwSeed            = T2CaloEgamma_SwSeed()
theT2CaloEgamma_SwSeed_NoCut      = T2CaloEgamma_SwSeed_NoCut()
theT2CaloEgamma_SwCluster         = T2CaloEgamma_SwCluster()
theT2CaloEgamma_Ringer            = T2CaloEgamma_Ringer()
theL2CaloMissingET_Fex            = L2CaloMissingET_Fex() 
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
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e5_NoCut
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e5
from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e10

from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma,   TrigCaloCellMaker_eGamma_cells,  TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
theTrigCaloCellMaker_eGamma_cells= TrigCaloCellMaker_eGamma_cells()
theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()

from InDetTrigRecExample.EFInDetConfig import *
theTrigEFIDInsideOut_Electron           = TrigEFIDInsideOut_Electron("Electron").getSequence()
theTrigEFIDOutsideInTRTOnly_Electron    = TrigEFIDOutsideInTRTOnly_Electron().getSequence()
theTrigEFIDCombined_Electron            = TrigEFIDCombined_Electron().getSequence()
theTrigEFIDInsideOut_Electron_DP = TrigEFIDSequence("Electron","electron","DataPrep").getSequence()
theTrigEFIDInsideOutMerged_Electron = TrigEFIDSequence("Electron","electron","InsideOutMerged").getSequence()


from TrigEgammaHypo.TrigEFTrackHypoConfig    import *
from TrigEgammaHypo.TrigEFElectronHypoConfig import *
from TrigEgammaRec.TrigEgammaRecConfig       import *
theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()


from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
terminateAlgo = PrescaleAlgo('terminateAlgo')

# ---

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
theTrigL2SiTrackFinder_eGammaA = TrigL2SiTrackFinder_eGammaA()
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaB
theTrigL2SiTrackFinder_eGammaB = TrigL2SiTrackFinder_eGammaB()
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaC
theTrigL2SiTrackFinder_eGammaC = TrigL2SiTrackFinder_eGammaC()

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA_Brem 
    theTrigL2SiTrackFinder_eGammaA_Brem = TrigL2SiTrackFinder_eGammaA_Brem() 
except:
    theTrigL2SiTrackFinder_eGammaA_Brem = None

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaBC
    theTrigL2SiTrackFinder_eGammaBC = TrigL2SiTrackFinder_eGammaBC()
except:
    theTrigL2SiTrackFinder_eGammaBC = None

try:
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma
    theTrigFastTrackFinder_Electron = TrigFastTrackFinder_eGamma()
except:
    theTrigFastTrackFinder_Electron = None
       
trkseqcomb1st = list(theTrigEFIDInsideOut_Electron_DP)
trkseqcomb1st.append(theTrigFastTrackFinder_Electron)
trkseqcombfull = list(trkseqcomb1st)
trkseqcombfull += theTrigEFIDInsideOutMerged_Electron


# ---

fexes = FreeStore()
hypos = FreeStore()

fexes.l2_calo = None
fexes.L2Electron_IdScan = L2ElectronFex_IdScan()
fexes.L2Electron_L2StarA = L2ElectronFex_L2StarA()
fexes.L2Electron_L2StarB = L2ElectronFex_L2StarB()
fexes.L2Electron_L2StarC = L2ElectronFex_L2StarC()

# New L2 Electron Hypos with L2StarB
hypos.L2Calo_e7T_loose1_L2StarB = L2CaloHypo_e7_loose1()
hypos.L2Electron_e7T_loose1_L2StarB = L2ElectronHypo_e7_loose1_L2StarB()
hypos.EFTrack_e7T_loose1_L2StarB = EFTrackHypo_e7_loose1()
hypos.EFElectron_e7T_loose1_L2StarB = TrigEFElectronHypo_e7_loose1()

hypos.L2Calo_e7_loose1_L2StarB = L2CaloHypo_e7_loose1()
hypos.L2Electron_e7_loose1_L2StarB = L2ElectronHypo_e7_loose1_L2StarB()
hypos.EFTrack_e7_loose1_L2StarB = EFTrackHypo_e7_loose1()
hypos.EFElectron_e7_loose1_L2StarB = TrigEFElectronHypo_e7_loose1()

hypos.L2Calo_e12_medium1_L2StarB = L2CaloHypo_e12_medium1()
hypos.L2Electron_e12_medium1_L2StarB = L2ElectronHypo_e12_medium1_L2StarB()
hypos.EFTrack_e12_medium1_L2StarB = EFTrackHypo_e12_medium1()
hypos.EFElectron_e12_medium1_L2StarB = TrigEFElectronHypo_e12_medium1()

hypos.L2Calo_e12Tvh_medium1_L2StarB = L2CaloHypo_e12_medium1()
hypos.L2Electron_e12Tvh_medium1_L2StarB = L2ElectronHypo_e12_medium1_L2StarB()
hypos.EFTrack_e12Tvh_medium1_L2StarB = EFTrackHypo_e12_medium1()
hypos.EFElectron_e12Tvh_medium1_L2StarB = TrigEFElectronHypo_e12_medium1()

#e7_medium1 L2Star B
hypos.L2Calo_e7_medium1_L2StarB = L2CaloHypo_e7_medium1()
hypos.L2Electron_e7_medium1_L2StarB = L2ElectronHypo_e7_medium1_L2StarB()
hypos.EFTrack_e7_medium1_L2StarB = EFTrackHypo_e7_medium1()
hypos.EFElectron_e7_medium1_L2StarB = TrigEFElectronHypo_e7_medium1()

hypos.L2Calo_e7T_medium1_L2StarB = L2CaloHypo_e7_medium1()
hypos.L2Electron_e7T_medium1_L2StarB = L2ElectronHypo_e7_medium1_L2StarB()
hypos.EFTrack_e7T_medium1_L2StarB = EFTrackHypo_e7_medium1()
hypos.EFElectron_e7T_medium1_L2StarB = TrigEFElectronHypo_e7_medium1()

# e22vh_medium L2Star A,B,C
hypos.L2Calo_e22vh_medium_L2StarA = L2CaloHypo_e22_medium()
hypos.L2Electron_e22vh_medium_L2StarA = L2ElectronHypo_e22_medium_L2StarA()
hypos.EFTrack_e22vh_medium_L2StarA = EFTrackHypo_e22_medium()
hypos.EFElectron_e22vh_medium_L2StarA = TrigEFElectronHypo_e22_medium()

hypos.L2Calo_e22vh_medium_L2StarB = L2CaloHypo_e22_medium()
hypos.L2Electron_e22vh_medium_L2StarB = L2ElectronHypo_e22_medium_L2StarB()
hypos.EFTrack_e22vh_medium_L2StarB = EFTrackHypo_e22_medium()
hypos.EFElectron_e22vh_medium_L2StarB = TrigEFElectronHypo_e22_medium()

hypos.L2Calo_e22vh_medium_L2StarC = L2CaloHypo_e22_medium()
hypos.L2Electron_e22vh_medium_L2StarC = L2ElectronHypo_e22_medium_L2StarC()
hypos.EFTrack_e22vh_medium_L2StarC = EFTrackHypo_e22_medium()
hypos.EFElectron_e22vh_medium_L2StarC = TrigEFElectronHypo_e22_medium()


# e24vh_medium1 L2Star A,B,C
hypos.L2Calo_e24vh_medium1_L2StarA = L2CaloHypo_e24_medium1()
hypos.L2Electron_e24vh_medium1_L2StarA = L2ElectronHypo_e24_medium1_L2StarA()
hypos.EFTrack_e24vh_medium1_L2StarA = EFTrackHypo_e24_medium1()
hypos.EFElectron_e24vh_medium1_L2StarA = TrigEFElectronHypo_e24_medium1()

hypos.L2Calo_e24vh_medium1_L2StarB = L2CaloHypo_e24_medium1()
hypos.L2Electron_e24vh_medium1_L2StarB = L2ElectronHypo_e24_medium1_L2StarB()
hypos.EFTrack_e24vh_medium1_L2StarB = EFTrackHypo_e24_medium1()
hypos.EFElectron_e24vh_medium1_L2StarB = TrigEFElectronHypo_e24_medium1()

hypos.L2Calo_e24vh_medium1_L2StarC = L2CaloHypo_e24_medium1()
hypos.L2Electron_e24vh_medium1_L2StarC = L2ElectronHypo_e24_medium1_L2StarC()
hypos.EFTrack_e24vh_medium1_L2StarC = EFTrackHypo_e24_medium1()
hypos.EFElectron_e24vh_medium1_L2StarC = TrigEFElectronHypo_e24_medium1()

##e12Tvh_loose1 L2StarB
hypos.L2Calo_e12Tvh_loose1_L2StarB = L2CaloHypo_e12_loose1()
hypos.L2Electron_e12Tvh_loose1_L2StarB = L2ElectronHypo_e12_loose1_L2StarB()
hypos.EFTrack_e12Tvh_loose1_L2StarB = EFTrackHypo_e12_loose1()
hypos.EFElectron_e12Tvh_loose1_L2StarB = TrigEFElectronHypo_e12_loose1()


# e12_medium L2Star A,B,C
hypos.L2Calo_e12_medium_L2StarA = L2CaloHypo_e12_medium()
hypos.L2Electron_e12_medium_L2StarA = L2ElectronHypo_e12_medium_L2StarA()
hypos.EFTrack_e12_medium_L2StarA = EFTrackHypo_e12_medium()
hypos.EFElectron_e12_medium_L2StarA = TrigEFElectronHypo_e12_medium()

hypos.L2Calo_e12_medium_L2StarB = L2CaloHypo_e12_medium()
hypos.L2Electron_e12_medium_L2StarB = L2ElectronHypo_e12_medium_L2StarB()
hypos.EFTrack_e12_medium_L2StarB = EFTrackHypo_e12_medium()
hypos.EFElectron_e12_medium_L2StarB = TrigEFElectronHypo_e12_medium()

hypos.L2Calo_e12_medium_L2StarC = L2CaloHypo_e12_medium()
hypos.L2Electron_e12_medium_L2StarC = L2ElectronHypo_e12_medium_L2StarC()
hypos.EFTrack_e12_medium_L2StarC = EFTrackHypo_e12_medium()
hypos.EFElectron_e12_medium_L2StarC = TrigEFElectronHypo_e12_medium()

# e5_medium L2Star A,B,C
hypos.L2Calo_e5_medium_L2StarA = L2CaloHypo_e5_medium()
hypos.L2Electron_e5_medium_L2StarA = L2ElectronHypo_e5_medium_L2StarA()
hypos.EFTrack_e5_medium_L2StarA = EFTrackHypo_e5_medium()
hypos.EFElectron_e5_medium_L2StarA = TrigEFElectronHypo_e5_medium()

hypos.L2Calo_e5_medium_L2StarB = L2CaloHypo_e5_medium()
hypos.L2Electron_e5_medium_L2StarB = L2ElectronHypo_e5_medium_L2StarB()
hypos.EFTrack_e5_medium_L2StarB = EFTrackHypo_e5_medium()
hypos.EFElectron_e5_medium_L2StarB = TrigEFElectronHypo_e5_medium()

hypos.L2Calo_e5_medium_L2StarC = L2CaloHypo_e5_medium()
hypos.L2Electron_e5_medium_L2StarC = L2ElectronHypo_e5_medium_L2StarC()
hypos.EFTrack_e5_medium_L2StarC = EFTrackHypo_e5_medium()
hypos.EFElectron_e5_medium_L2StarC = TrigEFElectronHypo_e5_medium()

##??????
#hypos.L2Calo_e5_medium_L2StarB = hypos.L2Calo_e5_medium_L2StarA
#hypos.L2Electron_e5_medium_L2StarB = L2ElectronHypo_e5_medium_L2StarB()
#hypos.EFTrack_e5_medium_L2StarB = hypos.EFTrack_e5_medium_L2StarA
#hypos.EFElectron_e5_medium_L2StarB = hypos.EFElectron_e5_medium_L2StarA

#hypos.L2Calo_e5_medium_L2StarC = hypos.L2Calo_e5_medium_L2StarA
#hypos.L2Electron_e5_medium_L2StarC = L2ElectronHypo_e5_medium_L2StarC()
#hypos.EFTrack_e5_medium_L2StarC = hypos.EFTrack_e5_medium_L2StarA
#hypos.EFElectron_e5_medium_L2StarC = hypos.EFElectron_e5_medium_L2StarA

# e5_tight1 L2Star A,B,C
hypos.L2Calo_e5_tight1_L2StarA = L2CaloHypo_e5_tight1()
hypos.L2Electron_e5_tight1_L2StarA = L2ElectronHypo_e5_tight1()
hypos.EFTrack_e5_tight1_L2StarA = EFTrackHypo_e5_tight1()
hypos.EFElectron_e5_tight1_L2StarA = TrigEFElectronHypo_e5_tight1()

hypos.L2Calo_e5_tight1_L2StarB = L2CaloHypo_e5_tight1()
hypos.L2Electron_e5_tight1_L2StarB = L2ElectronHypo_e5_tight1_L2StarB()
hypos.EFTrack_e5_tight1_L2StarB = EFTrackHypo_e5_tight1()
hypos.EFElectron_e5_tight1_L2StarB = TrigEFElectronHypo_e5_tight1()

hypos.L2Calo_e5_tight1_L2StarC = L2CaloHypo_e5_tight1()
hypos.L2Electron_e5_tight1_L2StarC = L2ElectronHypo_e5_tight1_L2StarC()
hypos.EFTrack_e5_tight1_L2StarC = EFTrackHypo_e5_tight1()
hypos.EFElectron_e5_tight1_L2StarC = TrigEFElectronHypo_e5_tight1()


##########################################################################################
#
# L2 & EF Chain configuration helper classes
#
##########################################################################################
L2TrackingFex_default = theTrigL2SiTrackFinder_eGammaA
L2ElectronFex_default = fexes.L2Electron_L2StarA

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    L2TrackingFex_default = theTrigIDSCAN_eGamma
    L2ElectronFex_default = L2ElectronFex_all()

class L2EFChain_e(L2EFChainDef):
    
    class L2Config:
        def __init__(self, suffix, 
                     TrigL2CaloHypo_1=None, TrigL2ElectronFex_1=None, TrigL2ElectronHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_'+suffix)
            self.TrigL2ElectronFex_1  = TrigL2ElectronFex_1  or L2ElectronFex_default
            #if TrigL2ElectronFex_1==None:
                #print 'No L2ElectronFex given, use default (suffix=%s)' % suffix
                #print 'self.TrigL2ElectronFex_1 = %s' % (self.TrigL2ElectronFex_1)
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
                               [L2TrackingFex_default], 
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
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Electron','BW:Egamma']
        else:
            self.groups = ['RATE:SingleElectron', 'BW:Egamma']
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
                           [theTrigSiTrack_eGamma_robust],
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

class L2EFChain_e_IdScan_HI(L2EFChain_e):

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
                           TrigEFIDSequence("ElectronHI","heavyIon","InsideOutNoTRT").getSequence() + [config.EFConfig.TrigEFTrackHypo_1],
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

#L2Star for 17.0.0
class L2EFChain_e_L2Star_fwdBackTrk(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigL2SiTrackFinder_eGammaA,
                            theTrigL2SiTrackFinder_eGammaB,
                            theTrigL2SiTrackFinder_eGammaC,
                            ],
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

class L2EFChain_e_NoIDTrkCut(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        
        # Step 2: ID Fex
        #append "_R1" to IDTrkNoCut chains running IDScan and SiTrack
        if '_R1' in self.sig_id:
            self.addL2Sequence(['L2_e_step1'],
                               [theTrigL2SiTrackFinder_eGammaA,
                                theTrigL2SiTrackFinder_eGammaB,
                                theTrigL2SiTrackFinder_eGammaC,
                                theTrigL2SiTrackFinder_eGammaBC,
                                theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma, theTrigTRTSegFinder_eGamma,
                                ],
                               'L2_e_step2')
            
        else:
            #remove IDScan and SiTrakck from IDTrkNoCut chains
            self.addL2Sequence(['L2_e_step1'],
                               [theTrigL2SiTrackFinder_eGammaA,
                                theTrigL2SiTrackFinder_eGammaB,
                                theTrigL2SiTrackFinder_eGammaC,
                                theTrigL2SiTrackFinder_eGammaBC,
                                theTrigTRTSegFinder_eGamma,
                                ],
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
                           theTrigEFIDInsideOut_Electron,
                           'EF_e_step2')
        self.addEFSequence('EF_e_step2', 
                           theTrigEFIDOutsideInTRTOnly_Electron,
                           'EF_e_step3')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step3', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step4')

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addL2Signature(['L2_e_step2'])
        self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])
        self.addEFSignature(['EF_e_step4'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id1'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id+'id2'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.sig_id),
            }


class L2EFChain_e_NoIDTrkCut_IDT(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step1', 
                           [theTrigCaloCellMaker_eGamma_cells, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')


        # Step 4: IDMerged Fex
  
        self.addEFSequence('EF_e_step1',
                           trkseqcombfull,
                           'EF_e_step2')
        
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step2', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step3')

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'1'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'2'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'1'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'2'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }

class L2EFChain_e_L2Star_NoIDTrkCut(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigL2SiTrackFinder_eGammaA, theTrigL2SiTrackFinder_eGammaB, theTrigL2SiTrackFinder_eGammaC],
                           'L2_e_step2')
        # Step 3: ID-Calo Fex and Hypo
        self.addL2Sequence(['L2_e_step2'],
                           [config.L2Config.TrigL2ElectronFex_1],
                           'L2_e_step3')
        
        
        ###### EF ######
        # Step 1: Calo Fex
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma_cells, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                           'EF_e_step1')
        # Step 2: ID Fex and Hypo
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOut_Electron,
                           'EF_e_step2')
        self.addEFSequence('EF_e_step2', 
                           theTrigEFIDOutsideInTRTOnly_Electron,
                           'EF_e_step3')
        # Step 3: ID-Calo Fex and Hypo
        self.addEFSequence('EF_e_step3', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
                           'EF_e_step4')

    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addL2Signature(['L2_e_step2'])
        self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])
        self.addEFSignature(['EF_e_step4'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id1'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id+'id2'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.sig_id),
            }
        
class L2EFChain_e_L2Star(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, strategy='',
                 l1threshold=None, l2calohypo=None, l2electronhypo=None,
                 eftrackhypo=None, efelectronhypo=None):
        ChainTemplate.__init__(self, sig, l2, ef)

        if l2calohypo == None:
            if hasattr(hypos, 'L2Calo_'+sig):
                l2calohypo = getattr(hypos, 'L2Calo_'+sig)
        if l2electronhypo == None:
            if hasattr(hypos, 'L2Electron_'+sig):
                l2electronhypo = getattr(hypos, 'L2Electron_'+sig)
        if eftrackhypo == None:
            if hasattr(hypos, 'EFTrack_'+sig):
                eftrackhypo = getattr(hypos, 'EFTrack_'+sig)
        if efelectronhypo == None:
            if hasattr(hypos, 'EFElectron_'+sig):
                efelectronhypo = getattr(hypos, 'EFElectron_'+sig)
            
        l2te = 'L2_%s' % self.sig_id
        efte = 'EF_%s' % self.sig_id
        self.addSequence(l1threshold, [theT2CaloEgamma_eGamma, l2calohypo], l2te+'cl').addSignature('L2')
        if strategy == 'A':
            self.continueSequence([theTrigL2SiTrackFinder_eGammaA], l2te+'id').addSignature('L2')
            self.continueSequence([fexes.L2Electron_L2StarA, l2electronhypo], l2te).addSignature('L2')
        elif strategy == 'B':
            self.continueSequence([theTrigL2SiTrackFinder_eGammaB], l2te+'id').addSignature('L2')
            self.continueSequence([fexes.L2Electron_L2StarB, l2electronhypo], l2te).addSignature('L2')
        elif strategy == 'C':
            self.continueSequence([theTrigL2SiTrackFinder_eGammaC], l2te+'id').addSignature('L2')
            self.continueSequence([fexes.L2Electron_L2StarC, l2electronhypo], l2te).addSignature('L2')
        else:
            self.continueSequence([theTrigL2SiTrackFinder_eGammaA, theTrigL2SiTrackFinder_eGammaB, theTrigL2SiTrackFinder_eGammaC], l2te+'id').addSignature('L2')
            self.continueSequence([fexes.L2Electron_L2StarA, l2electronhypo], l2te).addSignature('L2')
            
        self.continueSequence([theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], efte+'calo').addSignature('EF')
        self.continueSequence(theTrigEFIDInsideOut_Electron + [eftrackhypo], efte+'id').addSignature('EF')
        self.continueSequence([theTrigEgammaRec_eGamma, efelectronhypo], efte).addSignature('EF')
        
        self.addStreams('egamma')
        self.addGroups('RATE:SingleElectron', 'BW:Egamma')

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

class L2EFChain_e_TRT_Ringer(L2EFChain_e):

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Fex and Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_Ringer, config.L2Config.TrigL2CaloHypo_1],
                           'L2_e_step1')
        # Step 2: ID Fex
        self.addL2Sequence(['L2_e_step1'],
                           [theTrigTRTSegFinder_eGamma],
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
# Helper class for writing python code, eg HLT to L1 maps
#
##########################################################################################
class ElectronCodeHelper:
    def __init__(self):
        
        global Electrons

        iwidth = int(0)
        for echain in Electrons:
            if iwidth < len(echain.l2chain.sig_id):
                iwidth = len(echain.l2chain.sig_id) 
            
        for echain in Electrons:
            l2name = '\''+echain.l2chain.sig_id.rstrip()+'\''
            l1name = '\''+echain.l2chain.lower_chain_name.rstrip()+'\''
            lempty = ''
            for istep in range(len(echain.l2chain.sig_id), iwidth):
                lempty += ' '
            
            #print l2name+lempty+':'+l1name+','
            
        #l1map = {}
        for echain in Electrons:
            if echain.l2chain.lower_chain_name not in l1map:
                l1map[echain.l2chain.lower_chain_name] = echain.l2chain.inputTEs
                #print '\''+echain.l2chain.lower_chain_name+'\':',echain.l2chain.inputTEs,'\','

        #l1map = {}
        #for echain in Electrons:
            #if echain.l2chain.chain_counter != echain.efchain.chain_counter:
                #print echain.l2chain.sig_id+':',echain.l2chain.chain_counter,'!=',echain.efchain.chain_counter
                
        #l1map = {}
        #for echain in Electrons:
            #if echain.l2chain.chain_name != 'L2_'+echain.sig_id:
                #print echain.l2chain.sig_id+':',echain.l2chain.chain_name
            #if echain.efchain.chain_name != 'EF_'+echain.sig_id:
                #print echain.efchain.sig_id+':',echain.efchain.chain_name






#############################################
class L2EFChain_e_L2StarABrem(L2EFChain_e): 
    def defineSequences(self, config): 
        # L2 ######
        # Step 1: Calo Fex and Hypo 
        self.addL2Sequence(self.inputTEs, 
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
                           'L2_e_step1')    
        # Step 2: ID Fex 
        self.addL2Sequence(['L2_e_step1'], 
                           [theTrigL2SiTrackFinder_eGammaA_Brem], 
                           'L2_e_step2') 
        # Step 3: ID-Calo Fex and Hypo 
        self.addL2Sequence(['L2_e_step2'], 
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1], 
                           'L2_e_step3') 
        
        # EF ###### 
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

#############################################
class L2EFChain_e_CombinedTracking2step(L2EFChain_e): 
    def defineSequences(self, config): 
        # L2 ######
        # Step 1: Calo Fex and Hypo 
        self.addL2Sequence(self.inputTEs, 
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
                           'L2_e_step1')    


        self.addL2Sequence('L2_e_step1', 
                           trkseqcomb1st,
                           'L2_e_step2') 

        # Step 3: ID-Calo Fex and Hypo 
        self.addL2Sequence('L2_e_step2', 
                           [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1], 
                           'L2_e_step3') 

        # EF ###### 
        # Step 1: Calo Fex 
        self.addEFSequence('L2_e_step3', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                           'EF_e_step1') 
        #
        # Step 2: ID Fex and Hypo 
        self.addEFSequence('EF_e_step1', 
                           theTrigEFIDInsideOutMerged_Electron + [config.EFConfig.TrigEFTrackHypo_1], 
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

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }



############################################# Savannah #101482 1)
class L2EFChain_e_noL2ID(L2EFChain_e): 
    def defineSequences(self, config): 
        # L2 ######
        # Step 1: Calo Fex and Hypo 
        self.addL2Sequence(self.inputTEs, 
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
                           'L2_e_step1')

        # EF ###### 
        # Step 1: Calo Fex 
        self.addEFSequence('L2_e_step1', 
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
        #        self.addL2Signature(['L2_e_step2'])
        #       self.addL2Signature(['L2_e_step3'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            #            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
            #            'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }


############################################# Savannah #101482 2)
class L2EFChain_e_noL2IDnoEFID(L2EFChain_e): 
    def defineSequences(self, config): 
        # L2 ######
        # Step 1: Calo Fex and Hypo 
        self.addL2Sequence(self.inputTEs, 
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
                           'L2_e_step1')

        # EF ###### 
        # Step 1: Calo Fex 
        self.addEFSequence('L2_e_step1', 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                           'EF_e_step1') 

        # Step 3: ID-Calo Fex and Hypo 
        self.addEFSequence('EF_e_step1', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
                           'EF_e_step2') 
        
    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id),
            }










############################################# Savannah #101482 3)
class L2EFChain_e_noL2IDnoEF(L2EFChain_e): 

    def defineSequences(self, config): 
        # L2 ######
        # Step 1: Calo Fex and Hypo 
        self.addL2Sequence(self.inputTEs, 
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
                           'L2_e_step1')
     
        
    def defineSignatures(self):
        self.addL2Signature(['L2_e_step1'])


    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id),
            }



############################################# Savannah #101482 4)
class L2EFChain_e_noL2noEFID(L2EFChain_e): 
    def defineSequences(self, config): 
      
        # EF ###### 
        # Step 1: Calo Fex 
        self.addEFSequence(self.inputTEs, 
                           [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                           'EF_e_step1') 

        # Step 3: ID-Calo Fex and Hypo 
        self.addEFSequence('EF_e_step1', 
                           [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
                           'EF_e_step2') 
        
    def defineSignatures(self):
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id),
            }

############################################# Savannah #101482 5)
class L2EFChain_e_noL2(L2EFChain_e): 
    def defineSequences(self, config): 
  
        # EF ###### 
        # Step 1: Calo Fex 
        self.addEFSequence(self.inputTEs, 
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
        self.addEFSignature(['EF_e_step1'])
        self.addEFSignature(['EF_e_step2'])
        self.addEFSignature(['EF_e_step3'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id),
            }

