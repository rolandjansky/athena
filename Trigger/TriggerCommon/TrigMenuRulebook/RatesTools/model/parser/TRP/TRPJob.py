# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''
from model.parallelization.ProcessJob import ProcessJob

class TRPJob(ProcessJob):
    def __init__(self):
        ProcessJob.__init__(self)
        self.__fileName = ""
        self.__level = ""
        self.__rangeLumiBlock = (-1, -1)
        self.__stepSize = 10
        self.__blacklist = []
        

    __fileName = str()
    def GetFileName(self):
        return self.__fileName
    def SetFileName(self, val):
        self.__fileName = val
    
    
    __rangeLumiBlock = tuple()
    def GetLumiBlockRange(self):
        return self.__rangeLumiBlock
    def SetLumiBlockRange(self, val):
        self.__rangeLumiBlock = val 
    
    __stepSize = int()
    def GetStepSize(self):
        return self.__stepSize
    def SetStepSize(self, val):
        self.__stepSize = val    
    
    __blacklist = []
    def GetBlacklist(self):
        return self.__blacklist        
    def SetBlacklist(self, val):
        self.__blacklist = val