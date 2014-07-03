# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from ExeRunnerBase import ExeRunnerBase
from TestResult import TestResult
import os
import JetValidation

class CompareJetTestFiles(ExeRunnerBase):
  def __init__(self,argdict={}):
    testIdentifier = argdict.get('testIdentifierObj')
    self.refFile = argdict.get('ReferenceFile')
    self.newFile = argdict.get('NewFile')
    logFile = argdict.get('LogFile')
    #cmd = 'CompareJetTestFilesExec.py %s %s' % (self.refFile,self.newFile)
    #cmd = '/afs/cern.ch/user/s/sschramm/testarea/dev-RTT/InstallArea/python/JetValidation/CompareJetTestFilesExec.py %s %s' % (self.refFile,self.newFile)
    
    cmd = os.path.join( JetValidation.__path__[0], "CompareJetTestFilesExec.py") + " %s   %s" % (self.refFile,self.newFile)
    ExeRunnerBase.__init__(self,cmd,testIdentifier,logFile)
  
  def run(self):
    bad_refFile = not os.path.exists(self.refFile)
    bad_newFile = not os.path.exists(self.newFile)
    msg = ''
    msgTpl = '%s: file does not exist, not running'
    if bad_refFile:
      msg += msgTpl % self.refFile
    if bad_newFile:
      msg += msgTpl % self.newFile

    # Should we run this test?
    if msg: # This will turn up on the results web page for this job
      tr = TestResult(msg)
      self.testResults.append(tr)
      return
    
    # all ok - call the worker script.
    # You are free to call this as many times as you like.
    # For example, if your worker script was grepping an input file,
    # and you had many input file matches, you could loop over each file here
    # in the master script and for each file call ExeRunnerBase.run(). 
    # The advantage of this instead of passing the list of files to your worker
    # is that you would get on your job web page a test result for each input file. 
    ExeRunnerBase.run(self)
