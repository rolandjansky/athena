# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram


class TrigMissingETHypoMonitoringToolBase(GenericMonitoringTool):
    def __init__ (self, name="TrigMissingETHypoMonitoringToolBase"):
        super(TrigMissingETHypoMonitoringToolBase, self).__init__(name)

        self.Histograms = []

        self.hEx_log    = defineHistogram('Hypo_MEx_log',   type='TH1F', path='EXPERT', title="Missing E_{x};sgn(ME_{x}) log_{10}(ME_{x}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
        self.hEy_log    = defineHistogram('Hypo_MEy_log',   type='TH1F', path='EXPERT', title="Missing E_{y};sgn(ME_{y}) log_{10}(ME_{y}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
        self.hEz_log    = defineHistogram('Hypo_MEz_log',   type='TH1F', path='EXPERT', title="Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
        self.hMET_log   = defineHistogram('Hypo_MET_log',   type='TH1F', path='EXPERT', title="|Missing E_{T}|;log_{10}(ME_{T}/GeV)",           xbins=35, xmin=-1.875, xmax=5.375)
        self.hSumEt_log = defineHistogram('Hypo_SumEt_log', type='TH1F', path='EXPERT', title="Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",             xbins=35, xmin=-1.875, xmax=5.125)

        self.hEx_lin    = defineHistogram('Hypo_MEx_lin',   type='TH1F', path='EXPERT', title="Missing E_{x};ME_{x} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
        self.hEy_lin    = defineHistogram('Hypo_MEy_lin',   type='TH1F', path='EXPERT', title="Missing E_{y};ME_{y} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
        self.hEz_lin    = defineHistogram('Hypo_MEz_lin',   type='TH1F', path='EXPERT', title="Missing E_{z};ME_{z} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
        self.hMET_lin   = defineHistogram('Hypo_MET_lin',   type='TH1F', path='EXPERT', title="|Missing E_{T}|;ME_{T} (GeV)",  xbins=105, xmin=-13.5,  xmax=301.5)
        self.hSumEt_lin = defineHistogram('Hypo_SumEt_lin', type='TH1F', path='EXPERT', title="Sum |E_{T}|;SumE_{T} (GeV)",    xbins=155, xmin=-27.,   xmax=2000.)

        self.hMETPhi    = defineHistogram('Hypo_MET_phi',   type='TH1F', path='EXPERT', title="MET #phi;#phi (rad)",           xbins=32, xmin=-3.1416, xmax=3.1416)
        self.hXS        = defineHistogram('Hypo_XS',        type='TH1F', path='EXPERT', title="EF Significance; (XS/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)
        self.hXS2       = defineHistogram('Hypo_XS2',        type='TH1F', path='EXPERT', title="EF Significance 2; (XS2/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)


class TrigMissingETHypoMonitoringTool(TrigMissingETHypoMonitoringToolBase):
    def __init__ (self, name="TrigMissingETHypoMonitoringTool"):
        super(TrigMissingETHypoMonitoringTool, self).__init__(name)

        self.Histograms += [ self.hEx_log, self.hEy_log, self.hEz_log, self.hMET_log, self.hSumEt_log ]
        self.Histograms += [ self.hEx_lin, self.hEy_lin, self.hEz_lin, self.hMET_lin, self.hSumEt_lin, self.hMETPhi, self.hXS, self.hXS2 ]
