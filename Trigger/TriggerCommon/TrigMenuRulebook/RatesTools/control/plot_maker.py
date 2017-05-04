# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from control.DownloadTRP import get_runs, get_periods 


from model.parallelization.ProcessMaster import ProcessMaster

from model.parallelization.ProcessWorker import ProcessWorker
from model.parallelization.ProcessJob import ProcessJob 

from model.COOL.COOLTime import COOLTime

from model.basics.DataRange import DataRange
from model.basics.DataSelection import DataSelection 

from model.physics.RatesRun import RatesRun

import time


import os
def mkdir(path):
    if not len(path): return
        
    atoms = path.split("/")
    if atoms[0] == "":
        newFolder = atoms[len(atoms)-1]
        if newFolder == "": return
        
        atoms.remove(newFolder)
        containingFolder = "/".join(atoms)
        
        if not os.path.exists(containingFolder):
            mkdir(containingFolder)
        
        os.mkdir(path)    
    else:
        mkdir("%s/%s" % (os.environ["PWD"], path))








dictionary_of_doom = \
{
    2011: \
        {
            "L": \
                {
                    "egammamuon": ["EF_e22vh_medium1", "EF_2e12Tvh_medium", "EF_g80_loose", "EF_mu18_medium", "EF_mu15_mu10_EFFS_medium"],
                    "jettauetmiss": ["EF_tau125_medium1", "EF_j240_a4tc_EFFS", "EF_5j30_a4tc_EFFS", "EF_xe60_verytight_noMu", "EF_j75_a4tc_EFFS_xe55_noMu"],   
                 }
         }
}



totals = ["L1_L1A", "L2_total", "EF_total"]

streams = \
{
    "L2": [],
    "EF": [],
}

streams = {}









class PlotJob(ProcessJob):
    def __init__(self):
        ProcessJob.__init__(self)
        
    def GetFullRatesRun(self): return self.__ratesRun
    def SetFullRatesRun(self, val): self.__ratesRun = val 
        
    def GetSelection(self): return self.__selection
    def SetSelection(self, val): self.__selection = val        

    def GetYear(self): return self.__year
    def SetYear(self, val): self.__year = val
    
    def GetPeriod(self): return self.__period
    def SetPeriod(self, val): self.__period = val



class PlotWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(PlotJob().GetType())
        
    
    def ExecuteJob(self, job):
        run = job.GetFullRatesRun()
        selection = job.GetSelection()
        selectionSuperSet = selection.GetSuperSetAsRange()
        
        year = job.GetYear()
        period = job.GetPeriod()

        run.GetDeadTimeFilter().Disable()
        run.GetPhysicsReadyFilter().Disable()
        run.GetRunDurationFilter().Disable()
        
        
        #subSelection = run.GetTimeFilter().Intersection(selection)
        #if subSelection.IsEmpty(): return
        
        plotRun = COOLTime(selectionSuperSet.GetFirst()).GetRunNumber() == COOLTime(selectionSuperSet.GetLast()).GetRunNumber()  

        
        
        
        #run.SetTimeFilter(subSelection)
        #run.Load()
        
        
        plotAgainsts = []
        
        runNumber = None
        folder = "/tmp/ggalster/plots2/"
        titlePostfix = ""
        
        if plotRun:
            runNumber = COOLTime(selectionSuperSet.CenterOfGravity()).GetRunNumber()
            
            plotAgainsts = ["Time", "Luminosity"]            
            
            titlePostfix = "for run %i, period %s (%i)" % (runNumber, period, year)
            folder += "run/%i/" % runNumber
            
            mkdir(folder)
            
        else:
        
            plotAgainsts = ["Luminosity"]
            
            titlePostfix =  "for period %s (%i)" % (period, year)
            folder += "period/%i/%s/" % (year, period)
            
            mkdir(folder)




        periodLetter = period
        if len(period) != 1:
            for chr in periodLetter:
                if chr.isalpha():
                    periodLetter = chr
                    break

    
        triggerDict = dictionary_of_doom[year][periodLetter]
        
        for plotAgainst in plotAgainsts:
            for grouping, triggers in triggerDict.items():
                
                outputName = "%s%s_%s.eps" % (folder, grouping, plotAgainst)
                outputName = outputName.lower()
                
                plotTitle = "Rates%s" % titlePostfix
                
                run.PlotChainRates(*triggers, asFunctionOf=plotAgainst, printOpt=outputName, title=plotTitle, logY=True, yMin=1)
        
            
            
            
            outputName = "%s%s_%s.eps" % (folder, "total", plotAgainst)
            outputName = outputName.lower()
            
            plotTitle = "Total rates%s" % titlePostfix
            run.PlotTotalRates(*totals, asFunctionOf=plotAgainst, printOpt=outputName, title=plotTitle, logY=True, yMin=1, yMax = 75000)
            
        

        if plotRun:
            for level, streamCol in streams.items():
                outputName = "%sstream_%s_%s.eps" % (folder, level, plotAgainst)
                outputName = outputName.lower()
                
                plotTitle = "%s stream rates%s" % (level, titlePostfix)
            
                run.PlotStreamRates(*streamCol, printOpt=outputName, title=plotTitle, logY=True, yMin=1)            
        
        


