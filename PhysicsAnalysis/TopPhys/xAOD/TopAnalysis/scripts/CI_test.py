#! /usr/bin/env python
# This script is designed to be used as a quick runtime ctest in the CI framework
# We use python to better handle getting our validation cutfile which should be stable
# We will augment the NEvents line in our cutfile to limit the job
# This is primarily to catch any runtime changes introduced before they are merged

import sys, subprocess, random, os, shlex, ROOT, shutil

def CITest(testname, cutfile, cutfilepath, inputfile, sedreplace):
    """ Templated test to allow extension """
    print("CI Integration Test :",testname)

    # -- Settings --
    cutfilename   = cutfile
    inputfilename = inputfile

    # -- Move to a unique directory --
    cwd = os.getcwd()
    rundir = str(random.randrange(10**8))
    os.mkdir(rundir)
    os.chdir(rundir)

    # -- Get the validation file path from the most recent location --

    # -- Print the file location for debugging --
    print("CI Integration Test : Using cutfile %s"%(cutfilepath))
    print("CI Integration Test : Using inputfile %s"%(inputfilename))

    # -- Copy the cutfile locally to be updated -- 
    shutil.copyfile(cutfilepath, cutfilename)

    # -- Write the input file path to a temporary file --
    inputfilepath = open("input.txt","w")
    inputfilepath.write(inputfilename+"\n")
    inputfilepath.close()

    # -- Edit the cutfile to set any provided options --
    for (old, new) in sedreplace:
        cmd  = "sed -i -e 's/"+old+"/"+new+"/g' %s"%(cutfilename)
        proc = subprocess.Popen(shlex.split(cmd))
        proc.wait()

    # -- Run top-xaod --
    cmd  = "top-xaod %s input.txt"%(cutfilename)
    proc = subprocess.Popen(shlex.split(cmd))
    proc.wait()

    # Return to starting directory
    os.chdir(cwd)

    # -- Check the return code and exit this script with that --
    return proc.returncode
