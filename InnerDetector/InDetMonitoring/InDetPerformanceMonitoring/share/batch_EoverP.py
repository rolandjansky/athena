#!/bin/python

################################################################################
#
# This script allows the IDPerfMonEoverP code to be run in parallel on lxbatch
#
# In principle it should be easily adapted to work with any of the IDPerfMon 
#   codes with small changes to the relevant JobOptions file and calling it
#   from this script
#
# The user collects a list of data files to run over, and decides how many files
#   per sub-job.  For each sub-job the code then writes a bash script called 
#   runner<job_number>.sh in a configurable output directory and submits it to 
#   the lxbatch.
#
# runner.sh executes the jobOptions file 'runEoverPValidationBatch.py', which
#   is identical to the normal runEoverPValidationData.py except that it is
#   hardcoded to look for the input data files in "inputfiles.txt"
#
# runner.sh also copies the stdout and eoverpValidationOut.root files to the
#   output directory with a name unique to the LSF job ID (so nothing gets
#   accidentally overwritten).
#
# Instructions:
#   python batch_EoverP.py --arguments
#
# @author: will.di.clemente@cern.ch
#
################################################################################

import sys
import os
import errno
from subprocess import call

def submit(job_number,job_path,queue):
    # do the bsub
    print "Submitting job #{0} to queue {1}. . .".format(job_number,queue)
    # do the thing
    args = ["bsub","-q",queue,"{0}runner{1}.sh".format(job_path,job_number)]
    call(args)
    print ". . . Submitted"
    return

def getConfig():
    from optparse import OptionParser
    p = OptionParser()
    p.add_option("--jobTitle", "-j", dest="jobTitle", help="Name of the output directory (will be created if it doesn't exist)", default="EoverP")
    p.add_option("--inputList", "-i", dest="inputList", help="Text file with paths to the data files to be used", default="test.txt")
    p.add_option("--nFilesPerJob", "-n", dest="nFilesPerJob", type="int", help="Number of files per subjob", default=1)
    p.add_option("--queue", "-q", dest="queue", help="Which lxbatch queue to submit to", default='atlasb1')
    p.add_option("--athena", "-a", dest="athRel", help="Which Athena release do you want", default='20.7.7.9')
    (config, sys.argv[1:]) = p.parse_args(sys.argv[1:])
    return config
    
def writeRunner(dataFiles,path,jobNum,athrel):
    # write a shell scrpt that we will submit to lxbatch
    filename = '{0}runner{1}.sh'.format(path,jobNum)
    f = open(filename,'w')
    # write work space setup
    f.write('## setup working space\n'+
            'mkdir myWorkDir\n'+
            'echo $PWD\n'+
            'ls\n'+
            'cd myWorkDir\n'+
            'export WORKDIR=`pwd`\n\n')
    # create input data list
    f.write('echo "creating input data list"\n')
    #print dataFiles
    for file in dataFiles:
        f.write('echo ' + 
                file +
                ' >> inputfiles.txt\n')
    # write athena setup
    testarea = os.getenv('TestArea', os.environ['HOME'])
    f.write('echo "setup Athena"\n'+
            "alias asetup='source $AtlasSetup/scripts/asetup.sh'\n"+
            'ATHREL="{0}"\n'.format(athrel)+
            'export TESTAREA={0}\n'.format(testarea)+
            'cd ${TESTAREA}\n'+
            'asetup ${ATHREL},here\n'+
            'cd ${WORKDIR}\n\n')
    # write JO copy
    f.write('echo "copy JO"\n'+
            'cp ${TESTAREA}/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/runEoverPValidationBatch.py .\n\n')
    ### xrootd patch for athena version 20.7.7.X
    f.write('source /afs/cern.ch/work/w/wdicleme/public/Alignment/20.7.8/linker.sh\n\n')
    ###
    # write execute JO
    f.write('echo "running the code"\n'+
            'athena runEoverPValidationBatch.py '+
            ' > athena_out.log\n\n')
    #write copy results
    #directory = path+"/"+jobTitle
    #f.write('echo "Creating output directory if it does not arleady exist"\n'+
    #        'if [ ! -d '+directory+' ]\n'+
    #        'then\n'+
    #        '\tmkdir '+directory+
    #        '\nfi\n\n')
    f.write('echo "Copy results somewhere useful"\n'+
            'filepath=`pwd`\n'+
            'parentname="$(basename "$(dirname "$filepath")")"\n'+
            'echo "$parentname"\n')
    identifier = "Part{0}-LSFJOB_{1}".format(jobNum,'$parentname')
    f.write('cp athena_out.log ' + 
            '{0}athena_out-{1}.log\n'.format(path,identifier)) # athena out
    f.write('if [ -e eoverpValidationOut.root ] ; then\n'+
            'cp eoverpValidationOut.root '+
            '{0}eoverpValidationOut-{1}.root\n'.format(path,identifier)+
            'fi\n\n') # root files
    #cleanup
    f.write('ls -laR\n'+
            'cd ${WORKDIR}\n'+
            'cd ..\n'+
            'rm -rf myWorkDir')
    f.close()
    call(["chmod","777",filename])

if __name__ == '__main__':
    # command-line parsing
    config = getConfig()
    jobTitle = config.jobTitle
    inputList = config.inputList
    nFilesPerJob = config.nFilesPerJob
    athRel = config.athRel
    queue = config.queue

    # figure out how many jobs
    files = []
    nFiles = 0
    for line in open(inputList):
        line = line.strip()
        files.append(line)
        #nFiles = nFiles + 1
        nJobs = -1
    nFiles = len(files)
    print "Input file list has {0} files.".format(nFiles)
    if nFilesPerJob >= nFiles:
        nJobs = 1
    elif (nFiles%nFilesPerJob) == 0:
        nJobs = nFiles/nFilesPerJob
    else:
        nJobs = nFiles/nFilesPerJob + 1
    print "User input requested {0} files per job, submitting {1} subjobs to queue {2}".format(nFilesPerJob,nJobs,queue)
    
    # create output directory if it does not already exist
    path = os.getcwd()+"/{0}/".format(jobTitle)
    print "creating output directory: "+path
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise
        else:
            print "Directory already exists! Not making it (obviously)..."

    for job in range(nJobs):
        if (job+1)*nFilesPerJob > nFiles:
            writeRunner(files[job*nFilesPerJob:],path,job,athRel)
        else:
            writeRunner(files[job*nFilesPerJob:(job+1)*nFilesPerJob],path,job,athRel)
        submit(job,path,queue)
