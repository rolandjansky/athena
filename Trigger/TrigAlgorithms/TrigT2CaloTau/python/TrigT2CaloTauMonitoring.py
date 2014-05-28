# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

# Set labels for error monitoring histogram. The order has to match with T2CaloTauErrorMon.h file! Number has to match with histo definition.
errorlabels = 'LAr_Problem:Tile_Problem:N_ROBs<requested:empty_ROD_block:Fail_Seed:Fail_PreSeed:EM_S0_E0:EM_S1_E0:EM_S2_E0:EM_S3_E0:HAD_S1_E0:HAD_S2_E0:HAD_S3_E0:Other_Errors:Good_Clusters:All_Clusters'

class T2CaloTauOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloTauOnlineMonitoring"):
        super(T2CaloTauOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms += [ defineHistogram('EMRadius',     type='TH1F', title="L2CaloTau FEX EMRadius;EMRadius; nevents",                                                   xbins=100, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EMRadius3S',   type='TH1F', title="L2CaloTau FEX EMRadius3S;EMRadius3S; nevents",                                               xbins=100, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('HadRad',       type='TH1F', title="L2CaloTau FEX HadRad;HadRad; nevents",                                                       xbins=100, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('CaloRadius',   type='TH1F', title="L2CaloTau FEX CaloRadius;CaloRadius; nevents",                                               xbins=100, xmin=-0.5, xmax=1.5) ]

        self.Histograms += [ defineHistogram('IsoFrac',      type='TH1F', title="L2CaloTau FEX IsoFrac;IsoFrac; nevents",                                                     xbins=80, xmin=-1.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('StripWidth',   type='TH1F', title="L2CaloTau FEX StripWidth;StripWidth; nevents",                                               xbins=70, xmin=-0.1, xmax=0.6) ]

        self.Histograms += [ defineHistogram('EMFraction',   type='TH1F', title="L2CaloTau FEX EM Energy Fraction;EMFraction; nevents",                                       xbins=90, xmin=-0.6, xmax=1.2) ]

        ##Medium: cone 0.2
        self.Histograms += [ defineHistogram('EMEnMedium',     type='TH1F', title="L2CaloTau FEX EMEnMedium in (dR<0.2) cone;EMEnMedium [MeV]; nevents",                            xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('HADEnMedium',    type='TH1F', title="L2CaloTau FEX HADEnMedium in (dR<0.2) cone;HADEnMedium [MeV]; nevents",                          xbins=171, xmin=-13000, xmax=500000) ]

        ##Narrow: cone 0.1
        self.Histograms += [ defineHistogram('EMEnNarrow',   type='TH1F', title="L2CaloTau FEX EMEnNarrow in (dR<0.1) cone;EMEnNarrow [MeV]; nevents",                        xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('HADEnNarrow',  type='TH1F', title="L2CaloTau FEX HADEnNarrow in (dR<0.1) cone;HADEnNarrow [MeV]; nevents",                      xbins=171, xmin=-13000, xmax=500000) ]

        self.Histograms += [ defineHistogram('EtRawMedium',     type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone;EtRawMedium [MeV]; nevents",                      xbins=171, xmin=-13000, xmax=500000) ] 
        self.Histograms += [ defineHistogram('EtRawMediumEM0',  type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 0 of EM;EtRawMediumEM0 [MeV]; nevents",   xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumEM1',  type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 1 of EM;EtRawMediumEM1 [MeV]; nevents",   xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumEM2',  type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 2 of EM;EtRawMediumEM2 [MeV]; nevents",   xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumEM3',  type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 3 of EM;EtRawMediumEM3 [MeV]; nevents",   xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumHad0', type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 0 of Had;EtRawMediumHad0 [MeV]; nevents", xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumHad1', type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 1 of Had;EtRawMediumHad1 [MeV]; nevents", xbins=171, xmin=-13000, xmax=500000) ]
        self.Histograms += [ defineHistogram('EtRawMediumHad2', type='TH1F', title="L2CaloTau FEX EtRaw in a medium (dR<0.2) cone - Layer 2 of Had;EtRawMediumHad2 [MeV]; nevents", xbins=171, xmin=-13000, xmax=500000) ]

        self.Histograms += [ defineHistogram('CoreFraction', type='TH1F', title="EtRawNarrow/EtRawMedium; Core Fraction; nevents",  xbins=70, xmin=-0.2, xmax=1.2) ]

        self.Histograms += [ defineHistogram('EtaL1, PhiL1', type='TH2F', title="L1 ROI Eta vs Phi in T2CaloTau FEX; #eta; #varphi; nevents",                                 xbins=51, xmin=-2.55, xmax=2.55,
                                                                                                                                                                              ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]

        self.Histograms += [ defineHistogram('EtaL1',        type='TH1F', title="T2CaloTau L1 Eta; Eta; nevents",                                                             xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('PhiL1',        type='TH1F', title="T2CaloTau L1 Phi; Phi; nevents",                                                             xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)]

        self.Histograms += [ defineHistogram('Eta',          type='TH1F', title="T2CaloTau FEX Eta; Eta; nevents",                                                            xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('Phi',          type='TH1F', title="T2CaloTau FEX Phi; Phi; nevents",                                                            xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)]

        self.Histograms += [ defineHistogram('dEtaL2Tau_RoI, dPhiL2Tau_RoI', type='TH2F', title="dEta vs dPhi in L2CaloTau FEX; Delta-eta; Delta-phi",                        xbins=40 , xmin=-0.2, xmax=0.2,
                                                                                                                                                                              ybins=40 , ymin=-0.2, ymax=0.2) ]

        self.Histograms += [ defineHistogram('ConversionErrors',type='TH1F',title='L2CaloTau Conversion Errors; # Errors; # Clusters',xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('AlgorithmErrors', type='TH1F',title='L2CaloTau Algorithm Errors; # Errors; # Clusters', xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram('Quality',         type='TH1I',title='L2CaloTau FEX Error bit mask; Error; # Clusters',  xbins=16,xmin=0,xmax=16,labels=errorlabels )]

########## ##############################################################
#  add validation specific histograms. 
#  If you ever remove histograms from Online - move them into Validation
#
#########################################################################
class T2CaloTauValidationMonitoring(T2CaloTauOnlineMonitoring):
    def __init__ (self, name="T2CaloTauValidationMonitoring"):
        super(T2CaloTauValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

########## ##############################################################
#  add cosmic specific histograms. 
#
#########################################################################
class T2CaloTauCosmicMonitoring(T2CaloTauOnlineMonitoring):
    def __init__ (self, name="T2CaloTauCosmicMonitoring"):
        super(T2CaloTauCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('EtaL2vsL1', type='TH1F', title="L2CaloTau FEX Eta_L2 - Eta_L1; dEta; nevents", xbins=40, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('PhiL2vsL1', type='TH1F', title="L2CaloTau FEX Phi_L2 - Phi_L1; dPhi; nevents", xbins=40, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EMFraction', type='TH1F', title="L2CaloTau FEX EM Energy Fraction;EMFraction; nevents",xbins=90, xmin=-0.6, xmax=1.2) ]
        self.Histograms += [ defineHistogram('EMEnMedium', type='TH1F', title="L2CaloTau FEX EMEnMedium in (dR<0.3) cone;EMEnMedium [MeV]; nevents", xbins=54, xmin=-12000, xmax=150000) ]
        self.Histograms += [ defineHistogram('HADEnMedium', type='TH1F', title="L2CaloTau FEX HADEnMedium in (dR<0.3) cone;HADEnMedium [MeV]; nevents", xbins=54, xmin=-12000, xmax=150000) ]
        self.Histograms += [ defineHistogram('EMEnNarrow', type='TH1F', title="L2CaloTau FEX EMEnNarrow in (dR<0.1) cone;EMEnNarrow [MeV]; nevents", xbins=54, xmin=-12000, xmax=150000) ]
        self.Histograms += [ defineHistogram('HADEnNarrow', type='TH1F', title="L2CaloTau FEX HADEnNarrow in (dR<0.1) cone;HADEnNarrow [MeV]; nevents", xbins=54, xmin=-12000, xmax=150000) ]

class TrigT2CaloTauTimeMonitoring(TrigGenericMonitoringToolConfig) :
	   def __init__ (self, name="TrigT2CaloTauTimeMonitoring"):
	      super(TrigT2CaloTauTimeMonitoring,self).__init__(name)
	      self.defineTarget("Time")
	     
	      types_list=['Total','RegSel','BSCnv','Algor','SaveEM']
	      tools_list=['ESamp2','ESamp1','EaEmEn','EHadEn']
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
	                                            type='TH1F',title=hist_title
	                                            ,xbins=50,xmin=0.0,xmax=5)]
	      self.Histograms += [ defineHistogram('Eta, TotalTime',
	                type='TH2F', title="#eta vs. time", xbins=50, xmin=-2.5,
	                xmax=2.5, ybins=50, ymin=0, ymax=5) ]
