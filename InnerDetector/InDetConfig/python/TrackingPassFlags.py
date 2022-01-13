#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import AthenaCommon.SystemOfUnits as Units

def select( selInd, valuesmap ):
    for k,v in valuesmap.items():    
        ranges = [int(x) for x in k.split('-') if x != '']
        if len(ranges) == 2:
            if ranges[0] <= selInd and selInd <= ranges[1]: return v
        if len(ranges) == 1 and k.startswith('-'):
            if selInd <= ranges[0]: return v
        if len(ranges) == 1 and k.endswith('-'):
            if ranges[0] <= selInd: return v
    raise RuntimeError("No value can be selected from ranges {} given key {}".format( valuesmap.keys(), selInd ))

def minPT_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-1':   0.1 * Units.GeV,
    '2-13': 0.4 * Units.GeV,
    '14-':  0.5 * Units.GeV } )

def minSecondaryPT_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-1':   0.4 * Units.GeV,
    '2-18': 1.0 * Units.GeV,
    '19-': 3.0 * Units.GeV } )

def minTRTonlyPt_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-1':   0.4 * Units.GeV,
    '2-5': 1.0 * Units.GeV,
    '6-': 2.0 * Units.GeV, } )

def minClusters_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-14':  7,
    '15-':  8 } )

def maxHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-7':  3,
    '8-':  2 } )

def maxPixelHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-7':  2,
    '8-':  1 } )

def maxPrimaryImpact_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-15':  10.0 * Units.mm,
    '16-':  5.0 * Units.mm } )

def maxZImpact_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-8':  320.0 * Units.mm,
    '9-16':  250 * Units.mm,
    '17-':  200.0 * Units.mm } )

def nHolesMax_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-11':  3,
    '12-':  2 } )

def nHolesGapMax_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-11':  3,
    '12-':  2 } )

def Xi2max_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-11':  15.0,
    '12-':  9.0 } )

def Xi2maxNoAdd_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-11':  35.0,
    '12-':  25.0 } )

def seedFilterLevel_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-4':  1,
    '5-':  2 } )

def maxdImpactPPSSeeds_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-3':  1.7,
    '4-':  2.0 } )

def maxdImpactSSSSeeds_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-3':  1000.0,
    '4-16':  20.0,
    '17-': 5.0 * Units.mm } )

def doZBoundary_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-9':  False,
    '10-':  True } )

def TRTSegFinderPtBins_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-1':  70,
    '2-':  50 } )

def excludeUsedTRToutliers_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-5':  False,
    '6-':  True } )

def useParameterizedTRTCuts_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
                   {'-2':  False,
                    '3-':  True } )

def useNewParameterizationTRT_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-2':  False,
    '3-':  True } )

def minSecondaryTRTonTrk_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  10,
    '7-':  15 } )

def minSecondaryTRTPrecFrac_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  0.0,
    '7-':  0.3 } )

def maxSecondaryHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  2,
    '7-':  1 } )

def maxSecondaryPixelHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  2,
    '7-':  1 } )

def maxSecondarySCTHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  2,
    '7-':  1 } )

def maxSecondaryDoubleHoles_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  1,
    '7-':  0 } )

def rejectShortExtensions_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  False,
    '7-':  True } )

def SiExtensionCuts_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-6':  False,
    '7-':  True } )

def RoISeededBackTracking_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-12':  False,
    '13-':  True } )

def roadWidth_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-16':  20.0,
    '17-':  12.0 } )

def keepAllConfirmedPixelSeeds_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-17':  False,
    '18-':  True } )

def minRoIClusterEt_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-18':  0.0,
    '19-':  6000. * Units.MeV } )

def maxSeedsPerSP_Pixels_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-17':  5,
    '18-':   1 } )

def maxSeedsPerSP_Strips_ranges( inflags ):
    return select( inflags.InDet.Tracking.cutLevel,
    {'-17':  5,
    '18-':   5 } )

################################################################
    ## create set of tracking cut flags
