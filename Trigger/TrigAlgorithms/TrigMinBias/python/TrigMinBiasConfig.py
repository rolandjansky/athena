# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMinBias.TrigMinBiasConf import TrigTrackCounter, TrigTrackCounterHypo, \
    TrigVertexCounter, TrigVertexCounterHypo

# Monitoring
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

# Properties for histogram dimensions
from TrigMinBiasProperties import trigMinBiasProperties

fexes = {}
hypos = {}

#----------------------------------------------------------------------------------------

class MbTrkFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbTrkFexMonitoring"):
        super(MbTrkFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])

        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigTrackCounts object.
        
        self.Histograms += [ defineHistogram('TrkZ0',title='Reconstructed Track Z0;Z0   (mm);Tracks',
                                             type = 'TH1F',
                                             xbins = trigMinBiasProperties.trkZ0Bins(),
                                             xmin = trigMinBiasProperties.trkZ0Min(),
                                             xmax = trigMinBiasProperties.trkZ0Max(),
                                             opt="kVecUO") ]

        self.Histograms += [ defineHistogram('TrkPt',title='Reconstructed Track pT;pT   (GeV);Tracks',
                                             type = 'TH1F',
                                             xbins = trigMinBiasProperties.trkPtBins(),
                                             xmin = trigMinBiasProperties.trkPtMin(),
                                             xmax = trigMinBiasProperties.trkPtMax(),
                                             opt="kVecUO") ]

        self.Histograms += [ defineHistogram('TrkEta',title='Reconstructed Track Eta;eta   ;Tracks',
                                             type = 'TH1F',
                                             xbins = trigMinBiasProperties.trkEtaBins(),
                                             xmin = trigMinBiasProperties.trkEtaMin(),
                                             xmax = trigMinBiasProperties.trkEtaMax(),
                                             opt="kVecUO") ]

        self.Histograms += [ defineHistogram('TrkPhi',title='Reconstructed Track Phi;phi   ;Tracks',
                                             type = 'TH1F',
                                             xbins = trigMinBiasProperties.trkPhiBins(),
                                             xmin = trigMinBiasProperties.trkPhiMin(),
                                             xmax = trigMinBiasProperties.trkPhiMax(),
                                             opt="kVecUO") ]

        #--------------------------------------------------------------------------------------------
        # Other monitoring
        #
        self.Histograms += [ defineHistogram('NTrks',
                                             type='TH1I',
                                             title="Number of Tracks;NTrks;Events",
                                             xbins = 500, xmin=-0.5, xmax=499.5)]

class MbTrkHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbTrkHypoMonitoring"):
        super(MbTrkHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"])

        self.Histograms += [ defineHistogram('NTrksHypo',
                                             type='TH1I',
                                             title="Number of Tracks in Hypo;NTrks;Events",
                                             xbins = 500, xmin=-0.5, xmax=499.5)]

        self.Histograms += [ defineHistogram('NTrksSelected',
                                             type='TH1I',
                                             title="Number of selected tracks;NTrks;Events",
                                             xbins = 500, xmin=-0.5, xmax=499.5)]

class MbVxFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbVxFexMonitoring"):
        super(MbVxFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"])

        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigVertexCounts object.
        
        self.Histograms += [ defineHistogram('vtxNtrks',title='Number of tracks at vertex;Track multiplicity;Vertices/Ntrk',
                                             type = 'TH1F',
                                             xbins = 201,
                                             xmin = -0.5,
                                             xmax = 200.5) ]

        self.Histograms += [ defineHistogram('vtxNvtx',title='Number of vertices;Vertex multiplicity;Events/Vertex',
                                             type = 'TH1F',
                                             xbins = 52,
                                             xmin = -1.5,
                                             xmax = 50.5) ]
        
        self.Histograms += [ defineHistogram('vtxTrkPtSqSum',title='Sum p_{T}^{2} of tracks at vertex;p_{T}^{2}   (GeV^{2});Vertices/GeV^{2}',
                                             type = 'TH1F',
                                             xbins = 100,
                                             xmin = 0.,
                                             xmax = 1000.) ]

class MbVxHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbVxHypoMonitoring"):
        super(MbVxHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"])

        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigVertexCounts object.
        
        self.Histograms += [ defineHistogram('vtxNtrks',title='Number of tracks at vertex;Track multiplicity;Vertices/Ntrk',
                                             type = 'TH1F',
                                             xbins = 201,
                                             xmin = -0.5,
                                             xmax = 200.5) ]

        self.Histograms += [ defineHistogram('vtxNvtx',title='Number of vertices;Vertex multiplicity;Events/Vertex',
                                             type = 'TH1F',
                                             xbins = 52,
                                             xmin = -1.5,
                                             xmax = 50.5) ]

        self.Histograms += [ defineHistogram('vtxTrkPtSqSum',title='Sum p_{T}^{2} of tracks at vertex;p_{T}^{2}   (GeV^{2});Vertices/GeV^{2}',
                                             type = 'TH1F',
                                             xbins = 100,
                                             xmin = 0.,
                                             xmax = 1000.) ]

        self.Histograms += [ defineHistogram('vtxNtrksAccepted',title='Number of tracks at vertex;Track multiplicity;Vertices/Ntrk',
                                             type = 'TH1F',
                                             xbins = 201,
                                             xmin = -0.5,
                                             xmax = 200.5) ]
        
        self.Histograms += [ defineHistogram('vtxNvtxAccepted',title='Number of vertices;Vertex multiplicity;Events/Vertex',
                                             type = 'TH1F',
                                             xbins = 52,
                                             xmin = -1.5,
                                             xmax = 50.5) ]

        self.Histograms += [ defineHistogram('vtxTrkPtSqSumAccepted',title='Sum p_{T}^{2} of tracks at vertex;p_{T}^{2}   (GeV^{2});Vertices/GeV^{2}',
                                             type = 'TH1F',
                                             xbins = 100,
                                             xmin = 0.,
                                             xmax = 1000.) ]


#----------------------------------------------------------------------------------------
# algos configurables
class MbTrkFex ( TrigTrackCounter ) :
    __slots__ = []
    def __init__ (self, name="MbTrkFex"):
        super(MbTrkFex, self).__init__(name)
        time = TrigTimeHistToolConfig("MbTrkFexTimers")
        time.TimerHistLimits = [0, 10]
        
        # Set the histogram dimensions from TrigT2MinBiasProperties.
        self.Z0Bins = trigMinBiasProperties.trkZ0Bins()
        self.Z0Min = trigMinBiasProperties.trkZ0Min()
        self.Z0Max = trigMinBiasProperties.trkZ0Max()
        self.PtBins = trigMinBiasProperties.trkPtBins()
        self.PtMin = trigMinBiasProperties.trkPtMin()
        self.PtMax = trigMinBiasProperties.trkPtMax()
        self.EtaBins = trigMinBiasProperties.trkEtaBins()
        self.EtaMin = trigMinBiasProperties.trkEtaMin()
        self.EtaMax = trigMinBiasProperties.trkEtaMax()
        self.PhiBins = trigMinBiasProperties.trkPhiBins()
        self.PhiMin = trigMinBiasProperties.trkPhiMin()
        self.PhiMax = trigMinBiasProperties.trkPhiMax()

        self.AthenaMonTools += [ MbTrkFexMonitoring(), time]


MbTrkFex_1 = MbTrkFex # remove once cosmic slice migrated

EFMbTrkFex = MbTrkFex("EFMbTrkFex")
EFMbTrkFex.InputTrackContainerName='AmbigSolv'

#.. and LowPt versions
EFMbTrkFex_LowPt = MbTrkFex("EFMbTrkFex_LowPt")
EFMbTrkFex_LowPt.InputTrackContainerName='CombinedLowPtTracksEFID'

fexes["EFMbTrkFex"] = EFMbTrkFex
fexes["EFMbTrkFex_LowPt"] = EFMbTrkFex_LowPt

#------------------------------------------------------
# Hypo
class MbTrkHypo ( TrigTrackCounterHypo ):    
    __slots__ = []
    def __init__ (self, name="MbTrkHypo"):
        super(MbTrkHypo, self).__init__(name)
        time = TrigTimeHistToolConfig("MbTrkHypoTimers")
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ MbTrkHypoMonitoring(), time]

MbTrkHypo_1 = MbTrkHypo  # remove once cosmic clice migrated

# create HYPOs
EFMbTrkHypo = MbTrkHypo("EFMbTrkHypo")
EFMbTrkHypo.AcceptAll_EF = False
EFMbTrkHypo.CutMinAndMax = False
EFMbTrkHypo.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypo.Min_pt = 0.2
EFMbTrkHypo.Required_ntrks = 1
hypos["EFMbTrkHypo"] = EFMbTrkHypo

# Loose exclusive requirement
EFMbTrkHypoExclusiveLoose = MbTrkHypo("EFMbTrkHypoExclusiveLoose")
EFMbTrkHypoExclusiveLoose.AcceptAll_EF = False
EFMbTrkHypoExclusiveLoose.CutMinAndMax = True
EFMbTrkHypoExclusiveLoose.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoExclusiveLoose.Min_pt = 0.2
EFMbTrkHypoExclusiveLoose.Required_ntrks = 1
EFMbTrkHypoExclusiveLoose.MaxRequired_ntrks = 15
hypos["EFMbTrkHypoExclusiveLoose"] = EFMbTrkHypoExclusiveLoose

# Tigheter exclusive requirement
EFMbTrkHypoExclusiveTight = MbTrkHypo("EFMbTrkHypoExclusiveTight")
EFMbTrkHypoExclusiveTight.AcceptAll_EF = False
EFMbTrkHypoExclusiveTight.CutMinAndMax = True
EFMbTrkHypoExclusiveTight.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoExclusiveTight.Min_pt = 0.2
EFMbTrkHypoExclusiveTight.Required_ntrks = 2
EFMbTrkHypoExclusiveTight.MaxRequired_ntrks = 4
hypos["EFMbTrkHypoExclusiveTight"] = EFMbTrkHypoExclusiveTight

#..and PT versions
EFMbTrkHypo_PT = MbTrkHypo("EFMbTrkHypo_PT")
EFMbTrkHypo_PT.AcceptAll_EF = True
hypos["EFMbTrkHypo_PT"] = EFMbTrkHypo_PT

#..and the high multiplicity version
EFMbTrkHypoMh = MbTrkHypo("EFMbTrkHypoMh")
EFMbTrkHypoMh.AcceptAll_EF = False
EFMbTrkHypoMh.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh.Min_pt = 0.2
EFMbTrkHypoMh.Required_ntrks = 85
hypos["EFMbTrkHypoMh"] = EFMbTrkHypoMh


#..and the high multiplicity version for particle correlation trigger
EFMbTrkHypoMhPc = MbTrkHypo("EFMbTrkHypoMhPc")
EFMbTrkHypoMhPc.AcceptAll_EF = False
EFMbTrkHypoMhPc.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMhPc.Min_pt = 0.4
EFMbTrkHypoMhPc.Required_ntrks = 120
hypos["EFMbTrkHypoMhPc"] = EFMbTrkHypoMhPc

## hypo for HMT chains (HI2013)
EFMbTrkHypoMh_hip_140 = MbTrkHypo("EFMbTrkHypoMh_hip_140")
EFMbTrkHypoMh_hip_140.AcceptAll_EF = False
EFMbTrkHypoMh_hip_140.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_140.Min_pt = 0.4
EFMbTrkHypoMh_hip_140.Required_ntrks = 140
hypos["EFMbTrkHypoMh_hip_140"] = EFMbTrkHypoMh_hip_140


EFMbTrkHypoMh_hip_160 = MbTrkHypo("EFMbTrkHypoMh_hip_160")
EFMbTrkHypoMh_hip_160.AcceptAll_EF = False
EFMbTrkHypoMh_hip_160.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_160.Min_pt = 0.4
EFMbTrkHypoMh_hip_160.Required_ntrks = 160
hypos["EFMbTrkHypoMh_hip_160"] = EFMbTrkHypoMh_hip_160

EFMbTrkHypoMh_hip_170 = MbTrkHypo("EFMbTrkHypoMh_hip_170")
EFMbTrkHypoMh_hip_170.AcceptAll_EF = False
EFMbTrkHypoMh_hip_170.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_170.Min_pt = 0.4
EFMbTrkHypoMh_hip_170.Required_ntrks = 170
hypos["EFMbTrkHypoMh_hip_170"] = EFMbTrkHypoMh_hip_170

EFMbTrkHypoMh_hip_180 = MbTrkHypo("EFMbTrkHypoMh_hip_180")
EFMbTrkHypoMh_hip_180.AcceptAll_EF = False
EFMbTrkHypoMh_hip_180.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_180.Min_pt = 0.4
EFMbTrkHypoMh_hip_180.Required_ntrks = 180
hypos["EFMbTrkHypoMh_hip_180"] = EFMbTrkHypoMh_hip_180

EFMbTrkHypoMh_hip_190 = MbTrkHypo("EFMbTrkHypoMh_hip_190")
EFMbTrkHypoMh_hip_190.AcceptAll_EF = False
EFMbTrkHypoMh_hip_190.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_190.Min_pt = 0.4
EFMbTrkHypoMh_hip_190.Required_ntrks = 190
hypos["EFMbTrkHypoMh_hip_190"] = EFMbTrkHypoMh_hip_190

EFMbTrkHypoMh_hip_200 = MbTrkHypo("EFMbTrkHypoMh_hip_200")
EFMbTrkHypoMh_hip_200.AcceptAll_EF = False
EFMbTrkHypoMh_hip_200.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_200.Min_pt = 0.4
EFMbTrkHypoMh_hip_200.Required_ntrks = 200
hypos["EFMbTrkHypoMh_hip_200"] = EFMbTrkHypoMh_hip_200

EFMbTrkHypoMh_hip_210 = MbTrkHypo("EFMbTrkHypoMh_hip_210")
EFMbTrkHypoMh_hip_210.AcceptAll_EF = False
EFMbTrkHypoMh_hip_210.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_210.Min_pt = 0.4
EFMbTrkHypoMh_hip_210.Required_ntrks = 210
hypos["EFMbTrkHypoMh_hip_210"] = EFMbTrkHypoMh_hip_210

EFMbTrkHypoMh_hip_220 = MbTrkHypo("EFMbTrkHypoMh_hip_220")
EFMbTrkHypoMh_hip_220.AcceptAll_EF = False
EFMbTrkHypoMh_hip_220.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_220.Min_pt = 0.4
EFMbTrkHypoMh_hip_220.Required_ntrks = 220
hypos["EFMbTrkHypoMh_hip_220"] = EFMbTrkHypoMh_hip_220

EFMbTrkHypoMh_hip_225 = MbTrkHypo("EFMbTrkHypoMh_hip_225")
EFMbTrkHypoMh_hip_225.AcceptAll_EF = False
EFMbTrkHypoMh_hip_225.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_225.Min_pt = 0.4
EFMbTrkHypoMh_hip_225.Required_ntrks = 225
hypos["EFMbTrkHypoMh_hip_225"] = EFMbTrkHypoMh_hip_225

EFMbTrkHypoMh_hip_230 = MbTrkHypo("EFMbTrkHypoMh_hip_230")
EFMbTrkHypoMh_hip_230.AcceptAll_EF = False
EFMbTrkHypoMh_hip_230.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_230.Min_pt = 0.4
EFMbTrkHypoMh_hip_230.Required_ntrks = 230
hypos["EFMbTrkHypoMh_hip_230"] = EFMbTrkHypoMh_hip_230

EFMbTrkHypoMh_hip_240 = MbTrkHypo("EFMbTrkHypoMh_hip_240")
EFMbTrkHypoMh_hip_240.AcceptAll_EF = False
EFMbTrkHypoMh_hip_240.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_240.Min_pt = 0.4
EFMbTrkHypoMh_hip_240.Required_ntrks = 240
hypos["EFMbTrkHypoMh_hip_240"] = EFMbTrkHypoMh_hip_240

EFMbTrkHypoMh_hip_245 = MbTrkHypo("EFMbTrkHypoMh_hip_245")
EFMbTrkHypoMh_hip_245.AcceptAll_EF = False
EFMbTrkHypoMh_hip_245.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_245.Min_pt = 0.4
EFMbTrkHypoMh_hip_245.Required_ntrks = 245
hypos["EFMbTrkHypoMh_hip_245"] = EFMbTrkHypoMh_hip_245

EFMbTrkHypoMh_hip_250 = MbTrkHypo("EFMbTrkHypoMh_hip_250")
EFMbTrkHypoMh_hip_250.AcceptAll_EF = False
EFMbTrkHypoMh_hip_250.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_250.Min_pt = 0.4
EFMbTrkHypoMh_hip_250.Required_ntrks = 250
hypos["EFMbTrkHypoMh_hip_250"] = EFMbTrkHypoMh_hip_250

EFMbTrkHypoMh_hip_260 = MbTrkHypo("EFMbTrkHypoMh_hip_260")
EFMbTrkHypoMh_hip_260.AcceptAll_EF = False
EFMbTrkHypoMh_hip_260.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_260.Min_pt = 0.4
EFMbTrkHypoMh_hip_260.Required_ntrks = 260
hypos["EFMbTrkHypoMh_hip_260"] = EFMbTrkHypoMh_hip_260

EFMbTrkHypoMh_hip_270 = MbTrkHypo("EFMbTrkHypoMh_hip_270")
EFMbTrkHypoMh_hip_270.AcceptAll_EF = False
EFMbTrkHypoMh_hip_270.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_270.Min_pt = 0.4
EFMbTrkHypoMh_hip_270.Required_ntrks = 270
hypos["EFMbTrkHypoMh_hip_270"] = EFMbTrkHypoMh_hip_270

EFMbTrkHypoMh_hip_290 = MbTrkHypo("EFMbTrkHypoMh_hip_290")
EFMbTrkHypoMh_hip_290.AcceptAll_EF = False
EFMbTrkHypoMh_hip_290.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_290.Min_pt = 0.4
EFMbTrkHypoMh_hip_290.Required_ntrks = 290
hypos["EFMbTrkHypoMh_hip_290"] = EFMbTrkHypoMh_hip_290

EFMbTrkHypoMh_hip_300 = MbTrkHypo("EFMbTrkHypoMh_hip_300")
EFMbTrkHypoMh_hip_300.AcceptAll_EF = False
EFMbTrkHypoMh_hip_300.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_300.Min_pt = 0.4
EFMbTrkHypoMh_hip_300.Required_ntrks = 300
hypos["EFMbTrkHypoMh_hip_300"] = EFMbTrkHypoMh_hip_300

EFMbTrkHypoMh_hip_310 = MbTrkHypo("EFMbTrkHypoMh_hip_310")
EFMbTrkHypoMh_hip_310.AcceptAll_EF = False
EFMbTrkHypoMh_hip_310.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_310.Min_pt = 0.4
EFMbTrkHypoMh_hip_310.Required_ntrks = 310
hypos["EFMbTrkHypoMh_hip_310"] = EFMbTrkHypoMh_hip_310

EFMbTrkHypoMh_hip_320 = MbTrkHypo("EFMbTrkHypoMh_hip_320")
EFMbTrkHypoMh_hip_320.AcceptAll_EF = False
EFMbTrkHypoMh_hip_320.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_320.Min_pt = 0.4
EFMbTrkHypoMh_hip_320.Required_ntrks = 320
hypos["EFMbTrkHypoMh_hip_320"] = EFMbTrkHypoMh_hip_320

EFMbTrkHypoMh_hip_330 = MbTrkHypo("EFMbTrkHypoMh_hip_330")
EFMbTrkHypoMh_hip_330.AcceptAll_EF = False
EFMbTrkHypoMh_hip_330.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_330.Min_pt = 0.4
EFMbTrkHypoMh_hip_330.Required_ntrks = 330
hypos["EFMbTrkHypoMh_hip_330"] = EFMbTrkHypoMh_hip_330

EFMbTrkHypoMh_hip_340 = MbTrkHypo("EFMbTrkHypoMh_hip_340")
EFMbTrkHypoMh_hip_340.AcceptAll_EF = False
EFMbTrkHypoMh_hip_340.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_340.Min_pt = 0.4
EFMbTrkHypoMh_hip_340.Required_ntrks = 340
hypos["EFMbTrkHypoMh_hip_340"] = EFMbTrkHypoMh_hip_340

EFMbTrkHypoMh_hip_350 = MbTrkHypo("EFMbTrkHypoMh_hip_350")
EFMbTrkHypoMh_hip_350.AcceptAll_EF = False
EFMbTrkHypoMh_hip_350.Max_z0 = 401.#disable by setting to value higher than trkZ0Max
EFMbTrkHypoMh_hip_350.Min_pt = 0.4
EFMbTrkHypoMh_hip_350.Required_ntrks = 350
hypos["EFMbTrkHypoMh_hip_350"] = EFMbTrkHypoMh_hip_350


#----------------------------------------------------------------------------------------
# fex
class MbVxFex ( TrigVertexCounter ) :
    __slots__ = []
    def __init__ (self, name="MbVxFex"):
        super(MbVxFex, self).__init__(name)
        time = TrigTimeHistToolConfig("MbVxFexTimers")
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ MbVxFexMonitoring(), time]
        
EFMbVxFex = MbVxFex("EFMbVxFex")
EFMbVxFex.VtxContainerName = 'xPrimVx'
EFMbVxFex.pTmin = 400.

fexes["EFMbVxFex"] = EFMbVxFex

#------------------------------------------------------
# Hypo
class MbVxHypo ( TrigVertexCounterHypo ):
    __slots__ = []
    def __init__ (self, name="MbVxHypo"):
        super(MbVxHypo, self).__init__(name)
        time = TrigTimeHistToolConfig("MbVxHypoTimers")
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ MbVxHypoMonitoring(), time]




