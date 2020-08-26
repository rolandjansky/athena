#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
 
__author__ = "Mark Sutton"
__doc__    = "InDetTrigConfigSettings"
__all__    = [ "getInDetTrigConfig", "printDefinedSlices" ]


# overall access for ID Trigger configuration cuts
#  
#    tau = getInDetTrigConfig("tau")
#    pTmin        = tau.pTmin()
#    etaHalfWidth = tau.etaHalfWidth()
#    phiHalfWidth = getInDetConfig("tau").phiHalfWidth()
#
# and so on
#
# take care to only use defined signature types or it
# will throw an exception 

# this dictionary will get filled after all the classes 
# are definied 
#_ConfigSettings = {} 


#This function is a wrapper around functions that return name of collections
#It checks whether the collection is specified to be recorded into EDM and if so it marks it to be recordable
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
            


#Remap which eventually will be dropped once naming is aligned with signature settings
remap  = {
    "Muon"     : "muon",
    "MuonFS"   : "muon",
    "MuonLate" : "muon",
    "MuonCore" : "muonCore",
    "MuonIso"  : "muonIso",
    "eGamma"   : "electron",
    "Electron" : "electron",
    "Tau"      : "tau",
    "TauCore"  : "tauCore",
    "TauIso"   : "tauIso",
    "Jet"      : "bjet",
    "JetFS"    : "fullScan",
    "FS"       : "fullScan",
    "bjetVtx"  : "bjetVtx",
    "FullScan" : "fullScan",
    "BeamSpot" : "beamSpot",
    "Bphysics" : "bphysics",
    "Cosmic"   : "cosmics",
    "MinBias"  : "minBias400",
    "minBias"  : "minBias400"
}


# Start using already decided naming conventions
# NB: this is only needed at the moment since the signature menu code is 
#     inconcistent in what flags they pass the ID Trigger configuration 
#     for the FTF And precision tracking. Once that is tidied up, this
#     should be removed

#def remapSuffix( signature ):
#   suffix_map = {
#      'electron':  'Electron',
#      'muon':      'Muon',
#      'muonFS':    'MuonFS',
#      'muonLate':  'MuonLate',
#      'muonIso':   'MuonIso',
#      'bjet':      'Bjet',
#      'tau':       'Tau',
#      'tauId':     'Tau',
#      'tauEF':     'Tau',
#      'tauTrk':    'Tau',
#      'tauTrkTwo': 'Tau'
#   }
#
#   if signature in suffix_map :
#      return suffix_map[ signature ]
#  
#   return signature


def getInDetTrigConfig( name, doRemap = True ) :
   
    #Use remapping if needed, eventually we should get rid of this by aligning nomenclature between signatures and ID
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import remap
    signature = name

    #doRemap is only temporary parameter as FTF and PT gets different signature naming ...
    if doRemap: 
      signature = remap[name]
    #Crash if signature is not amongst one of the selected ones
    assert(signature is not None), 'Signature name: {} is not amongst commonly used ones, please contact IDTrig group (hn-atlas-inner-detector-trigger@cern.ch) if you would like to add new one'.format(signature)

    if signature in _ConfigSettings :
        return _ConfigSettings[signature]
    else : 
        #       don't just return None, and do nothing as this 
        #       will just hide the error until people try to use 
        #       the bad slice configuration
        raise Exception( "getInDetTrigConfig() called with incorrect non existent slice: "+signature )
        return None         


def printDefinedSlices() :
    for name in _ConfigSettings :
        print(name)


from AthenaCommon.SystemOfUnits import GeV


