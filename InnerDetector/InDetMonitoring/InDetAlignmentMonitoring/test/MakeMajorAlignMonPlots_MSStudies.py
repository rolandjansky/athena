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
from utilities_MS import *
import AtlasStyle
gStyle.SetOptStat(0)

oFext=""
nMAXFiles = 7
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
rootFiles = [TFile,TFile,TFile,TFile,TFile,TFile,TFile,TFile,TFile] 
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


## # Basic Barrel Residual distributions

print "PIXX"
PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.png"+oFext, "Pixel barrel", "Hits on tracks / 2#mum",
           "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


print "PIXY"
PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY.png"+oFext, "Pixel barrel", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


# Basic Pull distributions
print "PIXPULLX"
PIXPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX, outputDir+"/"+"PIXPULLX.png", "Pixel Barrel", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

print "PIXPULLY"
PIXPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY, outputDir+"/"+"PIXPULLY.png", "Pixel Barrel", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

print "PIXPULLX0"
PIXPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX0, outputDir+"/"+"PIXPULLX0.png", "Pixel Barrel L0", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)
print "pix_b1_pullx"
PIXPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX1, outputDir+"/"+"PIXPULLX1.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

print "PIXPULLX2"
PIXPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX2, outputDir+"/"+"PIXPULLX2.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY0, outputDir+"/"+"PIXPULLY0.png", "Pixel Barrel L0", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY1, outputDir+"/"+"PIXPULLY1.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY2, outputDir+"/"+"PIXPULLY2.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
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




TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR.png"+oFext, "TRT barrel", "Hits on tracks / 22#mum",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)





## ## Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXX0.png"+oFext, "Pixel Barrel L0", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1.png"+oFext, "Pixel Barrel L1", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2.png"+oFext, "Pixel Barrel L2", "Number of hits on tracks / 2#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

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

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC.png"+oFext, "TRT end-caps", "Hits on tracks",
          "residual [mm]", 0.2, 0.85, "#mum",canvasText,makeOutput,0.63,0.86)


SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean.png"+oFext, "Pixel/SCT Barrel", "residual mean [mm]",
         "", 0.19, 0.9, "#mum",
         canvasText,makeOutput,0.60,0.91,False)


SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth.png"+oFext, "Pixel/SCT Barrel", "residual width [mm]",
         "", 0.19, 0.9, "#mum",
         canvasText,makeOutput,0.60,0.91,False)



    #R#esidual mean and width distributions as a function of endcap disk
SiEcaResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXMean, outputDir+"/"+"SiEcaResXMean.png"+oFext, "Pixel/SCT Endcap A", "residual mean [#mum]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

SiEcaResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXWidth, outputDir+"/"+"SiEcaResXWidth.png"+oFext, "Pixel/SCT Endcap A", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

SiEccResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXMean, outputDir+"/"+"SiEccResXMean.png"+oFext, "Pixel/SCT Endcap C", "residual mean [#mum]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

SiEccResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXWidth, outputDir+"/"+"SiEccResXWidth.png"+oFext, "Pixel/SCT Endcap C", "residual width [#mum]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)


# TRT residual mean and width distributions

TRTBL0PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHI, outputDir+"/"+"TRTBLayer0vsPhi.png", "TRT Barrel Layer 0", "residual mean [#mum]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TRTBL0PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHIRMS, outputDir+"/"+"TRTBLayer0vsPhiRMS.png", "TRT Barrel Layer 0", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TRTBL1PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHI, outputDir+"/"+"TRTBLayer1vsPhi.png", "TRT Barrel Layer 1", "residual mean [#mum]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TRTBL1PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHIRMS, outputDir+"/"+"TRTBLayer1vsPhiRMS.png", "TRT Barrel Layer 1", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TRTBL2PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHI, outputDir+"/"+"TRTBLayer2vsPhi.png", "TRT Barrel Layer 2", "residual mean [#mum]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TRTBL2PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHIRMS, outputDir+"/"+"TRTBLayer2vsPhiRMS.png", "TRT Barrel Layer 2", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False,"SmallLegend")

TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0, outputDir+"/"+"z0.png"+oFext, "", "N Tracks",
          "z0 [mm]", 0.22, 0.9, "#mum",
          canvasText,makeOutput,0.66,0.86)

TrackETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackETA, outputDir+"/"+"eta.png"+oFext, "", "N Tracks / 0.05",
"Track #eta", 0.19, 0.9, "",
          canvasText,makeOutput,0.66,0.86)

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT.png", "", "N Tracks",
          "Track pT [GeV]", 0.19, 0.5, "",
          canvasText,makeOutput,0.66,0.86,False)

    #TRT MEAN/AVG as a function of the rings

    #CORRECT THE Y AXIS!! SCALE BY FACTOR 1000 ################


TRTEcaMeanR =  MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsRing_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTEcaMeanR, outputDir+"/"+"TRTEcaMeanR.png"+oFext, "TRT Endcap A", "residual Mean [#mum]",
          "Endcap Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

TRTEcaRmsR =  MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsRing_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTEcaRmsR, outputDir+"/"+"TRTEcaRmsR.png"+oFext, "TRT Endcap A", "residual Rms [mm]",
          "Endcap Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

TRTEccMeanR =  MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsRing_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTEccMeanR, outputDir+"/"+"TRTEccMeanR.png"+oFext, "TRT Endcap C", "residual Mean [#mum]",
          "Endcap Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)

TRTEccRmsR =  MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsRing_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTEccRmsR, outputDir+"/"+"TRTEccRmsR.png"+oFext, "TRT Endcap C", "residual Rms [mm]",
          "Endcap Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.63,0.86,False)


#TRT PLOTS AS A FUNCTION OF THE RINGS

