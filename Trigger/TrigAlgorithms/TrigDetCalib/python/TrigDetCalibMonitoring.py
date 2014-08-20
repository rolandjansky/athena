# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Definitions of the monitoring classes  

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

detectors = [ 
"PIXEL", "SCT","TRT","LAR","TTEM","TTHEC","TILE","MDT","RPC","TGC","CSC","FCALEM","FCALHAD"
]

detlabels=''
for a in detectors:
  detlabels += a+':'

nROBs      = defineHistogram('nROBs',            type='TH1I', title="No. of ROBs;#ROBs",                 xbins=300, xmin=0, xmax=300)
detVsNRobs = defineHistogram('detectors, nROBs', type='TH2I', title="No. of ROBs vs detectors;;#ROBs",   xbins=13,  xmin=20, xmax=33, ybins=300, ymin=0, ymax=300, labels=detlabels)
etaPhi     = defineHistogram('roIEta, roIPhi',   type='TH2F', title="position of processed RoI;eta;phi", xbins=100, xmin=-5, xmax=5, ybins=100, ymin=-3.2, ymax=3.2)

## For CheckForTracks:

eta_nROBs     = defineHistogram('eta_tracks_ISO, nROBs',   type='TH2F', title="eta position of processed tracks vs nROBs;eta;nROBs", xbins=50, xmin=-5, xmax=5, ybins=300, ymin=0, ymax=300)

PtTrk          =  defineHistogram('PtTrk',          type='TH1F', title="Track Pt;pT",           xbins=100, xmin=-100000., xmax=100000.) 
PtTrk_ISO      =  defineHistogram('PtTrk_ISO',      type='TH1F', title="Track Pt ISO;pT",       xbins=100, xmin=-100000., xmax=100000.) 
dR_Tracks      =  defineHistogram('dR_tracks',      type='TH1F', title="dR Tracks;DR",          xbins=60, xmin=0., xmax=6.)
eta_Tracks     =  defineHistogram('eta_tracks',     type='TH1F', title="Eta Tracks;eta",        xbins=50, xmin=-5., xmax=5.)
phi_Tracks     =  defineHistogram('phi_tracks',     type='TH1F', title="Phi Tracks;phi",        xbins=50, xmin=-5., xmax=5.)
eta_Tracks_ISO =  defineHistogram('eta_tracks_ISO', type='TH1F', title="Eta Tracks Iso;eta",    xbins=50, xmin=-5., xmax=5.)
phi_Tracks_ISO =  defineHistogram('phi_tracks_ISO', type='TH1F', title="Phi Tracks Iso;phi",    xbins=50, xmin=-5., xmax=5.)
IsoTracks      =  defineHistogram('iso_Tracks',     type='TH1I', title="N Iso Tracks;# tracks", xbins=30, xmin=0, xmax=30)

eta_pt_ISO     =  defineHistogram('eta_tracks_ISO, PtTrk_ISO',       type='TH2F', title="Tracks Iso;eta;pT",    xbins=20, xmin=-5., xmax=5., ybins=20, ymin=0., ymax=100000.) 
phi_pt_ISO     =  defineHistogram('phi_tracks_ISO, PtTrk_ISO',       type='TH2F', title="Tracks Iso;phi;pT",    xbins=20, xmin=-5., xmax=5., ybins=20, ymin=0., ymax=100000.) 
phi_eta_ISO    =  defineHistogram('phi_tracks_ISO, eta_tracks_ISO',  type='TH2F', title="Tracks Iso;phi;eta",    xbins=20, xmin=-5., xmax=5., ybins=20, ymin=-5., ymax=5.) 
n_pt_ISO       =  defineHistogram('iso_Tracks, PtTrk_ISO',       type='TH2F', title="Tracks Iso;# tracks;pT",    xbins=20, xmin=0., xmax=20, ybins=20, ymin=0., ymax=100000.) 
n_eta_ISO       =  defineHistogram('iso_Tracks, eta_tracks_ISO', type='TH2F', title="Tracks Iso;# tracks;eta",    xbins=20, xmin=0., xmax=20, ybins=20, ymin=-5., ymax=5.) 
n_phi_ISO       =  defineHistogram('iso_Tracks, phi_tracks_ISO', type='TH2F', title="Tracks Iso;# tracks;phi",    xbins=20, xmin=0., xmax=20, ybins=20, ymin=-5., ymax=5.) 

class TrigROBListWriterValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigROBListWriter_Validation"):
        super(TrigROBListWriterValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ nROBs, detVsNRobs, etaPhi ]


class TrigROBListWriterOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigROBListWriter_Online"):
        super(TrigROBListWriterOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ nROBs, detVsNRobs, etaPhi ]

class TrigROBListWriterCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigROBListWriter_Cosmic"):
        super(TrigROBListWriterCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.Histograms = [ nROBs, detVsNRobs, etaPhi ]

class TrigCheckForTracksValidationMonitoring(TrigGenericMonitoringToolConfig):
  def __init__ (self, name):
    super(TrigCheckForTracksValidationMonitoring, self).__init__(name)
    self.defineTarget("Validation")
    self.Histograms = [ nROBs, detVsNRobs, PtTrk, PtTrk_ISO, dR_Tracks, eta_Tracks, phi_Tracks, eta_Tracks_ISO, phi_Tracks_ISO, IsoTracks, eta_pt_ISO, phi_pt_ISO, phi_eta_ISO]


class TrigCheckForTracksOnlineMonitoring(TrigGenericMonitoringToolConfig):
  def __init__ (self, name):
    super(TrigCheckForTracksOnlineMonitoring, self).__init__(name)
    self.defineTarget("Online")
    self.Histograms = [ nROBs, detVsNRobs, PtTrk, PtTrk_ISO, dR_Tracks, eta_Tracks, phi_Tracks, eta_Tracks_ISO, phi_Tracks_ISO, IsoTracks, eta_pt_ISO, phi_pt_ISO, phi_eta_ISO]


class TrigCheckForTracksCosmicMonitoring(TrigGenericMonitoringToolConfig):
  def __init__ (self, name):
    super(TrigCheckForTracksCosmicMonitoring, self).__init__(name)
    self.defineTarget("Cosmic")
    self.Histograms = [ nROBs, detVsNRobs, PtTrk, PtTrk_ISO, dR_Tracks, eta_Tracks, phi_Tracks, eta_Tracks_ISO, phi_Tracks_ISO, IsoTracks, eta_pt_ISO, phi_pt_ISO, phi_eta_ISO]



def setCalibMonTools( algoObject ):
    algoName=algoObject.getName()
    print "Monitoring for "+algoName

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameOnl = algoName+"_Onl"
    onlTool = TrigCheckForTracksOnlineMonitoring(nameOnl )

    nameVal = algoName+"_Val"
    valTool = TrigCheckForTracksValidationMonitoring(nameVal)

    nameCos = algoName+"_Cos"
    cosTool = TrigCheckForTracksCosmicMonitoring(nameCos)

    algoObject.AthenaMonTools = [ time, onlTool, valTool, cosTool ]
