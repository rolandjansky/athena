# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from config.dbInfo import *
#----------------------------------------------------------------------------------------------------
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.physics.TriggerStream import TriggerStream

from model.basics.DataColumn import DataColumn
#----------------------------------------------------------------------------------------------------
class StreamCollection(object):
    def __init__(self, timeFilter = DataSelection(), parentRun = None):
        self.SetParentRun(parentRun)

        self.__filterTime = DataSelection()
        
        self.__streams = {}
        self.__channelDict = None
        
        self.SetTimeFilter(timeFilter)


    def GetParentRun(self): return self.__parentRun
    def SetParentRun(self, val): self.__parentRun = val

    def GetTimeFilter(self): return self.__filterTime
    def SetTimeFilter(self, val):
        self.__filterTime = val        
        for streamName in self.__streams.keys():
            self.__streams[streamName].SetTimeFilter(val)
        
    
    def GetNames(self): return self.__streams.keys()
    def Get(self, val): return self.__streams[val]
 
    
    def LoadDictionary(self, ForceReload = False):
        if (self.__channelDict != None) and not ForceReload: return 
        query = COOLQuery()
        query.SetIoVSelection(self.GetTimeFilter())
        
        self.__channelDict = {}
        
        folder = query.GetFolderHandle(dbTRFolderName, dbString)
        
        for channelId in folder.listChannels():
            name = folder.channelName(channelId)
            #print "Stream: ",name
            self.__channelDict[name] = channelId


    def ToDataDictionary(self, *streamNames, **kwargs):
        extract = "OutputRate"
        if kwargs.has_key("extract") and kwargs["extract"] != None: extract = kwargs["extract"] 
         
        if len(streamNames) == 0:
            if not len(self.GetNames()) == 0:
                return self.ToDataDictionary(*self.GetNames(), **kwargs)
            else:
                return {}
        
        data = {"LumiBlock": DataColumn()}
        for name in streamNames:
            data[name] = DataColumn()

        for i in sorted(self.GetTimeFilter()):
            data["LumiBlock"].append(i.GetLumiBlock())
            for streamName in streamNames:
                val = None
                
                fName = "Get"+extract
                f = getattr(self.Get(streamName).GetRateInfo(i), fName)
                val = f()
                data[streamName].append(val)

        return data
            
            
        
        
    def Load(self, *values):
        self.LoadDictionary()
        
        theList = []
        if len(values):
            if "*" in values:
                theList = self.__channelDict.keys()
            else: 
                for val in values:
                    theList.append(val)
        else:
            theList = self.GetNames()
                
        
        for streamName in theList:
            if not self.__streams.has_key(streamName):
                stream = TriggerStream(streamName, self.GetTimeFilter(), self)
                stream.SetChannelId(self.__channelDict[streamName])
                self.__streams[streamName] = stream

            
            self.__streams[streamName].SetTimeFilter(self.GetTimeFilter())
            
            self.__streams[streamName].Load()
       
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    from model.COOL.COOLTime import COOLTime
    
    a = StreamCollection(DataSelection(COOLTime(187196, 1), COOLTime(187196, 20)))
    stream = "L1_L1A"   
    a.Load(stream)
    a.Load("L2_total")
    
    a.Get(stream)
    
    dataDict = a.ToDataDictionary()
    for i in range(len(dataDict["LumiBlock"])):
        print dataDict["LumiBlock"][i],"\t",dataDict[stream][i], "\t", a.Get(stream).GetRateInfo(COOLTime(187196, dataDict["LumiBlock"][i])).GetOutputRate()
        