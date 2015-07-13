# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# LSFBackEnd.py
#   A job submission backend for CAFManager, in terms of the LSF system.

import os, re
from Logger import Logger
from DataClasses import FileRecord
from subprocess import Popen, PIPE
from Tools import joinFileRecords, getInputRunNumbers, getInputProjectTags
from LogReader import LogReader

#stageDirectory = 'os.getenv("WORKDIR")' # could also be '"/tmp/larcomm"'...

class LSFBackEnd(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor

  def linkFile(self, fileName, path) : # provide access for CAFMAnager tools fom the RunArgs dir
    cafMgrPath = os.getenv('CAFMGRPATH')
    os.system('cd ' + path + ' && ln -s ' + os.path.join(cafMgrPath, fileName))

  def makeRunArgs(self, job, config) :
    runArgsFile = 'runargs.py'
    # Provide links to needed files
    fullRunArgsFile = os.path.join(job.tmpOutputDir, runArgsFile)
    try: f = open(fullRunArgsFile, 'w')
    except Exception, inst:
      self.ERROR('Could not write to runargs file ' + runArgsFile + ': ' + str(inst))
      return None
    runNumbers = getInputRunNumbers(job.inputNames())
    runNumber = None
    if len(runNumbers) == 1 : runNumber = int(runNumbers[0])
    projectTags = getInputProjectTags(job.inputNames())
    projectTag = None
    if len(projectTags) == 1 : projectTag = projectTags[0]
    f.write('import os\n')
    f.write('CafJobId = "' + str(job.jobId) + '"\n')
    f.write('CafJobName = "' + str(job.name) + '"\n')
    f.write('CafJobType = "' + str(job.jobType) + '"\n')
    f.write("print 'Caf Job Parameters: ', CafJobId, CafJobName, CafJobType\n")
    f.write('CafJobInputs = ' + str(job.inputNames()) + '\n')
    f.write('CafJobOutputs = ' + str(job.tmpOutputNames()) + '\n')
    f.write("print 'CafJobInputs:'\n")
    f.write('for group in CafJobInputs :\n')
    f.write('  for f in group :\n')
    f.write("    print ' -> ' + f\n")
    f.write("print 'CafJobOutputs:'\n")
    f.write('for f in CafJobOutputs :\n')
    f.write("    print ' -> ' + f\n")
    f.write("CafRunNumbers = " + str(runNumbers) + "\n")
    f.write("CafRunNumber  = " + str(runNumber) + "\n")
    f.write("CafProjectTags = " + str(projectTags) + "\n")
    f.write("CafProjectTag  = '" + str(projectTag) + "'\n")
    f.write("print 'CafJobInputs:'\n")
    f.close()    
    return runArgsFile

  def submit(self, job, config, queue) :
    runArgsFile = self.makeRunArgs(job, config)
    if runArgsFile == None : return False
    self.linkFile('Tools.py', job.tmpOutputDir)
    self.linkFile('Logger.py', job.tmpOutputDir)
    self.linkFile('FileAccessor.py', job.tmpOutputDir)
    self.linkFile('cafMgr_athena', job.tmpOutputDir)
    if queue == '' : queue = config.defaultQueue
    jobType = self.m_data.getJobTypeByName(job.jobType)
    logFile = os.path.join(job.tmpOutputDir, 'job.log.gz')
    finalLogFile = os.path.join(job.outputDir(), 'job.log.gz')
    cmd = os.path.join(job.tmpOutputDir, 'cafMgr_athena') + ' ' \
      + config.releaseDir + ' ' \
      + runArgsFile + ' ' \
      + os.path.join(config.releaseDir, jobType.transform)
    batchCmd = 'bsub -q ' + queue + ' -J ' + job.name + ' -o ' + logFile + ' ' + cmd
    #batchCmd = 'bsub -q atlasb1 -J ' + job.name + ' -o ' + logFile + ' ' + cmd
    self.TRACE('LSF command : ' + batchCmd)
    try: result = Popen(batchCmd, shell=True, stdout=PIPE, stderr=PIPE).communicate()
    except Exception, inst:
      self.ERROR('Job submission failed with exception : ' + str(inst))
      return None
    self.TRACE('StdOut:')
    for l in result[0].split('\n') :
      self.TRACE(l)
    self.TRACE('StdErr: ' + result[1])
    l1 = result[0].split('>')
    if len(l1) == 0 : 
      self.ERROR('Error parsing LSF output (1) : ' + str(l1))
      return None
    l2 = l1[0].split('<')
    if len(l2) < 2 : 
      self.ERROR('Error parsing LSF output (2) : ' + str(l2))
      return None
    if not l2[1].isdigit() : 
      self.ERROR('Error parsing LSF output (2) : ' + str(l2[1]))
      return None
    self.TRACE('jobId = ' + l2[1])
    job.batchId = int(l2[1])
    reportedLogFile = FileRecord(finalLogFile, 'LOG', 'RESERVED')
    reportedRunArgs = FileRecord(os.path.join(job.outputDir(), 'runargs.py'), 'RUNARGS', 'RESERVED')
    return [ reportedRunArgs, reportedLogFile ]

  def abort(self, job) :
    os.system('bkill ' + str(job.batchId))

  def currentJobs(self, jobType = None) :
    s = os.popen('bjobs -u larcomm').read()
    ids = {}
    for line in s.split('\n') :
      fields = line.split()
      if len(fields) == 0 or not fields[0].isdigit() : continue      
      batchId = int(fields[0])
      job = self.m_data.getJobWithBatchId(batchId)
      if job == None : continue # can happen if someone else runs jobs as larcomm
      if jobType != None and job.jobType != jobType.name : continue
      if fields[2] == 'PEND' : ids[batchId] = 'SCHEDULED'
      if fields[2] == 'RUN'  : ids[batchId] = 'RUNNING'
    return ids

  def status(self, logFilePath) :
    logReader = LogReader(self.m_data, self.stream)
    errors = logReader.errors(logFilePath)
    if errors == None : return True
    return self.ERROR(errors)

  def nEvents(self, logFilePath) :
    logReader = LogReader(self.m_data, self.stream)
    return logReader.nEvents(logFilePath)

  def cpuTime(self, logFilePath) :
    logReader = LogReader(self.m_data, self.stream)
    return logReader.cpuTime(logFilePath)
