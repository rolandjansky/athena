# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
# 
# ConfigDB.py
#  An SQLite implementation for the storage of jobType and jobConfig data.
#  Provides an interface to add, remove and query the stored data

from SQLite_Base import SQLite_Base
import sqlite3 as sqlite
import os
from DataClasses import ConfigRecord, JobTypeRecord, FileSpecs

class ConfigDB(SQLite_Base) : 

  def __init__(self, fileName, mode = '', isolation_level = '', stream = None) : # iso_lvl can be IMMEDIATE OR EXCLUSIVE
    SQLite_Base.__init__(self, fileName, mode, isolation_level, stream)

  def initialize(self) :
    self.execute('create table configs  (id integer primary key autoincrement, release_dir text, batch_scheme text, default_queue text, tmp_output text)')
    self.execute('create table jobtypes (id integer primary key autoincrement, name text, transform text, inputs text, outputdir text,  outputFiles text, input_selector text, naming_scheme text, config_id integer, max_jobs integer, cron_exec text)')
    return self.commit('Could not create config DB')
  
  def makeConfigRecord(self, data) :
    return ConfigRecord(data[1], data[2], data[3], data[4], data[0])

  def makeJobTypeRecord(self, data) :
    inputs = FileSpecs(stream = self.stream)
    outputs = FileSpecs(stream = self.stream)
    if data[3] != '' : inputs.load(data[3])
    if data[5] != '' : outputs.load(data[5])
    return JobTypeRecord(data[1], data[2], inputs, data[4], outputs, data[6], data[7], data[8], data[9], data[10], data[0])

  def makeConfigData(self, config) :
    return (config.releaseDir, config.batchScheme, config.defaultQueue, config.tmpOutputDir)

  def makeJobTypeData(self, jobType) :
    return (jobType.name, jobType.transform, str(jobType.inputs), 
            jobType.outputDir, str(jobType.outputs), jobType.inputSelector, jobType.namingScheme, 
            jobType.configId, jobType.maxJobs, jobType.cronExec)
    
  def addConfig(self, config, commit = True, simulate = False) :
    if simulate :
      self.INFO('Will add config:')
      config.dump(self.stream, verbosity = 1)
      return True
    self.execute('insert into configs values(NULL, ?, ?, ?, ?)', self.makeConfigData(config))
    if not commit : return True
    if not self.commit('New config could not be committed to DB') : return False
    config.configId = self.lastId('configs')
    return True

  def addJobType(self, jobType, commit = True, simulate = False) :
    if simulate :
      self.INFO('Will add job type:')
      jobType.dump(self.stream, verbosity = 1)
      return True
    self.execute('insert into jobtypes values(NULL,?,?,?,?,?,?,?,?,?,?)', self.makeJobTypeData(jobType))
    if not commit : return True
    if not self.commit('New job type could not be committed to DB') : return False
    jobType.jobTypeId = self.lastId('jobTypes')
    return True

  def getConfig(self, configId) :
    data = self.fetchone('select * from configs where id=?', (configId,))
    if data == None or len(data) == 0 : return None
    return self.makeConfigRecord(data)

  def getJobType(self, jobTypeId) :
    data = self.fetchone('select * from jobtypes where id=?', (jobTypeId,))
    if data == None or len(data) == 0 : return None
    return self.makeJobTypeRecord(data)
 
  def getJobTypeByName(self, jobTypeName) :
    data = self.fetchone('select * from jobtypes where name=?', (jobTypeName,))
    if data == None or len(data) == 0 : return None
    return self.makeJobTypeRecord(data)

  def removeConfig(self, configId, simulate = False) :
    if simulate : 
      self.TRACE('Config ' + configId + ' will be removed')
      return True
    self.TRACE('Removing config ' + str(configId))
    self.execute('delete from configs where id=?', (configId,))
    self.commit('Could not remove config')

  def removeJobType(self, jobTypeId, simulate = False) :
    if simulate : 
      self.TRACE('Job type ' + jobTypeId + ' will be removed')
      return True
    self.TRACE('Removing job Type ' + str(jobTypeId))
    self.execute('delete from jobtypes where id=?', (jobTypeId,))
    self.commit('Could not remove job type')

  def getAllJobTypes(self, typeSelection) :
    results = []
    data = self.fetchall('select * from jobtypes')
    for datum in data :
      if typeSelection != None : 
        if not datum[1] in typeSelection : continue
      results.append(self.makeJobTypeRecord(datum))
    return results

  def getAllConfigs(self, selection) :
    results = []
    data = self.fetchall('select * from configs')
    for datum in data :
      if selection['id'] != None :
        if not datum[0] in selection['id'] : continue
      results.append(self.makeConfigRecord(datum))
    return results
    
  def modifyJobType(self, jobType, maxJobs = None, inputs = None, inputSelector = None, outputDir = None, transform = None, cronExec = None, configId = None, commit = True, simulate = False) : # add more options ?
    if maxJobs != None :
      if simulate :
        self.INFO(jobType.name + ' : maxJobs will be changed to ' + maxJobs)
        return True
      self.execute('UPDATE jobtypes SET max_jobs=? WHERE id=?', (maxJobs, jobType.jobTypeId))
    if inputs != None :
      inputSpec = FileSpecs(stream = self.stream)
      if inputSpec.load(inputs) : # this ensures the format is OK
        if simulate :
          self.INFO(jobType.name + ' : inputs will be changed to ' + str(inputSpec))
          return True
        self.execute('UPDATE jobtypes SET inputs=? WHERE id=?', (str(inputSpec), jobType.jobTypeId))
    if inputSelector != None :
        self.execute('UPDATE jobtypes SET input_selector=? WHERE id=?', (inputSelector, jobType.jobTypeId))
    if outputDir != None :
        self.execute('UPDATE jobtypes SET outputdir=? WHERE id=?', (outputDir, jobType.jobTypeId))
    if transform != None :
        self.execute('UPDATE jobtypes SET transform=? WHERE id=?', (transform, jobType.jobTypeId))
    if cronExec != None :
        self.execute('UPDATE jobtypes SET cron_exec=? WHERE id=?', (cronExec, jobType.jobTypeId))
    if configId != None :
        self.execute('UPDATE jobtypes SET config_id=? WHERE id=?', (configId, jobType.jobTypeId))
    if not commit : return True
    if not self.commit('New job type parameters could not be committed to DB') : return False
    return True

  def modifyConfig(self, config, releaseDir = None, defaultQueue = None, commit = True, simulate = False) : # add more options ?
    if releaseDir != None :
        self.execute('UPDATE configs SET release_dir=? WHERE id=?', (releaseDir, config.configId))
    if defaultQueue != None :
        self.execute('UPDATE configs SET default_queue=? WHERE id=?', (defaultQueue, config.configId))
    if not commit : return True
    if not self.commit('New config parameters could not be committed to DB') : return False
    return True

  def queryJobType(self, jobType, command, commit = True) :
    if command == 'maxJobs' : return jobType.maxJobs
    if command == 'inputs'  : return str(jobType.inputs)
    if command == 'inputSelector'  : return str(jobType.inputSelector)
    if command == 'outputDir'  : return str(jobType.outputDir)
    if command == 'transform'  : return str(jobType.transform)
    if command == 'cronExec'  : return str(jobType.cronExec)
    if command == 'configId'  : return str(jobType.configId)
    return None

  def queryConfig(self, config, command, commit = True) :
    if command == 'releaseDir' : return config.releaseDir
    if command == 'defaultQueue' : return config.defaultQueue
    return None
