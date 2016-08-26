#--
#
# Basic Barrel Residual distributions
#
print " <MakeResidualPlots.MakeResidualPlots> userCollectOutput=",userCollectOutput
print "                                       userWebMonitoring=",userWebMonitoring
#
psname = "/Users/martis/projectes/atlas/alineament/webmonitoringtest/InDetAlignmentWebMonitor/trunk/WebPage/detailed_plots/2016/ResidualDistributions.pdf"
if (userWebMonitoring):
    psname = "/var/vhost/atlas-alignment/secure/detailed_plots/2016/ResidualDistributions.pdf"

def initPsFile():
    NullCanvas = TCanvas()
    NullCanvas.Print(psname+"[")
    return NullCanvas
	
def closePsFile(NullCanvas):
    NullCanvas.Print(psname+"]")

Canvases = []
if (userCollectOutput):    
    NullCanvas = initPsFile()
    

# Pixel
PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
thiscan = DrawPlots(PIXX0, outputDir+"/"+userPrefix+"PIXIBL_X."+oFext, "IBL", "Hits on tracks / 12 #mum",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)

thiscan = DrawEvolutionPlot(PIXX0, outputDir+"/"+userPrefix+"Evol_PIXIBL_X."+oFext, "Pixel Barrel IBL", "Mean local x residual [mm]", 0.15, 0.89, "#mum",canvasText,makeOutput,0.60, 0.88)    
if (userCollectOutput): Canvases.append(thiscan)

PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
thiscan = DrawPlots(PIXX, outputDir+"/"+userPrefix+"PIXX."+oFext, "Pixel barrel + IBL", "Hits on tracks / 12 #mum",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)
    
PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
thiscan = DrawPlots(PIXY, outputDir+"/"+userPrefix+"PIXY."+oFext, "Pixel barrel + IBL", "Hits on tracks / 16 #mum",
                    "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)

# SCT
SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
thiscan = DrawPlots(SCTX, outputDir+"/"+userPrefix+"SCTX."+oFext, "SCT barrel", "Hits on tracks / 8 #mum",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)
print " <MakeResidualPlots> canvases: ",len(Canvases), " --> ", Canvases 

#SCTY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
#DrawPlots(SCTY, outputDir+"/"+userPrefix+"SCTY."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
#          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

# TRT
TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
thiscan = DrawPlots(TRTR, outputDir+"/"+userPrefix+"TRTR."+oFext, "TRT barrel", "Hits on tracks / 22 #mum",
                    "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)

#TRT no Tube
TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR_notube","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
thiscan = DrawPlots(TRTR, outputDir+"/"+userPrefix+"TRTR_notube."+oFext, "TRT barrel", "Hits on tracks / 22 #mum",
                    "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)
    
## Individual Barrel Layer Residual Distributions
PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
thiscan = DrawPlots(PIXX1, outputDir+"/"+userPrefix+"PIXX1."+oFext, "Pixel Barrel L0", "Hits on tracks / 12 #mum",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)
    
PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
thiscan = DrawPlots(PIXX2, outputDir+"/"+userPrefix+"PIXX2."+oFext, "Pixel Barrel L1", "Hits on tracks / 12 #mum",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
if (userCollectOutput): Canvases.append(thiscan)
    
PIXX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX3, outputDir+"/"+userPrefix+"PIXX3."+oFext, "Pixel Barrel L2", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# pixel barrel local y
PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+userPrefix+"PIXIBL_Y."+oFext, "IBL", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY1, outputDir+"/"+userPrefix+"PIXY1."+oFext, "Pixel Barrel L0", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY2, outputDir+"/"+userPrefix+"PIXY2."+oFext, "Pixel Barrel L1", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY3, outputDir+"/"+userPrefix+"PIXY3."+oFext, "Pixel Barrel L2", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# Pixel L1 L2 L3                                                                                                                                                                              
OLDPIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"oldpix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)                                    
DrawPlots(OLDPIXX, outputDir+"/"+userPrefix+"OLDPIXX."+oFext, "Pixel Barrel", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

OLDPIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"oldpix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)                                    
DrawPlots(OLDPIXY, outputDir+"/"+userPrefix+"OLDPIXY."+oFext, "Pixel Barrel", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# sct barrel
SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+userPrefix+"SCTX0."+oFext, "SCT Barrel L0", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+userPrefix+"SCTX1."+oFext, "SCT Barrel L1", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+userPrefix+"SCTX2."+oFext, "SCT Barrel L2", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+userPrefix+"SCTX3."+oFext, "SCT Barrel L3", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)









## Basic Endcap Residual distributions

# draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+userPrefix+"PIXECX."+oFext, "Pixel end-caps", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+userPrefix+"PIXECY."+oFext, "Pixel end-caps", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+userPrefix+"PIXECAX."+oFext, "PIX Endcap A", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+userPrefix+"PIXECCX."+oFext, "PIX Endcap C", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAY, outputDir+"/"+userPrefix+"PIXECAY."+oFext, "PIX Endcap A", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCY, outputDir+"/"+userPrefix+"PIXECCY."+oFext, "PIX Endcap C", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)


# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+userPrefix+"SCTECX."+oFext, "SCT Endcap", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+userPrefix+"SCTECAX."+oFext, "SCT Endcap A", "Hits on tracks / 8 #mum",
          "x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+userPrefix+"SCTECCX."+oFext, "SCT Endcap C", "Hits on tracks / 8 #mum",
          "x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+userPrefix+"TRTECA."+oFext, "TRT Endcap A", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_notubeEndcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+userPrefix+"TRTECA_notube."+oFext, "TRT Endcap A", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+userPrefix+"TRTECC."+oFext, "TRT Endcap C", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_notubeEndcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+userPrefix+"TRTECC_notube."+oFext, "TRT Endcap C", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+userPrefix+"TRTEC."+oFext, "TRT end-caps", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)


TRTBprof = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBprof, outputDir+"/"+userPrefix+"TRTBprof."+oFext, "TRT Barrel", "residual [#mum]", "wire number", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB0R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0R, outputDir+"/"+userPrefix+"TRTB0R."+oFext, "TRT Barrel layer 0", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB1R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1R, outputDir+"/"+userPrefix+"TRTB1R."+oFext, "TRT Barrel layer 1", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB2R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2R, outputDir+"/"+userPrefix+"TRTB2R."+oFext, "TRT Barrel layer 2", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)


#Modified eta/phi plots


#PIXMODETA =  MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_"

#
# Residual mean and width distributions as a function of layer, ring, stave in barrel 
#
PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelXvsModEta."+oFext, "Pixel Barrel", "local x residual mean [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_yresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelYvsModEta."+oFext, "Pixel Barrel", "local y residual mean [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelXvsModPhi."+oFext, "Pixel Barrel", "local x residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

PixelRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"pix_b_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelRes, outputDir+"/"+userPrefix+"PixelYvsModPhi."+oFext, "Pixel Barrel", "local y residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+userPrefix+"PixelECAXvsModPhi."+oFext, "Pixel Endcap A", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)
PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+userPrefix+"PixelECCXvsModPhi."+oFext, "Pixel Endcap C", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+userPrefix+"PixelECAYvsModPhi."+oFext, "Pixel Endcap A", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)
PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_yresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+userPrefix+"PixelECCYvsModPhi."+oFext, "Pixel Endcap C", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTXvsModEta."+oFext, "SCT Barrel", "local x residual mean [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False

SCTRes = MakePlots(residualsDir, legendTitles, markerColors,markerStyles,"sct_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTRes, outputDir+"/"+userPrefix+"SCTXvsModPhi."+oFext, "SCT Barrel", "local x residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False


SCTXPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_eca, outputDir+"/"+userPrefix+"SCTECAXvsModPhi_eca."+oFext, "SCT Endcap A", "local x residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

SCTXPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_ecc, outputDir+"/"+userPrefix+"SCTECCXvsModPhi_ecc."+oFext, "SCT Endcap C", "local x residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

          

PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+userPrefix+"PixelXvsModEtaWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_yresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+userPrefix+"PixelYvsModEtaWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)


PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelXvsModPhiWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelYvsModPhiWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)


#PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelECAXvsModPhiWidth."+oFext, "Pixel Endcap A", "residual width [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)

#PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelECAYvsModPhiWidth."+oFext, "Pixel Endcap A", "residual width [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)

#PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelECCXvsModPhiWidth."+oFext, "Pixel Endcap C", "residual width [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)

#PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_yresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(PixelXPhiW, outputDir+"/"+userPrefix+"PixelECCYvsModPhiWidth."+oFext, "Pixel Endcap C", "residual width [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)


#SCTXPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(SCTXPhiW_eca, outputDir+"/"+userPrefix+"SCTECAXvsModPhiWidth_eca."+oFext, "SCT Endcap A", "local x residual width [mm]","Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)


#SCTXPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
#DrawPlots(SCTXPhiW_ecc, outputDir+"/"+userPrefix+"SCTECCXvsModPhiWidth_ecc."+oFext, "SCT Endcap C", "residual width [mm]",
#          "Module Phi", 0.18, 0.88, "#mum",
#          canvasText,makeOutput,0.59, 0.87,False)

SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+userPrefix+"SiBarrelResXMean."+oFext, "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+userPrefix+"SiBarrelResXWidth."+oFext, "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)



#
# -- Extended plots
#
if (userExtended or True):
    # IBL split per 3D and planar sensors
    # -- local x
    PIXIBL_3DC_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DCSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DC_xRES, outputDir+"/"+userPrefix+"PIXIBL_3DC_xRES."+oFext, "IBL 3D sensors. C side", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)

    PIXIBL_3DA_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DASensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DA_xRES, outputDir+"/"+userPrefix+"PIXIBL_3DA_xRES."+oFext, "IBL 3D sensors. A side", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)

    PIXIBL_3D_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_xRES, outputDir+"/"+userPrefix+"PIXIBL_3D_xRES."+oFext, "IBL 3D sensors", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    PIXIBL_Planar_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_xRES, outputDir+"/"+userPrefix+"PIXIBL_Planar_xRES."+oFext, "IBL Planar sensors", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    # -- local y
    PIXIBL_3D_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_yRES, outputDir+"/"+userPrefix+"PIXIBL_3D_yRES."+oFext, "IBL 3D sensors", "Hits on track", "Local y residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    PIXIBL_Planar_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_yRES, outputDir+"/"+userPrefix+"PIXIBL_Planar_yRES."+oFext, "IBL Planar sensors", "Hits on track", "Local y residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    





if (userExtended and userClusters):
    PIXX0vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsINCANG,outputDir+"/"+userPrefix+"PIXIBL_XvsINCANG."+oFext,"IBL","Local x residual [mm]","Incident angle [radians]",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX1vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsINCANG,outputDir+"/"+userPrefix+"PIXX1vsINCANG."+oFext,"PIXEL Layer 0","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX2vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsINCANG,outputDir+"/"+userPrefix+"PIXX2vsINCANG."+oFext,"PIXEL Layer 1","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX3vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsINCANG,outputDir+"/"+userPrefix+"PIXX3vsINCANG."+oFext,"PIXEL Layer 2","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsCLUSIZ,outputDir+"/"+userPrefix+"PIXX0vsCLUSIZE."+oFext,"IBL","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    PIXX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsCLUSIZ,outputDir+"/"+userPrefix+"PIXX1vsCLUSIZE."+oFext,"PIXEL Layer 0","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsCLUSIZ,outputDir+"/"+userPrefix+"PIXX2vsCLUSIZE."+oFext,"PIXEL Layer 1","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsCLUSIZ,outputDir+"/"+userPrefix+"PIXX3vsCLUSIZE."+oFext,"PIXEL Layer 2","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

	# SCT 
    SCTX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX0vsCLUSIZ,outputDir+"/"+userPrefix+"SCTX0vsCLUSIZE."+oFext,"SCT Barrel layer 0","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX1vsCLUSIZ,outputDir+"/"+userPrefix+"SCTX1vsCLUSIZE."+oFext,"SCT Barrel layer 1","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX2vsCLUSIZ,outputDir+"/"+userPrefix+"SCTX2vsCLUSIZE."+oFext,"SCT Barrel layer 2","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX3vsCLUSIZ,outputDir+"/"+userPrefix+"SCTX3vsCLUSIZE."+oFext,"SCT Barrel layer 3","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    # SCT vs incident angle across strips
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX0s0vsIncAng."+oFext,"SCT Barrel layer 0 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX1s0vsIncAng."+oFext,"SCT Barrel layer 1 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX2s0vsIncAng."+oFext,"SCT Barrel layer 2 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX3s0vsIncAng."+oFext,"SCT Barrel layer 3 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX0s1vsIncAng."+oFext,"SCT Barrel layer 0 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX1s1vsIncAng."+oFext,"SCT Barrel layer 1 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX2s1vsIncAng."+oFext,"SCT Barrel layer 2 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+userPrefix+"SCTX3s1vsIncAng."+oFext,"SCT Barrel layer 3 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    # SCT barrel layer 0 Upper sectors (useful for cosmics)
    SCTUpper = MakePlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "sct_b0_xresvsmodetaphi_3d", "noFitWithStats", rootFiles, nFiles, False, 7, 10) 
    DrawPlots(SCTUpper, outputDir+"/"+userPrefix+"SCT_upper."+oFext, "SCT Layer 0 upper", "Average local x residual [mm]",
              "local x residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.65,0.89, False)
    
    SCTSides = MakePlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "sct_b0_xresvsmodetaphi_3d", "noFitWithStats", rootFiles, nFiles, False, 0, 4) 
    DrawPlots(SCTSides, outputDir+"/"+userPrefix+"SCT_sides."+oFext, "SCT Layer 0 sides", "Average local x residual [mm]",
              "local x residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.65,0.89, False)

if (userCollectOutput):
    print " <MakeResidualPlots> PsFile = ",psname
    for canvas in Canvases:
        print "  <MakeResidualPlots> storing canvas: ", canvas 
        PsFile = canvas.Print(psname)
    closePsFile(NullCanvas)

print " -- MakeResidualPlots -- completed "
