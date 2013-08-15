# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''
from model.parser.TRP.TRPWorker import TRPWorker
from model.parser.TRP.TRPSignatureJob import TRPSignatureJob

from model.parallelization.ProcessWorker import ProcessWorker
 

from model.basics.TempFile import TempFile

from model.basics.DataColumn import DataColumn

from model.physics.Signature import Signature 

import re, ROOT

class TRPSignatureWorker(ProcessWorker, TRPWorker):
    def __init__(self):
        super(TRPSignatureWorker,self).__init__()
        self.SetType(TRPSignatureJob().GetType())
        
        self.__tempFile = TempFile() 
    
    def __del__(self):
        self.__tempFile.Delete()
    
    
    __tempFile = None
    def ExecuteJob(self, job):
        try:
            self.JobIsOkay(job)
        except Exception as msg:
            self.PrintError("Job is invalid: " + str(msg))
            raise Exception("Job is invalid: " + msg.__str__())
            return
        
        
        fileName        = job.GetFileName()
        level           = job.GetLevel()
        rangeLumiBlock  = job.GetLumiBlockRange()
        stepSize        = job.GetStepSize()
        blacklist       = job.GetBlacklist()

        
        suffixKey = { "TBP": "input", \
                      "TAP": "prescale", \
                      "TAV": "raw", }
        
        for val in suffixKey.values():
            suffixKey[val] = val
        
        
        cutLumiBlock = self.MakeLumiBlockCut(rangeLumiBlock)
        
        rootFile = ROOT.TFile(fileName, "READ")
        rootTemp = ROOT.TFile(self.__tempFile.GetPath(), "RECREATE")
        
        
        
        treeName = level + "_Rate"
        tree = rootFile.Get(treeName)
        if not tree:
            self.PrintError("Invalid TRP: " + treeName + " not found.")
            raise Exception("Invalid TRP: " + treeName + " not found.")
        tree.SetBranchStatus("*", 0)
        tree.SetBranchStatus("LumiBlock", 1)

        
        
        patternBranch = "(" + str(tree.GetName())[:2] + ".*)_("
        if level == "L1":
            patternBranch += "TBP|TAP|TAV"
        else:
            patternBranch += "input|prescale|raw"            
        patternBranch += ")" 
        regexpBranch = re.compile(patternBranch) 
        
        
        branches = {}
        for branch in list(tree.GetListOfBranches()):
            if "grp" in branch.GetName().lower() or "str" in branch.GetName().lower():
                continue
            
            match = re.match(regexpBranch, branch.GetName())
            if match != None:
                signatureName = match.group(1)
                branchType = match.group(2)
                branchName = match.group(0)
                
                if not branches.has_key(signatureName):
                    branches[signatureName] = {}
                
                branches[signatureName][suffixKey[branchType]] = branchName
                tree.SetBranchStatus(branchName ,1)
            
            

        columns = {}
        subtree = tree.CopyTree(cutLumiBlock)
        
        
        if subtree:
            for i in range(0, subtree.GetEntries(), stepSize):
                subtree.GetEntry(i)
                
                if getattr(subtree, "LumiBlock") in blacklist:
                    continue
                
                for branch in branches.keys():
                    if not columns.has_key(branch):
                        columns[branch] = {}
                        
                    for branchType in branches[branch].keys():
                        if not columns[branch].has_key(branchType):
                            columns[branch][branchType] = DataColumn()
                        
                        val = getattr(subtree, branches[branch][branchType])
                        columns[branch][branchType].append(val)
        else:
            self.PrintMessage("No events in sub tree")
            raise Exception("Invalid LumiBlock range: sub tree empty.")
        
        rootTemp.Close()
        
        result = []
        for branch in branches.keys(): 
            sig = Signature()
            
            sig.SetName(branch)

            input = columns[branch]["input"].Mean()
            inputPS = columns[branch]["prescale"].Mean()

            rate = columns[branch]["raw"].Mean()
            rateErr = columns[branch]["raw"].Mean()
    

            sig.SetRate(rate)
            sig.SetRateError(rateErr)
            
            
            prescale = -1.0
            if inputPS > 0:
                prescale = input / (inputPS)
            sig.SetPrescale(prescale)
            
            
            result.append(sig)
        
        return result