EFMbVxHypo_PT = MbVxHypo("EFMbVxHypo_PT")
EFMbVxHypo_PT.AcceptAll_EF = True
EFMbVxHypo_PT.RejectPileup = False
EFMbVxHypo_PT.Required_ntrks = 70 
hypos["EFMbVxHypo_PT"] = EFMbVxHypo_PT



EFMbOneVxHypo = MbVxHypo("EFMbOneVxHypo")
EFMbOneVxHypo.AcceptAll_EF = False
EFMbOneVxHypo.RejectPileup = True
EFMbOneVxHypo.Required_ntrks = 5
EFMbOneVxHypo.Max_ntrks = 4 
hypos["EFMbOneVxHypo"] = EFMbOneVxHypo

# need property to require number of vertices > N
#here it is
# many here is at least 9 vertex have no requirement on number of tracks associated
EFMbManyVxHypo = MbVxHypo("EFMbManyVxHypo")
EFMbManyVxHypo.AcceptAll_EF = False
EFMbManyVxHypo.RejectPileup = False
EFMbManyVxHypo.CutNumVtx = True
EFMbManyVxHypo.Required_ntrks = 0
EFMbManyVxHypo.Min_Nvtx = 9
hypos["EFMbManyVxHypo"] = EFMbManyVxHypo

