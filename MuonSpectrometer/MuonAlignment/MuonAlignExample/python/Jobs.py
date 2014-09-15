# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import time
import commands
import sys
from threading import Thread

#-----------------------------------------------------------------------
class JobThread(Thread):
    """Class to run jobs locally in parallel threads."""
    
    def __init__ (self,jobstring):
        Thread.__init__(self)
        self.job = jobstring
        self.status = -1
    def run(self):
        print "  -> start thread" # % (self.job)
        process=os.popen(self.job)
        while 1:
            line = process.readline() 
            if not line:
                self.status=1
                break 

#-----------------------------------------------------------------------
class LocalJobList:
    """Class which handles locally running jobs."""
    
    def __init__(self,nJobs):
        self.nJobs = nJobs
        self.jobList = []    

    def process(self,jobfile,logfilename):
        print "  Run job locally: ",jobfile.name
        pipestr = "2>&1 >"
        if self.nJobs == 1:
            pipestr = "2>&1 |tee "
        jobstring = "sh %s %s %s\n" % (jobfile.name, pipestr, logfilename)
        print "jobstring: ",jobstring 
        if self.nJobs == 1:
            os.system(jobstring)
        else:    
            thread = JobThread(jobstring)
            self.jobList.append(thread)
            thread.start()

    def wait(self,logfilename):
        tmpJobList=self.jobList[:]
        for job in tmpJobList:
            while job.status!=1 :
                print "  waiting..."
                time.sleep(30)
            self.jobList.remove(job)

    def check(self,logfilename):
        return 1

#-----------------------------------------------------------------------

class SgeJobList:
    """Class which handles jobs running in the SGE batch."""

    def __init__(self,queue):
        self.queue = queue
        self.jobList = []    

    def qsub(self,submitstring):
        ret, out = commands.getstatusoutput(submitstring)
        if not ret==0:
            print "qsub returned error:\n", out
            return -1
        items=out.split()
        if not len(items) > 2:
            print "cannot parse output\n", out
            return -1
        try:
            return int(items[2])
        except ValueError:
            print "cannot parse output\n", out
            return -1

    def qstat(self,jobId):
        qstatstring = "qstat -u " + os.environ["USER"]
        #print "qstatstring: ",qstatstring
        ret, out = commands.getstatusoutput(qstatstring)
        if ret!=0:
            print "qstat failed:\n", out
            return -1
        for line in out.split("\n"):
            items=line.split()
            if len(items)<5 or items[0]!=str(jobId):
                continue
            if( items[4].find("E")!=-1):
                print "job %s returns error on batch" % jobId
                return -1
            return 0
        return 1
  
    def process(self,jobfile,logfilename):
        print "  Submit job to batch:",jobfile.name
        submitstring = 'qsub -q '+self.queue+' -j yes -o '+logfilename+ ' ' + jobfile.name
        #print "submitstring: ",submitstring 
        jobId = self.qsub(submitstring)
        if jobId != -1:
            self.jobList.append(jobId)
            print "  successfully submitted job:",jobId
        else:
            print "  could not submit job:",jobfile.name

    def wait(self,logfilename):
        tmpJobList=self.jobList[:]
        for jobId in tmpJobList:
            #qstatstring = "qstat -q %s -j %d" % (self.queue,jobId)
            #print "qstatstring: ",qstatstring
            #while  commands.getstatusoutput(qstatstring)[0] == 0:    
            while self.qstat(jobId) == 0:    
                print "  waiting..."
                time.sleep(30)
            self.jobList.remove(jobId)
                
    def check(self,logfilename):
        return 1

#-----------------------------------------------------------------------

class PbsJobList:
    """Class which handles jobs running in the Torque/PBS batch."""

    def __init__(self,queue):
        self.queue = queue
        self.jobList = []

    def qsub(self,submitstring):
        ret, out = commands.getstatusoutput(submitstring)
        if not ret==0:
            print "qsub returned error:\n", out
            return -1
        items=out.split(".")
        if not len(items) > 0:
            print "cannot parse output\n", out
            return -1
        try:
            return int(items[0])
        except ValueError:
            print "cannot parse output\n", out
            return -1

    def qstat(self,jobId):
        qstatstring = "qstat -u " + os.environ["USER"]
        ret, out = commands.getstatusoutput(qstatstring)
        if ret!=0:
            print "qstat failed:\n", out
            return -1
        for line in out.split("\n"):
            items=line.split()
            if len(items)<10:
                continue
            if items[0].find(".")==-1:
                continue
            idstring= items[0].split(".")[0]
            if int(idstring)!=jobId:
                continue
            if items[9].find("E")!=-1:
                print "job %s returns error on batch" % jobId
                return -1
            return 0
        return 1
  
    def process(self,jobfile,logfilename):
        print "  Submit job to batch:",jobfile.name
        submitstring = 'qsub -q '+self.queue+' -j oe -o '+logfilename+ ' ' + jobfile.name
        #print "submitstring: ",submitstring 
        jobId = self.qsub(submitstring)
        if jobId != -1:
            self.jobList.append(jobId)
            print "  successfully submitted job:",jobId
        else:
            print "  could not submit job:",jobfile.name

    def wait(self,logfilename):
        tmpJobList=self.jobList[:]
        for jobId in tmpJobList:
            while self.qstat(jobId) == 0:
                print "  waiting..."
                time.sleep(30)
            self.jobList.remove(jobId)
                
    def check(self,logfilename):
        return 1

#-----------------------------------------------------------------------

