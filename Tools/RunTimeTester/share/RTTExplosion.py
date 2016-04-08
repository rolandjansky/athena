######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

import os
from Logger import Logger

import string, sys, traceback

def exc2string2():
    list   = sys.exc_info()
    errmsg  = str(list[0])
    errmsg += str(list[1])
    errmsg += string.join(traceback.format_tb(list[2]))
    return errmsg
                

class RTTExplosion:
    """An RTT test to run its own TestSuite under RTT control
    This test checks that classes can be instantitiated. It also
    creates the databass needed to run RTTRegression"""
    

    def __init__(self, argDict):


        self.success =  0
        self.error   = -1

        # self.logger = self.argDict['logger']
        self.logger = Logger()
        msg = 'Instantiating RTTExplosion, args: %s' %str(argDict)
        self.logger.debug(msg)
        jDescriptor = argDict['JobDescriptor']
        # fixtureDir is set in JobsXMLReader when reading in the config file.
        self.fixtureDir = argDict['fixtureDir']
        # the current directory
        self.runPath   =  jDescriptor.runPath
        self.logDir    =  jDescriptor.runPath
        # directory of the source code under test
        self.rttSrcDir =  os.path.join(self.runPath, 'Tools/RunTimeTester/src')
        fixtureDir    =  os.path.basename(self.fixtureDir)

    def run(self):

        # prepend the source directory undertest (must be prepended,
        # otherwise the RTT source used to run the job will be used
        # for importing!
        list = [self.rttSrcDir]
        list.extend(sys.path)
        sys.path = list
        self.logger.debug('sys.path = %s' % str(sys.path))
        ## horrible hack until I figure out how to get CMT to do this
        temp = ['/afs/cern.ch/atlas/offline/external/PyXML/0.8.4-2.3.4/PyXML-0.8.4-2.3.4']
        temp.extend(sys.path)
        sys.path = temp
        os.chdir(self.rttSrcDir)
        # sys.path.append(self.rttSrcDir)

        try:
            from TestSuiteRun2 import TestSuiteRun
            self.logger.debug('RTTExplosion: Import of TestSuiteRun succeeded')
        except Exception, e:
            self.logger.error('RTTExplosion: Import of TestSuiteRun failed')
            list    = sys.exc_info()
            errmsg  = str(list[0])
            errmsg += str(list[1])
            errmsg += string.join(traceback.format_tb(list[2]))
            self.logger.error(errmsg)
            return self.error

        try:
            tsr = TestSuiteRun(self.rttSrcDir, self.fixtureDir, self.logDir)
            self.logger.debug('RTTExplosion: TestSuiteRun execution complete')
        except Exception, e:
            m = 'RTTExplosion: TestSuiteRun execution failed \n%s' % exc2string2()
            self.logger.error(m)
            
            return self.error


        if tsr.passed:
            self.logger.debug('RTTExplosion Test passed')
            return self.success
        self.logger.debug('RTTExplosion Test failed')
        return self.error
        
