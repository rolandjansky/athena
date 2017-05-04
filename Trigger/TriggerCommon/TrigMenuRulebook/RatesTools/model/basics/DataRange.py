# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
class DataRange(object):
    def __init__(self, a = None, b = None):
        self.__first = None
        self.__last = None

        self.SetValues(a, b)
    
    def __eq__(self, other):
        if self.IsEmpty() or other.IsEmpty():
            return self.IsEmpty() and other.IsEmpty()
        return self.GetFirst() == other.GetFirst() and self.GetLast() == other.GetLast()
        
    
    def __str__(self):
        if self.IsEmpty(): return ""
        if self.IsSingle():
            return "%i" % self.GetFirst()
        else:
            return "[%i:%i]" % (self.GetFirst(), self.GetLast())
    
    def SetValues(self, a = None, b = None):
        if b == None:
            b = a;
        
        self.__first = a
        self.__last = b
        
    def IsSingle(self):
        return self.GetFirst() == self.GetLast()
    
    def CountElements(self):
        if self.GetFirst() == None or self.GetLast() == None:
            return 0
        return int(self.GetLast()) - int(self.GetFirst())+1
            
    
    def IsEmpty(self):
        return self.CountElements() == 0
    
    def GetFirst(self): return self.__first
    def SetFirst(self, val):
        self.SetValues(val, self.GetLast())

    def GetLast(self): return self.__last
    def SetLast(self, val):
        self.SetValues(self.GetFirst(), val)
    
    
    def Contains(self, val):
        return self.GetFirst() <= val and val <= self.GetLast()
    
    def IsDisjoint(self, other):        
        return not (self.Contains(other.GetFirst()) or self.Contains(other.GetLast()) or other.Contains(self.GetFirst()) or other.Contains(self.GetLast()))
    
    def IsSupersetOf(self, other):
        if self.IsSingle():
            return self == other
        if other.IsSingle():
            return self.Contains(other.GetFirst())
        
        return self.Contains(other.GetFirst()) and self.Contains(other.GetLast())  

    def IsSubsetOf(self, other):
        return other.Contains(self.GetFirst()) and other.Contains(self.GetLast())
        
    def Intersection(self, other):
        if self == other: return self
        
        if self.IsDisjoint(other): return DataRange()
        
        if self.IsEmpty(): return self
        if other.IsEmpty(): return self
        
        if self.IsSingle(): return DataRange(self.GetFirst())
        if other.IsSingle(): return DataRange(other.GetFirst())
        
        if self.IsSupersetOf(other): return other
        if self.IsSubsetOf(other): return self
        
        if self.CenterOfGravity() < other.CenterOfGravity():
            return DataRange(other.GetFirst(), self.GetLast())
        else:
            return DataRange(self.GetFirst(), other.GetLast())
        
        
    def Complement(self, other):
        if self == other: return DataRange()    
        
        if self.IsEmpty(): return self
        
        if self.IsDisjoint(other):
            return self
        
        if self.GetFirst() < other.GetFirst() and other.GetLast() < self.GetLast():
            raise Exception("Need to split range into two ranges")
            
        if self.IsSubsetOf(other):
            return DataRange()
    
        if self.CenterOfGravity() < other.CenterOfGravity():
            return DataRange(self.GetFirst(), other.GetFirst()-1)
        else:
            return DataRange(self.GetFirst()+1, other.GetLast())
            
    def Union(self, other):
        if self == other: return self 
        
        if self.IsDisjoint(other):
            raise Exception("Can't make union range of disjoint ranges")
        
        return DataRange(min(self.GetFirst(),other.GetFirst()), max(self.GetLast(),other.GetLast()))
    
        
    def CenterOfGravity(self):
        if self.IsEmpty(): return None
        if self.IsSingle(): return float(self.GetFirst())
        return float(int(self.GetFirst()) + int(self.GetLast()))/2.0
        
    #def ToList(self):
    def ToSet(self):
        retval = set([])
        if not self.IsEmpty():
            i = self.GetFirst()
            while i <= self.GetLast():
                retval.add(i)
                i += 1
        return retval
        
    #def __iter__(self): return iter(self.ToList())
    def __iter__(self): return iter(self.ToSet())
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    a = DataRange(1,4)
    b = DataRange(5)
    
    print a.Complement(b)