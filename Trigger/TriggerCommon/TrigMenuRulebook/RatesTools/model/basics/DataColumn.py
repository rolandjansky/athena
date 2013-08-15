# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 4, 2011

@author: galster
'''

class DataColumn(list):
    def __init__(self):
        pass
    
    def Max(self, ignoreNone = True):
        max = None;
        for i in self:
            if i == None and ignoreNone: continue
            
            if max == None or i > max:
                max = i
        return max
    
    def Min(self, ignoreNone = True):
        min = None;
        for i in self:
            if i == None and ignoreNone: continue
            
            if min == None or i < min:
                min = i
        return min
    
    def Mean(self, ignoreNone = True):
        sum = 0.0
        N = float(len(self))
        
        for i in self:
            if i == None and ignoreNone: continue
            
            sum += float(i)
        try:
            return sum / N
        except:
            return None 
    

    def Error(self, ignoreNone = True):
        mean = self.Mean()
        N = len(self)
        
        if N <= 1: return None
        sum = 0
        for i in self:
            if i == None and ignoreNone: continue
            
            sum += (float(i) - mean)**2
        

        return ( sum / (N*(N - 1)) )**(0.5)

            
    
    
    def Median(self, ignoreNone = True):
        l = []
        
        for i in self:
            if i == None and ignoreNone: continue
            l.append(i)
        
        l = sorted(l)
        N = len(l)
        
        if N == 0: return None
        
        return l[int(round(float(N) / 2.0))-1]
    
    
if __name__ == "__main__":
    a = DataColumn()
    for i in range(4):
        a.append(0.0)
        
    for i in range(2):
        a.append(1.0)
        
    print a.Median()