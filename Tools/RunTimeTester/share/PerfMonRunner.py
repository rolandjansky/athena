from Logger import Logger
# from Factory_CmtLines import CmtLinesFactory
from ShellCommand import ShellCommand
import os
import os.path

class PerfMonRunner:
    def __init__(self, argDict):
        desc  = argDict.get('JobDescriptor')
        paths = desc.paths
        self.runPath  = desc.runPath
        self.fileName = argDict['fileName']
        self.options  = argDict.get('options', '')
        self.logger   = Logger()
        self.envSetupCmds = argDict['cmtLinesCmds']
        # self.envSetupCmds = CmtLinesFactory(self.logger).create(paths).lines()
        
    def run(self):
        cmds = []
        cmds.extend(self.envSetupCmds)
        cmds.append('cd %s;perfmon.py %s %s' % (self.runPath, self.options, self.fileName))
        
        # launch the commands
        sc    = ShellCommand(cmds, self.logger)            
        reply = sc.getRawReply()
        [self.logger.debug(str(l)) for l in reply]

        handle = None
        outFile = 'out.perfmon.stdout.txt'
        outPath = os.path.join(self.runPath, outFile)
            
        try:
            handle = open(outPath, 'w')
            handle.writelines(reply)
            handle.close()
            self.logger.debug('Successfully wrote out: %s' % outPath)
        except Exception, e:
            m  = 'Unable to output results of checkFile to file: %s\n' % outPath
            m += str(e)
            self.logger.error(m)
            if handle: handle.close()
            return 1
        
        return 0
