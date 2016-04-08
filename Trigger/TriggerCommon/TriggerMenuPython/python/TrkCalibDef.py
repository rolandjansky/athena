# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for TrkCalib chains
###########################################################################
__author__  = 'O.Igonkina, P.Bell'
__version__=""
__doc__="Implementation of tau trigger sequence "
from TriggerMenuPython.HltConfig import *

from AthenaCommon.Logging import logging
from TriggerMenuPython.TriggerPythonConfig import *

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Tau
#from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Tau_robust
from TrigT2IDTau.T2IDTauConfig import T2IDTau_Tau


from TrigDetCalib.TrigDetCalibConfig import *
CheckForTracks = CheckForTracks('CheckForTracks')
trk_All  = CheckForTracks_Trk_All('CheckForTracks_Trk_All')

trk9_Central  = CheckForTracks_Trk9_Central('CheckForTracks_Trk9_Central')
trk16_Central = CheckForTracks_Trk16_Central('CheckForTracks_Trk16_Central')
trk29_Central = CheckForTracks_Trk29_Central('CheckForTracks_Trk29_Central')

trk9_Fwd      = CheckForTracks_Trk9_Fwd('CheckForTracks_Trk9_Fwd')
trk16_Fwd     = CheckForTracks_Trk16_Fwd('CheckForTracks_Trk16_Fwd')
trk29_Fwd     = CheckForTracks_Trk29_Fwd('CheckForTracks_Trk29_Fwd')


from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo

from TrigDetCalib.TrigDetCalibConfig import *

robListWriter = TrkROBListWriter('TrkROBListWriter')
robListWriter.RoIDescriptorLabels = [""]

robListWriter_IDSCAN_rejAll = TrkROBListWriter('TrkROBListWriter_IDSCAN_rejAll')
robListWriter_IDSCAN_rejAll.rejectAll=True

robListWriter_SiTrk_rejAll = TrkROBListWriter('TrkROBListWriter_SiTrk_rejAll')
robListWriter_SiTrk_rejAll.rejectAll=True

idSubDetListWriter = TrigSubDetListWriter('InnerDetectorSubDetListWriter')
idSubDetListWriter.Subdetectors = "ID"
idSubDetListWriter.extraROBs = [ 7798784 ]


theDummyFEX = DummyFEX()
anotherDummyFEX = DummyFEX()

L1InfoHypo_physics = L1InfoHypo('L1InfoHypo_physics')
L1InfoHypo_physics.TriggerTypeBitMask = 0x80
L1InfoHypo_physics.TriggerTypeBit = 0x80

seed1 = 'L1_TAU8'
roi1  = 'HA8'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    seed1 = 'L1_TAU6'
    roi1  = 'HA6'	

###########################################################################
#
class L2EFChain_TrkCalib_SGAlgos(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config)

        self.AlgoName = []
     
    class L2Config:
        def __init__(self, signature):
            self.signature = signature

    class EFConfig:
        def __init__(self, signature):
            self.signature = signature

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig

    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = ['IDTracks']
        self.groups = ['RATE:IDTracks','BW:Detector']
        self.trigger_type = ['hello']

    def defineSequences(self, config):
        if (self.sig_id == 'trk9_Central_Tau_IDCalib'):
            self.AlgoName = [trk9_Central]
        if (self.sig_id == 'trk9_Fwd_Tau_IDCalib'):
            self.AlgoName = [trk9_Fwd]

        if (self.sig_id == 'trk16_Central_Tau_IDCalib'):
            self.AlgoName = [trk16_Central]
        if (self.sig_id == 'trk16_Fwd_Tau_IDCalib'):
            self.AlgoName = [trk16_Fwd]

        if (self.sig_id == 'trk29_Central_Tau_IDCalib'):
            self.AlgoName = [trk29_Central]
        if (self.sig_id == 'trk29_Fwd_Tau_IDCalib'):
            self.AlgoName = [trk29_Fwd]


        #L2 sequences
        self.addL2Sequence(self.inputTEs,
                           self.AlgoName,
                           'L2_')
        
    def defineSignatures(self):
        #L2 signatures
        self.addL2Signature([])
        self.addL2Signature([])
        self.addL2Signature([])
        self.addL2Signature(['L2_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
             'L2_':     'L2' + self.sig_id,
            }
        pass

Config_SGAlgos   = L2EFChain_TrkCalib_SGAlgos.Config
L2create_SGAlgos = L2EFChain_TrkCalib_SGAlgos.L2Config
EFcreate_SGAlgos = L2EFChain_TrkCalib_SGAlgos.EFConfig


from TriggerJobOpts.TriggerFlags import TriggerFlags
seeds = ",".join([ x for x in TriggerFlags.Lvl1.items() if "TAU" in x ])


TrkCalibItems = [
    
    #trk9_Central_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk9_Central_Tau_IDCalib',
                               'L2_trk9_Central_Tau_IDCalib', 862, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                              EFcreate_SGAlgos(None))),
    #trk9_Fwd_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk9_Fwd_Tau_IDCalib',
                               'L2_trk9_Fwd_Tau_IDCalib', 473, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                              EFcreate_SGAlgos(None))),

    #trk16_Central_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk16_Central_Tau_IDCalib',
                               'L2_trk16_Central_Tau_IDCalib', 864, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                              EFcreate_SGAlgos(None))),
    #trk16_Fwd_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk16_Fwd_Tau_IDCalib',
                               'L2_trk16_Fwd_Tau_IDCalib', 865, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                              EFcreate_SGAlgos(None))),
    
    #trk29_Central_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk29_Central_Tau_IDCalib',
                               'L2_trk29_Central_Tau_IDCalib', 866, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                              EFcreate_SGAlgos(None))),
    #trk29_Fwd_Tau_IDCalib
    L2EFChain_TrkCalib_SGAlgos('trk29_Fwd_Tau_IDCalib',
                               'L2_trk29_Fwd_Tau_IDCalib', 868, seeds,'', None, [roi1],
                               Config_SGAlgos(L2create_SGAlgos(None),
                                               EFcreate_SGAlgos(None))),
    ]