################################################################
def createTrackingPassFlags():
    #from InDetConfig.InDetConfigFlags import createInDetConfigFlags
    #icf = createInDetConfigFlags()
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    icf = AthConfigFlags()

    icf.addFlag("extension", "" ) ### for extension

    icf.addFlag("usePrdAssociationTool", False)
    icf.addFlag("isLowPt", False)
    icf.addFlag("useTIDE_Ambi", lambda pcf: pcf.InDet.Tracking.doTIDE_Ambi)

    icf.addFlag("minPT", minPT_ranges )
    icf.addFlag("minSecondaryPt", minSecondaryPT_ranges ) #Pt cut for back tracking + segment finding for these
    icf.addFlag("minTRTonlyPt", minTRTonlyPt_ranges ) #Pt cut for TRT only
    icf.addFlag("pT_SSScut", -1. ) # off

    # --- first set kinematic defaults
    icf.addFlag("maxPT", 1000.0 * Units.TeV) # off!
    icf.addFlag("minEta", -1) # off!
    icf.addFlag("maxEta", 2.7)


    # --- cluster cuts
    icf.addFlag("minClusters", lambda pcf:
                    3 if (pcf.Detector.EnablePixel and not pcf.Detector.EnableSCT) else
                    6  if (pcf.Detector.EnableSCT and not pcf.Detector.EnablePixel) else
                    6 if pcf.InDet.Tracking.doInnerDetectorCommissioning else
                    minClusters_ranges( pcf ) )

    icf.addFlag("minSiNotShared", lambda pcf:
                    5 if pcf.InDet.Tracking.doInnerDetectorCommissioning else
                    6)
    
    icf.addFlag("maxShared", 1) # cut is now on number of shared modules
    icf.addFlag("minPixel", 0)
    icf.addFlag("maxHoles", maxHoles_ranges )
    icf.addFlag("maxPixelHoles", maxPixelHoles_ranges )
    icf.addFlag("maxSctHoles", 2)
    icf.addFlag("maxDoubleHoles", 1)

    icf.addFlag("maxPrimaryImpact", lambda pcf: 10.0 * Units.mm if pcf.InDet.Tracking.doBLS
                else maxPrimaryImpact_ranges(pcf) )
    icf.addFlag("maxZImpact", maxZImpact_ranges )

    # --- this is for the TRT-extension
    icf.addFlag("minTRTonTrk", 9)
    icf.addFlag("minTRTPrecFrac", 0.3)

    # --- general pattern cuts for NewTracking

    icf.addFlag("radMax", 600.0 * Units.mm) # default R cut for SP in SiSpacePointsSeedMaker
    icf.addFlag("roadWidth", roadWidth_ranges )
    icf.addFlag("nHolesMax", nHolesMax_ranges )
    icf.addFlag("nHolesGapMax", nHolesGapMax_ranges ) # not as tight as 2*maxDoubleHoles
    icf.addFlag("Xi2max", Xi2max_ranges )
    icf.addFlag("Xi2maxNoAdd", Xi2maxNoAdd_ranges )
    icf.addFlag("nWeightedClustersMin", 6)

    # --- seeding
    icf.addFlag("seedFilterLevel", seedFilterLevel_ranges )
    icf.addFlag("maxTracksPerSharedPRD", 0)  ## is 0 ok for default??
    icf.addFlag("maxdImpactPPSSeeds", 2)
    icf.addFlag("maxdImpactSSSSeeds", lambda pcf: 10.0 * Units.mm if pcf.InDet.Tracking.doBLS
                else maxdImpactSSSSeeds_ranges(pcf) )
    icf.addFlag("maxZSpacePointsPPPSeeds", 2700.0 * Units.mm )
    icf.addFlag("maxZSpacePointsSSSSeeds", 2700.0 * Units.mm )
    icf.addFlag("maxSeedsPerSP_Pixels", maxSeedsPerSP_Pixels_ranges )
    icf.addFlag("maxSeedsPerSP_Strips", maxSeedsPerSP_Strips_ranges )
    icf.addFlag("keepAllConfirmedPixelSeeds", keepAllConfirmedPixelSeeds_ranges )
    icf.addFlag("keepAllConfirmedStripSeeds", False)

    # --- min pt cut for brem
    icf.addFlag("minPTBrem", 1. * Units.GeV) # off
    icf.addFlag("phiWidthBrem", 0.3 ) # default is 0.3
    icf.addFlag("etaWidthBrem", 0.2 ) # default is 0.3

    # --- Z Boundary Seeding
    icf.addFlag("doZBoundary", doZBoundary_ranges)
    
    # --------------------------------------
    # --- BACK TRACKING cuts
    # --------------------------------------

    # --- settings for segment finder
    icf.addFlag("TRTSegFinderPtBins", TRTSegFinderPtBins_ranges)
    icf.addFlag("maxSegTRTShared", 0.7)
    icf.addFlag("excludeUsedTRToutliers", excludeUsedTRToutliers_ranges)

    # --- triggers SegmentFinder and BackTracking
    icf.addFlag("useParameterizedTRTCuts", useParameterizedTRTCuts_ranges )
    icf.addFlag("useNewParameterizationTRT", useNewParameterizationTRT_ranges )
    icf.addFlag("maxSecondaryTRTShared", 0.7)

    # --- defaults for secondary tracking
    icf.addFlag("maxSecondaryImpact", 100.0 * Units.mm) # low lumi
    
    icf.addFlag("minSecondaryClusters"      , 4)
    icf.addFlag("minSecondarySiNotShared"   , 4)
    icf.addFlag("maxSecondaryShared"        , 1)  # cut is now on number of shared modules
    icf.addFlag("minSecondaryTRTonTrk"      , minSecondaryTRTonTrk_ranges)
    icf.addFlag("minSecondaryTRTPrecFrac"   , minSecondaryTRTPrecFrac_ranges)
    
    icf.addFlag("maxSecondaryHoles"         , maxSecondaryHoles_ranges)
    icf.addFlag("maxSecondaryPixelHoles"    , maxSecondaryPixelHoles_ranges)
    icf.addFlag("maxSecondarySCTHoles"      , maxSecondarySCTHoles_ranges)
    icf.addFlag("maxSecondaryDoubleHoles"   , maxSecondaryDoubleHoles_ranges)
    icf.addFlag("SecondarynHolesMax"        , 2 )
    icf.addFlag("SecondarynHolesGapMax"     , 2 )

    icf.addFlag("rejectShortExtensions"     , lambda pcf:
                    False if pcf.InDet.Tracking.doInnerDetectorCommissioning else
                    rejectShortExtensions_ranges( pcf ) ) # extension finder in back tracking
                    
    icf.addFlag("SiExtensionCuts"           , SiExtensionCuts_ranges) # cut in Si Extensions before fit

    # --- pattern cuts for back tracking
    icf.addFlag("SecondaryXi2max"           , 15.0)
    icf.addFlag("SecondaryXi2maxNoAdd"      , 50.0)

    # --- run back tracking and TRT only in RoI seed regions
    icf.addFlag("RoISeededBackTracking"     , RoISeededBackTracking_ranges and ( lambda pcf : pcf.Detector.EnableCalo ) )
    icf.addFlag("minRoIClusterEt"           , minRoIClusterEt_ranges)

    icf.addFlag("usePixel"       		  , lambda pcf : pcf.Detector.EnablePixel )
    icf.addFlag("useTRT"        		  , lambda pcf : pcf.Detector.EnableTRT )
    icf.addFlag("useSCT"        		  , lambda pcf : pcf.Detector.EnableSCT )
    icf.addFlag("useSCTSeeding"        	  	  , True )

    icf.addFlag("useITkPixel"       		  , lambda pcf : pcf.Detector.EnableITkPixel )
    icf.addFlag("useITkStrip"        		  , lambda pcf : pcf.Detector.EnableITkStrip )
    icf.addFlag("useITkStripSeeding"        	  , True )

    # --------------------------------------
    # --- TRT Only TRACKING cuts
    # --------------------------------------
    
    # --- TRT only
    icf.addFlag("minTRTonly"                , 15)
    icf.addFlag("maxTRTonlyShared"          , 0.7)
    icf.addFlag("useTRTonlyParamCuts"       , True)
    icf.addFlag("useTRTonlyOldLogic"        , False)
    icf.addFlag("TrkSel.TRTTrksEtaBins"                 , [999, 999, 999, 999, 999, 999, 999, 999, 999, 999])  # eta bins (10) for eta-dep cuts on TRT conversion tracks
    icf.addFlag("TrkSel.TRTTrksMinTRTHitsThresholds"    , [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0])  # eta-dep nTRT for TRT conversion tracks (> 15 is applied elsewhere)
    icf.addFlag("TrkSel.TRTTrksMinTRTHitsMuDependencies", [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0])  # eta-dep nTRT, mu dependence for TRT conversion tracks

    return icf

