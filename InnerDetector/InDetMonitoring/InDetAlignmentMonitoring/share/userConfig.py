 #
# userConfig.py
#
# in this file the user configures the input files, the directories from where the histograms are imported and the kind of output
#
# output type

oFext="png" # output file extention without the dot
outputDir = "../plots" # this sets the output directory where plot gifs will be saved

# specify the ROOT histogram files containing the monitoring plots here
# Strings can be empty BUT MUST BE 3

rootFileNames = ["/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_new/runBatch/Iter0/cosmics/TotalMonitoring.root",
                 "/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_new/runBatch/Iter2/cosmics/TotalMonitoring.root",
                 "/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_new/runBatch/Iter3/cosmics/TotalMonitoring.root"]

markerStyles = [kOpenCircle, kFullCircle, kOpenSquare] #determines marker styles for plots
markerColors = [kRed+1, kGray+2, kGreen+1] #determines marker colors for plots
legendTitles = ["Iter0","Iter1", "Iter2"] #determines legend titles
canvasText   = ["M7 cosmic rays","","",""] #specifies additional canvas text

normaliseHistos = False # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#following array determines ROOT file directory from which Si residual plots are taken
#for each of the input files 
residualsDir = [
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the 3 files 
trtResidualsDir = [
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which GenericTracks plots are taken
#for each of the 3 files 
genericTracksDir = [
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/"
    ] 

#following array determines ROOT file directory from which HitEfficiency plots are taken
#for each of the 3 files 
hitEffDir = [
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
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
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/"#,
    ] 

##----------------------------
