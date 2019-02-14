#! /usr/bin/env python
# This script is designed to be used as a quick runtime ctest in the CI framework
# We use python to better handle getting our validation cutfile which should be stable
# We will augment the NEvents line in our cutfile to limit the job
# This is primarily to catch any runtime changes introduced before they are merged

import sys, subprocess, random, os, shlex, ROOT, shutil
from PathResolver import PathResolver


def CITest(testname, cutfile, cutfilepath, inputfile, sedreplace):
    """ Templated test to allow extension """
    print "CI Integration Test :",testname

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
    print "CI Integration Test : Using cutfile %s"%(cutfilepath)
    print "CI Integration Test : Using inputfile %s"%(inputfilename)

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

# -- Main script -- #

# -- Get the validation file path from the most recent location --                                                                                                               
cutfilename = "validation-cuts.txt"
cutfilepath = ROOT.PathResolver.find_file(cutfilename,
                                          "DATAPATH",
                                          ROOT.PathResolver.RecursiveSearch)

returnCode = CITest("TOPQ1 MC",
                    cutfilename,
                    cutfilepath,
                    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/ContinuousIntegration/MC/p3731/"+
                    "mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6337_s3126_r9364_p3731/DAOD_TOPQ1.16479625._003348.pool.root.1",
                    [("#NEvents.*","NEvents 100")])

if returnCode != 0:
    print "Error in TOPQ1 MC"
    sys.exit(returnCode)

returnCode = CITest("TOPQ1 DATA",
                    cutfilename,
                    cutfilepath,
                    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/ContinuousIntegration/Data/p3730/"+
                    "Data_16_p3730/DAOD_TOPQ1.16771725._000150.pool.root.1",
                    [("#NEvents.*","NEvents 3000")])

if returnCode != 0:
    print "Error in TOPQ1 Data"
    sys.exit(returnCode)

# -- Return 0 as all tests were successful -- #
sys.exit(0)
