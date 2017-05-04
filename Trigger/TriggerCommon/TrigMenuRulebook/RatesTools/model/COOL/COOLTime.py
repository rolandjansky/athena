# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 15, 2011

@author: ggalster
'''

class COOLTime(object):

    __time = int()
    def __init__(self, *args, **kwargs):
        self.__time = 0;
        
        if len(args) == 1:
            self.SetCOOLTime(args[0])
        if len(args) == 2:
            self.SetRunNumber(args[0])
            self.SetLumiBlock(args[1])
    
    def __str__(self):
        return "%i (Run: %i, Lumi: %i)" % (self.__time, self.GetRunNumber(), self.GetLumiBlock())
    
    def __add__(self, x):
        return COOLTime(self.GetCOOLTime() + x)
    
    def __sub__(self, x):
        return COOLTime(self.GetCOOLTime() - x)
    
    def __eq__(self, other):
        return int(self) == other
    def __ne__(self, other):
        return int(self) != other    
    def __lt__(self, other):
        return int(self) < other
    def __le__(self, other):
        return int(self) <= other    
    def __gt__(self, other):
        return int(self) > other
    def __ge__(self, other):
        return int(self) >= other
    
    def __hash__(self):
        return int(self).__hash__()
    
    def __repr__(self):
        return repr(self.GetCOOLTime())

    def __int__(self):
        return self.GetCOOLTime()    
    
    def SetRunNumber(self, val): 
        self.__time = (int(val) << 32) + self.GetLumiBlock()
    
    def GetRunNumber(self):
        return (self.__time - self.GetLumiBlock()) >> 32
    
    def SetLumiBlock(self, val):
        self.__time = (self.GetRunNumber() << 32) + int(val) 
    
    def GetLumiBlock(self):
        return self.__time & 0xFFFF
    
    def SetCOOLTime(self, val):
        self.__time = int(val)
    
    def GetCOOLTime(self):
        return self.__time

    def Run(self, runNumber):
        return (COOLTime(runNumber, 0x0000), COOLTime(runNumber, 0xFFFF))

    def Min(self):
        return COOLTime(0x0000, 0x0000)
    
    def Max(self):
        return COOLTime(0x0000, 0xFFFF)

if __name__ == "__main__":
    
    a_min, a_max = COOLTime().Run(runNumber)