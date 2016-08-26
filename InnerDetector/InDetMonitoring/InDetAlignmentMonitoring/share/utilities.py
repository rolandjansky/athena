# Author:
#  John Alison johnda@hep.upenn.edu
#  Ben Cooper b.d.cooper@qmul.ac.uk


import sys, math
from array import array

from ROOT import *
import AtlasStyle 
TGaxis.SetMaxDigits(4)

# if this is true then legends are drawn in order of the input files
# if it is false then legends are drawn in order of max histogram first
# no need for this ever to be False really
forceDrawOrder = True

normaliserHisto = 0 #histogram which other hists are normalised to if normalisation is used. e.g. 0 is hist from first file
statsMethod = 3 #how width is calculated if don't use fit. 0 = plain RMS, 1 = RMS(95%), 2 = RMS within range (sigmaIterativeGaus), 3 = FWHM/2.35
interpolateFWHM = True # use linear interpolation in FWHM estimation
sigmaIterativeGaus = 1.5 #controls sigma range used for iterative single Gaussian fit
ZmumuVal = False #flag for ZmumuValidation particular things (axes range etc)
pTmin = 0       #the minimum pT for ZmumuValidation
pTmax = 100     #the maximum pT for ZmumuValidation
MinEntriesPerModule = 5 #Min number of entries per module to compute the residual maps values

theInputFileList = []
theLegendList = []
theMarkerList = []
theColorList = []
theTrackCollectionList = []
thePrefix = []

z_fix=366.5

