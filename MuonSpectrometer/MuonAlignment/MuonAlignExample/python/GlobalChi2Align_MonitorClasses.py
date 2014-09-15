# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   GlobalChi2Align_MonitorClasses.py
## @brief:  Interface classes which setup and call the plotting scripts.
## @author: Steffen Kaiser <steffen.kaiser@cern.ch>
## @date:   11/2009

import os

#-----------------------------------------------------------------------
class GlobalChi2Plotter:
    """Interface class to the script which plots the global chi2
    versus the chamber shift."""
    def __init__(self,scriptpath="."):
        self.scriptPath    = scriptpath
        self.executable    = "plots_x"
        self.outputPath     = ""
        self.inputPath     = ""
        self.rootName      = ""
        self.iteration     = 0
        self.L1            = False
        self.absoluteLines = False
        self.debug         = False
    def run(self):
        runstr = self.scriptPath + "/" + self.executable
        runstr += " -r " + self.rootName
        runstr += " -j " + str(self.iteration)
        if self.outputPath:
            runstr += " -o " + self.outputPath
        if self.inputPath:
            runstr += " -i " + self.inputPath
        if self.L1:
            runstr += " -l "
        if self.absoluteLines:
            runstr += " -a "
        if self.debug:
            runstr += " -d " 
        print "run script: ", runstr
        os.system(runstr)

#-----------------------------------------------------------------------
class TrendPlotter:
    """Interface class to the script which plots the change of the
    alignment parameters with the iterations."""
    def __init__(self,scriptpath="."):
        self.scriptPath    = scriptpath
        self.executable    = "plottrend_x"
        self.outputPath     = ""
        self.inputPath     = ""
        self.rootName      = ""
        self.L1            = False
        self.drawErrors    = False
        self.combineTower  = False
    def run(self):
        runstr = self.scriptPath + "/" + self.executable
        runstr += " -r " + self.rootName
        if self.outputPath:
            runstr += " -o " + self.outputPath
        if self.inputPath:
            runstr += " -i " + self.inputPath
        if self.L1:
            runstr += " -l "
        if self.drawErrors:
            runstr += " -e "
        if self.combineTower:
            runstr += " -c "    
        print "run script: ", runstr
        os.system(runstr)

#-----------------------------------------------------------------------
class ResidualPlotter:
    """Interface class to the script which plots the residuals."""
    def __init__(self,scriptpath="."):
        self.scriptPath    = scriptpath
        self.executable    = "plotres_x"
        self.outputPath     = ""
        self.fileNames     = []
        self.legendLabels  = []
        self.colors        = []
        self.type          = ""
    def run(self):
        runstr = self.scriptPath + "/" + self.executable
        runstr += " -i " + getString(self.fileNames)
        runstr += " -l " + getString(self.legendLabels)
        runstr += " -c " + getString(self.colors)
        if self.outputPath:
            runstr += " -o " + self.outputPath
        if self.type:
            runstr += " -t " + self.type
        print "run script: ", runstr
        os.system(runstr)
        #print os.popen(runstr).readlines()
        #ret,out = commands.getstatusoutput(runstr)
        #if ret != 0:
           # print "error when running: ",self.executable

#-----------------------------------------------------------------------
class Chi2ValidationPlotter:
    """Interface class to the script which makes validation plots."""
    def __init__(self,scriptpath="."):
        self.scriptPath    = scriptpath
        self.executable    = "chi2validation_x"
        self.outputPath     = ""
        self.inputPath     = ""
        self.rootName      = ""
        self.iteration     = 0
        self.L1            = False
        self.absoluteLines = False
        self.debug         = False
    def run(self):
        runstr = self.scriptPath + "/" + self.executable
        runstr += " -r " + self.rootName
        runstr += " -j " + str(self.iteration)
        if self.outputPath:
            runstr += " -o " + self.outputPath
        if self.inputPath:
            runstr += " -i " + self.inputPath
        if self.L1:
            runstr += " -l "
        if self.absoluteLines:
            runstr += " -a "
        if self.debug:
            runstr += " -d " 
        print "run script: ", runstr
        os.system(runstr)

#-----------------------------------------------------------------------
def getString(optionlist):
    """Method which concatenates a list to a comma separated string."""
    optionstring = ""
    for option in optionlist:
        if len(optionstring) > 0:
            optionstring += ","
        optionstring += str(option)
    return optionstring
