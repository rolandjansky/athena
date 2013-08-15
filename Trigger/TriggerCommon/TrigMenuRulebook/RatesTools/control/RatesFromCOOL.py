# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from config.dbInfo import *
#----------------------------------------------------------------------------------------------------
from PyCool import cool
#----------------------------------------------------------------------------------------------------
from model.COOL.COOLTime import COOLTime
from model.parser.COOLquery import COOLquery
#----------------------------------------------------------------------------------------------------
class PITObject(object):
    def __init__(self, time = COOLTime()):
        self.SetPointInTime(time)
    
    def GetPointInTime(self): return self.__time
    def SetPointInTime(self, val): self.__time = val
    
    def GetRunNumber(self): return self.__time.GetRunNumber()
    def SetRunNumber(self, val): self.__time.SetRunNumber(val)

    def GetLumiBlock(self): return self.__time.GetLumiBlock()
    def SetLumiBlock(self, val): self.__time.SetLumiBlock(val)    
#----------------------------------------------------------------------------------------------------    
class TriggerRate(PITObject):    
    def __init__(self, name = None, time = COOLTime()):
        PITObject.__init__(self, time)
        self.SetName(name)
        
        
    def GetName(self): return self.__name
    def SetName(self, val): self.__name = val
    
    def GetInput(self): return self.__rateInput
    def SetInput(self, val): self.__rateInput = val

    def GetPrescale(self): return self.__ratePrescale
    def SetPrescale(self, val): self.__ratePrescale = val
    
    def GetOutput(self): return self.__rateOutput
    def SetOutput(self, val): self.__rateOutput = val


    def LoadFromCOOL(self):
        fullName = self.GetName()
        
        if not len(fullName): raise Exception("Cannot load rate: No name given")
        
        
        level = fullName[0:2]
        print level
        
        q = COOLquery()
        q.SetIOVStart(self.GetPointInTime())
        q.SetIOVStop(self.GetPointInTime())
        
        channelId = None        
        
        if level == "L1":
            
            for obj in q.GetFolder("/TRIGGER/LVL1/Menu"):
                payload = obj.payload()
                if payload["ItemName"] == fullName:
                    channelId = obj.channelId()
                    break
            
            if channelId == None:
                raise Exception("ChannelId could not be determined")
            
            
            
            counterTypes = ["L1Accept", "BeforePrescale", "AfterPrescale"]
            
            turnCounters = {}
            counters = {}
            rates = {}
            for obj in q.GetFolder("/TRIGGER/LVL1/CTPCORELBDATA"):
                payload = obj.payload()
                for type in counterTypes:
                    turnCounters[type] = float(payload[type + "TurnCounter"])
                    
            
            q.SetChannelSelection(channelId)
            
            for obj in q.GetFolder("/TRIGGER/LUMI/LVL1COUNTERS"):
                payload = obj.payload()
                for type in counterTypes:                    
                    counters[type] = float(payload[type])                
            
            for type in counterTypes:
                rates[type] = counters[type] / turnCounters[type] * 11246.0            
            
            self.SetOutput(rates["L1Accept"])
            self.SetPrescale(rates["AfterPrescale"])
            self.SetInput(rates["BeforePrescale"])
            
            
        elif level in ["L2", "EF"]:
            for obj in q.GetFolder("/TRIGGER/HLT/Menu"):
                payload = obj.payload()
                if payload["ChainName"] == fullName:
                    channelId = obj.channelId()
                    break
            
            if channelId == None:
                raise Exception("ChannelId could not be determined")            
        
            
            db = cool.DatabaseSvcFactory.databaseService().openDatabase(dbString)
            folder = db.getFolder(dbFolderName)
            
            
            try:
                obj = folder.findObject(self.GetPointInTime().GetCOOLTime(), cool.ChannelSelection(channelId).firstChannel())
            except:
                raise Exception("Rates could not be read from COOL")
            
            payload = obj.payload()
            
            for t in ["Input", "Prescale", "Raw", "Output"]:
                print t, payload[t]
#----------------------------------------------------------------------------------------------------    
class Run(PITObject):
    def __init__(self, runNumber = None):
        PITObject.__init__(self, COOLTime(runNumber, 0))
        
        self.SetChainSelection([])
        self.SetFilters([])
    
    
    def GetFilters(self): return self.__filters
    def SetFilters(self, val): self.__filters = val
    
    def AddFilter(self, val): pass
    def RemoveFilter(self, val): pass
    
    
    def IsLumiBlockFiltered(self, lumiBlock):
        pass
    
    
    
    
    def GetChainSelection(self): return self.__chainSelection
    def SetChainSelection(self, val): self.__chainSelection = val
    
    def LoadFromCOOL(self):
        pass


#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    
    r = Run(187196)
    print r.GetPointInTime() 
    
    a = TriggerRate("L1_EM16", COOLTime(187196, 123))
    a.LoadFromCOOL()
    
    
    b = TriggerRate("L2_e20_medium", COOLTime(187196, 123))
    b.LoadFromCOOL()
    
    b = TriggerRate("EF_e20_medium", COOLTime(187196, 123))
    b.LoadFromCOOL()
    
    
    