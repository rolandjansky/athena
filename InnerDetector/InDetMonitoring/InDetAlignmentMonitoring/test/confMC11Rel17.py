
# specify the ROOT histogram files containing the monitoring plots here
# Strings can be empty BUT MUST BE 3
#rootFileNames = ["../run/MonMerge-05.root",
#                 "../run/MonMerge-EoPCalib.root",
#                 "../run/MonMerge-EoPCal_v2.root"
#                 ]

rootFileNames = [#"180636Rel17_v3-Merge.root",
#                 "180636_JetTau-Merge.root",
#                 "180636_JetTau_grl-Merge.root",
                  "189822JetTau_NovRel17_weight-Merge.root",
#  "j1mc-Merge.root",
#                 "j4mc-Merge.root",
#                 "j2mc-Merge.root",
#                 "j0mc-Merge.root"
#                 "j5mc-Merge.root",
#                 "j7mc-Merge.root",
#                 "j8mc-Merge.root"
                 "j4mc_reweighted_normW-Merge.root",
                 "189822JetTau_JulyRel17-Merge.root",
                 ""
                 ]

# this sets the output directory where plot gifs will be saved


normaliseHistos = True # set to true if you want to normalise to same area
unitArea = True # set to true if you want to draw residual histos to unit area

oFext="png" # output file extention without the dot
use15GevSelection = True

if not use15GevSelection :
  outputDir = "../run/testResults"
  #following array determines ROOT file directory from which Si residual plots are taken
  #for each of the 3 files 
  residualsDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"#,
    #    "/run_105001/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"
    ] 
  #following array determines ROOT file directory from which TRT residual plots are taken
  #for each of the 3 files 
  trtResidualsDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"
    ] 
  #following array determines ROOT file directory from which GenericTracks plots are taken
  #for each of the 3 files 
  genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/"
    ] 
  #following array determines ROOT file directory from which HitEfficiency plots are taken
  #for each of the 3 files 
  hitEffDir = [
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_NoTriggerSelection/HitEfficiencies/"
    ] 
  #following array determines ROOT file directory from which SivsTRT plots are taken
  #for each of the 3 files 
  siTRTDir = [
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/",
    "/IDAlignMon/SivsTRT/NoTriggerSelection/"#,
    ] 

  #following array determines ROOT file directory from which cosmic SplitTracks plots are taken
  #for each of the 3 files 
  splitTracksDir = [
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/"#,
    ] 
  canvasText = ["run 180636 ","Collisions '11 Alignment","",""] #specifies additional canvas text
else: #  for 15 gev selection
  outputDir = "../run/testResults15Gev"
  #following array determines ROOT file directory from which Si residual plots are taken
  #for each of the 3 files 
  residualsDir = [
#    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/"#,
    ] 
  #following array determines ROOT file directory from which TRT residual plots are taken
  #for each of the 3 files 
  trtResidualsDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
#    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/"
    ] 
  #following array determines ROOT file directory from which GenericTracks plots are taken
  #for each of the 3 files 
  genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
 #   "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/"
    ] 
  #following array determines ROOT file directory from which HitEfficiency plots are taken
  #for each of the 3 files 
  hitEffDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/HitEfficiencies/",
  #  "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/HitEfficiencies/"
    ] 
  #following array determines ROOT file directory from which SivsTRT plots are taken
  #for each of the 3 files 
  siTRTDir = [
    "/IDAlignMon/SivsTRT/noTrigSel_15GeV/",
   # "/IDAlignMon/SivsTRT/noTrigSel_15GeV/",
    "/IDAlignMon/SivsTRT/noTrigSel_15GeV/",
    "/IDAlignMon/SivsTRT/noTrigSel_15GeV/",
    "/IDAlignMon/SivsTRT/noTrigSel_15GeV/"
    ] 
   
  #following array determines ROOT file directory from which cosmic SplitTracks plots are taken
  #for each of the 3 files 
  splitTracksDir = [
    "/IDAlignMon/ResolvedTracks_noTrigSel_15GeV/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_noTrigSel_15GeV/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_noTrigSel_15GeV/TrackSegments/",
 #   "/IDAlignMon/ResolvedTracks_noTrigSel_15GeV/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_noTrigSel_15GeV/TrackSegments/"#,
    ]
  canvasText = ["ATLAS Preliminary","#sqrt{s} = 7 TeV","Track pT > 15 GeV",""] #specifies additional canvas text
   
markerStyles = [kFullCircle,kOpenCircle,kOpenSquare,kOpenSquare] #determines marker styles for plots
markerColors = [kBlue,kRed,kBlack, kViolet] #determines marker colors for plots
legendTitles = [" Reprocessing 2011 Fall", "jet reweighted MC", "Reprocessing 2011 Summer", ""] #determines legend titles

##----------------------------
