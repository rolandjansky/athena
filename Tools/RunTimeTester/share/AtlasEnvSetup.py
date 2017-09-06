from Logger            import Logger
from ShellCommand import ShellCommand
# from Factory_CmtLinesForScripts import CmtLinesForScriptsFactory
import os
import os.path

class AtlasEnvSetup:
    def __init__(self, argDict):
        self.desc          = argDict['JobDescriptor']
        self.RTTShare      = self.desc.paths.RTTLibDir
        self.RTTSrc        = self.desc.paths.RTTSrcDir
        self.userScript    = argDict['userScript']
        self.userScriptLoc = argDict['userScriptLoc']

        self.logger = Logger()

        picklefile = os.path.join(self.desc.runPath, 'rtt.argdict.cpickle')


        # self.cmds = CmtLinesForScriptsFactory(self.logger).create(self.desc).lines()
        self.cmds = argDict['cmtLinesCmds']
        self.cmds.append('cd %s;python UserScriptRunner.py %s' % (self.desc.runPath, picklefile))

        __module__ = self.userScript+'.py'
        __name__ = self.userScript

    def run(self):
        self.createRunnerScript() # dump user runner script into runpath
        sc = ShellCommand(self.cmds, self.logger)
        self.logger.debug(sc.getRawReply())

    def createRunnerScript(self):
        def readTemplate():
            f = os.path.join(self.RTTShare, 'UserScriptRunnerTemplate.py')
            h = open(f)
            conts = h.read()
            h.close()
            return conts
        
        def outputFile(conts):
            f = os.path.join(self.desc.runPath, 'UserScriptRunner.py')
            h = open(f, 'w')
            h.write(conts)
            h.close()
            self.logger.debug('Output file: %s' % f)

        conts = readTemplate()
        conts = conts.replace('USER_MODULE_NAME', self.userScript)
        conts = conts.replace('%RTT_SHARE_DIR%', self.RTTShare)
        conts = conts.replace('%RTT_SRC_DIR%', self.RTTSrc)
        conts = conts.replace('%USER_MODULE_LOCATION%', self.userScriptLoc)
        outputFile(conts)
