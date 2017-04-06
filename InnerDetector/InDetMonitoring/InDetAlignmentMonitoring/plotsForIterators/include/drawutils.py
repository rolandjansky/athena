import pprint

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
    gStyle.SetPadLeftMargin(0.14) # Used to be 0.20 --> Salva
    gStyle.SetPadBottomMargin(0.14)


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
            #print " - color - File ", i,  " --> color ",Color_i
            Colors[i] = Color_i
        Colors[1] = 1    
            
    if (doDebug):        
        for i in range(NFiles):
            print " color ",i,"=", Colors[i]

    return Colors

def drawAllCorr(aligniter):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    debug = False

    if (debug): print " -- drawAllCorr -- starting for aligniter = ",aligniter
    showErrors = True
    Allaligniter = TCanvas("AlignmentCorrections(All)","Alignment Corrections (All)", 0, 0, 1600, 800)
    Allaligniter.Divide(4,2)
    AllCorrections = {}
    yrange = [0,0,0,0,0,0,0]
    if (debug): print " -- drawAllCorr -- finding out histogram ranges... "
    for i in range(7):
        if (debug): print "  -- drawAllCorr -- extracting range for dof:",i
        for i_iter in aligniter:
            for bin in range(aligniter[i_iter].nModules()):
                #print " ** drawutils ** drawAllCorr ** i_iter:",i_iter,"  nModules = ",aligniter[i_iter].nModules()
                if abs(aligniter[i_iter].GetModule(bin).GetDoF(i)[1])>yrange[i]:
                    yrange[i] = abs(aligniter[i_iter].GetModule(bin).GetDoF(i)[1])
        yrange[i] *= 1.1
        
    if (debug): print " -- drawAllCorr -- finding out colors ..."
    Color = AutoColors(len(aligniter))
    for i_iter in aligniter:
        for i in range(7):
            Allaligniter.cd(i+1)
            AllCorrections[i_iter]={}
            name, value = aligniter[i_iter].GetModule(0).GetDoF(i)
            hname = 'All_%s_corrections_%d' % (name,i_iter)
            htitle = 'All %s' % (name)
            AllCorrections[i_iter][name] = TH1F(hname,htitle, aligniter[i_iter].nModules(),0,aligniter[i_iter].nModules())
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                AllCorrections[i_iter][name].SetYTitle("mm")
            else:
                AllCorrections[i_iter][name].SetYTitle("mrad")
                
            for bin in range(aligniter[i_iter].nModules()):
                AllCorrections[i_iter][name].SetBinContent(bin+1,aligniter[i_iter].GetModule(bin).GetDoF(i)[1])
                if showErrors:
                    AllCorrections[i_iter][name].SetBinError(bin+1,aligniter[i_iter].GetModule(bin).GetDoFError(i)[1])

                if aligniter[i_iter].nModules()<35:
                    AllCorrections[i_iter][name].GetXaxis().SetBinLabel(bin+1,aligniter[i_iter].GetModule(bin).GetName().split("/")[-1])
                    
            AllCorrections[i_iter][name].GetXaxis().LabelsOption("v")
                    
            #AllCorrections[i_iter][name].SetMarkerStyle(20)
            AllCorrections[i_iter][name].SetMarkerColor(Color[i_iter])
            AllCorrections[i_iter][name].SetLineColor(Color[i_iter])
            AllCorrections[i_iter][name].SetFillColor(Color[i_iter])
            AllCorrections[i_iter][name].SetFillStyle(1001)

            AllCorrections[i_iter][name].SetStats(False)
            if i_iter==0:
                AllCorrections[i_iter][name].GetYaxis().SetRangeUser(-yrange[i],yrange[i])
                AllCorrections[i_iter][name].DrawCopy()                        
            else:
                AllCorrections[i_iter][name].DrawCopy('same')

            #gPad.SetGridx()
            gPad.SetGridy() 
            gPad.SetRightMargin(0.05)
            gPad.SetLeftMargin(0.2)
            gPad.Update()
    return Allaligniter

