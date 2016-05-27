# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def rootSetup():
    from ROOT import gStyle
    gStyle.SetStatColor(0)
    gStyle.SetFillColor(38)
    gStyle.SetCanvasColor(0)
    gStyle.SetPadColor(0)
    gStyle.SetPadBorderMode(0)
    gStyle.SetCanvasBorderMode(0)
    gStyle.SetFrameBorderMode(0)
    gStyle.SetOptStat(1110)
    gStyle.SetStatH(0.3)
    gStyle.SetStatW(0.3)
    
    gStyle.SetTitleFillColor(0)
    #gStyle.SetTitleY(1.)
    #gStyle.SetTitleX(.1)
    gStyle.SetTitleBorderSize(0)
    gStyle.SetHistLineWidth(2)
    gStyle.SetFrameFillColor(0)
    #gStyle.SetLineWidth(2)
    #gStyle.SetTitleColor(0)
    #gStyle.SetTitleColor(1)
    gStyle.SetLabelSize(0.05,"x")
    gStyle.SetLabelSize(0.05,"y")
    gStyle.SetLabelOffset(0.02,"y")
    gStyle.SetTitleOffset(1.8,"y")
    gStyle.SetTitleSize(0.04,"y")
    gStyle.SetPadRightMargin(0.02)
    gStyle.SetPadLeftMargin(0.20)


def  AutoColors(NFiles):
    Colors = {}
    doDebug = False

    #import settings.py
    #import imp
    #s_utils = imp.load_source('init', 'include/settings.py')
    #print " xxxyyyzzz ", s_utils.TestUseBarrel
    
    if NFiles > 2:
        ColorStep = 50./(NFiles-1)
    Color_i = 0;
    
    if NFiles is 1:
        Colors[0] = 920+3 #kGray
    
    elif NFiles is 2:
        Colors[0] = 632+1 #kRed+1
        Colors[1] = 920+3 #kGray+3

    elif NFiles is 3:
        Colors[0] = 632+1 #kRed+1
        Colors[1] = 920+2 #kGray+2
        Colors[2] = 860+1 #kAzure+1

    elif NFiles is 4:
        if (True):
            Colors[0] = 616 # IBL
            Colors[1] = 1   # Pixel 
            Colors[2] = 632 # SCT
            Colors[3] = 416 # TRT
        else:
            Colors[0] = 629 # SCT
            Colors[1] = 801   
            Colors[2] = 398 # TRT
            Colors[3] = 813 
            
    elif NFiles is 8:
        Colors[0] = 616 #kMagenta+2     #IBL
        Colors[1] = 1 #kBlack           #Pixel
        Colors[2] = 632 #kRed           #SCT
        Colors[3] = 629 #kRed+2
        Colors[4] = 801 #kOrange+7
        Colors[5] = 416 #          #TRT
        Colors[6] = 398 
        Colors[7] = 813 

    elif NFiles > 3:
        for i in range(0,NFiles):
            Color_i = int(51+i*ColorStep)
            if Color_i>100:
                Color_i = 100
            #print " File ", i,  " --> color ",Color_i
            Colors[i] = Color_i
        Colors[1] = 1    
            
    if (doDebug):        
        for i in range(NFiles):
            print " color ",i,"=", Colors[i]
            
    return Colors

