# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 15, 2011

@author: ggalster
'''

from model.parser.COOLquery import COOLquery
from model.parser.COOLtime import COOLtime

from model.parser.TRP.COOLDeadTimeJob import COOLDeadTimeJob 
from model.parser.TRP.COOLDeadTimeWorker import COOLDeadTimeWorker

from model.parser.TRP.TRPLuminosityJob import TRPLuminosityJob 
from model.parser.TRP.TRPLuminosityWorker import TRPLuminosityWorker

from model.parser.TRP.TRPSignatureJob import TRPSignatureJob 
from model.parser.TRP.TRPSignatureWorker import TRPSignatureWorker


from model.physics.Run import Run
from model.physics.LumiBlockRange import LumiBlockRange

from model.parallelization.ProcessMaster import ProcessMaster

class TRPExtractor:
    __processMaster = None
    
    def __init__(self):
        self.__processMaster = ProcessMaster()
        
        self.__runNumber        = 0
        self.__stepSize         = 10
        self.__rangeDivisor     = 1
        self.__fileName         = ""
        self.__numberOfWorkers  = 6
        self.__physicsReady     = True
        self.__deadtimeThreshold = 1.0
    
    def __del__(self):
        self.__processMaster.KillAll()
    
    def GetRunNumber(self): return self.__runNumber
    def SetRunNumber(self, val): self.__runNumber = val 
    
    def GetStepSize(self): return self.__stepSize
    def SetStepSize(self, val): self.__stepSize = val

    def GetRangeDivisor(self): return self.__rangeDivisor
    def SetRangeDivisor(self, val): self.__rangeDivisor = val
    
    def GetFileName(self): return self.__fileName
    def SetFileName(self, val): self.__fileName = val

    def GetNumberOfWorkers(self): return self.__numberOfWorkers
    def SetNumberOfWorkers(self, val): self.__numberOfWorkers = val
    
    def GetOnlyReadyForPhysics(self): return self.__physicsReady
    def SetOnlyReadyForPhysics(self, val): self.__physicsReady = val    
    
    def GetDeadtimeThreshold(self): return self.__deadtimeThreshold
    def SetDeadtimeThreshold(self, val): self.__deadtimeThreshold = val
    
    def Execute(self):
        for i in range(20):
            self.__processMaster.AddWorker(COOLDeadTimeWorker())
        
        self.__processMaster.AddWorker(TRPLuminosityWorker())
        for i in range(self.GetNumberOfWorkers()):
            self.__processMaster.AddWorker(TRPSignatureWorker())
        
        
        query = COOLquery()
        query.SetIOVStart(COOLtime(self.GetRunNumber(), 0))
        query.SetIOVStop(COOLtime(self.GetRunNumber()+1, 0))
        
        
        
        
        keyChangesAll = [] 
        for obj in query.GetFolder("/TRIGGER/LVL1/Lvl1ConfigKey"):
            keyChangesAll.append(long(COOLtime(obj.until()).GetLumiBlock()))
        for obj in query.GetFolder("/TRIGGER/HLT/HltConfigKeys"):
            keyChangesAll.append(long(COOLtime(obj.until()).GetLumiBlock()))
        
        keyChangesAll = sorted(set(keyChangesAll))
        
        
        for dataTakeObject in query.GetFolder("/TDAQ/RunCtrl/DataTakingMode"): #loop over data taking modes            
            dataTakeFirstLumiBlock = COOLtime(dataTakeObject.since()).GetLumiBlock()+1
            dataTakeLastLumiBlock = COOLtime(dataTakeObject.until()).GetLumiBlock()
            
            
            if dataTakeFirstLumiBlock > dataTakeLastLumiBlock:
                continue 
            
            payload = dataTakeObject.payload()
            if payload["ReadyForPhysics"] == 0 and self.GetOnlyReadyForPhysics():
                continue
                        
            
            keyChangesDataTake = [dataTakeFirstLumiBlock]
            for i in keyChangesAll:
                if dataTakeFirstLumiBlock < i and i < dataTakeLastLumiBlock:
                    keyChangesDataTake.append(i)
            keyChangesDataTake.append(dataTakeLastLumiBlock)
            

            
            for i in range(1,len(keyChangesDataTake)):
                print (keyChangesDataTake[i-1], keyChangesDataTake[i])
                IOVstart = COOLtime(self.GetRunNumber(), keyChangesDataTake[i-1])
                IOVstop  = COOLtime(self.GetRunNumber(), keyChangesDataTake[i])
            
            
                #query.SetIOVStart(IOVstart)
                #query.SetIOVStop(IOVstop)

                deadTimeJob = COOLDeadTimeJob()
                deadTimeJob.SetFileName(self.GetFileName())
                deadTimeJob.SetDeadTimeThreshold(self.GetDeadtimeThreshold())
                deadTimeJob.SetLumiBlockRange((IOVstart.GetLumiBlock(), IOVstop.GetLumiBlock()))
                deadTimeJob.SetStepSize(self.GetStepSize())
                
                self.__processMaster.SubmitJob(deadTimeJob)
        
        
        self.__processMaster.Block()
        self.__processMaster.KillAll(COOLDeadTimeJob().GetType())
        
        deadTimesJobs = self.__processMaster.GetResult(COOLDeadTimeJob().GetType())
        
        for job in deadTimesJobs:
            
            blacklist = job.GetResult()
            
            joblist = [ TRPLuminosityJob() ]
            for level in ["L1", "L2", "EF"]:
                signatureJob = TRPSignatureJob()
                signatureJob.SetLevel(level)
                joblist.append(signatureJob)
            
            for blockJob in joblist:
                blockJob.SetFileName(self.GetFileName())
                blockJob.SetLumiBlockRange(job.GetLumiBlockRange())
                blockJob.SetStepSize(self.GetStepSize())
                blockJob.SetBlacklist(blacklist)
                self.__processMaster.SubmitJob(blockJob)
            
        self.__processMaster.Block()
        self.__processMaster.KillAll()
        
        
        luminosities = {}
        signatures = {}
        blacklists = {}
        for jobType in self.__processMaster.GetResult().values():
            for job in jobType:
                key = job.GetLumiBlockRange()
                result = job.GetResult()

                if not signatures.has_key(key):
                    signatures[key] = []                
                
                if job.GetType() == TRPSignatureJob().GetType():
                    signatures[key] += result

                if job.GetType() == TRPLuminosityJob().GetType():
                    luminosities[key] = result
                    blacklists[key] = job.GetBlacklist()
        
                break

        crtRun = Run(self.GetRunNumber())
        
        for block in signatures.keys():
            crtRange = LumiBlockRange(crtRun)
            
            
            crtRange.SetLumiBlockRange(block)
            
            crtRange.SetLuminosity(luminosities[block].Mean())
            crtRange.SetLuminosityError(luminosities[block].Error())
            
            for crtSignature in signatures[block]:
                crtSignature.SetLumiBlockRange(crtRange)
                crtRange.AddSignature(crtSignature)
            
            crtRun.AddLumiBlockRange(crtRange)
            
        return crtRun
        
        
        
if __name__ == "__main__":
    a = TRPExtractor()
    a.SetRunNumber(184169)
    a.SetOnlyReadyForPhysics(False)
    a.SetFileName("~/TriggerRates_ATLAS_184169.root")
    a.Execute()
    