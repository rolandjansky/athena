#!/usr/bin/env python
# Author
# John Alison johnda@hep.upenn.edu
# Ben Cooper b.d.cooper@qmul.ac.uk
# small adds Pierfrancesco Butti pbutti@nikhef.nl
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
from utilities_ZmumuCosmics import *
import AtlasStyle
gStyle.SetOptStat(0)

oFext=""
nMAXFiles = 2
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
for i in range(nMAXFiles) :
    print rootFileNames[i]


if makeOutput:
    print "Producing output in directory:", outputDir

#getting the TFile objects using the rootFileNames
rootFiles = [TFile,TFile,TFile,TFile] 
nFiles = 0
for i in range(nMAXFiles):
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

#options set in utilities_ZmumuCosmics.py
#residualsDir is the path to the residuals dir in the monitoring root file.
#legendTitles,markerColors,markerStyles,rootFiles are all seld explanatory.
#


##########################################

############HANDS ON PART#################

##########################################


#use this macro to produce some plots for the monitoring exercise. Use the example shown to include:
#1) some kinematics properties of tracks (pT,d0,z0,eta,phi)
#2) X and Y residuals as a function of the pixel Layers
#3) X residuals as a function of the sct layers. Compare the width of the residuals.
#4) Plots the residuals as a function of the incident angle theta and phi. What differences do you observe between
#Collisions and Cosmics?
#5) Plots everything you think is interesting to check on the basis of the


## # Basic Barrel Residual distributions

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.png"+oFext, "Pixel barrel", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY.png"+oFext, "Pixel barrel", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


# Basic Pull distributions

PIXPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX, outputDir+"/"+"PIXPULLX.png", "Pixel Barrel", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY, outputDir+"/"+"PIXPULLY.png", "Pixel Barrel", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)



## # Residual mean and width distributions as a function of layer, ring, stave in barrel 

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean.png"+oFext, "Pixel/SCT Barrel", "residual mean [#mum]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.63,0.86,False)


SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth.png"+oFext, "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.63,0.86,False)


        



