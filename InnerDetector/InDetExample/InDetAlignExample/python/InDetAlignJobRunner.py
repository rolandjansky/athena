# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Written by Juerg Beringer in April 2008.

import math
import os
import socket
import time
import subprocess


# Default template of job submission script
scriptTemplate = """#!/bin/sh
source %(cmthome)s/setup.sh -tag=%(release)s
mkdir -p %(rundir)s
cd %(rundir)s
athena.py %(configfile)s %(joboptionPath)s
"""


# Exception class
class InDetAlignJobRunnerError(AttributeError):
    """InDetAlignJobRunner exception class"""


class InDetAlignJobRunner:
    """InDetAlignJobRunner is a generic "job runner" class that can be used to run a
       number of jobs on a given set of input files in parallel on a batch system.
       Written by Juerg Beringer in April 2008."""


    def __init__(self,**options):
        """Constructor (takes any number of parameters as an argument)."""
        self.options = options
        self.optionDescription = {}
        self.setDefault('batchType','LSF','Type of batch system (LSF, background, configureOnly)')
        self.setDefault('batchQueue','8nm','Batch queue')
        self.setDefault('filesPerJob',1,'Number of input files per job')
        self.setDefault('joboptionPath',os.getcwd()+'/joboptions.py','Master joboption file, including full path')
        self.setDefault('release','13.2.0,opt,runtime','Software release (may include flags such as opt,runtime)')
        self.setDefault('cmthome','$HOME/cmthome','Location of cmthome directory')
        self.setDefault('jobnameTemplate','job%(jobnr)03i','Job name template (use %(jobnr)i for current job number, use <10 char for LSF)')
        self.setDefault('startdirTemplate',os.getcwd(),'Directory template from where the jobs are started')
        self.setDefault('rundirTemplate','.','Directory template where the jobs execute')
        self.setDefault('configfileTemplate','%(startdir)s/%(jobname)s-config.py','Job configuration file name template (use %(jobnr)i for current job number)')
        self.setDefault('scriptfileTemplate','%(startdir)s/%(jobname)s.sh','Script file name template (use %(jobnr)i for current job number)')
        self.setDefault('scriptTemplate',scriptTemplate,'Template of job submission script')
        self.setDefault('outputfileTemplate','%(startdir)s/%(jobname)s.root','Output file name template (use %(jobnr)i for current job number)')
        self.setDefault('histfileTemplate','%(startdir)s/%(jobname)s-hist.root','Histogram/ntuple file name template (use %(jobnr)i for current job number)')
        self.setDefault('logfileTemplate','%(startdir)s/%(jobname)s.log','Log file name template (use %(jobnr)i for current job number)')
        self.setDefault('logmail','','E-mail address for sending status e-mails')

        # The following will be regenerated for each job
        self.setDefault('jobnr',-1,'Current job number')
        self.setDefault('startdir',os.getcwd(),'Directory from where the jobs are started (generated)')
        self.setDefault('rundir','.','Directory where the jobs execute (generated)')
        self.setDefault('jobname','','Job name (generated)')
        self.setDefault('configfile','','Name of job configuration file (generated)')
        self.setDefault('outputfile','','Name of output file (generated)')
        self.setDefault('histfile','','Name of histogram/ntuple output file (generated)')
        self.setDefault('logfile','','Name of log file (generated)')
        self.setDefault('scriptfile','','Name of job submission script (generated)')
        self.setDefault('inputfiles',[],'List of input files (generated)')

        # Misc default values
        self.setDefault('EvtMax','-1','Maximum number of events to run over in each job')
        self.setDefault('alignmentFile','','File with alignment constants')
        self.setDefault('useNLumiBlocksPerBeamspot',1,'Number of LBs to use for each beamspot')
        self.setDefault('maxCountPerBeamspot',-1,'Maximum number of tracks/vertices to be used (or -1 for all)') 

        self.checkOptions()
        self.inputfiles = [ ]
        self.jobs = {}
        self.jobNames = {}


    def setDefault(self,name,value,desc=''):
        """Set a default option value with an optional option description"""
        self.options[name] = self.options.get(name,value)
        self.optionDescription[name] = desc


    def setOption(self,name,value):
        """Set an option value"""
        self.options[name] = value


    def checkOptions(self):
        """A generic routine for checking options. Intended to be implemented by
           a subclass if desired."""
        pass


    def showOptions(self):
        """Show current InDetAlignJobRunner options."""
        options = self.options.keys()
        options.sort()
        for o in options:
            print ("%-20s = %-30s  # %s" % (o,self.options[o],self.optionDescription.get(o,'')) )


    def addFiles(self,fileList):
        """ Add a list of input files to be processed."""
        self.inputfiles.extend(fileList)


    def configureJob(self,jobnr):
        """Configure options for job number jobnr."""
        if jobnr in self.jobs: raise InDetAlignJobRunnerError ('Job number %s already configured' % jobnr)

        # Check if any input files are available for this job
        iFirst = self.options['filesPerJob']*jobnr
        if iFirst >= len(self.inputfiles): raise InDetAlignJobRunnerError ('jobnr too high for available number of files')
        iLast = iFirst + self.options['filesPerJob']
        if iLast > len(self.inputfiles): iLast=len(self.inputfiles)

        # Update options for current job
        self.options['jobnr'] = jobnr
        self.options['inputfiles'] =  self.inputfiles[iFirst:iLast]
        for p in ('jobname', 'startdir', 'rundir', 'configfile', 'scriptfile', 'outputfile', 'histfile', 'logfile'):
            self.options[p] = self.options[p+'Template'] % self.options
        
        # Check if job's output files exist already in order to prevent overwriting of data
        for f in ('outputfile', 'histfile', 'logfile'):
            if os.access(self.options[f],os.F_OK): raise InDetAlignJobRunnerError ('File %s exists already' % self.options[f])

        # Make sure start directory where script and config files will be written to exists
        os.system('mkdir -p %(startdir)s' % self.options)

        # Write job files
        self._writeConfig()
        self._writeScript()

        # Record job configuration
        self.jobs[jobnr] = self.options.copy()
        self.jobNames[self.options['jobname']] = jobnr


    def _writeConfig(self):
        """Write job configuration file for current job."""
        config = open(self.options['configfile'],'w')
        config.write('# Job configuration data for job %(jobname)s\n' % self.options)
        config.write('# Generated by InDetAlignJobRunner.py\n\n')
        config.write('jobConfig = '+repr(self.options))
        config.write('\n')
        config.close()


    def _writeScript(self):
        """Write job execution script for current job."""
        script = open(self.options['scriptfile'],'w')
        script.write(self.options['scriptTemplate'] % self.options)
        script.close()
        os.chmod(self.options['scriptfile'],0o755)


    def submitBackground(self,jobnr):
        """Execute a configured job in the background"""
        if jobnr not in self.jobs: raise InDetAlignJobRunnerError ('Job number %s is not yet configured' % jobnr)
        scriptfile = self.jobs[jobnr]['scriptfile']
        logfile = self.jobs[jobnr]['logfile']
        os.system(scriptfile+' >& '+logfile+' &')


    def submitLSF(self,jobnr):
        """Execute a configured job as a LSF batch job"""
        if jobnr not in self.jobs: raise InDetAlignJobRunnerError ('Job number %s is not yet configured' % jobnr)
        batchCmd = 'bsub -q %(batchQueue)s -J %(jobname)s -o %(logfile)s %(scriptfile)s' % self.jobs[jobnr]
        print (batchCmd)
        os.system(batchCmd)


    def run(self):
        """Run all jobs either in batch or in the background, as specified by option batchType."""
        batchType = self.options['batchType']    # Currently the same for all jobs
        if batchType not in ('LSF','background','configureOnly'): raise InDetAlignJobRunnerError ('Cannot run job type %s' % type)
        filesPerJob = self.options['filesPerJob']
        njobs = int(math.ceil(float(len(self.inputfiles))/filesPerJob))
        self.options['njobs'] = njobs
        for i in range(njobs):
            print ('\nSubmitting job %s (using batchType=%s) ...' % (i,batchType))
            self.configureJob(i)
            if batchType=='LSF': self.submitLSF(i)
            if batchType=='background': self.submitBackground(i)
        self.log('InDetAlignJobRunner: starting jobs','',inputfiles=self.inputfiles,options=self.options)


    def getRunningLSFJobs(self):
        """Get list of jobs submitted by this InDetAlignJobRunner that are still running"""
        runningJobs = [ ]
        jobnamecol = subprocess.getoutput("bjobs 2>&1 | tail +2 | cut -c 60- | awk '{print $1}'").split()
        #jobnamecol = subprocess.getoutput("cat bjobs.log | tail +2 | cut -c 60- | awk '{print $1}'").split()
        for j in jobnamecol:
            if j in self.jobNames: runningJobs.append(j)
        return runningJobs


    def wait(self):
        """Wait until all jobs have completed (currently only implemented for LSF batch jobs)."""
        batchType = self.options['batchType']    # Currently the same for all jobs
        if batchType != 'LSF': raise InDetAlignJobRunnerError ('wait() is not yet implemented for %s' % batchType)
        while 1:
            time.sleep(30)    # Give LSF time to see recently submitted jobs
            runningJobs = self.getRunningLSFJobs()
            if not runningJobs: break
            print()
            print (time.asctime(),' Waiting for %2s job(s) (%s)' % (len(runningJobs),runningJobs))
        self.log('InDetAlignJobRunner: finished','',outputfiles=self.getOutputFiles())


    def getOutputFiles(self):
        """Get a list of all output files."""
        outputFiles = [ ]
        for i in self.jobs:
            outputFiles.append(self.jobs[i]['outputfile'])
        return outputFiles


    def log(self,subject,body,**data):
        if self.options['logmail']:
            msg = 'Generated by InDetAlignJobRunner on host '+socket.gethostname()+' by '+os.getlogin()+' at '+time.asctime()
            msg += '\n\nCurrent working directory = '+os.getcwd()
            if body: msg += '\n\n'+body
            for k in data.keys():
                if isinstance(data[k],dict):
                    msg += "\n\n%s:" % k
                    items = data[k].keys()
                    items.sort()
                    for i in items:
                        msg += "\n    %-30s = %s" % (i,data[k][i])
                else:
                    msg += "\n\n%-15s = %s" % (k,data[k])
            #print (msg)
            os.system('echo "%s" | mail -s "%s" "%s"' % (msg,subject,self.options['logmail']))


#
# Test code
#
if __name__ == '__main__':
    print ('Testing InDetAlignJobRunner:')
    t = InDetAlignJobRunner(batchType='configureOnly')
    t.addFiles([1,2,3])
    t.showOptions()
    t.run()
    #t.log("test","my message",options=t.options,inputFiles=t.inputfiles)
