from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigAFPJetAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigAFPJetAllTEHypoValidationMonitoring"):
        super(TrigAFPJetAllTEValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms =  [ defineHistogram('NJets',       type='TH1F', title="NJets No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms +=  [ defineHistogram('Yjj',       type='TH1F', title="Dijet Rapidity No cut", xbins=100, xmin=-5, xmax=5) ]
        self.Histograms +=  [ defineHistogram('Mjj',       type='TH1F', title="Dijet Mass No cut", xbins=50, xmin=0, xmax=1000) ]
        self.Histograms +=  [ defineHistogram('Etajj',       type='TH1F', title="Dijet Eta No cut", xbins=100, xmin=-5, xmax=5) ]
        self.Histograms +=  [ defineHistogram('Diff_pos_A',      type='TH1F', title="Diff position A side No cut", xbins=120, xmin=0, xmax=30) ]
        self.Histograms +=  [ defineHistogram('Diff_pos_C',      type='TH1F', title="Diff position C side No cut", xbins=120, xmin=0, xmax=30) ]
        self.Histograms +=  [ defineHistogram('N_trk_C',      type='TH1F', title="N tracks C side No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms +=  [ defineHistogram('N_trk_A',      type='TH1F', title="N tracks A side No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms +=  [ defineHistogram('Diff_y_C',      type='TH1F', title="Diff y C side No cut", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms +=  [ defineHistogram('Diff_y_A',      type='TH1F', title="Diff y A side No cut", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms +=  [ defineHistogram('Diff_x_C',      type='TH1F', title="Diff x C side No cut", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms +=  [ defineHistogram('Diff_x_A',      type='TH1F', title="Diff x A side No cut", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms +=  [ defineHistogram('x_A,param_x_A', type='TH2F', title="trk vs param x A side No cut", xbins=48, xmin=-115, xmax=-96, ybins=48, ymin=-115, ymax=-96) ]
        self.Histograms +=  [ defineHistogram('x_C,param_x_C', type='TH2F', title="trk vs param x C side No cut", xbins=48, xmin=-115, xmax=-96, ybins=48, ymin=-115, ymax=-96) ]
        self.Histograms +=  [ defineHistogram('y_A,param_y_A', type='TH2F', title="trk vs param y A side No cut", xbins=48, xmin=-12, xmax=8, ybins=48, ymin=-12, ymax=8) ]
        self.Histograms +=  [ defineHistogram('y_C,param_y_C', type='TH2F', title="trk vs param y C side No cut", xbins=48, xmin=-12, xmax=8, ybins=48, ymin=-12, ymax=8) ]
        
class TrigAFPJetAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigAFPJetAllTEOnlineMonitoring"):
        super(TrigAFPJetAllTEOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms =  [ defineHistogram('NJets',       type='TH1F', title="NJets No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
#        self.Histograms +=  [ defineHistogram('Yjj',       type='TH1F', title="Dijet Rapidity No cut", xbins=20, xmin=-5, xmax=5) ]
#        self.Histograms +=  [ defineHistogram('Mjj',       type='TH1F', title="Dijet Mass No cut", xbins=50, xmin=0, xmax=1000) ]
#        self.Histograms +=  [ defineHistogram('Etajj',       type='TH1F', title="Dijet Eta No cut", xbins=20, xmin=-5, xmax=5) ]
#        self.Histograms +=  [ defineHistogram('Diff_pos_A',      type='TH1F', title="Diff position A side No cut", xbins=60, xmin=0, xmax=60) ]
#        self.Histograms +=  [ defineHistogram('Diff_pos_C',      type='TH1F', title="Diff position C side No cut", xbins=60, xmin=0, xmax=60) ]
        self.Histograms +=  [ defineHistogram('N_trk_C',      type='TH1F', title="N tracks C side No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms +=  [ defineHistogram('N_trk_A',      type='TH1F', title="N tracks A side No cut", xbins=11, xmin=-0.5, xmax=10.5) ]
#        self.Histograms +=  [ defineHistogram('Diff_y_C',      type='TH1F', title="Diff y C side No cut", xbins=48, xmin=-5, xmax=5) ]
#        self.Histograms +=  [ defineHistogram('Diff_y_A',      type='TH1F', title="Diff y A side No cut", xbins=48, xmin=-5, xmax=5) ]
#        self.Histograms +=  [ defineHistogram('Diff_x_C',      type='TH1F', title="Diff x C side No cut", xbins=48, xmin=-10, xmax=10) ]
#        self.Histograms +=  [ defineHistogram('Diff_x_A',      type='TH1F', title="Diff x A side No cut", xbins=48, xmin=-10, xmax=10) ]
#        self.Histograms +=  [ defineHistogram('x_A,param_x_A', type='TH2F', title="trk vs param x A side No cut", xbins=48, xmin=-110, xmax=-90, ybins =48, ymin=--90, ymax=-110) ]
#        self.Histograms +=  [ defineHistogram('x_C,param_x_C', type='TH2F', title="trk vs param x C side No cut", xbins=48, xmin=-110, xmax=-90, ybins =48, ymin=-90, ymax=-110) ]
#        self.Histograms +=  [ defineHistogram('y_A,param_y_A', type='TH2F', title="trk vs param y A side No cut", xbins=48, xmin=-5, xmax=15, ybins =48, ymin=-5, ymax=15) ]
#        self.Histograms +=  [ defineHistogram('y_C,param_y_C', type='TH2F', title="trk vs param y C side No cut", xbins=48, xmin=-5, xmax=15, ybins =48, ymin=-5, ymax=15) ]
