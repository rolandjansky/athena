#!/usr/bin/env python
# Author
# John Alison johnda@hep.upenn.edu
# Ben Cooper b.d.cooper@qmul.ac.uk
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
#gROOT.SetBatch(True)


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

print "Reading in from .root files:", rootFileNames[0], rootFileNames[1], rootFileNames[2]
if makeOutput:
    print "Producing output in directory:", outputDir

#getting the TFile objects using the rootFileNames
rootFiles = [TFile,TFile,TFile,TFile]
nFiles = 0
for i in range(4):
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

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.png", "Pixel Barrel", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY.png", "Pixel Barrel", "Number of hits on tracks",
          "y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX.png", "SCT Barrel", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR.png", "TRT Barrel", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

# Basic Pull distributions

PIXPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX, outputDir+"/"+"PIXPULLX.png", "Pixel Barrel", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

#PIXPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
#DrawPlots(PIXPULLY, outputDir+"/"+"PIXPULLY.png", "Pixel Barrel", "Number of hits on tracks",
#          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTBPULL = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_pullR","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTBPULL, outputDir+"/"+"TRTBPULL.png", "TRT Barrel", "Number of hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTBPULLNOTUBE = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_pullR_notube","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTBPULLNOTUBE, outputDir+"/"+"TRTBPULLNOTUBE.png", "TRT Barrel (NoTube)", "Number of non-tube hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTECAPULL = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_Endcap_A","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECAPULL, outputDir+"/"+"TRTECAPULL.png", "TRT Endcap A", "Number of hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTECCPULL = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_Endcap_C","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECCPULL, outputDir+"/"+"TRTECCPULL.png", "TRT Endcap C", "Number of hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTECAPULLNOTUBE = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube_Endcap_A","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECAPULLNOTUBE, outputDir+"/"+"TRTECAPULLNOTUBE.png", "TRT Endcap A (NoTube)", "Number of non-tube hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

TRTECCPULLNOTUBE = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube_Endcap_C","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECCPULLNOTUBE, outputDir+"/"+"TRTECCPULLNOTUBE.png", "TRT Endcap C (NoTube)", "Number of non-tube hits on tracks",
          "pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX0, outputDir+"/"+"PIXPULLX0.png", "Pixel Barrel L0", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX1, outputDir+"/"+"PIXPULLX1.png", "Pixel Barrel L1", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX2, outputDir+"/"+"PIXPULLX2.png", "Pixel Barrel L2", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY0, outputDir+"/"+"PIXPULLY0.png", "Pixel Barrel L0", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY1, outputDir+"/"+"PIXPULLY1.png", "Pixel Barrel L1", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY2, outputDir+"/"+"PIXPULLY2.png", "Pixel Barrel L2", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)


PIXPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECA, outputDir+"/"+"PIXPULLXECA.png", "Pixel Endcap A", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECAY, outputDir+"/"+"PIXPULLYECA.png", "Pixel Endcap A", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECC, outputDir+"/"+"PIXPULLXECC.png", "Pixel Endcap C", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

PIXPULLECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECCY, outputDir+"/"+"PIXPULLYECC.png", "Pixel Endcap C", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)


SCTPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX, outputDir+"/"+"SCTPULLX.png", "SCT Barrel", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX0, outputDir+"/"+"SCTPULLX0.png", "SCT Barrel L0", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX1, outputDir+"/"+"SCTPULLX1.png", "SCT Barrel L1", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX2, outputDir+"/"+"SCTPULLX2.png", "SCT Barrel L2", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX3, outputDir+"/"+"SCTPULLX3.png", "SCT Barrel L3", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECA, outputDir+"/"+"SCTPULLXECA.png", "SCT Endcap A", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SCTPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECC, outputDir+"/"+"SCTPULLXECC.png", "SCT Endcap C", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.60,0.91)

SiBarrelPullXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_pullX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelPullXMean, outputDir+"/"+"SiBarrelPullXMean.png", "Pixel/SCT Barrel", "pull x mean",
          "", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEcaPullXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_pullX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaPullXMean, outputDir+"/"+"SiEcaPullXMean.png", "Pixel/SCT Endcap A", "pull x mean",
          "", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEccPullXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_pullX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccPullXMean, outputDir+"/"+"SiEccPullXMean.png", "Pixel/SCT Endcap C", "pull x mean",
          "", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiBarrelPullXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_pullX_width","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelPullXWidth, outputDir+"/"+"SiBarrelPullXWidth.png", "Pixel/SCT Barrel", "pull x width",
          "", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

#SiEcaPullXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_pullX_width","noFit",rootFiles,nFiles,False)
#DrawPlots(SiEcaPullXWidth, outputDir+"/"+"SiEcaPullXWidth.png", "Pixel/SCT Endcap A", "pull x width",
#          "", 0.19, 0.9, "",
#          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEccPullXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_pullX_width","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccPullXWidth, outputDir+"/"+"SiEccPullXWidth.png", "Pixel/SCT Endcap C", "pull x width",
          "", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# Residual mean and width distributions as a function of layer, ring, stave in barrel 

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean.png", "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth.png", "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PixelXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEta, outputDir+"/"+"PixelXvsModEta.png", "Pixel Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+"PixelXvsModEtaWidth.png", "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+"PixelXvsModPhi.png", "Pixel Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+"PixelXvsModPhiWidth.png", "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEta, outputDir+"/"+"SCTXvsModEta.png", "SCT Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEtaW, outputDir+"/"+"SCTXvsModEtaWidth.png", "SCT Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


SCTXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi, outputDir+"/"+"SCTXvsModPhi.png", "SCT Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhiW, outputDir+"/"+"SCTXvsModPhiWidth.png", "SCT Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXX0.png", "Pixel Barrel L0", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1.png", "Pixel Barrel L1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2.png", "Pixel Barrel L2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+"SCTX0.png", "SCT Barrel L0", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+"SCTX1.png", "SCT Barrel L1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+"SCTX2.png", "SCT Barrel L2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+"SCTX3.png", "SCT Barrel L3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

# Basic Endcap Residual distributions

## draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX.png", "Pixel Endcap", "Number of hits on tracks",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+"PIXECAX.png", "PIX Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+"PIXECCX.png", "PIX Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY.png", "Pixel Endcap", "Number of hits on tracks",
          "Local y residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

PIXECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAY, outputDir+"/"+"PIXECAY.png", "PIX Endcap A", "Number of hits on tracks",
          "y residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCY, outputDir+"/"+"PIXECCY.png", "PIX Endcap C", "Number of hits on tracks",
          "y residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)


# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX.png", "SCT Endcap", "Number of hits on tracks",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+"SCTECAX.png", "SCT Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+"SCTECCX.png", "SCT Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC.png", "TRT Endcap", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput, 0.62, 0.91)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+"TRTECA.png", "TRT Endcap A", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+"TRTECC.png", "TRT Endcap C", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

# Residual mean and width distributions as a function of endcap disk
SiEcaResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXMean, outputDir+"/"+"SiEcaResXMean.png", "Pixel/SCT Endcap A", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEcaResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXWidth, outputDir+"/"+"SiEcaResXWidth.png", "Pixel/SCT Endcap A", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEccResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXMean, outputDir+"/"+"SiEccResXMean.png", "Pixel/SCT Endcap C", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SiEccResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXWidth, outputDir+"/"+"SiEccResXWidth.png", "Pixel/SCT Endcap C", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# endcap residual distributions for each silicon disk

PIXECAX_DISK1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_disk1","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX_DISK1, outputDir+"/"+"PIXECAX_DISK1.png", "Pixel X ECA Disk 1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXECAX_DISK2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_disk2","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX_DISK2, outputDir+"/"+"PIXECAX_DISK2.png", "Pixel X ECA Disk 2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXECAX_DISK3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_disk3","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX_DISK3, outputDir+"/"+"PIXECAX_DISK3.png", "Pixel X ECA Disk 3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXECCX_DISK1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_disk1","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX_DISK1, outputDir+"/"+"PIXECCX_DISK1.png", "Pixel X ECC Disk 1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXECCX_DISK2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_disk2","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX_DISK2, outputDir+"/"+"PIXECCX_DISK2.png", "Pixel X ECC Disk 2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXECCX_DISK3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_disk3","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX_DISK3, outputDir+"/"+"PIXECCX_DISK3.png", "Pixel X ECC Disk 3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)



