"""
Module contains tools[classes] that can be called
from out of the RTT through configuration files

"""

import os, logging, sys, imp, traceback
import os.path
sys.path.append(os.path.join(os.getcwd(), '../src'))
import fnmatch, string, re
from popen2 import popen4

from  popen2              import Popen4
from shellCommand         import shellCommand
#from siteSpecificsFactory import siteSpecificsFactory
from Factory_SiteSpecifics import SiteSpecificsFactory
from SiteSpecifics        import * 
from DBFileRetriever      import DBFileRetriever
from ShellCommand         import ShellCommand

import anydbm

# logger=logging.getLogger('rtt')
class Logger:
    def __init__(self):
        self.debugL    = []
        self.infoL     = []
        self.warningL  = []
        self.errorL    = []

    def debug(self, line):    self.debugL.append(line)
    def info(self, line):     self.infoL.append(line)
    def warning(self, line):  self.debugL.append(line)
    def error(self, line):    self.errorL.append(line)
    def critical(self, line): self.criticalL.append(line)

def importName(moduleName, name):
    """From Python Cookbook
       Import a named object from a module
    """
    try:
        module = __import__(moduleName, globals(), locals(), [name])
    except ImportError:
        logger.error('ImportName failed to import %s %s' % (moduleName, name))
        return None
    return vars(module)[name]

class FileComparator:

    def __init__(self, paramDict):

        self.diffFile        ="differ.out"
        self.testFile        = paramDict['testFile']
        self.refFile         = paramDict['refFile']
        self.diffFile        = paramDict['diffFile']
        self.refFileLocation = paramDict.get('refFileLocation')
        
        jDescriptor          = paramDict.get('jDescriptor')
        self.shareArea       = jDescriptor.paths.shareArea
        self.dbFileRetriever = DBFileRetriever(jDescriptor)

        # self.logger   = paramDict['logger']
        self.logger   = Logger()
        self.success  =  0
        self.error    = -1


    def run(self):
        
        cwd                  =  os.getcwd()

        if self.refFileLocation  == None:
            self.refFileLocation = 'rttDB'
            self.logger.warning("No ref file location specified. Will use default rttDB")
        
        testF = ""
        refF  = ""
        #reference files will be installed or will be in RTT DB
        try:
            testFile =""
            refFile  =""
            testFile = os.path.join(cwd,self.testFile)
            refFile  = self.getRefFile(self.refFileLocation)

            testF = open(testFile, "rb").read()
            refF  = open(refFile, "rb").read()
            
        except Exception, e:
            self.logger.error("Could not read files to compare ")
            self.logger.error(str(e))
            msg = 'File Comparator failed : ' +str(e)
            # jDescriptor.reporter.failure(msg)
            self.logger.error(msg)
            return self.error
        

        if testF !=refF:
            self.logger.debug("The following two files are different")
            self.logger.debug("File 1 :"+ str(testFile))
            self.logger.debug("File 2 :"+ str(refFile))
            diffCommand = "diff "+testFile +" "+refFile+" >"+self.diffFile
            runDiff =Popen4(diffCommand)
            runDiff.wait()
            
            msg =  'File Comparator failed. \n'
            msg += 'The following files are different : \n%s\n%s' % (
                str(testFile), str(refFile))
            # jDescriptor.reporter.failure(msg)
            self.logger.error(msg)
            return self.error   
        else:
            self.logger.info("The following two files are the same ")
            self.logger.info("File 1: "+str(testFile))
            self.logger.info("File 2: "+str(refFile))
            return self.success

    def getRefFile(self, location):

        if location == 'install':
            refFile = os.path.join(self.shareArea, self.refFile)
            return refFile
        elif location == 'rttDB':
            refFile = self.dbFileRetriever.writeRefFile(self.refFile)
            self.dbFileRetriever.storeFileInDB(self.testFile)
            return refFile
        else:
            self.logger.error("Ref File location unknown "+str(location))
            
        

    def usage(self):
        self.logger.info("Usage : FileComparator(<dict>)")
        self.logger.info("success ==> Files same; error ==> Files differ or exception")