##########################################################################################################
def DrawPlots(inputTuple, outputName, plotTitle, yAxisTitle, xAxisTitle, legendLeftX, legendUpperY, units, 
              canvasText, makeOutput, textBoxLeftX=0.59, textBoxUpperY=0.87, dynamicYRange=True, plotCosmetics="Default"):

    debug = False
    if (debug): print " \n <DrawPlots> -- start -- drawing \n   inputTuple = ",inputTuple

    # dynamicYRange=True means that the y-range of the histogram drawn first is adjusted so that 
    # all the histograms will fit into this range. If False then the default y-range of the first histogram is used.

    #print MakeMajorAlignMonPlots.userCosmetics
    userCosmetics = "SmallLegend" # trying to pass a global variable... still I don't know how (Salva 20/May/2015)
    if (len(userCosmetics)==0): 
        plotCosmetics = "Default"
    else:
        plotCosmetics = userCosmetics
    
    if(plotCosmetics=="Default"):
        legendMarkerSize = 1.5
        legendTextSize = 0.04
        legendMarkerSize = 1.25
        legendYLineSpacing = 0.05 # determines how spaced out in y the legend entries are (may want to increase/decrease depending on legend text size)
        legendYLineSpacing = 0.045 # determines how spaced out in y the legend entries are (may want to increase/decrease depending on legend text size)
        legendMarkerYPosMod = [0.0,0.0,0.0,0.0] #for some reason the marker doesn't appear next to legend text in Y without some modification
        meanWidthOnSeparateLine = True# if true mean and width are displayed in legend on separate lines, if false they are on the same line
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
        legendTextSize = 0.025
        legendMarkerSize = 1.5
        legendYLineSpacing = 0.035
        legendMarkerYPosMod = [-0.002,0.0,0.0,0.0]
        meanWidthOnSeparateLine = False
        showMean = True
        plotTitleOnFirstLine = False
        yAxisTitleOffset = 1.25 # NOT USED now handled by AtlasStyle

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
    if (debug): print " <DrawPlots> cosmetics set and canvas already open :)" 

        
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
    if (debug or True): print " <DrawPlots> maxYVal = ",maxYVal,", minYVal = ",minYVal

    # drawing the first histogram
    hist = inputTuple[1]
    if (debug): print " <DrawPlots> going to draw ", hist, " ....." 
    if hist.GetName()=="pT":
        gPad.SetLogy()
        #I was asked to set y linear 19/06/13
        # gPad.SetLogy()
        #I am trying to fix the X range for Pt plots. Please do it in a better way!! (PF 28/03/13)
    if hist.GetName()=="pT_n":
        gPad.SetLogy()
    if hist.GetName()=="pT_p":
        gPad.SetLogy()

    histoTitle = hist.GetName()
    #if histoTitle.find('pT')!=-1  and ZmumuVal:
    #    hist.GetXaxis().SetRangeUser(pTmin,pTmax)
    #hist.GetXaxis().SetRangeUser(-0.4,0.4)
    if dynamicYRange:
        if hist.GetName()=="pix_b_residualy":
            hist.GetYaxis().SetRangeUser(minYVal,maxYVal*1.33)
        else:
            thisMin = minYVal*0.7+0.01
            thisMax = maxYVal*1.38
            if (debug): print " <DrawPlots> DynamicRange = False --> user range from:",thisMin, " --> ", thisMax
            #hist.GetYaxis().SetRangeUser(minYVal*1.10,maxYVal*1.10)
            hist.GetYaxis().SetRangeUser(thisMin, thisMax)
            
            
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
        if (debug): print " <DrawPlots> going to draw ",hist.GetName()
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
    SetOwnership(marker, False)

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
        if histoTitle.find('pT')!=-1 and ZmumuVal:
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

    # drawing fith histogram and its legend
    if (len(inputTuple)) > 12:
        hist = inputTuple[12+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[12+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[12+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker5 = legendTuple[0]
        marker5.Draw("same")

    # drawing sixth histogram and its legend
    if (len(inputTuple)) > 15:
        hist = inputTuple[15+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[12+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[15+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker6 = legendTuple[0]
        marker6.Draw("same")

    # drawing seventh histogram and its legend
    if (len(inputTuple)) > 18:
        hist = inputTuple[18+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[15+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[18+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker7 = legendTuple[0]
        marker7.Draw("same")

    # drawing eigth histogram and its legend
    if (len(inputTuple)) > 21:
        hist = inputTuple[21+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[18+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[21+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker8 = legendTuple[0]
        marker8.Draw("same")

    # drawing nineth histogram and its legend
    if (len(inputTuple)) > 24:
        hist = inputTuple[24+1]
        if histoTitle.find('pT')!=-1 and ZmumuVal:
            hist.GetXaxis().SetRangeUser(pTmin,pTmax)
        if "mean_" in hist.GetName():
            hist.Draw("histosame")
        else:
            hist.Draw("same,e")
            #hist.Draw("same,e")
        tf1 = inputTuple[21+0]
        if(tf1.GetName()!="noFitWithStats"): tf1.Draw("same")

        legendUpperY = legendUpperY - legendYOffset
        legendTitle = inputTuple[24+2]
        m_l.DrawLatex(legendLeftX,legendUpperY,legendTitle)
        legendTuple = defineLegendTextMarker(units, hist, tf1,legendLeftX,legendUpperY,showMean,meanWidthOnSeparateLine,legendMarkerYPosMod[3],legendMarkerSize)
        m_l2.DrawLatex(legendLeftX,legendUpperY-legendYLineSpacing,legendTuple[1])
        if meanWidthOnSeparateLine==True:
            m_width.DrawLatex(legendLeftX,legendUpperY-(2*legendYLineSpacing),legendTuple[2])
        marker9 = legendTuple[0]
        marker9.Draw("same")

    if (debug): print " <DrawPlots> going to save file ... as ", outputName
    print " <DrawPlots> going to save file ... as ", outputName

    if makeOutput:
        can.SaveAs(outputName)
        print " <DrawPlots> file ", outputName, " succesfuly stored "

    if (debug): print " <DrawPlots> -- completed -- "
    return can    
        
##########################################################################################################
def DrawEvolutionPlot(inputTuple, outputName, plotTitle, yAxisTitle, legendLeftX, legendUpperY, units, 
              canvasText, makeOutput, textBoxLeftX=0.59, textBoxUpperY=0.87, dynamicYRange=True, plotCosmetics="Default"):
    debug = False
    if (debug): print " \n <DrawEvolutionPlots> -- start -- drawing \n   inputTuple = ",inputTuple

    nPoints = len(inputTuple)/3 # for each file there are 3 entries in the Tuple
    if (debug): print " \n <DrawEvolutionPlots> nPoints = ",nPoints

    # capture the histograms    
    histoGram = [TH1,TH1,TH1,TH1,TH1,TH1,TH1,TH1,TH1]
    theLegend = []
    for i in range(0,nPoints):
        j = 1 + i*3
        histoGram[i] = inputTuple[j]
        theLegend.append(inputTuple[j+1])
        print " capturing histo ", i, " at index ",j, "  histo -> ", histoGram[i].GetName(), "  legend: ",theLegend[i]
        
    #create the histogram to store the evolution. 
    # one entry per file
    evolutionHisto = TH1F("evolution_"+histoGram[0].GetName(), "Evolution of "+histoGram[0].GetTitle(), nPoints, -0.5, nPoints-0.5)                                 

    # change the x-axis bin labels
    for i in range(0,nPoints):
      evolutionHisto.GetXaxis().SetBinLabel(i+1, str(theLegend[i]))

    # Y axis title  
    evolutionHisto.GetYaxis().SetTitle(yAxisTitle)
        
    theMaximum = 0.
    # fill the evolution histogram 
    for i in range(0,nPoints):
        newval = histoGram[i].GetMean()
        newerr = histoGram[i].GetRMS()/math.sqrt(histoGram[i].GetEntries())
        thismax = abs(newval) + abs(newerr)
        evolutionHisto.SetBinContent(i+1, newval) 
        evolutionHisto.SetBinError(i+1, newerr) 
        if (thismax >theMaximum): theMaximum = thismax

    # rescale the maximum
    theMaximum *= 1.10
    
    # symmetrize range
    evolutionHisto.SetMaximum(theMaximum)
    evolutionHisto.SetMinimum(-theMaximum)

    # open the canvas
    canvasName = "evolutionCanvas_"+outputName    
    can = TCanvas(canvasName,outputName,800,600)
    can.cd()
    evolutionHisto.Draw()

    if makeOutput:
        can.SaveAs(outputName)
        
    if (debug): print " <DrawEvolutionPlot> -- completed -- "
    return can
###########################################################################################################################################
def MakePlots2D(histogramDir,legendTitles,markerColors,markerStyles,histogramName,rootFiles,nFiles,profile=False):
    debug = False
    maxval = 0.0
    max_hist = 0

    histoGram = [TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2,TH2]
    if profile:
        histoGram = [TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D,TProfile2D]
    Tuples = []
    
    for i in range(nFiles):
        print "<MakePlots2D> === ", histogramName, "==="
        histoGram[i] = GetHistogram(rootFiles[i],histogramDir[i],histogramName,markerColors[i],markerStyles[i], i)
        Tuples.append(histoGram[i])
        
    return Tuples

def DrawPlots2D(inputTuple, outputDir,outputName, xAxisTitle, yAxisTitle, legendLeftX, legendUpperY, units, 
              canvasText, makeOutput,textBoxLeftX=0.59, textBoxUpperY=0.87, dynamicYRange=True, plotCosmetics="Default"):

    debug = False

    nHist = len(inputTuple)
    
    for i in range(nHist):
        can = TCanvas("Canvas_"+str(i),"Canvas_"+str(i),1200,900)
        gStyle.SetPadTopMargin(0.08)
        gStyle.SetPadRightMargin(0.12)
        gStyle.SetLabelOffset(0.015,"y")
        gStyle.SetLabelSize(0.045,"x")
        gStyle.SetLabelSize(0.045,"y")
        Palette_EffiPlots = TExec("SetPalette_TrafficLights","gStyle->SetPalette(1)")
        #if(paletteStyle != 1):
        #    Palette_EffiPlots = TExec("Palette_TrafficLights",'TPython::Exec("preparePalette('+str(paletteStyle)+')")')
    
        myDrawOptions = "colz"
        inputTuple[i].GetXaxis().SetTitle(xAxisTitle)
        inputTuple[i].GetYaxis().SetTitle(yAxisTitle)
        inputTuple[i].GetZaxis().SetRangeUser(-0.03,0.03)
        inputTuple[i].Draw(myDrawOptions)
        Palette_EffiPlots.Draw();
        gPad.Update()
        
        if makeOutput:
            can.SaveAs(outputDir+"File_"+str(i)+"_"+outputName)

    return
###########################
def MakePlots(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles, normaliseHistos=False, unitArea=False):

    debug = False
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH1,TH1,TH1,TH1,TH1,TH1,TH1,TH1,TH1]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakePlots> ===  ",histogramName,"  ==="
        if (debug): 
            print " <MakePlots> retriveing ",histogramName," from file i= ",i," --> ",rootFiles[i]
            print " length of rootFiles array: ", len(rootFiles) ,"  rootFile[",i,"] =", rootFiles[i]
            print " length of histogramDir array: ", len(histogramDir) 
            print " length of histoGram array: ", len(histoGram)     
            print " length of markerColors array: ", len(markerColors) ,"   color[",i,"] =", markerColors[i]
            print " length of markerStyles array: ", len(markerStyles) ,"   style[",i,"] =", markerStyles[i]
            print " histogramName: ", histogramName
            print " GetHistogram(",rootFiles[i],",", histogramDir[i],")"
        histoGram[i] = GetHistogram(rootFiles[i],histogramDir[i],histogramName,markerColors[i],markerStyles[i], i)
        
    for i in range(nFiles):
        #normalise histograms to unit area if desired
        if histoGram[i].Integral() > 0:
            if unitArea:
                #print "for hist ",i, " scaling" 
                histoGram[i].Scale(1/histoGram[i].Integral())
                histoGram[i].GetYaxis().SetTitle("Arbitrary units")
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
    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
    if nFiles==7:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6]
        elif max_hist==6:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
    if nFiles==8:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
    if nFiles==9:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[8] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]





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
    
    histoGram = [TH2,TH2,TH2, TH2,TH2,TH2, TH2,TH2,TH2]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1, TH1, TH1, TH1, TH1]
    myProfile = [TProfile,TProfile,TProfile,TProfile,TProfile,TProfile,TProfile,TProfile,TProfile]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]

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
    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
    if nFiles==7:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6]
        elif max_hist==6:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
    if nFiles==8:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
    if nFiles==9:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[8] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]



    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################   

###########################################################################################################################################        
def MakeProfSigmaPlots(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles, symmetricRange=False):

    # this function takes as argument a TH2 and draws the mean profile or the rms profile 
    debug = False
    normaliseHistos = False # not normalization
    unitArea = False # not unit area

    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH2,TH2,TH2,TH2,TH2, TH2]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1, TH1]
    myProfile = [TProfile,TProfile,TProfile,TProfile,TProfile, TProfile]
    #myProject = [TH1, TH1, TH1, TH1, TH1, TH1]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeProfSigmaPlots> ===  ",histogramName,"  ==="
        if (debug): print " <MakeProfSigmaPlots> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
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
            #returnHistogram[i].SetBinContent(bin+1, myProfile[i].GetBinContent(bin+1))
            #returnHistogram[i].SetBinError(bin+1, myProfile[i].GetBinError(bin+1))
            returnHistogram[i].SetBinContent(bin+1, histoGram[i].ProjectionY("_py",bin+1,bin+1).GetRMS())
            returnHistogram[i].SetBinError(bin+1, histoGram[i].ProjectionY("_py",bin+1,bin+1).GetRMSError())
        
        # perform the deired fit to the histogram
        fit = MakeFit(returnHistogram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,returnHistogram[i],legendTitles[i])

    '''#unify range
    if (debug): print " <MakeProfSigmaPlots> maxval = ",maxval
    for i in range(nFiles):
        if (symmetricRange): 
            returnHistogram[i].GetYaxis().SetRangeUser(-maxval,maxval)
        else:
            returnHistogram[i].GetYaxis().SetRangeUser(0,maxval)'''
        

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
    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]




    if (debug): print "  <MakeProfSigmaPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################   
     
def MakePlotsFrom3D(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles,  symmetricRange=False, binRangeLower=-1, binRangeUpper=-1):

    # this function takes as argument a TH3 and obtains the profile in one of its axis. 
    debug = False
    normaliseHistos = True # not normalization
    unitArea = False # not unit area

    # in case we limit the range of bins
    rangeLimit = False
    if (binRangeLower > 0): rangeLimit = True
    if (binRangeUpper >0 and binRangeUpper >= binRangeLower): rangeLimit = True     
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH3,TH3,TH3,TH3,TH3,TH3]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1,TH1]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple]
    
    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeProfPlotsFrom3D> ===  ",histogramName,"  ==="
        if (debug): print " <MakeProfPlotsFrom3D> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] =GetHistogram3D(rootFiles[i],histogramDir[i],histogramName)


    for i in range(nFiles):
        # make the profile
        if (rangeLimit): histoGram[i].GetYaxis().SetRange(binRangeLower,binRangeUpper);
        hname = "residuals_" + str(i)    
        returnHistogram[i] = histoGram[i].ProjectionZ(hname, 4, 10, binRangeLower, binRangeUpper)            
            
        # find which histogram has largest y-value - this will be drawn first
        if returnHistogram[i].GetMaximum() > maxval:
            maxval = returnHistogram[i].GetMaximum()
            max_hist = i
        if returnHistogram[i].GetMinimum() < -maxval:
            maxval = -returnHistogram[i].GetMinimum()
            max_hist = i
        maxval = 1.05 * maxval

        # build the returned histograms
        returnHistogram[i].SetMarkerStyle(markerStyles[i])
        returnHistogram[i].SetMarkerColor(markerColors[i])
        returnHistogram[i].SetLineColor(markerColors[i])

        # perform the desired fit to the histogram
        fit = MakeFit(returnHistogram[i],fitType,markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,returnHistogram[i],legendTitles[i])

    #unify range
    if (debug): print " <MakePlotsFrom3D> maxval = ",maxval
    for i in range(nFiles):
        if (symmetricRange): 
            if (debug): print " <MakePlotsFrom3D> symmetric range with maxval = ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(-maxval,maxval)
            #returnHistogram[i].GetYaxis().SetRangeUser(-maxval/4,maxval)
        else:
            if (debug): print " <MakePlotsFrom3D> range from 0 --> maxval = ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(0,maxval)
        if (normaliseHistos):
            returnHistogram[i].Scale(returnHistogram[normaliserHisto].Integral()/returnHistogram[i].Integral())            

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

    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]



    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################        
