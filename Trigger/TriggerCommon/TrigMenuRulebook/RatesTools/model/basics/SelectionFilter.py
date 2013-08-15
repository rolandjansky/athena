# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.DataSelection import DataSelection

class SelectionFilter(object):
    def __init__(self, type):
        self.Enable()
        self.SetType(type)    
        
    def Disable(self): self.UseFilter(False)
    def Enable(self): self.UseFilter(True)
    def UseFilter(self, val): self.__active = val
    def IsActive(self): return self.__active

    def UseAsDefinitionFilter(self): self.SetType("Definition")
    def UseAsComplementFilter(self): self.SetType("Complement")
    def UseAsIntersectionFilter(self): self.SetType("Intersection")    
    def UseAsUnionFilter(self): self.SetType("Union")

    def IsDefinitionFilter(self): return self.GetType() == "Definition"
    def IsComplementFilter(self): return self.GetType() == "Complement"
    def IsIntersectionFilter(self): return self.GetType() == "Intersection"
    def IsUnionFilter(self): return self.GetType() == "Union"

    def GetSupportedTypes(self): return ["Complement", "Intersection", "Union", "Definition"]    
    
    def SetType(self, type):
        if not type in self.GetSupportedTypes(): raise Exception("Unknown filter type")
        self.__filterType = type
        
    
    def GetType(self): return self.__filterType
    
    
    def FilterValues(self, input = DataSelection()):
        if self.IsActive():
            filtered = self.GetFilteredValues(input)
            
            if self.IsDefinitionFilter(): return filtered
            
            if self.IsComplementFilter(): return input.Complement(filtered)
            
            if self.IsIntersectionFilter(): return input.Intersection(filtered)
            
            if self.IsUnionFilter(): return input.Union(filtered)
            
            raise Exception("Return method not supported")
        else:
            return input