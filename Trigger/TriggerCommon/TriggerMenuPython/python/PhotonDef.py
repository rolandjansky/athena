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

    from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHCounter,TrigTRTHTHhypo
    theTrigTRTHTHCounter = TrigTRTHTHCounter()
    theTrigTRTHTHhypo = TrigTRTHTHhypo()

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


seed1 = 'L1_EM12'
roi1  = 'EM12'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    seed1 = 'L1_EM14'
    roi1  = 'EM14'

    
    
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
        if('jetcalib' in self.ef_chain_name):
            self.physics_streams = ['jetcalibdelayed']
            self.groups = ['RATE:SingleJet', 'BW:Jets']
        elif('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Photon','BW:Egamma']
        elif ('g200_etcut' in self.ef_chain_name):
            self.physics_streams = ['jets']       
            self.groups = ['RATE:SinglePhoton','BW:Egamma']
        else:
            self.groups = ['RATE:SinglePhoton','BW:Egamma']

        self.trigger_type = []

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.sig_id+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.sig_id),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.sig_id+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.sig_id),
            }

#-----------------------------------
class L2EFChain_g_hiptrt(L2EFChain_g):
#-----------------------------------
    
    class L2Config:
        def __init__(self, suffix,
                     TrigL2CaloHypo_1=None, TrigL2PhotonFex_1=None, TrigL2PhotonHypo_1=None):
            self.suffix = suffix
            self.TrigL2CaloHypo_1   = TrigL2CaloHypo_1   or TrigL2CaloHypo('TrigL2CaloHypo_'+suffix)
            self.trtHTHitCounter   = theTrigTRTHTHCounter
            self.trtHTHitHypo   = theTrigTRTHTHhypo
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

    def defineSequences(self, config):
        ###### L2 ######
        # Step 1: Calo Hypo
        self.addL2Sequence(self.inputTEs,
                           [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
                           'L2_g_step1')

        # Step 2: Photon Fex and Hypo
        self.addL2Sequence('L2_g_step1',
                           [config.L2Config.trtHTHitCounter, config.L2Config.trtHTHitHypo],
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
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Photon','BW:Egamma']
        else:
            self.groups = ['RATE:SinglePhoton','BW:Egamma']
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


class L2EFChain_gamma_NoAlg(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['egamma']
        self.calib_streams = []
        self.groups = ['RATE:SinglePhoton','BW:Egamma']


        self.trigger_type = ['hello']
        
Photons = [
    
    L2EFChain_g('g3_nocut', 'L2_g3_nocut', 609, 'L1_EM2', 'EF_g3_nocut', 609, ['EM2'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g3_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g3_nocut()))),
    
    L2EFChain_g('g3_loose', 'L2_g3_loose', 966, 'L1_EM2', 'EF_g3_loose', 966, ['EM2'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g3(), L2PhotonFex_1(), L2PhotonHypo_g3_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g3_loose()))),
    
    L2EFChain_g('g5_nocut', 'L2_g5_nocut', 572, 'L1_EM3', 'EF_g5_nocut', 572, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),

    L2EFChain_g('g5_nocut_cosmic', 'L2_g5_nocut_cosmic', 729, 'L1_EM3_EMPTY', 'EF_g5_nocut_cosmic', 729, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),

    L2EFChain_g('g5_NoCut_cosmic', 'L2_g5_NoCut_cosmic', 729, 'L1_EM3_EMPTY', 'EF_g5_NoCut_cosmic', 729, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),
    
    L2EFChain_g('g5_nocut_firstempty', 'L2_g5_nocut_firstempty', 964, 'L1_EM3_FIRSTEMPTY', 'EF_g5_nocut_firstempty', 964, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g5_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_nocut()))),
    
    L2EFChain_g('g9_etcut', 'L2_g9_etcut',251, 'L1_EM3', 'EF_g9_etcut', 251, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g9_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g9_etcut()))),
    
    L2EFChain_g('g11_etcut', 'L2_g11_etcut',252, 'L1_EM5', 'EF_g11_etcut', 252, ['EM5'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g11_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g11_etcut()))),

    L2EFChain_g('g13_etcut', 'L2_g13_etcut',2252, 'L1_EM10', 'EF_g13_etcut', 2252, ['EM10'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g10_nocut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g13_etcut()))),


    #needed for upsi chains 90881
    L2EFChain_g('g6T_etcut', 'L2_g6T_etcut',1413, 'L1_EM5', 'EF_g6T_etcut', 1413, ['EM6'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g6_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g6_etcut()))),


    ]


