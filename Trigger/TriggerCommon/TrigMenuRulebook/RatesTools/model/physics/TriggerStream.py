# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.COOL.COOLTime import COOLTime
#----------------------------------------------------------------------------------------------------
from config.dbInfo import *
#----------------------------------------------------------------------------------------------------
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime
from model.physics.StreamRate import StreamRate
#----------------------------------------------------------------------------------------------------
class TriggerStream(object):
    def __init__(self, name, timeFilter = DataSelection(), parentCollection = None):
        self.SetStreamCollection(parentCollection)
        
        self.__filterTime = DataSelection()
        self.__rates = {}
        
        self.SetName(name)
        self.SetTimeFilter(timeFilter)
        self.SetChannelId(None)
        
        self.SetIsDummy(True)
    
    def GetIsDummy(self): return self.__isDummy
    def SetIsDummy(self, val): self.__isDummy = val

    def GetStreamCollection(self): return self.__streamCollection
    def SetStreamCollection(self, val): self.__streamCollection = val
    
    def GetName(self): return self.__name
    def SetName(self, val): self.__name = val
    
    def GetLevel(self):
        return self.GetName()[0:2] 
    
    def GetChannelId(self):
        if self.__id != None:
            return self.__id
        else:
            query = COOLQuery()

            folder = query.GetFolderHandle(dbTRFolderName, dbString, True)
            
            for channelId in folder.listChannels():
                name = folder.channelName(channelId)
                if name == self.GetName():
                    self.SetChannelId(channelId)             
                    #print name, self.GetName(), channelId
                    break
                
            if self.__id == None:
                raise Exception("Could not get channelId for " + self.GetName())
        
            return self.GetChannelId()
      
    def SetChannelId(self, val): self.__id = val
        
    def GetTimeFilter(self): return self.__filterTime
    def SetTimeFilter(self, val):
        self.__filterTime = val
            
    
    def GetRateInfo(self,pointInTime): return self.__rates[pointInTime]
    
    def Load(self):        
        ratesOldTimes = DataSelection().FromList(self.__rates.keys())
        ratesNewTimes = self.GetTimeFilter()
        
        ratesCreateTimes = ratesNewTimes.Complement(ratesOldTimes)
        
        if not ratesCreateTimes.IsEmpty():
            timeSet = ratesCreateTimes.ToSet()
            
            # Make default rates
            for i in timeSet:
                self.__rates[i] = StreamRate(COOLTime(i), self)
            
            if not self.GetChannelId():
                print >> sys.stderr, "Warning: Channel %s not found. Loading dummy." % self.GetName()
            else:

                # Fill raw rates
                query = COOLQuery()
                query.SetIoVSelection(ratesCreateTimes)
                query.SelectSingleChannel(self.GetChannelId())
            
                result = query.GetFolder(dbTRFolderName, dbString)
            
                for obj, payload in result:
                    self.__rates[obj.since()].SetInputRate(payload["Input"])
                    self.__rates[obj.since()].SetOutputRate(payload["Output"])
            
                self.SetIsDummy((len(result) == 0) and not self.GetIsDummy() == False)
                
                
        deleteRates = ratesOldTimes.Complement(ratesNewTimes)
        for i in deleteRates:
            self.__rates.pop(i)

#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":    
        
    a = TriggerStream("L1_L1A", DataSelection(COOLTime(187196, 0x0000), COOLTime(187196, 20)))
    a.Load()

    for i in sorted(a.GetTimeFilter()):
        print i, "\t", a.GetRateInfo(i).GetOutputRate()
    
