#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

__author__ = "Mark Sutton, Matous Vozak"
__doc__    = "ConfigSettings"
__all__    = [ "getInDetTrigConfig" ]



from AthenaCommon.SystemOfUnits import GeV
#This function is a wrapper around functions that return name of collections
#It checks whether the collection is specified to be recorded into EDM and if so it marks it to be recordable
#Collection has to be defined in EDM! see:
#https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TrigEDMConfig/python/TriggerEDMRun3.py
#Maybe overkill?
def makeRecordable(getName):
   def recordCollection(*args, **kwargs):
      #Retrieve record decision
      record=kwargs.pop("doRecord",False)

      #Get the name of the collection
      collectionName = getName(*args, **kwargs)

      from TrigEDMConfig.TriggerEDMRun3 import recordable
      if record:
           return recordable( collectionName )
      else:
          return collectionName
   return recordCollection



#     Configuration of Tracking parameters for different signatures
#-----------------------------------------------------------------------------

class _Settings :
   def __init__(self, name = "Settings") :
      self._name = name

      # default values
      self._pTmin               = 1.*GeV
      self._d0SeedMax           = 4.0
      self._d0SeedPPSMax        = 1.7
      self._doZFinder           = False
      self._doResMon            = False
      self._doSpPhiFiltering    = True
      self._doCloneRemoval      = True
      self._checkRedundantSeeds = False
      self._dRdoubletMax        = 270
      self._seedRadBinWidth     = 2
      self._etaHalfWidth        = 0.1
      self._phiHalfWidth        = 0.1
      self._doFullScan          = False
      self._monPS               = 1
      self._monPtMin            = 1*GeV
      self._doTRT               = False #Apply TRT extension sequence after ambiguity solving
      self._keepTrackParameters = False #Keep track parameters in conversion to TrackParticles
      self._usePixelSP          = True
      self._d0TrackMax          = 20.
      self._z0TrackMax          = 300.
      self._isLRT               = False

#Previous version in:
#https://gitlab.cern.ch/atlas/athena/-/blob/master/InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigSliceSettings.py#L70

   @property
   def name(self):
      return self._name

   @property
   def pTmin(self):
      return self._pTmin

   @property
   def d0SeedMax(self):
      return self._d0SeedMax

   @property
   def d0SeedPPSMax(self):
      return self._d0SeedPPSMax

   @property
   def doZFinder(self):
      return self._doZFinder

   @property
   def doResMon(self):
      return self._doResMon

   @property
   def doSpPhiFiltering(self):
      return self._doSpPhiFiltering

   @property
   def doCloneRemoval(self):
      return self._doCloneRemoval

   @property
   def checkRedundantSeeds(self):
      return self._checkRedundantSeeds

   @property
   def dRdoubletMax(self):
      return self._dRdoubletMax

   @property
   def seedRadBinWidth(self):
      return self._seedRadBinWidth

   @property
   def etaHalfWidth(self):
      return self._etaHalfWidth

   @property
   def phiHalfWidth(self):
      return self._phiHalfWidth

   @property
   def doFullScan(self):
      return self._doFullScan

   @property
   def monPS(self):
      return self._monPS

   @property
   def monPtMin(self):
      return self._monPtMin

   @property
   def doTRT(self):
      return self._doTRT

   @property
   def usePixelSP(self):
       return self._usePixelSP
   @property
   def d0TrackMax(self):
       return self._d0TrackMax

   @property
   def z0TrackMax(self):
       return self._z0TrackMax

   @property
   def keepTrackParameters(self):
      return self._keepTrackParameters

   @property
   def isLRT(self):
       return self._isLRT

   def printout(self):
      print( self._name, " :")
      print( "   pTmin                : ", self._pTmin )
      print( "   d0SeedMax            : ", self._d0SeedMax )
      print( "   d0SeedPPSMax         : ", self._d0SeedPPSMax )
      print( "   doZFinder            : ", self._doZFinder )
      print( "   doResMon             : ", self._doResMon )
      print( "   doSpPhiFiltering     : ", self._doSpPhiFiltering )
      print( "   doCloneRemoval       : ", self._doCloneRemoval )
      print( "   checkRedundantSeeds  : ", self._checkRedundantSeeds )
      print( "   dRdoubletMax         : ", self._dRdoubletMax )
      print( "   seedRadBinWidth      : ", self._seedRadBinWidth )
      print( "   etaHalfWidth         : ", self._etaHalfWidth )
      print( "   phiHalfWidth         : ", self._phiHalfWidth )
      print( "   doFullScan           : ", self._doFullScan )
      print( "   monPS                : ", self._monPS )
      print( "   monPtMin             : ", self._monPtMin )


