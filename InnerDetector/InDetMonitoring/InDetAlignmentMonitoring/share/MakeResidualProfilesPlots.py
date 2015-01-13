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

#SCTXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
#DrawPlots(SCTXEtaW, outputDir+"/"+"SCTXvsModEtaWidth."+oFext, "SCT Barrel", "residual width [mm]",
#          "Module Eta", 0.19, 0.9, "#mum",
#          canvasText,makeOutput,0.60,0.91,False)


SCTXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi, outputDir+"/"+"SCTXvsModPhi."+oFext, "SCT Barrel", "residual mean [mm]",
          "Module Phi", 0.19, 0.9, "#mum",
          canvasText,makeOutput,0.60,0.91,False)

#SCTXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(SCTXPhiW, outputDir+"/"+"SCTXvsModPhiWidth."+oFext, "SCT Barrel", "residual width [mm]",
#          "Module Phi", 0.19, 0.9, "#mum",
#          canvasText,makeOutput,0.60,0.91,False)

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


print " -- MakeResidualProfilesPlots -- completed "
