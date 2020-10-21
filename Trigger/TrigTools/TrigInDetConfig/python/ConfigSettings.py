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
   def keepTrackParameters(self):
      return self._keepTrackParameters
   
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
      self._etaHalfWidth    = 0.4
      self._phiHalfWidth    = 0.4
      self._doTRT           = False

class _Tracking_fullScan( _Settings ):     
   def __init__( self ): 
      _Settings.__init__(self)
      self._etaHalfWidth    = 3.
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False

class _Tracking_minBias( _Settings ):     
   def __init__( self ): 
      _Settings.__init__(self)
      self._doFullScan      = True
      self._pTmin           = 0.2*GeV # TODO: double check
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

class _Tracking_cosmic( _Settings ):     
   def __init__( self ): 
      _Settings.__init__(self)
      self._doFullScan      = True
      self._d0SeedMax       = 1000.0
      self._d0SeedPPSMax    = 1000.0
      self._etaHalfWidth    = 3
      self._phiHalfWidth    = 3.14159
      self._doTRT           = False

class _Tracking_bphysics( _Settings ):     
   def __init__( self ): 
      _Settings.__init__(self)
      self._d0SeedMax           = 10. 
      self._doSpPhiFiltering    = False
      self._etaHalfWidth        = 0.75
      self._phiHalfWidth        = 0.75
      self._checkRedundantSeeds = True
      self._doTRT               = False


#Map to retrieve available configurations of Tracking
_TrackingConfigSettings = { 
    "electron"     : _Tracking_electron(),

    #Muon signatures
    "muon"         : _Tracking_muon(),
    "muonIso"      : _Tracking_muonIso(),
    "muonCore"     : _Tracking_muonCore(),

    "tau"          : _Tracking_tau(), 
    "tauCore"      : _Tracking_tauCore(),
    "tauIso"       : _Tracking_tauIso(),

    "bjet"         : _Tracking_bjet(),
    "fullScan"     : _Tracking_fullScan(),

    "minBias400"   : _Tracking_minBias(),
    "beamSpot"     : _Tracking_beamSpot(),
    "cosmics"      : _Tracking_cosmic(),
    "bphysics"     : _Tracking_bphysics(), 

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

class _Settings_muonCore( _GlobalSettings ): 
   def __init__( self ): 
      _GlobalSettings.__init__(self)
      self._name     = "muonCore" #To be appended to alg names
      self._roi      = "HLT_Roi_Muon" 
      self._configFT = _FastTracking(      signatureType = 'muonCore', nameSuffix = 'MuonCore' )
      self._configPT = _PrecisionTracking( signatureType = 'muonCore', nameSuffix = 'MuonCore' )
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

class _Settings_tauIso( _GlobalSettings ): 
   def __init__( self ): 
      _GlobalSettings.__init__(self)
      self._name     = "tauIso" #To be appended to alg names
      self._roi      = "HLT_Roi_TauIso" #FIXME: possibly different!  
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

class _Settings_bjet( _GlobalSettings ): 
   def __init__( self ): 
      _GlobalSettings.__init__(self)
      self._name     = "bjet" #To be appended to alg names
      self._roi      = "HLT_Roi_Bjet" #FIXME: possibly different!  
      self._configFT = _FastTracking(      signatureType = 'bjet',   nameSuffix = 'Bjet' ) 
      self._configPT = _PrecisionTracking( signatureType = 'bjet',   nameSuffix = 'Bjet' ) 
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
      self._configFT = _FastTracking(      signatureType = 'minBias400',  nameSuffix = 'MinBias' ) #
      self._configPT = _PrecisionTracking( signatureType = 'minBias400',  nameSuffix = 'MinBias' ) #Final collection is being renamed to just tau apparently...
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

class _Settings_cosmics( _GlobalSettings ): 
   def __init__( self ): 
      _GlobalSettings.__init__(self)
      self._name     = "cosmics" #To be appended to alg names
      self._roi      = "HLT_Roi_Cosmics" #FIXME: possibly different!  
      self._configFT = _FastTracking(      signatureType = 'cosmics',  nameSuffix = 'Cosmic' ) #
      self._doRecord = False 


_ConfigSettings = { 
   "electron"     : _Settings_electron(),
    #Muon signatures
    "muon"        : _Settings_muon(),
    "muonIso"     : _Settings_muonIso(),
    "muonCore"    : _Settings_muonCore(),
    "muonFS"      : _Settings_muonFS(),
    "muonLate"    : _Settings_muonLate(),
   
    #Tau signatures
    "tauCore"     : _Settings_tauCore(),
    "tauIso"      : _Settings_tauIso(),
    #Might be potentially removed
    "tauId"       : _Settings_tauId(),     
    "tauTrk"      : _Settings_tauTrk(),    
    "tauTrkTwo"   : _Settings_tauTrkTwo(), 
    "tauEF"       : _Settings_tauEF(),     
   
    "bjet"        : _Settings_bjet(),
    "jet"         : _Settings_jet(),
   
    "fullScan"    : _Settings_fullScan(),
   
    "beamSpot"    : _Settings_beamSpot(),
    "cosmics"     : _Settings_cosmics(),
    "minBias"     : _Settings_minBias(),
    #"bphysics"    : _Settings_bphysics(), 
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
   #"bjetVtx"  : "bjetVtx",
   #"FullScan" : "fullScan",
   "BeamSpot" : "beamSpot",
   #"Bphysics" : "bphysics",
   #"Cosmic"   : "cosmics",
   #"MinBias"  : "minBias400",
   #"minBias"  : "minBias400"
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


