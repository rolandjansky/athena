# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


__doc__ = """A base class for pre-staging files from tape to disk. Specific tape staging
systems need to derive from this class and implement member functions
getFilesStatus(), preStageFiles()."""

import time,re,os
import rfio
from PyJobTransformsCore.TransformLogger import TransformLogger,logging


class FilePreStageError(IOError):
    """exception thrown in the case of problems"""
    def __init__(self,*vargs):
        IOError.__init__(self,*vargs)


class FilePreStager(TransformLogger):
    STAGED      = 'On disk'          # file is on disk
    STAGEIN     = 'Being staged from tape to disk'     # file is in the process of being put on disk
    ONTAPE      = 'Not on disk (but on tape)'   # file is not on disk
    NOTONTAPE   = 'Not on tape'      # file is not on tape
    INVALID     = 'Invalid'          # invalid status
    UNKNOWN     = 'Unknown status'   # unknown status
    CANBEMIGR   = 'Waiting for tape migration (on the stager disk)' 
    # list of all possible file status
    fileStatus = ( STAGED, STAGEIN, ONTAPE, NOTONTAPE, INVALID, UNKNOWN, CANBEMIGR)
    # list of problematic file status
    problemStatus = ( NOTONTAPE, INVALID, UNKNOWN )

    def __init__(self,filenamePattern,maxWaitingTime,pollingInterval,name=None):
        """<filenamePattern>: regular expression to match the filenames that are in this stager.
        <pollingInterval>: default polling interval (seconds) to check file status when files are being staged from tape to disk.
        <maxWaitingTime>: default maximum time (seconds) to wait for all files to be staged.
        <name>: name by which this stager will be known"""
        global theFileStagerRobot
        if not name: name = self.__class__.__name__
        TransformLogger.__init__(self,name)
        self.__name = name
        self.__filesToPreStage = {} # map with (key,value)=(filename,status)
        self.setFilenamePattern( filenamePattern )
        self.__maxWaitingTime = maxWaitingTime
        self.__pollingInterval = pollingInterval
        # list of files to pre-stage and their last status (key=filename,value=status)
        theFileStagerRobot.addStager( self )
        

    def _notImplemented(self,funcName):
        raise FilePreStageError('%s has not implemented function %s' % (self.__class__.__name__, funcName) )

    
    def getFilesStatus(self,listOfFiles):
        """Return a dictionary containing the status of each file in <listOfFiles>.
        The key is the filename and the value is the status (one of FilePreStager.fileStatus).
        <listOfFiles> is either a python list (or tuple) with filenames or a single filename.
        If <listOfFiles> is empty, return an empty dictionary (and do nothing else).
        Must be implemented in derived class."""
        self._notImplemented(FilePreStager.getFilesStatus.__name__)


    def preStageFiles(self,listOfFile):
        """Initiate staging of files from tape to disk. Function must return immediately
        and not wait until the files are staged. Raise a FilePreStageError exception in case of errors.
        <listOfFiles> is either a python list (or tuple) with filenames or a single filename.
        If <listOfFiles> is empty, do nothing.
        Must be implemented in derived class."""
        self._notImplemented(FilePreStager.preStageFiles.__name__)


    def name(self):
        return self.__name
    

    def fileExists(self,filename):
        """Check that the file exists in the tape system"""
        return rfio.exists(self.removePrefix(filename))
        

    def isFileInStager(self,filename):
        """Return boolean to indicate whether <filename> is known to the stager (i.e. on tape)."""
        return self.__filenamePattern.search(filename) is not None


    def maxWaitingTime(self):
        """Default maximum waiting time (seconds) for stageAllFilesAndWait()"""
        return self.__maxWaitingTime


    def pollingInterval(self):
        """Default interval (seconds) for polling the status of the files being staged"""
        return self.__pollingInterval


    def filenamePattern(self):
        return self.__filenamePattern.pattern


    def setFilenamePattern(self,pattern):
        self.__filenamePattern = re.compile(pattern)
        

    def setMaxWaitingTime(self,t):
        """Set the default maximum waiting time (seconds) for stageAllFilesAndWait()"""
        self.__maxWaitingTime = t


    def setPollingInterval(self,t):
        """Set the default interval (seconds) for polling the status of the files being staged"""
        self.__pollingInterval = t


    def removePrefix(self,filename):
        """Utility function. Remove prefix until first : (needed for certain commands)"""
        colon = filename.find(':')
        if colon == -1:
            return filename
        else:
            firstChar = colon + 1
            if firstChar == len(filename):
                return ''
            else:
                return filename[firstChar:]


    def addFilesToPreStage(self,listOfFiles):
        """Add a list (or tuple) of files or a single file to the list of files to be pre-staged.
        <listOfFiles> is either a python list (or tuple) with filenames or a single filename (string).
        Each file is tested with self.isFileInStager(), and a FilePreStageError exception
        is raised if the file is not in the stager."""
        # compatability with single filename
        if not listOfFiles: return
        if type(listOfFiles).__name__ == 'str': listOfFiles = [ listOfFiles ]
        for f in listOfFiles:
            if not self.isFileInStager(f):
                raise FilePreStageError( 'File %s does not seem to belong in %s' % (f,self.__class__.__name__) )
            if not self.fileExists(f):
                raise FilePreStageError( 'File %s does not exist in %s' % (f,self.__class__.__name__) )
            self.__filesToPreStage[f] = FilePreStager.UNKNOWN


    def updateStatus(self,printStatus='none'):
        """Update the status of all files. Print out status according to the value of <printStatus>:
        'none'   : Don't print anything.
        'changed : Only print files with changed state
        'all'    : Print out all files
        """
        fileList = self.__filesToPreStage
        if not fileList: return
        statusDict = self.getFilesStatus(fileList.keys())
        if printStatus == 'changed':
            self.printInfo( "Status of requested files with changed status on: %s" % time.asctime() )
        elif printStatus == 'all':
            self.printInfo( "Status of all requested files on: %s" % time.asctime() )
        nChanged = 0
        for filename,status in statusDict.items():
            if printStatus == 'all': self.printInfo( "  %s: %s" % (filename,status) )
            if status != fileList[filename]:
                # print any changes
                if printStatus == 'changed': self.printInfo( "  %s: %s" % (filename,status) )
                nChanged += 1
                # update state
                fileList[filename] = status
        if nChanged == 0 and printStatus == 'changed':
            self.printInfo( "  (No changes since last check)" )


    def filesToPreStage(self):
        return self.__filesToPreStage

        
    def getFilesWithStatus(self,status,*vargs):
        """Return the list of files which have one of the status given in the argument list"""
        statusList = [ status ] + list(vargs)
        return [ f for f,s in self.__filesToPreStage.items() if s in statusList ]


    def getFilesNotWithStatus(self,status,*vargs):
        """Return a list of files which do NOT have any of the status given in the argument list"""
        statusList = [ status ] + list(vargs)
        return [ f for f,s in self.__filesToPreStage.items() if s not in statusList ]


    def stageAllFiles(self,needUpdate=True):
        """Initiate stage-in of files that are not already on disk. Return immediately,
        and do not wait until all files are staged."""
        if not self.__filesToPreStage: return
        if needUpdate: self.updateStatus()
        toBeStaged =  self.getFilesWithStatus( FilePreStager.ONTAPE )
        if toBeStaged:
            self.printInfo("Pre-staging file(s) %s" % ','.join(toBeStaged))
            self.preStageFiles( toBeStaged )



