# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# DataAccessor_SQLite :
#   The DB access class for CAFManager. Provides low-level DB access.
#   This implementation uses 2 SQLite databases as backend (Operations DB
#   for jobs and files, Config DB for jobTypes and jobConfigs)

import os, time, traceback
from Logger import Logger
from DataClasses import ConfigRecord, JobTypeRecord, FileRecord, JobRecord
from Checkers import ConfigChecker, JobTypeChecker, FileChecker, JobChecker
from ConfigDB import ConfigDB
from OperationsDB import OperationsDB
from FileAccessor import FileAccessor
import JobInputSelectors

class DataAccessor_SQLite(Logger) :

  def __init__(self, configFile, stream = None) :
    Logger.__init__(self, stream)
    if not os.access(configFile, os.R_OK) : 
      self.m_ok = False
      self.ERROR('Could not access confing file ' + configFile)
      return
    f = open(configFile)
    self.m_cfgPath = f.readline()[:-1]
    self.m_opsPath = f.readline()[:-1]
    if os.access(self.m_cfgPath, os.F_OK) :
      self.m_cfgDB = ConfigDB(self.m_cfgPath, stream = stream)
    else :
      self.m_cfgDB = ConfigDB(self.m_cfgPath, 'new', stream = stream)
    if os.access(self.m_opsPath, os.F_OK) :
      self.m_opsDB = OperationsDB(self.m_opsPath, stream = stream)
    else :
      self.m_opsDB = OperationsDB(self.m_opsPath, 'new', stream = stream)
    self.m_ok = self.m_cfgDB.m_ok and self.m_opsDB.m_ok

  def ok(self) :
    return self.m_ok

  def addConfig(self, config, commit = True, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if not ConfigChecker(self, self.stream).isOK(config) : return False
    return self.m_cfgDB.addConfig(config, commit, simulate)

  def addJobType(self, jobType, commit = True, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if not JobTypeChecker(self, self.stream).isOK(jobType, isNewType=True) : return self.ERROR('JobType verification failed')
    return self.m_cfgDB.addJobType(jobType, commit, simulate)
  
  def addFile(self, fileRec, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if  not FileChecker(self, self.stream).isOK(fileRec) : return False
    return self.m_opsDB.addFile(fileRec, commit)

  def addFiles(self, fileRecs, checkForDupes = True, commit = True) :
    if checkForDupes:
      self.ERROR("Check for duplicates is not implemented. Use checkForDupes = False if you know what you're doing")
      return []
    if not self.m_ok : 
      self.ERROR('DB is not ready')
      return []
    goodRecs = []
    for f in fileRecs :
      if not FileChecker(self, self.stream).isOK(f) : continue
      goodRecs.append(f)
    return self.m_opsDB.addFiles(goodRecs, commit)

  def addJob(self, job, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.addJob(job, commit)

  def getAllConfigs(self, selection) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.getAllConfigs(selection)

  def getAllJobTypes(self, typeSelection = None) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.getAllJobTypes(typeSelection)

  def getAllFiles(self, selection = None, statusSelection = None, nMax = -1) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    files = self.m_opsDB.getAllFiles(selection, statusSelection, nMax)
    if statusSelection == None or not 'invalid' in statusSelection : return files
    files2 = []
    for f in files :
      if f.status == 'OK' : continue
      toInclude = False
      if f.status == 'RESERVED' :
        jobIds = self.getJobIdsForFiles([f])[f]
        if len(jobIds) == 0 : toInclude = True
      if f.status == 'INVALID' : toInclude = True
      if not toInclude : continue
      files2.append(f)
    return files2

  def getAllJobs(self, selection = None, typeSelection = None, statusSelection = None, nMax = -1, getJobFiles = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    jobs = self.m_opsDB.getAllJobs(selection, typeSelection, statusSelection, nMax, getJobFiles)
    if statusSelection == None or not 'invalid' in statusSelection : return jobs
    jobs2 = [] 
    for job in jobs : 
      if JobChecker(self, None).isOK(job) : continue
      jobs2.append(job)
    return jobs2

  def removeConfig(self, config, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    self.m_cfgDB.removeConfig(config.configId, simulate = simulate)

  def removeJobType(self, jobType, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.removeJobType(jobType.jobTypeId, simulate = simulate)
  
  def removeFile(self, f, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.removeFile(f.fileId, simulate = simulate)

  def removeJob(self, job, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    for f in job.outputs :
      self.TRACE('Processing ' + f.name)
      if not f or not FileChecker(self, self.stream).isOK(f) : 
        if f : 
          self.TRACE('Job File ' + f.name + ' is invalid, skipping')
        else :
          self.TRACE('An output file could not be retrieved - skipping')
        continue
      if not f.remove(self) : return False
    outputDir = job.outputDir()
    if outputDir : 
      acc = FileAccessor(outputDir)
      if acc.access() : acc.recursiveremove()
    acc = FileAccessor(job.tmpOutputDir)
    if acc.access() : acc.recursiveremove()
    return self.m_opsDB.removeJob(job.jobId, simulate = simulate)

  def getJob(self, jobId = None, jobName = None, withFiles = True, silent = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if jobName :
      jobId = self.m_opsDB.getJobId(jobName)
      if not jobId : 
        if silent : return None
        return self.ERROR('Problem recovering ID of job ' + jobName)
    return self.m_opsDB.getJob(jobId, withFiles = withFiles)
     
  def getJobId(self, jobName = None) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.getJobId(jobName)
     
  def getJobType(self, jobTypeId) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.getJobType(jobTypeId)
       
  def getJobTypeByName(self, jobTypeName) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.getJobTypeByName(jobTypeName)

  def getJobIdsForFiles(self, files) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.getJobIdsForFiles(files, checkOutput = True)

  def getUsedFiles(self, jobType) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.getUsedFiles(jobType.name)

  def getLastJobId(self) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.lastId('jobs')
    
  def getLastConfigId(self) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.lastId('configs')

  def getConfig(self, configId) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.getConfig(configId)
        
  def getJobWithBatchId(self, batchId) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.getJobWithBatchId(batchId)
  
  def getJobsWithStatus(self, status) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.getJobsWithStatus(status)
          
  def getConfigForJob(self, job) :
    jobType = self.getJobTypeByName(job.jobType)
    return self.getConfig(jobType.configId)

  def modifyJob(self, job, status = None, outputs = None, nEvents = None, cpuTime = None, properties = None, commit = True, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.modifyJob(job, status, outputs, nEvents, cpuTime, properties, commit, simulate)
  
  def modifyJobType(self, jobType, maxJobs = None, inputs = None, inputSelector = None, outputDir = None, transform = None, cronExec = None, configId = None, commit = True, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.modifyJobType(jobType, maxJobs, inputs, inputSelector, outputDir, transform, cronExec, configId, commit, simulate)

  def modifyConfig(self, config, releaseDir = None, defaultQueue = None, commit = True, simulate = False) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.modifyConfig(config, releaseDir, defaultQueue, commit, simulate)

  def queryFile(self, f, command, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.queryFile(f, command, commit)

  def queryJob(self, job, command, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_opsDB.queryJob(job, command, commit)

  def queryJobType(self, jobType, command, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if command == 'freeFiles' :
      inputSelector = JobInputSelectors.instance(jobType, self, self.stream)
      return '\n'.join([f.name for f in inputSelector.getFreeInputs()[0]])
    if command == 'allFiles' :
      inputSelector = JobInputSelectors.instance(jobType, self, self.stream)
      return '\n'.join([f.name for f in inputSelector.getFilesMatching(jobType.inputs.entries[0])])
    return self.m_cfgDB.queryJobType(jobType, command, commit)

  def queryConfig(self, config, command, commit = True) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_cfgDB.queryConfig(config, command, commit)

  def commit(self) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    try:
      self.m_cfgDB.commit('Changes could not be committed to config DB')
    except Exception, inst:
      self.m_cfgDB.rollback()
      traceback.print_exc()
      return self.ERROR('Changes could not be committed to config DB :' + str(inst))
    try:
      self.m_opsDB.commit('Changes could not be committed to operations DB')
    except Exception, inst:
      self.m_opsDB.rollback()
      traceback.print_exc()
      return self.ERROR('Changes could not be committed to operations DB :' + str(inst))
    
  def rollback(self) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    self.m_cfgDB.rollback()
    self.m_opsDB.rollback()
    
  def backupFile(self, path) :
    i = 1
    while os.access(path + '.' + str(i) + '.gz', os.F_OK) : i = i + 1
    newPath = path + '.' + str(i)
    self.TRACE('At ' +  time.asctime() + ' : backing up ' + path + ' to ' + newPath)
    os.system('cp %s %s && gzip %s' % (path, newPath, newPath))

  def backupFiles(self) :
    self.backupFile(self.m_cfgPath)
    self.backupFile(self.m_opsPath)