#############################################################
def OLD_drawCorrEvolution(aligniter, labelList, drawErrors=False, drawLine=True, whichdof=-1):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    from ROOT import TLegend

    doDebug = True
    showErrors = True
    drawAllDofs = True
    nUsedDofs = 7
    if (whichdof != -1):
        drawAllDofs = False 
        if (whichdof > 5): nUsedDofs = 7 # 7/oct/2015 with the BowX we may have up to 7 dofs. Unless stated draw the ordinary 6.

    lowerDof = 0;
    upperDof = 5;

    if (not drawAllDofs):
        lowerDof = whichdof
        upperDof = whichdof
    
    # a canvas for all dofs
    ThisCanvas = TCanvas("AlignmentCorrectionsEvol","Evolution of alignment corrections (All)", 0, 0, 1600, 800)
    if (drawAllDofs): ThisCanvas.Divide(4,2)

    
    hCorrectionsEvol = {}
    hCorrectionsEvolStruct={}
    yrange = [0.002,0.002,0.002,0.01,0.01,0.01, 0.002] # minimum is 0.001 (mm or mrad)
    # here aligniter = num of iterations
    numOfIterations = len(aligniter)
    if (doDebug): print " numOfIterations=", numOfIterations
    numOfAlignableStruct = aligniter[0].nModules()
    if (doDebug): print " numOfAlignableStructe=", numOfAlignableStruct
    EvolColor = AutoColors(numOfAlignableStruct)    
   

    # find out range
    #for i in range(lowerDof, upperDof):
    for i in range (nUsedDofs):
        for iter in range(numOfIterations):
            for bin in range(aligniter[iter].nModules()):
                thisValue = abs(aligniter[iter].GetModule(bin).GetDoF(i)[1])+ aligniter[iter].GetModule(bin).GetDoFError(i)[1]
                if abs(thisValue)>yrange[i]:
                    yrange[i] = thisValue
        yrange[i] *= 1.025
        if (doDebug): print " dof=",i," --> range: ",yrange[i]

    # maximum range
    maxTransRange = 0.100
    if (yrange[0]> maxTransRange): yrange[0] = maxTransRange       
    if (yrange[1]> maxTransRange): yrange[1] = maxTransRange       
    if (yrange[2]> maxTransRange): yrange[2] = maxTransRange       
    #yrange[0] = 0.04
    #yrange[1] = 0.04
        
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
            
        name = aligniter[0].GetModule(0).GetDoF(dof)[0]
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
        #for struct in range(0,2):
            if (doDebug): print " >> looping on struct:", struct
            hCorrectionsEvolStruct[dof]={}
            hname = 'Dof_%s_corrections_Evol_struct_%d' % (name, struct)
            htitle = 'Corrections evolution for structure %s (struct %d)' % (name, struct)
            hCorrectionsEvolStruct[dof][struct] = TH1F(hname, htitle, numOfIterations, -0.5, numOfIterations*1.-0.5)
            hCorrectionsEvolStruct[dof][struct].SetLineColor(EvolColor[struct])
            hCorrectionsEvolStruct[dof][struct].SetStats(False)
            # once the histogram is created, fill it with the corrections of each iteration
            accumvalue=0
            for iter in range(numOfIterations-1): # -1 to account for the overall integration
                value = aligniter[iter].GetModule(struct).GetDoF(dof)[1]
                accumvalue = accumvalue+value
                # if (doDebug): print " dof:",dof, "  struct:",struct,"  iter:",iter,"  delta=",value
                hCorrectionsEvolStruct[dof][struct].SetBinContent(iter+1, value)
                # if (doDebug): print "hCorrectionsEvolStruct[",dof,"][",struct,"].SetBinContent(",iter+1,",", value,")" 
                if (drawErrors): hCorrectionsEvolStruct[dof][struct].SetBinError(iter+1, aligniter[iter].GetModule(struct).GetDoFError(dof)[1])
                # now draw the corrections for this structure
            if (doDebug): print "Accumulated", hname, ":", accumvalue
            # store the accumulated    
            if (dof == 0): aligniter[numOfIterations-1].GetModule(struct).setTx(accumvalue)
            if (dof == 1): aligniter[numOfIterations-1].GetModule(struct).setTy(accumvalue)
            if (dof == 2): aligniter[numOfIterations-1].GetModule(struct).setTz(accumvalue)
            if (dof == 3): aligniter[numOfIterations-1].GetModule(struct).setRx(accumvalue)
            if (dof == 4): aligniter[numOfIterations-1].GetModule(struct).setRy(accumvalue)
            if (dof == 5): aligniter[numOfIterations-1].GetModule(struct).setRz(accumvalue)
            if (dof == 6): aligniter[numOfIterations-1].GetModule(struct).setBx(accumvalue)
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
                myLegend.AddEntry(hCorrectionsEvolStruct[dof][struct],aligniter[0].GetModule(struct).GetName(), "l")
                myLegend.Draw()
                gPad.Update()    
        
    return ThisCanvas