def MakeProfPlotsFrom3D(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles,  symmetricRange=False, binRangeLower=-1, binRangeUpper=-1):

    # this function takes as argument a TH3 and obtains the profile in one of its axis. 
    debug = False
    normaliseHistos = False # not normalization
    unitArea = False # not unit area

    # make sure the styles are integers
    markerStyle = []
    for marker in markerStyles:
        markerStyle.append(int(marker))
    markerColor = []
    for marker in markerColors:
        markerColor.append(int(marker))
    
    # in case we limit the range of bins
    rangeLimit = False
    if (binRangeLower > 0): rangeLimit = True
    if (binRangeUpper >0 and binRangeUpper >= binRangeLower): rangeLimit = True     
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    max_hist = 0
    
    histoGram = [TH3,TH3,TH3,TH3,TH3,TH3, TH3,TH3,TH3]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1,TH1, TH1, TH1, TH1]
    myProfile = [TH2D,TH2D,TH2D,TH2D,TH2D,TH2D, TH2D, TH2D, TH2D]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple, tuple, tuple, tuple]
    tempProf = [TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F]
    
    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeProfPlotsFrom3D> ===  ",histogramName,"  ==="
        if (debug): print " <MakeProfPlotsFrom3D> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] =GetHistogram3D(rootFiles[i],histogramDir[i],histogramName)
        thisName = "IBLStavesSagitta_File_"+str(i)
        tempProf[i] = TH1F(thisName,"IBL staves residuals",histoGram[i].GetXaxis().GetNbins(),histoGram[i].GetXaxis().GetXmin(),histoGram[i].GetXaxis().GetXmin());


    SubtractFirstHistoTest = False

    
    for i in range(nFiles):
        # make the profile
        if (rangeLimit): histoGram[i].GetYaxis().SetRange(binRangeLower,binRangeUpper);
        myProfile[i] = histoGram[i].Project3D("ZX")
        #now project ring by ring
        for ring in range(myProfile[i].GetNbinsX()):
            residualsdist = myProfile[i].ProjectionY("res_by_ring",ring+1,ring+1)
            if (debug): print ">> ring ",ring, " mean = ", residualsdist.GetMean(), "+-", residualsdist.GetMeanError()
            tempProf[i].SetBinContent(ring+1, residualsdist.GetMean())
            tempProf[i].SetBinError(ring+1, residualsdist.GetMeanError())
            
        if (SubtractFirstHistoTest and i==0):
            #copy thishisto as reference
            RefHisto = tempProf[i].Clone()
        if (SubtractFirstHistoTest):
            tempProf[i].Add(RefHisto,-1)
            tempProf[i].Fit("pol0")

            
        # find which histogram has largest y-value - this will be drawn first
        if tempProf[i].GetMaximum() > maxval:
            maxval = tempProf[i].GetMaximum()
        if tempProf[i].GetMinimum() < -maxval:
            maxval = -tempProf[i].GetMinimum()
        maxval = 1.20 * maxval
        maxval = 0.020
        if (SubtractFirstHistoTest): maxval = 0.020
        #if (maxval < 0.45): maxval = 0.450
        #if (maxval < 0.10): maxval = 0.100

        # build the returned histograms
        returnHistogram[i] = TH1F("new_"+histoGram[i].GetName(), histoGram[i].GetTitle(),                                  
                           histoGram[i].GetNbinsX(), histoGram[i].GetXaxis().GetXmin(), histoGram[i].GetXaxis().GetXmax())
        returnHistogram[i].SetMarkerStyle(markerStyle[i])
        returnHistogram[i].SetMarkerColor(markerColor[i])
        returnHistogram[i].SetLineColor(markerColor[i])

        for bin in range(histoGram[i].GetNbinsX()):
            returnHistogram[i].SetBinContent(bin+1, tempProf[i].GetBinContent(bin+1))
            returnHistogram[i].SetBinError(bin+1, tempProf[i].GetBinError(bin+1))
            if (debug): print ">> ring_2 ",bin," mean = ", tempProf[i].GetBinContent(bin+1), "+-",tempProf[i].GetBinError(bin+1)
        
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
            #returnHistogram[i].GetYaxis().SetRangeUser(-maxval/4,maxval)
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

    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
    if nFiles==7:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6]
        elif max_hist==6:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
    if nFiles==8:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
    if nFiles==9:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[8] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]


    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################        
def MakexResSagittaPlotsFrom3D(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles,  symmetricRange=False, binRangeLower=-1, binRangeUpper=-1):

    # this function takes as argument a TH3 and obtains the profile in one of its axis. 
    debug = False
    normaliseHistos = False # not normalization
    unitArea = False # not unit area

    # make sure the styles are integers
    markerStyle = []
    for marker in markerStyles:
        markerStyle.append(int(marker))
    markerColor = []
    for marker in markerColors:
        markerColor.append(int(marker))
    

    # in case we limit the range of bins
    rangeLimit = False
    if (binRangeLower > 0): rangeLimit = True
    if (binRangeUpper >0 and binRangeUpper >= binRangeLower): rangeLimit = True     
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = 0.0
    minval = 0.0
    max_hist = 0
    
    histoGram = [TH3,TH3,TH3,TH3,TH3,TH3, TH3, TH3, TH3]
    returnHistogram = [TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F]
    myProfile = [TH2, TH2, TH2, TH2, TH2, TH2, TH2, TH2, TH2]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple,tuple]
    tempProf = [TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F, TH1F]
    
    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakexResSagittaPlotsFrom3D> ===  ",histogramName,"  ==="
        if (debug): print " <MakexResSagittaPlotsFrom3D> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] =GetHistogram3D(rootFiles[i],histogramDir[i],histogramName)
        thisName = "IBLStavesSagitta_File_"+str(i)
        tempProf[i] = TH1F(thisName,"IBL Staves sagitta",histoGram[i].GetYaxis().GetNbins()+2,histoGram[i].GetYaxis().GetXmin(),histoGram[i].GetYaxis().GetXmin()+2);
                 
        # adding two extra bins. One will be leaved blank. The second filled with the 
        
    for i in range(nFiles):
        for stave in range(16):
            if (debug): print " ---- stave --- ", stave
            lowerbin = stave+1
            upperbin = stave+1
            if (stave == 14): continue; #leave blank bin
            if (stave == 15): #last point corresponds to the entire detector
                lowerbin = 0
                upperbin = 0
                # make the profile
            #if (rangeLimit): histoGram[i].GetYaxis().SetRange(binRangeLower,binRangeUpper);
            histoGram[i].GetYaxis().SetRange(lowerbin,upperbin)
            myProfile[i] = histoGram[i].Project3D("ZX")
        
            # profile with the middle bins:
            #print " myProfile[i] Nxbins = ",myProfile[i].GetNbinsX()
            myProfile[i].GetXaxis().SetRangeUser(-1.5,0.5)
            middleProf = myProfile[i].ProjectionY("middle_r",9,10)
            middlepointValue = middleProf.GetMean()
            middlepointError = middleProf.GetMeanError()
            if (debug): print " middlepoint=", middlepointValue, "+-", middlepointError
            
            edgeProf1 =  myProfile[i].ProjectionY("edge_r1",1,2)
            edgepointValue = edgeProf1.GetMean()
            edgepointError = edgeProf1.GetMeanError()
            if (debug): print " edgepoint=", edgepointValue, "+-", edgepointError
            
            edgeProf2 =  myProfile[i].ProjectionY("edge_r2",19,20)
            edgepointValue = edgeProf2.GetMean()
            edgepointError = edgeProf2.GetMeanError()
            if (debug): print " edgepoint=", edgepointValue, "+-", edgepointError
            
            edgeProf1.Add(edgeProf2);
            edgepointValue = edgeProf1.GetMean()
            edgepointError = edgeProf1.GetMeanError()
            if (debug): print " edgepoint=", edgepointValue, "+-", edgepointError

            sagittaValue =  middlepointValue -  edgepointValue;
            sagittaError = math.sqrt(middlepointError*middlepointError + edgepointError*edgepointError)
            if (debug or stave == 15): print " >> sagitta= [ File",i,",",legendTitles[i],"] =", sagittaValue, "+-", sagittaError

            tempProf[i].SetBinContent(stave+1,1000*sagittaValue); # values in microns
            tempProf[i].SetBinError(stave+1,1000*sagittaError);
            # end of loop on staves
            
            # sometimes ROOT likes to draw a negative portion of y-axis when no negative entries
            #RemoveNegativeYAxis(histoGram[i],histogramName)

        # find which histogram has largest y-value - this will be drawn first
        if tempProf[i].GetMaximum() > maxval:
            maxval = 1.05 * tempProf[i].GetMaximum()
            if tempProf[i].GetMinimum() < -maxval:
                maxval = -tempProf[i].GetMinimum()
                maxval = 1.05 * maxval

        minbin = tempProf[i].GetMinimumBin()
        minWithErr = tempProf[i].GetBinContent(minbin) - tempProf[i].GetBinError(minbin)/2        
        if minWithErr < minval:
            minval = minWithErr

        #if (maxval < 0.45): maxval = 0.450
        #if (maxval < 0.10): maxval = 0.100

        # buld the returned histograms
        returnHistogram[i] = TH1F("new_"+histoGram[i].GetName()+"_"+str(i), histoGram[i].GetTitle(),                                  
                           tempProf[i].GetNbinsX(), tempProf[i].GetXaxis().GetXmin(), tempProf[i].GetXaxis().GetXmax())
        returnHistogram[i].SetMarkerStyle(markerStyle[i])
        returnHistogram[i].SetMarkerColor(markerColor[i])
        returnHistogram[i].SetLineColor(markerColor[i])
        
        for bin in range(tempProf[i].GetNbinsX()):
            if (bin == 14): continue
            returnHistogram[i].SetBinContent(bin+1, tempProf[i].GetBinContent(bin+1))
            returnHistogram[i].SetBinError(bin+1, tempProf[i].GetBinError(bin+1))
            if (bin<histoGram[i].GetNbinsY()):
                returnHistogram[i].GetXaxis().SetBinLabel(bin+1, str(bin))
            if (bin == histoGram[i].GetNbinsY()+1):
                returnHistogram[i].GetXaxis().SetBinLabel(bin+1, "IBL")
        
        # perform the deired fit to the histogram
        fit = MakeFit(returnHistogram[i],fitType, markerColors[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit,returnHistogram[i],legendTitles[i])

    #unify range
    if (debug): print " <MakeProfPlotsFrom3D> maxval = ",maxval
    for i in range(nFiles):
        if (symmetricRange): 
            if (debug): print " <MakeProfPlotsFrom3D> symmetric range with maxval = ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(-maxval,maxval)
        else:
            if (debug): print " <MakeProfPlotsFrom3D> range from minval=",mival," --> maxval= ",maxval
            returnHistogram[i].GetYaxis().SetRangeUser(minval,maxval)
        

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

    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
    if nFiles==7:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6]
        elif max_hist==6:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
    if nFiles==8:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
    if nFiles==9:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[8] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]


    if (debug): print "  <MakeProfPlots> for ",histogramName, " **  COMPLETED  ** "
    return totalTuple #returning histograms and fits

