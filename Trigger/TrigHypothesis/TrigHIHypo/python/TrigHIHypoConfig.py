# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import TrigHIJetBaseSubtraction
from TrigHIHypo.TrigHIHypoConf import TrigHIEFTrackHypo
from TrigHIHypo.TrigHIHypoConf import TrigHIL2VtxMultHypo
from TrigHIHypo.TrigHIHypoConf import ETSumHypo

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

from AthenaCommon.SystemOfUnits import GeV, TeV
hypos = {}
fexes = {}
    
class HIJetBaseSubtractionBase (TrigHIJetBaseSubtraction):
    __slots__ = []
    def __init__(self, name):
        super( HIJetBaseSubtractionBase, self ).__init__( name )
        
        from TrigHIHypo.TrigHIHypoMonitoring import TrigHIJetBaseSubtractionValidationMonitoring, TrigHIJetBaseSubtractionOnlineMonitoring, TrigHIJetBaseSubtractionCosmicMonitoring
        validation = TrigHIJetBaseSubtractionValidationMonitoring()
        online = TrigHIJetBaseSubtractionOnlineMonitoring()
        cosmic = TrigHIJetBaseSubtractionCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("HIJetBaseSubtraction_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
class HIJetBaseSubtraction (HIJetBaseSubtractionBase):
    __slots__ = []
    def __init__(self, name = "HIJetBaseSubtraction"):
        super( HIJetBaseSubtraction, self ).__init__( name )

        self.coneRadius = 0.4
        #Area of Calorimeter: 2*Pi*9.8
        self.CaloArea = 61.57522
        #Background eta weights
        self.BgnEtaBin = 50
        self.BgEtaReg=[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,
                     1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                     2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
                     3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,
                     4.0,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,5.0]
 

        self.BgWeights=[1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
        1.0,1.0,1.0]

BaseSubtraction = HIJetBaseSubtraction()

# monitoring
class HIEFTrackHypo_OnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIEFTrackHypo_OnlineMonitoring"):
        super(HIEFTrackHypo_OnlineMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        
        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [GeV]",
                                             xbins = 100, xmin=0.0, xmax=50)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPartIn',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('numTrkPartOut',
                                             type='TH1F',
                                             title="Number of output Tracks; Number of tracks",
                                             xbins = 25, xmin=0., xmax=25.)]


class HIEFTrackHypo_loose1_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIEFTrackHypo_loose1_ValidationMonitoring"):
        super(HIEFTrackHypo_loose1_ValidationMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [GeV]",
                                             xbins = 100, xmin=0.0, xmax=50)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPartIn',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('numTrkPartOut',
                                             type='TH1F',
                                             title="Number of output Tracks; Number of tracks",
                                             xbins = 25, xmin=0., xmax=25.)]

class HIEFTrackHypo_loose2_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIEFTrackHypo_loose2_ValidationMonitoring"):
        super(HIEFTrackHypo_loose2_ValidationMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [GeV]",
                                             xbins = 100, xmin=0.0, xmax=50)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPartIn',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('numTrkPartOut',
                                             type='TH1F',
                                             title="Number of output Tracks; Number of tracks",
                                             xbins = 25, xmin=0., xmax=25.)]


class HIEFTrackHypo_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIEFTrackHypo_ValidationMonitoring"):
        super(HIEFTrackHypo_ValidationMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [GeV]",
                                             xbins = 100, xmin=0.0, xmax=50)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPartIn',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('numTrkPartOut',
                                             type='TH1F',
                                             title="Number of output Tracks; Number of tracks",
                                             xbins = 25, xmin=0., xmax=25.)]


class HIEFTrackHypo_AtLeastOneTrack_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIEFTrackHypo_AtLeastOneTrack_ValidationMonitoring"):
        super(HIEFTrackHypo_AtLeastOneTrack_ValidationMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [GeV]",
                                             xbins = 100, xmin=0.0, xmax=50)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPartIn',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('numTrkPartOut',
                                             type='TH1F',
                                             title="Number of output Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=100.)]


