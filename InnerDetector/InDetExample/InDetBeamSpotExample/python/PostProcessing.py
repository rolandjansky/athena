#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
This module defines the generic infrastructure for task postprocessing.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'


import dircache, glob, time, sys, os, math
import subprocess

from InDetBeamSpotExample.TaskManager import TaskAnalyzer, TaskManager, getKey
from InDetBeamSpotExample.Utils import getUserName




# Exception classes
class PostProcessingError(Exception):
    """Generic postprocessing exception class."""
    def __init__(self,message,executedSteps,newStatus=None):
        self.message = message
        self.executedSteps = executedSteps
        self.newStatus = newStatus
    def __str__(self):
        return self.message

class PostponeProcessing(Exception):
    """Exception used to postpone postprocessing."""
    def __init__(self,message,executedSteps,newStatus=None):
        self.message = message
        self.newStatus = newStatus
    def __str__(self):
        return self.message


def runPostProcStep(taskman,taskDict,oldStatus,previousSteps,step,postprocLib,jobName):
    """Instantiate and run a single postprocessing step."""
    if hasattr(postprocLib,step):
        print ('...',step,'\n')
        # First reload taskDict - a previous step might have updated the info
        taskDict = taskman.getTaskDict(taskDict['DSNAME'],taskDict['TASKNAME'])
        postprocClass = getattr(postprocLib,step)(taskman,taskDict,oldStatus,previousSteps,postprocLib,jobName)
        postprocClass.run()
        return postprocClass.executedSteps
    else:
        raise PostProcessingError('ERROR: undefined postprocessing step: '+step,previousSteps,TaskManager.StatusCodes['POSTPROCFAILED'])


def doPostProcessing(taskman,taskDict,postprocSteps,postprocLib,forceRun=False,jobName=None):
    """Execute a series of postprocessing steps. The task status is updated before
       and after the postprocessing. Log files are handled by each postprocessing step
       individually. Returns a list of executed postprocessing steps."""
    # NOTE: At present, we don't use per-job postprocessing steps. Should this feature
    #       be removed?
    dsName = taskDict['DSNAME']
    taskName = taskDict['TASKNAME']
    prePostProcStatus = taskDict['STATUS']
    executedSteps = []

    # Check if there's anything to do
    if not postprocSteps:
        return []   # nothing to do

    #update again from DataBase
    prePostProcStatus = taskman.getStatus(dsName,taskName)
    # Don't run postprocessing if status is already postprocessing
    if prePostProcStatus>=TaskManager.StatusCodes['POSTPROCRUNNING'] and not forceRun:
        print ('Exiting postprocessing without doing anything: task %s/%s status is %s\n' % (dsName,taskName,getKey(TaskManager.StatusCodes,prePostProcStatus)))
        return []
    
    # Start postprocessing
    taskman.setStatus(dsName,taskName,TaskManager.StatusCodes['POSTPROCRUNNING'])
    if jobName:
        print ('Postprocessing for task %s/%s - job %s\nOld status: %s\n' % (dsName,taskName,jobName,getKey(TaskManager.StatusCodes,prePostProcStatus)))
    else:
        print ('Postprocessing for task %s/%s\nOld status: %s\n' % (dsName,taskName,getKey(TaskManager.StatusCodes,prePostProcStatus)))

    # Get list of postprocessing status files that we may have to remove later
    if jobName:
        postprocStamps = glob.glob('%s/%s/%s/*.status.POSTPROCESSING' % (dsName,taskName,jobName))
    else:
        postprocStamps = glob.glob('%s/%s/*/*.status.POSTPROCESSING' % (dsName,taskName))

    # Do each specified processing step. Postprocessing may be interrupted either deliberately
    # by a postprocessing step or due to errors.
    try:
        for step in postprocSteps:
            executedSteps = runPostProcStep(taskman,taskDict,prePostProcStatus,executedSteps,step,postprocLib,jobName)

    except PostponeProcessing as e:
        # Stop postprocessing chain w/o error. New status will be determined below if not
        # specified in the exception.
        if e.newStatus:
            taskman.setStatus(dsName,taskName,e.newStatus)
            print ('%i step(s) completed successfully: ' % len(executedSteps),executedSteps,'\n')
            return executedSteps

    except PostProcessingError as e:
        print (e)
        if e.newStatus:
            taskman.setStatus(dsName,taskName,e.newStatus)
            print ('Executed steps: ',e.executedSteps)
            return e.executedSteps
        else:
            if (taskDict['NJOBS_SUBMITTED']+taskDict['NJOBS_RUNNING']) > 0:
                # Not all the jobs have run, so ignore error and postpone to later
                pass

            else:
                # All the jobs have run, so nothing new in the future
                taskman.setStatus(dsName,taskName,TaskManager.StatusCodes['POSTPROCFAILED'])
                print ('Executed steps: ',e.executedSteps)
                return e.executedSteps

    except Exception as e:
        # Any other postprocessing error. Task status becomes POSTPROCFAILED.
        print (e)
        taskman.setStatus(dsName,taskName,TaskManager.StatusCodes['POSTPROCFAILED'])
        print ('Executed steps: ',executedSteps)
        return executedSteps

    else:
        # After successful postprocessing, delete POSTPROCESSING status files
        # and mark corresponding jobs as completed
        for p in postprocStamps:
            os.system('rm -f %s' % p)
            basename = p[:-15]
            os.system('touch %s.COMPLETED' % basename)

    # Redetermine current status from files on disk
    a = TaskAnalyzer('.',dsName,taskName)
    if a.analyzeFiles():
        a.updateStatus(taskman)
    else:
        # Postprocessing could have deleted the task and migrated it to tape
        taskman.setDiskStatus(dsName,taskName,TaskManager.OnDiskCodes['DELETED'])

    print ('%i step(s) completed successfully: ' % len(executedSteps),executedSteps,'\n')
    return executedSteps


