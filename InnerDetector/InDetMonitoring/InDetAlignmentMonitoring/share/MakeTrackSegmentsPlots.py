# 
# This script steers the option to produce the track-segment matching and properties plots
#
xLabels = 0.17
yLabels = 0.91
#
DELTAD0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAD0,outputDir+"/"+"DELTAD0.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta d_{0} [mm]",xLabels,yLabels,"#mum",
          canvasText,makeOutput);

DELTAPHI0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAPHI0,outputDir+"/"+"DELTAPHI0.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta #phi [radians]",xLabels,yLabels,"mrad",
          canvasText,makeOutput);

DELTAETA = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAETA,outputDir+"/"+"DELTAETA.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta #eta",xLabels,yLabels,"",
          canvasText,makeOutput);

DELTAQOPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPt","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAQOPT,outputDir+"/"+"DELTAQOPT.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta q/p_{T}  [GeV^{-1}]",xLabels,yLabels,"TeV^{-1}",
          canvasText,makeOutput);

DELTAPT = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_pt","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAPT,outputDir+"/"+"DELTAPT.png","#Delta(Up-Down) Tracks","number of tracks",
          "#Delta p_{T}  [GeV]",xLabels,yLabels,"GeV",
          canvasText,makeOutput);

DELTAZ0 = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0","noFitWithStats",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAZ0,outputDir+"/"+"DELTAZ0.png","#Delta(Up-Down) Tracks","number of tracks"
          ,"#Delta z_{0} [mm]",xLabels,yLabels,"#mum",
          canvasText,makeOutput);

DELTAQ = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_charge","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTAQ,outputDir+"/"+"DELTAQ.png","#Delta(Up-Down) Tracks","number of tracks"
          ,"#Delta q",xLabels,yLabels,"#Delta q ",
          canvasText,makeOutput);

DELTANHITS = MakePlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_nHits","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(DELTANHITS,outputDir+"/"+"DELTANHITS.png","#Delta(Up-Down) Tracks","number of tracks"
          ,"#Delta N_{hits}",xLabels,yLabels,"#Delta N_{hits} ",
          canvasText,makeOutput);

# profiles vs pt
DELTAD0VSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAD0VSPT,outputDir+"/"+"DELTAD0vsPT.png","#Delta(Up-Down) Tracks","#Delta d_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAZ0VSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAZ0VSPT,outputDir+"/"+"DELTAZ0vsPT.png","#Delta(Up-Down) Tracks","#Delta z_{0} [mm]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAPHIVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAPHIVSPT,outputDir+"/"+"DELTAPHIvsPT.png","#Delta(Up-Down) Tracks","#Delta #phi_{0} [rad]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

DELTAETAVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAETAVSPT,outputDir+"/"+"DELTAETAvsPT.png","#Delta(Up-Down) Tracks","#Delta #eta", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.881, False);

DELTAQOPTVSPT = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsPt","noFit",rootFiles,nFiles,True)
DrawPlots(DELTAQOPTVSPT,outputDir+"/"+"DELTAQOPTvsPT.png","#Delta(Up-Down) Tracks","#Delta q/p_{T} [GeV^{-1}]", "p_{T} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

#profiles vs z0
if (userExtended):
    DELTAD0VSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_d0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAD0VSZ0,outputDir+"/"+"DELTAD0vsZ0.png","#Delta(Up-Down) Tracks","#Delta d_{0} [mm]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAZ0VSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_z0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAZ0VSZ0,outputDir+"/"+"DELTAZ0vsZ0.png","#Delta(Up-Down) Tracks","#Delta z_{0} [mm]", "z_{0} [GeV]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAPHIVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_phi0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAPHIVSZ0,outputDir+"/"+"DELTAPHIvsZ0.png","#Delta(Up-Down) Tracks","#Delta #phi_{0} [rad]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    DELTAETAVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_eta0VsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAETAVSZ0,outputDir+"/"+"DELTAETAvsZ0.png","#Delta(Up-Down) Tracks","#Delta #eta", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.881, False);
    DELTAQOPTVSZ0 = MakeProfPlots(splitTracksDir,legendTitles,markerColors,markerStyles,"delta_qOverPtVsZ0","noFit",rootFiles,nFiles,True)
    DrawPlots(DELTAQOPTVSZ0,outputDir+"/"+"DELTAQOPTvsZ0.png","#Delta(Up-Down) Tracks","#Delta q/p_{T} [GeV^{-1}]", "z_{0} [mm]",xLabels,yLabels,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    

print " -- MakeTrackSegmentsPlots -- completed "
#exit()