if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus() or \
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):    
    Photons += [
        L2EFChain_g('g15_loose', 'L2_g15_loose', 37, 'L1_EM10', 'EF_g15_loose', 37, ['EM10'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_loose()))), # validated
        L2EFChain_g('g10_medium', 'L2_g10_medium', 34, 'L1_EM5', 'EF_g10_medium', 34, ['EM5'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_medium()))), # validated
        L2EFChain_g('g10_tight', 'L2_g10_tight', 634, 'L1_EM5', 'EF_g10_tight', 634, ['EM5'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_tight()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_tight()))), # validated
        L2EFChain_g('g12_tight', 'L2_g12_tight', 635, 'L1_EM5', 'EF_g12_tight', 635, ['EM5'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_tight()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_tight()))), # validated
        L2EFChain_g('g15_medium', 'L2_g15_medium', 370, 'L1_EM10', 'EF_g15_medium', 370, ['EM10'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_medium()))), # validated
        L2EFChain_g('g15_tight', 'L2_g15_tight', 636, 'L1_EM10', 'EF_g15_tight', 636, ['EM10'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_tight()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_tight()))), # validated
        
        ]
    
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() or \
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus())   :    
    Photons += [
        L2EFChain_g('g10_NoCut_cosmic', 'L2_g10_NoCut_cosmic', 730, 'L1_EM6_EMPTY', 'EF_g10_NoCut_cosmic', 730, ['EM6'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g10_nocut()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_nocut()))),
        
        L2EFChain_g('g10_loose', 'L2_g10_loose', 40, 'L1_EM6', 'EF_g10_loose', 40, ['EM6'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose()))), # validated

        L2EFChain_g('g10_loose_TauMass', 'L2_g10_loose_TauMass', 3340, 'L1_EM6', 'EF_g10_loose_TauMass', 3340, ['EM6'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose_TauMass()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose_TauMass()))), # validated


        L2EFChain_g('g12Tvh_loose', 'L2_g12Tvh_loose', 972, 'L1_EM10VH', 'EF_g12Tvh_loose', 972, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_loose()))),

        L2EFChain_g('g12Tvh_loose_TauMass', 'L2_g12Tvh_loose_TauMass', 3972, 'L1_EM10VH', 'EF_g12Tvh_loose_TauMass', 3972, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_loose_TauMass()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_loose_TauMass()))),


        L2EFChain_g('g12Tvh_medium', 'L2_g12Tvh_medium', 568, 'L1_EM10VH', 'EF_g12Tvh_medium', 568, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_medium()))),


        L2EFChain_g('g15vh_loose', 'L2_g15vh_loose', 3937, 'L1_EM10VH', 'EF_g15vh_loose', 3937, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_loose()))), # validated

        L2EFChain_g('g15vh_medium', 'L2_g15vh_medium', 38, 'L1_EM10VH', 'EF_g15vh_medium', 38, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_medium()))), # validated
        
        L2EFChain_g('g10_medium', 'L2_g10_medium', 34, 'L1_EM6', 'EF_g10_medium', 34, ['EM6'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_medium()))), # validated

        L2EFChain_g('g15_loose', 'L2_g15_loose', 42, 'L1_EM6', 'EF_g15_loose', 42, ['EM6'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g15_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g15_loose()))), # validated
    


        ]
else:
    Photons += [
        L2EFChain_g('g10_NoCut_cosmic', 'L2_g10_NoCut_cosmic', 730, 'L1_EM5_EMPTY', 'EF_g10_NoCut_cosmic', 730, ['EM5'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g10_nocut()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_nocut()))),

        L2EFChain_g('g10_loose', 'L2_g10_loose', 45, 'L1_EM5', 'EF_g10_loose', 45, ['EM5'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose()))), # validated        
        
        L2EFChain_g('g12_loose', 'L2_g12_loose', 600, 'L1_EM7', 'EF_g12_loose', 600, ['EM7'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_loose()))), 

	L2EFChain_g('g12_medium', 'L2_g12_medium', 601, 'L1_EM7', 'EF_g12_medium', 601, ['EM7'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g12_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g12_medium()))),
        ]
    

