# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import subprocess,sys,os

def getJobIDfromlastJob():
    for line in open("qsub.log"):
        pass
    parts = line.split(" ")
    return parts[3].split("\n")[0]

from . import batchJobBase

class batchJob(batchJobBase.batchJobBase):

    def submit(self, dryRun=False):
        cpt = 1
        if self.memMB == 1499:
            self.memMB = 1970
        elif self.memMB == 2499:
            #ram = 5250
            self.memMB = 2583
            cpt = 2
        cmd = "sbatch -e "+self.basedir+"/"+self.name+".log -o "+self.basedir+"/"+self.name+".log --time="+str(self.hours)+":00:00 --mem-per-cpu="+str(self.memMB)+" -p haswell,romeo --cpus-per-task="+str(cpt)+" -N 1 --ntasks "+str(self.nCores)
        if os.environ.get('SLURM_MAIL_USER'):
            cmd +=  " --mail-type='END,FAIL,TIME_LIMIT' --mail-user='"+os.environ.get('SLURM_MAIL_USER')+"'"

        if len(self.dependsOnOk)>0 or len(self.dependsOnAny)>0:
            cmd += " -d "
            if len(self.dependsOnOk)>0:
                okdeps = "afterok:"+",afterok:".join(self.dependsOnOk)
                cmd += okdeps+","
            if len(self.dependsOnAny)>0:
                anydeps = "afterany:"+",afterany:".join(self.dependsOnAny)
                cmd += anydeps+","
            cmd = cmd[:-1]

        if "ecm" in self.basedir:
            jobname = os.path.relpath(self.basedir+"/"+self.name+".log", self.basedir+"/../..")
        else:
            jobname = os.path.relpath(self.basedir+"/"+self.name+".log", self.basedir+"/..")
        cmd += " -J "+jobname+" "+self.basedir+"/"+self.name+".sh >> qsub.log \n"

        if dryRun:
            print (cmd)
            self.id = "-1"
        else:
            print(cmd)
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

            p.wait()

            self.id = getJobIDfromlastJob()

        print("Submitted "+str(self.id)+" ("+self.name+")\n\n")

def finalizeJobs(dryRun):
    return True

