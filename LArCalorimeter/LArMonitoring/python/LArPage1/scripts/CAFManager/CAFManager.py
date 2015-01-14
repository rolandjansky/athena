# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# CAFManager:
#   The "hub" class for all higher-level commands 
#   (CAFManager operations and higher-level DB queries)


import os, time, traceback
from subprocess import Popen, PIPE
from Logger import Logger, OutputStream
from DataClasses import ConfigRecord, JobTypeRecord, FileRecord, JobRecord, FileSpec
from DataAccessor_SQLite import DataAccessor_SQLite
from FileAccessor import FileAccessor
from LSFBackEnd import LSFBackEnd
from LogReader import LogReader
from Checkers import ConfigChecker, JobTypeChecker, JobChecker, FileChecker
import JobInputSelectors
import JobNamers

class CAFManager(Logger) :

  def __init__(self, configFile = '', stream = OutputStream()) :
    Logger.__init__(self, stream)
    if not os.access(configFile, os.F_OK) :
      configFile = '.cafmgrconfig'
      if not os.access(configFile, os.F_OK) :
        configFile = os.path.join(os.getenv('HOME'), '.cafmgrconfig')
      if not os.access(configFile, os.F_OK) :
        configFile = os.path.join(os.getenv('CAFMGRPATH'), '.cafmgrconfig')

    self.m_data = DataAccessor_SQLite(configFile, self.stream)
    self.m_batch = LSFBackEnd(self.m_data, self.stream)

  def ok(self) : 
    return self.m_data.ok()

  def db(self) :
    return self.m_data

  def checkForNewFiles(self, typeSelection = None, simulate = False, noCatalog = False) :
    nOps = 0
    jobTypes = self.m_data.getAllJobTypes()
    for jobType in jobTypes :
      if typeSelection != None and not jobType.name in typeSelection : continue
      if jobType.maxJobs < 0 :
        self.TRACE('Skipping disabled jobType ' + jobType.name)
        continue
      if len(jobType.inputs.entries) == 0 :
        self.TRACE('Skipping jobType ' + jobType.name + ' with no inputs')
        continue
      inputSelector = JobInputSelectors.instance(jobType, self.m_data, self.stream)
      self.TRACE('Checking for new inputs for job ' + jobType.name)
      for i in jobType.inputs.entries :
        self.TRACE('  Checking input path ' + i.path)
        files = FileAccessor(i.path, self.stream, i.minimumAge, useCastorCatalog = not noCatalog).getFiles()
        self.TRACE('  Getting existing inputs for ' + i.path)
        existing = [ f.name for f in inputSelector.getFilesMatching(i) ]
        #files = FileAccessor(i.path, self.stream).getFiles()
        self.TRACE('  Determining which files are new (from ' + str(len(files)) + ' in catalog and ' + str(len(existing)) + ' already in DB)')
        setOfNewFiles = set(files) - set(existing)
        self.TRACE('  Building new file list...')
        newFiles = [f for f in files if f in setOfNewFiles ] # keeps newFiles in the same order as files
        self.TRACE('Found ' + str(len(newFiles)) + ' new files')
        self.TRACE('  Adding files to database...')
        filesToAdd = [FileRecord(f, i.fileType) for f in newFiles]
        if not simulate :
          added = self.m_data.addFiles(filesToAdd, False)
        else :
          added = filesToAdd
        for f in added : self.TRACE('   Added ' + str(f.name))
        nOps = nOps + len(added)
        self.INFO('Done with '  + jobType.name + ', added ' + str(len(added)) + ' files')
    return nOps        

  def submitJobs(self, typeSelection = None, queue = '', simulate=False, options = []) :
    nOps = 0
    jobTypes = self.m_data.getAllJobTypes()
    for jobType in jobTypes :
      if typeSelection != None and not jobType.name in typeSelection : continue
      if not JobTypeChecker(self.m_data, self.stream).isOK(jobType) : return False
      if jobType.maxJobs < 0 :
        self.TRACE('Skipping disabled jobType ' + jobType.name)
        continue
      nJobs = None
      if jobType.maxJobs > 0 : nJobs = len(self.m_batch.currentJobs(jobType))
      if nJobs >= jobType.maxJobs :
        self.TRACE('Number of current batch jobs (' + str(nJobs) + ') is at the limit for jobType ' + jobType.name + ' (' + str(jobType.maxJobs) + ') : will not submit more')
      inputSelector = JobInputSelectors.instance(jobType, self.m_data, self.stream)
      while nJobs < jobType.maxJobs :
        optStr = ''
        if options != [] : optStr += ', options = ' + str(options)
        self.TRACE('Trying to submit jobs for type ' + jobType.name + ' (currently running : ' + str(nJobs) + '/' + str(jobType.maxJobs) + ')' + optStr)
        if not self.submitJob(jobType, inputSelector, queue, simulate, options) : break
        if nJobs != None : 
          nJobs = nJobs + 1
          nOps = nOps + 1
    return nOps
        
  def submitJob(self, jobType, inputSelector, queue = '', simulate=False, options = []) :
    config = self.m_data.getConfig(jobType.configId)
    fullTransPath = os.path.join(config.releaseDir, jobType.transform)
    job = JobRecord(None, jobType.name, 'SCHEDULED', None, [])
    if not inputSelector.setInputs(job, options) : return False
    namer = eval('JobNamers.' + jobType.namingScheme)
    namer.configure(self.m_data, job, self.stream)
    self.TRACE('Naming job...')    
    outputDir = namer.setValues()
    if not outputDir : return False
    if simulate :
      self.INFO('Will submit job ' + job.name)
      job.jobId = -1
      job.batchId = -1
      flatInputs = []
      for group in job.inputs :
        for i in group :
          flatInputs.append(i)
      job.inputs = flatInputs
      job.dump(self, verbosity=1)
      return False # no point submitting more 
    self.TRACE('Creating ' + str(job.tmpOutputDir))
    try:
      try: os.makedirs(job.tmpOutputDir) # this is the recursive mkdir
      except Exception, inst : 
        traceback.print_exc()
        return self.ERROR('Could not make temporary output dir : ' + str(inst))
      self.TRACE('Submitting job ' + job.name + ' : ' + outputDir)
      for f in jobType.outputs.entries :
        fileRec = FileRecord(os.path.join(outputDir, f.path), f.fileType, 'RESERVED')
        if not self.m_data.addFile(fileRec, False) :
          try: os.system('rm -rf ' + job.tmpOutputDir)
          except Exception, inst : 
             traceback.print_exc()
             return self.ERROR('Could not remove temporary output dir : ' + str(inst))    
          self.m_data.rollback()
          return self.ERROR('Could not commit a job output file, aborting submission')
        job.outputs.append(fileRec)
      reportedFiles = self.m_batch.submit(job, config, queue)
      if job.batchId == None : raise OSError(99, 'Batch system failed to return a batch Id')
    except  Exception, inst :
      os.system('rm -rf ' + job.tmpOutputDir)
      self.m_data.rollback()
      traceback.print_exc()
      return  self.ERROR('Job submission failed : ' + str(inst) + ', removing temporary output dir ' + job.tmpOutputDir)
    abort = False
    for reportedFile in reportedFiles :      
      if not self.m_data.addFile(reportedFile, False) :
        abort = True
        self.ERROR('Could not add job file ' + reportedFile.name + ', aborting submission')
        break
      job.outputs.append(reportedFile)
    if not abort :
      if not self.m_data.addJob(job) : # commits if successful
        abort = True
        self.ERROR('Could not add job to DB - will abort it in batch and remove temporary output dir')
    if abort : 
      self.ERROR('Will now abort it in batch and remove temporary output dir')
      self.m_batch.abort(job)
      try: os.system('rm -rf ' + job.tmpOutputDir)
      except Exception, inst : 
        traceback.print_exc()
        self.ERROR('Could not remove temporary output dir : ' + str(inst))    
      self.m_data.rollback()
      return False
    return True
  
  def harvestJobs(self, extra = '') : # extra: run on more jobs than the regular SCHEDULED|RUNNING
    nOps = 0
    batchIds = self.m_batch.currentJobs()
    # First : check if some SCHEDULED jobs have started running
    for batchId in batchIds :
      job = self.m_data.getJobWithBatchId(batchId)
      if job.status == 'SCHEDULED' and batchIds[batchId] == 'RUNNING' :
        self.TRACE('Setting state of job ' + job.name + ' (batchId = ' + str(job.batchId) + ') to RUNNING')
        self.m_data.modifyJob(job, status = 'RUNNING')
        nOps = nOps + 1
    # Second : check if some RUNNING or SCHEDULED jobs have finished 
    jobs  = self.m_data.getJobsWithStatus('RUNNING')
    jobs.extend(self.m_data.getJobsWithStatus('SCHEDULED')) # in case jobs already finished before being moved from SCHEDULED->RUNNING
    for job in jobs : 
      if job.batchId in batchIds : continue 
      logFile = job.tmpOutputNames()[-1]
      if self.m_batch.status(logFile) :
        self.TRACE('Setting state of job ' + job.name + ' (batchId = ' + str(job.batchId) + ') to RUN_SUCCESS')
        if not self.m_data.modifyJob(job, status = 'RUN_SUCCESS') : return False
        nOps = nOps + 1
      else :
        self.TRACE('Setting state of job ' + job.name + ' (batchId = ' + str(job.batchId) + ') to RUN_FAILED')
        if not  self.m_data.modifyJob(job, status = 'RUN_FAILED') : return False
        nOps = nOps + 1
      nEvents = self.m_batch.nEvents(logFile)
      if not self.m_data.modifyJob(job, nEvents = nEvents) : return False
      self.TRACE('Setting job nEvents to ' + str(nEvents))
      cpuTime = self.m_batch.cpuTime(logFile)
      if not self.m_data.modifyJob(job, cpuTime = cpuTime) : return False
      self.TRACE('Setting job CPU time to ' + str(cpuTime))
    # Third : move output to permanent location
    jobs  = self.m_data.getJobsWithStatus('RUN_SUCCESS')
    jobs.extend(self.m_data.getJobsWithStatus('RUN_FAILED'))
    if extra.find('OUTPUT_ERROR') >= 0 : 
      jobs.extend(self.m_data.getJobsWithStatus('OUTPUT_ERROR'))
    for job in jobs :      
      self.TRACE('Moving output of ' + job.name)
      permDir = FileAccessor(job.outputDir(), self.stream)
      permDir.recursiveremove()
      for f in job.outputs :
        tmpFileName = os.path.join(job.tmpOutputDir, os.path.split(f.name)[1])
        fAcc = FileAccessor(tmpFileName, self.stream)
        zipIt = (f.fileType == 'LOGZIP')
        if not fAcc.moveToPermanent(job.outputDir(), self.m_data.getConfigForJob(job), zipIt=zipIt) :
          self.TRACE('Setting state of job ' + str(job.jobId) + ' (batchId = ' + str(job.batchId) + ') to OUTPUT_ERROR')
          self.m_data.modifyJob(job, status = 'OUTPUT_ERROR')
          nOps = nOps + 1
          return None
      if job.status == 'RUN_SUCCESS' or  job.status == 'OUTPUT_ERROR' : # the latter in case of a retry after initial failure to move output 
        self.TRACE('Setting state of job ' + str(job.jobId) + ' (batchId = ' + str(job.batchId) + ') to SUCCESS - output moved to permanent location')
        self.m_data.modifyJob(job, status = 'SUCCESS')
        nOps = nOps + 1
      if job.status == 'RUN_FAILED'  : 
        self.TRACE('Setting state of job ' + str(job.jobId) + ' (batchId = ' + str(job.batchId) + ') to FAILED - output moved to permanent location')
        self.m_data.modifyJob(job, status = 'FAILED')
        nOps = nOps + 1
      tmpDir = FileAccessor(job.tmpOutputDir, self.stream)
      tmpDir.recursiveremove()
    return nOps 
      
  def cronExec(self, typeSelection = None, simulate=False) :
    nOps = 0
    jobTypes = self.m_data.getAllJobTypes()
    for jobType in jobTypes :
      if typeSelection != None and not jobType.name in typeSelection : continue
      if not JobTypeChecker(self.m_data, self.stream).isOK(jobType) : return False
      if jobType.cronExec == '' : continue
      config = self.m_data.getConfig(jobType.configId)
      logFileRoot = jobType.name + '_' + time.strftime("%Y-%m-%d-%X", time.localtime()) + '.log'
      logFileName = os.path.join(config.tmpOutputDir, logFileRoot)
      self.TRACE('Running cronExec for jobType ' + jobType.name + ', output in ' + logFileName)
      cronCmd = 'cafMgr_exec ' + config.tmpOutputDir + ' ' + jobType.cronExec + ' ' + jobType.name + ' | tee ' + logFileName 
      cronProc = Popen(cronCmd, shell=True, stdout=PIPE, stderr=PIPE)
      result = cronProc.communicate()
      logFile = open(logFileName, 'w')
      logFile.write('stdout:\n')
      logFile.write(result[0])
      logFile.write('\n\nstderr:\n')
      logFile.write(result[1])
      logFile.write('\n\nreturn code: ' + str(cronProc.returncode) + '\n')
      logFile.close()
      permDir = jobType.outputDir + '/CronLogs'
      fAcc = FileAccessor(logFileName, self.stream)
      self.TRACE('Moving file ' + logFileName + ' to permanent location ' + permDir + '/' + logFileRoot)
      if not fAcc.moveToPermanent(permDir, config) :
        self.ERROR('Could not move file ' + logFileName + ' to permanent location.')
        return nOps
      fAcc.remove()
      nOps = nOps + 1
    return nOps

  def getLogFile(self, job, force = False, action = None, keep = False) :
    localName = './job_' + job.name + '.log.gz'
    local = FileAccessor(localName, self.stream)
    existsAlready = local.access()
    if force or not existsAlready :
      logFile = job.outputs[-1]
      l = FileAccessor(logFile.name, self.stream)
      self.TRACE('Staging log file ' + logFile.name)
      if not l.copy(localName) : 
        self.ERROR('Could not retrieve log file for ' + job.name)
        return False
    if action == 'test' :
      logReader = LogReader(self.m_data, self.stream)
      output = logReader.errors(localName)
    elif action == 'events' :
      logReader = LogReader(self.m_data, self.stream)
      output = logReader.nEvents(localName)
    elif action == 'cpuTime' :
      logReader = LogReader(self.m_data, self.stream)
      output = logReader.cpuTime(localName)
    else :
      output = None
    if not keep and not existsAlready : local.remove()
    return output

  def getOutputs(self, job, fileName, force = False) :
    localName = job.name + "-" + fileName
    local = FileAccessor(localName, self.stream)
    existsAlready = local.access()
    if not force and existsAlready : return self.INFO('File ' + fileName + ' already staged for job ' + job.name)
    for of in job.outputs :
      if os.path.split(of.name)[1] == fileName :
        self.TRACE('Staging output file ' + fileName + ' to ' + localName)         
        l = FileAccessor(of.name, self.stream)
        if not l.copy(localName) : 
          return self.ERROR('Could not retrieve output file ' + fileName + ' for ' + job.name)
        return True

    return self.ERROR('Invalid output file ' + fileName + ' for ' + job.name)

  def checkOutputs(self, job, simulate = False) :
    for o in job.outputs :
      l = FileAccessor(o.name, self.stream)
      if not l.access() :
        self.TRACE('File ' + o.name + ' is not accessible')
        if simulate :
          self.TRACE('Would set state of job ' + job.jobName + ' to OUTPUT_ERROR')
          continue
        self.TRACE('Setting state of job ' + job.jobName + ' to OUTPUT_ERROR')
        self.m_data.modifyJob(job, status = 'OUTPUT_ERROR')
      else :
        self.TRACE('File ' + o.name + ' is accessible')
    return True