###########################################################################################################################################        
def MakeModuleResPlotsFrom3D(histogramDir,legendTitles,markerColors,markerStyles,histogramName, fitType, rootFiles, nFiles,  normaliseHistos=False, sector=0, ring=-50):

    # this function takes as argument a TH3 and obtains the profile in one of its axis. 
    debug = False
    #normaliseHistos = True # not normalization
    unitArea = False # not unit area
    doStave = False
    #if ring = -50 --> entire stave
    if (ring == -50): doStave = True
    if (True and doStave): print " <MakeModuleResPlotsFrom3D> === Draw the entire stave", sector

    # make sure the styles are integers
    markerStyle = []
    for marker in markerStyles:
        markerStyle.append(int(marker))
    markerColor = []
    for marker in markerColors:
        markerColor.append(int(marker))
        
    # in case we limit the range of bins
    if ("pix_b0" in histogramName): # IBL
        if (sector > 13): sector = 13
        if (sector <  0): sector = 0
        if (ring  > 9): ring = 9
        if (ring < -10): ring = -10
        logicalRing = ring+11
    if ("sct_b" in histogramName): # SCT layer
        if (sector<0): sector = 0 
        logicalRing = ring+6+1 # (staringt at ring -6 --> bin 1) 
            
    #gets histograms from the files, normalises if desired and makes fits
    #returns histograms and fits
    maxval = -999999
    max_hist = 0
    
    histoGram = [TH3,TH3,TH3,TH3,TH3, TH3, TH3, TH3, TH3]
    returnHistogram = [TH1, TH1, TH1, TH1, TH1, TH1, TH1, TH1, TH1]
    Tuples = [tuple,tuple,tuple,tuple,tuple,tuple, tuple, tuple, tuple]

    #first have to get all the histograms because they may be used to normalise each other etc 
    for i in range(nFiles):
        print " <MakeModuleResPlotsFrom3D> ===  ",histogramName,"  ===  sector:", sector, " ring:",ring, " (logical ring/bin:",logicalRing,")" 
        if (debug): print " <MakeModuleResPlotsFrom3D> retriveing ",histogramName," from file ",i," --> ",rootFiles[i]
        histoGram[i] =GetHistogram3D(rootFiles[i],histogramDir[i],histogramName)

    for i in range(nFiles):
        # make the profile
        if (doStave): logicalRing = 0 # all staves
        histName = "LocalXRes_sector_" + str(sector) + "_ring_" + str(ring) + "_File_" + str(i)
        if (ring<0 and not doStave): histName = "LocalXRes_sector_" + str(sector-1) + "_ring__" + str(-ring) + "_File_" + str(i)
        returnHistogram[i] = histoGram[i].ProjectionZ(histName,logicalRing,logicalRing,sector,sector)
        #rebinned histograms
        #returnHistogram[i] = (histoGram[i].ProjectionZ(histName,logicalRing,logicalRing,sector,sector)).RebinX()
        
        
        # perform the deired fit to the histogram
        fit = MakeFit(returnHistogram[i], fitType, markerColor[i])
        returnHistogram[i].SetMarkerStyle(markerStyle[i])
        returnHistogram[i].SetMarkerColor(markerColor[i])
        returnHistogram[i].SetLineColor(markerColor[i])

        # make a tuple object that can be passed to draw method
        Tuples[i] = returnTuple(fit, returnHistogram[i], legendTitles[i])

    for i in range(nFiles):
        #normalise histograms to unit area if desired
        if returnHistogram[i].Integral() > 0:
            if unitArea:
                #print "for hist ",i, " scaling" 
                returnHistogram[i].Scale(1/returnHistogram[i].Integral())
                returnHhistogram[i].GetYaxis().SetTitle("Arbitrary units")
            #elif  i > 0 and normaliseHistos:
            elif normaliseHistos:
                returnHistogram[i].Scale(returnHistogram[normaliserHisto].Integral()/returnHistogram[i].Integral())

        # find which histogram has largest y-value - this will be drawn first
        if returnHistogram[i].GetMaximum() > maxval:
            max_hist = i
            maxval = returnHistogram[i].GetMaximum()

    if (True): print " < MakeModuleResPlotsFrom3D> max_hist = ", max_hist, "  max_val = ", maxval 
    
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
    if nFiles==5:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3]
    if nFiles==6:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4]
    if nFiles==7:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6]
        elif max_hist==6:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5]
    if nFiles==8:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6]
    if nFiles==9:
        if max_hist==0 or forceDrawOrder:
            totalTuple = Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==1:
            totalTuple = Tuples[1] + Tuples[0] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==2:
            totalTuple = Tuples[2] + Tuples[0] + Tuples[1] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==3:
            totalTuple = Tuples[3] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==4:
            totalTuple = Tuples[4] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[5] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==5:
            totalTuple = Tuples[5] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[6] + Tuples[7] + Tuples[8]
        elif max_hist==6:
            totalTuple = Tuples[6] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[7] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[7] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[8]
        elif max_hist==7:
            totalTuple = Tuples[8] + Tuples[0] + Tuples[1] + Tuples[2] + Tuples[3] + Tuples[4] + Tuples[5] + Tuples[6] + Tuples[7]


    if (debug): print " <MakeModuleResPlotsFrom3D> totalTuple:", totalTuple
    if (debug): print "  <MakeModuleResPlotsFrom3D> for ",histogramName, " **  COMPLETED  ** "
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
def preparePalette(paletteStyle, targetGreen=25, ScaleMax=100):
    debug = False;
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

    if (paletteStyle == 4): 
    #widht plots lights: blue -> green -> orange -> red but not linear
        NRGBs = 5
        NCont = 99
        if (ScaleMax < 4*targetGreen): ScaleMax = 4*targetGreen
        stops = array("d", [ 0.5*targetGreen/ScaleMax, 1.*targetGreen/ScaleMax, 2.0*targetGreen/ScaleMax, 3.*targetGreen/ScaleMax, 1.00])
        red   = array("d", [ 0.20, 0.02, 1.00, 0.99, 0.20])# 0.95
        green = array("d", [ 0.00, 0.99, 0.99, 0.05, 0.05])# 0.95
        blue  = array("d", [ 0.99, 0.02, 0.01, 0.02, 0.05])# 0.02
    
        TColor.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
        gStyle.SetNumberContours(NCont);

    if (debug):
        for i in range(len(stops)):
            print " i=",i,stops[i]," target:", targetGreen," scalemax:", ScaleMax
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


  cutBin = 0

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

    nBins = 20
    newBins = array('d',[-20.0,-15.0,-10.0,-8.0,-6.0,-5.0,-4.0,-3.0,-2.0,-1.0,0.0,1.0,2.0,3.0,4.0,5.0,6.0,8.0,10.0,15.0,20.0])

    debug = False
    makeDistribution = False #gets set to true if histogramName contains "bin-X"

    if "residual" in histogramName:
        if "pix_b_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_b_residualx_pt")
        if "pix_b0_" in histogramName:
            h2d = rootFile.Get(histogramDir + "pix_b0_residualx_pt")
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

