# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
class StreamRate(object):
    def __init__(self, pit, parentStream = None):
        self.SetPointInTime(pit)
        self.SetStream(parentStream)
        
        self.SetInputRate(None)
        self.SetOutputRate(None)        

    def GetStream(self): return self.__chain
    def SetStream(self, val): self.__chain = val
    
    def GetPointInTime(self): return self.__pit
    def SetPointInTime(self, val): self.__pit = val

    def GetInputRate(self): return self.__inputRate
    def SetInputRate(self, val): self.__inputRate = val
    
    def GetOutputRate(self): return self.__outputRate
    def SetOutputRate(self, val): self.__outputRate = val    
#----------------------------------------------------------------------------------------------------
