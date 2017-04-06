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
# ./MakeAlignmentMonitoringPlots_ForCosmics.py -c <configurationFile>
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

## Basic Barrel Residual distributions

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX.png", "Pixel Barrel", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx_fine","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX.png", "SCT Barrel", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR.png", "TRT Barrel", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

# Residual mean and width distributions as a function of layer, ring, stave in barrel 

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean.png", "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth.png", "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

PixelXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEta, outputDir+"/"+"PixelXvsModEta.png", "Pixel Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+"PixelXvsModEtaWidth.png", "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)


PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+"PixelXvsModPhi.png", "Pixel Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+"PixelXvsModPhiWidth.png", "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SCTXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEta, outputDir+"/"+"SCTXvsModEta.png", "SCT Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SCTXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEtaW, outputDir+"/"+"SCTXvsModEtaWidth.png", "SCT Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)


SCTXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi, outputDir+"/"+"SCTXvsModPhi.png", "SCT Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SCTXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhiW, outputDir+"/"+"SCTXvsModPhiWidth.png", "SCT Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)


## Individual Barrel Layer Residual Distributions

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

## Basic Endcap Residual distributions

PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+"PIXECAX.png", "PIX Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+"PIXECCX.png", "PIX Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+"SCTECAX.png", "SCT Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+"SCTECCX.png", "SCT Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)


# Residual mean and width distributions as a function of endcap disk
SiEcaResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXMean, outputDir+"/"+"SiEcaResXMean.png", "Pixel/SCT Endcap A", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput)

SiEcaResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXWidth, outputDir+"/"+"SiEcaResXWidth.png", "Pixel/SCT Endcap A", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput)

SiEccResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXMean, outputDir+"/"+"SiEccResXMean.png", "Pixel/SCT Endcap C", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput)

SiEccResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXWidth, outputDir+"/"+"SiEccResXWidth.png", "Pixel/SCT Endcap C", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput)

# TRT residual mean and width distributions

TRTBL0PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHI, outputDir+"/"+"TRTBLayer0vsPhi.png", "TRT Barrel Layer 0", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBL0PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHIRMS, outputDir+"/"+"TRTBLayer0vsPhiRMS.png", "TRT Barrel Layer 0", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBL1PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHI, outputDir+"/"+"TRTBLayer1vsPhi.png", "TRT Barrel Layer 1", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBL1PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHIRMS, outputDir+"/"+"TRTBLayer1vsPhiRMS.png", "TRT Barrel Layer 1", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBL2PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHI, outputDir+"/"+"TRTBLayer2vsPhi.png", "TRT Barrel Layer 2", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBL2PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHIRMS, outputDir+"/"+"TRTBLayer2vsPhiRMS.png", "TRT Barrel Layer 2", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBRADIUS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUS, outputDir+"/"+"TRTBVsStrawLayer.png", "TRT Barrel", "residual mean [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput)

TRTBRADIUSRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUSRMS, outputDir+"/"+"TRTBVsStrawLayerRMS.png", "TRT Barrel", "residual RMS [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput)


# Add TRT endcap plots e.g. trt_ec_aveResVsPhiSec_Endcap_A, trt_ec_rmsResVsPhiSec_Endcap_A, trt_ec_aveResVsRing_Endcap_A, trt_ec_rmsResVsRing_Endcap_A




## Generic Track

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

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_nolog","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT.png", "", "N Tracks",
          "pT", 0.19, 0.9, "",
          canvasText,makeOutput)

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
          canvasText,makeOutput,0.60,0.41)

SiEcaHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_eca","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaHitEff, outputDir+"/"+"SiEcaHitEff.png", "Si Endcap A Hit Eff.", "Hit Efficiency",
          "ECA Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41)

SiEccHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccHitEff, outputDir+"/"+"SiEccHitEff.png", "Si Endcap C Hit Eff.", "Hit Efficiency",
          "ECC Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41)

TRTB0HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0HitEff, outputDir+"/"+"TRTB0HitEff.png", "TRT Barrel Layer 0 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 0 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41)

TRTB1HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1HitEff, outputDir+"/"+"TRTB1HitEff.png", "TRT Barrel Layer 1 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 1 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41)

TRTB2HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2HitEff, outputDir+"/"+"TRTB2HitEff.png", "TRT Barrel Layer 2 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 2 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41)

#TRTECAHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ea","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECAHitEff, outputDir+"/"+"TRTECAHitEff.png", "TRT Endcap A Hit Eff.", "Hit Efficiency",
#          "Endcap A Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41)
#
#TRTECCHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ec","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECCHitEff, outputDir+"/"+"TRTECCHitEff.png", "TRT Endcap C Hit Eff.", "Hit Efficiency",
#          "Endcap C Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41)


# Split track distributions

DELTAD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAD0,outputDir+"/"+"DELTAD0.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta d0 [mm]",0.2,0.9,"#mum",
          canvasText,makeOutput);

DELTAPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAPHI0,outputDir+"/"+"DELTAPHI0.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta phi [radians]",0.2,0.95,"mrad",
          canvasText,makeOutput);

DELTAETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAETA,outputDir+"/"+"DELTAETA.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta #eta",0.2,0.95,"",
          canvasText,makeOutput);

DELTAQOPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPt","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAQOPT,outputDir+"/"+"DELTAQOPT.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta Q/p_{T}  [GeV^{-1}]",0.2,0.95,"TeV^{-1}",
          canvasText,makeOutput);

DELTAZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAZ0,outputDir+"/"+"DELTAZ0.png","#Delta(Up-Down) Tracks","number of tracks"
          ,"#Delta z0 [mm]",0.2,0.90,"#mum",
          canvasText,makeOutput);



SIGMAQPTVSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_qOverPtVsPt","noFit",rootFiles,nFiles,False)
DrawPlots(SIGMAQPTVSPT,outputDir+"/"+"SIGMAQPTVSPT.png","","#sigma(q/p_{T}) [1/GeV]"
          ,"#p_{T}^{upper} [GeV]",0.4,0.80,"#mum",
          canvasText,makeOutput,0.60,0.91,True,"SmallLegend");

SIGMAD0VSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_d0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(SIGMAD0VSPT,outputDir+"/"+"SIGMAD0VSPT.png","","#sigma(d_{0})"
          ,"#p_{T}^{upper} [GeV]",0.4,0.80,"#mum",
          canvasText,makeOutput,0.60,0.91,True,"SmallLegend");

## Split track distributions as function of other parameters
#
## vs eta
DELTAD0VSETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_d0VsEta","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAD0VSETA,outputDir+"/"+"DELTAD0VSETA.png","","<#Deltad0(Up-Down Tracks>"
          ,"#eta^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAZ0VSETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_z0VsEta","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAZ0VSETA,outputDir+"/"+"DELTAZ0VSETA.png","","<#Deltaz0(Up-Down Tracks>"
          ,"#eta^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAPHI0VSETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_phi0VsEta","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAPHI0VSETA,outputDir+"/"+"DELTAPHI0VSETA.png","","<#Deltaphi0(Up-Down Tracks>"
          ,"#eta^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAQOVERPTVSETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_qOverPtVsEta","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAQOVERPTVSETA,outputDir+"/"+"DELTAQOVERPTVSETA.png","","<#DeltaqOverPt(Up-Down Tracks>"
          ,"#eta^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAETA0VSETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_eta0VsEta","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAETA0VSETA,outputDir+"/"+"DELTAETA0VSETA.png","","<#Deltaeta0(Up-Down Tracks>"
          ,"#eta^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

# vs phi
DELTAD0VSPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_d0VsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAD0VSPHI0,outputDir+"/"+"DELTAD0VSPHI0.png","","<#Deltad0(Up-Down Tracks>"
          ,"#phi0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAZ0VSPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_z0VsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAZ0VSPHI0,outputDir+"/"+"DELTAZ0VSPHI0.png","","<#Deltaz0(Up-Down Tracks>"
          ,"#phi0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAPHI0VSPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_phi0VsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAPHI0VSPHI0,outputDir+"/"+"DELTAPHI0VSPHI0.png","","<#Deltaphi0(Up-Down Tracks>"
          ,"#phi0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAQOVERPTVSPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_qOverPtVsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAQOVERPTVSPHI0,outputDir+"/"+"DELTAQOVERPTVSPHI0.png","","<#DeltaqOverPt(Up-Down Tracks>"
          ,"#phi0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAETA0VSPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_eta0VsPhi0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAETA0VSPHI0,outputDir+"/"+"DELTAETA0VSPHI0.png","","<#Deltaeta0(Up-Down Tracks>"
          ,"#phi0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

