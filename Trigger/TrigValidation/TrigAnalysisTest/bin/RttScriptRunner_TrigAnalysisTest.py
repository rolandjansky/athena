# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: RttScriptRunner.py
# @purpose: Run any command in RTT post-processing
# @author: Frank Winklmeier (fwinkl)
# $Id: RttScriptRunner.py,v 1.3 2008-08-22 11:35:28 fwinkl Exp $
# need to make copy because of odd RTT python search

import sys
import os

class RttScriptRunner_TrigAnalysisTest:
    """
    Class to run any command within the RTT post-processing
    'command': command to run (including any arguments)
               %TESTNAME is replaced with the jobDisplayName (or test name if not set)
               %JOBLOG is replaced with the RTT job log file
               %BRANCH is replaces with the release branch
               $<ENVVAR> is replaced with the environment variable (if exists)
               
    'logfile': log file, if not given write to RTT logger
    """
    
    def __init__(self, paramDict={}):

        from Logger import Logger
        
        self.success   =  0
        self.error     = -1
        self.logger    = Logger()

        self.command = paramDict.get('command', None)
        self.logfile = paramDict.get('logfile', "test."+self.__class__.__name__+".log")

        jDescriptor = paramDict.get('JobDescriptor',None)
        if jDescriptor:
            testname = jDescriptor.jobDisplayName
            if testname=='': testname = jDescriptor.name
            self.command = self.command.replace("%TESTNAME",testname)
            self.command = self.command.replace("%JOBLOG",jDescriptor.log)
            self.command = self.command.replace("%BRANCH",jDescriptor.paths.branch)

            for k,v in os.environ.iteritems():
                self.command = self.command.replace("$"+k,v)
                
    def run(self):
        
        if not self.command:
            self.logger.error("'command' parameter not set")
            return self.error

        self.logger.debug("ScriptRunner: Executing '%s'" % self.command)

        # Run command and get output
        import subprocess as sp
        proc = sp.Popen(self.command, shell=True, stdout=sp.PIPE, stderr=sp.STDOUT)
        stdout = proc.communicate()[0]

        # Write output to log file or RTT logger
        if self.logfile:
            f = open(self.logfile,"w")
            f.write(stdout)
            f.close()
        else:
            for l in stdout.splitlines(True): self.logger.info(l.rstrip('\n'))
            
        if proc.returncode!=0: return self.error
        else: return self.success


# This is just for testing outside of the RTT
if __name__ == "__main__":

    sys.path.append('/afs/cern.ch/atlas/project/RTT/prod/Tools/RunTimeTester/src')
    sys.path.append('/afs/cern.ch/atlas/project/RTT/prod/Tools/RunTimeTester/share')    

    params = {'command' : 'ls -l',
              'logfile': 'rttscript.log'}
    
    r = RttScriptRunner_TrigAnalysisTest(params)
    status = r.run()
    
    for l in r.logger.debugL: print l,
    for l in r.logger.infoL: print l,
    for l in r.logger.warningL: print l,
    for l in r.logger.errorL: print l,

    sys.exit(status)
    
