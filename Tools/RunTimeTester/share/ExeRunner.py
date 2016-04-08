######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in from    #
# more complicated classes in __init__               #
#                                                    #
######################################################

from Logger   import Logger
from commands import getstatusoutput
import os.path

class ExeRunner:    
    def __init__(self, paramDict={}):
        jDescriptor  = paramDict['JobDescriptor']
        self.exeName  = paramDict['exeName']
        self.exeParamString = paramDict.get('exeParamString','')
        self.logger  = Logger()
        self.runPath = jDescriptor.runPath

    def stdOutToFile(self, output, fName):
        handle = None
        outPath = os.path.join(self.runPath, fName)
        try:
            handle = open(outPath, 'w')
            handle.write(output)
            handle.close()
            self.logger.debug('Successfully wrote out: %s' % outPath)
        except Exception, e:
            m  = 'Unable to output results of ExeRunner %s to file: %s\n' % (self.exeName, outPath)
            m += str(e)
            self.logger.error(m)
            if handle: handle.close()

    def run(self):
        cmd = '%s %s' % (self.exeName, self.exeParamString)
        stat, output = getstatusoutput(cmd)
        header = 'RTT ran: %s\n\n' % cmd
        output = header + output
        {0:self.logger.debug}.get(stat, self.logger.error)(output)
        self.stdOutToFile(output, 'ExeRunner.%s.stdout' % self.exeName)
        return {0:0}.get(stat, 1)