# vs d0
DELTAD0VSD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_d0VsD0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAD0VSD0,outputDir+"/"+"DELTAD0VSD0.png","","<#Deltad0(Up-Down Tracks>"
          ,"#d0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAZ0VSD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_z0VsD0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAZ0VSD0,outputDir+"/"+"DELTAZ0VSD0.png","","<#Deltaz0(Up-Down Tracks>"
          ,"#d0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAPHI0VSD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_phi0VsD0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAPHI0VSD0,outputDir+"/"+"DELTAPHI0VSD0.png","","<#Deltaphi0(Up-Down Tracks>"
          ,"#d0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAQOVERPTVSD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_qOverPtVsD0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAQOVERPTVSD0,outputDir+"/"+"DELTAQOVERPTVSD0.png","","<#DeltaqOverPt(Up-Down Tracks>"
          ,"#d0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAETA0VSD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_eta0VsD0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAETA0VSD0,outputDir+"/"+"DELTAETA0VSD0.png","","<#Deltaeta0(Up-Down Tracks>"
          ,"#d0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");


# vs z0
DELTAD0VSZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_d0VsZ0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAD0VSZ0,outputDir+"/"+"DELTAD0VSZ0.png","","<#Deltad0(Up-Down Tracks>"
          ,"#z0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAZ0VSZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_z0VsZ0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAZ0VSZ0,outputDir+"/"+"DELTAZ0VSZ0.png","","<#Deltaz0(Up-Down Tracks>"
          ,"#z0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAPHI0VSZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_phi0VsZ0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAPHI0VSZ0,outputDir+"/"+"DELTAPHI0VSZ0.png","","<#Deltaphi0(Up-Down Tracks>"
          ,"#z0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAQOVERPTVSZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_qOverPtVsZ0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAQOVERPTVSZ0,outputDir+"/"+"DELTAQOVERPTVSZ0.png","","<#DeltaqOverPt(Up-Down Tracks>"
          ,"#z0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAETA0VSZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_eta0VsZ0","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAETA0VSZ0,outputDir+"/"+"DELTAETA0VSZ0.png","","<#Deltaeta0(Up-Down Tracks>"
          ,"#z0^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

# vs pT
DELTAD0VSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_d0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAD0VSPT,outputDir+"/"+"DELTAD0VSPT.png","","<#Deltad0(Up-Down Tracks>"
          ,"#p_{T}^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAZ0VSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_z0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAZ0VSPT,outputDir+"/"+"DELTAZ0VSPT.png","","<#Deltaz0(Up-Down Tracks>"
          ,"#p_{T}^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAPHI0VSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_phi0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAPHI0VSPT,outputDir+"/"+"DELTAPHI0VSPT.png","","<#Deltaphi0(Up-Down Tracks>"
          ,"#p_{T}^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAQOVERPTVSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_qOverPtVsPt","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAQOVERPTVSPT,outputDir+"/"+"DELTAQOVERPTVSPT.png","","<#DeltaqOverPt(Up-Down Tracks>"
          ,"#p_{T}^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

DELTAETA0VSPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"average_delta_eta0VsPt","noFit",rootFiles,nFiles,False)
DrawPlots(DELTAETA0VSPT,outputDir+"/"+"DELTAETA0VSPT.png","","<#Deltaeta0(Up-Down Tracks>"
          ,"#p_{T}^{upper}",0.2,0.45,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");



## We draw the curve for positive and negative tracks on top of each other for each file (so files are not compared in this case)
newLegendTitles = ["Negative Tracks","Positive Tracks"]

# as function of eta
histogramNames = ["average_delta_d0VsEta_n","average_delta_d0VsEta_p"]
for i in range(nFiles):
    DELTAD0VSETA = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAD0VSETA,outputDir+"/"+"DELTAD0VSETA_"+str(i)+".png",legendTitles[i],"<#Deltad0(Up-Down) Tracks>"
          ,"#eta^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_z0VsEta_n","average_delta_z0VsEta_p"]
for i in range(nFiles):
    DELTAZ0VSETA = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAZ0VSETA,outputDir+"/"+"DELTAZ0VSETA_"+str(i)+".png",legendTitles[i],"<#Deltaz0(Up-Down) Tracks>"
          ,"#eta^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_phi0VsEta_n","average_delta_phi0VsEta_p"]
