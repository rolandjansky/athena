#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
JobRunner is a set of classes aimed at simplifying the running of
(athena) jobs on a set of input files. JobRunner provides each
job with its set of configuration parameters (using a Python
dictonary) and runs it over a sub-sample (or all) of the input files.
Jobs can run interactively, on different batch systems, or on the Grid.

Written by Juerg Beringer (LBNL) in 2008.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: JobRunner.py 747883 2016-05-18 06:58:10Z amorley $'

import math
import os
import pwd
import socket
import time
import pprint
import glob
import re
import subprocess


# Default template of job submission script
scriptTemplate = """#!/bin/sh
touch %(runflag)s
rm %(subflag)s
%(cmdsetup)s
mkdir -p %(rundir)s
cd %(rundir)s
echo "Preprocessing and copying files (`date`) ..."
# Redirect TMPDIR to avoid problems with concurrent jobs
%(maketmpdir)s
export TMPDIR=`pwd`/tmp
%(cmdjobpreprocessing)s
%(cmdcopyfiles)s
echo ''
echo "Creating POOL file catalog (`date`) ..."
%(cmddefinepoolcatalog)s
echo ''
echo "Running athena (`date`) ..."
/usr/bin/time -p athena.py %(configfile)s %(joboptionpath)s
status=$?
touch %(exitflag)s
echo $status > %(exitstatus)s
echo "Postprocessing (`date`) ..."
%(cmdjobpostprocessing)s
touch %(doneflag)s
rm %(runflag)s
%(cmdexit)s
"""


# Exception class
class JobRunnerError(AttributeError):
    """JobRunner exception class"""


# Utility class to hold job parameter data
class JobRunnerParameter:
    """JobRunner job parameter data"""

    def __init__(self,name,value='',description='',isSpecial=False):
        self.name = name
        self.value = value
        self.description = description
        self.isSpecial = isSpecial    # Special parameter, won't be propagated in normal way to jobs

    def __str__(self):
        if self.isSpecial:
            return "%-20s = %-20s  # SPECIAL - %s" % (self.name,self.value,self.description) 
        else:
            if self.description:
                return "%-20s = %-20s  # %s" % (self.name,self.value,self.description)
            else:
                return "%-20s = %-20s" % (self.name,self.value)

def GetRelease():
    project = os.getenv('AtlasProject')
    version = os.getenv('AtlasVersion')
    platform = os.getenv('%s_PLATFORM' % project, os.getenv('CMTCONFIG'))
    return ','.join([project, version] + platform.split('-'))

