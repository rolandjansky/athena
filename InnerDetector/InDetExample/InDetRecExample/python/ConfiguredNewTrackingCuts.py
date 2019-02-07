# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#########################################################################
# ConfiguredNewtrackingCuts class
#########################################################################
# Python Setup Class for NewTracking reconstruction
#
# Author: Markus Elsing
#
#########################################################################

import AthenaCommon.SystemOfUnits as Units
class ConfiguredNewTrackingCuts :

  def __init__ (self, mode = "Offline"):

    self.__mode      = mode
    self.__extension = ""
    self.__set_indetflags()     #pointer to InDetFlags, don't use them directly
                                #to allow sharing this code with the trigger

    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.BeamFlags import jobproperties
    from RecExConfig.RecFlags import rec
    
    # --- put defaults to run Pixel/SCT/TRT
    self.__usePixel = DetFlags.haveRIO.pixel_on()
    self.__useSCT   = DetFlags.haveRIO.SCT_on()
    self.__useTRT   = DetFlags.haveRIO.TRT_on()
    self.__useSCTSeeding = True

    # --------------------------------------
    # --- NEW TRACKING cuts
    # --------------------------------------
    #
    # --- start with is cutLevel() == 1, defaults for 2010 data
    self.__minPT                   = 0.1 * Units.GeV
    self.__minSecondaryPt          = 0.4 * Units.GeV  # Pt cut for back tracking + segment finding for these
    self.__minTRTonlyPt            = 0.4 * Units.GeV  # Pt cut for TRT only

    # --- first set kinematic defaults 
    self.__maxPT                   = None            # off !
    self.__minEta                  = -1              # off !
    self.__maxEta                  = 2.7

    # --- cluster cuts
    self.__minClusters             = 7                # Igor 6, was 7
    self.__minSiNotShared          = 6
    self.__maxShared               = 1                # cut is now on number of shared modules
    self.__minPixel                = 0                
    self.__maxHoles                = 3                # was 5
    self.__maxPixelHoles           = 2                # was 5
    self.__maxSctHoles             = 2                # was 5
    self.__maxDoubleHoles          = 1                # was 2
    self.__maxPrimaryImpact        = 10.0 * Units.mm  # low lumi
    self.__maxZImpact              = 320. * Units.mm  # Was 250 mm

    # --- this is for the TRT-extension
    self.__minTRTonTrk             = 9
    self.__minTRTPrecFrac          = 0.3

    # --- general pattern cuts for NewTracking
    self.__radMax                  = 600. * Units.mm # default R cut for SP in SiSpacePointsSeedMaker
    self.__roadWidth               = 20.
    self.__nHolesMax               = self.__maxHoles
    self.__nHolesGapMax            = self.__maxHoles # not as tight as 2*maxDoubleHoles
    self.__Xi2max                  = 15.0
    self.__Xi2maxNoAdd             = 35.0
    self.__nWeightedClustersMin    = 6

    # --- seeding 
    self.__seedFilterLevel         = 1
    self.__maxdImpactPPSSeeds      = 1.7
    self.__maxdImpactSSSSeeds      = 1000.0

    # --- min pt cut for brem
    self.__minPTBrem               = 1. * Units.GeV # off
    self.__phiWidthBrem            = 0.3 # default is 0.3
    self.__etaWidthBrem            = 0.2 # default is 0.3


    # --- Z Boundary Seeding
    self.__doZBoundary             = False
    
    # --------------------------------------
    # --- BACK TRACKING cuts
    # --------------------------------------
    
    # --- settings for segment finder 
    self.__TRTSegFinderPtBins        = 70
    self.__maxSegTRTShared           = 0.7
    self.__excludeUsedTRToutliers    = False

    # --- triggers SegmentFinder and BackTracking
    self.__useParameterizedTRTCuts   = False
    self.__useNewParameterizationTRT = False
    self.__maxSecondaryTRTShared     = 0.7

    # --- defaults for secondary tracking
    self.__maxSecondaryImpact        = 100.0 * Units.mm # low lumi
    self.__minSecondaryClusters      = 4
    self.__minSecondarySiNotShared   = 4
    self.__maxSecondaryShared        = 1   # cut is now on number of shared modules
    self.__minSecondaryTRTonTrk      = 10
    self.__minSecondaryTRTPrecFrac   = 0.
    self.__maxSecondaryHoles         = 2
    self.__maxSecondaryPixelHoles    = 2
    self.__maxSecondarySCTHoles      = 2
    self.__maxSecondaryDoubleHoles   = 1
    self.__SecondarynHolesMax        = self.__maxSecondaryHoles
    self.__SecondarynHolesGapMax     = self.__maxSecondaryHoles

    self.__rejectShortExtensions     = False # extension finder in back tracking
    self.__SiExtensionCuts           = False # cut in Si Extensions before fit

    # --- pattern cuts for back tracking
    self.__SecondaryXi2max           = 15.0
    self.__SecondaryXi2maxNoAdd      = 50.0

    # --- run back tracking and TRT only in RoI seed regions
    self.__RoISeededBackTracking     = False

    # --------------------------------------
    # --- TRT Only TRACKING cuts
    # --------------------------------------
    
    # --- TRT only
    self.__minTRTonly                = 15
    self.__maxTRTonlyShared          = 0.7
    self.__useTRTonlyParamCuts       = False
    self.__useTRTonlyOldLogic        = True
    
    #
    # --------------------------------------
    # --- now start tighening cuts level by level
    # --------------------------------------
    #
    if self.__indetflags.cutLevel() >= 2:
      # --- cutLevel() == 3, defaults for 2011 first processing
      self.__minPT                     = 0.4 * Units.GeV
      self.__minSecondaryPt            = 1.0 * Units.GeV  # Pt cut for back tracking + segment finding for these
      self.__minTRTonlyPt              = 1.0 * Units.GeV  # Pt cut for TRT only
      self.__TRTSegFinderPtBins        = 50

    if self.__indetflags.cutLevel() >= 3:
      # --- cutLevel() == 3, defaults for 2011 reprocessing
      self.__useParameterizedTRTCuts   = True # toggles BackTracking and TRT only
      self.__useNewParameterizationTRT = True

    if self.__indetflags.cutLevel() >= 4:
      # --- PUTF cuts
      self.__maxdImpactPPSSeeds        = 2.0     # loosen cut on PPS seeds
      self.__maxdImpactSSSSeeds        = 20.0    # apply cut on SSS seeds

    if self.__indetflags.cutLevel() >= 5:
      # --- PUTF cuts
      self.__seedFilterLevel           = 2       # increased seed filter level

    if self.__indetflags.cutLevel() >= 6:
      # --- stop using TRT outlies from failed extension fits to create BackTracks or TRT Only
      self.__excludeUsedTRToutliers    = True             # TRT outliers are added to the exclusion list
      # --- TRT only cuts
      self.__minTRTonlyPt              = 2.0 * Units.GeV  # increase Pt cut for TRT only to the value of egamma for 1 track conversions
      self.__useTRTonlyOldLogic        = False            # turn off ole overlap logic to reduce number of hits
      # self.__maxTRTonlyShared          = 0.2              # reduce number of shared hits

    if self.__indetflags.cutLevel() >= 7:
      # --- more BackTracking cuts
      self.__minSecondaryTRTonTrk      = 15               # let's not allow for short overlap tracks
      self.__maxSecondaryHoles         = 1                # tighten hole cuts 
      self.__maxSecondaryPixelHoles    = 1                # tighten hole cuts 
      self.__maxSecondarySCTHoles      = 1                # tighten hole cuts 
      self.__maxSecondaryDoubleHoles   = 0                # tighten hole cuts 
      self.__minSecondaryTRTPrecFrac   = 0.3              # default for all tracking now, as well for BackTracking
      self.__rejectShortExtensions     = True             # fall back onto segment if TRT extension is short
      self.__SiExtensionCuts           = True             # use cuts from ambi scoring already early
      # self.__maxSecondaryTRTShared     = 0.2              # tighen shared hit cut for segment maker ?

    if self.__indetflags.cutLevel() >= 8:
      # --- slightly tighen NewTracking cuts
      self.__maxHoles                  = 2                # was 3
      self.__maxPixelHoles             = 1                # was 2
    

    if self.__indetflags.cutLevel() >= 9:
      # --- tighten maxZ for the IP parameter
      self.__maxZImpact              = 250 * Units.mm 

    if self.__indetflags.cutLevel() >= 10:
      # --- turn on Z Boundary seeding
      self.__doZBoundary              = True

    if self.__indetflags.cutLevel() >= 11:
      # --- turn on eta dependent cuts
      self.__useTRTonlyParamCuts      = True

    if self.__indetflags.cutLevel() >= 12:
      # --- Tighten track reconstruction criteria
      self.__Xi2max                  = 9.0  # was 15 
      self.__Xi2maxNoAdd             = 25.0 # was 35
      self.__nHolesMax               = 2 # was 3
      self.__nHolesGapMax            = 2 # was 3

    if self.__indetflags.cutLevel() >= 13 and DetFlags.detdescr.Calo_allOn():
      # --- turn on RoI seeded for Back Tracking and TRT only
      self.__RoISeededBackTracking   = True

    if self.__indetflags.cutLevel() >= 14 :
      self.__minPT                   = 0.5 * Units.GeV

    if self.__indetflags.cutLevel() >= 15 :
      self.__minClusters             = 8 #based on studies by R.Jansky     

    if self.__indetflags.cutLevel() >= 16:
      print '--------> FATAL ERROR, cut level undefined, abort !'
      import sys
      sys.exit()
    
    # --------------------------------------
    # --- now the overwrites for special setups
    # --------------------------------------
    
    # --- do robust reconstruction

    if self.__indetflags.doRobustReco():
      # ---- new tracking
      self.__minClusters             = 7                # Igor 6, was 7
      self.__maxHoles                = 5                # was 5
      self.__maxPixelHoles           = 2                # was 5
      self.__maxSctHoles             = 5                # was 5
      self.__maxDoubleHoles          = 4                # was 2
      self.__maxZImpact              = 500.0 * Units.mm
      # ---- back tracking
      self.__maxSecondaryHoles       = 5
      self.__maxSecondaryPixelHoles  = 5
      self.__maxSecondarySCTHoles    = 5
      self.__maxSecondaryDoubleHoles = 2

    if self.__indetflags.doInnerDetectorCommissioning():
      self.__minClusters             = 6
      self.__nWeightedClustersMin    = 6
      self.__minSiNotShared   = 5
      self.__rejectShortExtensions = False


