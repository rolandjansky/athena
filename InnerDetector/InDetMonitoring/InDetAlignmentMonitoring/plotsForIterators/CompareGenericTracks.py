#===========================================================================
#===========================================================================
# Description:
#  This file compares the GenericTrack distributions, is called from makeComparision
#
# Author:
#  John Alison johnda@hep.upenn.edu
#===========================================================================

# Generic Tracks
#==============================


# Barrel and Endcaps
cGenTracks = []
genericTracksName = ["pTRes","pT","phi"]
canvasName = ["pT Resolution","pT Spectrum","Phi Distribution"]

for i in range(len(genericTracksName)):
    cGenTracks.append(TCanvas(canvasName[i],"Generic Tracks", 1100,800))

# Barrel 
cGenTracksBarrel = []
genericTracksBarrelName = ["Ntrthits_per_track_barrel","chi2oDoF_barrel","trk_pT_asym_barrel"]
canvasBarrelName = ["Number of TRT hits (Barrel)","Chi2 per DoF (Barrel)","pT Asymetry (Barrel)"]

for i in range(len(genericTracksBarrelName)):
    cGenTracksBarrel.append(TCanvas(canvasBarrelName[i],"Generic Tracks", 1100,800))
    
# EndcapA
cGenTracksEndcapA = []
genericTracksEndcapAName = ["Ntrthits_per_track_eca","chi2oDoF_eca","trk_pT_asym_eca"]
canvasEndcapAName = ["Number of TRT hits (Endcap A)","Chi2 per DoF (Endcap A)","pT Asymetry (Endcap A)"]

for i in range(len(genericTracksEndcapAName)):
    cGenTracksEndcapA.append(TCanvas(canvasEndcapAName[i],"Generic Tracks", 1100,800))

# EndcapC
cGenTracksEndcapC = []
genericTracksEndcapCName = ["Ntrthits_per_track_ecc","chi2oDoF_ecc","trk_pT_asym_ecc"]
canvasEndcapCName = ["Number of TRT hits (Endcap C)","Chi2 per DoF (Endcap C)","pT Asymetry (Endcap C)"]

for i in range(len(genericTracksEndcapCName)):
    cGenTracksEndcapC.append(TCanvas(canvasEndcapCName[i],"Generic Tracks", 1100,800))

cHitMapB = TCanvas("Hit Map Before Alignment","Hit Map Before Alignment",1100,800)
cHitMapA = TCanvas("Hit Map After Alignment","Hit Map After Alignment",1100,800)

cHitMapEndcapA_B = TCanvas("Hit Map Before Alignment (Endcap A)","Hit Map Before Alignment",1100,800)
cHitMapEndcapA_A = TCanvas("Hit Map After Alignment (Endcap A)","Hit Map After Alignment",1100,800)

cHitMapEndcapC_B = TCanvas("Hit Map Before Alignment (Endcap C)","Hit Map Before Alignment",1100,800)
cHitMapEndcapC_A = TCanvas("Hit Map After Alignment (Endcap C)","Hit Map After Alignment",1100,800)


for thisTrackSelection in trackSelections:
    # Both Barrel and Endcap
    # =====================
    for i in range(len(genericTracksName)):
        
        # Residuals Vs Phi
        cGenTracks[i].cd()
        procHistogram(genericTracksName[i],thisTrackSelection+"GenericTracks/",1)
                        
    dirName = thisTrackSelection.split('/')[1]+"BarrelGenericTracks"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.mkdir("GenericTracks",dirName).cd()

    for j in range(len(genericTracksName)):
        cGenTracks[j].Write()

    # Barrel
    # ================
    for i in range(len(genericTracksBarrelName)):
        
        # Residuals Vs Phi
        cGenTracksBarrel[i].cd()
        procHistogram(genericTracksBarrelName[i],thisTrackSelection+"GenericTracks/",1)

    cHitMapB.cd()
    procHitMap("hitMap_barrel",thisTrackSelection+"GenericTracks/",0)

    cHitMapA.cd()
    procHitMap("hitMap_barrel",thisTrackSelection+"GenericTracks/",1)


    dirName = thisTrackSelection.split('/')[1]+"BarrelGenericTracks"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("Barrel")
    gDirectory.mkdir("GenericTracks",dirName).cd()

    for j in range(len(genericTracksBarrelName)):
        cGenTracks[j].Write()

    cHitMapA.Write()
    cHitMapB.Write()
    

    # EndcapA
    # =====================
    for i in range(len(genericTracksEndcapAName)):
        
        cGenTracksEndcapA[i].cd()
        procHistogram(genericTracksEndcapAName[i],thisTrackSelection+"GenericTracks/",1)
    
    cHitMapEndcapA_B.cd()
    procHitMap("hitMap_endcapA",thisTrackSelection+"GenericTracks/",0)

    cHitMapEndcapA_A.cd()
    procHitMap("hitMap_endcapA",thisTrackSelection+"GenericTracks/",1)

    dirName = thisTrackSelection.split('/')[1]+"EndcapAGenericTracks"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("EndcapA")
    gDirectory.mkdir("GenericTracks",dirName).cd()

    for j in range(len(genericTracksEndcapAName)):
        cGenTracksEndcapA[j].Write()

    cHitMapEndcapA_B.Write()
    cHitMapEndcapA_A.Write()

    # EndcapC
    # =====================
    for i in range(len(genericTracksEndcapCName)):
        
        cGenTracksEndcapC[i].cd()
        procHistogram(genericTracksEndcapCName[i],thisTrackSelection+"GenericTracks/",1)

    cHitMapEndcapC_B.cd()
    procHitMap("hitMap_endcapC",thisTrackSelection+"GenericTracks/",0)

    cHitMapEndcapC_A.cd()
    procHitMap("hitMap_endcapC",thisTrackSelection+"GenericTracks/",1)

    dirName = thisTrackSelection.split('/')[1]+"EndcapCGenericTracks"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("EndcapC")
    gDirectory.mkdir("GenericTracks",dirName).cd()

    for j in range(len(genericTracksEndcapCName)):
        cGenTracksEndcapC[j].Write()

    cHitMapEndcapC_B.Write()
    cHitMapEndcapC_A.Write()