SCTECAX_DISK1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk1","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK1, outputDir+"/"+"SCTECAX_DISK1.png", "SCT X ECA Disk 1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk2","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK2, outputDir+"/"+"SCTECAX_DISK2.png", "SCT X ECA Disk 2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk3","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK3, outputDir+"/"+"SCTECAX_DISK3.png", "SCT X ECA Disk 3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK4 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk4","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK4, outputDir+"/"+"SCTECAX_DISK4.png", "SCT X ECA Disk 4", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK5 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk5","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK5, outputDir+"/"+"SCTECAX_DISK5.png", "SCT X ECA Disk 5", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK6 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk6","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK6, outputDir+"/"+"SCTECAX_DISK6.png", "SCT X ECA Disk 6", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK7 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk7","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK7, outputDir+"/"+"SCTECAX_DISK7.png", "SCT X ECA Disk 7", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECAX_DISK8 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_disk8","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX_DISK8, outputDir+"/"+"SCTECAX_DISK8.png", "SCT X ECA Disk 8", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)


SCTECCX_DISK1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk1","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK1, outputDir+"/"+"SCTECCX_DISK1.png", "SCT X ECC Disk 1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk2","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK2, outputDir+"/"+"SCTECCX_DISK2.png", "SCT X ECC Disk 2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk3","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK3, outputDir+"/"+"SCTECCX_DISK3.png", "SCT X ECC Disk 3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK4 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk4","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK4, outputDir+"/"+"SCTECCX_DISK4.png", "SCT X ECC Disk 4", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK5 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk5","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK5, outputDir+"/"+"SCTECCX_DISK5.png", "SCT X ECC Disk 5", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK6 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk6","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK6, outputDir+"/"+"SCTECCX_DISK6.png", "SCT X ECC Disk 6", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK7 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk7","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK7, outputDir+"/"+"SCTECCX_DISK7.png", "SCT X ECC Disk 7", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTECCX_DISK8 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_disk8","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX_DISK8, outputDir+"/"+"SCTECCX_DISK8.png", "SCT X ECC Disk 8", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)


## Residual mean/width as a function of internal endcap disks

PixelECAXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelECAXPhi, outputDir+"/"+"PixelECAXvsModPhi.png", "Pixel Endcap A", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


PixelECCXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelECCXPhi, outputDir+"/"+"PixelECCXvsModPhi.png", "Pixel Endcap C", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECAXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECAXPhi, outputDir+"/"+"SCTECAXvsModPhi.png", "SCT Endcap A", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


SCTECCXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECCXPhi, outputDir+"/"+"SCTECCXvsModPhi.png", "SCT Endcap C", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


## TRT residual mean and width distributions

