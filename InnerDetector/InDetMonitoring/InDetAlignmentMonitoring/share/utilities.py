# Author:
#  John Alison johnda@hep.upenn.edu
#  Ben Cooper b.d.cooper@qmul.ac.uk


import sys, math
#from ROOT import TH1,TF1,TLatex,TCanvas,TMarker,TH1F, gROOT
from array import array

from ROOT import *
from AtlasStyle import *
#SetAtlasStyle()
TGaxis.SetMaxDigits(4)


# if this is true then legends are drawn in order of the input files
# if it is false then legends are drawn in order of max histogram first
# no need for this ever to be False really
forceDrawOrder = True

normaliserHisto = 0 #histogram which other hists are normalised to if normalisation is used. e.g. 0 is hist from first file
statsMethod = 3 #how width is calculated if don't use fit. 0 = plain RMS, 1 = RMS(95%), 2 = RMS within range (sigmaIterativeGaus), 3 = FWHM/2.35
interpolateFWHM = True # use linear interpolation in FWHM estimation
sigmaIterativeGaus = 1.5 #controls sigma range used for iterative single Gaussian fit
ZmumuVal = True #flag for ZmumuValidation particular things (axes range etc)
pTmin = 0 #the minimum pT for ZmumuValidation
pTmax = 100 #the maximum pT for ZmumuValidation
MinEntriesPerModule = 1 #Min number of entries per module to compute the residual maps values

##########################################################################################################
def DrawPlots(inputTuple, outputName, plotTitle, yAxisTitle, xAxisTitle, legendLeftX, legendUpperY, units, 
              canvasText, makeOutput, textBoxLeftX=0.60, textBoxUpperY=0.88, dynamicYRange=True, plotCosmetics="Default"):

    debug = False
    if (debug): print " <DrawPlots> -- start -- drawing ",inputTuple

    # dynamicYRange=True means that the y-range of the histogram drawn first is adjusted so that 
    # all the histograms will fit into this range. If False then the default y-range of the first histogram is used.

    if(plotCosmetics=="Default"):
        legendTextSize = 0.035
        legendMarkerSize = 1.5
        legendYLineSpacing = 0.05 # determines how spaced out in y the legend entries are (may want to increase/decrease depending on legend text size)
        legendMarkerYPosMod = [0.0,0.0,0.0,0.0] #for some reason the marker doesn't appear next to legend text in Y without some modification
        meanWidthOnSeparateLine = False# if true mean and width are displayed in legend on separate lines, if false they are on the same line
        showMean = True
        plotTitleOnFirstLine = False # puts the plotTitle next to the first text on the plot e.g. ATLAS
                                     #yAxisTitleOffset = 1.42 # NOT USED now handled by AtlasStyle
        yAxisTitleOffset = 1.8
    if(plotCosmetics=="ApprovedPlots"):
        legendTextSize = 0.05
        legendMarkerSize = 1.7
        legendYLineSpacing = 0.07 
        legendMarkerYPosMod = [-0.004,-0.004,-0.004,-0.004]
        meanWidthOnSeparateLine = False
        showMean = False
        plotTitleOnFirstLine = False 
        yAxisTitleOffset = 1.55 # NOT USED now handled by AtlasStyle

    if(plotCosmetics=="SmallLegend"):
        legendTextSize = 0.04
        legendMarkerSize = 1.7
        legendYLineSpacing = 0.035
        #legendMarkerYPosMod = -0.000
        legendMarkerYPosMod = [-0.002,0.0,0.0,0.0]
        meanWidthOnSeparateLine = False
        showMean = False
        plotTitleOnFirstLine = False
        yAxisTitleOffset = 1.55 # NOT USED now handled by AtlasStyle

    if(plotCosmetics=="SeparateLine"):
        legendTextSize = 0.035
        legendMarkerSize = 1.5
        legendYLineSpacing = 0.05 # determines how spaced out in y the legend entries are (may want to increase/decrease depending on legend text size)
        legendMarkerYPosMod = [0.0,0.0,0.0,0.0] #for some reason the marker doesn't appear next to legend text in Y without some modification
        meanWidthOnSeparateLine = True # if true mean and width are displayed in legend on separate lines, if false they are on the same line
        showMean = True
        plotTitleOnFirstLine = False 
        yAxisTitleOffset = 1.8 # NOT USED now handled by AtlasStyle

    can = TCanvas(outputName,outputName,800,600)
    can.cd()
    if (debug): print " <DrawPlots> cosmetics set and canvas already open" 

        
    # determining the max and min histograms
    maxYVal = -9999999.0
    minYVal = 9999999.0
    for i in range(len(inputTuple)):
        
        if i==1 or i==4 or i==7 or i==10:
             #Fixing the Overflow problem. - Hack, can be done better 
            inputTuple[i].SetBinContent(inputTuple[i].GetNbinsX()+1,inputTuple[i].GetBinContent(inputTuple[i].GetNbinsX()))
            #Fixing the Underflow problem - Hack, can be done better 
            inputTuple[i].SetBinContent(0,inputTuple[i].GetBinContent(1))
            if inputTuple[i].GetMaximum() > maxYVal:
                #maxYVal = inputTuple[i].GetMaximum()
                maxYVal = inputTuple[i].GetBinContent(inputTuple[i].GetMaximumBin())
            if inputTuple[i].GetMinimum() < minYVal:
                #minYVal = inputTuple[i].GetMinimum()
                minYVal = inputTuple[i].GetBinContent(inputTuple[i].GetMinimumBin())
    #minYVal = 0.0 #overriding for now 
    print " <DrawPlots> maxYVal = ",maxYVal,", minYVal = ",minYVal

    # drawing the first histogram
    hist = inputTuple[1]
    if (debug): print " <DrawPlots> going to draw ", hist, " ....." 
    #if hist.GetName()=="pT":

        #I was asked to set y linear 19/06/13
        # gPad.SetLogy()
        #I am trying to fix the X range for Pt plots. Please do it in a better way!! (PF 28/03/13)
    histoTitle = hist.GetName()
    #if histoTitle.find('pT')!=-1  and ZmumuVal:
    #    hist.GetXaxis().SetRangeUser(pTmin,pTmax)
    if dynamicYRange:
        if hist.GetName()=="pix_b_residualy":
            hist.GetYaxis().SetRangeUser(minYVal,maxYVal*1.33)
        else:
            #hist.GetYaxis().SetRangeUser(minYVal*1.10,maxYVal*1.10)
            hist.GetYaxis().SetRangeUser(minYVal*0.8,maxYVal*1.20)
            
            
                
            
    hist.GetXaxis().SetTitle(xAxisTitle)
    if hist.GetYaxis().GetTitle()!="Arbitrary units":
        hist.GetYaxis().SetTitle(yAxisTitle)
    #hist.GetYaxis().SetTitleOffset(yAxisTitleOffset) # now handled by ATLAS style
    if "D0bsVsPhi0" in hist.GetName() or "D0bsVsEta" in hist.GetName():
        hist.GetYaxis().SetLabelOffset(0.015)
        hist.GetYaxis().SetLabelSize(0.045)
    if "mean_" in hist.GetName():
        hist.Draw("histo")
    else:
        hist.Draw()

    if (debug): print " <DrawPlots> drawn !!! "
        
    # drawing function associated with first histogram
    tf1 = inputTuple[0]
    if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")
    if (debug): print " <DrawPlots> function drawn !!! "

    # canvas text (right hand side)
    latexAtlas = TLatex()
    latexAtlas.SetNDC()
    #latexAtlas.SetTextFont(72)
    latexAtlas.SetTextColor(1)
    latexAtlas2 = TLatex()
    latexAtlas2.SetNDC()

    if(plotTitleOnFirstLine):
        latexAtlas.DrawLatex(textBoxLeftX,textBoxUpperY,canvasText[0] + "  " + plotTitle)
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.06,canvasText[1])
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.12,canvasText[2])
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.18,canvasText[3])    
    else:
        latexAtlas.DrawLatex(textBoxLeftX,textBoxUpperY,canvasText[0])
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.06,plotTitle)
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.12,canvasText[1])
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.18,canvasText[2])
        latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.24,canvasText[3])  
        #latexAtlas3 = TLatex()
        #latexAtlas3.SetNDC()
        #latexAtlas3.SetTextSize(0.08)
        #latexAtlas3.DrawLatex(textBoxLeftX,textBoxUpperY-0.40,canvasText[3])    

    if (debug): print " <DrawPlots> first set of legends drawn !!! "

    # drawing legend associated with first histogram
    legendTitle = inputTuple[2]
    m_l = TLatex()
    m_l.SetTextSize(legendTextSize)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
    m_l2 = TLatex()
    m_l2.SetTextSize(legendTextSize)
    m_l2.SetTextAlign(12)
    m_l2.SetNDC()
    if (debug): print " <DrawPlots> calling to defineLegend hist:",hist
    legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[0],legendMarkerSize)
    m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
    if (debug): print " <DrawPlots> back from defineLegend "
    if meanWidthOnSeparateLine==True:
        m_width = TLatex()
        m_width.SetTextSize(legendTextSize)
        m_width.SetTextAlign(12)
        m_width.SetNDC()
        m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
    marker = legendTuple[0]
    marker.Draw("same")

    if (debug): print " <DrawPlots> second set of legends drawn !!! "
 
    #determining how far offset in Y the next legend entry should be
    legendYOffset = 2*legendYLineSpacing
    if meanWidthOnSeparateLine==True:
        legendYOffset = 3*legendYLineSpacing

    ## uncomment to display fraction of histogram within sigma range
    #m_l3 = TLatex()
    #m_l3.SetTextSize(legendTextSize)
    #m_l3.SetTextAlign(12)
    #m_l3.SetNDC()
    #m_l3.SetTextColor(hist.GetMarkerColor())
    #text = "Frac. = " + str(round(findFractionWithinSigmaRange(hist,sigmaIterativeGaus),3)*100) + "%" 
    #m_l3.DrawLatex(legendLeftX,legendUpperY-0.35,text)    

    ## uncomment to display fit properties
    #print "fit for ",hist.GetName()," ",legendTitle,": chi2/DOF = ",tf1.GetChisquare(),"/",tf1.GetNDF()," = ",tf1.GetChisquare()/tf1.GetNDF()
    #m_l3 = TLatex()
    #m_l3.SetTextSize(legendTextSize)
    #m_l3.SetTextAlign(12)
    #m_l3.SetNDC()
    #m_l3.SetTextColor(hist.GetMarkerColor())
    #text = "#chi^{2}/DOF = " + str(round(tf1.GetChisquare()/tf1.GetNDF(),2)) 
    #m_l3.DrawLatex(legendLeftX,legendUpperY-0.35,text)

    ## uncomment to print fraction of data covered by 1-sigma of fit (assumes mean of fit = 0.0)
    #maxFitRange = tf1.GetParameter(1) + (tf1.GetParameter(2)*sigmaIterativeGaus)
    #minFitRange = tf1.GetParameter(1) - (tf1.GetParameter(2)*sigmaIterativeGaus)
    #maxFitBin = hist.FindBin(maxFitRange)
    #minFitBin = hist.FindBin(minFitRange)
    #intGral = hist.Integral(minFitBin,maxFitBin)
    #fullIntGral = hist.Integral(1,hist.GetNbinsX())
    ##print "maxFitRange =  ",maxFitRange,", minFitRange = ", minFitRange
    ##print "maxFitBin =  ",maxFitBin,", minFitBin = ", minFitBin
    #print "histo ",hist.GetName()," fraction covered by fit = ",intGral/fullIntGral
    #m_l4 = TLatex()
    #m_l4.SetTextSize(legendTextSize)
    #m_l4.SetTextAlign(12)
    #m_l4.SetNDC()
    #m_l4.SetTextColor(hist.GetMarkerColor())
    #text = "Fit Frac. = " + str(round(intGral/fullIntGral,2)) 
    #m_l4.DrawLatex(legendLeftX+0.5,legendUpperY-0.35,text)

    ## uncomment to draw core and tail functions on 
    #coreFunc = TF1("coreFunc","gaus",-0.5,0.5)
    #coreFunc.SetParameter(0,tf1.GetParameter(0))
    #coreFunc.SetParameter(1,tf1.GetParameter(1))
    #coreFunc.SetParameter(2,tf1.GetParameter(2))
    #coreFunc.SetLineColor(hist.GetMarkerColor())
    #coreFunc.SetLineStyle(2)
    #coreFunc.Draw("same")
    #integralCore = coreFunc.Integral(-0.5,0.5)
    #print "integralCore = ",integralCore
    #tailFunc = TF1("tailFunc","gaus",-0.5,0.5)
    #tailFunc.SetParameter(0,tf1.GetParameter(3))
    #tailFunc.SetParameter(1,tf1.GetParameter(4))
    #tailFunc.SetParameter(2,tf1.GetParameter(5))
    #tailFunc.SetLineColor(hist.GetMarkerColor())
    #tailFunc.SetLineStyle(2)
    #tailFunc.Draw("same")
    #integralTail = tailFunc.Integral(-0.5,0.5)
    #fracCore = integralCore/(integralCore+integralTail)
    #print "integralTail = ",integralTail
    #print "fracCore = " ,fracCore
    #m_l4 = TLatex()
    #m_l4.SetTextSize(legendTextSize)
    #m_l4.SetTextAlign(12)
    #m_l4.SetNDC()
    #m_l4.SetTextColor(hist.GetMarkerColor())
    #text = "Core Frac. = " + str(round(fracCore,2)) 
    #m_l4.DrawLatex(legendLeftX+0.5,legendUpperY-0.35,text)

    # drawing second histogram and its legend
    if (len(inputTuple)) > 3:
        hist = inputTuple[3+1]
        histoTitle = hist.GetName()
        if histoTitle.find('pT')!=-1  and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
        #hist.Draw("same,e")
        tf1 = inputTuple[3+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[3+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[1],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker2 = legendTuple[0]
        marker2.Draw("same")

        ## uncomment to display fraction of histogram within sigma range
        #m_l3.SetTextColor(hist.GetMarkerColor())
        #text = "Frac. = " + str(round(findFractionWithinSigmaRange(hist,sigmaIterativeGaus),3)*100) + "%" 
        #m_l3.DrawLatex(legendLeftX,legendUpperY-0.4,text)    

        ## uncomment to display fit properties
        #print "fit for ",hist.GetName()," ",legendTitle,": chi2/DOF = ",tf1.GetChisquare(),"/",tf1.GetNDF()," = ",tf1.GetChisquare()/tf1.GetNDF()
        #m_l3.SetTextColor(hist.GetMarkerColor())
        #text = "#chi^{2}/DOF = " + str(round(tf1.GetChisquare()/tf1.GetNDF(),2)) 
        #m_l3.DrawLatex(legendLeftX,legendUpperY-0.4,text)

        ## uncomment to print fraction of data covered by 1-sigma of fit (assumes mean of fit = 0.0)
        #maxFitRange = tf1.GetParameter(1) + (tf1.GetParameter(2)*sigmaIterativeGaus)
        #minFitRange = tf1.GetParameter(1) - (tf1.GetParameter(2)*sigmaIterativeGaus)
        #maxFitBin = hist.FindBin(maxFitRange)
        #minFitBin = hist.FindBin(minFitRange)
        #intGral = hist.Integral(minFitBin,maxFitBin)
        #fullIntGral = hist.Integral(1,hist.GetNbinsX())
        #print "histo ",hist.GetName()," fraction covered by fit = ",intGral/fullIntGral
        #m_l4.SetTextColor(hist.GetMarkerColor())
        #text = "Fit Frac. = " + str(round(intGral/fullIntGral,2)) 
        #m_l4.DrawLatex(legendLeftX+0.5,legendUpperY-0.4,text)

        ### uncomment to draw core and tail functions on 
        #coreFunc2 = TF1("coreFunc","gaus",-0.5,0.5)
        #coreFunc2.SetParameter(0,tf1.GetParameter(0))
        #coreFunc2.SetParameter(1,tf1.GetParameter(1))
        #coreFunc2.SetParameter(2,tf1.GetParameter(2))
        #coreFunc2.SetLineColor(hist.GetMarkerColor())
        #coreFunc2.SetLineStyle(2)
        ##coreFunc2.Draw("same")
        #integralCore2 = coreFunc2.Integral(-0.5,0.5)
        #print "integralCore = ",integralCore
        #tailFunc2 = TF1("tailFunc","gaus",-0.5,0.5)
        #tailFunc2.SetParameter(0,tf1.GetParameter(3))
        #tailFunc2.SetParameter(1,tf1.GetParameter(4))
        #tailFunc2.SetParameter(2,tf1.GetParameter(5))
        #tailFunc2.SetLineColor(hist.GetMarkerColor())
        #tailFunc2.SetLineStyle(2)
        ##tailFunc2.Draw("same")
        #integralTail2 = tailFunc2.Integral(-0.5,0.5)
        #fracCore2 = integralCore2/(integralCore2+integralTail2)
        #print "integralTail = ",integralTail2
        #print "fracCore = " , fracCore2
        #m_l4.SetTextColor(hist.GetMarkerColor())
        #text = "Core Frac. = " + str(round(fracCore2,2)) 
        #m_l4.DrawLatex(legendLeftX+0.5,legendUpperY-0.4,text)



    # drawing third histogram and its legend
    if (len(inputTuple)) > 6:
        hist = inputTuple[6+1]
        histoTitle = hist.GetName()
        if histoTitle.find('pT')!=-1  and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
        #hist.Draw("same,e")
        tf1 = inputTuple[6+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[6+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[2],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker3 = legendTuple[0]
        marker3.Draw("same")

    # drawing fourth histogram and its legend
    if (len(inputTuple)) > 9:
        hist = inputTuple[9+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[9+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[9+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker4 = legendTuple[0]
        marker4.Draw("same")

    if (debug): print " <DrawPlots> going to save file ... "

    if makeOutput:
        can.SaveAs(outputName)

    if (debug): print " <DrawPlots> -- completed -- "

###########################################################################################################################################        
def MakePlots(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles, normaliseHistos, unitArea=False):

    debug = False
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH1,TH1,TH1,TH1,TH1]
    Tuples = [tuple,tuple,tuple,tuple,tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakePlots> ===  ",histogramName,"  ==="
        if (debug): print " <MakePlots> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] = GetHistogram(rootFiles[i],histogramDir[i],histogramName,markerColors[i],markerStyles[i], i)
        
    for i in range(nFiles):
        #normalise histograms to unit area if desired
        if histoGram[i].Integral() > 0:
            if unitArea:
                #print "for hist ",i, " scaling" 
                histoGram[i].Scale(1/histoGram[i].Integral())
                histoGram[i].GetYaxis().SetTitle("Arbitrary units")
            #elif  i > 0 and normaliseHistos:
            elif normaliseHistos:
                histoGram[i].Scale(histoGram[normaliserHisto].Integral()/histoGram[i].Integral())

        # sometimes ROOT likes to draw a negative portion of y-axis when no negative entries
        RemoveNegativeYAxis(histoGram[i],histogramName)

        # find which histogram has largest y-value - this will be drawn first
        if histoGram[i].GetMaximum() > maxval:
            max_hist = i
            maxval = histoGram[i].GetMaximum()

        # perform the deired fit to the histogram
        fit = MakeFit(histoGram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,histoGram[i],legendTitles[i])

        

    if nFiles==1:
        totalTuple = Tuples[0]
    if nFiles==2:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0]
    if nFiles==3:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1]

    if nFiles==4:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2]



    if (debug): print "  <MakePlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################        
def MakeProfPlots(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles, symmetricRange=False):

    # this function takes as argument a TH2 and draws the mean profile or the rms profile 
    debug = False
    normaliseHistos = False # not normalization
    unitArea = False # not unit area

    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH2,TH2,TH2,TH2,TH2]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1]
    myProfile = [TProfile,TProfile,TProfile,TProfile,TProfile]
    Tuples = [tuple,tuple,tuple,tuple,tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeProfPlots> ===  ",histogramName,"  ==="
        if (debug): print " <MakeProfPlots> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] = GetHistogram(rootFiles[i],histogramDir[i],histogramName,markerColors[i],markerStyles[i])
        
    for i in range(nFiles):
        # make the profile
        myProfile[i] = histoGram[i].ProfileX()

        # sometimes ROOT likes to draw a negative portion of y-axis when no negative entries
        #RemoveNegativeYAxis(histoGram[i],histogramName)

        # find which histogram has largest y-value - this will be drawn first
        if myProfile[i].GetMaximum() > maxval:
            maxval = myProfile[i].GetMaximum()
        if myProfile[i].GetMinimum() < -maxval:
            maxval = -myProfile[i].GetMinimum()
        maxval = 1.10 * maxval

        # buld the returned histograms
        returnHistogram[i] = TH1F("new_"+histoGram[i].GetName(), histoGram[i].GetTitle(),                                  
                           histoGram[i].GetNbinsX(), histoGram[i].GetXaxis().GetXmin(), histoGram[i].GetXaxis().GetXmax())
        returnHistogram[i].SetMarkerStyle(markerStyles[i])
        returnHistogram[i].SetMarkerColor(markerColors[i])
        returnHistogram[i].SetLineColor(markerColors[i])

        for bin in range(histoGram[i].GetNbinsX()):
            returnHistogram[i].SetBinContent(bin+1, myProfile[i].GetBinContent(bin+1))
            returnHistogram[i].SetBinError(bin+1, myProfile[i].GetBinError(bin+1))
        
        # perform the deired fit to the histogram
        fit = MakeFit(returnHistogram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,returnHistogram[i],legendTitles[i])

    #unify range
    if (debug): print " <MakeProfPlots> maxval = ",maxval
    for i in range(nFiles):
        if (symmetricRange): 
            returnHistogram[i].GetYaxis().SetRangeUser(-maxval,maxval)
        else:
            returnHistogram[i].GetYaxis().SetRangeUser(0,maxval)
        

    if nFiles==1:
        totalTuple = Tuples[0]
    if nFiles==2:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0]
    if nFiles==3:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1]

    if nFiles==4:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2]



    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################        
