from Logger import Logger
# from Factory_CmtLines import CmtLinesFactory
from ShellCommand import ShellCommand
import os
import os.path

class DiffPoolFilesRunner:
    def __init__(self, argDict):
        desc              = argDict.get('JobDescriptor')
        paths             = desc.paths
        self.runPath      = desc.runPath
        self.logger       = Logger()
        self.theFile      = argDict['fileName']
        self.theRefFile   = argDict['refFileName']        
        self.envSetupCmds = argDict['cmtLinesCmds']        
        # self.envSetupCmds = CmtLinesFactory(self.logger).create(paths).lines()
        
    def outcome(self, lines):
        diffPoolSuccessStr = '## Comparison : [OK]'
        for line in lines:
            if line.find(diffPoolSuccessStr)!=-1: return 0
        return 1

    def run(self):
        cmds = []
        cmds.extend(self.envSetupCmds)
        cmds.append('cd %s' % self.runPath)
        cmds.append('diffPoolFiles.py -f %s -r %s' % (self.theFile, self.theRefFile))

        # launch the commands
        sc    = ShellCommand(cmds, self.logger)            
        reply = sc.getRawReply()
        [self.logger.debug(str(l)) for l in reply]

        handle = None
        outFile = '%s__%s.diffPool' % (os.path.basename(self.theFile), os.path.basename(self.theRefFile))
        outPath = os.path.join(self.runPath, outFile)
            
        try:
            handle = open(outPath, 'w')
            handle.writelines(reply)
            handle.close()
            self.logger.debug('Successfully wrote out: %s' % outPath)
            return self.outcome(reply)
        except Exception, e:
            m  = 'Unable to output results of diffPoolFiles to file: %s\n' % outPath
            m += str(e)
            self.logger.error(m)
            if handle: handle.close()
            return 1
        
