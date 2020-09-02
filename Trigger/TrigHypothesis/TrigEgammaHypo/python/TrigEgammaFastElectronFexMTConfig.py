# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastElectronFexMT
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ParticleCaloExtensionTool= Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())

# ---------------------------------------------------------------
# class for common setups (like monitoring)
class EgammaFastElectronFexBase(TrigEgammaFastElectronFexMT):
    __slots__ = []
    def __init__(self, name):
        super(EgammaFastElectronFexBase,self).__init__(name)

        # Tracking cuts
        self.TrackPt = 1.0 * GeV
        self.TrackPtHighEt = 2.0 * GeV
        self.ClusEt = 20.0 * GeV
        self.CaloTrackdEtaNoExtrap = 0.5
        self.CaloTrackdEtaNoExtrapHighEt = 0.1
        # Calo-Tracking matching cuts
        self.CaloTrackdETA = 0.5
        self.CaloTrackdPHI = 0.5
        self.CaloTrackdEoverPLow  = 0.0
        self.CaloTrackdEoverPHigh = 999.0

        self.ParticleCaloExtensionTool = ParticleCaloExtensionTool

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
            monTool = GenericMonitoringTool('MonTool')
            monTool.defineHistogram('CaloTrackdEta', path='EXPERT', type='TH1F', title="FastElectron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
            monTool.defineHistogram('CaloTrackdPhi', path='EXPERT', type='TH1F', title="FastElectron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
            monTool.defineHistogram('CaloTrackEoverP', path='EXPERT', type='TH1F', title="FastElectron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12)
            monTool.defineHistogram('PtTrack', path='EXPERT', type='TH1F', title="FastElectron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
            monTool.defineHistogram('PtCalo', path='EXPERT', type='TH1F', title="FastElectron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
            monTool.defineHistogram('CaloEta',path='EXPERT',  type='TH1F', title="FastElectron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5)
            monTool.defineHistogram('CaloPhi', path='EXPERT', type='TH1F', title="FastElectron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2)
            monTool.defineHistogram('CaloTrackdEtaNoExtrapMon', path='EXPERT',type='TH1F', title="FastElectron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)

            self.MonTool = monTool

 
# ---------------------------------------------------------------
# TrigEgammaFastElectronFex configurations
# ---------------------------------------------------------------

class EgammaFastElectronFex_all(EgammaFastElectronFexBase):
    __slots__ = []
    def __init__(self,name="EgammaFastElectronFex_all"):
        super(EgammaFastElectronFex_all, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # Co-ordinates of calorimeter face for extrapolation 
        self.RCalBarrelFace = 1470.0*mm
        self.ZCalEndcapFace = 3800.0*mm

class EgammaFastElectronFex_1(EgammaFastElectronFex_all):
    __slots__ = []
    def __init__(self,name="EgammaFastElectronFex_1"):
        super(EgammaFastElectronFex_1, self).__init__(name)
        self.AcceptAll = True
        
class EgammaFastElectronFex_Clean(EgammaFastElectronFex_all):
    __slots__ = []
    def __init__(self,name="EgammaFastElectronFex_Clean"):
        super(EgammaFastElectronFex_Clean, self).__init__(name)
        self.AcceptAll = False
        self.CaloTrackdETA = 0.2
        self.CaloTrackdPHI = 0.3

class EgammaFastElectronFex_LowPt(EgammaFastElectronFex_all):
    __slots__ = []
    def __init__(self,name="EgammaFastElectronFex_LowPt"):
        super(EgammaFastElectronFex_1, self).__init__(name)
        self.AcceptAll = False

class EgammaFastElectronFex_HighPt(EgammaFastElectronFex_all):
    __slots__ = []
    def __init__(self,name="EgammaFastElectronFex_HighPt"):
        super(EgammaFastElectronFex_1, self).__init__(name)
        self.AcceptAll = False
        self.TrackPt = 2.0 * GeV

