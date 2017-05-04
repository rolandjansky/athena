# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
class TimeInfo(object):
    def __init__(self, pit, parentCollection = None):
        self.SetTimeCollection(parentCollection)
        
        self.SetPointInTime(pit)
        
        self.SetStartTime(None)
        self.SetEndTime(None)

    def GetTimeCollection(self): return self.__timeCollection
    def SetTimeCollection(self, val): self.__timeCollection = val
    
    def GetPointInTime(self): return self.__pit
    def SetPointInTime(self, val): self.__pit = val
    
    def GetStartTime(self): return self.__begin
    def SetStartTime(self, val): self.__begin = val

    def GetEndTime(self): return self.__end
    def SetEndTime(self, val): self.__end = val
    
    def GetDuration(self):
        try:
            return self.GetEndTime() - self.GetStartTime()
        except:
            return None
            
    def GetTime(self):
        try:
            return (self.GetStartTime() + self.GetEndTime()) / 2.0
        except:
            return None
    
    def GetUnixTime(self):
        try:
            return int(self.GetTime() / 10.0**9)
        except:
            return None        
    
#----------------------------------------------------------------------------------------------------