#########################################################################################
def GetHistogram(rootFile,histogramDir,histogramName,markerColor,markerStyle, fileID=0):

    debug = False

    # make sure the styles are integers
    markerStyle = int(markerStyle)
    markerColor = int(markerColor)

    if (debug): 
        print " -- GetHistogram -- -- START -- -- "
        print "    rootFile = ", rootFile
        print "    histogramDir = ", histogramDir
        print "    histogramName = ", histogramName
        print "    markerColor = ", markerColor
        print "    markerStyle = ",markerStyle
        print "    fileID = ",fileID
    
    if histogramName=="oldpix_b_residualx":
        histoGramL1X=rootFile.Get(histogramDir + "pix_b1_residualx")
        histoGramL2X=rootFile.Get(histogramDir + "pix_b2_residualx")
        histoGramL3X=rootFile.Get(histogramDir + "pix_b3_residualx")
        histoGram = histoGramL1X.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramL2X)
        histoGram.Add(histoGramL3X)
        
        
    elif histogramName=="oldpix_b_residualy":
        histoGramL1Y=rootFile.Get(histogramDir + "pix_b1_residualy")
        histoGramL2Y=rootFile.Get(histogramDir + "pix_b2_residualy")
        histoGramL3Y=rootFile.Get(histogramDir + "pix_b3_residualy")
        histoGram = histoGramL1Y.Clone()
        histoGram.Sumw2()
        histoGram.Add(histoGramL2Y)
        histoGram.Add(histoGramL3Y)
        
    elif histogramName=="pix_ec_residualx":
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
    elif "pix_b0_residualx_pt" in histogramName:
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
    elif "pix_b0_resXvsetaLumiBlock" in histogramName:
        histoGram = rootFile.Get(histogramDir + histogramName)
        histoGram.RebinX(32)
        print "histogram",histoGram
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




    if histogramName=="delta_z0":
        histoGram.SetAxisRange(-1.,1.)

    #Overlaps mean
    if histogramName=="pix_b_Oyresyvsmodeta":
        histoGram.Scale(1000.)
        histoGram.GetYaxis().SetRangeUser(-300,200)

    if histogramName=="pix_b_Oyresyvsmodphi":
        histoGram.Scale(1000.)
        histoGram.GetYaxis().SetRangeUser(-300,300)

    if histogramName=="pix_b_Oxresxvsmodeta" or histogramName=="pix_b_Oxresxvsmodphi" or histogramName=="pix_eca_Oxresxvsmodphi" or histogramName=="pix_ecc_Oxresxvsmodphi" or histogramName=="pix_eca_Oyresyvsmodphi" or histogramName=="pix_ecc_Oyresyvsmodphi":
        histoGram.Scale(1000.)
        

    if histogramName=="sct_b_Oxresxvsmodeta" or histogramName=="sct_b_Oyresxvsmodeta" or histogramName=="sct_b_Oyresxvsmodphi" or histogramName=="sct_b_Oxresxvsmodphi" or histogramName=="sct_b_yresxvsmodphi" or histogramName=="sct_eca_Oxresxvsmodphi" or histogramName=="sct_ecc_Oxresxvsmodphi":
        histoGram.Scale(1000.)
        


    if histogramName=="si_barrel_resX_mean" or histogramName=="si_eca_resX_mean" or histogramName=="si_ecc_resX_mean":
        #Patch for the mean
        histoGram.Scale(1000)
        histoGram.GetYaxis().SetRangeUser(-10,10)
    if histogramName=="pix_b_xresvsmodphi" or  histogramName=="pix_ecc_xresvsmodphi" or histogramName=="pix_eca_xresvsmodphi":
        #or histogramName=="si_ecc_resX_mean" or histogramName=="si_eca_resX_mean" or histogramName=="si_barrel_resX_mean": Peo 
        histoGram.GetYaxis().SetRangeUser(-.02,.02) #pri
    if histogramName=="pix_b_yresvsmodphi" or histogramName=="pix_eca_yresvsmodphi" or histogramName=="pix_ecc_yresvsmodphi":
        histoGram.GetYaxis().SetRangeUser(-.1,.1) #pri
    if histogramName=="sct_b_xresvsmodeta" or histogramName=="pix_b_xresvsmodeta": 
        histoGram.GetYaxis().SetRangeUser(-.02,.02) #pri
    if histogramName=="sct_b_xresvsmodphi" or histogramName=="sct_eca_xresvsmodphi" or histogramName=="sct_ecc_xresvsmodphi":   
        histoGram.GetYaxis().SetRangeUser(-.02,.02) #pri

    if histogramName=="sct_b_Oxresxvsmodeta_mean" or histogramName=="pix_b_Oxresxvsmodeta_mean" or histogramName=="pix_b_Oyresyvsmodphi_mean":
        histoGram.GetYaxis().SetRangeUser(-.15,.15)
    if histogramName=="pix_b_xresvsmodeta_width" or histogramName=="pix_b_xresvsmodphi_width" or histogramName=="sct_b_xresvsmodeta_width" or histogramName=="sct_b_xresvsmodphi_width" or histogramName=="si_barrel_resX_rms" or histogramName=="si_eca_resX_rms" or histogramName=="si_ecc_resX_rms":
        histoGram.GetYaxis().SetRangeUser(0.00,0.050)
        
    if histogramName=="si_barrel_pullX_mean" or histogramName=="si_eca_pullX_mean" or histogramName=="si_ecc_pullX_mean":
        histoGram.GetYaxis().SetRangeUser(-.5,.5)

    if histogramName=="si_barrel_pullX_width": 
        histoGram.GetYaxis().SetRangeUser(0,1.5)

        
    if histogramName=="measurements_eff_vs_layer_ecc" or histogramName=="measurements_eff_vs_layer_eca" or histogramName=="measurements_eff_vs_layer_barrel":
        histoGram.GetYaxis().SetRangeUser(0.9,1.01)
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
        histoGram.GetYaxis().SetRangeUser(-11,11)
        
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
    if histogramName=="delta_eta0" or histogramName=="delta_z0":
        histoGram.Rebin(2)
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
        histoGram.SetAxisRange(-1.,1.)

    #print "bin size for ",histogramName," = " , histoGram.GetBinWidth(50)        


    if (debug): print " -- GetHistogram -- -- COMPLETED -- -- "
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