def MakeProfPlotsFrom3D(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles,  symmetricRange=False, binRangeLower=-1, binRangeUpper=-1):

    # this function takes as argument a TH3 and obtains the profile in one of its axis. 
    debug = False
    normaliseHistos = False # not normalization
    unitArea = False # not unit area

    # in case we limit the range of bins
    rangeLimit = False
    if (binRangeLower > 0): rangeLimit = True
    if (binRangeUpper >0 and binRangeUpper >= binRangeLower): rangeLimit = True     
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH3,TH3,TH3,TH3,TH3]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1]
    myProfile = [TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D]
    Tuples = [tuple,tuple,tuple,tuple,tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeProfPlotsFrom3D> ===  ",histogramName,"  ==="
        if (debug): print " <MakeProfPlotsFrom3D> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] =GetHistogram3D(rootFiles[i],histogramDir[i],histogramName)

    for i in range(nFiles):
        # make the profile
        if (rangeLimit): histoGram[i].GetYaxis().SetRange(binRangeLower,binRangeUpper);
        myProfile[i] = histoGram[i].Project3D("ZX")
        tempProf = myProfile[i].ProfileX()
        
        # sometimes ROOT likes to draw a negative portion of y-axis when no negative entries
        #RemoveNegativeYAxis(histoGram[i],histogramName)

        # find which histogram has largest y-value - this will be drawn first
        if tempProf.GetMaximum() > maxval:
            maxval = tempProf.GetMaximum()
        if tempProf.GetMinimum() < -maxval:
            maxval = -tempProf.GetMinimum()
        maxval = 1.10 * maxval
        #if (maxval < 0.45): maxval = 0.450
        #if (maxval < 0.10): maxval = 0.100

        # buld the returned histograms
        returnHistogram[i] = TH1F("new_"+histoGram[i].GetName(), histoGram[i].GetTitle(),                                  
                           histoGram[i].GetNbinsX(), histoGram[i].GetXaxis().GetXmin(), histoGram[i].GetXaxis().GetXmax())
        returnHistogram[i].SetMarkerStyle(markerStyles[i])
        returnHistogram[i].SetMarkerColor(markerColors[i])
        returnHistogram[i].SetLineColor(markerColors[i])

        for bin in range(histoGram[i].GetNbinsX()):
            returnHistogram[i].SetBinContent(bin+1, tempProf.GetBinContent(bin+1))
            returnHistogram[i].SetBinError(bin+1, tempProf.GetBinError(bin+1))
        
        # perform the deired fit to the histogram
        fit = MakeFit(returnHistogram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,returnHistogram[i],legendTitles[i])

    #unify range
    if (debug): print " <MakeProfPlotsFrom3D> maxval = ",maxval
    for i in range(nFiles):
        if (symmetricRange): 
            if (debug): print " <MakeProfPlotsFrom3D> symmetric range with maxval = ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(-maxval,maxval)
        else:
            if (debug): print " <MakeProfPlotsFrom3D> range from 0 --> maxval = ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(0,maxval)
        

    if nFiles==1:
        totalTuple = Tuples[0]
    if nFiles==2:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0]
    if nFiles==3:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1]

    if nFiles==4:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2]



    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

##############################################################################################################
def MakeTwoPlotsFromSameFile(histogramDir,legendTitles,markerColors,markerStyles,histogramNames, fitType, rootFiles, fileToUse, normaliseHistos, unitArea=False):
    
    #gets 2 histograms from same file, normalises if desired and makes fits
    #returns histograms and fits

    maxval = 0.0
    max_hist = 0

    histoGram = [TH1,TH1]
    Tuples = [tuple,tuple]

    histoGram[0] = GetHistogram(rootFiles[fileToUse],histogramDir[fileToUse],histogramNames[0],markerColors[0],markerStyles[0])
    histoGram[1] = GetHistogram(rootFiles[fileToUse],histogramDir[fileToUse],histogramNames[1],markerColors[1],markerStyles[1])

    for i in range(2):

        #normalise histograms to unit area if desired
        if histoGram[i].Integral() > 0:
            if unitArea:
                histoGram[i].Scale(1/histoGram[i].Integral())
                histoGram[i].GetYaxis().SetTitle("Arbitrary units")
            elif  i > 0 and normaliseHistos:
                histoGram[i].Scale(histoGram[0].Integral()/histoGram[i].Integral())
        
        # sometimes ROOT likes to draw a negative portion of y-axis when no negative entries
        RemoveNegativeYAxis(histoGram[i],histogramNames[i])

        if histoGram[i].GetMaximum() > maxval:
            max_hist = i
            maxval = histoGram[i].GetMaximum()

        # perform the deired fit to the histogram
        fit = MakeFit(histoGram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,histoGram[i],legendTitles[i])


    if max_hist==0:
        totalTuple = Tuples[0] + Tuples[1]
    if max_hist==1:
        totalTuple = Tuples[1] + Tuples[0]

    return totalTuple #returning histograms and fits

###########################################################################################################################
def preparePalette(paletteStyle):

    if (paletteStyle == 3): 
    #trafic lights: black -> red -> orange -> green for efficiency plots
        NRGBs = 5
        NCont = 99
        stops = array("d", [ 0.05, 0.50, 0.75, 0.90, 1.00])
        red   = array("d", [ 0.10, 0.80, 0.99, 0.99, 0.04])
        green = array("d", [ 0.10, 0.05, 0.95, 0.90, 0.95])
        blue  = array("d", [ 0.10, 0.05, 0.05, 0.05, 0.04])
    
        TColor.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
        gStyle.SetNumberContours(NCont);

    return;

###########################################################################################################################
def MakeSiEndcapResidualDistribution(histogramName,histogramDir,rootFile):

    print "making Si endcap residuals"

    if "eca" in histogramName:
        h2d = rootFile.Get(histogramDir + "si_eca_resX")
    elif "ecc" in histogramName:
        h2d = rootFile.Get(histogramDir + "si_ecc_resX")
    else:
        print "EXITING because endcap not recognised in MakeSiEndcapResidualDistribution"
        sys.exit()

    if "pix" in histogramName:
        if "residualx_disk1" in histogramName:
            endcapHist = (h2d.ProjectionY(histogramName,1,1,"e")).Clone()
        elif "residualx_disk2" in histogramName:
            endcapHist = (h2d.ProjectionY(histogramName,2,2,"e")).Clone()
        elif "residualx_disk3" in histogramName:
            endcapHist = (h2d.ProjectionY(histogramName,3,3,"e")).Clone()
        else:
            print "EXITING because pixel disk not recognised in MakeSiEndcapResidualDistribution"
            sys.exit()

    if "sct" in histogramName:
        if "residualx_disk1" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,4,4,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,5,5,"e")).Clone()
        elif "residualx_disk2" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,6,6,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,7,7,"e")).Clone()
        elif "residualx_disk3" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,8,8,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,9,9,"e")).Clone()
        elif "residualx_disk4" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,10,10,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,11,11,"e")).Clone()
        elif "residualx_disk5" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,12,12,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,13,13,"e")).Clone()
        elif "residualx_disk6" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,14,14,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,15,15,"e")).Clone()
        elif "residualx_disk7" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,16,16,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,17,17,"e")).Clone()
        elif "residualx_disk8" in histogramName:
            sctEndcapSide0 = (h2d.ProjectionY(histogramName,18,18,"e")).Clone()
            sctEndcapSide1 = (h2d.ProjectionY(histogramName,19,19,"e")).Clone()
        else:
            print "EXITING because SCT disk not recognised in MakeSiEndcapResidualDistribution"
            sys.exit()
        endcapHist = sctEndcapSide0
        endcapHist.Sumw2()
        endcapHist.Add(sctEndcapSide1)


    return endcapHist

def FindCutBin(axis, cut, maxOrMin):


  cutBin = -100

  for i in range(axis.GetNbins()+1):
    
    edge = 0.0
    if maxOrMin < 0:
      edge = axis.GetBinLowEdge(i)
    else:
      edge = axis.GetBinUpEdge(i)
      
    if math.fabs(edge - cut) < 0.00001:
      cutBin = i
      break

    if i==axis.GetNbins():
      print "ERROR - cannot find cut bin!!!! Set to -100!!!"

  return cutBin

####################################################################
def MakeResidualMean1dHisto(histogramName,histogramDir,rootFile):

    debug = False

    xAxisLimit = 10.0
    nBins1d = 100
    meanOrError=0

    if "mean1d" in histogramName:
        if "pix" in histogramName and "yres" in histogramName:
            xAxisLimit = 50.0
            nBins1d = 500
        h1d = TH1F(histogramName,histogramName,nBins1d,-xAxisLimit,xAxisLimit)
        meanOrError=0
    if "meanerror1d" in histogramName:
        h1d = TH1F(histogramName,histogramName,300,0.,30.)
        meanOrError=1
    if "nentries1d" in histogramName:
        h1d = TH1F(histogramName,histogramName,100,0.,100000.)
        meanOrError=2
    if debug:
        print "histogramName = ",histogramName 

    if "xres" in histogramName:
        resStr = "xres"
    if "yres" in histogramName:
        resStr = "yres"

    if "pix_b" in histogramName:
        if "b0" in histogramName:
                h3d = rootFile.Get(histogramDir + "pix_b0_" + resStr + "vsmodetaphi_3d")
        elif "b1" in histogramName:
                h3d = rootFile.Get(histogramDir + "pix_b1_" + resStr + "vsmodetaphi_3d")
        elif "b2" in histogramName:
                h3d = rootFile.Get(histogramDir + "pix_b2_" + resStr + "vsmodetaphi_3d")                                
        else:
            print "don't recognise histo name in MakeResidualMean1dHisto - EXITING"
            sys.exit()

    nBinsX = h3d.GetNbinsX()
    nBinsY = h3d.GetNbinsY()

    nModules = 0

    for i in range(nBinsX):

        for j in range(nBinsY):

            hProj = h3d.ProjectionZ("proj",i,i,j,j,"e")

            if hProj.GetEntries() <= 0:
                if debug:
                    print "Module has zero entries - probably a dead module"
                continue
            
            if(meanOrError==0):
                h1d.Fill((hProj.GetMean())*1000.)#fill with mean in microns
                if abs(hProj.GetMean()*1000.) > xAxisLimit:
                    print "WARNING: in MakeResidualMean1dHisto have a module with residual mean > ",xAxisLimit," microns"
            elif(meanOrError==1):
                h1d.Fill(hProj.GetMeanError()*1000.)
            else:
                h1d.Fill(hProj.GetEntries())
            nModules = nModules + 1

    if debug:
        print "nModules =",nModules 

    return h1d
        
