# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 15, 2011

@author: ggalster
'''

class Run(object):
    __runNumber = int()
    __lumiBlockRanges = []

    def __init__(self, runNumber = None):
        self.__runNumber = runNumber
        self.__lumiBlockRanges = []
        
    
    def GetRunNumber(self): return self.__runNumber
    def SetRunNumber(self, val): self.__runNumber = val


    def GetLumiBlockRanges(self, val): return self.__lumiBlockRanges
    def SetLumiBlockRanges(self, val): self.__lumiBlockRanges = val
    
    def AddLumiBlockRange(self, val):
        val.SetRun(self)
        self.__lumiBlockRanges.append(val)
        
    def RemoveLumiBlockRange(self, val): self.__lumiBlockRanges.remove(val)
    