def drawAllCorr(detector):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    debug = False

    if (debug): print " -- drawAllCorr -- starting for detector = ",detector
    showErrors = True
    Alldetector = TCanvas("AlignmentCorrections(All)","Alignment Corrections (All)")
    Alldetector.Divide(3,2)
    AllCorrections = {}
    yrange = [0,0,0,0,0,0]
    if (debug): print " -- drawAllCorr -- finding out histogram ranges... "
    for i in range(6):
        if (debug): print "  -- drawAllCorr -- extracting range for dof:",i
        for det in detector:
            for bin in range(detector[det].nModules()):
                #print " ** drawutils ** drawAllCorr ** det:",det,"  nModules = ",detector[det].nModules()
                if abs(detector[det].GetModule(bin).GetDoF(i)[1])>yrange[i]:
                    yrange[i] = abs(detector[det].GetModule(bin).GetDoF(i)[1])
        yrange[i] *= 1.1
        
    if (debug): print " -- drawAllCorr -- finding out colors ..."
    Color = AutoColors(len(detector))
    for det in detector:
        for i in range(6):
            Alldetector.cd(i+1)
            AllCorrections[det]={}
            name, value = detector[det].GetModule(0).GetDoF(i)
            hname = 'All_%s_corrections_%d' % (name,det)
            htitle = 'All %s Corrections_%d' % (name,det)
            AllCorrections[det][name] = TH1F(hname,htitle, detector[det].nModules(),0,detector[det].nModules())
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                AllCorrections[det][name].SetYTitle("mm")
            else:
                AllCorrections[det][name].SetYTitle("mrad")
                
            for bin in range(detector[det].nModules()):
                AllCorrections[det][name].SetBinContent(bin+1,detector[det].GetModule(bin).GetDoF(i)[1])
                if showErrors:
                    AllCorrections[det][name].SetBinError(bin+1,detector[det].GetModule(bin).GetDoFError(i)[1])

                if detector[det].nModules()<35:
                    AllCorrections[det][name].GetXaxis().SetBinLabel(bin+1,detector[det].GetModule(bin).GetName())
                    
            #AllCorrections[det][name].SetMarkerStyle(20)
            AllCorrections[det][name].SetMarkerColor(Color[det])
            AllCorrections[det][name].SetLineColor(Color[det])
            AllCorrections[det][name].SetFillColor(Color[det])
            AllCorrections[det][name].SetFillStyle(1001)

            AllCorrections[det][name].SetStats(False)
            if det==0:
                AllCorrections[det][name].GetYaxis().SetRangeUser(-yrange[i],yrange[i])
                AllCorrections[det][name].DrawCopy()                        
            else:
                AllCorrections[det][name].DrawCopy('same')

            gPad.SetGridx()
            gPad.SetGridy() 
            gPad.Update()
    return Alldetector