class FileGrepper:
    
    def __init__(self, paramDict={}):

        self.success   =  0
        self.error     = -1
        # self.logger  = paramDict['logger']
        self.logger    = Logger()
        self.iFilePat  = str(paramDict.get('inputFile', None))
        self.outFile   = paramDict.get('outputFile', None)


        # the searchList and and vetoList entries in argDict are in fact
        # a single string with a comma acting as a separator (!!????)
        # undo this data structure
        # slightly tricky: if no entry in the dictionary, init to empty list
        # otherwise is a (comma seperated) string. Is so split into a list
        # the user can also pass the arguments as a true list
        # labeled as searchStrings and vetoStrings.
        self.searchList = paramDict.get('searchList', [])
        if self.searchList: self.searchList = self.searchList.split(',')
        self.searchList.extend(paramDict.get('searchStrings', []))

        self.vetoList = paramDict.get('vetoList', [])
        if self.vetoList: self.vetoList = self.vetoList.split(',')
        self.vetoList.extend(paramDict.get('vetoStrings', []))

        
        jDescriptor = paramDict['JobDescriptor']
        self.runPath = jDescriptor.runPath
        
    def getInFile(self):
        
        # if self.argDict.get('useJobLogFile', None):
        #    return  self.jDescriptor.log
            
        if not self.iFilePat:
            m = "FileGrepper: existing - no input file pattern was given"
            self.logger.error(m)
            return None            
        
        possFiles = [f for f in os.listdir(self.runPath)
                     if fnmatch.fnmatch(f, self.iFilePat)]
        if not possFiles:
            m = 'FileGrepper  No file matches the  in-file name pattern'
            m += ' %s' % (self.iFilePat)
            self.logger.warning(m)
            return None
            
        inFile = possFiles[0]
        if len(possFiles)>1:
            m = 'FileGrepper  More than one file matching in file name pattern'
            m += ' %s:  %s\n using %s' % (self.iFilePat, possFiles, inputFile)
            self.logger.warning(m)

        return inFile
    

    def run(self):


        inFile = self.getInFile()
        if not inFile: return self.error

        if not self.outFile:
            m = "FileGrepper: existing - no out file was given"
            self.logger.error(m)
            return self.error

        inFile  = os.path.join(self.runPath, inFile)
        self.outFile = os.path.join(self.runPath, self.outFile)


        
        print inFile

        lines = open(inFile, 'r').readlines()
        
        def lineWanted(line):
            for pattern in searchList:
                if re.search(pattern, line):
                    print 'match: pattern, line',pattern, line
                    return True
            return False
        
        lines = [l for l in lines if lineWanted(l)]

        def lineVetoed(line):
            for pattern in vetoList:
                if re.search(pattern, line):
                    print 'veto: pattern, line',pattern, line
                    return True
            return False
            
        lines = [l for l in lines if not lineVetoed(l)]

        outF = open(self.outFile, "w")
        [outF.write(l) for l in lines]
        outF.close()

        m  = 'FileGrepper:\n '
        m += 'input file: %s out file %s\n' % (inFile, self.outFile)
        m += 'Strings to match: %s\n' % str(searchList)
        m += 'Veto strings: %s\n' % str(vetoList)
        m += 'No of lines with a matches: %d' % len(lines)
        self.logger.debug(m)
        print m
        
        if lines: return self.success
        return self.error
    

