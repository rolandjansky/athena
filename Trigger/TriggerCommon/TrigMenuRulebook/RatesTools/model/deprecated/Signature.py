# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 15, 2011

@author: ggalster
'''

class Signature():
    __name = str()
    __run = None
    __lumiBlock = None
    __lumiBlockRange = None
    def __init__(self):
        self.__name = ""
        self.__run = None
        self.__lumiBlock = None
        self.__lumiBlockRange = None
        
        
        self.__rate = None
        self.__rateErr = None
        
        self.__prescale = None
        self.__prescaleErr = None
        
    def GetName(self): return self.__name
    def SetName(self, val): self.__name = val    
    
    def GetRun(self): return self.__run
    def SetRun(self, val): self.__run = val
            
    def SetLumiBlockRange(self, val):        
        self.__lumiBlock = None
        self.__lumiBlockRange = val
        self.SetRun(val.GetRun())
        
    def GetLumiBlockRange(self):        
        return self.__lumiBlockRange
    
    
    
    
    def GetRate(self): return self.__rate
    def SetRate(self, val): self.__rate = val
    
    def GetRateError(self): return self.__rateErr
    def SetRateError(self, val): self.__rateErr = val
    
    
    def GetPrescale(self): return self.__prescale
    def SetPrescale(self, val): self.__prescale = val
    
    def GetPrescaleError(self): return self.__prescaleErr
    def SetPrescaleError(self, val): self.__prescaleErr = val    