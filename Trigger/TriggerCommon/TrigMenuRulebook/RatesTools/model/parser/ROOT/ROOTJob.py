# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

from model.parallelization.ProcessJob import ProcessJob

class ROOTJobException(Exception):
    def __init__(self, type = "" ,file = "", tree = "", branch = ""):
        self.SetType(type)
        self.SetFile(file)
        self.SetTree(tree)
        self.SetBranch(branch)        
        
    def GetTree(self): return self.__tree
    def SetTree(self, val): self.__tree = val
    
    def GetBranch(self): return self.__branch
    def SetBranch(self, val): self.__branch = val

    def GetType(self): return self.__type
    def SetType(self, val): self.__type = val    
    
    def GetFile(self): return self.__file
    def SetFile(self, val): self.__file = val

    def __str__(self):
        type = self.GetType()
        
        file = self.GetFile()
        tree = self.GetTree()
        branch = self.GetBranch()
        
        msg = type + ":"
        if len(branch):
            msg += " branch: " + str(branch)
        if len(tree):
            msg += " tree: " + str(tree)
        if len(file):
            msg += " file: " + str(file)
        
        return msg
            

class ROOTJob(ProcessJob):
    def __init__(self):
        ProcessJob.__init__(self)
        
        self.SetBranches("")
        self.SetCut("1")
        self.SetFileName("")
        self.SetStepSize(1)
        self.SetTree("")
    
    
    def GetFileName(self): return self.__fileName
    def SetFileName(self, val): self.__fileName = val
    
    def GetTree(self): return self.__tree
    def SetTree(self,val): self.__tree = val
    
    
    def GetBranches(self): return self.__branch
    def SetBranches(self, val): self.__branch = val
    
    def GetStepSize(self): return self.__stepSize
    def SetStepSize(self, val): self.__stepSize = val
    
    def GetCut(self): return self.__cut
    def SetCut(self, val): self.__cut = val
    
    
    def Test(self):
        import ROOT
        if not os.path.exists(self.GetFileName()):
            raise ROOTJobException("File not found", self.GetFileName())
            #raise Exception("File not found: " + str(self.GetFileName()))
        
        file = ROOT.TFile(self.GetFileName(), "READ")
        if not file:
            raise ROOTJobException("Invalid ROOT file", self.GetFileName())
            #raise Exception("File is not a valid ROOT file: " + str(self.GetFileName()))
        
        tree = file.Get(self.GetTree())
        if not tree:
            raise ROOTJobException("Tree not found", self.GetFileName(), self.GetTree())
            #raise Exception("Tree not found: "+str(self.GetTree())+", in file " + str(self.GetFileName()))
        
        for branch in self.GetBranches():
            br = tree.GetBranch(branch)
            if not br:
                raise ROOTJobException("Branch not found", self.GetFileName(), self.GetTree(), branch)
                #raise Exception("Branch not found: " + branch + ", in tree "+str(self.GetTree())+", in file " + str(self.GetFileName()))
                    
                