class FileStagerRobot(TransformLogger):
    def __init__(self,name=None):
        if not name: name = self.__class__.__name__
        TransformLogger.__init__(self,name)
        self.setLoggerLevel( logging.INFO )
        self.__name = name
        self.__stagerList = []


    def name(self):
        return self.__name
        

    def setLoggerParentName(self,name):
        """Override from TransformLogger: propagate to all stagers"""
        TransformLogger.setLoggerParentName(self,name)
        for stager in self.__stagerList:
            stager.setLoggerParentName(name)


    def addStager(self,stager):
        if not isinstance(stager,FilePreStager):
            raise FilePreStageError('%s is not a FilePreStager' % stager.__class__.__name__)
        name = stager.name()
        oldStager = self.getStager(name)
        if oldStager is not None:
            self.logger().warning("File pre-stager %s already in %s. Keeping old one.", name,self.__name)
        else:
            self.logger().debug("Adding file pre-stager %s to %s", name, self.__name)
            stager.setLoggerParentName(self.name())
            self.__stagerList.append( stager )
        

    def getStager(self,name):
        for stager in self.__stagerList:
            if stager.name() == name: return stager
        # not found
        return None

         
    def getStagerForFile(self,filename):
        """Return the stager object if the filename needs a stager (i.e. matches one of the
        filenamePatterns). If the filename does not need a stager, None is returned.
        Static function: must be called as FilePreStager.getStager()"""
        for stager in self.__stagerList:
            if stager.isFileInStager(filename): return stager
        return None
    

    def addFilesToStagerIfNeeded(self,listOfFiles):
        """For all files in <listOfFiles>, if the file is in a tape stager system,
        add it to the list of files to be pre-staged using that stager.
        Return the list of files that were added to a stager.
        <listOfFiles> is either a python list (or tuple) with filenames or a single filename.
        Static function: must be called as FilePreStager.addFileToStagerIfNeeded()"""
        if not listOfFiles: return []
        # compatibility with single filename
        if type(listOfFiles).__name__ == 'str': listOfFiles = [ listOfFiles ]
        preStageList = []
        for f in listOfFiles:
            stager = self.getStagerForFile(f)
            if stager:
                stager.addFilesToPreStage(f)
                preStageList.append(f)
                
        return preStageList
    

    def updateStatus(self,printStatus='none'):
        for stager in self.__stagerList:
            stager.updateStatus(printStatus)
            

    def filesToPreStage(self):
        """Full list of files that should be pre-staged"""
        fileDict = {}
        for stager in self.__stagerList:
            fileDict.update( stager.filesToPreStage() )
        return fileDict
    

    def getFilesWithStatus(self, status, *vargs):
        fileList = []
        for stager in self.__stagerList:
            fileList += stager.getFilesWithStatus( status, *vargs )
        return fileList
        

    def getFilesNotWithStatus(self, status, *vargs):
        fileList = []
        for stager in self.__stagerList:
            fileList += stager.getFilesNotWithStatus( status, *vargs )
        return fileList


    def getFilesNotYetStaged(self):
        """Full list of files that are not yet pre-staged (and can be)"""
        return self.getFilesWithStatus( FilePreStager.STAGEIN, FilePreStager.ONTAPE )


    def getProblemFiles(self):
        """return list of files that are in some problem state"""
        return self.getFilesWithStatus( *list(FilePreStager.problemStatus) )


    def stageAllFiles(self,needUpdate=True):
        """Initiate stage-in of files that are not already on disk. Return immediately,
        and do not wait until all files are staged."""
        for stager in self.__stagerList:
            stager.stageAllFiles(needUpdate)
            

    def waitUntilAllFilesStaged(self):
        """Pre-stage all files to disk, and do not return before all files are staged.
        Return the dictionary of files (key=filename, value=status) that were not staged
        within the maxWaitingTime. If all goes well, therefore, an empty dictionary is returned.
        <maxWaitingTime> is the maximum time (seconds) to wait for all files to arrive on disk.
        If not given (or None), the value set in the constructor is used.
        <pollingInterval> is the time interval between checking the status of the files.
        If  <maxWaitingTime> or <pollingInterval> are not given (or None), their value
        is taken from the values set in the constructor."""
        # set maxWaitingTime and pollingInterval to the longest of all stagers
        maxWaitingTime = 0
        pollingInterval = 0
        for stager in self.__stagerList:
            maxWait = stager.maxWaitingTime()
            if maxWait > maxWaitingTime: maxWaitingTime = maxWait
            poll = stager.pollingInterval()
            if poll > pollingInterval: pollingInterval = poll
        startTime = time.time()
        self.updateStatus('all')
        problemFiles = self.getProblemFiles()
        notYetOnDisk = self.getFilesNotYetStaged()
        while not problemFiles and notYetOnDisk and time.time() - startTime < maxWaitingTime:
            self.stageAllFiles(False)
            time.sleep( pollingInterval )
            self.updateStatus('changed')
            problemFiles = self.getProblemFiles()
            notYetOnDisk = self.getFilesNotYetStaged()

        fileDict = self.filesToPreStage()
        problemDict = {}
        if problemFiles:
            mess = "Problems with the following to-be-staged files:"
            for filename in problemFiles:
                status = fileDict[filename]
                problemDict[filename] = status
                mess += os.linesep + "%s: %s" % (filename,status)
            self.printError( mess )
        # add others not yet staged
        notYetOnDisk = self.getFilesNotWithStatus( FilePreStager.STAGED )
        if notYetOnDisk:
            mess = "Files not staged to disk within the maximum waiting time (%s s):" % maxWaitingTime
            for filename in notYetOnDisk:
                status = fileDict[filename]
                problemDict[filename] = status
                # only printout if not already printed out above
                if filename not in problemFiles:
                    mess += os.linesep + "%s: %s" % (filename,status)
            self.printError( mess )
        if not problemDict:
            self.printInfo( "All requested files are staged to disk" )

        return problemDict


# Main entry point: the global file stager robot 
theFileStagerRobot = FileStagerRobot()
