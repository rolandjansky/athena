# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
"""
To do:
= = = = = = = = =



"""
#----------------------------------------------------------------------------------------------------
# Load root, kill x 
import sys
sys.argv.append('-b')
print sys.argv
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.keeppolling = 0
#----------------------------------------------------------------------------------------------------

ROOT.gROOT.Reset()
ROOT.gROOT.SetStyle("Plain")

ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetOptStat(000000)
ROOT.gStyle.SetPalette(1)

ROOT.gStyle.SetTitleBorderSize(0)
#----------------------------------------------------------------------------------------------------
from model.COOL.COOLTime import COOLTime
#----------------------------------------------------------------------------------------------------
from model.basics.DataSelection import DataSelection
from model.basics.DataRange import DataRange
from model.basics.DataColumn import DataColumn
#----------------------------------------------------------------------------------------------------
from model.physics.LuminosityCollection import LuminosityCollection
from model.physics.ChainCollection import ChainCollection
from model.physics.StreamCollection import StreamCollection 
from model.physics.TimeCollection import TimeCollection
#----------------------------------------------------------------------------------------------------
from model.physics.filters.FilterRunDuration import FilterRunDuration 
from model.physics.filters.FilterPhysicsReady import FilterPhysicsReady
from model.physics.filters.FilterDeadTime import FilterDeadTime
#----------------------------------------------------------------------------------------------------
class RatesRun(object):
    def __init__(self, timeFilter = DataSelection()):
        self.SetRunDurationFilter(FilterRunDuration("Definition"))
        self.GetRunDurationFilter().Enable()
        
        self.SetDeadTimeFilter(FilterDeadTime(0.05))
        self.GetDeadTimeFilter().Enable()
        
        self.SetPhysicsReadyFilter(FilterPhysicsReady("Intersection"))
        self.GetPhysicsReadyFilter().Enable()
        
        self.SetListOfFilters([])
        
        self.SetTimeFilter(timeFilter)

        
        self.SetTotalRatesCollection(StreamCollection(self.GetTimeFilter(), self))        
        
        
        self.SetTimeCollection(TimeCollection(self.GetTimeFilter(), self))
        
        self.SetStreamCollection(StreamCollection(self.GetTimeFilter(), self))
        
        self.SetLuminosityCollection(LuminosityCollection(self.GetTimeFilter(), self))
        
        
        
        self.SetChainCollection(ChainCollection(self.GetTimeFilter(), self))
        
        
        self.ApplyTimeFilter()
        """
        self.GetChainCollection().SetTimeFilter(timeFilter)
        self.GetChainCollection().LoadDictionaries()
        self.GetChainCollection().SetTimeFilter(DataSelection())
        """
    
    
        
    def GetListOfFilters(self): return self.__listOfFilters
    def SetListOfFilters(self, val): self.__listOfFilters = val
    
    def AddSelectionFilter(self, val): self.__listOfFilters.append(val)
    def RemoveSelectionFilter(self, val):
        if val in self.GetListOfFilters():
            self.__listOfFilters.remove(val)
     

    def GetRunNumbers(self): return self.__runNumbers            
    def GetTimeFilter(self): return self.__timeFilter
    def SetTimeFilter(self, val):
        selection = val
        
        selection = self.GetRunDurationFilter().FilterValues(selection)
        
        selection = self.GetPhysicsReadyFilter().FilterValues(selection)
        
        selection = self.GetDeadTimeFilter().FilterValues(selection)
        
        for filter in self.GetListOfFilters():
            selection = filter.FilterValues(selection)
        
        

        
        self.__runNumbers = set([])
        for timeRange in selection.GetListOfRanges():
            for runNumber in range(COOLTime(timeRange.GetFirst()).GetRunNumber(), \
                                   COOLTime(timeRange.GetLast()).GetRunNumber()+1):
                self.__runNumbers.add(runNumber)
        
        
        
        
        self.__timeFilter = selection
        
    def ApplyTimeFilter(self, val = None):
        if val == None: val = self.GetTimeFilter()
        self.__chainCollection.SetTimeFilter(val)
        self.__luminosityCollection.SetTimeFilter(val)
        self.__streamCollection.SetTimeFilter(val)
        self.__totalRatesCollection.SetTimeFilter(val) 
        self.__timeCollection.SetTimeFilter(val)
        
    
    def GetChainCollection(self): return self.__chainCollection
    def SetChainCollection(self, val): self.__chainCollection = val

    def GetLuminosityCollection(self): return self.__luminosityCollection
    def SetLuminosityCollection(self, val): self.__luminosityCollection = val    

    def GetStreamCollection(self): return self.__streamCollection
    def SetStreamCollection(self, val): self.__streamCollection = val    

    def GetTotalRatesCollection(self): return self.__totalRatesCollection
    def SetTotalRatesCollection(self, val): self.__totalRatesCollection = val

    def GetTimeCollection(self): return self.__timeCollection
    def SetTimeCollection(self, val): self.__timeCollection = val
    
    
    
    def GetRunDurationFilter(self): return self.__runDurationFilter
    def SetRunDurationFilter(self, val): self.__runDurationFilter = val

    def GetPhysicsReadyFilter(self): return self.__physicsReadyFilter
    def SetPhysicsReadyFilter(self, val): self.__physicsReadyFilter = val    
    
    def GetDeadTimeFilter(self): return self.__deadTimeFilter
    def SetDeadTimeFilter(self, val): self.__deadTimeFilter = val    
    
    def ToDataDictionary(self):
        return dict( \
            self.GetTimeCollection().ToDataDictionary().items() \
            + self.GetLuminosityCollection().ToDataDictionary().items() \
            + self.GetTotalRatesCollection().ToDataDictionary().items() \
            + self.GetStreamCollection().ToDataDictionary().items() \
            + self.GetChainCollection().ToDataDictionary().items() \
        )
    
    
    def PlotFromCollection(self, collection, *varNames, **kwargs):
        print kwargs
        asFunctionOf = "UnixTime"
        if kwargs.has_key("asFunctionOf"): asFunctionOf = kwargs["asFunctionOf"]
        
        usingRate = None
        if kwargs.has_key("usingRate"): usingRate = kwargs["usingRate"]
        
        
        drawOpt = ""
        if kwargs.has_key("drawOpt"): drawOpt = kwargs["drawOpt"]
        
        printOpt = None
        if kwargs.has_key("printOpt"): printOpt = kwargs["printOpt"]        
        
        plotType = 1
        for varName in varNames:
            if len(varName) >= 17:
                plotType = 2
                                
        if kwargs.has_key("plotType"): plotType = kwargs["plotType"]
        
        logY = False
        if kwargs.has_key("logY"): logY = kwargs["logY"]
        
        logX = False
        if kwargs.has_key("logX"): logX = kwargs["logX"]
        
        
        title = "No title"
        if kwargs.has_key("title"): title = kwargs["title"]
        
        timeFormat = "%d/%m %Hh"
        if kwargs.has_key("timeFormat"): timeFormat = kwargs["timeFormat"]
        
        nVars = len(varNames)
        if nVars == 0:
            return
        
        collection.Load(*varNames)
        
        xData = dict(\
                     self.GetLuminosityCollection().ToDataDictionary().items() +\
                     self.GetTimeCollection().ToDataDictionary().items() \
                     )
        
        if not xData.has_key(asFunctionOf): raise Exception("Cannot plot as function of %s" % asFunctionOf)
        
        yData = collection.ToDataDictionary(*varNames, extract = usingRate)
        
        
        
        xExtrema = DataColumn()
        xExtrema.append(xData[asFunctionOf].Min())
        xExtrema.append(xData[asFunctionOf].Max()) 
        
        yExtrema = DataColumn()
        for varName in yData.keys():
            if not varName == "LumiBlock":
                yExtrema.append(yData[varName].Min())
                yExtrema.append(yData[varName].Max())         
        
        
        multiplicator = 0.05
        if logX: multiplicator = 10**(multiplicator)
        
        xMax = xExtrema.Max() + (xExtrema.Max()-xExtrema.Min()) * 0.05                  
        xMin = xExtrema.Min() - (xExtrema.Max()-xExtrema.Min()) * 0.05

        multiplicator = 0.05
        if logY: multiplicator = 10**(multiplicator)
                
        yMax = yExtrema.Max() + (yExtrema.Max()-yExtrema.Min()) * 0.05
        yMin = yExtrema.Min() - (yExtrema.Max()-yExtrema.Min()) * 0.05
 
        
        if kwargs.has_key("xMax"): xMax = kwargs["xMax"]
        if kwargs.has_key("xMin"): xMin = kwargs["xMin"]
        if kwargs.has_key("yMax"): yMax = kwargs["yMax"]
        if kwargs.has_key("yMin"): yMin = kwargs["yMin"]
        
        
        
        xBins = 1000
        if kwargs.has_key("xBins"): xBins = kwargs["xBins"]
        yBins = 1000
        if kwargs.has_key("yBins"): xBins = kwargs["yBins"]        

        
        
        
        xLabel = ""
        if asFunctionOf == "Luminosity":
            xLabel = "Luminosity [10^{30} cm^{-2} s^{-1}]"
        
        if asFunctionOf == "UnixTime":
            xLabel = "Time"            
        
        if kwargs.has_key("xLabel"): xLabel = kwargs["xLabel"]
        
        
        
        yLabel = ""
        if kwargs.has_key("yLabel"): yLabel = kwargs["yLabel"]
        
        
        
        xLabelSize = 0.035
        yLabelSize = 0.035
        if plotType == 1:
            xLabelSize = 0.035
            yLabelSize = 0.035
        elif plotType == 2:
            xLabelSize = 0.025
            yLabelSize = 0.025
        if kwargs.has_key("xLabelSize"): xLabelSize = kwargs["xLabelSize"]
        if kwargs.has_key("yLabelSize"): yLabelSize = kwargs["yLabelSize"]
        
        
        xTitleSize = 0.04
        yTitleSize = 0.04
        if plotType == 1:
            xTitleSize = 0.04
            yTitleSize = 0.04
        elif plotType == 2:
            xTitleSize = 0.03
            yTitleSize = 0.03
        if kwargs.has_key("xTitleSize"): xTitleSize = kwargs["xTitleSize"]
        if kwargs.has_key("yTitleSize"): yTitleSize = kwargs["yTitleSize"]        
        
        
        
        
        n = 0
        histograms = {}
        for varName in varNames:
            n += 1
            histograms[varName] = ROOT.TH2F(varName, varName, xBins, xMin, xMax, yBins, yMin, yMax)
            histograms[varName].SetTitle(title)
    
            
            
            histograms[varName].GetXaxis().SetTitle(xLabel)
            histograms[varName].GetXaxis().SetTitleSize(xTitleSize)
            histograms[varName].GetXaxis().SetRangeUser(xMin, xMax)
            histograms[varName].GetXaxis().SetLabelSize(xLabelSize)
            if asFunctionOf == "UnixTime" or asFunctionOf == "Time":
                histograms[varName].GetXaxis().SetTimeDisplay(1)
                histograms[varName].GetXaxis().SetTimeFormat(timeFormat)
            
            
            
            histograms[varName].GetYaxis().SetTitle(yLabel)
            histograms[varName].GetYaxis().SetTitleSize(yTitleSize)
            histograms[varName].GetYaxis().SetRangeUser(yMin, yMax)
            histograms[varName].GetYaxis().SetLabelSize(yLabelSize)
            
            histograms[varName].SetMarkerStyle(20)
            histograms[varName].SetMarkerSize(1)
            histograms[varName].SetMarkerColor(n)
            
        for i in range(len(yData["LumiBlock"])):
            for varName in varNames:
                if xData[asFunctionOf][i] == None or yData[varName][i] == None: continue
                
                histograms[varName].Fill( \
                                       xData[asFunctionOf][i], \
                                       yData[varName][i], \
                                       1.0 \
                                      )
               
        canvas = None
        
        if plotType == 1:
            canvas = ROOT.TCanvas(title, title, 1000, 500)
            canvas.SetRightMargin(0.20)        
        elif plotType == 2:
            canvas = ROOT.TCanvas(title, title, 888, 800)
            canvas.SetBottomMargin(0.27)
        else:
            canvas = ROOT.TCanvas(title, title)
        
        
        isFirst = True
        crtDrawOpt = drawOpt
        for varName in varNames:
            histograms[varName].Draw(crtDrawOpt)
            if isFirst:
                crtDrawOpt = "%s SAME" % drawOpt
                isFirst = False            
        
        
        canvas.SetLogy(int(logY))
        canvas.SetLogx(int(logX))
        
        
        leg = None         
        if plotType == 1:
            leg = ROOT.TLegend(0.81, 0.10, 0.99, 0.90);
            leg.SetTextSize(0.03)
        elif plotType == 2:
            leg = ROOT.TLegend(0.10, 0.19, 0.90, 0.01);
            leg.SetTextSize(0.02)
        else:
            leg = ROOT.TLegend(0.81, 0.10, 0.99, 0.90);
        
        leg.SetLineWidth(1)
        leg.SetFillColor(0)
        
        #leg.SetLineColor(0)
                
        for name in histograms.keys():
            leg.AddEntry(histograms[name], name, "p")

        leg.Draw()
        
        
        canvas.Update()
        
        if not printOpt == None:
            canvas.Print(printOpt)
            
        
        import time
        time.sleep(10)


    
    def PlotStreamRates(self, *streamNames, **kwargs):
        def updateOption(name, val):
            if not kwargs.has_key(name): kwargs[name] = val
        
        updateOption("yLabel", "Rate [Hz]")
        
        updateOption("yLog", True)
        
        #updateOption("title", "Rates for run %i" % self.GetRunNumber())
        updateOption("title", "Rates for run")
     
        print kwargs
           
        self.PlotFromCollection(self.GetStreamCollection(), *streamNames, **kwargs)        
    
    def PlotTotalRates(self, *streamNames, **kwargs):
        def updateOption(name, val):
            if not kwargs.has_key(name): kwargs[name] = val
        
        updateOption("yLabel", "Rate [Hz]")
        
        updateOption("yLog", True)
        
        #updateOption("title", "Rates for run %i" % self.GetRunNumber())
        updateOption("title", "Rates for run")
     
        print kwargs
           
        self.PlotFromCollection(self.GetTotalRatesCollection(), *streamNames, **kwargs)    
    
    
    def PlotChainRates(self, *streamNames, **kwargs):
        def updateOption(name, val):
            if not kwargs.has_key(name): kwargs[name] = val
        
        updateOption("yLabel", "Rate [Hz]")
        
        #updateOption("title", "Rates for run %i" % self.GetRunNumber())
        updateOption("title", "Rates for run")
     
        print kwargs        
        
        self.PlotFromCollection(self.GetChainCollection(), *streamNames, **kwargs)
        
        
    def Load(self):
        self.SetTimeFilter(self.GetTimeFilter())  
        self.ApplyTimeFilter()
              
        print "Loading time"
        self.__timeCollection.Load()
                
        print "Loading luminosity"
        self.__luminosityCollection.Load()
        
        print "Loading chains"
        self.__chainCollection.Load()

        print "Loading streams"
        self.__streamCollection.Load()
        
        print "Loading total rates"
        self.__totalRatesCollection.Load()
                
        print "Chains:", self.__chainCollection.GetNames()
        print "Streams:", self.__streamCollection.GetNames()
        print "Totals:", self.__totalRatesCollection.GetNames()