TRTBL0PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHI, outputDir+"/"+"TRTBLayer0vsPhi.png", "TRT Barrel Layer 0", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBL0PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHIRMS, outputDir+"/"+"TRTBLayer0vsPhiRMS.png", "TRT Barrel Layer 0", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBL1PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHI, outputDir+"/"+"TRTBLayer1vsPhi.png", "TRT Barrel Layer 1", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBL1PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHIRMS, outputDir+"/"+"TRTBLayer1vsPhiRMS.png", "TRT Barrel Layer 1", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBL2PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHI, outputDir+"/"+"TRTBLayer2vsPhi.png", "TRT Barrel Layer 2", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBL2PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHIRMS, outputDir+"/"+"TRTBLayer2vsPhiRMS.png", "TRT Barrel Layer 2", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBRADIUS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUS, outputDir+"/"+"TRTBVsStrawLayer.png", "TRT Barrel", "residual mean [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBRADIUSRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUSRMS, outputDir+"/"+"TRTBVsStrawLayerRMS.png", "TRT Barrel", "residual RMS [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECAPHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsPhiSec_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECAPHI, outputDir+"/"+"TRTECAvsPhi.png", "TRT Endcap A", "residual mean [mm]",
          "Phi Sector", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCPHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsPhiSec_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCPHI, outputDir+"/"+"TRTECCvsPhi.png", "TRT Endcap C", "residual mean [mm]",
          "Phi Sector", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECAPHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsPhiSec_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECAPHIRMS, outputDir+"/"+"TRTECAvsPhiRMS.png", "TRT Endcap A", "residual RMS [mm]",
          "Phi Sector", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCPHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsPhiSec_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCPHIRMS, outputDir+"/"+"TRTECCvsPhiRMS.png", "TRT Endcap C", "residual RMS [mm]",
          "Phi Sector", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECARING = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsRing_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECARING, outputDir+"/"+"TRTECAvsRing.png", "TRT Endcap A", "residual mean [mm]",
          "Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCRING = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsRing_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCRING, outputDir+"/"+"TRTECCvsRing.png", "TRT Endcap C", "residual mean [mm]",
          "Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECARINGRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsRing_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECARINGRMS, outputDir+"/"+"TRTECAvsRingRMS.png", "TRT Endcap A", "residual RMS [mm]",
          "Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCRINGRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_rmsResVsRing_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCRINGRMS, outputDir+"/"+"TRTECCvsRingRMS.png", "TRT Endcap C", "residual RMS [mm]",
          "Ring", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


## Generic Track

TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0.png", "", "N Tracks",
          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackZ0PV = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0_pvcorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0PV, outputDir+"/"+"z0PV.png", "", "N Tracks",
          "z0 w.r.t PV [mm]", 0.19, 0.9, "#mum",
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

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_nolog","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT_nolog.png", "", "N Tracks",
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




# Hit Efficiencies

SiBarrelHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelHitEff, outputDir+"/"+"SiBarrelHitEff.png", "Si Barrel Hit Eff.", "Hit Efficiency",
          "Barrel Layer", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

SiEcaHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_eca","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaHitEff, outputDir+"/"+"SiEcaHitEff.png", "Si Endcap A Hit Eff.", "Hit Efficiency",
          "ECA Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

SiEccHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccHitEff, outputDir+"/"+"SiEccHitEff.png", "Si Endcap C Hit Eff.", "Hit Efficiency",
          "ECC Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

TRTB0HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0HitEff, outputDir+"/"+"TRTB0HitEff.png", "TRT Barrel Layer 0 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 0 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

TRTB1HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1HitEff, outputDir+"/"+"TRTB1HitEff.png", "TRT Barrel Layer 1 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 1 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

TRTB2HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2HitEff, outputDir+"/"+"TRTB2HitEff.png", "TRT Barrel Layer 2 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 2 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

TRTECAHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_ring_trt_ec_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECAHitEff, outputDir+"/"+"TRTECAHitEff.png", "TRT Endcap A Hit Eff.", "Hit Efficiency",
          "Endcap A Wheel", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

TRTECCHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_ring_trt_ec_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCHitEff, outputDir+"/"+"TRTECCHitEff.png", "TRT Endcap C Hit Eff.", "Hit Efficiency",
          "Endcap C Wheel", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False,"SmallLegend")

## SivsTRT

TRTExtEffEta = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_eta0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTExtEffEta, outputDir+"/"+"TRTExtEffVsEta.png", "TRT Extension Eff.", "TRT Extension Eff.",
          "Track Eta", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTExtEffPhi = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_phi0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTExtEffPhi, outputDir+"/"+"TRTExtEffVsPhi.png", "TRT Extension Eff.", "TRT Extension Eff.",
          "Track Phi", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

## Generic Tracks asymmetry plots
#
# charge asymmetry vs track pT
ASYM_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_pT_asym","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PT, outputDir+"/"+"TrackAsym_pT.png", "Full ID", "(neg-pos)/pos+neg",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_PT_BARREL = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_pT_asym_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PT_BARREL, outputDir+"/"+"TrackAsym_barrel_pT.png", "Barrel", "(neg-pos)/pos+neg",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_PT_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_pT_asym_eca","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PT_ECA, outputDir+"/"+"TrackAsym_eca_pT.png", "Endcap A", "(neg-pos)/pos+neg",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_PT_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_pT_asym_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PT_ECC, outputDir+"/"+"TrackAsym_ecc_pT.png", "Endcap C", "(neg-pos)/pos+neg",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# charge asymmetry vs track phi0

#there is no overall ID phi0 asymmetry plot
ASYM_PHI_BARREL = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_phi0_asym_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PHI_BARREL, outputDir+"/"+"TrackAsym_phi_barrel.png", "Barrel", "(neg-pos)/pos+neg",
          "Track Phi0", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_PHI_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_phi0_asym_eca","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PHI_ECA, outputDir+"/"+"TrackAsym_phi_eca.png", "Endcap A", "(neg-pos)/pos+neg",
          "Track Phi0", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_PHI_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_phi0_asym_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_PHI_ECC, outputDir+"/"+"TrackAsym_phi_ecc.png", "Endcap C", "(neg-pos)/pos+neg",
          "Track Phi0", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# charge asymmetry vs track d0 (corrected for primary vtx)

