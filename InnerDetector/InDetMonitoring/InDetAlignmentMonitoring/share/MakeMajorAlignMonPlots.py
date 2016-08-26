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
userCosmetics = ""

import sys, os
from ROOT import *
from utilities import *
#import utilities 
import AtlasStyle
gStyle.SetOptStat(0)
nMAXFiles = 3

#gROOT.SetBatch()


##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("--ALL", dest="inputALL", help="Do plot all categories apart from Track Segments", action="store_true", default = False)
	parser.add_option("--BeamSpot", dest="inputBeamSpot", help="Plot beam spot related histograms", action="store_true",default=False)
	parser.add_option("--canvasText", dest="canvasText", help="Global labels for all the plots (max 4 labels)", default="")
	parser.add_option("--Clusters", dest="inputClusters", help="Plot of cluster size and residuals vs cluster size", action="store_true",default=False)
	parser.add_option("--collectOutput", dest="collectOutput", help= "if output is stored as pdf, then they are collected in one single file", action="store_true", default = False)
	parser.add_option("--Cosmetics", dest="inputCosmetics", help="choose among listed plot cosmetics (Default, ApprovedPlots...", default="")
	parser.add_option("--eps", dest="inputEPS", help="output files saved as EPSs", action="store_true", default = False)
	parser.add_option("--Extended", dest="inputExtended", help="Draw more plots", action="store_true", default = False)
	parser.add_option("--HitErrors", dest="inputHitErrors", help="Do hit error plots for all systems", action="store_true",default=False)
	parser.add_option("--HitMaps", dest="inputHitMaps", help="Do hit maps plots for all systems layer by layer", action="store_true",default=False)
	parser.add_option("--Hits", dest="inputHits", help="Do hit plots for all systems and layer by layer", action="store_true",default=False)
	parser.add_option("--IBLresiduals", dest="inputIBL", help="print detailed residual distributions in IBL", action="store_true", default = False)
	parser.add_option("--inputColors",dest="inputColors",help="colors to be used with the input files", default = "")
	parser.add_option("--inputFiles",dest="inputFiles",help="list of files", default = "")
	parser.add_option("--inputLabels",dest="inputLabels",help="labels to be used with the input files", default = "")
	parser.add_option("--inputMarkers",dest="inputMarkers",help="markers to be used with the input files", default = "")
	parser.add_option("--inputTrackCollection", dest="inputTrackCollection",help = "Track collection to be drawn from an input file", default ="")
	parser.add_option("--inputFolder", dest="inputFolder",help = "Main folder of the input files where monitoring histograms are kept", default ="")
	parser.add_option("--Modules", dest="inputResByModule", help="Print the residuals module by module", action="store_true", default=False)
	parser.add_option("--outputFolder", dest="outputFolder", help= "Name of the folder where output files are kept", default ="../plots")
	parser.add_option("--pdf", dest="inputPDF", help="output files saved as PDFs", action="store_true", default = False)
	parser.add_option("--Prefix", dest="inputPrefix", help="prexif added to all output file names", default = "")
	parser.add_option("--Pulls", dest="inputPulls", help="Do pulls plots for all systems and then layer by layer", action="store_true",default=False)
	parser.add_option("--ResidualMaps", dest="inputResidualMaps", help="Do residuals plots for all systems layer by layer", action="store_true",default=False)
	parser.add_option("--Residuals", dest="inputResiduals", help="Do residuals plots for all systems and then layer by layer", action="store_true",default=False)
	parser.add_option("--Overlaps", dest="inputOverlaps", help="Do Overlap residuals plots for all systems and then layer by layer", action="store_true",default=False)
	parser.add_option("--Script", dest="inputScript", help="The user may provie its own script", default="")
	parser.add_option("--SetBatch", dest="inputSetBatch", help="When SetBatch is used histogram displays are not open", action="store_true", default=True)
	parser.add_option("--ShowPlots", dest="inputSetBatch", help="When ShowPlots is used the histogram display is open", action="store_false", default=True)
	parser.add_option("--TrackParams", dest="inputTrackParams", help="Do track parameter plots", action="store_true",default=False)
	parser.add_option("--TrackSegments", dest="inputTrackSegments", help="Do track segment matching plots", action="store_true",default=False)
	parser.add_option("--WebMonitoring", dest="WebMonitoring", help="to be set in case of producing output for the web monitoring", action="store_true",default=False)
    
	(config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

	return config


##########################################################
#            dealing with the options                    #
##########################################################
config = optParsing()
userHitErrors = config.inputHitErrors
userHits = config.inputHits
userHitMaps = config.inputHitMaps
userIBL = config.inputIBL
userPulls = config.inputPulls
userResidualMaps = config.inputResidualMaps
userResiduals = config.inputResiduals
userOverlaps = config.inputOverlaps
userTrackParams = config.inputTrackParams
userTrackSegments = config.inputTrackSegments
userExtended = config.inputExtended
userPDF = config.inputPDF
userEPS = config.inputEPS
userScript = config.inputScript
userSetBatch = config.inputSetBatch 
userResByModule = config.inputResByModule
userBeamSpot = config.inputBeamSpot
userClusters = config.inputClusters
userCosmetics = config.inputCosmetics
userInputFiles = config.inputFiles.split()
userInputLabels = config.inputLabels.split()
userInputMarkers = config.inputMarkers.split()
userColors = config.inputColors.split()
userInputTrackCollection = config.inputTrackCollection.split()
userInputFolder = config.inputFolder.split()
userOuputFolder = config.outputFolder
userCollectOutput = config.collectOutput and userPDF
userWebMonitoring = config.WebMonitoring
userCanvasText = config.canvasText.split()
userPrefix = config.inputPrefix    

if (config.inputALL):
    userHitErrors = False
    userHitMaps = True
    userHits = True
    userIBL = False
    userPulls = True
    userResidualMaps = False
    userResiduals = True
    userOverlaps  = True
    userTrackParams = True
    userExtended = True
    userResByModule = False # as this plots to many histograms, this can be only activated on purpose
    userBeamSpot = False
    userClusters = False



#some initialization
configFileName = ""
makeOutput = True
if (userSetBatch): 
    gROOT.SetBatch()
#
outputFileExtension = "png"
if (userPDF): outputFileExtension="pdf"

#
doResiduals = False or userResiduals 
doOverlaps  = False or userOverlaps
doResidualProfiles = False 
doPulls = False or userPulls
doTrackParams = False or userTrackParams
doCosmicTrackSegments = False or userTrackSegments
doHitMaps = False or userHitMaps
doResidualMaps = False or userResidualMaps
doHits = False or userHits
doHitErrors = False or userHitErrors
doIBL = False or userIBL
doBeamSpot = False or userBeamSpot
doResByModule = False or userResByModule
doClusters = False or userClusters

print "\n ****************************"
print " ** MakeMajorAlignMonPlots **"
print " **************************** \n"


#reading in the root file names from input arguments
for i in range(len(sys.argv)):
    if sys.argv[i]=="-c":
        configFileName = sys.argv[i+1]

#if not configfile is given take the default configuration file:
if (configFileName == ""):
    configFileName = "userConfig.py"
    print " -- MakeMajorAlignMonPlots -- using the default configuration file: ", configFileName

# reading in configuration file
execfile(configFileName) 

print "Reading in from .root files:"
print " -- MakeMajorAlignMonPlots -- List of input monitoring files: "
for thisfile in rootFileNames:
    print thisfile
    

if makeOutput:
    if not os.path.isdir(outputDir):
       print " -- MakeMajorAlignMonPlots -- Producing output in new directory:", outputDir
       os.mkdir(outputDir)
    else:
        print " -- MakeMajorAlignMonPlots -- Producing output in existing directory:", outputDir


#getting the TFile objects using the rootFileNames
rootFiles = []
nFiles = 0
for thisfile in rootFileNames:
    if (rootFileNames[nFiles] != ""):
        rootFiles.append(TFile(rootFileNames[nFiles]))
        nFiles = nFiles + 1

#nFiles is used in the drawing of the plots
print " -- MakeMajorAlignMonPlots -- Using in total ",nFiles, " input files "

# Drawing and saving of the plots....
# Plots are first "made" using MakePlots - that is, the plots are fetched from the input 
# files and any fits that should be done are made. MakePlots returns a tuple object which 
# contains the histogram and fit. MakePlots also performs normalisation if specified (see config options)
# tuple is passed to DrawPlots where the plots are drawn on top of each other, with legend, axistitles etc
# DrawPlots is also where the figure is saved 

# Basic Barrel Residual distributions
if (doResiduals): execfile("MakeResidualPlots.py")
if (doOverlaps): execfile("MakeResidualOverlapPlots.py")
if (doResidualProfiles): execfile("MakeResidualProfilesPlots.py") 
if (doIBL): execfile("MakeResidualsIBL.py")
if (doPulls): execfile("MakePullPlots.py") 
    
# track parameters
if (doTrackParams): execfile("MakeTrackParamsPlots.py")
    
# hit maps
if (doHitMaps): execfile("MakeHitMapsPlots.py")

# residual maps
if (doResidualMaps): execfile("MakeResidualMapsPlots.py")

# hits per detector and hit efficiencies
if (doHits): execfile("MakeHitPlots.py")   

#short cut for track-segments plots with cosmic Rays
if (doCosmicTrackSegments): execfile("MakeTrackSegmentsPlots.py")

# hit errors
if (doHitErrors): execfile("MakeHitErrorPlots.py") 

# beam spot, lumiblocks etc
if (doBeamSpot): execfile("MakeBeamSpotPlots.py") 

# beam spot, lumiblocks etc
if (doClusters): execfile("MakeClusterSizePlots.py") 

# Residuals module by module
if (doResByModule): execfile("MakeResidualsPerModule.py") 

#user script
if (len(userScript)>0):
    if (os.path.isfile(userScript)):
        execfile(userScript)
    else:
        print " -- MakeMajorAlignMonPlots -- user script ", userScript, " does not exist "
    
    
# extract statistics:
execfile("MakeStatisticsTable.py")

print " -- MakeMajorAlignMonPlots -- Game over. Insert coin. -- "


