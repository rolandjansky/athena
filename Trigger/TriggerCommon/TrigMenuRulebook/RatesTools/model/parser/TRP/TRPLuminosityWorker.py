# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''
from model.parser.TRP.TRPWorker import TRPWorker
from model.parser.TRP.TRPLuminosityJob import TRPLuminosityJob

from model.parallelization.ProcessWorker import ProcessWorker
 

from model.basics.TempFile import TempFile

from model.basics.DataColumn import DataColumn 

import ROOT

class TRPLuminosityWorker(ProcessWorker, TRPWorker):
    def __init__(self):
        super(TRPLuminosityWorker,self).__init__()
        self.SetType(TRPLuminosityJob().GetType())
        
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
        blacklist       = job.GetBlacklist()
        
        cutLumiBlock = self.MakeLumiBlockCut(rangeLumiBlock)
        
        
        rootFile = ROOT.TFile(fileName, "READ")
        rootTemp = ROOT.TFile(self.__tempFile.GetPath(), "RECREATE")
        
        
        
        lumiColumn = DataColumn()
        if rootFile and rootTemp:
            lumiTree = "Lu_Rate_Luminosity"
            lumiMeassure = "ATLAS_PREFERRED_Inst_CalibLumi_Val"
            
            tree = rootFile.Get(lumiTree)
            if not tree:
                raise Exception("Luminosity tree not found")
            
            tree.SetBranchStatus("*",0)
            tree.SetBranchStatus("LumiBlock",1)
            tree.SetBranchStatus(lumiMeassure,1)
            subtree = tree.CopyTree(cutLumiBlock)
            if not subtree:
                raise Exception("Luminosity subtree tree could not be created")
            subtree.SetBranchStatus("LumiBlock", 0)
            
            for i in range(0,subtree.GetEntries(), stepSize):
                subtree.GetEntry(i)
                if getattr(subtree, "LumiBlock") in blacklist:
                    continue
                lumiColumn.append(1e30 * float(getattr(subtree, lumiMeassure)))
        else:
            raise Exception("Could not open root files")
        
        return lumiColumn