class FilesWithStringsFinder:
    
    def __init__(self, paramDict):

        self.logger            = Logger()
        self.inputFilePatterns = paramDict.get('inputFilePatterns', [])
        self.searchPatterns    = paramDict.get('searchPatterns',    [])
        self.dirWithFiles      = paramDict.get('dirWithFiles', 'resultsDir')
        self.writeTxt          = paramDict.get('writeTxt', 'yes')
        self.writeHTML         = paramDict.get('writeHTML','yes')
        jDescriptor            = paramDict.get('JobDescriptor', None)
        self.runPath           = jDescriptor.runPath
        outputFile             = paramDict.get('outputFile','filesWithStrings')
        self.outputFile        = os.path.join(self.runPath,outputFile)
        self.resultsDirs       = jDescriptor.paths.resultsDirs
        self.writers           =  []


        dict = {
            'runPath':           self.runPath,
            'outputFile':        self.outputFile,
            'inputFilePatterns': self.inputFilePatterns,
            'searchPatterns':    self.searchPatterns,
            'logger':            self.logger
            }

        class Writer:
            def __init__(self, dict):
                self.ofile  = None
                self.outfn = os.path.join( dict['runPath'],
                                           dict['outputFile'])

                
                fpats = dict['inputFilePatterns'] 
                spats = dict['searchPatterns']

                header  = ['Files to match:']
                header.extend([('  %s ' % i ) for i in fpats])
                header.append(' ')
                header.append('\n\nStrings to match:')
                header.extend([('  %s ' % s ) for s in spats])
                header.append(' ')
                self.header = [self.terminateLine(line) for line in header]
                self.logger   = dict['logger']

            
            def openFile(self):
                """File is opened when there is a need to write (is not
                possible to open the file at contruction). On opening
                write out (once!) the header. This part of the action writing
                an item is common to all derived sclasses - they do the rest.
                """

                if self.ofile: return

                self.ofile = open(self.outfn, 'w')
                if not self.ofile:
                    msg = 'FilesEithStringsFinder: could not open file %s' % self.outfn
                    self.logger.error(msg)
                    return
                    
                [self.ofile.write(line) for line in self.header]

        class HTMLWriter(Writer):
            def __init__(self, dict):
                Writer.__init__(self, dict)
                self.outfn  += '.html'
                self.logger   = dict['logger']

        
                htmlHeader = []
                htmlHeader.append('<html>\n')
                htmlHeader.append('<head>\n')
                htmlHeader.append('   <title>FilesWithStringsFinder</title>\n')
                htmlHeader.append('</head>\n')
                htmlHeader.append('<body>\n')
                htmlHeader.extend(self.header)
                htmlHeader.append('<table>\n')
                self.header = htmlHeader

            def terminateLine(self, line):
                line+='<br>\n'
                return line

            def outputItem(self, item):

                if not self.ofile: return
                
                fn  = os.path.basename(item[0])
                href ='<a href="%s">  %s </a>' % (fn, fn)
                htext =  'contains "%s"'% (item[1])



                line  = '<tr>\n'
                line += '<td>'+href+'</td><td>'+htext+'</td>\n'
                line += '</tr>\n'
                
                self.ofile.write(line)

                
            def closeFile(self):
                if not self.ofile: return

                self.ofile.write('</table>\n')
                self.ofile.write('</body>\n')
                self.ofile.write('</html>\n')
                self.ofile.close()
            
        class TxtWriter(Writer):
            def __init__(self, dict):
                Writer.__init__(self, dict)
                self.outfn  += '.txt'
                self.logger   = dict['logger']
                    
            def terminateLine(self, line):
                line+='\n'
                return line


            def outputItem(self, item):
                # output to the html file needs to be htmlified
                # delay opening the files until now, otherwise there is
                # the danger the output directories have not been constructed.

                if not self.ofile: return

                width = 60
                fn = os.path.basename(item[0])
                line = 'file: %s contains "%s" \n' % (string.ljust(fn, width),
                                                      item[1]
                                                      )
                self.ofile.write(line)

            def closeFile(self):
                if not self.ofile: return
                self.ofile.close()
            
        if self.writeTxt == 'yes':
            self.writers.append(TxtWriter(dict))
        if self.writeHTML == 'yes':
            self.writers.append(HTMLWriter(dict))

        self.logger.debug('FilesWithStringsFinder Writers: %s' % (
            str(self.writers)))
    def run(self):

        files = self.filesInDirs()
        self.logger.debug('FilesWithStringsFinder: filesInDirs %s' % str(files))

        fPatterns     = self.inputFilePatterns
        filesToSearch = []
        
        [filesToSearch.extend(fnmatch.filter(files,
                                             ipfp)) for ipfp in fPatterns]

        strs         = self.searchPatterns

        self.logger.debug('FilesWithStringsFinder: files to search %s' % str(filesToSearch))
        self.logger.debug('FilesWithStringsFinder: searchPatterns %s' % str(strs))
        print 'searchpatterns',strs

        class StringInFile:
            def __init__(self, base, file):
                ifn = os.path.join(base,file)
                print 'checking file ',ifn,'... '
                self.contents = open(ifn, 'r').read()
            def check(self, string):
                print '    ... for string', string
                if self.contents.find(string) > 0: return 0
                return -1
            
        filesWithString = []
        for f in filesToSearch:
            sFile = StringInFile(dwf, f)
            [filesWithString.append((f,s))  for s in strs if sFile.check(s)]

        self.logger.debug('filesWithString %s' % str(filesWithString))

        [writer.openFile() for writer in self.writers]

        for writer in self.writers:
            [writer.outputItem(item) for item in filesWithString]

        [writer.closeFile() for writer in self.writers]

        return 0

    def filesInDirs(self):
        """
        return a list of files found using the descriptor Paths object scan
        the descriptor is passed as an argument, rather than getting
        it from the dictionary asthe descriptor passed may not be the local
        on: watchers pass the descriptors of their confederation minders, not
        their own descriptors.
        """

        msgHead ='FilesWithStringsFinder.filesInDirs: '

        logicalDir = self.dirWithFiles
        files = []
        
        self.logger.debug('%s files to search %s' % (msgHead, str(logicalDir)))

        if logicalDir == 'resultsDir':
            key = 'resBase/build/branch/runType/release/jobGroup'
            if key not in self.resultsDirs:
                self.logger.error('results directory not in Paths %s' % key)
                return files
            dwf = self.resultsDirs[key]
            self.logger.debug('FilesWithStringsFinder violfile directory is %s' % dwf)
            files = os.listdir(dwf)
            return files
        elif logicalDir == 'runDir':
            files = os.listdir(self.runPath)
            self.logger.debug('FilesWithStringsFinder violfile directory is %s' % self.runPath)
            return files
        else:
            msg = 'FilesWithStringFinder: Unknown directory to scan %s' % dwf
            self.logger.error(msg)
            return files

