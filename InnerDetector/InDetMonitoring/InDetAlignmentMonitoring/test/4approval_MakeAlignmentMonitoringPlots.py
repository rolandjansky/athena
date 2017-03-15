#!/usr/bin/env python
# Author
# John Alison johnda@hep.upenn.edu
# Ben Cooper b.d.cooper@qmul.ac.uk
# small adds Priscilla Pani ppani@nikhef.nl
# this file also needs the utilities.py and AtlasStyle.py files

# used to draw the alignment monitoring plots side-by-side of up to 3 monitoring.root files (number of files can be 1,2 or 3)
# need to provide a configuration file - see makeMonitoringPlotsExampleConfigFile.py

##--------------------------
# usage:
#
# ./MakeAlignmentMonitoringPlots.py -c <configurationFile>
#
##--------------------------

import sys
from ROOT import *
from utilities import *
import AtlasStyle
gStyle.SetOptStat(0)



#some initialization
configFileName = ""
makeOutput = True

#reading in the root file names from input arguments
for i in range(len(sys.argv)):
    if sys.argv[i]=="-c":
        configFileName = sys.argv[i+1]

# reading in configuration file
fd = open(configFileName)
config = fd.readlines()
exec(''.join(config))

print "Reading in from .root files:"
for i in range(3) :
    print rootFileNames[i]


if makeOutput:
    print "Producing output in directory:", outputDir

#getting the TFile objects using the rootFileNames
rootFiles = [TFile,TFile,TFile]
nFiles = 0
for i in range(3):
    if rootFileNames[i]!="": 
        rootFiles[i] = TFile(rootFileNames[i])
        nFiles = nFiles + 1

#nFiles is used in the drawing of the plots
print "nFiles = ",nFiles


# Drawing and saving of the plots....
# Plots are first "made" using MakePlots - that is, the plots are fetched from the input 
# files and any fits that should be done are made. MakePlots returns a tuple object which 
# contains the histogram and fit. MakePlots also performs normalisation if specified (see config options)
# tuple is passed to DrawPlots where the plots are drawn on top of each other, with legend, axistitles etc
# DrawPlots is also where the figure is saved 

# Basic Barrel Residual distributions

# parameter explanation:
# def DrawPlots(inputTuple, outputName, plotTitle, yAxisTitle, xAxisTitle, legendLeftX, legendUpperY, units, 
#              canvasText, makeOutput, textBoxLeftX=0.60, textBoxUpperY=0.91, dynamicYRange=True):


PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX."+oFext, "Pixel barrel", "Hits on tracks / 3 #mum",
          "Local x residual [mm]", 0.2, 0.87, "#mum",canvasText,makeOutput,0.60,0.91)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY."+oFext, "Pixel barrel", "Hits on tracks / 20 #mum",
          "Local y residual [mm]", 0.2, 0.87, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX."+oFext, "SCT barrel", "Hits on tracks / 3 #mum",
          "Local x residual [mm]", 0.2, 0.87, "#mum",canvasText,makeOutput,0.60,0.91)

TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR."+oFext, "TRT barrel", "Hits on tracks / 22 #mum",
          "Local residual [mm]", 0.18, 0.9, "#mum",canvasText,makeOutput, 0.65, 0.9)

## Basic Endcap Residual distributions

# draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX."+oFext, "Pixel end-caps", "Hits on tracks / 3 #mum",
          "Local x residual [mm]", 0.21, 0.87, "#mum",canvasText,makeOutput,0.62,0.91)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY."+oFext, "Pixel end-caps", "Hits on tracks / 20 #mum",
          "Local y residual [mm]", 0.21, 0.87, "#mum",canvasText,makeOutput,0.62,0.91)


# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX."+oFext, "SCT Endcap", "Hits on tracks / 3 #mum",
          "Local x residual [mm]", 0.21, 0.87, "#mum",canvasText,makeOutput,0.62,0.91)


TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC."+oFext, "TRT end-caps", "Hits on track / 22 #mums",
          "Local residual [mm]", 0.18, 0.89, "#mum",canvasText,makeOutput, 0.65, 0.9)