#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
     
    def make_dir(folder):
        folderAtoms = folder.split("/")
        atoms = []
        for atom in folderAtoms:
            atoms.append(atom)
            subFolder = "/".join(atoms)
            if not os.path.exists(subFolder) and len(subFolder):
                os.mkdir(subFolder)
        
    
    from control.DownloadTRP import get_runs
    from model.COOL.COOLQuery import COOLQuery
    import os
    
    totals = ["EF_total","L2_total", "L1_L1A"]
    triggersDictionary = {
                          "L": {
                                    "egammamuon": ["EF_e22vh_medium1", "EF_2e12Tvh_medium", "EF_g80_loose", "EF_mu18_medium", "EF_mu15_mu10_EFFS_medium"],
                                    "jettauetmiss": ["EF_tau125_medium1", "EF_j240_a4tc_EFFS", "EF_5j30_a4tc_EFFS", "EF_xe60_verytight_noMu", "EF_j75_a4tc_EFFS_xe55_noMu"], 
                                },
                          }
        
    function = ["Time", "Luminosity"]
    
    
    period = "L"
    year = 2011
    
    #triggers = ["EF_e22vh_medium1", "EF_2e12Tvh_medium", "EF_g80_loose", "EF_mu18_medium", "EF_mu15_mu10_EFFS_medium", "EF_tau125_medium1", "EF_j240_a4tc_EFFS", "EF_5j30_a4tc_EFFS", "EF_xe60_verytight_noMu", "EF_j75_a4tc_EFFS_xe55_noMu", "L2_e22vh_medium1", "L2_2e12Tvh_medium", "L2_g80_loose", "L2_mu18_medium", "L2_mu15_mu10_EFFS_medium", "L2_tau125_medium1", "L2_j240_a4tc_EFFS", "L2_5j30_a4tc_EFFS", "L2_xe60_verytight_noMu", "L2_j75_a4tc_EFFS_xe55_noMu"]
    
    dataRangeList = []
    for runNumber in get_runs(year, period):
        print "[+] Processing",runNumber
        
        folder = "/tmp/ggalster/plots/run/%i/" % runNumber
    
        selection = DataSelection(*COOLTime().Run(runNumber))
        run = RatesRun()
        
        #run.GetRunDurationFilter().Disable()
        #run.GetDeadTimeFilter().Disable()
        
        run.SetTimeFilter(selection)
        
        run.Load()
        
        
        length = len(run.GetTimeFilter().ToSet())
        if not length:
            print "[-] Run has no valid duration, skipping"
            continue
        else:
            print "[?] Subjective length of run is", length, "lumiblocks"
            
            if length < 20:
                print "[-] Less than 20 lumiblocks to plot, skipping run."
                continue

        
        
        
        
        for grouping, triggers in triggersDictionary[period].items():
            run.GetChainCollection().Load(*triggers)
    
            workingTriggers = []
            for trigger in triggers:
                if run.GetChainCollection().Get(trigger).GetIsDummy(): continue
                
                workingTriggers.append(trigger)
            
            if not len(workingTriggers):
                print "[-] No trigger info for ", grouping, ", run:", runNumber
            else:
                print "[+] Plotting", grouping, "using", len(workingTriggers), "trigger(s)"
                make_dir(folder)
                  
                
                plotTitle = "Rates for run %i, period %s (%i)" % (runNumber, period, year)
                
                for func in function:
                    outputName = folder + grouping+ "_" + func + ".png"
                    run.PlotChainRates(*workingTriggers, printOpt=outputName, title=plotTitle, asFunctionOf=func, logY=True, yMin=1)    
        
        
        
        
        
    
        run.GetTotalRatesCollection().Load(*totals)
        
        workingTotals = []
        for total in totals:
            if not run.GetTotalRatesCollection().Get(total).GetIsDummy():
                workingTotals.append(total)
        
        if not len(workingTotals):
            print "[-] No total info for run:", runNumber

        else:
            print "[+] Plotting total rates using", len(workingTotals), "total(s)"
            
            make_dir(folder)
            
            plotTitle = "Total rates for run %i, period %s (%i)" % (runNumber, period, year)
            
            for func in function:
                outputName = folder + "TotalRate" + "_" + func + ".png"
                run.PlotTotalRates(*workingTotals, printOpt=outputName, title=plotTitle, logY = True, asFunctionOf=func, yMax = 75000)
        
        
        
        
        for dataRange in run.GetTimeFilter().GetListOfRanges():
            dataRangeList.append(dataRange)
        
        
        
        
    
    
    
    
    
    
    
    
    
    run = RatesRun()
        
    run.GetRunDurationFilter().Disable()
    run.GetDeadTimeFilter().Disable()
    run.GetPhysicsReadyFilter().Disable()
    
    
    folder = "/tmp/ggalster/plots/period/%i/%s/" % (year, period)
    
    selection = DataSelection()
    selection.SetListOfRanges(dataRangeList)
    
    run.SetTimeFilter(selection)
    
    run.Load()
    
    for grouping, triggers in triggersDictionary[period].items():
        run.GetChainCollection().Load(*triggers)
    
        workingTriggers = []
        for trigger in triggers:
            if run.GetChainCollection().Get(trigger).GetIsDummy(): continue
            
            workingTriggers.append(trigger)
        
        if not len(workingTriggers):
            print "[-] No trigger info for ", grouping, ", period:", period
        else:
            print "[+] Plotting", grouping, "using", len(workingTriggers), "trigger(s)"
            make_dir(folder)
            outputName = folder + grouping+ ".png"  
            
            plotTitle = "Rates for period %s (%i)" % (period, year)
            run.PlotChainRates(*workingTriggers, printOpt=outputName, title=plotTitle, asFunctionOf="Luminosity", logY=True, yMin=1)    
        
    
    
    
    
    
    run.GetTotalRatesCollection().Load(*totals)
    
    workingTotals = []
    for total in totals:
        if not run.GetTotalRatesCollection().Get(total).GetIsDummy():
            workingTotals.append(total)
    
    if not len(workingTotals):
        print "[-] No total info for period:", period
        
    else:
        print "[+] Plotting total rates using", len(workingTotals), "total(s)"
        
        make_dir(folder)
        outputName = folder + "TotalRate.png"
        plotTitle = "Total rates for period %s (%i)" % (period, year)
        
        run.PlotTotalRates(*workingTotals, printOpt=outputName, title=plotTitle, logY = True, asFunctionOf="Luminosity", yMax = 75000)

    
    
    
    """
    dataDict = run.ToDataDictionary()
    print run.GetTimeFilter()
    print "Mean:", dataDict["Luminosity"].Mean(), "+/-", dataDict["Luminosity"].Error(), "Max:", dataDict["Luminosity"].Max()
     
     
    selectionRange = run.GetTimeFilter().GetSuperSetAsRange()
     
    selection = DataSelection(selectionRange.GetFirst(), selectionRange.GetFirst() + 30)
     
    run.SetTimeFilter(selection)
    run.Load()
     
    dataDict = run.ToDataDictionary()
    print run.GetTimeFilter()
    print "Mean", dataDict["Luminosity"].Mean(), "+/-", dataDict["Luminosity"].Error(), "Max:", dataDict["Luminosity"].Max()
    """
     
     
