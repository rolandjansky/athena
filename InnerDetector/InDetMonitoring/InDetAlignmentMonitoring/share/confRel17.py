
# specify the ROOT histogram files containing the monitoring plots here
# Strings can be empty BUT MUST BE 3
#rootFileNames = ["../run/MonMerge-05.root",
#                 "../run/MonMerge-EoPCalib.root",
#                 "../run/MonMerge-EoPCal_v2.root"
#                 ]

rootFileNames = ["/afs/cern.ch/user/m/martis/Athena/release/17.2.7.4.1/InnerDetector/InDetExample/InDetAlignExample/run/Run209899_Iter3/run209899/TotalMonitoring.root",
                 "/afs/cern.ch/user/m/martis/Athena/release/17.2.7.4.1/InnerDetector/InDetExample/InDetAlignExample/run/Iter3/run212144/TotalMonitoring.root",
#"/afs/cern.ch/user/m/martis/Athena/release/17.2.7.4.1/InnerDetector/InDetExample/InDetAlignExample/run/Iter3/run212144/TotalMonitoring.root"]
""]

markerStyles = [kFullCircle,kOpenCircle,kFullSquare] #determines marker styles for plots
markerColors = [kRed,kBlack,kBlack] #determines marker colors for plots
legendTitles = ["Run209899","Run212144 L1", "Run 212144 L2"] #determines legend titles

# this sets the output directory where plot gifs will be saved

# not working nMAXFIles = 3
normaliseHistos = True # set to true if you want to normalise to same area
unitArea = True # set to true if you want to draw residual histos to unit area

oFext="png" # output file extention without the dot
use15GevSelection = False

if not use15GevSelection :
    #outputDir = "../run/testResults"
  outputDir = "../plots"
  #following array determines ROOT file directory from which Si residual plots are taken
  #for each of the 3 files 
  residualsDir = [
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/",
    #    "/run_105001/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals/"
    ] 
  #following array determines ROOT file directory from which TRT residual plots are taken
  #for each of the 3 files 
  trtResidualsDir = [
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/Residuals/"
    ] 
  #following array determines ROOT file directory from which GenericTracks plots are taken
  #for each of the 3 files 
  genericTracksDir = [
    "/IDAlignMon/AlignTracksNormalRefitted_all/GenericTracks/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/GenericTracks/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/GenericTracks/"
    ] 
  #following array determines ROOT file directory from which HitEfficiency plots are taken
  #for each of the 3 files 
  hitEffDir = [
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/"
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
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/ResolvedTracks_NoTriggerSelection/TrackSegments/"#,
    ] 
  canvasText = ["run 209998 ","Hola hola","",""] #specifies additional canvas text
else: #  for 15 gev selection
  outputDir = "../run/testResults15Gev"
  #following array determines ROOT file directory from which Si residual plots are taken
  #for each of the 3 files 
  residualsDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/"#,
    ] 
  #following array determines ROOT file directory from which TRT residual plots are taken
  #for each of the 3 files 
  trtResidualsDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/Residuals/"
    ] 
  #following array determines ROOT file directory from which GenericTracks plots are taken
  #for each of the 3 files 
  genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/"
    ] 
  #following array determines ROOT file directory from which HitEfficiency plots are taken
  #for each of the 3 files 
  hitEffDir = [
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracksNormalRefitted_all/HitEfficiencies/"
    ] 
  #following array determines ROOT file directory from which SivsTRT plots are taken
  #for each of the 3 files 
  siTRTDir = [
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
    ]
  canvasText = ["ATLAS Preliminary","#sqrt{s} = 7 TeV","Track pT > 15 GeV",""] #specifies additional canvas text
   
##----------------------------
