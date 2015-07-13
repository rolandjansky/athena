# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
# 
# Checkers:
#   Helper classes to check the validity of a data object 
#   (see DataClasses.py for definitions)


import os
from Tools import AllowedValues
from Logger import Logger
import JobInputSelectors
import JobNamers
from FileAccessor import FileAccessor

class FileTypeChecker(AllowedValues) :
  def __init__(self) :
    AllowedValues.__init__(self, ['POOL', 'RAW', 'ESD', 'AOD', 'DPD', 'NTUP', 'HIST', 'TXT', 'LOG', 'LOGZIP', 'RUNARGS', 'ANY'])


class FileSpecChecker(Logger) :
  def __init__(self, stream = None) :
    Logger.__init__(self, stream)
    self.m_typeChecker = FileTypeChecker()
    
  def isOK(self, fileSpecs) :
    for f in fileSpecs.entries :
      if not self.m_typeChecker.isAllowed(f.fileType) : 
        return self.ERROR('Invalid inputs (type = ' + f.fileType + ')')
    return True


class ConfigChecker(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor

  def isOK(self, config) :
    return True


class JobTypeChecker(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor
    self.m_specChecker = FileSpecChecker(stream)
    
  def isOK(self, jobType, isNewType = False) :
    if isNewType :
      oldType = self.m_data.getJobTypeByName(jobType.name)
      if oldType : return self.ERROR('A job type with name ' + jobType.name + ' is already defined')
    try:
      namer = eval(jobType.namerClass())
    except Exception, inst : 
      s = 'Available choices:'
      for name in dir(JobNamers) : 
        if type(eval('JobNamers.' + name)).__name__ == 'classobj' : s += ' ' + name      
      return self.ERROR('Could not instantiate ' + jobType.namerClass() + ' : ' + str(inst) + '\n' + s)
    try:
      inputSelector = eval(jobType.inputSelectorClass())
    except Exception, inst : 
      s = 'Available choices:'
      for name in dir(JobInputSelectors) : 
        if type(eval('JobInputSelectors.' + name)).__name__ == 'classobj' : s += ' ' + name
      return self.ERROR('Cound not instantiate ' + jobType.inputSelectorClass() + ' : ' + str(inst) + '\n' + s)
    if jobType.configId == 0 : jobType.configId = self.m_data.getLastConfigId()
    config = self.m_data.getConfig(jobType.configId)
    if config == None :
      return self.ERROR('Unknown config ' + str(jobType.configId))
    releaseDir = config.releaseDir
    fullTransName = os.path.join(releaseDir, jobType.transform)
    if not os.access(fullTransName, os.R_OK) :
      return self.ERROR('Transform not found at ' + fullTransName)
    if not FileAccessor(jobType.outputDir).access() :
      return self.ERROR('Could not access output directory ' + jobType.outputDir)
    if not self.m_specChecker.isOK(jobType.inputs)  : return self.ERROR('invalid jobType inputs')
    if not self.m_specChecker.isOK(jobType.outputs) : return self.ERROR('invalid jobType outputs')
    return True
    

class FileChecker(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor
    self.m_typeChecker = FileTypeChecker()
    self.m_statChecker = AllowedValues(['OK', 'RESERVED', 'INVALID'])

  def isOK(self, fileRec, shouldExist = False) :
    if shouldExist and not os.access(fileRec.name, os.R_OK) :
      return self.ERROR('File ' + fileRec.name + ' is not readable')
    if not self.m_typeChecker.isAllowed(fileRec.fileType) :
      return self.ERROR('Unknown file type ' + fileRec.fileType)
    if not self.m_statChecker.isAllowed(fileRec.status) :
      return self.ERROR('Unknown file status ' + fileRec.status)
    return True


class JobChecker(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor
    self.m_statChecker = AllowedValues(['SUCCESS', 'RUN_SUCCESS', 'FAILED', 'RUN_FAILED', 'SCHEDULED', 'OUTPUT_ERROR'])

  def isOK(self, job, outputFiles = []) :
    if self.m_data.getJobTypeByName(job.jobType) == None : 
      return self.ERROR('Unknown job type ' + job.jobType)
    if not self.m_statChecker.isAllowed(job.status) :
      return self.ERROR('Unknown job status ' + job.status)
    for i in job.inputs :
      if i.status == 'INVALID' : return False
    for o in job.outputs :
      if o.status == 'INVALID' : return False
    for out in outputFiles :
      #print out, os.path.split(out[0])
      outputDir = os.path.split(out[0])[0] 
      if not os.access(outputDir, os.F_OK) : 
        try: os.makedirs(outputDir) # this is the recursive mkdir
        except: pass
      if not os.access(outputDir, os.W_OK) :
        return self.ERROR('Could not get write access to output directory ' + outputDir)
      if not self.m_fileTypes.isAllowed(out[1]) :
        return self.ERROR('Unknown output file type ' + fileType)
    return True

      