ASYM_D0C = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0c_asym","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_D0C, outputDir+"/"+"TrackAsym_d0c.png", "Full ID", "(neg-pos)/pos+neg",
          "Track d0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_D0C_BARREL = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0c_asym_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_D0C_BARREL, outputDir+"/"+"TrackAsym_barrel_d0c.png", "Barrel", "(neg-pos)/pos+neg",
          "Track d0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_D0C_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0c_asym_eca","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_D0C_ECA, outputDir+"/"+"TrackAsym_eca_d0c.png", "Endcap A", "(neg-pos)/pos+neg",
          "Track d0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_D0C_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0c_asym_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_D0C_ECC, outputDir+"/"+"TrackAsym_ecc_d0c.png", "Endcap C", "(neg-pos)/pos+neg",
          "Track d0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# charge asymmetry vs track z0 (corrected for primary vtx)

ASYM_Z0C = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_z0c_asym","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_Z0C, outputDir+"/"+"TrackAsym_z0c.png", "Full ID", "(neg-pos)/pos+neg",
          "Track z0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_Z0C_BARREL = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_z0c_asym_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_Z0C_BARREL, outputDir+"/"+"TrackAsym_barrel_z0c.png", "Barrel", "(neg-pos)/pos+neg",
          "Track z0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_Z0C_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_z0c_asym_eca","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_Z0C_ECA, outputDir+"/"+"TrackAsym_eca_z0c.png", "Endcap A", "(neg-pos)/pos+neg",
          "Track z0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ASYM_Z0C_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_z0c_asym_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_Z0C_ECC, outputDir+"/"+"TrackAsym_ecc_z0c.png", "Endcap C", "(neg-pos)/pos+neg",
          "Track z0 [mm] (corrected for pvtx)", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


# charge asymmetry vs track eta0

