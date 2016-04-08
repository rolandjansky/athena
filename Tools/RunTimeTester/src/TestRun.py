# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
TestRun subclasses threading.Thread to allow concurrent running of instances.


Does roughly the following:

1. Reads the RTT configuration file, which is used to set up a large number
   of paths and other variables

2. Uses factory method to set up a Commander (mode specific commands)

3. Deletes existing  run directories

4. Reads the Jobs configuration file - this sets up a list of Job Descriptors
   (one JobDescriptor per job)
   JobDescriptors contain info abouta job. They also create a run directory for
   thir job.

5. Submit the jobs. One JobMinder per job is created. JobMinders incoporate the
   JobDescriptor, and then monitor the progress of the job.class 

6. A poll command is periodically sent to the JobMinders. These check their job status.
   This status is interogated, and html pages are updated with new status info.

7. After all jobs have completed perform cleanup activities
"""
#----- RTT modules -----


from RTTpath                  import RTTpath
from MethodTimer              import MethodTimer
from RTTCloseDownKeyMaker     import dictToDocument
from makeRTTDOM               import makeRTTDOM
import threading
import os
from MethodTracer        import MetaTracer
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------


class TestRun(threading.Thread, MethodTimer):
    def __init__(self,
                 jobMinders,
                 packageName,
                 closeDownKey,
                 closeDownKeyMaker,
                 logger,
                 statusText,
                 vetoFlag = False):

        MethodTimer.__init__(self)

        self.logger            = logger
        self.closeDownKeyMaker = closeDownKeyMaker
        self.name              = packageName 
        self.xmlConverter      = None # set later
        self.statusText        = statusText
        self.vetoFlag          = vetoFlag
        
        m = self.__class__.__name__+' '+self.name+' ' + '__init__'
        self.logger.debug(m)

        self.emailStats        = {}
        self.jobMinders        = jobMinders
        self.phase             = 'Initial'

        self.nActiveJobs = 0
        self.doneJobs       = 0
        self.successfulJobs = 0


        self.nActiveJobs    = len(self.jobMinders)
        self.totalJobs      = len(self.jobMinders)
        self.closeDownKey   = closeDownKey
        # For convenience, a copy of the key to be used to close down this
        # job will be output to the run directory. To make the xml file
        # start by creating a document. The output is done elsewhere.
        self.closeDownDom = dictToDocument(self.closeDownKey.__dict__, makeRTTDOM('RTTCloseDownKey'))

        self.logger.info('TestRun %d active jobs' % self.nActiveJobs)

        self.removeBadMinders()
        self.logger.info('TestRun made run directories')

        self.logger.info('TestRun initialising thread')
        threading.Thread.__init__(self)

# ------------------------------------------------------------------------

    def operatorCloseDownTest(self):
        return self.closeDownKeyMaker.laterMatch(self.closeDownKey.__dict__)

# -------------------------------------------------------------------------

    def removeBadMinders(self):
        """Override in derived classes."""
        pass
    
#-------------------------------------------------------------------------

    #  clean up operations on html pages: parent class placeholder
    def closeDown(self):
        pass    
                
# -------------------------------------------------------------------------

    def run(self): pass

# -------------------------------------------------------------------------

    def __str__(self):
        text = 'TestRun: no of minders: '+str(len(self.jobMinders))+'\n'
        for job in self.jobMinders:
            text += str(job)
            text += '\n'

        return text

# -------------------------------------------------------------------------

    def printMethodTimes(self):
        m = ' -- %s --\n%s\n-- xml converter--\n%s' % (
            self.name,
            self.formatMethodTimes(),
            self.xmlConverter.formatMethodTimes()
            )
        self.logger.debug(m)

    def dumpMinders(self):
        [m.dump() for m in self.jobMinders]


    def dataForMonitoring(self):
        dict = {
            'name': self.name,
            'nSucc': self.successfulJobs,
            'nDone': self.doneJobs,
            'nJobs': self.totalJobs
            }
        return dict

    def setXMLConverter(self, xmlConverter):

        self.xmlConverter = xmlConverter
        self.xmlConverter.setTestRun(self)
        
        # now pass to all job minders        
        [m.setSummaryCallBack(self.xmlConverter) for m in self.jobMinders]
        
