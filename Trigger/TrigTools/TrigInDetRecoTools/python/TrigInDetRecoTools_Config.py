# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.ConfiguredSettings import CommonSettings
from TrigInDetRecoTools.TrigInDetRecoTools_ConfigB import FactoryForStrategyB
from TrigInDetRecoTools.TrigInDetRecoTools_ConfigC import FactoryForStrategyC
from TrigInDetRecoTools.TrigInDetRecoTools_ConfigF import FactoryForStrategyF
from TrigInDetRecoTools.TrigInDetRecoTools_ConfigA import FactoryForStrategyA

class TrigL2PattRecoStrategyFactory() :
    __slots__=[]
    def __init__(self, name = 'TrigL2PattRecoStrategyFactory') :
        self.allowedStrNames = ['A', 'B', 'C', 'F']

        self.commonSettings = CommonSettings()
        self.factoryForStrB = FactoryForStrategyB()        
        self.factoryForStrC = FactoryForStrategyC()
        self.factoryForStrF = FactoryForStrategyF()
        self.factoryForStrA = FactoryForStrategyA()
    def createStrategy(self, instName, strName, roiPhi=0.1, roiEta=0.1, ptCut = 2500.0) :
        if strName in self.allowedStrNames :
            if instName in self.commonSettings.allowedInstanceNames :
                if strName=='B' :
                    configuredStrB = self.factoryForStrB.createStrategy(instName)
                    return configuredStrB      
                
                if strName=='A' :
                    configuredStrA = self.factoryForStrA.createStrategy(instName)
                    return configuredStrA      

                
                if strName=='C' :
                    configuredStrC = self.factoryForStrC.createStrategy(instName)
                    return configuredStrC
                
                if strName=='F' :
                    configuredStrF = self.factoryForStrF.createStrategy(instName)
                    return configuredStrF

            else :
                print "Instance "+instName+" of the Strategy"+strName+" is not supported !"
                return None
        else :
            print "Strategy"+strName+" is not supported !"
            return None



