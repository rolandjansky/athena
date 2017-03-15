#============= Use some initial positions (Eg: known alignment in CSC)
# Turning on CSC Truth, compares the L2 alignments derived here to
# the true L2 misalignments present in the CSC data.  A truth directory with 
# comparisons of the module final positions (compompared to truth) will 
# be created and the Total Dx and Dz will now include the initial CSC 
# misalignments.
doCSCTruth = False

#============= Name of the alignment log file ==============
#alignlogfilename = "trtalignlog.txt"
alignlogfilename = "alignlogfile.txt"

#= New file the result of the last iteration
newpositionfile = dirHead+"/"+dirPrefix+str(numIter-1)+"/"+alignlogfilename

#= To compare alignment in the first iteration
referencefile = dirHead+"/"+dirPrefix+str(0+skipIter)+"/"+alignlogfilename
if doCSCTruth:
    path = str(os.environ['TestArea'])+"/InnerDetector/InDetAlignAlgs/TRT_AlignAlgs/share/"
    referencefile = path+"CSCPerfectAlignments.txt"
#= To compare to the M6 after L1 alignment
#referencefile = "M6L2NominalCogs.txt"

#=============================================
#======= imports/includes ====================
#=============================================
import sys, os, string
from ROOT import *
gStyle.SetLineWidth(2)
include("InDetAlignExample/RootSetup.py")

#=========Barrel Module data structure============
include("InDetAlignExample/BarrelModule.py")

#=========Load Functions to draw graphs===========
include("InDetAlignExample/AlignmentHistoSetup.py")

#=========Read in alignment data==================
include("InDetAlignExample/ReadinAlignmentDataNew.py")
#include("InDetAlignExample/ReadinAlignmentData.py")
    
#=========Write out alignment data==================
include("InDetAlignExample/WriteAlignmentAnalysis.py")


    
#==========Exit after writing root file===========
sys.exit()
