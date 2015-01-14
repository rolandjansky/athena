# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# OperationsDB.py          
#  An SQLite implementation for the storage of job and file data.
#  Provides an interface to add, remove and query the stored data

from SQLite_Base import SQLite_Base
import sqlite3 as sqlite
import os, time
from DataClasses import FileRecord, JobRecord
from FileAccessor import FileAccessor
from Tools import RegexpList

class OperationsDB(SQLite_Base) : 

  def __init__(self, fileName, mode = '', isolation_level = '', stream = None) : # iso_lvl can be IMMEDIATE OR EXCLUSIVE
    SQLite_Base.__init__(self, fileName, mode, isolation_level, stream)

  def initialize(self) :
    self.execute('create table files (id integer primary key autoincrement, name text, type text, status text, creationtime text)')
    self.execute('create table jobs  (id integer primary key autoincrement, name text, type text, status text, tmp_output_dir text, creationtime text, batch_id integer, cpu_time integer, nEvents integer, properties text)')
    self.execute('create table inputs  (job_id integer, file_id integer)')
    self.execute('create table outputs (job_id integer, file_id integer)')
    return self.commit('Could not create operations DB')
         
  def makeFileRecord(self, data) :
    return FileRecord(data[1], data[2], data[3], data[4], data[0])

  def makeJobRecord(self, data, inputs = None, outputs = None) :
    return JobRecord(data[1], data[2], data[3], inputs, outputs, data[4], data[5], data[6], data[7], data[8], data[9], data[0])
    
  def makeFileData(self, fileRec) :
    return (fileRec.name, fileRec.fileType, fileRec.status, fileRec.creationTime)
  
  def makeJobData(self, jobRec) :
    return (jobRec.name, jobRec.jobType, jobRec.status, jobRec.tmpOutputDir, jobRec.creationTime, jobRec.batchId, jobRec.cpuTime, jobRec.nEvents, jobRec.properties)

  def addFile(self, fileRec, commit = True, noCheck = False) :
    if not noCheck :
      if len(self.getFileIDs([fileRec.name])) != 0 :
        return self.ERROR('File ' + fileRec.name + ' was already registered')
    fileRec.creationTime = time.asctime()
    self.execute('insert into files values(NULL, ?, ?, ?, ?)', self.makeFileData(fileRec))
    fileRec.fileId = self.lastId('files')
    if not commit : return True
    if not self.commit('New file could not be committed to DB') : return False
    return True

  def addFiles(self, fileRecs, commit = True) :
    for fileRec in fileRecs :
      fileRec.creationTime = time.asctime()
      self.execute('insert into files values(NULL, ?, ?, ?, ?)', self.makeFileData(fileRec))
      fileRec.fileId = self.lastId('files')
    if not commit : return fileRecs
    if not self.commit('New file could not be committed to DB') : return []
    return fileRecs

  def getFileIDs(self, fileNames) : # returns a list b
    format = ()
    for item in fileNames : format += ('?',)
    return self.convertToSimpleList(self.fetchall('select id from files where name in ' + str(format).replace("'",''), tuple(fileNames)))

  def getNotExisting(self, fileNames) : # returns a list b
    if len(fileNames) > 100 :
      result = set([])
      remaining = fileNames
      while len(remaining) > 0 :
        result = result.union(self.getNotExisting(remaining[:100]))
        remaining = remaining[100:]
      return result
    format = ()
    for item in fileNames : format += ('?',)
    existing = self.convertToSimpleList(self.fetchall('select name from files where name in ' + str(format).replace("'",''), tuple(fileNames)))
    s = set(fileNames)
    s2 = set(existing)
    return s.difference(s2)

  def getFileIdByName(self, fileName) :
    data = self.fetchall('select id from files where name=?', (fileName,))
    files = [ item[0] for item in data ]
    return files

  def getFile(self, fileId) :
    data = self.fetchone('select * from files where id=?', (fileId,))
    if data == None : 
      self.ERROR('Could not find file with ID ' + str(fileId))
      return FileRecord('', 'ANY', 'INVALID', None, fileId)
    return self.makeFileRecord(data)

  def getJobId(self, jobName) :
    data = self.fetchone('select id from jobs where name=?', (jobName,))
    if not data or len(data) == 0 :
      #self.ERROR('Could not find job with name ' + jobName)
      return None
    if len(data) > 1 : 
      #self.ERROR('Multiple jobs with name ' + jobName)
      return None
    return data[0]

  def getJob(self, jobId, withFiles = True) :
    data = self.fetchone('select * from jobs where id=?', (jobId,))
    if data == None : 
      self.ERROR('Could not find job with ID ' + str(jobId))
      return JobRecord('', 'INVALID', [], [], '', None, None, jobId)
    (inputs, outputs) = (None, None)
    if withFiles : (inputs, outputs) = self.getJobIO(jobId)
    return self.makeJobRecord(data, inputs, outputs)

  def getJobIO(self, jobId) :
    inputs = []
    inputIds = self.getInputFilesForJob(jobId)
    for inputId in inputIds : 
      inputs.append(self.getFile(inputId))
    outputs = []
    outputIds = self.getOutputFilesForJob(jobId)
    for outputId in outputIds :
      outputs.append(self.getFile(outputId))
    return (inputs, outputs)

  def addJob(self, job, commit = True) :
    job.creationTime = time.asctime()
    self.execute('insert into jobs values(NULL,?,?,?,?,?,?,?,?,?)', self.makeJobData(job))
    jobId = self.lastId('jobs')
    job.jobId = jobId
    for inputGroup in job.inputs : 
      for fileRec in inputGroup :
        self.execute('insert into inputs values(?, ?)', (jobId, fileRec.fileId))
    for fileRec in job.outputs :
      self.execute('insert into outputs values(?, ?)', (jobId, fileRec.fileId))
    if not commit : return True
    if not self.commit('New job could not be committed to DB') : return False
    return True

  def getJobIdsForFiles(self, files, checkOutput = False) :
    match = {}
    for fileRec in files :
      results1 = self.fetchall('select job_id from inputs where file_id=?', (fileRec.fileId,))
      if checkOutput : results2 = self.fetchall('select job_id from outputs where file_id=?', (fileRec.fileId,))
      resultsAsList = []
      for r in results1 : resultsAsList.append(r[0])
      if checkOutput : 
       for r in results2 : resultsAsList.append(r[0])
      match[fileRec] = resultsAsList
    return match 

  def getJobIdsForTypeName(self, typeName) :
    data = self.fetchall('select id from jobs where type=?', (typeName,))
    jobs = [ item[0] for item in data ]
    return jobs

  def getInputFilesForJob(self, jobId) :
    results = self.fetchall('select file_id from inputs where job_id=?', (jobId,))
    l = [ r[0] for r in results ]
    return l

  def getOutputFilesForJob(self, jobId) :
    results = self.fetchall('select file_id from outputs where job_id=?', (jobId,))
    l = [ r[0] for r in results ]
    return l

  def removeFile(self, fileId, checkJobs = True, commit = True, simulate = False) :
    if checkJobs :
      dummyFile = FileRecord(None, None, fileId = fileId)
      jobs = self.getJobIdsForFiles([ dummyFile ])
      if len(jobs[dummyFile]) > 0 : 
        return self.ERROR('Cannot remove file ' + str(fileId) + ' as it is referenced in jobs ' + str(jobs[dummyFile]))
    if simulate : 
      self.INFO('File ' + str(fileId) + ' will be removed')
      return True
    self.TRACE('Removing file ' + str(fileId))
    self.execute('delete from files where id=?', (fileId,))
    if not commit : return True
    return self.commit('Could not delete file')

  def removeJob(self, jobId, commit = True, simulate = False) :
    outputFiles = self.getOutputFilesForJob(jobId)
    if simulate : 
      self.INFO('Job ' + str(jobId) + ' will be removed')
      return True
    self.TRACE('Removing job ' + str(jobId))
    for f in outputFiles : 
      if not self.removeFile(f, False, False) : return False
    self.execute('delete from inputs  where job_id=?', (jobId,))
    self.execute('delete from outputs where job_id=?', (jobId,))          
    self.execute('delete from jobs where id=?', (jobId,))
    if not commit : return True
    return self.commit('Could not remove job ' + str(jobId))    

  def getAllFiles(self, selection = None, statusSelection = None, nMax = -1) :
    results = self.fetchall('select * from files')
    if selection :
      ids = selection['id']
      names = selection['name']
      if names : names = RegexpList(names, self.stream)
    fileTuples = []
    for r in results :
      if selection :
        if type(r[1]) != type(u'') : 
          self.ERROR('Invalid file record ' + str(r))
          continue
        if ids and not r[0] in ids : continue
        if names and not names.match(r[1]) : continue
      fileTuples.append(r)
    if nMax > 0 and len(fileTuples) > nMax : fileTuples = fileTuples[-nMax:]
    files = []
    for r in fileTuples :
      files.append(self.makeFileRecord(r))
    return files

  def getAllJobs(self, selection = None, typeSelection = None, statusSelection = None, nMax = -1, getJobFiles = False) :
    jobTuples = []
    results = self.fetchall('select * from jobs')
    if selection :
      ids = selection['id']
      names = selection['name']
      batchIds = selection['batchId']
      if names : names = RegexpList(names, self.stream)
    for r in results :
      if selection :
        if ids and not r[0] in ids : continue
        if names and not names.match(r[1]) : continue
        if batchIds and not r[6] in batchIds : continue
      if typeSelection != None and not r[2] in typeSelection : continue
      if statusSelection :
        if 'running' in statusSelection and r[3] != 'RUNNING' : continue
        if 'not running' in statusSelection and r[3] == 'RUNNING' : continue
        if 'scheduled' in statusSelection and r[3] != 'SCHEDULED' : continue
        if 'not scheduled' in statusSelection and r[3] == 'SCHEDULED' : continue
        if 'success' in statusSelection and r[3] != 'SUCCESS' : continue
        if 'not success' in statusSelection and r[3] == 'SUCCESS' : continue
        if 'failed' in statusSelection and r[3] != 'FAILED' and r[3] != 'RUN_FAILED' : continue
        if 'missing output' in statusSelection  :
          if r[3] == 'SCHEDULED' or r[3] == 'RUNNING' : continue
          (inputs, outputs) = self.getJobIO(r[0])
          keep = True
          for f in outputs :
            if not FileAccessor(f.name).access() :
              keep = False
              break
          if keep : continue
      jobTuples.append(r)
    if nMax > 0 and len(jobTuples) > nMax : jobTuples = jobTuples[-nMax:]
    jobs = []
    for r in jobTuples :
      if getJobFiles :
        (inputs, outputs) = self.getJobIO(r[0])
        jobs.append(self.makeJobRecord(r, inputs, outputs))
      else :
        jobs.append(self.makeJobRecord(r))
    return jobs

  def getLastJobId(self) :
    return self.lastId('jobs')
      
  def getJobWithBatchId(self, batchId) :
    data = self.fetchone('select id from jobs where batch_id=?', (batchId,))
    if not data : return None
    return self.getJob(data[0])
  
  def getJobsWithStatus(self, status) :
    data = self.fetchall('select id from jobs where status=?', (status,))
    jobs = [ self.getJob(item[0]) for item in data ]
    return jobs

  def modifyJob(self, job, status, outputs, nEvents, cpuTime, properties, commit = True, simulate = False) : # add more options if needed
    if status != None :
      if simulate :
        self.INFO('Status of ' + job.name + ' will be changed to ' + status)
        return True
      if not self.execute('UPDATE jobs SET status=? WHERE id=?', (status, job.jobId)) : return False
      if status == 'SUCCESS' :
        fileStatus = 'OK'
      else :
        fileStatus = 'INVALID'
      for f in job.outputs :
        if not self.execute('UPDATE files SET status=? WHERE id=?', (fileStatus, f.fileId)) : return False
      if not commit : return True
      if not self.commit('New job status could not be committed to DB') : return False
    if nEvents != None :
      if simulate :
        self.INFO('nEvents of ' + job.name + ' will be changed to ' + str(nEvents))
        return True
      if not self.execute('UPDATE jobs SET nEvents=? WHERE id=?', (nEvents, job.jobId)) : return False
      if not commit : return True
      if not self.commit('New job status could not be committed to DB') : return False
    if cpuTime != None :
      if simulate :
        self.INFO('cpu_time of ' + job.name + ' will be changed to ' + str(cpuTime))
        return True
      if not self.execute('UPDATE jobs SET cpu_time=? WHERE id=?', (cpuTime, job.jobId)) : return False
      if not commit : return True
      if not self.commit('New job status could not be committed to DB') : return False
    if properties != None :
      if simulate :
        self.INFO('properties of ' + job.name + ' will be changed to ' + str(properties))
        return True
      if not self.execute('UPDATE jobs SET properties=? WHERE id=?', (properties, job.jobId)) : return False
      if not commit : return True
      if not self.commit('New job properties could not be committed to DB') : return False
    if outputs != None :
      self.INFO('Output modification not enabled yet')
      pass
    return True

  def queryFile(self, f, command, commit = True) :
    if command == 'inputTo'  : 
      data = self.fetchall('select job_id from inputs where file_id=?', (f.fileId,))
      jobs = [ self.getJob(item[0], withFiles=False).name for item in data ]
      print 'jobs = ', jobs
      return ','.join(jobs)
    return None

  def queryJob(self, job, command, commit = True) :
    if command == 'status'  : return job.status
    if command == 'outputNames' : return ','.join([f.name for f in job.outputs])
    if command == 'inputNames'  : return ','.join([f.name for f in job.inputs])
    if command == 'properties'  : return job.properties
    return None

  def getUsedFiles(self, jobId) :
    data = self.fetchall('select file_id from inputs where exists (select id from jobs where id=job_id and type=?)', (jobId,))
    files = [ item[0] for item in data ]
    return files

