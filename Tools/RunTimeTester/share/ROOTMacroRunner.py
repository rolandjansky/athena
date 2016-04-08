from Logger            import Logger
import os, os.path
import os, sys

# from Factory_CmtLinesForScripts import CmtLinesForScriptsFactory

class ROOTMacroRunner:
    def __init__(self, argDict):
        
        jDescriptor       = argDict['JobDescriptor']
        self.srcDir = jDescriptor.paths.RTTSrcDir
        if self.srcDir not in sys.path: sys.path.append(self.srcDir)

        self.macroArg            = argDict.get('macroArg', None)
        self.macro               = argDict.get('macro', None)
        # self.logger            = argDict['logger']
        self.logger              = Logger()
        
        self.identifiedName      = jDescriptor.identifiedName
        self.runPath             = jDescriptor.runPath

        paths                    = jDescriptor.paths
        self.runType             = paths.runType
        self.distArea            = paths.distArea
        self.branch              = paths.branch
        self.release             = paths.release
        self.workDirs            = paths.workDirs

        self.nightlyPathFragment = ''
        if self.branch != 'Numbered':
            self.nightlyPathFragment = 'nightlies/%s' % self.branch

        self.fullPathToMacro     = os.path.join(jDescriptor.runPath, self.macro)
        self.envSetupCmds        = argDict['cmtLinesCmds']
        # self.envSetupCmds        = CmtLinesForScriptsFactory(self.logger).create(jDescriptor).lines()
        self.aclickTweakMacro    = os.path.join(paths.RTTLibDir, 'aclicTweak.C')

        try:
            compileTheMacro = argDict['compileTheMacro']
        except:
            compileTheMacro = False

        self.compileFlag = ''
        if compileTheMacro: self.compileFlag = '+'
        
        self.maxPrintLines = 500
        self.timeLimit     = 60*60 # secs

        self.rootCmds      = self.makeRootCommand()

        msg  = 'root command for job '
        msg += str(self.identifiedName)
        msg += ':\n' + str(self.rootCmds)
        self.logger.debug(msg)

    def macroExists(self):
        if not os.path.exists(self.fullPathToMacro):
            msg  = 'Path to ROOT macro does not exist: %s' % self.fullPathToMacro
            self.logger.error(msg)
            return False
        return True    
        
    def run(self):
        # check the macro exists. NOTE: this cannot be done in __init__ because
        # class instantiation occurs before copying of auxFilePatterns.
        if not self.macroExists():
            self.logger.warning("Root macro does not exit")
            return 1
        

        # note the current directory to reset it later
        currCwd = os.getcwd()

        # cmds = []
        # following cd must be part of the shell command has so other threads
        # cant mess it up
        # cmds.append('cd ' + self.runPath) 
        # cmds.extend(self.rootCmds)
        self.logger.info("Launching macro "+str(self.rootCmds))

        # to import, need the RTT src dir. tried to do the import in __init__.
        # Could import there, but ShellCommand was not visible here.
        from ShellCommand import ShellCommand
        sc    = ShellCommand(self.rootCmds,
                             self.logger,
                             self.maxPrintLines,
                             self.timeLimit)

        reply = sc.getReply()
        [self.logger.debug(str(l)) for l in reply]
        # reset the current directory as it was before
        os.chdir(currCwd)

        # reply is a list of strings, let's turn it into a single string
        replyString = reduce(lambda x,y: x+y+'\n', reply,'')

        # output reply to the logger
        name = str(self.identifiedName)
        msg  = 'root command output for job %s\n %s' % (name, replyString)
        self.logger.info(msg)

        # Also output reply to file 'ROOToutput.log' in the run path
        # This file is a keep file pattern in the TopLevelJobGroup.xml file.
        h = open(os.path.join(self.runPath,'ROOToutput.log'),'a+')
        h.write(replyString)
        h.close()

        return 0

    def makeRootCommand(self):
        rootCmds = self.envSetupCmds
        # rootCmds.append('source setup.sh >& /dev/null')
        # must cd to runpath just before root call (threading)
        rootCmds.append('cd %s' % self.runPath)

        if self.macro:
            # aclickTweak.C: a K.Ciba macro to allow 32 bit compilation in ROOT
            # on a 64 bit native platform
            line = 'root -b -q %s %s%s' % (self.aclickTweakMacro, self.macro, self.compileFlag)
            if self.macroArg:
                self.macroArg = self.macroArg.replace(' ', '\\ ')
                line += '(%s)' % self.macroArg
            line = self.escapeTheCommand(line)
            rootCmds.append(line)

        return rootCmds
     
    def escapeTheCommand(self, cmd):

        self.toBeEscaped = ['(',')','"']
        
        for char in self.toBeEscaped:
            pieces = cmd.split(char)
            
            cmd = pieces[0]
            for piece in pieces[1:]:
                cmd += '\\'+char+piece

        return cmd


