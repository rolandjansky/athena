# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.basics.DataRange import DataRange
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime
from model.physics.TriggerChain import TriggerChain

from model.basics.DataColumn import DataColumn
#----------------------------------------------------------------------------------------------------
class ChainCollection(object):
    def __init__(self, timeFilter = DataSelection(), parentRun = None):
        self.SetParentRun(parentRun)

        self.__filterTime = DataSelection()
        
        self.__chains = {}
        
        self.__loadedRuns = set([])
        self.__channelDict = {}
        self.__uniqueDict = {}
        self.__lowerChainDict = {}
        
        self.SetTimeFilter(timeFilter)


    def GetParentRun(self): return self.__parentRun
    def SetParentRun(self, val): self.__parentRun = val

    def GetTimeFilter(self): return self.__filterTime
    def SetTimeFilter(self, val):
        self.__runNumbers = set([])
        for timeRange in val.GetListOfRanges():
            for runNumber in range(COOLTime(timeRange.GetFirst()).GetRunNumber(), \
                                   COOLTime(timeRange.GetLast()).GetRunNumber()+1):
                self.__runNumbers.add(runNumber)
                        
        self.__filterTime = val        
        for chainName in self.__chains.keys():
            self.__chains[chainName].SetTimeFilter(val)
        
    
    def GetNames(self): return self.__chains.keys()
    def Get(self, val): return self.__chains[val]
 
 
    def GetAllNames(self): return self.__channelDict.keys()
 
    def LoadDictionaries(self, ForceReload = False):
        if ForceReload:
            self.__loadedRuns = set([])
            self.__channelDict = {}
            self.__uniqueDict = {}
            self.__lowerChainDict = {}            

        def allocate_dictionary(varName):
            if not self.__channelDict.has_key(varName):
                self.__channelDict[varName] = {}
            if not self.__uniqueDict.has_key(varName):
                self.__uniqueDict[varName] = {}
            if not self.__lowerChainDict.has_key(varName):
                self.__lowerChainDict[varName] = set([])
        
        for runNumber in self.__runNumbers - self.__loadedRuns:
            
            query = COOLQuery()
            query.SetIoV(*COOLTime().Run(runNumber))
            
            
            for obj, payload in query.GetFolder("/TRIGGER/LVL1/Menu"):
                varName = payload["ItemName"]
                
                allocate_dictionary(varName)                
                self.__channelDict[varName][runNumber] = obj.channelId()
                self.__uniqueDict[varName][runNumber] = obj.channelId()            
            
            for obj, payload in query.GetFolder("/TRIGGER/HLT/Menu"):
                varName = payload["ChainName"]
                
                allocate_dictionary(varName)
                self.__channelDict[varName][runNumber] = obj.channelId()
                self.__uniqueDict[varName][runNumber] = payload["ChainCounter"]
                
                for lowerChainName in payload["LowerChainName"].split(", "):
                    if len(lowerChainName):
                        self.__lowerChainDict[varName].add(lowerChainName)
                         
        """
        for varName in self.__channelDict.keys():
            print varName, ":"
            for runNumber in self.__channelDict[varName].keys():
                print "\t", runNumber 
        """
 
    """
    def LoadDictionaries(self, ForceReload = False):
        
        if ForceReload:
            self.__channelDict = {}
            self.__uniqueDict = {}
            self.__lowerChainDict = {}            

        def allocate_run_in_dictionaries(runNumber):
            if not self.__channelDict.has_key(runNumber):
                self.__channelDict[runNumber] = {}
            if not self.__uniqueDict.has_key(runNumber):
                self.__uniqueDict[runNumber] = {}
            if not self.__lowerChainDict.has_key(runNumber):
                self.__lowerChainDict[runNumber] = {}        
        
        query = COOLQuery()
        
        newFilter = self.GetTimeFilter()
        for runNumber in self.__channelDict.keys():
            newFilter = newFilter.Complement(DataSelection(COOLTime(runNumber, 0x0000),COOLTime(runNumber, 0xFFFE)))

        if newFilter.IsEmpty():
            return


        query.SetIoVSelection(newFilter)
     
        for obj, payload in query.GetFolder("/TRIGGER/LVL1/Menu"):
            for runNumber in range(COOLTime(obj.since()).GetRunNumber(), COOLTime(obj.until()).GetRunNumber()):
                allocate_run_in_dictionaries(runNumber)
                
                self.__channelDict[runNumber][payload["ItemName"]] = obj.channelId()
                self.__uniqueDict[runNumber][payload["ItemName"]] = obj.channelId()
                self.__lowerChainDict[runNumber][payload["ItemName"]] = []

        for obj, payload in query.GetFolder("/TRIGGER/HLT/Menu"):
            for runNumber in range(COOLTime(obj.since()).GetRunNumber(), COOLTime(obj.until()).GetRunNumber()):
                allocate_run_in_dictionaries(runNumber)
            
                self.__channelDict[runNumber][payload["ChainName"]] = obj.channelId()
                self.__uniqueDict[runNumber][payload["ChainName"]] = payload["ChainCounter"]
                self.__lowerChainDict[runNumber][payload["ChainName"]] = []
                
                for lowerChainName in payload["LowerChainName"].split(", "):
                    if len(lowerChainName):
                        self.__lowerChainDict[runNumber][payload["ChainName"]].append(lowerChainName)
    
    """

    def ToDataDictionary(self, *chainNames, **kwargs):
        extract = "RateAfterDecision"
        if kwargs.has_key("extract") and kwargs["extract"] != None: extract = kwargs["extract"]
        
        if len(chainNames) == 0: 
            if not len(self.GetNames()) == 0:
                return self.ToDataDictionary(*self.GetNames(), **kwargs)
            return {}
        
        data = {"LumiBlock": DataColumn()}
        for name in chainNames:
            data[name] = DataColumn()

        for i in sorted(self.GetTimeFilter()):
            data["LumiBlock"].append(i.GetLumiBlock())
            for chainName in chainNames:
                val = None
                
                fName = "Get"+extract
                f = getattr(self.Get(chainName).GetRateInfo(i), fName)
                val = f()
                data[chainName].append(val)
        
        
        
        return data        


    def Load(self, *values):

        self.LoadDictionaries()
    
        def build_dep_tree(name):
            deptree = [name]

            if self.__lowerChainDict.has_key(name):
                for lowerChainName in self.__lowerChainDict[name]:
                    deptree = build_dep_tree(lowerChainName) + deptree
            
            return deptree
        
        deptree = []
        theList = []
        
        if len(values) == 0:
            pass
        elif "*" in values:
            theList = self.GetNames()
        else:
            for value in values:
                theList.append(value)
                
        fulltree = []        
        for chainName in theList:
            fulltree += build_dep_tree(chainName)
        
        for chain in fulltree:
            if not chain in deptree:
                deptree.append(chain)  
        
        for chainName in deptree:
            if not self.__chains.has_key(chainName):
                chain = TriggerChain(chainName, self.GetTimeFilter(), self)
                
                
                chain.SetChannelIdDict(self.__channelDict[chainName])
                chain.SetUniqueIdDict(self.__uniqueDict[chainName])
                
                
                self.__chains[chainName] = chain

            
            for lowerChainName in self.__lowerChainDict[chainName]:
                self.__chains[chainName].SetLowerChain(self.__chains[lowerChainName])
            
            
            self.__chains[chainName].SetTimeFilter(self.GetTimeFilter())
            
            self.__chains[chainName].Load()

#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    from model.COOL.COOLTime import COOLTime
    #.Union(COOLTime(187219, 1), COOLTime(187219, 20)))
    a = ChainCollection(DataSelection(*COOLTime().Run(189184)))
    a.LoadDictionaries()
    a.Load("L2_e20_medium")
    
    print a.GetNames()
    """
    for pit in a.GetTimeFilter():
        print a.Get("L2_e20_medium").GetRateInfo(pit).GetCrossSection()
    """

#----------------------------------------------------------------------------------------------------
