# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection 

from model.basics.SelectionFilter import SelectionFilter

class FilterMask(SelectionFilter):
    def __init__(self, mask = DataSelection(), type = "Intersection"):
        SelectionFilter.__init__(self, type)
        self.SetMask(mask)
    
    def GetMask(self): return self.__mask
    def SetMask(self, val): self.__mask = val
    
    def GetFilteredValues(self, input = None):
        return self.GetMask()
    


if __name__ == "__main__":
    pass