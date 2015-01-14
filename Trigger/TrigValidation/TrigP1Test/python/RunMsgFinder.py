# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: RunMsgFinder.py
# @purpose: Find messages printed during running by algorithms
# @author: Frank Winklmeier (fwinkl)
# $Id: RunMsgFinder.py,v 1.6 2008-10-23 12:11:49 fwinkl Exp $

import sys, os, fnmatch, re

class RunMsgFinder:
    
    def __init__(self, paramDict={}):

        from Logger import Logger
        
        jDescriptor = paramDict.get('JobDescriptor',None)
        if jDescriptor:
            self.joblog = jDescriptor.log
        else:
            self.joblog = "test.log"
            
        self.success   =  0
        self.error     = -1
        self.logger    = Logger()

        self.logfile = paramDict.get('logfile', 'test.RunMsgFinder.log')

        # Ignore list
        ignore = ['.*EventLoopMgr\s*INFO\s*Change of HLT prescales requested.*',
                  '.*EventLoopMgr\s*INFO.*calling HLTConfigSvc::updatePrescaleSets.*',
                  'State is.*<f>orward, <b>ackward, <u>ser_command.*',
                  '.*WARNING Item .* required: 1 of:.*',
                  ]
        self.ignoreList = [re.compile(s) for s in ignore]
        self.ignoreFirstNEvents = 1
        
    def ignore(self, msg):
        """Return true if this message should be ignored"""
        
        for regexp in self.ignoreList:
            if regexp.match(msg): return True
        return False
    
    def extractRunMsg(self, file):
        """Extract the messages during running"""

        running = False
        event = 0
        runMsg = []
        ignoreMsg = []
        
        # Regular expression to extract event number
        evtRE = re.compile("\s*PerfMonSvc:\s*INFO \[evt:\s*([0-9]*)\].*")
        
        for line in file:
            if line.find('state is "PREPARED"')>-1:
                running = True
                event = 1
                continue
            
            if line.find('Psc stopRun started')>-1:
                running = False
                break
            
            if running:
                m = evtRE.match(line)
                if m and len(m.groups())==1: event = int(m.group(1)) + 1
                    
                if line.find('PerfMonSvc')==-1 and \
                       line.find('Py:athena')==-1:

                    if self.ignore(line) or event<=self.ignoreFirstNEvents:
                        ignoreMsg += [ "%-11s %s" % ("[evt #%i]" % event ,line) ]
                    else:
                        runMsg += [ "%-11s %s" % ("[evt #%i]" % event ,line) ]
                    continue
                            
        return (runMsg,ignoreMsg)
            
    def run(self):
        self.logger.debug('Running RunMsgFinder')

        self.logger.debug('Analyzing %s' % self.joblog)
        runMsg,ignoreMsg = self.extractRunMsg(open(self.joblog,'r'))

        if len(runMsg)>0: status = self.error
        else: status = self.success

        try:
            log = open(self.logfile,"w")
        except:
            self.logger.error("Cannot open %s" % self.logfile)
            
        log.write('='*80+'\n')
        log.write('RunMsgFinder: Found %d messages during RUNNING state\n' % len(runMsg))
        log.write('='*80+'\n')
        log.writelines(runMsg)

        log.write('\n'+'='*80+'\n')
        log.write('RunMsgFinder: Ignored %d messages\n' % len(ignoreMsg))
        log.write('='*80+'\n')
        log.writelines(ignoreMsg)
        log.close()
        
        return status

    
if __name__ == "__main__":

    sys.path.append('/afs/cern.ch/atlas/project/RTT/prod/Tools/RunTimeTester/src')
    sys.path.append('/afs/cern.ch/atlas/project/RTT/prod/Tools/RunTimeTester/share')    

    params = {}
    r = RunMsgFinder(params)
    status = r.run()
    
    for l in r.logger.all_lines: print l

    sys.exit(status)
    