### RobustReco mode ####################
def createRobustRecoTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = ""

    icf.minClusters             = 7
    icf.maxHoles                = 5
    icf.maxPixelHoles           = 2
    icf.maxSctHoles             = 5
    icf.maxDoubleHoles          = 4

    icf.maxZImpact              = 500*Units.mm

    icf.maxSecondaryHoles       = 5
    icf.maxSecondaryPixelHoles  = 5
    icf.maxSecondarySCTHoles    = 5
    icf.maxSecondaryDoubleHoles = 2

### ITk mode ####################
def createITkTrackingPassFlags():
    icf = createTrackingPassFlags()   
    icf.extension               = ""

    # --- ITk flags
    icf.addFlag("useEtaDepCuts"             , True)
    icf.addFlag("etaBins"                   , [-1.0, 2.0, 2.6, 4.0])
    icf.addFlag("minPTSeed"                 , 0.9 * Units.GeV)
    icf.addFlag("maxPrimaryImpactSeed"      , 2.0 * Units.mm)
    icf.addFlag("maxZImpactSeed"            , 200.0 * Units.mm)

    icf.maxEta                  = 4.0
    icf.minPT                   = [0.9 * Units.GeV, 0.4 * Units.GeV, 0.4 * Units.GeV]

    # --- cluster cuts
    icf.minClusters             = [9, 8, 7]
    icf.minSiNotShared          = [7, 6, 5]
    icf.maxShared               = [2]
    icf.minPixel                = [1]
    icf.maxHoles                = [2]
    icf.maxPixelHoles           = [2]
    icf.maxSctHoles             = [2]
    icf.maxDoubleHoles          = [1]
    icf.maxPrimaryImpact        = [2.0 * Units.mm, 2.0 * Units.mm, 10.0 * Units.mm]
    icf.maxZImpact              = [200.0 * Units.mm]

    # --- general pattern cuts for NewTracking
    icf.nHolesMax               = icf.maxHoles
    icf.nHolesGapMax            = icf.maxHoles

    icf.Xi2max                  = [9.0]
    icf.Xi2maxNoAdd             = [25.0]
    icf.nWeightedClustersMin    = [6]

    # --- seeding
    icf.maxdImpactSSSSeeds      = [20.0 * Units.mm]
    icf.radMax                  = 1100. * Units.mm

    # --- min pt cut for brem
    icf.minPTBrem               = [1000.0 * Units.mm]
    icf.phiWidthBrem            = [0.3]
    icf.etaWidthBrem            = [0.2]

    return icf


def createITkFastTrackingPassFlags():

    icf = createITkTrackingPassFlags()

    icf.minPT                 = [1.0 * Units.GeV, 0.4 * Units.GeV, 0.4 * Units.GeV]
    icf.maxZImpact            = [150.0 * Units.mm]
    icf.minPixel              = [3]
    icf.nHolesMax             = [1]
    icf.nHolesGapMax          = [1]
    icf.minPTSeed             = 1.0 * Units.GeV
    icf.maxZImpactSeed        = 150.0 * Units.mm
    icf.useITkStripSeeding    = False

    return icf

### ITk LRT mode ####################
def createITkLargeD0TrackingPassFlags():

    icf = createTrackingPassFlags()
    icf.extension               = "LargeD0"
    icf.usePrdAssociationTool = True

    icf.useEtaDepCuts      = True
    icf.maxPT              = [1.0 * Units.TeV]
    icf.minPT              = [1000 * Units.MeV]
    icf.maxEta             = 4.0
    icf.etaBins            = [-1.0, 4.0]
    icf.maxPrimaryImpact   = [300 * Units.mm]
    icf.maxZImpact         = [500 * Units.mm]
    icf.maxSecondaryImpact = [300.0 * Units.mm]
    icf.minSecondaryPt     = [1000 * Units.MeV]
    icf.minClusters        = [8]
    icf.minSiNotShared     = [6]
    icf.maxShared          = [2]
    icf.minPixel           = [0]
    icf.maxHoles           = [1]
    icf.maxPixelHoles      = [1]
    icf.maxSctHoles        = [1]
    icf.maxDoubleHoles     = [0]
    icf.maxZImpactSeed     = 500.0 * Units.mm
    icf.maxPrimaryImpactSeed = 300.0 * Units.mm
    icf.minPTSeed          = 1000 * Units.MeV
    icf.radMax             = 1100. * Units.mm
    icf.nHolesMax          = icf.maxHoles
    icf.nHolesGapMax       = icf.maxHoles
    icf.seedFilterLevel    = 1
    icf.maxTracksPerSharedPRD = 2

    icf.roadWidth          = 5
    icf.doZBoundary        = True

    # --- seeding
    icf.maxdImpactSSSSeeds       = [300.0 * Units.mm]
    icf.maxZSpacePointsPPPSeeds  = 500 * Units.mm

    # --- min pt cut for brem
    icf.minPTBrem                = [1000.0 * Units.mm]
    icf.phiWidthBrem             = [0.3]
    icf.etaWidthBrem             = [0.2]

    icf.Xi2max                  = [9.0]
    icf.Xi2maxNoAdd             = [25.0]
    icf.nWeightedClustersMin    = [6]

    return icf