def MakeResidualVsPtHisto(histogramName,histogramDir,rootFile,meanOrFWHM=0):

    # this method will usually fill a 1-d histogram with the mean or FWHM of residual/pull distributions as a function of track pT
    # however, if the histogramName ends with string "bin-X", where X is an integer, it will return the actual residual/pull
    # distribution in the bin X

    # binning used for the x-axis (pT)
    #nBins = 20
    #newBins = array('d',[-20.0,-15.0,-10.0,-8.0,-6.0,-5.0,-4.0,-3.0,-2.0,-1.0,0.0,1.0,2.0,3.0,4.0,5.0,6.0,8.0,10.0,15.0,20.0])

    nBins = 22
    newBins = array('d',[-30.0,-20.0,-15.0,-10.0,-8.0,-6.0,-5.0,-4.0,-3.0,-2.0,-1.0,0.0,1.0,2.0,3.0,4.0,5.0,6.0,8.0,10.0,15.0,20.0,30.0])

    debug = False
    makeDistribution = False #gets set to true if histogramName contains "bin-X"

    if "residual" in histogramName:
        if "pix_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_b_residualx_pt")
        elif "pix_eca_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_eca_residualx_pt")
        elif "pix_ecc_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_ecc_residualx_pt")
        elif "sct_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_b_residualx_pt")
        elif "sct_eca_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_eca_residualx_pt")
        elif "sct_ecc_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_ecc_residualx_pt")
        elif "trt_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "trt_b_residualR_pt_")
            nBins = 14
            newBins = array('d',[-30.0,-20.0,-10.0,-8.0,-4.0,-2.0,-1.0,0.0,1.0,2.0,4.0,8.0,10.0,20.0,30.0])    
        elif "trt_eca_" in histogramName:
            h2d = rootFile.Get(histogramDir + "trt_ec_residualR_pt_Endcap_A")
            nBins = 14
            newBins = array('d',[-30.0,-20.0,-10.0,-8.0,-4.0,-2.0,-1.0,0.0,1.0,2.0,4.0,8.0,10.0,20.0,30.0])    
        elif "trt_ecc_" in histogramName:
            h2d = rootFile.Get(histogramDir + "trt_ec_residualR_pt_Endcap_C")
            nBins = 14
            newBins = array('d',[-30.0,-20.0,-10.0,-8.0,-4.0,-2.0,-1.0,0.0,1.0,2.0,4.0,8.0,10.0,20.0,30.0])    
        else:
            print "don't recognise histo name in MakeResidualVsPtHisto - EXITING"
            sys.exit()
    elif "pullx" in histogramName:
        if "pix_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_b_pullx_pt")
        elif "pix_eca_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_eca_pullx_pt")
        elif "pix_ecc_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_ecc_pullx_pt")
        elif "sct_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_b_pullx_pt")
        elif "sct_eca_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_eca_pullx_pt")
        elif "sct_ecc_" in histogramName:
            h2d = rootFile.Get(histogramDir + "sct_ecc_pullx_pt")
        else:
            print "don't recognise histo name in MakeResidualVsPtHisto - EXITING"
            sys.exit()

    
    
    newHist = TH1F("newHist","newHist",nBins,newBins)

    #check to see if histogramName contains "bin-X"
    #if so we just return the projection for that bin
    binNum = 0
    if "bin-" in histogramName:
        makeDistribution = True
        binNumStr = (histogramName.split('-'))[1]
        binNum = int(binNumStr)
        print "binNumStr = ", binNumStr, ", binNum = ",binNum
    if makeDistribution:
        lowBin = FindCutBin(h2d.GetXaxis(),newBins[binNum-1],-1)
        highBin = FindCutBin(h2d.GetXaxis(),newBins[binNum],1)
        projHist = (h2d.ProjectionY(histogramName,lowBin,highBin,"e")).Clone()
        #projHist.Rebin(2)
        return projHist

    #otherwise fill 1-d histo with mean/width vs pT
    for i in range(nBins):

        lowBin = FindCutBin(h2d.GetXaxis(),newBins[i],-1)
        highBin = FindCutBin(h2d.GetXaxis(),newBins[i+1],1)
        projHist = (h2d.ProjectionY(histogramName,lowBin,highBin,"e")).Clone()
        #projHist.Rebin(2)
        mean = projHist.GetMean()
        rms = projHist.GetRMS()
        entries = projHist.GetEntries()
        meanErr = 1000.0
        if entries > 0:
            sqrtEntries = math.sqrt(entries);
            meanErr = rms/sqrtEntries
            fwhmArray = findMeanRMSUsingFWHM(projHist)
            fWHM = fwhmArray[1]
            #gausFit = singleGaus(projHist)
            gausFit = singleGausIterative(projHist,sigmaIterativeGaus)
            gausSigma = gausFit.GetParameter(2)
            gausSigmaErr = gausFit.GetParError(2)

        if debug: print "lowRange = ", newBins[i],", lowBin = ",lowBin,", highRange = ",newBins[i+1],", highBin = ",highBin,", content = ",projHist.GetEntries(),", mean = ",mean

        if entries > 0 and newBins[i] < -2.0 or newBins[i] >= 2.0:#we don't show bins below 2 GeV (can have handful of entries)
            if meanOrFWHM==0:
                newHist.SetBinContent(i+1,mean)
                newHist.SetBinError(i+1,meanErr)
            elif meanOrFWHM==1:
                if entries > 1000:
                    newHist.SetBinContent(i+1,fWHM)
                    newHist.SetBinError(i+1,0.000000001)
            else:
                if entries > 1000:
                    newHist.SetBinContent(i+1,gausSigma)
                    newHist.SetBinError(i+1,gausSigmaErr)

    return newHist
        
def MakeErrVsPtHisto(histogramName,histogramDir,rootFile):

    #print "making residual vs pt"

    
    h2d = rootFile.Get(histogramDir + histogramName)

    nBins = 20
    newBins = array('d',[0.0,0.8*2.0,0.8*4.0,0.8*6.0,0.8*8.0,0.8*10.0,0.8*12.0,0.8*14.0,0.8*16.0,0.8*18.0,0.8*20.0,0.8*22.0,0.8*24.0,0.8*26.0,
                         0.8*28.0,0.8*30.0,0.8*32.0,0.8*36.0,0.8*40.0,0.8*44.0,0.8*50.0])
    
    newHist = TH1F("newHist","newHist",nBins,newBins)
    
    
    for i in range(nBins):

        lowBin = FindCutBin(h2d.GetXaxis(),newBins[i],-1)
        highBin = FindCutBin(h2d.GetXaxis(),newBins[i+1],1)
        projHist = (h2d.ProjectionY(histogramName,lowBin,highBin,"e")).Clone()
        mean = projHist.GetMean()
        rms = projHist.GetRMS()
        entries = projHist.GetEntries()
        meanErr = 1000.0
        if entries > 0:
            sqrtEntries = math.sqrt(entries);
            meanErr = rms/sqrtEntries
            #fwhmArray = findMeanRMSUsingFWHM(projHist)
            #fWHM = fwhmArray[1]

        #print "lowRange = ", newBins[i],", lowBin = ",lowBin,", highRange = ",newBins[i+1],", highBin = ",highBin,", content = ",projHist.GetEntries(),", mean = ",mean

        if entries > 0:
            newHist.SetBinContent(i+1,mean)
            newHist.SetBinError(i+1,meanErr)

    return newHist

def GetHistogram(rootFile,histogramDir,histogramName,markerColor,markerStyle, fileID=0):

    if histogramName=="pix_ec_residualx":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_residualx")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_residualx")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Rebin()
        histoGram.Add(histoGramECC)
    elif histogramName=="sct_ec_residualx":
        histoGramECA = rootFile.Get(histogramDir + "sct_eca_residualx")
        histoGramECC = rootFile.Get(histogramDir + "sct_ecc_residualx")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="pix_ec_residualx_fine":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_residualx_fine")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_residualx_fine")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Rebin()
        histoGram.Add(histoGramECC)
    elif histogramName=="pix_ec_residualy_fine":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_residualy_fine")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_residualy_fine")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="pix_ec_residualy":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_residualy")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_residualy")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Rebin()
        histoGram.Add(histoGramECC)
    elif histogramName=="sct_ec_residualx":
        histoGramECA = rootFile.Get(histogramDir + "sct_eca_residualx")
        histoGramECC = rootFile.Get(histogramDir + "sct_ecc_residualx")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="sct_ec_residualx_fine":
        histoGramECA = rootFile.Get(histogramDir + "sct_eca_residualx_fine")
        histoGramECC = rootFile.Get(histogramDir + "sct_ecc_residualx_fine")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="trt_ec_residualR":
        histoGramECA = rootFile.Get(histogramDir + "trt_ec_residualR_Endcap_A")
        histoGramECC = rootFile.Get(histogramDir + "trt_ec_residualR_Endcap_C")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Rebin()
        histoGram.Add(histoGramECC)

    elif histogramName=="pix_b_pullx":
        histoGramB0 = rootFile.Get(histogramDir + "pix_b0_pullx")
        histoGramB1 = rootFile.Get(histogramDir + "pix_b1_pullx")
        histoGramB2 = rootFile.Get(histogramDir + "pix_b2_pullx")
        histoGram = histoGramB0.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramB1)
        histoGram.Add(histoGramB2)
    elif histogramName=="pix_b_pully":
        histoGramB0 = rootFile.Get(histogramDir + "pix_b0_pully")
        histoGramB1 = rootFile.Get(histogramDir + "pix_b1_pully")
        histoGramB2 = rootFile.Get(histogramDir + "pix_b2_pully")
        histoGram = histoGramB0.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramB1)
        histoGram.Add(histoGramB2)
    elif histogramName=="pix_ec_pullx":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_pullx")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_pullx")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="pix_ec_pully":
        histoGramECA = rootFile.Get(histogramDir + "pix_eca_pully")
        histoGramECC = rootFile.Get(histogramDir + "pix_ecc_pully")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="sct_b_pullx":
        histoGramB0 = rootFile.Get(histogramDir + "sct_b0_pullx")
        histoGramB1 = rootFile.Get(histogramDir + "sct_b1_pullx")
        histoGramB2 = rootFile.Get(histogramDir + "sct_b2_pullx")
        histoGramB3 = rootFile.Get(histogramDir + "sct_b3_pullx")
        histoGram = histoGramB0.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramB1)
        histoGram.Add(histoGramB2)
        histoGram.Add(histoGramB3)
    elif histogramName=="sct_ec_pullx":
        histoGramECA = rootFile.Get(histogramDir + "sct_eca_pullx")
        histoGramECC = rootFile.Get(histogramDir + "sct_ecc_pullx")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="trt_ec_pullR":
        histoGramECA = rootFile.Get(histogramDir + "trt_ec_pullR_Endcap_A")
        histoGramECC = rootFile.Get(histogramDir + "trt_ec_pullR_Endcap_C")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="trt_ec_pullR_notube":
        histoGramECA = rootFile.Get(histogramDir + "trt_ec_pullR_notube_Endcap_A")
        histoGramECC = rootFile.Get(histogramDir + "trt_ec_pullR_notube_Endcap_C")
        histoGram = histoGramECA.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramECC)
    elif histogramName=="pT_nolog":
        histoGram = rootFile.Get(histogramDir + "pT")
        histoGram.SetName("pT_nolog")
    elif "D0bsVs" in histogramName:
        histoGram =  ((rootFile.Get(histogramDir + histogramName)).ProfileX()).Clone()
    elif "residualx_disk" in histogramName:
        histoGram = MakeSiEndcapResidualDistribution(histogramName,histogramDir,rootFile)
    elif "residualx_pt_mean" in histogramName or "residual_pt_mean" in histogramName  or "pullx_pt_mean" in histogramName or "residualx_pt_bin" in histogramName:
        histoGram = MakeResidualVsPtHisto(histogramName,histogramDir,rootFile)
    elif "residualx_pt_fwhm" in histogramName or "residual_pt_fwhm" in histogramName:
        histoGram = MakeResidualVsPtHisto(histogramName,histogramDir,rootFile,1)
    elif "pullx_pt_width" in histogramName or "residual_pt_width" in histogramName:
        histoGram = MakeResidualVsPtHisto(histogramName,histogramDir,rootFile,2)
    elif histogramName=="pix_b_xres_mean1d" or  histogramName=="pix_b_xres_meanerror1d" or  histogramName=="pix_b_xres_nentries1d":
        confStr = "mean1d"
        if "meanerror1d" in histogramName:
            confStr = "meanerror1d"
        if "nentries1d" in histogramName:
            confStr = "nentries1d"            
        histoGramB0 = MakeResidualMean1dHisto("pix_b0_xres_"+confStr,histogramDir,rootFile)
        histoGramB1 = MakeResidualMean1dHisto("pix_b1_xres_"+confStr,histogramDir,rootFile)
        histoGramB2 = MakeResidualMean1dHisto("pix_b2_xres_"+confStr,histogramDir,rootFile)
        histoGram = histoGramB0.Clone()
        histoGram.Add(histoGramB1)
        histoGram.Add(histoGramB2)
    elif histogramName=="pix_b_yres_mean1d":
        histoGramB0 = MakeResidualMean1dHisto("pix_b0_yres_mean1d",histogramDir,rootFile)
        histoGramB1 = MakeResidualMean1dHisto("pix_b1_yres_mean1d",histogramDir,rootFile)
        histoGramB2 = MakeResidualMean1dHisto("pix_b2_yres_mean1d",histogramDir,rootFile)
        histoGram = histoGramB0.Clone()
        histoGram.Add(histoGramB1)
        histoGram.Add(histoGramB2)
    elif "err" in histogramName and "VsPt" in histogramName:
        histoGram = MakeErrVsPtHisto(histogramName,histogramDir,rootFile)
    else:
        histoGram = rootFile.Get(histogramDir + histogramName)

    # IBL residuals    
    if "IBL_residual" in histogramName:
        if histogramName=="IBL_residualx_3DSensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_xresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, True, False, fileID)
        if histogramName=="IBL_residualx_PlanarSensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_xresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, False, True, fileID)
        if histogramName=="IBL_residualy_3DSensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_yresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, True, False, fileID)
        if histogramName=="IBL_residualy_PlanarSensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_yresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, False, True, fileID)
        if histogramName=="IBL_residualx_3DASensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_xresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, True, False, fileID, "ASide")
        if histogramName=="IBL_residualx_3DCSensors":
            histoGram3D = GetHistogram3D(rootFile, histogramDir, "pix_b0_xresvsmodetaphi_3d") # retrieve the 3D histogram
            histoGram = getIBLResidualBySensorType(histoGram3D, 0, True, False, fileID, "CSide")
        histoGram.SetMarkerColor(markerColor)
        histoGram.SetMarkerStyle(markerStyle)


    #
    # - end of searching histograms
    # - now... let's do some basic crosschecks
    #    
    if not histoGram : #is None:#checking histograms exist    
        print "EXITING because failed to find histogram ",histogramDir + histogramName
        sys.exit()
         
    if histoGram.Integral()==0:#checking for empty histos
        print "WARNING zero entries in histogram ",histogramDir + histogramName

 #    if histogramName=="trt_b_residualR" or histogramName=="trt_ec_residualR_Endcap_A" or histogramName=="trt_ec_residualR_Endcap_C":
