#!/usr/bin/env python
# Author
# John Alison johnda@hep.upenn.edu
# Ben Cooper b.d.cooper@qmul.ac.uk
# this file also needs the utilities.py and AtlasStyle.py files

# used to draw the alignment monitoring plots side-by-side of up to 3 monitoring.root files (number of files can be 1,2 or 3)

##--------------------------
# usage:
#
# ./MakeAlignmentMonitoringPlots.py -c <configurationFile>
#
##--------------------------

import sys
from ROOT import *
from utilities import *
#import AtlasStyle
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

print "Reading in from .root files:", rootFileNames[0], rootFileNames[1], rootFileNames[2]
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

#PIXB_XRESMEAN_1D = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xres_mean1d","noFit",rootFiles,nFiles,normaliseHistos,unitArea)
#DrawPlots(PIXB_XRESMEAN_1D, outputDir+"/"+"PIXB_XRESMEAN_1D.png", "Pixel barrel", "Number of Modules",
#          "<Local x residual> [#mum]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")
#
#PIXB_XRESMEANERROR_1D = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xres_meanerror1d","noFit",rootFiles,nFiles,normaliseHistos,unitArea)
#DrawPlots(PIXB_XRESMEANERROR_1D, outputDir+"/"+"PIXB_XRESMEANERROR_1D.png", "Pixel barrel", "Number of Modules",
#          "<Local x residual> Error [#mum]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")
#
#PIXB_XRESNENTRIES_1D = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xres_nentries1d","noFit",rootFiles,nFiles,normaliseHistos,unitArea)
#DrawPlots(PIXB_XRESNENTRIES_1D, outputDir+"/"+"PIXB_XRESNENTRIES_1D.png", "Pixel barrel", "Number of Modules",
#          "<Local x residual> Error [#mum]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")
#
#PIXB_YRESMEAN_1D = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_yres_mean1d","noFit",rootFiles,nFiles,normaliseHistos,unitArea)
#DrawPlots(PIXB_YRESMEAN_1D, outputDir+"/"+"PIXB_YRESMEAN_1D.png", "Pixel barrel", "Number of Modules",
#          "<Local x residual> [#mum]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")


# Basic Barrel Residual distributions

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.eps", "Pixel barrel", "Hits on tracks / 4 #mum ",
          "Local x residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX.eps", "Pixel end-caps", "Hits on tracks / 4 #mum ",
          "Local x residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY.eps", "Pixel barrel", "Hits on tracks / 8 #mum ",
          "Local y residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY.eps", "Pixel end-caps", "Hits on tracks / 32 #mum ",
          "Local y residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX.eps", "SCT barrel", "Hits on tracks / 4 #mum ",
          "Local x residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX.eps", "SCT end-caps", "Hits on tracks / 4 #mum ",
          "Local x residual [mm]", 0.22, 0.90, "#mum",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")


TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR.eps", "TRT barrel", "Hits on tracks / 12 #mum ",
          "Residual [mm]", 0.21, 0.90, "#mum",canvasText,makeOutput, 0.62, 0.87,True,"ApprovedPlotsMay")

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC.eps", "TRT end-caps", "Hits on tracks / 12 #mum ",
          "Residual [mm]", 0.21, 0.90, "#mum",canvasText,makeOutput, 0.62, 0.87,True,"ApprovedPlotsMay")


TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+"TRTECA.eps", "TRT endcap A", "Hits on tracks / 12 #mum ",
          "Residual [mm]", 0.21, 0.90, "#mum",canvasText,makeOutput, 0.62, 0.87,True,"ApprovedPlotsMay")

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+"TRTECC.eps", "TRT endcap C", "Hits on tracks / 12 #mum ",
          "Residual [mm]", 0.21, 0.90, "#mum",canvasText,makeOutput, 0.62, 0.87,True,"ApprovedPlotsMay")

# Residual Mean as a function of barrel layer, endcap disk.

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean.eps", "Pixel/SCT Barrel", "Residual mean [mm]",
          "Barrel Layer", 0.22, 0.9, "#mum",
          canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

SiEcaResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXMean, outputDir+"/"+"SiEcaResXMean.eps", "Pixel/SCT Endcap A", "Residual mean [mm]",
          "", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

SiEccResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXMean, outputDir+"/"+"SiEccResXMean.eps", "Pixel/SCT Endcap A", "Residual mean [mm]",
          "", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

TRTBRADIUS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUS, outputDir+"/"+"TRTBVsStrawLayer.eps", "TRT Barrel", "Residual mean [mm]",
          "Straw Layer", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

TRTECAPHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsPhiSec_Endcap_A","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECAPHI, outputDir+"/"+"TRTECAvsPhi.eps", "TRT Endcap A", "Residual mean [mm]",
          "Phi Sector", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

TRTECCPHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_aveResVsPhiSec_Endcap_C","noFit",rootFiles,nFiles,False)
DrawPlots(TRTECCPHI, outputDir+"/"+"TRTECCvsPhi.eps", "TRT Endcap C", "Residual mean [mm]",
          "Phi Sector", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

# Residual Width as a function of track pT

PIXBRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(PIXBRESXFWHM_PT, outputDir+"/"+"PixBarrelResXFWHMVsPt.eps", "Pixel Barrel", "Local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

SCTBRESXFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(SCTBRESXFWHM_PT, outputDir+"/"+"SctBarrelResXFWHMVsPt.eps", "SCT Barrel", "Local residual x FWHM/2.35 [mm]",
          "Track p_{T}", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

TRTBRESFWHM_PT = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_residual_pt_fwhm","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRESFWHM_PT, outputDir+"/"+"TRTBarrelResFWHMVsPt.eps", "TRT Barrel", "Residual FWHM/2.35 [mm]",
          "Track p_{T}", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

# Basic Pull distributions

PIXPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX, outputDir+"/"+"PIXPULLX.eps", "Pixel Barrel", "Hits on tracks",
          "x Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY, outputDir+"/"+"PIXPULLY.eps", "Pixel Barrel", "Hits on tracks",
          "y Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXECPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECPULLX, outputDir+"/"+"PIXECPULLX.eps", "Pixel Endcaps", "Hits on tracks",
          "x Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

PIXECPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECPULLY, outputDir+"/"+"PIXECPULLY.eps", "Pixel Endcaps", "Hits on tracks",
          "y Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

SCTPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX, outputDir+"/"+"SCTPULLX.eps", "SCT Barrel", "Hits on tracks",
          "x Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

SCTECPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECPULLX, outputDir+"/"+"SCTECPULLX.eps", "SCT Endcaps", "Hits on tracks",
          "x Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

TRTBPULLNOTUBE = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_pullR_notube","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTBPULLNOTUBE, outputDir+"/"+"TRTBPULLNOTUBE.eps", "TRT Barrel", "Precision hits on tracks",
          "Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

TRTECPULLNOTUBE = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECPULLNOTUBE, outputDir+"/"+"TRTECPULLNOTUBE.eps", "TRT Endcaps", "Precision hits on tracks",
          "Pull", 0.22, 0.9, "",canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")


# Impact parameter plots

TrackD0BS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_bscorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0BS, outputDir+"/"+"d0BS.eps", "", "N Tracks",
          "Impact Parameter (corrected for beamspot) [mm]", 0.22, 0.9, "#mum",
          canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

TrackD0PV = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_pvcorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0PV, outputDir+"/"+"d0PV.eps", "", "N Tracks",
          "Impact Parameter (corrected for primary vertex) [mm]", 0.22, 0.9, "#mum",
          canvasText,makeOutput,0.62,0.87,True,"ApprovedPlotsMay")

D0BSvsPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI, outputDir+"/"+"d0BSvsPHI.eps", "", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

D0BSvsPHI_Barrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_Barrel","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_Barrel, outputDir+"/"+"d0BSvsPHI_Barrel.eps", "Barrel Tracks", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

D0BSvsPHI_ECA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_ECA","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_ECA, outputDir+"/"+"d0BSvsPHI_ECA.eps", "Endcap A Tracks", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

D0BSvsPHI_ECC = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_ECC","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsPHI_ECC, outputDir+"/"+"d0BSvsPHI_ECC.eps", "Endcap C Tracks", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #phi", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87 ,False,"ApprovedPlotsMay")

D0BSvsETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsEta","noFit",rootFiles,nFiles,False)
DrawPlots(D0BSvsETA, outputDir+"/"+"d0BSvsETA.eps", "Full ID", "mean d_{0} (corrected for beamspot) [mm]",
          "Track #eta", 0.22, 0.9, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

# Track parameter errors

ERRD0_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRD0_PT, outputDir+"/"+"errD0vspT.eps", "", "#sigma_{d0} [mm]",
          "Track p_{T}", 0.55, 0.6, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")

ERRCURV_PT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"errPt_Pt2VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(ERRCURV_PT, outputDir+"/"+"errCurvaturevspT.eps", "", "#sigma_{q/p_{T}} [1/GeV]",
          "Track p_{T}", 0.55, 0.6, "#mum",canvasText,makeOutput,0.62,0.87,False,"ApprovedPlotsMay")
