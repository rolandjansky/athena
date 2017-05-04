# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.COOL.COOLTime import COOLTime
#----------------------------------------------------------------------------------------------------
class PITObject(object):
    def __init__(self, pit = None):     
        self.SetPointInTime(pit)
        
    def GetPointInTime(self): return self.__pointInTime
    def SetPointInTime(self, val): self.__pointInTime = COOLTime(val)
#----------------------------------------------------------------------------------------------------
class TriggerRate(PITObject):
    def __init__(self, pit, rateIn, rateOut, prescale):
        PITObject.__init__(self, pit)
        self.SetInputRate(rateIn)
        self.SetOutputRate(rateOut)
        self.SetPrescale(prescale)
    
    def GetInputRate(self): return self.__inputRate
    def SetInputRate(self, val): self.__inputRate = val
    
    def GetOutputRate(self): return self.__outputRate
    def SetOutputRate(self, val): self.__outputRate = val    

    def GetPrescale(self): return self.__prescale
    def SetPrescale(self, val): self.__prescale = val        
#----------------------------------------------------------------------------------------------------
class TriggerChain(object):
    def __init__(self, chainName):
        pass
    
    def GetChainName(self): pass
    def SetChainName(self, val): pass
    
    # lower chain
    def GetLowerChainName(self): pass
    def SetLowerChainName(self, val): pass
    
    def GetLowerChain(self): pass
    def SetLowerChain(self, val): pass
    
    # (chain collection)
    def GetAssociatedChainCollection(self): pass
    def SetAssociatedChainCollection(self, val): pass
    
    
    # filters
    def GetInclusionFilter(self): pass
    def SetInclusionFilter(self, val): pass
    def GetExclusionFilter(self): pass
    def SetExclusionFilter(self, val): pass    
    def GetEffectiveFilter(self): pass
    def SetEffectiveFilter(self, val): pass        
    def GetFilters(self): pass
    def SetFilters(self, inclusion, exclusion): pass
    
    
    # TriggerRate collection


class TriggerChainCollection(object): 
    def GetAssociatedRun(self): pass
    def SetAssociatedRun(self, val): pass
    
    def GetAssociatedRun(self): pass
    def SetAssociatedRun(self, val): pass
    # TriggerChain collection
    # filters    
    pass

class PhysicsRun(object):
    pass

class PhysicsRunCollection(object):
    pass