class HIEFTrackHypo (TrigHIEFTrackHypo):
    __slots__ = []
    def __init__(self, name):
        super( HIEFTrackHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AthenaMonTools = [ HIEFTrackHypo_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]
        
        #from TrigHIHypo.TrigHIHypoMonitoring import TrigHIJetBaseSubtractionValidationMonitoring, TrigHIJetBaseSubtractionOnlineMonitoring, TrigHIJetBaseSubtractionCosmicMonitoring
        #validation = TrigHIJetBaseSubtractionValidationMonitoring()
        #online = TrigHIJetBaseSubtractionOnlineMonitoring()
        #cosmic = TrigHIJetBaseSubtractionCosmicMonitoring()
        
        #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        #time = TrigTimeHistToolConfig("HIJetBaseSubtraction_Time")
        
        #self.AthenaMonTools = [ time, validation, online, cosmic ]
        self.MinNumTracks = 1
        self.MaxNumTracks = 10


class HIEFTrackHypo_loose1 (TrigHIEFTrackHypo):
    __slots__ = []
    def __init__(self, name):
        super( HIEFTrackHypo_loose1, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AthenaMonTools = [ HIEFTrackHypo_loose1_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]
        
        #from TrigHIHypo.TrigHIHypoMonitoring import TrigHIJetBaseSubtractionValidationMonitoring, TrigHIJetBaseSubtractionOnlineMonitoring, TrigHIJetBaseSubtractionCosmicMonitoring
        #validation = TrigHIJetBaseSubtractionValidationMonitoring()
        #online = TrigHIJetBaseSubtractionOnlineMonitoring()
        #cosmic = TrigHIJetBaseSubtractionCosmicMonitoring()
        
        #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        #time = TrigTimeHistToolConfig("HIJetBaseSubtraction_Time")
        
        #self.AthenaMonTools = [ time, validation, online, cosmic ]
        self.MinNumTracks = 1
        self.MaxNumTracks = 17

class HIEFTrackHypo_loose2 (TrigHIEFTrackHypo):
    __slots__ = []
    def __init__(self, name):
        super( HIEFTrackHypo_loose2, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AthenaMonTools = [ HIEFTrackHypo_loose1_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]
        
        #from TrigHIHypo.TrigHIHypoMonitoring import TrigHIJetBaseSubtractionValidationMonitoring, TrigHIJetBaseSubtractionOnlineMonitoring, TrigHIJetBaseSubtractionCosmicMonitoring
        #validation = TrigHIJetBaseSubtractionValidationMonitoring()
        #online = TrigHIJetBaseSubtractionOnlineMonitoring()
        #cosmic = TrigHIJetBaseSubtractionCosmicMonitoring()
        
        #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        #time = TrigTimeHistToolConfig("HIJetBaseSubtraction_Time")
        
        #self.AthenaMonTools = [ time, validation, online, cosmic ]
        self.MinNumTracks = 2
        self.MaxNumTracks = 17

class HIEFTrackHypo_AtLeastOneTrack (TrigHIEFTrackHypo):
    __slots__ = []
    def __init__(self, name):
        super( HIEFTrackHypo_AtLeastOneTrack, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AthenaMonTools = [ HIEFTrackHypo_AtLeastOneTrack_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]
        
        self.MinNumTracks = 1
        self.MaxNumTracks = 10000

class ETSumHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name, min=0, max=100*GeV, comps=''):
        super(ETSumHypoMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms = [ defineHistogram('SumEt',
                                            type='TH1F',
                                            title='SumEt over '+comps,
                                            xbins = 100, xmin=min, xmax=4.5*TeV)]
        
        self.Histograms += [ defineHistogram('SumEtPassing',
                                             type='TH1F',
                                             title='SumEt over '+comps+' for passing events',
                                             xbins = 100, xmin=min, xmax=max)]

class ETSumHypoBase(ETSumHypo):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( ETSumHypoBase, self ).__init__(name, **kwargs )

        comps = self.getDefaultProperty('Components')
        if hasattr(self, 'Components'): # if was set
            comps = self.Components                

        
        self.AthenaMonTools = [ ETSumHypoMonitoring('Monitoring', min=0, max=self.UpperBound+50.*GeV, comps=' '.join(comps) ) ]


class HIL2VtxMultHypo (TrigHIL2VtxMultHypo):
    __slots__ = []
    def __init__(self, name, weightThreshold=500, minVertices=1):
        super( HIL2VtxMultHypo, self ).__init__( name )

        self.AthenaMonTools = [ TrigTimeHistToolConfig("Time") ]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # allow at most 1 vertex
        self.MinVertices = minVertices
        self.WeightThreshold = weightThreshold

        self.AthenaMonTools += [ HIL2VtxMultHypoMonitoring() ]

#----------------------------------------------------------------------------------------
# HIL2VtxMultHypo monitoring

from TriggerJobOpts.TriggerFlags  import TriggerFlags

class HIL2VtxMultHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIL2VtxMultHypoMonitoring"):
        super(HIL2VtxMultHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])

        self.Histograms += [ defineHistogram('numVertices',
                                             type  = 'TH1I',
                                             title = "Vertices;evts",
                                             xbins = 10, xmin=0.0, xmax=10.)]
        
        menuName = TriggerFlags.triggerMenuSetup()      
        range = 5000 if '_HI_' in menuName else 2500

        self.Histograms += [ defineHistogram('vertexWeight',
                                             type  = 'TH1D',
                                             title = "Vertex Weight;evts",
                                             xbins = range, xmin=0, xmax=range)]
        self.Histograms += [ defineHistogram('vertexWeightMax',
                                             type  = 'TH1D',
                                             title = " Max Vertex Weight;evts",
                                             xbins = range, xmin=0, xmax=range)]
        

hypos["ETSumHypo_fte75_veto"] = ETSumHypoBase(name='ETSumHypo_fte75_veto', LowerBound=-1., UpperBound=75*GeV )
hypos["CentralityHypo_80_60"] = ETSumHypoBase(name='CentralityHypo_80_60', LowerBound=40.*GeV, UpperBound=240*GeV )
hypos["CentralityHypo_100_60"] = ETSumHypoBase(name='CentralityHypo_100_60', LowerBound=-1., UpperBound=240*GeV )

hypos["HIL2VtxMultHyp_100"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_100', weightThreshold=100 )
hypos["HIL2VtxMultHyp_125"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_125', weightThreshold=125 )
hypos["HIL2VtxMultHyp_150"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_150', weightThreshold=150 )
hypos["HIL2VtxMultHyp_200"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_200', weightThreshold=200 )
hypos["HIL2VtxMultHyp_300"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_300', weightThreshold=300 )
hypos["HIL2VtxMultHyp_350"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_350', weightThreshold=350 )
hypos["HIL2VtxMultHyp_400"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_400', weightThreshold=400 )
hypos["HIL2VtxMultHyp_500"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_500', weightThreshold=500 )
hypos["HIL2VtxMultHyp_550"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_550', weightThreshold=550 )
hypos["HIL2VtxMultHyp_600"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_600', weightThreshold=600 )
hypos["HIL2VtxMultHyp_700"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_700', weightThreshold=700 )
hypos["HIL2VtxMultHyp_800"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_800', weightThreshold=800 )
hypos["HIL2VtxMultHyp_900"]  = HIL2VtxMultHypo(name='HIL2VtxMultHyp_900', weightThreshold=900 )
hypos["HIL2VtxMultHyp_1000"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1000', weightThreshold=1000 )
hypos["HIL2VtxMultHyp_1100"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1100', weightThreshold=1100 )
hypos["HIL2VtxMultHyp_1200"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1200', weightThreshold=1200 )
hypos["HIL2VtxMultHyp_1300"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1300', weightThreshold=1300 )
hypos["HIL2VtxMultHyp_1400"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1400', weightThreshold=1400 )
hypos["HIL2VtxMultHyp_1500"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1500', weightThreshold=1500 )
hypos["HIL2VtxMultHyp_1600"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1600', weightThreshold=1600 )
hypos["HIL2VtxMultHyp_1700"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1700', weightThreshold=1700 )
hypos["HIL2VtxMultHyp_1800"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1800', weightThreshold=1800 )
hypos["HIL2VtxMultHyp_1900"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_1900', weightThreshold=1900 )
hypos["HIL2VtxMultHyp_2000"] = HIL2VtxMultHypo(name='HIL2VtxMultHyp_2000', weightThreshold=2000 )



from TrigHIHypo.TrigHIHypoConf import LbyLTopoClusterHypo
class LbyLTopoClusterHypoConfig(LbyLTopoClusterHypo):
    def __init__(self, name="LbyLTopoClusterHypoConfig"):
        super(LbyLTopoClusterHypoConfig, self).__init__(name)

	from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

	class LbyLHypoMonitoring(TrigGenericMonitoringToolConfig):
	    def __init__ (self, name="LbyLHypoMOnitoring"):
	        """ defines histograms for validation """
	        super(LbyLHypoMonitoring, self).__init__(name)
        	self.defineTarget(['Online', 'Validation'])
        	import math
        	self.Histograms = [ defineHistogram('AllDPhi', type='TH1F', title="Cluster distance;radians; n pairs", xbins=64, xmin=0., xmax=math.pi) ]
		self.AthenaMonTools = [ LbyLHypoMonitoring() ]

	        print self

if __name__ == "__main__":
    print LbyLTopoClusterHypoConfig()
