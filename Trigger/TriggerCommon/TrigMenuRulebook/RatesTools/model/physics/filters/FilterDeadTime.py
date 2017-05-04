# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection 
from model.COOL.COOLTime import COOLTime

from model.physics.TriggerChain import TriggerChain

from model.basics.SelectionFilter import SelectionFilter

class FilterDeadTime(SelectionFilter):
    def __init__(self, threshold, type = "Intersection"):
        SelectionFilter.__init__(self, type)
        self.SetThreshold(threshold)
        self.__chain = TriggerChain("L1_RD0_FILLED")
    
    def GetThreshold(self): return self.__threshold
    def SetThreshold(self, val):
        self.__threshold = val
    
    def GetFilteredValues(self, input = None):
        self.__chain.SetTimeFilter(input)
        self.__chain.Load()
        
        outValues = set([])
        for i in input:
            deadTime = None
            try:
                deadTime = 1 - (self.__chain.GetRateInfo(i).GetRateAfterDecision() / self.__chain.GetRateInfo(i).GetRateAfterPrescale())
            except Exception as e:
                deadTime = 1.0
            
            if deadTime < self.GetThreshold():
                outValues.add(i)
                
        return DataSelection().FromList(outValues)

    


if __name__ == "__main__":
    selection = DataSelection(COOLTime(187219,20),COOLTime(187219,300))
    deadTimeFiltered = FilterDeadTime(0.01).FilterValues(selection)