#         histoGram.Sumw2()#so have errors for TRT histograms - all others already have errors defined
#     if histogramName=="trt_b_pullR" or histogramName=="trt_ec_pullR_Endcap_A" or histogramName=="trt_ec_pullR_Endcap_C":
#         histoGram.Sumw2()#so have errors for TRT histograms - all others already have errors defined
#     if histogramName=="trt_b_pullR_notube" or histogramName=="trt_ec_pullR_notube_Endcap_A" or histogramName=="trt_ec_pullR_notube_Endcap_C":
#         histoGram.Sumw2()#so have errors for TRT histograms - all others already have errors defined


    histoGram.Sumw2()#so have errors
    histoGram.SetMarkerColor(markerColor)
    histoGram.SetMarkerStyle(markerStyle)
    histoGram.SetLineColor(markerColor)


    # rebinning histograms used for the approved plots
#     if histogramName=="pix_b_residualy":
#         histoGram.Rebin(2)
#     if histogramName=="pix_b_residualx_fine" or histogramName=="pix_ec_residualx_fine" or histogramName=="pix_eca_residualx_fine" or histogramName=="pix_ecc_residualx_fine":
#         histoGram.Rebin(2)
#         histoGram.SetAxisRange(-.25,.25)
#     if histogramName=="pix_b_residualy_fine":
#         histoGram.Rebin(4)
#         histoGram.SetAxisRange(-.75,.75)
#     if histogramName=="pix_eca_residualy_fine" or histogramName=="pix_ecc_residualy_fine" or histogramName=="pix_ec_residualy_fine":
#         histoGram.Rebin(16)
#         histoGram.SetAxisRange(-.75,.75)
#     if histogramName=="sct_b_residualx_fine" or histogramName=="sct_ec_residualx_fine" or histogramName=="sct_eca_residualx_fine" or histogramName=="sct_ecc_residualx_fine":
#         histoGram.Rebin(2)
#         histoGram.SetAxisRange(-.25,.25)
#     if histogramName=="pix_b_residualx" or histogramName=="pix_ec_residualx" or histogramName=="sct_b_residualx" or histogramName=="sct_ec_residualx":
#         histoGram.SetAxisRange(-.5,.5)

  
 
    # uncomment to rebin histograms
    #histoGram.Rebin(16);
#    if histogramName=="pix_b_residualy" :
#        histoGram.Rebin(2);
    if histogramName=="trt_b_residualR":
        histoGram.Rebin(2);
        
    if histogramName=="sct_b_residualx_fine":
        histoGram.Rebin()

        # comment this while aligning cosmic ray data 2014. Salvador Marti. 24/October/2014
        #    if histogramName=="pix_b_residualy_fine" or \
        #   histogramName=="sct_b_residualx_fine" or \
        #   histogramName=="pix_b_residualx":
        #histoGram.SetAxisRange(-0.15,0.15)

        
    if histogramName=="pix_b_residualx_fine" or \
           histogramName=="pix_ec_residualx_fine":
        histoGram.SetAxisRange(-.1,.1) 

    # if histogramName=="pix_b0_residualx" or histogramName=="pix_b1_residualx" or histogramName=="pix_b2_residualx" or histogramName=="pix_ec_residualx" or histogramName=="pix_eca_residualx" or histogramName=="pix_ecc_residualx" :#or histogramName=="pix_b_residualy":
    #histoGram.SetAxisRange(-.25,.25) #pri res

        # if histogramName=="sct_b_residualx" or histogramName=="sct_b0_residualx" or histogramName=="sct_b1_residualx" or histogramName=="sct_b2_residualx" or histogramName=="sct_b3_residualx" or histogramName=="sct_ec_residualx" or histogramName=="sct_eca_residualx" or histogramName=="sct_ecc_residualx":
        #histoGram.SetAxisRange(-.25,.25) #pri res

    if histogramName=="sct_ec_residualx_fine":
        histoGram.Rebin()
        histoGram.SetAxisRange(-.2,.2)

    # Following lines commented by Salva for Cosmic 2014 data --> need broader ranges 24/October/2014
    # #changing the x or y axis range of certain histograms from their defaults
    # if histogramName=="trt_b_residualR" or histogramName=="trt_ec_residualR_Endcap_A" or histogramName=="trt_ec_residualR_Endcap_C" or histogramName=="trt_ec_residualR" :
    # histoGram.SetAxisRange(-0.5,0.5)
    # if "asym" in histogramName:
    # histoGram.GetYaxis().SetRangeUser(-0.3,0.3)
        # priscilla

#    print " priscilla roofile ", rootFile.GetName()


    if histogramName=="delta_z0":
        histoGram.SetAxisRange(-1.,1.)

    if histogramName=="si_barrel_resX_mean" or histogramName=="si_eca_resX_mean" or histogramName=="si_ecc_resX_mean":
        #Patch for the mean
        histoGram.Scale(1000)
        histoGram.GetYaxis().SetRangeUser(-5,5)
    if histogramName=="pix_b_xresvsmodphi" or  histogramName=="pix_ecc_xresvsmodphi" or histogramName=="pix_eca_xresvsmodphi":
        #or histogramName=="si_ecc_resX_mean" or histogramName=="si_eca_resX_mean" or histogramName=="si_barrel_resX_mean": Peo 
        histoGram.GetYaxis().SetRangeUser(-.01,.01) #pri
    if histogramName=="pix_b_yresvsmodphi" or histogramName=="pix_eca_yresvsmodphi" or histogramName=="pix_ecc_yresvsmodphi":
        histoGram.GetYaxis().SetRangeUser(-.05,.05) #pri
    if histogramName=="sct_b_xresvsmodeta" or histogramName=="pix_b_xresvsmodeta": 
        histoGram.GetYaxis().SetRangeUser(-.015,.015) #pri
    if histogramName=="sct_b_xresvsmodphi" or histogramName=="sct_eca_xresvsmodphi" or histogramName=="sct_ecc_xresvsmodphi":   
        histoGram.GetYaxis().SetRangeUser(-.01,.01) #pri

    if histogramName=="sct_b_Oxresxvsmodeta_mean" or histogramName=="pix_b_Oxresxvsmodeta_mean" or histogramName=="pix_b_Oyresyvsmodphi_mean":
        histoGram.GetYaxis().SetRangeUser(-.05,.05)
    if histogramName=="pix_b_xresvsmodeta_width" or histogramName=="pix_b_xresvsmodphi_width" or histogramName=="sct_b_xresvsmodeta_width" or histogramName=="sct_b_xresvsmodphi_width" or histogramName=="si_barrel_resX_rms" or histogramName=="si_eca_resX_rms" or histogramName=="si_ecc_resX_rms":
        histoGram.GetYaxis().SetRangeUser(0.00,0.100)
        
    if histogramName=="si_barrel_pullX_mean" or histogramName=="si_eca_pullX_mean" or histogramName=="si_ecc_pullX_mean":
        histoGram.GetYaxis().SetRangeUser(-.4,.4)

    if histogramName=="measurements_eff_vs_layer_ecc" or histogramName=="measurements_eff_vs_layer_eca" or histogramName=="measurements_eff_vs_layer_barrel":
        histoGram.GetYaxis().SetRangeUser(0.9,1.05)
    if  histogramName=="hits_eff_vs_phiSector_trt_b0"  or histogramName=="hits_eff_vs_phiSector_trt_b1" or histogramName=="hits_eff_vs_phiSector_trt_b2":
        histoGram.GetYaxis().SetRangeUser(0.7,1.02)

    if histogramName=="eff_trthits_eta0" or histogramName=="eff_trthits_phi0":
        histoGram.GetYaxis().SetRangeUser(0.6,1.05)
    if histogramName=="trt_b_rmsRes_l0" or histogramName=="trt_b_rmsRes_l1" or histogramName=="trt_b_rmsRes_l2" or histogramName=="trt_b_rmsResOverPhiVsStrawLayer": 
        histoGram.GetYaxis().SetRangeUser(.16,.29)
    if "trt_ec_rms" in histogramName:
        histoGram.GetYaxis().SetRangeUser(.16,.29)
        
    if histogramName=="trt_b_aveResOverPhiVsStrawLayer" or histogramName=="trt_b_aveRes_l0" or histogramName=="trt_b_aveRes_l1" or histogramName=="trt_b_aveRes_l2":
        histoGram.Scale(1000.)
        histoGram.GetYaxis().SetRangeUser(-5,5)
        
    if histogramName=="trt_ec_aveResVsPhiSec_Endcap_C" or histogramName=="trt_ec_aveResVsPhiSec_Endcap_A" or histogramName=="trt_ec_aveResVsRing_Endcap_C" or histogramName=="trt_ec_aveResVsRing_Endcap_A":
        histoGram.Scale(1000.)
        histoGram.GetYaxis().SetRangeUser(-5,5)
    if histogramName=="ntracks":
        histoGram.SetAxisRange(0.,10.)
 #   if histogramName=="pT" or histogramName=="pT_nolog":
 #       histoGram.SetAxisRange(-50.0,50.0)
 # commented for cosmic ray data 2014 (M6) Salvador Marti 24/October/2014
 #if histogramName=="d0":
 #       histoGram.SetAxisRange(-25.0,25.0)
    if histogramName=="d0_bscorr" or histogramName=="d0_pvcorr":
        histoGram.SetAxisRange(-1.0,1.0)
    if histogramName=="z0_pvcorr":
        histoGram.SetAxisRange(-1.0,1.0)

        #if histogramName=="delta_eta0":
        #histoGram.SetAxisRange(-0.02,0.02)
    if histogramName=="delta_d0" or histogramName=="delta_phi0" or histogramName=="delta_eta0" or histogramName=="delta_z0" or histogramName=="delta_qOverPt":
        histoGram.Sumw2()
    if histogramName=="delta_d0" or histogramName=="delta_phi0" or histogramName=="delta_qOverPt":
        histoGram.Rebin(4)
    if "hits_eff" in histogramName:
        histoGram.GetYaxis().SetRangeUser(0.6,1.0)

    #if "err" in histogramName and "VsPt" in histogramName:
    #    newBins = array('d',[0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,12.0,14.0,16.0,18.0,20.0,25.0,30.0,35.0,40.0])
    #    histoGram = histoGram.Rebin(19,"new",newBins)
    #    if "D0" in histogramName:
    #        histoGram.GetYaxis().SetRangeUser(0.0,0.15)


    if "D0bsVs" in histogramName:
        histoGram.GetYaxis().SetRangeUser(-0.04,0.04)
        if "VsPt" in histogramName:
            #newBins = array('d',[-10.0,-8.0,-6.0,-5.0,-4.0,-3.0,-2.0,-1.0,0.0,1.0,2.0,3.0,4.0,5.0,6.0,8.0,10.0])
            #histoGram = histoGram.Rebin(16,"new",newBins)
            histoGram.SetAxisRange(-10.,10.)
        if "VsEta" in histogramName:
            histoGram.SetAxisRange(-2.6,2.6)
    if "residualx_pt" in histogramName:#silicon histograms
        if "mean" in histogramName:
            histoGram.GetYaxis().SetRangeUser(-0.005,0.005)
        if "fwhm" in histogramName:
            if "sct" in histogramName:
                histoGram.GetYaxis().SetRangeUser(0.02,0.05)
            if "pix" in histogramName:
                histoGram.GetYaxis().SetRangeUser(0.005,0.035)
    if "residual_pt" in histogramName:#trt histograms
        if "mean" in histogramName:
            histoGram.GetYaxis().SetRangeUser(-0.005,0.005)
        if "fwhm" in histogramName or "width" in histogramName:
            histoGram.GetYaxis().SetRangeUser(0.100,0.150)
    if "pullx_pt" in histogramName:#silicon histograms
        if "mean" in histogramName:
            histoGram.GetYaxis().SetRangeUser(-0.03,0.03)
        if "width" in histogramName:
            histoGram.GetYaxis().SetRangeUser(0.90,1.05)


            #if "aveResVsPhiSec" in histogramName or "aveResVsRing" in histogramName: 
            #histoGram.GetYaxis().SetRangeUser(-0.005,0.005)       

    if "average_delta_d0" in histogramName or "average_delta_z0" in histogramName:
        histoGram.GetYaxis().SetRangeUser(-.1,.1)
    if "average_delta_phi0" in histogramName:
        histoGram.GetYaxis().SetRangeUser(-.002,.002)  

    if "average_delta_qOverPt" in histogramName:
        histoGram.GetYaxis().SetRangeUser(-.0015,.0015)  
    if "average_delta_eta0" in histogramName:
        histoGram.GetYaxis().SetRangeUser(-.02,.02)  

    if histogramName=="trt_b_residualR" or histogramName=="trt_ec_residualR":
        max_bin =  histoGram.GetMaximumBin()
        max_bin_val = (histoGram.GetBinContent(max_bin)) + histoGram.GetBinError(max_bin)
        histoGram.GetYaxis().SetRangeUser(0.0,max_bin_val + (0.5*max_bin_val))

    #print "bin size for ",histogramName," = " , histoGram.GetBinWidth(50)        


    
    return histoGram

###########################################################################
def GetHistogram3D(rootFile,histogramDir,histogramName):
    histoGram = rootFile.Get(histogramDir + histogramName)
    return histoGram

    
###########################################################################
def RemoveNegativeYAxis(histoGram,histogramName):

    ## This is to ensure that the y-axis doesn't go negative for e.g. residuals plots
    ## (sometimes ROOT likes to do this)
    if histogramName=="pix_b_residualx" or histogramName=="pix_eca_residualx" or histogramName=="pix_ecc_residualx" or histogramName=="sct_b_residualx" or histogramName=="sct_eca_residualx" or histogramName=="sct_ecc_residualx" or histogramName=="trt_b_residualR" or histogramName=="trt_ec_residualR_Endcap_A" or histogramName=="trt_ec_residualR_Endcap_C" or histogramName=="pix_b0_residualx" or histogramName=="pix_b1_residualx" or histogramName=="pix_b2_residualx" or histogramName=="sct_b0_residualx" or histogramName=="sct_b1_residualx" or histogramName=="sct_b2_residualx" or histogramName=="sct_b3_residualx" or histogramName=="delta_qOverPt" or histogramName=="delta_d0" or histogramName=="delta_z0" or histogramName=="delta_phi0" or histogramName=="delta_eta0" or histogramName=="chi2oDoF" or histogramName=="pix_b_residualx_fine" or histogramName=="sct_b_residualx_fine":
        max_bin =  histoGram.GetMaximumBin()
        max_bin_val = (histoGram.GetBinContent(max_bin)) + histoGram.GetBinError(max_bin)
        histoGram.GetYaxis().SetRangeUser(0.0,max_bin_val + (0.1*max_bin_val))

    if histogramName=="trt_b_residualR" or histogramName=="trt_ec_residualR":
        max_bin =  histoGram.GetMaximumBin()
        max_bin_val = (histoGram.GetBinContent(max_bin)) + histoGram.GetBinError(max_bin)
        histoGram.GetYaxis().SetRangeUser(0.0,max_bin_val + (0.5*max_bin_val))

def MakeFit(histoGram,fitType,markerColor):
    
    if fitType=="noFit": #do not perform a fit
        fit = noFit(histoGram)

    elif fitType=="noFitWithStats": #do not perform a fit
        fit = noFitWithStats(histoGram)

    elif fitType=="doubleGaus": #double Gaussian fit
        fit = doubleGaus(histoGram)

    elif fitType=="freeDoubleGaus": #this fit is unstable, not recommended
        fit = freeDoubleGaus(histoGram,[500.,0.,0.05,1000.,0.,0.1])

    elif fitType=="singleGaus": #single Gaussian fit
        fit = singleGaus(histoGram)

    elif fitType=="singleGausRestricted": #single Gaussian fit
        fit = singleGausRestricted(histoGram)

    elif fitType=="singleGausIterative": #single Gaussian fit iterated
        fit = singleGausIterative(histoGram,sigmaIterativeGaus)

    else:
        print "EXITING: fitType - ",fitType," does not exist" 
        sys.exit()

    fit.SetLineColor(markerColor)

    return fit