ASYM_ETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta_asym","noFit",rootFiles,nFiles,False)
DrawPlots(ASYM_ETA, outputDir+"/"+"TrackAsym_eta.png", "Full ID", "(neg-pos)/pos+neg",
          "Track #eta", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

# residual means as a function of track pT 

PIXBRESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(PIXBRESX_PT, outputDir+"/"+"PixBarrelResXMeanVsPt.png", "Pixel Barrel", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXBRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(PIXBRESXFWHM_PT, outputDir+"/"+"PixBarrelResXFWHMVsPt.png", "Pixel Barrel", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECARESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECARESX_PT, outputDir+"/"+"PixECAResXMeanVsPt.png", "Pixel ECA", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECARESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECARESXFWHM_PT, outputDir+"/"+"PixECAResXFWHMVsPt.png", "Pixel ECA", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECCRESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECCRESX_PT, outputDir+"/"+"PixECCResXMeanVsPt.png", "Pixel ECC", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECCRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECCRESXFWHM_PT, outputDir+"/"+"PixECCResXFWHMVsPt.png", "Pixel ECC", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTBRESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SCTBRESX_PT, outputDir+"/"+"SctBarrelResXMeanVsPt.png", "SCT Barrel", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTBRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(SCTBRESXFWHM_PT, outputDir+"/"+"SctBarrelResXFWHMVsPt.png", "SCT Barrel", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECARESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECARESX_PT, outputDir+"/"+"SctECAResXMeanVsPt.png", "SCT ECA", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECARESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECARESXFWHM_PT, outputDir+"/"+"SctECAResXFWHMVsPt.png", "SCT ECA", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECCRESX_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECCRESX_PT, outputDir+"/"+"SctECCResXMeanVsPt.png", "SCT ECC", "mean local residual x [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECCRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECCRESXFWHM_PT, outputDir+"/"+"SctECCResXFWHMVsPt.png", "SCT ECC", "local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBRES_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_residual_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRES_PT, outputDir+"/"+"TRTBarrelResMeanVsPt.png", "TRT Barrel", "mean residual [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTBRESFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_residual_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRESFWHM_PT, outputDir+"/"+"TRTBarrelResFWHMVsPt.png", "TRT Barrel", "residual FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECARES_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_eca_residual_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECARES_PT, outputDir+"/"+"TRTECAResMeanVsPt.png", "TRT Endcap A", "mean residual [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECARESFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_eca_residual_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECARESFWHM_PT, outputDir+"/"+"TRTECAResFWHMVsPt.png", "TRT Endcap A", "residual FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCRES_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ecc_residual_pt_mean","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCRES_PT, outputDir+"/"+"TRTECCResMeanVsPt.png", "TRT Endcap C", "mean residual [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

TRTECCRESFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ecc_residual_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCRESFWHM_PT, outputDir+"/"+"TRTECCResFWHMVsPt.png", "TRT Endcap C", "residual FWHM/2.35 [mm]",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


## Pixel Pull widths as a function of track pT
PIXBPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(PIXBPULLXWIDTH_PT, outputDir+"/"+"PixBarrelPullXWidthVsPt.png", "Pixel Barrel", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECAPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECAPULLXWIDTH_PT, outputDir+"/"+"PixECAPullXWidthVsPt.png", "Pixel ECA", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

PIXECCPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(PIXECCPULLXWIDTH_PT, outputDir+"/"+"PixECCPullXWidthVsPt.png", "Pixel ECC", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTBPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTBPULLXWIDTH_PT, outputDir+"/"+"SctBarrelPullXWidthVsPt.png", "SCT Barrel", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECAPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECAPULLXWIDTH_PT, outputDir+"/"+"SctECAPullXWidthVsPt.png", "SCT ECA", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

SCTECCPULLXWIDTH_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_pullx_pt_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTECCPULLXWIDTH_PT, outputDir+"/"+"SctECCPullXWidthVsPt.png", "SCT ECC", "width pull x",
          "Track p_{T}", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")



# track parameter errors

ERRD0_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRD0_PT, outputDir+"/"+"errD0vspT.png", "Full ID", "#sigma_{d0} [mm]",
          "Track p_{T}", 0.35, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ERRTHETA_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errCotThetaVsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRTHETA_PT, outputDir+"/"+"errCotThetavspT.png", "Full ID", "#sigma_{cot#theta}",
          "Track p_{T}", 0.35, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ERRPHI0_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errPhi0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRPHI0_PT, outputDir+"/"+"errPhivspT.png", "Full ID", "#sigma_{#phi}",
          "Track p_{T}", 0.35, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ERRPT_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errPtVsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRPT_PT, outputDir+"/"+"errPtvspT.png", "Full ID", "#sigma_{p_{T}} [GeV]",
          "Track p_{T}", 0.35, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

ERRCURV_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errPt_Pt2VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRCURV_PT, outputDir+"/"+"errCurvaturevspT.png", "Full ID", "#sigma_{q/p_{T}} [1/GeV]",
          "Track p_{T}", 0.35, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")


# plots of impact parameter (corrected for beamspot) versus other track parameters

TrackD0BS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_bscorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0BS, outputDir+"/"+"d0BS.png", "", "N Tracks",
          "Impact Parameter w.r.t BS [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackD0PV = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_pvcorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0PV, outputDir+"/"+"d0PV.png", "", "N Tracks",
          "Impact Parameter w.r.t PV [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

D0BSvsPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI, outputDir+"/"+"d0BSvsPHI.png", "Full ID", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPHI_Barrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_Barrel","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_Barrel, outputDir+"/"+"d0BSvsPHI_Barrel.png", "ID Barrel", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPHI_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_ECA","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_ECA, outputDir+"/"+"d0BSvsPHI_ECA.png", "ID ECA", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPHI_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_ECC","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_ECC, outputDir+"/"+"d0BSvsPHI_ECC.png", "ID ECC", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPt","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPT, outputDir+"/"+"d0BSvsPT.png", "Full ID", "mean d_{0} (corrected for beamspot) [mm]",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPT_Barrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPt_Barrel","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPT_Barrel, outputDir+"/"+"d0BSvsPT_Barrel.png", "ID Barrel", "mean d_{0} (corrected for beamspot) [mm]",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPT_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPt_ECA","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPT_ECA, outputDir+"/"+"d0BSvsPT_ECA.png", "ID ECA", "mean d_{0} (corrected for beamspot) [mm]",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsPT_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPt_ECC","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPT_ECC, outputDir+"/"+"d0BSvsPT_ECC.png", "ID ECC", "mean d_{0} (corrected for beamspot) [mm]",
          "Track p_{T} [GeV]", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")

D0BSvsETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsEta","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsETA, outputDir+"/"+"d0BSvsETA.png", "Full ID", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #eta", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False,"SmallLegend")