class _Settings : 
  def __init__(self, name="") :
        if name == "" :
            self._name = "Settings"
        else :
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

        self._roiBaseName         = "HLT_Roi"
        self._tracksBaseName      = "HLT_IDTrack" # TrackParticles
        self._trkTracksBaseName   = "HLT_IDTrkTrack" # TrkTrack collection, This will eventually replace FTFBaseName
        self._suffix              = "" #Signature name suffix (e.g Electron,Muon,...) not necessarily the same as name! 
        self._trkTracksFTFBaseName= "TrigFastTrackFinder_Tracks" # Base Name of the track collection being produced by FTF, full name is appended by signature suffix, this is just temporary name and will be replaced by HLT_IDTrkTrack_Signature


        self._doTRT               = False
        self._isRecordable        = True

  def name(self) :
   return self._name

  #If signature name provided separate by underscore to be used for nomenclature
  def suffix(self):
      return '' if not self._name else '_' + self._suffix 

  def isSignature(self, signature):
      return (self._name == signature)

  def pTmin(self) :
   return self._pTmin 

  def d0SeedMax(self) :
   return self._d0SeedMax

  def d0SeedPPSMax(self) :
   return self._d0SeedPPSMax

  def doZFinder(self) :
   return self._doZFinder

  def doResMon(self) :
   return self._doResMon

  def doSpPhiFiltering(self) :
   return self._doSpPhiFiltering

  def doCloneRemoval(self) :
   return self._doCloneRemoval

  def checkRedundantSeeds(self) :
   return self._checkRedundantSeeds

  def dRdoubletMax(self) :
   return self._dRdoubletMax 

  def seedRadBinWidth(self) :
   return self._seedRadBinWidth

  def etaHalfWidth(self) :
   return self._etaHalfWidth

  def phiHalfWidth(self) :
   return self._phiHalfWidth

  def doFullScan(self) :
   return self._doFullScan

  def monPS(self) :
   return self._monPS

  def monPtMin(self) :
   return self._monPtMin

  def roiName(self) :
   #Expected: HLT_Roi_Electron
   return "{}{}".format( self._roiBaseName, self.suffix() )

  def trackCollection(self) :
   #Expected: HLT_Tracks_Electron
   return self._trackCollection


  #------------------------
  #TrkTracks getters

  #AS might not be very descriptive, should we switch to AmbiSol at least?
  @makeRecordable
  def trkTracksAS(self, doRecord = False):
   return '{}{}_AS'.format( self._trkTracksBaseName, self.suffix() )

  #TE might not be very descriptive, should we switch to TRTExt at least?
  @makeRecordable
  def trkTracksTE(self, doRecord = False):
   return '{}{}_TE'.format( self._trkTracksBaseName, self.suffix() )

  #IDTrig might not be very descriptive but it is used convention probably since Run1...
  @makeRecordable
  def trkTracksPT(self, doRecord = False):
   return '{}{}_IDTrig'.format( self._trkTracksBaseName, self.suffix() )

  @makeRecordable
  def trkTracksFTF(self, doRecord = False):
   return '{}{}'.format( self._trkTracksFTFBaseName, self.suffix() )

  #------------------------
  #Track Particle getters

  #IDTrig might not be very descriptive but it is used convention probably since Run1...
  @makeRecordable
  def tracksPT(self, doRecord = True ) :
   return '{}{}_IDTrig'.format( self._tracksBaseName, self.suffix() )

  @makeRecordable
  def tracksFTF(self, doRecord = True) :
   return '{}{}_FTF'.format( self._tracksBaseName, self.suffix() )

  def doTRT(self) :
   return self._doTRT

  def isRecordable(self) :
   return self._isRecordable



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
       print( "   roiName              : ", self._roiName ) 
       print( "   trackCollection      : ", self._trackCollection ) 


#Note:
#pTmin cuts adapted from: 
#https://gitlab.cern.ch/atlas/athena/-/blob/master/InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigSliceSettings.py#L70


# electron configuration ...

class _Settings_electron( _Settings ) : 
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "electron"
        self._suffix              = "Electron"
        self._doCloneRemoval      = False
        self._checkRedundantSeeds = True

        self._doTRT               = True


# muon configuration ...