###########################################################################        
def MakeFit(histoGram, fitType, markerColor):

    # make sure the styles are integers
    markerColor = int(markerColor)
    
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
                if (debug):
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
#            if ("mu" in units): text += "  "
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

        if (cloneHist.GetNbinsX()<9): rebinHistogram = False
            
        if rebinHistogram==True:
            rebinfactorisgood = False
            rebinfactor = 2
            if (cloneHist.GetNbinsX() % 2 == 0 and not rebinfactorisgood):
                rebinfactorisgood = True
            if (cloneHist.GetNbinsX() % 3 == 0 and not rebinfactorisgood):
                rebinfactor = 3
                rebinfactorisgood = True
            if (cloneHist.GetNbinsX() % 5 == 0 and not rebinfactorisgood):
                rebinfactor = 5
                rebinfactorisgood = True
            if (not rebinfactorisgood):
               rebinfactor = 1
               rebinfactorisgood = True
            if (debug): print "rebinning histogram:",cloneHist.GetName()," of Nbins: ", cloneHist.GetNbinsX(), " with rebin factor: ",rebinfactor
            if (rebinfactorisgood): cloneHist.Rebin(rebinfactor)



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

    #min = -1.5
    #max = 1.5

    min = hist.GetBinLowEdge(1)
    max = (hist.GetBinLowEdge(hist.GetNbinsX()))+hist.GetBinWidth(hist.GetNbinsX())
    min = -2.0
    max =  2.0
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
              canvasText,markerStyle,markerColor,outputName="test.png", textBoxLeftX=0.55, textBoxUpperY=0.84):

    can = TCanvas(outputName,outputName,800,600)
    can.cd()
    plotTitleOnFirstLine = True

    hist.GetXaxis().SetTitle(xAxisTitle)
    hist.GetYaxis().SetTitle(yAxisTitle)
    hist.GetYaxis().SetTitleOffset(1.2)
    hist.SetMarkerStyle(markerStyle)
    hist.SetMarkerColor(markerColor)
    hist.Draw()
  
    # canvas text (right hand side)
    latexAtlas = TLatex()
    latexAtlas.SetNDC()
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
    debug = False
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
                             
    isPixel = True
    #split the pixels hit maps histo of the end caps as all the disks are kept in the same histogram
    debug = False
    if (debug): print "\n\n -- getPIXEndCapResMapHisto -- remember to set debug = False \n\n" 
    if (debug): print " -- getPIXEndCapResMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()
    if ("sct" in inputHisto.GetName()):  
        isPixel = False
        print " -- getPIXEndCapResMapHisto -- ERROR -- dealing with SCT histogram ", inputHisto.GetName()
        
    print " -- getPIXEndCapResMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()

    if (isPixel):
        outputHisto = TH2F("d"+str(disk)+"_"+inputHisto.GetName(), inputHisto.GetTitle()+" disk "+str(disk), 
                           inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax(),
                           1, inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax()) # same range to facilitate drawing on top
        
    if (debug): print " -- getPIXEndCapResMapHisto -- ouputHisto= ", outputHisto.GetName(), "  Xbins=", outputHisto.GetNbinsX(), "  Ybins=", outputHisto.GetNbinsY()    
    for i in range(outputHisto.GetNbinsX()):
        #project the histo per disk and module
        thisHisto = inputHisto.ProjectionZ(inputHisto.GetName()+"_d"+str(disk)+"_m"+str(i),disk+1,disk+1,i+1,i+1)
        if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
            outputHisto.SetBinContent(i+1, 1, 1000.*thisHisto.GetMean())
            if (debug): print " -- getPIXEndCapResMapHisto -- disk: ", disk," module: ",i," entries:",thisHisto.GetEntries(),"  res:",1000.*thisHisto.GetMean()," bincontent=",outputHisto.GetBinContent(i+1,disk+1)

                

    # this is a crosscheck to avoid crashes
    if (outputHisto.GetEntries() == 0): outputHisto.SetBinContent(1,1,0.)
                
    if (debug): print " -- getPIXEndCapResMapHisto -- completed -- "
        
    return outputHisto

###########################################################################################################
def getSCTEndCapResMapHisto (inputHisto, disk):
                            
    #split the sct hit maps histo of the end caps as all the disks are kept in the same histogram
    debug = False
    if (debug): print "\n\n -- getSCTEndCapResMapHisto -- remember to set debug = False \n\n" 
    if (debug): 
        print " -- getSCTEndCapResMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()
        print " -- getSCTEndCapResMapHisto -- start -- extracting disk ", disk ," from histo ",inputHisto.GetName()

    outputHisto = TH2F("d"+str(disk)+"_"+inputHisto.GetName(), inputHisto.GetTitle()+" disk "+str(disk), 
                       inputHisto.GetNbinsX(), inputHisto.GetXaxis().GetXmin(), inputHisto.GetXaxis().GetXmax(),
                       inputHisto.GetNbinsY(), inputHisto.GetYaxis().GetXmin(), inputHisto.GetYaxis().GetXmax()) # same range to facilitate drawing on top
        
    if (debug): 
        print " -- getSCTEndCapResMapHisto -- ouputHisto= ", outputHisto.GetName(), "  Xbins=", outputHisto.GetNbinsX(), "  Ybins=", outputHisto.GetNbinsY()
        print "    loop on rings ", outputHisto.GetNbinsX(), " and modules ",outputHisto.GetNbinsY() 
    for i in range(outputHisto.GetNbinsX()):
        for j in range(outputHisto.GetNbinsY()):
        #project the histo per disk and module
            thisHisto = inputHisto.ProjectionZ(inputHisto.GetName()+"_d"+str(disk)+"_r"+str(i)+"_m"+str(j),i+1,i+1,j+1,j+1)
            if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
                outputHisto.SetBinContent(i+1, j+1, 1000.*thisHisto.GetMean())
                if (debug): print " -- getSCTEndCapResMapHisto -- disk: ", disk," ring",i," module: ",j," entries:",thisHisto.GetEntries(),"  res:",1000.*thisHisto.GetMean()," bincontent=",outputHisto.GetBinContent(i+1,j+1)

                

    # this is a crosscheck to avoid crashes
    if (outputHisto.GetEntries() == 0): outputHisto.SetBinContent(1,1,0.)
                
    if (debug): print " -- getSCTEndCapResMapHisto -- completed -- "
        
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
    debug = False
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
            print " lenght of histoGram: ", len(histoGram)     
            histoGram[i] = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0)

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
        totalMinEntries = 1          
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
    debug = False
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
def MakeResidualMaps(histogramDir, legendTitles, rootFiles, fileID, detecName="pixels", barrelEndCap="BAR", coordinate=0, unifiedScale = True, zAxisRange= 25, 
                     Type = "mean", SeparateSides=True, IsRun1=False):
    # The hit maps have to be plotted for each track collection
    # this gets histograms from the files, normalises if desired and makes fits
    # and returns histograms and fits
    debug = False
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
    if (detecName=="PIX" and barrelEndCap=="BAR"): 
        if (IsRun1):
            nLayers = 3
        else:
            nLayers = 4
        
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
        
    if (Type == "clustersize"):
        print " good clustersize :)"

    if (Type == "detailed"):
        print " detailed maps :) "
                
    #first have to get all the histograms because they may be used to normalise each other etc     
    if (barrelEndCap == "BAR"):
        for i in range(nLayers):
            myHistoName = shortName + str(i) + "_" + myCoordinate +"resvsmodetaphi_3d"   
            if (detecName=="SCT"): 
                myHistoName = shortName + str(i) + "_" + "s"+str(coordinate) + "_" + myCoordinate +"resvsmodetaphi_3d"
                if (Type == "clustersize"): 
                    myHistoName = shortName + str(i) + "_clustersizePhivsmodetaphi_3d_" + "s"+str(coordinate)
                if (Type == "detailed"): 
                    myHistoName = shortName + str(i) + "_" + "s"+str(coordinate) + "_biased_" + myCoordinate +"resvsmodetaphi_3d"
            if (detecName=="PIX"):
                myHistoName = shortName + str(i) + "_" + myCoordinate +"resvsmodetaphi_3d"            
                if (Type == "detailed"): 
                    myHistoName = shortName + str(i) + "_detailed_" + myCoordinate +"resvsmodetaphi_3d"            
            myHistoName = Check3DHistoExists(rootFiles[fileID],histogramDir[fileID],myHistoName)
            if (debug): print " -- MakeResidualMaps -- fetching histogram names: detector ",detecName,"  layer/disk ",i,"   histo=",myHistoName 
            histoGram3D = GetHistogram(rootFiles[fileID],histogramDir[fileID],myHistoName,0,0) # retrieve the 3D histogram
            hname = myHistoName+"_proj"
            htitle = detecName + " residual map " + "(" + Type +")" 
            histoGram[i] = get2DResidualMap(histoGram3D, i, Type)
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
            if (debug): print " -- MakeResidualMaps --> histoEChits:",myHistoName ," Bins X = ", histoEChits.GetNbinsX(), "   Bins Y= ", histoEChits.GetNbinsY()            
            histoGram[disk] = getSCTEndCapResMapHisto (histoEChits, disk)


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
        if (debug): print " -- MakeResidualMaps -- drawing Z range= ",-totalRange," --> ",totalRange
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
                  
    if (Type == "width" or Type== "clustersize"): 
        for layer in range(nLayers):
            histoGram[layer].SetMinimum(0)

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

