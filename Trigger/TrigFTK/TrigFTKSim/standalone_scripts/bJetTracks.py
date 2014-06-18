# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TChain,TH1F,TFile,TH2F,TLorentzVector,gSystem,TCanvas

import ROOT
import math
import sys, os
import glob
import AtlasStyle
from NtupleHelpers import Track, TrackHists,TrackCompHists
#ROOT.gROOT.ProcessLine('.L loader.C+')

#
#  Load offline Tracking ntuple
# 

trkD3PD= TChain("InDetTrackTree","");
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00001.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00002.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00003.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00004.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00005.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00006.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00007.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00008.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00009.InDetTrackD3PD.root")
trkD3PD.Add("/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100wD3PD.130130013729/user.johnda.002205.EXT0._00010.InDetTrackD3PD.root")
nD3PDEvents = trkD3PD.GetEntries()
print "We have ",nD3PDEvents,"D3PD events"
#
#  End loading offline Tracking ntuple
# 


#
# Loading FTK Stuff
#

gSystem.Load("../standalone/libftk_classes.so");
import ftkutils

try :
    import cftkutils
    print "cftkutils module found."
except ImportError as errmsg:
    print errmsg
    print "To enable the C++ extension try to compile it with \"make\". The Makefile could need adjustements\nThe TrigFTKSim/standalone directory has also do be added in the LD_LIBRARY_PATH."
    sys.exit(-1)


def NormalizeOutputs(cross=True) :
    """This function compares the ftkouts content, extracting the files used and remove from the list the files that don't match in the different FTKTree. After the cleanup the FTKTree content is rebuilt"""
    global ftkouts
    global ftkinputs

    def make_baselist(flist, nskip) :
        newlist = []
        for f in flist :
            newlist.append(os.path.basename(f)[nskip:])
        return newlist

    def common_baselist(reflist,chklist) :
        final = []
        
        for refelem in reflist :
            if refelem in chklist :
                final.append(refelem)
        return final

    def wrapperoutname(path) :
        bsname = os.path.basename(path)
        tokens = bsname.split('.')
        #print bsname, tokens
        if len(tokens)<0 :
            raise ValueError("Wrong wrapper file name")
        
        if tokens[-1]=="dat" :
            outname = ".".join(tokens[:-1])
        elif tokens[-2]=="dat" and tokens[-1]=="bz2" :
            outname = ".".join(tokens[:-2])
        else :
            print "Invalid input file extension in:", path
            outname = "none"
        return outname+".root"
    
    print "Normalize internal lists of the files"
    
    # check the internal consistency of the lists and extract the base list
    for out in ftkouts :
        cmnbaselist = None # initilize the common list in the output tree
        
        # check unmerged roads
        for lst in out.unmergedroads_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,9)
            else :
                chkbaselist = make_baselist(lst,9)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check unmerged tracks
        for lst in out.unmergedtracks_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,10)
            else :
                chkbaselist = make_baselist(lst,10)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check merged roads
        for lst in out.mergedroads_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,9)
            else :
                chkbaselist = make_baselist(lst,9)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check merged tracks
        for lst in out.mergedtracks_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,10)
            else :
                chkbaselist = make_baselist(lst,10)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # if an ftkinputs object exists a match with its content is done and the cmbbaselist is intersected
        if ftkinputs :
            cmninlist = []            
            for fn in ftkinputs.filelist :
                # the expected output name for this wrapper file is calculated
                ofn = wrapperoutname(fn)
                if ofn in cmnbaselist :
                    cmninlist.append(fn)
            print "Wrapper files matched %d (%d)" % (len(ftkinputs.filelist),len(cmninlist))
            out.inputfiles = cmninlist

            # synchronize the two lists
            synclist = []
            for fn in cmninlist :
                ofn = wrapperoutname(fn)
                if ofn in cmnbaselist :
                    synclist.append(ofn)
            # apply a new baselist that is compatible with the wrapper files and in sync
            cmnbaselist = synclist
            # store the list of matched input files in the  object
        print "In", os.path.basename(out.outdir), len(cmnbaselist), "files"
                
        # reduce the list of files only to files common in all the outputs
        out.outputfiles = cmnbaselist
        
    if len(ftkouts)<2 or not cross:
        return 0

    # select the reference output, the first
    cmnoutlist = ftkouts[0].outputfile
    nstart = len(cmnlist)

    print "Normalize among lists"
    for outchk in ftkouts[1:] : # output look
        cmnoutlist = common_baselist(cmnoutlist,out.outputfiles)
    # end output loop

    if ftkinputs :
        cmninlist = []            
        for fn in ftkinputs.filelist :
            # the expected output name for this wrapper file is calculated
            ofn = wrapperoutname(fn)
            if ofn in cmnbaselist :
                cmninlist.append(fn)
        for out in ftkouts :
            out.inputfiles = cmninlist

        # synchronize the two lists
        synclist = []
        for fn in cmninlist :
            ofn = wrapperoutname(fn)
            if ofn in cmnoutlist :
                synclist.append(ofn)
        # apply a new baselist that is compatible with the wrapper files and in sync
        cmnoutlist = synclist

    ndiff = nstart-len(cmnlist)
    print "Common number of files:", len(cmnoutlist), "(%d removed)" % ndiff
    # set the single lists to the new common values
    if ndiff>0 :
        for out in ftkouts :
            out.outputfiles = cmnoutlist
    return ndiff



