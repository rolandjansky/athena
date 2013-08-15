# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 7, 2011

@author: galster
'''

import ROOT

class TRPWorker:
    
    def JobIsOkay(self, job):
        try:
            fileROOT = ROOT.TFile(job.GetFileName())
            if not fileROOT:
                raise
            if not fileROOT.IsOpen():
                raise
        
            fileROOT.Close()
        except:
            raise Exception("Could not open file " + job.GetFileName())
        
        if job.GetLumiBlockRange()[0] > job.GetLumiBlockRange()[1]:
            raise Exception("LumiBlock range is invalid. Got %i to %i" % (job.GetLumiBlockRange()[0], job.GetLumiBlockRange()[1]))
        
        if job.GetStepSize() < 1:
            raise Exception("Invalid step size")
            
    
    
    def MakeLumiBlockCut(self, range):
        cut = ""
        start = range[0]
        stop = range[1]

        if start != -1:
            if stop != -1:
                cut = str(start) + " <= LumiBlock && LumiBlock <= " + str(stop)
            else:
                cut = str(start) + " <= LumiBlock"
        else:
            if stop != -1:
                cut = "LumiBlock <= " + str(stop)
        return cut