def get1DFrom2D(inputHisto, thistype = "mean",custombins=[]):
    hname = inputHisto.GetName()+"_ResMean"
    htitle= "residual map " + "(" + thistype +")"
    
    outputHisto = TH1F(hname,htitle,inputHisto.GetXaxis().GetNbins(),
                       inputHisto.GetXaxis().GetXmin(),
                       inputHisto.GetZaxis().GetXmax())

    #fill the histo

    for i in range (outputHisto.GetXaxis().GetNbins()):
        thisHisto =inputHisto.ProjectionY(hname+"_ymean"+str(i),i+1,i+1)
        if (thishist.GetEntries()>=MinEntriesPerModule):
            meeanFWHMTuple = findMeanRMSUsingFWHM(thisHisto)
            if (thistype=="mean"):  outputHisto.SetBinContent(i+1,1000.*meanFWHMTuple[0])
            if (thistype=="width"): outputHisto.SetBinContent(i+1,1000.*meanFWHMTuple[1])
    return outputHisto
###########################################################################################################################
def get2DResidualMap(inputHisto, layer, thistype = "mean"):
    # the input histo is a 3D
    hname = inputHisto.GetName() + "_ResMean"
    htitle = " residual map " + "(" + thistype +")"  

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
            thisHisto = inputHisto.ProjectionZ(hname+"_zmean_"+str(layer)+str(i)+str(j),i+1,i+1,j+1,j+1)
            if (thisHisto.GetEntries() >= MinEntriesPerModule): # min number of entries
                #FindMeanRMSUsingFWHM
                meanFWHMTuple = findMeanRMSUsingFWHM(thisHisto)
                #print "mean =", meanFWHMTuple[0]
                #print "FWHM/2.35 = ", meanFWHMTuple[1]
                #outputHisto.SetBinContent(i+1,j+1,1000.*thisHisto.GetMean())
                if (thistype == "mean"): outputHisto.SetBinContent(i+1,j+1,1000.*meanFWHMTuple[0])
                if (thistype == "width"): outputHisto.SetBinContent(i+1,j+1,1000.*meanFWHMTuple[1])
                if (thistype == "clustersize"): outputHisto.SetBinContent(i+1,j+1, thisHisto.GetMean())#outputHisto.SetBinContent(i+1,j+1, meanFWHMTuple[0])
                if (thistype == "detailed"): outputHisto.SetBinContent(i+1,j+1,1000.*meanFWHMTuple[0])

    return outputHisto

###########################################################################################################################
def Check3DHistoExists(rootFile,histogramDir,histogramName):
    # this function is basically to avoid an execution error after splitting the sct barrel residual histograms in side 0 and 1

    myhist = rootFile.Get(histogramDir + histogramName)
    #print " myhist = ", myhist, "  id(myhist)=",id(myhist)
    
    if (not not myhist): 
        # print " 1) exists :)"
        outputName = histogramName
    else: 
        # print " 1) does not exist !!! :("
       if (histogramName == "sct_b0_s0_xresvsmodetaphi_3d"): histogramName = "sct_b0_xresvsmodetaphi_3d"
       if (histogramName == "sct_b0_s1_xresvsmodetaphi_3d"): histogramName = "sct_b0_xresvsmodetaphi_3d"
       if (histogramName == "sct_b1_s0_xresvsmodetaphi_3d"): histogramName = "sct_b1_xresvsmodetaphi_3d"
       if (histogramName == "sct_b1_s1_xresvsmodetaphi_3d"): histogramName = "sct_b1_xresvsmodetaphi_3d"
       if (histogramName == "sct_b2_s0_xresvsmodetaphi_3d"): histogramName = "sct_b2_xresvsmodetaphi_3d"
       if (histogramName == "sct_b2_s1_xresvsmodetaphi_3d"): histogramName = "sct_b2_xresvsmodetaphi_3d"
       if (histogramName == "sct_b3_s0_xresvsmodetaphi_3d"): histogramName = "sct_b3_xresvsmodetaphi_3d"
       if (histogramName == "sct_b3_s1_xresvsmodetaphi_3d"): histogramName = "sct_b3_xresvsmodetaphi_3d"
    
    outputName = histogramName
    return outputName

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
        if (not (detecName == "PIX" and i == 0)):ATLZaxis.Draw();
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
    gStyle.SetPalette(1) # 53= dark body radiator; 1= standard; 55 = rainbow
    Palette_EffiPlots = TExec("SetPalette_TrafficLights","gStyle->SetPalette(1)")
    if(paletteStyle != 1): Palette_EffiPlots = TExec("Palette_TrafficLights",'TPython::Exec("preparePalette('+str(paletteStyle)+')")') 
    Palette_EffiPlots.Draw();

    # title of the histograms
    latexTitle = TLatex()
    latexTitle.SetNDC()
    latexTitle.SetTextColor(1)
    gStyle.SetPaintTextFormat("4.0f")
    if ("eff") in zAxisTitle: gStyle.SetPaintTextFormat("4.2f")
    if ("cluster" in zAxisTitle): gStyle.SetPaintTextFormat("4.1f")

    for i in range(nHist):
        can.cd(i+1)
        myDrawOptions = "colz text"
        if ("detailed" in inputTuple[i].GetTitle()): 
            myDrawOptions = "colz" #do not print the values
            print " detailed found --> no numbers "
        print " histo ",i," title: ", inputTuple[i].GetTitle() 
        inputTuple[i].GetXaxis().SetTitle(xAxisTitle)
        inputTuple[i].GetYaxis().SetTitle(yAxisTitle)
        inputTuple[i].GetZaxis().SetTitle(zAxisTitle)
        # --> it is not working # gStyle.SetPadTickX(0) # hitmaps have a different axis on top
        if (debug): print " -- DrawHitMaps -- i=",i,"   detec=",detecName,"   Barrel/Endcap= ",barrelEndCap 
        if (detecName == "PIX" and barrelEndCap == "BAR"): 
            inputTuple[i].Draw(myDrawOptions)    
            myTitle = "Pixel barrel layer " + str(i-1)
            if (i == 0): myTitle = " IBL " # special case   
        if (detecName == "SCT" and barrelEndCap == "BAR"): 
            inputTuple[i].Draw(myDrawOptions)    
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
           
        if ("detailed" in inputTuple[i].GetTitle()):
            DrawModuleGrid ( detecName, inputTuple[i], i);    
            
            
    if (debug): print " -- DrawHitMaps -- completed -- "    
    if makeOutput:
        can.SaveAs(outputName)

    return can

###########################################################################################################################
def DrawModuleGrid(detecName, inputHisto, layer=0):
    debug = False
    if (debug): print " -- DrawModuleGrid -- start -- for detect", detecName, " NbinsX:",inputHisto.GetNbinsX()

    nbinsx = inputHisto.GetNbinsX()
    nbinsy = inputHisto.GetNbinsY()
    xmin = inputHisto.GetXaxis().GetBinLowEdge(1)    
    xmax = inputHisto.GetXaxis().GetBinUpEdge(nbinsx)    
    ymin = inputHisto.GetYaxis().GetBinLowEdge(1)    
    ymax = inputHisto.GetYaxis().GetBinUpEdge(nbinsy)    
    nCells = 1
    firstRing = -6
    nRings = 12

    if (detecName == "SCT"): 
        nCells = inputHisto.GetNbinsX()/12
        print " -- DrawModuleGrid --  NbinsX:",inputHisto.GetNbinsX()," --> module cells:", nCells, "x", nCells
    if (detecName == "PIX"): 
        nCells = inputHisto.GetNbinsX()/13
        firstRing = -6
        nRings = 13
        if (layer==0):
            nCells = inputHisto.GetNbinsX()/20
            firstRing = -10
            nRings = 20
            
        print " -- DrawModuleGrid -- detector:", detecName," layer:",layer,"  NbinsX:",inputHisto.GetNbinsX()," --> module cells:", nCells, "x", nCells

    nSectors = inputHisto.GetNbinsY()/nCells
    
    for ring in range(nRings):
        logicRing = firstRing+ring
        thisx = inputHisto.GetXaxis().GetBinLowEdge((ring+1)*nCells+1) 
        xline = TLine(thisx,ymin,thisx,ymax)
        xline.SetLineColor(kBlue)
        xline.Draw()    
        print " ring: ",ring, "  logic ring: ",firstRing+ring," bin:",(ring+1)*nCells+1," x=",thisx
        SetOwnership(xline, False)

    for sector in range(nSectors):
        thisy = inputHisto.GetYaxis().GetBinLowEdge((sector+1)*nCells+1) 
        yline = TLine(xmin,thisy,xmax,thisy)
        yline.Draw()    
        print " sector: ", sector," bin:",sector*nCells+1," y=",thisy
        SetOwnership(yline, False)
    

    return
