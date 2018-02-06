# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.01 $"
__doc__="Class defining data periods and access to GRLs"

import sys
import xml.etree.ElementTree as ET
from TriggerMenu.api.TriggerEnums import TriggerPeriod

class TriggerPeriodData:
    # From https://atlas-tagservices.cern.ch/tagservices/RunBrowser/runBrowserReport/rBR_Period_Report.php?fnt=data17_13TeV
    # period: start,end,maxlumi
    periodMap2015 = {
        'All' :(276262, 284484, 0)
    }
    periodMap2016 = {
        'AD3' :(296939,302872,0),
        'D4plus' :(302919,311481,0),
    }
    periodMap2017 = {
        'B' :(325713,328393,15815),
        'B1':(325713,325790,3216),
        'B2':(326439,326439,22.4),
        'B3':(326446,326551,7008),
        'B4':(326657,326695,6199),
        'B5':(326834,326945,7915),
        'B6':(327057,327265,11202),
        'B7':(327342,327490,14642),
        'B8':(327582,328393,15815),
        'C' :(329385,330470,16776),
        'C1':(329385,329484,289),
        'C2':(329542,329542,3422),
        'C3':(329716,329778,7729),
        'C4':(329780,329964,13293),
        'C5':(330025,330203,16365),
        'C6':(330294,330294,14993),
        'C7':(330328,330328,7213),
        'C8':(330470,330470,16776),
        'D' :(330857,332304,17466),
        'D1':(330857,330875,2.77),
        'D2':(331019,331020,3800),
        'D3':(331033,331239,16242),
        'D4':(331462,331479,15952),
        'D5':(331697,331975,16385),
        'D6':(332303,332304,17466),
        'E' :(332720,334779,14059),
        'E1':(332720,332896,3485),
        'E2':(332915,333192,9599),
        'E3':(333367,333487,10230),
        'E4':(333519,333650,10534),
        'E5':(333707,333979,9515),
        'E6':(333994,333994,14059),
        'E7':(334264,334580,9745),
        'E8':(334588,334779,9061),
        'F' :(334842,335290,12070),
        'F1':(334842,334907,10643),
        'F2':(334960,335022,11067),
        'F3':(335056,335082,12070),
        'F4':(335083,335170,10520),
        'F5':(335177,335290,10658),
        'G' :(335302,335302,1658),
        'G1':(335302,335302,1658),
        'H' :(336497,336782,14165),
        'H1':(336497,336497,306),
        'H2':(336505,336505,4224),
        'H3':(336506,336548,11603),
        'H4':(336567,336782,14165),
        'I' :(336832,337833,18456),
        'I1':(336832,337005,16400),
        'I2':(337052,337215,17078),
        'I3':(337263,337833,18456),
        'K' :(338183,340453,20614),
        'K1':(338183,338349,15706),
        'K2':(338377,339070,20486),
        'K3':(339197,339197,917),
        'K4':(339205,339205,15216),
        'K5':(339346,339849,20614),
        'K6':(339957,340453,15750),
        'N' :(341257,341649,16018),
        'N1':(341257,341294,1957),
        'N4':(341419,341649,1590),
        'N2':(341312,341312,16018),
    }
    
    grlbase = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/"
    y2017grlpath = grlbase+"data17_13TeV/20171130/data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"
    y2016grlpath = grlbase+"data16_13TeV/20170605/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"
    y2015grlpath = grlbase+"data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml"
    def __init__(self, period, customGRL=None):
        if customGRL:
            self.loadGRL(customGRL)
        elif period >= TriggerPeriod.runNumber: #run number assume 2017
            self.loadGRL(self.y2017grlpath)
        elif period & TriggerPeriod.y2015: 
            self.loadGRL(self.y2015grlpath)
        elif period & TriggerPeriod.y2016: 
            self.loadGRL(self.y2016grlpath)
        elif period & TriggerPeriod.y2017: 
            self.loadGRL(self.y2017grlpath)
        self.skimPeriod(period)

    def loadGRL(self, grlpath):
        grlroot = ET.parse(grlpath).getroot()
        self.grl = {}
        for run in grlroot.findall('NamedLumiRange/LumiBlockCollection'):
            self.grl[ int(run.find('Run').text)] = [(int(x.get('Start')), int(x.get('End'))) for x in run.findall('LBRange')]

    @classmethod
    def testCustomGRL(cls, grlpath):
        try:
            grlroot = ET.parse(grlpath).getroot()
            testgrl = {}
            for run in grlroot.findall('NamedLumiRange/LumiBlockCollection'):
                testgrl[ int(run.find('Run').text)] = [(int(x.get('Start')), int(x.get('End'))) for x in run.findall('LBRange')]
            return any(len(lb)!=0 for lb in testgrl.itervalues())
        except:
            return False

    def skimPeriod(self, period):
        if period >= TriggerPeriod.runNumber:
            if period in self.grl:
                blocks = self.grl[period]
                self.grl = {}
                self.grl[period] = blocks
            else:
                self.grl = {}
                self.grl[period] = [(0,9999)]
        else:
            ranges = []
            if period & TriggerPeriod.y2015     :
                ranges.append( self.periodMap2015['All'] )
            if period & TriggerPeriod.y2016     :
                ranges.append( self.periodMap2016['AD3'] )
                ranges.append( self.periodMap2016['D4plus'] )
            if period & TriggerPeriod.y2017periodB1:
                ranges.append( self.periodMap2017['B1'] )
            if period & TriggerPeriod.y2017periodB2B4:
                for i in range(2,4+1): ranges.append( self.periodMap2017['B%d' %i] )
            if period & TriggerPeriod.y2017periodB5B7:
                for i in range(5,7+1): ranges.append( self.periodMap2017['B%d' %i] )
            if period & TriggerPeriod.y2017periodB8     :
                ranges.append( self.periodMap2017['B8'] )
            if period & TriggerPeriod.y2017periodC     :
                for i in range(1,8+1): ranges.append( self.periodMap2017['C%d' %i] )
            if period & TriggerPeriod.y2017periodD1D5:
                for i in range(1,5+1): ranges.append( self.periodMap2017['D%d' %i] )
            if period & TriggerPeriod.y2017periodD6     :
                ranges.append( self.periodMap2017['D6'] )
            if period & TriggerPeriod.y2017periodE     :
                ranges.append( self.periodMap2017['E'] )
            if period & TriggerPeriod.y2017periodF     :
                ranges.append( self.periodMap2017['F'] )
            if period & TriggerPeriod.y2017periodGHI :
                ranges.append( self.periodMap2017['G'] )
                ranges.append( self.periodMap2017['H'] )
                ranges.append( self.periodMap2017['I'] )
            if period & TriggerPeriod.y2017periodK     :
                ranges.append( self.periodMap2017['K'] )
            if period & TriggerPeriod.y2017periodN     :
                ranges.append( self.periodMap2017['N'] )
            for run in self.grl.keys()[:]:
                if not any([run >= x[0] and run <= x[1] for x in ranges]): self.grl.pop(run)

def test():
    print TriggerPeriodData( TriggerPeriod.y2015 ).grl

if __name__ == "__main__":
    sys.exit(test())

