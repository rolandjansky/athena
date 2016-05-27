
#
nProcessedEvents = []
nRecoTracks = []
nAlignTracks = []
nIBLHits = []

# loop on all files and get entries from relevant histograms    
for i in range(nFiles):
    histoGram = GetHistogram(rootFiles[i],genericTracksDir[i],"LumiBlock",markerColors[i],markerStyles[i], i)
    nProcessedEvents.append(histoGram.GetEntries()) 

    histoGram = GetHistogram(rootFiles[i],genericTracksDir[i],"Nhits_per_track",markerColors[i],markerStyles[i], i)
    nRecoTracks.append(histoGram.GetEntries()) 

    histoGram = GetHistogram(rootFiles[i],genericTracksDir[i],"Nhits_per_track",markerColors[i],markerStyles[i], i)
    nAlignTracks.append(histoGram.GetEntries())
    
    histoGram = GetHistogram(rootFiles[i],residualsDir[i],"pix_b0_residualx",markerColors[i],markerStyles[i], i)
    nIBLHits.append(histoGram.GetEntries()) 
    
# print the results
print ""

for i in range(nFiles):
    print " File ", i, legendTitles[i], 
    print "    # Events: ", nProcessedEvents[i], "  # tracks: ", nRecoTracks[i], "  # AlignTracks: ", nAlignTracks[i],  "  # IBL hits: ", nIBLHits[i]
    
print ""
