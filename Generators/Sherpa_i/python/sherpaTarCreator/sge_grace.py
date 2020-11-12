# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import subprocess,sys,os,re

def getJobIDfromlastJob():
    for line in open("qsub.log"):
        pass
    parts = line.split(" ")
    jobid = parts[2].split('.')[0] if 'array' in line else parts[2]
    return jobid

from batchJobBase import *

class batchJob(batchJobBase):

    def submit(self, dryRun=False):

        cmd = "qsub -e "+self.basedir+"/"+self.name+".log -o "+self.basedir+"/"+self.name+".log"

        if len(self.dependsOnOk)>0 or len(self.dependsOnAny)>0:
            cmd += " -hold_jid "
            if len(self.dependsOnOk)>0:
                #okdeps = "afterok:"+",afterok:".join(self.dependsOnOk)
                #cmd += okdeps+","
                cmd += str(self.dependsOnOk[-1])
            if len(self.dependsOnAny)>0:
                #anydeps = "afterany:"+",afterany:".join(self.dependsOnAny)
                #cmd += anydeps+","
                cmd += str(self.dependsOnAny[-1])

        if "ecm" in self.basedir:
            jobname = os.path.basename(os.path.normpath(self.basedir+"/../"))+"_"+self.name
        else:
            jobname = os.path.basename(os.path.normpath(self.basedir))+"_"+self.name
        #cmd += " -J "+jobname+" "+self.basedir+"/"+self.name+".sh" # >> qsub.log \n"

        user = os.environ.get('USER')
        cmd += " >> qsub.log <<EOF \n"
        cmd += "#!/bin/bash -l\n"
        cmd += "set -e;\n"
        cmd += "#$ -S /bin/bash\n"
        cmd += "#$ -l h_rt=%i:0:0\n" % self.hours
        cmd += "#$ -l mem=%iG\n" % int(round(0.001*self.memMB))
        cmd += "#$ -N j%s\n" % jobname
        cmd += "#$ -pe mpi %i\n" % self.nCores
        cmd += "#$ -wd %s\n" % self.basedir
        cmd += "cd %s;\n" % self.basedir
        cmd += "ulimit -f 3000000;\n"
        cmd += "export PARROT_CVMFS_ALIEN_CACHE=/lustre/scratch/scratch/%s/cvmfs_cache/\n" % user
        cmd += 'export PARROT_CVMFS_REPO="<default-repositories> url=http://cvmfs-atlas-nightlies.cern.ch/cvmfs/atlas-nightlies.cern.ch"\n'
        cmd += "module unload compilers mpi\n"
        cmd += "module load compilers/gnu/4.9.2\n"
        cmd += "module load cctools/7.0.11/gnu-4.9.2\n"
        cmd += "module load mpi/openmpi/1.8.4/gnu-4.9.2\n"
        cmd += "module load python/2.7.12\n"
        cmd += "module load python2/recommended\n"
        cmd += "module load scons/2.3.4\n"
        cmd += "module load texinfo/6.6\n"
        mpiwrap = 'gerun ' if self.nCores > 4 else ''
        cmd += "(parrot_run bash -c \'"+mpiwrap+self.basedir+"/"+self.name+".sh\') >> "+self.basedir+"/"+self.name+".log;\n"
        cmd += "exit 0;\n"
        cmd += "EOF\n"

        if dryRun:
            print (cmd+"\n")
            self.id = -1
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

            p.wait()
            self.id = getJobIDfromlastJob()

        print("Submitted "+str(self.id)+" ("+self.name+")")


def finalizeJobs(dryRun):
    return True
