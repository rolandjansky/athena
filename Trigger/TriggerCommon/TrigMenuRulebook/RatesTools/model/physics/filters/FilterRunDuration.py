# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection 

from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime

from model.basics.SelectionFilter import SelectionFilter

class FilterRunDuration(SelectionFilter):
    def __init__(self, type = "Definition"):
        SelectionFilter.__init__(self, type)
    
    def GetFilteredValues(self, input = None):
        query = COOLQuery()
        
        query.SetIoVSelection(input)
        
        values = DataSelection()
        
        for obj, payload in query.GetFolder("/TDAQ/RunCtrl/EOR_Params"):
            values = values.Union(DataSelection(COOLTime(obj.since()), COOLTime(obj.until()-1))) 
        
        return input.Intersection(values)
    


if __name__ == "__main__":
    a = FilterRunDuration("Definition")
    
    for range in a.FilterValues(DataSelection(COOLTime(187219, 0x0000),COOLTime(187219, 10))).GetListOfRanges():
        print COOLTime(range.GetFirst()).GetLumiBlock(), COOLTime(range.GetLast()).GetLumiBlock()