def createITkLargeD0FastTrackingPassFlags():

    icf = createITkLargeD0TrackingPassFlags()

    icf.maxEta             = 2.4
    icf.etaBins            = [-1.0, 2.4]
    icf.minSecondaryPt     = [5.0 * Units.GeV]
    icf.minPT              = [5.0 * Units.GeV]
    icf.minPTSeed          = 5.0 * Units.GeV
    icf.nWeightedClustersMin = [8]
    icf.maxPrimaryImpact   = [150 * Units.mm]
    icf.maxSecondaryImpact = [150 * Units.mm]
    icf.maxPrimaryImpactSeed = 150. * Units.mm
    icf.maxdImpactSSSSeeds = [150.0 * Units.mm]
    icf.maxZImpact         = [200 * Units.mm]
    icf.maxZImpactSeed     = 200. * Units.mm
    icf.radMax             = 400. * Units.mm

    return icf

### HighPileUP mode ####################
def createHighPileupTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = "HighPileup"
    icf.seedFilterLevel         = 1
    icf.minPT                   = 0.900 * Units.GeV
    icf.minClusters             = 9
    icf.maxPixelHoles           = 0

    return icf

## MinBias mode ########################
def createMinBiasTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension                 = "MinBias"
    icf.minPT = lambda pcf: (0.3 if  pcf.InDet.Tracking.doHIP300 else 0.1) * Units.GeV

    icf.minClusters               = 5
    icf.minSecondaryPt            = 0.4 * Units.GeV  # Pt cut for back tracking + segment finding for these
    icf.minTRTonlyPt              = 0.4 * Units.GeV  # Pt cut for TRT only
    icf.TRTSegFinderPtBins        = 50
    icf.maxdImpactSSSSeeds        = 20.0    # apply cut on SSS seeds
    icf.excludeUsedTRToutliers    = False   # TRT outliers are added to the exclusion list
    icf.useTRTonlyOldLogic        = True    # turn off ole overlap logic to reduce number of hits
    icf.maxSecondaryImpact        = 100.0 * Units.mm # low lumi

    return icf

## LargeD0 mode ########################
def createLargeD0TrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension          = "LargeD0"
    icf.usePrdAssociationTool = True
    icf.maxPT              = 1.0 * Units.TeV
    icf.minPT              = 900 * Units.MeV
    icf.maxEta             = 5
    icf.maxPrimaryImpact   = 300.0 * Units.mm
    icf.maxZImpact         = 1500.0 * Units.mm
    icf.maxSecondaryImpact = 300.0 * Units.mm
    icf.minSecondaryPt     = 500.0 * Units.MeV
    icf.minClusters        = 7
    icf.minSiNotShared     = 5
    icf.maxShared          = 2   # cut is now on number of shared modules
    icf.minPixel           = 0
    icf.maxHoles           = 2
    icf.maxPixelHoles      = 1
    icf.maxSctHoles        = 2
    icf.maxDoubleHoles     = 1
    icf.radMax             = 600. * Units.mm
    icf.nHolesMax          = icf.maxHoles
    icf.nHolesGapMax       = icf.maxHoles # not as tight as 2*maxDoubleHoles
    icf.seedFilterLevel    = 1
    icf.maxTracksPerSharedPRD = 2

    return icf


## R3LargeD0 mode ########################
def createR3LargeD0TrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension          = "R3LargeD0"
    icf.usePrdAssociationTool = True
    icf.maxPT              = 1.0 * Units.TeV
    icf.minPT              = 1.0 * Units.GeV                                                                                    
    icf.maxEta             = 3                                                                                                        
    icf.maxPrimaryImpact   = 300.0 * Units.mm
    icf.maxZImpact         = 500 * Units.mm    
    icf.maxSecondaryImpact = 300.0 * Units.mm  
    icf.minSecondaryPt     = 1000.0 * Units.MeV 
    icf.minClusters        = 8                  
    icf.minSiNotShared     = 6                 
    icf.maxShared          = 2   # cut is now on number of shared modules                                                                                  
    icf.minPixel           = 0
    icf.maxHoles           = 2
    icf.maxPixelHoles      = 1
    icf.maxSctHoles        = 1  
    icf.maxDoubleHoles     = 0  
    icf.radMax             = 600. * Units.mm
    icf.nHolesMax          = icf.maxHoles
    icf.nHolesGapMax       = 1 
    icf.seedFilterLevel    = 1  
    icf.maxTracksPerSharedPRD   = 2
    icf.Xi2max                  = 9.0  
    icf.Xi2maxNoAdd             = 25.0 
    icf.roadWidth               = 5. 
    icf.nWeightedClustersMin    = 8   
    icf.maxdImpactSSSSeeds      = 300.0
    icf.doZBoundary             = True
    icf.keepAllConfirmedStripSeeds = True
    icf.maxSeedsPerSP_Strips = 1
    icf.usePixel             = False

    return icf