# list of the histograms 
hCorrectionsEvol = {}
hCorrectionsEvolStruct={}
#######################################################################
# new version of the drawCorrEvolution
#############################################################
def drawCorrEvolution(aligniter, labelList, drawErrors=False, drawLine=True, whichdof=-1, outputFormat=1):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    from ROOT import TLegend
    from ROOT import TLine
    import time
    import os
    #check with environment variable if server runs as local test 
    runLocalTest = False
    try:
        runLocalTest = os.getenv('RUNLOCALTEST', False)
    except:
        runLocalTest = False

    doDebug = True
    showErrors = True
    drawAllDofs = True
    nUsedDofs = 7

    if (whichdof != -1):
        drawAllDofs = False 
        if (whichdof > 5): nUsedDofs = 7 # 7/oct/2015 with the BowX we may have up to 7 dofs. Unless stated draw the ordinary 6.

    lowerDof = 0;
    upperDof = 6;

    if (not drawAllDofs):
        lowerDof = whichdof
        upperDof = whichdof

    # output formats
    ALLINONE = 1
    ONEBYONE = 2
    SPLITBYSTRUCT = 3

    Allaligniter = {} # list of canvas 
    finalName = ["","","","","","","",""] #name of the output files

    gridALLENTRIES = 1
    gridNEWRUN = 2
    xGridType = gridNEWRUN
    
    #    
    pathfiles = "."
    if (runLocalTest): 
        pathfiles = "/Users/martis/projectes/atlas/alineament/webmonitoringtest/InDetAlignmentWebMonitor/trunk/WebPage/constant/"

    plotfile = {}

    rootSetup()
    
    # -- start the body of the code --
    if (doDebug):    
        print "\n -- drawCorrEvolution -- START -- \n"
        print "              runLocalTest= ",runLocalTest
        print "                 pathfiles= ",pathfiles
        print "              outputFormat= ",outputFormat
        
    
    # min axis range for each dof
    #         Tx     Ty     Tz     Bx     Rx    Ry    Rz    Bx
    yrange = [2, 2, 2, 5, 0.01, 0.01, 0.01] # minimum is 0.001 (mm or mrad)

    # here aligniter = num of iterations
    
    numberOfSamples = len(aligniter)-1
    if (doDebug): print " -- drawCorrEvolution -- numberOfSamples=", numberOfSamples
    numOfAlignableStruct = aligniter[0].nModules()
    if (doDebug): print " numOfAlignableStructe=", numOfAlignableStruct
    EvolColor = AutoColors(numOfAlignableStruct)    
   
    # a canvas for all dofs
    ThisCanvas = TCanvas("AlignmentCorrectionsEvol","Evolution of alignment corrections (All)",920,600)
    if (drawAllDofs and outputFormat == ALLINONE): ThisCanvas.Divide(4,2)

        
    # find out range
    #for i in range(lowerDof, upperDof):
    for i in range (nUsedDofs):
        for iter in range(numberOfSamples):
            for bin in range(aligniter[iter].nModules()):
                thisValue = abs(aligniter[iter].GetModule(bin).GetDoF(i)[1])+ aligniter[iter].GetModule(bin).GetDoFError(i)[1]
                if( i<4 ): thisValue = thisValue*1.e3
                if abs(thisValue)>yrange[i]:
                    yrange[i] = thisValue
        yrange[i] *= 1.25
        if (doDebug): print " dof=",i," --> range: ",yrange[i]

    #        
    #  --- Y range of the plots --- cap the drawing range 
    # maximum range
    maxTransRange = 0.100
    if (yrange[0]> maxTransRange): yrange[0] = maxTransRange       
    if (yrange[1]> maxTransRange): yrange[1] = maxTransRange       
    if (yrange[2]> maxTransRange): yrange[2] = maxTransRange       
    if (yrange[3]> maxTransRange): yrange[3] = maxTransRange #Bx       
            
    # Tx and Ty should have the same range
    if (yrange[0] > yrange[1]): 
        yrange[1] = yrange[0]
    else:
        yrange[0] = yrange[1]    
    # Tz range should be as minimum as Tx and Ty
    if (yrange[2] < yrange[0]): yrange[2] = yrange[0]    

    #Bx range --> keep it independent
                
    # Rx and Ry should have the same range
    if (yrange[3] > yrange[4]): 
        yrange[4] = yrange[3]
    else:
        yrange[3] = yrange[4]  
         
    # Rz keep it independent


    #yrange[0] = 0.004
    #yrange[1] = 6.5
    #yrange[2] = 0.040
    #yrange[3] = 0.04
    #yrange[4] = 0.04
    #yrange[5] = 0.06
    #yrange[3] = 5

    #
    # prepare the legend
    #
    myLegend = TLegend(0.80,0.85,0.99,0.99)
    myLegend.SetFillColor(0) # white
    # and a line to split the runs if xGridType = gridNEWRUN
    myGridLine = TLine()
    myGridLine.SetLineStyle(3);
    
    # loop dof by dof (Tx, Ty...) and fill a polyline with teh corrections for each iteration
    if (doDebug): 
        print " -- drawCorrEvolution -- going to draw form lowerDof=", lowerDof, " to upperDof = ", upperDof
        
    #for dof in range(nUsedDofs):
    for dof in range(lowerDof, upperDof+1):
    #for dof in range(lowerDof, 6):
        # extract dof name
        name = aligniter[0].GetModule(0).GetDoF(dof)[0]
        if (doDebug): 
            print "\n -- drawCorrEvolution -- going to draw dof= %d (%s)" %(dof,name)

        #dealing with canvases
        if (outputFormat == ALLINONE):   
            ThisCanvas.cd()
            if (drawAllDofs): 
                ThisCanvas.cd(dof+1)
        if (outputFormat == ONEBYONE):  
            Allaligndofs[dof] = {}
            Allaligndofs[dof] = TCanvas("AlignCorrectionsEvol_%s" %(name), "Alignment_corrections_evolution_%s" %(name), 920, 600)
            print " -- drawCorrEvolution -- new canvas declared %s" %Allaligndofs[dof].GetName()
        if (outputFormat == SPLITBYSTRUCT): 
            Allaligndofs[dof] = {}
            Allaligndofs[dof] = TCanvas("AlignCorrectionsEvol_%s_byDOF" %(name), "Alignment_corrections_evolution_%s_byDOF" %(name), 920, 600)
            if (numOfAlignableStruct >= 15):
                Allaligndofs[dof].Divide(5,3)
                print " -- drawCorrEvolution -- Divide (5x3)"
            print " -- drawCorrEvolution -- new canvas declared %s for %d structures" %(Allaligndofs[dof].GetName(), numOfAlignableStruct)
            
        for imod in aligniter[0].det:
            module = aligniter[0].GetModule(imod)
            # prepare the histogram that will store the corrections 
            hname = 'Dof_%s_mod%s_corrections_Evol' % (name, module.FullName.replace("/","_") )
            htitle = 'Evolution of %s corrections' % (name)
            #htitle = 'Evolution of d%s/dLB corrections' % (name)
            if (doDebug): 
                print " -- drawCorrEvolution -- dof= %d  hname= %s" %(dof, hname)
                print "                                 htitle= ", htitle
                
            # build the histogram that is the main frame to plot the corrections for this dof (evolution of dof for each sample)
            hCorrectionsEvol[(dof,module)] = TH1F(hname, htitle, numberOfSamples, -0.5, numberOfSamples*1.-1.5) # the last sample is the accumulation
            
            hist = hCorrectionsEvol[(dof,module)]
            # set the x axis labels
            hist.GetXaxis().SetLabelSize(0.05)
            
            if (numberOfSamples > 8): # this is in case there are too many
                hist.GetXaxis().SetLabelSize(0.04)
                hist.GetXaxis().SetBit(18)
                if (numberOfSamples > 16): # reduce further the size
                    hist.GetXaxis().SetLabelSize(0.03)
                if (numberOfSamples > 24): # reduce further the size
                    hist.GetXaxis().SetLabelSize(0.02)
            hist.GetXaxis().SetLabelSize(0.05)
            hist.GetYaxis().SetLabelSize(0.05)
            
            for iter in range(numberOfSamples): 
                hist.GetXaxis().SetBinLabel(iter+1,'Iter_%d' % (iter))
                if (len(labelList)>iter): # use label given by user
                    hist.GetXaxis().SetBinLabel(iter+1,labelList[iter])
            
            # label of the Y axis
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hist.SetYTitle("#mum")
                #hist.SetYTitle("dTy/dLB [mm/25LB]")
            else:
                hist.SetYTitle("mrad")
                
            # set the range     
            hist.GetYaxis().SetRangeUser(-yrange[dof],yrange[dof])
            hist.SetStats(False)
            # going to draw the frame 
            # -be ware, in case of drawing by structure need to change to the right pad
            if (outputFormat == ALLINONE or outputFormat == ONEBYONE):
                hist.DrawCopy("p")
                gPad.SetGridy()
                if (xGridType == gridALLENTRIES): gridPad.SetGridx()
                
            
        # draw the frame
        #hist.DrawCopy()
        gPad.SetGridx()
        gPad.SetGridy()

        #
        # once the frame is plotted, loop on every structure and fill a histogram and draw it
        #
        if (doDebug):
            print "  -- drawCorrEvolution --  dealing with", name, "corrections. Y range for this dof: (",dof,")   range: +-",yrange[dof]
            print "                           reminder: numOfAlignableStruct = ",numOfAlignableStruct

        hCorrectionsEvolStruct[dof]={}
        for struct in range(numOfAlignableStruct):
            if (doDebug and False): print " >> looping on struct:", struct
            if (outputFormat == SPLITBYSTRUCT): 
                Allaligndofs[dof].cd(struct+1)
                # then draw frame
                hCorrectionsEvol[dof].DrawCopy()
                gPad.SetGridy()
                if (xGridType == gridALLENTRIES): gridPad.SetGridx()
            hname = 'Dof_%s_corrections_Evol_struct_%d' % (name, struct)
            htitle = 'Corrections evolution for structure %s (struct %d)' % (name, struct)
            hCorrectionsEvolStruct[dof][struct] = TH1F(hname, htitle, numberOfSamples, -0.5, numberOfSamples*1.-1.5) # last point is for the accumulation --> do not show
            hist = hCorrectionsEvolStruct[dof][struct]
            #print " -- drawCorrEvolution -- hCorrectionsEvolStruct has ",hCorrectionsEvolStruct[dof][struct].GetNbinsX(), ' bins in X'
            hist.SetLineColor(EvolColor[struct])
            hist.SetLineWidth(1)
            #hist.SetLineColor(1)
            hist.SetStats(False)
            # once the histogram is created, fill it with the corrections of each iteration

            currentLabel = "000000" # dummy value
            for iterval in range(numberOfSamples):  
                value = aligniter[iterval].GetModule(struct).GetDoF(dof)[1]
                error = aligniter[iterval].GetModule(struct).GetDoFError(dof)[1]
                if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                    hist.SetBinContent(iterval+1, value*1.e3)
                    if (drawErrors): hist.SetBinError(iterval+1, error*1.e3)
                else:
                    hist.SetBinContent(iterval+1, value)
                    if (drawErrors): hist.SetBinError(iterval+1, error)
                
            hist.Draw("l,same")
            gPad.Update()
                

        #t = time.time()
        #finalName[dof] = "%f_evolution_%s" %(t, name)
        
        if (outputFormat == ONEBYONE or outputFormat == SPLITBYSTRUCT):
            print " -- drawCorrEvol -- saving plots from ONEBYONE " 
            #Allaligndofs[dof].SaveAs(pathfiles + finalName[dof] + ".png", "png")
            #Allaligndofs[dof].SaveAs(pathfiles + finalName[dof] + ".pdf", "pdf")
            #Allaligndofs[dof].SaveAs(pathfiles + finalName[dof] + ".root", "root")
            #Allaligndofs[dof].SaveAs(pathfiles + finalName[dof] + ".C", "C")

    if (outputFormat == ALLINONE):
        print " -- drawCorrEvol -- saving plots from ALLINONE " 
        #t = time.time()
        #myFinalName = "%f_evolution_dofs" %(t)
        #ThisCanvas.SaveAs(pathfiles+myFinalName + ".png","png")
        #ThisCanvas.SaveAs(pathfiles+myFinalName + ".pdf","pdf")
        #plotfile['png']={0:myFinalName + ".png"}
        #plotfile['pdf']={0:myFinalName + ".pdf"}
        
    if (outputFormat == ONEBYONE or outputFormat == SPLITBYSTRUCT):            
        print "here"
        #plotfile['png']={0:finalName[0] + ".png", 1:finalName[1] + ".png", 2:finalName[2] + ".png", 3:finalName[3] + ".png", 4:finalName[4] + ".png", 5:finalName[5] + ".png", 6:finalName[6] + ".png"}
        #plotfile['pdf']={0:finalName[0] + ".pdf", 1:finalName[1] + ".pdf", 2:finalName[2] + ".pdf", 3:finalName[3] + ".pdf", 4:finalName[4] + ".pdf", 5:finalName[5] + ".pdf", 6:finalName[6] + ".pdf"}

    #return plotfile
    return ThisCanvas

