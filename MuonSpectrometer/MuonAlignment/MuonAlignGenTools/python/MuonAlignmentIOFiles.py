# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



## @file:   MuonAlignmentIOFiles.py
## @brief:  Returns names of input and output files used specifically 
#           for muon alignment 
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.IOFiles import IOFiles

## MuonAlignmentIOFiles is used to return names of input and output files
#  used for muon alignment.  For A-lines, this class calls a separate ALinesFiles
#  class which can be configured outside ATHENA.
class MuonAlignmentIOFiles () :

    _asztInputFile      = ""
    _asztOutputFile     = ""
    _asztInputFileName  = ""
    _asztOutputFileName = ""
    _level1AsciiFile    = ""
    _inputLevel1File    = ""
    _ntupleFileName     = ""
    _ntupleFilePath     = ""
    _ntupleFile         = ""
    _IOFiles            = IOFiles()
    _rootFilePathIsSet  = False
    _asztFilePathIsSet  = False
    _initialAsztFilePathIsSet = False
    
    def __init__(self):
        mlog = logging.getLogger( 'MuonAlignmentIOFiles::__init__' )
        if not MuonAlignmentIOFiles._rootFilePathIsSet:
            self.setRootFilePath(align.outputPath())
        if not MuonAlignmentIOFiles._asztFilePathIsSet:
            self.setAsztFilePath(mualign.asztFilePath())
        if not MuonAlignmentIOFiles._initialAsztFilePathIsSet:
            self.setInitialAsztFilePath(mualign.initialAsztFilePath())   

    def setRootFilePath(self,path) :
        MuonAlignmentIOFiles._IOFiles.rootFilePath = path
        MuonAlignmentIOFiles._rootFilePathIsSet = True

    def setAsztFilePath(self,path) :
        MuonAlignmentIOFiles._IOFiles.asztFilePath = path
        MuonAlignmentIOFiles._asztFilePathIsSet = True  
    
    def setInitialAsztFilePath(self,path) :
        MuonAlignmentIOFiles._IOFiles.initialAsztFilePath = path   
        MuonAlignmentIOFiles._initialAsztFilePathIsSet = True

    def asztInputFile(self) :
        if not MuonAlignmentIOFiles._asztInputFile :
            if align.iteration()>0 or \
                    align.dataType()=='MC' or align.dataType()=='simCosmic' :
                MuonAlignmentIOFiles._asztInputFile = MuonAlignmentIOFiles._IOFiles.asztInputFile(
                    align.fileRootName(),
                    align.iteration(),
                    align.jobIndex())
            else :
                print "setting asztInputFile to ''"
                MuonAlignmentIOFiles._asztInputFile=""
            print "asztInputFile: ",MuonAlignmentIOFiles._asztInputFile
        return MuonAlignmentIOFiles._asztInputFile

    def asztOutputFile(self) :
        if not MuonAlignmentIOFiles._asztOutputFile :
            MuonAlignmentIOFiles._asztOutputFile = MuonAlignmentIOFiles._IOFiles.asztOutputFile(
                align.fileRootName(),
                align.iteration(),
                align.jobIndex())
        return MuonAlignmentIOFiles._asztOutputFile

    def asztInputFileName(self) :
        if not MuonAlignmentIOFiles._asztInputFileName :
            MuonAlignmentIOFiles._asztInputFileName =  MuonAlignmentIOFiles._IOFiles.asztInputFileName(
                align.fileRootName(),
                align.iteration(),
                align.jobIndex())
        return MuonAlignmentIOFiles._asztInputFileName

    def asztOutputFileName(self) :
        if not MuonAlignmentIOFiles._asztOutputFileName :
            MuonAlignmentIOFiles._asztOutputFileName = MuonAlignmentIOFiles._IOFiles.asztOutputFileName(
                align.fileRootName(),
                align.iteration(),
                align.jobIndex())
        return MuonAlignmentIOFiles._asztOutputFileName

    def level1AsciiFile(self) :
        if not MuonAlignmentIOFiles._level1AsciiFile :
            if align.iteration()>0 or \
                    align.dataType()=='MC' or align.dataType()=='simCosmic' :
                MuonAlignmentIOFiles._level1AsciiFile = MuonAlignmentIOFiles._IOFiles.level1AsciiFile(
                    align.fileRootName(),
                    align.iteration(),
                    align.jobIndex())
            else :
                MuonAlignmentIOFiles._level1AsciiFile=""
            print "level1AsciiFile: ",MuonAlignmentIOFiles._level1AsciiFile
        return MuonAlignmentIOFiles._level1AsciiFile

    def inputLevel1File(self) :
        if not MuonAlignmentIOFiles._inputLevel1File :
            if align.iteration()>0 :
                MuonAlignmentIOFiles._inputLevel1File = MuonAlignmentIOFiles._IOFiles.inputLevel1File(
                    align.fileRootName(),
                    align.iteration(),
                    align.jobIndex())
            elif align.dataType()=='MC' or align.dataType()=='simCosmic' :
                MuonAlignmentIOFiles._inputLevel1File = MuonAlignmentIOFiles._IOFiles.inputLevel1File(
                    align.fileRootName(),
                    align.iteration())                
            else :
                MuonAlignmentIOFiles._inputLevel1File=""
            print "level1AsciiFile: ",MuonAlignmentIOFiles._inputLevel1File
        return MuonAlignmentIOFiles._inputLevel1File

    def ntupleFileName(self) :
        if not MuonAlignmentIOFiles._ntupleFileName :
            MuonAlignmentIOFiles._ntupleFileName = MuonAlignmentIOFiles._IOFiles.ntupleFileName(
                align.fileRootName(),
                align.iteration(),
                align.jobIndex())
        return MuonAlignmentIOFiles._ntupleFileName

    def ntupleFilePath(self) :
        if not MuonAlignmentIOFiles._ntupleFilePath :
            MuonAlignmentIOFiles._ntupleFilePath = MuonAlignmentIOFiles._IOFiles.ntupleFilePath()
        return MuonAlignmentIOFiles._ntupleFilePath

    def ntupleFile(self) :
        if not MuonAlignmentIOFiles._ntupleFile :
            MuonAlignmentIOFiles._ntupleFile = MuonAlignmentIOFiles._IOFiles.ntupleFile(
                align.fileRootName(),
                align.iteration(),
                align.jobIndex())
        return MuonAlignmentIOFiles._ntupleFile
        

   