EFMbVxHypo = MbVxHypo("EFMbVxHypo")
EFMbVxHypo.AcceptAll_EF = False
EFMbVxHypo.RejectPileup = False
EFMbVxHypo.Required_ntrks = 70
hypos["EFMbVxHypo"] = EFMbVxHypo


# -------------------------------------------------------------------

# -----------------------------------------------------------
EFMbVxHypoMh_hip_30 = MbVxHypo("EFMbVxHypoMh_hip_30")
EFMbVxHypoMh_hip_30.AcceptAll_EF = False
EFMbVxHypoMh_hip_30.RejectPileup = False
EFMbVxHypoMh_hip_30.Required_ntrks = 30
hypos["EFMbVxHypoMh_hip_30"] = EFMbVxHypoMh_hip_30

EFMbVxHypoMh_hip_50 = MbVxHypo("EFMbVxHypoMh_hip_50")
EFMbVxHypoMh_hip_50.AcceptAll_EF = False
EFMbVxHypoMh_hip_50.RejectPileup = False
EFMbVxHypoMh_hip_50.Required_ntrks = 50
hypos["EFMbVxHypoMh_hip_50"] = EFMbVxHypoMh_hip_50

EFMbVxHypoMh_hip_60 = MbVxHypo("EFMbVxHypoMh_hip_60")
EFMbVxHypoMh_hip_60.AcceptAll_EF = False
EFMbVxHypoMh_hip_60.RejectPileup = False
EFMbVxHypoMh_hip_60.Required_ntrks = 60
hypos["EFMbVxHypoMh_hip_60"] = EFMbVxHypoMh_hip_60

