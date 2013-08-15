# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.parallelization.ProcessWorker import ProcessWorker
 
from model.basics.TempFile import TempFile

from model.basics.DataColumn import DataColumn 

from model.parser.ROOT.ROOTJob import ROOTJob

import ROOT

class ROOTWorker(ProcessWorker):
    def __init__(self):
        super(ROOTWorker,self).__init__()
        self.SetType(ROOTJob().GetType())
        self.__tempFile = TempFile() 
    
    def __del__(self):
        self.__tempFile.Delete()
        
    
    __tempFile = None
    def ExecuteJob(self, job):
        job.Test()
        return self.ProcessJob(job)
    
    def ProcessJob(self, job):
        fileName        = job.GetFileName()
        treeName        = job.GetTree()
        branchNames     = job.GetBranches()
        stepSize        = job.GetStepSize()
        cut             = job.GetCut()
        
        
        result = {}
        for name in branchNames:
            result[name] = DataColumn()
        
        rootFile = ROOT.TFile(fileName, "READ")
        rootTemp = ROOT.TFile(self.__tempFile.GetPath(), "RECREATE")
        
                    
        tree = rootFile.Get(treeName)
        
        tree.SetBranchStatus("*",0)
        for name in branchNames:
            tree.SetBranchStatus(name,1)
        
        #print cut
        subtree = tree.CopyTree(cut)
        
        if subtree:
            for i in range(0,subtree.GetEntries(), stepSize):
                subtree.GetEntry(i)
                for name in branchNames:
                    val = getattr(subtree, name)
                    result[name].append(val)
                                    
        rootTemp.Close()
                
        return result
    
    