#===========================================================================
#===========================================================================
# Description:
#  This file compares the Efficiency distributions, is called from makeComparision
#
# Author:
#  John Alison johnda@hep.upenn.edu
#===========================================================================

endcapName = ["Endcap_A","Endcap_C"]

# Ratio Hits Vs phi Sector
# Barrel 
cHitsPhiSec = TCanvas("Hits Vs PhiSector","Hits vs Phi Sec", 1100,800)
cHitsPhiSec.Divide(1,3)
hitsVsPhiSectorName = ["hits_eff_vs_phiSector_trt_b0","hits_eff_vs_phiSector_trt_b1","hits_eff_vs_phiSector_trt_b2"]

# Endcap
cHitsPhiSecEndcap = [TCanvas("Hits Vs PhiSector (Endcap A)","Hits vs Phi Sec", 1100,800)
                     ,TCanvas("Hits Vs PhiSector (Endcap C)","Hits vs Phi Sec", 1100,800)]

hitsVsPhiSectorEndcapName = "hits_eff_vs_phiSector_trt_ec_"

# Ratio tube Hits Vs phi Sector
# Barrel
cTubeHitsPhiSec = TCanvas("Tube Hits Vs PhiSector","Tube Hits vs Phi Sec", 1100,800)
cTubeHitsPhiSec.Divide(1,3)
tubeHitsVsPhiSectorName = ["tubeHits_eff_vs_phiSector_trt_b0","tubeHits_eff_vs_phiSector_trt_b1","tubeHits_eff_vs_phiSector_trt_b2"]

# Endcap
cTubeHitsPhiSecEndcap = [TCanvas("Tube Hits Vs PhiSector (Endcap A)","Tube Hits vs Phi Sec", 1100,800)
                         ,TCanvas("Tube Hits Vs PhiSector (Endcap C)","Tube Hits vs Phi Sec", 1100,800)]

tubeHitsVsPhiSectorEndcapName = "tubeHits_eff_vs_phiSector_trt_ec_"

# Ratio tube Hits Vs phi Sector
# Barrel
cOutliersPhiSec = TCanvas("Outliers Vs PhiSector","Outliers vs Phi Sec", 1100,800)
cOutliersPhiSec.Divide(1,3)
outliersVsPhiSectorName = ["outliers_eff_vs_phiSector_trt_b0","outliers_eff_vs_phiSector_trt_b1","outliers_eff_vs_phiSector_trt_b2"]

# Endcap 
cOutliersPhiSecEndcap = [TCanvas("Outliers Vs PhiSector (Endcap A)","Outliers vs Phi Sec", 1100,800)
                         ,TCanvas("Outliers Vs PhiSector (Endcap C)","Outliers vs Phi Sec", 1100,800)]
outliersVsPhiSectorEndcapName = "outliers_eff_vs_phiSector_trt_ec_"

# Ratio Hits Vs phi Sector
# Barrel
cTotalHitsPhiSec = TCanvas("Total Hits Vs PhiSector","TotalHits vs Phi Sec", 1100,800)
cTotalHitsPhiSec.Divide(1,3)
totalHitsVsPhiSectorName = ["hits_vs_phiSector_trt_b0","hits_vs_phiSector_trt_b1","hits_vs_phiSector_trt_b2"]

# Endcap
cTotalHitsPhiSecEndcap = [TCanvas("Total Hits Vs PhiSector (EndcapA)","TotalHits vs Phi Sec", 1100,800)
                          ,TCanvas("Total Hits Vs PhiSector (EndcapC)","TotalHits vs Phi Sec", 1100,800)]
totalHitsVsPhiSectorEndcapName = "hits_vs_phiSector_trt_ec_"

# Ratio tube TotalHits Vs phi Sector
# Barrel
cTotalTubeHitsPhiSec = TCanvas("Total Tube Hits Vs PhiSector","Total Tube Hits vs Phi Sec", 1100,800)
cTotalTubeHitsPhiSec.Divide(1,3)
totalTubeHitsVsPhiSectorName = ["tubeHits_vs_phiSector_trt_b0","tubeHits_vs_phiSector_trt_b1","tubeHits_vs_phiSector_trt_b2"]

# Endcap
cTotalTubeHitsPhiSecEndcap = [TCanvas("Total Tube Hits Vs PhiSector (EndcapA)","Total Tube Hits vs Phi Sec", 1100,800)
                              ,TCanvas("Total Tube Hits Vs PhiSector (EndcapC)","Total Tube Hits vs Phi Sec", 1100,800)]
totalTubeHitsVsPhiSectorEndcapName = "tubeHits_vs_phiSector_trt_ec_"

# Total Outliers Vs phi Sector
# Barrel
cTotalOutliersPhiSec = TCanvas("Total Outliers Vs PhiSector","Outliers vs Phi Sec", 1100,800)
cTotalOutliersPhiSec.Divide(1,3)
totalOutliersVsPhiSectorName = ["outliers_vs_phiSector_trt_b0","outliers_vs_phiSector_trt_b1","outliers_vs_phiSector_trt_b2"]

# Endcap 
cTotalOutliersPhiSecEndcap = [TCanvas("Total Outliers Vs PhiSector (EndcapA)","Outliers vs Phi Sec", 1100,800)
                               ,TCanvas("Total Outliers Vs PhiSector (EndcapC)","Outliers vs Phi Sec", 1100,800)]
