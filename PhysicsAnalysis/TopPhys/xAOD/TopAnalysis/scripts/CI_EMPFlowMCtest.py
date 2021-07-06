#! /usr/bin/env python
# This script is designed to be used as a quick runtime ctest in the CI framework
# We use python to better handle getting our validation cutfile which should be stable
# We will augment the NEvents line in our cutfile to limit the job
# This is primarily to catch any runtime changes introduced before they are merged

from PathResolver import PathResolver
from CI_test import CITest
import sys,ROOT

cutfilename = "validation-cuts.txt"
cutfilepath = ROOT.PathResolver.find_file(cutfilename,
                                          "DATAPATH",
                                          ROOT.PathResolver.RecursiveSearch)

returnCode = CITest("DAOD_PHYS MC",
                    cutfilename,
                    cutfilepath,
                    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/ContinuousIntegration/R22/MC/p4441/"+
                    "valid1.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_PHYS.e6337_s3227_r12405_p4441/"+
                    "DAOD_PHYS.24524637._000004.pool.root.1",
                    [("#NEvents.*","NEvents 500")])

if returnCode != 0:
    print("Error in DAOD_PHYS MC")
    sys.exit(returnCode)

# -- Return 0 as all tests were successful -- #
sys.exit(0)