#############################################################
def drawCorrEvolution(detector, labelList, drawErrors=False, drawLine=True, whichdof=-1):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    from ROOT import TLegend

    doDebug = True
    showErrors = True
    drawAllDofs = True
    nUsedDofs = 6
    if (whichdof != -1):
        drawAllDofs = False 
        if (whichdof > 5): nUsedDofs = 7 # 7/oct/2015 with the BowX we may have up to 7 dofs. Unless stated draw the ordinary 6.

    lowerDof = 0;
    upperDof = 5;

    if (not drawAllDofs):
        lowerDof = whichdof
        upperDof = whichdof
    
    # a canvas for all dofs
    ThisCanvas = TCanvas("AlignmentCorrectionsEvol","Evolution of alignment corrections (All)")
    if (drawAllDofs): ThisCanvas.Divide(3,2)

    
    hCorrectionsEvol = {}
    hCorrectionsEvolStruct={}
    yrange = [0.002,0.002,0.002,0.01,0.01,0.01, 0.002] # minimum is 0.001 (mm or mrad)
    # here detector = num of iterations
    numOfIterations = len(detector)
    if (doDebug): print " numOfIterations=", numOfIterations
    numOfAlignableStruct = detector[0].nModules()
    if (doDebug): print " numOfAlignableStructe=", numOfAlignableStruct
    EvolColor = AutoColors(numOfAlignableStruct)    
   

    # find out range
    #for i in range(lowerDof, upperDof):
    for i in range (nUsedDofs):
        for iter in range(numOfIterations):
            for bin in range(detector[iter].nModules()):
                thisValue = abs(detector[iter].GetModule(bin).GetDoF(i)[1])+ detector[iter].GetModule(bin).GetDoFError(i)[1]
                if abs(thisValue)>yrange[i]:
                    yrange[i] = thisValue
        yrange[i] *= 1.025
        if (doDebug): print " dof=",i," --> range: ",yrange[i]

    # maximum range
    maxTransRange = 0.1
    if (yrange[0]> maxTransRange): yrange[0] = maxTransRange       
    if (yrange[1]> maxTransRange): yrange[1] = maxTransRange       
    if (yrange[2]> maxTransRange): yrange[2] = maxTransRange       
            
    # Tx and Ty should have the same range
    if (yrange[0] > yrange[1]): 
        yrange[1] = yrange[0]
    else:
        yrange[0] = yrange[1]    
    # Tz range should be as minimum as Tx and Ty
    if (yrange[2] < yrange[0]): yrange[2] = yrange[0]    

    # Rx and Ry should have the same range
    if (yrange[3] > yrange[4]): 
        yrange[4] = yrange[3]
    else:
        yrange[3] = yrange[4]   
    # Rz range should be as minimum as Rx and Ry
    #if (yrange[5] < yrange[3]): yrange[5] = yrange[3]    

    # prepare the legend
    myLegend = TLegend(0.65,0.65,0.92,0.92)
    
    # loop dof by dof (Tx, Ty...) and fill a polyline with teh corrections for each iteration
    if (doDebug): 
        print " ** drawCorrEvolution ** lowerDof=", lowerDof, "  upperDof = ", upperDof
    #for dof in range(nUsedDofs):
    for dof in range(lowerDof, upperDof+1):
        if (doDebug): 
            print " ** drawCorrEvolution ** going to draw dof=", dof
        ThisCanvas.cd()
        if (drawAllDofs): 
            ThisCanvas.cd(dof+1)
            
        name = detector[0].GetModule(0).GetDoF(dof)[0]
        hname = 'Dof_%s_corrections_Evol' % (name)
        htitle = 'Corrections evolution for %s' % (name)
        if (doDebug): 
            print " ** drawCorrEvolution ** dof=", dof, " hname  = ", hname
            print "                     htitle = ", htitle
        # this histogram is the main frame
        hCorrectionsEvol[dof] = TH1F(hname, htitle, numOfIterations-1, -0.5, numOfIterations*1.-1.5)
        # set the x axis labels
        if (numOfIterations > 12):
            hCorrectionsEvol[dof].GetXaxis().SetLabelSize(0.03)
            hCorrectionsEvol[dof].GetXaxis().SetBit(18)
        for iter in range(numOfIterations-1): # -1 do not include the total sum in the plot
            hCorrectionsEvol[dof].GetXaxis().SetBinLabel(iter+1,'Iter_%d' % (iter))
            if (len(labelList)>iter): # use label given by user
                hCorrectionsEvol[dof].GetXaxis().SetBinLabel(iter+1,labelList[iter])
        # label Y axis
        if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
            hCorrectionsEvol[dof].SetYTitle("mm")
        else:
            hCorrectionsEvol[dof].SetYTitle("mrad")
        hCorrectionsEvol[dof].GetYaxis().SetRangeUser(-yrange[dof],yrange[dof])
        hCorrectionsEvol[dof].SetStats(False)
        hCorrectionsEvol[dof].DrawCopy()
        gPad.SetGridx()
        gPad.SetGridy()

    #once the frame is plotted, loop on every structure and fill a histogram and draw it
        print "  .. dealing with", name, "corrections .. with range for this dof: (",dof,")   range: +-",yrange[dof]
        print " ** ==> numOfAlignableStruct = ",numOfAlignableStruct
        for struct in range(numOfAlignableStruct):
            if (doDebug): print " >> loopin on struct:", struct
            hCorrectionsEvolStruct[dof]={}
            hname = 'Dof_%s_corrections_Evol_struct_%d' % (name, struct)
            htitle = 'Corrections evolution for structure %s (struct %d)' % (name, struct)
            hCorrectionsEvolStruct[dof][struct] = TH1F(hname, htitle, numOfIterations, -0.5, numOfIterations*1.-0.5)
            hCorrectionsEvolStruct[dof][struct].SetLineColor(EvolColor[struct])
            hCorrectionsEvolStruct[dof][struct].SetStats(False)
            # once the histogram is created, fill it with the corrections of each iteration
            accumvalue=0
            for iter in range(numOfIterations-1): # -1 to account for the overall integration
                value = detector[iter].GetModule(struct).GetDoF(dof)[1]
                accumvalue = accumvalue+value
                # if (doDebug): print " dof:",dof, "  struct:",struct,"  iter:",iter,"  delta=",value
                hCorrectionsEvolStruct[dof][struct].SetBinContent(iter+1, value)
                # if (doDebug): print "hCorrectionsEvolStruct[",dof,"][",struct,"].SetBinContent(",iter+1,",", value,")" 
                if (drawErrors): hCorrectionsEvolStruct[dof][struct].SetBinError(iter+1, detector[iter].GetModule(struct).GetDoFError(dof)[1])
                # now draw the corrections for this structure
            if (doDebug): print "Accumulated", hname, ":", accumvalue
            # store the accumulated    
            if (dof == 0): detector[numOfIterations-1].GetModule(struct).setTx(accumvalue)
            if (dof == 1): detector[numOfIterations-1].GetModule(struct).setTy(accumvalue)
            if (dof == 2): detector[numOfIterations-1].GetModule(struct).setTz(accumvalue)
            if (dof == 3): detector[numOfIterations-1].GetModule(struct).setRx(accumvalue)
            if (dof == 4): detector[numOfIterations-1].GetModule(struct).setRy(accumvalue)
            if (dof == 5): detector[numOfIterations-1].GetModule(struct).setRz(accumvalue)
            if (dof == 6): detector[numOfIterations-1].GetModule(struct).setBx(accumvalue)
            if (drawErrors): 
                hCorrectionsEvolStruct[dof][struct].SetFillColor(EvolColor[struct])
                hCorrectionsEvolStruct[dof][struct].SetFillStyle(3354)
                # hCorrectionsEvolStruct[dof][struct].DrawCopy('same e3')
                # --> original hCorrectionsEvolStruct[dof][struct].DrawCopy('same e3')
                hCorrectionsEvolStruct[dof][struct].SetMarkerStyle(20)
                hCorrectionsEvolStruct[dof][struct].SetMarkerSize(0.5)
                hCorrectionsEvolStruct[dof][struct].SetMarkerColor(EvolColor[struct])
                hCorrectionsEvolStruct[dof][struct].DrawCopy('same p e3 x0')
                for iter in range(numOfIterations-1):
                    hCorrectionsEvolStruct[dof][struct].SetBinError(iter+1, 0)
                hCorrectionsEvolStruct[dof][struct].SetFillStyle(0)
                hCorrectionsEvolStruct[dof][struct].DrawCopy('same l')

            else:
                myOption = "same"
                if (drawLine): 
                    myOption += " l"
                else:
                    myOption += " p"
                    hCorrectionsEvolStruct[dof][struct].SetMarkerStyle(20)
                    hCorrectionsEvolStruct[dof][struct].SetMarkerColor(EvolColor[struct])
                        
                hCorrectionsEvolStruct[dof][struct].DrawCopy(myOption)
            gPad.Update()
            if (dof == nUsedDofs-1 and False): 
                myLegend.AddEntry(hCorrectionsEvolStruct[dof][struct],detector[0].GetModule(struct).GetName(), "l")
                myLegend.Draw()
                gPad.Update()    
        
    return ThisCanvas

