# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from TrigDetCalib.TrigDetCalibConf import TrigROBListWriter, TrigSubDetListWriter
from TrigDetCalib.TrigDetCalibConf import TrigROBSelector
from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter
from TrigDetCalib.TrigDetCalibConf import TrigEtaHypo
from TrigDetCalib.TrigDetCalibConf import TrigCheckForTracks
from TrigDetCalib.TrigDetCalibConf import TrigCheckForMuons
from TrigDetCalib.TrigDetCalibMonitoring import setCalibMonTools

from AthenaCommon.Logging import logging


class LArROBSelector(TrigROBSelector) :
  __slots__ = []
  def __init__(self, name = 'LArROBSelector'):
    super( LArROBSelector, self ).__init__( name )
    self.Subdetectors = 'LAr,ID'


class TrkROBSelector(TrigROBSelector) :
  __slots__ = []
  def __init__(self, name = 'TrkROBSelector'):
    super( TrkROBSelector, self ).__init__( name )
    self.Subdetectors = 'ID'


class TileROBSelector(TrigROBSelector) :
  __slots__ = []
  def __init__(self, name = 'TileROBSelector'):
    super( TileROBSelector, self ).__init__( name )
    self.Subdetectors = 'TILE'

class GenericROBSelector(TrigROBSelector) :
  __slots__ = []
  def __init__(self, name = 'GenericROBSelector', subDets = ''):
    super( GenericROBSelector, self ).__init__( name )
    self.Subdetectors = subDets


class L2ROBListWriter(TrigROBListWriter):
  __slots__ = []
  def __init__(self, name = 'L2ROBListWriter', addCTPResult = False, addL2Result = False,
               etaWidth = 0.1, phiWidth = 0.1):
    super( L2ROBListWriter, self ).__init__( name )
    self.AddCTPResult = addCTPResult
    self.AddL2Result  = addL2Result
    self.etaWidth = etaWidth
    self.phiWidth = phiWidth
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterValidationMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterOnlineMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterCosmicMonitoring
    validation = TrigROBListWriterValidationMonitoring()
    online = TrigROBListWriterOnlineMonitoring()
    cosmic = TrigROBListWriterCosmicMonitoring()
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(name + '_Time')
    self.AthenaMonTools = [ validation, online, time, cosmic ]


class GenericL2ROBListWriter(L2ROBListWriter):
  __slots__ = []
  def __init__(self, name = 'GenericL2ROBListWriter', subDets = '', roILabels = [ 'initialRoI' ], addCTPResult = True, addL2Result = True,
               etaWidth = 0.1, phiWidth = 0.1):
    super( GenericL2ROBListWriter, self ).__init__( name, addCTPResult, addL2Result, etaWidth, phiWidth)
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = roILabels
  
  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = GenericROBSelector(self.name + '_ROBSelector_L2', subDets)


class LArL2ROBListWriter(L2ROBListWriter):
  __slots__ = []
  def __init__(self, name = 'LArL2ROBListWriter', addCTPResult = True, addL2Result = True,
               etaWidth = 0.1, phiWidth = 0.1):
    super( LArL2ROBListWriter, self ).__init__( name, addCTPResult, addL2Result, etaWidth, phiWidth)
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = [ 'TrigT2CaloEgamma', 'initialRoI' ]
  
  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = LArROBSelector('LArROBSelector_L2')

class TrkROBListWriter(L2ROBListWriter):
  __slots__ = []
  def __init__(self, name = 'TrkROBListWriter', etaWidth = 0.1, phiWidth = 0.1):
    super( TrkROBListWriter, self ).__init__( name, addCTPResult = True, addL2Result = True)
    self.MaxRoIsPerEvent = -1
    self.RoIDescriptorLabels = [ 'TrigT2CaloEgamma', 'initialRoI' ]

  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = TrkROBSelector()


class EFROBListWriter(TrigROBListWriter):
  __slots__ = []
  def __init__(self, name = 'EFROBListWriter', addCTPResult = False, addL2Result = False, addEFResult = False,
               etaWidth = 0.1, phiWidth = 0.1):
    super( EFROBListWriter, self ).__init__(name)
    self.AddCTPResult = addCTPResult
    self.AddL2Result  = addL2Result
    self.AddEFResult  = addEFResult
    self.etaWidth = etaWidth
    self.phiWidth = phiWidth
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterValidationMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterOnlineMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterCosmicMonitoring
    validation = TrigROBListWriterValidationMonitoring()
    online = TrigROBListWriterOnlineMonitoring()
    cosmic = TrigROBListWriterCosmicMonitoring()
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig( name + '_Time' )
    self.AthenaMonTools = [ validation, online, time, cosmic ]


