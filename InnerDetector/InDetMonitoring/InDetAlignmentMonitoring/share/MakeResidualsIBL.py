#
#
#
#
PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+userPrefix+"PIXIBL_X."+oFext, "Pixel Barrel IBL", "#hits on track",
          "Local x residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

DrawEvolutionPlot(PIXX0, outputDir+"/"+userPrefix+"Evol_PIXIBL_X."+oFext, "Pixel Barrel IBL", "Mean local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+userPrefix+"PIXIBL_Y."+oFext, "Pixel Barrel IBL", "#hits on track",
          "Local y residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

#

IBL_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(IBL_xRESvsETA, outputDir+"/"+userPrefix+"IBL_xRESvsETA."+oFext, "IBL", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60,0.88, False) # 0.41, 0.40

IBL_yRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(IBL_yRESvsETA, outputDir+"/"+userPrefix+"IBL_yRESvsETA."+oFext, "IBL", "Average local y residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)


PIX1_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b1_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX1_xRESvsETA, outputDir+"/"+userPrefix+"PIX1_xRESvsETA."+oFext, "Pixel layer 1", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

PIX2_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b2_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX2_xRESvsETA, outputDir+"/"+userPrefix+"PIX2_xRESvsETA."+oFext, "Pixel layer 2", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

PIX3_xRESvsETA =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b3_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True)
DrawPlots(PIX3_xRESvsETA, outputDir+"/"+userPrefix+"PIX3_xRESvsETA."+oFext, "Pixel layer 3", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

IBL_xRESvsETAupper =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 3, 6)
DrawPlots(IBL_xRESvsETAupper, outputDir+"/"+userPrefix+"IBL_xRESvsETAupper."+oFext, "Upper IBL", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

IBL_xRESvsETAlower =  MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 10, 13)
DrawPlots(IBL_xRESvsETAlower, outputDir+"/"+userPrefix+"IBL_xRESvsETAlower."+oFext, "Lower IBL", "Average local x residual [mm]",
          "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

PIXIBL_3D_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXIBL_3D_xRES, outputDir+"/"+userPrefix+"PIXIBL_3D_xRES."+oFext, "IBL 3D sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
          canvasText,makeOutput,0.64,0.89)
    
PIXIBL_Planar_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXIBL_Planar_xRES, outputDir+"/"+userPrefix+"PIXIBL_Planar_xRES."+oFext, "IBL Planar sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
          canvasText,makeOutput,0.64,0.89)
    
# -- local y
PIXIBL_3D_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXIBL_3D_yRES, outputDir+"/"+userPrefix+"PIXIBL_3D_yRES."+oFext, "IBL 3D sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
          canvasText,makeOutput,0.64,0.89)

PIXIBL_Planar_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXIBL_Planar_yRES, outputDir+"/"+userPrefix+"PIXIBL_Planar_yRES."+oFext, "IBL Planar sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
          canvasText,makeOutput,0.64,0.89)

for stave in range(14): 
    IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, stave+1, stave+1) 
    DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_xRESvsETA_stave"+str(stave+1)+"."+oFext, "IBL stave "+str(stave+1), "Average local x residual [mm]",
              "ring along stave [#eta-index]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)

    IBLStave = MakeModuleResPlotsFrom3D  (residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", 
                                         "noFitWithStats", rootFiles, nFiles, True, stave+1)
    DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_xRES_stave"+str(stave+1)+"."+oFext, "IBL stave "+str(stave), "Hits", "Local x residual [#mum]",
              0.18, 0.88, "#mum", canvasText, makeOutput, 0.65, 0.89, True)

    IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, stave+1, stave+1) 
    DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_yRESvsETA_stave"+str(stave+1)+"."+oFext, "IBL stave "+str(stave+1), "Average local y residual [mm]",
              "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

    IBLStave = MakeModuleResPlotsFrom3D  (residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_yresvsmodetaphi_3d", 
                                         "noFitWithStats", rootFiles, nFiles, True, stave+1)
    DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_yRES_stave"+str(stave+1)+"."+oFext, "IBL stave "+str(stave+1), "Hits", "Local y residual [#mum]",
              0.18, 0.88, "#mum", canvasText, makeOutput, 0.65, 0.89, True)

# stave bending plot for upper staves
IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 3, 6) 
DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_xRESvsETA_stave_upper."+oFext, "Upper IBL staves", "Average local x residual [mm]",
          "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