####################################################################
def defineLegendTextMarker(units, histoGram, funcTion, legendLeftX, legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod,legendMarkerSize):

    debug = False
    if (debug): print " ** defineLegendTextMarker ** START \n     units:", units," histogram:",histoGram," funcTion:",funcTion," showMean:",showMean
    text = ""
    text2 = ""

    if(funcTion.GetName()!=""):
        if(funcTion.GetName()!="noFitWithStats"):

            newMean = funcTion.GetParameter(1)
            newRMS = funcTion.GetParameter(2)

        else: 
            if (debug): print " ** defineLegendTextMarker ** finding mean with method: ", statsMethod
            if statsMethod==0:
                ## Just using straight Mean/RMS for full bin range
                newMean = histoGram.GetMean()
                newRMS = histoGram.GetRMS()

            elif statsMethod==1:
                ### using integral method to determine 95% range for mean/rms calculation
                meanRMSTuple = findMeanRMSFromTruncatedDistribution(histoGram)
                newMean = meanRMSTuple[0]
                newRMS = meanRMSTuple[1]

            elif statsMethod==2:
                ### using a range defined by RMS of histogram
                meanRMSTuple = findMeanRMSUsingRange(histoGram,sigmaIterativeGaus)
                newMean = meanRMSTuple[0]
                newRMS = meanRMSTuple[1]

            elif statsMethod==3:
                ### using FWHM corrected to Gaussian sigma
                meanRMSTuple = findMeanRMSUsingFWHM(histoGram)
                newMean = meanRMSTuple[0]
                newRMS = meanRMSTuple[1]
                print "mean = ",newMean
                print "FWHM/2.35 = ",newRMS

            else:
                print "no valid statsMethod set!!!!"
            
        #adding the mean to the legend text object
        if showMean:
            if (debug): print " ** defineLegendTextMarker ** dealing with showMean"
            text = "#mu="
            if histoGram.GetName()=="delta_phi0" or histoGram.GetName()=="delta_qOverPt" or histoGram.GetName()=="delta_eta0":
                text += str(round(1000*newMean,2))
            elif "pull" in histoGram.GetName():
                text += str(round(newMean,2))
            elif "mean_" in histoGram.GetName():
                text += str(round(newMean,2))
            else:
            #text += str(int(1000*newMean))
                text += str(int(round(newMean,3)*1000))
            text += " "+units
            if (debug): print " ** defineLegendTextMarker ** showMean completed"
            
                


        #adding the "width" to the legend text object
        #unless have requested they are on a separate line
        if meanWidthOnSeparateLine==False:
            if showMean:
                #if "mean_" in histoGram.GetName(): 
                 #   text += "\\n"
                #else:
                text += ", "

            if funcTion.GetName()!="noFitWithStats" : 
                #text += ", #sigma="
                text += "#sigma="
            else:
                if statsMethod==0 or statsMethod==2:
                    text += "RMS="
                if statsMethod==1:
                    text += "RMS_{95%}="
                if statsMethod==3:                
                    text += "FWHM/2.35="

            if histoGram.GetName()=="delta_phi0" or histoGram.GetName()=="delta_qOverPt" or histoGram.GetName()=="delta_eta0":
                text += str(round(1000*newRMS,2))
            elif "pull" in histoGram.GetName():
                text += str(round(newRMS,2))
            elif "mean_" in histoGram.GetName():
                text += str(round(newRMS,2))
            else:
                text += str(int(round(round(newRMS,4)*1000,0))) #will produce correctly rounded to 1 micron accuracy
                #text += str(round(newRMS,4)*1000) # to see 0.1 micron accuracy in width
#                if (int(newRMS*1000/10) > 1):
#                    text += str(int(round(newRMS*1000, 1)))
#                else: ####b priscilla!!!!!!!!!!!!
#                    text += str(round(newRMS*1000, 1))
            text += " "+units
        
        else: #have requested mean width on separate line, so fill separate text object
            if funcTion.GetName()!="noFitWithStats" : 
                text2 += "#sigma="
            else:
                if statsMethod==0 or statsMethod==2:
                    text2 += "RMS="
                if statsMethod==1:
                    text2 += "RMS_{95%}="
                if statsMethod==3:                
                    text2 += "FWHM/2.35="
            if histoGram.GetName()=="delta_phi0" or histoGram.GetName()=="delta_qOverPt" or histoGram.GetName()=="delta_eta0":
                text2 += str(round(1000*newRMS,2))
            elif "pull" in histoGram.GetName():
                text2 += str(round(newRMS,2))
            elif "mean_" in histoGram.GetName():
                text2 += str(round(newRMS,2))
            else:
                text2 += str(int(round(newRMS,3)*1000))
            text2 += " "+units
                
    tsize=0.06
    marker = TMarker(legendLeftX-(0.4*tsize),legendUpperY+legendMarkerYPosMod,8)
    marker.SetNDC()
    marker.SetMarkerStyle(histoGram.GetMarkerStyle())
    marker.SetMarkerSize(legendMarkerSize)
    marker.SetMarkerColor(histoGram.GetMarkerColor())

    return marker, text, text2

def findMeanRMSFromTruncatedDistribution(histoGram):
    # this finds the statistical mean/rms using a truncated distribution such that contains 95% of the entries of the 
    # full range (including overflow/underflow)
    # i.e. the mean and RMS defined using bins which contain 95% of entries (symmetrically) around the mean bin
    # NOTE - if there is significant underflow/overflow this will not be reliable (see explicit check below)

    cloneHist = histoGram.Clone()

    hmean = cloneHist.GetMean()
    print "*************************************************************************"
    print "mean = ",hmean, ", RMS = ",cloneHist.GetRMS(), ", nBins = ",cloneHist.GetNbinsX()
    hFullIntegral = cloneHist.Integral(0,cloneHist.GetNbinsX()+1)
    print "full integral = ",hFullIntegral,", nentries = ",cloneHist.GetEntries()

    print "underflow = ",cloneHist.GetBinContent(0), ", overflow = ",cloneHist.GetBinContent(cloneHist.GetNbinsX()+1)
    totalOverflow = cloneHist.GetBinContent(0) + cloneHist.GetBinContent(cloneHist.GetNbinsX()+1)

    hTargetIntegral = 0.95*hFullIntegral
    print "target integral = ",hTargetIntegral
    hMeanBin = cloneHist.FindBin(hmean)
    print "meanbin = ",hMeanBin

    # this prescription to calculate 95% RMS from a histogram is only valid if the overflow/underflow is 
    # sufficiently small. If overflow/underflow is too large, then the targetIntegral may lie within the underflow/overflow
    # and thus we cannot reliably compute the spread
    if hFullIntegral - hTargetIntegral < totalOverflow:
        print "WARNING - potentially 95% range will go into overflow bins and not be valid. 95% RMS set to zero!!!"
        #return 0.0,0.0
        

    lowBin = hMeanBin
    highBin = hMeanBin
    hIntegral = 0.0
    while hIntegral < hTargetIntegral:
        # find the bin which contains the mean
        # then integrate symmetrically around this bin, increasing the range 
        # until the integral is larger than the target
        # this defines your upper and lower bounds for mean/rms.
        lowBin = lowBin - 1
        highBin = highBin + 1
        hIntegral = cloneHist.Integral(lowBin,highBin)
        #print "integral between bins ",lowBin,"-",highBin," = ",hIntegral

    print "For Histogram " , cloneHist.GetName(), "Mean/RMS bin range is",lowBin,"-",highBin, ", axis range ",cloneHist.GetBinLowEdge(lowBin)," - ", cloneHist.GetBinLowEdge(highBin) + cloneHist.GetBinWidth(highBin)

    cloneHist.GetXaxis().SetRange(lowBin,highBin)
    hNewMean = cloneHist.GetMean()
    hNewRMS = cloneHist.GetRMS()
    print "mean after constrained axis = ",hNewMean
    print "RMS after constrained axis = ",hNewRMS
    
    return hNewMean, hNewRMS
        

####################################################
def findMeanRMSUsingRange(histoGram, sigmaRange):

    cloneHist = histoGram.Clone()

    hMean = cloneHist.GetMean()
    hRMS = cloneHist.GetRMS()
    print "original mean = ", hMean," RMS = ",hRMS

    lowBin = cloneHist.FindBin(hMean - hRMS*sigmaRange)
    highBin = cloneHist.FindBin(hMean + hRMS*sigmaRange)

    #print "lowBin = ",lowBin,", highBin = ",highBin, ", nBins = ", cloneHist.GetNbinsX()
    
    cloneHist.GetXaxis().SetRange(lowBin,highBin)
    hNewMean = cloneHist.GetMean()
    hNewRMS = cloneHist.GetRMS()
    #print "mean after constrained axis = ",hNewMean
    #print "RMS after constrained axis = ",hNewRMS

    return hNewMean, hNewRMS

def findFractionWithinSigmaRange(histoGram, sigmaRange):

    hMean = histoGram.GetMean()
    hRMS = histoGram.GetRMS()
    #print "original mean = ", hMean," RMS = ",hRMS

    lowBin = histoGram.FindBin(hMean - hRMS*sigmaRange)
    highBin = histoGram.FindBin(hMean + hRMS*sigmaRange)

    #print "lowBin = ",lowBin,", highBin = ",highBin, ", nBins = ", histoGram.GetNbinsX()
    
    intGral = histoGram.Integral(lowBin,highBin)
    fullIntGral = histoGram.Integral(1,histoGram.GetNbinsX())
    print "histo ",histoGram.GetName()," fraction covered by fit = ",intGral/fullIntGral

    return intGral/fullIntGral

####################################################################
def findMeanRMSUsingFWHM(histoGram):

    # we clone the histogram here because otherwise the original histogram will be rebinned
    # unfortunately this rebinning happens after the y-axis range has been defined in DrawPlots()
    # and so messes up drawing of histograms
    cloneHist = histoGram.Clone()
    debug = False

    if debug: print " ** findMeanRMSUsingFWHM ** original bin width = ",cloneHist.GetBinWidth(1)


    rebinHistogram = True
    rebinCount = 0
    
    while rebinHistogram==True:
        rebinCount += 1
        rebinHistogram = False

        hMaxBin = cloneHist.GetMaximumBin()
        hMax = cloneHist.GetBinContent(hMaxBin)
        hHalfMax = hMax/2.0

        if debug: print " ** findMeanRMSUsingFWHM ** hNBins = ", cloneHist.GetNbinsX(), ", hMaxBin =  ", hMaxBin, ", hMax = ", hMax, ", hHalfMax = ",hHalfMax

        lowBin = hMaxBin
        highBin = hMaxBin
        hLowContent = hMax
        hHighContent = hMax
        while (hLowContent > hHalfMax and lowBin > 0) :
            lowBin = lowBin - 1
            hLowContent = cloneHist.GetBinContent(lowBin)
            if debug: print "lowcotent =  ",hLowContent
        while (hHighContent > hHalfMax and highBin <= cloneHist.GetNbinsX()):
            highBin = highBin + 1
            hHighContent = cloneHist.GetBinContent(highBin)
            if debug: print "highcotent =  ",hHighContent

        lowBinTest = lowBin - abs(hMaxBin - lowBin)
        highBinTest = highBin + abs(highBin - hMaxBin)

        while lowBinTest < hMaxBin:
            if cloneHist.GetBinContent(lowBinTest) > cloneHist.GetBinContent(lowBinTest+1):
                rebinHistogram = True
                lowBinTest = hMaxBin #exits while loop
            lowBinTest = lowBinTest + 1

        while highBinTest > hMaxBin:
            if cloneHist.GetBinContent(highBinTest) > cloneHist.GetBinContent(highBinTest-1):
                rebinHistogram = True
                highBinTest = hMaxBin #exits while loop
            highBinTest = highBinTest - 1

        if rebinHistogram==True:
            rebinfactor = 2
            if (cloneHist.GetNbinsX() % 2 == 1): rebinfactor = 5
            if (debug): print "rebinning histogram of Nbins: ", cloneHist.GetNbinsX(), " with rebin factor: ",rebinfactor
            cloneHist.Rebin(rebinfactor)



    # analytical solution to linear interpolation between the two bins where the half max threshold is crossed
    # using y1 = A*x1 + B, and y2 = A*x2 + B, solving for A and B
    lowFuncA = (cloneHist.GetBinContent(lowBin) - cloneHist.GetBinContent(lowBin+1))/(cloneHist.GetBinCenter(lowBin) - cloneHist.GetBinCenter(lowBin+1))
    lowFuncB = cloneHist.GetBinContent(lowBin) - lowFuncA*(cloneHist.GetBinCenter(lowBin))

    highFuncA = (cloneHist.GetBinContent(highBin-1) - cloneHist.GetBinContent(highBin))/(cloneHist.GetBinCenter(highBin-1) - cloneHist.GetBinCenter(highBin))
    highFuncB = cloneHist.GetBinContent(highBin-1) - highFuncA*(cloneHist.GetBinCenter(highBin-1))

    lowFWHM = -99999.0
    highFWHM = 99999.0

    if interpolateFWHM:
        # using x = (y - B)/A
        if lowFuncA != 0.0:
            lowFWHM = (hHalfMax - lowFuncB)/lowFuncA
        if highFuncA != 0.0:
            highFWHM = (hHalfMax - highFuncB)/highFuncA
    else:
        lowFWHM = cloneHist.GetBinLowEdge(lowBin) + cloneHist.GetBinWidth(lowBin)
        highFWHM = cloneHist.GetBinLowEdge(highBin)

    if debug: print "lowBin = ", lowBin-hMaxBin, ", lowFWHM =  ", lowFWHM, " || highBin = ",highBin-hMaxBin, ", highFWHM = ", highFWHM, ", FWHM = ",(highFWHM - lowFWHM)/2.35482


    hNewMean = cloneHist.GetMean()
    hNewRMS = (highFWHM - lowFWHM)/2.35482 #divide to make equivalent to Gaussian sigma

    return hNewMean, hNewRMS


def returnTuple(FZ01,hist,legendTitle):

    return FZ01,hist,legendTitle


def noFit(hist):
    #dummy function, does not preform a fit
    FZ01 = TF1()
    return FZ01

def noFitWithStats(hist):
    #dummy function, this fit will not be drawn
    FZ01 = TF1("noFitWithStats","gaus",-1.0,1.0)
    return FZ01


def singleGaus(hist):

    #min = -2.5
    #max = 2.5

    min = hist.GetBinLowEdge(1)
    max = (hist.GetBinLowEdge(hist.GetNbinsX()))+hist.GetBinWidth(hist.GetNbinsX())
    FZ01 = TF1("FZ01", "gaus", min,max)

    FZ01.SetLineWidth(2)
    hist.Fit("FZ01", "ORQ","same")
    
    return FZ01

def singleGausRestricted(hist):

    # performs single Gaussian fit within 1.5*RMS range
    max = hist.GetMean() + 1.5*hist.GetRMS()
    min = hist.GetMean() - 1.5*hist.GetRMS()
    
    FZ01 = TF1("FZ01", "gaus", min,max)

    FZ01.SetLineWidth(2)
    hist.Fit("FZ01", "ORQ","same")
    
    return FZ01

