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

import sys,os
from datetime import datetime
from ROOT import *
from utilities import *
import AtlasStyle
gStyle.SetOptStat(0)

def optParsing():
    from optparse import OptionParser
    Now=datetime.now()
    parser=OptionParser()
    parser.add_option("--Tracks",dest="Tracks",help="Track Collection to use for plotting",default="ExtendedTracks_all")
    parser.add_option("--inFiles",dest="inFiles",help="Comma separated list of files",default="")
    parser.add_option("--outDir",dest="outDir",help="Output directory where to place the plots",default="Outputs"+Now.strftime('%Y_%m_%d_%H_%M_%S'))
    #parser.add_option("--legendTitles",dest="legendTitles",help="Comma Separated list of titles to give to the plots",default="Default Legend")
    #parser.add_option("--canvasText",dest="canvasText",help="Comma separated list of  canvas texts",default="#bf{#it{ATLAS}} Internal")
    (config,sys.argv[1:]) = parser.parse_args(sys.argv[1:])
    return config


config=optParsing()
outputDir=config.outDir
normaliseHistos = True
rootFileNames=(config.inFiles).split(',')

print rootFileNames

if not os.path.exists(outputDir):
    os.makedirs(outputDir)


runN="run_242651"
residualsDir=[runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
              runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
              runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
              runN+"/IDAlignMon/"+config.Tracks+"/Residuals/"
              ]

trtResidualsDir=[
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/Residuals/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/Residuals/"
    ]

genericTracksDir = [
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/GenericTracks/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/GenericTracks/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/GenericTracks/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/GenericTracks/"
    ]

hitEffDir = [
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/HitEfficiencies/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/HitEfficiencies/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/HitEfficiencies/",
    "/"+runN+"/IDAlignMon/"+config.Tracks+"/HitEfficiencies/"
    ]


markerStyles = [kFullCircle,kOpenCircle,kOpenSquare, kFullSquare] #determines marker styles for plots
markerColors = [kBlue,kRed,kBlack,kBlack] #determines marker colors for plots
#legendTitles = ["Nominal Geometry MC","Misaligned MC","Realigned MC",""] #determines legend titles
legendTitles = ["BC_Error=False","Commissioning"]
#legendTitles   = ["Rel19","Rel17"]
canvasText = ["#bf{#it{ATLAS}} Internal","Cosmics","Reco_tf x277","19.1.3.7"]



oFext=""
nMAXFiles = len(rootFileNames)
#some initialization
configFileName = ""
makeOutput = True
unitArea = True

#reading in the root file names from input arguments
#for i in range(len(sys.argv)):
#    if sys.argv[i]=="-c":
#        configFileName = sys.argv[i+1]

# reading in configuration file
#fd = open(configFileName)
#config = fd.readlines()
#exec(''.join(config))


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
        rootFiles[i].GetListOfKeys()
        nFiles = nFiles + 1

#nFiles is used in the drawing of the plots
print "nFiles = ",nFiles


# Drawing and saving of the plots....
# Plots are first "made" using MakePlots - that is, the plots are fetched from the input 
# files and any fits that should be done are made. MakePlots returns a tuple object which 
# contains the histogram and fit. MakePlots also performs normalisation if specified (see config options)
# tuple is passed to DrawPlots where the plots are drawn on top of each other, with legend, axistitles etc
# DrawPlots is also where the figure is saved 


## # Basic Barrel Residual distributions

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.png"+oFext, "Pixel barrel", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
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


PIXPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX0, outputDir+"/"+"PIXPULLX0.png", "Pixel Barrel IBL", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX1, outputDir+"/"+"PIXPULLX1.png", "Pixel Barrel L0", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX2, outputDir+"/"+"PIXPULLX2.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX3, outputDir+"/"+"PIXPULLX3.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY0, outputDir+"/"+"PIXPULLY0.png", "Pixel Barrel IBL", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY1, outputDir+"/"+"PIXPULLY1.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY2, outputDir+"/"+"PIXPULLY2.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY3, outputDir+"/"+"PIXPULLY3.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


PIXPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECA, outputDir+"/"+"PIXPULLXECA.png", "Pixel Endcap A", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECAY, outputDir+"/"+"PIXPULLYECA.png", "Pixel Endcap A", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECC, outputDir+"/"+"PIXPULLXECC.png", "Pixel Endcap C", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECCY, outputDir+"/"+"PIXPULLYECC.png", "Pixel Endcap C", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX.png"+oFext, "SCT barrel", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


SCTPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX, outputDir+"/"+"SCTPULLX.png", "SCT Barrel", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX0, outputDir+"/"+"SCTPULLX0.png", "SCT Barrel L0", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX1, outputDir+"/"+"SCTPULLX1.png", "SCT Barrel L1", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX2, outputDir+"/"+"SCTPULLX2.png", "SCT Barrel L2", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX3, outputDir+"/"+"SCTPULLX3.png", "SCT Barrel L3", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECA, outputDir+"/"+"SCTPULLXECA.png", "SCT Endcap A", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECC, outputDir+"/"+"SCTPULLXECC.png", "SCT Endcap C", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)




## ## Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXX0.png"+oFext, "Pixel Barrel IBL", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1.png"+oFext, "Pixel Barrel L0", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2.png"+oFext, "Pixel Barrel L1", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX3, outputDir+"/"+"PIXX3.png"+oFext, "Pixel Barrel L2", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+"PIXY0.png"+oFext, "Pixel Barrel IBL", "Number of hits on tracks / 2#mum",
          "y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY1, outputDir+"/"+"PIXY1.png"+oFext, "Pixel Barrel L0", "Number of hits on tracks / 2#mum",
          "y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY2, outputDir+"/"+"PIXY2.png"+oFext, "Pixel Barrel L1", "Number of hits on tracks / 2#mum",
          "y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY3, outputDir+"/"+"PIXY3.png"+oFext, "Pixel Barrel L2", "Number of hits on tracks / 2#mum",
          "y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+"SCTX0.png"+oFext, "SCT Barrel L0", "Number of hits on tracks / 4#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+"SCTX1.png"+oFext, "SCT Barrel L1", "Number of hits on tracks / 4#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+"SCTX2.png"+oFext, "SCT Barrel L2", "Number of hits on tracks / 4#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+"SCTX3.png"+oFext, "SCT Barrel L3", "Number of hits on tracks / 4#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

## ## Basic Endcap Residual distributions

## # draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX.png"+oFext, "Pixel end-caps", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.18, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

## # Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY.png"+oFext, "Pixel end-caps", "Hits on tracks / 20#mum",
          "Local y residual [mm]", 0.18, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX.png"+oFext, "SCT Endcap", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)




TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0.png", "", "N Tracks",
          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)


TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0, outputDir+"/"+"z0.png", "", "N Tracks",
          "z0 [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)


TrackPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"phi","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPHI, outputDir+"/"+"phi.png", "", "N Tracks",
          "phi [radians]", 0.19, 0.9, "radians",
          canvasText,makeOutput)

TrackETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackETA, outputDir+"/"+"eta.png", "", "N Tracks",
          "eta", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT.png", "", "N Tracks",
          "pT", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False)


TrackSUMMARY = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"summary","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackSUMMARY, outputDir+"/"+"summary.png", "N Hits Summary", "N Tracks",
          "", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackCHI2DOF = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"chi2oDoF","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackCHI2DOF, outputDir+"/"+"chi2oDoF.png", "", "N Tracks",
          "Chi2/DOF", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNTRACKS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"ntracks","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRACKS, outputDir+"/"+"ntracks.png", "Tracks/Event", "N Events",
          "N Tracks", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNTRTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Ntrthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRTHits, outputDir+"/"+"nTRTHits.png", "", "N Tracks",
          "N TRT Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHits, outputDir+"/"+"nSCTHits.png", "", "N Tracks",
          "N SCT Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsBarrel, outputDir+"/"+"nSCTHitsBarrel.png", "", "N Tracks",
          "N SCT Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEca, outputDir+"/"+"nSCTHitsEca.png", "", "N Tracks",
          "N SCT Hits Eca", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEcc, outputDir+"/"+"nSCTHitsEcc.png", "", "N Tracks",
          "N SCT Hits Ecc", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHits, outputDir+"/"+"nPIXHits.png", "", "N Tracks",
          "N PIX Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsBarrel, outputDir+"/"+"nPIXHitsBarrel.png", "", "N Tracks",
          "N PIX Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEca, outputDir+"/"+"nPIXHitsEca.png", "", "N Tracks",
          "N PIX Hits ECA", 0.69, 0.6, "",
          canvasText,makeOutput)


TrackNPIXHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEcc, outputDir+"/"+"nPIXHitsEcc.png", "", "N Tracks",
          "N PIX Hits ECC", 0.69, 0.6, "",
          canvasText,makeOutput)


















paletteType = 1
for i in range(nFiles):
    PIXBAR_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR")
    DrawHitMaps(PIXBAR_HitMaps, outputDir+"/"+"PIXBAR_HitMaps_File_"+str(i+1)+"."+oFext, "Pixel barrel", "Hits on tracks",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

    PIXECA_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECA")
    DrawHitMaps(PIXECA_HitMaps, outputDir+"/"+"PIXECA_HitMaps_File_"+str(i+1)+"."+oFext, "Pixel barrel", "Hits on tracks",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,"PIX","ECA")

    PIXECC_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECC")
    DrawHitMaps(PIXECC_HitMaps, outputDir+"/"+"PIXECC_HitMaps_File_"+str(i+1)+"."+oFext, "Pixel barrel", "Hits on tracks",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,"PIX","ECC")

    SCTBAR_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "BAR")
    DrawHitMaps(SCTBAR_HitMaps, outputDir+"/"+"SCTBAR_HitMaps_File_"+str(i+1)+"."+oFext, "SCT barrel", "Hits on tracks",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText, makeOutput,0.60,0.91)


