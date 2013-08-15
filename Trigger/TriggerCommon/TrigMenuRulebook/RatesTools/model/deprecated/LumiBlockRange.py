# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 15, 2011

@author: ggalster
'''

from model.parser.COOLtime import COOLtime
from model.physics.Signature import Signature

class LumiBlockRange():
    
    __run = None
    __luminosity = float()
    __luminosityErr = float()
    __signatures = dict()
    
    __lumiBlockFirst = int()
    __lumiBlockLast = int()
    
    def __init__(self, run = None):
        self.__run = None
        self.__luminosity = 0.0
        self.__luminosityErr = 0.0
        self.__signatures = {}
        
        self.__lumiBlockFirst = 0
        self.__lumiBlockLast = 0
        
        if run != None:
            self.SetRun(run)        


    def GetRun(self): return self.__run
    def SetRun(self, val): 
        self.__run = val

    def SetFirstLumiBlock(self, val): self.__lumiBlockFirst = val
    def SetLastLumiBlock(self, val): self.__lumiBlockLast = val
    
    def SetLumiBlockRange(self, val):
        self.__lumiBlockFirst = val[0]
        self.__lumiBlockLast = val[1]
    
    def GetLuminosity(self): return self.__luminosity
    def SetLuminosity(self, val): self.__luminosity = val
    
    def GetLuminosityError(self): return self.__luminosityErr
    def SetLuminosityError(self, val): self.__luminosityErr = val    
    
    def GetSignatures(self): return self.__signatures
    def SetSignatures(self, val): self.__signatures = val
    
    def GetSignature(self, val):
        if not val in self.__signatures.keys():
            sig = Signature()
            sig.SetName(val)
            sig.SetLumiBlockRange(self)
            self.__signatures[val] = sig 
        return self.__signatures[val]
    
    def AddSignature(self,val):
        val.SetLumiBlockRange(self)
        self.__signatures[val.GetName()] = val
        
    def RemoveSignature(self, val):
        self.__signatures.pop(val.GetName())
        