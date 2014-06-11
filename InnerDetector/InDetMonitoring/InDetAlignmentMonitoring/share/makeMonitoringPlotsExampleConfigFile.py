
# specify the ROOT histogram files containing the monitoring plots here that you want to compare
# Anything from 1 file to 4 files can be specified
rootFileNames = ["FilesForNov2010ValidationPlots/user.bencooper.periodEMinBias.t0pro04_v01.minbiasGRL.15.6.9.8_B.merged_monitoring-387.root",
                 "FilesForNov2010ValidationPlots/user.bencooper.periodEJetTauEtmiss.repro05_v02.jetetmissGRL.TriggerAware.merged_monitoring-455.root",
                 "",
                 ""
                 ]

# this sets the output directory where plot gifs will be saved
outputDir = "AlignmentValidationResults"

normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#following array determines ROOT file directory from which Si residual plots are taken
#for each of the (up to) 4 files 
residualsDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"
                ] 
#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the (up to) 4 files 
trtResidualsDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"
                ] 
#following array determines ROOT file directory from which GenericTracks plots are taken
#for each of the (up to) 4 files 
genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/"
                ] 
#following array determines ROOT file directory from which HitEfficiency plots are taken
#for each of the (up to) 4 files 
hitEffDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/"
                ] 
#following array determines ROOT file directory from which SivsTRT plots are taken
#for each of the (up to) 4 files 
siTRTDir = [
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/"
                ] 

#following array determines ROOT file directory from which cosmic SplitTracks plots are taken
#for each of the (up to) 4 files 
splitTracksDir = ["/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
                  "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
                  "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",                  
                  "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/"] 

markerStyles = [kFullCircle,kOpenCircle,kOpenSquare, kFullSquare] #determines marker styles for plots
markerColors = [kBlue,kRed,kBlack,kBlack] #determines marker colors for plots
legendTitles = ["Monitoring A","Monitoring B", "", ""] #determines legend titles
canvasText = ["Data Runs","Collisions '10 Alignment","",""] #specifies additional canvas text
##----------------------------