EFMbVxHypoMh_hip_70 = MbVxHypo("EFMbVxHypoMh_hip_70")
EFMbVxHypoMh_hip_70.AcceptAll_EF = False
EFMbVxHypoMh_hip_70.RejectPileup = False
EFMbVxHypoMh_hip_70.Required_ntrks = 70
hypos["EFMbVxHypoMh_hip_70"] = EFMbVxHypoMh_hip_70

EFMbVxHypoMh_hip_80 = MbVxHypo("EFMbVxHypoMh_hip_80")
EFMbVxHypoMh_hip_80.AcceptAll_EF = False
EFMbVxHypoMh_hip_80.RejectPileup = False
EFMbVxHypoMh_hip_80.Required_ntrks = 80
hypos["EFMbVxHypoMh_hip_80"] = EFMbVxHypoMh_hip_80

EFMbVxHypoMh_hip_90 = MbVxHypo("EFMbVxHypoMh_hip_90")
EFMbVxHypoMh_hip_90.AcceptAll_EF = False
EFMbVxHypoMh_hip_90.RejectPileup = False
EFMbVxHypoMh_hip_90.Required_ntrks = 90
hypos["EFMbVxHypoMh_hip_90"] = EFMbVxHypoMh_hip_90

EFMbVxHypoMh_hip_100 = MbVxHypo("EFMbVxHypoMh_hip_100")
EFMbVxHypoMh_hip_100.AcceptAll_EF = False
EFMbVxHypoMh_hip_100.RejectPileup = False
EFMbVxHypoMh_hip_100.Required_ntrks = 100
hypos["EFMbVxHypoMh_hip_100"] = EFMbVxHypoMh_hip_100

