# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# JobInputSelectors.py
#  Provides an interface for the task of extracting the list of inputs for the next job,
#  given the list of all available inputs. 
#  Standard implementations (extract a given number of files, all files for the next run, etc.)
#  are also provided, more can be added.


import os, re
from Logger import Logger
from Tools import getRunNumber, Regexp

class JobInputSelector(Logger) :
  
  def __init__(self) :
    pass
  
  def configure(self, dataAccessor, jobType, stream) :
    self.stream = stream
    self.m_data = dataAccessor
    self.m_jobType = jobType
    self.m_inputs = None
    self.m_config = self.m_data.getConfig(self.m_jobType.configId)

  def setInputs(self, job, options = []) :
    selectedInputs = self.getSelectedInputs(options)
    if selectedInputs == None or selectedInputs == [] : 
      self.TRACE('No available inputs found')
      return False
    self.TRACE('Using ' + str(len(selectedInputs)) + ' input group(s), sizes = ' + str([len(group) for group in selectedInputs]))
    job.inputs = selectedInputs
    return True

  def getFreeInputs(self) :
    freeInputs = []
    for i in self.m_jobType.inputs.entries :
      files = self.getFilesMatching(i) # list of FileRecords
      fileDict = dict([ (f.fileId, f) for f in files ]) # no dict comprehension in this python version...
      self.TRACE('All files ' + str(len(files)))
      usedFiles = self.m_data.getUsedFiles(self.m_jobType)
      self.TRACE('Used files ' + str(len(usedFiles)))
      freeFileIds = list(set(fileDict.keys()) - set(usedFiles))
      freeFileIds.sort()
      freeFiles = [ fileDict[fileId] for fileId in freeFileIds ]
      self.TRACE('Free files ' + str(len(freeFiles)))
      freeInputs.append(freeFiles)
    if len(freeInputs) < len(self.m_jobType.inputs) : 
      self.TRACE('Could not find all required input files')
      return []
    self.TRACE('Free inputs ' + str([ len(input) for input in freeInputs ]))
    return freeInputs

  def select(self, options = []) : # to be redefined - should return the selection, and remove it from m_inputs...
    return None

  def getSelectedInputs(self, options = []) :
    if self.m_inputs == None : self.m_inputs = self.getFreeInputs()
    return self.select(options)

  # This returns a list of FileRecords
  def getFilesMatching(self, fileSpec) :
    files = self.m_data.getAllFiles()
    results = []
    regexp = Regexp(fileSpec.path)
    for f in files :
      if f.fileType != fileSpec.fileType : continue
      if regexp.match(f.name) : results.append(f)
    return results

  # This returns a dictionary of jobType String -> T/F
  def getAssignedJobTypes(self, fileRecs) :
    assignedJobs = {}
    fileIds = [ fileRec.fileId for fileRec in fileRecs ]
    jobIds = self.m_data.getJobTypesForFiles(fileIds)
    return jobIds


def instance(jobType, dataAccessor, stream) :
  try:
    inputSelector = eval(jobType.inputSelector)
  except Exception, inst :
    Logger(stream).ERROR('Cannot instantiate selector of type ' + str(jobType.inputSelector) + ' : ' + str(inst))
    return None
  inputSelector.configure(dataAccessor, jobType, stream)
  return inputSelector

class RequireCounts(JobInputSelector) :
  def __init__(self, min = None, max = None) :
    JobInputSelector.__init__(self)
    
    # first case: 1 input, min/max provided
    if type(min) == int and type(max) == int :
      self.minCounts = [ min ]
      self.maxCounts = [ max ]
      return

    # second case : a vector of count pairs is provided as first argument 
    if type(min) == type([]) and type(max) == type([]) :
      self.minCounts = []
      self.maxCounts = []    
      for i, l in enumerate(min) :     
        self.minCounts.append(l)
        self.maxCounts.append(max[i])
      return    
    raise TypeError

  def select(self, options = []) :   
    selected = []
    for i, group in enumerate(self.m_inputs) :
      if len(group) < self.minCounts[i] : 
        self.TRACE('  Number of available files is insufficient : ' + str(len(group)) + ' < ' + str(self.minCounts[i]))
        return None
      if len(group) > self.maxCounts[i] : 
        selected.append(group[:self.maxCounts[i]])
        self.m_inputs[i] = group[self.maxCounts[i]:]
        self.TRACE('  Number of available files is too large : ' + str(len(group)) + ' > ' + str(self.maxCounts[i]) + ' : truncating to ' + str(self.maxCounts[i]))
        continue
      self.m_inputs[i] = []
      selected.append(group)
    return selected
      

class FullRuns(JobInputSelector) :
  def __init__(self, nRuns = 1) :
    JobInputSelector.__init__(self)
    self.nRuns = 1

  def select(self, options = []) :
    selected = []
    selectedRun = None
    for i, group in enumerate(self.m_inputs) :
      for j, f in enumerate(group) :
        runNum = getRunNumber(f.name)
        if runNum == None : 
          self.ERROR('FullRuns selector cannot find run number for file ' + f.name)
          return None
        if selectedRun == None : selectedRun = runNum
        if runNum != selectedRun :
          selected.append(group[:j])
          self.m_inputs[i] = group[j:]
          break
      if 'force-end' in options : 
        selected.append(group)
        self.m_inputs[i] = []
    return selected


class FullRunsWithLimit(JobInputSelector) :
  def __init__(self, nRuns = 1, fileLimit = -1) :
    JobInputSelector.__init__(self)
    self.nRuns = 1
    self.fileLimit = fileLimit
    self.isLastInRun = False

  def setInputs(self, job, options = []) :
    if not JobInputSelector.setInputs(self, job, options) : return False
    if self.isLastInRun : job.addProperty('LAST_IN_RUN')
    return True

  def select(self, options = []) :
    selected = []
    selectedRun = None
    self.isLastInRun = True
    for i, group in enumerate(self.m_inputs) :
      for j, f in enumerate(group) :
        endPoint = -1
        if 'force-end' in options and j == len(group) - 1 : # we reached the end: if force-end, get all the rest
         endPoint = len(group)
        else :
          runNum = getRunNumber(f.name)
          if runNum == None : 
            self.ERROR('FullRuns selector cannot find run number for file ' + f.name)
            return None
          if selectedRun == None : selectedRun = runNum
          if runNum != selectedRun :
            self.TRACE('Stopping at index ' + str(j) + ' since a new run number is encountered')
            endPoint = j
          else  :
            if self.fileLimit > 0 and j == self.fileLimit : 
              self.TRACE('Stopping at index ' + str(j) + ' since file limit is reached (total = ' + str(len(group)) + ')')
              endPoint = j
              if j < len(group) - 1 : 
                self.TRACE('Total number of files = ' + str(len(group)) + ' > ' + str(j) + ', setting isLastInRun to False')
                self.isLastInRun = False
        if endPoint < 0 : continue
        if self.fileLimit > 0 and endPoint > self.fileLimit : 
          endPoint = self.fileLimit
          self.isLastInRun = False
        selected.append(group[:endPoint])
        self.m_inputs[i] = group[endPoint:]
        break
    return selected
