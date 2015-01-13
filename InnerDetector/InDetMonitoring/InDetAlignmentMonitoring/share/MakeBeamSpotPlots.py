#
# start with lumiblock
#
LUMIBLOCK = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"LumiBlock","noFit",rootFiles,nFiles,False) 
DrawPlots(LUMIBLOCK, outputDir+"/"+"LumiBlock."+oFext, "", "Events",
          "LumiBlock", 0.65, 0.80, "",
          canvasText,makeOutput,0.60,0.85,True)

