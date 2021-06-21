#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
      self._newConfig           = False
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
      self._addSingleTrackVertices = False
      self._TracksMaxZinterval  = 1 #mm
      self._minNSiHits_vtx      = None
      self._vertex_jet          = None
      self._adaptiveVertex_jet  = False
      self._dodEdxTrk           = False

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

   def printout(self):
      from AthenaCommon.Logging import logging
      log = logging.getLogger("InDetTrigConfig: ")

      log.info( " {} :".format( self._name ) )                               # noqa: ATL901
      log.info( " {} :".format( self._input_name ) )                         # noqa: ATL901
      log.info( "   pTmin                 : {}".format( self._pTmin ) )                 # noqa: ATL901
      log.info( "   TripletDoPPS          : {}".format( self._TripletDoPPS ) )          # noqa: ATL901
      log.info( "   Triplet_D0Max         : {}".format( self._Triplet_D0Max ) )         # noqa: ATL901
      log.info( "   Triplet_D0_PPS_Max    : {}".format( self._Triplet_D0_PPS_Max ) )    # noqa: ATL901
      log.info( "   doZFinder             : {}".format( self._doZFinder ) )             # noqa: ATL901
      log.info( "   doResMon              : {}".format( self._doResMon ) )              # noqa: ATL901
      log.info( "   DoPhiFiltering        : {}".format( self._DoPhiFiltering ) )        # noqa: ATL901
      log.info( "   doCloneRemoval        : {}".format( self._doCloneRemoval ) )        # noqa: ATL901
      log.info( "   doSeedRedundancyCheck : {}".format( self._doSeedRedundancyCheck ) ) # noqa: ATL901
      log.info( "   DoubletDR_Max         : {}".format( self._DoubletDR_Max ) )         # noqa: ATL901
      log.info( "   SeedRadBinWidth       : {}".format( self._SeedRadBinWidth ) )       # noqa: ATL901
      log.info( "   etaHalfWidth          : {}".format( self._etaHalfWidth ) )          # noqa: ATL901
      log.info( "   phiHalfWidth          : {}".format( self._phiHalfWidth ) )          # noqa: ATL901
      log.info( "   doFullScan            : {}".format( self._doFullScan ) )            # noqa: ATL901
      log.info( "   monPS                 : {}".format( self._monPS ) )                 # noqa: ATL901
      log.info( "   monPtMin              : {}".format( self._monPtMin ) )              # noqa: ATL901
      log.info( "   doTRT                 : {}".format( self._doTRT ) )
      log.info( "   keepTrackParameters   : {}".format( self._keepTrackParameters ) )
      log.info( "   UsePixelSpacePoints   : {}".format( self._UsePixelSpacePoints ) )
      log.info( "   TrackInitialD0Max     : {}".format( self._TrackInitialD0Max ) )
      log.info( "   TrackZ0Max            : {}".format( self._TrackZ0Max ) )
      log.info( "   adaptiveVertex        : {}".format( self._adaptiveVertex ) )
      log.info( "   isLRT                 : {}".format( self._isLRT ) )
      log.info( "   LRTD0Min              : {}".format( self._LRTD0Min ) )
      log.info( "   LRTHardPtmin          : {}".format( self._LRTHardPtMin ) )
      log.info( "   doJseedHitDV          : {}".format( self._doJseedHitDV ) )
      log.info( "   nClustersMin          : {}".format( self._nClustersMin ) )
      log.info( "   useBremModel          : {}".format( self._useBremModel ) )
      log.info( "   suffix                : {}".format( self._suffix ) )
      log.info( "   record                : {}".format( self._record ) )
      log.info( "   Roi                   : {}".format( self._Roi ) )
      log.info( "   addSingleTrackVertices: {}".format( self._addSingleTrackVertices ) )