#Tracking configuration for different signatures
class _Tracking_electron( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doCloneRemoval      = True #Previously False in Run2!
      self._checkRedundantSeeds = True
      self._doTRT               = True
      self._keepTrackParameters = True

class _Tracking_photon(_Settings):
    def __init__(self):
        _Settings.__init__(self)

class _Tracking_tauCore( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doTRT           = True

class _Tracking_tauIso( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth       = 0.4
      self._phiHalfWidth       = 0.4
      self._doTRT               = True

class _Tracking_tau( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._pTmin           = 0.8*GeV
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4
      self._doTRT               = True

class _Tracking_muon( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._d0SeedMax           = 10.0
      self._doResMon            = True
      self._doSpPhiFiltering    = False
      self._checkRedundantSeeds = True
      self._monPtMin            = 12*GeV
      self._doTRT               = False

class _Tracking_muonCore( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._d0SeedMax           = 10.0
      self._doSpPhiFiltering    = False
      self._checkRedundantSeeds = True
      self._doTRT               = False

class _Tracking_muonIso( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth        = 0.35
      self._phiHalfWidth        = 0.35
      self._doTRT               = False

class _Tracking_bjet( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth    = 0.2
      self._phiHalfWidth    = 0.201
      self._doTRT           = False

class _Tracking_bjetVtx(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._phiHalfWidth = 0.201
        self._etaHalfWidth = 0.2
        self._pTmin = 5.*GeV
        self._doTRT = False

class _Tracking_fullScan( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False
      self._dRdoubletMax    = 200
      self._seedRadBinWidth = 10

class _Tracking_fullScan2(_Settings):
    def __init__(self):
        self._pTmin = 2. * GeV
        self._etaHalfWidth = 3.0
        self._phiHalfWidth = 3.14159
        self._doFullScan = True

class _Tracking_fullScan500(_Settings):
    def __init__(self):
        self._pTmin = 0.5 * GeV
        self._etaHalfWidth = 3.0
        self._phiHalfWidth = 3.14159
        self._doFullScan = True

class _Tracking_minBias400( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doFullScan      = True
      self._pTmin           = 0.39*GeV
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False

class _Tracking_minBias( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doFullScan      = True
      self._pTmin           = 0.2*GeV
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False

class _Tracking_minBias2( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doFullScan      = True
      self._pTmin           = 0.5*GeV
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False

class _Tracking_beamSpot( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doFullScan      = True
      self._doZFinder       = True
      self._dRdoubletMax    = 200
      self._seedRadBinWidth = 10
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False
      self._checkRedundantSeeds = True

class _Tracking_cosmic( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._doFullScan      = True
      self._d0SeedMax       = 1000.0
      self._d0SeedPPSMax    = 1000.0
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False
      #do these settings still apply, were defined in TrigFastTrackFinder_Config.py but not in slicesettings
      self._d0TrackMax = 1000.
      self._z0TrackMax = 1000.

class _Tracking_cosmicsN(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._etaHalfWidth = 3.0
        self._phiHalfWidth = 3.14159
        self._doFullScan = True
        self._pTmin = 0.5*GeV

class _Tracking_beamgas(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._pTmin = 0.1 * GeV
        self._etaHalfWidth = 3.0
        self._phiHalfWidth = 3.14159
        self._doFullScan = True

class _Tracking_hadCalib(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._pTmin = 0.5*GeV
        self._etaHalfWidth = 0.4
        self._phiHalfWidth = 0.4

class _Tracking_bphysics( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._d0SeedMax           = 10.
      self._doSpPhiFiltering    = False
      self._etaHalfWidth        = 0.75
      self._phiHalfWidth        = 0.75
      self._checkRedundantSeeds = True
      self._doTRT               = False

class _Tracking_bphysicsHighPt( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._d0SeedMax           = 10.
      self._doSpPhiFiltering    = False
      self._etaHalfWidth        = 0.75
      self._phiHalfWidth        = 0.75
      self._doTRT               = False
      self._pTmin = 2. * GeV

class _Tracking_heavyIon(_Settings):
   def __init__( self ):
      _Settings.__init__(self)

class _Tracking_heavyIonFS(_Settings):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth=3.0
      self._phiHalfWidth=3.14159
      self._doFullScan=True

class _Tracking_lowPt(_Settings):
   def __init__( self ):
      _Settings.__init__(self)
      self._pTmin = 0.1 * GeV
      self._etaHalfWidth = 3.0
      self._phiHalfWidth = 3.14159
      self._doFullScan = True


class _Tracking_electronLRT(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._usePixelSP = False
        self._d0SeedMax = 300.
        self._d0TrackMax = 300.
        self._z0TrackMax = 500.
        self._isLRT = True

class _Tracking_muonLRT(_Settings):
    def __init__(self):
        _Settings.__init__(self)
        self._usePixelSP = False
        self._d0SeedMax = 300.
        self._d0TrackMax = 300.
        self._z0TrackMax = 500.
        self._isLRT = True

class _Tracking_tauLRT( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._pTmin           = 0.8*GeV
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4
      self._doTRT               = True
      self._usePixelSP = False
      self._d0SeedMax = 300.
      self._d0TrackMax = 300.
      self._z0TrackMax = 500.
      self._isLRT = True


class _Tracking_fullScanLRT( _Settings ):
   def __init__( self ):
      _Settings.__init__(self)
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False
      self._checkRedundantSeeds = True
      self._usePixelSP = False
      self._d0SeedMax = 300.
      self._d0TrackMax = 300.
      self._z0TrackMax = 500.
      self._d0SeedPPSMax = 300.
      self._doFullScan = True
      self._isLRT = True

class _Tracking_bjetLRT(_Settings):
    def __init__(self):
      _Settings.__init__(self)
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4
      self._doTRT           = False
      self._usePixelSP = False
      self._d0SeedMax = 300.
      self._d0TrackMax = 300.
      self._z0TrackMax = 500.
      self._isLRT = True


#Map to retrieve available configurations of Tracking
_TrackingConfigSettings = {
    "electron"     : _Tracking_electron(),
    "electronLRT"  : _Tracking_electronLRT(),

    "photon"       : _Tracking_photon(),

    #Muon signatures
    "muon"         : _Tracking_muon(),
    "muonIso"      : _Tracking_muonIso(),
    "muonCore"     : _Tracking_muonCore(),
    "muonLRT"      : _Tracking_muonLRT(),

    "tau"          : _Tracking_tau(),
    "tauCore"      : _Tracking_tauCore(),
    "tauIso"       : _Tracking_tauIso(),
    "tauLRT"       : _Tracking_tauLRT(),

    "bjet"         : _Tracking_bjet(),
    "bjetVtx"      : _Tracking_bjetVtx(),
    "bjetLRT"      : _Tracking_bjetLRT(),

    "fullScan"     : _Tracking_fullScan(),
    "fullScanLRT"  : _Tracking_fullScanLRT(),
    "fullScan2"    : _Tracking_fullScan2(),
    "fullScan500"  : _Tracking_fullScan500(),

    "minBias"      : _Tracking_minBias(),
    "minBias2"     : _Tracking_minBias2(),
    "minBias400"   : _Tracking_minBias400(),

    "beamSpot"     : _Tracking_beamSpot(),
    "cosmics"      : _Tracking_cosmic(),
    "cosmicsN"     : _Tracking_cosmicsN(),
    "beamgas"      : _Tracking_beamgas(),

    "bphysics"     : _Tracking_bphysics(),
    "bphysHighPt"  : _Tracking_bphysicsHighPt(),

    "lowPt"        : _Tracking_lowPt(),

    "hadCalib"     : _Tracking_hadCalib(),

    "heavyIon"     : _Tracking_heavyIon(),
    "heavyIonFS"   : _Tracking_heavyIonFS()

                  }


#Function that returns specific configuration of tracking cuts and flags
def getInDetTrigTrackingConfig( name ) :
   if name in _TrackingConfigSettings :
      return _TrackingConfigSettings[name]
   else :
        #       don't just return None, and do nothing as this
        #       will just hide the error until people try to use
        #       the bad slice configuration
      raise Exception( "getInDetTrigTrackingConfig() called with incorrect non existent slice: "+name )
      return None




#     Full Signature Configuration of ID Trig
#-----------------------------------------------------------------------------

class _FastTracking():
   def __init__( self, signatureType, nameSuffix ) :
      self._signatureType = signatureType #Steer which cuts,flags to load
      self._config        = getInDetTrigTrackingConfig( signatureType )
      self._suffix        = nameSuffix

   #Retrieve trackCollection key
   @makeRecordable #Allows to record collection if doRecord = True
   def trkTracksFTF(self, doRecord=False):
      return 'HLT_IDTrkTrack_{}_FTF'.format( self._suffix )

   #Retrieve TrackParticle key
   @makeRecordable #Allows to record collection if doRecord = True
   def tracksFTF(self, doRecord = True):
      return 'HLT_IDTrack_{}_FTF'.format( self._suffix )

   #Retrieve TrackParticle key
   @property
   def setting(self):
      return self._config

   def isSignature(self, signature):
      return (self._signatureType == signature)

   @property
   def signatureType(self):
      return self._signatureType
   @property
   def config(self):
      return self._config

class _PrecisionTracking():
   def __init__( self, signatureType, nameSuffix ) :
      self._signatureType = signatureType #Steer which cuts,flags to load
      self._config        = getInDetTrigTrackingConfig( signatureType ) #This might not be needed in the future as most likely the setting will be the same between FT and PT
      self._suffix        = nameSuffix

   #Retrieve final precision trackCollection
   @makeRecordable
   def trkTracksPT(self, doRecord = False):
      return 'HLT_IDTrkTrack_{}_IDTrig'.format( self._suffix )

   #Retrieve final TrackParticle key
   @makeRecordable
   def tracksPT(self, doRecord = False):
      return 'HLT_IDTrack_{}_IDTrig'.format( self._suffix )

   #Retrieve trackCollection from Ambiguity solving stage
   #AS might not be very descriptive, should we switch to AmbiSol at least?
   @makeRecordable
   def trkTracksAS(self, doRecord = False):
      return 'HLT_IDTrkTrack_{}_AmbSol'.format( self._suffix )

   #Retrieve trackCollection from TRT track extension stage
   #TE might not be very descriptive, should we switch to TRTExt at least?
   @makeRecordable
   def trkTracksTE(self, doRecord = False):
      return 'HLT_IDTrkTrack_{}_TRTExt'.format( self._suffix )

   #Retrieve TrackParticle key
   @property
   def setting(self):
      return self._config

   def isSignature(self, signature):
      return (self._signatureType == signature)


class _GlobalSettings() :
   def __init__( self ) :
      self._name      = "" #To be appended to alg names
      self._roi       = ""
      self._configFT  =  None #Fast tracking configuration
      self._configPT  =  None #Precision tracking configuration
      self._doRecord  = False #Allow recording of track collections
      self._isLRT     = False

   @property
   def FT(self):
      return self._configFT

   @property
   def PT(self):
      return self._configPT

   @property
   def RoI(self):
      return self._roi

   @property
   def name(self):
      return self._name

   @property
   def isRecordable(self):
      return self._doRecord

   @property
   def isLRT(self):
       return self._isLRT



class _Settings_electron( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "electron" #To be appended to alg names
      self._roi       = "HLT_Roi_Electron"
      self._configFT  = _FastTracking(      signatureType = 'electron', nameSuffix = 'Electron' )
      self._configPT  = _PrecisionTracking( signatureType = 'electron', nameSuffix = 'Electron' )
      self._doRecord  = True #Allow recording of track collections


class _Settings_muon( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "muon" #To be appended to alg names
      self._roi       = "HLT_Roi_Muon"
      self._configFT  = _FastTracking(      signatureType = 'muon', nameSuffix = 'Muon' )
      self._configPT  = _PrecisionTracking( signatureType = 'muon', nameSuffix = 'Muon' )
      self._doRecord  = True #Allow recording of track collections

class _Settings_muonIso( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "muonIso" #To be appended to alg names
      self._roi       = "HLT_Roi_MuonIso"
      self._configFT  = _FastTracking(      signatureType = 'muonIso', nameSuffix = 'MuonIso' )
      self._configPT  = _PrecisionTracking( signatureType = 'muonIso', nameSuffix = 'MuonIso' )
      self._doRecord  = True #Allow recording of track collections

class _Settings_muonLate( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "muonLate" #To be appended to alg names
      self._roi       = "HLT_Roi_Muon"  #FIXME
      self._configFT  = _FastTracking(      signatureType = 'muon', nameSuffix = 'MuonLate' )
      self._configPT  = _PrecisionTracking( signatureType = 'muon', nameSuffix = 'MuonLate' )
      self._doRecord  = True #Allow recording of track collections

class _Settings_muonFS( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "muonFS" #To be appended to alg names
      self._roi      = "HLT_Roi_Muon"  #FIXME
      self._configFT =   _FastTracking(      signatureType = 'muon', nameSuffix = 'MuonFS' )
      self._configPT =   _PrecisionTracking( signatureType = 'muon', nameSuffix = 'MuonFS' )
      self._doRecord = True #Allow recording of track collections

class _Settings_muonLRT( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "muonLRT" #To be appended to alg names
      self._roi       = "HLT_Roi_Muon"
      self._configFT  = _FastTracking(      signatureType = 'muonLRT', nameSuffix = 'MuonLRT' )
      self._configPT  = _PrecisionTracking( signatureType = 'muonLRT', nameSuffix = 'Muon' )
      self._doRecord  = True #Allow recording of track collections
      self._isLRT      = True

class _Settings_muonCore( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "muonCore" #To be appended to alg names
      self._roi      = "HLT_Roi_Muon"
      self._configFT = _FastTracking(      signatureType = 'muonCore', nameSuffix = 'MuonCore' )
      self._configPT = _PrecisionTracking( signatureType = 'muonCore', nameSuffix = 'MuonCore' )
      self._doRecord = True #Allow recording of track collections

class _Settings_tauTau( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauTau" #To be appended to alg names
      self._roi      = "HLT_Roi_Tau" #FIXME: possibly different!
      self._configFT =  _FastTracking(      signatureType = 'tau', nameSuffix = 'Tau' )
      #There should not be a need for tauCore PT!
      self._configPT =   _PrecisionTracking( signatureType = 'tau', nameSuffix = 'Tau' )
      self._doRecord = True #Allow recording of track collections

class _Settings_tauCore( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauCore" #To be appended to alg names
      self._roi      = "HLT_Roi_TauCore" #FIXME: possibly different!
      self._configFT =  _FastTracking(      signatureType = 'tauCore', nameSuffix = 'TauCore' )
      #There should not be a need for tauCore PT!
      #self._configPT =   #_PrecisionTracking( signatureType = 'tauCore', nameSuffix = 'TauCore' )
      self._doRecord = True #Allow recording of track collections

class _Settings_tauLRT( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauLRT" #To be appended to alg names
      self._roi      = "HLT_Roi_TauCore" #FIXME: possibly different!
      self._configFT =  _FastTracking(      signatureType = 'tauLRT', nameSuffix = 'TauLRT' )
      #There should not be a need for tauCore PT!
      #self._configPT =   #_PrecisionTracking( signatureType = 'tauCore', nameSuffix = 'TauCore' )
      self._doRecord = True #Allow recording of track collections
      self._isLRT = True

class _Settings_tauIso( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauIso" #To be appended to alg names
      self._roi      = "RoiForTauIso" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tauIso', nameSuffix = 'TauIso' )
      self._configPT = _PrecisionTracking( signatureType = 'tauIso', nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = True #Allow recording of track collections

#This might be redundant but lets keep them for the time being...
class _Settings_tauId( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauId" #To be appended to alg names
      self._roi      = "HLT_Roi_Tau" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tau', nameSuffix = 'Tau' )
      self._configPT = _PrecisionTracking( signatureType = 'tau', nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False #FIXME: Do I need to record these?

#This might be redundant but lets keep them for the time being...
class _Settings_tauEF( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauEF" #To be appended to alg names
      self._roi      = "HLT_Roi_TauIso" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tauIso', nameSuffix = 'TauIso' ) #
      self._configPT = _PrecisionTracking( signatureType = 'tau',    nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False #FIXME: Do I need to record these?

#This might be redundant but lets keep them for the time being...
class _Settings_tauTrk( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauTrk" #To be appended to alg names
      self._roi      = "HLT_Roi_Tau" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tau',    nameSuffix = 'Tau' ) #
      self._configPT = _PrecisionTracking( signatureType = 'tau',    nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False #FIXME: Do I need to record these?

#This might be redundant but lets keep them for the time being...
class _Settings_tauTrkTwo( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauTrkTwo" #To be appended to alg names
      self._roi      = "HLT_Roi_TauIso" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tauIso', nameSuffix = 'TauIso' ) #
      self._configPT = _PrecisionTracking( signatureType = 'tau',    nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False #FIXME: Do I need to record these?

class _Settings_tauIsoBDT( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "tauIsoBDT" #To be appended to alg names
      self._roi      = "HLT_Roi_TauIsoBDT" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'tauIso', nameSuffix = 'TauIso' ) #
      self._configPT = _PrecisionTracking( signatureType = 'tauIso',    nameSuffix = 'Tau' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False #FIXME: Do I need to record these?

class _Settings_bjet( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "bjet" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'bjet',   nameSuffix = 'Bjet' )
      self._configPT = _PrecisionTracking( signatureType = 'bjet',   nameSuffix = 'Bjet' )
      self._doRecord = True

class _Settings_bjetVtx( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "bjetVtx" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'bjetVtx',   nameSuffix = 'Bjet' )
      self._configPT = _PrecisionTracking( signatureType = 'bjetVtx',   nameSuffix = 'Bjet' )
      self._doRecord = False

class _Settings_bjetLRT( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "bjetLRT" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'bjetLRT',   nameSuffix = 'BjetLRT' )
      self._configPT = _PrecisionTracking( signatureType = 'bjetLRT',   nameSuffix = 'BjetLRT' )
      self._isLRT = True
      self._doRecord = True

class _Settings_jet( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "jet" #To be appended to alg names
      self._roi      = "HLT_Roi_jetFS" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'fullScan',  nameSuffix = 'FS' ) #
      self._configPT = _PrecisionTracking( signatureType = 'fullScan',  nameSuffix = 'FS' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = True

class _Settings_minBias( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "minBias" #To be appended to alg names
      self._roi      = "HLT_Roi_MinBias" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'minBias',  nameSuffix = 'MinBias' ) #
      self._configPT = _PrecisionTracking( signatureType = 'minBias',  nameSuffix = 'MinBias' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = True

class _Settings_minBias400( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "minBias" #To be appended to alg names
      self._roi      = "HLT_Roi_MinBias" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'minBias400',  nameSuffix = 'MinBias' ) #
      self._configPT = _PrecisionTracking( signatureType = 'minBias400',  nameSuffix = 'MinBias' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = True

class _Settings_minBias2( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "minBias" #To be appended to alg names
      self._roi      = "HLT_Roi_MinBias" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'minBias2',  nameSuffix = 'MinBias' ) #
      self._configPT = _PrecisionTracking( signatureType = 'minBias2',  nameSuffix = 'MinBias' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = True

class _Settings_beamSpot( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "beamSpot" #To be appended to alg names
      self._roi      = "HLT_Roi_FS" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'beamSpot',  nameSuffix = 'BeamSpot' ) #
      #PT probably isn't necessary for BS
      #self._configPT =   _PrecisionTracking( signatureType = 'beamSpot',  nameSuffix = 'BeamSpot' ) #Final collection is being renamed to just tau apparently...
      self._doRecord = False

class _Settings_fullScan( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "fullScan" #To be appended to alg names
      self._roi      = "HLT_Roi_FS" #FIXME: possibly different!
      self._configFT = _FastTracking(   signatureType = 'fullScan',  nameSuffix = 'FS' ) #
      self._doRecord = False

class _Settings_fullScan2( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "fullScan2" #To be appended to alg names
      self._roi      = "HLT_Roi_FS" #FIXME: possibly different!
      self._configFT = _FastTracking(   signatureType = 'fullScan2',  nameSuffix = 'FS' ) #
      self._doRecord = False

class _Settings_fullScan500( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "fullScan500" #To be appended to alg names
      self._roi      = "HLT_Roi_FS" #FIXME: possibly different!
      self._configFT = _FastTracking(   signatureType = 'fullScan500',  nameSuffix = 'FS' ) #
      self._doRecord = False

class _Settings_fullScanLRT( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "fullScanLrt" #To be appended to alg names
      self._roi      = "HLT_Roi_FS" #FIXME: possibly different!
      self._configFT = _FastTracking(   signatureType = 'fullScanLRT',  nameSuffix = 'FSLRT' ) #
      self._doRecord = False
      self._isLRT    = True

class _Settings_cosmics( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "cosmics" #To be appended to alg names
      self._roi      = "HLT_Roi_Cosmics" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'cosmics',  nameSuffix = 'Cosmic' ) #
      self._doRecord = False

class _Settings_cosmicsN( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "cosmicsN" #To be appended to alg names
      self._roi      = "HLT_Roi_Cosmics" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'cosmicsN',  nameSuffix = 'Cosmic' ) #
      self._doRecord = False

class _Settings_bphysics( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "bphysics" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'bphysics',   nameSuffix = 'Bjet' ) #TODO: get correct nameSuffix
      self._configPT = _PrecisionTracking( signatureType = 'bphysics',   nameSuffix = 'Bjet' )
      self._doRecord = False

class _Settings_bphysicsHighPt( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "bphysicsHighPt" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'bphysHighPt',   nameSuffix = 'Bjet' ) #TODO: Get correct nameSuffix
      self._configPT = _PrecisionTracking( signatureType = 'bphysHighPt',   nameSuffix = 'Bjet' )
      self._doRecord = False

class _Settings_bmumux( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name      = "bmumux" #To be appended to alg names
      self._roi       = "HLT_Roi_Bmumux"
      self._configFT  = _FastTracking(      signatureType = 'bphysics', nameSuffix = 'Bmumux' )
      self._configPT  = _PrecisionTracking( signatureType = 'bphysics', nameSuffix = 'Bmumux' )
      self._doRecord  = True #Allow recording of track collections

class _Settings_beamgas( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "beamgas" #To be appended to alg names
      self._roi      = "HLT_Roi_Beamgas" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'beamgas',   nameSuffix = 'beamgas' ) #TODO: Get correct nameSuffix
      self._configPT = _PrecisionTracking( signatureType = 'beamgas',   nameSuffix = 'beamgas' )
      self._doRecord = False

class _Settings_hadCalib( _GlobalSettings ):
   def __init__( self ):
      _GlobalSettings.__init__(self)
      self._name     = "hadcalib" #To be appended to alg names
      self._roi      = "HLT_Roi_hadCalib" #FIXME: possibly different!
      self._configFT = _FastTracking(      signatureType = 'hadCalib',   nameSuffix = 'beamgas' ) #TODO: Get correct nameSuffix
      self._configPT = _PrecisionTracking( signatureType = 'hadCalib',   nameSuffix = 'beamgas' )
      self._doRecord = False

_ConfigSettings = {
   "electron"     : _Settings_electron(),
    #Muon signatures
    "muon"        : _Settings_muon(),
    "muonIso"     : _Settings_muonIso(),
    "muonCore"    : _Settings_muonCore(),
    "muonFS"      : _Settings_muonFS(),
    "muonLate"    : _Settings_muonLate(),
    "muonLRT"     : _Settings_muonLRT(),

    #Tau signatures
    "tauTau"      : _Settings_tauTau(),
    "tauCore"     : _Settings_tauCore(),
    "tauIso"      : _Settings_tauIso(),
    "tauLRT"      : _Settings_tauLRT(),
    #Might be potentially removed
    "tauId"       : _Settings_tauId(),
    "tauTrk"      : _Settings_tauTrk(),
    "tauTrkTwo"   : _Settings_tauTrkTwo(),
    "tauEF"       : _Settings_tauEF(),
    "tauIsoBDT"   : _Settings_tauIsoBDT(),

    "bjet"        : _Settings_bjet(),
    "bjetVtx"        : _Settings_bjetVtx(),
    "bjetLRT"     : _Settings_bjetLRT(),
    "jet"         : _Settings_jet(),

    "fullScan"    : _Settings_fullScan(),
    "fullScan2"   : _Settings_fullScan2(),
    "fullScan500" : _Settings_fullScan500(),
    "fullScanLRT" : _Settings_fullScanLRT(),

    "beamSpot"    : _Settings_beamSpot(),
    "cosmics"     : _Settings_cosmics(),
    "minBias"     : _Settings_minBias400(),
    "minBias2"     : _Settings_minBias2(),
    "minBias0"     : _Settings_minBias(),
    "bphysics"    : _Settings_bphysics(),
    "bphysHighPt" : _Settings_bphysicsHighPt(),
    "bmumux"      : _Settings_bmumux(),

    "beamgas"     : _Settings_beamgas(),
    "hadCalib"    : _Settings_hadCalib()
                  }

#FTF Remap which eventually will be dropped once naming is aligned with signature settings
remap  = {
   #"Muon"     : "muon",
   #"MuonFS"   : "muon",
   #"MuonLate" : "muon",
   #"MuonCore" : "muonCore",
   #"MuonIso"  : "muonIso",
   #"eGamma"   : "electron",
   #"Electron" : "electron",
   #"Tau"      : "tau",
   #"TauCore"  : "tauCore",
   #"TauIso"   : "tauIso",
   #"TauId"    : "tau",
   #"TauTrk"   : "tau",
   #"TauTrkTwo": "tauIso",
   #"TauEF"    : "tauIso",
   #"Jet"      : "bjet",
   #"JetFS"    : "fullScan",
   "FS"       : "fullScan",
   #"FullScan" : "fullScan",
   "BeamSpot" : "beamSpot",
   #"Bphysics" : "bphysics",
   #"Cosmic"   : "cosmics",
   #"MinBias"  : "minBias400",
   #"minBias"  : "minBias400",
   "minBias400": "minBias",
   "tau": "tauTau"
}

def remap_type( signature ):
   if signature in remap:
      return remap[signature]
   else:
      return signature



#Function that returns specific signature setting/configuration
#FIXME: rename to InDetTrigSignatureConfig ?
def getInDetTrigConfig( name ):
#Most of the remapping has been removed, should we also replace FS & BeamSpot?
   rName = remap_type( name )

   if rName in _ConfigSettings :
      return _ConfigSettings[rName]
   else :
    #       don't just return None, and do nothing as this
    #       will just hide the error until people try to use
    #       the bad slice configuration
      raise Exception( "getInDetTrigConfig() called with incorrect non existent slice: "+rName )
      return None