IBLStave = MakeProfPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 11, 14) 
DrawPlots(IBLStave, outputDir+"/"+userPrefix+"IBL_xRESvsETA_stave_lower."+oFext, "Lower IBL staves", "Average local x residual [mm]",
          "ring along stave [#eta]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.65,0.89, False)

IBL_xSagitta =  MakexResSagittaPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, False)
DrawPlots(IBL_xSagitta, outputDir+"/"+userPrefix+"IBL_xSagitta."+oFext, "IBL", "Local x residual Sagitta [#mum]",
          "sector [#stave]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.60, 0.88, False)


MAGH = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"mag_vs_LB","noFit",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(MAGH, outputDir+"/"+userPrefix+"MagnitudeVsLB."+oFext, "Distortion magnitude", "Magnitude [#mu m]", "LumiBlock", 0.15, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)

if (userExtended):
    IBLvsLB = MakePlots2D(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_resXvsetaLumiBlock",rootFiles,nFiles)
    DrawPlots2D(IBLvsLB,outputDir+"/"+userPrefix,"IBL_resXvsetaLumiBlock."+oFext,"LumiBlock","Local X residual",0.18,0.88,"[mm]",canvasText,True)
    
    IBLvsLB_planars = MakePlots2D(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_resXvsetaLumiBlock_planars",rootFiles,nFiles)
    DrawPlots2D(IBLvsLB,outputDir+"/"+userPrefix,"IBL_resXvsetaLumiBlock_planars."+oFext,"LumiBlock","Local X residual",0.18,0.88,"[mm]",canvasText,True)


if (userExtended):
    par = [-999,-999,-999,-999]
    par2 = [-999,-999,-999,-999]

    magVSLB  = TH1F("magvslb","magvslb",IBLvsLB_planars[0].GetXaxis().GetNbins(),IBLvsLB_planars[0].GetXaxis().GetXmin(),IBLvsLB_planars[0].GetXaxis().GetXmax())
    baseVSLB = TH1F("basevslb","basevslb",IBLvsLB_planars[0].GetXaxis().GetNbins(),IBLvsLB_planars[0].GetXaxis().GetXmin(),IBLvsLB_planars[0].GetXaxis().GetXmax())
    for ibin in range(1,IBLvsLB_planars[0].GetXaxis().GetNbins()+1):
        proj=IBLvsLB_planars[0].ProjectionY("Projection_IBL_LB"+str(ibin)+"-"+str(ibin),ibin,ibin)
        par = MakeStaveShapeFit(proj,True,True,True,outputDir+"/"+userPrefix+"Projection_IBL_LB"+str(ibin)+"-"+str(ibin)+"."+oFext)
        magVSLB.SetBinContent(ibin, par[0]*1000)
        magVSLB.SetBinError(ibin,par[1]*1000)
        baseVSLB.SetBinContent(ibin,par[2]*1000)
        baseVSLB.SetBinError(ibin,par[3]*1000)

    magVSLB.GetYaxis().SetRangeUser(-10,10)
    baseVSLB.GetYaxis().SetRangeUser(-10,10)

    par2 = MakeMagnitudeLinearFit(magVSLB,False,True,False,outputDir+"/"+userPrefix+"LinearFitToMagnitude"+"."+oFext)
    par2 = MakeMagnitudeLinearFit(magVSLB,False,True,True,outputDir+"/"+userPrefix+"ConstantFitToMagnitude"+"."+oFext)
    simpleDrawSingleHist(magVSLB,"magnitude [#mum]","Luminosity Block","",canvasText,markerStyles[0],markerColors[0],outputDir+"/"+userPrefix+"magvslb_planars."+oFext)
    simpleDrawSingleHist(baseVSLB,"baseline [#mum]","Luminosity Block","",canvasText,markerStyles[0],markerColors[0],outputDir+"/"+userPrefix+"basevslb_planars."+oFext)

    

    par = [-999,-999,-999,-999]
    magVSLB  = TH1F("magvslb","magvslb",IBLvsLB[0].GetXaxis().GetNbins(),IBLvsLB[0].GetXaxis().GetXmin(),IBLvsLB[0].GetXaxis().GetXmax())
    baseVSLB = TH1F("basevslb","basevslb",IBLvsLB[0].GetXaxis().GetNbins(),IBLvsLB[0].GetXaxis().GetXmin(),IBLvsLB[0].GetXaxis().GetXmax())
    for ibin in range(1,IBLvsLB[0].GetXaxis().GetNbins()+1):
        proj=IBLvsLB_planars[0].ProjectionY("Projection_IBL_LB"+str(ibin)+"-"+str(ibin),ibin,ibin)
        par = MakeStaveShapeFit(proj,True,True,True,outputDir+"/"+userPrefix+"Projection_IBL_LB"+str(ibin)+"-"+str(ibin)+"."+oFext)
        magVSLB.SetBinContent(ibin, par[0]*1000)
        magVSLB.SetBinError(ibin,par[1]*1000)
        baseVSLB.SetBinContent(ibin,par[2]*1000)
        baseVSLB.SetBinError(ibin,par[3]*1000)

    magVSLB.GetYaxis().SetRangeUser(-10,10)
    baseVSLB.GetYaxis().SetRangeUser(-10,10)

    par2 = MakeMagnitudeLinearFit(magVSLB,False,True,False,outputDir+"/"+userPrefix+"LinearFitToMagnitude"+"."+oFext)
    par2 = MakeMagnitudeLinearFit(magVSLB,False,True,True,outputDir+"/"+userPrefix+"ConstantFitToMagnitude"+"."+oFext)
    simpleDrawSingleHist(magVSLB,"magnitude [#mum]","Luminosity Block","",canvasText,markerStyles[0],markerColors[0],outputDir+"/"+userPrefix+"magvslb."+oFext)
    simpleDrawSingleHist(baseVSLB,"baseline [#mum]","Luminosity Block","",canvasText,markerStyles[0],markerColors[0],outputDir+"/"+userPrefix+"basevslb."+oFext)

print " -- MakeResidualIBL -- completed "