EFMbVxHypoMh_hip_110 = MbVxHypo("EFMbVxHypoMh_hip_110")
EFMbVxHypoMh_hip_110.AcceptAll_EF = False
EFMbVxHypoMh_hip_110.RejectPileup = False
EFMbVxHypoMh_hip_110.Required_ntrks = 110
hypos["EFMbVxHypoMh_hip_110"] = EFMbVxHypoMh_hip_110

EFMbVxHypoMh_hip_120 = MbVxHypo("EFMbVxHypoMh_hip_120")
EFMbVxHypoMh_hip_120.AcceptAll_EF = False
EFMbVxHypoMh_hip_120.RejectPileup = False
EFMbVxHypoMh_hip_120.Required_ntrks = 120
hypos["EFMbVxHypoMh_hip_120"] = EFMbVxHypoMh_hip_120

EFMbVxHypoMh_hip_130 = MbVxHypo("EFMbVxHypoMh_hip_130")
EFMbVxHypoMh_hip_130.AcceptAll_EF = False
EFMbVxHypoMh_hip_130.RejectPileup = False
EFMbVxHypoMh_hip_130.Required_ntrks = 130
hypos["EFMbVxHypoMh_hip_130"] = EFMbVxHypoMh_hip_130

EFMbVxHypoMh_hip_140 = MbVxHypo("EFMbVxHypoMh_hip_140")
EFMbVxHypoMh_hip_140.AcceptAll_EF = False
EFMbVxHypoMh_hip_140.RejectPileup = False
EFMbVxHypoMh_hip_140.Required_ntrks = 140
hypos["EFMbVxHypoMh_hip_140"] = EFMbVxHypoMh_hip_140

