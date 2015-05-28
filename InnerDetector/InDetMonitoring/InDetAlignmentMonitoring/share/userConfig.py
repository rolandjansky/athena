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

SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM8/collisions/TotalMonitoring.root", "Collision M8", kFullCircle, kGray+3)
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM8/cosmics/TotalMonitoring.root", "Cosmics M8", kOpenSquare, kGray+1)
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM9/collisions/TotalMonitoring.root", "Collision M9", kFullCircle, kOrange+2,"/IDAlignMon/ExtendedTracks_all/Residuals/")
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM9/cosmics/TotalMonitoring.root", "Cosmics M9", kOpenSquare, kOrange-4)

####
canvasText   = ["Run 264034", "","",""] #specifies additional canvas text



    
normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#following array determines ROOT file directory from which Si residual plots are taken
#for each of the input files 
residualsDir = [
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the 3 files 
trtResidualsDir = [
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/CombinedInDetTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/",
    "/IDAlignMon/ExtendedTracks_all/Residuals/"
    ] 

#following array determines ROOT file directory from which GenericTracks plots are taken
#for each of the 3 files 
genericTracksDir = [
    "/IDAlignMon/ExtendedTracks_all/GenericTracks/",
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_all/GenericTracks/",
    "/IDAlignMon/CombinedInDetTracks_all/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_all/GenericTracks/",
    "/IDAlignMon/ExtendedTracks_all/GenericTracks/"
    ] 

#following array determines ROOT file directory from which HitEfficiency plots are taken
#for each of the 3 files 
hitEffDir = [
    "/IDAlignMon/ExtendedTracks_all/HitEfficiencies/",
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_all/HitEfficiencies/",
    "/IDAlignMon/CombinedInDetTracks_all/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_all/HitEfficiencies/",
    "/IDAlignMon/ExtendedTracks_all/HitEfficiencies/"
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
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/",
    "/IDAlignMon/Tracks_NoTriggerSelection/TrackSegments/"
    ] 

# in case user give files in the new format (March-April 2015)
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
            residualsDir[i] = theTrackCollectionList[i]
            trtResidualsDir[i] = theTrackCollectionList[i]
            
##----------------------------
