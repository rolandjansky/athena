# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
# 
# DataClasses: 
#   The classes describing the basic data types (files, jobs, jobTypes and jobConfigs),
#   as they are used internally.

import os
from Logger import Logger
from FileAccessor import FileAccessor

class ConfigRecord :
  def __init__(self, releaseDir, batchScheme, defaultQueue, tmpOutputDir, configId = None) : 
    self.configId = configId
    self.releaseDir = releaseDir
    self.batchScheme = batchScheme
    self.defaultQueue = defaultQueue
    self.tmpOutputDir = tmpOutputDir

  def dump(self, log, verbosity) :
    log.INFO('%3d Release : %s' % (self.configId, self.releaseDir))
    if verbosity == 0 : return
    log.INFO('    TempOut : %s' % self.tmpOutputDir)
    log.INFO('    Batch   : %10s %10s' % (self.batchScheme, self.defaultQueue))


class JobTypeRecord :
  def __init__(self, name, transform, inputs, outputDir, outputs, inputSelector, namingScheme, configId, maxJobs, cronExec, jobTypeId = None) : 
    self.jobTypeId = jobTypeId
    self.name = name
    self.transform = transform
    self.inputs = inputs #FileSpecs of FileCountSpec
    self.outputDir = outputDir
    self.outputs = outputs # FileSpecs of FileSpec
    self.inputSelector = inputSelector
    self.namingScheme = namingScheme
    self.configId = configId
    self.maxJobs = maxJobs
    self.cronExec = cronExec

  def dump(self, log, verbosity) :
    log.INFO('%3d %-20s config =%3d max =%4d %-30s, %s' % (self.jobTypeId, self.name, self.configId, self.maxJobs, self.transform, self.cronExec))
    if verbosity == 0 : return
    for i in self.inputs.entries :
      log.INFO('    Input  : %-50s (type = %s)' % (i.path, i.fileType))
    log.INFO('    InputSelector : %s' % self.inputSelector) 
    log.INFO('    Output : %s (scheme = %s)' % (self.outputDir, self.namingScheme))
    for o in self.outputs.entries :
      log.INFO('    Output : %-20s (type = %s)' % (o.path, o.fileType))

  def inputSelectorClass(self) : 
    return 'JobInputSelectors.' + self.inputSelector
    
  def namerClass(self) : 
    return 'JobNamers.' + self.namingScheme


class JobRecord :
  def __init__(self, name, jobType, status, inputs, outputs, tmpOutputDir = None, 
               creationTime = None, batchId = None, cpuTime = None, nEvents = None, properties = None,
               jobId = None) :
    self.jobId = jobId
    self.name = name
    self.jobType = jobType
    self.status = status
    self.creationTime = creationTime
    self.batchId = batchId
    self.cpuTime = cpuTime
    self.nEvents = nEvents
    self.properties = properties
    self.inputs = inputs     # list of FileRecord
    self.outputs = outputs   # list of FileRecord
    self.tmpOutputDir = tmpOutputDir

  def dump(self, log, verbosity) :
    properties = ""
    if self.properties != None : properties = self.properties
    cpuTime = ''
    if self.cpuTime != None : cpuTime = '%6s sec' % self.cpuTime
    nEvents = ''
    if self.nEvents != None : nEvents = '%8s evts' % self.nEvents
    log.INFO('%3d %-30s %-20s %-10s %6d %24s %10s %13s %s' % 
             (self.jobId, self.name, self.jobType, self.status, self.batchId, 
              self.creationTime, cpuTime, nEvents, properties))
    if verbosity == 0 : return
    log.INFO('    Temp output : %s' % self.tmpOutputDir)
    for k, i in enumerate(self.inputs) :
      log.INFO('          Input %2d : %s' % (k+1, i.name))
    for k, o in enumerate(self.outputs) :
      log.INFO('         Output %2d : %s' % (k+1, o.name))

  def inputNames(self) : 
    return [ [ str(f.stdName()) for f in set ] for set in self.inputs ] # str to avoid unicode - Athena doesn't like it

  def outputNames(self) :
    return [ str(f.stdName()) for f in self.outputs ] # str to avoid unicode - Athena doesn't like it
      
  def outputDir(self) :
    if self.outputs == None or self.outputs == [] or not self.outputs[0] : return None
    return os.path.split(self.outputs[0].name)[0]
    
  def tmpOutputNames(self) :
    return [ str(os.path.join(self.tmpOutputDir, os.path.split(o.stdName())[1])) for o in self.outputs ] # str to avoid unicode - Athena doesn't like it

  def addProperty(self, prop) :
    if self.properties == None : self.properties = ''
    if len(self.properties) > 0 : self.properties += ' '
    self.properties += prop

