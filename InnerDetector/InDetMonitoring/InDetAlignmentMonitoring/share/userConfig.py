#
# userConfig.py
#
# in this file the user configures the input files, the directories from where the histograms are imported and the kind of output
#

outputDir = "../plots_NN_Digi" # this sets the output directory where plot gifs will be saved

# output type
oFext="png" # output file extention without the dot
if (userPDF): oFext="pdf"

# specify the ROOT histogram files containing the monitoring plots here

### NEW METHODIn


#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM8/cosmics/TotalMonitoring.root", "Cosmics M8", kOpenSquare, kGray+1)
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runTest/Iter0/collisions/TotalMonitoring.root", "Digital Clustering", kFullCircle, kOrange+2)
SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL2/Iter1/collisions/TotalMonitoring.root", "NN Clustering", kFullCircle, kBlue+2)


####
canvasText   = ["Run 265573", "","",""] #specifies additional canvas text

SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL3IBL/Iter1_4Dofs/collisions/TotalMonitoring.root", "Aligned (L3 4Dofs)", kFullCircle, kGreen+2,"/IDAlignMon/ExtendedTracks_all/Residuals/")

SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL3IBL/Iter1_3Dofs/collisions/TotalMonitoring.root", "Aligned (L3 3Dofs)", kOpenSquare, kAzure+1,"/IDAlignMon/ExtendedTracks_all/Residuals/")

SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL3IBL/Iter0_4Dofs/collisions/TotalMonitoring.root", "before L3", kOpenCircle, kRed,"/IDAlignMon/ExtendedTracks_all/Residuals/")

#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0/collisions/TotalMonitoring.root", "M9 cosmic alignment", kOpenCircle, kRed,"/IDAlignMon/ExtendedTracks_all/Residuals/")

#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runTest/Iter0/collisions/TotalMonitoring.root", "digital clustering", kOpenSquare, kAzure+1,"/IDAlignMon/ExtendedTracks_all/Residuals/")
#SetNextInputFile("/afs/cern.ch/user/p/pbutti/spacework/public/900GeV_PreAlignment_ForSalva.root", "900 GeV", kOpenSquare, kAzure+1,"/IDAlignMon/ExtendedTracks_all/Residuals/")

#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/M9Alignment/runBatchL27/Iter2/cosmics/TotalMonitoring.root", "Aligned (latest step)", kFullCircle, kGray+3,"/IDAlignMon/ExtendedTracks_all/Residuals/")
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/M9Alignment/runBatchL1/Iter0/cosmics/TotalMonitoring.root", "Initial (with fixed DB)", kFullCircle, kRed+1,"/IDAlignMon/ExtendedTracks_all/Residuals/")

# M9 
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.4.11/CollisionAlignment/runBatchL1/Iter0_RecoWithM9/cosmics/TotalMonitoring.root", "M9 BON Best Alignment", kOpenSquare, kAzure+5,"/IDAlignMon/ExtendedTracks_all/Residuals/")
#SetNextInputFile("/afs/cern.ch/work/m/martis/public/athena/20.1.0.3/M8Alignment/runTotal/Iter0/cosmics/TotalMonitoring.root", "M8 Best Alignment", kOpenSquare, kAzure+5,"/IDAlignMon/ExtendedTracks_all/Residuals/")

# MC 
#SetNextInputFile("/afs/cern.ch/user/p/pbutti/spacework/public/MC_Cosmics_BON_Total_Monitoring.root","MC BON",kFullCross, kGreen+1,"/IDAlignMon/ExtendedTracks_all/Residuals/")



####
canvasText   = ["Run265573 @ 13TeV", "ExtendedTracks","",""] #specifies additional canvas text
    
normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area
TrackCollections = ["ExtendedTracks_all","ExtendedTracks_all","ExtendedTracks_all","ExtendedTracks_all","ExtendedTracks_all","ExtendedTracks_all"]
Prefix = "/IDAlignMon/"
#following array determines ROOT file directory from which Si residual plots are taken
#for each of the input files 

residualsDir     = []
trtResidualsDir  = []
genericTracksDir = []
hitEffDir        = []
splitTracksDir   = []
siTRTDir         = []
for TrackCollection in TrackCollections:
    residualsDir.append(Prefix+TrackCollection+"/Residuals/")
    trtResidualsDir.append(Prefix+TrackCollection+"/Residuals/")
    genericTracksDir.append(Prefix+TrackCollection+"/GenericTracks/")
    hitEffDir.append(Prefix+TrackCollection+"/HitEfficiencies/")
    splitTracksDir.append(Prefix+TrackCollection+"/TrackSegments/")
    siTRTDir.append("")

#following array determines ROOT file directory from which cosmic SplitTracks plots are taken
#for each of the 3 files 

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