class LxbatchJobList:
    """Class which handles jobs running in the lxbatch system."""

    def __init__(self,queue):
        self.queue = queue
        self.jobList = []    

    def bsub(self,submitstring):
        ret, out = commands.getstatusoutput(submitstring)
        if not ret==0:
            print "bsub returned error:\n", out
            return -1
        items=out.split()
        if len(items) != 7:
            print "cannot parse output\n", out
            return -1
        try:
            jobId=items[1].replace("<","").replace(">","")
            return int(jobId)
        except ValueError:
            print "cannot parse output\n", out
            return -1

    def bjobs(self,jobId):
        bjobsstring = "bjobs -u " + os.environ["USER"]
        ret, out = commands.getstatusoutput(bjobsstring)
        if ret!=0:
            print "bjobs failed:\n", out
            return -1
        for line in out.split("\n"):
            items=line.split()
            if items[0]!=str(jobId):
                continue
            #if( items[4].find("E")!=-1):
            #    print "job %s returns error on batch" % jobId
            #    return -1
            return 0
        return 1
  
    def process(self,jobfile,logfilename):
        print "  Submit job to batch:",jobfile.name
        submitstring = 'bsub -q '+self.queue+' -o '+logfilename+ ' ' + jobfile.name
        #submitstring = ''
        #print "submitstring: ",submitstring 
        jobId = self.bsub(submitstring)
        if jobId != -1:
            self.jobList.append(jobId)
            print "  successfully submitted job:",jobId
        else:
            print "  could not submit job:",jobfile.name

    def wait(self,logfilename):
        tmpJobList=self.jobList[:]
        for jobId in tmpJobList:
            #qstatstring = "qstat -q %s -j %d" % (self.queue,jobId)
            #print "qstatstring: ",qstatstring
            #while  commands.getstatusoutput(qstatstring)[0] == 0:    
            print "jobId=",jobId
            while self.bjobs(jobId) == 0:    
                print "  waiting..."
                time.sleep(30)
            self.jobList.remove(jobId)

    def check(self,logfilename):
        ret, out=commands.getstatusoutput("ls %s.gz" % logfilename)
        if ret==0:
            os.system("gunzip %s.gz" % (logfilename))
        ret, out=commands.getstatusoutput("ls %s" % logfilename)
        if ret==0:
            grepstring = "grep \'leaving with code 0\' %s" % (logfilename)
            ret, out=commands.getstatusoutput(grepstring)
            os.system("gzip %s" % (logfilename))
            if (len(out)>0):
                return 1
        return 0

#-----------------------------------------------------------------------

class PathenaJobList:
    """Class which handles jobs running in pathena."""

    def __init__(self,doPrun=False):
        self.jobId=-99
        self.nCPUs=1
        self.prun=doPrun

    def bjobs(self) :
        if self.jobId==-99:
            print "need jobId"
            sys.exit(3)

        if not self.prun:
            print "Pathena bjobs(), jobId: ",self.jobId
        else:
            print "Pathena bjobs(), jobId: ",self.jobId

        bjobstring = "pbook -c 'show(" + str(self.jobId) + ")'"
        ret, out = commands.getstatusoutput(bjobstring)
        for line in out.split("\n"):
            items=line.split()
            if len(items)>0 and items[0]!='jobStatus':
                continue
            if len(items)>2 :
                if items[2]!='frozen':
                    return 0
                else :
                    print "jobStatus: ",items[2]
        return 1
                
    def process(self,jobfile,logfilename):
        
        if not self.prun:
            print "  Submitting pathena job: ",jobfile.name
        else:
            print "  Submitting prun job: ",jobfile.name

        pipestr = "2>&1 |tee "
        jobstring = "sh %s %s %s\n" % (jobfile.name, pipestr, logfilename)
        os.system(jobstring)

        # run sync() to write jobID to logfile
        jobstring2 = "pbook -c 'sync()' %s %s\n" % (pipestr,logfilename)
        os.system(jobstring2)

        # get jobID from logfile
        ret, out = commands.getstatusoutput("cat "+logfilename)
        lines=out.split('\n')
        for line in lines:
            items=line.split()
            if len(items)>3 and items[3].startswith("JobID") :
                self.jobId=items[3][6:]
                print "jobId=",self.jobId        
        
        # get number of jobs from logfile
        jobstring3 = "pbook -c 'show(%s)' %s %s\n" % (self.jobId,pipestr,logfilename)
        os.system(jobstring3)

        ret, out = commands.getstatusoutput("cat "+logfilename)
        lines=out.split('\n')
        for line in lines:
            items=line.split()
            if len(items)>3 and items[3].startswith("nJobs") :
                self.nCPUs=items[2]
                print "nCPUs=",self.nCPUs        
        
    def wait(self,logfilename):
        print "Pathena wait(), logfilename=",logfilename 

        # get jobID from logfile
        if self.jobId==-99:
            print "getting jobId"            
            ret, out = commands.getstatusoutput("cat "+logfilename)
            lines=out.split('\n')
            for line in lines:
                items=line.split()
                print "items: ",items
                if items[0]=="JobID":
                    self.jobId=items[2]
                    break                        
            if self.jobId==-99:
                sys.exit(3)

            # get number of jobs from logfile
            logfilename2=logfilename+".pbook"
            pipestr = "2>&1 |tee "
            jobstring3 = "pbook -c 'show(%s)' %s %s\n" % (self.jobId,pipestr,logfilename2)
            os.system(jobstring3)
            ret, out = commands.getstatusoutput("cat "+logfilename2)
            lines=out.split('\n')
            for line in lines:
                items=line.split()
                if (len(items)>3 and items[0]=="nJobs") :                       
                    self.nCPUs=items[2]
                    break
        print "jobId=",self.jobId
        print "nCPUs=",self.nCPUs        

        while self.bjobs() == 0:
            print " waiting for jobID ",self.jobId,"..."
            time.sleep(360)

    def check(self,logfilename):
        return 1
