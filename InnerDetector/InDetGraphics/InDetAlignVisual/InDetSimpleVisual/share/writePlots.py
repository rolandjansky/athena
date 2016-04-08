# ================================
# Creates Writes out the Histograms
# Author John Alison <johnda@hep.upenn.edu>

def writeGeometryHists(name,hists):

    geo1 = []
    for i in range(len(hists)):
        geo1.append(hists[i].geo1)
    WriteHist("Geometry 1"+name,geo1
              ,drawEndcapLines=False,drawTRTFirst=False)
    geo2 = []
    for i in range(len(hists)):
        geo2.append(hists[i].geo2)
    WriteHist("Geometry 2"+name,geo2
              ,drawEndcapLines=True)

    DxyVsR = []
    for i in range(len(hists)):
        DxyVsR.append(hists[i].DxyVsR)
    WriteHist("Delta XY vs R"+name,DxyVsR)

    DxVsR = []
    for i in range(len(hists)):
        DxVsR.append(hists[i].DxVsR)
    WriteHist("Delta X vs R"+name,DxVsR)

    DxVsZ = []
    for i in range(len(hists)):
        DxVsZ.append(hists[i].DxVsZ)
    WriteHist("Delta X vs Z"+name,DxVsZ)

    DyVsR = []
    for i in range(len(hists)):
        DyVsR.append(hists[i].DyVsR)
    WriteHist("Delta Y vs R"+name,DyVsR)

    DyVsZ = []
    for i in range(len(hists)):
        DyVsZ.append(hists[i].DyVsZ)
    WriteHist("Delta Y vs Z"+name,DyVsZ)

    DphiVsZ = []
    for i in range(len(hists)):
        DphiVsZ.append(hists[i].DphiVsZ)
    WriteHist("Delta Phi vs Z"+name,DphiVsZ
              ,drawEndcapLines=True)

    DphiVsR = []
    for i in range(len(hists)):
        DphiVsR.append(hists[i].DphiVsR)
    WriteHist("Delta phi vs R"+name,DphiVsR)

    DrVsR = []
    for i in range(len(hists)):
        DrVsR.append(hists[i].DrVsR)
    WriteHist("Delta R vs R"+name,DrVsR)

    DzVsR = []
    for i in range(len(hists)):
        DzVsR.append(hists[i].DzVsR)
    WriteHist("Delta Z vs R"+name,DzVsR)


    DrVsPhi = []
    for i in range(len(hists)):
        DrVsPhi.append(hists[i].DrVsPhi)
    WriteHist("Delta R vs phi"+name,DrVsPhi)

    DxyVsZ = []
    for i in range(len(hists)):
        DxyVsZ.append(hists[i].DxyVsZ)
    WriteHist("Delta XY vs Z"+name,DxyVsZ)

    Dx = []
    for i in range(len(hists)):
        Dx.append(hists[i].Dx)
    WriteHist("Delta X"+name,Dx
              ,drawEndcapLines=False,drawTRTFirst=True)

    Dy = []
    for i in range(len(hists)):
        Dy.append(hists[i].Dy)
    WriteHist("Delta Y"+name,Dy
              ,drawEndcapLines=False,drawTRTFirst=True)

    Dz = []
    for i in range(len(hists)):
        Dz.append(hists[i].Dz)
    WriteHist("Delta Z"+name,Dz
              ,drawEndcapLines=False,drawTRTFirst=True)


writeGeometryHists("",[iblHists,pixelHists,sctHists,trtHists])

for thisMod in range(len(drawTRTModules)):
    thisLay = drawTRTModules[thisMod][0]
    thisPhi = drawTRTModules[thisMod][1]

    thisTRTHists = trtModuleHists[(thisLay),(thisPhi)]
    
    thisTRTDir = rootFile.mkdir("TRT Module Lay: "+str(thisLay)+" Phi: "+str(thisPhi))

    thisTRTDir.cd()
    writeGeometryHists(" Lay_"+str(thisLay)+"Phi_"+str(thisPhi),[thisTRTHists])
    rootFile.cd()

IDTransCan.Write()
