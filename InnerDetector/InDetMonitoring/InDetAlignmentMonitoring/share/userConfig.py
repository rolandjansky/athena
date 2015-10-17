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

### NEW METHODIn

SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0010/collisions/TotalMonitoring.root", "281411_0010", kFullCircle, kRed+2,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0011/collisions/TotalMonitoring.root", "281411_0011", kOpenSquare, kAzure+1,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0012/collisions/TotalMonitoring.root", "281411_0012", kFullCircle, kYellow+1,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0013/collisions/TotalMonitoring.root", "281411_0013", kOpenSquare, kViolet+1,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0014/collisions/TotalMonitoring.root", "281411_0014", kFullCircle, kOrange+1,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0015/collisions/TotalMonitoring.root", "281411_0015", kOpenSquare, kGreen+1,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0016/collisions/TotalMonitoring.root", "281411_0016", kFullCircle, kGray+3,"Tracks_alignSelection","run_281411")
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0017/collisions/TotalMonitoring.root", "281411_0017", kOpenSquare, kRed+1,"Tracks_alignSelection","run_281411")
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.6.4/runLB/Iter0_281411_0018/collisions/TotalMonitoring.root", "281411_0018", kFullCircle, kAzure,"Tracks_alignSelection","run_281411")

####
canvasText   = ["Align Tracks", "","",""] #specifies additional canvas text
    
normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#following array determines ROOT file folders. 
# Silicon (IBL, PIX & SCT residuals)
residualsDir = [
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

# TRT residuals
trtResidualsDir = [
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/",
    "/IDAlignMon/AlignTracks_all/Residuals/"
    ] 

# Track parameters, hits, etc
genericTracksDir = [
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    "/IDAlignMon/AlignTracks_all/GenericTracks/",
    ] 

# Hit efficiency 
hitEffDir = [
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    "/IDAlignMon/AlignTracks_all/HitEfficiencies/",
    ] 
#following array determines ROOT file directory from which SivsTRT plots are taken
#for each of the 3 files 
siTRTDir = [
    "",
    "",
    ""
    ] 

#following array determines ROOT file directory from which cosmic SplitTracks plots are taken
splitTracksDir = [
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/AlignTracks_NoTriggerSelection/TrackSegments/"
    ] 

#
# in case user give files in the new format (March-April 2015)
#

if len(theInputFileList)>0:
    rootFileNames = []
    markerStyles = []
    markerColors = []
    legendTitles = []
    for i in range(len(theInputFileList)):
        rootFileNames.append(theInputFileList[i])
        markerStyles.append(theMarkerList[i])
        markerColors.append(theColorList[i])
        legendTitles.append(theLegendList[i])
        if (len(theTrackCollectionList[i])>0):
            residualsDir[i] = "/"+thePrefix[i]+"/IDAlignMon/"+ theTrackCollectionList[i] + "/Residuals/"
            trtResidualsDir[i] = "/"+thePrefix[i]+"/IDAlignMon/"+ theTrackCollectionList[i] + "/Residuals/"
            genericTracksDir[i] ="/"+thePrefix[i]+"/IDAlignMon/"+ theTrackCollectionList[i] + "/GenericTracks/"
            hitEffDir[i] = "/"+thePrefix[i]+"/IDAlignMon/"+ theTrackCollectionList[i] + "/HitEfficiencies/"
            splitTracksDir[i] = "/"+thePrefix[i]+splitTracksDir[i]
            
            
##----------------------------
