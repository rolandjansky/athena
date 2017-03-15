#--
#
# Basic Barrel Residual distributions
#
print " <MakeResidualPlots.MakeRun2PaperPlots> userCollectOutput=",userCollectOutput
print "                                        userWebMonitoring=",userWebMonitoring
#

plotResiduals = True
plotTrackParams = True
plotHits = True
plotResMaps = False

psname = "plots.pdf"

def initPsFile():
    NullCanvas = TCanvas()
    NullCanvas.Print(psname+"[")
    return NullCanvas
	
def closePsFile(NullCanvas):
    NullCanvas.Print(psname+"]")

Canvases = []
if (userCollectOutput):    
    NullCanvas = initPsFile()
    

# RESIDUALS
if (plotResiduals):
    IBLX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(IBLX, outputDir+"/"+userPrefix+"Residual_IBL_X."+oFext, "IBL", "Hits on tracks",
                        "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    IBLY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(IBLY, outputDir+"/"+userPrefix+"Residual_IBL_Y."+oFext, "IBL", "Hits on tracks",
                        "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    
    PIXIBL_Planar_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_xRES, outputDir+"/"+userPrefix+"Residual_IBLPlanar_x."+oFext, "IBL Planar sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)
    
    PIXIBL_Planar_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_yRES, outputDir+"/"+userPrefix+"Residual_IBLPlanar_y."+oFext, "IBL Planar sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)

    PIXIBL_3D_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_xRES, outputDir+"/"+userPrefix+"Residual_IBL3D_x."+oFext, "IBL 3D sensors", "Hits on track", "Local x residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)

    PIXIBL_3D_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_yRES, outputDir+"/"+userPrefix+"Residual_IBL3D_y."+oFext, "IBL 3D sensors", "Hits on track", "Local y residual [mm]", 0.16, 0.90, "#mum",
              canvasText,makeOutput,0.64,0.89)



        
    PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"PIXONLY_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIXX, outputDir+"/"+userPrefix+"Residual_PIXBarrel_X."+oFext, "Pixels", "Hits on tracks",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"PIXONLY_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
    thiscan = DrawPlots(PIXY, outputDir+"/"+userPrefix+"Residual_PIXBarrel_Y."+oFext, "Pixels", "Hits on tracks",
                    "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(PIXX, outputDir+"/"+userPrefix+"Residual_IBLPIXBarrel_X."+oFext, "Pixel barrel + IBL", "Hits on tracks",
                        "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(PIXY, outputDir+"/"+userPrefix+"Residual_IBLPIXBarrel_Y."+oFext, "Pixel barrel + IBL", "Hits on tracks",
                        "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(PIXX, outputDir+"/"+userPrefix+"Residual_PIXEndCaps_X."+oFext, "Pixel End caps", "Hits on tracks",
                        "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(PIXY, outputDir+"/"+userPrefix+"Residual_PIXEndCaps_Y."+oFext, "Pixel End caps", "Hits on tracks",
                        "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(SCTX, outputDir+"/"+userPrefix+"Residual_SCTBarrel_X."+oFext, "SCT barrel", "Hits on tracks",
                        "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(SCTX, outputDir+"/"+userPrefix+"Residual_SCTEndcaps_X."+oFext, "SCT End caps", "Hits on tracks",
                        "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(TRTR, outputDir+"/"+userPrefix+"Residual_TRTBarrel_R."+oFext, "TRT barrel", "Hits on tracks",
                        "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

    TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos, unitArea)
    thiscan = DrawPlots(TRTR, outputDir+"/"+userPrefix+"Residual_TRTEndcaps_R."+oFext, "TRT End caps", "Hits on tracks",
                        "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)
    if (userCollectOutput): Canvases.append(thiscan)

        
        
# track params
if (plotTrackParams):
    TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_bscorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackD0, outputDir+"/"+"Track_d0.png", "", "N Tracks", "Impact Parameter [mm]", 0.19, 0.9, "#mum", canvasText, makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackZ0, outputDir+"/"+"Track_z0.png", "", "N Tracks", "z0 [mm]", 0.19, 0.9, "#mum", canvasText,makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"phi","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPHI, outputDir+"/"+"Track_phi.png", "", "N Tracks", "phi [radians]", 0.19, 0.9, "radians", canvasText,makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackETA, outputDir+"/"+"Track_eta.png", "", "N Tracks", "#eta", 0.19, 0.9, "", canvasText,makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)
        
    TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPT, outputDir+"/"+"Track_pT_logScale.png", "", "N Tracks", "q p_{T} [GeV]", 0.19, 0.9, "", canvasText,makeOutput,0.60,0.86,False)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_nolog","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPT, outputDir+"/"+"Track_pT_linearScale.png", "", "N Tracks", "q p_{T} [GeV]", 0.19, 0.9, "", canvasText,makeOutput,0.60,0.91,False)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_n","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPT, outputDir+"/"+"Track_pTNeg.png", "", "N Tracks", "p_{T}(#mu^{-}) [GeV]", 0.19, 0.9, "", canvasText,makeOutput,0.60,0.91,False)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_p","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPT, outputDir+"/"+"Track_pTPos.png", "", "N Tracks", "p_{T}(#mu^{+}) [GeV]", 0.19, 0.9, "", canvasText,makeOutput,0.60,0.91,False)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pTabs","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackPT, outputDir+"/"+"Track_pTabs.png", "", "N Tracks", "p_{T} [GeV]", 0.19, 0.9, "", canvasText,makeOutput, 0.60, 0.91, False)
    if (userCollectOutput): Canvases.append(thiscan)

    TrackCHI2DOF = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"chi2oDoF","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackCHI2DOF, outputDir+"/"+"Track_chi2oDoF.png", "", "N Tracks", "Chi2/DOF", 0.69, 0.6, "", canvasText,makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)


