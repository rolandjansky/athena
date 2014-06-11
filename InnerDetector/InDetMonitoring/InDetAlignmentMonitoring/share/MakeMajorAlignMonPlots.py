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


nMAXFiles = 3
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

# Basic Barrel Residual distributions

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX."+oFext, "Pixel barrel", "Hits on tracks / 10#mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY."+oFext, "Pixel barrel", "Hits on tracks / 20#mum",
          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
          "Local x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

#SCTY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
#DrawPlots(SCTY, outputDir+"/"+"SCTY."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
#          "Local y residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)


TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR."+oFext, "TRT barrel", "Hits on tracks / 22#mum",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

# d0 plots added 

#TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFit",rootFiles,nFiles,normaliseHistos)
#DrawPlots(TrackD0, outputDir+"/"+"d0."+oFext, "", "N Tracks",
#          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
#          canvasText,makeOutput)

TrackD0_phi_barr = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0_vs_phi_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(TrackD0_phi_barr, outputDir+"/"+"d0_phi_barr."+oFext, "Barrel", "Impact Parameter [mm]", "#phi [rad] " , 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackD0_phi_eca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0_vs_phi_eca","noFit",rootFiles,nFiles,False)
DrawPlots(TrackD0_phi_eca, outputDir+"/"+"d0_phi_eca."+oFext, "Endcap A", "Impact Parameter [mm]", "#phi [rad] " , 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackD0_phi_ecc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"trk_d0_vs_phi_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(TrackD0_phi_ecc, outputDir+"/"+"d0_phi_ecc."+oFext, "Endcap C", "Impact Parameter [mm]", "#phi [rad] " , 0.19, 0.9, "#mum",
          canvasText,makeOutput)


# Residual mean and width distributions as a function of layer, ring, stave in barrel 

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean."+oFext, "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth."+oFext, "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

PixelXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEta, outputDir+"/"+"PixelXvsModEta."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False) #False

PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+"PixelXvsModEtaWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)


PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+"PixelXvsModPhi."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+"PixelXvsModPhiWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

#Priscilla's add

PixelYPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelYPhi, outputDir+"/"+"PixelYvsModPhi."+oFext, "Pixel Barrel", "residual Y mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# PixelYPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(PixelYPhiW, outputDir+"/"+"PixelYvsModPhiWidth."+oFext, "Pixel Barrel", "residual Y width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)


PixelXPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi_eca, outputDir+"/"+"PixelXvsModPhi_eca."+oFext, "Pixel Endcap A", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# PixelXPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(PixelXPhiW_eca, outputDir+"/"+"PixelXvsModPhiWidth_eca."+oFext, "Pixel Endcap A", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)

PixelYPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelYPhi_eca, outputDir+"/"+"PixelYvsModPhi_eca."+oFext, "Pixel Endcap A", "residual Y mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# PixelYPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(PixelYPhiW_eca, outputDir+"/"+"PixelYvsModPhiWidth_eca."+oFext, "Pixel Endcap A", "residual Y width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)

PixelXPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi_ecc, outputDir+"/"+"PixelXvsModPhi_ecc."+oFext, "Pixel Endcap C", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# PixelXPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(PixelXPhiW_ecc, outputDir+"/"+"PixelXvsModPhiWidth_ecc."+oFext, "Pixel Endcap C", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)

PixelYPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelYPhi_ecc, outputDir+"/"+"PixelYvsModPhi_ecc."+oFext, "Pixel Endcap C", "residual Y mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# PixelYPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(PixelYPhiW_ecc, outputDir+"/"+"PixelYvsModPhiWidth_ecc."+oFext, "Pixel Endcap C", "residual Y width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)
# --


SCTXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEta, outputDir+"/"+"SCTXvsModEta."+oFext, "SCT Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SCTXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXEtaW, outputDir+"/"+"SCTXvsModEtaWidth."+oFext, "SCT Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)


SCTXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi, outputDir+"/"+"SCTXvsModPhi."+oFext, "SCT Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

SCTXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhiW, outputDir+"/"+"SCTXvsModPhiWidth."+oFext, "SCT Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# Priscilla's add
SCTXPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_eca, outputDir+"/"+"SCTXvsModPhi_eca."+oFext, "SCT Endcap A", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# SCTXPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_eca, outputDir+"/"+"SCTXvsModPhiWidth_eca."+oFext, "SCT Endcap A", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)

SCTXPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_ecc, outputDir+"/"+"SCTXvsModPhi_ecc."+oFext, "SCT Endcap C", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

# SCTXPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_ecc, outputDir+"/"+"SCTXvsModPhiWidth_ecc."+oFext, "SCT Endcap C", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.60,0.91,False)


## Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXX0."+oFext, "Pixel Barrel L0", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1."+oFext, "Pixel Barrel L1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2."+oFext, "Pixel Barrel L2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+"SCTX0."+oFext, "SCT Barrel L0", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+"SCTX1."+oFext, "SCT Barrel L1", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+"SCTX2."+oFext, "SCT Barrel L2", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+"SCTX3."+oFext, "SCT Barrel L3", "Number of hits on tracks",
          "x residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput,0.60,0.91)

## Basic Endcap Residual distributions

# draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX."+oFext, "Pixel end-caps", "Hits on tracks / 20#mum",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY."+oFext, "Pixel end-caps", "Hits on tracks / 20#mum",
          "Local y residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)



PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+"PIXECAX."+oFext, "PIX Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+"PIXECCX."+oFext, "PIX Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX."+oFext, "SCT Endcap", "Hits on tracks / 10#mum",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)


SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+"SCTECAX."+oFext, "SCT Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+"SCTECCX."+oFext, "SCT Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+"TRTECA."+oFext, "TRT Endcap A", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+"TRTECC."+oFext, "TRT Endcap C", "Number of hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC."+oFext, "TRT end-caps", "Hits on tracks",
          "residual [mm]", 0.2, 0.9, "#mum",canvasText,makeOutput)


# Residual mean and width distributions as a function of endcap disk
#SiEcaResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_mean","noFit",rootFiles,nFiles,False)
#DrawPlots(SiEcaResXMean, outputDir+"/"+"SiEcaResXMean."+oFext, "Pixel/SCT Endcap A", "residual mean [mm]",
#          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

SiEcaResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_eca_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaResXWidth, outputDir+"/"+"SiEcaResXWidth."+oFext, "Pixel/SCT Endcap A", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

SiEccResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXMean, outputDir+"/"+"SiEccResXMean."+oFext, "Pixel/SCT Endcap C", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

SiEccResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_ecc_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccResXWidth, outputDir+"/"+"SiEccResXWidth."+oFext, "Pixel/SCT Endcap C", "residual width [mm]",
          "", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

# TRT residual mean and width distributions

TRTBL0PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHI, outputDir+"/"+"TRTBLayer0vsPhi."+oFext, "TRT Barrel Layer 0", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBL0PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL0PHIRMS, outputDir+"/"+"TRTBLayer0vsPhiRMS."+oFext, "TRT Barrel Layer 0", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBL1PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHI, outputDir+"/"+"TRTBLayer1vsPhi."+oFext, "TRT Barrel Layer 1", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBL1PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL1PHIRMS, outputDir+"/"+"TRTBLayer1vsPhiRMS."+oFext, "TRT Barrel Layer 1", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBL2PHI = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHI, outputDir+"/"+"TRTBLayer2vsPhi."+oFext, "TRT Barrel Layer 2", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBL2PHIRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBL2PHIRMS, outputDir+"/"+"TRTBLayer2vsPhiRMS."+oFext, "TRT Barrel Layer 2", "residual RMS [mm]",
          "Module Phi", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBRADIUS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUS, outputDir+"/"+"TRTBVsStrawLayer."+oFext, "TRT Barrel", "residual mean [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)

TRTBRADIUSRMS = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_rmsResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBRADIUSRMS, outputDir+"/"+"TRTBVsStrawLayerRMS."+oFext, "TRT Barrel", "residual RMS [mm]",
          "Straw Layer", 0.19, 0.9, "#mum",canvasText,makeOutput,0.60,0.91,False)


# Add TRT endcap plots e.g. trt_ec_aveResVsPhiSec_Endcap_A, trt_ec_rmsResVsPhiSec_Endcap_A, trt_ec_aveResVsRing_Endcap_A, trt_ec_rmsResVsRing_Endcap_A




## Generic Track

TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0."+oFext, "", "N Tracks",
          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0, outputDir+"/"+"z0."+oFext, "", "N Tracks",
          "z0 [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"phi","noFit",rootFiles,nFiles, False)#,normaliseHistos)
DrawPlots(TrackPHI, outputDir+"/"+"phi."+oFext, "", "N Tracks",
          "phi [radians]", 0.19, 0.9, "radians",
          canvasText,makeOutput) # none orig

TrackETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackETA, outputDir+"/"+"eta."+oFext, "", "N Tracks",
          "eta", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT."+oFext, "", "N Tracks",
          "pT", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackSUMMARY = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"summary","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackSUMMARY, outputDir+"/"+"summary."+oFext, "N Hits Summary", "N Tracks",
          "", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackCHI2DOF = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"chi2oDoF","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackCHI2DOF, outputDir+"/"+"chi2oDoF."+oFext, "", "N Tracks",
          "Chi2/DOF", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNTRACKS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"ntracks","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRACKS, outputDir+"/"+"ntracks."+oFext, "Tracks/Event", "N Events",
          "N Tracks", 0.69, 0.6, "",
          canvasText,makeOutput) 

TrackNTRTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Ntrthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRTHits, outputDir+"/"+"nTRTHits."+oFext, "", "N Tracks",
          "N TRT Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHits, outputDir+"/"+"nSCTHits."+oFext, "", "N Tracks",
          "N SCT Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsBarrel, outputDir+"/"+"nSCTHitsBarrel."+oFext, "", "N Tracks",
          "N SCT Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEca, outputDir+"/"+"nSCTHitsEca."+oFext, "", "N Tracks",
          "N SCT Hits Eca", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEcc, outputDir+"/"+"nSCTHitsEcc."+oFext, "", "N Tracks",
          "N SCT Hits Ecc", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHits, outputDir+"/"+"nPIXHits."+oFext, "", "N Tracks",
          "N PIX Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsBarrel, outputDir+"/"+"nPIXHitsBarrel."+oFext, "", "N Tracks",
          "N PIX Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEca, outputDir+"/"+"nPIXHitsEca."+oFext, "", "N Tracks",
          "N PIX Hits ECA", 0.69, 0.6, "",
          canvasText,makeOutput)


TrackNPIXHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEcc, outputDir+"/"+"nPIXHitsEcc."+oFext, "", "N Tracks",
          "N PIX Hits ECC", 0.69, 0.6, "",
          canvasText,makeOutput)




# Hit Efficiencies

SiBarrelHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelHitEff, outputDir+"/"+"SiBarrelHitEff."+oFext, "Si Barrel Hit Eff.", "Hit Efficiency",
          "Barrel Layer", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,True)

SiEcaHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_eca","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaHitEff, outputDir+"/"+"SiEcaHitEff."+oFext, "Si Endcap A Hit Eff.", "Hit Efficiency",
          "ECA Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

SiEccHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccHitEff, outputDir+"/"+"SiEccHitEff."+oFext, "Si Endcap C Hit Eff.", "Hit Efficiency",
          "ECC Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB0HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0HitEff, outputDir+"/"+"TRTB0HitEff."+oFext, "TRT Barrel Layer 0 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 0 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB1HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1HitEff, outputDir+"/"+"TRTB1HitEff."+oFext, "TRT Barrel Layer 1 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 1 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB2HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2HitEff, outputDir+"/"+"TRTB2HitEff."+oFext, "TRT Barrel Layer 2 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 2 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

#TRTECAHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ea","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECAHitEff, outputDir+"/"+"TRTECAHitEff."+oFext, "TRT Endcap A Hit Eff.", "Hit Efficiency",
#          "Endcap A Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41,False)

#TRTECCHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ec","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECCHitEff, outputDir+"/"+"TRTECCHitEff."+oFext, "TRT Endcap C Hit Eff.", "Hit Efficiency",
#          "Endcap C Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41,False)

# SivsTRT

if siTRTDir[0] != "":
    TRTExtEffEta = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_eta0","noFit",rootFiles,nFiles,False)
    DrawPlots(TRTExtEffEta, outputDir+"/"+"TRTExtEffVsEta."+oFext, "TRT Extension Eff.", "TRT Extension Eff.",
              "Track Eta", 0.19, 0.41, "",
              canvasText,makeOutput,0.60,0.41,False)

    TRTExtEffPhi = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_phi0","noFit",rootFiles,nFiles,False)
    DrawPlots(TRTExtEffPhi, outputDir+"/"+"TRTExtEffVsPhi."+oFext, "TRT Extension Eff.", "TRT Extension Eff.",
              "Track Phi", 0.19, 0.41, "",
              canvasText,makeOutput,0.60,0.41,False)





