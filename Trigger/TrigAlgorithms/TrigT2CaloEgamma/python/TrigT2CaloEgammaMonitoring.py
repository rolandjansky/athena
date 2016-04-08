# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigT2CaloEgammaValMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaValMonitoring"):
      super(TrigT2CaloEgammaValMonitoring,self).__init__(name)
      self.defineTarget("Validation")
      
      self.Histograms = [defineHistogram('Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
      self.Histograms += [defineHistogram('Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
      self.Histograms += [defineHistogram('Eta,Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                
      self.Histograms += [defineHistogram('Et',type='TH1F',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4) ]
      self.Histograms += [defineHistogram('Had1Et',type='TH1F',
                                          title="Had E_{T} of Clusters (first had layer); Had E_{T} [MeV]; number of RoIs",
                                          xbins=60,xmin=0,xmax=5e3) ]
      self.Histograms += [defineHistogram('Rcore',type='TH1F',
                                          title="3x7/7x7 energy of Clusters; Rcore; number of RoIs",
                                          xbins=60,xmin=0,xmax=1) ]
      self.Histograms += [defineHistogram('Eratio',type='TH1F',
                                          title="Eratio (s1) of Clusters; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [defineHistogram('frac73',type='TH1F',
                                          title="Energy in a 7 strips (around hottest strip) minus energy in 3 strips divided by energy in 3 strips; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [ defineHistogram ('ConversionErrors'
                                            ,type='TH1F',title='Conversion Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('AlgorithmErrors',
                                            type='TH1F',title='Algorithm Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('Eta, ConversionErrors' 	 
                ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' 	 
                ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)] 	 
      self.Histograms += [ defineHistogram ('Phi, ConversionErrors' 	 
               ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]
      self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Eta;#eta;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)]
      self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Phi;#phi;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]

class TrigT2CaloEgammaTimeMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaTimeMonitoring"):
      super(TrigT2CaloEgammaTimeMonitoring,self).__init__(name)
      self.defineTarget("Time")
      
      types_list=['Total','RegSel','BSCnv','Algor','SaveEM']
      tools_list=['ESamp2','ESamp1','EaEmEn','EHadEn']
      for tool in tools_list:
         for type in types_list:
             varname=tool+type+";"+tool+type+"_Time"
             hist_title=tool+type
             thismax=1.0
             if ( (type.find("RegSel")>-1) or (type.find("SaveEM")>-1) ):
                thismax=0.2
             if ( (type.find("Algor")>-1) ):
                thismax=0.5
             self.Histograms+= [defineHistogram (varname,
                                            type='TH1F',title=hist_title
                                            ,xbins=40,xmin=0.0,xmax=thismax)]
      self.Histograms+= [defineHistogram ('TotalTime',
                                            type='TH1F',title='TotalTime'
                                            ,xbins=50,xmin=0.0,xmax=5)]

class TrigT2CaloEgammaAllTimeMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaAllTimeMonitoring"):
      super(TrigT2CaloEgammaAllTimeMonitoring,self).__init__(name)
      self.defineTarget("Time")

      types_list=['Total','RegSel','BSCnv','Algor','SaveEM']
      tools_list=['EmaAll']
      for tool in tools_list:
         for type in types_list:
             hist_title=tool+type
             thismax=1.0
             if ( (type.find("RegSel")>-1) or (type.find("SaveEM")>-1) ):
                thismax=0.2
             if ( (type.find("Algor")>-1) ):
                thismax=0.5
             self.Histograms+= [defineHistogram (hist_title,
                                            type='TH1F',title=hist_title
                                            ,xbins=40,xmin=0.0,xmax=thismax)]
      self.Histograms+= [defineHistogram ('TotalTime',
                                            type='TH1F',title='TotalTime'
                                            ,xbins=50,xmin=0.0,xmax=5)]

class TrigT2CaloEgammaRingerTimeMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaRingerTimeMonitoring"):
      super(TrigT2CaloEgammaRingerTimeMonitoring,self).__init__(name)
      self.defineTarget("Time")

      self.Histograms+= [defineHistogram ('Time_RingerFexGetRings',
                                            type='TH1F',title='Time_RingerFexGetRings'
                                            ,xbins=50,xmin=0.0,xmax=5)]
      self.Histograms+= [defineHistogram ('Time_RingerFexRegSel',
                                            type='TH1F',title='Time_RingerFexRegSel'
                                            ,xbins=50,xmin=0.0,xmax=5)]
      #self.Histograms+= [defineHistogram ('Time_RingerFexSaveRoI',
      #                                      type='TH1F',title='RingerFexSaveRoI'
      #                                      ,xbins=50,xmin=0.0,xmax=5)]
      self.Histograms+= [defineHistogram ('Time_RingerFexExecute',
                                            type='TH1F',title='Time_RingerFexExecute'
                                            ,xbins=50,xmin=0.0,xmax=5)]

class TrigT2CaloEgammaCosmicMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaCosmicMonitoring"):
      super(TrigT2CaloEgammaCosmicMonitoring,self).__init__(name)
      self.defineTarget("Cosmic")

      self.Histograms = [defineHistogram('Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
      self.Histograms += [defineHistogram('Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
      self.Histograms += [defineHistogram('Eta,Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                
      self.Histograms += [defineHistogram('Et',type='TH1F',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4) ]
      self.Histograms += [defineHistogram('Had1Et',type='TH1F',
                                          title="Had E_{T} of Clusters (first had layer); Had E_{T} [MeV]; number of RoIs",
                                          xbins=60,xmin=0,xmax=5e3) ]
      self.Histograms += [defineHistogram('Rcore',type='TH1F',
                                          title="3x7/7x7 energy of Clusters; Rcore; number of RoIs",
                                          xbins=60,xmin=0,xmax=1) ]
      self.Histograms += [defineHistogram('Eratio',type='TH1F',
                                          title="Eratio (s1) of Clusters; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [defineHistogram('frac73',type='TH1F',
                                          title="Energy in a 7 strips (around hottest strip) minus energy in 3 strips divided by energy in 3 strips; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [ defineHistogram ('ConversionErrors'
                                            ,type='TH1F',title='Conversion Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('AlgorithmErrors',
                                            type='TH1F',title='Algorithm Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('Eta, ConversionErrors' 	 
                ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' 	 
                ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)] 	 
      self.Histograms += [ defineHistogram ('Phi, ConversionErrors' 	 
               ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]
      self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Eta;#eta;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)]
      self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Phi;#phi;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]


class TrigT2CaloEgammaOnMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaOnMonitoring",doTimeHist=False):
      super(TrigT2CaloEgammaOnMonitoring,self).__init__(name)
      self.defineTarget("Online")

      self.Histograms = [defineHistogram('Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
      self.Histograms += [defineHistogram('Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
      self.Histograms += [defineHistogram('Eta,Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                
      self.Histograms += [defineHistogram('Et',type='TH1F',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4) ]
      self.Histograms += [defineHistogram('Had1Et',type='TH1F',
                                          title="Had E_{T} of Clusters (first had layer); Had E_{T} [MeV]; number of RoIs",
                                          xbins=60,xmin=0,xmax=5e3) ]
      self.Histograms += [defineHistogram('Rcore',type='TH1F',
                                          title="3x7/7x7 energy of Clusters; Rcore; number of RoIs",
                                          xbins=60,xmin=0,xmax=1) ]
      self.Histograms += [defineHistogram('Eratio',type='TH1F',
                                          title="Eratio (s1) of Clusters; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [defineHistogram('frac73',type='TH1F',
                                          title="Energy in a 7 strips (around hottest strip) minus energy in 3 strips divided by energy in 3 strips; Eratio; number of RoIs",
                                          xbins=60,xmin=0,xmax=1.5) ]
      self.Histograms += [ defineHistogram ('ConversionErrors'
                                            ,type='TH1F',title='Conversion Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('AlgorithmErrors',
                                            type='TH1F',title='Algorithm Errors bit mask'
                                            ,xbins=10,xmin=0,xmax=10)]
      self.Histograms += [ defineHistogram ('Eta, ConversionErrors' 	 
                ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' 	 
                ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)] 	 
      self.Histograms += [ defineHistogram ('Phi, ConversionErrors' 	 
               ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]
      self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Eta;#eta;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=50,xmin=-2.5,xmax=2.5)]
      self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' 	 
               ,type='TH2F',title='AlgorithmErrors vs Phi;#phi;Algorithm Errors'
               ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.2,xmax=3.2)]
      if doTimeHist:
          self.Histograms += [ defineHistogram('Eta, TotalTime',
                type='TH2F', title="#eta vs. time", xbins=50, xmin=-2.5,
                xmax=2.5, ybins=50, ymin=0, ymax=5) ]

    
class TrigT2CaloEgammaSwValMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaSwValMonitoring"):
      super(TrigT2CaloEgammaSwValMonitoring,self).__init__(name)
      self.defineTarget("Validation")
      
      self.Histograms = [defineHistogram('Eta', type='TH1F', title="#eta of Clusters; #eta; nRoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
      self.Histograms += [defineHistogram('Phi',type='TH1F', title="#phi of Clusters; #phi; nRoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
      self.Histograms += [defineHistogram('Eta,Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]
      
      self.Histograms += [defineHistogram('nRoIs',type='TH1F',
                                          title="Number of RoIs; nRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedRoIs',type='TH1F',
                                          title="Number of Cached RoIs; nCachedRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedClusters',type='TH1F',
                                          title="Number of Cached Clusters; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nWindows',type='TH1F',
                                          title="Number of Windows Above Threshold; nWindows; nRoIs",
                                          xbins=100,xmin=0,xmax=100) ]
      self.Histograms += [defineHistogram('nSeeds',type='TH1F',
                                          title="Number of Seeds Found; nSeeds; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nClusters',type='TH1F',
                                          title="Number of Clusters Found; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nOverlaps',type='TH1F',
                                          title="Number of Overlaps Removed; nOverlaps; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]

      self.Histograms += [defineHistogram('L1dR', type='TH1F', title="dR between clusters and L1RoI; dR; nClusters", xbins=100,xmin=0,xmax=1) ]
      self.Histograms += [defineHistogram('L2dR', type='TH1F', title="dR between all clusters found; dR; nClusters", xbins=100,xmin=0,xmax=10) ]


class TrigT2CaloEgammaSwOnMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaSwOnMonitoring"):
      super(TrigT2CaloEgammaSwOnMonitoring,self).__init__(name)
      self.defineTarget("Online")
      
      self.Histograms += [defineHistogram('nRoIs',type='TH1F',
                                          title="Number of RoIs; nRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedRoIs',type='TH1F',
                                          title="Number of Cached RoIs; nCachedRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedClusters',type='TH1F',
                                          title="Number of Cached Clusters; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nWindows',type='TH1F',
                                          title="Number of Windows Above Threshold; nWindows; nRoIs",
                                          xbins=100,xmin=0,xmax=100) ]
      self.Histograms += [defineHistogram('nSeeds',type='TH1F',
                                          title="Number of Seeds Found; nSeeds; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nClusters',type='TH1F',
                                          title="Number of Clusters Found; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nOverlaps',type='TH1F',
                                          title="Number of Overlaps Removed; nOverlaps; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]


class TrigT2CaloEgammaSwCosMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigT2CaloEgammaSwCosMonitoring"):
      super(TrigT2CaloEgammaSwCosMonitoring,self).__init__(name)
      self.defineTarget("Cosmic")
      
      self.Histograms += [defineHistogram('nRoIs',type='TH1F',
                                          title="Number of RoIs; nRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedRoIs',type='TH1F',
                                          title="Number of Cached RoIs; nCachedRoIs; nEvents",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nCachedClusters',type='TH1F',
                                          title="Number of Cached Clusters; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nWindows',type='TH1F',
                                          title="Number of Windows Above Threshold; nWindows; nRoIs",
                                          xbins=100,xmin=0,xmax=100) ]
      self.Histograms += [defineHistogram('nSeeds',type='TH1F',
                                          title="Number of Seeds Found; nSeeds; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nClusters',type='TH1F',
                                          title="Number of Clusters Found; nClusters; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
      self.Histograms += [defineHistogram('nOverlaps',type='TH1F',
                                          title="Number of Overlaps Removed; nOverlaps; nRoIs",
                                          xbins=10,xmin=0,xmax=10) ]
