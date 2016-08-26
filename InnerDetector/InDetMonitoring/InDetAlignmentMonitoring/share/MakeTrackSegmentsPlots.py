# 
# This script steers the option to produce the track-segment matching and properties plots
#
xLabels = 0.18
yLabels = 0.88
#
DELTAD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAD0,outputDir+"/"+"DELTAD0."+oFext,"#Delta(Up-Down) Tracks","Number of tracks",
          "#Delta d_{0} [mm]",xLabels,yLabels,"#mum",
          canvasText,makeOutput);

DELTAPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAPHI0,outputDir+"/"+"DELTAPHI0."+oFext,"#Delta(Up-Down) Tracks","Number of tracks",
          "#Delta #phi_{0} [rad]",xLabels,yLabels,"mrad",
          canvasText,makeOutput);

DELTAETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAETA,outputDir+"/"+"DELTAETA."+oFext,"#Delta(Up-Down) Tracks","Number of tracks",
          "#Delta #eta",xLabels,yLabels,"",
          canvasText,makeOutput);

DELTAQOPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPt","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAQOPT,outputDir+"/"+"DELTAQOPT."+oFext,"#Delta(Up-Down) Tracks","Number of tracks",
          "#Delta q/p_{T}  [GeV^{-1}]",xLabels,yLabels,"TeV^{-1}",
          canvasText,makeOutput);

DELTAPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_pt","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAPT,outputDir+"/"+"DELTAPT."+oFext,"#Delta(Up-Down) Tracks","Number of tracks",
          "#Delta p_{T}  [GeV]",xLabels,yLabels,"GeV",
          canvasText,makeOutput);

DELTAZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAZ0,outputDir+"/"+"DELTAZ0."+oFext,"#Delta(Up-Down) Tracks","Number of tracks"
          ,"#Delta z_{0} [mm]",xLabels,yLabels,"#mum",
          canvasText,makeOutput);

DELTAQ = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_charge","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAQ,outputDir+"/"+"DELTAQ."+oFext,"#Delta(Up-Down) Tracks","Number of tracks"
          ,"#Delta q",xLabels,yLabels,"#Delta q ",
          canvasText,makeOutput);

DELTANHITS = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_nHits","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTANHITS,outputDir+"/"+"DELTANHITS."+oFext,"#Delta(Up-Down) Tracks","Number of tracks"
          ,"#Delta N_{hits}",xLabels,yLabels,"#Delta N_{hits} ",
          canvasText,makeOutput);

# profiles vs pt
DELTAD0VSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAD0VSPT,outputDir+"/"+"DELTAD0vsPT."+oFext,"#Delta(Up-Down) Tracks","#Delta d_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAZ0VSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAZ0VSPT,outputDir+"/"+"DELTAZ0vsPT."+oFext,"#Delta(Up-Down) Tracks","#Delta z_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAPHIVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAPHIVSPT,outputDir+"/"+"DELTAPHIvsPT."+oFext,"#Delta(Up-Down) Tracks","#Delta #phi_{0} [rad]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAETAVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAETAVSPT,outputDir+"/"+"DELTAETAvsPT."+oFext,"#Delta(Up-Down) Tracks","#Delta #eta", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.881, False);

DELTAQOPTVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAQOPTVSPT,outputDir+"/"+"DELTAQOPTvsPT."+oFext,"#Delta(Up-Down) Tracks","#Delta q/p_{T} [GeV^{-1}]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

# profiles vs z0
DELTAD0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsZ0","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAD0,outputDir+"/"+"DELTAD0vsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta d_{0} [mm]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsZ0","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAZ0,outputDir+"/"+"DELTAZ0vsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta z_{0} [mm]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAPHI = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsZ0","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAPHI,outputDir+"/"+"DELTAPHIvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta #phi_{0} [rad]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAETA = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsZ0","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAETA,outputDir+"/"+"DELTAETAvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta #eta", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.881, False);

DELTAQOPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsZ0","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAQOPT,outputDir+"/"+"DELTAQOPTvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta q/p_{T} [GeV^{-1}]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


# sigma vs pt (From TH2, using RMS)
SIGMAD0VSPT = MakeProfSigmaPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(SIGMAD0VSPT,outputDir+"/"+"SIGMAD0vsPT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta d_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

SIGMAZ0VSPT = MakeProfSigmaPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(SIGMAZ0VSPT,outputDir+"/"+"SIGMAZ0vsPT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta z_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

SIGMAPHIVSPT = MakeProfSigmaPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(SIGMAPHIVSPT,outputDir+"/"+"SIGMAPHIvsPT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta #phi_{0} [rad]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

SIGMAETAVSPT = MakeProfSigmaPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(SIGMAETAVSPT,outputDir+"/"+"SIGMAETAvsPT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta #eta", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

SIGMAQOPTVSPT = MakeProfSigmaPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsPt","noFit",rootFiles,nFiles,True)
DrawPlots(SIGMAQOPTVSPT,outputDir+"/"+"SIGMAQOPTvsPT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta q/p_{T} [GeV^{-1}]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


#profiles vs z0
if (userExtended):
    DELTAD0VSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAD0VSZ0,outputDir+"/"+"DELTAD0vsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta d_{0} [mm]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAZ0VSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAZ0VSZ0,outputDir+"/"+"DELTAZ0vsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta z_{0} [mm]", "z_{0} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAPHIVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAPHIVSZ0,outputDir+"/"+"DELTAPHIvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta #phi_{0} [rad]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAETAVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAETAVSZ0,outputDir+"/"+"DELTAETAvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta #eta", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.881, False);
    DELTAQOPTVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAQOPTVSZ0,outputDir+"/"+"DELTAQOPTvsZ0."+oFext,"#Delta(Up-Down) Tracks","#Delta q/p_{T} [GeV^{-1}]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    

# sigma vs pt (Reprocessed plots: This plots are only available if the collection has been reprocessed)
#print "<MakeTrackSegmentsPlots> TrackSegmentCollection = ", TrackSegmentCollection
#if (TrackSegmentCollection == "AlignTracks_NoTriggerSelection" and userExtended):
if (userExtended):
	SIGMAD0VSPT_FIT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_d0VsPt","noFit",rootFiles,nFiles,True)
	DrawPlots(SIGMAD0VSPT_FIT,outputDir+"/"+"SIGMAD0vsPT_FIT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta d_{0} [mm]", "p_{T}  [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

	SIGMAZ0VSPT_FIT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_z0VsPt","noFit",rootFiles,nFiles,True)
	DrawPlots(SIGMAZ0VSPT_FIT,outputDir+"/"+"SIGMAZ0vsPT_FIT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta z_{0} [mm]", "p_{T}  [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

	SIGMAPHIVSPT_FIT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_phi0VsPt","noFit",rootFiles,nFiles,True)
	DrawPlots(SIGMAPHIVSPT_FIT,outputDir+"/"+"SIGMAPHIvsPT_FIT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta #phi_{0} [rad]", "p_{T}  [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

	SIGMAETAVSPT_FIT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_eta0VsPt","noFit",rootFiles,nFiles,True)
	DrawPlots(SIGMAETAVSPT_FIT,outputDir+"/"+"SIGMAETAvsPT_FIT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta #eta", "p_{T}  [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

	SIGMAQOPTVSPT_FIT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"sigma_delta_qOverPtVsPt","noFit",rootFiles,nFiles,True)
	DrawPlots(SIGMAQOPTVSPT_FIT,outputDir+"/"+"SIGMAQOPTvsPT_FIT."+oFext,"#Delta(Up-Down) Tracks","#sigma #Delta q/p_{T} [GeV^{-1}]", "p_{T}  [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
print " -- MakeTrackSegmentsPlots -- completed "
#exit()