#
#   Load the FTK ntuples
#
nRegions = 64
nSubRegions = 4
nLayers = 8

# Load the FTK sim output
ftkouts = []
FTKSimDir = "/share/t3data2/johnda/samples/testPrivProd/output/raw_8Lc_15x16x36_20121221mc10ttbar50_np30000000"
out = cftkutils.FTKTree(0,FTKSimDir,64,nSubRegions,nLayers)
ftkouts.append(out)

# Load the FTK Wrapped files
wrapperFilesDir = "/share/t3data2/johnda/samples/testPrivProd/user.johnda.mc12_14TeV.FTK.161805.WH125_munubb_Pythia8.py.recon.MWT2Test100.130118060049/"
print "Take input from directory:", wrapperFilesDir
files_input = glob.glob(wrapperFilesDir+"/*.dat*")
files_input.sort()
ftkinputs = ftkutils.FTKFileChain(files_input)
NormalizeOutputs(False) # the cross synchronization is not done automatically
for out in ftkouts :
    out.Connect()
print "THe LOOP"

#
# End loading FTK Stuff
#


#
#  Helper function for the matching
#

def matchTrkEvent(runNum,eventNum):
    for event in trkD3PD:
        if runNum == event.ei_RunNumber and eventNum == event.ei_EventNumber:
            return event

    print "ERROR: No match"
    return 0


def matchTrk(track_ftk,event):
    nOfflineTrks = event.trk_n
    closestDr = 3.0
    closestTrk = 0
    for itrk in range(nOfflineTrks):

        thisTrk = Track(event.trk_eta[itrk],
                        event.trk_phi_wrtBS[itrk],
                        event.trk_pt[itrk],
                        event.trk_d0_wrtBS[itrk],
                        event.trk_z0_wrtBS[itrk])

        thisDr = track_ftk.fourVec.DeltaR(thisTrk.fourVec)
        if thisDr < closestDr:
            closestDr = thisDr
            closestTrk = thisTrk
    
    return closestTrk


#
#  the ACtual logic.
#

# Make output
outFile = TFile("bJetTracks.root", "recreate")
outFile.cd()
hFTK          = TrackHists("FTK")
hFTKvsOffline = TrackCompHists("FTKvsOffline")

# Not sure what this loop is ...
for outid, out in enumerate(ftkouts) : # output stuctures loop
    
    # skip disabled FTKTree
    if not out.isEnabled:
        continue

    # check if input files were set
    if not out.inputfiles :
        print "No input files for", os.path.basename(out.outdir)
        continue

    # prepare the object to loop on the events
    fchain = ftkutils.FTKFileChain(out.inputfiles)

    
    eventcount = 0
    maxEvents = 2 # only do 2 events for now (too slow)

    #
    # Loop on FTK evetns
    #
    while fchain.nextevent() :
        if(eventcount >= maxEvents):
            print "Event max reached"
            break

        eventcount+=1 # increment the event counter
        print "event number:",eventcount

        # synchronize the FTK output structure
        out.GetEntry(eventcount-1) # the event counter has a shift

        runNum = fchain.FTKEvent.run
        eventNum = fchain.FTKEvent.event

        # MAtch this FTK event to the one in offline track ntuple
        print "matching events:",
        offlineTracks = matchTrkEvent(runNum,eventNum)
        # Note offlineTracks is LARGE, should try to aviod copying it
        print ".......Done:"
        
        if offlineTracks:
            #print "NFTK Tracks",out.GetNUnmergedTracks() 
            print "getting FTK tracks:",
            FTKTracks = out.GetUnmergedTracks() # Get list of FTK tracks

            # get flat list of tracks not nested
            AllFTKTracks = []
            for subRegs in range(nRegions*nSubRegions):
                if not FTKTracks[subRegs]:
                    continue
                for trk in range(len(FTKTracks[subRegs])):
                    AllFTKTracks.append(FTKTracks[subRegs][trk])

            print ".......Done:"

            # Loop over FTK tracks
            for trkIndex, trkParam in enumerate(AllFTKTracks):
                track_ftk = Track(eta = trkParam[0],
                                  phi = trkParam[1],                 
                                  pt  = trkParam[3],
                                  d0  = trkParam[2],
                                  z0  = trkParam[4] )

                hFTK.Fill(track_ftk)

                # Match to offline track
                print "matching offline Track...",
                track_offline = None#matchTrk(track_ftk,offlineTracks) 
                # Probably very time consuming
                print ".......Done:"

                # Plot.
                if track_offline:
                    hFTKvsOffline.Fill(track_ftk,track_offline)

                                

    print "Done FTK Event Loop"

print "Writing histograms"

hFTK.Write()
hFTKvsOffline.Write()
outFile.Close()
print "Done"