## LowPtLargeD0 mode ########################
def createLowPtLargeD0TrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension          = "LowPtLargeD0"
    icf.usePrdAssociationTool = True
    icf.maxPT              = 1.0 * Units.TeV
    icf.minPT              = 100 * Units.MeV
    icf.maxEta             = 5
    icf.maxPrimaryImpact   = 300.0 * Units.mm
    icf.maxZImpact         = 1500.0 * Units.mm
    icf.maxSecondaryImpact = 300.0 * Units.mm
    icf.minSecondaryPt     = 400.0 * Units.MeV
    icf.minClusters        = 5
    icf.minSiNotShared     = 5
    icf.maxShared          = 2   # cut is now on number of shared modules
    icf.minPixel           = 0
    icf.maxHoles           = 2
    icf.maxPixelHoles      = 1
    icf.maxSctHoles        = 2
    icf.maxDoubleHoles     = 1
    icf.radMax             = 600. * Units.mm
    icf.nHolesMax          = icf.maxHoles
    icf.nHolesGapMax       = icf.maxHoles
    icf.seedFilterLevel    = 1
    icf.maxTracksPerSharedPRD = 2

    return icf

## LowPt mode ########################
def createLowPtTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "LowPt"
    icf.usePrdAssociationTool = True
    icf.isLowPt          = True
    icf.maxPT = lambda pcf: (1e6  if pcf.InDet.Tracking.doMinBias else pcf.InDet.Tracking.Pass.minPT + 0.3) * Units.GeV
    icf.minPT            = 0.050 * Units.GeV
    icf.minClusters      = 5
    icf.minSiNotShared   = 4
    icf.maxShared        = 1   # cut is now on number of shared modules
    icf.minPixel         = 2   # At least one pixel hit for low-pt (assoc. seeded on pixels!)
    icf.maxHoles         = 2
    icf.maxPixelHoles    = 1
    icf.maxSctHoles      = 2
    icf.maxDoubleHoles   = 1
    icf.radMax           = 600. * Units.mm
    icf.nHolesMax        = icf.maxHoles
    icf.nHolesGapMax     = icf.maxHoles # not as tight as 2*maxDoubleHoles
    icf.maxPrimaryImpact = lambda pcf: 100.0 * Units.mm if pcf.InDet.Tracking.doMinBias else maxPrimaryImpact_ranges( pcf )
    
    return icf

## ITkConversionFinding mode ########################
def createITkConversionFindingTrackingPassFlags(): #To be updated
    icf = createTrackingPassFlags()
    icf.extension               = "ConversionFinding"
    icf.usePrdAssociationTool = True

    icf.useEtaDepCuts           = True
    icf.etaBins                 = [-1.0,4.0]
    icf.minPT                   = [0.9 * Units.GeV]
    icf.maxPrimaryImpact        = [10.0 * Units.mm]
    icf.maxZImpact              = [150.0 * Units.mm]
    icf.minClusters             = [6]
    icf.minSiNotShared          = [6]
    icf.maxShared               = [0]
    icf.minPixel                = [0]
    icf.maxHoles                = [0]
    icf.maxPixelHoles           = [1]
    icf.maxSctHoles             = [2]
    icf.maxDoubleHoles          = [1]

    icf.nHolesMax               = icf.maxHoles
    icf.nHolesGapMax            = icf.maxHoles
    icf.nWeightedClustersMin    = [6]
    icf.maxdImpactSSSSeeds      = [20.0 * Units.mm]
    icf.radMax                  = 1000. * Units.mm
    icf.doZBoundary             = False

    icf.Xi2max                  = [9.0]
    icf.Xi2maxNoAdd             = [25.0]
    icf.minPTBrem               = [1000.0 * Units.mm]
    icf.phiWidthBrem            = [0.3]
    icf.etaWidthBrem            = [0.2]

    return icf

## VeryLowPt mode ########################
def createVeryLowPtTrackingPassFlags():
    icf = createTrackingPassFlags() #TODO consider using createLowPtTrackingPassFlags as a base here
    icf.extension        = "VeryLowPt"
    icf.usePrdAssociationTool = True
    icf.isLowPt          = True
    icf.maxPT            = lambda pcf : (1e6 if pcf.InDet.Tracking.doMinBias  else  pcf.InDet.Tracking.Pass.minPT + 0.3) * Units.GeV # some overlap
    icf.minPT            = 0.050 * Units.GeV
    icf.minClusters      = 3
    icf.minSiNotShared   = 3
    icf.maxShared        = 1   # cut is now on number of shared modules
    icf.minPixel         = 3   # At least one pixel hit for low-pt (assoc. seeded on pixels!)
    icf.maxHoles         = 1
    icf.maxPixelHoles    = 1
    icf.maxSctHoles      = 1
    icf.maxDoubleHoles   = 0
    icf.nHolesMax        = 1
    icf.nHolesGapMax     = 1 # not as tight as 2*maxDoubleHoles
    icf.radMax           = 600. * Units.mm # restrivt to pixels

    return icf

## ForwardTracks mode ########################
def createForwardTracksTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "ForwardTracks"
    icf.usePrdAssociationTool = True
    icf.useTIDE_Ambi     = False
    icf.minEta           = 2.4 # restrict to minimal eta
    icf.maxEta           = 2.7
    icf.minPT            = 2 * Units.GeV
    icf.minClusters      = 3
    icf.minSiNotShared   = 3
    icf.maxShared        = 1
    icf.minPixel         = 3
    icf.maxHoles         = 1
    icf.maxPixelHoles    = 1
    icf.maxSctHoles      = 1
    icf.maxDoubleHoles   = 0
    icf.nHolesMax        = icf.maxHoles
    icf.nHolesGapMax     = icf.maxHoles
    icf.radMax           = 600. * Units.mm
    icf.useTRT           = False # no TRT for forward tracks

    return icf

