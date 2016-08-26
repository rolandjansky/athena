#Script to plot Overlap Plots. (PF)

#Pixel
#round is not the best function to use for this scope. For the moment I use this. Think about pass to Decimal.

mmToum=1000.

if (userExtended):
    PIX_OXRX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRX0, outputDir+"/"+userPrefix+"PIXIBL_OXRX."+oFext, "IBL", "Hits on tracks / "+str(round(PIX_OXRX0[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRX1, outputDir+"/"+userPrefix+"PIX0_OXRX."+oFext, "B Layer", "Hits on tracks / "+str(round(PIX_OXRX1[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRX2, outputDir+"/"+userPrefix+"PIX1_OXRX."+oFext, "PIX Layer 1", "Hits on tracks / "+str(round(PIX_OXRX2[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRX3, outputDir+"/"+userPrefix+"PIX3_OXRX."+oFext, "PIX Layer 3", "Hits on tracks / "+str(round(PIX_OXRX3[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    PIX_OXRY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_xoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRY0, outputDir+"/"+userPrefix+"PIXIBL_OXRY."+oFext, "IBL", "Hits on tracks / "+str(round(PIX_OXRY0[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_xoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRY1, outputDir+"/"+userPrefix+"PIX0_OXRY."+oFext, "B Layer", "Hits on tracks / "+str(round(PIX_OXRY1[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_xoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRY2, outputDir+"/"+userPrefix+"PIX1_OXRY."+oFext, "PIX Layer 1", "Hits on tracks / "+str(round(PIX_OXRY2[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OXRY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_xoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OXRY3, outputDir+"/"+userPrefix+"PIX3_OXRY."+oFext, "PIX Layer 3", "Hits on tracks / "+str(round(PIX_OXRY3[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "X Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    

    
#This histogram should be empty but there are entries
    PIX_OYRX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRX0, outputDir+"/"+userPrefix+"PIXIBL_OYRX."+oFext, "IBL", "Hits on tracks / "+str(round(PIX_OYRX0[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    
    PIX_OYRX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRX1, outputDir+"/"+userPrefix+"PIX0_OYRX."+oFext, "B Layer", "Hits on tracks / "+str(round(PIX_OYRX1[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OYRX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRX2, outputDir+"/"+userPrefix+"PIX1_OYRX."+oFext, "PIX Layer 1", "Hits on tracks / "+str(round(PIX_OYRX2[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OYRX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRX3, outputDir+"/"+userPrefix+"PIX3_OYRX."+oFext, "PIX Layer 3", "Hits on tracks / "+str(round(PIX_OYRX3[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    

#This histogram shouldn't be filled
#PIX_OYRY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_yoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
#thiscan = DrawPlots(PIX_OYRY0, outputDir+"/"+userPrefix+"PIXIBL_OYRY."+oFext, "IBL", "Hits on tracks / "+str(round(PIX_OYRY0[1].GetBinWidth(1)*mmToum,1))+" #mum",
#                    "Y Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OYRY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_yoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRY1, outputDir+"/"+userPrefix+"PIX0_OYRY."+oFext, "B Layer", "Hits on tracks / "+str(round(PIX_OYRY1[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OYRY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_yoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRY2, outputDir+"/"+userPrefix+"PIX1_OYRY."+oFext, "PIX Layer 1", "Hits on tracks / "+str(round(PIX_OYRY2[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

    PIX_OYRY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_yoverlapresidualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIX_OYRY3, outputDir+"/"+userPrefix+"PIX3_OYRY."+oFext, "PIX Layer 3", "Hits on tracks / "+str(round(PIX_OYRY3[1].GetBinWidth(1)*mmToum,1))+" #mum",
                        "Y Overlap Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

#
# Overlap Residual mean as a function of eta / phi modified modules
# 

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oxresxvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRXvsModEta."+oFext, "Pixel Barrel", "X Overlap local x residual mean [#mum]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oyresxvsmodeta","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRXvsModEta."+oFext, "Pixel Barrel", "Y Overlap local x residual mean [mm]",
#          "Module Eta", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False


#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oxresyvsmodeta","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRYvsModEta."+oFext, "Pixel Barrel", "X Overlap local y residual mean [mm]",
#          "Module Eta", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oyresyvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRYvsModEta."+oFext, "Pixel Barrel", "Y Overlap local y residual mean [#mum]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRXvsModPhi."+oFext, "Pixel Barrel", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oyresxvsmodphi","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRXvsModPhi."+oFext, "Pixel Barrel", "Y Overlap local x residual mean [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False

#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oxresyvsmodphi","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRYvsModPhi."+oFext, "Pixel Barrel", "X Overlap local y residual mean [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_Oyresyvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRYvsModPhi."+oFext, "Pixel Barrel", "Y Overlap local y residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_eca_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRXvsModPhiECA."+oFext, "Pixel Endcap A", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

#This plot is empty
#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_eca_Oyresyvsmodphi","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRYvsModPhiECA."+oFext, "Pixel Endcap A", "Y Overlap local y residual mean [#mum]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False



PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_ecc_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOXRXvsModPhiECC."+oFext, "Pixel Endcap C", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


#This plot is empty.
#PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_ecc_Oyresyvsmodphi","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelOYRYvsModPhiECC."+oFext, "Pixel Endcap C", "Y Overlap local y residual mean [#mum]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False) #False




#Barrel Endcap Overlap residuals and mean for Pixel


#General SCT Overlap Residuals
if (userExtended):
    SCT_OXRX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX0, outputDir+"/"+userPrefix+"SCT0_OXRX."+oFext, "SCT Layer 0", "Hits on tracks / "+str(round(SCT_OXRX0[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OXRX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX1, outputDir+"/"+userPrefix+"SCT1_OXRX."+oFext, "SCT Layer 1", "Hits on tracks / "+str(round(SCT_OXRX1[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OXRX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX2, outputDir+"/"+userPrefix+"SCT2_OXRX."+oFext, "SCT Layer 2", "Hits on tracks / "+str(round(SCT_OXRX2[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OXRX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_xoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX3, outputDir+"/"+userPrefix+"SCT3_OXRX."+oFext, "SCT Layer 3", "Hits on tracks / "+str(round(SCT_OXRX3[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)



    SCT_OYRX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX0, outputDir+"/"+userPrefix+"SCT0_OYRX."+oFext, "SCT Layer 0", "Hits on tracks / "+str(round(SCT_OYRX0[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OYRX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX1, outputDir+"/"+userPrefix+"SCT1_OYRX."+oFext, "SCT Layer 1", "Hits on tracks / "+str(round(SCT_OYRX1[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OYRX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX2, outputDir+"/"+userPrefix+"SCT2_OYRX."+oFext, "SCT Layer 2", "Hits on tracks / "+str(round(SCT_OYRX2[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


    SCT_OYRX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_yoverlapresidualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    DrawPlots(SCT_OXRX3, outputDir+"/"+userPrefix+"SCT3_OYRX."+oFext, "SCT Layer 3", "Hits on tracks / "+str(round(SCT_OYRX3[1].GetBinWidth(1)*mmToum,1))+" #mum",
              "X Overlap Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)



# SCT Overlap residuals mean vs eta and phi


SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_Oxresxvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOXRXvsModEta."+oFext, "SCT Barrel", "X Overlap local x residual mean [#mum]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_Oyresxvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOYRXvsModEta."+oFext, "SCT Barrel", "Y Overlap local x residual mean [#mum]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False



SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOXRXvsModPhi."+oFext, "SCT Barrel", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_Oyresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOYRXvsModPhi."+oFext, "SCT Barrel", "Y Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_eca_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOXRXvsModPhiECA."+oFext, "SCT Endcap A", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_ecc_Oxresxvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTOXRXvsModPhiECC."+oFext, "SCT Endcap C", "X Overlap local x residual mean [#mum]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False







