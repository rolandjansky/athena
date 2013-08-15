# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from config.dbInfo import *
#----------------------------------------------------------------------------------------------------
import sys
#----------------------------------------------------------------------------------------------------
from model.basics.DataSelection import DataSelection
from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime
from model.physics.TriggerRate import TriggerRate
#----------------------------------------------------------------------------------------------------
class TriggerChain(object):
    def __init__(self, name, timeFilter = DataSelection(), parentCollection = None):
        self.SetChainCollection(parentCollection)
        
        self.__filterTime = DataSelection()
        self.__lowerChains = {}
        self.__rates = {}
        
        self.SetName(name)
        self.SetTimeFilter(timeFilter)
        self.SetChannelIdDict({})
        self.SetUniqueIdDict({})
        
        self.SetIsDummy(True)
    
    def GetChainCollection(self): return self.__chainCollection
    def SetChainCollection(self, val): self.__chainCollection = val
    
    def GetIsDummy(self): return self.__isDummy
    def SetIsDummy(self, val): self.__isDummy = val
    
    def GetName(self): return self.__name
    def SetName(self, val): self.__name = val
    
    def GetLevel(self):
        return self.GetName()[0:2] 
    
    def __get_overlap_with_run(self, runNumber):
        return self.GetTimeFilter().Intersection(DataSelection(COOLTime(runNumber, 0x0000),COOLTime(runNumber, 0xFFFF)))
    
    def LoadDictionaries(self, *runNumbers):
        if len(runNumbers) == 0:          
            missingRuns = self.__runNumbers.difference(self.__id.keys())
            if len(missingRuns) == 0: return  
            
            return self.LoadDictionaries(*missingRuns)
        
        
        for runNumber in runNumbers:
            IoV = self.__get_overlap_with_run(runNumber).GetListOfRanges()[0]
            
            query = COOLQuery()
            query.SetIoV(COOLTime(IoV.GetFirst()), COOLTime(IoV.GetFirst()+1))

            channelIdFolder = "/TRIGGER/HLT/Menu"
            channelIdName = "ChainName"
            if self.GetLevel() == "L1":
                channelIdFolder = "/TRIGGER/LVL1/Menu"
                channelIdName = "ItemName"


            uId = None
            cId = None

            for obj, payload in query.GetFolder(channelIdFolder):
                if payload[channelIdName] == self.GetName():
                    
                    cId = obj.channelId()
                    
                    uId = obj.channelId()
                    if self.GetLevel() != "L1":
                        uId = payload["ChainCounter"]
                    
                    break
            
            self.__uniqueId[runNumber] = uId
            self.__id[runNumber] = cId 
            
    
    def GetChannelId(self, runNumber): return self.__id[runNumber]
    def GetChannelIdDict(self): return self.__id
    def SetChannelIdDict(self, val): self.__id = val
    
    
    def GetUniqueId(self, runNumber): return self.__uniqueId[runNumber]
    def GetUniqueIdDict(self): return self.__uniqueId
    def SetUniqueIdDict(self, val): self.__uniqueId = val
     
    
    def GetTimeFilter(self): return self.__filterTime
    def SetTimeFilter(self, val):
        self.__runNumbers = set([])
        for timeRange in val.GetListOfRanges():
            for runNumber in range(COOLTime(timeRange.GetFirst()).GetRunNumber(), \
                                   COOLTime(timeRange.GetLast()).GetRunNumber()+1):
                self.__runNumbers.add(runNumber)
        
        self.__filterTime = val
        for chainName in self.__lowerChains.keys():
            self.__lowerChains[chainName].SetTimeFilter(val)
            
    
    def GetLowerChains(self): return self.__lowerChains.items()
    def SetLowerChains(self, val):
        self.__lowerChains.clear()
        for chain in val:
            self.__lowerChains[chain.GetName()] = chain
    
    def GetLowerChainNames(self): return self.__lowerChains.keys()
    
    def GetLowerChain(self, name): return self.__lowerChains[name]
    def SetLowerChain(self, val): self.__lowerChains[val.GetName()] = val
    
    def GetRateInfo(self,pointInTime): return self.__rates[pointInTime]
    
    def Load(self):
        self.LoadDictionaries()
        
        for chainName in self.__lowerChains.keys():
            self.__lowerChains[chainName].Load()
        
        ratesOldTimes = DataSelection().FromList(self.__rates.keys())
        ratesNewTimes = self.GetTimeFilter()
        
        ratesCreateTimes = ratesNewTimes.Complement(ratesOldTimes)
                
        if not ratesCreateTimes.IsEmpty():
            
            #print "Loading new chains...."            
            #import time
            #t = time.time()
              
            for runNumber in self.__runNumbers:
                IoV = self.__get_overlap_with_run(runNumber)



                timeSet = IoV.ToSet()
                
                # Make default rates
                for i in timeSet:
                    self.__rates[i] = TriggerRate(COOLTime(i), self)
                

                # Make dummy?
                if not self.GetChannelId(runNumber):
                    print >> sys.stderr, "Warning: Cannot load chain %s for run %i: No menu information found. Loading dummy." % (self.GetName(), runNumber)
                    continue


                # Fill raw rates
                query = COOLQuery()
                query.SetIoVSelection(IoV)

                
                if self.GetLevel() == "L1":
    
                    rates = {}                    
                    """
                        IMPORT RATES FROM COOL
                    """
                    turnCounters = {}
                    counters = {}
                    query.SelectSingleChannel(None)
                    for i in IoV:
                        counters[i] = {}
                        rates[i] = {}
                        turnCounters[i] = {}
                        
                    counterTypes = ["L1Accept", "BeforePrescale", "AfterPrescale"]
                    actualPITs = set([])
                        
                    for obj, payload in query.GetFolder("/TRIGGER/LVL1/CTPCORELBDATA"):
                        actualPITs.add(obj.since())
                        for type in counterTypes:
                            turnCounters[obj.since()][type] = float(payload[type + "TurnCounter"])
                            
                    #print "asdasdasd",self.GetChannelId(runNumber)
                    query.SelectSingleChannel(self.GetChannelId(runNumber))
                    
                    for obj, payload in query.GetFolder("/TRIGGER/LUMI/LVL1COUNTERS"):
                        actualPITs.add(obj.since())
                        for type in counterTypes:                    
                            counters[obj.since()][type] = float(payload[type])                
                    
                    for i in actualPITs:
                        for type in counterTypes:
                            rates[i][type] = counters[i][type] / turnCounters[i][type] * 11246.0
    
                        self.__rates[i].SetRateAfterDecision(rates[i]["L1Accept"])
                        self.__rates[i].SetRateBeforePrescale(rates[i]["BeforePrescale"])
    
    
    
    
    
                    """
                        IMPORT PRESCALES FROM COOL
                    """
                    for obj, payload in query.GetFolder("/TRIGGER/LVL1/Prescales"):
                        validityRange = DataSelection(obj.since(), obj.until()-1).Intersection(IoV)
                        for i in validityRange:
                            self.__rates[i].SetPrescale(payload["Lvl1Prescale"]) 
    
    
    
                
                else: # Lvl 2 + EF
                    query.SelectSingleChannel(self.GetUniqueId(runNumber))
                    """
                        IMPORT RATES FROM COOL
                    """
                    dbFolderName = None
                    if self.GetLevel() == "L2":
                        dbFolderName = dbL2FolderName
                    else:
                        dbFolderName = dbEFFolderName
                        
                        
                    for obj, payload in query.GetFolder(dbFolderName, dbString):
                        self.__rates[obj.since()].SetRateBeforePrescale(payload["Input"])
                        self.__rates[obj.since()].SetRateAfterDecision(payload["Raw"])
                                  
                                            
    
                    
                    """
                        IMPORT PRESCALES PASSTHROUGH FROM COOL
                    """
                    query.SelectSingleChannel(2 * self.GetUniqueId() + int(self.GetLevel() != "L1")) 
                    for obj, payload in query.GetFolder("/TRIGGER/HLT/Prescales"):
                        validityRange = DataSelection(obj.since(), obj.until()-1).Intersection(ratesCreateTimes)
                        for i in validityRange:
                            self.__rates[i].SetPrescale(payload["Prescale"])
                            self.__rates[i].SetPassThrough(payload["Passthrough"])
    
    
    
            if self.GetIsDummy():
                for i in ratesCreateTimes:
                    if not self.__rates[i].GetRateBeforePrescale() == None:
                        self.SetIsDummy(False)
                        break
            """
            for i in ratesCreateTimes:
                print self.GetName(), "COOLTime:", COOLTime(i)
                print "Rate before PS\t", self.__rates[i].GetRateBeforePrescale()
                print "Raw output rate\t", self.__rates[i].GetRawOutputRate()
                print "Corrected rate\t", self.__rates[i].GetCorrectedOutput()
                print "Prescale\t", self.__rates[i].GetPrescale()
                print "Total Prescale\t", self.__rates[i].GetTotalPrescale()
                print "Pass Through\t", self.__rates[i].GetPassThrough()
                print "Cross section\t", self.__rates[i].GetCrossSection()
                print
            """
    
        deleteRates = ratesOldTimes.Complement(ratesNewTimes)
        for i in deleteRates:
            self.__rates.pop(i)
        
        #self.SetTimeFilter(self.GetvalTimeFilter())
        pass
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    selection = DataSelection(817259261984769, 817310801592355)

    from model.physics.filters.FilterRunDuration import FilterRunDuration
    selection = FilterRunDuration().FilterValues(selection)

    for tRange in selection.GetListOfRanges():
        print COOLTime(tRange.GetFirst()), COOLTime(tRange.GetLast())

    a = TriggerChain("L1_RD0_FILLED", selection)
    a.LoadDictionaries()

    print a.GetChannelIdDict().keys()

    a.Load()

    print a.GetIsDummy()
    
    
    
