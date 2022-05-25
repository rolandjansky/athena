#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__author__ = "Mark Sutton, Matous Vozak"
__doc__    = "ConfigSettingsBase"




from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.SystemOfUnits import GeV

class _ConfigSettingsBase() :
   def __init__( self ) :
     # default values
      self._input_name          = None
      self._name                = None
      self._suffix              = None
      self._pTmin               = 1.*GeV
      self._newConfig           = True
      self._TripletDoPPS        = True
      self._Triplet_D0Max       = 4.0
      self._Triplet_D0_PPS_Max  = 1.7
      self._DoPhiFiltering      = True
      self._doZFinder           = False
      self._doZFinderOnly       = False
      self._doResMon            = False
      self._doCloneRemoval      = True
      self._doSeedRedundancyCheck = False
      self._DoubletDR_Max       = 270
      self._SeedRadBinWidth     = 2
      self._holeSearch_FTF      = False
      self._electronPID         = False
      self._etaHalfWidth        = 0.1
      self._phiHalfWidth        = 0.1
      self._zedHalfWidth        = -999 # don't set this parameter unless it is >= 0
      self._doFullScan          = False
      self._monPS               = 1
      self._monPtMin            = 1*GeV
      self._doTRT               = True
      self._keepTrackParameters = False # Keep track parameters in conversion to TrackParticles
      self._UsePixelSpacePoints = True
      self._TrackInitialD0Max   = 20.0
      self._TrackZ0Max          = 300.0
      self._isLRT               = False
      self._UseTrigSeedML       = None
      self._RoadWidth           = 10
      self._nClustersMin        = None
      self._roi                 = None
      self._isLRT               = False
      self._LRTD0Min            = None
      self._LRTHardPtMin        = None
      self._doRecord            = True
      self._vertex              = None
      self._adaptiveVertex      = False
      self._actsVertex          = False
      self._addSingleTrackVertices = False
      self._TracksMaxZinterval  = 1 #mm
      self._minNSiHits_vtx      = None
      self._vertex_jet          = None
      self._adaptiveVertex_jet  = False
      self._actsVertex_jet      = False
      self._dodEdxTrk           = False 
      self._doHitDV             = False 
      self._doDisappearingTrk   = False
      self._usePixelNN          = False
      self._useDynamicRoiZWidth = False


      #precision tracking configuration values
      self._maxRPhiImpactPT   = None
      self._maxZImpactPT      = None
      self._maxEtaPT          = None
      self._minSiClustersPT   = None
      self._maxSiHolesPT      = None
      self._maxPixelHolesPT   = None
      self._maxSCTHolesPT    = None
      self._maxDoubleHolesPT  = None
      self._usePixelPT        = None
      self._useSCTPT          = None
      self._doEmCaloSeedPT    = None
      self._minTRTonTrkPT     = None
      self._useSiSPSeededTrackFinder = False

      if hasattr(self.__class__, 'override') and callable(getattr(self.__class__, 'override')) :
         self.override()

   # assign to this override method to add additioal global functionality 
   # to the base class, such as to globally override any of the 
   # variables above
   # def override(self):   
   #      pass

   def tracks_FTF(self):
      if not self._suffix:
         raise Exception( "ID Trigger configuration:  called with non existent slice: ", self._name, self._input_name  )
      if self._doRecord:
         return recordable('HLT_IDTrack_{}_FTF'.format( self._suffix ))
      else:
         return 'HLT_IDTrack_{}_FTF'.format( self._suffix )

   def tracks_IDTrig(self):
      if self._doRecord:
         return recordable('HLT_IDTrack_{}_IDTrig'.format( self._suffix ))
      else:
         return 'HLT_IDTrack_{}_IDTrig'.format( self._suffix )


   def trkTracks_FTF(self):
         return 'HLT_IDTrkTrack_{}_FTF'.format( self._suffix )

   def trkTracks_IDTrig(self):
         return 'HLT_IDTrkTrack_{}_IDTrig'.format( self._suffix )

   def tracks_EFID(self):
      if self._doRecord:
         return recordable('HLT_IDTrack_{}_EFID'.format( self._suffix ))
      else:
         return 'HLT_IDTrack_{}_EFID'.format( self._suffix )

   def trkTracks_EFID(self):
      return 'HLT_IDTrkTrack_{}_EFID'.format( self._suffix )


   @property
   def name(self):
      return self._name

   @property
   def input_name(self):
      return self._input_name

   @property
   def suffix(self):
      return self._suffix

   @property
   def pTmin(self):
      return self._pTmin

   @property
   def newConfig(self):
      return self._newConfig

   @property
   def TripletDoPPS(self):
      return self._TripletDoPPS

   @property
   def Triplet_D0Max(self):
      return self._Triplet_D0Max

   @property
   def Triplet_D0_PPS_Max(self):
      return self._Triplet_D0_PPS_Max

   @property
   def DoPhiFiltering(self):
      return self._DoPhiFiltering

   @property
   def doZFinder(self):
      return self._doZFinder

   @property
   def doZFinderOnly(self):
      return self._doZFinderOnly

   @property
   def doResMon(self):
      return self._doResMon

   @property
   def electronPID(self):
      return self._electronPID

   @property
   def doCloneRemoval(self):
      return self._doCloneRemoval

   @property
   def doSeedRedundancyCheck(self):
      return self._doSeedRedundancyCheck

   @property
   def DoubletDR_Max(self):
      return self._DoubletDR_Max

   @property
   def SeedRadBinWidth(self):
      return self._SeedRadBinWidth

   @property
   def etaHalfWidth(self):
      return self._etaHalfWidth

   @property
   def phiHalfWidth(self):
      return self._phiHalfWidth

   @property
   def zedHalfWidth(self):
      return self._zedHalfWidth

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

   @property
   def UsePixelSpacePoints(self):
       return self._UsePixelSpacePoints

   @property
   def TrackInitialD0Max(self):
       return self._TrackInitialD0Max

   @property
   def holeSearch_FTF(self):
       return self._holeSearch_FTF

   @property
   def TrackZ0Max(self):
       return self._TrackZ0Max

   @property
   def isLRT(self):
       return self._isLRT

   @property
   def LRT_D0Min(self):
       return self._LRTD0Min

   @property
   def LRT_HardMinPt(self):
       return self._LRTHardPtMin

   @property
   def roi(self):
      return self._roi

   @property
   def UseTrigSeedML(self):
      return self._UseTrigSeedML

   @property
   def RoadWidth(self):
      return self._RoadWidth

   @property
   def nClustersMin(self):
      return self._nClustersMin

   @property
   def isRecordable(self):
      return self._doRecord

   @property
   def doRecord(self):
      return self._doRecord

   @property
   def vertex(self):
      if not self._vertex:
         raise Exception( "ID Trigger configuration: vertex not defined for slice: ", self._name, self._input_name  )
      if self._doRecord:
         return recordable(self._vertex)
      else:
         return self._vertex

   @property
   def vertex_jet(self):
      if not self._vertex_jet:
         raise Exception( "ID Trigger configuration: vertex_jet not defined for slice: ", self._name, self._input_name  )
      if self._doRecord:
         return recordable(self._vertex_jet)
      else:
         return self._vertex_jet

   @property
   def adaptiveVertex(self):
       return self._adaptiveVertex

   @property
   def adaptiveVertex_jet(self):
       return self._adaptiveVertex_jet

   @property
   def actsVertex(self):
       return self._actsVertex

   @property
   def actsVertex_jet(self):
       return self._actsVertex_jet

   @property
   def addSingleTrackVertices(self):
       return self._addSingleTrackVertices

   @property
   def minNSiHits_vtx(self):
       return self._minNSiHits_vtx

   @property
   def TracksMaxZinterval(self):
      return self._TracksMaxZinterval

   @property
   def dodEdxTrk(self):
       return self._dodEdxTrk

   @property
   def doHitDV(self):
       return self._doHitDV

   @property
   def doDisappearingTrk(self):
       return self._doDisappearingTrk

   @property
   def usePixelNN(self):
       return self._usePixelNN

   @property
   def maxRPhiImpact(self):
      return self._maxRPhiImpactPT

   @property
   def maxZImpact(self):
      return self._maxZImpactPT

   @property
   def maxEta(self):
      return self._maxEtaPT

   @property
   def minSiClusters(self):
      return self._minSiClustersPT

   @property
   def maxSiHoles(self):
      return self._maxSiHolesPT

   @property
   def maxPixelHoles(self):
      return self._maxPixelHolesPT

   @property
   def maxSCTHoles(self):
      return self._maxSCTHolesPT

   @property
   def maxDoubleHoles(self):
      return self._maxDoubleHolesPT

   @property
   def usePixel(self):
      return self._usePixelPT
   
   @property
   def useSCT(self):
      return self._useSCTPT
   
   @property
   def doEmCaloSeed(self):
      return self._doEmCaloSeedPT

   @property
   def minTRTonTrk(self):
      return self._minTRTonTrkPT

   @property
   def useDynamicRoiZWidth(self):
      return self._useDynamicRoiZWidth

   @property
   def useSiSPSeededTrackFinder(self):
      return self._useSiSPSeededTrackFinder

   def printout(self):
      from AthenaCommon.Logging import logging
      log = logging.getLogger("InDetTrigConfig: ")

      log.info( " %s :", self._name )
      log.info( " %s :", self._input_name )
      log.info( "   pTmin                 : %s", self._pTmin )
      log.info( "   TripletDoPPS          : %s", self._TripletDoPPS )
      log.info( "   Triplet_D0Max         : %s", self._Triplet_D0Max )
      log.info( "   Triplet_D0_PPS_Max    : %s", self._Triplet_D0_PPS_Max )
      log.info( "   doZFinder             : %s", self._doZFinder )
      log.info( "   doResMon              : %s", self._doResMon )
      log.info( "   DoPhiFiltering        : %s", self._DoPhiFiltering )
      log.info( "   doCloneRemoval        : %s", self._doCloneRemoval )
      log.info( "   doSeedRedundancyCheck : %s", self._doSeedRedundancyCheck )
      log.info( "   DoubletDR_Max         : %s", self._DoubletDR_Max )
      log.info( "   SeedRadBinWidth       : %s", self._SeedRadBinWidth )
      log.info( "   etaHalfWidth          : %s", self._etaHalfWidth )
      log.info( "   phiHalfWidth          : %s", self._phiHalfWidth )
      log.info( "   doFullScan            : %s", self._doFullScan )
      log.info( "   monPS                 : %s", self._monPS )
      log.info( "   monPtMin              : %s", self._monPtMin )
      log.info( "   doTRT                 : %s", self._doTRT )
      log.info( "   keepTrackParameters   : %s", self._keepTrackParameters )
      log.info( "   UsePixelSpacePoints   : %s", self._UsePixelSpacePoints )
      log.info( "   TrackInitialD0Max     : %s", self._TrackInitialD0Max )
      log.info( "   TrackZ0Max            : %s", self._TrackZ0Max )
      log.info( "   adaptiveVertex        : %s", self._adaptiveVertex )
      log.info( "   isLRT                 : %s", self._isLRT )
      log.info( "   LRTD0Min              : %s", self._LRTD0Min )
      log.info( "   LRTHardPtmin          : %s", self._LRTHardPtMin )
      log.info( "   doHitDV               : %s", self._doHitDV )
      log.info( "   nClustersMin          : %s", self._nClustersMin )
      log.info( "   suffix                : %s", self._suffix )
      log.info( "   roi                   : %s", self._roi )
      log.info( "   addSingleTrackVertices: %s", self._addSingleTrackVertices )
      log.info( "   maxRPhiImpact         : %s", self._maxRPhiImpactPT )
      log.info( "   maxZImpact            : %s", self._maxZImpactPT )
      log.info( "   maxEta                : %s", self._maxEtaPT )
      log.info( "   maxSiHoles            : %s", self._maxSiHolesPT )
      log.info( "   maxPixelHoles         : %s", self._maxPixelHolesPT )
      log.info( "   maxSCTHoles           : %s", self._maxSCTHolesPT )
      log.info( "   maxDoubleHoles        : %s", self._maxDoubleHolesPT )
      log.info( "   usePixel              : %s", self._usePixelPT )
      log.info( "   useSCT                : %s", self._useSCTPT )
      log.info( "   doEmCaloSeed          : %s", self._doEmCaloSeedPT )
      log.info( "   minTRTonTrk           : %s", self._minTRTonTrkPT )
      log.info( "   useDynamicRoiZWidth   : %s", self._useDynamicRoiZWidth )
      log.info( "   useSiSPSeededTrackFinder : %s", self._useSiSPSeededTrackFinder )

if __name__=='__main__':
   config = _ConfigSettingsBase()
   config.printout()
