#
# start with lumiblock
#
LUMIBLOCK = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"LumiBlock","noFit",rootFiles,nFiles,False) 
DrawPlots(LUMIBLOCK, outputDir+"/"+"LumiBlock."+oFext, "", "Events",
          "LumiBlock", 0.65, 0.80, "",
          canvasText,makeOutput,0.60,0.85,True)

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