#    if rec.Commissioning():
#      self.__minClusters             = 7                # Igor 6, was 7
#      self.__maxHoles                = 5                # was 5
#      self.__maxPixelHoles           = 2                # was 5
#      self.__maxSctHoles             = 5                # was 5
#      self.__maxDoubleHoles          = 4                # was 2
#      self.__maxPrimaryImpact        = 50.0  * Units.mm # low lumi
#      self.__maxZImpact              = 500.0 * Units.mm

#    if rec.Commissioning():
#      self.__maxSecondaryHoles       = 5
#      self.__maxSecondaryPixelHoles  = 5
#      self.__maxSecondarySCTHoles    = 5
#      self.__maxSecondaryDoubleHoles = 2
#      self.__SecondaryXi2max         = 50.0 
#      self.__SecondaryXi2maxNoAdd    = 100.0 

    # --- SLHC setup
    if mode == "SLHC":
      self.__extension               = "SLHC"
      if self.__indetflags.useEtaDependentCuts():
        self.__maxEta                  = 4.0
        self.__etaBins                 = [-1.0, 2.0, 2.6, 4.0]
        self.__minPT                   = [0.9 * Units.GeV, 0.4 * Units.GeV, 0.4 * Units.GeV]
     
        # --- cluster cuts
        self.__minClusters             = [9, 8, 7]
        self.__minSiNotShared          = [7, 6, 5]
        self.__maxShared               = [2]
        self.__minPixel                = [1]
        self.__maxHoles                = [2]
        self.__maxPixelHoles           = [2]
        self.__maxSctHoles             = [2]
        self.__maxDoubleHoles          = [1]
        self.__maxPrimaryImpact        = [2.0 * Units.mm, 2.0 * Units.mm, 10.0 * Units.mm]
        self.__maxZImpact              = [200.0 * Units.mm]
     
        # --- general pattern cuts for NewTracking
        self.__nHolesMax               = self.__maxHoles
        self.__nHolesGapMax            = self.__maxHoles
        self.__Xi2max                  = [9.0]
        self.__Xi2maxNoAdd             = [25.0]
        self.__nWeightedClustersMin    = [6]
     
        # --- seeding 
        self.__maxdImpactSSSSeeds       = [20.0 * Units.mm]
     
        # --- min pt cut for brem
        self.__minPTBrem                = [1000.0 * Units.mm]
        self.__phiWidthBrem             = [0.3]
        self.__etaWidthBrem             = [0.2]  
        
      else:
        
        # --- higher pt cut and impact parameter cut
        self.__minPT                   = 0.9 * Units.GeV      
        self.__maxPrimaryImpact        = 2.0 * Units.mm # highlumi
        self.__maxZImpact              = 250.0 * Units.mm
        
        # --- cluster cuts
        self.__minClusters             = 9
        self.__minSiNotShared          = 8
        #self.__maxShared               = 3 # cut is now on number of shared modules
        #self.__maxHoles                = 3
        #self.__maxPixelHoles           = D2
        #self.__maxSctHoles             = 2
        #self.__maxDoubleHoles          = 2
        # --- also tighten pattern cuts 
        self.__radMax                  = 1000. * Units.mm
        #self.__seedFilterLevel         = 1
        #self.__nHolesMax               = self.__maxHoles
        #self.__nHolesGapMax            = self.__maxHoles
        #self.__Xi2max                  = 15.0
        #self.__Xi2maxNoAdd             = 35.0
        #self.__nWeightedClustersMin    = self.__minClusters-1
     
    # --- IBL setup
    if mode == "IBL" :
      self.__extension               = "IBL"
      self.__seedFilterLevel         = 1
      self.__minPT                   = 0.900 * Units.GeV
      self.__minClusters             = 10
      self.__maxPixelHoles           = 1

    # --- High pile-up setup
    if mode == "HighPileup" :
      self.__extension               = "HighPileup"
      self.__seedFilterLevel         = 1
      self.__minPT                   = 0.900 * Units.GeV
      self.__minClusters             = 9
      self.__maxPixelHoles           = 0

    # --- mode for min bias, commissioning or doRobustReco
    if mode == 'MinBias' or self.__indetflags.doRobustReco(): 
      if self.__indetflags.doHIP300():
       self.__minPT                     = 0.300 * Units.GeV
      else:
       self.__minPT                     = 0.100 * Units.GeV
      self.__minClusters               = 5
      self.__minSecondaryPt            = 0.4 * Units.GeV  # Pt cut for back tracking + segment finding for these
      self.__minTRTonlyPt              = 0.4 * Units.GeV  # Pt cut for TRT only
      self.__TRTSegFinderPtBins        = 50
      self.__maxdImpactSSSSeeds        = 20.0    # apply cut on SSS seeds
      self.__excludeUsedTRToutliers    = False   # TRT outliers are added to the exclusion list
      self.__useTRTonlyOldLogic        = True    # turn off ole overlap logic to reduce number of hits
      self.__maxSecondaryImpact        = 100.0 * Units.mm # low lumi

    # --- mode for high-d0 tracks
    if mode == "LargeD0": 
      self.__extension          = "LargeD0" # this runs parallel to NewTracking
      self.__maxPT              = 1.0 * Units.TeV
      self.__minPT              = 900 * Units.MeV
      self.__maxEta             = 5
      self.__maxPrimaryImpact   = 300.0 * Units.mm
      self.__maxZImpact         = 1500.0 * Units.mm
      self.__maxSecondaryImpact = 300.0 * Units.mm
      self.__minSecondaryPt     = 500.0 * Units.MeV
      self.__minClusters        = 7
      self.__minSiNotShared     = 5
      self.__maxShared          = 2   # cut is now on number of shared modules
      self.__minPixel           = 0   
      self.__maxHoles           = 2
      self.__maxPixelHoles      = 1
      self.__maxSctHoles        = 2
      self.__maxDoubleHoles     = 1
      self.__radMax             = 600. * Units.mm
      self.__nHolesMax          = self.__maxHoles
      self.__nHolesGapMax       = self.__maxHoles # not as tight as 2*maxDoubleHoles  
      self.__seedFilterLevel   = 1
      self.__maxTracksPerSharedPRD = 2

    # --- mode for high-d0 tracks down to 100 MeV (minPT, minClusters, minSecondaryPt cuts loosened to MinBias level)
    if mode == "LowPtLargeD0": 
      self.__extension          = "LowPtLargeD0" # this runs parallel to NewTracking
      self.__maxPT              = 1.0 * Units.TeV
      self.__minPT              = 100 * Units.MeV
      self.__maxEta             = 5
      self.__maxPrimaryImpact   = 300.0 * Units.mm
      self.__maxZImpact         = 1500.0 * Units.mm
      self.__maxSecondaryImpact = 300.0 * Units.mm
      self.__minSecondaryPt     = 400.0 * Units.MeV
      self.__minClusters        = 5
      self.__minSiNotShared     = 5
      self.__maxShared          = 2   # cut is now on number of shared modules
      self.__minPixel           = 0   
      self.__maxHoles           = 2
      self.__maxPixelHoles      = 1
      self.__maxSctHoles        = 2
      self.__maxDoubleHoles     = 1
      self.__radMax             = 600. * Units.mm
      self.__nHolesMax          = self.__maxHoles
      self.__nHolesGapMax       = self.__maxHoles # not as tight as 2*maxDoubleHoles  
      self.__seedFilterLevel   = 1
      self.__maxTracksPerSharedPRD = 2
    
    # --- mode for high-d0 tracks down to 100 MeV (minPT, minClusters, minSecondaryPt cuts loosened to MinBias level)
    if mode == "DisplacedSoftPion":
      self.__extension             = "DisplacedSoftPion" # this runs parallel to NewTracking
      self.__maxPT                 = 1.0 * Units.TeV
      self.__minPT                 = 200 * Units.MeV
      self.__maxEta                = 5
      self.__maxPrimaryImpact      = 150.0 * Units.mm
      self.__maxZImpact            = 1000.0 * Units.mm
      self.__maxSecondaryImpact    = 50.0 * Units.mm
      self.__minSecondaryPt        = 400.0 * Units.MeV
      self.__minClusters           = 6
      self.__minSiNotShared        = 4
      self.__maxShared             = 2   # cut is now on number of shared modules
      self.__minPixel              = 0
      self.__maxHoles              = 2
      self.__maxPixelHoles         = 0
      self.__maxSctHoles           = 2
      self.__maxDoubleHoles        = 0
      self.__radMax                = 600. * Units.mm
      self.__nHolesMax             = self.__maxHoles
      self.__nHolesGapMax          = self.__maxHoles # not as tight as 2*maxDoubleHoles
      self.__usePixel              = False
      self.__seedFilterLevel       = 1
      self.__maxTracksPerSharedPRD = 2

    # --- change defaults for low pt tracking  
    if mode == "LowPt": 
      self.__extension        = "LowPt" # this runs parallel to NewTracking
      self.__maxPT            = self.__minPT + 0.3 * Units.GeV # some overlap
      self.__minPT            = 0.050 * Units.GeV
      self.__minClusters      = 5
      self.__minSiNotShared   = 4
      self.__maxShared        = 1   # cut is now on number of shared modules
      self.__minPixel         = 2   # At least one pixel hit for low-pt (ass seeded on pixels!)                
      self.__maxHoles         = 2
      self.__maxPixelHoles    = 1
      self.__maxSctHoles      = 2
      self.__maxDoubleHoles   = 1
      self.__radMax           = 600. * Units.mm
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles # not as tight as 2*maxDoubleHoles

      if self.__indetflags.doMinBias():
        self.__maxPT            = 1000000 * Units.GeV # Won't accept None *NEEDS FIXING*
        self.__maxPrimaryImpact = 100.0 * Units.mm
 
    if mode == "SLHCConversionFinding":
      self.__extension        = "SLHCConversionFinding" # this runs parallel to NewTracking
      self.__minPT                   = 0.9 * Units.GeV
      self.__maxPrimaryImpact        = 10.0 * Units.mm 
      self.__maxZImpact              = 150.0 * Units.mm 
      self.__minClusters             = 6 
      self.__minSiNotShared          = 4 
      #self.__maxShared               = 3 
      self.__maxHoles                = 0 
      #self.__maxPixelHoles           = D2
      #self.__maxSctHoles             = 2
      #self.__maxDoubleHoles          = 2
      # --- also tighten pattern cuts
      self.__radMax                  = 1000. * Units.mm 
      self.__radMin                  = 0. * Units.mm # not turn on this cut for now 
      #self.__seedFilterLevel         = 1
      #self.__nHolesMax               = self.__maxHoles
      #self.__nHolesGapMax            = self.__maxHoles
      #self.__Xi2max                  = 15.0
      #self.__Xi2maxNoAdd             = 35.0
      #self.__nWeightedClustersMin    = self.__minClusters-1
      # --- turn on Z Boundary seeding                                                                                                  
      self.__doZBoundary              = False # 


    # --- change defaults for very low pt tracking  
    if mode == "VeryLowPt": 
      self.__extension        = "VeryLowPt" # this runs parallel to NewTracking
      self.__maxPT            = self.__minPT + 0.3 * Units.GeV # some overlap
      self.__minPT            = 0.050 * Units.GeV
      self.__minClusters      = 3
      self.__minSiNotShared   = 3
      self.__maxShared        = 1   # cut is now on number of shared modules
      self.__minPixel         = 3   # At least one pixel hit for low-pt (ass seeded on pixels!)                
      self.__maxHoles         = 1
      self.__maxPixelHoles    = 1
      self.__maxSctHoles      = 1
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles # not as tight as 2*maxDoubleHoles
      self.__radMax           = 600. * Units.mm # restrivt to pixels

      if self.__indetflags.doMinBias():
        self.__maxPT            = 100000 * Units.GeV # Won't accept None *NEEDS FIXING*

    # --- change defaults for forward muon tracking
    if mode == "ForwardTracks":
      self.__extension        = "ForwardTracks" # this runs parallel to NewTracking
      self.__minEta           = 2.4 # restrict to minimal eta
      self.__maxEta           = 2.7
      self.__minPT            = 2 * Units.GeV
      self.__minClusters      = 3
      self.__minSiNotShared   = 3
      self.__maxShared        = 1
      self.__minPixel         = 3
      self.__maxHoles         = 1
      self.__maxPixelHoles    = 1
      self.__maxSctHoles      = 1
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles
      self.__radMax           = 600. * Units.mm

      self.__useTRT           = False # no TRT for forward tracks


    if mode == "ForwardSLHCTracks":
      self.__extension        = "ForwardSLHCTracks" # this runs parallel to NewTracking
      self.__minEta           = 2.4 # restrict to minimal eta
      self.__maxEta           = 3.0
      self.__minPT            = 0.9 * Units.GeV
      self.__minClusters      = 5
      self.__minSiNotShared   = 3
      self.__maxShared        = 1
      self.__minPixel         = 3
      self.__maxHoles         = 1
      self.__maxPixelHoles    = 1
      self.__maxSctHoles      = 1
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles
      self.__radMax           = 600. * Units.mm

      self.__useTRT           = False # no TRT for forward tracks

    if mode == "VeryForwardSLHCTracks": 
      self.__extension        = "VeryForwardSLHCTracks" # this runs parallel to NewTracking 
      self.__minEta           = 2.4 # restrict to minimal eta 
      self.__maxEta           = 4.0 
      self.__minPT            = 0.9 * Units.GeV 
      self.__minClusters      = 5 
      self.__minSiNotShared   = 3 
      self.__maxShared        = 1 
      self.__minPixel         = 3 
      self.__maxHoles         = 1 
      self.__maxPixelHoles    = 1 
      self.__maxSctHoles      = 0 
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles
      self.__radMax           = 600. * Units.mm
      self.__useTRT           = False # no TRT for forward tracks

    # --- change defauls for beam gas tracking 
    if mode == "BeamGas":
      self.__extension        = "BeamGas" # this runs parallel to NewTracking
      self.__minPT            = 0.500 * Units.GeV
      self.__maxPrimaryImpact = 300. * Units.mm 
      self.__maxZImpact       = 2000. * Units.mm 
      self.__minClusters      = 6
      self.__maxHoles         = 3
      self.__maxPixelHoles    = 3
      self.__maxSctHoles      = 3
      self.__maxDoubleHoles   = 1
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles # not as tight as 2*maxDoubleHoles

    # --- setup for lumi determination based on vertices
    if mode == "VtxLumi" :
      self.__extension               = "VtxLumi"
      self.__seedFilterLevel         = 1
      self.__minPT                   = 0.900 * Units.GeV
      self.__minClusters             = 7
      self.__maxPixelHoles           = 1
      self.__radMax                  = 600. * Units.mm
      self.__nHolesMax               = 2
      self.__nHolesGapMax            = 1
      self.__useTRT                  = False

 # --- setup for beamspot determination based on vertices
    if mode == "VtxBeamSpot" :
      self.__extension               = "VtxBeamSpot"
      self.__seedFilterLevel         = 1
      self.__minPT                   = 0.900 * Units.GeV
      self.__minClusters             = 9
      self.__maxPixelHoles           = 0
      self.__radMax                  = 320. * Units.mm
      self.__nHolesMax               = 2
      self.__nHolesGapMax            = 1
      self.__useTRT                  = False

  # --- changes for cosmics
    if mode == "Cosmics":
      self.__minPT            = 0.500 * Units.GeV
      self.__maxPrimaryImpact = 1000. * Units.mm 
      self.__maxZImpact       = 10000. * Units.mm 
      self.__minClusters      = 4
      self.__minSiNotShared   = 4
      self.__maxHoles         = 3
      self.__maxPixelHoles    = 3
      self.__maxSctHoles      = 3
      self.__maxDoubleHoles   = 1
      self.__minTRTonTrk      = 15
      self.__minTRTOnly       = 15
      self.__roadWidth        = 60.
      self.__seedFilterLevel  = 3
      self.__Xi2max           = 60.0
      self.__Xi2maxNoAdd      = 100.0
      self.__nWeightedClustersMin = 8
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles # not as tight as 2*maxDoubleHoles

    # --- changes for heavy ion
    if mode == "HeavyIon":
      self.__maxZImpact       = 200. * Units.mm 
      self.__minPT            = 0.500 * Units.GeV
      self.__minClusters      = 9
      self.__minSiNotShared   = 7
      self.__maxShared        = 2 # was 1, cut is now on number of shared modules
      self.__maxHoles         = 0
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__Xi2max           = 6. 
      self.__Xi2maxNoAdd      = 10.
      if self.__indetflags.cutLevel() == 1:
        self.__seedFilterLevel  = 1
      elif self.__indetflags.cutLevel() == 2:
        self.__seedFilterLevel  = 2
        self.__maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
      elif self.__indetflags.cutLevel() == 3: # This is for MB data
        self.__minPT            = 0.300 * Units.GeV
        self.__seedFilterLevel  = 2
        self.__maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
        self.__useParameterizedTRTCuts   = False
        self.__useNewParameterizationTRT = False
      elif self.__indetflags.cutLevel() == 4: # ==CutLevel 2 with loosened hole cuts and chi^2 cuts
       self.__seedFilterLevel  = 2
       self.__maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
       self.__maxHoles               = 2 
       self.__maxPixelHoles       = 1 
       self.__maxSctHoles         = 1
       self.__maxDoubleHoles   = 0
       self.__Xi2max                   = 9. 
       self.__Xi2maxNoAdd        = 25.
      elif self.__indetflags.cutLevel() == 5: # ==CutLevel 3 with loosened hole cuts and chi^2 cuts
       self.__minPT            = 0.300 * Units.GeV
       self.__seedFilterLevel  = 2
       self.__maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
       self.__useParameterizedTRTCuts   = False
       self.__useNewParameterizationTRT = False
       self.__maxHoles               = 2
       self.__maxPixelHoles       = 1
       self.__maxSctHoles         = 1
       self.__maxDoubleHoles   = 0
       self.__Xi2max                   = 9. 
       self.__Xi2maxNoAdd        = 25.

      self.__radMax           = 600. * Units.mm # restrict to pixels + first SCT layer
      self.__useTRT           = False 

    # --- changes for Pixel/SCT segments
    from AthenaCommon.DetFlags    import DetFlags
    if ( DetFlags.haveRIO.pixel_on() and not DetFlags.haveRIO.SCT_on() ):
      self.__minClusters = 3
    elif ( DetFlags.haveRIO.SCT_on() and not DetFlags.haveRIO.pixel_on() ):
      self.__minClusters = 6
      
    # --- changes for Pixel segments
    if mode == "Pixel":
      self.__extension        = "Pixel" # this runs parallel to NewTracking
      self.__minPT            = 0.1 * Units.GeV
      self.__minClusters      = 3
      self.__maxHoles         = 1
      self.__maxPixelHoles    = 1
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__minSiNotShared   = 3
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__useSCT           = False
      self.__useTRT           = False 

      if self.__indetflags.doMinBias():
        if self.__indetflags.doHIP300():
          self.__minPT            = 0.300 * Units.GeV
        else:
          self.__minPT            = 0.05 * Units.GeV
        self.__maxPT            = 100000 * Units.GeV # Won't accept None *NEEDS FIXING*

      if self.__indetflags.doHeavyIon():
        self.__minPT            = 0.1 * Units.GeV
        self.__maxHoles         = 0
        self.__maxPixelHoles    = 0
        self.__minSiNotShared   = 3
        self.__maxShared        = 0
        self.__seedFilterLevel  = 2
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = self.__maxHoles      
        self.__useSCT           = False
        self.__useTRT           = False 
        
      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = 3
        self.__maxShared        = 0    # no shared hits in cosmics
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = self.__maxHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6


    if mode == "PixelPrdAssociation":
      self.__extension        = "PixelPrdAssociation" # this runs after NewTracking
      self.__minPT            = 5.0 * Units.GeV
      self.__minClusters      = 4
      self.__maxHoles         = 0
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__minSiNotShared   = 3
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      # self.__useSCT           = False
      self.__useSCT           = True
      # self.__useTRT           = False
      self.__useTRT           = True
      self.__useSCTSeeding    = False
      self.__maxEta           = 2.2

    if mode == "PixelFourLayer":
      self.__extension        = "PixelFourLayer" # this runs after NewTracking
      self.__minPT            = 5.0 * Units.GeV
      self.__minClusters      = 4
      self.__maxHoles         = 0
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__minSiNotShared   = 3
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__useSCT           = True
      self.__useTRT           = True
      self.__useSCTSeeding    = False
      self.__maxEta           = 2.2

    if mode == "PixelThreeLayer":
      self.__extension        = "PixelThreeLayer" # this runs after NewTracking
      self.__minPT            = 5.0 * Units.GeV
      self.__minClusters      = 3
      self.__maxHoles         = 0
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__minSiNotShared   = 0
      self.__maxShared        = 0
      self.__seedFilterLevel  = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__useSCT           = True
      self.__useTRT           = True
      self.__useSCTSeeding    = False
      self.__maxEta           = 2.2
      self.__radMax           = 150. * Units.mm # to pixel layer-4 (~ 122.5 mm)


    # --- changes for SCT segments
    if mode == "SCT":
      self.__extension        = "SCT" # this runs parallel to NewTracking
      self.__minPT            = 0.1 * Units.GeV
      self.__minClusters      = 7
      self.__maxHoles         = 2
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 2
      self.__maxDoubleHoles   = 1
      self.__minSiNotShared   = 5
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__usePixel         = False
      self.__useTRT           = False 

      if self.__indetflags.doMinBias():
        if self.__indetflags.doHIP300():
           self.__minPT            = 0.3 * Units.GeV
        else:
           self.__minPT            = 0.1 * Units.GeV

      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = 0
        self.__maxSctHoles      = 3
        self.__maxShared        = 0   # no shared hits in cosmics
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = self.__maxHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6

        if self.__indetflags.doInnerDetectorCommissioning():
          self.__minClusters      = 4
          self.__minSiNotShared   = 4
          self.__nWeightedClustersMin = 4

    # --- TRT subdetector tracklet cuts
    if mode == "TRT":
      self.__minPT                   = 0.4 * Units.GeV
      self.__minTRTonly              = 15
      self.__maxTRTonlyShared        = 0.7

    # --- mode for SCT and TRT 
    if mode == "SCTandTRT":
      self.__extension        = "SCTandTRT" # this runs parallel to NewTracking
      self.__minPT            = 0.4 * Units.GeV
      self.__minClusters      = 7
      self.__maxHoles         = 2
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 2
      self.__maxDoubleHoles   = 0
      self.__minSiNotShared   = 5
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles      
      self.__usePixel         = False
      self.__useTRT           = True 

      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = 0
        self.__maxSctHoles      = 3
        self.__maxShared        = 0   # no shared hits in cosmics
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = self.__maxHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6

        if self.__indetflags.doInnerDetectorCommissioning():
          self.__minClusters      = 4
          self.__nWeightedClustersMin = 4
          self.__minSiNotShared   = 4
          self.__rejectShortExtensions     = False

    if mode == "DBM":
      self.__extension               = "DBM"
      self.__minEta                  = 3.05
      self.__maxEta                  = 3.45
      self.__Xi2maxNoAdd             = 10000
      self.__Xi2max                  = 10000
      self.__nWeightedClustersMin    = 0
      self.__seedFilterLevel         = 1
      self.__maxdImpactPPSSeeds      = 100000.0 * Units.mm
      self.__maxdImpactSSSSeeds      = 100000.0 * Units.mm 
      self.__maxPrimaryImpact        = 100000.0 * Units.mm  # low lumi
      self.__maxZImpact              = 320000.0 * Units.mm  # Was 250 mm
      self.__maxPT            = 100000.0 * Units.GeV # some overlap
      self.__minPT            = 0.0 * Units.GeV
      self.__minClusters      = 0
      self.__minSiNotShared   = 0
      self.__maxShared        = 1000   # cut is now on number of shared modules
      self.__minPixel         = 0   
      self.__maxHoles         = 0
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 0
      self.__maxDoubleHoles   = 0
      self.__radMax           = 600000. * Units.mm
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles # not as tight as 2*maxDoubleHoles
      self.__useTRT           = False
      self.__useSCT           = False
      self.__usePixel         = True

        
