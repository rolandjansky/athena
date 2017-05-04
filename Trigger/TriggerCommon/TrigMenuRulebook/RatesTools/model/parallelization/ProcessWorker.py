# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''
import sys
import time
import multiprocessing
import random
import string

from model.parallelization.ProcessJob import ProcessJob
from model.basics.Outputter import Outputter

class ProcessWorker(multiprocessing.Process, Outputter):
    def __init__(self):
        super(ProcessWorker,self).__init__()
        self.SetName("".join([random.choice(string.letters) for i in xrange(15)]))
        self.SetType("")
        self.SetControlPipe(None)
        self.SetJobQueue(None)
        self.SetResultQueue(None)
        self.SetErrorQueue(None)


    __type = str()
    def GetType(self):
        return self.__type
    def SetType(self, val):
        self.__type = val


    __name = str()
    def GetName(self):
        return self.__name
    def SetName(self, val):
        self.__name = val        


    __resultQueue = multiprocessing.Queue()    
    def GetResultQueue(self):
        return self.__resultQueue
    def SetResultQueue(self, val):
        self.__resultQueue = val

    __errorQueue = multiprocessing.Queue()    
    def GetErrorQueue(self):
        return self.__errorQueue
    def SetErrorQueue(self, val):
        self.__errorQueue = val

    __jobQueue = multiprocessing.Queue()
    def GetJobQueue(self):
        return self.__jobQueue
    def SetJobQueue(self, val):
        self.__jobQueue = val
                          

        
    __controlPipe = multiprocessing.connection
    def GetControlPipe(self):
        return self.__controlPipe
    def SetControlPipe(self, val):
        self.__controlPipe = val


    def run(self):
        while True:
            if self.__controlPipe.poll():
                message = self.__controlPipe.recv()
                if message == "TERM":
                    break
            
            self.__controlPipe.send("IDLE")
            job = ProcessJob() 
            try:
                job = self.__jobQueue.get(False)
                                
                self.__controlPipe.send("BUSY")
                
                result = self.ExecuteJob(job)                
                job.SetResult(result)
                self.__resultQueue.put(job)

                sys.stdout.flush()
                
                self.__controlPipe.send("IDLE")
                                
            except multiprocessing.queues.Empty:
                #print "Nothing to do"
                pass

            except Exception as e:
                job.SetResult(e)
                self.__errorQueue.put(job)
                self.__controlPipe.send("ERROR")
            
            time.sleep(1)
                
                
        #print "I died"
