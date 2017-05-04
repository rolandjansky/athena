# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''


class ProcessJob(object):
    def __init__(self):
        self.SetType(self.__class__)
        self.SetResult(None)
    
    __type = None
    def SetType(self, val):
        self.__type = val
    def GetType(self):
        return self.__type

    
    __result = None
    def SetResult(self, val):
        self.__result = val
    def GetResult(self):
        return self.__result