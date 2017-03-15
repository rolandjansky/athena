#===========================================================================
#===========================================================================
# Description:
#  This file compares the Residual distributions, is called from makeComparision
#
# Author:
#  John Alison johnda@hep.upenn.edu
#===========================================================================

# Residual Window
#==============================
minResWin = -0.1
maxResWin = 0.1

sideNames = ["","Side_A","Side_C"]
endcapNames = ["Endcap_A","Endcap_C"]

# Fitted Residual
#==============================
cTRTFittedResidual = [TCanvas("Fitted Residual","TRT Residual", 1100,800),
                      TCanvas("Fitted Residual A-Side","TRT Residual A", 1100,800),
                      TCanvas("Fitted Residual C-Side","TRT Residual C", 1100,800)]
barrelResidualName = ["trt_b_residualR","trt_b_residualR"+sideNames[1],"trt_b_residualR"+sideNames[2]]

cTRTFittedResidualEndcap = [TCanvas("Fitted Residual Endcap A","TRT Residual Endcap A", 1100,800)
                            ,TCanvas("Fitted Residual Endcap C","TRT Residual Endcap C", 1100,800)]
endcapResidualName = "trt_ec_residualR_"

endcapARingRes = []
endcapCRingRes = []
for i in range(40):
    endcapARingRes.append(TCanvas("Fitted Residual A-Side Ring "+str(i),"TRT Residual A Ring "+str(i), 1100,800))
    endcapCRingRes.append(TCanvas("Fitted Residual C-Side Ring "+str(i),"TRT Residual C Ring "+str(i), 1100,800))
cTRTEndcapRingResiduals = [endcapARingRes,endcapCRingRes]

# Residual RMS Vs Phi Sector
#==============================

# Barrel
# Both A and C
# Residuals Vs phi Sector
cTRTResidualsPhiSec = [TCanvas("Residuals Vs PhiSector","TRT Residuals vs Phi Sec", 1100,800)
                       ,TCanvas("Residuals Vs PhiSector A-Side","TRT Residuals vs Phi Sec (A-side)", 1100,800)
                       ,TCanvas("Residuals Vs PhiSector C-Side","TRT Residuals vs Phi Sec (C-side)", 1100,800)]

for i in range(len(cTRTResidualsPhiSec)):
    cTRTResidualsPhiSec[i].Divide(1,3)

residualsVsPhiSectorName = ["trt_b_aveRes_l0","trt_b_aveRes_l1","trt_b_aveRes_l2"]


# RMS Vs phi Sector
cTRTResidualRMSPhiSec = [TCanvas("Residual RMS Vs PhiSector","TRT Residual RMS vs Phi Sec", 1100,800)
                         ,TCanvas("Residual RMS Vs PhiSector A-Side","TRT Residual RMS vs Phi Sec (A-side)", 1100,800)
                         ,TCanvas("Residual RMS Vs PhiSector C-Side","TRT Residual RMS vs Phi Sec (C-side)", 1100,800)]

for i in range(len(cTRTResidualRMSPhiSec)):
    cTRTResidualRMSPhiSec[i].Divide(1,3)

residualRMSVsPhiSectorName = ["trt_b_rmsRes_l0","trt_b_rmsRes_l1","trt_b_rmsRes_l2"]

#Endcap 
# Res Vs phi Sector
cTRTResidualsVSPhiSecEndcap = [TCanvas("Residuals Vs PhiSector Endcap A","TRT Residuals vs Phi Sec Endcap A", 1100,800)
                               ,TCanvas("Residuals Vs PhiSector Endcap C","TRT Residuals vs Phi Sec Endcap C", 1100,800)]

residualsVsPhiSectorEndcapName = "trt_ec_aveResVsPhiSec_"

# RMS Vs phi Sector
cTRTResidualRMSPhiSecEndcap = [TCanvas("Residual RMS Vs PhiSector Endcap A","TRT Residual RMS vs Phi Sec Endcap A", 1100,800)
                                ,TCanvas("Residual RMS Vs PhiSector Endcap C","TRT Residual RMS vs Phi Sec Endcap C", 1100,800)]
residualRMSVsPhiSectorEndcapName = "trt_ec_rmsResVsPhiSec_"

#Endcap 
# Res Vs ring
cTRTResidualsVSRingEndcap = [TCanvas("Residuals Vs Ring Endcap A","TRT Residuals vs Ring Endcap A", 1100,800)
                               ,TCanvas("Residuals Vs Ring Endcap C","TRT Residuals vs Ring Endcap C", 1100,800)]

residualsVsRingEndcapName = "trt_ec_aveResVsRing_"