## BeamGas mode ########################
def createBeamGasTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "BeamGas"
    icf.usePrdAssociationTool = True
    icf.minPT            = 0.500 * Units.GeV
    icf.maxPrimaryImpact = 300. * Units.mm
    icf.maxZImpact       = 2000. * Units.mm
    icf.minClusters      = 6
    icf.maxHoles         = 3
    icf.maxPixelHoles    = 3
    icf.maxSctHoles      = 3
    icf.maxDoubleHoles   = 1
    icf.nHolesMax        = 3
    icf.nHolesGapMax     = 3 # not as tight as 2*maxDoubleHoles

    return icf

## VtxLumi mode ########################
def createVtxLumiTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = "VtxLumi"
    icf.seedFilterLevel         = 1
    icf.minPT                   = 0.900 * Units.GeV
    icf.minClusters             = 7
    icf.maxPixelHoles           = 1
    icf.radMax                  = 600. * Units.mm
    icf.nHolesMax               = 2
    icf.nHolesGapMax            = 1
    icf.useTRT                  = False

    return icf

## VtxBeamSpot mode ########################
def createVtxBeamSpotTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = "VtxBeamSpot"
    icf.seedFilterLevel         = 1
    icf.minPT                   = 0.900 * Units.GeV
    icf.minClusters             = 9
    icf.maxPixelHoles           = 0
    icf.radMax                  = 320. * Units.mm
    icf.nHolesMax               = 2
    icf.nHolesGapMax            = 1
    icf.useTRT                  = False

    return icf

## Cosmics mode ########################
def createCosmicsTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "Cosmics"
    icf.minPT            = 0.500 * Units.GeV
    icf.maxPrimaryImpact = 1000. * Units.mm
    icf.maxZImpact       = 10000. * Units.mm
    icf.minClusters      = 4
    icf.minSiNotShared   = 4
    icf.maxHoles         = 3
    icf.maxPixelHoles    = 3
    icf.maxSctHoles      = 3
    icf.maxDoubleHoles   = 1
    icf.minTRTonTrk      = 15
    icf.minTRTonly       = 15
    icf.roadWidth        = 60.
    icf.seedFilterLevel  = 3
    icf.Xi2max           = 60.0
    icf.Xi2maxNoAdd      = 100.0
    icf.nWeightedClustersMin = 8
    icf.nHolesMax        = 3
    icf.nHolesGapMax     = 3 # not as tight as 2*maxDoubleHoles

    return icf

## Heavyion mode #######################
def createHeavyIonTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "HeavyIon"
    icf.maxZImpact       = 200. * Units.mm
    icf.minClusters      = 9
    icf.minSiNotShared   = 7
    icf.maxShared        = 2 # was 1, cut is now on number of shared modules

    icf.nHolesMax        = 0
    icf.nHolesGapMax     = 0
    icf.Xi2max           = 6.
    icf.Xi2maxNoAdd      = 10.

    # CutLevel dependendent flags:
    # CutLevel 3 MinBias
    # CutLevel 4  # ==CutLevel 2 with loosened hole cuts and chi^2 cuts
    # CutLevel 5 # ==CutLevel 3 with loosened hole cuts and chi^2 cuts    
    icf.seedFilterLevel = lambda pcf: 2 if pcf.InDet.Tracking.cutLevel >= 2 else 1
    
    icf.maxdImpactSSSSeeds =  lambda pcf: \
                              20. if pcf.InDet.Tracking.cutLevel >= 2 else 1000.
    
    icf.minPT              = lambda pcf: \
                             0.3 *Units.GeV  if pcf.InDet.Tracking.cutLevel in [3, 5] else 0.5 * Units.GeV
    icf.useParameterizedTRTCuts = lambda pcf: \
                                  False if pcf.InDet.Tracking.cutLevel >= 3 else True #Make these false on all HI cut levels >=3, since standard cut levels set it true from levels >=3
    icf.useNewParameterizationTRT = lambda pcf: \
                                    False if pcf.InDet.Tracking.cutLevel >= 3 else True

    #set this to 1.7 for all HI cut levels >=4, since standard cut levels set it to 2.0 from levels >=4. Not sure it has any effect, since we don't usually run mixed seeds (also true for HI?)
    icf.maxdImpactPPSSeeds = lambda pcf: \
                             1.7 if pcf.InDet.Tracking.cutLevel >= 4 else True
    
    icf.maxHoles = lambda pcf: 2 if pcf.InDet.Tracking.cutLevel in [4, 5] else 0
    icf.maxPixelHoles = lambda pcf: 1 if pcf.InDet.Tracking.cutLevel in [4, 5] else 0
    icf.maxSctHoles = lambda pcf: 1 if pcf.InDet.Tracking.cutLevel in [4, 5] else 0
    icf.maxDoubleHoles   = 0    
    icf.Xi2max           = lambda pcf: 9. if pcf.InDet.Tracking.cutLevel in [4, 5] else 6.
    icf.Xi2maxNoAdd      = lambda pcf: 25. if pcf.InDet.Tracking.cutLevel in [4, 5] else 10.
    icf.radMax           = 600. * Units.mm # restrict to pixels + first SCT layer
    icf.useTRT           = False

    return icf

