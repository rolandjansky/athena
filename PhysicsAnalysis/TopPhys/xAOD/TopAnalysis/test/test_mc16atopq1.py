#!/usr/bin/env python

# art-description: DxAOD MC16a TOPQ1 : top-xaod validation-cuts.txt
# art-type: grid
# art-output: output.root

import ROOT
from PathResolver import PathResolver
import subprocess, sys

# -- Settings -- 
cutfilename   = "validation-cuts.txt"
inputpath     = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TopAnalysis/p3215/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e5458_s3126_r9364_r9315_p3215/DAOD_TOPQ1.11627611._000441.pool.root.1"

# -- Get the validation file path from the most recent location --
cutfilepath   = ROOT.PathResolver.find_file(cutfilename,
                                            "DATAPATH", 
                                            ROOT.PathResolver.RecursiveSearch)

# -- Make the input file path -- 
inputfile = open("input.txt","w")
inputfile.write(inputpath+"\n")
inputfile.close()

# -- Run the top-xaod via Popen --
proc = subprocess.Popen( ["top-xaod", cutfilepath, "input.txt"] )

# -- Wait until the job is finished --
proc.wait()

# -- Return the return code of the job --
sys.exit( proc.returncode )