# RMS Vs ring
cTRTResidualRMSRingEndcap = [TCanvas("Residual RMS Vs Ring Endcap A","TRT Residual RMS vs Ring Endcap A", 1100,800)
                                ,TCanvas("Residual RMS Vs Ring Endcap C","TRT Residual RMS vs Ring Endcap C", 1100,800)]
residualRMSVsRingEndcapName = "trt_ec_rmsResVsRing_"


# Residual Vs Straw Layer
#==============================
cTRTResidualsStrawLayer = []
canvasName = ["Residuals Vs Straw Layer","Residuals Vs StrawLayer (Upper)","Residuals Vs StrawLayer (Lower)","Residuals Vs StrawLayer (Right)","Residuals Vs StrawLayer (Left)"]

for i in range(5):
    cTRTResidualsStrawLayer.append(TCanvas(canvasName[i],"TRT Residuals vs StrawLayer", 1100,800))
    cTRTResidualsStrawLayer[i].Divide(3,2)
  
posName = [""," Upper"," Lower"," Left"," Right"]
residualsVsStrawLayerName = "trt_b_aveResOverPhiVsStrawLayer"

cTRTResidualsStrawLayerStackLevel = []
residualsVsStrawLayerStackLevelName = []
residualRMSVsStrawLayerStackLevelName = []

for i in range(32):
    residualsVsStrawLayerStackLevelName.append("trt_b_aveResVsStrawLayerStackLevel_"+str(i))
    residualRMSVsStrawLayerStackLevelName.append("trt_b_rmsResVsStrawLayerStackLevel_"+str(i))

    cTRTResidualsStrawLayerStackLevel.append(TCanvas("Residuals Vs Straw Layer Phi Sector "+str(i),"TRT Residual Vs Straw Later Phi Sector "+str(i),1100,800))
    cTRTResidualsStrawLayerStackLevel[i].Divide(3,2)


# RMS  Vs Straw Layer
#=========================
residualRMSVsStrawLayerName = "trt_b_rmsResOverPhiVsStrawLayer"

#  L/R 
#=========================
cLRBreakDownName = TCanvas("hitTypeBreakDown","Hit Type Break Down",1100,800)
lRBreakDownName = "trt_b_lr"

cLRBreakDownNameEndcap = [TCanvas("Hit Type Break Down (EndcapA)","Hit Type Break Down",1100,800)
                          ,TCanvas("Hit Type Break Down (EndcapC)","Hit Type Break Down",1100,800)]

lRBreakDownNameEndcap = "trt_ec_lr_"

# rT relation
#=========================
cRTRelation = [TCanvas("rT Relation (Before Alignment)","Rt relation (Before Alignment)",1100,800),TCanvas("rT Relation (After Alignment)","Rt relation (After Alignment)",1100,800)]
rTRelationName = "trt_b_rt"

cRTRelationEndcap = [[TCanvas("rT Relation (EndcapA) Before Alignment","Rt relation (Before Alignment)",1100,800),
                      TCanvas("rT Relation (EndcapA) After Alignment","Rt relation (After Alignment)",1100,800)],
                     [TCanvas("rT Relation (EndcapC) Before Alignment","Rt relation (Before Alignment)",1100,800),
                      TCanvas("rT Relation (EndcapC) After Alignment","Rt relation (After Alignment)",1100,800)]
                     ]
    
rTRelationNameEndcap = "trt_ec_rt_"

