# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# L2 muCombMT Fex Algorithm Configuration:
# Stefano Giagu <stefano.giagu@cern.ch>
##############################


from TrigmuComb.TrigmuCombConf import TrigmuCombMTConfig
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool,defineHistogram

class TrigmuCombMTConfig (muCombMT):
    __slots__ = []
    def __new__( cls, *args, **kwargs ):
      newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1]) ] + list(args)
      if len(args) == 3:
        if args[2] == 'OLDextr':
          newargs = ['%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2]) ] + list(args)
      return super( TrigmuCombMTConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
      super( TrigmuCombMTConfig, self ).__init__( name )

      self.UseAthenaFieldService = True
      self.MuCombStrategy = 0

      self.UseBackExtrapolatorG4 = True
      if len(args) == 3:
        if args[2] == 'OLDextr':
          self.UseBackExtrapolatorG4 = False

      self.MinPtTRK = 0.* GeV

      self.WinEtaSigma_g4 = 7.0
      self.WinPhiSigma_g4 = 7.0
      self.Chi2Weight_g4  = 2.0
      if TriggerFlags.run2Config=='2016':
        self.WinEtaSigma_g4 = 4.0
        self.WinPhiSigma_g4 = 4.0
        self.Chi2Weight_g4  = 1.0

      if globalflags.DetDescrVersion().find('CSC')!=-1:
        self.IDSCANBarrelRes  = [0.02169,0.0004186]
        self.IDSCANEndcap1Res = [0.03054,0.000325]
        self.IDSCANEndcap2Res = [0.03557,0.0005383]
        self.IDSCANEndcap3Res = [0.04755,0.0007718]
        self.IDSCANEndcap4Res = [0.06035,0.0001145]

      if 'Validation' in TriggerFlags.enableMonitoring():
        monTool = GenericMonitoringTool('MonTool')
        monTool.Histograms = [ defineHistogram('Efficiency', path='EXPERT', type='TH1F', title="Monitored Track matching efficiency from #muComb", xbins=3, xmin=-1.5, xmax=1.5) ]
        monTool.Histograms += [ defineHistogram('ErrorFlagMC', path='EXPERT', type='TH1F', title="Error Flags during event processing from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('MatchFlagMC', path='EXPERT', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('StrategyMC', path='EXPERT', type='TH1F', title="Combination Strategy from #muComb; Strategy Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('PtMS', path='EXPERT', type='TH1F', title="p_{T} MS from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('PtID', path='EXPERT', type='TH1F', title="p_{T} ID from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('PtMC', path='EXPERT', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('PtFL', path='EXPERT', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('EtaFL', path='EXPERT', type='TH1F', title="MS Eta failed matchs from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) ]
        monTool.Histograms += [ defineHistogram('PhiFL', path='EXPERT', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)", xbins=96, xmin=-3.1416, xmax=3.1416) ]
        monTool.Histograms += [ defineHistogram('EtaID', path='EXPERT', type='TH1F', title="Eta ID from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) ]
        monTool.Histograms += [ defineHistogram('ZetaID', path='EXPERT', type='TH1F', title="z0 ID from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.) ]
        monTool.Histograms += [ defineHistogram('EtaMS', path='EXPERT', type='TH1F', title="Eta MS from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) ]
        monTool.Histograms += [ defineHistogram('ZetaMS', path='EXPERT', type='TH1F', title="z0 MS from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.) ]
        monTool.Histograms += [ defineHistogram('DEta', path='EXPERT', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta", xbins=100, xmin=0.0, xmax=0.2) ]
        monTool.Histograms += [ defineHistogram('DPhi', path='EXPERT', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)", xbins=100, xmin=0.0, xmax=0.2) ]
        monTool.Histograms += [ defineHistogram('DZeta', path='EXPERT', type='TH1F', title="Zeta difference between muon and matched ID track from #muComb; DeltaZeta (cm)", xbins=100, xmin=-60, xmax=60) ]
        monTool.Histograms += [ defineHistogram('DeltaR', path='EXPERT', type='TH1F', title="DeltaR between muon and matched ID track from #muComb; DeltaR", xbins=100, xmin=0., xmax=0.5) ]
        monTool.Histograms += [ defineHistogram('PtMS, PtID', path='EXPERT', type='TH2F', title="PtID vs PtMS from #muComb", xbins=105, xmin=-105.0, xmax=105.0, ybins=105, ymin=-105.0, ymax=105.0) ]
        monTool.Histograms += [ defineHistogram('EtaMS, DEta', path='EXPERT', type='TH2F', title="Eta diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) ]
        monTool.Histograms += [ defineHistogram('EtaMS, DPhi', path='EXPERT', type='TH2F', title="Phi diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) ]
        self.MonTool = monTool

      if 'Online' in  TriggerFlags.enableMonitoring():
        monTool = GenericMonitoringTool('MonTool')
        monTool.Histograms += [ defineHistogram('ErrorFlagMC', path='EXPERT', type='TH1F', title="Error Flags during event processing from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('MatchFlagMC', path='EXPERT', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('StrategyMC', path='EXPERT', type='TH1F', title="Combination Strategy from #muComb; Strategy Code", xbins=12, xmin=-1.5, xmax=10.5) ]
        monTool.Histograms += [ defineHistogram('PtMC', path='EXPERT', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('PtFL', path='EXPERT', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) ]
        monTool.Histograms += [ defineHistogram('EtaFL', path='EXPERT', type='TH1F', title="MS Eta failed matchs from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) ]
        monTool.Histograms += [ defineHistogram('PhiFL', path='EXPERT', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)", xbins=96, xmin=-3.1416, xmax=3.1416) ]
        monTool.Histograms += [ defineHistogram('DEta', path='EXPERT', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta", xbins=100, xmin=0.0, xmax=0.2) ]
        monTool.Histograms += [ defineHistogram('DPhi', path='EXPERT', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)", xbins=100, xmin=0.0, xmax=0.2) ]
        self.MonTool = monTool
