# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

# Enable monitoring
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AppMgr import ToolSvc
from TrigFTKBankGen.TrigFTKBankGenConf import *

PattMergeRoot = PattMergeRootAlgo("PattMergeRoot")

from AthenaCommon.AppMgr import theApp
# Number of events to be processed
theApp.EvtMax = 1

from AthenaCommon.AppMgr import ServiceMgr

#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#
#ServiceMgr.THistSvc.Output += ["outputHisto DATAFILE='outputHisto.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for PattMergeRoot only
PattMergeRoot.OutputLevel = INFO


#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

# Specify in which step of the algorithm the merging is performed
#   Possible options:
#     - initialize
#     - execute
#     - finalize
PattMergeRoot.WhereToRunMerging = "initialize"

# cut on coverage, applied to MERGED_ROOTFILE and OUT_FILE
PattMergeRoot.MinCoverage = 2

# output file (root or compressed text)
#  Specify output filename (must end with .root)
#  If additional the patterns shall be written in ascii/text 
#  format specify 'TextOutFile' (.bz2 is added automatically). 
#  If TextOutFile is empty do not write patterns in text format.
PattMergeRoot.RootOutFile  = "MergedPatterns.root"
PattMergeRoot.TextOutFile  = ""   # e.g. "MergedPatterns"


# temporary file, with all patterns, prior to merging
#  if no file is given, construct name by appending .textImport.root
#  to OUT_FILE
PattMergeRoot.TextImportRootFile = ""

# Do split output into NSub sub-regions
PattMergeRoot.NSub = 0

# Delete files after merging
# (Deletes all specified .root-files)
# If problem occured while importing text file, keep this text file
PattMergeRoot.DeleteFilesAfterMerging = False

# input files: various input formats can be mixed
PattMergeRoot.InputFiles = [ 
 "patterns_sub0.patt.bz2",
 "patterns_sub1.patt.bz2",
 "patterns_sub2.patt.bz2",
 "patterns_sub3.root",
 "patterns_sub4.root"
]

alg += PattMergeRoot


#==============================================================
#
# End of job options file
#
###############################################################
