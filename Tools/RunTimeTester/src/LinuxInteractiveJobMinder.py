# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from WorkerJobMinder import WorkerJobMinder
from RTTpath         import RTTpath
from RTTSException   import RTTCodingError

from popen2 import Popen4

# ==================================================================

class LinuxInteractiveJobMinder(WorkerJobMinder):
    
    def __init__ (self, argBag, jDescriptor):

        commander                 = argBag.commander
        argBag.submitCommand      = commander.submitCommand(argBag.runPath, argBag.log)
        argBag.postProcessCommand = commander.postProcessorCommand(argBag.runPath)


        WorkerJobMinder.__init__(self, argBag, jDescriptor)

        self.logger.debug( "initialising LinuxInteractiveJobMinder")

    def dump(self):
        self.logger.debug('|-------------------------------------------|')
        self.logger.debug('|                                           |')
        self.logger.debug('| Linux InteractiveJob Minder  dump         |')
        self.logger.debug('|                                           |')
        self.logger.debug('|-------------------------------------------|')

        self.logger.debug(self.__str__())

    def fire(self, what):
        """
        Submits the job by spawning a subprocess.
        The Popen objects captures std error, and std out
        'what' is a a key in to tthe self.scripts dictionary
        the popen object is bound to the object ot allow subsequent
        interogation. This is a specificity of interactive running.
        """

        cmd = self.scripts[what]['cmd']

        m =  'fire commandL\n: %s' % cmd
        self.logger.debug(m)

        self.popen  = Popen4(cmd)
        self.subLog = self.popen.fromchild.readlines()


    def jobIsRunning(self):
        queryOK   = True
        isRunning = False
            
        if self.popen.poll()==-1:
            isRunning = True
            print 'open.poll == -1'
            return (queryOK, isRunning)
            
        #if not RTTpath(self.runPath, self.log).exists():
        #    isRunning = True
        #    print 'log file %s does nto exist ' % self.log
        #    return (queryOK, isRunning)

        print 'running is done %s' % str(self.popen)

        return (queryOK, isRunning)

    def fromQueued(self):
        self.fire('runScript')
        return 'running'

    def fromRunning(self):
        queryOK, isRunning =  self.jobIsRunning()
        self.diskspace('fromRunning, start')
        if not queryOK:
            msg = 'Unsuccesful query of a job running interactively'
            raise RTTCodingError(msg)

        if isRunning:  return 'running' # no state transition

        self.fromRunningActions()
        
        if self.doPostProcessing():
            self.diskspace('Launching postprocessing')
            return 'postProcessingQueued'
        
        self.diskspace('Job done')
        return 'done'

    def fromRunningActions(self):
        self.checkLog()
        self.diskspace('Running checks')
        self.runChecks()
        # chainFileCopier present only if running in a chain 
        if self.chainFileCopier:
            self.chainFileCopier.copyFiles()

        # if keep file is also a chain file, this code  will silently fail. Intended that the chain
        # file copier is interogated to find new location
        self.copyKeepFiles()
        self.diskspace('After copyKeepFiles')


    def fromPostProcessingQueued(self):
        self.fire('postProcessorScript')
        return 'postProcessingRunning'
            
    def fromPostProcessingRunning(self):
        queryOK, isRunning =  self.jobIsRunning()

        if not queryOK:
            msg = 'Unsuccesful query of a job running interactively'
            raise RTTCodingError(msg)

        if isRunning:  return 'postProcessingRunning'

        self.readTestResults()
        self.copyKeepFiles()
        self.cleanSpace()

        return 'done'
        
