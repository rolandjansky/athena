#
#
#
#
PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXIBL_X."+oFext, "Pixel Barrel IBL", "Number of hits on tracks",
          "Local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+"PIXIBL_Y."+oFext, "Pixel Barrel IBL", "Number of hits on tracks",
          "Local y residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

#

IBL_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(IBL_xRESvsETA, outputDir+"/"+"IBL_xRESvsETA."+oFext, "IBL", "Average local x residual",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.41,0.40, False)

IBL_yRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(IBL_yRESvsETA, outputDir+"/"+"IBL_yRESvsETA."+oFext, "IBL", "Average local y residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)


PIX1_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b1_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX1_xRESvsETA, outputDir+"/"+"PIX1_xRESvsETA."+oFext, "Pixel layer 1", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

PIX2_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b2_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX2_xRESvsETA, outputDir+"/"+"PIX2_xRESvsETA."+oFext, "Pixel layer 1", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

PIX3_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b3_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX3_xRESvsETA, outputDir+"/"+"PIX3_xRESvsETA."+oFext, "Pixel layer 1", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

IBL_xRESvsETAupper =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 3, 6)
DrawPlots(IBL_xRESvsETAupper, outputDir+"/"+"IBL_xRESvsETAupper."+oFext, "Upper IBL", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

IBL_xRESvsETAlower =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 10, 13)
DrawPlots(IBL_xRESvsETAlower, outputDir+"/"+"IBL_xRESvsETAlower."+oFext, "Lower IBL", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

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

for stave in range(14): 
    IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, stave+1, stave+1) 
    DrawPlots(IBLStave, outputDir+"/"+"IBL_xRESvsETA_stave"+str(stave)+"."+oFext, "IBL stave "+str(stave), "Average local x residual [mm]",
              "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

    IBLStave = MakeModuleResPlotsFrom3D  (residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", 
                                         "noFitWithStats", rootFiles, nFiles, True, stave+1)
    DrawPlots(IBLStave, outputDir+"/"+"IBL_xRES_stave"+str(stave)+"."+oFext, "IBL stave "+str(stave), "Hits", "Local x residual [#mum]",
              0.18, 0.88, "#mum", canvasText, makeOutput, 0.65, 0.89, True)

    IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, stave+1, stave+1) 
    DrawPlots(IBLStave, outputDir+"/"+"IBL_yRESvsETA_stave"+str(stave)+"."+oFext, "IBL stave "+str(stave), "Average local y residual [mm]",
              "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

    IBLStave = MakeModuleResPlotsFrom3D  (residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", 
                                         "noFitWithStats", rootFiles, nFiles, True, stave+1)
    DrawPlots(IBLStave, outputDir+"/"+"IBL_yRES_stave"+str(stave)+"."+oFext, "IBL stave "+str(stave), "Hits", "Local y residual [#mum]",
              0.18, 0.88, "#mum", canvasText, makeOutput, 0.65, 0.89, True)

# stave bending plot for upper staves
IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 3, 6) 
DrawPlots(IBLStave, outputDir+"/"+"IBL_xRESvsETA_stave_upper."+oFext, "Upper IBL staves", "Average local x residual [mm]",
          "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 11, 14) 
DrawPlots(IBLStave, outputDir+"/"+"IBL_xRESvsETA_stave_lower."+oFext, "Lower IBL staves", "Average local x residual [mm]",
          "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

IBL_xSagitta =  MakexResSagittaPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, False)
DrawPlots(IBL_xSagitta, outputDir+"/IBL_xSagitta."+oFext, "IBL", "Local x residual Sagitta [#mum]",
          "sector [#stave]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)
    
print " -- MakeResidualIBL -- completed "
