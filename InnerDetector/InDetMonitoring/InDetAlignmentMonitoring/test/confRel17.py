# output type

oFext="png" # output file extention without the dot
outputDir = "../plots" # this sets the output directory where plot gifs will be saved

# specify the ROOT histogram files containing the monitoring plots here
# Strings can be empty BUT MUST BE 3
#rootFileNames = ["../run/MonMerge-05.root",
#                 "../run/MonMerge-EoPCalib.root",
#                 "../run/MonMerge-EoPCal_v2.root"
#                 ]

rootFileNames = ["/afs/cern.ch/user/m/martis/Athena/release/19.1.3.7/run/monitoring.root", 
                 "",
                 ""]

markerStyles = [kFullCircle,kOpenCircle,kFullSquare] #determines marker styles for plots
markerColors = [kRed+2,kBlack,kBlack] #determines marker colors for plots
legendTitles = ["Run 242651","legend 2", "legend 3"] #determines legend titles
canvasText   = ["M6 cosmic rays","","",""] #specifies additional canvas text

normaliseHistos = True # set to true if you want to normalise to same area
unitArea = True # set to true if you want to draw residual histos to unit area

#following array determines ROOT file directory from which Si residual plots are taken
#for each of the input files 
residualsDir = [
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the 3 files 
trtResidualsDir = [
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 
#following array determines ROOT file directory from which GenericTracks plots are taken
#for each of the 3 files 
genericTracksDir = [
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/"
    ] 
#following array determines ROOT file directory from which HitEfficiency plots are taken
#for each of the 3 files 
hitEffDir = [
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/"
    ] 
#following array determines ROOT file directory from which SivsTRT plots are taken
#for each of the 3 files 
siTRTDir = [
    "",
    "",
    ""
    ] 

#following array determines ROOT file directory from which cosmic SplitTracks plots are taken
#for each of the 3 files 
splitTracksDir = [
    "/IDAlignMon/ResolvedSCTTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/"#,
    ] 

##----------------------------