class WatcherFilesWithStringsFinder(FilesWithStringsFinder):
    def __init__(self, dict):
        FilesWithStringsFinder.__init__(self, dict)
        jDescriptor        = dict['JobDescriptor']
        self.confederation = jDescriptor.confederation
        
    def filesInDirs(self, jDescriptor):
        """return a list of files found using the minders of the Watcher's
        confederation of Worker jobs"""

        files = []
        for minder in self.confederation:
            files.extend(FilesWithStringsFinder.filesInDirs())

        return files

# ============ROOTMacroRunner===================
     
class ROOTMacroRunner:
    def __init__(self, argDict):
        
        self.macroArg     = argDict.get('macroArg', None)
        self.macro        = argDict.get('macro', None)
        # self.logger     = argDict['logger']
        self.logger       = Logger()
        
        jDescriptor       = argDict['JobDescriptor']
        self.runPath      = jDescriptor.runPath
        self.runType      = jDescriptor.paths.runType
        self.distBase     = jDescriptor.paths.hardCoded.distBase
        self.distArea     = jDescriptor.paths.distArea
        self.branch       = jDescriptor.paths.branch
        self.release      = jDescriptor.paths.release
        self.workDirs     = jDescriptor.paths.workDirs
        self.nightlyBranchFragments = jDescriptor.paths.hardCoded.nightlyPathFragments
        
        self.fullPathToMacro = os.path.join(self.jDescriptor.runPath,
                                            self.macro)


        try:
            compileTheMacro = argDict['compileTheMacro']
        except:
            compileTheMacro = False


        self.compileFlag = ''
        if compileTheMacro: self.compileFlag = '+'
        
        self.maxPrintLines = 500
        self.timeLimit     = 60*60 # secs
        self.rootCmd       = self.makeRootCommand()
        msg  = 'root command for job '
        msg += str(jDescriptor.identifiedName)
        msg += ':\n' + str(self.rootCmd)
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

        cmds = []
        # following cd must be part of the shell command has so other threads
        # cant mess it up
        cmds.append('cd ' + self.runPath) 
        cmds.extend(self.rootCmd)
        self.logger.info("Launching macro "+str(cmds))
        sc            = ShellCommand(cmds, self.maxPrintLines, self.timeLimit)
        reply         = sc.getReply()
        self.logger.debug(str(reply))
        # reset the current directory as it was before
        os.chdir(currCwd)

        # reply is a list of strings, let's turn it into a single string
        replyString = reduce(lambda x,y: x+y+'\n', reply,'')

        # output reply to the logger
        name = str(self.jDescriptor.identifiedName)
        msg  = 'root command output for job %s\n %s' % (name, replyString)
        self.logger.info(msg)

        # Also output reply to file 'ROOToutput.log' in the run path
        # This file is a keep file pattern in the TopLevelJobGroup.xml file.
        h = open(os.path.join(self.runPath,'ROOToutput.log'),'a+')
        h.write(replyString)
        h.close()

        return 0

    def ROOTenvSetupAndLaunch(self):        
        info = {}
        sendToDevNull = ' >& /dev/null'

        if self.runType=='kit':
            aDir = str(self.distBase)
            pathToLocalSetupScript =  os.path.join(str(self.distArea),
                                                   'cmtsite/setup.sh')             
        else:
            aDir = os.path.join(self.distBase,
                                self.nightlyPathFragment[self.branch])
                        
            pathToLocalSetupScript = os.path.join(self.workDirs['workBase/'],
                                                  'cmtHome/setup.sh')
                                    

        localScriptTags = ' -tag=opt,AtlasOffline,' + str(self.release)
        atlasOfflineRunTimeDir = os.path.join(aDir,'AtlasOffline',
                                              str(self.release),
                                              'AtlasOfflineRunTime/')        
        pathToAtlasOfflineRunTimeSetupScript = os.path.join(
            atlasOfflineRunTimeDir,'cmt/setup.sh')
        
        info['localSetupScriptCmd']   = 'source ' + pathToLocalSetupScript + localScriptTags + sendToDevNull
        info['releaseSetupScriptCmd'] = 'source ' + pathToAtlasOfflineRunTimeSetupScript + sendToDevNull
        info['launchRootCmd']         = 'root -b -q '
        return info
            
    def makeRootCommand(self):
        # returns a list of shell commands, untermintated by ';' to be added
        # to a list of commands to be sent to shellCmd

        cmds = self.ROOTenvSetupAndLaunch()

        cmdLines = []
        if self.macro:            
            cmdLines.append(cmds['localSetupScriptCmd'])
            cmdLines.append(cmds['releaseSetupScriptCmd'])
            line = cmds['launchRootCmd'] + self.macro
            if self.macroArg:
                line += self.compileFlag+'('+self.macroArg+')'
            line = self.escapeTheCommand(line)
            cmdLines.append(line)

        
        return cmdLines
    
    def escapeTheCommand(self, cmd):

        self.toBeEscaped = ['(',')','"']
        
        for char in self.toBeEscaped:
            pieces = cmd.split(char)
            
            cmd = pieces[0]
            for piece in pieces[1:]:
                cmd += '\\'+char+piece

        return cmd