# Main JobRunner class
class JobRunner:
    """JobRunner base class"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        self.params = { }        # Dictionary of JobRunnerParameter, indexed by parameter name
        self.paramOrder = [ ]    # Ordered list of parameter names
        self.paramToBeCopied = [ ]  # List of parameters specifying files to be copied before job starts
        self.jobs = { }          # Dictionary of job configuratinos (params), indexed by jobnr
        self.jobStatus = { }     # Dictionary of job exit status, indexed by jobnr

        # Job parameters (parameters flagged as special require specific code in configureJob)
        self.setParam('jobnr',-1,'Number of job last configured',True)
        self.setParam('inputfiles',[],'List of all input files',True)
        self.setParam('filesperjob',1,'Number of input files per job')
        self.setParam('lbperjob',0,'Number of luminosity blocks per job if doing LB bunching')
        self.setParam('maxjobs',0,'Maximum number of jobs to run (0=no maximum)')
        self.setParam('jobname','job%(jobnr)03i','Job name template')
        self.setParam('jobdir',os.getcwd(),'Directory template from where the jobs are started')
        self.setParam('rundir','.','Directory template where the jobs execute')
        self.setParam('configfile','%(jobdir)s/%(jobname)s.config.py','Job configuration file name template (use %(jobnr)i for current job number)')
        self.setParam('logfile','%(jobdir)s/%(jobname)s.log','Log file name template')
        self.setParam('scriptfile','%(jobdir)s/%(jobname)s.sh','Script file name template')
        self.setParam('outputfileprefix','%(jobdir)s/%(jobname)s-','Output file name prefix template')
        self.setParam('outputfilelist',[],'List of desired output files (w/o outputfileprefix)')
        self.setParam('outputfilestring','','Generated comma-separated list of desired output files with full names',True)
        self.setParam('filesforpoolcatalog',[],'Files to insert into POOL file catalog')
        self.setParam('addinputtopoolcatalog',True,"Add job's input files to POOL file catalog?")
        self.setParam('subflag','%(jobdir)s/%(jobname)s.status.SUBMITTED','Status flag template for submitted jobs')
        self.setParam('runflag','%(jobdir)s/%(jobname)s.status.RUNNING','Status flag template for running jobs')
        self.setParam('exitflag','"%(jobdir)s/%(jobname)s.exit.$status"','Status flag template to record athena exit status')
        self.setParam('exitstatus','"%(jobdir)s/%(jobname)s.exitstatus.dat"','File with exit status value')
        self.setParam('postprocflag','%(jobdir)s/%(jobname)s.status.POSTPROCESSING','Status flag template for jobs requiring postprocessing')
        self.setParam('completedflag','%(jobdir)s/%(jobname)s.status.COMPLETED','Status flag template for completed jobs')
        self.setParam('doneflag','','Actual status flag used at end of job',True)
        self.setParam('logmail','','E-mail address for sending status e-mails')
        self.setParam('release',GetRelease(),'Software release (may include flags such as opt,runtime)')
        self.setParam('setuprelease',True,'Setup ATLAS software release?')
        self.setParam('returnstatuscode',False,'Return athena status code from script?')
        self.setParam('joboptionpath',os.getcwd()+'/joboptions.py','Master joboption file (w/full path if not in release)')
        self.setParam('taskpostprocsteps','','Task-level postprocessing steps (defined at task submission)')
        self.setParam('jobpostprocsteps','','Job-level postprocessing steps (updated by template)')
        self.setParam('cmdsetup','','Statement for setting up release',True)
        self.setParam('cmddefinepoolcatalog','','Statement for defining POOL file catalog',True)
        self.setParam('cmdcopyfiles','','Statement to copy files to local directory',True)
        self.setParam('cmdjobpreprocessing','','Statement(s) to execute for preprocessing',False)
        self.setParam('cmdjobpostprocessing','','Statement(s) to execute for postprocessing',False)
        self.setParam('cmdexit','','Statement used to exit shell script (allows passing up athena exit code)',True)
        self.setParam('maketmpdir','mkdir -p tmp','Statement to create tmp directory')
        self.setParam('script',scriptTemplate,'Template of job submission script',True)

        # Set additional parameters given to constructor
        for k in params.keys():
            self.setParam(k,params[k])

        self.checkParams()


    def setParam(self,name,value=None,description=None,isSpecial=None,insertAtFront=False):
        """Set the value of a job parameter. Job parameters may be templates that name
           previously defined job parameters and will be evaluated when a given job
           is configured. Therefore the order in which setParam is called for the different
           parameters is relevant. insertAtFront can be set to True to force early
           evaluation of a given parameter."""
        p = self.params[name] = self.params.get(name,JobRunnerParameter(name))
        if value!=None:
            p.value = value
        if description!=None:
            p.description = description
        if isSpecial!=None:
            p.isSpecial=isSpecial
        if not name in self.paramOrder:
            if insertAtFront:
                self.paramOrder.insert(0,name)
            else:
                self.paramOrder.append(name)


    def appendParam(self,name,value=None,description=None,isSpecial=None,insertAtFront=False,endOfLine='\n'):
        """Append to the value of a job parameter. If the parameter doesn't exist yet,
           setParam is called to create it. If it does exist, only the value is updated
           and the description and flag arguments are ignored."""
        if name in self.params:
            if value!=None:
                p = self.params[name]
                if isinstance(p.value,str) and p.value:
                    p.value = p.value + endOfLine + value
                else:
                    p.value = p.value + value
        else:
            self.setParam(name,value,description,isSpecial,insertAtFront)


    def getParam(self,name):
        """Get value of a parameter by name."""
        return self.params[name].value


    def isSpecialParam(self,name):
        """Check if parameter requires special treatment."""
        return self.params[name].isSpecial


    def registerToBeCopied(self,paramName):
        """Register a parameter holding the name of an input file to be
           copied before a job starts."""
        if paramName not in self.paramToBeCopied:
            self.paramToBeCopied.append(paramName)


    def checkParams(self):
        """A generic routine for checking job parameters. Check that all parameters
           can be evaluated. May be overrridden by a subclass if desired."""
        if len(self.paramOrder)!=len(self.params):
            raise JobRunnerError, 'Inconsistent parameter definition'
        try:
            tmp = { }
            for p in self.paramOrder:
                value = ''   # this is necessary to prevent an exception from picking up the previous value
                value = self.getParam(p)
                if isinstance(value,str):
                    tmp[p] = value % tmp
                else:
                    tmp[p] = value
        except:
            raise JobRunnerError, 'Unable to evaluate parameter: '+p+' = '+value+' (check parameter order)'


    def showParams(self,maxLineLength=80):
        """Show current job parameters."""
        for p in self.paramOrder:
            s = str(self.params[p])
            if maxLineLength > 0 and len(s) > maxLineLength:
                s = s[0:maxLineLength-3] + '...'
            print s


    def dumpParams(self,format='%(name)-20s = %(value)s',maxLineLength=0):
        """Dump all parameters into a string in a user-specified format."""
        dump = ''
        for p in self.paramOrder:
            s = format % self.params[p].__dict__
            if maxLineLength:
                s = s[0:maxLineLength-3]+'...'
            dump += s+'\n'
        return dump


    def addFiles(self,fileList):
        """ Add a list of input files to be processed."""
        self.getParam('inputfiles').extend(fileList)


    def addFilesToPoolFileCatalog(self,fileList):
        """Add a list of files to be inserted to the POOL file catalog."""
        self.getParam('filesforpoolcatalog').extend(fileList)


    def configureJob(self,jobnr,inputList=[]):
        """Configure parameters for a single job and write job configuration files. If inputLIst
           is given, this is the list of input files used for the job."""
        if jobnr in self.jobs:
            raise JobRunnerError, 'Job number %s already configured' % jobnr
        jobConfig = { }

        jobConfig['jobnr'] = jobnr
        self.setParam('jobnr',jobnr)

        # Check if any input files are available for this job
        if inputList:
            jobConfig['inputfiles'] = inputList
        else:
            inputfiles = self.getParam('inputfiles')
            iFirst = self.getParam('filesperjob')*jobnr
            if iFirst >= len(inputfiles):
                raise JobRunnerError, 'Jobnr = %i too high for available number of files' % jobnr
            iLast = iFirst + self.getParam('filesperjob')
            if iLast > len(inputfiles):
                iLast=len(inputfiles)
            jobConfig['inputfiles'] = inputfiles[iFirst:iLast]

        # Update regular parameters
        for p in self.paramOrder:
            if self.isSpecialParam(p): continue
            value = self.getParam(p)
            if isinstance(value,str):
                jobConfig[p] = value % jobConfig
            else:
                jobConfig[p] = value

        # Define statement to set up the release
        if jobConfig['setuprelease']:
            # Use explicit command if given
            jobConfig['cmdsetup'] = self.getParam('cmdsetup') % jobConfig
            # otherwise take from CMDSETUP env var if set or assume normal asetup 
            if not jobConfig['cmdsetup']:
                jobConfig['cmdsetup'] = os.getenv('CMDSETUP','source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh %(release)s --noautocdtest') % jobConfig
        else:
            jobConfig['cmdsetup'] = ''

        # Collect files to be copied locally, and replace corresponding jobConfig
        # parameters by base file name (to point to the local copy)
        jobConfig['cmdcopyfiles'] = self.getParam('cmdcopyfiles') % jobConfig
        if not jobConfig['cmdcopyfiles']:
            filestobecopied = ''
            for p in self.paramToBeCopied:
                if not jobConfig[p]: continue
                filestobecopied = ' '.join([filestobecopied,jobConfig[p]])
                jobConfig[p] = os.path.basename(jobConfig[p])
            if filestobecopied:
                jobConfig['cmdcopyfiles'] = 'cp -p '+filestobecopied+' .'

        # Unless the user sets a specific value for parameter cmddefinepoolcatalog, determine
        # what should be added and create appropriate catalog
        jobConfig['cmddefinepoolcatalog'] = self.getParam('cmddefinepoolcatalog') % jobConfig
        if not jobConfig['cmddefinepoolcatalog']:
            if jobConfig['addinputtopoolcatalog']:
                argstring = ' '.join(jobConfig['filesforpoolcatalog']+jobConfig['inputfiles']) % jobConfig
            else:
                argstring = ' '.join(jobConfig['filesforpoolcatalog']) % jobConfig
            if argstring:
                jobConfig['cmddefinepoolcatalog'] = 'pool_insertFileToCatalog '+argstring

        # Define statement used to exit the shell script
        if jobConfig['returnstatuscode']:
            jobConfig['cmdexit'] = self.getParam('cmdexit') % jobConfig
            if not jobConfig['cmdexit']:
                jobConfig['cmdexit'] = 'exit $status' % jobConfig
        else:
            jobConfig['cmdexit'] = ''
  
        # Generate string with comma-separated list of output files with full name
        jobConfig['outputfilestring'] = ','.join([jobConfig['outputfileprefix']+f for f in jobConfig['outputfilelist']])

        # Update remaining special parameters
        if jobConfig['taskpostprocsteps']:
            jobConfig['doneflag'] = jobConfig['postprocflag']
        else:
            jobConfig['doneflag'] = jobConfig['completedflag']
        jobConfig['script'] = self.getParam('script') % jobConfig

        # Check if job's output files exist already in order to prevent overwriting of data
        # NOTE: cannot just check for existence of jobdir, since this might be set to the
        #       current directory
        for f in jobConfig['outputfilelist']:
            if os.access(jobConfig['outputfileprefix']+f,os.F_OK):
                raise JobRunnerError, 'Job output file %s exists already' % jobConfig[f]
        for f in ('configfile', 'scriptfile', 'logfile'):
            if os.access(jobConfig[f],os.F_OK):
                raise JobRunnerError, 'Job configuration or log file %s exists already' % jobConfig[f]

        # Make sure start directory where script and config files will be written to exists
        os.makedirs('%(jobdir)s' % jobConfig) 

        # Write job configuration file
        config = open(jobConfig['configfile'],'w')
        config.write('# Job configuration data for job %(jobname)s\n' % jobConfig)
        config.write('# Generated by JobRunner.py\n\n')
        if pprint.isreadable(jobConfig):
            config.write('jobConfig = '+pprint.pformat(jobConfig))
        else:
            config.write('jobConfig = '+repr(jobConfig))
        config.write('\n')
        config.close()

        # Write script file
        script = open(jobConfig['scriptfile'],'w')
        script.write(jobConfig['script'])
        script.close()
        os.chmod(jobConfig['scriptfile'],0755)

        # Record job configuration
        self.jobs[jobnr] = jobConfig


    def configure(self):
        """Configure all jobs."""
        if self.getParam('filesperjob')==0 or not self.getParam('inputfiles'):
            raise JobRunnerError, "No input files or illegal parameter 'filesperjob'"
        lbperjob = self.getParam('lbperjob')
        if lbperjob:
            # Bunched job submission with all files from LB range in single job
            if lbperjob<=0:
                raise JobRunnerError, 'Negative number of luminosity blocks per job not allowed'
            inputfiles = self.getParam('inputfiles')
            jobInputDict = {}
            jobLBDict = {}
            lbpattern = re.compile('lb(\d+)')
            for f in inputfiles:
                lbnrs = lbpattern.findall(f)
                
                if(len(lbnrs) < 1) :
                    raise JobRunnerError, 'Unable to determine luminosity block number of file %s' % f

                if(len(lbnrs) > 2) :
                    raise JobRunnerError, 'Too many luminosity block numbers in filename %s' % f

                lbnr = int(lbnrs[0])
                if(len(lbnrs) > 1) :
                    lbnrmax = int(lbnrs[1])
                else :
                    lbnrmax = lbnr    

                while (lbnr <=  lbnrmax) :                    

                    jobId = int((lbnr-1)/lbperjob)
                    #print 'LB = %4i  jobid = %i' % (lbnr,jobId)
                    if not jobId in jobInputDict:
                        jobInputDict[jobId] = [f]
                        jobLBDict[jobId] = [lbnr]
                    else:
                        if not f in jobInputDict[jobId] :
                            jobInputDict[jobId].append(f)
                        if not lbnr in jobLBDict[jobId] :    
                            jobLBDict[jobId].append(lbnr)

                    lbnr = lbnr+1

            maxJobs = self.getParam('maxjobs')
            if not maxJobs:
                maxJobs = len(jobInputDict)
            for i in  sorted(jobInputDict.keys())[:maxJobs]:
                if i<0:
                    jobnr = 0
                else:
                    jobnr = i*lbperjob+1    # use first LB number as job number
                self.setParam('lbList', jobLBDict[i])
                self.configureJob(jobnr,jobInputDict[i])

        else:
            # Normal (unbunched job submission)
            njobs = int(math.ceil(float(len(self.getParam('inputfiles')))/self.getParam('filesperjob')))
            if self.getParam('maxjobs'):
                njobs = min(self.getParam('maxjobs'),njobs)
            print '\nConfiguring %i job(s) ...' % (njobs)
            for i in range(njobs):
                self.configureJob(i)


    def submitJob(self,jobConfig):
        """This method is to be overridden by dervied JobRunner classes. This default
           implementation in the JobRunner base class runs jobs directly, printing the
           output onto the termminal. If a specific implementation of submitJob directly
           runs a job, it should return the exit status of the job. If it only submits the
           job, the status should be returned as None."""
        scriptfile = jobConfig['scriptfile']
        logfile = jobConfig['logfile']
        exitstatus = jobConfig['exitstatus']
        # Write only to standard output, don't produce log file
        #status = subprocess.call(scriptfile, shell=True) >> 8   # Convert to standard Unix exit code
        # Write to both stdout and log file, preserve exit status code
        status = subprocess.call('%s 2>&1 | tee %s ; exit `cat %s`'
            % (scriptfile,logfile,exitstatus), shell=True) >> 8   # Convert to standard Unix exit code
        return status


    def runJob(self,jobnr):
        """Run a single configured job."""
        if not jobnr in self.jobs:
            raise JobRunnerError, 'Job number %s is not configured' % jobnr
        jobConfig = self.jobs[jobnr]
        subprocess.call('touch '+jobConfig['subflag'], shell=True)
        status = self.submitJob(jobConfig)
        self.jobStatus[jobnr] = status


    def run(self,jobnr=None):
        """Run all configured job(s)"""
        if not self.jobs:
            raise JobRunnerError, 'No configured jobs'
        njobs = len(self.jobs)
        self.log('JobRunner: starting '+str(njobs)+' jobs',self.dumpParams())
        for j in sorted(self.jobs.keys()):
            print '\nStarting job %i (using %s) ...\n' % (j,self.__class__)
            self.runJob(j)


    def getNJobs(self):
        if not self.jobs:
            return 0
        else:
            return len(self.jobs)


    def getRunningJobs(self):
        """Get list of jobs submitted by this JobRunner that are still running"""
        runningJobs = [ ]
        for j in self.jobs:
            if os.path.exists(self.jobs[j]['subflag']) or os.path.exists(self.jobs[j]['runflag']):
                runningJobs.append(self.jobs[j]['jobname'])
        return runningJobs


    def wait(self):
        """Wait until all jobs have completed"""
        while 1:
            time.sleep(30)
            runningJobs = self.getRunningJobs()
            if not runningJobs: break
            print
            print time.asctime(),' Waiting for %2s job(s) (%s)' % (len(runningJobs),runningJobs)
        self.log('JobRunner: finished',self.dumpParams(),outputFiles=self.getOutputFiles())


    def getOutputFiles(self):
        """Get a list of all output files."""
        outputFiles = [ ]
        for j in self.jobs:
            for f in sorted(glob.glob(self.jobs[j]['outputfileprefix']+'*')):
                outputFiles.append(f)
        return outputFiles


    def log(self,subject,body,**data):
        """Send a log message to a set of users specified by parameter 'logmail'."""
        if self.getParam('logmail'):
            msg = 'Generated by JobRunner on host '+socket.gethostname()+' by '+pwd.getpwuid(os.getuid())[0]+' at '+time.asctime()
            msg += '\n\nCurrent working directory = '+os.getcwd()
            if body: msg += '\n\n'+body
            for k in data.keys():
                if isinstance(data[k],dict):
                    msg += "\n\n%s:" % k
                    items = data[k].keys()
                    items.sort()
                    for i in items:
                        msg += "\n    %-20s = %s" % (i,data[k][i])
                else:
                    msg += "\n\n%-20s = %s" % (k,data[k])
            subprocess.call("echo '%s' | mail -s '%s' '%s'" %
                (msg,subject,self.getParam('logmail')), shell=True)


#
# Test code
#
if __name__ == '__main__':
    print __doc__
    print 'Testing JobRunner:'
    t = JobRunner()
    t.addFiles(['file1.dat','file2.dat','file3.dat'])
    t.showParams()
    #t.configure()
    #t.run()
