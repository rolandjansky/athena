#===========================================================================
#===========================================================================
# Description:
#  This file is the steering file used to compare two monitoring.root files
# Usage:
#   - specify the two files to be compared, afterAlignmentFile, beforeAlignmentFile
#   - specify the outputfile
#   - run:
#         athena CompareIDAlignemntMonitoring.py
#
#   - examine the outputfile
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

#====================================
# The Reference File (Black)
afterAlignmentFile = "/afs/cern.ch/user/a/atlidali/w1/users/johnda/testNewIterator/Iter10/TotalMonitoring.root"

#====================================
# The Monitored File (Red)
beforeAlignmentFile = "/afs/cern.ch/user/a/atlidali/w1/users/johnda/testNewIterator/Iter0/TotalMonitoring.root"

#====================================
# The Output File
outputFile = "AlignmentOutput.root"

#====================================
include("InDetAlignmentMonitoring/makeComparision.py")