### Pixel mode ###############################################
def createPixelTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "Pixel"
    icf.isLowPt          = lambda pcf : pcf.Tracking.doMinBias

    def _minPt( pcf ):
        if pcf.Beam.Type == "cosmics":
            return 0.5 * Units.GeV
        if pcf.Reco.EnableHI:
            return 0.1 * Units.GeV
        if pcf.InDet.Tracking.doMinBias:
            if pcf.InDet.Tracking.doHIP300:
                return 0.300 * Units.GeV
            else:
                return 0.05 * Units.GeV
        return 0.1 * Units.GeV
    
    icf.minPT            = lambda pcf : _minPt    
    icf.minClusters      = 3

    def _pick( default, hion, cosmics):
        def _internal( pcf ):
            if pcf.Reco.EnableHI:
                return hion
            if pcf.Beam.Type == "cosmics":
                return cosmics
            return default
        return _internal
    
    icf.maxHoles         = lambda pcf : _pick( default = 1, hion = 0, cosmics = 3 )
    icf.maxPixelHoles    = lambda pcf : _pick( default = 1, hion = 0, cosmics = 3 )
    icf.maxSctHoles      = 0
    icf.maxDoubleHoles   = 0
    icf.minSiNotShared   = 3
    icf.maxShared        = 0
    icf.seedFilterLevel  = lambda pcf : _pick( default = 2, hion = 2, cosmics = 3 ) 
    icf.nHolesMax        = lambda pcf : _pick( default = 1, hion = 0, cosmics = 3 )
    icf.nHolesGapMax     = lambda pcf : _pick( default = 1, hion = 0, cosmics = 3 )
    icf.useSCT           = False
    icf.useTRT           = False
    icf.minSecondaryPt   = 3 * Units.GeV
    icf.maxPrimaryImpact = lambda pcf: 1000. * Units.mm if pcf.Beam.Type =="cosmics" else 5. * Units.mm 
    icf.roadWidth        = lambda pcf: 60.0 if pcf.Beam.Type =="cosmics" else 12.0
    icf.maxZImpact       = lambda pcf: 10000. * Units.mm if pcf.Beam.Type == "cosmics" else maxZImpact_ranges( pcf )
    icf.Xi2max           = lambda pcf: 60.0  if pcf.Beam.Type =="cosmics" else Xi2max_ranges( pcf )
    icf.Xi2maxNoAdd      = lambda pcf: 100.0  if pcf.Beam.Type =="cosmics" else Xi2maxNoAdd_ranges( pcf )
    icf.nWeightedClustersMin = 6

    return icf

########## Disappearing mode ######################
def createDisappearingTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "Disappearing"
    icf.usePrdAssociationTool = True
    icf.minPT            = 5 * Units.GeV
    icf.minClusters      = 4
    icf.maxHoles         = 0
    icf.maxPixelHoles    = 0
    icf.maxSctHoles      = 0
    icf.maxDoubleHoles   = 0
    icf.minSiNotShared   = 3
    icf.maxShared        = 0
    icf.seedFilterLevel  = 2
    icf.nHolesMax        = 0
    icf.nHolesGapMax     = 0
    icf.useSCT           = True
    icf.useTRT           = True
    icf.useSCTSeeding    = False
    icf.maxEta           = 2.2

    return icf

########## SCT mode ######################
def createSCTTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension        = "SCT"
    icf.minClusters      = 7
    icf.maxDoubleHoles   = 1
    icf.minSiNotShared   = 5
    icf.usePixel         = False
    icf.useTRT           = False


    def _pick( default, cosmics, minbias, hion):
        def _internal( pcf ):
            if pcf.Beam.Type == "cosmics":
                return cosmics
            if pcf.InDet.Tracking.doMinBias:
                if pcf.InDet.Tracking.doHIP300:
                    return hion
                else:
                    return minbias
            return default
        return _internal

    icf.minPT            = lambda pcf : _pick( default = 0.1 * Units.GeV, minbias=0.1 * Units.GeV, hion=0.3* Units.GeV, cosmics = 0.5* Units.GeV )
    icf.maxPrimaryImpact = lambda pcf: 1000. * Units.mm if pcf.Beam.Type == "cosmics" else maxPrimaryImpact_ranges( pcf )        
    icf.maxZImpact       = lambda pcf: 10000. * Units.mm if pcf.Beam.Type == "cosmics" else maxZImpact_ranges( pcf )
    maxHolesDefault = 2
    icf.maxHoles         = lambda pcf: 3 if pcf.Beam.Type == "cosmics" else maxHolesDefault
    icf.nHolesMax        = lambda pcf: 3 if pcf.Beam.Type == "cosmics" else maxHolesDefault
    icf.nHolesGapMax     = lambda pcf: 3 if pcf.Beam.Type == "cosmics" else maxHolesDefault
    icf.maxPixelHoles    = lambda pcf: 0 if pcf.Beam.Type == "cosmics" else 0
    icf.maxSctHoles      = lambda pcf: 3 if pcf.Beam.Type == "cosmics" else 2
    icf.maxShared        = 0
    icf.roadWidth        = lambda pcf: 60. if pcf.Beam.Type == "cosmics" else roadWidth_ranges( pcf )
    icf.seedFilterLevel  = lambda pcf: 3 if pcf.Beam.Type == "cosmics" else 2
    icf.Xi2max           = lambda pcf: 60.0 if pcf.Beam.Type == "cosmics" else Xi2max_ranges( pcf )
    icf.Xi2maxNoAdd      = lambda pcf: 100.0 if pcf.Beam.Type == "cosmics" else Xi2maxNoAdd_ranges( pcf )
    icf.nWeightedClustersMin = lambda pcf: 4 if pcf.InDet.Tracking.doInnerDetectorCommissioning and pcf.Beam.Type == "cosmics" else 6
    icf.minClusters      = lambda pcf: 4 if pcf.InDet.Tracking.doInnerDetectorCommissioning and pcf.Beam.Type == "cosmics" else minClusters_ranges( pcf )
    icf.minSiNotShared   = lambda pcf: 4 if pcf.InDet.Tracking.doInnerDetectorCommissioning and pcf.Beam.Type == "cosmics" else 5
    
    return icf