###########################################################################################################################
def DrawSCTECMap(inputHisto, disk):
    debug = False
    if (debug): print " -- DrawSCTECMap -- start -- for disk ", disk, " entries:",inputHisto.GetEntries(),"  binsX=",inputHisto.GetNbinsX(),"  binsY=",inputHisto.GetNbinsY()

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
    #blank.Print()

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
            if (inputHisto.GetBinContent(ring+1,module+1) == 0): 
                pmod[module].SetFillColor(kGray+1)
            pmod[module].Draw("Fsame")
            
            SetOwnership(pmod[module], False)

    gPad.Modified()    
    gPad.Update()
    #inputHisto.Draw("colz")
    #gPad.Update()
    
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
def DrawResPerStave(inputTuple, outputName, outputFile, nFiles):
    debug = False
    can = TCanvas(outputName, outputName, 900, 800)
    can.Divide(4,5)

    for i in range(len(inputTuple)):
        if (debug): print " == DrawResPerStave == index",i," histogram in 3*i+1=",3*i+1,"   total Tuple size:", len(inputTuple)
        mytuple = inputTuple[i]
        can.cd(i+1)
        maxval = 0
        for file in range (nFiles):
            hist = mytuple[3*file+1]
            if (hist.GetMaximum() > maxval):
                firstHisto = hist
                maxval = hist.GetMaximum()

        print " == DrawResPerStave == Max histo --> ",  firstHisto.GetTitle()          
        firstHisto.Draw()
        
        for file in range (nFiles):         
            hist = mytuple[3*file+1]
            hist.Draw("same")
                
    can.SaveAs(outputFile)

    return
###########################################################################################################################
def locateColor(inputHisto, xbin, ybin=1):

    colorIndex = (inputHisto.GetListOfFunctions().FindObject("palette")).GetValueColor(inputHisto.GetBinContent(xbin,ybin))

    return colorIndex
##########################################################################################################
def SetNextInputFile(myFile, myLegend, myMarker, myColor, myTrackColl="", myPrefix=""):

    theInputFileList.append(myFile)
    theLegendList.append(myLegend)
    theMarkerList.append(myMarker)
    theColorList.append(myColor)
    theTrackCollectionList.append(myTrackColl)
    thePrefix.append(myPrefix)

    return

##########################################################################################################
def SetCanvasText(userText):
    newcanvastext = ["","","",""]
    if (len(userText)>=1): newcanvastext[0] = userText[0] 
    if (len(userText)>=2): newcanvastext[1] = userText[1] 
    if (len(userText)>=3): newcanvastext[2] = userText[2] 
    if (len(userText)>=4): newcanvastext[3] = userText[3]
    
    return newcanvastext
##########################################################################################################
def nFilesGiven():

    return len(theInputFileLis)

#####################
## IBL wise plots

def ConvertHistoInGraph(histo,planar=False):
    z_axis = [-322.8975, -301.7925, -280.6875,-259.5825,-228.2775,-186.7725,-145.2675,-103.7625,-62.2575,-20.7525,20.7525,62.2575,103.7625,145.2675,186.7725,228.2775,259.5825,280.6875,301.7925,322.8975]
    nbins_h = 21
    shift=0
    if planar:
        nbins_h=13
        shift=4
    graph = TGraphErrors()
    filled_bins = []
    for etabin in range(1,nbins_h):
        if (histo.GetBinContent(etabin) != 0 ):
            filled_bins.append(etabin)
    for ibin in range(len(filled_bins)):
        graph.Set(ibin+1)
        graph.SetPoint(ibin,z_axis[filled_bins[ibin]-1+shift],histo.GetBinContent(filled_bins[ibin]))
        graph.SetPointError(ibin,0,histo.GetBinError(filled_bins[ibin]))
    graph.GetXaxis().SetRangeUser(-z_fix,z_fix)
    return graph

def MakeStaveShapeFit(projection,planar=False,savePlot=True,Fixbase=False,outputname=""):
    g = ConvertHistoInGraph(projection,planar)
    g.GetXaxis().SetRangeUser(-z_fix,z_fix)
    g.GetYaxis().SetRangeUser(-0.01,0.01)
    fit = TF1("fit", "[1] - ([2]*(x*x-[0]*[0]))/([0]*[0])",z_fix,z_fix);
    fit.FixParameter(0, z_fix);
    if (Fixbase):
        fit.FixParameter(1,0)
    r = g.Fit(fit,"EXSQS");
    mag   =fit.GetParameter(2);
    mag_er=fit.GetParError(2);

    base     =fit.GetParameter(1);
    base_er  =fit.GetParError(1);
    chi = r.Chi2()

    dof =r.Ndf()

    if (savePlot):
        can = TCanvas("c","c",800,600)
        g.GetXaxis().SetTitle("Global z Position[mm]")
        g.GetYaxis().SetTitle("Local X Residual mean [mm]")
        g.Draw("AP")
        text = TLatex()
        text.SetTextFont(42)
        text.SetTextSize(0.04)
        text.SetTextColor(kBlack)
        text.SetNDC()
        text1 = "Chi2/Dof = "  + str(round(chi,2)) +"/"+ str(dof)
        text2 = "Magnitude = " + str(round(mag*1000,2)) + " #pm " + str(round(mag_er*1000,2)) + " #mum"
        text3 = "Baseline  = " + str(round(base*1000,2)) + " #pm " + str(round(base_er*1000,2)) + " #mum"
        fitType = "Parabolic"
        text4 = "Fit type =" + fitType
        text.DrawLatex(0.18,0.86,text1)
        text.DrawLatex(0.18,0.80,text2)
        text.DrawLatex(0.18,0.74,text3)
        text.DrawLatex(0.18,0.68,text4)
        can.SaveAs(outputname)
    
    return [mag,mag_er,base,base_er]

def MakeStaveShapeFitLinear(projection,planar=False,savePlot=True,Fixbase=False,outputname=""):
    g = ConvertHistoInGraph(projection,planar)
    g.GetXaxis().SetRangeUser(-z_fix,z_fix)
    g.GetYaxis().SetRangeUser(-0.003,0.003)
    fit = TF1("fit", "x*[0]+[1]",z_fix,z_fix);
    if (Fixbase):
        fit.FixParameter(1,0)
    r = g.Fit(fit,"EXSQS");
    mag   =fit.GetParameter(0);
    mag_er=fit.GetParError(0);

    base     =fit.GetParameter(1);
    base_er  =fit.GetParError(1);
    chi = r.Chi2()

    dof =r.Ndf()

    if (savePlot):
        can = TCanvas("c","c",800,600)
        g.GetXaxis().SetTitle("Global z Position[mm]")
        g.GetYaxis().SetTitle("Local X Residual mean [mm]")
        g.Draw("AP")
        text = TLatex()
        text.SetTextFont(42)
        text.SetTextSize(0.04)
        text.SetTextColor(kBlack)
        text.SetNDC()
        text1 = "Chi2/Dof = "  + str(round(chi,2)) +"/"+ str(dof)
        text2 = "Slope = " + str(round(mag*1000,2)) + " #pm " + str(round(mag_er*1000,2)) + " #mum"
        text3 = "Intercept  = " + str(round(base*1000,2)) + " #pm " + str(round(base_er,2)) + " #mum"
        fitType = "Linear"
        text4 = "Fit type =" + fitType
        text.DrawLatex(0.18,0.86,text1)
        text.DrawLatex(0.18,0.80,text2)
        text.DrawLatex(0.18,0.74,text3)
        text.DrawLatex(0.18,0.68,text4)
        can.SaveAs(outputname)
    
    return [mag,mag_er,base,base_er]

def MakeMagnitudeLinearFit(histogram,planar=False,savePlot=True,FixSlope=False,outputname=""):
    histo = TH1
    histo = histogram.Clone() 
    fit = TF1("fit", "x*[0]+[1]",z_fix,z_fix);
    if (FixSlope):
        fit.FixParameter(0,0)
    r = histo.Fit(fit,"EXSQS");
    mag   =fit.GetParameter(0);
    mag_er=fit.GetParError(0);

    base     =fit.GetParameter(1);
    base_er  =fit.GetParError(1);
    chi = r.Chi2()

    dof =r.Ndf()

    if (savePlot):
        can = TCanvas("c","c",800,600)
        histo.GetXaxis().SetTitle("Lumiblock")
        histo.GetYaxis().SetTitle("Magnitude [#mum]")
        histo.Draw()
        text = TLatex()
        text.SetTextFont(42)
        text.SetTextSize(0.04)
        text.SetTextColor(kBlack)
        text.SetNDC()
        text1 = "Chi2/Dof = "  + str(round(chi,2)) +"/"+ str(dof)
        text2 = "Slope = " + str(round(mag,4)) + " #pm " + str(round(mag_er,4)) + " #mum"
        text3 = "Intercept  = " + str(round(base,2)) + " #pm " + str(round(base_er,2)) + " #mum"
        fitType = "Linear"
        text4 = "Fit type =" + fitType
        text.DrawLatex(0.18,0.86,text1)
        text.DrawLatex(0.18,0.80,text2)
        text.DrawLatex(0.18,0.74,text3)
        text.DrawLatex(0.18,0.68,text4)
        can.SaveAs(outputname)
    
    return [mag,mag_er,base,base_er]

