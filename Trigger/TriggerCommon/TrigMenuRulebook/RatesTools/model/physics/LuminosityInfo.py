# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
class LuminosityInfo(object):
    def __init__(self, pit, parentCollection = None):
        self.SetLuminosityCollection(parentCollection)
        
        self.SetPointInTime(pit)
        
        self.SetLuminosity(None)
        self.SetLuminosityPerBunchCrossing(None)

    def GetLuminosityCollection(self): return self.__lumiCollection
    def SetLuminosityCollection(self, val): self.__lumiCollection = val
    
    def GetPointInTime(self): return self.__pit
    def SetPointInTime(self, val): self.__pit = val
    
    def GetLuminosity(self): return self.__luminosity
    def SetLuminosity(self, val): self.__luminosity = val
    
    def GetLuminosityPerBunchCrossing(self): return self.__luminosityPerBX
    def SetLuminosityPerBunchCrossing(self, val): self.__luminosityPerBX = val    
#----------------------------------------------------------------------------------------------------
