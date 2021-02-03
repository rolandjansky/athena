# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import subprocess,sys,os

joblist = []
dependencelist = {} #"id":["childid"]

from . import batchJobBase

class batchJob(batchJobBase.batchJobBase):

    def submit(self):
        if self.memMB == 1:
            self.memMB = 2000
        elif self.memMB == 2:
            self.memMB = 6000

        self.id=os.path.relpath(self.basedir+"/"+self.name+".sh", "../")
        self.id = self.id.replace(".","-")

        joblist.append(self)
        for parentid in self.dependsOnOk+self.dependsOnAny:
            dependencelist.setdefault(parentid, []).append(self.id)

        #create submit file
        submitfile = ""
        submitfile += "executable = "+self.basedir+"/"+self.name+".sh\n"
        submitfile += "output = "+self.basedir+"/"+self.name+".log\n"
        submitfile += "error = "+self.basedir+"/"+self.name+".log\n"
        submitfile += "log = outHTC.log\n"
        submitfile += "+RequestRuntime = "+str(int(self.hours*3600))+"\n"
        submitfile += "RequestCpus = "+str(max(self.nCores,int(self.memMB/2000.)))+"\n"
        submitfile += "queue\n"
        with open(self.basedir+"/"+self.name+".sub", 'w') as f:
            f.write(submitfile)
        print("Created "+str(self.id)+" ("+self.basedir+"/"+self.name+".sub")

def finalizeJobs(dryRun):

    #create dagfile
    dagfile = ""
    for job in joblist:
        dagfile += "JOB "+job.id+" "+job.basedir+"/"+job.name+".sub\n"

    for parent,children in dependencelist.items():
        dagfile += "PARENT "+parent+" CHILD"
        for child in children:
            dagfile += " "+child
        dagfile += "\n"
    
    #save file
    with open("dagfile.dag", 'w') as f:
        f.write(dagfile)

    #submit job
    batchname = os.path.basename(os.path.normpath(os.getcwd()))
    cmd = "condor_submit_dag -force -batch-name "+batchname+" dagfile.dag"

    if dryRun:
        print (cmd+"\n")
        print ("dagfile: \n"+dagfile)
    else:
        print (cmd)
        p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        retcode = p.communicate()
        if len(retcode[0]):
            print (retcode[0])
        if len(retcode[1]):
            print (retcode[1])
        if p.returncode:
            print ("ERROR: error while submitting job")
            print ("return code: " + str(p.returncode))
            sys.exit(11)

