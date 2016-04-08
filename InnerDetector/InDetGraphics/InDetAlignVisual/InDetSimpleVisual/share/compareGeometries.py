#==============================================================
# Author: John Alison (johnda@hep.upenn.edu)
#
# Date: 22 Aug 2008
#
# Description: Takes the output of the GetDetectorPositions packages
# run on  two different geometries and compares them.
# It draws the two geometries and plots the difference
# as a function of several parameters. 
#
#Usage:
#  1) Run InDetRecExample or poolToText.py with the jobOptions with the line
#       include("InDetSimpleVisual/GetDetectorPositions.py")
#     included for two different geometries.
#  2) Point the inputFile1 and inputFile2 to the textfile outputs of GetDetectorPositions
#  3) Name the output file
#  4) Then do:
#       athena CompareGeometries.py
#  5) The root file you named is created with all the relavent output.
#==============================================================
#inputFile1 = "IDgeometry_DB.txt" 
#inputFile1 = "M9Cosmics_BOFF/IDgeometry_L1_BON_Tp20.txt"
inputFile1 = "M8Cosmics_L3/preM7.txt"
inputFile2 = "M8Cosmics_L3/M8_afterL3.txt"
outputFile = "IDAlignment3D.root"

# The following options set the scales in the relevant plots
# ==================
#deltaR = 1.5
#deltaPhiTrans = 2
#deltaPhi = 0.0008
deltaX = 0.1
#deltaY = 2
#deltaZ = 2
#deltaXY = 3
#TRASL_FACTOR = 500

# Do we do detailed (wire level) drawing in the TRT ?
#  (Need the detailed text file as input)
#  Can do this by adding:
#    job.GetDetectorPositions.DoDetailedTRT = True
#   after including GetDetectorPositions.py
# list of modules to draw the canvases for
#drawTRTModules = [ (lay_1,phi_1), (lay_2,phi_2), ... ]
drawTRTModules = [ (0,8), (0,9) ]

# include the main script
#==========================
include("InDetSimpleVisual/makeComparison.py")

