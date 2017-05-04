# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from model.basics.DataRange import DataRange
#----------------------------------------------------------------------------------------------------
class DataSelection(object):
    def __init__(self, a = None, b = None):
        self.__ranges = []
        
        r = DataRange(a,b)
                
        if not r.IsEmpty():
            self.__ranges.append(r)        
    
    def __str__(self):
        st = ""
        for ran in self.GetListOfRanges():
            st += "%s, " % (ran.__str__())
        return st[0:-2]
    
    def GetListOfRanges(self): return self.__ranges
    def SetListOfRanges(self, val): self.__ranges = val
    
    
        
    """
    def ToList(self):        
        retval = []
        for ran in self.GetListOfRanges():
            for value in ran:
                retval.append(value)
        return retval
    """
    def ToSet(self):        
        retval = set([])
        for ran in self.GetListOfRanges():
            retval = retval.union(ran)
        return retval
    
    def CountElements(self):
        count = 0
        for ran in self.GetListOfRanges():
            count += ran.CountElements()
        return count
    
    def IsEmpty(self):
        return self.CountElements() == 0
            
    
    def FromList(self, values):
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(values))
        return sel
    
    #def __iter__(self): return iter(self.ToList())
    def __iter__(self): return iter(self.ToSet())

    def __make_range_list(self, values):
        rangeList = []
        
        if not len(values): return rangeList
        
        values = sorted(set(values))
         
        first = values[0]
        prev = first-1
        for value in values + [None]:
            if not value == prev+1:
                rangeList.append(DataRange(first, prev))
                first = value
            
            prev = value
            
        return rangeList

    
    def Union(self, other):
        """
        values = []
        for value in self + other:
            values.append(value)
            
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(values))
        """
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(self.ToSet().union(other.ToSet())))
        return sel
    
    def Complement(self, other):
        """
        values = []
        otherList = other
        for value in self:
            if not value in otherList:
                values.append(value)
      
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(values))
        
        return sel
        ""
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(self.ToSet()-other.ToSet()))
        return sel 
        """
        #raise Exception("ERROR in Complement calculation")
        
        def calculate_intersection(someRange):
            #print "someRange\t",someRange
            for otherRange in other.GetListOfRanges():
                #print "otherRange\t",otherRange
                if someRange.IsSupersetOf(otherRange):
                    #print "TRUE"
                    
                    first = DataRange(someRange.GetFirst(), otherRange.GetFirst()-1)
                    last = DataRange(otherRange.GetLast()+1, someRange.GetLast())
                    
                    val = calculate_intersection(first).union(calculate_intersection(last))
                    #print "val\t",val
                    return val
                
                else:
                    someRange = someRange.Complement(otherRange)
            
            #print "(val)\t",someRange.ToSet()
            return someRange.ToSet()
        
        
        values = set([])
        for selfRange in self.GetListOfRanges():
            values = values.union(calculate_intersection(selfRange))
        
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(self.ToSet()-other.ToSet()))
        return sel 
                

    def Intersection(self, other):
        """
        values = []
        otherList = other
        for value in self:
            if value in otherList:
                values.append(value)
      
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(values))
        ""
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(self.ToSet() & other.ToSet()))
        return sel
        """
        values = set([])
        for selfRange in self.GetListOfRanges():
            for otherRange in other.GetListOfRanges():            
                if not selfRange.IsDisjoint(otherRange):
                    values = values.union(selfRange.Intersection(otherRange).ToSet())
        
        sel = DataSelection()
        sel.SetListOfRanges(self.__make_range_list(values))
        return sel

        
    
    def GetSuperSet(self):
        ranges = self.GetListOfRanges()
        begin = None
        end = None
        if not len(ranges) == 0:
            begin = ranges[0].GetFirst()
            end = ranges[0].GetLast()
            for i in range(1,len(ranges)):
                begin = min(begin,ranges[i].GetFirst())
                end = max(end,ranges[i].GetLast())
        
        return DataSelection(begin,end)
        
    
    def GetSuperSetAsRange(self):
        try:
            return self.GetSuperSet().GetListOfRanges()[0]
        except:
            return DataRange()
        
    
    def IsSuperSet(self):
        ranges = self.GetListOfRanges()
        if len(ranges) != 1: return False
        selfRange = ranges[0]
        superSet = self.GetSuperSetAsRange()
        return selfRange == superSet
    
        
            
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    #a = DataSelection(1,6).Complement(DataSelection(5))
    #b = DataSelection(3,7)
    
    a = DataSelection(0,40)
    b = DataSelection(11,29).Complement(DataSelection(15)).Complement(DataSelection(17))
    
    print a.Intersection(b)
    
    """
    a = DataSelection(1,3).Union(DataSelection(7,10)).Union(DataSelection(5))
    b = DataSelection(3,7).Complement(DataSelection(5))
    
    
    print "a", a
    print "b", b
    
    print "a in b", a.Intersection(b)
    print "a / b", a.Complement(b)
    
    """
