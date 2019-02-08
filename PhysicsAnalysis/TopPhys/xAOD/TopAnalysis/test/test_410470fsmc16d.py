#!/usr/bin/env python                                                                                         

# art-description: DxAOD MC16d TOPQ1 FullSim 410470                                                          
# art-type: grid                                                                                              
# art-output: output.root                                                                                     
# art-input: user.artprod.user.iconnell.410470.DAOD_TOPQ1.e6337_s3126_r10201_p3554.ART.v2
# art-input-nfiles: 2
# art-include: 21.2/AnalysisTop

import ROOT
from PathResolver import PathResolver
import subprocess, sys, shlex, random, shutil, os, glob

# Create empty PoolFileCatalog
os.system('art.py createpoolfile')

# -- Settings --
cutfilename    = "artcut_"+os.path.basename( sys.argv[0] ).replace(".py",".txt")
#inputfilenames = glob.glob("*.root*")

try:  
   inputfilenames=os.environ["ArtInFile"]
except KeyError: 
   print "Please set the environment variable ArtInFile"
   sys.exit(1)

# -- Get the validation file path from the most recent location --
cutfilepath   = ROOT.PathResolver.find_file(cutfilename,
                                            "DATAPATH", 
                                            ROOT.PathResolver.RecursiveSearch)

# -- Print the file location for debugging --
print "ART Test : Using cutfile (%s) from %s"%(cutfilename, cutfilepath)
print "ART Test : Using inputfiles :",inputfilenames
print "Running on full statistics"

# -- Copy the cutfile locally to be updated -- 
shutil.copyfile(cutfilepath, cutfilename)

 # -- Write the input file path to a temporary file --
lines = inputfilenames.split(",")
with open('input.txt', 'w') as inputfilepath:
    for line in lines:
       print "line : ", line
       inputfilepath.write(line+'\n')

f = open('input.txt', 'r')
print "Printing input.txt"
print f.read()
f.close()

# -- Run top-xaod --
cmd  = "top-xaod %s input.txt"%(cutfilename)
proc = subprocess.Popen(shlex.split(cmd))
proc.wait()

# -- Print the art-result return code
print "art-result: " + str(proc.returncode)

# -- Check the return code and exit this script with that --
sys.exit( proc.returncode )