class PostProcessingStep:

    def __init__(self,taskman,taskDict,oldStatus,previousSteps,postprocLib,jobName=None):
        self.stepName = self.__class__.__name__
        self.executedSteps = previousSteps
        self.executedSteps.append( self.stepName )
        self.taskman = taskman
        self.taskDict = taskDict
        self.dsName = taskDict['DSNAME']
        self.taskName = taskDict['TASKNAME']
        self.taskDir = '/'.join([self.dsName,self.taskName])
        self.oldStatus = oldStatus
        self.postprocLib = postprocLib
        self.jobName = jobName
        self.baseName     = '%s-%s' % (self.dsName,self.taskName)
        self.baseNameStep = '%s-%s.%s' % (self.dsName,self.taskName,self.stepName)
        self.logfile = open('%s/%s.log' % (self.taskDir,self.baseNameStep), 'w')
        self.log('Starting postprocessing:  %s' % self.__class__.__name__)
        self.log('* Data set name = %s' % self.dsName, noTime=True)
        self.log('* Task name     = %s' % self.taskName, noTime=True)
        self.log('* Old status    = %s' % getKey(TaskManager.StatusCodes,self.oldStatus), noTime=True)
        self.log('* Job name      = %s' % self.jobName, noTime=True)
        self.log('* User name     = %s' % getUserName(), noTime=True)
        self.log('* Host name     = %s\n' % os.uname()[1], noTime=True)
        pass

    def __del__(self):
        self.log('Done')
        self.logfile.close()

    def log(self,msg=None,text=None,noTime=False,doPrint=False):
        outputFiles = [self.logfile]
        if doPrint:
            outputFiles.append(sys.stdout)
        for out in outputFiles:
            if msg:
                if noTime:
                    out.write(' '*31)
                    out.write(msg)
                    out.write('\n')
                else:
                    out.write('\n%-30s %s\n' % (time.strftime('%a %b %d %X %Z %Y'),msg))
            if text:
                #out.write('\n')
                out.write(text)
                out.write('\n')

    def logExec(self,cmd,doPrint=False,checkStatusCode=True,errorMsg='',abortOnError=True):
        (status,output) = subprocess.getstatusoutput(cmd)
        status = status >> 8   # Convert to standard Unix exit code 
        self.log('Executing: %s' % cmd, output)
        if doPrint or status:
            print (output)
            print()
        if status and checkStatusCode:
            if not errorMsg:
                errorMsg = 'ERROR in postprocessing step %s while executing:\n\n%s\n' % (self.stepName,cmd)
            self.log(text='\nERROR: exit status = %i' % (status))
            self.log(text=errorMsg)
            if abortOnError:
                raise PostProcessingError(errorMsg,self.executedSteps)
        return status

    def getFileName(self,postFix,stepName='',absPath=False):
        if stepName:
            name = '%s.%s%s' % (self.baseName,stepName,postFix)
        else:
            name = self.baseNameStep+postFix
        if absPath:
            name = '%s/%s' % (self.taskDir,name)
        return name

    def jobList(self):
        try:
            l = dircache.listdir(self.taskDir)
        except Exception:
            l = []
        return l

    def taskFileList(self,fileNamePattern,statusName='POSTPROCESSING',jobName=None):
        """Get list of files with specific names from all jobs in a given state."""
        oldDir = os.getcwd()
        os.chdir(self.taskDir)
        if jobName:
            statusFiles = glob.glob('%s/*.status.%s' % (jobName,statusName))
        else:
            statusFiles = glob.glob('*/*.status.%s' % (statusName))
        files = []
        for s in statusFiles:
            b = '/'.join(s.split('/')[:-1])
            files.extend(glob.glob('%s/%s' % (b,fileNamePattern)))
        os.chdir(oldDir)
        return sorted(files)

    def hadd(self,dir,outputFileName,inputFiles,maxFilesPerHadd=100):
        """Execute staged hadd for many files."""
        if len(inputFiles)<=maxFilesPerHadd:
            self.logExec('cd %s; hadd -f %s %s' % (dir,outputFileName,' '.join(inputFiles)))
        else:
            nSteps = int(math.ceil(float(len(inputFiles))/maxFilesPerHadd))
            partFiles = []
            for index in range(nSteps):
                minIndex = index*maxFilesPerHadd
                maxIndex = min((index+1)*maxFilesPerHadd,len(inputFiles))
                stepInputFiles = inputFiles[minIndex:maxIndex]
                stepOutputFileName = '%s.part%i' % (outputFileName,index)
                self.logExec('cd %s; hadd -f %s %s' % (dir,stepOutputFileName,' '.join(stepInputFiles)))
                partFiles.append(stepOutputFileName)
            self.logExec('cd %s; hadd -f %s %s' % (dir,outputFileName,' '.join(partFiles)))
            self.logExec('cd %s; rm -f %s' % (dir,' '.join(partFiles)))            

    def getJobConfig(self,jobName=None):
        config = {}
        if not jobName:
            jobName = '*'
        configFile = glob.glob('%s/%s/%s' % (self.taskDir,jobName,'*.config.py.final.py'))
        if not configFile:
            configFile = glob.glob('%s/%s/%s' % (self.taskDir,jobName,'*.config.py'))
        if configFile:
            exec(open(configFile[0],'r'),config)    # Eval config file and put defs into config dict
        return config['jobConfig']

    def addResult(self,resultFileName):
        # FIXME: strip leading dirs from resultFilename, so I can pass an absolute file name?
        if not resultFileName:
            return
        resultFiles = self.taskDict['RESULTFILES']
        if resultFiles is None:
            resultFiles = ''
        if resultFileName not in resultFiles.split():
            resultFiles = ' '.join([resultFiles,resultFileName])
            self.taskman.setValue(self.dsName,self.taskName,'RESULTFILES',resultFiles)
            self.taskDict['RESULTFILES'] = resultFiles
