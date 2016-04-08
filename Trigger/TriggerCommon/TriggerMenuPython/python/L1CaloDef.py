# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for L1Calo 
#
#########################################################################

from TriggerMenuPython.HltConfig import *

from AthenaCommon.Include import include
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags  import TriggerFlags


from TrigDetCalib.TrigDetCalibConfig import L1CaloSubDetListWriter

l2_l1caloSubDetListWriter = L1CaloSubDetListWriter("L1CaloSubDetListWriter")
l2_l1caloSubDetListWriter.Subdetectors = "L1Calo"
l2_l1caloSubDetListWriter.MaxRoIsPerEvent = 1
l2_l1caloSubDetListWriter.addL1Calo = True

class L2EFChain_L1CaloPartialEventBuilding(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config)

        self.l1caloWriter = []

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix           
    
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
    
    def defineStreamGroupTriggerType(self):
        self.calib_streams = ['L1CaloCalib']
        self.groups = ['RATE:Calibration', 'RATE:L1CaloCalib','BW:Detector']

        
    def defineSequences(self, config):
        self.l1caloWriter = [l2_l1caloSubDetListWriter]
        
        
        self.addL2Sequence(self.inputTEs,
                           self.l1caloWriter,
                           'L2_')

    def defineSignatures(self):
        self.addL2Signature(['L2_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':     'L2_Calib' + self.sig_id,
            }
        pass


Config   = L2EFChain_L1CaloPartialEventBuilding.Config
L2create = L2EFChain_L1CaloPartialEventBuilding.L2Config
EFcreate = L2EFChain_L1CaloPartialEventBuilding.EFConfig

L1CaloCalibChains = [

    L2EFChain_L1CaloPartialEventBuilding("L1Calocalibration", "L2_L1CaloCalibration",22, 'L1_RD0_FILLED','',None,[''],
                                Config(L2create(None),
                                       EFcreate(None))),
    
    L2EFChain_L1CaloPartialEventBuilding("L1Calocalibration_L1BGRP7", "L2_L1CaloCalibration_L1BGRP7",33, 'L1_BGRP7','',None,[''],
                                         Config(L2create(None),
                                                EFcreate(None))),
    
    
                                         
    ]
