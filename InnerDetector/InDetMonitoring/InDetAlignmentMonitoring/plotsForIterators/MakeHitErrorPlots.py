#
# MakeHitErrorPlots.py
#

# Pixel barrel
PIXBAR_HitError_x = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_x_pix_b","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_x, outputDir+"/"+"PIXBAR_HitError_x."+oFext, "Pixel barrel", "Pixel hits errors (local x)",
          "Hit error (local x) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

PIXBAR_HitError_x_WR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_x_pix_b_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_x_WR, outputDir+"/"+"PIXBAR_HitError_x_WideRange."+oFext, "Pixel barrel", "Pixel hits errors (local x)",
          "Hit error (local x) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

PIXBAR_HitError_y = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_y_pix_b","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_y, outputDir+"/"+"PIXBAR_HitError_y."+oFext, "Pixel barrel", "Pixel hits errors (local y)",
          "Hit error (local y) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

PIXBAR_HitError_y_WR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_y_pix_b_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_y_WR, outputDir+"/"+"PIXBAR_HitError_y_WideRange."+oFext, "Pixel barrel", "Pixel hits errors (local y)",
          "Hit error (local y) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

# IBL
IBL_HitError_x = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_x_ibl_b","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(IBL_HitError_x, outputDir+"/"+"IBL_HitError_x."+oFext, "IBL", "IBL hits errors (local x)",
          "Hit error (local x) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

IBL_HitError_x_WR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_x_ibl_b_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(IBL_HitError_x_WR, outputDir+"/"+"IBL_HitError_x_WideRange."+oFext, "IBL", "IBL hits errors (local x)",
          "Hit error (local x) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

IBL_HitError_y = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_y_ibl_b","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(IBL_HitError_y, outputDir+"/"+"IBL_HitError_y."+oFext, "IBL", "IBL hits errors (local y)",
          "Hit error (local y) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

IBL_HitError_y_WR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_y_ibl_b_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(IBL_HitError_y_WR, outputDir+"/"+"IBL_HitError_y_WideRange."+oFext, "IBL", "IBL hits errors (local x)",
          "Hit error (local y) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

# PIX end caps
PIXEC_HitError_x = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_x_pix_ec","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_x, outputDir+"/"+"PIXEC_HitError_x."+oFext, "Pixel end caps", "Pixel hits errors (local x)",
          "Hit error (local x) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

PIXEC_HitError_y = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_y_pix_ec","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXBAR_HitError_y, outputDir+"/"+"PIXEC_HitError_y."+oFext, "Pixel end caps", "Pixel hits errors (local y)",
          "Hit error (local y) [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

# SCT
SCTBAR_HitErrorWR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_sct_b_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTBAR_HitErrorWR, outputDir+"/"+"SCTBAR_HitErrorWR."+oFext, "SCT barrel", "SCT Hits errors",
          "Hit error [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

SCTBAR_HitError = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_sct_b","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTBAR_HitError, outputDir+"/"+"SCTBAR_HitError."+oFext, "SCT barrel", "SCT Hits errors",
          "Hit error [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)

SCTEC_HitError = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"m_hiterror_sct_ec_WideRange","noFit",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTEC_HitError, outputDir+"/"+"SCTEC_HitError."+oFext, "SCT end caps", "SCT Hits errors",
          "Hit error [mm]", 0.65, 0.7, "#mum",canvasText,makeOutput,0.60,0.91)
