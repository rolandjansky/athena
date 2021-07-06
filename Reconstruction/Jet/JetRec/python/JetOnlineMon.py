# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# define histograms for monitoring of HLT jets
def getMonTool_TrigJetAlgorithm(path):
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool')
  monTool.HistPath = path
  monTool.defineHistogram( 'TIME_jetreco', path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=200 )
  monTool.defineHistogram( 'TIME_jetprovider', path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=200 )
  monTool.defineHistogram( 'TIME_jetmod', path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=100 )

  monTool.defineHistogram( 'JET_n', path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=100 )
  monTool.defineHistogram( 'JET_pt',    path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=500 )
  monTool.defineHistogram( 'JET_et',    path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=500 )
  monTool.defineHistogram( 'JET_m',     path='EXPERT', type='TH1F', title='Counts',
                           xbins=100, xmin=0, xmax=200 )
  monTool.defineHistogram( 'JET_eta',   path='EXPERT', type='TH1F', title='Counts',
                           xbins=50, xmin=-5, xmax=5 )
  monTool.defineHistogram( 'JET_phi',   path='EXPERT', type='TH1F', title='Counts',
                           ybins=64, ymin=-3.2, ymax=3.2)

  monTool.defineHistogram( 'JET_eta,JET_phi',   path='EXPERT', type='TH2F', title='Counts',
                           xbins=50, xmin=-5, xmax=5,
                           ybins=64, ymin=-3, ymax=3)
  return monTool
