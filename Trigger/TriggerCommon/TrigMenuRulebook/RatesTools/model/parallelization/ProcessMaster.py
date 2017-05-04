# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''

import time
import multiprocessing

class ProcessMaster:
    def __init__(self):
        self.__manager = multiprocessing.Manager() 
        self.__jobQueue = {}
        self.__resultQueue = {}
        self.__pipes = {}
        self.__workers = {}
        self.__workerState = {}
    
    def __del__(self):
        self.KillAll()
    
    
    __jobQueue = {}
    def GetJobQueue(self, val):
        self.PrepareQueues(val)
        return self.__jobQueue[val]
    
    __errorQueue = {}
    def GetErrorQueue(self, val):
        self.PrepareQueues(val)
        return self.__errorQueue[val]    
    
    __resultQueue = {}
    def GetResultQueue(self, val):
        self.PrepareQueues(val)
        return self.__resultQueue[val]
    
     
    def GetResult(self, type = None):
        return self.__getFromQueue(self.__resultQueue, type)

    def GetError(self, type = None):
        return self.__getFromQueue(self.__errorQueue, type)

    def __getFromQueue(self, queue, type = None):
        result = None
        if type == None:
            result = {}
            for t in queue.keys():
                typeResult = self.__getFromQueue(queue, t)
                if len(typeResult):
                    result[t] = typeResult
        else:
            if queue.has_key(type):
                result = []
                count = queue[type].qsize()
                for i in range(count):
                    result.append(queue[type].get())
        return result
        
    
    __workers = {}
    __workerStates = {}
    __pipes = {}

    def GetWorkers(self):
        return self.__workers
    
    def SetWorkers(self, val):
        return self.__workers
    
    def AddWorker(self, worker):
        name = worker.GetName()
        type = worker.GetType()
        if not name in self.__workers.keys():
            worker.SetJobQueue(self.GetJobQueue(type))
            worker.SetResultQueue(self.GetResultQueue(type))
            worker.SetErrorQueue(self.GetErrorQueue(type))
            
            masterPipe, workerPipe = multiprocessing.Pipe()
            
            
            worker.SetControlPipe(workerPipe)
            self.__pipes[name] = masterPipe
            
            self.__workers[name] = worker
            self.__workerState[name] = "INIT"
            self.__workers[name].start()
            
        
    
    def KillWorker(self, worker):
        name = worker.GetName()
        if name in self.__workers.keys():
            if self.__workers[name].is_alive():
                self.__pipes[name].send("TERM")
            self.__workers.pop(name)
            self.__workerState.pop(name)
            self.__pipes.pop(name)
            
    
    def KillAll(self, type = None):
        for worker in self.__workers.values():
            if worker.GetType() == type or type == None:
                self.KillWorker(worker)
                
    def PrepareQueues(self, type):
        if not self.__jobQueue.has_key(type): 
            self.__jobQueue[type] = self.__manager.Queue()
        if not self.__resultQueue.has_key(type): 
            self.__resultQueue[type] = self.__manager.Queue()
        if not self.__errorQueue.has_key(type): 
            self.__errorQueue[type] = self.__manager.Queue()            

            
        
        
    def SubmitJob(self, job):
        thisJob = job
        type = thisJob.GetType()
        self.PrepareQueues(type)
        self.__jobQueue[type].put(thisJob)
        
    
    def GetWorkerStates(self, type = None):
        subset = {}
        
        for worker in self.__workers.values():
            name = worker.GetName()
              
            alive = worker.is_alive()
            if alive:
                while self.__pipes[name].poll():
                    message = self.__pipes[name].recv()
                    self.__workerState[name] = message
            else:
                self.__workerState[name] = "DEAD"
                
            if worker.GetType() == type or type == None:
                subset[name] = self.__workerState[name] 

                        
        return subset

    
    
    def __getQueueCount(self, queue, type = None):
        jobs = 0
        if type == None:
            for t in queue.keys():
                jobs += self.__getQueueCount(queue, t)
                
        else:
            if queue.has_key(type):
                jobs = queue[type].qsize()
        
        return jobs
    
    def GetNJobs(self, type = None):
        return self.__getQueueCount(self.__jobQueue, type)
    
    def GetNErrors(self, type = None):
        return self.__getQueueCount(self.__errorQueue, type)    
    
    def GetNResults(self, type = None):
        return self.__getQueueCount(self.__resultQueue, type)
    
        
    def IsActive(self, type = None):
        if type == None:
            for t in self.__jobQueue.keys():
                if self.IsActive(t):
                    return True
            return False
        else:
            if ("BUSY" in self.GetWorkerStates(type).values() or "ERROR" in self.GetWorkerStates(type).values() or self.GetNJobs(type) != 0):
                return True
            return False
        
        
    def Block(self, type = None, output = True):
        lastRound = False
        while True:
            if output:
                print self.GetNJobs(type), self.GetNErrors(type), self.GetWorkerStates(type).values()             
            
            if self.IsActive(type):
                lastRound = False        
            else:
                if lastRound: break
                lastRound = True 
            
            time.sleep(1)
            
    