#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ = "Mark Sutton, Matous Vozak"
__doc__    = "ConfigSettings"
__all__    = [ "getInDetTrigConfig" ]

import math 
from TrigInDetConfig.ConfigSettingsBase import _ConfigSettingsBase
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.SystemOfUnits import GeV


# Function that returns specific signature setting/configuration
# Rename to InDetTrigSignatureConfig ?
def getInDetTrigConfig( name ):
   if name in _ConfigSettings :
      config = _ConfigSettings[name]
      # keep a record of the configuration that is input
      # will use this to uniquely identify the algorithms
      config._input_name = name
      return config
   else :
      #       don't just return None, and do nothing as this
      #       will just hide the error until people try to use
      #       the bad slice configuration
      raise Exception( "getInDetTrigConfig() called with non existent slice: "+name )
      return None


class _ConfigSettings_electron( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name      = "electron"
      self._suffix    = "Electron"
      self._roi       = "HLT_Roi_Electron"
      # this soze of 0.05 should be increased to 0.1
      self._etaHalfWidth        = 0.05
      self._phiHalfWidth        = 0.1
      self._doCloneRemoval      = True #Previously False in Run2!
      self._doSeedRedundancyCheck = True
      self._doTRT               = True
      self._keepTrackParameters = True
      self._electronPID         = True

      
class _ConfigSettings_muon( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name      = "muon"
      self._suffix    = "Muon"
      self._roi       = "HLT_Roi_L2SAMuon"
      self._Triplet_D0Max       = 10.0
      self._doResMon            = True
      self._DoPhiFiltering      = False
      self._doSeedRedundancyCheck = True
      self._monPtMin            = 12*GeV


class _ConfigSettings_muonIso( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name      = "muonIso"
      self._suffix    = "MuonIso"
      self._roi       = "HLT_Roi_MuonIso"
      self._etaHalfWidth        = 0.35
      self._phiHalfWidth        = 0.35
      self._zedHalfWidth        = 10.0


class _ConfigSettings_tau( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "tau"
      self._suffix   = "Tau"
      self._roi      = "HLT_Roi_Tau"
      self._vertex   = "HLT_IDVertex_Tau"
      self._adaptiveVertex  = True
      self._pTmin           = 0.8*GeV
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4
      self._doTRT           = True
      # potential change coming up ...
      # self._minNSiHits_vtx = 6


class _ConfigSettings_tauCore( _ConfigSettingsBase ):
    def __init__( self ):
       _ConfigSettingsBase.__init__(self)
       self._name     = "tauCore"
       self._suffix   = "TauCore"
       self._roi      = "HLT_Roi_TauCore"
       self._holeSearch_FTF = True

class _ConfigSettings_tauIso( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "tauIso"
      self._suffix   = "TauIso"
      self._roi      = "RoiForTauIso"
      self._etaHalfWidth   = 0.4
      self._phiHalfWidth   = 0.4
      self._zedHalfWidth   = 7.0
      self._adaptiveVertex = True
      self._addSingleTrackVertices = True
      self._vertex         = "HLT_IDVertex_Tau"
      # potential change coming up ...
      # self._minNSiHits_vtx = 6

   def tracks_IDTrig(self):
      if self._doRecord:
         return recordable('HLT_IDTrack_Tau_IDTrig')
      else:
         return 'HLT_IDTrack_Tau_IDTrig'



class _ConfigSettings_bjet( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "bjet"
      self._suffix   = "Bjet"
      self._roi      = "HLT_Roi_Bjet"
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4


class _ConfigSettings_jetSuper( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "jetSuper"
      self._suffix   = "JetSuper"
      self._vertex   = "HLT_IDVertex_JetSuper"
      self._adaptiveVertex = True
      self._addSingleTrackVertices = True
      self._roi      = "HLT_Roi_JetSuper"
      self._doFullScan = True
      self._pTmin      = 1*GeV


class _ConfigSettings_minBias( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "minBias"
      self._suffix   = "MinBias"
      self._roi      = "HLT_Roi_MinBias"
      self._doFullScan      = True
      self._pTmin           = 0.2*GeV # TODO: double check
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = math.pi
      self._doZFinder       = True
      self._doZFinderOnly   = True



class _ConfigSettings_beamSpot( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "beamSpot"
      self._suffix   = "BeamSpot"
      self._roi      = "HLT_Roi_FS"
      self._doFullScan      = True
      self._doZFinder       = True
      self._DoubletDR_Max   = 200
      self._SeedRadBinWidth = 10
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = math.pi
      self._doTRT           = False
      self._doSeedRedundancyCheck = True
      self._doRecord        = False


class _ConfigSettings_fullScan( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "fullScan"
      self._suffix   = "FS"
      self._roi      = "HLT_Roi_FS"
      self._vertex              = "HLT_IDVertex_FS"
      self._adaptiveVertex      = True
      # not just yet, still hoping to validate
      # self._actsVertex          = True
      # these are being evaluated and may be added
      # self._addSingleTrackVertices = True
      # self._TracksMaxZinterval = 3
      self._vertex_jet          = "HLT_IDVertex_FS"
      self._adaptiveVertex_jet  = True
      self._doFullScan      = True
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = math.pi
      self._doTRT           = False
      self._DoubletDR_Max   = 200
      self._SeedRadBinWidth = 10
      self._TripletDoPPS    = False
      self._nClustersMin    = 8
      self._RoadWidth       = 5
      self._UseTrigSeedML   = 4
      self._dodEdxTrk         = True
      self._doHitDV           = True
      self._doDisappearingTrk = True


class _ConfigSettings_beamSpotFS( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "fullScan"
      self._suffix   = "FS"
      self._roi      = "HLT_Roi_FS"
      self._doFullScan      = True
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = math.pi
      self._doTRT           = False
      self._DoubletDR_Max   = 200
      self._SeedRadBinWidth = 10
      self._TripletDoPPS    = False
      self._nClustersMin    = 8
      self._RoadWidth       = 5
      self._UseTrigSeedML   = 4
      self._doRecord        = False


class _ConfigSettings_fullScanUTT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "fullScanUTT"
      self._suffix   = "FS"
      self._roi      = "HLT_Roi_FS"
      self._doFullScan      = True
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = math.pi
      self._doTRT           = False
      self._DoubletDR_Max   = 200
      self._SeedRadBinWidth = 10
      self._TripletDoPPS    = False
      self._nClustersMin    = 8
      self._RoadWidth       = 5
      self._UseTrigSeedML   = 4
      self._vertex          = "HLT_IDVertex_FS"


class _ConfigSettings_cosmics( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name        = "cosmics"
      self._suffix      = "Cosmic"
      self._roi         = "HLT_Roi_Cosmics"
      self._Triplet_D0Max       = 1000.0
      self._Triplet_D0_PPS_Max  = 1000.0
      self._TrackInitialD0Max   = 1000.
      self._TrackZ0Max          = 1000.
      self._doFullScan      = True
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = math.pi


class _ConfigSettings_bmumux( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name      = "bphysics"
      self._suffix    = "Bmumux"
      self._roi       = "HLT_Roi_Bmumux"
      self._Triplet_D0Max       = 10.
      self._DoPhiFiltering      = False
      self._etaHalfWidth        = 0.75
      self._phiHalfWidth        = 0.75
      self._doSeedRedundancyCheck = True


class _ConfigSettings_electronLRT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name       = "electronLRT"
      self._suffix     = "ElecLRT"
      self._roi        = "HLT_Roi_Electron"
      self._etaHalfWidth        = 0.1
      self._phiHalfWidth        = 0.4
      self._UsePixelSpacePoints = False
      self._Triplet_D0Max       = 300.
      self._TrackInitialD0Max   = 300.
      self._TrackZ0Max          = 500.
      self._keepTrackParameters = True
      self._doSeedRedundancyCheck = True
      self._nClustersMin        = 8
      self._isLRT               = True
      #pt config
      self._newConfig           = True
      self._maxZImpactPT        = 500.
      self._maxRPhiImpactPT     = 300.
      self._maxEtaPT            = 2.7
      self._maxDoubleHolesPT    = 0
      self._maxSiHolesPT        = 2
      self._maxPixelHolesPT     = 1
      self._maxSCTHolesPT       = 1
      self._minSiClustersPT     = 8
      self._doEmCaloSeedPT      = False
      self._minTRTonTrkPT       = 0


class _ConfigSettings_muonLRT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name       = "muonLRT"
      self._suffix     = "MuonLRT"
      self._roi        = "HLT_Roi_Muon"
      self._UsePixelSpacePoints = False
      self._etaHalfWidth        = 0.2
      self._phiHalfWidth        = 0.4
      self._Triplet_D0Max       = 300.
      self._TrackInitialD0Max   = 300.
      self._TrackZ0Max          = 500.
      self._doSeedRedundancyCheck = True
      self._nClustersMin        = 8
      self._isLRT               = True
      self._doResMon            = True
      self._DoPhiFiltering      = False
      #pt config
      self._newConfig           = True
      self._maxZImpactPT        = 500.
      self._maxRPhiImpactPT     = 300.
      self._maxEtaPT            = 2.7
      self._maxDoubleHolesPT    = 0
      self._maxSiHolesPT        = 2
      self._maxPixelHolesPT     = 1
      self._maxSCTHolesPT       = 1
      self._minSiClustersPT     = 8
      self._doEmCaloSeedPT      = False
      self._minTRTonTrkPT       = 0


class _ConfigSettings_tauLRT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "tauLRT"
      self._suffix   = "TauLRT"
      self._roi      = "HLT_Roi_TauCore"
      self._pTmin        = 0.8*GeV
      self._etaHalfWidth = 0.4
      self._phiHalfWidth = 0.4
      self._doTRT        = True
      self._UsePixelSpacePoints = False
      self._Triplet_D0Max       = 300.
      self._TrackInitialD0Max   = 300.
      self._TrackZ0Max          = 500.
      self._nClustersMin        = 8
      self._isLRT               = True
      #pt config
      self._newConfig           = True
      self._maxZImpactPT        = 500.
      self._maxRPhiImpactPT     = 300.
      self._maxEtaPT            = 2.7
      self._maxDoubleHolesPT    = 0
      self._maxSiHolesPT        = 2
      self._maxPixelHolesPT     = 1
      self._maxSCTHolesPT       = 1
      self._minSiClustersPT     = 8
      self._doEmCaloSeedPT      = False
      self._minTRTonTrkPT       = 0


class _ConfigSettings_bjetLRT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "bjetLRT"
      self._suffix   = "BjetLRT"
      self._roi      = "HLT_Roi_Bjet"
      self._etaHalfWidth = 0.4
      self._phiHalfWidth = 0.4
      self._UsePixelSpacePoints = False
      self._Triplet_D0Max       = 300.
      self._TrackInitialD0Max   = 300.
      self._TrackZ0Max          = 500.
      self._nClustersMin        = 8
      self._isLRT               = True
      #pt config
      self._newConfig           = True
      self._maxZImpactPT        = 500.
      self._maxRPhiImpactPT     = 300.
      self._maxEtaPT            = 2.7
      self._maxDoubleHolesPT    = 0
      self._maxSiHolesPT        = 2
      self._maxPixelHolesPT     = 1
      self._maxSCTHolesPT       = 1
      self._minSiClustersPT     = 8
      self._doEmCaloSeedPT      = False
      self._minTRTonTrkPT       = 0


class _ConfigSettings_fullScanLRT( _ConfigSettingsBase ):
   def __init__( self ):
      _ConfigSettingsBase.__init__(self)
      self._name     = "fullScanLRT"
      self._suffix   = "FSLRT"
      self._roi      = "HLT_Roi_FS"
      self._doFullScan      = True
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = math.pi
      self._doTRT           = False
      self._doSeedRedundancyCheck = True
      self._UsePixelSpacePoints   = False
      self._Triplet_D0Max         = 300.
      self._TrackInitialD0Max     = 300.
      self._TrackZ0Max            = 500.
      self._Triplet_D0_PPS_Max    = 300.
      self._DoubletDR_Max         = 200
      self._nClustersMin          = 8
      self._isLRT                 = True
      self._LRTD0Min              = 2.0
      self._LRTHardPtMin          = 1.0*GeV
      #pt config
      self._newConfig           = True
      self._maxZImpactPT        = 500.
      self._maxRPhiImpactPT     = 300.
      self._maxEtaPT            = 2.7
      self._maxDoubleHolesPT    = 0
      self._maxSiHolesPT        = 2
      self._maxPixelHolesPT     = 1
      self._maxSCTHolesPT       = 1
      self._minSiClustersPT     = 8
      self._doEmCaloSeedPT      = False
      self._minTRTonTrkPT       = 0





_ConfigSettings = {
   "electron"     : _ConfigSettings_electron(),
   "Electron"     : _ConfigSettings_electron(),

    "muon"        : _ConfigSettings_muon(),
    "muonIso"     : _ConfigSettings_muonIso(),
    "muonIsoMS"   : _ConfigSettings_muonIso(),
    "muonCore"    : _ConfigSettings_muon(),
    "muonFS"      : _ConfigSettings_muon(),
    "muonLate"    : _ConfigSettings_muon(),

    "Muon"        : _ConfigSettings_muon(),
    "MuonIso"     : _ConfigSettings_muonIso(),
    "MuonCore"    : _ConfigSettings_muon(),
    "MuonFS"      : _ConfigSettings_muon(),
    "MuonLate"    : _ConfigSettings_muon(),

    "tau"         : _ConfigSettings_tau(),
    "tauTau"      : _ConfigSettings_tau(),
    "tauCore"     : _ConfigSettings_tauCore(),
    "tauIso"      : _ConfigSettings_tauIso(),
    "tauIsoBDT"   : _ConfigSettings_tauIso(),

    "bjet"        : _ConfigSettings_bjet(),
    "Bjet"        : _ConfigSettings_bjet(),

    "jet"         : _ConfigSettings_fullScan(),
    #    "jet"         : _ConfigSettings_bjet(),
    "fullScan"    : _ConfigSettings_fullScan(),
    "FS"          : _ConfigSettings_fullScan(),

    "jetSuper"    : _ConfigSettings_jetSuper(),

    "beamSpot"    : _ConfigSettings_beamSpot(),
    "BeamSpot"    : _ConfigSettings_beamSpot(),
    "beamSpotFS"  : _ConfigSettings_beamSpotFS(),

    "cosmics"     : _ConfigSettings_cosmics(),
    "bmumux"      : _ConfigSettings_bmumux(),
    "bphysics"    : _ConfigSettings_bmumux(),
    "minBias"     : _ConfigSettings_minBias(),

    "electronLRT"    : _ConfigSettings_electronLRT(),
    "muonLRT"        : _ConfigSettings_muonLRT(),
    "tauLRT"         : _ConfigSettings_tauLRT(),
    "bjetLRT"        : _ConfigSettings_bjetLRT(),
    "fullScanLRT"    : _ConfigSettings_fullScanLRT() }
