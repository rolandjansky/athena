# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

# TrigTileLookForMuAlg histograms

class TrigTileLookForMuAlgOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TileLookMuOnlineMonitoring"):
        super(TrigTileLookForMuAlgOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('NTileMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileLookForMuAlg (Online);Number of muons",
                                             xbins=10, xmin=0., xmax=10.) ] 
        self.Histograms += [ defineHistogram('EtaTileMu',
                                             type='TH1F',
                                             title="#eta from TrigTileLookForMuAlg (Online);#eta",
                                             xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTileMu',
                                             type='TH1F',
                                             title="#phi from TrigTileLookForMuAlg (Online);#phi (rad)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ] 
        self.Histograms += [ defineHistogram('ETileMu',
                                             type='TH1F',
                                             title="Energy from TrigTileLookForMuAlg (Online);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMu, PhiTileMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileLookForMuAlg (Online);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        self.Histograms += [ defineHistogram('EtaTileMu, ETileMu',
                                             type='TH2F',
                                             title="#eta vs. Energy from TrigTileLookForMuAlg (Online);#eta;E (GeV)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileMu, ETileMu',
                                             type='TH2F',
                                             title="#phi vs. Energy from TrigTileLookForMuAlg (Online);#phi (rad);E (GeV)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]
        # Loose selection

        self.Histograms += [ defineHistogram('ETileMuLoose',
                                             type='TH1F',
                                             title="Energy for Loose Selection from TrigTileLookForMuAlg (Online);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMuLoose, PhiTileMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi for Loose Selection from TrigTileLookForMuAlg (Online);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        # Monitoring errors

        self.Histograms += [ defineHistogram('ConversionErrors',
                                             type='TH1F',
                                             title='Conversion Errors bit mask (Online);Conversion Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('AlgorithmErrors',
                                             type='TH1F',
                                             title='Algorithm Errors bit mask (Online);Algorithm Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('EtaTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#eta vs. Conversion Errors (Online);#eta;Conversion Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#phi vs. Conversion Errors (Online);#phi (rad);Conversion Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('EtaTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#eta vs. Algorithm Errors (Online);#eta;Algorithm Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#phi vs. Algorithm Errors (Online);#phi (rad);Algorithm Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]

        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:
            self.Histograms += [ defineHistogram('NCellsAccess',
                                                 type='TH1F',
                                                 title="Number of the accessed cells (Online);Number of cells",
                                                 xbins=200, xmin=-0.5, xmax=599.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs (Online)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs for |#eta|<0.7 (Online)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Online);#eta_{TileMu};#eta_{RoI}",
                                                 xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ]
            self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Online);#phi_{TileMu} (rad);#phi_{RoI} (rad)",
                                                 xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]
            self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                                 type='TH2F',
                                                 title="RoIs for zero TileMu (Online);#eta_{RoI};#phi_{RoI} (rad)",
                                                 xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]



class TrigTileLookForMuAlgCosmicMonitoring(TrigTileLookForMuAlgOnlineMonitoring):
    def __init__(self, name="TileLookMuCosmicMonitoring"):
        super(TrigTileLookForMuAlgCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('NTileMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileLookForMuAlg (Cosmic);Number of muons",
                                             xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMu',
                                             type='TH1F',
                                             title="#eta from TrigTileLookForMuAlg (Cosmic);#eta",
                                             xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTileMu',
                                             type='TH1F',
                                             title="#phi from TrigTileLookForMuAlg (Cosmic);#phi (rad)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ] 
        self.Histograms += [ defineHistogram('ETileMu',
                                             type='TH1F',
                                             title="Energy from TrigTileLookForMuAlg (Cosmic);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMu, PhiTileMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileLookForMuAlg (Cosmic);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ] 
        self.Histograms += [ defineHistogram('EtaTileMu, ETileMu',
                                             type='TH2F',
                                             title="#eta vs. Energy from TrigTileLookForMuAlg (Cosmic);#eta;E (GeV)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileMu, ETileMu',
                                             type='TH2F',
                                             title="#phi vs. Energy from TrigTileLookForMuAlg (Cosmic);#phi (rad);E (GeV)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]
        # Loose selection

        self.Histograms += [ defineHistogram('ETileMuLoose',
                                             type='TH1F',
                                             title="Energy for Loose Selection from TrigTileLookForMuAlg (Cosmic);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMuLoose, PhiTileMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi for Loose Selection from TrigTileLookForMuAlg (Cosmic);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        # Monitoring errors

        self.Histograms += [ defineHistogram('ConversionErrors',
                                             type='TH1F',
                                             title='Conversion Errors bit mask (Cosmic);Conversion Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('AlgorithmErrors',
                                             type='TH1F',
                                             title='Algorithm Errors bit mask (Cosmic);Algorithm Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('EtaTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#eta vs. Conversion Errors (Cosmic);#eta;Conversion Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#phi vs. Conversion Errors (Cosmic);#phi (rad);Conversion Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('EtaTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#eta vs. Algorithm Errors (Cosmic);#eta;Algorithm Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#phi vs. Algorithm Errors (Cosmic);#phi (rad);Algorithm Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]

        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:                              
            self.Histograms += [ defineHistogram('NCellsAccess',
                                                 type='TH1F',
                                                 title="Number of the accessed cells (Cosmic);Number of cells",
                                                 xbins=200, xmin=-0.5, xmax=599.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs (Cosmic)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs for |#eta|<0.7 (Cosmic)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Cosmic);#eta_{TileMu};#eta_{RoI}",
                                                 xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ]
            self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Cosmic);#phi_{TileMu} (rad);#phi_{RoI} (rad)",
                                                 xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]                    
            self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                                 type='TH2F',
                                                 title="RoIs for zero TileMu (Cosmic);#eta_{RoI};#phi_{RoI} (rad)",
                                                 xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]



class TrigTileLookForMuAlgValidationMonitoring(TrigTileLookForMuAlgOnlineMonitoring):
    def __init__(self, name="TileLookMuValidationMonitoring"):
        super(TrigTileLookForMuAlgValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('NTileMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileLookForMuAlg (Validation);Number of muons",
                                             xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMu',
                                             type='TH1F',
                                             title="#eta from TrigTileLookForMuAlg (Validation);#eta",
                                             xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTileMu',
                                             type='TH1F',
                                             title="#phi from TrigTileLookForMuAlg (Validation);#phi (rad)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ] 
        self.Histograms += [ defineHistogram('ETileMu',
                                             type='TH1F',
                                             title="Energy from TrigTileLookForMuAlg (Validation);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMu, PhiTileMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileLookForMuAlg (Validation);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        self.Histograms += [ defineHistogram('EtaTileMu, ETileMu',
                                             type='TH2F',
                                             title="#eta vs. Energy from TrigTileLookForMuAlg (Validation);#eta;E (GeV)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileMu, ETileMu',
                                             type='TH2F',
                                             title="#phi vs. Energy from TrigTileLookForMuAlg (Validation);#phi (rad);E (GeV)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]
        # Loose selection

        self.Histograms += [ defineHistogram('ETileMuLoose',
                                             type='TH1F',
                                             title="Energy for Loose Selection from TrigTileLookForMuAlg (Validation);E (GeV)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileMuLoose, PhiTileMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi for Loose Selection from TrigTileLookForMuAlg (Validation);#eta;#phi (rad)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        # Monitoring errors

        self.Histograms += [ defineHistogram('ConversionErrors',
                                             type='TH1F',
                                             title='Conversion Errors bit mask (Validation);Conversion Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('AlgorithmErrors',
                                             type='TH1F',
                                             title='Algorithm Errors bit mask (Validation);Algorithm Errors',
                                             xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('EtaTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#eta vs. Conversion Errors (Validation);#eta;Conversion Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, ConversionErrors',
                                             type='TH2F',
                                             title='#phi vs. Conversion Errors (Validation);#phi (rad);Conversion Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('EtaTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#eta vs. Algorithm Errors (Validation);#eta;Algorithm Errors',
                                             xbins=34,xmin=-1.7,xmax=1.7,ybins=6,ymin=0,ymax=6)]
        self.Histograms += [ defineHistogram('PhiTileMu, AlgorithmErrors',
                                             type='TH2F',
                                             title='#phi vs. Algorithm Errors (Validation);#phi (rad);Algorithm Errors',
                                             xbins=64,xmin=-3.14,xmax=3.14,ybins=6,ymin=0,ymax=6)]

        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:
            self.Histograms += [ defineHistogram('NCellsAccess',
                                                 type='TH1F',
                                                 title="Number of the accessed cells (Validation);Number of cells",
                                                 xbins=200, xmin=-0.5, xmax=599.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs (Validation)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                                 type='TH1F',
                                                 title="Efficiency with RoIs for |#eta|<0.7 (Validation)",
                                                 xbins=2, xmin=-0.5, xmax=1.5) ]
            self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Validation);#eta_{TileMu};#eta_{RoI}",
                                                 xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ]
            self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                                 type='TH2F',
                                                 title="Matched TileMu with RoIs (Validation);#phi_{TileMu} (rad);#phi_{RoI} (rad)",
                                                 xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]
            self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                                 type='TH2F',
                                                 title="RoIs for zero TileMu (Validation);#eta_{RoI};#phi_{RoI} (rad)",
                                                 xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]



# TrigTileRODMuAlg histograms

class TrigTileRODMuAlgOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name='TileRODMuOnlineMonitoring'):
        super(TrigTileRODMuAlgOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('NTileRODMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileRODMuAlg (Online)",
                                             xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu',
                                             type='TH1F',
                                             title="#eta from TrigTileRODMuAlg (Online)",
                                             xbins=34, xmin=-1.7, xmax=1.7) ] 
        self.Histograms += [ defineHistogram('PhiTileRODMu',
                                             type='TH1F',
                                             title="#phi from TrigTileRODMuAlg (Online)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('ETileRODMu',
                                             type='TH1F',
                                             title="E(GeV) from TrigTileRODMuAlg (Online)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, PhiTileRODMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileRODMuAlg (Online)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#eta vs. E(GeV) from TrigTileRODMuAlg (Online)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#phi vs. E(GeV) from TrigTileRODMuAlg (Online)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('ETileRODMuLoose',
                                             type='TH1F',
                                             title="E(GeV) Loose Selection from TrigTileRODMuAlg (Online)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMuLoose, PhiTileRODMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi Loose Selection from TrigTileRODMuAlg (Online)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:
          self.Histograms += [ defineHistogram('NROBFrags',
                                               type='TH1F',
                                               title="N of ROBFragments",
                                               xbins=70, xmin=-0.5, xmax=69.5) ]
          self.Histograms += [ defineHistogram('EffEvtswRoI',
                                               type='TH1F',
                                               title="Efficiency with RoIs",
                                               xbins=2, xmin=-0.5, xmax=1.5) ]
          self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                               type='TH1F',
                                               title="Efficiency with RoIs for |#eta|<.0.7",
                                               xbins=2, xmin=-0.5, xmax=1.5) ]
          self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#eta(TileMu);#eta(RoI)",
                                               xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ]
          self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#phi(TileMu);#phi(RoI)",
                                               xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]
          self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                               type='TH2F',
                                               title="RoIs for zero TileMu;#eta(RoI);#phi(RoI)",
                                               xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]



class TrigTileRODMuAlgCosmicMonitoring(TrigTileRODMuAlgOnlineMonitoring):
    def __init__(self, name='TileRODMuCosmicMonitoring'):
        super(TrigTileRODMuAlgCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('NTileRODMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileRODMuAlg (Cosmic)",
                                             xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu',
                                             type='TH1F',
                                             title="#eta from TrigTileRODMuAlg (Cosmic)",
                                             xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTileRODMu',
                                             type='TH1F',
                                             title="#phi from TrigTileRODMuAlg (Cosmic)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('ETileRODMu',
                                             type='TH1F',
                                             title="E(GeV) from TrigTileRODMuAlg (Cosmic)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('ETileRODMuLoose',
                                             type='TH1F',
                                             title="E(GeV) Loose Selection from TrigTileRODMuAlg (Cosmic)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, PhiTileRODMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileRODMuAlg (Cosmic)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#eta vs. E(GeV) from TrigTileRODMuAlg (Cosmic)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#phi vs. E(GeV) from TrigTileRODMuAlg (Cosmic)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMuLoose, PhiTileRODMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi Loose Selection from TrigTileRODMuAlg (Cosmic)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]

        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:
          self.Histograms += [ defineHistogram('NROBFrags',
                                               type='TH1F',
                                               title="N of ROBFragments",
                                               xbins=70, xmin=-0.5, xmax=69.5) ]
          self.Histograms += [ defineHistogram('EffEvtswRoI',
                                               type='TH1F',
                                               title="Efficiency with RoIs",
                                               xbins=2, xmin=-0.5, xmax=1.5) ]
          self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                               type='TH1F',
                                               title="Efficiency with RoIs for |#eta|<0.7",
                                               xbins=2, xmin=-0.5, xmax=1.5) ]
          self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#eta(TileMu);#eta(RoI)",
                                               xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ]
          self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#phi(TileMu);#phi(RoI)",
                                               xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]
          self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                               type='TH2F',
                                               title="RoIs for zero TileMu;#eta(RoI);#phi(RoI)",
                                               xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]



class TrigTileRODMuAlgValidationMonitoring(TrigTileRODMuAlgOnlineMonitoring):
    def __init__(self, name='TileRODMuValidationMonitoring'):
        super(TrigTileRODMuAlgValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('NTileRODMu',
                                             type='TH1F',
                                             title="Number of muons from TrigTileRODMuAlg (Validation)",
                                             xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu',
                                             type='TH1F',
                                             title="#eta from TrigTileRODMuAlg (Validation)",
                                             xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTileRODMu',
                                             type='TH1F',
                                             title="#phi from TrigTileRODMuAlg (Validation)",
                                             xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('ETileRODMu',
                                             type='TH1F',
                                             title="E(GeV) from TrigTileRODMuAlg (Validation)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, PhiTileRODMu',
                                             type='TH2F',
                                             title="#eta vs. #phi from TrigTileRODMuAlg (Validation)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ] 
        self.Histograms += [ defineHistogram('ETileRODMuLoose',
                                             type='TH1F',
                                             title="E(GeV) Loose Selection from TrigTileRODMuAlg (Validation)",
                                             xbins=200, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EtaTileRODMuLoose, PhiTileRODMuLoose',
                                             type='TH2F',
                                             title="#eta vs. #phi Loose Selection from TrigTileRODMuAlg (Validation)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=64, ymin=-3.14, ymax=3.14) ]
        self.Histograms += [ defineHistogram('EtaTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#eta vs. E(GeV) from TrigTileRODMuAlg (Validation)",
                                             xbins=34, xmin=-1.7, xmax=1.7, ybins=200, ymin=0., ymax=10.) ]
        self.Histograms += [ defineHistogram('PhiTileRODMu, ETileRODMu',
                                             type='TH2F',
                                             title="#phi vs. E(GeV) from TrigTileRODMuAlg (Validation)",
                                             xbins=64, xmin=-3.14, xmax=3.14, ybins=200, ymin=0., ymax=10.) ]

        ReadRoIsFromL1 = True
        if ReadRoIsFromL1:
          self.Histograms += [ defineHistogram('NROBFrags',
                                               type='TH1F',
                                               title="N of ROBFragments",
                                               xbins=70, xmin=-0.5, xmax=69.5) ]
          self.Histograms += [ defineHistogram('EffEvtswRoI',
                                               type='TH1F',
                                               title="Efficiency with RoIs",
                                               xbins=2, xmin=-0.5, xmax=1.5) ] 
          self.Histograms += [ defineHistogram('EffEvtswRoI_LimitedEta',
                                               type='TH1F',
                                               title="Efficiency with RoIs for |#eta|<0.7",
                                               xbins=2, xmin=-0.5, xmax=1.5) ]
          self.Histograms += [ defineHistogram('EtaTileMu_wRoI, EtaRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#eta(TileMu);#eta(RoI)",
                                               xbins=34, xmin=-1.7, xmax=1.7, ybins=68, ymin=-1.7, ymax=1.7) ] 
          self.Histograms += [ defineHistogram('PhiTileMu_wRoI, PhiRoI_wTileMu',
                                               type='TH2F',
                                               title="Matched TileMu with RoIs;#phi(TileMu);#phi(RoI)",
                                               xbins=64, xmin=-3.14, xmax=3.14, ybins=128, ymin=-3.14, ymax=3.14) ]
          self.Histograms += [ defineHistogram('EtaRoI_NoTileMu, PhiRoI_NoTileMu',
                                               type='TH2F',
                                               title="RoIs for zero TileMu;#eta(RoI);#phi(RoI)",
                                               xbins=50, xmin=-2.5, xmax=2.5, ybins=64, ymin=-3.14, ymax=3.14) ]   



# TrigTileMuFex histograms

class TrigTileMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTileMuFexOnlineMonitoring"):
        super(TrigTileMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('NTrack',
                                             type='TH1F',
                                             title="N of tracks within the given RoI (Online)",
                                             xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('PtTrackAll',
                                             type='TH1F',
                                             title="P_{T} ^{Track} (GeV)",
                                             xbins=200, xmin=-50., xmax=50.) ]
        self.Histograms += [ defineHistogram('EtaTrackAll',
                                             type='TH1F',
                                             title="Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('PhiTrackAll',
                                             type='TH1F',
                                             title="Phi(Track)",
                                             xbins = 100, xmin=-3.2, xmax=3.2)] 
        self.Histograms += [ defineHistogram('EtaTrackAll, PhiTrackAll',
                                             type='TH2F',
                                             title="Phi(Track) vs. Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5., ybins = 100, ymin=-3.2, ymax=3.2)]   



class TrigTileMuFexCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTileMuFexCosmicMonitoring"):
        super(TrigTileMuFexCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('NTrack',
                                             type='TH1F',
                                             title="N of ttacks within the given RoI (Cosmic)",
                                             xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('PtTrackAll',
                                             type='TH1F',
                                             title="P_{T} ^{Track} (GeV)",
                                             xbins=200, xmin=-50., xmax=50.) ]
        self.Histograms += [ defineHistogram('EtaTrackAll',
                                             type='TH1F',
                                             title="Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('PhiTrackAll',
                                             type='TH1F',
                                             title="Phi(Track)",
                                             xbins = 100, xmin=-3.2, xmax=3.2)] 
        self.Histograms += [ defineHistogram('EtaTrackAll, PhiTrackAll',
                                             type='TH2F',
                                             title="Phi(Track) vs. Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5., ybins = 100, ymin=-3.2, ymax=3.2)]   



class TrigTileMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTileMuFexValidationMonitoring"):
        super(TrigTileMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation") 
 
        self.Histograms  = [ defineHistogram('NTrack',
                                             type='TH1F',
                                             title="N of tracks within the given RoI (Validation)",
                                             xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('PtTrackAll',
                                             type='TH1F',
                                             title="P_{T} ^{Track} (GeV)",
                                             xbins=200, xmin=-50., xmax=50.) ]
        self.Histograms += [ defineHistogram('EtaTrackAll',
                                             type='TH1F',
                                             title="Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('PhiTrackAll',
                                             type='TH1F',
                                             title="Phi(Track)",
                                             xbins = 100, xmin=-3.2, xmax=3.2)] 
        self.Histograms += [ defineHistogram('EtaTrackAll, PhiTrackAll',
                                             type='TH2F',
                                             title="Phi(Track) vs. Eta(Track)",
                                             xbins = 100, xmin=-5., xmax=5., ybins = 100, ymin=-3.2, ymax=3.2)]   

        GetTruthMuon = False
        if GetTruthMuon:
          self.Histograms += [ defineHistogram('NTruth',
                                               type='TH1F',
                                               title="N of tracks with matching Truth (Validation)",
                                               xbins=20, xmin=-0.5, xmax=19.5) ]
          self.Histograms += [ defineHistogram('DelEtaTruth',
                                               type='TH1F',
                                               title="Eta(Track)-Eta(Truth)",
                                               xbins=200, xmin=-0.1, xmax=0.1) ]
          self.Histograms += [ defineHistogram('DelPhiTruth',
                                               type='TH1F',
                                               title="Phi(Track)-Phi(Truth)",
                                               xbins=200, xmin=-0.1, xmax=0.1) ]
          self.Histograms += [ defineHistogram('PtTruth, PtTrack',
                                               type='TH2F',
                                               title="P_{T} ^{Track} (GeV) vs. P_{T} ^{True} (GeV)",
                                               xbins=200, xmin=-20., xmax=20., ybins=200, ymin=-20., ymax=20.) ]
          self.Histograms += [ defineHistogram('DelPtTruth',
                                               type='TH1F',
                                               title="Pt(Track)-Pt(Truth)",
                                               xbins=200, xmin=-10., xmax=10.) ]
