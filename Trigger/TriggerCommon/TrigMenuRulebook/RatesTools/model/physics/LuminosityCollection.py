# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.basics.DataColumn import DataColumn
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.physics.LuminosityInfo import LuminosityInfo
#----------------------------------------------------------------------------------------------------
class LuminosityCollection(object):
    def __init__(self, timeFilter, parentRun = None):
        self.SetParentRun(parentRun)
        
        self.SetTimeFilter(timeFilter)
        self.__lumi = {}
    
    def GetTimeFilter(self): return self.__timeFilter
    def SetTimeFilter(self, val): self.__timeFilter = val
    
    def GetParentRun(self): return self.__parentRun
    def SetParentRun(self, val): self.__parentRun = val 
    
    def GetLuminosityInfo(self, pointInTime): return self.__lumi[pointInTime]
    
    def ToDataDictionary(self):
        self.Load()
        data = {}
        
        for name in ["LumiBlock", "Luminosity", "LuminosityPerBunchCrossing"]:
            data[name] = DataColumn()
        
        for i in sorted(self.GetTimeFilter()):
            data["LumiBlock"].append(i.GetLumiBlock())
            data["Luminosity"].append(self.GetLuminosityInfo(i).GetLuminosity())
            data["LuminosityPerBunchCrossing"].append(self.GetLuminosityInfo(i).GetLuminosityPerBunchCrossing())
        return data
        
    
    def Load(self):
        #for lumiBlock in self.__lumi.keys():
        #    self.__lumi[lumiBlock].Load()
        
        lumiOld = DataSelection().FromList(self.__lumi.keys())
        lumiNew = self.GetTimeFilter()
        
        lumiCreate = lumiNew.Complement(lumiOld)
        
        if not lumiCreate.IsEmpty():
            
            for i in lumiCreate:
                self.__lumi[i] = LuminosityInfo(i, self) 
            
            
            query = COOLQuery()
            query.SelectSingleChannel(0)
            query.SetIoVSelection(lumiCreate)
            
            for obj, payload in query.GetFolder("/TRIGGER/LUMI/LBLESTONL"):
                self.__lumi[obj.since()].SetLuminosity(payload["LBAvInstLumi"])
                self.__lumi[obj.since()].SetLuminosityPerBunchCrossing(payload["LBAvEvtsPerBX"])
                    
        
        lumiDelete = lumiOld.Complement(lumiNew)
        for i in lumiDelete:
            self.__lumi.pop(i)
    
#----------------------------------------------------------------------------------------------------