def singleGausIterative(hist, sigmaRange):

    debug = False

    # first perform a single Gaus fit across full range of histogram
    min = hist.GetBinLowEdge(1)
    max = (hist.GetBinLowEdge(hist.GetNbinsX()))+hist.GetBinWidth(hist.GetNbinsX())
    fitA = TF1("fitA", "gaus", min,max)
    hist.Fit("fitA","ORQN","same")
    fitAMean = fitA.GetParameter(1)
    fitASig = fitA.GetParameter(2)

    # performs a second fit with range determined by first fit
    max = fitAMean + (fitASig*sigmaRange)
    min = fitAMean - (fitASig*sigmaRange)
    fitB = TF1("fitB", "gaus", min,max)
    hist.Fit("fitB","ORQN","same")
    fitMean = fitB.GetParameter(1)
    fitSig = fitB.GetParameter(2)
    
    newFitSig = 99999
    newFitMean = 99999
    i = 0
    max = fitMean + (fitSig*sigmaRange)
    min = fitMean - (fitSig*sigmaRange)
    fit = TF1("fit", "gaus", min,max)

    while abs(fitSig - newFitSig) > 0.0005 or abs(fitMean - newFitMean) > 0.0005:
        
        if(i > 0):
            fitMean = newFitMean
            fitSig = newFitSig
        #print "i = ",i," fitMean = ",fitMean," fitSig = ",fitSig
        max = fitMean + (fitSig*sigmaRange)
        min = fitMean - (fitSig*sigmaRange)
        fit.SetRange(min,max)
        hist.Fit("fit","ORQN","same")
        newFitMean = fit.GetParameter(1)
        newFitSig = fit.GetParameter(2)
        #print "i = ",i," newFitMean = ", newFitMean, " newFitSig = ",newFitSig
        if(i > 50):
            if debug:
                print "WARNING terminate iterative gaus fit because of convergence problems"
                print "final mean =  ", newFitMean, ", previous iter mean = ", fitMean
                print "final sigma =  ", newFitSig, ", previous iter sigma = ", fitSig
            break

        i = i + 1



    if debug:
        print "Final i = ",i," finalFitMean = ", fit.GetParameter(1), " finalFitSig = ",fit.GetParameter(2)

    fit.SetLineWidth(2)
    
    return fit


def freeDoubleGaus(hist,fitInput):

    #this fit is not very stable, very sensitive to 
    #initial fitInput parameters (width,mean, normalisation etc)
    #use instead the doubleGaus method below

    min = -0.5
    max = 0.5
    #print "Fitting from",min,"to",max
    FZ01 = TF1("FZ01","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2)",min,max)
    FZ01.SetParameter(0,fitInput[0])
    FZ01.SetParameter(1,fitInput[1])
    FZ01.SetParameter(2,fitInput[2])
    FZ01.SetParameter(3,fitInput[3])
    FZ01.SetParameter(4,fitInput[4])
    FZ01.SetParameter(5,fitInput[5])
    FZ01.SetLineWidth(2)

    hist.Fit("FZ01", "ORQ","same")

    return FZ01

def doubleGaus(hist):

    #first we make a single Gauss fit over entire range
    #to determine the range for the core Gauss fit below
    min_temp = hist.GetBinLowEdge(1)
    max_temp = (hist.GetBinLowEdge(hist.GetNbinsX()))+hist.GetBinWidth(hist.GetNbinsX())
    fit_temp = TF1("fit_temp","gaus",min_temp,max_temp)
    hist.Fit("fit_temp", "RQON")
    fitMean_temp = fit_temp.GetParameter(1)
    fitSig_temp  = fit_temp.GetParameter(2)

    #we make a single Gauss fit to the core
    minCore = fitMean_temp - 1.0*fitSig_temp
    maxCore = fitMean_temp + 1.0*fitSig_temp
    FCore = TF1("FCore","gaus",minCore,maxCore)
    hist.Fit("FCore","RQON")

    #we make the first double Gauss fit, fixing the mean and width of one 
    #of the Gaussians to that of the core Gauss fit, and letting the second
    #Gauss vary but with some sensible starting point
    FFixCore = TF1("FFixCore","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2)",min_temp,max_temp)
    FFixCore.SetParameter(0,FCore.GetParameter(0))
    FFixCore.FixParameter(1,FCore.GetParameter(1))
    FFixCore.FixParameter(2,FCore.GetParameter(2))
    #FFixCore.SetParameter(3,FCore.GetParameter(0)/3.0) #normalisation
    FFixCore.SetParameter(3,FCore.GetParameter(0)/5.0) #normalisation
    FFixCore.SetParameter(4,FCore.GetParameter(1))     #mean
    #FFixCore.SetParameter(5,FCore.GetParameter(2)*5.0) #width
    FFixCore.SetParameter(5,FCore.GetParameter(2)*2.0) #width
    hist.Fit("FFixCore","RQON")
        
    #we do the final double Gaussian fit letting everything vary but using 
    #as inputs the result of the previous constrained fit
    FFull = TF1("FFull","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2)",min_temp,max_temp)
    FFull.SetParameter(0,FFixCore.GetParameter(0))
    FFull.SetParameter(1,FFixCore.GetParameter(1))
    FFull.SetParameter(2,FFixCore.GetParameter(2))
    FFull.SetParameter(3,FFixCore.GetParameter(3))
    FFull.SetParameter(4,FFixCore.GetParameter(4))
    FFull.SetParameter(5,FFixCore.GetParameter(5))
    hist.Fit("FFull","RQO","same")
    
    #print "FFull core mean = ",FFull.GetParameter(1)," width = ",FFull.GetParameter(2)
    #print "FFull tail mean = ",FFull.GetParameter(4)," width = ",FFull.GetParameter(5)
        
    
    return FFull

def doubleFit(hist,units,color,xLeft,yLow,same):
    min_temp = hist.GetBinLowEdge(0)
    max_temp = hist.GetBinLowEdge(hist.GetNbinsX())
    fit_temp = TF1("fit_temp","gaus",min_temp,max_temp)
    if same:
        hist.Fit("fit_temp", "ORQ","same")
    else:
        hist.Fit("fit_temp", "ORQ","")
    fitMean_temp = fit_temp.GetParameter(1)
    fitSig_temp  = fit_temp.GetParameter(2)
    min = fitMean_temp - 1.5* fitSig_temp
    max = fitMean_temp + 1.5 * fitSig_temp
    fit = TF1("fit","gaus",min,max )
    fit.SetLineColor(color)
    if same:
        hist.Fit("fit", "ORQ","same")
    else:
        hist.Fit("fit", "ORQ","")
    fit.Draw("same")

    # Draw the after mean and sigma
    text = "#mu="
    text += str(round(fit.GetParameter(1),5))
    text += " "+units+", #sigma="
    text += str(round(fit.GetParameter(2),4))
    text += " "+units
    m_l = TLatex()
    m_l.SetLineColor(color)
    m_l.SetTextSize(0.03)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.SetTextColor(color)
    print text
    m_l.DrawLatex(xLeft,yLow,text)

def quoteMeanAndRMS(hist,units,color,xLeft,yLow,same):
    
    # Draw the after mean and sigma
    text = "mean = "
    text += str(round(hist.GetMean(),5))
    text += " "+units+", RMS = "
    text += str(round(hist.GetRMS(),4))
    text += " "+units
    m_l = TLatex()
    m_l.SetLineColor(color)
    m_l.SetTextSize(0.03)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.SetTextColor(color)
    print text
    m_l.DrawLatex(xLeft,yLow,text)


def plotStatVersusFile(rootFiles, histDirectories, fileLabels, nFiles, histogramName, meanFWHM=0):

    # plots mean of a particular histogram for different files as separate 
    # entries on x-axis

    statHist = TH1F("statHist","statHist",3,0.,3.)

    for i in range(nFiles):

        hist = GetHistogram(rootFiles[i],histDirectories[i],histogramName,1,29)
        #RemoveNegativeYAxis(hist,histogramName)
        #hist.Scale(2.0)
        mean = hist.GetMean()
        rms = hist.GetRMS()
        entries = hist.GetEntries()
        sqrtEntries = math.sqrt(entries);
        meanErr = rms/sqrtEntries
        print "mean = ", mean, ", rms = ", rms, ", entries = ",entries, ", sqrtEntries = ", sqrtEntries, ", meanErr = ", meanErr
        
        if meanFWHM==0:
            statHist.SetBinContent(i+1,mean)
            statHist.SetBinError(i+1,meanErr)
        else:
            meanFWHMTuple = findMeanRMSUsingFWHM(hist)
            print "mean =", meanFWHMTuple[0]
            print "FWHM/2.35 = ", meanFWHMTuple[1]
            statHist.SetBinContent(i+1,meanFWHMTuple[1])
            statHist.SetBinError(i+1,0.00000001)#no error defined for FWHM

        statHist.GetXaxis().SetBinLabel(i+1,fileLabels[i])
        
    if meanFWHM==0:
        statHist.GetYaxis().SetRangeUser(-0.005,0.005)
    #else:
    #    statHist.GetYaxis().SetRangeUser(0.0,0.2)
    statHist.GetXaxis().SetLabelSize(0.06)


    return statHist


def simpleDrawSingleHist(hist, yAxisTitle, xAxisTitle, plotTitle,  
              canvasText, outputName="test.png", textBoxLeftX=0.60, textBoxUpperY=0.91):

    can = TCanvas(outputName,outputName,800,600)
    can.cd()

    hist.GetXaxis().SetTitle(xAxisTitle)
    hist.GetYaxis().SetTitle(yAxisTitle)
    hist.GetYaxis().SetTitleOffset(1.42)
    hist.Draw()
  
    # canvas text (right hand side)
    latexAtlas = TLatex()
    latexAtlas.SetNDC()
    latexAtlas.SetTextColor(1)
    latexAtlas2 = TLatex()
    latexAtlas2.SetNDC()

    latexAtlas.DrawLatex(textBoxLeftX,textBoxUpperY,canvasText[0])
    latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.06,plotTitle)
    latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.12,canvasText[1])
    latexAtlas2.DrawLatex(textBoxLeftX,textBoxUpperY-0.18,canvasText[2])

    can.SaveAs(outputName)

def Test(histogramName):

    print "histoName = ", histogramName

    if "bin-" in histogramName:
#        binNum = (histogramName.split('-'))
#        print "binNum = ", binNum[1]
        binNum = (histogramName.split('-'))[1]
        print "binNum = ", binNum        

#############################################################################################################
def niceSCTBarrelMap (inputHisto):
    #remove the ugly bin at eta=0 of the sct barrel maps and label the eta rings properly
    debug = False
    outputHisto = inputHisto.Clone()
    if (inputHisto.GetNbinsX() == 13):
        if (debug): print " -- niceSCTBarrelMap -- input histogram ",inputHisto.GetTitle()," has 13 bins"
        outputHisto = TH2D("new"+inputHisto.GetName(), inputHisto.GetTitle(), 12, -6.5, 5.5, 
                           inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax())
        #change the X axis bin labels
        thisetaring = -7 # init with a value one below
        for bin in range(outputHisto.GetNbinsX()):
            thisetaring += 1
            if (thisetaring == 0): thisetaring = 1
            outputHisto.GetXaxis().SetBinLabel(bin+1, str(thisetaring))

        # fill the histogram
        logicbinx = 0
        for binx in range(outputHisto.GetNbinsX()):
            logicbinx +=1
            if (logicbinx == 7): logicbinx += 1 #avoid the bin of the unexistent ring 0
            for biny in range(outputHisto.GetNbinsY()):    
                outputHisto.SetBinContent(binx+1, biny+1, inputHisto.GetBinContent(logicbinx, biny+1)) 

        outputHisto.SetXTitle(inputHisto.GetXaxis().GetTitle())
        outputHisto.SetYTitle(inputHisto.GetYaxis().GetTitle())
                
    return outputHisto

###########################################################################################################
def getPIXEndCapMapHisto (inputHisto, disk):
    
    #split the pixels hit maps histo of the end caps as all the disks are kept in the same histogram
    debug = True
    if (debug): print " -- getPIXEndCapMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()

    outputHisto = TH2F(inputHisto.GetName()+"_d"+str(disk),inputHisto.GetTitle()+" disk "+str(disk), 
                       inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax(),
                       1, inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax()) # same range to facilitate drawing on top

    if (debug): print " -- getPIXEndCapMapHisto -- outputHisto = ",outputHisto.GetName()," xbins:", outputHisto.GetNbinsX()," ybins:",outputHisto.GetNbinsY()
    for i in range(inputHisto.GetNbinsY()):
        outputHisto.SetBinContent(i+1,1,inputHisto.GetBinContent(disk+1,i+1))

    return outputHisto

###########################################################################################################
def getSCTEndCapMapHisto (inputHisto, disk):
    
    #split the pixels hit maps histo of the end caps as all the disks are kept in the same histogram
    debug = False
    if (debug): print " -- getSCTEndCapMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()
        
    #inputHist consist in a TH3 with:
    # x axis is disk (9)
    # y axis is ring (up to 3 rings per disk)
    # z axis is module phi (up to 52 modules per ring)
    outputHisto = TH2F(inputHisto.GetName()+"_d"+str(disk),inputHisto.GetTitle()+" disk "+str(disk), 
                       inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax(),
                       inputHisto.GetNbinsZ(), inputHisto.GetZaxis().GetXmin(), inputHisto.GetZaxis().GetXmax()) 

    if (debug): print " -- getSCTEndCapMapHisto -- outputHisto = ",outputHisto.GetName()," xbins:", outputHisto.GetNbinsX()," ybins:",outputHisto.GetNbinsY()
    for ring in range(outputHisto.GetNbinsX()):
        for module in range(outputHisto.GetNbinsY()):
            outputHisto.SetBinContent(ring+1,module+1,inputHisto.GetBinContent(disk+1,ring+1,module+1))
            if (debug): print " -- getSCTEndCapMapHisto -- disk: ", disk," ring:",ring," phi:",module,"  entries:",inputHisto.GetBinContent(disk+1,ring+1,module+1)

    return outputHisto

###########################################################################################################
def getPIXEndCapResMapHisto (inputHisto, disk):
    
    #split the pixels hit maps histo of the end caps as all the disks are kept in the same histogram
    debug = False
    if (debug): print " -- getPIXEndCapResMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()

    outputHisto = TH2F("d"+str(disk)+"_"+inputHisto.GetName(), inputHisto.GetTitle()+" disk "+str(disk), 
                       inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax(),
                       1, inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax()) # same range to facilitate drawing on top

    if (debug): print " -- getPIXEndCapResMapHisto -- ouputHisto= ", outputHisto.GetName(), "  Xbins=", outputHisto.GetNbinsX(), "  Ybins=", outputHisto.GetNbinsY()    
    for i in range(outputHisto.GetNbinsX()):
        #project the histo per disk and module
        thisHisto = inputHisto.ProjectionZ(inputHisto.GetName()+"_d"+str(disk)+"_m"+str(i),disk+1,disk+1,i+1,i+1)
        if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
            outputHisto.SetBinContent(i+1, 1, 1000.*thisHisto.GetMean())
            if (debug): print " -- getPIXEndCapMapHisto -- disk: ", disk," module: ",i," entries:",thisHisto.GetEntries(),"  res:",1000.*thisHisto.GetMean()," bincontent=",outputHisto.GetBinContent(i+1,disk+1)


    # this is a crosscheck to avoid crashes
    if (outputHisto.GetEntries() == 0): outputHisto.SetBinContent(1,1,0.)
                
    if (debug): print " -- getPIXEndCapResMapHisto -- completed -- "
        
    return outputHisto