#######################################################################
def drawCorrVsHits(detector):
    from ROOT import TCanvas
    from ROOT import TGraph
    from array import array
    from ROOT import gPad
    CorrVsHits = TCanvas("CorrVsHits", "Alignment Corrections vs hits")
    CorrVsHits.Divide(3,2)
    hCorrVsHits = {}
    Color = AutoColors(len(detector))
    for det in detector:
        hCorrVsHits[det] = {}
        for i in range(6):
            CorrVsHits.cd(i+1)
            name, value = detector[det].GetModule(0).GetDoF(i)
            hname = 'CorrVsHits_%s_corrections_%d' % (name,det)
            htitle = '%s Corrections vs hits_%d' % (name,det)
            xpoints = []
            ypoints = []
            for j in range(detector[det].nModules()):
                xpoints.append(detector[det].GetModule(j).Hits)
                ypoints.append(detector[det].GetModule(j).GetDoF(i)[1])
            x = array("f",xpoints)
            y = array("f",ypoints)
            hCorrVsHits[det][name] = TGraph(detector[det].nModules(),x,y)
            hCorrVsHits[det][name].SetTitle(htitle)
            hCorrVsHits[det][name].GetXaxis().SetTitle("Hits per module")
            hCorrVsHits[det][name].GetYaxis().SetTitle(name)
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hCorrVsHits[det][name].GetYaxis().SetTitle(name+" (mm)")
            else:
                hCorrVsHits[det][name].GetYaxis().SetTitle(name+" (mrad)")
            hCorrVsHits[det][name].SetMarkerStyle(4)
            hCorrVsHits[det][name].SetMarkerSize(0.5)
            hCorrVsHits[det][name].SetMarkerColor(Color[det])
            if det==0:
                hCorrVsHits[det][name].Draw("Ap")
            else:
                hCorrVsHits[det][name].Draw("psame")
            gPad.SetGridx()
            gPad.SetGridy()
            gPad.SetLogx()
    CorrVsHits.Update()
    return CorrVsHits, hCorrVsHits  
    