class FileRecord : 
  def __init__(self, name, fileType, status = 'OK', creationTime = None, fileId = None) :
    self.fileId = fileId
    self.name = name
    self.fileType = fileType
    self.status = status
    self.creationTime = creationTime
    
  def dump(self, log, verbosity) :
    log.INFO('%3d %-70s %-10s %-10s %s' % (self.fileId, self.name, self.fileType, self.status, self.creationTime))
    
  def stdName(self) :
    return FileAccessor(self.name).stdForm()
  
  def remove(self, log) :
    if not FileAccessor(self.name, log.stream).remove() : return False
    self.status = 'INVALID'
    return True
    
#---------------------------------------------------------------------------------
    
class FileSpec :
  def __init__(self, path, fileType = 'ANY', minimumAge = 0) :
    self.path = path
    self.fileType = fileType
    self.minimumAge = minimumAge

  def __str__(self) :
    lf = [ self.path, self.fileType ]
    return ':'.join(lf)


class FileSpecs(Logger) :
  def __init__(self, entries = [], stream = None) :
    Logger.__init__(self, stream)
    self.entries = []
    for en in entries : self.entries.append(e)

  def append(self, spec) :
    self.entries.append(spec)
    return True

  def __len__(self) :
    return len(self.entries)

  def __str__(self) :
    lf = []
    for entry in self.entries : 
      lf.append(str(entry))
    return ','.join(lf)
    
  def load(self, sf) :
    self.entries = []
    if sf == '' : return self.ERROR('Invalid string form for FileSpecs: empty string')
    lf1 = sf.split(',')
    for data in lf1 :
      if data == '' : return self.ERROR('Invalid string form for FileSpecs : empty item in comma-separated list')
      lf2 = data.split(':')
      if len(lf2) != 2 : return self.ERROR('Invalid string form for FileSpecs : item is not a :-separated pair <path>:<type>')
      self.append(FileSpec(lf2[0], lf2[1]))
    return True

  def addDefaults(self, inputFilesAndTypes) :
    if inputFilesAndTypes == '' : 
      self.ERROR('Invalid input files and types')
      return None
    newItems = []
    inputItems = inputFilesAndTypes.split(',')
    for item in inputItems :
      fields = item.split(':')
      if len(fields) > 2 :
        self.ERROR('Invalid input files and types : too many arguments')
        return None
      if len(fields) == 1 : 
        fields.append('ANY')
      newItems.append(':'.join(fields))
    return ','.join(newItems)

  def loadFromString(self, inputFilesAndTypes) :
    newInputs = self.addDefaults(inputFilesAndTypes)
    return self.load(newInputs)
  
  
class JobTypeSpec(Logger) :
  def __init__(self, stream = None) :
    Logger.__init__(self, stream)
    self.m_name = ''
    self.m_transform = ''
    self.m_inputs = FileSpecs(stream = self.stream)
    self.m_outputDir = ''
    self.m_outputs = FileSpecs(stream = self.stream)
    self.m_configId = 0
    self.m_inputSelector = 'RequireCounts()'
    self.m_namingScheme = 'JobId()'
    self.m_maxJobs = -1
    self.m_cronExec = ''

  def setName(self, name) :
    self.m_name = name
    
  def setTransform(self, transform) :
    self.m_transform = transform
    
  def addInputs(self, path, fileType = 'ANY', minimumAge = 0) :
    self.m_inputs.append(FileSpec(path, fileType, minimumAge))
    
  def setOutputDir(self, path) :
    self.m_outputDir = path
    
  def addOutput(self, path, fileType = 'ANY') :
    self.m_outputs.append(FileSpec(path, fileType))
    
  def setInputSelector(self, inputSelector) :
    self.m_inputSelector = inputSelector
  
  def setNamingScheme(self, scheme) :
    self.m_namingScheme = scheme
    
  def setConfigId(self, configId) :
    self.m_configId = configId

  def setMaxJobs(self, maxJobs) :
    if maxJobs > 0 : self.m_maxJobs = maxJobs

  def setCronExec(self, cronExec) :
    self.m_cronExec = cronExec

