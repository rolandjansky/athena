#
# Basic Pull distributions

PIXPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX, outputDir+"/"+"PIXPULLX.png", "Pixel Barrel", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY, outputDir+"/"+"PIXPULLY.png", "Pixel Barrel", "Number of hits on tracks",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


# layer by layer
PIXPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX0, outputDir+"/"+"PIXPULLX0.png", "Pixel Barrel IBL", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX1, outputDir+"/"+"PIXPULLX1.png", "Pixel Barrel L0", "Number of hits on tracks",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX2, outputDir+"/"+"PIXPULLX2.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLX3, outputDir+"/"+"PIXPULLX3.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY0, outputDir+"/"+"PIXPULLY0.png", "Pixel Barrel IBL", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY1, outputDir+"/"+"PIXPULLY1.png", "Pixel Barrel L1", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY2, outputDir+"/"+"PIXPULLY2.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLY3, outputDir+"/"+"PIXPULLY3.png", "Pixel Barrel L2", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


PIXPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECA, outputDir+"/"+"PIXPULLXECA.png", "Pixel Endcap A", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECAY, outputDir+"/"+"PIXPULLYECA.png", "Pixel Endcap A", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECC, outputDir+"/"+"PIXPULLXECC.png", "Pixel Endcap C", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

PIXPULLECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_pully","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXPULLECCY, outputDir+"/"+"PIXPULLYECC.png", "Pixel Endcap C", "Number of hits on tracks / 0.1",
          "y pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

# SCT
SCTPULLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX, outputDir+"/"+"SCTPULLX.png", "SCT Barrel", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX0, outputDir+"/"+"SCTPULLX0.png", "SCT Barrel L0", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX1, outputDir+"/"+"SCTPULLX1.png", "SCT Barrel L1", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX2, outputDir+"/"+"SCTPULLX2.png", "SCT Barrel L2", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLX3, outputDir+"/"+"SCTPULLX3.png", "SCT Barrel L3", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLECA = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECA, outputDir+"/"+"SCTPULLXECA.png", "SCT Endcap A", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

SCTPULLECC = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_pullx","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTPULLECC, outputDir+"/"+"SCTPULLXECC.png", "SCT Endcap C", "Number of hits on tracks / 0.1",
          "x pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


# TRT barrel
TRTPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_pullR","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTPULLR, outputDir+"/"+"TRTPULLR.png", "TRT Barrel", "Number of hits on tracks",
          "R pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

TRTPULLNOTUBER = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_b_pullR_notube","singleGaus",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTPULLNOTUBER, outputDir+"/"+"TRTPULLNOTUBER.png", "TRT Barrel", "Number of hits on tracks",
          "R pull (no tube)", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)


# TRT end caps
TRTECAPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_Endcap_A","singleGaus",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTECAPULLR, outputDir+"/"+"TRTECAPULLR.png", "TRT Endcap A", "Number of hits on tracks",
          "R pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

TRTECAPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube_Endcap_A","singleGaus",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTECAPULLR, outputDir+"/"+"TRTECAPULLRNOTUBE.png", "TRT Endcap A", "Number of hits on tracks",
          "R pull (no tube)", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)



TRTECCPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_Endcap_C","singleGaus",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTECCPULLR, outputDir+"/"+"TRTECCPULLR.png", "TRT Endcap C", "Number of hits on tracks",
          "R pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

TRTECCPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube_Endcap_C","singleGaus",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTECCPULLR, outputDir+"/"+"TRTECCPULLRNOTUBE.png", "TRT Endcap C", "Number of hits on tracks",
          "R pull (no tube)", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

TRTECSPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR","singleGaus",rootFiles,nFiles,normaliseHistos)#unitArea)
DrawPlots(TRTECAPULLR, outputDir+"/"+"TRTECSPULLR.png", "TRT Endcaps", "Number of hits on tracks",
          "R pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)

TRTECSPULLR = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"trt_ec_pullR_notube","singleGaus",rootFiles,nFiles,normaliseHistos)#unitArea)
DrawPlots(TRTECAPULLR, outputDir+"/"+"TRTECSPULLRNOTUBE.png", "TRT Endcaps", "Number of hits on tracks",
          "R pull", 0.2, 0.9, "",canvasText,makeOutput,0.63,0.86)



PullRes =  MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"si_barrel_pullX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(PullRes, outputDir+"/"+"SiBarrelPullMean."+oFext, "PIX & SCT Barrel", "local x pull", "", 
          0.18, 0.88, " ", canvasText,makeOutput,0.59, 0.87,False) 

PullRes =  MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"si_barrel_pullX_width","noFit",rootFiles,nFiles,False)
DrawPlots(PullRes, outputDir+"/"+"SiBarrelPullWidth."+oFext, "PIX & SCT Barrel", "local x pull", "", 
          0.18, 0.88, " ", canvasText,makeOutput,0.59, 0.87,False) 