class GenericEFROBListWriter(EFROBListWriter):
  __slots__ = []
  def __init__(self, name = 'GenericEFROBListWriter', subDets = '', roILabels = [ 'initialRoI' ], 
               addCTPResult = True, addL2Result = True, addEFResult = True,
               etaWidth = 0.1, phiWidth = 0.1):
    super( LArEFROBListWriter, self ).__init__( name, addCTPResult, addL2Result, addEFResult, etaWidth, phiWidth)
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = roILabels

  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = GenericROBSelector(self.name + '_ROBSelector_EF', subDets)


class LArEFROBListWriter (EFROBListWriter):
  __slots__ = []
  def __init__(self, name = 'LArEFROBListWriter', addCTPResult = True, addL2Result = True, addEFResult = True,
               etaWidth = 0.1, phiWidth = 0.1):
    super( LArEFROBListWriter, self ).__init__( name, addCTPResult, addL2Result, addEFResult, etaWidth, phiWidth)
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = [ 'TrigT2CaloEgamma', 'initialRoI' ]

  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = LArROBSelector('LArROBSelector_EF')


class TileSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TileSubDetListWriter'):
    super( TileSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('TileSubDetListWriter_Time')
    self.SubdetId= ['Tile','TDAQ_CTP']
    self.MaxRoIsPerEvent = 1
    self.extraROBs = [ 5373968 ]
    self.AthenaMonTools = [ time ]

    #self.MonitoringHistoPath = '/EXPERT/TileSubDetListWriter/'


class CSCSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'CSCSubDetListWriter'):
    super( CSCSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('CSCSubDetListWriter_Time')
    self.SubdetId= [ 'CSC' ]  # should match the code in TrigSubDetListWriter.cxx
    self.MaxRoIsPerEvent = 1
    self.AthenaMonTools = [ time ]
    
class IBLSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'IBLSubDetListWriter'):
    super( IBLSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('IBLSubDetListWriter_Time')
    self.SubdetId= ['IBL','DBM','TDAQ_CTP']
    self.MaxRoIsPerEvent = 1
    self.AthenaMonTools = [ time ]

class PixelSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'PixelSubDetListWriter'):
    super( PixelSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('PixelSubDetListWriter_Time')
    self.SubdetId= ['Pixel','TDAQ_CTP']
    self.MaxRoIsPerEvent = 1
    self.AthenaMonTools = [ time ]
    #self.MonitoringHistoPath = '/EXPERT/PixelSubDetListWriter/'

class L1CaloSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'L1CaloSubDetListWriter'):
    super( L1CaloSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('L1CaloSubDetListWriter_Time')
    self.SubdetId= ['TDAQ_L1CALO']
    self.MaxRoIsPerEvent = 1
    self.AthenaMonTools = [ time ]
    #self.MonitoringHistoPath = '/EXPERT/L1CaloSubDetListWriter/'

class GenericSubDetListWriter(TrigSubDetListWriter) :
  __slots__ = []
  def __init__(self, name = 'GenericSubDetListWriter', subDetIds = [], maxRoIsPerEvent = 1):
    super( GenericSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig('GenericSubDetListWriter_Time')
    self.SubdetId = subDetIds
    self.MaxRoIsPerEvent = maxRoIsPerEvent
    self.AthenaMonTools = [ time ]


## Hypothesis algos to select only a particular eta range

class EtaHypo_HEC(TrigEtaHypo) :
  __slots__ = []
  def __init__(self, name = 'EtaHypo_HEC'):
    super( EtaHypo_HEC, self ).__init__( name )
    self.EtaMinCut = 1.4
    self.EtaMaxCut = 3.3

class EtaHypo_FCAL(TrigEtaHypo) :
  __slots__ = []
  def __init__(self, name = 'EtaHypo_FCAL'):
    super( EtaHypo_FCAL, self ).__init__( name )
    self.EtaMinCut = 2.9
    self.EtaMaxCut = 5.1


#### TrkIDAlign from StoreGate -->

class CheckForTracks(TrigCheckForTracks) :
  __slots__ = []
  def __init__(self, name = 'CheckForTracks'):
    super(CheckForTracks, self ).__init__( name )
    
  def setDefaults(self, handle):
    if not hasattr( handle, 'ROBSelector' ) :
      handle.ROBSelector = TrkROBSelector()


class CheckForTracks_Trk9_Central(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Central'):
    super(CheckForTracks_Trk9_Central, self ).__init__( name )
    self.pT_min = 9000.
    self.etaLowEdge = 0.0
    self.etaEdge = 1.4
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)

class CheckForTracks_Trk9_Fwd(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Fwd'):
    super(CheckForTracks_Trk9_Fwd, self ).__init__( name )
    self.pT_min = 9000.
    self.etaLowEdge = 1.35
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)

class CheckForTracks_Trk9_Central_Beamspot(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Central_Beamspot'):
    super(CheckForTracks_Trk9_Central_Beamspot, self ).__init__( name )
    self.pT_min = 3000.
    self.etaLowEdge = 0.0
    self.etaEdge = 1.4
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_BeamSpot'
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False
    self.lookForAnyTracks = True

    setCalibMonTools(self)

class CheckForTracks_Trk9_Fwd_Beamspot(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Fwd_Beamspot'):
    super(CheckForTracks_Trk9_Fwd_Beamspot, self ).__init__( name )
    self.pT_min = 3000.
    self.etaLowEdge = 1.35
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_BeamSpot'
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False
    self.lookForAnyTracks = True

    setCalibMonTools(self)

class CheckForTracks_Trk9_Central_SiTrack(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Central_SiTrack'):
    super(CheckForTracks_Trk9_Central_SiTrack, self ).__init__( name )
    self.pT_min = 9000.
    self.etaLowEdge = 0.0
    self.etaEdge = 1.4
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)

class CheckForTracks_Trk9_Fwd_SiTrack(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk9_Fwd_SiTrack'):
    super(CheckForTracks_Trk9_Fwd_SiTrack, self ).__init__( name )
    self.pT_min = 9000.
    self.etaLowEdge = 1.35
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)

    

class CheckForTracks_Trk16_Central(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk16_Central'):
    super(CheckForTracks_Trk16_Central, self ).__init__( name )
    self.pT_min = 16000.
    self.etaLowEdge = 0.0
    self.etaEdge = 1.4
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)
    
class CheckForTracks_Trk16_Fwd(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk16_Fwd'):
    super(CheckForTracks_Trk16_Fwd, self ).__init__( name )
    self.pT_min = 16000.
    self.etaLowEdge = 1.35
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)


class CheckForTracks_Trk29_Central(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk29_Central'):
    super(CheckForTracks_Trk29_Central, self ).__init__( name )
    self.pT_min = 29000.
    self.etaLowEdge = 0.0
    self.etaEdge = 1.4
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)
    
class CheckForTracks_Trk29_Fwd(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk29_Fwd'):
    super(CheckForTracks_Trk29_Fwd, self ).__init__( name )
    self.pT_min = 29000.
    self.etaLowEdge = 1.35
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)


class CheckForTracks_Trk_All(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_Trk_All'):
    super(CheckForTracks_Trk_All, self ).__init__( name )
    self.pT_min = 1000.
    self.etaLowEdge = 0.0
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrackCollection_TrigFastTrackFinder_TauIso'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False

    setCalibMonTools(self)


class CheckForTracks_eGamma(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_eGamma_SiTrk'):
    super(CheckForTracks_eGamma_SiTrk, self ).__init__( name )
    self.pT_min = 2000.
    self.etaLowEdge = 0.0
    self.etaEdge = 5.0
    self.tracksName = 'HLT_TrigFastTrackFinder_eGamma'
    self.lookForAnyTracks = True
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False
    setCalibMonTools(self)


class CheckForTracks_FullScan(CheckForTracks):
  __slots__ = []
  def __init__(self, name = 'CheckForTracks_FullScan_SiTrk'):
    super(CheckForTracks_FullScan_SiTrk, self ).__init__( name )
    self.pT_min = 2000.
    self.etaLowEdge = 0.0
    self.etaEdge = 5.0
    self.lookForAnyTracks = True
    self.tracksName = 'HLT_TrigFastTrackFinder_FullScan'
    #self.extraROBs = [ 7798784, 8060928, 8126464 ]
    self.AddCTPResult = True
    self.AddL2Result = False
    self.AddEFResult = False
    setCalibMonTools(self)


#### TrkIDAlign from StoreGate <--

    

class TDAQ_L1SubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_L1SubDetListWriter'):
    super( TDAQ_L1SubDetListWriter, self ).__init__( name )
    self.SubdetId= ['HLTResult','TDAQ_CTP']
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False
    #self.extraROBs = [ 7798784 ] # ROB ID for eformat::TDAQ_CTP
    

class TDAQ_L2SubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_L2SubDetListWriter'):
    super( TDAQ_L2SubDetListWriter, self ).__init__( name )
    self.SubdetId= ['TDAQ_LVL2','TDAQ_CTP']
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False
    #self.extraROBs = [ 7798784 ] # ROB ID for eformat::TDAQ_CTP, need CTP (hardware) ROB for HLT result


class TDAQ_EFSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_EFSubDetListWriter'):
    super( TDAQ_EFSubDetListWriter, self ).__init__( name )
    self.SubdetId= ['L2EFResult']
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False
    #self.extraROBs = [ 7798784 ] # ROB ID for eformat::TDAQ_CTP, need CTP (hardware) ROB for HLT result

class TDAQ_HLTSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_HLTSubDetListWriter'):
    super( TDAQ_HLTSubDetListWriter, self ).__init__( name )
    self.SubdetId= ['HLTResult']
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False
    #self.extraROBs = [ 7798784 ] # ROB ID for eformat::TDAQ_CTP, need CTP (hardware) ROB for HLT result

class TDAQ_TrigResultSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_EFSubDetListWriter'):
    super( TDAQ_TrigResultSubDetListWriter, self ).__init__( name )
    self.SubdetId= ['HLTResult']
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False
    #self.extraROBs = [ 7798784 ] # ROB ID for eformat::TDAQ_CTP, need CTP (hardware) ROB for HLT result

    
class TDAQ_ROB_SubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = 'TDAQ_ROB_SubDetListWriter', config = ''):
    super( TDAQ_EFSubDetListWriter, self ).__init__( name )
    self.Subdetectors = ''
    self.MaxRoIsPerEvent = 1
    self.addL1Calo = False

    log = logging.getLogger( 'TrigDetCalibConfig.py' )

    if config == 'L1':
      #self.extraROBs = [ 7798784 ] # ROB ID (code) for eformat::TDAQ_LVL1
      self.SubdetId= ['TDAQ_CTP']
    elif config == 'L2':
      #self.extraROBs = [ 8060928 ] # ROB ID (code) for eformat::TDAQ_LVL2
      self.SubdetId= ['TDAQ_LVL2']
    elif config == 'EF':
      #self.extraROBs = [ 8126464 ] # ROB ID (code) for eformat::TDAQ_EVENT_FILTER
      self.SubdetId= ['TDAQ_EVENT_FILTER']
    elif config == 'SFI':
      #self.extraROBs = [ 7929856 ] # ROB ID (code) for eformat::TDAQ_SFI
      self.SubdetId= ['TDAQ_SFI']
    else:
      log.warning('Unknown configuration string: '+config)
      log.warning('Will reset to config=SFI')
      #self.extraROBs = [ 7929856 ] # ROB ID (code) for eformat::TDAQ_SFI      

class ScoutTestAlg(ScoutingStreamWriter):
  __slots__ = []
  def __init__(self, name = 'ScoutTestAlg', CollTypeName = []):
    super( ScoutTestAlg, self ).__init__( name )
    self.CollectionTypeName = CollTypeName

    #For Example:
    self.CollectionTypeName = ["TrigRoiDescriptor#secondaryRoI_EF", "TrigMuonEFContainer#MuonEF","TrigOperationalInfo#DataScouting"]





class TrigCheckForMuons_pebM(TrigCheckForMuons):
  __slots__ = []
  def __init__(self, name = 'TrigCheckForMuons_pebM', 
               etaWidth = 0.3, phiWidth = 0.3):
    super( TrigCheckForMuons_pebM , self ).__init__( name )

    self.AddCTPResult = True
    self.AddHLTResult  = True
    self.etaWidth = etaWidth
    self.phiWidth = phiWidth
    self.MaxRoIsPerEvent = -1
    self.muonLabels = ["MuonEFInfo",""]
    self.PEBInfoLabel = "pebM"
    
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterValidationMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterOnlineMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterCosmicMonitoring
    validation = TrigROBListWriterValidationMonitoring(name+"ValidationMonitoring")
    online = TrigROBListWriterOnlineMonitoring(name+"OnlineMonitoring")
    cosmic = TrigROBListWriterCosmicMonitoring(name+"CosmicMonitoring")
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(name + '_Time')
    self.AthenaMonTools = [ validation, online, time, cosmic ]

class TrigCheckForMuons_peb075(TrigCheckForMuons):
  __slots__ = []
  def __init__(self, name = 'TrigCheckForMuons_peb075', 
               etaWidth = 0.75, phiWidth = 0.75):
    super( TrigCheckForMuons_peb075 , self ).__init__( name )

    self.AddCTPResult = True
    self.AddHLTResult  = True
    self.etaWidth = etaWidth
    self.phiWidth = phiWidth
    self.MaxRoIsPerEvent = -1
    self.muonLabels = ["MuonEFInfo",""]
    self.PEBInfoLabel = "peb075"

    #self.ROBSelector.Subdetectors = "All"  # this allows to modify list of subdetectors to include
    
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterValidationMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterOnlineMonitoring
    from TrigDetCalib.TrigDetCalibMonitoring import TrigROBListWriterCosmicMonitoring
    validation = TrigROBListWriterValidationMonitoring(name+"ValidationMonitoring")
    online = TrigROBListWriterOnlineMonitoring(name+"OnlineMonitoring")
    cosmic = TrigROBListWriterCosmicMonitoring(name+"CosmicMonitoring")
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(name + '_Time')
    self.AthenaMonTools = [ validation, online, time, cosmic ]