Photons += [
    
    L2EFChain_g('g17_etcut', 'L2_g17_etcut',253, 'L1_EM10', 'EF_g17_etcut', 253, ['EM10'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g17_etcut()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g17_etcut()))),
    
    L2EFChain_g('g20_etcut', 'L2_g20_etcut',525, seed1, 'EF_g20_etcut', 525, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g20_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g20_etcut()))),

    L2EFChain_g('g24_etcut', 'L2_g24_etcut',107, seed1, 'EF_g24_etcut', 107, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g24_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g24_etcut()))),

    L2EFChain_g('g27_etcut', 'L2_g27_etcut', 1638, seed1, 'EF_g27_etcut', 1638, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g27_etcut()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g27_etcut()))),

    L2EFChain_g('g50_etcut', 'L2_g50_etcut', 801, 'L1_EM30', 'EF_g50_etcut', 801, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g50_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g50_etcut()))),

    L2EFChain_g('g100_etcut', 'L2_g100_etcut', 948, 'L1_EM30', 'EF_g100_etcut', 802, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g100_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g100_etcut()))),
    
    L2EFChain_g('g150_etcut', 'L2_g150_etcut', 803, 'L1_EM30', 'EF_g150_etcut', 803, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g150_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g150_etcut()))),
    
    L2EFChain_g('g200_etcut', 'L2_g200_etcut', 804, 'L1_EM30', 'EF_g200_etcut', 804, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g_nocut(), L2PhotonFex_1(), L2PhotonHypo_g200_etcut()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g200_etcut()))),
    
    L2EFChain_g('g5', 'L2_g5', 577, 'L1_EM3', 'EF_g5', 577, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5()))),
    
    L2EFChain_g('g5_loose', 'L2_g5_loose', 947, 'L1_EM3', 'EF_g5_loose', 947, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5_loose()))), 

    L2EFChain_g('g5_loose_TauMass', 'L2_g5_loose_TauMass', 2947, 'L1_EM3', 'EF_g5_loose_TauMass', 2947, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_loose_TauMass()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g5_loose_TauMass()))), 


    L2EFChain_g('g5_medium', 'L2_g5_medium', 29, 'L1_EM3', 'EF_g5_medium', 29, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_medium()))),
   
   L2EFChain_g('g5_tight', 'L2_g5_tight', 618, 'L1_EM3', 'EF_g5_tight', 618, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_tight()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_tight()))),

    L2EFChain_g('g5_loose_L1TE50', 'L2_g5_loose_L1TE50', 920, 'L1_EM3_TE50', 'EF_g5_loose_L1TE50', 920, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g5_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g5_loose()))), # validated

    L2EFChain_g('g7_loose', 'L2_g7_loose', 143, 'L1_EM3', 'EF_g7_loose', 143, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g7_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g7_loose()))),

    L2EFChain_g('g7_medium', 'L2_g7_medium', 144, 'L1_EM3', 'EF_g7_medium', 144, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g7_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g7_medium()))),

    L2EFChain_g('g7_tight', 'L2_g7_tight', 627, 'L1_EM3', 'EF_g7_tight', 627, ['EM3'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g7_tight()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g7_tight()))),

    L2EFChain_g('g9_tight', 'L2_g9_tight', 630, 'L1_EM5', 'EF_g9_tight', 630, ['EM5'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g9_tight()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g9_tight()))),

    L2EFChain_g('g6_loose', 'L2_g6_loose', 935, 'L1_EM3', 'EF_g6_loose', 935, ['EM4'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g4(), L2PhotonFex_1(), L2PhotonHypo_g6_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g6_loose()))), 

    L2EFChain_g('g10_loose_NL', 'L2_g10_loose_NL', 46, 'L1_EM3_NL', 'EF_g10_loose_NL', 46, ['EM3'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g10_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g10_loose()))), # validated

    L2EFChain_g('g20_loose', 'L2_g20_loose', 39, seed1, 'EF_g20_loose', 39, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose()))), # validated

    # Delayed
    L2EFChain_g('g20_loose_jetcalibdelayed', 'L2_g20_loose', 2100, seed1, 'EF_g20_loose_jetcalibdelayed', 2100, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose()))), # validated

    L2EFChain_g('g20_medium', 'L2_g20_medium', 708, seed1, 'EF_g20_medium', 708, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_medium()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_medium()))),

    L2EFChain_g('g20_medium_AthSel', 'L2_g20_medium_AthSel', 5708, seed1, 'EF_g20_medium_AthSel', 5708, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_medium()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_medium_AthSel()))),

    L2EFChain_g('g20_tight', 'L2_g20_tight', 798, seed1, 'EF_g20_tight', 798, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_tight()))), 
    
    L2EFChain_g('g20i_loose', 'L2_g20i_loose', 40, 'L1_EM14I', 'EF_g20i_loose', 40, ['EM14I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20i_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20i_loose()))), # validated
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    Photons += [

        L2EFChain_g('g20vh_loose', 'L2_g20vh_loose', 12, 'L1_EM10VH', 'EF_g20vh_loose', 12, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose()))),
        
        L2EFChain_g('g20vh_medium', 'L2_g20vh_medium', 3913, 'L1_EM10VH', 'EF_g20vh_medium', 3913, ['EM10VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g20_medium()))),

        L2EFChain_g('g20Tvh_loose', 'L2_g20Tvh_loose', 35, 'L1_EM16VH', 'EF_g20Tvh_loose', 35, ['EM16VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose()))),

        L2EFChain_g('g20Tvh_loose_TauMass', 'L2_g20Tvh_loose_TauMass', 3235, 'L1_EM16VH', 'EF_g20Tvh_loose_TauMass', 3235, ['EM16VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose_TauMass()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose_TauMass()))),


        L2EFChain_g('g20Tvh_medium', 'L2_g20Tvh_medium', 731, 'L1_EM16VH', 'EF_g20Tvh_medium', 731, ['EM16VH'],
                    L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_medium()),
                                       L2EFChain_g.EFConfig('', EFPhotonHypo_g20_medium()))),
        
        ]

Photons += [
    L2EFChain_g('g25_loose', 'L2_g25_loose', 42, seed1, 'EF_g25_loose', 42, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g25_loose()))), # validated
    
    L2EFChain_g('g25i_loose', 'L2_g25i_loose', 41, 'L1_EM14I', 'EF_g25i_loose', 41, ['EM14I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25i_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25i_loose()))), # validated

    L2EFChain_g('g25_medium', 'L2_g25_medium', 5016, seed1, 'EF_g25_medium', 5016, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g25_medium()))), 
    
    L2EFChain_g('g25_tight', 'L2_g25_tight', 294, seed1, 'EF_g25_tight', 294, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25_tight()))), # validated
    
    L2EFChain_g('g25i_tight', 'L2_g25i_tight', 293, 'L1_EM14I', 'EF_g25i_tight', 293, ['EM14I'],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g25i_tight()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g25i_tight()))), # validated
    
    L2EFChain_g('g30_loose', 'L2_g30_loose', 82, seed1, 'EF_g30_loose', 82, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g30_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g30_loose()))),

    L2EFChain_g('g30_medium', 'L2_g30_medium', 83, seed1, 'EF_g30_medium', 83, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g30_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g30_medium()))),

    L2EFChain_g('g30_medium_AthSel', 'L2_g30_medium_AthSel', 5583, seed1, 'EF_g30_medium_AthSel', 5583, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g30_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g30_medium_AthSel()))),

    L2EFChain_g('g35_loose', 'L2_g35_loose', 707, 'L1_EM30', 'EF_g35_loose', 707, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g35_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g35_loose()))),
    
    L2EFChain_g('g35_medium', 'L2_g35_medium', 5017, 'L1_EM30', 'EF_g35_medium', 5017, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g35_medium()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g35_medium()))),

    L2EFChain_g('g40_loose', 'L2_g40_loose', 579, 'L1_EM30', 'EF_g40_loose', 579, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g40_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g40_loose()))),

    # delayed
    L2EFChain_g('g40_loose_jetcalibdelayed', 'L2_g40_loose', 2101, 'L1_EM30', 'EF_g40_loose_jetcalibdelayed', 2101, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g40_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g40_loose()))),
    
    L2EFChain_g('g40_tight', 'L2_g40_tight', 867, 'L1_EM30', 'EF_g40_tight', 867, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g40_tight()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g40_tight()))),
        
    L2EFChain_g('g50_loose', 'L2_g50_loose', 703, seed1, 'EF_g50_loose', 703, [roi1],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g50_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g50_loose()))),

    L2EFChain_g('g60_loose', 'L2_g60_loose', 501, 'L1_EM30', 'EF_g60_loose', 501, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g60_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g60_loose()))),

    L2EFChain_g('g60_loose_jetcalibdelayed', 'L2_g60_loose', 2102, 'L1_EM30', 'EF_g60_loose_jetcalibdelayed', 2102, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g60_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g60_loose()))),

    L2EFChain_g('g80_loose', 'L2_g80_loose', 503, 'L1_EM30', 'EF_g80_loose', 503, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g80_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g80_loose()))),
    
    L2EFChain_g('g100_loose', 'L2_g100_loose', 524, 'L1_EM30', 'EF_g100_loose', 524, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g100_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g100_loose()))),    

    L2EFChain_g('g120_loose', 'L2_g120_loose', 536, 'L1_EM30', 'EF_g120_loose', 536, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g120_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g120_loose()))),    

    L2EFChain_g('g120_loose_AthSel', 'L2_g120_loose_AthSel', 5836, 'L1_EM30', 'EF_g120_loose_AthSel', 5836, ['EM30'],
                L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g120_loose()),
                                   L2EFChain_g.EFConfig('', EFPhotonHypo_g120_loose_AthSel()))),    

    
    ##     ######################################################
    ##     # Chains for exotic monopoles
    ##     ######################################################

    L2EFChain_g_hiptrt('g22_hiptrt', 'L2_g22_hiptrt', 1609, 'L1_EM16', 'EF_g22_hiptrt', 1609, ['EM16'],
                       L2EFChain_g_hiptrt.Config(L2EFChain_g_hiptrt.L2Config('', L2CaloHypo_g_nocut()),
                                                 L2EFChain_g_hiptrt.EFConfig('', EFPhotonHypo_g10_nocut()))),

    ##g_nocut_hiptrtL2.
    L2EFChain_g_hiptrt('g_nocut_hiptrtL2', 'L2_g_nocut_hiptrtL2', 992, 'L1_EM18VH', 'EF_g_nocut_hiptrtL2', 992, ['EM18VH'],
                       L2EFChain_g_hiptrt.Config(L2EFChain_g_hiptrt.L2Config('', L2CaloHypo_g_nocut()),
                                                 L2EFChain_g_hiptrt.EFConfig('', EFPhotonHypo_g10_nocut()))),
   
    L2EFChain_g_hiptrt('g_nocut_hiptrtL2_L1EM30', 'L2_g_nocut_hiptrtL2_L1EM30', 13, 'L1_EM30', 'EF_g_nocut_hiptrtL2_L1EM30', 13, ['EM30'],
                       L2EFChain_g_hiptrt.Config(L2EFChain_g_hiptrt.L2Config('', L2CaloHypo_g_nocut()),
                                                 L2EFChain_g_hiptrt.EFConfig('', EFPhotonHypo_g10_nocut()))),
    


    ##     ######################################################
    ##     # Chains with photon conversion reconstruction at EF
    ##     ######################################################

    L2EFChain_g_EFID('g20_loose_cnv', 'L2_g20_loose_cnv', 1999, seed1, 'EF_g20_loose_cnv', 1999, [roi1],
                     L2EFChain_g.Config(L2EFChain_g.L2Config('', L2CaloHypo_g7(), L2PhotonFex_1(), L2PhotonHypo_g20_loose()),
                                        L2EFChain_g.EFConfig('', EFPhotonHypo_g20_loose_cnv()))), # validated

    L2EFChain_gamma_NoAlg('L1EM3_NoAlg', 
                          'L2_L1EM3_NoAlg', 921, 'L1_EM3', 
                          'EF_L1EM3_NoAlg', 921,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),

    L2EFChain_gamma_NoAlg('L1EM5_NoAlg',
                          'L2_L1EM5_NoAlg', 580, 'L1_EM5',
                          'EF_L1EM5_NoAlg', 580,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),

    L2EFChain_gamma_NoAlg('L1EM10_NoAlg', 
                          'L2_L1EM10_NoAlg', 942, 'L1_EM10', 
                          'EF_L1EM10_NoAlg', 942,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),

    L2EFChain_gamma_NoAlg('L1EM12_NoAlg', 
                          'L2_L1EM12_NoAlg', 941, 'L1_EM12', 
                          'EF_L1EM12_NoAlg', 941,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),

    L2EFChain_gamma_NoAlg('L1EM14_NoAlg', 
                          'L2_L1EM14_NoAlg', 940, 'L1_EM14', 
                          'EF_L1EM14_NoAlg', 940,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),

    L2EFChain_gamma_NoAlg('L1EM16_NoAlg', 
                          'L2_L1EM16_NoAlg', 939, 'L1_EM16', 
                          'EF_L1EM16_NoAlg', 939,
                          [],
                          L2EFChain_g.Config(L2EFChain_g.L2Config(''), L2EFChain_g.EFConfig(''))),
    ]
