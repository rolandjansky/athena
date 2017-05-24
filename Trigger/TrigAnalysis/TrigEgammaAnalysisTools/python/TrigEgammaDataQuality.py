# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Various classes for retaining 
# pertinent information for Data Quality assessment
# Allows for easy use of creating DQ plots in 
# trigEgammaDQ.py

# Class to retain maps of trigger names and plots
class TrigEgammaPlotHolder:
    def __init__(self,name):
        self.name = name
        self.RunNumber=""
        self.Lumi=""
        self.Sequence=""
        self.Triggers=[]
        self.Efficiency={}
        self.Distribution={}
        self.Resolution={}
        self.Category={}
        self.Map={'Efficiency':self.Efficiency,
                'Distribution':self.Distribution,
                'Resolution':self.Resolution,
                'Category':self.Category}
    def setRunNumber(self,run):
        if not self.RunNumber:
            self.RunNumber=run
    def setSequence(self,seq):
        if not self.Sequence:
            self.Sequence=seq
    def setLumi(self,lumi):
        if not self.Lumi:
            self.Lumi=lumi

    def updateTriggers(self,trigger):
        if(trigger not in self.Triggers):
            self.Triggers.append(trigger)

    def updateEfficiency(self,trigger,histos):
        self.Efficiency.update({trigger:histos})
    def updateDistribution(self,trigger,histos):
        self.Distribution.update({trigger:histos})
    def updateResolution(self,trigger,histos):
        self.Resolution.update({trigger:histos})
    def updateCategory(self,trigger,histos):
        self.Category.update({trigger:histos})

    def updatePlots(self,trigger,plot,histos):
        if('Efficiency' in plot):
            self.updateEfficiency(trigger,histos)
        elif('Distributions' in plot):
            self.updateDistribution(trigger,histos)
        elif('Resolutions' in plot):
            self.updateResolution(trigger,histos)
        else:
            print 'No update'

    def getPlots(self,trigger,plot):
        group = self.Map[plot]
        if(trigger in group):
            return group[trigger]
        else:
            return []



