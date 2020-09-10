#! /usr/bin/env python
# This script is designed to be used as a quick runtime ctest in the CI framework
# We use python to better handle getting our validation cutfile which should be stable
# We will augment the NEvents line in our cutfile to limit the job
# This is primarily to catch any runtime changes introduced before they are merged

from PathResolver import PathResolver
from CI_test import CITest
import sys,ROOT

cutfilename = "validation-cuts-pflow.txt"
cutfilepath = ROOT.PathResolver.find_file(cutfilename,
                                          "DATAPATH",
                                          ROOT.PathResolver.RecursiveSearch)

returnCode = CITest("TOPQ1 DATA",
                    cutfilename,
                    cutfilepath,
                    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/AnalysisTop/ContinuousIntegration/Data/p4173/"+
                    "data16_13TeV.AllYear.physics_Main.PhysCont.DAOD_TOPQ1.grp16_v01_p4173/DAOD_TOPQ1.21542408._000018.pool.root.1",
                    [("#NEvents.*","NEvents 3000")])

if returnCode != 0:
    print "Error in EMPFlow TOPQ1 Data"
    sys.exit(returnCode)

# -- Return 0 as all tests were successful -- #
sys.exit(0)
