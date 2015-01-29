#--

#
# Basic Barrel Residual distributions
#
# Pixel
PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX."+oFext, "Pixel barrel", "Hits on tracks / 10#mum",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY."+oFext, "Pixel barrel", "Hits on tracks / 20#mum",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

# SCT
SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

#SCTY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
#DrawPlots(SCTY, outputDir+"/"+"SCTY."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
#          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

# TRT
TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR."+oFext, "TRT barrel", "Hits on tracks / 22#mum",
          "residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput)

## Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXIBL_X."+oFext, "Pixel Barrel IBL", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1."+oFext, "Pixel Barrel L0", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2."+oFext, "Pixel Barrel L1", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX3, outputDir+"/"+"PIXX3."+oFext, "Pixel Barrel L2", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

# pixel barrel local y
PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+"PIXIBL_Y."+oFext, "Pixel Barrel IBL", "Number of hits on tracks",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY1, outputDir+"/"+"PIXY1."+oFext, "Pixel Barrel L0", "Number of hits on tracks",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY2, outputDir+"/"+"PIXY2."+oFext, "Pixel Barrel L1", "Number of hits on tracks",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

PIXY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY3, outputDir+"/"+"PIXY3."+oFext, "Pixel Barrel L2", "Number of hits on tracks",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

# sct barrel
SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+"SCTX0."+oFext, "SCT Barrel L0", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+"SCTX1."+oFext, "SCT Barrel L1", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+"SCTX2."+oFext, "SCT Barrel L2", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+"SCTX3."+oFext, "SCT Barrel L3", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.65,0.89)

#
# Residual mean and width distributions as a function of layer, ring, stave in barrel 
#

PixelXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEta, outputDir+"/"+"PixelXvsModEta."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False) #False
'''
PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+"PixelXvsModEtaWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+"PixelXvsModPhi."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+"PixelXvsModPhiWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)
'''
SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean."+oFext, "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth."+oFext, "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

# Priscilla's add
SCTXPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_eca, outputDir+"/"+"SCTXvsModPhi_eca."+oFext, "SCT Endcap A", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

# SCTXPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_eca, outputDir+"/"+"SCTXvsModPhiWidth_eca."+oFext, "SCT Endcap A", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.65,0.89,False)

SCTXPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_ecc, outputDir+"/"+"SCTXvsModPhi_ecc."+oFext, "SCT Endcap C", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89,False)

# SCTXPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_ecc, outputDir+"/"+"SCTXvsModPhiWidth_ecc."+oFext, "SCT Endcap C", "residual width [mm]",
#           "Module Phi", 0.19, 0.9, "#mum",
#           canvasText,makeOutput,0.65,0.89,False)


## Basic Endcap Residual distributions

# draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX."+oFext, "Pixel end-caps", "Hits on tracks",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY."+oFext, "Pixel end-caps", "Hits on tracks",
          "Local y residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)

PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+"PIXECAX."+oFext, "PIX Endcap A", "Number of hits on tracks",
          "Local x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+"PIXECCX."+oFext, "PIX Endcap C", "Number of hits on tracks",
          "Local x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAY, outputDir+"/"+"PIXECAY."+oFext, "PIX Endcap A", "Number of hits on tracks",
          "Local y residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

PIXECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCY, outputDir+"/"+"PIXECCY."+oFext, "PIX Endcap C", "Number of hits on tracks",
          "Local y residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)


# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX."+oFext, "SCT Endcap", "Hits on tracks / 10#mum",
          "Local x residual [mm]", 0.21, 0.93, "#mum",canvasText,makeOutput,0.62,0.91)


SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+"SCTECAX."+oFext, "SCT Endcap A", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+"SCTECCX."+oFext, "SCT Endcap C", "Number of hits on tracks",
          "x residual [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.65,0.89)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+"TRTECA."+oFext, "TRT Endcap A", "Number of hits on tracks",
          "residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+"TRTECC."+oFext, "TRT Endcap C", "Number of hits on tracks",
          "residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput)

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC."+oFext, "TRT end-caps", "Hits on tracks",
          "residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput)

#
# -- Extended plots
#
if (userExtended or True):
    # IBL split per 3D and planar sensors
    # -- local x
    PIXIBL_3DC_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DCSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DC_xRES, outputDir+"/"+"PIXIBL_3DC_xRES."+oFext, "IBL 3D sensors. C side", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)

    PIXIBL_3DA_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DASensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DA_xRES, outputDir+"/"+"PIXIBL_3DA_xRES."+oFext, "IBL 3D sensors. A side", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)

    PIXIBL_3D_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_xRES, outputDir+"/"+"PIXIBL_3D_xRES."+oFext, "IBL 3D sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)
    
    PIXIBL_Planar_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_xRES, outputDir+"/"+"PIXIBL_Planar_xRES."+oFext, "IBL Planar sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)
    
    # -- local y
    PIXIBL_3D_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_yRES, outputDir+"/"+"PIXIBL_3D_yRES."+oFext, "IBL 3D sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)
    
    PIXIBL_Planar_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_yRES, outputDir+"/"+"PIXIBL_Planar_yRES."+oFext, "IBL Planar sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)
    

if (userExtended):
    PIXX0vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_inciphi","noFit",rootFiles,nFiles,True)
    
    PIXX1vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsINCANG,outputDir+"/"+"PIXX1vsINCANG."+oFext,"","Local x residual [mm]","Incident angle [radians]",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX2vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsINCANG,outputDir+"/"+"PIXX2vsINCANG."+oFext,"","Local x residual [mm]","Incident angle [radians]",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX3vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsINCANG,outputDir+"/"+"PIXX3vsINCANG."+oFext,"","Local x residual [mm]","Incident angle [radians]",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsCLUSIZ,outputDir+"/"+"PIXX0vsCLUSIZE."+oFext,"","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    PIXX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsCLUSIZ,outputDir+"/"+"PIXX1vsCLUSIZE."+oFext,"","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsCLUSIZ,outputDir+"/"+"PIXX2vsCLUSIZE."+oFext,"","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsCLUSIZ,outputDir+"/"+"PIXX3vsCLUSIZE."+oFext,"","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


print " -- MakeResidualPlots -- completed "
