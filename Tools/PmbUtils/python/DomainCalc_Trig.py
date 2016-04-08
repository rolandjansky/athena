# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

#  Very simple so far - simply define jobs named directly by their
#  userJobId's, and do not define any domain definitions.

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_Trig(DomainCalcBase):

    def __init__(self):
        return

    def getName(self):
        return "trig"
    def getPrettyName(self):
        return "Trigger"

    def defineJobs(self):
        return [ "Bjet",
                 "Electron",
                 "Jet",
                 "ETmiss",
                 "MinBias",
                 "Muon",
                 "Photon",
                 "Tau",
                 ]

    def getCDJobDefinitions(self,dbkey):
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        l=[]
        for j in self.defineJobs():
            l += [ PerfMonDozerJobDef(dbkey,'Trigger/TrigValidation/TriggerTest',j,j) ]
        return l