class _Settings_muon( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "muon"
        self._suffix              = "Muon"
        self._d0SeedMax           = 10.0
        self._doResMon            = True
        self._doSpPhiFiltering    = False
        self._checkRedundantSeeds = True
        self._monPtMin            = 12*GeV

        self._doTRT               = False

class _Settings_muonCore( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "muonCore"
        self._suffix              = "MuonCore"
        self._d0SeedMax           = 10.0
        self._doSpPhiFiltering    = False
        self._checkRedundantSeeds = True

        self._doTRT               = False
        self._isRecordable        = False # Do we need core? doesn't seem to be in EDM rn


class _Settings_muonIso( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "muonIso"
        self._suffix              = "MuonIso"
        self._etaHalfWidth        = 0.35
        self._phiHalfWidth        = 0.35

        self._doTRT               = False

class _Settings_muonFS( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "muonFS"
        self._suffix              = "MuonFS"

        self._d0SeedMax           = 10.0
        self._doResMon            = True
        self._doSpPhiFiltering    = False
        self._checkRedundantSeeds = True
        self._monPtMin            = 12*GeV

        self._doTRT               = False

class _Settings_muonLate( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "muonLate"
        self._suffix              = "MuonLate"
        self._d0SeedMax           = 10.0
        self._doResMon            = True
        self._doSpPhiFiltering    = False
        self._checkRedundantSeeds = True
        self._monPtMin            = 12*GeV

        self._doTRT               = False


# bjet 

class _Settings_bjet( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "bjet"
        self._suffix          = "Bjet"
        self._etaHalfWidth    = 0.4
        self._phiHalfWidth    = 0.4

        self._doTRT               = False

class _Settings_bjetVtx( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "bjetVtx"
        self._suffi           = "BjetVtx"
        self._pTmin           = 5.*GeV
        self._etaHalfWidth    = 0.1
        self._phiHalfWidth    = 0.1

        self._doTRT               = False

# fullscan 

class _Settings_fullScan( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "fullScan"
        self._suffix          = "FS"
        self._doFullScan      = True
        self._etaHalfWidth    = 3
        self._phiHalfWidth    = 3.14159

        self._doTRT               = False

# beamspot 

class _Settings_beamSpot( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "beamSpot"
        self._suffix          = "BeamSpot"
        self._doFullScan      = True
        self._doZFinder       = True
        self._dRdoubletMax    = 200 
        self._seedRadBinWidth = 10
        self._etaHalfWidth    = 3
        self._phiHalfWidth    = 3.14159

        self._doTRT           = False
        self._isRecordable    = False


# minBias

class _Settings_minBias( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "minBias"
        self._suffix          = "MinBias"
        self._doFullScan      = True
        self._pTmin           = 0.2*GeV # TODO: double check
        self._etaHalfWidth    = 3
        self._phiHalfWidth    = 3.14159

        self._doTRT               = False

# cosmic(s) need to change this to just "cosmic" 
# singlular rahter than cosmicS plural

class _Settings_cosmic( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "cosmic"
        self._suffix          = "Cosmic"
        self._doFullScan      = True
        self._d0SeedMax       = 1000.0
        self._d0SeedPPSMax    = 1000.0
        self._etaHalfWidth    = 3
        self._phiHalfWidth    = 3.14159

        self._doTRT           = False

# bphysics

class _Settings_bphysics( _Settings ) :     
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name                = "bphysics"
        self._suffix              = "Bphys"
        self._d0SeedMax           = 10. 
        self._doSpPhiFiltering    = False
        self._etaHalfWidth        = 0.75
        self._phiHalfWidth        = 0.75
        self._checkRedundantSeeds = True

        self._doTRT               = False

# tau configuration ...

class _Settings_tauCore( _Settings ) : 
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "tauCore"
        self._suffix          = "TauCore"

        self._doTRT               = True

class _Settings_tauIso( _Settings ) : 
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "tauIso"
        self._suffix          = "TauIso"
        self._etaHalfWidth    = 0.4
        self._phiHalfWidth    = 0.4

        self._doTRT               = True

class _Settings_tau( _Settings ) : 
    def __init__( self ) : 
        _Settings.__init__(self)
        self._name            = "tau"
        self._suffix          = "Tau"
        self._pTmin           = 0.8*GeV 
        self._etaHalfWidth    = 0.4
        self._phiHalfWidth    = 0.4

        self._doTRT               = True

# overall map for use, ie 

_ConfigSettings = { 
    "muon"     : _Settings_muon(),
    "muonIso"  : _Settings_muonIso(),
    "muonCore" : _Settings_muonCore(),
    #Atm the config is very similar  (probably once more variables added they will be more distinct, but if not should we start from the same conf (muon) and change just RoI/TrackCollection Names?)
    "muonFS"   : _Settings_muonFS(),
    "muonLate" : _Settings_muonLate(),
    
    "electron"  : _Settings_electron(),
    "photon"    : _Settings(name="photon"),
    
    "tau"       : _Settings_tau(),
    "tauCore"   : _Settings_tauCore(),
    "tauIso"    : _Settings_tauIso(),
    #FIXME
    #Need to be careful here, collections below are probably unnecessary, need to make sure if that is the case and we (tau group)need to remove them 
    "tauId"     : _Settings_tau(), #Does this need separate setting?
    "tauTrk"    : _Settings_tau(), #Does this need separate setting?
    "tauTrkTwo" : _Settings_tau(), #Does this need separate setting?
    "tauEF"     : _Settings_tau(), #Does this need separate setting?

    "bjet"      : _Settings_bjet(),
    "bjetVtx"   : _Settings_bjetVtx(),


    "beamSpot"  : _Settings_beamSpot(),
    
    "cosmics"   : _Settings_cosmic(),
    "fullScan"  : _Settings_fullScan(),
    "minBias400": _Settings_minBias(),
    "bphysics"  : _Settings_bphysics() }