######################################################################################################################################        
def MakeHitMaps(histogramDir, legendTitles, rootFiles, fileID, detecName="pixels", barrelEndCap="BAR", unifiedScale = True, coordinate=0, inputType="measurements"):
    # The hit maps have to be plotted for each track collection
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits

    # in this case coordinate is irrelevant for the pixels
    # for sct, coordiante can take two values
    # 0 --> side 0 
    # 1 --> side 1
    debug = True
    totalMaxEntries = -9999999
    totalMinEntries = 99999999

    histoGram = [TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]

    detecName = detecName.upper() # set to upper case
    detecName = detecName[0:3] #truncate
    barrelEndCap = barrelEndCap.upper()
    barrelEndCap = barrelEndCap[0:3]
    if (debug): print " -- MakeHitMaps -- detecName= ",detecName,"  barrelEndCap= ",barrelEndCap

    # handle coordinate
    if (coordinate < 0): coordinate = 0
    if (coordinate > 1): coordinate = 1        

    #handle type
    tsosType = "measurements" # default
    typeName = inputType.lower() # set to lower case    
    typeName = typeName[0:3] #truncate
    if (debug):print " -- MakeHitMaps -- typeName= ", typeName
    if (typeName == "mea"): tsosType = "measurements"  
    if (typeName == "hit"): tsosType = "hits"  
    if (typeName == "hol"): tsosType = "holes"
    if (typeName == "out"): tsosType = "outliers"
                  
    nLayers = 4 #default 4 layers 
    if (detecName=="PIX" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="PIX" and barrelEndCap=="ECA"): nLayers = 3 
    if (detecName=="PIX" and barrelEndCap=="ECC"): nLayers = 3 
    if (detecName=="SCT" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="SCT" and barrelEndCap=="ECA"): nLayers = 9 
    if (detecName=="SCT" and barrelEndCap=="ECC"): nLayers = 9 

    shortName = "measurements_vs_Eta_Phi_pix_b"
    if (detecName=="PIX" and barrelEndCap=="BAR"): shortName = tsosType + "_vs_Eta_Phi_pix_b" 
    if (detecName=="PIX" and barrelEndCap=="ECA"): shortName = "measurements_vs_Eta_Phi_pix_eca"
    if (detecName=="PIX" and barrelEndCap=="ECC"): shortName = "measurements_vs_Eta_Phi_pix_ecc"
    if (detecName=="SCT" and barrelEndCap=="BAR"): shortName = "measurements_vs_Eta_Phi_sct_b" 
    if (detecName=="SCT" and barrelEndCap=="ECA"): shortName = "measurements_vs_Eta_Phi_sct_eca_3d_s" 
    if (detecName=="SCT" and barrelEndCap=="ECC"): shortName = "measurements_vs_Eta_Phi_sct_ecc_3d_s" 
        
        
    # first one has to retrieve all histograms because they may be used to normalise each other, etc.     
    if (detecName == "PIX" and barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i)
            if (debug): print " -- MakeHitMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram[i] = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)
            if (detecName=="SCT" and barrelEndCap=="BAR"): histoGram[i] = niceSCTBarrelMap (histoGram[i])

    if (detecName == "SCT" and barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i) + "_s" +str(coordinate)
            if (debug): print " -- MakeHitMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram[i] = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)
            if (detecName=="SCT" and barrelEndCap=="BAR"): histoGram[i] = niceSCTBarrelMap (histoGram[i])

    if (detecName == "PIX" and barrelEndCap != "BAR"):
        histoEChits = GetHistogram(rootFiles[fileID],histogramDir[fileID],shortName,0,0)             
        for disk in range(nLayers):
            histoGram[disk] = getPIXEndCapMapHisto (histoEChits, disk)

    if (detecName == "SCT" and barrelEndCap != "BAR"):
        myHistoName = shortName + str(coordinate)
        histoEChits = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)             
        print " -- MakeHitMaps -- sct end cap -- shortName = ",shortName,"   layers (disks)= ", nLayers
        for disk in range(nLayers):
            if (debug): print " -- MakeHitMaps -- sct end cap -- retrieving histo for disk: ",disk, " -> ", histoEChits.GetName()
            histoGram[disk] = getSCTEndCapMapHisto (histoEChits, disk)
            
    #Now, the histograms are available
    if (unifiedScale):
        for layer in range(nLayers):
            thismax = histoGram[layer].GetMaximum()
            if (thismax > totalMaxEntries): totalMaxEntries = thismax
            thismin = histoGram[layer].GetMinimum()
            if (thismax < totalMinEntries): totalMinEntries = thismin
        if (debug): print " -- MakeHitMaps -- entries= ",totalMinEntries, " --> ",totalMaxEntries
        # now set the common maximum & minimum
        if (totalMinEntries == 0): totalMinEntries = -1  
        totalMinEntries = 0          
        for layer in range(nLayers):
            histoGram[layer].SetMaximum(totalMaxEntries)
            histoGram[layer].SetMinimum(totalMinEntries)

    # make a tuple object that can be passed to draw method
    totalTuple = (histoGram[0], histoGram[1], histoGram[2], histoGram[3]) # default for 4 layers in the barel        
    if (nLayers == 3): totalTuple = (histoGram[0], histoGram[1], histoGram[2]) # pixel end caps (3 disks)
    if (nLayers == 9): totalTuple = (histoGram[0], histoGram[1], histoGram[2],  histoGram[3],  histoGram[4],  histoGram[5],  histoGram[6],  histoGram[7],  histoGram[8])
        
    return totalTuple #returning histograms and fits

######################################################################################################################################        
def MakeHitEffMaps(histogramDir, legendTitles, rootFiles, fileID, detecName="pixels", barrelEndCap="BAR", unifiedScale = True, coordinate=0):
    # The hit maps have to be plotted for each track collection
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits

    # in this case coordinate is irrelevant for the pixels
    # for sct, coordiante can take two values
    # 0 --> side 0 
    # 1 --> side 1
    debug = True
    totalMaxEntries = -9999999
    totalMinEntries = 99999999

    histoGram = [TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]

    detecName = detecName.upper() # set to upper case
    detecName = detecName[0:3] #truncate
    barrelEndCap = barrelEndCap.upper()
    barrelEndCap = barrelEndCap[0:3]
    if (debug): print " -- MakeHitMaps -- detecName= ",detecName,"  barrelEndCap= ",barrelEndCap

    # handle coordinate
    if (coordinate < 0): coordinate = 0
    if (coordinate > 1): coordinate = 1        
        
    nLayers = 4 #default 4 layers 
    if (detecName=="PIX" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="PIX" and barrelEndCap=="ECA"): nLayers = 3 
    if (detecName=="PIX" and barrelEndCap=="ECC"): nLayers = 3 
    if (detecName=="SCT" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="SCT" and barrelEndCap=="ECA"): nLayers = 9 
    if (detecName=="SCT" and barrelEndCap=="ECC"): nLayers = 9 

    shortName = "measurements_vs_Eta_Phi_pix_b"
    if (detecName=="PIX" and barrelEndCap=="BAR"): shortName = "measurements_eff_vs_Eta_Phi_pix_b" 
    if (detecName=="PIX" and barrelEndCap=="ECA"): shortName = "NO_measurements_vs_Eta_Phi_pix_eca"
    if (detecName=="PIX" and barrelEndCap=="ECC"): shortName = "NO_measurements_vs_Eta_Phi_pix_ecc"
    if (detecName=="SCT" and barrelEndCap=="BAR"): shortName = "measurements_eff_vs_Eta_Phi_sct_b" 
    if (detecName=="SCT" and barrelEndCap=="ECA"): shortName = "NO_measurements_vs_Eta_Phi_sct_eca_3d_s" 
    if (detecName=="SCT" and barrelEndCap=="ECC"): shortName = "NO_measurements_vs_Eta_Phi_sct_ecc_3d_s" 
        
        
    #first have to get all the histograms because they may be used to normalise each other etc     
    if (detecName == "PIX" and barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i)
            if (debug): print " -- MakeHitMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram[i] = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)
            if (detecName=="SCT" and barrelEndCap=="BAR"): histoGram[i] = niceSCTBarrelMap (histoGram[i])

    if (detecName == "SCT" and barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i) + "_s" +str(coordinate)
            if (debug): print " -- MakeHitMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram[i] = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)
            if (detecName=="SCT" and barrelEndCap=="BAR"): histoGram[i] = niceSCTBarrelMap (histoGram[i])

    if (detecName == "PIX" and barrelEndCap != "BAR"):
        histoEChits = GetHistogram(rootFiles[fileID],histogramDir[fileID],shortName,0,0)             
        for disk in range(nLayers):
            histoGram[disk] = getPIXEndCapMapHisto (histoEChits, disk)

    if (detecName == "SCT" and barrelEndCap != "BAR"):
        myHistoName = shortName + str(coordinate)
        histoEChits = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)             
        print " -- MakeHitMaps -- sct end cap -- shortName = ",shortName,"   layers (disks)= ", nLayers
        for disk in range(nLayers):
            if (debug): print " -- MakeHitMaps -- sct end cap -- retrieving histo for disk: ",disk, " -> ", histoEChits.GetName()
            histoGram[disk] = getSCTEndCapMapHisto (histoEChits, disk)
            
    #Now, the histograms are available
    if (unifiedScale):
        for layer in range(nLayers):
            thismax = histoGram[layer].GetMaximum()
            if (thismax > totalMaxEntries): totalMaxEntries = thismax
            thismin = histoGram[layer].GetMinimum()
            if (thismax < totalMinEntries): totalMinEntries = thismin
        if (debug): print " -- MakeHitMaps -- entries= ",totalMinEntries, " --> ",totalMaxEntries
        # now set the common maximum & minimum
        if (totalMinEntries == 0): totalMinEntries = -1 
        totalMaxEntries = 1
        totalMinEntries = 0          
        for layer in range(nLayers):
            histoGram[layer].SetMaximum(totalMaxEntries)
            histoGram[layer].SetMinimum(totalMinEntries)

    # make a tuple object that can be passed to draw method
    totalTuple = (histoGram[0], histoGram[1], histoGram[2], histoGram[3]) # default for 4 layers in the barel        
    if (nLayers == 3): totalTuple = (histoGram[0], histoGram[1], histoGram[2]) # pixel end caps (3 disks)
    if (nLayers == 9): totalTuple = (histoGram[0], histoGram[1], histoGram[2],  histoGram[3],  histoGram[4],  histoGram[5],  histoGram[6],  histoGram[7],  histoGram[8])
        
    return totalTuple #returning histograms and fits

######################################################################################################################################        
def MakeResidualMaps(histogramDir, legendTitles, rootFiles, fileID, detecName="pixels", barrelEndCap="BAR", coordinate=0, unifiedScale = True, zAxisRange= 25):
    # The hit maps have to be plotted for each track collection
    # this gets histograms from the files, normalises if desired and makes fits
    # and returns histograms and fits
    debug = True
    totalMax = -9999999
    totalMin = 99999999
    totalRange = totalMin #intialize to an arbitrary large value

    histoGram = [TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]

    detecName = detecName.upper() # set to upper case
    detecName = detecName[0:3] #truncate
    barrelEndCap = barrelEndCap.upper()
    barrelEndCap = barrelEndCap[0:3]
    myCoordinate = "x" # local x residual 
    if (detecName=="PIX" and coordinate==1): myCoordinate = "y" # local y residual of pixel modules
    
    if (debug): print " -- MakeResidualMaps -- detecName= ",detecName,"  barrelEndCap= ",barrelEndCap
        
    nLayers = 4 #default 4 layers 
    if (detecName=="PIX" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="PIX" and barrelEndCap=="ECA"): nLayers = 3 
    if (detecName=="PIX" and barrelEndCap=="ECC"): nLayers = 3 
    if (detecName=="SCT" and barrelEndCap=="BAR"): nLayers = 4 
    if (detecName=="SCT" and barrelEndCap=="ECA"): nLayers = 9 
    if (detecName=="SCT" and barrelEndCap=="ECC"): nLayers = 9 

    shortName = "pix_b"
    if (detecName=="PIX" and barrelEndCap=="BAR"): shortName = "pix_b" 
    if (detecName=="PIX" and barrelEndCap=="ECA"): shortName = "pix_eca_" # 3d histogram contains 3 disks and 48 modules per disk
    if (detecName=="PIX" and barrelEndCap=="ECC"): shortName = "pix_ecc_" 
    if (detecName=="SCT" and barrelEndCap=="BAR"): shortName = "sct_b" 
    if (detecName=="SCT" and barrelEndCap=="ECA"): shortName = "sct_eca_d" 
    if (detecName=="SCT" and barrelEndCap=="ECC"): shortName = "sct_ecc_d" 
        
        
    #first have to get all the histograms because they may be used to normalise each other etc     
    if (barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i) + "_" + myCoordinate +"resvsmodetaphi_3d"
            if (debug): print " -- MakeResidualMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram3D = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0) # retrieve the 3D histogram
            hname = myHistoName+"_proj"
            htitle = detecName + " residual map " + "(mean)" 
            histoGram[i] = get2DResidualMap(histoGram3D, i)
            if (detecName=="SCT" and barrelEndCap=="BAR"): histoGram[i] = niceSCTBarrelMap (histoGram[i])

    if (detecName == "PIX" and barrelEndCap != "BAR"):
        myHistoName = shortName + myCoordinate + "resvsmodphidisk_3d" 
        histoECResi = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)             
        for disk in range(nLayers):
            histoGram[disk] = getPIXEndCapResMapHisto (histoECResi, disk)

    if (detecName == "SCT" and barrelEndCap != "BAR"):
        for disk in range(nLayers):
            myHistoName = shortName + str(disk) + "_s" + str(coordinate)+ "_xresvsmodetaphi_3d" 
            histoEChits = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)             
            histoGram[disk] = getPIXEndCapResMapHisto (histoEChits, disk)
            
    #Now, the histograms are available
    if (unifiedScale):
        for layer in range(nLayers):
            if (debug): print " -- MakeResidualMaps -- histoGram[",layer,"].GetName()= ",histoGram[layer].GetName()
            thismax = histoGram[layer].GetMaximum()
            if (thismax > totalMax): totalMax = thismax
            thismin = histoGram[layer].GetMinimum()
            if (thismax < totalMin): totalMin = thismin
        if (debug): print " -- MakeResidualMaps -- computed Z range= ",totalMin, " --> ",totalMax
        totalRange = math.fabs(totalMax)
        if (math.fabs(totalMin) > totalRange): totalRange = math.fabs(totalMin)
        if (totalRange > zAxisRange): totalRange = zAxisRange
        if (totalRange < zAxisRange): totalRange = zAxisRange
            
        # now set the common maximum & minimum
        for layer in range(nLayers):
            histoGram[layer].SetMaximum(totalRange)
            histoGram[layer].SetMinimum(-totalRange)
    else:
        # each layer has its own range, but it should be symmetric
        for layer in range(nLayers):
            thismax = histoGram[layer].GetMaximum()
            thismin = histoGram[layer].GetMinimum()
            if (math.fabs(thismin) > math.fabs(thismax)):
                thismax = math.fabs(thismin)
            histoGram[layer].SetMaximum(thismax)
            histoGram[layer].SetMinimum(-thismax)
                  
    # make a tuple object that can be passed to draw method
    totalTuple = (histoGram[0], histoGram[1], histoGram[2], histoGram[3]) # default for 4 layers in the barel        
    if (nLayers == 3): totalTuple = (histoGram[0], histoGram[1], histoGram[2]) # pixel end caps (3 disks)
    if (nLayers == 9): totalTuple = (histoGram[0], histoGram[1], histoGram[2],  histoGram[3],  histoGram[4],  histoGram[5],  histoGram[6],  histoGram[7],  histoGram[8])
        
    return totalTuple #returning histograms and fits

