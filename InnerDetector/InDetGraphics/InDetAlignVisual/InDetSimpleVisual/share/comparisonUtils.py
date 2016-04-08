# Functions needed by CompareGeometries
# Author John Alison <johnda@hep.upenn.edu>

# Function to fill the Barrel Histograms
def fillBarrelHists(x1,y1,z1
                    ,x2,y2,z2
                    ,theHists
                    ):

    #wkd2
    #print x1,y1,z1,x2,y2,z2

    # Get the nicknames for the hists
    h_geo1   = theHists.geo1[1]  
    h_geo2   = theHists.geo2[1]    
    h_XvsR   = theHists.DxVsR[1]    
    h_XvsZ   = theHists.DxVsZ[1]    
    h_YvsR   = theHists.DyVsR[1]    
    h_YvsZ   = theHists.DyVsZ[1]    
    h_PhivsZ = theHists.DphiVsZ[1]  
    h_XYvsR  = theHists.DxyVsR[1]   
    h_PhivsR = theHists.DphiVsR[1]  
    h_RvsR   = theHists.DrVsR[1]    
    h_ZvsR   = theHists.DzVsR[1]    
    h_XYvsZ  = theHists.DxyVsZ[1]   
    h_RvsPhi = theHists.DrVsPhi[1]  
    h_X      = theHists.Dx[1]       
    h_Y      = theHists.Dy[1]      
    h_Z      = theHists.Dz[1]       

    h_geo1.Fill(x1,y1)
    h_geo2.Fill(x2,y2)

    h_XvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),x1 - x2)
    h_XvsZ.Fill(z2,x1 - x2)

    h_YvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),y1 - y2)
    h_YvsZ.Fill(z2,y1 - y2)

    h_PhivsZ.Fill(z2,atan2(y1,x1)-atan2(y2,x2))

    h_XYvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),sqrt(pow(x1 - x2,2)+pow(y1 - y2 ,2)))

    #wkd2 - div by zero check
    if not sqrt(x1*x1+y1*y1) == 0:
        h_PhivsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),1.0/sqrt(x1*x1+y1*y1)*((x1 - x2)*y1 - (y1 - y2)* x1))

        h_RvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)), (1.0/sqrt(x1*x1+y1*y1)*((x1 - x2)*x1 + (y1 - y2)* y1)))

    h_X.Fill(x1 - x2)
    h_Y.Fill(y1 - y2)
    h_Z.Fill(z1 - z2)

    if z1 > 0:
        h_ZvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),sqrt(pow(z1 - z2,2)))
        
        h_XYvsZ.Fill(z2,sqrt(pow(x1 - x2,2)+pow(y1 - y2 ,2)))
        
        if x2:
            h_RvsPhi.Fill(atan2(y2,x2),sqrt(pow(x1,2)+pow(y1,2)) -sqrt(pow(x2,2)+pow(y2,2)))        

    return

# Function to fill the Endcap Histograms
def fillEndcapHists(x1,y1,z1
                    ,x2,y2,z2
                    ,theHists
                    ,side
                    ):

#---wkd2
#    print x1, y1, z1, x2, y2, z2
#wkd2---

    h_geo1   = theHists.geo1[side]
    h_geo2   = theHists.geo2[side]
    h_XvsR   = theHists.DxVsR[side]
    h_XvsZ   = theHists.DxVsZ[side]  
    h_YvsR   = theHists.DyVsR[side]  
    h_YvsZ   = theHists.DyVsZ[side]  
    h_PhivsZ = theHists.DphiVsZ[side]
    h_XYvsR  = theHists.DxyVsR[side]
    h_PhivsR = theHists.DphiVsR[side]
    h_RvsR   = theHists.DrVsR[side]  
    h_ZvsR   = theHists.DzVsR[side]    
    h_XYvsZ  = theHists.DxyVsZ[side]   
    h_RvsPhi = theHists.DrVsPhi[side]  
    h_X      = theHists.Dx[side]       
    h_Y      = theHists.Dy[side]      
    h_Z      = theHists.Dz[side]       

    h_geo1.Fill(z1,atan2(y1,x1))
    h_geo2.Fill(z2,atan2(y2,x2))
    
    h_XvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),x1 - x2)
    h_XvsZ.Fill(z2,x1 - x2)

    h_YvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),y1 - y2)
    h_YvsZ.Fill(z2,y1 - y2)
 
    h_PhivsZ.Fill(z2,atan2(y1,x1)-atan2(y2,x2))
    
    h_XYvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),sqrt(pow(x1 - x2,2)+pow(y1 - y2 ,2)))

    #wkd2 - div by 0 check
    if not sqrt(x1*x1+y1*y1) == 0:
        h_PhivsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),1.0/sqrt(x1*x1+y1*y1)*((x1 - x2)*y1 - (y1 - y2)* x1))
    
        h_RvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)), (1.0/sqrt(x1*x1+y1*y1)*((x1 - x2)*x1 + (y1 - y2)* y1)))

    h_ZvsR.Fill(sqrt(pow(x2,2) + pow(y2,2)),sqrt(pow(z1 - z2,2)))
    
    h_XYvsZ.Fill(z2,sqrt(pow(x1 - x2,2)+pow(y1 - y2 ,2)))


    h_X.Fill(x1 - x2)
    h_Y.Fill(y1 - y2)
    h_Z.Fill(z1 - z2)

    if x2:
        h_RvsPhi.Fill(atan2(y2,x2),sqrt(pow(x1,2)+pow(y1,2)) -sqrt(pow(x2,2)+pow(y2,2)))

    return

