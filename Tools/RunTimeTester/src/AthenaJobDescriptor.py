#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Class that stocks information about Athena jobs.
Responsible for setting up the run directory for a job. Eventually becomes a data member
of JobMinder.
"""
import os, string, copy, shutil

from Paths               import Paths
from os.path             import join, exists
from WorkerJobDescriptor import WorkerJobDescriptor
from Tools2              import searchPaths
from Tools               import mkdirIfNew
from formatCollection    import formatCollection
from ShellCommand        import ShellCommand
from RTTSException       import RTTInputError

# -------------------------------------------------------------------------
# import logging
# logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class FileLocator:
    """Finds a file on a search path."""
    def __init__(self, searchPath, logger):
        self.paths  = searchPath['path'].split(searchPath['separater'])
        self.logger = logger

    def locateFile(self, fn):
        for p in self.paths:
            fpath = os.path.join(p, fn)
            if os.path.exists(fpath):
                self.logger.info('Found file: %s' % fpath)
                return fpath
            else:
                self.logger.debug('%s: inexistant.' % fpath)
        m = 'File %s not found in paths %s' % (fn, str(self.paths))
        self.logger.error(m)
        return None

class AthenaJobDescriptor(WorkerJobDescriptor):    
    
    def __init__(self, paths, logger, jobSerialNumber, argDict):

        # init the base class

        self.commandLineFlags        = '' # corresponds to -c athena option
        self.commandLineString       = '' # an ad verbatim string of whatever options are required
        self.name                    = '' # filled in latter
        self.suggestedQueue          = ''
        self.confederation           = ''
        self.jobGroup                = ''
        # self.datasets                = {}
        self.mode                    = ''
        self.preconfigJobOptions     = []
        self.jobOptions              = []
        self.displayClass            = []
        self.displayProcess          = []
        self.displayComponent        = []
        self.defaultAthenaExecutable = 'athena.py'
        self.athenaFlavour           = self.defaultAthenaExecutable
        self.athenaCommonFlags       = False
        
        # list of scripts to run before the athena command.
        # 2/10/06: this list is provided by AthenaATNJobDescriptor
        # and is passed here through argDict
        self.preScripts = []

        # argDict contains data read from the config files -
        # this is the way variables such as jobOptions are set.
        WorkerJobDescriptor.__init__(self, paths, logger, jobSerialNumber, argDict)
        
        self.package = paths.package
        
        self.fixJobOptions()

        # recalculate the run and results paths (was done in Descriptor)
        # note that self.name is set in self.fixName()
        pathExtensions = [
            ('package',    self.package),
            ('jobGroup',   self.jobGroup),
            ('jobName',    self.name),
            ('SN',         str(self.jobSerialNumber))
            ]

        self.setRunPath(pathExtensions)
        self.setKeepFiles()
        # self.logger.debug('AthenaJobDescriptor.__init__ end')

    # ------------------------------------------------------------------------

    def pruneAttrs(self):
        return ['package']

    # ------------------------------------------------------------------------

    def  setKeepFiles(self):
        # copy the job options from the install area.
        kfdict = {}
        for jo in self.jobOptions:
            srcFile     = join(self.runPath, jo)
            destFile    = join(self.resPath, jo)
            infoString  = "Job options called with the athena command"
            displayColor= "#cc3333"
            self.keepFiles[srcFile] = {'keepFileString':destFile,
                                       'infoString':infoString,
                                       'displayColor':displayColor}

    # --------------------------------------------------------------

    def fixJobOptions(self):
        def fixExtension(jo):
            # name: need to come up with a name that be used to mkdir a
            # directory when (added) concatenated with a base path
            ext =jo[-3:]
            if ext !='.py':
                m = "Adding .py extension to jobOptions" + jo
                self.logger.debug(m)
                jo += '.py'
                
            return jo
        
        self.jobOptions = [fixExtension(jo) for jo in self.jobOptions]

        def fixPackage(jo):
            tokens = jo.split('/')
            if len(tokens) == 0: return jo
            if len(tokens) == 1: 
                jo = os.path.join(self.package, jo)
                return jo
            if len(tokens) == 2: return jo
            if len(tokens) > 2:
                return os.path.join(tokens[-2], tokens[-1])

        self.jobOptions = [fixPackage(jo) for jo in self.jobOptions]

    # --------------------------------------------------------------

    def fixName(self):
        # provide a name that be used to mkdir a directory
        # when (added) concatenated with a base path

        self.name,ext         = os.path.splitext(self.jobOptions[0])
        self.name             = os.path.basename(self.name)
        self.identifiedName   = self.name+str(self.jobSerialNumber)
        self.log              = self.identifiedName+"_log"
        # self.elog             = self.identifiedName+"_elog"

    # --------------------------------------------------------------

    def isDefaultExecutable(self):
        return self.defaultAthenaExecutable == self.athenaFlavour

    # --------------------------------------------------------------
        
    def athenaCmdOptions(self):
        batchFlag = '-b'
        
        if self.commandLineString:
            self.commandLineFlags = '' # can't have both, override
            
            if self.isDefaultExecutable() and self.commandLineString.find(batchFlag)==-1: # impose -b option
                self.commandLineString = '%s %s' % (batchFlag, self.commandLineString)
            return self.commandLineString

        preconfig = ''
        for p in self.preconfigJobOptions:
            preconfig += ' %s' % p

        if preconfig:
            preconfig = '-p %s' % preconfig
        
        if not self.isDefaultExecutable():
            batchFlag = '' # doesn't necessarily exist for other athena flavours
            
        if self.commandLineFlags:
            self.commandLineFlags = self.commandLineFlags.replace("'",'"')
            return "%s -c '%s' %s" % (preconfig, self.commandLineFlags, batchFlag)
        else:
            return '%s %s' % (preconfig, batchFlag)

    # --------------------------------------------------------------

    def athenaCommand(self):

        cmd = ''
        
        for ps in self.preScripts:
            cmd += 'source %s\n' % (ps)

        njob = 0
        for jo in self.jobOptions:
            cmd += 'echo "Starting job %d"\n' % njob
            cmd += '%s %s %s <&-\n' % (self.athenaFlavour,
                                       self.athenaCmdOptions(),
                                       jo)
            cmd += 'echo "RTT determined exit status to be: " $?'
            njob += 1
        return cmd
            
    # --------------------------------------------------------------

    def __str__(self):
        """Dumps out descriptor"""

        s = WorkerJobDescriptor.__str__(self)
        for k in self.__dict__.keys():
            line = 'job %s:' % k
            line.ljust(24)
            line += str(self.__dict__[k])+'\n'
            s += line
                        
        return s

    # --------------------------------------------------------------

    def dump(self):
        """Dumps out descriptor"""

        self.logger.info('|-------------------------------------------|')
        self.logger.info('| Athena jobdescriptor  dump                |')
        self.logger.info('|-------------------------------------------|')
        self.logger.info(self.__str__())

    # --------------------------------------------------------------
    def locateJobOptions(self, jo):
        """
        Use the joboptions search path from Paths and the list of jobOptions
        (elements of the form <package>/<options> to find the full path to the
        jobOptions files.
        """
        
        class JOLocator:
            def __init__(self, searchPath, logger):
                self.paths  = searchPath.split(',')
                self.logger = logger
            def locateJO(self, jo):
                for p in self.paths:
                    fpath = os.path.join(p, jo)
                    if os.path.exists(fpath): return fpath
                m = 'JobOptions %s not found in paths %s' % (
                    jo, str(self.paths))
                self.logger.error(m)
                return None


        locator = JOLocator(self.paths.jobOptionsSearchPath, self.logger)
        return locator.locateJO(jo)

    # --------------------------------------------------------------

    def setupRunDir(self):
        """
        copy job options to a subdirectory to <rundir>/<package>/<options>
        """
        class FullPathJOMaker:
            def __init__(self, searchPath, logger):
                self.locator = FileLocator(searchPath, logger)
            def __call__(self, jo) :
                return self.locator.locateFile(jo)

        fpJOMaker = FullPathJOMaker(self.paths.searchPaths['jobOptionsSearchPath'], self.logger)

        missingJobOptions = [jo for jo in self.jobOptions if not fpJOMaker(jo)]

        if missingJobOptions:
            msg = 'The following job option files could not be found in %s\n' % self.paths.searchPaths['jobOptionsSearchPath']
            for jo in missingJobOptions: msg += '%s\n' % jo
            raise RTTInputError(msg)
            
        def makeDirAndCopy(dir, jo, logger):
            # jo of the form <package>/<options>.py
            localPackageDir = os.path.join(dir, os.path.dirname(jo))
            optionsFile     = os.path.basename(jo)
            mkdirIfNew(localPackageDir)

            fpJO = fpJOMaker(jo)
            logger.debug('JobOption Copy: \n%s\n to %s' % (fpJO, localPackageDir))
            shutil.copy(fpJO, localPackageDir)
            
        [makeDirAndCopy(self.runPath, jo, self.logger) for jo in self.jobOptions]

        def addDataSets(dir, datasets, jo):
            # calculate the string to add to the end of the joboptions to specifiy the data sets
            def getdataset():
                for datasetType in datasets.keys():
                    for ds in datasets[datasetType]:
                        yield ds
            def clean(text):
                # convert first '+=' to '+'    
                return text.replace('+=', '=', 1)
                        
            bottomJOText = ''
            athenaCommonFlagsText  = 'from AthenaCommon.AthenaCommonFlags import athenaCommonFlags\n'

            for ds in getdataset():            
                dataset = '["%s"]\n' % ds.physical.replace('/castor', '%s/castor' % ds.castorProtocol())
                athenaCommonFlagsText += 'athenaCommonFlags.FilesInput += %s' % dataset
                bottomJOText += '%s += %s\n' % (ds.whichAthenaList(), dataset)

            bottomJOText = clean(bottomJOText)    
            athenaCommonFlagsText = clean(athenaCommonFlagsText)
            if not bottomJOText: return # no datasets
            
            # append the dataset string to the end of the job options
            file_ = open(os.path.join(dir, jo))
            contents = file_.read()
            file_.close()

            if self.athenaCommonFlags:
                contents_  = '# The following athenaCommonFlags text was pre-pended by the RTT.\n\n' 
                contents_ += athenaCommonFlagsText
                contents_ += '# End of RTT pre-pending.\n'
                contents   = contents_ + contents
                
            contents += '\n\n'
            contents += '# Everything below this line was appended by the RTT.'
            contents += '\n\n'
            contents += bottomJOText

            # Write out the new contents
            file_ = open(os.path.join(dir, jo),'w')
            file_.write(contents)
            file_.close()
                       
        # only add datasets for athena, not athenaMT/PT (they add data via command line)
        if self.hasData() and self.isDefaultExecutable(): 
            [addDataSets(self.runPath, self.datasets, jo) for jo in self.jobOptions]
            

    def dataForXMLNode(self):
        names = ['displayComponent', 'displayClass', 'displayProcess']
        dict = WorkerJobDescriptor.dataForXMLNode(self)
        for n in names: dict[n] = self.__dict__[n]
        return dict

    def deferredMinderUpdate(self):
       WorkerJobDescriptor.deferredDataSetUpdate(self) 
        
class AthenaPilotJobDescriptor(AthenaJobDescriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict):

        AthenaJobDescriptor.__init__(self, paths, logger, jobSerialNumber, argDict)
        self.logger.debug('AthenaPilotJobDescriptor.__init__ start')
        self.rttPilotJob = True
        self.suggestedQueue = 'short'
        self.logger.debug('AthenaPilotJobDescriptor.__init__ end %s' % (self.name))

    def fixName(self):
        AthenaJobDescriptor.fixName(self)
        self.name += 'Pilot'
        self.identifiedName   = self.name+str(self.jobSerialNumber)
        self.log              = self.identifiedName+"_log"
        # self.elog             = self.identifiedName+"_elog"


    def setupRunDir(self):
        """
        use the base class methodf to copy job options.
        this method modifies them to run one event.
        """

        AthenaJobDescriptor.setupRunDir(self)
        
        def limitEvents(dir, nevents, jo):

            # append the dataset string to the end of the job options
            fn = os.path.join(dir, jo)
            opts = open(fn, 'r')
            contents = opts.read()
            opts.close()
            contents += 'theApp.EvtMax = %d\n' % nevents
            opts = open(fn, 'w')
            opts.write(contents)
            opts.close()
                       

        [limitEvents(self.runPath, 1, jo) for jo in self.jobOptions]



class AthenaATNJobDescriptor(AthenaJobDescriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict):

        self.optionsAndScripts = argDict.pop('optionsAndScripts', [])
        self.atnName           = argDict.pop('atnName', '')

        AthenaJobDescriptor.__init__(self, paths, logger, jobSerialNumber, argDict)
        self.logger.debug('AthenaATNJobDescriptor.__init__ start')
        self.rttATNJob = True

        # need to recalculate work and results derectories
        pathExtensions = [
            ('package',    self.package),
            ('jobGroup',   self.jobGroup),
            ('jobName',    self.name),
            ('SN',         str(self.jobSerialNumber))
            ]
 
        self.setRunPath(pathExtensions)
        self.logger.debug('AthenaATNJobDescriptor.__init__ end %s' % self.name)

        
    def fixJobOptions(self): pass
    
    def setupRunDir(self):
        """
        Copy the presripts passed through the ATN configuration file to the
        run directory
        """
                
        locator = FileLocator(self.paths.searchPaths['PATH'], self.logger)
        preScripts =  [locator.locateFile(ps) for ps in self.preScripts]
        preScripts =  [ps for ps in preScripts if not  ps is None]
        

        [shutil.copy(s, self.runPath) for s in preScripts]

    def fixName(self):
        AthenaJobDescriptor.fixName(self)
        self.name             = self.atnName
        self.identifiedName   = self.name+str(self.jobSerialNumber)
        self.log              = self.identifiedName+"_log"
        # self.elog             = self.identifiedName+"_elog"

            
    # --------------------------------------------------------------

    def athenaCommand(self):
        """
        ATN delivers the job options and the command line flags in a
        single string.
        """

        cmd = ''

        for item in self.optionsAndScripts:                    
            for ps in item[1]:
                cmd += 'source %s\n' % (ps)

            njob = 0
            for jo in item[0]:
                cmd += 'echo "Starting job %s"\n' % njob
                cmd += '%s %s<&-\n' % (self.athenaFlavour, jo)
                cmd += 'echo "RTT determined exit status to be: " $?'
        return cmd
            
        
                 
