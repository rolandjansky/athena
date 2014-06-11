#===========================================================================
#===========================================================================
# Description:
#  This file loops over all the histograms in the monitoring file and makes
#   the comparison.
#
#  By default all profile, 1D and 2D histograms are processed using
#   the function in rootArtist (procProfile, procHistogram, and procHitMap)
#  To See how exceptions are dealt with read CompareAllExceptions.py
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

# Load the Root artist
include("InDetAlignmentMonitoring/rootArtist.py")

# Load the Exceptions
include("InDetAlignmentMonitoring/CompareAllExceptions.py")

# Make the directories in the rootfile
include("InDetAlignmentMonitoring/makeDirectories.py")

# Function that writes out the canvases 
include("InDetAlignmentMonitoring/writeOutput.py")

# Main script to loop over the contents of the monitoring.root file.
# ==================================================================
a_mainDir = a_file.GetDirectory("IDAlignMon")
b_mainDir = b_file.GetDirectory("IDAlignMon")

# Get the list of track collections and make it a set
a_trackCollections = set([ k.GetTitle() for k in a_mainDir.GetListOfKeys() ])
b_trackCollections = set([ k.GetTitle() for k in b_mainDir.GetListOfKeys() ])

# Getting the track collections which are in both files
allTrackCollections = a_trackCollections.intersection(b_trackCollections)

# Loop over track collections
for thisTrackCollection in allTrackCollections:
    print "Doing Track Collection",thisTrackCollection
    print "=================================================================="
    a_trackColDir = a_file.GetDirectory("IDAlignMon/"+thisTrackCollection)
    b_trackColDir = b_file.GetDirectory("IDAlignMon/"+thisTrackCollection)

    # Get the list of modules for this track collection and make it a set
    a_modules = set([ k.GetTitle() for k in a_trackColDir.GetListOfKeys() ])
    b_modules = set([ k.GetTitle() for k in b_trackColDir.GetListOfKeys() ])

    # Getting the modules common to both files
    allModules = a_modules.intersection(b_modules)

    if not madeDirectories:
        madeDirectories = True
        makeDirectories(allModules,allTrackCollections)
    
    # Loop over modules
    for thisModule in allModules:
        print "\tDoing Module",thisModule
        thisDir = "IDAlignMon/"+thisTrackCollection+"/"+thisModule
        a_modulesDir = a_file.GetDirectory(thisDir)
        b_modulesDir = b_file.GetDirectory(thisDir)

        # Get the list of histograms in this module
        a_hists = set([ k.GetName() for k in a_modulesDir.GetListOfKeys() ])
                
        b_hists = set([ k.GetName() for k in b_modulesDir.GetListOfKeys() ])
        
        # Getting the histograms common to both files
        allHists = a_hists.intersection(b_hists)
                
        for thisHist in allHists:
            a_thisHist = a_file.Get(thisDir+"/"+thisHist)

            # Skip empty hists
            if not a_thisHist:
                print "Warning ",thisHist," is NULL"
                print "Skipping it..."
                continue

            # Need to skip the metadata
            if isinstance(a_thisHist,TTree):
                #print "Skipping",thisHist
                continue

            thisCan = TCanvas(a_thisHist.GetTitle(),a_thisHist.GetTitle(),1100,800) 
            
            # print thisHist
            if isinstance(a_thisHist,TProfile):
                thisCan.cd()
                if thisHist in isAnException:
                    isAnException[thisHist][0](thisHist,thisDir+"/",isAnException[thisHist][1])
                else:
                    procProfile(thisHist,thisDir+"/")
            elif isinstance(a_thisHist,TH2):
                # For 2-D hist we write Out two canvases
                
                # Before
                beforeCan = TCanvas(a_thisHist.GetTitle()+" Before Alignment"
                                    ,a_thisHist.GetTitle()+" Before Alignment",1100,800)
                beforeCan.cd()
                procHitMap(thisHist,thisDir+"/",0)
                writeCan(beforeCan,thisHist,thisModule,thisTrackCollection)

                # After 
                afterCan = TCanvas(a_thisHist.GetTitle()+" After Alignment"
                                   ,a_thisHist.GetTitle()+" After Alignment",1100,800)
                afterCan.cd()
                procHitMap(thisHist,thisDir+"/",1)
                writeCan(afterCan,thisHist,thisModule,thisTrackCollection)

                # Dont write out the other canvases
                continue
                
            elif isinstance(a_thisHist,TH1):
                thisCan.cd()
                if thisHist in isAnException:
                    isAnException[thisHist][0](thisHist,thisDir+"/",isAnException[thisHist][1])
                else:
                    procHistogram(thisHist,thisDir+"/")
            else:
                print "Havent classified",thisHist
                print "It is of type:",type(a_thisHist)
                continue
                
            writeCan(thisCan,thisHist,thisModule,thisTrackCollection)


                
            
            