#######################################################################
def drawCorrVsHits(aligniter):
    from ROOT import TCanvas
    from ROOT import TGraph
    from array import array
    from ROOT import gPad
    CorrVsHits = TCanvas("CorrVsHits", "Alignment Corrections vs hits")
    CorrVsHits.Divide(3,2)
    hCorrVsHits = {}
    Color = AutoColors(len(aligniter))
    for det in aligniter:
        hCorrVsHits[det] = {}
        for i in range(6):
            CorrVsHits.cd(i+1)
            name, value = aligniter[det].GetModule(0).GetDoF(i)
            hname = 'CorrVsHits_%s_corrections_%d' % (name,det)
            htitle = '%s Corrections vs hits_%d' % (name,det)
            xpoints = []
            ypoints = []
            for j in range(aligniter[det].nModules()):
                xpoints.append(aligniter[det].GetModule(j).Hits)
                ypoints.append(aligniter[det].GetModule(j).GetDoF(i)[1])
            x = array("f",xpoints)
            y = array("f",ypoints)
            hCorrVsHits[det][name] = TGraph(aligniter[det].nModules(),x,y)
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
    
def drawPixBarrelCorrDistributions(aligniter):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    PixBarrelCorrDistributions = TCanvas("PixBarrelCorrDistributions","Pixel Barrel Corrections Distributions")
    PixBarrelCorrDistributions.Divide(4,2)
    hPixBarrelCorrDistributions = {}
    detname = "Pixel Barrel"
    Color = AutoColors(len(aligniter))
    for d in aligniter:
        hPixBarrelCorrDistributions[d] = {}
        for i in range(7):
            PixBarrelCorrDistributions.cd(i+1)
            name, value = aligniter[d].GetModule(0).GetDoF(i)
            hname = '%s_%sCorrections_%d' % (detname,name,d)
            htitle = '%s %s Corrections' % (detname,name)
            if name is 'Tx':
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,200,-0.005,0.005)
            elif name is 'Ty':
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,200,-0.02,0.02)
            else:
                hPixBarrelCorrDistributions[d][name] = TH1F(hname,htitle,200,-0.25,0.25)
            if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                hPixBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mm)")
            else:
                hPixBarrelCorrDistributions[d][name].GetXaxis().SetTitle(name+" (mrad)")    
            
            hPixBarrelCorrDistributions[d][name].GetYaxis().SetTitle("Modules")
            hPixBarrelCorrDistributions[d][name].SetLineColor(Color[d])
            for module in aligniter[d].ReturnPixelBarrelModules():
                hPixBarrelCorrDistributions[d][name].Fill(module.GetDoF(i)[1])
            if d == 0:
                hPixBarrelCorrDistributions[d][name].Draw()
            else:
                hPixBarrelCorrDistributions[d][name].Draw("Same")   
            gPad.SetGridx()
            gPad.SetGridy()
    PixBarrelCorrDistributions.Update()
    return PixBarrelCorrDistributions, hPixBarrelCorrDistributions  
    
