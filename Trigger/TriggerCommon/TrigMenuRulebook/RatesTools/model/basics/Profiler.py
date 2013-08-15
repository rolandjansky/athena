# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 13, 2011

@author: ggalster
'''

import time
class Profiler(object):
    def __init__(self, name = None):
        if name != None:
            self.__file = file(name, "a")

        self.__bias     =   time.time()
        self.__start    =   0
        self.__stop     =   0
        self.__name     =   "Profiler"
        
        self.__level    =   0


    def __enter__(self):
        self.start()
        return self
        
    def __exit__(self, type, value, traceback):
        self.stop()
        
    
    def setName(self, val): self.__name = val
    def getName(self): return self.__name
    
    def setLevel(self, val): self.__level = val
    def getLevel(self): return self.__level
    
    def setFile(self, val): self.__file = val
    def getFile(self): return self.__file    

    def setBias(self, val): self.__bias = val
    def getBias(self): return self.__bias

    def new(self, name):
        instance = Profiler()
        instance.setFile(self.getFile())
        instance.setName(name)
        instance.setLevel(self.getLevel()+1)
        instance.setBias(self.getBias())
        return instance
    
    def start(self):
        self.__start = time.time()
        self.__stop = self.__start 
        self.__printStart()
    
    def stop(self):
        self.__stop = time.time()
        self.__printStop()
        
        
    def __print(self):
        now = time.time() - self.__bias
        dT = self.__stop - self.__start
        
        string = "[%10f] [%10f] %30s" % (now, dT, __name__)
        for i in range(self.getLevel()):
            string += "\t"
        
        return string
    
    def __output(self, string):
        self.__file.write(string)
        self.__file.flush()
    
    def __printStop(self):
        string = self.__print()
        string += "Timer: '" + self.getName() + "' done.\n"
        self.__output(string)
        
    def __printStart(self):
        string = self.__print()
        string += "Timer: '" + self.getName() + "' started.\n"        
        self.__output(string)
        
    
    
        
if __name__ == '__main__':
    profiler = Profiler('/pcatr-srv1/home/ggalster/test.log')
    
    with profiler.new("loop") as loopTimer:
        time1 = time.time()
        while time.time() - (time1 + 3) < 0:
            with loopTimer.new("Sleep"):
                time.sleep(1)