def main():
    
    year = 2011
    periodLetter = "L"
    
    
    print "[+] Getting sub periods"
    periods = []
    for period in get_periods(year):
        if periodLetter in period and not "to" in period and not periodLetter == period:
            periods.append(period)
    
    runsByPeriod = {}
    for period in periods:
        runsByPeriod[period] = get_runs(year, period)


    print "[+] Setting up ranges and dividing  into sub periods"
    rangesByPeriod = {}
    for period, runNumbers in runsByPeriod.items():
        rangesByPeriod[period] = []
        for runNumber in runNumbers:
            rangesByPeriod[period].append( DataRange(*COOLTime().Run(runNumber)) )
    

    print "[+] Prepare initial load."
    rangesTotal = []
    for ranges in rangesByPeriod.values():
        rangesTotal = rangesTotal + ranges
        
    selection = DataSelection()
    selection.SetListOfRanges(rangesTotal)
    
    rangesByPeriod[periodLetter] = [ selection.GetSuperSetAsRange() ]
    
    
    """
    for period, ranges in rangesByPeriod.items():
        for ran in ranges:
            print period, "\t", COOLTime(ran.GetFirst()), "\t", COOLTime(ran.GetLast())
    """
    
    
    
    print "[+] Initial load...",
    run = RatesRun()    
    
    run.GetDeadTimeFilter().Disable()
    run.GetRunDurationFilter().Disable()
    run.GetPhysicsReadyFilter().Disable()
    
    
    run.SetTimeFilter(selection)
    
    
    run.Load()
    
        
    for triggers in dictionary_of_doom[year][period].values():
        print ".",
        run.GetChainCollection().Load(*triggers)
    
    for stream in streams.values():
        print ".",
        run.GetStreamCollection().Load(*stream)
        
    print "."
    run.GetTotalRatesCollection().Load(*totals)
    
    
    
    
    
    
    processMaster = ProcessMaster()
    
    numWorkers = min(8, len(rangesTotal))
    
    print "[+] Adding %i workers..." % numWorkers
    for i in range(numWorkers):
        processMaster.AddWorker(PlotWorker())
    
    
    for period, ranges in rangesByPeriod.items():
        for ran in ranges:
            job = PlotJob()
            job.SetFullRatesRun(run)
            
            subSelection = DataSelection(ran.GetFirst(), ran.GetLast())
            
            job.SetSelection(subSelection)
            
            job.SetYear(year)
            job.SetPeriod(period)
            
            
            
            #processMaster.SubmitJob(job)
        
    
    while processMaster.IsActive():
        time.sleep(1)
        
        
    
    

if __name__ == "__main__":
    main()