for i in range(nFiles):
    DELTAPHI0VSETA = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAPHI0VSETA,outputDir+"/"+"DELTAPHI0VSETA_"+str(i)+".png",legendTitles[i],"<#Deltaphi0(Up-Down) Tracks>"
          ,"#eta^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_qOverPtVsEta_n","average_delta_qOverPtVsEta_p"]
for i in range(nFiles):
    DELTAQOVERPTVSETA = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAQOVERPTVSETA,outputDir+"/"+"DELTAQOVERPTVSETA_"+str(i)+".png",legendTitles[i],"<#DeltaqOverPt(Up-Down) Tracks>"
          ,"#eta^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_eta0VsEta_n","average_delta_eta0VsEta_p"]
for i in range(nFiles):
    DELTAETA0VSETA = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAETA0VSETA,outputDir+"/"+"DELTAETA0VSETA_"+str(i)+".png",legendTitles[i],"<#Deltaeta0(Up-Down) Tracks>"
          ,"#eta^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");



# as function of d0
histogramNames = ["average_delta_d0VsD0_n","average_delta_d0VsD0_p"]
for i in range(nFiles):
    DELTAD0VSD0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAD0VSD0,outputDir+"/"+"DELTAD0VSD0_"+str(i)+".png",legendTitles[i],"<#Deltad0(Up-Down) Tracks>"
          ,"d0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_z0VsD0_n","average_delta_z0VsD0_p"]
for i in range(nFiles):
    DELTAZ0VSD0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAZ0VSD0,outputDir+"/"+"DELTAZ0VSD0_"+str(i)+".png",legendTitles[i],"<#Deltaz0(Up-Down) Tracks>"
          ,"d0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_phi0VsD0_n","average_delta_phi0VsD0_p"]
for i in range(nFiles):
    DELTAPHI0VSD0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAPHI0VSD0,outputDir+"/"+"DELTAPHI0VSD0_"+str(i)+".png",legendTitles[i],"<#Deltaphi0(Up-Down) Tracks>"
          ,"d0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

istogramNames = ["average_delta_qOverPtVsD0_n","average_delta_qOverPtVsD0_p"]
or i in range(nFiles):
   DELTAQOVERPTVSD0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
   DrawPlots(DELTAQOVERPTVSD0,outputDir+"/"+"DELTAQOVERPTVSD0_"+str(i)+".png",legendTitles[i],"<#DeltaqOverPt(Up-Down) Tracks>"
         ,"d0^{upper}",0.2,0.90,"#mum",
         canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_eta0VsD0_n","average_delta_eta0VsD0_p"]
for i in range(nFiles):
    DELTAETA0VSD0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAETA0VSD0,outputDir+"/"+"DELTAETA0VSD0_"+str(i)+".png",legendTitles[i],"<#Deltaeta0(Up-Down) Tracks>"
          ,"d0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

# as function of z0
histogramNames = ["average_delta_d0VsZ0_n","average_delta_d0VsZ0_p"]
for i in range(nFiles):
    DELTAD0VSZ0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAD0VSZ0,outputDir+"/"+"DELTAD0VSZ0_"+str(i)+".png",legendTitles[i],"<#Deltad0(Up-Down) Tracks>"
          ,"z0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_z0VsZ0_n","average_delta_z0VsZ0_p"]
for i in range(nFiles):
    DELTAZ0VSZ0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAZ0VSZ0,outputDir+"/"+"DELTAZ0VSZ0_"+str(i)+".png",legendTitles[i],"<#Deltaz0(Up-Down) Tracks>"
          ,"z0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_phi0VsZ0_n","average_delta_phi0VsZ0_p"]
for i in range(nFiles):
    DELTAPHI0VSZ0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAPHI0VSZ0,outputDir+"/"+"DELTAPHI0VSZ0_"+str(i)+".png",legendTitles[i],"<#Deltaphi0(Up-Down) Tracks>"
          ,"z0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