for thisTrackSelection in trackSelections:
    for side in range(len(sideNames)):

        cTRTFittedResidual[side].cd()
        cTRTFittedResidual[side].SetLogy(1)
        processAndFitTRTResiduals(barrelResidualName[side],thisTrackSelection+"Residuals/")
        
        for lay in range(3):
            # Residuals Vs Phi
            cTRTResidualsPhiSec[side].cd(lay+1)
            procProfile(residualsVsPhiSectorName[lay]+sideNames[side],thisTrackSelection+"Residuals/",minResWin,maxResWin)
        
            # RMS Vs Phi
            cTRTResidualRMSPhiSec[side].cd(lay+1)
            procProfile(residualRMSVsPhiSectorName[lay]+sideNames[side],thisTrackSelection+"Residuals/")
        
        for region in range(5):
            # Residuals Vs Straw Layer
            # =========================
            cTRTResidualsStrawLayer[region].cd(side+1)
            procProfile(residualsVsStrawLayerName+sideNames[side]+posName[region],thisTrackSelection+"Residuals/")
            
            # Residual RMS Vs Straw Layer
            # ============================
            cTRTResidualsStrawLayer[region].cd(side+4)
            procProfile(residualRMSVsStrawLayerName+sideNames[side]+posName[region],thisTrackSelection+"Residuals/")
            
                
        for phi in range(32):
            cTRTResidualsStrawLayerStackLevel[phi].cd(2*side+1)
            procProfile(residualsVsStrawLayerStackLevelName[phi]+"_"+sideNames[side],thisTrackSelection+"Residuals/",minResWin,maxResWin)
            cTRTResidualsStrawLayerStackLevel[phi].cd(2*side+2)
            procProfile(residualRMSVsStrawLayerStackLevelName[phi]+"_"+sideNames[side],thisTrackSelection+"Residuals/")


    cLRBreakDownName.cd()
    cLRBreakDownName.SetLogy(1)
    procHistogram(lRBreakDownName, thisTrackSelection+"Residuals/")

    cRTRelation[0].cd()
    procHitMap(rTRelationName, thisTrackSelection+"Residuals/",0)

    cRTRelation[1].cd()
    procHitMap(rTRelationName, thisTrackSelection+"Residuals/",1)

    for endcap in range(2):

        cTRTFittedResidualEndcap[endcap].cd()
        cTRTFittedResidualEndcap[endcap].SetLogy(1)
        processAndFitTRTResiduals(endcapResidualName+endcapNames[endcap],thisTrackSelection+"Residuals/","#mum")
        
        cTRTResidualsVSPhiSecEndcap[endcap].cd()
        procProfile(residualsVsPhiSectorEndcapName+endcapNames[endcap],thisTrackSelection+"Residuals/")
        
        cTRTResidualRMSPhiSecEndcap[endcap].cd()
        procProfile(residualRMSVsPhiSectorEndcapName+endcapNames[endcap],thisTrackSelection+"Residuals/")

        cTRTResidualsVSRingEndcap[endcap].cd()
        procProfile(residualsVsRingEndcapName+endcapNames[endcap],thisTrackSelection+"Residuals/")
        
        cTRTResidualRMSRingEndcap[endcap].cd()
        procProfile(residualRMSVsRingEndcapName+endcapNames[endcap],thisTrackSelection+"Residuals/")
        
        cLRBreakDownNameEndcap[endcap].cd()
        cLRBreakDownNameEndcap[endcap].SetLogy(1)
        procHistogram(lRBreakDownNameEndcap+endcapNames[endcap], thisTrackSelection+"Residuals/")
        
        cRTRelationEndcap[endcap][0].cd()
        procHitMap(rTRelationNameEndcap+endcapNames[endcap], thisTrackSelection+"Residuals/",0)
        
        cRTRelationEndcap[endcap][1].cd()
        procHitMap(rTRelationNameEndcap+endcapNames[endcap], thisTrackSelection+"Residuals/",1)

        for ring in range(40):
            cTRTEndcapRingResiduals[endcap][ring].cd()
            processAndFitTRTResiduals(endcapResidualName+"ring_"+str(ring)+"_"+endcapNames[endcap],thisTrackSelection+"Residuals/","#mum")
        
    #Write out the histograms

    # Barrel 
    dirName = thisTrackSelection.split('/')[1]+"BarrelResiduals"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("Barrel")
    gDirectory.mkdir("Residuals",dirName).cd()

    for side in range(len(sideNames)):
        cTRTResidualsPhiSec[side].Write()
        cTRTResidualRMSPhiSec[side].Write()

    for i in range(3):
        cTRTFittedResidual[i].Write()

    for i in range(5):
        cTRTResidualsStrawLayer[i].Write()
        
    for i in range(32):
        cTRTResidualsStrawLayerStackLevel[i].Write()

    cLRBreakDownName.Write()

    cRTRelation[0].Write()
    cRTRelation[1].Write()
    
    # Endcaps

    for endcap in range(2):
        if not endcap:
            dirName = thisTrackSelection.split('/')[1]+"EndcapAResiduals"
            rootfile.cd(thisTrackSelection.split('/')[1])
            gDirectory.cd("EndcapA")
            gDirectory.mkdir("Residuals",dirName).cd()
        else:
            dirName = thisTrackSelection.split('/')[1]+"EndcapCResiduals"
            rootfile.cd(thisTrackSelection.split('/')[1])
            gDirectory.cd("EndcapC")
            gDirectory.mkdir("Residuals",dirName).cd()

                        
        cTRTFittedResidualEndcap[endcap].Write()
        for ring in range(40):
            cTRTEndcapRingResiduals[endcap][ring].Write()

        cTRTResidualsVSPhiSecEndcap[endcap].Write()
        cTRTResidualRMSPhiSecEndcap[endcap].Write()

        cTRTResidualsVSRingEndcap[endcap].Write()
        cTRTResidualRMSRingEndcap[endcap].Write()

        cLRBreakDownNameEndcap[endcap].Write()

        cRTRelationEndcap[endcap][0].Write()
        cRTRelationEndcap[endcap][1].Write()

