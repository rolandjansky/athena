# specify the ROOT histogram files containing the monitoring plots here that you want to compare
# Anything from 1 file to 4 files can be specified
#rootFileNames = ["/afs/cern.ch/user/p/pbutti/spacework/MultiMuonsMonitoring.root"]
    
rootFileNames = ["/afs/cern.ch/user/p/pbutti/spacework/IBLAlignDev_19/run_Analog/Zmumu_Rel19_Analog.root",
                 "/afs/cern.ch/user/p/pbutti/spacework/IBLAlignDevV2/run_Analog/Zmumu_Rel19_Digital.root",
                 ""
                 ]
# this sets the output directory where plot gifs will be saved.
outputDir="Clustering_Rel19vsRel7_Digital"
#outputDir="Comparison"
normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

residualsDir = [
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/"
    ]

trtResidualsDir =[
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/",
    "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/"
    ]
genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_alignSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_alignSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_alignSelection/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_alignSelection/GenericTracks/"
    ]

#for each of the (up to) 4 files
markerStyles = [kFullCircle,kOpenCircle,kOpenSquare, kFullSquare] #determines marker styles for plots
markerColors = [kBlue,kRed,kBlack,kBlack] #determines marker colors for plots
#legendTitles = ["Nominal Geometry MC","Misaligned MC","Realigned MC",""] #determines legend titles
#legendTitles = ["Analog","Digital"]
legendTitles   = ["Rel19","Rel17"]
canvasText = ["#bf{#it{ATLAS}} Internal","Digital Clustering","",""] #specifies additional canvas text
##----------------------------
