# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# JobNamers.py
#   Provides and interface for the task of producing a job name
#   from the list of its input files.
#   Standard implementations (from jobId, run and event numbers) are provided, 
#   more can be added

import os, re
from Logger import Logger
from Tools import getRunNumber

class JobNamer(Logger) :
  def __init__(self) :
    Logger.__init__(self, None)
  
  def configure(self, dataAccessor, job, stream) :
    self.m_data = dataAccessor
    self.m_job = job
    self.m_jobType = self.m_data.getJobTypeByName(job.jobType)
    self.m_config = self.m_data.getConfig(self.m_jobType.configId)
    self.stream = stream
  
  def setValues(self) : # this is the main interface method (see CAFManager)
    self.m_job.name = self.name()
    if not self.m_job.name : return False
    self.m_job.tmpOutputDir = self.tmpOutputDir()
    if not self.m_job.tmpOutputDir : return False
    return self.outputDir()
  
  def name(self) :  # redefine!
    return None
  
  def outputDir(self) : # sensible default
    name = self.m_job.name
    if not name : return None
    return os.path.join(self.m_jobType.outputDir, name)

  def tmpOutputDir(self) : # sensible default
    name = self.m_job.name
    if not name : return None
    return os.path.join(self.m_config.tmpOutputDir, name)


class JobId(JobNamer) :
  
  def __init__(self) :
    JobNamer.__init__(self)
  
  def name(self) :
    jobId = self.m_data.getLastJobId() + 1
    return self.m_jobType.name + '_' + str(jobId)


class RunNumber(JobNamer) :
  
  def __init__(self) :
    JobNamer.__init__(self)
  
  def name(self) :
    if not len(self.m_job.inputs) : 
      self.ERROR('RunNumber namer : job has empty inputs, cannot set name')
      return None
    if not len(self.m_job.inputs[0]) : 
      self.ERROR('RunNumber namer : first job input group is empty, cannot set name')
      return None
    f = self.m_job.inputs[0][0] # first file in the first group
    runNum = getRunNumber(f.name)
    if runNum == None :
      self.ERROR('RunNumber namer : job has invalid input file name format, cannot set name')
      return None
    return self.m_jobType.name + '_' + runNum


class RunNumberAndIndex(JobNamer) :
  
  def __init__(self) :
    JobNamer.__init__(self)
    self.limit = 9999
    
  def name(self) :
    if not len(self.m_job.inputs) : 
      self.ERROR('RunNumberAndIndex namer : job has empty inputs, cannot set name')
      return None
    if not len(self.m_job.inputs[0]) : 
      self.ERROR('RunNumberAndIndex namer : first job input group is empty, cannot set name')
      return None
    f = self.m_job.inputs[0][0] # first file in the first group
    runNum = getRunNumber(f.name)
    if runNum == None :
      self.ERROR('RunNumberAndIndex namer : job has invalid input file name format, cannot set name')
      return None
    i = 1
    candidateName = None
    while i < self.limit : # sanity check...
      candidateName = self.m_jobType.name + '_' + runNum + '_' + str(i)
      if not self.m_data.getJobId(candidateName) : break
      i = i + 1
    if i >= self.limit :
      self.ERROR('RunNumberAndIndex namer : limit (' + str(self.limit) + ') was reached: either too many blocks for this run, or (more likely) a problem with the name')
      return None
    return candidateName


class EvtNumber(JobNamer) :
  
  def __init__(self) :
    JobNamer.__init__(self)
  
  def name(self) :
    if not len(self.m_job.inputs) : 
      self.ERROR('EvtNumber namer : job has empty inputs, cannot set name')
      return None
    if not len(self.m_job.inputs[0]) : 
      self.ERROR('EvtNumber namer : first job input group is empty, cannot set name')
      return None
    f = self.m_job.inputs[0][0] # first file in the first group    
    mo = re.match('.*run([0-9]*)evt([0-9]*).*\._([0-9]*)\.data', f.name)
    if mo != None and len(mo.groups()) == 3 : # case of run, evt, version
      return self.m_jobType.name + '_' + '_'.join(mo.groups())
    mo = re.match('.*evt([0-9]*)\._([0-9]*)\.data', f.name)
    if mo != None and len(mo.groups()) == 2 : # case of evt, version
      return self.m_jobType.name + '_' + '_'.join(mo.groups())
    #mo = re.match('([0-9]+).*From([0-9]+)\.data', os.path.split(f.name)[1])
    #if mo != None and len(mo.groups()) == 2 : # case of XXEventsFromRunYY.data
    #  return self.m_jobType.name + '_' + mo.groups()[1] + '_' + mo.groups()[0]
    #mo = re.match('.*From([0-9]+)\.data', os.path.split(f.name)[1])
    #if mo != None and len(mo.groups()) == 1 : # case of SomeEventsFromRunYY.data
    #  return self.m_jobType.name + '_' + mo.groups()[0] + '_' + job.jobId
    fileName = os.path.split(f.name)[1]
    fileSplits = fileName.split('.')
    if len(fileSplits) > 0 and len(fileSplits[0]) > 0 : return self.m_jobType.name + '_' + fileSplits[0] 
    self.ERROR('EvtNumber namer : cannot find event number in input file name, cannot set name')
    return None


