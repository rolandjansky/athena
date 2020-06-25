# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# define histograms for monitoring of HLT jets
def getMonTool_TrigJetAlgorithm(path):
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool')
  monTool.HistPath = path
  monTool.defineHistogram( 'nJets', path='EXPERT', type='TH1F', title='Counts', xbins=100, xmin=0, xmax=100 )
  monTool.defineHistogram( 'pt',    path='EXPERT', type='TH1F', title='Counts', xbins=100, xmin=0, xmax=500 )
  monTool.defineHistogram( 'et',    path='EXPERT', type='TH1F', title='Counts', xbins=100, xmin=0, xmax=500 )
  monTool.defineHistogram( 'eta',   path='EXPERT', type='TH1F', title='Counts', xbins=50, xmin=-5, xmax=5 )
  monTool.defineHistogram( 'phi',   path='EXPERT', type='TH1F', title='Counts', xbins=50, xmin=-3, xmax=3 )
  return monTool
