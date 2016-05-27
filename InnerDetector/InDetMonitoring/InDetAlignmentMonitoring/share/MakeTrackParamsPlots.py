# track parameters

TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0.png", "", "N Tracks",
          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_bscorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0BS.png", "", "N Tracks", "Impact Parameter [mm]", 0.19, 0.9, "#mum", canvasText, makeOutput)

#TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0_pvcorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
#DrawPlots(TrackD0, outputDir+"/"+"d0PV.png", "", "N Tracks",
#          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
#          canvasText,makeOutput)

#TrackZ0PV = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0_pvcorr","noFitWithStats",rootFiles,nFiles,normaliseHistos)
#DrawPlots(TrackZ0PV, outputDir+"/"+"z0PV.png", "", "N Tracks",
#          "z0 w.r.t PV [mm]", 0.19, 0.9, "#mum",
#          canvasText,makeOutput)


TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0, outputDir+"/"+"z0.png", "", "N Tracks",
          "z0 [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackPHI = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"phi","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPHI, outputDir+"/"+"phi.png", "", "N Tracks",
          "phi [radians]", 0.19, 0.9, "radians",
          canvasText,makeOutput)

TrackETA = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackETA, outputDir+"/"+"eta.png", "", "N Tracks",
          "#eta", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackETAAsym = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"eta_asym","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackETAAsym, outputDir+"/"+"eta_asym.png", "", "(pos-neg)/(pos+neg)",
          "#eta", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT.png", "", "N Tracks", "q p_{T} [GeV]", 0.19, 0.9, "", canvasText,makeOutput,0.60,0.86,False)

TrackPT = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_nolog","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT, outputDir+"/"+"pT_nolog.png", "", "N Tracks",
          "q p_{T} [GeV]", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False)

TrackPTqdiff = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_diff","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPTqdiff, outputDir+"/"+"pTAqdiff.png", "Positive - Negative", "N Tracks", "p_{T} [GeV]", 0.19, 0.9, "", canvasText, makeOutput, 0.60,0.85,False)

TrackPT_NQ = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_n","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT_NQ, outputDir+"/"+"pT_negativeQ.png", "", "N Tracks",
          "p_{T} negative charged tracks [GeV]", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False)

TrackPT_PQ = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"pT_p","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackPT_PQ, outputDir+"/"+"pT_positiveQ.png", "", "N Tracks",
          "p_{T} positive charged tracks [GeV]", 0.19, 0.9, "",
          canvasText,makeOutput,0.60,0.91,False)

TrackSUMMARY = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"summary","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackSUMMARY, outputDir+"/"+"summary.png", "N Hits Summary", "N Tracks",
          "", 0.19, 0.9, "",
          canvasText,makeOutput)

TrackCHI2DOF = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"chi2oDoF","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackCHI2DOF, outputDir+"/"+"chi2oDoF.png", "", "N Tracks",
          "Chi2/DOF", 0.69, 0.6, "",
          canvasText,makeOutput)

errD0 = False
if (userExtended and errD0):
    ERRD0VSP = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsP","noFit",rootFiles,nFiles,False)
    DrawPlots(ERRD0VSP,outputDir+"/"+"D0ErrorVsP.png","","#sigma d_{0} [mm]", "p [GeV]",0.2,0.9,"#mum", canvasText,makeOutput, 0.60, 0.80, False);

    ERRD0VSEta = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsEta","noFit",rootFiles,nFiles,False)
    DrawPlots(ERRD0VSEta,outputDir+"/"+"D0ErrorVsEta.png","","#sigma d_{0} [mm]", "#eta",0.2,0.9," ", canvasText,makeOutput, 0.60, 0.91, False);

    ERRD0VSPhi = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsPhi","noFit",rootFiles,nFiles,False)
    DrawPlots(ERRD0VSPhi,outputDir+"/"+"D0ErrorVsPhi.png","","#sigma d_{0} [mm]", "#phi_{0} [rad]",0.2,0.9," ", canvasText,makeOutput, 0.60, 0.91, False);

    ERRD0VSPhi = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"errD0VsPhiBarrel","noFit",rootFiles,nFiles,False)
    DrawPlots(ERRD0VSPhi,outputDir+"/"+"D0ErrorVsPhiBarrel.png","","#sigma d_{0} [mm]", "#phi_{0} [rad]",0.2,0.9," ", canvasText,makeOutput, 0.60, 0.91, False);

TrackNTRACKS = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"ntracks","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRACKS, outputDir+"/"+"ntracks.png", "Tracks/Event", "N Events",
          "N Tracks", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNTRTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Ntrthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNTRTHits, outputDir+"/"+"nTRTHits.png", "", "N Tracks",
          "N TRT Hits", 0.17, 0.87, "",
          canvasText,makeOutput)

TrackNSCTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHits, outputDir+"/"+"nSCTHits.png", "", "N Tracks",
          "N SCT Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsBarrel, outputDir+"/"+"nSCTHitsBarrel.png", "", "N Tracks",
          "N SCT Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEca, outputDir+"/"+"nSCTHitsEca.png", "", "N Tracks",
          "N SCT Hits Eca", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEcc, outputDir+"/"+"nSCTHitsEcc.png", "", "N Tracks",
          "N SCT Hits Ecc", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHits, outputDir+"/"+"nPIXHits.png", "", "N Tracks",
          "N PIX Hits", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsBarrel, outputDir+"/"+"nPIXHitsBarrel.png", "", "N Tracks",
          "N PIX Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEca, outputDir+"/"+"nPIXHitsEca.png", "", "N Tracks",
          "N PIX Hits ECA", 0.69, 0.6, "",
          canvasText,makeOutput)


TrackNPIXHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEcc, outputDir+"/"+"nPIXHitsEcc.png", "", "N Tracks",
          "N PIX Hits ECC", 0.69, 0.6, "",
          canvasText,makeOutput)

NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"TracksPerLumiBlock","noFit",rootFiles,nFiles,False)
DrawPlots(NTracks, outputDir+"/"+"NTracksPerLumiBlock.png", "", "N Tracks", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)

NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NPixPerLumiBlock","noFit",rootFiles,nFiles,False)
DrawPlots(NTracks, outputDir+"/"+"NPixPerLumiBlock.png", "", "# pixel hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)

NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NSCTPerLumiBlock","noFit",rootFiles,nFiles,False)
DrawPlots(NTracks, outputDir+"/"+"NSCTPerLumiBlock.png", "", "#SCT hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)

NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NTRTPerLumiBlock","noFit",rootFiles,nFiles,False)
DrawPlots(NTracks, outputDir+"/"+"NTRTPerLumiBlock.png", "", "#TRT hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)