########## TRT subdetector tracklet cuts  ##########
def createTRTTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = "TRT"
    icf.minPT                   = 0.4 * Units.GeV
    icf.minTRTonly              = 15
    icf.maxTRTonlyShared        = 0.7

    return icf


########## TRT standalone tracklet cuts  ##########
def createTRTStandaloneTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.minSecondaryTRTPrecFrac = 0.15
    # Mu- and eta- dependent cuts on nTRT
    icf.TrkSel.TRTTrksEtaBins                  = [ 0.7,   0.8,   0.9,  1.2,  1.3,  1.6,  1.7,  1.8,  1.9,  999]  # eta bins (10) for eta-dep cuts on TRT conversion tracks
    icf.TrkSel.TRTTrksMinTRTHitsThresholds     = [  27,    18,    18,   18,   26,   28,   26,   24,   22,    0]  # eta-dep nTRT for TRT conversion tracks (> 15 is applied elsewhere)
    icf.TrkSel.TRTTrksMinTRTHitsMuDependencies = [ 0.2,  0.05,  0.05, 0.05, 0.15, 0.15, 0.15, 0.15, 0.15,    0]  # eta-dep nTRT, mu dependence for TRT conversion tracks

    return icf

########## DBM mode ################################
def createDBMTrackingPassFlags():
    icf = createTrackingPassFlags()
    icf.extension               = "DBM"
    icf.useTIDE_Ambi            = False
    icf.minEta                  = 3.05
    icf.maxEta                  = 3.45
    icf.Xi2maxNoAdd             = 10000
    icf.Xi2max                  = 10000
    icf.nWeightedClustersMin    = 0
    icf.seedFilterLevel         = 1
    icf.maxdImpactPPSSeeds      = 100000.0 * Units.mm
    icf.maxdImpactSSSSeeds      = 100000.0 * Units.mm
    icf.maxPrimaryImpact        = 100000.0 * Units.mm  # low lumi
    icf.maxZImpact              = 320000.0 * Units.mm  # Was 250 mm
    icf.maxPT            = 100000.0 * Units.GeV # some overlap
    icf.minPT            = 0.0 * Units.GeV
    icf.minClusters      = 0
    icf.minSiNotShared   = 0
    icf.maxShared        = 1000   # cut is now on number of shared modules
    icf.minPixel         = 0
    maxHolesDefault     = 0
    icf.maxHoles         = maxHolesDefault
    icf.maxPixelHoles    = 0
    icf.maxSctHoles      = 0
    icf.maxDoubleHoles   = 0
    icf.radMax           = 600000. * Units.mm
    icf.nHolesMax        = maxHolesDefault
    icf.nHolesGapMax     = maxHolesDefault
    icf.useTRT           = False
    icf.useSCT           = False
    icf.usePixel         = True

    return icf


#####################################################################
#####################################################################
#####################################################################

if __name__ == "__main__":

  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  ConfigFlags.Input.Files=defaultTestFiles.RAW
  
  from AthenaCommon.Logging import logging
  l = logging.getLogger('AthConfigFlags')
  from AthenaCommon.Constants import WARNING
  l.setLevel(WARNING)
  ConfigFlags.loadAllDynamicFlags()

  assert ConfigFlags.InDet.Tracking.cutLevel == 19 , "default cut level is wrong"
  assert ConfigFlags.InDet.Tracking.Pass.minRoIClusterEt == 6000.0 * Units.MeV, "wrong cut value {} ".format(ConfigFlags.InDet.Tracking.Pass.minRoIClusterEt)
  ConfigFlags.InDet.Tracking.cutLevel = 2
  assert ConfigFlags.InDet.Tracking.Pass.minRoIClusterEt == 0.0, "wrong cut value {} ".format(ConfigFlags.InDet.Tracking.Pass.minRoIClusterEt)
  assert ConfigFlags.InDet.Tracking.BeamGasPass.minRoIClusterEt == 0.0, "wrong cut value {}, not following cutLevel setting ".format(ConfigFlags.InDet.Tracking.BeamGasPass.minRoIClusterEt)

  assert ConfigFlags.InDet.Tracking.HeavyIonPass.minSiNotShared == 7, "wrong cut value, overwrite"
  assert ConfigFlags.InDet.Tracking.HeavyIonPass.minRoIClusterEt == 0.0, "wrong cut value, overwrite"

  print("ConfigFlags.InDet.Tracking.SCTandTRTPass.minPT",ConfigFlags.InDet.Tracking.SCTandTRTPass.minPT * 1.0)
  print("type(ConfigFlags.InDet.Tracking.SCTandTRTPass.minPT)",type(ConfigFlags.InDet.Tracking.SCTandTRTPass.minPT))

  print("ConfigFlags.InDet.Tracking.Pass.minSecondaryPt",ConfigFlags.InDet.Tracking.Pass.minSecondaryPt * 1.0)
  print("type(ConfigFlags.InDet.Tracking.Pass.minSecondaryPt)",type(ConfigFlags.InDet.Tracking.Pass.minSecondaryPt))
  
  print("ConfigFlags.InDet.Tracking.SCTandTRTPass.roadWidth",ConfigFlags.InDet.Tracking.SCTandTRTPass.roadWidth * 1.0)

  #ConfigFlags.dump()
  print( "allok" )   


