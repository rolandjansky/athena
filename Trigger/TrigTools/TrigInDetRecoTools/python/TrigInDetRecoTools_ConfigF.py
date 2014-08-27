# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2PattRecoStrategyF
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyF
 
from AthenaCommon.Logging import logging  
log = logging.getLogger("FactoryForStrategyF")

class FactoryForStrategyF() :
    __slots__=[]
    def __init__(self, name = 'FactoryForStrategyF') :
        self.strategy = None
        self.settings = SettingsForStrategyF()
    
    def createStrategy(self, instName) :
        if instName in self.settings.allowedInstanceNames :
             self.strategy = TrigL2PattRecoStrategyF("StrategyF_"+instName)
             self.strategy.pTCut = self.settings[('pTmin',instName)]
             self.strategy.Chi2Cut = self.settings[('Chi2Cut',instName)]
             return self.strategy
        else :
            print "Instance "+instName+" of the Strategy F is not supported !"
            return None


    