def drawSctBarrelCorrDistributions(aligniter):
    from ROOT import TCanvas
    from ROOT import TH1F
    from ROOT import gPad
    SctBarrelCorrDistributions = TCanvas("SctBarrelCorrDistributions","SCT Barrel Corrections Distributions")
    SctBarrelCorrDistributions.Divide(3,2)
    hSctBarrelCorrDistributions = {}
    detname = "SCT Barrel"
    Color = AutoColors(len(aligniter))
    for d in aligniter:
        hSctBarrelCorrDistributions[d] = {}
        for i in range(6):
            SctBarrelCorrDistributions.cd(i+1)
            name, value = aligniter[d].GetModule(0).GetDoF(i)
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
            for module in aligniter[d].ReturnSctBarrelModules():
                hSctBarrelCorrDistributions[d][name].Fill(module.GetDoF(i)[1])
            if d == 0:
                hSctBarrelCorrDistributions[d][name].Draw()
            else:
                hSctBarrelCorrDistributions[d][name].Draw("Same")   
            gPad.SetGridx()
            gPad.SetGridy()
    SctBarrelCorrDistributions.Update()
    return SctBarrelCorrDistributions, hSctBarrelCorrDistributions
        
def drawL3CorrVsHits(aligniter,det,bec):
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
    Color = AutoColors(len(aligniter))
    for d in aligniter:
        hCorrVsHits[d] = {}
        for i in range(6):
            L3CorrVsHits.cd(i+1)
            name, value = aligniter[d].GetModule(0).GetDoF(i)
            hname = '%s_CorrVsHits_%s_corrections' % (detname,name)
            htitle = '%s %s Corrections vs hits' % (detname,name)
            xpoints = []
            ypoints = []
            for j in aligniter[d].ReturnModules(det,bec):
                xpoints.append(aligniter[d].GetModule(j).Hits)
                ypoints.append(aligniter[d].GetModule(j).GetDoF(i)[1])
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
    
    
def drawStavePlots(aligniter,det):   
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
            stavemodules = aligniter.ReturnModules(det,0,lay,phi)
            hStaveCorrections = {}
            for i in range(6):
                StavePlots[ncanvas].cd(i+1)
                name, value = aligniter.GetModule(0).GetDoF(i)
                hname = 'Stave_%s_L%d_Phi%d_%s_corrections' % (detname,lay,phi,name)
                htitle = 'Stave %s L%d Phi%d %s Corrections' % (detname,lay,phi,name)
                hStaveCorrections[name] = TH1F(hname,htitle, len(stavemodules),0,len(stavemodules))
                if name is 'Tx' or name is 'Ty' or name is 'Tz' or name is 'Bx':
                    hStaveCorrections[name].SetYTitle("mm")
                else:
                    hStaveCorrections[name].SetYTitle("mrad")
                bin = 1
                for mod in stavemodules:
                    hStaveCorrections[name].SetBinContent(bin,aligniter.GetModule(mod).GetDoF(i)[1])
                    bin = bin+1
    
                #hStaveCorrections[name].Draw()
                XAxis = hStaveCorrections[name].GetXaxis()
                XAxis.CenterLabels()
                bin = 1
                for mod in stavemodules:
                    XAxis.SetBinLabel(bin,str(aligniter.GetModule(mod).Eta))
                    bin = bin + 1
                hStaveCorrections[name].SetStats(False)
                hStaveCorrections[name].DrawCopy()
                gPad.SetGridx()
                gPad.SetGridy()
            ncanvas = ncanvas + 1
    return StavePlots
    
def drawStaves(aligniter):
    StavePlots = []
    if aligniter.CountModules(1,0) > 0:
        StavePlots.append(drawStavePlots(aligniter,1))
    if aligniter.CountModules(2,0) > 0:
        StavePlots.append(drawStavePlots(aligniter,2))
    return StavePlots   