# ============EchoFile===================

class EchoFile:
    def __init__(self, argDict):

        self.inFile   = argDict.get('datafile')
        # self.logger = params['logger']
        self.logger   = Logger()

    def run(self):
        inF  = open(self.inFile, 'r').readlines()
        outF = open('echoedFile.txt', 'w')

        for line in inF:
            outF.write(line)

        outF.close()
        

class FileToDict:
    def __init__(self, argDict):

        self.inFile   = argDict.get('datafile')
        self.logger   = Logger()
        # self.logger = params['logger']

    def run(self):
        self.DataDict      = self.makeDict(self.inFile)

        return self.DataDict
        

    def retrieveFile(self, key, db):
        pass
    
    
    def linesCleanUp(self, lines):
        ls =[]
        for line in lines:
            line =string.strip(line)
            if len(line)>0:
                ls.append(line)

        return ls


    def makeDict(self,file):
        lines         = open(file, 'r').readlines()
        lines         = self.linesCleanUp(lines)
        
        self.dict={}
        for item in lines:
            keyValuePairs     = string.split(item, ":")
            for m in  keyValuePairs:
                keyValuePairs = self.linesCleanUp(keyValuePairs)
            self.dict[keyValuePairs[0]] = int( keyValuePairs[1])
            
        return self.dict




# -------------------------------------------

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
            self.logger.error('RTTExplosion: TestSuiteRun execution failed')
            return self.error


        if tsr.passed:
            self.logger.debug('RTTExplosion Test passed')
            return self.success
        self.logger.debug('RTTExplosion Test failed')
        return self.error
        
        
