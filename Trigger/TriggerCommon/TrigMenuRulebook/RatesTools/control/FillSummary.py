# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Sep 12, 2011

@author: ggalster
'''
#----------------------------------------------------------------------------------------------------
import ROOT

#----------------------------------------------------------------------------------------------------
from model.basics.DataColumn import DataColumn 
from model.basics.DataSelection import DataSelection

from model.COOL.COOLTime import COOLTime

from model.physics.RatesRun import RatesRun
from model.physics.filters.FilterPhysicsReady import FilterPhysicsReady  
#----------------------------------------------------------------------------------------------------
def fill_summary_plot(*runNumbers):
    runSelection = DataSelection() 
    
    for runNumber in runNumbers:
        runSelection = runSelection.Union(DataSelection(*COOLTime().Run(runNumber)))
       
    variables = ["L1_L1A", "L2_total", "EF_total"]
    #variables = ["L1_L1A"]
        
    run = RatesRun(runSelection)
    run.Load()
    run.GetTotalRatesCollection().Load(*variables)


    fillSelection = FilterPhysicsReady().FilterValues(runSelection)    
    fillRanges = fillSelection.GetListOfRanges()

    
    fillSelectionEffective = run.GetTimeFilter().Intersection(fillSelection)
    
    
    
    # Setting up full buffer system
    fillDataFull = {}
    for varName in variables:
        fillDataFull[varName] = {}
        for fill in range(len(fillRanges)):
            fillDataFull[varName][fill] = {}
            for type in ["Rate", "Time"]:
                fillDataFull[varName][fill][type] = DataColumn()
    
    
    
    
    # Reading everything into raw buffer
    for pit in sorted(fillSelectionEffective):
        fillNo = None
        
        for fill in range(len(fillRanges)):
            if fillRanges[fill].Contains(pit):
                fillNo = fill
                break
                    
        for varName in variables:
            rate = run.GetTotalRatesCollection().Get(varName).GetRateInfo(pit).GetOutputRate()
            time = run.GetTimeCollection().GetTimeInfo(pit).GetUnixTime()
            
            fillDataFull[varName][fillNo]["Rate"].append(rate)
            fillDataFull[varName][fillNo]["Time"].append(time)
            
    
    # Reading everything into compact buffer    
    fillData = {}
    for varName in variables:
        fillData[varName] = {}
        for type in ["Time", "Mean", "Peak"]:
            fillData[varName][type] = DataColumn()
            
        for fill in range(len(fillRanges)):
            
            fillData[varName]["Time"].append(fillDataFull[varName][fill]["Time"].Min())
            fillData[varName]["Mean"].append(fillDataFull[varName][fill]["Rate"].Mean())

            peak = None
            for i in range(min(len(fillDataFull[varName][fill]["Rate"]),10)):
                peak = max(peak, fillDataFull[varName][fill]["Rate"][i])            
            
            fillData[varName]["Peak"].append(peak)
    
        
        
    
    
    # Start considering the plot.........
    histogramTypes = ["Mean", "Peak"]
    

    nMax = None
    for varName in variables:
        nMax = max(nMax, fillData[varName]["Time"].Max())        
    nMin = nMax
    for varName in variables:
        nMin = min(nMin, fillData[varName]["Time"].Min())        
    xMin = nMin - (nMax-nMin)*0.05
    xMax = nMax + (nMax-nMin)*0.05
    
    
    nMax = None
    for varName in variables:
        for type in histogramTypes:
            nMax = max(nMax, fillData[varName][type].Max())    
    
    nMin = nMax
    for varName in variables:
        for type in histogramTypes:
            nMin = min(nMin, fillData[varName][type].Min())    
    
    yMin = nMin - (nMax-nMin)*0.05
    yMax = nMax + (nMax-nMin)*0.05
    
    xBins = 1000
    yBins = 1000
    
    
    
    xLabel = "Time"
    xTitleSize = 0.04
    
    
    yLabel = "Rate [Hz]"
    yTitleSize = 0.04
    
    
    
    xLabelSize = 0.03
    yLabelSize = 0.03
    
    timeFormat = "%d/%m %Hh"
    
    
    drawOpt = ""
    
    
    color = 0
    histograms = {}
    for varName in variables:
        histograms[varName] = {}
        
        color += 1
        for type in histogramTypes:
            
            
            histograms[varName][type] = ROOT.TH2F(varName + "_" + type, "", xBins, xMin, xMax, yBins, yMin, yMax)
            
            for i in range(len(fillData[varName][type])):
                histograms[varName][type].Fill(fillData[varName]["Time"][i], fillData[varName][type][i], 1.0)
        
            histograms[varName][type].GetXaxis().SetTitle(xLabel)
            histograms[varName][type].GetXaxis().SetTitleSize(xTitleSize)
            histograms[varName][type].GetXaxis().SetRangeUser(xMin, xMax)
            histograms[varName][type].GetXaxis().SetLabelSize(xLabelSize)
            histograms[varName][type].GetXaxis().SetTimeDisplay(1)
            histograms[varName][type].GetXaxis().SetTimeFormat(timeFormat)
            
            
            
            histograms[varName][type].GetYaxis().SetTitle(yLabel)
            histograms[varName][type].GetYaxis().SetTitleSize(yTitleSize)
            histograms[varName][type].GetYaxis().SetRangeUser(yMin, yMax)
            histograms[varName][type].GetYaxis().SetLabelSize(yLabelSize)
            
            marker = 20
            if type == "Mean":
                marker = 20
            elif type == "Peak":
                marker = 3
                
            histograms[varName][type].SetMarkerStyle(marker)
            histograms[varName][type].SetMarkerSize(1)
            histograms[varName][type].SetMarkerColor(color) 
    
    
    canvas = ROOT.TCanvas("canvas", "", 1000, 500)    
    canvas.SetRightMargin(0.20)

    
    isFirst = True
    crtDrawOpt = drawOpt
    for varName in variables:
        for type in histogramTypes:
            histograms[varName][type].Draw(crtDrawOpt)
            if isFirst:
                crtDrawOpt = "%s SAME" % drawOpt
                isFirst = False
                
                

    canvas.SetLogy(1)        

    leg = ROOT.TLegend(0.81, 0.10, 0.99, 0.90);
    leg.SetTextSize(0.03)    
    
    leg.SetLineWidth(1)
    leg.SetFillColor(0)    
    
    for varName in variables:
        for type in histogramTypes:
            leg.AddEntry(histograms[varName][type], varName + ", " + type, "p")
    
    
    leg.Draw()
        
        
    canvas.Update()
    
    import time
    time.sleep(1000)
#----------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    fill_summary_plot(187219, 187196)
#----------------------------------------------------------------------------------------------------