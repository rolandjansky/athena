#===========================================================================
#===========================================================================
# Description:
#  This file runs the comparision configured in CompareIDAlignemntMonitoring.py
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

from ROOT import *
include("InDetAlignmentMonitoring/AtlasStyle.py")

gStyle.SetOptFit(110)
monitoredColor = kRed+1
referenceColor = kBlack

# read in the monitoring plots 
if not os.access(afterAlignmentFile,os.F_OK):
    print afterAlignmentFile,"does not exist!"
    sys.exit(0)
a_file = TFile(afterAlignmentFile)

if not os.access(beforeAlignmentFile,os.F_OK):
    print beforeAlignmentFile,"does not exist!"
    sys.exit(0)
b_file = TFile(beforeAlignmentFile)

# Create the output file
rootfile = TFile(outputFile, "RECREATE")

include("InDetAlignmentMonitoring/CompareAll.py")
rootfile.Close()
sys.exit()


# IGNORRE THE OLD STUFF BELOW
#for i in trackSelections:
#    thisDir = rootfile.mkdir(i.split('/')[1],i.split('/')[1])
#    thisDir.mkdir("Barrel")
#    thisDir.mkdir("EndcapA")
#    thisDir.mkdir("EndcapC")
#
#for i in trackSegmentsTrackSelection:
#    test = rootfile.cd(i.split('/')[1]) 
#    if test:
#        rootfile.cd()
#    else:
#        rootfile.mkdir(i.split('/')[1],i.split('/')[1])
#
## Load the drawing functions
#include("InDetAlignmentMonitoring/rootArtist.py")
#
## Compare the residuals
#include("InDetAlignmentMonitoring/CompareResiduals.py")
#
## Compare the Generic Track Parameters
#include("InDetAlignmentMonitoring/CompareGenericTracks.py")
#
## Compare the Generic Track Parameters
#include("InDetAlignmentMonitoring/CompareEfficiencies.py")
#
## Compare the Track Segments
#if trackSegmentsTrackSelection.__len__() > 0:
#    include("InDetAlignmentMonitoring/CompareTrackSegments.py")
#
## Exit
#rootfile.Close()
#sys.exit()
#===============================