# -------------------------------------------
class RTTRegression:
    """An RTT test to run its own TestSuite under RTT control
    This test checks that classes can be instantitiated. It also
    creates the databass needed to run RTTRegression"""
    

    def __init__(self, argDict):


        self.success =  0
        self.error   = -1

        # self.logger = argDict['logger']
        self.logger  = Logger()
        msg = 'Instantiating RTTRegression, args: %s' %str(argDict)
        self.logger.debug(msg)
        # fixtureDir is set in JobsXMLReader when reading in the config file.
        self.fixtureDir = argDict['fixtureDir']
        # the current directory
        jDescriptor    = argDict['JobDescriptor']
        self.runPath   =  jDescriptor.runPath
        self.logDir    =  jDescriptor.runPath
        # directory of the source code under test
        self.rttSrcDir =  os.path.join(self.runPath, 'Tools/RunTimeTester/src')
        self.runPath   =  jDescriptor.runPath
        fixture        =  os.path.basename(self.fixtureDir)
        self.dbName    =  os.path.join(self.runPath, fixture+'.db')
        self.refdbName =  os.path.join(self.runPath, 'refFile_'+fixture+'.db')
        # do not open the dir does not exist yet
        self.ofName    =  os.path.join(self.runPath, fixture+'_regression.log')

        
    def run(self):

        outFile = open(self.ofName, 'w')

        if not os.path.exists(self.dbName):
            msg = 'None existant path: %s' %  self.dbName
            self.logger.error(msg)
            outFile.write(msg+'\n')
            outFile.close()
            
            return self.error
        if not os.path.exists(self.refdbName):
            msg = 'None existant path: %s' %  self.refdbName
            self.logger.error(msg)
            outFile.write(msg+'\n')
            outFile.close()
            
            return self.error

        newDB = anydbm.open(self.dbName,    'r')
        oldDB = anydbm.open(self.refdbName, 'r')
        result = self.success
            
    
        onlyInNew = [k for k in newDB.keys() if k not in oldDB.keys()]

        text = 'Number of keys in reference db %d\n' % len(oldDB.keys())
        text = 'Number of keys in new       db %d\n' % len(newDB.keys())
        
        
        if onlyInNew:

            result = self.error
    
            text += '\n'
            text +='Reference - %s: date: %s\n' % (oldDB['fixtureDir'],
                                                   oldDB['date'])
            
            text +=  'New       - %s: date: %s\n' % (newDB['fixtureDir'],
                                                     newDB['date'])
            
            text += '\n'
            text +=  ' keys in new database, but not in old\n'
            text +=  str(onlyInNew)+'\n'
            text += '\n'
    
        onlyInOld = [k for k in oldDB.keys() if k not in newDB.keys()]
        if onlyInOld:

            result = self.error
        
            text +=  '\n'
            text +=  ' keys in old database, but not in new\n'
            text +=  str(onlyInOld)+'\n'
            text += '\n'
    
        keys = [k for k in oldDB.keys() if k in newDB.keys()]
        toRemove = ['fixtureDir', 'date']
        [keys.remove(k) for k in toRemove if k in keys]
        if keys:
            text +=  'differences:\n'
            text += '\n'

            for k in keys:
                
        
                if oldDB[k] != newDB[k]:

                    result = self.error
                    
                    text +=  'Key: %s\n' % k
                    text += '\n'
                    text +=  '   old:\n'
                    text +=  '   ' +str(oldDB[k])+'\n'
                    text += '\n'
                    text +=  '   new:\n'
                    text +=  '   '+str(newDB[k])+'\n'


        text += '\n'

        totTests = 0
        text +=  'Number of points examined:\n'
        for k in keys:
            line = ''
            line += k.ljust(30)
            ntestOld = len(oldDB[k].split(','))
            ntestNew = len(newDB[k].split(','))
            # assert(ntestOld == ntestNew)
            num = '%d' % ntestOld
            line += num.ljust(5)
            # print line
            totTests += ntestOld

        text +=  'No of test classes which pass: %d\n' % len(keys)
        text +=  'Total number of tests passed:  %d\n ' %totTests

        outFile.write(text)
        outFile.close()
        return result
        
        

# ============JunkTest===================

class JunkTest:
    def __init__(self, argDict):
        self.int = 1
        self.logger   = Logger()

    def run(self):
        # self.logger.error('JunkTest RTT test run method')
        print 'JunkTest RTT test run method'
        return 0
        

# -------------------------------------------
        
if __name__ == '__main__':
    open('junk.dat','w').write('aaaa\naaabb\naabbcccc\naabbddd\nzz\nyyzzzz\n')

    
    dict = {
        'inputFile': 'junk*',
        'outputFile': 'FileGrepper.log',
        # 'searchList': 'aa,bb',
        'vetoList': 'c,d',
        'searchStrings': ['zz', 'yyzzzz'],
        'vetoStrings':    ['y'],
        'logger': logger,
        'JobDescriptor': []
        }

    fg = FileGrepper(dict)
    print fg.run()
    
    