def drawPixBarrelCorrDistributions(detector):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    PixBarrelCorrDistributions = TCanvas("PixBarrelCorrDistributions","Pixel Barrel Corrections Distributions")
    PixBarrelCorrDistributions.Divide(3,2)
    hPixBarrelCorrDistributions = {}
    detname = "Pixel Barrel"
    Color = AutoColors(len(detector))
    for d in detector:
        hPixBarrelCorrDistributions[d] = {}
        for i in range(6):
            PixBarrelCorrDistributions.cd(i+1)
            name, value = detector[d].GetModule(0).GetDoF(i)
            hname = '%s_%sCorrections_%d' % (detname,name,d)
            htitle = '%s %s Corrections' % (detname,name)
            if name is 'Tx':
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.005,0.005)
            elif name is 'Ty':
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.02,0.02)
            else:
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.25,0.25)
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hPixBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mm)")
            else:
                hPixBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mrad)")    
            
            hPixBarrelCorrDistributions[d][name].GetYaxis().SetTitle("Modules")
            hPixBarrelCorrDistributions[d][name].SetLineColor(Color[d])
            for module in detector[d].ReturnPixelBarrelModules():
                hPixBarrelCorrDistributions[d][name].Fill(module.GetDoF(i)[1])
            if d == 0:
                hPixBarrelCorrDistributions[d][name].Draw()
            else:
                hPixBarrelCorrDistributions[d][name].Draw("Same")   
            gPad.SetGridx()
            gPad.SetGridy()
    PixBarrelCorrDistributions.Update()
    return PixBarrelCorrDistributions, hPixBarrelCorrDistributions  
    
def drawSctBarrelCorrDistributions(detector):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    SctBarrelCorrDistributions = TCanvas("SctBarrelCorrDistributions","SCT Barrel Corrections Distributions")
    SctBarrelCorrDistributions.Divide(3,2)
    hSctBarrelCorrDistributions = {}
    detname = "SCT Barrel"
    Color = AutoColors(len(detector))
    for d in detector:
        hSctBarrelCorrDistributions[d] = {}
        for i in range(6):
            SctBarrelCorrDistributions.cd(i+1)
            name, value = detector[d].GetModule(0).GetDoF(i)
            hname = '%s_%sCorrections_%d' % (detname,name,d)
            htitle = '%s %s Corrections' % (detname,name)
            if name is 'Tx':
                hSctBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.005,0.005)
            elif name is 'Ty':
                hSctBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.1,0.1)
            else:
                hSctBarrelCorrDistributions[d][name] = TH1F(hname,htitle,50,-0.25,0.25)
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hSctBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mm)")
            else:
                hSctBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mrad)")    
            
            hSctBarrelCorrDistributions[d][name].GetYaxis().SetTitle("Modules")
            hSctBarrelCorrDistributions[d][name].SetLineColor(Color[d])
            for module in detector[d].ReturnSctBarrelModules():
                hSctBarrelCorrDistributions[d][name].Fill(module.GetDoF(i)[1])
            if d == 0:
                hSctBarrelCorrDistributions[d][name].Draw()
            else:
                hSctBarrelCorrDistributions[d][name].Draw("Same")   
            gPad.SetGridx()
            gPad.SetGridy()
    SctBarrelCorrDistributions.Update()
    return SctBarrelCorrDistributions, hSctBarrelCorrDistributions
        