###########################################################################################################################
def getIBLResidualBySensorType(inputHisto, layer, draw3DSensors, drawPlanarSensors, fileID=0, side = "all"):
    sideFor3DSensors = side.upper()
    sideFor3DSensors = sideFor3DSensors[0:3]
    drawCSide = False
    drawASise = False
    if (draw3DSensors and sideFor3DSensors == "ALL"):
        drawCSide = True
        drawASide = True
    if (draw3DSensors and sideFor3DSensors == "ASI"):
        drawCSide = False
        drawASide = True
    if (draw3DSensors and sideFor3DSensors == "CSI"):
        drawCSide = True
        drawASide = False

    # the input histo is a 3D
    hname = inputHisto.GetName() + "_ResMean"
    if (draw3DSensors): hname = hname + "_3D_F" + str(fileID) 
    if (drawPlanarSensors): hname = hname + "_Planar" + str(fileID)     
    htitle = " residual map " + "(mean)" 

    # define the 2d map
    outputHisto = TH2F(hname, htitle, inputHisto.GetXaxis().GetNbins(), 
                       inputHisto.GetXaxis().GetXmin(),
                       inputHisto.GetXaxis().GetXmax(),
                       inputHisto.GetYaxis().GetNbins(), 
                       inputHisto.GetYaxis().GetXmin(),
                       inputHisto.GetYaxis().GetXmax());

    outputHisto1D = TH1F(hname, htitle, inputHisto.GetZaxis().GetNbins(), 
                       inputHisto.GetZaxis().GetXmin(),
                       inputHisto.GetZaxis().GetXmax());
    # fill the map
    for i in range (outputHisto.GetXaxis().GetNbins()):
        for j in range (outputHisto.GetYaxis().GetNbins()):
            thisHisto = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j),i+1,i+1,j+1,j+1)
            #if (i < 4 ): thisHisto = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j), 1, 4, 1, outputHisto.GetYaxis().GetNbins())
            #if (4<= i and i <= 15): thisHisto = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j), 5, 16, 1, outputHisto.GetYaxis().GetNbins())
            #if (16 <= i): thisHisto = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j), 17, 20, 1, outputHisto.GetYaxis().GetNbins())
            if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
                outputHisto.SetBinContent(i+1,j+1,1000.*thisHisto.GetMean())


    if (draw3DSensors): 
        outputHisto1D = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j), 1, 4, 1, outputHisto.GetYaxis().GetNbins())
        outputHisto1D_2 = inputHisto.ProjectionZ(hname+"_zmean2"+str(layer)+str(i)+str(j), 17, 20, 1, outputHisto.GetYaxis().GetNbins())
        if ( not drawCSide): outputHisto1D.Reset()
        if ( not drawASide): outputHisto1D_2.Reset()
        outputHisto1D.Add(outputHisto1D_2)

    if (drawPlanarSensors): 
        outputHisto1D = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j), 5, 16, 1, outputHisto.GetYaxis().GetNbins())

    return outputHisto1D

###########################################################################################################################
def get2DResidualMap(inputHisto, layer):
    # the input histo is a 3D
    hname = inputHisto.GetName() + "_ResMean"
    htitle = " residual map " + "(mean)" 

    # define the 2d map
    outputHisto = TH2F(hname, htitle, inputHisto.GetXaxis().GetNbins(), 
                       inputHisto.GetXaxis().GetXmin(),
                       inputHisto.GetXaxis().GetXmax(),
                       inputHisto.GetYaxis().GetNbins(), 
                       inputHisto.GetYaxis().GetXmin(),
                       inputHisto.GetYaxis().GetXmax());
    # fill the map
    for i in range (outputHisto.GetXaxis().GetNbins()):
        for j in range (outputHisto.GetYaxis().GetNbins()):
            thisHisto = inputHisto.ProjectionZ(hname+"_zmean"+str(layer)+str(i)+str(j),i+1,i+1,j+1,j+1)
            if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
                outputHisto.SetBinContent(i+1,j+1,1000.*thisHisto.GetMean())
    
    return outputHisto
###########################################################################################################################
def PrintHitMapExtraAxis (i, inputHis, detecName = "PIX", barrelEndCap = "BAR"):
    debug = False
    if (barrelEndCap == "BAR" and True):            
        whereInY = inputHis.GetYaxis().GetXmax()
        whereInXmin = inputHis.GetXaxis().GetXmin()
        whereInXmax = inputHis.GetXaxis().GetXmax()
        if (debug): print " WhereInY: ", whereInY, '  whereInXmin:',  whereInXmin,'  whereInXmax:', whereInXmax
        zAxisRange = 840 # in mm and for SCT
        if (detecName == "PIX"): zAxisRange = 400
        if (detecName == "PIX" and i == 0): zAxisRange = 330 #IBL   
        ATLZaxis = TGaxis(whereInXmin,whereInY,whereInXmax,whereInY,-zAxisRange, zAxisRange, 510,"-")
        SetOwnership(ATLZaxis, False)
        ATLZaxis.SetName("Z")
        ATLZaxis.SetLabelSize(inputHis.GetZaxis().GetLabelSize());
        ATLZaxis.SetLabelOffset(0.0);
        ATLZaxis.SetLabelFont(inputHis.GetZaxis().GetLabelFont());
        ATLZaxis.SetTitleSize(inputHis.GetZaxis().GetTitleSize());
        ATLZaxis.SetTitleFont(inputHis.GetZaxis().GetTitleFont());
        ATLZaxis.SetTitle(" z [mm]");
        ATLZaxis.Draw();
    return
            
###########################################################################################################################
def DrawHitMaps(inputTuple, outputName, xAxisTitle, yAxisTitle, zAxisTitle, legendLeftX, legendUpperY, units, 
              canvasText, makeOutput, detecName = "PIX", barrelEndCap = "BAR", paletteStyle = 1):

    debug = False
    # dynamicYRange=True means that the y-range of the histogram drawn first is adjusted so that 
    # all the histograms will fit into this range. If False then the default y-range of the first histogram is used.
    
    detecName = detecName.upper() # set to upper case
    detecName = detecName[0:3] #truncate
    barrelEndCap = barrelEndCap.upper()
    barrelEndCap = barrelEndCap[0:3]
    
    if (debug): print " -- DrawHitMaps -- start --- output name = ",outputName,"   detec=",detecName,"   Barrel/Endcap= ",barrelEndCap 

    nHist = len(inputTuple)
    if (debug): print " -- DrawHitMaps -- nHist= ", nHist,"  inputTuple= ",inputTuple
    
    can = TCanvas(outputName,outputName,1200,900)
    gStyle.SetPadTopMargin(0.08)
    gStyle.SetPadRightMargin(0.12)
    gStyle.SetLabelOffset(0.015,"y")
    gStyle.SetLabelSize(0.045,"x")
    gStyle.SetLabelSize(0.045,"y")
    gStyle.SetPadTickX(0) # no X axis ticks in the top 
    can.Divide(2,2)
    if (detecName == "PIX" and barrelEndCap != "BAR"): 
        del can
        can = TCanvas(outputName,outputName,900,300)
        can.Divide(3,1)

    if (detecName == "SCT" and barrelEndCap != "BAR"): 
        del can
        can = TCanvas(outputName,outputName,900,900)
        can.Divide(3,3)

    if (debug): print " -- DrawHitMaps -- canvas ", outputName," created :)"
        
    if (debug): print " -- DrawHitMaps -- going to loop over ",nHist," histograms of detec=",detecName,"   Barrel/Endcap= ",barrelEndCap 
    # colors
    #gStyle.SetPalette(1) # 53= dark body radiator; 1= standard; 55 = rainbow
    Palette_EffiPlots = TExec("SetPalette_TrafficLights","gStyle->SetPalette(1)")
    if(paletteStyle != 1): Palette_EffiPlots = TExec("Palette_TrafficLights",'TPython::Exec("preparePalette('+str(paletteStyle)+')")') 

    # title of the histograms
    latexTitle = TLatex()
    latexTitle.SetNDC()
    latexTitle.SetTextColor(1)

    for i in range(nHist):
        can.cd(i+1)
        inputTuple[i].GetXaxis().SetTitle(xAxisTitle)
        inputTuple[i].GetYaxis().SetTitle(yAxisTitle)
        inputTuple[i].GetZaxis().SetTitle(zAxisTitle)
        # --> it is not working # gStyle.SetPadTickX(0) # hitmaps have a different axis on top
        if (debug): print " -- DrawHitMaps -- i=",i,"   detec=",detecName,"   Barrel/Endcap= ",barrelEndCap 
        if (detecName == "PIX" and barrelEndCap == "BAR"): 
            inputTuple[i].Draw("colz text")    
            myTitle = "Pixel barrel layer " + str(i-1)
            if (i == 0): myTitle = " IBL " # special case   
        if (detecName == "SCT" and barrelEndCap == "BAR"): 
            inputTuple[i].Draw("colz text")    
            myTitle = "SCT barrel layer " + str(i)
        if (detecName == "PIX" and barrelEndCap != "BAR"): 
            DrawPixelECMap(inputTuple[i])
            myTitle = "Pixel end cap disk " + str(i)
        if (detecName == "SCT" and barrelEndCap != "BAR"): 
            DrawSCTECMap(inputTuple[i], i)
            myTitle = "SCT end cap disk " + str(i)
        Palette_EffiPlots.Draw();
        gPad.Update()
        PrintHitMapExtraAxis(i, inputTuple[i], detecName, barrelEndCap)
            
        # legend
        if (debug): print " -- DrawHitMaps -- histogram title =",myTitle 
        latexTitle.DrawLatex(legendLeftX, legendUpperY, myTitle)
           
            
            
    if (debug): print " -- DrawHitMaps -- completed -- "    
    if makeOutput:
        can.SaveAs(outputName)

    return

###########################################################################################################################
def DrawSCTECMap(inputHisto, disk):
    debug = False
    if (debug): print " -- DrawSCTECMap -- start -- for disk ", disk, " entries:",inputHisto.GetEntries()

    if (debug): print "define boxsct"
    boxsct = TH2F(inputHisto.GetName()+"_box_"+str(disk), inputHisto.GetTitle(), 52, -10, 10, 52, -10, 10);
    for i in range(inputHisto.GetNbinsX()):
        for j in range(inputHisto.GetNbinsY()):
            boxsct.SetBinContent(i+1,j+1,inputHisto.GetBinContent(i+1,j+1))
    boxsct.GetXaxis().SetLabelColor(kWhite)
    boxsct.GetYaxis().SetLabelColor(kWhite)
    boxsct.GetXaxis().SetTitle("X axis")
    boxsct.GetYaxis().SetTitle("Y axis")
    boxsct.SetMaximum(inputHisto.GetMaximum())
    boxsct.SetMinimum(inputHisto.GetMinimum())
    SetOwnership(boxsct, False)
    boxsct.Draw("colz text")    
    gPad.Update()
    
    #now draw a blank on top
    blank = TBox(boxsct.GetXaxis().GetXmin(),boxsct.GetYaxis().GetXmin(), boxsct.GetXaxis().GetXmax(), boxsct.GetYaxis().GetXmax())
    blank.SetFillStyle(1001) # solid
    blank.SetFillColor(kWhite)
    SetOwnership(blank, False)
    blank.Draw()
    gPad.Update()
    blank.Print()

    # Let's draw the wheels 
    # -parameters
    nrings = [2,3,3,3,3,3,2,2,1] # number of rings per disk     
    nmods = [52, 40, 40] # number of modules in each ring (outer, middle, inner) 
  
    inner_radius_f = [0.75, 0.49, 0.34];
    outer_radius_f = [0.99, 0.74, 0.48];
    if (disk==7): inner_radius_f[1]= 0.60;   # the one before the last wheel has short middle modules


    for ring in range (nrings[disk]):    
        if (debug): print " -- DrawSCTECMap -- ring: ", ring, "  modules= ", nmods[ring]
        phistep = 2*3.14159265/nmods[ring]
        outerR = outer_radius_f[ring] * (boxsct.GetXaxis().GetXmax()-boxsct.GetXaxis().GetXmin())/2
        innerR = inner_radius_f[ring] * (boxsct.GetXaxis().GetXmax()-boxsct.GetXaxis().GetXmin())/2
        xCenter = (boxsct.GetXaxis().GetXmax()-boxsct.GetXaxis().GetXmin())/2 + boxsct.GetXaxis().GetXmin()
        yCenter = (boxsct.GetYaxis().GetXmax()-boxsct.GetYaxis().GetXmin())/2 + boxsct.GetYaxis().GetXmin()
        xmod = array("f",[0.0]*5)
        ymod = array("f",[0.0]*5) 
        pmod = [None] * nmods[ring]
        for module in range(nmods[ring]):
            phi = module * phistep
            xmod[0] = xCenter + innerR*math.cos(phi-phistep/2)
            xmod[1] = xCenter + innerR*math.cos(phi+phistep/2)
            xmod[2] = xCenter + outerR*math.cos(phi+phistep/2)
            xmod[3] = xCenter + outerR*math.cos(phi-phistep/2)
            xmod[4] = xmod[0]
            ymod[0] = yCenter + innerR*math.sin(phi-phistep/2)
            ymod[1] = yCenter + innerR*math.sin(phi+phistep/2)
            ymod[2] = yCenter + outerR*math.sin(phi+phistep/2)
            ymod[3] = yCenter + outerR*math.sin(phi-phistep/2)
            ymod[4] = ymod[0]
            if (debug): print " -- DrawSCTECMap -- disk:",disk," ring:",ring," module:",module,"  entries:",inputHisto.GetBinContent(ring+1,module+1)
            pmod[module] = TPolyLine(5, xmod, ymod)
            pmod[module].SetFillColor(locateColor(boxsct,ring+1,module+1))
            pmod[module].Draw("Fsame")    
            SetOwnership(pmod[module], False)

    gPad.Modified()    
    gPad.Update()
    if (debug): print " -- DrawSCTECMap -- completed -- disk ", disk

    return

###########################################################################################################################
def DrawPixelECMap(inputHisto):
    debug = False
    if (debug): print " -- DrawPixelECMap -- start -- inputHisto=",inputHisto.GetName()

    # hide the labels
    inputHisto.GetXaxis().SetLabelColor(kWhite)
    inputHisto.GetYaxis().SetLabelColor(kWhite)
    inputHisto.GetXaxis().SetTitle("X axis")
    inputHisto.GetYaxis().SetTitle("Y axis")
    # first draw the histogram in colz mode to obtain the Z axis scale
    inputHisto.Draw("colz text")

    #now draw a blank on top
    blank = TBox(inputHisto.GetXaxis().GetXmin(),inputHisto.GetYaxis().GetXmin(), inputHisto.GetXaxis().GetXmax(), inputHisto.GetYaxis().GetXmax())
    blank.SetFillStyle(1001) # solid
    blank.SetFillColor(kWhite)
    SetOwnership(blank, False)
    blank.Draw()
    gPad.Update()
    if (debug): blank.Print()

    # Let's draw the wheel
    # -parameters
    npixmods = 48 # each wheel has 48 modules 
    phistep = 2*3.14159265/npixmods
    outerR = (inputHisto.GetXaxis().GetXmax()-inputHisto.GetXaxis().GetXmin())/2
    innerR = outerR/2
    xCenter = (inputHisto.GetXaxis().GetXmax()-inputHisto.GetXaxis().GetXmin())/2
    yCenter = (inputHisto.GetYaxis().GetXmax()-inputHisto.GetYaxis().GetXmin())/2
    xmod = array("f",[0.0]*5)
    ymod = array("f",[0.0]*5) 
    pmod = [None] * npixmods
    for module in range(npixmods):
        phi = module * phistep
        xmod[0] = xCenter + innerR*math.cos(phi-phistep/2)
        xmod[1] = xCenter + innerR*math.cos(phi+phistep/2)
        xmod[2] = xCenter + outerR*math.cos(phi+phistep/2)
        xmod[3] = xCenter + outerR*math.cos(phi-phistep/2)
        xmod[4] = xmod[0]
        ymod[0] = yCenter + innerR*math.sin(phi-phistep/2)
        ymod[1] = yCenter + innerR*math.sin(phi+phistep/2)
        ymod[2] = yCenter + outerR*math.sin(phi+phistep/2)
        ymod[3] = yCenter + outerR*math.sin(phi-phistep/2)
        ymod[4] = ymod[0]
        if (debug): print " -- DrawPixelECMap -- module ", module, "  (x0,y0) = (",xmod[0],", ",ymod[0],") "  
        pmod[module] = TPolyLine(5, xmod, ymod)
        pmod[module].SetFillColor(locateColor(inputHisto,module+1))
        pmod[module].Draw("Fsame")    
        SetOwnership(pmod[module], False)

    gPad.Modified()    
    gPad.Update()
    if (debug): print " -- DrawPixelECMap -- completed -- "

    return

###########################################################################################################################
def locateColor(inputHisto, xbin, ybin=1):

    colorIndex = (inputHisto.GetListOfFunctions().FindObject("palette")).GetValueColor(inputHisto.GetBinContent(xbin,ybin))

    return colorIndex
    

