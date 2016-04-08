from Logger import Logger
from ShellCommand import ShellCommand
import os
import os.path

class CheckFileRunner:
    def __init__(self, argDict):
        desc  = argDict.get('JobDescriptor')
        paths = desc.paths
        self.runPath = desc.runPath
        self.logger  = Logger()
        self.envSetupCmds = argDict['cmtLinesCmds']

    def checkFileStatus(self, lines):
        for line in lines:
            if line.find('RTT determined that checkFile exit status was:') != -1:
                return int(line.split(':')[1].strip())
        return 1
            
    def run(self):
        poolFiles = [f for f in os.listdir(self.runPath) if f.endswith('.pool.root')]

        statusCode = 0

        for poolFile in poolFiles:
            pathToPoolFile = os.path.join(self.runPath, poolFile)

            cmds = []
            cmds.extend(self.envSetupCmds)
            cmds.append('checkFile.py -f %s' % pathToPoolFile)
            cmds.append('echo "RTT determined that checkFile exit status was: " $?')

            # launch the commands
            sc    = ShellCommand(cmds, self.logger)            
            reply = sc.getRawReply()
            [self.logger.debug(str(l)) for l in reply]

            handle = None
            outFile = poolFile + '.checkFile'
            outPath = os.path.join(self.runPath, outFile)
            
            try:
                handle = open(outPath, 'w')
                handle.writelines(reply)
                handle.close()
                self.logger.debug('Successfully wrote out: %s' % outPath)
                status = self.checkFileStatus(reply)
                if status!=0:
                    self.logger.warning('CheckFile returned error, non-zero status (%d) for %s' % (status, poolFile))
                statusCode += status
            except Exception, e:
                m  = 'Unable to output results of checkFile to file: %s\n' % outPath
                m += str(e)
                self.logger.error(m)
                if handle: handle.close()
                return 1
        return statusCode
            


