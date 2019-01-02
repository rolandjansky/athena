# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def HLTResultMTMakerCfg():
   from TrigOutputHandlingConf import HLTResultMTMaker
   from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

   m = HLTResultMTMaker()
   m.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTResultMTMaker')
   m.MonTool.Histograms = [ defineHistogram( 'TIME_build', path='EXPERT', type='TH1F', title='Time of result construction in;[micro seccond]',
                                             xbins=100, xmin=0, xmax=1000 ),
                            defineHistogram( 'nstreams', path='EXPERT', type='TH1F', title='number of streams',
                                             xbins=60, xmin=0, xmax=60 ),
                            defineHistogram( 'nfrags', path='EXPERT', type='TH1F', title='number of HLT results',
                                             xbins=10, xmin=0, xmax=10 ),
                            defineHistogram( 'sizeMain', path='EXPERT', type='TH1F', title='Main (physics) HLT Result size;4B words',
                                             xbins=100, xmin=-1, xmax=999 ) ] # 1000 k span
   return m