def drawL3CorrVsHits(detector,det,bec):
    from ROOT import TCanvas
    from ROOT import TGraph
    from array import array
    from ROOT import gPad       
    if det == 1:
        if bec == None:
            detname = "PIX"
        elif bec == 0:
            detname = "PIXBarrel"
        elif bec == -1:
            detname = "PIXECC"
        elif bec == 1:
            detname = "PIXECA"
        
    elif det == 2:
        if bec == None:
            detname = "SCT"
        elif bec == 0:
            detname = "SCTBarrel"
        elif bec == -1:
            detname = "SCTECC"
        elif bec == 1:
            detname = "SCTECA"
        
    L3CorrVsHits = TCanvas(detname+"CorrVsHits", detname+" Alignment Corrections vs hits")
    L3CorrVsHits.Divide(3,2)
    hCorrVsHits = {}
    Color = AutoColors(len(detector))
    for d in detector:
        hCorrVsHits[d] = {}
        for i in range(6):
            L3CorrVsHits.cd(i+1)
            name, value = detector[d].GetModule(0).GetDoF(i)
            hname = '%s_CorrVsHits_%s_corrections' % (detname,name)
            htitle = '%s %s Corrections vs hits' % (detname,name)
            xpoints = []
            ypoints = []
            for j in detector[d].ReturnModules(det,bec):
                xpoints.append(detector[d].GetModule(j).Hits)
                ypoints.append(detector[d].GetModule(j).GetDoF(i)[1])
            x = array("f",xpoints)
            y = array("f",ypoints)
            hCorrVsHits[d][name] = TGraph(len(x),x,y)
            hCorrVsHits[d][name].SetTitle(htitle)
            hCorrVsHits[d][name].GetXaxis().SetTitle("Hits per module")
            hCorrVsHits[d][name].GetYaxis().SetTitle(name)
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hCorrVsHits[d][name].GetYaxis().SetTitle(name+" (mm)")
            else:
                hCorrVsHits[d][name].GetYaxis().SetTitle(name+" (mrad)")
            hCorrVsHits[d][name].SetMarkerStyle(4)
            hCorrVsHits[d][name].SetMarkerSize(0.5)
            hCorrVsHits[d][name].SetMarkerColor(Color[d])
            if d == 0:
                hCorrVsHits[d][name].Draw("Ap")
            else:
                hCorrVsHits[d][name].Draw("Ap")
            
            gPad.SetGridx()
            gPad.SetGridy()
    L3CorrVsHits.Update()
    return L3CorrVsHits, hCorrVsHits    
    
    
def drawStavePlots(detector,det):   
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    StavePlots = {}
    if det == 1:
        Lay = [0,1,2]
        Phi = [22,38,52]
        detname = "PIX"
    if det == 2:
        Lay = [0,1,2,3]
        Phi = [32,40,48,56]
        detname = "SCT"
    ncanvas = 0
    for lay in Lay:
        for phi in range(Phi[lay]):
            StavePlots[ncanvas] = TCanvas("Stave%sPlots_L%d_Phi%d" %(detname, lay,phi),"Stave Plots %s L%d Phi%d" %(detname,lay,phi))
            StavePlots[ncanvas].Divide(3,2)
            stavemodules = detector.ReturnModules(det,0,lay,phi)
            hStaveCorrections = {}
            for i in range(6):
                StavePlots[ncanvas].cd(i+1)
                name, value = detector.GetModule(0).GetDoF(i)
                hname = 'Stave_%s_L%d_Phi%d_%s_corrections' % (detname,lay,phi,name)
                htitle = 'Stave %s L%d Phi%d %s Corrections' % (detname,lay,phi,name)
                hStaveCorrections[name] = TH1F(hname,htitle, len(stavemodules),0,len(stavemodules))
                if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                    hStaveCorrections[name].SetYTitle("mm")
                else:
                    hStaveCorrections[name].SetYTitle("mrad")
                bin = 1
                for mod in stavemodules:
                    hStaveCorrections[name].SetBinContent(bin,detector.GetModule(mod).GetDoF(i)[1])
                    bin = bin+1
    
                #hStaveCorrections[name].Draw()
                XAxis = hStaveCorrections[name].GetXaxis()
                XAxis.CenterLabels()
                bin = 1
                for mod in stavemodules:
                    XAxis.SetBinLabel(bin,str(detector.GetModule(mod).Eta))
                    bin = bin + 1
                hStaveCorrections[name].SetStats(False)
                hStaveCorrections[name].DrawCopy()
                gPad.SetGridx()
                gPad.SetGridy()
            ncanvas = ncanvas + 1
    return StavePlots
    
def drawStaves(detector):
    StavePlots = []
    if detector.CountModules(1,0) > 0:
        StavePlots.append(drawStavePlots(detector,1))
    if detector.CountModules(2,0) > 0:
        StavePlots.append(drawStavePlots(detector,2))
    return StavePlots   

