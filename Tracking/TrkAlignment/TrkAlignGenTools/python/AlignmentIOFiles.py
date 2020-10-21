# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


## @file:   AlignmentIOFiles.py
## @brief:  Used inside Athena for alignment to get various 
## 	    files for input and output.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## AlignmentIOFiles provides methods for getting file names used 
#  for alignment.
class AlignmentIOFiles () :

    _rootNameBase = ""
    _rootName = ""
    _evtList = ""
    _matrixPath = ""

    ## constructor
    def __init__(self):
        pass

    ## filename containing list of good events 
    #  (each line is run and event number)
    def eventList(self, append="") :
        if self._evtList == "" :            
            self._evtList = "goodEvts_" + self.rootName() + ".txt"
        return self._evtList

    ## binary version of matrix files
    def matrixPathBin(self, append="") : 
        if self._matrixPath == "" :
            self._matrixPath = "matrix_" + self.rootName()
        return self._matrixPath + ".bin"

    ## text version of matrix files
    def matrixPathTxt(self, append="") : 
        if self._matrixPath == "" :
            self._matrixPath = "matrix_" + self.rootName()
        return self._matrixPath + ".txt"

    ## root name used in creating all file names
    def rootName(self,append="") :
        if self._rootName == "" :
            self._rootName = self.rootNameBase()+"_iter"+str(align.iteration())
        return self._rootName

    ## root name without iteration
    def rootNameBase(self,append="") :
        if self._rootNameBase == "" :
            self._rootNameBase = align.fileRootName()
            if append != "":
                self._rootNameBase = self._rootNameBase + "_" + append
        return self._rootNameBase
