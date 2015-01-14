# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# LogReader.py
#   A helper class to parse an athena/LSF log files for errors

import os, re
from Logger import Logger

class LogReader(Logger) :
  def __init__(self, dataAccessor, stream = None) :
    Logger.__init__(self, stream)
    self.m_data = dataAccessor

  def openFile(self, logFilePath) :
    if not os.access(logFilePath, os.R_OK) :
      self.ERROR('Log file ' + logFilePath + ' not readable')
      return None
    try : logFile = open(logFilePath)
    #try : output = os.popen('tac ' + logFilePath) # tac reverses the file
    except Exception, inst : 
      self.ERROR('Log file ' + logFilePath + ' not accessible : ' + str(inst))
      return None
    return logFile

  def errors(self, logFilePath) :
    logFile = self.openFile(logFilePath)
    if not logFile : return 'Log file ' + logFilePath + ' not accessible'
    matcher1 = re.compile('ByteStreamInputSvc *ERROR *fail to open file')
    matcher2 = re.compile('^RuntimeError')
    status = 0
    for line in logFile :
      if matcher1.match(line) : return 'Some input files could not be accessed - declaring job as failed'
      if matcher2.match(line) : return 'a runtime error occured - declaring job as failed'
      if line.startswith('Successfully completed') : status = status + 1
      if line.find('leaving with code 0: "successful run"') >= 0 : status = status + 2
      if line.startswith('Exited with error code') : return 'LSF exited with an error'
    if status == 0 : return 'Both athena and LSF exited with an error'
    if status == 1 : return 'athena exited with an error'
    if status == 2 : return 'LSF exited with an error'
    return None

  def nEvents(self, logFilePath) :
    logFile = self.openFile(logFilePath)
    if not logFile : return None
    matcher = re.compile('.*EventCounter:EventCounter::finalize - total events:.*')
    for line in logFile :
      if matcher.match(line) : 
        fields = line.split()
        if len(fields) < 7 or not fields[6].isdigit() : 
          self.ERROR('EventCounter::finalize output not in expected format!')
          return None
        return int(fields[6])
    return None

  def cpuTime(self, logFilePath) :
    logFile = self.openFile(logFilePath)
    if not logFile : return None
    matcher = re.compile(' *CPU time   : *.* sec.*')
#    matcher = re.compile('.*CPU time.*')
    for line in logFile :
      if matcher.match(line) : 
        fields = line.split()
        try :
          result = int(float(fields[3]))
        except Exception, inst :
          self.ERROR('Error while retrieving CPU time : ' + str(inst) + ' (input was ' + line + ')')
          return None
        return result
    return None