EFMbVxHypoMh_hip_150 = MbVxHypo("EFMbVxHypoMh_hip_150")
EFMbVxHypoMh_hip_150.AcceptAll_EF = False
EFMbVxHypoMh_hip_150.RejectPileup = False
EFMbVxHypoMh_hip_150.Required_ntrks = 150
hypos["EFMbVxHypoMh_hip_150"] = EFMbVxHypoMh_hip_150

EFMbVxHypoMh_hip_155 = MbVxHypo("EFMbVxHypoMh_hip_155")
EFMbVxHypoMh_hip_155.AcceptAll_EF = False
EFMbVxHypoMh_hip_155.RejectPileup = False
EFMbVxHypoMh_hip_155.Required_ntrks = 155
hypos["EFMbVxHypoMh_hip_155"] = EFMbVxHypoMh_hip_155

EFMbVxHypoMh_hip_160 = MbVxHypo("EFMbVxHypoMh_hip_160")
EFMbVxHypoMh_hip_160.AcceptAll_EF = False
EFMbVxHypoMh_hip_160.RejectPileup = False
EFMbVxHypoMh_hip_160.Required_ntrks = 160
hypos["EFMbVxHypoMh_hip_160"] = EFMbVxHypoMh_hip_160

EFMbVxHypoMh_hip_170 = MbVxHypo("EFMbVxHypoMh_hip_170")
EFMbVxHypoMh_hip_170.AcceptAll_EF = False
EFMbVxHypoMh_hip_170.RejectPileup = False
EFMbVxHypoMh_hip_170.Required_ntrks = 170
hypos["EFMbVxHypoMh_hip_170"] = EFMbVxHypoMh_hip_170

EFMbVxHypoMh_hip_175 = MbVxHypo("EFMbVxHypoMh_hip_175")
EFMbVxHypoMh_hip_175.AcceptAll_EF = False
EFMbVxHypoMh_hip_175.RejectPileup = False
EFMbVxHypoMh_hip_175.Required_ntrks = 175
hypos["EFMbVxHypoMh_hip_175"] = EFMbVxHypoMh_hip_175

EFMbVxHypoMh_hip_180 = MbVxHypo("EFMbVxHypoMh_hip_180")
EFMbVxHypoMh_hip_180.AcceptAll_EF = False
EFMbVxHypoMh_hip_180.RejectPileup = False
EFMbVxHypoMh_hip_180.Required_ntrks = 180
hypos["EFMbVxHypoMh_hip_180"] = EFMbVxHypoMh_hip_180


