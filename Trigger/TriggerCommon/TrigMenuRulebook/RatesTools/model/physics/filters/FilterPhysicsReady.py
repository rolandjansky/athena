# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection 

from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime

from model.basics.SelectionFilter import SelectionFilter

class FilterPhysicsReady(SelectionFilter):
    def __init__(self, type = "Intersection"):
        SelectionFilter.__init__(self, type)
    
    def GetFilteredValues(self, input = None):
        self.__lastInput = input
        
        query = COOLQuery()
        
        query.SetIoVSelection(input)
        
        self.__filteredValues = DataSelection()

        for obj, payload in query.GetFolder("/TDAQ/RunCtrl/DataTakingMode"):
            if payload["ReadyForPhysics"] == 1:                    
                self.__filteredValues = self.__filteredValues.Union(DataSelection(COOLTime(obj.since()), COOLTime(obj.until()-1)))
        
        return self.__filteredValues
    
    


if __name__ == "__main__":
    a = FilterPhysicsReady("Definition")
    print a.FilterValues(DataSelection(COOLTime(187219,0x0000),COOLTime(187219,0xFFFF)))