totalOutliersVsPhiSectorEndcapName = "outliers_vs_phiSector_trt_ec_"

# Ratios Vs Straw Layer
cRatioStrawLayer = TCanvas("Ratios Vs Straw Layer","Ratios vs Straw Layer", 1100,800)
cRatioStrawLayer.Divide(1,3)
ratioNames = ["hits_eff_vs_StrawLayer_trt_b","tubeHits_eff_vs_StrawLayer_trt_b","outliers_eff_vs_StrawLayer_trt_b"]

# Ratios Vs Ring number
# Endcap
cRatioRingNumberEndcap = [TCanvas("Ratios Vs Ring Number (Endcap A)","Ratios Vs Ring Number",1100,800)
                          ,TCanvas("Ratios Vs Ring Number (Endcap C)","Ratios Vs Ring Number",1100,800)]
for i in range(2):
    cRatioRingNumberEndcap[i].Divide(1,3)

ratioRingNumberEndcapNames = ["hits_eff_vs_ring_trt_ec_","tubeHits_eff_vs_ring_trt_ec_","outliers_eff_vs_ring_trt_ec_"]

# Numbers Vs Ring number
# Endcap A
cNumberVsRingNumberEndcap = [TCanvas("Total number Vs Ring Number (Endcap A)","Number Vs Ring Number",1100,800)
                             ,TCanvas("Total number Vs Ring Number (Endcap C)","Number Vs Ring Number",1100,800)]
for i in range(2):
    cNumberVsRingNumberEndcap[i].Divide(1,3)

numberRingNumberEndcapNames = ["hits_vs_ring_trt_ec_","tubeHits_vs_ring_trt_ec_","outliers_vs_ring_trt_ec_"]

for thisTrackSelection in trackSelections:
    # Barrel
    for i in range(3):

        cHitsPhiSec.cd(i+1)
        procProfile(hitsVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")

        cTubeHitsPhiSec.cd(i+1)
        procProfile(tubeHitsVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")

        cOutliersPhiSec.cd(i+1)
        procProfile(outliersVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")

        cTotalHitsPhiSec.cd(i+1)
        procProfile(totalHitsVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")

        cTotalTubeHitsPhiSec.cd(i+1)
        procProfile(totalTubeHitsVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")

        cTotalOutliersPhiSec.cd(i+1)
        procProfile(totalOutliersVsPhiSectorName[i],thisTrackSelection+"HitEfficiencies/")
        
        cRatioStrawLayer.cd(i+1)
        procProfile(ratioNames[i],thisTrackSelection+"HitEfficiencies/")

    dirName = thisTrackSelection.split('/')[1]+"BarrelHitEfficiencies"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("Barrel")
    gDirectory.mkdir("HitEfficiencies",dirName).cd()
    
    cHitsPhiSec.Write()
    cTubeHitsPhiSec.Write()
    cOutliersPhiSec.Write()
    cTotalHitsPhiSec.Write()
    cTotalTubeHitsPhiSec.Write()
    cTotalOutliersPhiSec.Write()
    cRatioStrawLayer.Write()

    for endcap in range(2):
        cHitsPhiSecEndcap[endcap].cd()
        procProfile(hitsVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")

        cTubeHitsPhiSecEndcap[endcap].cd()
        procProfile(tubeHitsVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")
    
        cOutliersPhiSecEndcap[endcap].cd()
        procProfile(outliersVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")
    
        cTotalHitsPhiSecEndcap[endcap].cd()
        procProfile(totalHitsVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")
    
        cTotalTubeHitsPhiSecEndcap[endcap].cd()
        procProfile(totalTubeHitsVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")
    
        cTotalOutliersPhiSecEndcap[endcap].cd()
        procProfile(totalOutliersVsPhiSectorEndcapName+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")

        for i in range(3):
            cRatioRingNumberEndcap[endcap].cd(i+1)
            procProfile(ratioRingNumberEndcapNames[i]+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")
            
            cNumberVsRingNumberEndcap[endcap].cd(i+1)
            procProfile(numberRingNumberEndcapNames[i]+endcapName[endcap],thisTrackSelection+"HitEfficiencies/")

        if not endcap:
            dirName = thisTrackSelection.split('/')[1]+"EncapAHitEfficiencies"
            rootfile.cd(thisTrackSelection.split('/')[1])
            gDirectory.cd("EndcapA")
            gDirectory.mkdir("HitEfficiencies",dirName).cd()
        else:
            dirName = thisTrackSelection.split('/')[1]+"EncapCHitEfficiencies"
            rootfile.cd(thisTrackSelection.split('/')[1])
            gDirectory.cd("EndcapC")
            gDirectory.mkdir("HitEfficiencies",dirName).cd()



        cHitsPhiSecEndcap[endcap].Write()
        cTubeHitsPhiSecEndcap[endcap].Write()
        cOutliersPhiSecEndcap[endcap].Write()
        cTotalHitsPhiSecEndcap[endcap].Write()
        cTotalTubeHitsPhiSecEndcap[endcap].Write()
        cTotalOutliersPhiSecEndcap[endcap].Write()
        cRatioRingNumberEndcap[endcap].Write()
        cNumberVsRingNumberEndcap[endcap].Write()
    