EFMbVxHypoMh_hip_185 = MbVxHypo("EFMbVxHypoMh_hip_185")
EFMbVxHypoMh_hip_185.AcceptAll_EF = False
EFMbVxHypoMh_hip_185.RejectPileup = False
EFMbVxHypoMh_hip_185.Required_ntrks = 185
hypos["EFMbVxHypoMh_hip_185"] = EFMbVxHypoMh_hip_185

EFMbVxHypoMh_hip_190 = MbVxHypo("EFMbVxHypoMh_hip_190")
EFMbVxHypoMh_hip_190.AcceptAll_EF = False
EFMbVxHypoMh_hip_190.RejectPileup = False
EFMbVxHypoMh_hip_190.Required_ntrks = 190
hypos["EFMbVxHypoMh_hip_190"] = EFMbVxHypoMh_hip_190


EFMbVxHypoMh_hip_200 = MbVxHypo("EFMbVxHypoMh_hip_200")
EFMbVxHypoMh_hip_200.AcceptAll_EF = False
EFMbVxHypoMh_hip_200.RejectPileup = False
EFMbVxHypoMh_hip_200.Required_ntrks = 200
hypos["EFMbVxHypoMh_hip_200"] = EFMbVxHypoMh_hip_200

EFMbVxHypoMh_hip_210 = MbVxHypo("EFMbVxHypoMh_hip_210")
EFMbVxHypoMh_hip_210.AcceptAll_EF = False
EFMbVxHypoMh_hip_210.RejectPileup = False
EFMbVxHypoMh_hip_210.Required_ntrks = 210
hypos["EFMbVxHypoMh_hip_210"] = EFMbVxHypoMh_hip_210

EFMbVxHypoMh_hip_220 = MbVxHypo("EFMbVxHypoMh_hip_220")
EFMbVxHypoMh_hip_220.AcceptAll_EF = False
EFMbVxHypoMh_hip_220.RejectPileup = False
EFMbVxHypoMh_hip_220.Required_ntrks = 220
hypos["EFMbVxHypoMh_hip_220"] = EFMbVxHypoMh_hip_220

EFMbVxHypoMh_hip_225 = MbVxHypo("EFMbVxHypoMh_hip_225")
EFMbVxHypoMh_hip_225.AcceptAll_EF = False
EFMbVxHypoMh_hip_225.RejectPileup = False
EFMbVxHypoMh_hip_225.Required_ntrks = 225
hypos["EFMbVxHypoMh_hip_225"] = EFMbVxHypoMh_hip_225

EFMbVxHypoMh_hip_230 = MbVxHypo("EFMbVxHypoMh_hip_230")
EFMbVxHypoMh_hip_230.AcceptAll_EF = False
EFMbVxHypoMh_hip_230.RejectPileup = False
EFMbVxHypoMh_hip_230.Required_ntrks = 230
hypos["EFMbVxHypoMh_hip_230"] = EFMbVxHypoMh_hip_230

EFMbVxHypoMh_hip_240 = MbVxHypo("EFMbVxHypoMh_hip_240")
EFMbVxHypoMh_hip_240.AcceptAll_EF = False
EFMbVxHypoMh_hip_240.RejectPileup = False
EFMbVxHypoMh_hip_240.Required_ntrks = 240
hypos["EFMbVxHypoMh_hip_240"] = EFMbVxHypoMh_hip_240

EFMbVxHypoMh_hip_245 = MbVxHypo("EFMbVxHypoMh_hip_245")
EFMbVxHypoMh_hip_245.AcceptAll_EF = False
EFMbVxHypoMh_hip_245.RejectPileup = False
EFMbVxHypoMh_hip_245.Required_ntrks = 245
hypos["EFMbVxHypoMh_hip_245"] = EFMbVxHypoMh_hip_245

EFMbVxHypoMh_hip_260 = MbVxHypo("EFMbVxHypoMh_hip_260")
EFMbVxHypoMh_hip_260.AcceptAll_EF = False
EFMbVxHypoMh_hip_260.RejectPileup = False
EFMbVxHypoMh_hip_260.Required_ntrks = 260
hypos["EFMbVxHypoMh_hip_260"] = EFMbVxHypoMh_hip_260











#----------------------------------------------------------------------------------------
