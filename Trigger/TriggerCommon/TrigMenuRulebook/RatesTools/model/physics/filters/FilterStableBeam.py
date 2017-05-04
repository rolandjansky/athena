# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection 

from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime

from model.basics.SelectionFilter import SelectionFilter

class FilterStableBeam(SelectionFilter):
    def __init__(self, type = "Intersection"):
        SelectionFilter.__init__(self, type)
        
        print "Filter is not completed yet. This is work in progress."
    
    def GetFilteredValues(self, input = None):
        self.__lastInput = input
        
        
        inputSuperset = input.GetSuperSetAsRange() 
        
        iovRange = DataSelection(inputSuperset.GetFirst()).Union(DataSelection(inputSuperset.GetLast()))
        
        query = COOLQuery()
        query.SetIoVSelection(iovRange)
        
        begin = None
        end = None
        for obj, payload, in query.GetFolder("/TRIGGER/LUMI/LBLB"):
            if begin == None: begin = payload["StartTime"]
            if end == None: end = payload["EndTime"]
        
        
        
        query.SetIoV(begin, end)
        
        
        timeValues = DataSelection()
        for obj, payload in query.GetFolder("/LHC/DCS/FILLSTATE"):
            if payload["ReadyForPhysics"] == 1:                    
                timeValues = timeValues.Union(DataSelection(COOLTime(obj.since()), COOLTime(obj.until()-1)))

        
        ranges = []
        for timeRange in timeValues.GetListOfRanges():
            begin = None
            end = None
            for obj, payload, in query.GetFolder("/TRIGGER/LUMI/LBLB"):
                if begin == None: begin = payload["StartTime"]
                if end == None: end = payload["EndTime"]            
        
        
        
        self.__filteredValues = DataSelection()
        

        
        return self.__filteredValues
    
    


if __name__ == "__main__":
    a = FilterPhysicsReady("Definition")
    print a.FilterValues(DataSelection(COOLTime(187219,0x0000),COOLTime(187219,0xFFFF)))