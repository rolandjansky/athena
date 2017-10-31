#! /usr/bin/env python
# This script is designed to be used as a quick runtime ctest in the CI framework
# We use python to better handle getting our validation cutfile which should be stable
# We will augment the NEvents line in our cutfile to limit the job
# This is primarily to catch any runtime changes introduced before they are merged

import sys, subprocess, random, os, shlex, ROOT, shutil
from PathResolver import PathResolver

# -- Settings --
cutfilename   = "validation-cuts.txt"
inputfilename = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TopAnalysis/p3215/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e5458_s3126_r9364_r9315_p3215/DAOD_TOPQ1.11627611._000441.pool.root.1"

# -- Move to a unique directory --
rundir = str(random.randrange(10**8))
os.mkdir(rundir)
os.chdir(rundir)

# -- Get the validation file path from the most recent location --
cutfilepath   = ROOT.PathResolver.find_file(cutfilename,
                                            "DATAPATH", 
                                            ROOT.PathResolver.RecursiveSearch)

# -- Print the file location for debugging --
print "runtime_ctest : Using cutfile (%s) from %s"%(cutfilename, cutfilepath)

# -- Copy the cutfile locally to be updated -- 
shutil.copyfile(cutfilepath, cutfilename)

# -- Write the input file path to a temporary file --
inputfilepath = open("input.txt","w")
inputfilepath.write(inputfilename+"\n")
inputfilepath.close()

# -- Edit the cutfile to set NEvents to 10 --
cmd  = "sed -i -e 's/#NEvents.*/NEvents 50/g' %s"%(cutfilename)
proc = subprocess.Popen(shlex.split(cmd))
proc.wait()

# -- Run top-xaod --
cmd  = "top-xaod %s input.txt"%(cutfilename)
proc = subprocess.Popen(shlex.split(cmd))
proc.wait()

# -- Check the return code and exit this script with that --
sys.exit( proc.returncode )
