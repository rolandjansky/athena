# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''
from model.parser.TRP.TRPWorker import TRPWorker
from model.parser.TRP.COOLDeadTimeJob import COOLDeadTimeJob

from model.parallelization.ProcessWorker import ProcessWorker
 

from model.basics.TempFile import TempFile

from model.basics.DataColumn import DataColumn 

import ROOT

class COOLDeadTimeWorker(ProcessWorker, TRPWorker):
    def __init__(self):
        super(COOLDeadTimeWorker,self).__init__()
        self.SetType(COOLDeadTimeJob().GetType())
        
        self.__tempFile = TempFile() 
    
    def __del__(self):
        self.__tempFile.Delete()
    
    
    __tempFile = None
    def ExecuteJob(self, job):
        try:
            self.JobIsOkay(job)
        except Exception as msg:
            self.PrintError("Job is invalid")
            raise Exception("Job is invalid: " + msg.__str__())
        
        
        fileName        = job.GetFileName()
        rangeLumiBlock  = job.GetLumiBlockRange()
        stepSize        = job.GetStepSize()

        
        cutLumiBlock = self.MakeLumiBlockCut(rangeLumiBlock)
        
        
        rootFile = ROOT.TFile(fileName, "READ")
        rootTemp = ROOT.TFile(self.__tempFile.GetPath(), "RECREATE")
        
        
        blackDead = []
        deadColumn = DataColumn()
        if rootFile and rootTemp:
            deadTree = "L1_Rate"
            deadMeassure = "L1_EM16_DT"
            
            tree = rootFile.Get(deadTree)
            if not tree:
                raise Exception("Dead time tree not found")
            
            tree.SetBranchStatus("*",0)
            tree.SetBranchStatus("LumiBlock",1)
            tree.SetBranchStatus(deadMeassure,1)
            subtree = tree.CopyTree(cutLumiBlock)
            if not subtree:
                raise Exception("Subtree could not be created")
            subtree.SetBranchStatus("LumiBlock", 0)
            
            for i in range(0,subtree.GetEntries(), stepSize):
                subtree.GetEntry(i)
                dTime = float(getattr(subtree, deadMeassure))
                deadColumn.append(dTime)
                
                if dTime > job.GetDeadTimeThreshold():
                    blackDead.append(int(getattr(subtree, "LumiBlock")))
                    
                
        else:
            raise Exception("Could not open root files")
         
        
        
        return blackDead