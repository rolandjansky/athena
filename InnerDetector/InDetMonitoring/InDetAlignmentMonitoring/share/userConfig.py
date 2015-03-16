 #
# userConfig.py
#
# in this file the user configures the input files, the directories from where the histograms are imported and the kind of output
#

outputDir = "../plots" # this sets the output directory where plot gifs will be saved

# output type
oFext="png" # output file extention without the dot
if (userPDF): oFext="pdf"

# specify the ROOT histogram files containing the monitoring plots here
# Strings can be empty BUT MUST BE 3

rootFileNames = ["/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0_BON_2T_PIX/cosmics/TotalMonitoring.root",
                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Laura_Iter0_BON_2T_NoPIX_TotalMonitoring.root",
                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0_BON_1T/cosmics/TotalMonitoring.root",
                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_BON/cosmics/TotalMonitoring.root"]

#"/afs/cern.ch/user/l/lbarranc/public/TotalMonitoring_M8_2T_Iter0.root",

#rootFileNames = ["/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_NoPixel_BOFF/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_NoPixel_BOFF/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_NoPixel_BON/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_NoPixel_BON/cosmics/TotalMonitoring.root"]                
#rootFileNames = ["/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0_BON_1T/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0_BON_2T/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0_BOFF/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M7Alignment/runBatchL1/Iter0_BOFF/cosmics/TotalMonitoring.root"]
#                 "/afs/cern.ch/work/m/martis/public/athena/19.3.0.3/runBatch/Iter0/cosmics/TotalMonitoring.root"]
#                 "/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runBatchL1/Iter0/cosmics/TotalMonitoring.root"]
#"/afs/cern.ch/work/m/martis/public/athena/19.3.0.3/runBatch/Iter5/cosmics/TotalMonitoring.root",
#                "/afs/cern.ch/work/m/martis/public/athena/19.3.0.3/runBatch2/Iter1/cosmics/TotalMonitoring.root"]

#rootFileNames = ["/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_2/runBatch/Iter0/cosmics/TotalMonitoring.root",
#                 "/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_2/runBatch/Iter0_GOOD/cosmics/TotalMonitoring.root",
#                 ""]#"/afs/cern.ch/work/m/martis/public/athena/19.3.0.1_M7_new/runBatchL3/Iter2/cosmics/TotalMonitoring.root"]

markerStyles = [kFullCircle, kOpenSquare, kOpenSquare, kFullCircle] #determines marker styles for plots
#markerColors = [kOrange-2, kOrange+8, kAzure+2, kViolet-1] #determines marker colors for plots
markerColors = [kOrange+8, kAzure+1, kViolet-1, kGray+2] #determines marker colors for plots
legendTitles = ["2T. Pixels ON","2T. Pixels OFF", "1 T. Pixels OFF", "M7 B ON (2 T)"] #determines legend titles
canvasText   = ["M8 cosmic rays", "Release 20.1.0.3","",""] #specifies additional canvas text

normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#following array determines ROOT file directory from which Si residual plots are taken
#for each of the input files 
residualsDir = [
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the 3 files 
trtResidualsDir = [
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which GenericTracks plots are taken
#for each of the 3 files 
genericTracksDir = [
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/"
    ] 

#following array determines ROOT file directory from which HitEfficiency plots are taken
#for each of the 3 files 
hitEffDir = [
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/"
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
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/"
    ] 

##----------------------------