istogramNames = ["average_delta_qOverPtVsZ0_n","average_delta_qOverPtVsZ0_p"]
or i in range(nFiles):
   DELTAQOVERPTVSZ0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
   DrawPlots(DELTAQOVERPTVSZ0,outputDir+"/"+"DELTAQOVERPTVSZ0_"+str(i)+".png",legendTitles[i],"<#DeltaqOverPt(Up-Down) Tracks>"
         ,"z0^{upper}",0.2,0.90,"#mum",
         canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_eta0VsZ0_n","average_delta_eta0VsZ0_p"]
for i in range(nFiles):
    DELTAETA0VSZ0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAETA0VSZ0,outputDir+"/"+"DELTAETA0VSZ0_"+str(i)+".png",legendTitles[i],"<#Deltaeta0(Up-Down) Tracks>"
          ,"z0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

# as function of pT
histogramNames = ["average_delta_d0VsPt_n","average_delta_d0VsPt_p"]
for i in range(nFiles):
    DELTAD0VSPT = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAD0VSPT,outputDir+"/"+"DELTAD0VSPT_"+str(i)+".png",legendTitles[i],"<#Deltad0(Up-Down) Tracks>"
          ,"p_{T}^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_z0VsPt_n","average_delta_z0VsPt_p"]
for i in range(nFiles):
    DELTAZ0VSPT = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAZ0VSPT,outputDir+"/"+"DELTAZ0VSPT_"+str(i)+".png",legendTitles[i],"<#Deltaz0(Up-Down) Tracks>"
          ,"p_{T}^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_phi0VsPt_n","average_delta_phi0VsPt_p"]
for i in range(nFiles):
    DELTAPHI0VSPT = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAPHI0VSPT,outputDir+"/"+"DELTAPHI0VSPT_"+str(i)+".png",legendTitles[i],"<#Deltaphi0(Up-Down) Tracks>"
          ,"p_{T}^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

istogramNames = ["average_delta_qOverPtVsPt_n","average_delta_qOverPtVsPt_p"]
or i in range(nFiles):
   DELTAQOVERPTVSPT = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
   DrawPlots(DELTAQOVERPTVSPT,outputDir+"/"+"DELTAQOVERPTVSPT_"+str(i)+".png",legendTitles[i],"<#DeltaqOverPt(Up-Down) Tracks>"
         ,"p_{T}^{upper}",0.2,0.90,"#mum",
         canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_eta0VsPt_n","average_delta_eta0VsPt_p"]
for i in range(nFiles):
    DELTAETA0VSPT = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAETA0VSPT,outputDir+"/"+"DELTAETA0VSPT_"+str(i)+".png",legendTitles[i],"<#Deltaeta0(Up-Down) Tracks>"
          ,"p_{T}^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");


# as function of phi0
histogramNames = ["average_delta_d0VsPhi0_n","average_delta_d0VsPhi0_p"]
for i in range(nFiles):
    DELTAD0VSPHI0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAD0VSPHI0,outputDir+"/"+"DELTAD0VSPHI0_"+str(i)+".png",legendTitles[i],"<#Deltad0(Up-Down) Tracks>"
          ,"phi0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_z0VsPhi0_n","average_delta_z0VsPhi0_p"]
for i in range(nFiles):
    DELTAZ0VSPHI0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAZ0VSPHI0,outputDir+"/"+"DELTAZ0VSPHI0_"+str(i)+".png",legendTitles[i],"<#Deltaz0(Up-Down) Tracks>"
          ,"phi0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_phi0VsPhi0_n","average_delta_phi0VsPhi0_p"]
for i in range(nFiles):
    DELTAPHI0VSPHI0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAPHI0VSPHI0,outputDir+"/"+"DELTAPHI0VSPHI0_"+str(i)+".png",legendTitles[i],"<#Deltaphi0(Up-Down) Tracks>"
          ,"phi0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_qOverPtVsPhi0_n","average_delta_qOverPtVsPhi0_p"]
for i in range(nFiles):
    DELTAQOVERPTVSPHI0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAQOVERPTVSPHI0,outputDir+"/"+"DELTAQOVERPTVSPHI0_"+str(i)+".png",legendTitles[i],"<#DeltaqOverPt(Up-Down) Tracks>"
          ,"phi0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");

histogramNames = ["average_delta_eta0VsPhi0_n","average_delta_eta0VsPhi0_p"]
for i in range(nFiles):
    DELTAETA0VSPHI0 = MakeTwoPlotsFromSameFile(splitTracksDir,newLegendTitles,markerColors,markerStyles,histogramNames,"noFit",rootFiles,i,False)
    DrawPlots(DELTAETA0VSPHI0,outputDir+"/"+"DELTAETA0VSPHI0_"+str(i)+".png",legendTitles[i],"<#Deltaeta0(Up-Down) Tracks>"
          ,"phi0^{upper}",0.2,0.90,"#mum",
          canvasText,makeOutput,0.60,0.91,False,"SmallLegend");