# track hits
if (plotHits):
    TrackNTRACKS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"ntracks","noFit",rootFiles,nFiles,normaliseHistos)
    DrawPlots(TrackNTRACKS, outputDir+"/"+"Tracks_NTracks.png", "Tracks/Event", "N Events", "N Tracks", 0.69, 0.6, "", canvasText,makeOutput)

    TrackNPIXHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
    DrawPlots(TrackNPIXHits, outputDir+"/"+"HITS_nPIXHits.png", "", "N Tracks", "N PIX Hits", 0.69, 0.6, "",canvasText,makeOutput)

    TrackNSCTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
    DrawPlots(TrackNSCTHits, outputDir+"/"+"HITS_nSCTHits.png", "", "N Tracks", "N SCT Hits", 0.69, 0.6, "",canvasText,makeOutput)

    TrackNTRTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Ntrthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
    DrawPlots(TrackNTRTHits, outputDir+"/"+"HITS_nTRTHits.png", "", "N Tracks", "N TRT Hits", 0.17, 0.87, "",canvasText,makeOutput)

    TrackNHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
    thiscan = DrawPlots(TrackNHits, outputDir+"/"+"HITS_nHits.png", "", "N Tracks", "N ID Hits", 0.69, 0.6, "",canvasText,makeOutput)


# residual maps
if (plotResMaps):
    MinEntriesPerModule = 20
    RangeOfPixelsMaps = 10.
    RangeOfSCTMaps = 10.

    for i in range(nFiles):
    
        print " -- MakeResidualMapsPlots -- file ", i
        PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, RangeOfPixelsMaps)
        outputFileName = outputDir+"/"+userPrefix+"PIXBAR_xResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXBAR_xResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput)

        #########################
        # detailed residual maps
        PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, RangeOfPixelsMaps, "detailed")
        outputFileName = outputDir+"/"+userPrefix+"PIXBAR_xResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXBAR_xResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput)    

        # SCT Barrel: unbiased residual maps 
        SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR",0, True, RangeOfSCTMaps)
        outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s0ResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                    0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")
    
        SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 0, True, RangeOfSCTMaps, "detailed")
        outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s0ResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                    0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")    

####################    
if (userCollectOutput):
    print " <MakeResidualPlots> PsFile = ",psname
    for canvas in Canvases:
        print "  <MakeResidualPlots> storing canvas: ", canvas 
        PsFile = canvas.Print(psname)
    closePsFile(NullCanvas)
        
print " -- MakeRun2PaperPlots -- completed "
