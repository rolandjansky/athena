# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author: N. Berger (Nicolas.Berger@cern.ch)
#
# Logger.py          
#   A simple class to format output messages

import os, sys, time

class OutputStream :
  def __init__(self, stdout = None, stderr = None) :
    if stdout != None : 
      self.stdout = stdout
    else :
      self.stdout = sys.stdout
    if stderr != None : 
      self.stderr = stderr
    else:
      self.stderr = sys.stderr

class FileStream :
  def __init__(self, fileName) :
    fileDir = os.path.split(fileName)[0]
    if not os.access(fileDir, os.F_OK) :
      try: os.makedirs(fileDir)
      except Exception, inst : 
        self.stdout.write('Could not make directory ' + fileDir + ' : ' + str(inst))
        return
    self.logFile = open(fileName, 'a', 0)
    self.stdout = self.logFile
    self.stderr = self.logFile
    
  def __del__(self) :
    self.logFile.close()

class NullStream :
  def __init__(self) :
    self.stdout = open('/dev/null', 'w')
    self.stderr = open('/dev/null', 'w')

class Logger :
  def __init__(self, stream = None) :
    if stream != None : 
      self.stream = stream
    else :
      self.stream = NullStream()

  def ERROR(self, message, val = False) :
    self.stream.stderr.write('ERROR(' + time.asctime() + ') ' + message + '\n')
    return val

  def TRACE(self, message, lineFeed = True) :
    self.stream.stderr.write('TRACE(' + time.asctime().split()[3] + ') ' + message + '\n')

  def INFO(self, message, lineFeed = True) :
    lf = ''
    if lineFeed : lf = '\n'
    self.stream.stdout.write(message + lf)

class FileLogger(Logger) :
  def __init__(self, fileName) :
    Logger.__init__(self, FileStream(fileName))