"""
    runNumbers = [189288, 189280, 189242, 189207, 189205, 189184, 186877]
    
    
    selection = DataSelection()
    ct = COOLTime()
    ct
    listOfRanges = []
    for i in set(runNumbers):
        listOfRanges.append(DataRange(*ct.Run(i)))
        
    selection.SetListOfRanges(listOfRanges)
    run = RatesRun(selection)
    
    print selection
    
    run.Load()
    

    
    run.PlotChainRates("EF_e22_medium", "EF_mu18_medium", "EF_xe70_noMu", loe gY = True, printOpt = "/tmp/ggalster/Chains.png")
    
    run.PlotStreamRates("L1_L1A", "L2_total", "EF_total", yMin = 200, yMax = 75000, logY = True, printOpt = "/tmp/ggalster/Totals.png")

    run.PlotStreamRates("EF_str_Egamma_physics", "EF_str_Muons_physics", "EF_str_JetTauEtmiss_physics", "EF_str_MinBias_physics", "EF_str_express_express", logY = True, printOpt = "/tmp/ggalster/Streams.png")
    
"""
"""    
    for f in ["Time", "Luminosity"]:
        for using in ["RateAfterDecision", "OverallOutputRate"]:
            run.PlotChainRates("EF_e22_medium", "EF_mu18_medium", "EF_xe70_noMu", usingRate = using, asFunctionOf = f, logY = True)
""" 
#----------------------------------------------------------------------------------------------------