for i in range(0,40):
    TRTA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_ring_"+str(i)+"_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(TRTA, outputDir+"/"+"TRT_ECA_R"+str(i)+".png"+oFext, "TRT Endcap A Ring " +str(i), "Number of hits on tracks / 60#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    del TRTA

    TRTC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_ring_"+str(i)+"_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(TRTC, outputDir+"/"+"TRT_ECC_R"+str(i)+".png"+oFext, "TRT Endcap C Ring " +str(i), "Number of hits on tracks / 60#mum",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    
    


Zmumu = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Zmumu","noFit",rootFiles,nFiles,True)
DrawPlots(Zmumu,outputDir+"/"+"Zmumu.png","Full ID", "Events","Dimuon Invariant Mass GeV/c2",0.19,0.9,"#mum",canvasText,makeOutput,0.66,0.86)

for i in range(0,3):
  for j in range(1,14):
    PIXETA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b"+str(i)+"_xresvsmodetaphi_3deta_module_"+str(j),"noFitWithStats",rootFiles,nFiles,normaliseHistos)
    DrawPlots(PIXETA,outputDir+"/"+"PIXXETAB"+str(i)+"_"+str(j)+".png","Pix B L"+str(i)+" #eta mod"+str(j),"Hits on track / 2#mum","Local x residual [mm]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86)
    del PIXETA

for i in range(0,3):
  for j in range(1,14):
    PIXETA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b"+str(i)+"_yresvsmodetaphi_3deta_module_"+str(j),"noFitWithStats",rootFiles,nFiles,normaliseHistos)
    DrawPlots(PIXETA,outputDir+"/"+"PIXYETAB"+str(i)+"_"+str(j)+".png","Pix B L"+str(i)+" #eta mod"+str(j),"Hits on track / 10#mum","Local y residual [mm]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86)
    del PIXETA
    

for i in range(0,4):
  for j in range(1,14):
    SCTETA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b"+str(i)+"_xresvsmodetaphi_3deta_module_"+str(j),"noFitWithStats",rootFiles,nFiles,normaliseHistos)
    DrawPlots(SCTETA,outputDir+"/"+"SCTETAB"+str(i)+"_"+str(j)+".png","SCT B L0 #eta mod"+str(j),"Hits on track / 2#mum","Local x residual [mm]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86)
    del SCTETA

## for i in range(0,3):
##  MEANPIX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_barrel_pix_b"+str(i)+"_xresvsmodetaphi_3d","noFitWithStats",rootFiles,nFiles,normaliseHistos)
##  DrawPlots(MEANPIX,outputDir+"/"+"MEANPIXB"+str(i)+".png","Pixel Barrel L"+str(i),"Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")

  
## for i in range(0,4):
##  MEANSCT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_barrel_sct_b"+str(i)+"_xresvsmodetaphi_3d","noFitWithStats",rootFiles,nFiles,normaliseHistos)
##  DrawPlots(MEANSCT,outputDir+"/"+"MEANSCTB"+str(i)+".png","SCT Barrel L"+str(i),"Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")
  

## ECAPIXMEAN = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_endcappix_eca_xresvsmodphi_2d","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(ECAPIXMEAN,outputDir+"/"+"ECAPIXMEAN.png","Pixel Endcap A","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")

## ECCPIXMEAN = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_endcappix_ecc_xresvsmodphi_2d","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(ECCPIXMEAN,outputDir+"/"+"ECCPIXMEAN.png","Pixel Endcap C","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")

## ECASCTMEAN = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_endcapsct_eca_xresvsmodphi_2d","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(ECASCTMEAN,outputDir+"/"+"ECASCTMEAN.png","SCT Endcap A","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")

## ECCSCTMEAN = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_endcapsct_ecc_xresvsmodphi_2d","noFitWithStats",rootFiles,nFiles,normaliseHistos)

## DrawPlots(ECCSCTMEAN,outputDir+"/"+"ECCSCTMEAN.png","SCT Endcap C","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")

## MEANPIXBAR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_pix_barrel","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(MEANPIXBAR,outputDir+"/"+"MEANPIXBAR.png","Pixel Barrel","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine") 

## MEANPIXEndCap = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_pix_endcap","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(MEANPIXEndCap,outputDir+"/"+"MEANPIXEndCap.png","Pixel EndCaps","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine") 

## MEANSCTBAR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_sct_barrel","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(MEANSCTBAR,outputDir+"/"+"MEANSCTBAR.png","SCT Barrel","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine") 

## MEANSCTEndCap = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mean_sct_endcap","noFitWithStats",rootFiles,nFiles,normaliseHistos)
## DrawPlots(MEANSCTEndCap,outputDir+"/"+"MEANSCTEndCap.png","SCT EndCaps","Number of modules / 0.08#mum","Residual Mean [#mum]",0.2,0.9,"#mum",canvasText,makeOutput,0.63,0.86,True,"SeparateLine")




## #SCT REQUESTED PLOTS RESIDUALS

PIXXL0BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerPix_L0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXXL0BRes, outputDir+"/"+"PIXXL0BRes.png"+oFext, "Pixel barrel X Res L0", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL0BRes, outputDir+"/"+"PIXXL0BRes.eps"+oFext, "Pixel barrel X Res L0", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL0BRes, outputDir+"/"+"PIXXL0BRes.C"+oFext, "Pixel barrel X Res L0", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXXL1BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerPix_L1","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXXL1BRes, outputDir+"/"+"PIXXL1BRes.png"+oFext, "Pixel barrel X Res L1", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL1BRes, outputDir+"/"+"PIXXL1BRes.eps"+oFext, "Pixel barrel X Res L1", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL1BRes, outputDir+"/"+"PIXXL1BRes.C"+oFext, "Pixel barrel X Res L1", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


PIXXL2BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerPix_L2","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXXL2BRes, outputDir+"/"+"PIXXL2BRes.png"+oFext, "Pixel barrel X Res L2", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL2BRes, outputDir+"/"+"PIXXL2BRes.eps"+oFext, "Pixel barrel X Res L2", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXXL2BRes, outputDir+"/"+"PIXXL2BRes.C"+oFext, "Pixel barrel X Res L2", "Hits on tracks / 2#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


#L0

SCTL0S0Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerSCT_L0_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL0S0Res, outputDir+"/"+"SCTL0S0BRes.png"+oFext, "SCT barrel X Res L0_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL0S0Res, outputDir+"/"+"SCTL0S0BRes.eps"+oFext, "SCT barrel X Res L0_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL0S0Res, outputDir+"/"+"SCTL0S0BRes.C"+oFext, "SCT barrel X Res L0_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


SCTL0S1Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerS1SCT_L0_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL0S1Res, outputDir+"/"+"SCTL0S1BRes.png"+oFext, "SCT barrel X Res L0_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL0S1Res, outputDir+"/"+"SCTL0S1BRes.eps"+oFext, "SCT barrel X Res L0_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL0S1Res, outputDir+"/"+"SCTL0S1BRes.C"+oFext, "SCT barrel X Res L0_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


#L1

SCTL1S0Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerSCT_L1_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL1S0Res, outputDir+"/"+"SCTL1S0BRes.png"+oFext, "SCT barrel X Res L1_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL1S0Res, outputDir+"/"+"SCTL1S0BRes.eps"+oFext, "SCT barrel X Res L1_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL1S0Res, outputDir+"/"+"SCTL1S0BRes.C"+oFext, "SCT barrel X Res L1_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTL1S1Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerS1SCT_L1_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL1S1Res, outputDir+"/"+"SCTL1S1BRes.png"+oFext, "SCT barrel X Res L1_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL1S1Res, outputDir+"/"+"SCTL1S1BRes.eps"+oFext, "SCT barrel X Res L1_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL1S1Res, outputDir+"/"+"SCTL1S1BRes.C"+oFext, "SCT barrel X Res L1_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)



#L2

SCTL2S0Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerSCT_L2_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL2S0Res, outputDir+"/"+"SCTL2S0BRes.png"+oFext, "SCT barrel X Res L2_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL2S0Res, outputDir+"/"+"SCTL2S0BRes.eps"+oFext, "SCT barrel X Res L2_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL2S0Res, outputDir+"/"+"SCTL2S0BRes.C"+oFext, "SCT barrel X Res L2_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTL2S1Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerS1SCT_L2_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL2S1Res, outputDir+"/"+"SCTL2S1BRes.png"+oFext, "SCT barrel X Res L2_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL2S1Res, outputDir+"/"+"SCTL2S1BRes.eps"+oFext, "SCT barrel X Res L2_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL2S1Res, outputDir+"/"+"SCTL2S1BRes.C"+oFext, "SCT barrel X Res L2_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)



#L3

SCTL3S0Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerSCT_L3_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL3S0Res, outputDir+"/"+"SCTL3S0BRes.png"+oFext, "SCT barrel X Res L3_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL3S0Res, outputDir+"/"+"SCTL3S0BRes.eps"+oFext, "SCT barrel X Res L3_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL3S0Res, outputDir+"/"+"SCTL3S0BRes.C"+oFext, "SCT barrel X Res L3_S0", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

SCTL3S1Res = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_Barrel_LayerS1SCT_L3_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTL3S1Res, outputDir+"/"+"SCTL3S1BRes.png"+oFext, "SCT barrel X Res L3_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL3S1Res, outputDir+"/"+"SCTL3S1BRes.eps"+oFext, "SCT barrel X Res L3_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(SCTL3S1Res, outputDir+"/"+"SCTL3S1BRes.C"+oFext, "SCT barrel X Res L3_S1", "Hits on tracks / 4#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)





PIXYL0BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_Y_vs_Silicon_Barrel_LayerPix_L0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXYL0BRes, outputDir+"/"+"PIXYL0BRes.png"+oFext, "Pixel barrel Y Res L0", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL0BRes, outputDir+"/"+"PIXYL0BRes.eps"+oFext, "Pixel barrel Y Res L0", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL0BRes, outputDir+"/"+"PIXYL0BRes.C"+oFext, "Pixel barrel Y Res L0", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXYL1BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_Y_vs_Silicon_Barrel_LayerPix_L1","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXYL1BRes, outputDir+"/"+"PIXYL1BRes.png"+oFext, "Pixel barrel Y Res L1", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL1BRes, outputDir+"/"+"PIXYL1BRes.eps"+oFext, "Pixel barrel Y Res L1", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL1BRes, outputDir+"/"+"PIXYL1BRes.C"+oFext, "Pixel barrel Y Res L1", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

PIXYL2BRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_Y_vs_Silicon_Barrel_LayerPix_L2","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXYL2BRes, outputDir+"/"+"PIXYL2BRes.png"+oFext, "Pixel barrel Y Res L2", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL2BRes, outputDir+"/"+"PIXYL2BRes.eps"+oFext, "Pixel barrel Y Res L2", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
DrawPlots(PIXYL2BRes, outputDir+"/"+"PIXYL2BRes.C"+oFext, "Pixel barrel Y Res L2", "Hits on tracks / 10#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)


#PIX ECA X 

for i in range(0,3):
    
    PIXXECALRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_ECA_LayerPix_L"+str(i),"noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXXECALRes, outputDir+"/"+"PIXXL"+str(i)+"ECARes.png"+oFext, "Pixel ECA X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXXECALRes, outputDir+"/"+"PIXXL"+str(i)+"ECARes.eps"+oFext, "Pixel ECA X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXXECALRes, outputDir+"/"+"PIXXL"+str(i)+"ECARes.C"+oFext, "Pixel ECA X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

    del PIXXECALRes


#PIX ECA Y

for i in range(0,3):
    
    PIXYECALRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_Y_vs_Silicon_ECA_LayerPix_L"+str(i),"noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXYECALRes, outputDir+"/"+"PIXYL"+str(i)+"ECARes.png"+oFext, "Pixel ECA Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXYECALRes, outputDir+"/"+"PIXYL"+str(i)+"ECARes.eps"+oFext, "Pixel ECA Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXYECALRes, outputDir+"/"+"PIXYL"+str(i)+"ECARes.C"+oFext, "Pixel ECA Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

    del PIXYECALRes


#PIX ECC X 

for i in range(0,3):
    
    PIXXECCLRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_ECC_LayerPix_L"+str(i),"noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXXECCLRes, outputDir+"/"+"PIXXL"+str(i)+"ECCRes.png"+oFext, "Pixel ECC X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXXECCLRes, outputDir+"/"+"PIXXL"+str(i)+"ECCRes.eps"+oFext, "Pixel ECC X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXXECCLRes, outputDir+"/"+"PIXXL"+str(i)+"ECCRes.C"+oFext, "Pixel ECC X Res L"+str(i), "Hits on tracks / 4#mum",
              "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    
    del PIXXECCLRes   
    

#PIX ECC Y

for i in range(0,3):
    
    PIXYECCLRes = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_Y_vs_Silicon_ECC_LayerPix_L"+str(i),"noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXYECCLRes, outputDir+"/"+"PIXYL"+str(i)+"ECCRes.png"+oFext, "Pixel ECC Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXYECCLRes, outputDir+"/"+"PIXYL"+str(i)+"ECCRes.eps"+oFext, "Pixel ECC Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)
    DrawPlots(PIXYECCLRes, outputDir+"/"+"PIXYL"+str(i)+"ECCRes.C"+oFext, "Pixel ECC Y Res L"+str(i), "Hits on tracks / 20#mum",
              "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.63,0.86)

    del PIXYECCLRes



#SCT EndCaps X I became smarter I guess..

EndCaps = ["ECA","ECC"]

for EndC in EndCaps:   #endcaps cycle
    for i in range(0,9): #layer cycle
        
        SCTECRes0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_"+EndC+"_LayerSCT_L"+str(i)+"_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
        DrawPlots(SCTECRes0, outputDir+"/"+"SCTL"+str(i)+"S0"+EndC+"Res.png"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S0", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)
        DrawPlots(SCTECRes0, outputDir+"/"+"SCTL"+str(i)+"S0"+EndC+"Res.eps"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S0", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)

        DrawPlots(SCTECRes0, outputDir+"/"+"SCTL"+str(i)+"S0"+EndC+"Res.C"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S0", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)
        
        SCTECRes1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"Residual_X_vs_Silicon_"+EndC+"_LayerS1SCT_L"+str(i)+"_S0","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
        DrawPlots(SCTECRes1, outputDir+"/"+"SCTL"+str(i)+"S1"+EndC+"Res.png"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S1", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)
        DrawPlots(SCTECRes1, outputDir+"/"+"SCTL"+str(i)+"S1"+EndC+"Res.eps"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S1", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)
        DrawPlots(SCTECRes1, outputDir+"/"+"SCTL"+str(i)+"S1"+EndC+"Res.C"+oFext, "SCT "+EndC+" X Res L"+str(i)+"_S1", "Hits on tracks / 4#mum",
                  "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.63,0.86)
        


        del SCTECRes1
        del SCTECRes0
        



