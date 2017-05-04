# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.basics.DataColumn import DataColumn
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.physics.TimeInfo import TimeInfo
#----------------------------------------------------------------------------------------------------
class TimeCollection(object):
    def __init__(self, timeFilter, parentRun = None):
        self.SetParentRun(parentRun)
        
        self.SetTimeFilter(timeFilter)
        self.__time = {}
    
    def GetTimeFilter(self): return self.__timeFilter
    def SetTimeFilter(self, val): self.__timeFilter = val
    
    def GetParentRun(self): return self.__parentRun
    def SetParentRun(self, val): self.__parentRun = val 
    
    def GetTimeInfo(self, pointInTime): return self.__time[pointInTime]
    
    def ToDataDictionary(self):
        data = {"Time": DataColumn(), "UnixTime": DataColumn()}
    
        for i in sorted(self.GetTimeFilter()):
            data["Time"].append(self.GetTimeInfo(i).GetTime())
            data["UnixTime"].append(self.GetTimeInfo(i).GetUnixTime())
    
        return data
    
    def Load(self):
        
        lumiOld = DataSelection().FromList(self.__time.keys())
        lumiNew = self.GetTimeFilter()
        
        lumiCreate = lumiNew.Complement(lumiOld)
        
        if not lumiCreate.IsEmpty():
            
            for i in lumiCreate:
                self.__time[i] = TimeInfo(i, self)     
            
            query = COOLQuery()
            query.SelectSingleChannel(0)
            query.SetIoVSelection(lumiCreate)
            for obj, payload in query.GetFolder("/TRIGGER/LUMI/LBLB"):
                self.__time[obj.since()].SetStartTime(payload["StartTime"])
                self.__time[obj.since()].SetEndTime(payload["EndTime"])
                

        

        lumiDelete = lumiOld.Complement(lumiNew)
        for i in lumiDelete:
            self.__time.pop(i)
    
#----------------------------------------------------------------------------------------------------
