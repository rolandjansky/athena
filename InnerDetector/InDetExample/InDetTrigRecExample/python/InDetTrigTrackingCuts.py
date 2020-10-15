# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class InDetTrigTrackingCuts :
  """
  A copy of ConfiguredNewtrackingCuts from InDetRecExample used by the trigger.
  
  think twice before jumping to wrong conclusions

  """


  def __init__ (self, mode = "offline"):

    import AthenaCommon.SystemOfUnits as Units

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

    # --- first set kinematic defaults 
    self.__minPT                   = 0.500 * Units.GeV
    self.__maxPT                   = None            # off !
    self.__maxEta                  = 2.7

    # --- cluster cuts
    self.__minClusters             = 7                # Igor 6, was 7
    self.__minSiNotShared          = 5
    self.__maxShared               = 2
    self.__minPixel                = 0                
    self.__maxHoles                = 3                # was 5
    self.__maxPixelHoles           = 2                # was 5
    self.__maxSctHoles             = 3                # was 5
    self.__maxDoubleHoles          = 1                # was 2
    self.__maxPrimaryImpact        = 10.0 * Units.mm  # low lumi
    self.__maxZImpact              = 320. * Units.mm    
    if self.__indetflags.doRobustReco():
      self.__minClusters             = 7                # Igor 6, was 7
      self.__maxHoles                = 5                # was 5
      self.__maxPixelHoles           = 2                # was 5
      self.__maxSctHoles             = 5                # was 5
      self.__maxDoubleHoles          = 4                # was 2
      self.__maxZImpact              = 500.0 * Units.mm

    # --- seeding 
    self.__seedFilterLevel         = 1
    self.__maxdImpactPPSSeeds      = 1.7
    self.__maxdImpactSSSSeeds      = 1000.0

    # --- min pt cut for brem
    self.__minPTBrem               = 1. * Units.GeV # off
    self.__phiWidthBrem            = 0.3 # default is 0.3
    self.__etaWidthBrem            = 0.2 # default is 0.3
    
    # --- this is for the TRT-extension + segements and backtracking
    self.__minTRTonTrk               = 9
    self.__useParameterizedTRTCuts   = False
    self.__useNewParameterizationTRT = False 

    # --- general pattern cuts for NewTracking
    self.__radMax                  = 600. * Units.mm # default R cut for SP in SiSpacePointsSeedMaker
    self.__roadWidth               = 20.
    self.__seedFilterLevel         = 2
    self.__nHolesMax               = self.__maxHoles
    self.__nHolesGapMax            = self.__maxHoles # not as tight as 2*maxDoubleHoles
    self.__Xi2max                  = 15.0
    self.__Xi2maxNoAdd             = 35.0
    self.__nWeightedClustersMin    = 6
    if rec.Commissioning():
      self.__SecondarynHolesMax      = 5
      self.__SecondarynHolesGapMax   = 4
    else:
      self.__SecondarynHolesMax      = 1
      self.__SecondarynHolesGapMax   = 1
    
    # --- defaults for secondary tracking
    self.__maxSecondaryImpact      = 100.0 * Units.mm # low lumi
    self.__minSecondaryClusters    = 4
    self.__minSecondarySiNotShared = 4
    self.__maxSecondaryShared      = 2
    self.__minSecondaryTRTonTrk    = 10
    self.__maxSecondaryHoles       = 2
    self.__maxSecondaryPixelHoles  = self.__maxSecondaryHoles # --> no effect
    self.__maxSecondarySCTHoles    = self.__maxSecondaryHoles # --> no effect
    self.__maxSecondaryDoubleHoles = 1
 #   if rec.Commissioning():
 #     self.__maxSecondaryHoles       = 5
 #     self.__maxSecondaryPixelHoles  = self.__maxSecondaryHoles # --> no effect
 #     self.__maxSecondarySCTHoles    = self.__maxSecondaryHoles # --> no effect
 #     self.__maxSecondaryDoubleHoles = 2
 #   else:

    # --- RegionSelector
    self.__RegSelZmax = 225. * Units.mm       #change in sync with RS, problems with strategyC?

    # --- pattern cuts for back tracking
    self.__SecondaryXi2max         = 15.0
    self.__SecondaryXi2maxNoAdd    = 50.0
    if rec.Commissioning():
      self.__SecondaryXi2max         = 50.0 
      self.__SecondaryXi2maxNoAdd    = 100.0 

    # --- settings for segment finder 
    self.__TRTSegFinderPtBins        = 70
    self.__maxSegTRTShared           = 0.3
    self.__excludeUsedTRToutliers    = False

    
    # --- TRT only
    self.__minTRTonlyMinPt         = 0.5 * Units.GeV
    self.__minTRTonly              = 15
    self.__maxTRTonlyShared        = 0.3

    # --- TRT standalone has harder cuts
    if mode == "TRT":
      self.__minTRTonlyMinPt         = 0.5 * Units.GeV
      self.__minTRTonly              = 15
      self.__maxTRTonlyShared        = 0.3
      self.__useTRTonlyParamCuts       = False
      self.__useTRTonlyOldLogic        = True


    #
    # --------------------------------------
    # --- now start tighening cuts level by level
    # --------------------------------------
    #
    mxlevel = 11
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
      self.__minSecondaryTRTPrecFrac   = 0.5              # default for all tracking now, as well for BackTracking
      self.__rejectShortExtensions     = True             # fall back onto segment if TRT extension is short
      self.__SiExtensionCuts           = True             # use cuts from ambi scoring already early
      # self.__maxSecondaryTRTShared     = 0.2              # tighen shared hit cut for segment maker ?

    if self.__indetflags.cutLevel() >= 8:
      # --- slightly tighen NewTracking cuts
      self.__maxHoles                  = 2                # was 3
      self.__maxPixelHoles             = 1                # was 2
    
    if self.__indetflags.cutLevel() >= 9:
      self.__maxZImpact              = 200. * Units.mm     #this should come from RS/Roi

    if self.__indetflags.cutLevel() >= 10:
      self.__doZBoundary              = True

    if self.__indetflags.cutLevel() >= mxlevel:
      print 'InDetTrigTrackingCuts INFO using cutLevel %d/%d' % (mxlevel,self.__indetflags.cutLevel())

    # --- SLHC setup
    if mode == "SLHC":
      self.__extension        = "SLHC"
      # --- higher pt cut and impact parameter cut
      self.__minPT                   = 1.0 * Units.GeV      
      self.__maxPrimaryImpact        = 2.0 * Units.mm # highlumi
      # --- cluster cuts
      self.__minClusters             = 9
      self.__minSiNotShared          = 8
      self.__maxShared               = 5
      self.__maxHoles                = 3
      self.__maxPixelHoles           = self.__maxHoles
      self.__maxSctHoles             = self.__maxHoles
      self.__maxDoubleHoles          = 2
      # --- also tighten patter cuts 
      self.__radMax                  = 1000. * Units.mm
      self.__seedFilterLevel         = 1
      self.__nHolesMax               = self.__maxHoles
      self.__nHolesGapMax            = 2*self.__maxDoubleHoles
      #self.__Xi2max                  = 15.0
      #self.__Xi2maxNoAdd             = 35.0
      self.__nWeightedClustersMin    = self.__minClusters-1

    if mode == "LRT":
      self.__minClusters             = 8
      self.__minSiNotShared          = 6
      self.__maxShared               = 1
      self.__maxHoles                = 2
      self.__maxPixelHoles           = 1
      self.__maxSctHoles             = 1
      self.__maxDoubleHoles          = 0
      self.__maxPrimaryImpact        = 300.0 * Units.mm
      self.__maxZImpact              = 500. * Units.mm    
      self.__roadWidth               = 5.
      self.__seedFilterLevel         = 1
      self.__nHolesMax               = self.__maxHoles
      self.__nHolesGapMax            = 1
      self.__Xi2max                  = 9.
      self.__Xi2maxNoAdd             = 25.0
      self.__nWeightedClustersMin    = 8
      self.__doZBoundary             = True
      self.__maxdImpactSSSSeeds      = 300.0

      
    if mode == "FTK":
      self.__extension        = "FTK"
      # --- cluster cuts
      #self.__minClusters             = 9
      #self.__minSiNotShared          = 8
      #self.__maxShared               = 5
      self.__maxHoles                = 4
      self.__maxPixelHoles           = 2
      self.__maxSctHoles             = 4
      self.__maxDoubleHoles          = 2

    # --- changes for Pixel/SCT segments
    from AthenaCommon.DetFlags    import DetFlags
    if ( DetFlags.haveRIO.pixel_on() and not DetFlags.haveRIO.SCT_on() ):
      self.__minClusters = 3
    elif ( DetFlags.haveRIO.SCT_on() and not DetFlags.haveRIO.pixel_on() ):
      self.__minClusters = 6
      
    # --- change defaults for low pt tracking  
    if mode == "LowPt": 
      self.__extension        = "LowPt" # this runs parallel to NewTracking
      self.__maxPT            = self.__minPT + 0.3 * Units.GeV # some overlap
      self.__minPT            = 0.100 * Units.GeV
      self.__minClusters      = 5
      self.__minPixel         = 1   # At least one pixel hit for low-pt (ass seeded on pixels!)                
      self.__maxHoles         = 4
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 2
      self.__radMax           = 600. * Units.mm # restrivt to pixels
    # --- change defauls for beam gas tracking 
    if mode == "BeamGas":
      self.__extension        = "BeamGas" # this runs parallel to NewTracking
      self.__minPT            = 0.500 * Units.GeV
      self.__maxPrimaryImpact = 300. * Units.mm 
      self.__maxZImpact       = 2000. * Units.mm 
      self.__minClusters      = 6
      self.__maxHoles         = 3
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 1
    # --- changes for cosmics
    if mode == "Cosmics":
      self.__minPT            = 0.500 * Units.GeV
      self.__maxPrimaryImpact = 1000. * Units.mm 
      self.__maxZImpact       = 10000. * Units.mm 
      self.__minClusters      = 4
      self.__maxHoles         = 3
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 1
      self.__minTRTonTrk      = 15
      self.__minTRTOnly       = 15
      self.__roadWidth        = 60.
      self.__seedFilterLevel  = 3
      self.__Xi2max           = 60.0
      self.__Xi2maxNoAdd      = 100.0
      self.__nWeightedClustersMin = 8
    # --- changes for heavy ion
    if mode == "HeavyIon":
      self.__minPT            = 0.500 * Units.GeV
      self.__minClusters      = 9
      self.__minSiNotShared   = 7
      self.__maxShared        = 1
      self.__maxHoles         = 1
      self.__maxPixelHoles    = 0
      self.__maxSctHoles      = 1
      self.__maxDoubleHoles   = 0
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = self.__maxHoles
      self.__Xi2max           = 6. 
      self.__Xi2maxNoAdd      = 10.
      self.__seedFilterLevel  = 1
      self.__radMax           = 600. * Units.mm # restrict to pixels + first SCT layer
      self.__useTRT           = False 
    # --- changes for Pixel segments
    if mode == "Pixel":
      self.__extension        = "Pixel" # this runs parallel to NewTracking
      self.__minPT            = 0.5 * Units.GeV
      self.__minClusters      = 3
      self.__maxHoles         = 1
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 1
      self.__minSiNotShared   = 3
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
      self.__useSCT           = False
      self.__useTRT           = False 
      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = self.__maxHoles # --> no effect
        self.__maxSctHoles      = self.__maxHoles # --> no effect
        self.__maxShared        = 3
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6
#      elif rec.Commissioning():
#        self.__maxPrimaryImpact        = 50.0 * Units.mm # low lumi
#        self.__maxZImpact              = 500.0 * Units.mm

    # --- changes for SCT segments
    if mode == "SCT":
      self.__extension        = "SCT" # this runs parallel to NewTracking
      self.__minPT            = 0.5 * Units.GeV
      self.__minClusters      = 7
      self.__maxHoles         = 2
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 1
      self.__minSiNotShared   = 5
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
      self.__usePixel         = False
      self.__useTRT           = False 
      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = self.__maxHoles # --> no effect
        self.__maxSctHoles      = self.__maxHoles # --> no effect
        self.__maxShared        = 3
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6
    # --- mode for SCT and TRT 
    if mode == "SCTandTRT":
      self.__extension        = "SCTandTRT" # this runs parallel to NewTracking
      self.__minPT            = 0.5 * Units.GeV
      self.__minClusters      = 7
      self.__maxHoles         = 2
      self.__maxPixelHoles    = self.__maxHoles # --> no effect
      self.__maxSctHoles      = self.__maxHoles # --> no effect
      self.__maxDoubleHoles   = 1
      self.__minSiNotShared   = 5
      self.__maxShared        = 0
      self.__seedFilterLevel  = 2
      self.__nHolesMax        = self.__maxHoles
      self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
      self.__usePixel         = False
      self.__useTRT           = True 
      if self.__indetflags.doCosmics():
        self.__minPT            = 0.500 * Units.GeV
        self.__maxPrimaryImpact = 1000. * Units.mm 
        self.__maxZImpact       = 10000. * Units.mm 
        self.__maxHoles         = 3
        self.__maxPixelHoles    = self.__maxHoles # --> no effect
        self.__maxSctHoles      = self.__maxHoles # --> no effect
        self.__maxShared        = 3
        self.__roadWidth        = 60.
        self.__seedFilterLevel  = 3 # 2 ?
        self.__nHolesMax        = self.__maxHoles
        self.__nHolesGapMax     = 2*self.__maxDoubleHoles      
        self.__Xi2max           = 60.0
        self.__Xi2maxNoAdd      = 100.0
        self.__nWeightedClustersMin = 6
  #    elif rec.Commissioning():
  #      self.__minClusters             = 7               # Igor 6, was 7
  #      self.__maxHoles                = 5               # was 5
  #      self.__maxSctHoles             = 5               # was 5
  #      self.__maxDoubleHoles          = 4               # was 2
  #      self.__maxPrimaryImpact        = 50.0 * Units.mm # low lumi
  #      self.__maxZImpact              = 500.0 * Units.mm


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

  def maxPT( self ) :
    return self.__maxPT

  def minPTBrem( self ) :
    return self.__minPTBrem

  def maxPrimaryImpact( self ) :
    return self.__maxPrimaryImpact

  def maxSecondaryImpact( self ) :
    return self.__maxSecondaryImpact

  def maxZImpact( self ) :
    return self.__maxZImpact

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

  def minTRTonTrk( self ) :
    return self.__minTRTonTrk

  def useParameterizedTRTCuts( self ) :
    return self.__useParameterizedTRTCuts

  def useNewParameterizationTRT( self ) :
    return self.__useNewParameterizationTRT

  def minSecondaryTRTonTrk( self ) :
    return self.__minSecondaryTRTonTrk

  def minTRTonly( self ) :
    return self.__minTRTonly

  def maxTRTonlyShared( self ) :
    return self.__maxTRTonlyShared

  def minTRTonlyMinPt( self ) :
    return self.__minTRTonlyMinPt

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

  def useTRT( self ) :
    return self.__useTRT

  def RegSelZmax( self ) :
    return self.__RegSelZmax

  def setRegSelZmax( self, val):
    self.__RegSelZmax = val

  def printInfo( self ) :
    print '****** Inner Detector Trigger Track Reconstruction Cuts *********************************'
    print '*'
    print '* SETUP is  : ',self.__mode
    print '* extension : ',self.__extension
    print '*'
    print '* Pixel used                  :  ', self.__usePixel
    print '* SCT used                    :  ', self.__useSCT
    print '* TRT used                    :  ', self.__useTRT
    print '*'  
    print '* min pT                      : ', self.__minPT, ' MeV'
    print '* max Z IP                    : ', self.__maxZImpact, ' mm'
    print '* max eta                     : ', self.__maxEta
    print '*'
    print '* NewTracking cuts:'
    print '* -----------------'
    print '* max Rphi IP (primaries)     : ', self.__maxPrimaryImpact, ' mm'
    print '* min number of clusters      :  ', self.__minClusters
    print '* min number of pixel hits    :  ', self.__minPixel
    print '* min number of NOT shared    :  ', self.__minSiNotShared
    print '* max number of shared        :  ', self.__maxShared
    print '* max number of Si holes      :  ', self.__maxHoles
    print '* max number of Pixel holes   :  ', self.__maxPixelHoles
    print '* max number of SCT holes     :  ', self.__maxSctHoles
    print '* max number of double holes  :  ', self.__maxDoubleHoles
    print '*'
    if self.__useTRT:
      print '* useParameterizedTRTCuts     :  ', self.__useParameterizedTRTCuts
      print '* useNewParameterizationTRT   :  ', self.__useNewParameterizationTRT
      print '* min TRT on track extension  :  ', self.__minTRTonTrk
      print '*'
    print '* seed filter level           :  ', self.__seedFilterLevel  
    print '* maximal R of SP for seeding :  ', self.__radMax 
    print '* max holes in pattern        :  ', self.__nHolesMax
    print '* max holes gap in pattern    :  ', self.__nHolesGapMax
    print '* Xi2 max                     :  ', self.__Xi2max
    print '* Xi2 max no add              :  ', self.__Xi2maxNoAdd
    print '*'  
    print '* nWeightedClustersMin        :  ', self.__nWeightedClustersMin 
    print '*'
    print '* RegionSelector z limit      :  ', self.__RegSelZmax

    if self.__useSCT and self.__useTRT:
      print '* BackTracking cuts:'
      print '* ------------------'
      print '* max Rphi IP (secondaries)   :  ', self.__maxSecondaryImpact, ' mm'
      print '* min number of clusters      :  ', self.__minSecondaryClusters
      print '* min number of NOT shared    :  ', self.__minSecondarySiNotShared
      print '* max number of shared        :  ', self.__maxSecondaryShared
      print '* max number of Si holes      :  ', self.__maxSecondaryHoles
      print '* max number of Pixel holes   :  ', self.__maxSecondaryPixelHoles
      print '* max number of SCT holes     :  ', self.__maxSecondarySCTHoles
      print '* max number of double holes  :  ', self.__maxSecondaryDoubleHoles
      print '* min TRT on track            :  ', self.__minSecondaryTRTonTrk
      print '* max holes in pattern        :  ', self.__SecondarynHolesMax
      print '* max holes gap in pattern    :  ', self.__SecondarynHolesGapMax
      print '* Xi2 max                     :  ', self.__SecondaryXi2max
      print '* Xi2 max no add              :  ', self.__SecondaryXi2maxNoAdd
      print '*'  
    if self.__useTRT:
      print '* TRT only cuts:'
      print '* --------------'
      print '* min TRT only hits           :  ', self.__minTRTonly
      print '* max TRT shared fraction     :  ', self.__maxTRTonlyShared
      print '* min TRT only min pt         :  ', self.__minTRTonlyMinPt, ' MeV'
      print '*'
    print '************************************************************************************'

