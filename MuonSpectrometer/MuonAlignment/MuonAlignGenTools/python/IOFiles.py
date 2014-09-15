# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   ALinesFiles.py
## @brief:  Returns names for input and output 
#           A-lines files.  Can be used outside 
#           ATHENA framework.  
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

## ALinesFiles is used to return names of input and output 
#  A-lines files outside the Athena framework.
class IOFiles() :
    def __init__(self) :
        IOFiles.initialAsztFilePath = "asztfiles"
        IOFiles.asztFilePath = "."
        IOFiles.rootFilePath = "."

    def setInitialAsztFilePath(self, initPath) :
        IOFiles.initialAsztFilePath = initPath
        return

    def setAsztFilePath(self, initPath) :
        IOFiles.asztFilePath = initPath
        return
    def setRootFilePath(self, initPath) :
        IOFiles.rootFilePath = initPath
        return

    def level1AsciiFile(self,name,iteration=0,index=-1) :
        if(iteration<1):
            return self.getPath(self.initialAsztFilePath)+\
                   self.level1AsciiFileName(name,iteration,index)
        else:
            return self.getPath(self.asztFilePath)+\
                   self.level1AsciiFileName(name,iteration,index)

    def inputLevel1File(self,name,iteration=0,index=-1) :
        if(iteration<1):
            return self.getPath(self.initialAsztFilePath)+\
                   self.inputLevel1FileName(name,iteration,index)
        else:
            return self.getPath(self.asztFilePath)+\
                   self.inputLevel1FileName(name,iteration,index)

    def asztInputFile(self,name,iteration=0,index=-1) :
        if(iteration<1):
            return self.getPath(self.initialAsztFilePath)+\
                   self.asztInputFileName(name,iteration,index)
        else:
            return self.getPath(self.asztFilePath)+\
                   self.asztInputFileName(name,iteration,index)
    
    def asztOutputFile(self,name,iteration=0,index=-1) :
        return self.getPath(self.asztFilePath)+\
               self.asztOutputFileName(name,iteration,index)
 
    def asztInputFileName(self,name,iteration=0,index=-1) :
        if(iteration<1):
            return name+".txt"
        else:
            return self.asztFileName(name,iteration-1,index)

    def asztOutputFileName(self,name,iteration=0,index=-1) :
        return self.asztFileName(name,iteration,index)

    def asztFileName(self,name,iteration,index) :
        return name+self.getString(iteration,index)+".txt"

    def level1AsciiFileName(self,name,iteration,index) :
        return "level1_"+name+self.getString(iteration,index)+".txt"

    def inputLevel1FileName(self,name,iteration,index) :
        return "level1_"+name+self.getString(iteration-1,index)+".txt"

    def ntupleFileName(self,name,iteration=0,index=-1) :
        return "MuonAlign_"+name+self.getString(iteration,index)+".root"

    def ntupleFilePath(self) :
        return self.getPath(self.rootFilePath)

    def ntupleFile(self,name,iteration=0,index=-1):
        return self.ntupleFilePath() + self.ntupleFileName(name,iteration,index)
        
    def getString(self,iteration,index) :
        retstring = ""
        if iteration>-1:
            retstring += "_iter"+str(iteration)
        if str(index) == "*":
            retstring += "_index*"
        elif index>-1:
            retstring += "_index"+str(index)
        return retstring

    def getPath(self,path):
        if len(path)>0 and not path.endswith("/"):
            path += "/"
        elif len(path)==0:
            path = "./"
        return path
       
