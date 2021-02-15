#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Utilities for writing job transforms for use at T0 and at the CAF Task Management System.
"""
__author__  = 'Juerg Beringer'
__version__ = 'TrfUtils.py atlas/athena'


import os, sys, pprint
import json, yaml
from InDetBeamSpotExample import JobRunner
from InDetBeamSpotExample.TaskManager import TaskManager
from optparse import OptionParser
import subprocess

def readJSON(fname):
    """Read a JSON file and return its data."""
    f = open(fname,'r')
    """JSON converts strings to unicode, so we use YAML instead."""
    data = yaml.load(f)
    f.close()
    return data

def writeJSON(fname, data):
    """Serialize data and write to fname."""
    f = open(fname,'w')
    print (json.dumps(data), file=f)
    f.close()

def getDataSetName(name, sep='#'):
    """Extract dataset name from a qualified file name."""
    if sep in name:
        return name.split(sep)[0]
    else:
        return None


def getFileName(name, sep='#'):
    """Extract file name from a qualified file name."""
    if sep in name:
        return name.split(sep)[1]
    else:
        return name


def parseQualifiedFileNames(inputList, sep='#'):
    """Parse a list of qualified file names (file names that are prepended with
       their dataset name). Returns a tuple consisting of the dataset name and the
       list of unqualified file names."""
    if not inputList:
        return (None, [])
    dataset = getDataSetName(inputList[0],sep)
    files = []
    for f in inputList:
        files.append(getFileName(f,sep))
    return (dataset,files)


def getGuid(name):
    """Get GUID of file name from local Pool file catalog (from Armin Nairz, as used at Tier-0)."""
    pfc='PoolFileCatalog.xml'
    if not os.path.isfile(pfc) :
        try :
            (s,o) = subprocess.getstatusoutput('uuidgen')
            guid = o.strip()
        except Exception:
            guid = 'UUIDGENERROR'
    else :
        try :
            cmd  = 'grep -B2 %s %s' % (name,pfc)
            cmd += ' | grep "File ID" | sed -e "s/  <File ID=\\\"//g" | sed -e "s/\\\">//g"'
            (s,o) = subprocess.getstatusoutput(cmd)
            guid = o.strip()
            # there might be a PFC from earlier processing steps
            if guid == '' :
                (s,o) = subprocess.getstatusoutput('uuidgen')
                guid = o.strip()
        except Exception:
            guid = 'PFCPARSINGERROR'
    return guid


def getFileDescription(fileName,dsname=''):
    """Return a dictionary with the standard file description required by T0 conventions.
       If no explicit dataset name is provided in dsname, the dataset name is
       assumed to be prepended to the fileName (first part, before '#')."""
    if not dsname:
        dsname = getDataSetName(fileName)
    d = {
        'dataset': dsname,
        'subFiles':[
            {
                'name': getFileName(fileName),
                'file_guid': getGuid(fileName),
                'file_size': 0,
                'nentries': 0,
                'checkSum': 0,
                
                }
            ]
        }
    return d


class JobRunnerTransform:
    """Job transform for running a JobRunner job at T0 or at the CAF Task Management
       System. Note that this class may abort execution by calling exit() in case of errors.
       Except in case of syntactical errors caught by OptionParser, a jobReport will always
       be produced."""

    def __init__(self, inputParamName, outputParamName, templateOutputName='outputfile', jobDirOutputName='',
                 mandatoryArgs = [], optionalArgs = []):
        self.inputParamName = inputParamName
        self.outputParamName = outputParamName
        self.mandatoryArgs = mandatoryArgs
        if inputParamName not in mandatoryArgs:
            mandatoryArgs.append(inputParamName)
        if outputParamName not in mandatoryArgs:
            mandatoryArgs.append(outputParamName)
        self.optionalArgs = optionalArgs
        self.runner = None
        self.templateOutputName = templateOutputName
        self.jobDirOutputName = jobDirOutputName
        self.outputList = [ ]   # List of qualified output files (ie file names including dataset name)
        self.reportName = 'jobReport.json'
        self.prodDir = '.'
        self.prodTaskDb = ''

        # Process command line args and extract argdict
        parser = OptionParser(usage="%prog --argJSON=<JSON file>")
        parser.add_option('-a', '--argJSON', dest='argJSON', help='Local file with JSON-serialized dictionary of key/value pairs')
        (options,args) = parser.parse_args()
        if len(args) != 0:
            self.report('WRONGARGSNUMBER_ERROR','Wrong number of command line arguments')
            parser.error('wrong number of command line arguments')
        if not options.argJSON:
            self.report('NOARGDICT_ERROR','Must use --argJSON to specify argdict.json file')
            parser.error('option --argJSON is mandatory')
        try:
            self.argdictFileName = options.argJSON 
            self.argdict = readJSON(options.argJSON)
        except Exception as e:
            self.report('ARGDICTNOTREADABLE_ERROR','File %s with JSON-serialized argdict cannot be read' % options.argJSON)
            print ('ERROR: file %s with JSON-serialized argdict cannot be read' % options.argJSON)
            print ('DEBUG: Exception =',e)
            sys.exit(1)

        # Print argdict
        print ('\nInput argdict (%s):\n' % options.argJSON)
        print (pprint.pformat(self.argdict))
        print ('\n')

        # Check for all mandatory parameters
        missingArgs = [ x for x in mandatoryArgs if x not in self.argdict ]
        if missingArgs:
            self.report('MISSINGPARAM_ERROR','Mandatory parameter(s) missing from argdict: '+str(missingArgs))
            print ('ERROR: mandatory parameter(s) missing from argdict:', missingArgs)
            sys.exit(1)

        # Extract input and output dataset and file names
        # NOTE: inputs come from a list, output is a single file (but there might be others)
        self.inputds, self.inputfiles = parseQualifiedFileNames(self.argdict[inputParamName])
        if not self.inputfiles:
            self.report('NOINPUTFILE_ERROR','No input file specified (only dataset name?)')
            print ('ERROR: no input file specified')
            sys.exit(1)
        self.outputfile = getFileName(self.argdict[outputParamName])
        #self.outputList.append(self.argdict[outputParamName])
        self.outputds = getDataSetName(self.argdict[outputParamName])
        if not self.outputds:
            self.report('NODATASET_ERROR','No dataset given in parameter '+outputParamName)
            print ('ERROR: No dataset given in parameter',outputParamName)
            sys.exit(1)
        splitDSName = self.outputds.split('.')
        if len(splitDSName)<5:
            self.report('DATASETNAME_ERROR',"Output dataset name %s doesn't conform to standard naming convention" % self.outputds)
            print ("ERROR: Output dataset name %s doesn't conform to standard naming convention" % self.outputds)
            sys.exit(1)
        self.dataset = '.'.join(splitDSName[:-3])
        self.taskname = '.'.join(splitDSName[-2:])
        self.jobname = os.path.basename(self.outputfile)
        if '_attempt' in self.argdict:
            self.jobname = self.jobname + '.v%s' % self.argdict['_attempt']

    def setProdDir(self,dir):
        if os.access(dir,os.W_OK):
            self.prodDir = dir
        else:
            # Continue anyway or abort?
            print ('ERROR: No write access to production directory',dir,'- will use current working directory instead:', os.getcwd())
            self.prodDir = os.getcwd()
            sys.exit(1)

    def setProdTaskDatabase(self,taskdb):
        self.prodTaskDb = taskdb

    def getJobRunner(self,**jobRunnerArgs):
        if self.runner:
            print ('WARNING: Overwriting already configured JobRunner')
        self.runner = JobRunner.JobRunner(jobdir=self.prodDir+'/'+self.dataset+'/'+self.taskname+'/'+self.jobname,
                                          jobname=self.jobname,
                                          inputds=self.inputds,
                                          inputfiles=self.inputfiles,
                                          outputds=self.outputds,
                                          filesperjob=len(self.inputfiles),
                                          setuprelease=False,
                                          addinputtopoolcatalog=False,
                                          returnstatuscode=True)
        self.runner.appendParam('cmdjobpreprocessing',
                                'cp %s %s/%s.argdict.json' % (self.argdictFileName, self.runner.getParam('jobdir'),self.jobname))
        self.runner.setParam(self.templateOutputName,self.outputfile)
        if self.jobDirOutputName:
            self.runner.appendParam('cmdjobpostprocessing',
                                    'cp %s %s/%s-%s' % (self.outputfile, self.runner.getParam('jobdir'),self.jobname,self.jobDirOutputName))
        for k,v in jobRunnerArgs.items():
            self.runner.setParam(k,v)
        for k,v in self.argdict.items():
            if k in self.mandatoryArgs: continue
            if k in self.optionalArgs: continue
            self.runner.setParam(k,v)
        return self.runner

    def addOutput(self, paramName, templateName, jobDirName=''):
        """Add an additional output file to the output dataset. If jobDirName is set, the
           output file will also be copied under that name to the job directory."""
        #self.outputList.append(self.argdict[paramName])
        f = getFileName(self.argdict[paramName])
        self.runner.setParam(templateName,f)
        if jobDirName:
            self.runner.appendParam('cmdjobpostprocessing',
                                    'cp %s %s/%s-%s' % (f,self.runner.getParam('jobdir'),self.jobname,jobDirName))
    def showParams(self):
        print ('JobRunner parameters:\n')
        self.runner.showParams()

    def configure(self):
        self.runner.configure()
        # If JobRunner configuration was successful, move any earlier jobs
        # out of the way if _attempt is specified. This should guarantee
        # that the task directory contains only valid jobs
        if '_attempt' in self.argdict:
            currentAttempt = int(self.argdict['_attempt'])
            for i in range(-1,currentAttempt):
                if i==-1:
                    d = '%s/%s/%s/%s' % (self.prodDir,self.dataset,self.taskname,os.path.basename(self.outputfile))
                else:
                    d = '%s/%s/%s/%s.v%s' % (self.prodDir,self.dataset,self.taskname,os.path.basename(self.outputfile),i)
                if os.path.exists(d):
                    retriedJobDir = '%s/%s/%s/RETRIED_JOBS' % (self.prodDir,self.dataset,self.taskname)
                    print ('\nMoving previous job directory %s to %s' % (d,retriedJobDir))
                    os.system('mkdir -p %s' % (retriedJobDir))
                    os.system('mv -f %s %s' % (d,retriedJobDir))

    def addTaskToDatabase(self,comment=''):
        if self.prodTaskDb:
            try:
                with TaskManager(self.prodTaskDb) as taskman:
                    taskman.addTask(self.dataset,
                                    self.taskname,
                                    self.runner.getParam('joboptionpath'),
                                    self.runner.getParam('release'),
                                    self.runner.getNJobs(),
                                    self.runner.getParam('taskpostprocsteps'),
                                    comment=comment)
            except Exception as e:
                print ('ERROR: Unable to add task to task manager database '+self.prodTaskDb)
                print ('DEBUG: Exception =',e)
        else:
            print ('WARNING: No task manager database configured')

    def run(self):
        self.runner.run()
        #print (self.runner.jobStatus)


    def go(self,commentForTaskDb=''):
        """Show parameters, configure job, update task database, run job and produce report.
           This method will ensure that a job report is always produced, independently of any errors."""
        try:
            self.showParams()
            self.configure()
        except Exception as e:
            self.report('JOBRUNNER_CONFIGURE_ERROR','Unable to configure JobRunner job - perhaps same job was already configured / run before?')
            print ("ERROR: Unable to configure JobRunner job - perhaps same job was already configured / run before?")
            print ('DEBUG: Exception =',e)
        else:
            try:
                self.addTaskToDatabase(commentForTaskDb)
                self.run()
            finally:
                self.report()


    def report(self,errAcronym='',moreText=''):
        if errAcronym:
            jobStatus = 999
            jobStatusAcronym = errAcronym
        else:
            try:
                jobStatus = self.runner.jobStatus[0]   # Assume we always run single jobs
                jobStatusAcronym = 'OK' if jobStatus==0 else 'ATHENA_ERROR'
            except Exception:
                jobStatus = 999
                jobStatusAcronym = 'NOJOBSTATUS_ERROR'
                moreText = "Jobrunner terminated abnormally and w/o a job status; athena job may or may not have run"

        jobStatusAcronym = jobStatusAcronym[:128]   # 128 char limit in T0 DB
        report =  {'exitCode': jobStatus,
                   'exitAcronym': jobStatusAcronym,
                   'files': { 'output':[] }
                  }
        if moreText:
            report['exitMsg'] = moreText

        # If there was no error, store outputs (request from Armin to not give any outputs for failed jobs).
        # Must also check that output file indeed exists.
        if jobStatus==0:
            for f in self.outputList:
                if os.path.exists(getFileName(f)):
                    report['files']['output'].append(getFileDescription(f))

        # Write jobReport file
        writeJSON(self.reportName,report)

        # Copy  jobReport file to job directory - note we do this only if there was
        # no error, otherwise we might overwrite an older report from an OK job
        if jobStatus==0:
            try:
                os.system('cp %s %s/%s.%s' % (self.reportName,self.runner.getParam('jobdir'),self.jobname,self.reportName) )
            except Exception as e:
                print ('WARNING: Copying of job report file (%s) to job directory failed' % self.reportName)
                print ('DEBUG: Exception =',e)

        # Nicely print job report to stdout
        print ('\n\nJob report (jobReport.json):\n')
        print (pprint.pformat(report))
        print ('\n')
