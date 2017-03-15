#
# start with lumiblock
#
LUMIBLOCK = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"LumiBlock","noFit",rootFiles,nFiles,False) 
DrawPlots(LUMIBLOCK, outputDir+"/"+"LumiBlock."+oFext, "", "Events",
          "LumiBlock", 0.65, 0.80, "",
          canvasText,makeOutput,0.60,0.85,True)

BSPAR = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"XBs","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(BSPAR, outputDir+"/"+"BeamSpot_X.png", "", "Events", "Beam Spot X [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

BSPAR = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"YBs","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(BSPAR, outputDir+"/"+"BeamSpot_Y.png", "", "Events", "Beam Spot Y [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

BSPAR = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"ZBs","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(BSPAR, outputDir+"/"+"BeamSpot_Z.png", "", "Events", "Beam Spot Z [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

BSPAR = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"TiltX_Bs","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(BSPAR, outputDir+"/"+"BeamSpot_TiltX.png", "", "Events", "Beam Spot Tilt X", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

BSPAR = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"TiltY_Bs","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(BSPAR, outputDir+"/"+"BeamSpot_TiltY.png", "", "Events", "Beam Spot Tilt Y", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackD0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"d0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackD0, outputDir+"/"+"d0.png", "", "N Tracks",
          "Impact Parameter [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

TrackZ0 = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"z0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackZ0, outputDir+"/"+"z0.png", "", "N Tracks",
          "z0 [mm]", 0.19, 0.9, "#mum",
          canvasText,makeOutput)

D0VSPHI0 = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"D0VsPhi0","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(D0VSPHI0,outputDir+"/"+"D0vsPHI0.png","","d_{0} [mm]", "#phi_{0} [rad]",0.2,0.9,"", canvasText,makeOutput, 0.60, 0.91, False);

#D0VSPHI0 = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"D0bsVsPhi0_Barrel","noFit",rootFiles,nFiles,normaliseHistos)
#DrawPlots(D0VSPHI0,outputDir+"/"+"D0vsPHI0_BAR.png","Barrel","d_{0} [mm]", "#phi_{0} [rad]",0.2,0.9,"", canvasText,makeOutput, 0.60, 0.91, False);


