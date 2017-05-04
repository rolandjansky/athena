# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.physics.RatesRun import RatesRun
from model.physics.filters.FilterDeadTime import FilterDeadTime
from model.physics.filters.FilterRunDuration import FilterRunDuration
from model.physics.filters.FilterPhysicsReady import FilterPhysicsReady

from model.basics.DataSelection import DataSelection
from model.COOL.COOLTime import COOLTime
#----------------------------------------------------------------------------------------------------
class RatesRunCollection(object):
    def __init__(self):
        self.SetListOfRuns({})
        
        self.SetListOfFilters([])
        
        self.SetRunDurationFilter(FilterRunDuration("Definition"))
        self.GetRunDurationFilter().Enable()
        
        self.SetDeadTimeFilter(FilterDeadTime(0.05))
        self.GetDeadTimeFilter().Enable()
        
        self.SetPhysicsReadyFilter(FilterPhysicsReady("Intersection"))
        self.GetPhysicsReadyFilter().Enable()
    
        self.SetPhysicsReadyFilter(FilterPhysicsReady("Intersection"))
        self.GetPhysicsReadyFilter().Enable()
                
   
    
    def AddRun(self, run): self.__runs[run.GetRunNumber()] = run
    def RemoveRun(self, run): self.__runs.pop(run.GetRunNumber())
    def GetRun(self, runNumber): return self.__runs[runNumber]
    
    def GetRunNumbers(self): return self.__runs.keys()
    
    def GetListOfRuns(self): return self.__runs
    def SetListOfRuns(self, val): self.__runs = val
    
    def GetTimeFilter(self):
        timeFilter = DataSelection()
        for runNumber in self.GetRunNumbers():
            timeFilter = timeFilter.Union(self.GetRun(runNumber).GetTimeFilter())
        return timeFilter
    



    def GetDeadTimeFilter(self): return self.__deadTimeFilter
    def SetDeadTimeFilter(self, val): self.__deadTimeFilter = val
    
    def GetRunDurationFilter(self): return self.__runDurationFilter
    def SetRunDurationFilter(self, val): self.__runDurationFilter = val
    
    def GetPhysicsReadyFilter(self): return self.__physicsReadyFilter
    def SetPhysicsReadyFilter(self, val): self.__physicsReadyFilter = val
            
    def GetListOfFilters(self): return self.__listOfFilters
    def SetListOfFilters(self, val): self.__listOfFilters = val

    def AddSelectionFilter(self, val): self.__listOfFilters.append(val)
    def RemoveSelectionFilter(self, val):
        if val in self.GetListOfFilters():
            self.__listOfFilters.remove(val)


    def GetPointInTime(self, val):
        return self.GetRun(COOLTime(val).GetRunNumber())
    
    
    
    
    def Load(self, *runNumbers):
        if len(runNumbers) == 0:
            for runNumber in self.__runs.keys():
                self.__runs[runNumber].Load()        
        else:
            for runNumber in runNumbers:
                run = RatesRun(runNumber)
                
                run.SetDeadTimeFilter(self.GetDeadTimeFilter())
                run.SetRunDurationFilter(self.GetRunDurationFilter())
                run.SetPhysicsReadyFilter(self.GetPhysicsReadyFilter())
                run.SetListOfFilters(self.GetListOfFilters())
                
                                
                self.AddRun(run)
                
                self.Load()
#----------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    runNumbers = [187219, 187196]
    
    a = RatesRunCollection()
    
    
    #a.Load(*runNumbers)    
    #a.GetDeadTimeFilter().SetThreshold(0.10)

    
    pass