#        elif rec.Commissioning():
#        self.__minClusters             = 7               # Igor 6, was 7
#        self.__maxHoles                = 5               # was 5
#        self.__maxSctHoles             = 5               # was 5
#        self.__maxDoubleHoles          = 4               # was 2
#        self.__maxPrimaryImpact        = 50.0 * Units.mm # low lumi
#        self.__maxZImpact              = 500.0 * Units.mm
        
# ----------------------------------------------------------------------------
# --- private method
  def __set_indetflags(self):
    from InDetRecExample.InDetJobProperties import InDetFlags
    self.__indetflags = InDetFlags

# ----------------------------------------------------------------------------
# --- return methods for the cut values - the main purpose of this class
# ----------------------------------------------------------------------------

  def mode( self ) :
    return self.__mode

  def extension( self ) :
    return self.__extension

  def minPT( self ) :
    return self.__minPT
  
  def etaBins( self ) :
    return self.__etaBins

  def maxPT( self ) :
    return self.__maxPT

  def minPTBrem( self ) :
    return self.__minPTBrem

  def phiWidthBrem( self ) :
    return self.__phiWidthBrem

  def etaWidthBrem( self ) :
    return self.__etaWidthBrem

  def maxPrimaryImpact( self ) :
    return self.__maxPrimaryImpact

  def maxSecondaryImpact( self ) :
    return self.__maxSecondaryImpact

  def maxZImpact( self ) :
    return self.__maxZImpact

  def minEta( self ) :
    return self.__minEta

  def maxEta( self ) :
    return self.__maxEta

  def minClusters( self ) :
    return self.__minClusters

  def minPixel( self ) :
    return self.__minPixel  

  def minSecondaryClusters( self ) :
    return self.__minSecondaryClusters

  def minSiNotShared( self ) :
    return self.__minSiNotShared

  def minSecondarySiNotShared( self ) :
    return self.__minSecondarySiNotShared

  def maxShared( self ) :
    return self.__maxShared

  def maxSecondaryShared( self ) :
    return self.__maxSecondaryShared

  def maxHoles( self ) :
    return self.__maxHoles

  def maxPixelHoles( self ) :
    return self.__maxPixelHoles

  def maxSCTHoles( self ) :
    return self.__maxSctHoles

  def maxSecondaryHoles( self ) :
    return self.__maxSecondaryHoles

  def maxSecondaryPixelHoles( self ) :
    return self.__maxSecondaryPixelHoles

  def maxSecondarySCTHoles( self ) :
    return self.__maxSecondarySCTHoles

  def maxDoubleHoles( self ) :
    return self.__maxDoubleHoles
    
  def maxSecondaryDoubleHoles( self ) :
    return self.__maxSecondaryDoubleHoles

  def maxSecondaryTRTShared( self ) :
    return self.__maxSecondaryTRTShared

  def minTRTonTrk( self ) :
    return self.__minTRTonTrk

  def minTRTPrecFrac( self ) :
    return self.__minTRTPrecFrac

  def useParameterizedTRTCuts( self ) :
    return self.__useParameterizedTRTCuts

  def useNewParameterizationTRT( self ) :
    return self.__useNewParameterizationTRT

  def minSecondaryTRTonTrk ( self ) :
    return self.__minSecondaryTRTonTrk

  def minSecondaryTRTPrecFrac( self ) :
    return self.__minSecondaryTRTPrecFrac

  def TRTSegFinderPtBins ( self ) :
    return self.__TRTSegFinderPtBins

  def minTRTonly( self ) :
    return self.__minTRTonly

  def maxTRTonlyShared( self ) :
    return self.__maxTRTonlyShared

  def excludeUsedTRToutliers ( self ) :
    return self.__excludeUsedTRToutliers

  def rejectShortExtensions ( self ) :
    return self.__rejectShortExtensions

  def SiExtensionCuts ( self ) :
    return self.__SiExtensionCuts

  def useTRTonlyParamCuts ( self ) :
    return self.__useTRTonlyParamCuts

  def useTRTonlyOldLogic ( self ) :
    return self.__useTRTonlyOldLogic

  def minSecondaryPt( self ) :
    return self.__minSecondaryPt

  def minTRTonlyPt( self ) :
    return self.__minTRTonlyPt

  def RoadWidth( self ) :
    return self.__roadWidth

  def seedFilterLevel( self ) :
    return self.__seedFilterLevel

  def radMax( self ) :
    return self.__radMax

  def nHolesMax( self ) :
    return self.__nHolesMax

  def nHolesGapMax( self ) :
    return self.__nHolesGapMax

  def Xi2max( self ) :
    return self.__Xi2max
  
  def Xi2maxNoAdd( self ) :
    return self.__Xi2maxNoAdd

  def SecondarynHolesMax( self ) :
    return self.__SecondarynHolesMax

  def SecondarynHolesGapMax( self ) :
    return self.__SecondarynHolesGapMax

  def SecondaryXi2max( self ) :
    return self.__SecondaryXi2max
  
  def SecondaryXi2maxNoAdd( self ) :
    return self.__SecondaryXi2maxNoAdd

  def nWeightedClustersMin( self ) :
    return self.__nWeightedClustersMin 

  def maxdImpactPPSSeeds( self ) :
    return self.__maxdImpactPPSSeeds

  def maxdImpactSSSSeeds( self ) :
    return self.__maxdImpactSSSSeeds

  def usePixel( self ) :
    return self.__usePixel
  
  def useSCT( self ) :
    return self.__useSCT

  def useSCTSeeding( self ) :
    return self.__useSCTSeeding

  def useTRT( self ) :
    return self.__useTRT
  
  def doZBoundary( self ) :
    return self.__doZBoundary

  def RoISeededBackTracking( self ) :
    return self.__RoISeededBackTracking

  def printInfo( self ) :
    print '****** Inner Detector Track Reconstruction Cuts ************************************'
    print '*'
    print '* SETUP is  : ',self.__mode
    print '* extension : ',self.__extension
    print '*'
    print '* InDetFlags.cutLevel() = ', self.__indetflags.cutLevel()
    print '*'
    if self.__indetflags.useEtaDependentCuts() and self.__mode == "SLHC":
      tmp_list = list(self.__etaBins)
      tmp_list[0] = 0.
      print '* Using dynamic cuts with eta ranges :', tmp_list
    print '* Pixel used                  :  ', self.__usePixel
    print '* SCT used                    :  ', self.__useSCT
    print '* TRT used                    :  ', self.__useTRT
    print '*'  
    print '* min pT                      :  ', self.__minPT, ' MeV'
    if self.__indetflags.useEtaDependentCuts() and self.__mode == "SLHC":
      print '* min pT for seeding          :  ', self.__minPT[0], ' MeV'
    print '* max Z IP                    :  ', self.__maxZImpact, ' mm'
    if self.__indetflags.useEtaDependentCuts() and self.__mode == "SLHC":
      print '* max Z IP for seeding        :  ', self.__maxZImpact[0], ' mm'
    print '* min eta                     :  ', self.__minEta
    print '* max eta                     :  ', self.__maxEta
    if self.__mode=="LowPt":
      print '* max PT                      :  ', self.__maxPT, ' MeV'
    print '*'
    print '* NewTracking cuts:'
    print '* -----------------'
    print '* max Rphi IP (primaries)     :  ', self.__maxPrimaryImpact, ' mm'
    if self.__indetflags.useEtaDependentCuts() and self.__mode == "SLHC":
      print '* max Rphi IP for seeding     :  ', self.__maxPrimaryImpact[0], ' mm'
    print '* min number of clusters      :  ', self.__minClusters
    print '* min number of pixel hits    :  ', self.__minPixel
    print '* min number of NOT shared    :  ', self.__minSiNotShared
    print '* max number of shared        :  ', self.__maxShared
    print '* max number of Si holes      :  ', self.__maxHoles
    print '* max number of Pixel holes   :  ', self.__maxPixelHoles
    print '* max number of SCT holes     :  ', self.__maxSctHoles
    print '* max number of double holes  :  ', self.__maxDoubleHoles
    print '*'
    print '* seed filter level           :  ', self.__seedFilterLevel  
    print '* maximal R of SP for seeding :  ', self.__radMax 
    print '* max holes in pattern        :  ', self.__nHolesMax
    print '* max holes gap in pattern    :  ', self.__nHolesGapMax
    print '* Xi2 max                     :  ', self.__Xi2max
    print '* Xi2 max no add              :  ', self.__Xi2maxNoAdd
    if self.__indetflags.useEtaDependentCuts() and self.__mode == "SLHC":
      print '* max impact on seeds PPS/SSS :  ', self.__maxdImpactPPSSeeds,', ',self.__maxdImpactSSSSeeds[0]
    else:
      print '* max impact on seeds PPS/SSS :  ', self.__maxdImpactPPSSeeds,', ',self.__maxdImpactSSSSeeds
    print '* nWeightedClustersMin        :  ', self.__nWeightedClustersMin 
    if self.__useTRT:
      print '* min TRT on track extension  :  ', self.__minTRTonTrk
      print '* min TRT precision fraction  :  ', self.__minTRTPrecFrac
    if self.__indetflags.doBremRecovery() and self.__mode == "Offline":
      print '*'
      print "* -> Brem recovery enabled !"
      print '* min pT for brem reocvery    :  ', self.__minPTBrem, ' MeV'
      if self.__indetflags.doCaloSeededBrem():
        print "* -> in Calo seeded mode !!!"
        print '* phi Width of road for brem  :  ', self.__phiWidthBrem
        print '* eta Width of road for brem  :  ', self.__etaWidthBrem
    print '*'  
    if self.__useTRT and self.__RoISeededBackTracking:
      print '*'  
      print '* RoI seeded BackTracking and TRT only !!!'
      print '*'
    if self.__useSCT and self.__useTRT:
      print '* BackTracking cuts:'
      print '* ------------------'
      print '* min pt                      :  ', self.__minSecondaryPt, ' MeV'
      print '* max Rphi IP (secondaries)   :  ', self.__maxSecondaryImpact, ' mm'
      print '* min number of clusters      :  ', self.__minSecondaryClusters
      print '* min number of NOT shared    :  ', self.__minSecondarySiNotShared
      print '* max number of shared        :  ', self.__maxSecondaryShared
      print '* max number of Si holes      :  ', self.__maxSecondaryHoles
      print '* max number of Pixel holes   :  ', self.__maxSecondaryPixelHoles
      print '* max number of SCT holes     :  ', self.__maxSecondarySCTHoles
      print '* max number of double holes  :  ', self.__maxSecondaryDoubleHoles
      print '* min TRT on track            :  ', self.__minSecondaryTRTonTrk
      print '* min TRT precision fraction  :  ', self.__minSecondaryTRTPrecFrac
      print '* max TRT shared fraction     :  ', self.__maxSecondaryTRTShared
      print '* max holes in pattern        :  ', self.__SecondarynHolesMax
      print '* max holes gap in pattern    :  ', self.__SecondarynHolesGapMax
      print '* Xi2 max                     :  ', self.__SecondaryXi2max
      print '* Xi2 max no add              :  ', self.__SecondaryXi2maxNoAdd
      print '* TRT segment finder pt bins  :  ', self.__TRTSegFinderPtBins
      print '* rejectShortExtensions       :  ', self.__rejectShortExtensions
      print '* SiExtensionsCuts            :  ', self.__SiExtensionCuts
      print '*'
    if self.__useTRT:
      print '* useParameterizedTRTCuts     :  ', self.__useParameterizedTRTCuts
      print '* useNewParameterizationTRT   :  ', self.__useNewParameterizationTRT
      print '* excludeUsedTRToutliers      :  ', self.__excludeUsedTRToutliers
      print '*'  
      print '* TRT only cuts:'
      print '* --------------'
      print '* min pt                      :  ', self.__minTRTonlyPt, ' MeV'
      print '* min TRT only hits           :  ', self.__minTRTonly
      print '* max TRT shared fraction     :  ', self.__maxTRTonlyShared
      print '* useTRTonlyParamCuts         :  ', self.__useTRTonlyParamCuts
      print '* old transition hit logic    :  ', self.__useTRTonlyOldLogic
      print '*'
    print '************************************************************************************'