# Function to make the geometry histograms
def makeSubSysGeometryHist(name,title,color):
    
    h_Ecc = TH2F(name+"_Ecc",title+" (Endcap C)",2400,-2900,-500,2400,-3.14,3.14)
    h_Ecc.SetMarkerColor(color+1)
    h_Ecc.GetXaxis().SetTitle("z [mm]")
    h_Ecc.GetYaxis().SetTitle("phi")

    h_b = TH2F(name,title,2400,-1200,1200,2400,-1200,1200)
    h_b.SetMarkerColor(color+1)
    h_b.GetXaxis().SetTitle("x [mm]")
    h_b.GetYaxis().SetTitle("y [mm]")

    h_Eca = TH2F(name+"_Eca",title+" (Endcap A)",2400,400,2900,2400,-3.14,3.14)
    h_Eca.SetMarkerColor(color+1)
    h_Eca.GetXaxis().SetTitle("z [mm]")
    h_Eca.GetYaxis().SetTitle("phi")

    return h_Ecc,h_b,h_Eca,

# Make the difference hists for a particular sub system
def makeSubSysDiffHist(name,title,xName,xMins,xMaxes,yName,yRange,color,nBins = 120):


    h_Ecc =  TH2F(name+"_Ecc",title+" (Endcap C)", nBins,xMins[0],xMaxes[0],nBins,-1*yRange,yRange)
    h_Ecc.SetMarkerColor(color+1)
    h_Ecc.GetXaxis().SetTitle(xName)
    h_Ecc.GetYaxis().SetTitle(yName)

    h_b =  TH2F(name,title,nBins,xMins[1],xMaxes[1],nBins,-1*yRange,yRange)
    h_b.SetMarkerColor(color+1)
    h_b.GetXaxis().SetTitle(xName)
    h_b.GetYaxis().SetTitle(yName)

    h_Eca =  TH2F(name+"_Eca",title+" (Endcap A)", nBins,xMins[2],xMaxes[2],nBins,-1*yRange,yRange)
    h_Eca.SetMarkerColor(color+1)
    h_Eca.GetXaxis().SetTitle(xName)
    h_Eca.GetYaxis().SetTitle(yName)

    return h_Ecc,h_b,h_Eca

def make1D_SubSysDiffHist(name,title,xName,xMins,xMaxes,yName,color):
    nBins = 120
    h_Ecc =  TH1F(name+"_Ecc",title+" (Endcap C)", nBins,xMins[0],xMaxes[0])
    h_Ecc.SetMarkerColor(color+1)
    h_Ecc.SetLineColor(color+1)
    h_Ecc.GetXaxis().SetTitle(xName)
    h_Ecc.GetYaxis().SetTitle(yName)

    h_b =  TH1F(name,title,nBins,xMins[1],xMaxes[1])
    h_b.SetMarkerColor(color+1)
    h_b.SetLineColor(color+1)
    h_b.GetXaxis().SetTitle(xName)
    h_b.GetYaxis().SetTitle(yName)

    h_Eca =  TH1F(name+"_Eca",title+" (Endcap A)", nBins,xMins[2],xMaxes[2])
    h_Eca.SetMarkerColor(color+1)
    h_Eca.SetLineColor(color+1)
    h_Eca.GetXaxis().SetTitle(xName)
    h_Eca.GetYaxis().SetTitle(yName)

    return h_Ecc,h_b,h_Eca

# Function to write out all the histograms
def WriteHist(title
              ,hists
              ,drawEndcapLines=False
              ,drawTRTFirst=False):
    
    can =  TCanvas(title,title, 700, 500)
    can.Divide(3)

    can.cd(1)
    for i in range(len(hists)):
        if i:
            hists[i][0].Draw("same")
        else:
            hists[i][0].Draw("")
    
    
    if drawEndcapLines:
        lineLength = 0.0008
        endcapLinesC_ForGeo = DrawTRTEndcapCLines(lineLength)


    can.cd(2)
    for i in range(len(hists)):
        if i:
            hists[i][1].Draw("same")
        else:
            hists[i][1].Draw("")

    can.cd(3)
    for i in range(len(hists)):
        if i:
            hists[i][2].Draw("same")
        else:
            hists[i][2].Draw("")

    
    if drawEndcapLines:
        lineLength = 0.0008
        endcapLinesA_ForGeo = DrawTRTEndcapALines(lineLength)

    can.Write()

    return

# Function to read in the data from the input text files
def readInData(inputfile):
    m_iblElements = [] # IBL separate because it has more eta modules than the PIX.
    m_pixelElements = []
    m_sctElements = []
    m_trtElements = []
    for line in inputfile:
        words = line.split()
        
        m_thisPosition = []
        for i in range(len(words)-1):
            m_thisPosition.append(float(words[i+1]))
    
        # Pixel
        if words[0] == "1":
            if words[1] == "4" or words[1] == "-4": 
                # DBM, we're gonna lump it in with the IBL
                m_iblElements.append(m_thisPosition)

            elif words[1] == "0" and words[2] == "0": # IBL
                m_iblElements.append(m_thisPosition)

            else:
                m_pixelElements.append(m_thisPosition)
            
        # SCT
        if words[0] == "2":
            m_sctElements.append(m_thisPosition)

        # TRT
        if words[0] == "3":
            m_trtElements.append(m_thisPosition)

    return m_iblElements,m_pixelElements,m_sctElements,m_trtElements


# Functions to draw lines on the TRT endcap plots
def DrawTRTEndcapCLines(yRange):

    #B Wheels
    bLines =[]
    for i in range(9):
        bWheels = -2710 + 122*i
        bLines.append(TLine(bWheels,-1*yRange,bWheels,yRange))
        bLines[i].SetLineStyle(2)
        bLines[i].Draw()

    seperator = TLine(-1725,-1*yRange,-1725,yRange)
    seperator.SetLineColor(kRed)
    seperator.Draw()

    #A Wheels
    aLines = []
    do16plane = False
    if do16plane:
        for i in range(7):
            # For the 16-plane wheel
            aWheels = -1705 + 142 *i;
            aLines.append(TLine(aWheels,-1*yRange,aWheels,yRange))
            aLines[i].SetLineStyle(2)
            aLines[i].Draw()
    else:
        for i in range(13):
            # For the 8-plane wheel
            aWheels = -1705 + 71 *i;
            aLines.append(TLine(aWheels,-1*yRange,aWheels,yRange))
            aLines[i].SetLineStyle(2)
            aLines[i].Draw()
        
    return bLines, seperator, aLines

def DrawTRTEndcapALines(yRange):
    #B Wheels
    bLines =[]
    for i in range(9):
        bWheels = 2710 - 122*i
        bLines.append(TLine(bWheels,-1*yRange,bWheels,yRange))
        bLines[i].SetLineStyle(2)
        bLines[i].Draw()

    seperator = TLine(1725,-1*yRange,1725,yRange)
    seperator.SetLineColor(kRed)
    seperator.Draw()

    #A Wheels
    aLines = []
    do16plane = False
    if do16plane:
        for i in range(7):
            aWheels = 1705 - 142 *i;
            aLines.append(TLine(aWheels,-1*yRange,aWheels,yRange))
            aLines[i].SetLineStyle(2)
            aLines[i].Draw()
    else:
        for i in range(13):
            aWheels = 1705 - 71 *i;
            aLines.append(TLine(aWheels,-1*yRange,aWheels,yRange))
            aLines[i].SetLineStyle(2)
            aLines[i].Draw()
    return bLines, seperator, aLines



def globalStrawLayer(wheel,strawlayer):
    if wheel < 6:
        return wheel*16 + strawlayer

    return 6*16 + (wheel - 6)*8 + strawlayer

def drawStrawPlaneTransCan(bec,wheel,strawlayer,x1,y1,x2,y2):

    # 0 - 160
    strawPlane = int(globalStrawLayer(wheel,strawlayer))
    
    # Fill TRT Trans can
    if bec == 2:
        if strawPlane >= len(trtEndcapAStrawPlanes):
            return

        trtEndcapAStrawPlanes[strawPlane].cd()
        
        line.SetLineColor(kGreen+1)
        line.DrawArrow(x1,y1,x1+TRASL_FACTOR*(x1-x2),y1+TRASL_FACTOR*(y1-y2),0.01,"") 
        
    if bec == -2:
        if strawPlane >= len(trtEndcapCStrawPlanes):
            return

        trtEndcapCStrawPlanes[strawPlane].cd()
        line.SetLineColor(kGreen+1)
        line.DrawArrow(x1,y1,x1+TRASL_FACTOR*(x1-x2),y1+TRASL_FACTOR*(y1-y2),0.01,"")
    return
