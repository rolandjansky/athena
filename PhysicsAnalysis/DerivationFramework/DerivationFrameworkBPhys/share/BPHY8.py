#====================================================================
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#   
# @file   BPHY8.py
#
# @author W. Walkowiak, <wolfgang.walkowiak@cern.ch>
#
# Based on example derivation formats.
# It requires the reductionConf flag BPHY8 in Reco_tf.py   
#
# Produces DxAODs for the B(s)->mu+mu- analysis including the reference
# channels B+->J/psiK+ and Bs->J/psiPhi:
# * For data vertex containers for all three channels are produced
#   in parallel.
# * For signal or reference channel MC the appropriate configuration
#   is set according to the dataset number (DSN).  The list associating
#   known dataset numbers to decay channels (below) needs to be adjusted
#   in case there are new MC samples with new numbers.
#
#====================================================================
# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# for debugging output
from pprint import pprint
from egammaRec.Factories import getPropertyValue

# more debug messages
## svcMgr.MessageSvc.debugLimit = 5000000
## svcMgr.MessageSvc.debugLimit = 5000

# Set up Bmumu configuration (metadata) tracking tool.
# This tool imports our defaults from Bmumu_metadata.cxx.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__Bmumu_metadata
BPHY8_MetaDataTool = DerivationFramework__Bmumu_metadata( 
    name              = "BPHY8_metadata",
    DerivationName    = "BPHY8",
    OutputLevel       = WARNING,
    # verbosity of python script (0 - 10)
    verbose           = 10
)
# local shorthand and ensure default contents of __slots__ dict are
# available as attributes
from DerivationFrameworkBPhys.BPhysPyHelpers import BPhysEnsureAttributes
BPHY8cf = BPhysEnsureAttributes(BPHY8_MetaDataTool)

# add it to the ToolSvc chain
ToolSvc += BPHY8_MetaDataTool

print BPHY8_MetaDataTool
pprint(BPHY8_MetaDataTool.properties())

# data or simulation?
if globalflags.DataSource() == 'geant4':
    BPHY8cf.isSimulation = True

# project tag
BPHY8cf.projectTag = rec.projectName()

# release 21 or newer?
from PyJobTransforms.trfUtils import releaseIsOlderThan
BPHY8cf.isRelease21 = not releaseIsOlderThan(21,0)

print "BPHY8: isSimulation = %s" % BPHY8cf.isSimulation
print "BPHY8: project tag  = %s" % BPHY8cf.projectTag
print "BPHY8: DerivationFrameworkIsMonteCarlo = %s" % \
    DerivationFrameworkIsMonteCarlo
print "BPHY8: release 21 or up: %s" % BPHY8cf.isRelease21

#====================================================================
# MC dataset categories (lists of dataset numbers)
#====================================================================
BPHY8cf.mcBsmumu          = [300203,300306,300307,300308,300402,300426,300430,300432,300433,300434,300446,300447]
BPHY8cf.mcBplusJpsiKplus  = [300203,300306,300307,300308,300997,300999,300404,300405,300406,300437]
BPHY8cf.mcBsJpsiPhi       = [300203,300306,300307,300308,300401,300438,300448,300449]
BPHY8cf.mcBplusJpsiPiplus = [300406,300437]
BPHY8cf.mcBhh             = [300431]
BPHY8cf.mcNoTrigger       = [300446,300447,300448,300449]

#====================================================================
# Defaults for BPHY8 configuration
#====================================================================
# Blind search?
BPHY8cf.doBmumuBlinding = True
#
# Thinning level
# 0 - simple vertex thinning using Thin_vtxTrk.
# 1 - thinning subdecay vertex candidates using Thin_vtxTrk.
# 2 - thinning subdecay vertex candidates using BmumuThinningTool.
#
BPHY8cf.thinLevel = 2
#
# Track particle collection
BPHY8cf.TrkPartContName = "InDetTrackParticles"
#
# Primary vertex collection
BPHY8cf.PVContName = "PrimaryVertices"
#
# Muon collection
BPHY8cf.MuonCollection = "Muons"
#
# Apply MCP calibration to muons? (only for MC)
#
# Options:
# 0 : none
# 1 : apply calibration on-the-fly inside the muon container
#     (calibration effects B candidate building)
# 2 : apply calibration and store it in an extra container for
#     calibrated muons
#     (calibration does not effect B candidate building)
# 3 : apply calibration and store it in an extra container for
#     calibrated muons and use this container for B candidate building
#
BPHY8cf.useCalibratedMuons = 3
## BPHY8cf.useCalibratedMuons = 2

# make sure data are not affected
# Update: Needs to be applied to data as well
## if not BPHY8cf.isSimulation:
##    BPHY8cf.useCalibratedMuons = 0

#
# Configuration for MuonCalibrationAndSmearingTool
#
# Set string values to "_READ_" to only read the default values
# from the tool.
#
# THE FOLLOWING COMMENT IS VOID AS OF 2017-07-30. 
# Note: Several JOs of the MuonCalibrationAndSmearingTool are not
# (yet) available in AtlasDerivation-20.7.8.x caches which uses
# MuonMomentumCorrections-01-00-35 instead of the latest
# MuonMomentumCorrections-01-00-60.
# For now these options will be disabled further below.
#
# MC
if BPHY8cf.isSimulation:
    BPHY8cf.McstYear                  = "Data16";
    BPHY8cf.McstRelease               = "_READ_"
    BPHY8cf.McstStatComb              = True
    BPHY8cf.McstSagittaCorr           = True
    BPHY8cf.McstSagittaRelease        = "_READ_"
    BPHY8cf.McstDoSagittaMCDistortion = False
#
# data 15
else:
    if BPHY8cf.projectTag.startswith("data15"):
        BPHY8cf.McstYear                  = "Data15";
        BPHY8cf.McstRelease               = "_READ_"
        BPHY8cf.McstStatComb              = True
        BPHY8cf.McstSagittaCorr           = False
        BPHY8cf.McstSagittaRelease        = "_READ_"
        BPHY8cf.McstDoSagittaMCDistortion = False
#
# data 16
    if BPHY8cf.projectTag.startswith("data16"):
        BPHY8cf.McstYear                  = "Data16";
        BPHY8cf.McstRelease               = "_READ_"
        BPHY8cf.McstStatComb              = True
        BPHY8cf.McstSagittaCorr           = True
        BPHY8cf.McstSagittaRelease        = "_READ_"
        BPHY8cf.McstDoSagittaMCDistortion = False
#
# data 17
# w.w., 2017-10-07 for now the Data16 tag as there  is no Data17
# tag for McstYear in the MuonCalibrationAndSmearingTool yet.
# TODO: Revise once Data17 tag becomes available.
    if BPHY8cf.projectTag.startswith("data17"):
        BPHY8cf.McstYear                  = "Data16";
        BPHY8cf.McstRelease               = "_READ_"
        BPHY8cf.McstStatComb              = True
        BPHY8cf.McstSagittaCorr           = True
        BPHY8cf.McstSagittaRelease        = "_READ_"
        BPHY8cf.McstDoSagittaMCDistortion = False

# wide mumu mass range?
BPHY8cf.doUseWideMuMuMassRange = False

# other default settings
BPHY8cf.GlobalChi2CutBase    = 15.

# Global mass values (in MeV, from PDG 2015)
BPHY8cf.GlobalMuonMass  = 105.6584
BPHY8cf.GlobalPionMass  = 139.57061
BPHY8cf.GlobalKaonMass  = 493.677
BPHY8cf.GlobalJpsiMass  = 3096.92
BPHY8cf.GlobalBplusMass = 5279.29
BPHY8cf.GlobalB0Mass    = 5279.61
BPHY8cf.GlobalBsMass    = 5366.79

# Cut values for kaon candidates
BPHY8cf.GlobalKaonPtCut  = 1000.
BPHY8cf.GlobalKaonEtaCut = 2.5

# primary vertex association types (interpreted bit-wise)
## BPHY8cf.doVertexType   = 15 # ALL
BPHY8cf.doVertexType   = 8  # only Z0_BA
## BPHY8cf.doVertexType   = 9   # only SUM_PT2 and Z0_BA

# minimum number of tracks in PV considered for PV association
BPHY8cf.minNTracksInPV = 3

# use invariant mass based on combined muon track information in mass cuts?
BPHY8cf.useMuCalcMass = True

# add MUCALC mass from non-modified muons for debugging
BPHY8cf.addMucalcMassForDebug = False

# PV types to be considered in calculation of minLogChi2ToAnyPV variable
BPHY8cf.MinChi2ToAnyPVTypes = [1, 3]

# MCP cuts for JpsiFinder
BPHY8cf.useJpsiFinderMCPCuts = False

# reject muons in JpsiPlus1Track or JpsiPlus2Track finders
BPHY8cf.GlobalMuonsUsedInJpsi = "NONE"  # default to turn it off

# mode of minLogChi2ToAnyPV calculation:
#   0 : no such calculation
#   1 : use all PVs of requested type(s)
#   2 : exclude PVs associated to SVs
#   3 : replace PVs associated to SVs by
#       corresponding refitted PVs
BPHY8cf.AddMinChi2ToAnyPVMode = 3

# Vertex isolation -- track selection requirements
# (Sizes of all lists below need to be identical!)
# Set to "Custom" (for strings) or -1. (for numerics) to disable setting
BPHY8cf.IsoTrackCategoryName = ["LoosePt05"]
BPHY8cf.IsoTrackCutLevel     = ["Loose"    ]
BPHY8cf.IsoTrackPtCut        = [   500.    ]
BPHY8cf.IsoTrackEtaCut       = [    -1.    ]
BPHY8cf.IsoTrackPixelHits    = [    -1     ]
BPHY8cf.IsoTrackSCTHits      = [    -1     ]
BPHY8cf.IsoTrackbLayerHits   = [    -1     ]
BPHY8cf.IsoTrackIBLHits      = [    -1     ]
# Vertex isolation -- cone sizes
# (Sizes of all lists below need to be identical!)
BPHY8cf.IsolationConeSizes   = [ 0.7]
BPHY8cf.IsoTrkImpLogChi2Max  = [ 5.0]
# Track types to be used (bit pattern)
# track sets to consider:
# bit : meaning
#  0   : tracks close to PV associated
#        with SV
#  1   : tracks associated with dummy PV
#        ("type-0 PV tracks")
#  2   : tracks associated with PV of type 1
#  3   : tracks associated with PV of type 2
#  4   : tracks associated with PV of type 3
#  5   : all tracks not selected by bits
#        0 to 4.
#        (E.g. 63 means to consider all tracks.)
BPHY8cf.useIsoTrackTypes    = [ 35 ]
# use of speed-optimized algorithm
BPHY8cf.IsoUseOptimizedAlgo = True
## BPHY8cf.IsoUseOptimizedAlgo = False

# Isolation for muons from B candidate -- track selection requirements
# (Sizes of all lists below need to be identical!)
# Set to "Custom" (for strings) or -1. (for numerics) to disable setting
BPHY8cf.MuIsoTrackCategoryName = ["LoosePt05"]
BPHY8cf.MuIsoTrackCutLevel     = ["Loose"    ]
BPHY8cf.MuIsoTrackPtCut        = [    500.   ]
BPHY8cf.MuIsoTrackEtaCut       = [     -1.   ]
BPHY8cf.MuIsoTrackPixelHits    = [     -1    ]
BPHY8cf.MuIsoTrackSCTHits      = [     -1    ]
BPHY8cf.MuIsoTrackbLayerHits   = [     -1    ]
BPHY8cf.MuIsoTrackIBLHits      = [     -1    ]
# Muon isolation -- cone sizes
# (Sizes of all lists below need to be identical!)
BPHY8cf.MuIsolationConeSizes   = [ 0.7]
BPHY8cf.MuIsoTrkImpLogChi2Max  = [ 5.0]
# Track types to be used (bit pattern)
# track sets to consider:
# bit : meaning
#  0   : tracks close to PV associated
#        with SV
#  1   : tracks associated with dummy PV
#        ("type-0 PV tracks")
#  2   : tracks associated with PV of type 1
#  3   : tracks associated with PV of type 2
#  4   : tracks associated with PV of type 3
#  5   : all tracks not selected by bits
#        0 to 4.
#        (E.g. 63 means to consider all tracks.)
BPHY8cf.useMuIsoTrackTypes    = [ 35 ]

# Closest track finding -- track selection requirements
# Set to "Custom" (for strings) or -1. (for numerics) to disable setting
BPHY8cf.CloseTrackCategoryName = ["LoosePt05"]
BPHY8cf.CloseTrackCutLevel     = ["Loose"    ]
BPHY8cf.CloseTrackPtCut        = [   500.    ]
BPHY8cf.CloseTrackEtaCut       = [    -1.    ]
BPHY8cf.CloseTrackPixelHits    = [    -1     ]
BPHY8cf.CloseTrackSCTHits      = [    -1     ]
BPHY8cf.CloseTrackbLayerHits   = [    -1     ]
BPHY8cf.CloseTrackIBLHits      = [    -1     ]
# Track types to be used (bit pattern)
# track sets to consider:
# bit : meaning
#  0   : tracks close to PV associated
#        with SV
#  1   : tracks associated with dummy PV
#        ("type-0 PV tracks")
#  2   : tracks associated with PV of type 1
#  3   : tracks associated with PV of type 2
#  4   : tracks associated with PV of type 3
#  5   : all tracks not selected by bits
#        0 to 4.
#        (E.g. 63 means to consider all tracks.)
#
# Correspondence to Run 1 settings:
#
# Option to only use tracks from specific primary vertices:
# (always excluding B decay tracks)
#  CloseTrackOption:
# old  new  
#  0 : 63 : use all tracks (default)
#  1 :  1 : use only tracks from PV associated with B vertex
#  2 :  1 : use all tracks which are not from PVs other than 
#           PV associated with B vertex 
#  3 : 35 : use all tracks which are not from PVs other than
#           PV associated with B vertex but including those
#           from the dummy vertex (type 0 vertex)
#  4 : 35 : same as option 3 but using the vertex pointers 
#           for comparing
BPHY8cf.useCloseTrackTypes   = [ 35 ]
# use 3-dimensional information in minimization
BPHY8cf.CloseTrackMinDCAin3D = True
# maximum chi2 distance of closest track to B vertex
BPHY8cf.CloseTrackMaxLogChi2    = 7.
# maximum chi2 distance of closest track to B vertex for track counting
BPHY8cf.NCloseTrackMaxLogChi2   = 1.

# BTrackVertexMapLogger / BPhysTrackVertexMapTools
# maximum number of events to dump track-to-vertex assoc. maps for
# (Set to -1 for no limit, to 0 to disable.)
BPHY8cf.DebugTrkToVtxMaxEvents = 0

#====================================================================
# General job setup
#====================================================================
# run number
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as BPHY8_af
BPHY8_f = BPHY8_af.fopen(athenaCommonFlags.PoolAODInput()[0])
if len(BPHY8_f.run_numbers) > 0:
    BPHY8cf.runNumber = int(BPHY8_f.run_numbers[0])

# for MC run specific channel(s) only,
# for data run 2-, 3- and 4-prong algorithms in parallel
if BPHY8cf.isSimulation:
    # MC channel number (ie dataset number for MC)
    if len(BPHY8_f.infos['mc_channel_number']) > 0:
        BPHY8cf.mcChNumber = int((BPHY8_f.infos['mc_channel_number'])[0])
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBsmumu):
            BPHY8cf.doChannels.append("Bsmumu") 
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBplusJpsiKplus):
            BPHY8cf.doChannels.append("BJpsiK")
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBsJpsiPhi):
            BPHY8cf.doChannels.append("BsJpsiPhi")
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBplusJpsiPiplus):
            BPHY8cf.doChannels.append("BJpsiPi")
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBhh):
            BPHY8cf.doChannels.append("Bhh")
        # use trigger?
        if (BPHY8cf.mcChNumber in BPHY8cf.mcNoTrigger):
            BPHY8cf.doTriggerInfo = False
    # no blind search for MC
    BPHY8cf.doBmumuBlinding = False
else:
    # for data
    BPHY8cf.doChannels += ["Bsmumu", "BJpsiK", "BsJpsiPhi"]

print "BPHY8 job setup: run               : %d" % BPHY8cf.runNumber
print "BPHY8 job setup: MC channel number : %d" % BPHY8cf.mcChNumber
print "BPHY8 job setup: isSimulation      : %s" % BPHY8cf.isSimulation
print "BPHY8 job setup: doChannels        :",
for BPHY8_channel in BPHY8cf.doChannels:
    print "%s" % (BPHY8_channel),
print

# abort if no channels are to be run on
assert len(BPHY8cf.doChannels) > 0

#====================================================================
# Mass ranges
#====================================================================
BPHY8cf.GlobalBMassUpperCut      = 7000.
BPHY8cf.GlobalBMassLowerCut      = 3500.
BPHY8cf.GlobalDiMuonMassUpperCut = 7000.
BPHY8cf.GlobalDiMuonMassLowerCut = 2000.
BPHY8cf.GlobalJpsiMassUpperCut   = 7000.
BPHY8cf.GlobalJpsiMassLowerCut   = 2000.
BPHY8cf.GlobalBlindLowerCut      = 5166.
BPHY8cf.GlobalBlindUpperCut      = 5526.

if BPHY8cf.doUseWideMuMuMassRange:
    BPHY8cf.GlobalBMassUpperCut      = 10000.
    BPHY8cf.GlobalBMassLowerCut      =  3250.
    BPHY8cf.GlobalDiMuonMassUpperCut = 10000.
    BPHY8cf.GlobalDiMuonMassLowerCut =  2000.
    BPHY8cf.GlobalJpsiMassUpperCut   = 10000.
    BPHY8cf.GlobalJpsiMassLowerCut   =  2000.

#====================================================================
# Vertexing chi2 cuts for n-prong decays
#====================================================================
# The global chi2 cut times NdF
BPHY8cf.Chi2Cut2Prong = BPHY8cf.GlobalChi2CutBase * 1.
BPHY8cf.Chi2Cut3Prong = BPHY8cf.GlobalChi2CutBase * 4.
BPHY8cf.Chi2Cut4Prong = BPHY8cf.GlobalChi2CutBase * 6.

#====================================================================
# Muons or tracks for JpsiFinder
#====================================================================
BPHY8cf.JfTwoMuons         = True
BPHY8cf.JfTwoTracks        = False
BPHY8cf.JfTrackThresholdPt = 0. # MeV
if "Bhh" in BPHY8cf.doChannels:
    BPHY8cf.JfTwoMuons         = False
    BPHY8cf.JfTwoTracks        = True
    BPHY8cf.JfTrackThresholdPt = 3500. # MeV
    
#====================================================================
# DEBUGGING SETUP
#====================================================================
#
# Dump contents of this file to log
if BPHY8cf.verbose > 4:
    import inspect
    thisfile = inspect.getfile(inspect.currentframe())
    print "# >>>------------------ %s ------------------------" % thisfile
    with open (thisfile, 'r') as fin:
        print fin.read()
    print "# <<<------------------ %s ------------------------" % thisfile

#====================================================================
# CALIBRATION SEQUENCES
#====================================================================
#
# For parameters of the MuonCalibrationAndSmearingTool see:
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15#Muon_momentum_scale_and_resoluti
#
# ordered dicts
from collections import OrderedDict
BPHY8_CalibrationAlgs = OrderedDict()

# Create calibrated muons if requested
BPHY8cf.CalMuonCollection  = BPHY8cf.MuonCollection
BPHY8cf.UsedMuonCollection = BPHY8cf.MuonCollection
BPHY8cf.AllMuonCollections = [ BPHY8cf.MuonCollection ]
if BPHY8cf.useCalibratedMuons > 0:
    BPHY8cf.adjustMucalcKinematics = True    
    if BPHY8cf.useCalibratedMuons > 1:
        BPHY8cf.CalMuonCollection = BPHY8cf.DerivationName+"_CalibratedMuons"
        BPHY8cf.adjustMucalcKinematics = False
        BPHY8cf.AllMuonCollections += [ BPHY8cf.CalMuonCollection ]
    if BPHY8cf.useCalibratedMuons > 2:
        BPHY8cf.UsedMuonCollection = BPHY8cf.CalMuonCollection
        BPHY8cf.adjustMucalcKinematics = True
    BPHY8_MuonCalTool = CfgMgr.CP__MuonCalibrationAndSmearingTool(
        BPHY8cf.DerivationName+"_MCPTool",
        OutputLevel            = INFO )
    if BPHY8cf.McstYear != "_READ_":
        BPHY8_MuonCalTool.Year = BPHY8cf.McstYear
    if BPHY8cf.McstRelease != "_READ_":
        BPHY8_MuonCalTool.Release = BPHY8cf.McstRelease
    # read back string values
    BPHY8cf.McstYear           = getPropertyValue(BPHY8_MuonCalTool, "Year")
    BPHY8cf.McstRelease        = getPropertyValue(BPHY8_MuonCalTool, "Release")
    # additional options for MuonMomentumCorrections-01-00-64 and up
    # Don't decorate with Eigen (from MuonMomentumCorrections-01-00-62
    # onwards, see ATLASG-1126)
    BPHY8_MuonCalTool.noEigenDecor          = True
    BPHY8_MuonCalTool.StatComb              = BPHY8cf.McstStatComb
    BPHY8_MuonCalTool.SagittaCorr           = BPHY8cf.McstSagittaCorr
    BPHY8_MuonCalTool.doSagittaMCDistortion = BPHY8cf.McstDoSagittaMCDistortion
    if BPHY8cf.McstSagittaRelease != "_READ_":
        BPHY8_MuonCalTool.SagittaRelease = BPHY8cf.McstSagittaRelease
    # read back string value
    BPHY8cf.McstSagittaRelease = getPropertyValue(BPHY8_MuonCalTool,
                                                      "SagittaRelease")
    ToolSvc +=  BPHY8_MuonCalTool
    print BPHY8_MuonCalTool
    pprint(BPHY8_MuonCalTool.properties())
    BPHY8_CalibrationAlgs["CalMuonProvider"] = CfgMgr.CP__CalibratedMuonsProvider(
        BPHY8cf.DerivationName+"_CalMuonProvider",
        Input       = BPHY8cf.MuonCollection,
        Output      = BPHY8cf.CalMuonCollection,
        Tool        = BPHY8_MuonCalTool,
        OutputLevel = INFO )  # output only if set to VERBOSE

# for quick debugging
## BPHY8cf.adjustMucalcKinematics = False
    
for BPHY8_name in BPHY8_CalibrationAlgs.keys():
    print BPHY8_CalibrationAlgs[BPHY8_name] 
    pprint(BPHY8_CalibrationAlgs[BPHY8_name].properties())

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
# setup vertexing tools and services
# superseeded by BPHYVertexTools
## include( "JpsiUpsilonTools/configureServices.py" )

# we need to have the DiMuon finder running for channels with Jpsi as well
BPHY8_recoList = []
if [BPHY8_i for BPHY8_i in BPHY8cf.doChannels \
    if BPHY8_i in ["Bsmumu", "BJpsiK", "BsJpsiPhi", "BJpsiPi", "Bhh"]]:
    BPHY8_recoList += ["DiMuon"]
if "BJpsiK"    in BPHY8cf.doChannels: BPHY8_recoList += [ "BJpsiK" ]
if "BsJpsiPhi" in BPHY8cf.doChannels: BPHY8_recoList += [ "BsJpsiPhi" ]
if "BJpsiPi"   in BPHY8cf.doChannels: BPHY8_recoList += [ "BJpsiPi" ]

# setup of vertexing tools per channel
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY8_VertexTools = OrderedDict()
for BPHY8_reco in BPHY8_recoList:
    BPHY8_VertexTools[BPHY8_reco] = BPHYVertexTools(BPHY8cf.DerivationName+"_"+BPHY8_reco)
    print BPHY8_VertexTools[BPHY8_reco]

# setup of vertex finder tools
from JpsiUpsilonTools.JpsiUpsilonToolsConf import \
    Analysis__JpsiFinder, Analysis__JpsiPlus1Track, Analysis__JpsiPlus2Tracks

BPHY8_FinderTools = OrderedDict()
for BPHY8_reco in BPHY8_recoList:
# a) for DiMuon
    if BPHY8_reco == "DiMuon":
        BPHY8_FinderTools[BPHY8_reco] = Analysis__JpsiFinder(
            name                        = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Finder",
            OutputLevel                 = INFO,
            muAndMu                     = BPHY8cf.JfTwoMuons,
            muAndTrack                  = False,
            TrackAndTrack               = BPHY8cf.JfTwoTracks,
            doTagAndProbe               = False,
            assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
            track1Mass                  = BPHY8cf.GlobalMuonMass,
            track2Mass                  = BPHY8cf.GlobalMuonMass,
            muonThresholdPt             = 0.,
            trackThresholdPt            = BPHY8cf.JfTrackThresholdPt,
            invMassUpper                = BPHY8cf.GlobalDiMuonMassUpperCut,
            invMassLower                = BPHY8cf.GlobalDiMuonMassLowerCut,
            Chi2Cut                     = BPHY8cf.Chi2Cut2Prong,
            oppChargesOnly	        = True,
            sameChargesOnly             = False,
            allChargeCombinations       = False,
            allMuons                    = True,
            combOnly                    = False,
            atLeastOneComb              = False,
            useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
            muonCollectionKey           = BPHY8cf.UsedMuonCollection,
            TrackParticleCollection     = BPHY8cf.TrkPartContName,
            V0VertexFitterTool          = BPHY8_VertexTools[BPHY8_reco].TrkV0Fitter,             # V0 vertex fitter
            useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
            TrkVertexFitterTool         = BPHY8_VertexTools[BPHY8_reco].TrkVKalVrtFitter,        # VKalVrt vertex fitter
            TrackSelectorTool           = BPHY8_VertexTools[BPHY8_reco].InDetTrackSelectorTool,
            ConversionFinderHelperTool  = BPHY8_VertexTools[BPHY8_reco].InDetConversionHelper,
            VertexPointEstimator        = BPHY8_VertexTools[BPHY8_reco].VtxPointEstimator,
            useMCPCuts                  = BPHY8cf.useJpsiFinderMCPCuts )
    
# b) for BJpsiK or BJpsiPi
    if BPHY8_reco in [ "BJpsiK", "BJpsiPi" ] :
        BPHY8_kaonHypo = (False if BPHY8_reco == "BJpsiPi" else True)
        BPHY8_FinderTools[BPHY8_reco] = Analysis__JpsiPlus1Track(
            name                        = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Finder",
            OutputLevel                 = INFO,
            pionHypothesis              = not BPHY8_kaonHypo,
            kaonHypothesis              = BPHY8_kaonHypo,
            trkThresholdPt              = BPHY8cf.GlobalKaonPtCut,
            trkMaxEta                   = BPHY8cf.GlobalKaonEtaCut,
            BThresholdPt                = 1000.,
            BMassUpper                  = BPHY8cf.GlobalBMassUpperCut,
            BMassLower                  = BPHY8cf.GlobalBMassLowerCut,
            JpsiContainerKey            = BPHY8cf.DerivationName+"DiMuonCandidates",
            JpsiMassUpper               = BPHY8cf.GlobalJpsiMassUpperCut,
            JpsiMassLower               = BPHY8cf.GlobalJpsiMassLowerCut,
            MuonsUsedInJpsi             = BPHY8cf.GlobalMuonsUsedInJpsi,
            TrackParticleCollection     = BPHY8cf.TrkPartContName,
            TrkVertexFitterTool         = BPHY8_VertexTools[BPHY8_reco].TrkVKalVrtFitter,        # VKalVrt vertex fitter
            TrackSelectorTool           = BPHY8_VertexTools[BPHY8_reco].InDetTrackSelectorTool,
            UseMassConstraint           = True)
    
# c) for BsJpsiPhi
    if BPHY8_reco == "BsJpsiPhi":
        BPHY8_FinderTools[BPHY8_reco] = Analysis__JpsiPlus2Tracks(
            name                        = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Finder",
            OutputLevel                 = INFO,
            pionpionHypothesis          = False,
            kaonkaonHypothesis          = True,
            kaonpionHypothesis          = False,
            trkThresholdPt              = BPHY8cf.GlobalKaonPtCut,
            trkMaxEta                   = BPHY8cf.GlobalKaonEtaCut,
            BThresholdPt                = 1000.,
            BMassUpper                  = BPHY8cf.GlobalBMassUpperCut,
            BMassLower                  = BPHY8cf.GlobalBMassLowerCut,
            JpsiContainerKey            = BPHY8cf.DerivationName+"DiMuonCandidates",
            JpsiMassUpper               = BPHY8cf.GlobalJpsiMassUpperCut,
            JpsiMassLower               = BPHY8cf.GlobalJpsiMassLowerCut,
            MuonsUsedInJpsi             = BPHY8cf.GlobalMuonsUsedInJpsi,
            TrackParticleCollection     = BPHY8cf.TrkPartContName,
            TrkVertexFitterTool         = BPHY8_VertexTools[BPHY8_reco].TrkVKalVrtFitter,        # VKalVrt vertex fitter
            TrackSelectorTool           = BPHY8_VertexTools[BPHY8_reco].InDetTrackSelectorTool,
            UseMassConstraint           = True)
        
ToolSvc += BPHY8_FinderTools.values()
for BPHY8_name in BPHY8_FinderTools.keys():
    print BPHY8_FinderTools[BPHY8_name] 
    pprint(BPHY8_FinderTools[BPHY8_name].properties())
    
#--------------------------------------------------------------------
# Setup the vertex reconstruction "call" tool(s). They are part of the
# derivation framework.
# These Augmentation tools add output vertex collection(s) into the
# StoreGate and add basic decorations which do not depend on the vertex
# mass hypothesis (e.g. lxy, ptError, etc).
# There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need
# two instance of the Reco tool if the JpsiFinder mass window is wide enough.
#
# The reconstruction tools must be interleaved with the vertex selection
# and augmentation tools as e.g. the Jpsimumu container ist needed for
# ????
#

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__Bmumu_reco_mumu, \
    DerivationFramework__Reco_dimuTrk, \
    DerivationFramework__Reco_dimuTrkTrk
BPHY8_RecoTools = OrderedDict()
for BPHY8_reco in BPHY8_recoList:
# a) for DiMuon
    if BPHY8_reco == "DiMuon":
        BPHY8_RecoTools[BPHY8_reco] = DerivationFramework__Bmumu_reco_mumu(
            name                   = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Reco",
            JpsiFinder             = BPHY8_FinderTools[BPHY8_reco],
            OutputVtxContainerName = BPHY8cf.DerivationName+BPHY8_reco+"Candidates",
            PVContainerName        = BPHY8cf.PVContName,
            RefPVContainerName     = BPHY8cf.DerivationName+"DiMuonRefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            MinNTracksInPV         = BPHY8cf.minNTracksInPV,
            DoVertexType           = BPHY8cf.doVertexType)
# b) for BJpsiK
    if BPHY8_reco in [ "BJpsiK", "BJpsiPi" ] :
        BPHY8_RecoTools[BPHY8_reco] = DerivationFramework__Reco_dimuTrk(
            name                   = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Reco",
            Jpsi1PlusTrackName     = BPHY8_FinderTools[BPHY8_reco],
            OutputVtxContainerName = BPHY8cf.DerivationName+BPHY8_reco+"Candidates",
            PVContainerName        = BPHY8cf.PVContName,
            RefPVContainerName     = BPHY8cf.DerivationName+BPHY8_reco+"RefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            MinNTracksInPV         = BPHY8cf.minNTracksInPV,
            DoVertexType           = BPHY8cf.doVertexType)
# c) for BsJpsiPhi
    if BPHY8_reco == "BsJpsiPhi":
        BPHY8_RecoTools[BPHY8_reco] = DerivationFramework__Reco_dimuTrkTrk(
            name                   = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Reco",
            Jpsi2PlusTrackName     = BPHY8_FinderTools[BPHY8_reco],
            OutputVtxContainerName = BPHY8cf.DerivationName+BPHY8_reco+"Candidates",
            PVContainerName        = BPHY8cf.PVContName,
            RefPVContainerName     = BPHY8cf.DerivationName+BPHY8_reco+"RefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            MinNTracksInPV         = BPHY8cf.minNTracksInPV,
            DoVertexType           = BPHY8cf.doVertexType)
        
ToolSvc += BPHY8_RecoTools.values()
for BPHY8_name in BPHY8_RecoTools.keys():
    print BPHY8_RecoTools[BPHY8_name]
    pprint(BPHY8_RecoTools[BPHY8_name].properties())

#--------------------------------------------------------------------
# Augmentation of vertices by MUCALC mass and it's error
#
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__BPhysAddMuonBasedInvMass

BPHY8_MuMassTools = OrderedDict()
# a) for Bsmumu
if "Bsmumu" in BPHY8cf.doChannels:
    # augment B(s)->mumu candidates
    BPHY8_MuMassTools["Bsmumu"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_Bsmumu",
        BranchPrefix               = "Bsmumu",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)
# b) for BJpsiK, BsJpsiPhi and BJpsiPi retain the Jpsi
if [i for i in BPHY8cf.doChannels if i in ["BJpsiK", "BsJpsiPhi", "BJpsiPi"]]:
    BPHY8_MuMassTools["Jpsimumu"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_Jpsimumu",
        BranchPrefix               = "Jpsimumu",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)
# c) for BJpsiK
if "BJpsiK" in BPHY8cf.doChannels:
    # augment B+/- ->JpsiK+/- candidates
    BPHY8_MuMassTools["BJpsiK"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_BJpsiK",
        BranchPrefix               = "BJpsiK",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"BJpsiKCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)
# d) for BsJpsiPhi
if "BsJpsiPhi" in BPHY8cf.doChannels:
    # augment Bs ->JpsiPhi candidates
    BPHY8_MuMassTools["BsJpsiPhi"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_BsJpsiPhi",
        BranchPrefix               = "BsJpsiPhi",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"BsJpsiPhiCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass, BPHY8cf.GlobalKaonMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)
# e) for BJpsiPi
if "BJpsiPi" in BPHY8cf.doChannels:
    # augment B+/- ->JpsiPi+/- candidates
    BPHY8_MuMassTools["BJpsiPi"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_BJpsiPi",
        BranchPrefix               = "BJpsiPi",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"BJpsiPiCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalPionMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)
# f) for Bhh
if "Bhh" in BPHY8cf.doChannels:
    # augment B->hh candidates
    BPHY8_MuMassTools["Bhh"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_Bhh",
        BranchPrefix               = "Bhh",
        OutputLevel                = WARNING,
        AdjustToMuonKinematics     = BPHY8cf.adjustMucalcKinematics,
        VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
        PrimaryVertexContainerName = BPHY8cf.PVContName,
        MinNTracksInPV             = BPHY8cf.minNTracksInPV,
        PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
        DoVertexType               = BPHY8cf.doVertexType)

######################## duplication for debugging only #######################

if BPHY8cf.addMucalcMassForDebug:
    # a) for Bsmumu
    if "Bsmumu" in BPHY8cf.doChannels:
        # augment B(s)->mumu candidates
        BPHY8_MuMassTools["Bsmumu2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_Bsmumu2",
            BranchPrefix               = "Bsmumu2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
    # b) for BJpsiK and BsJpsiPhi retain the Jpsi
    if [i for i in BPHY8cf.doChannels if i in ["BJpsiK", "BsJpsiPhi"]]:
        BPHY8_MuMassTools["Jpsimumu2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_Jpsimumu2",
            BranchPrefix               = "Jpsimumu2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
    # c) for BJpsiK
    if "BJpsiK" in BPHY8cf.doChannels:
        # augment B+/- ->JpsiK+/- candidates
        BPHY8_MuMassTools["BJpsiK2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_BJpsiK2",
            BranchPrefix               = "BJpsiK2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"BJpsiKCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
    # d) for BsJpsiPhi
    if "BsJpsiPhi" in BPHY8cf.doChannels:
        # augment Bs ->JpsiPhi candidates
        BPHY8_MuMassTools["BsJpsiPhi2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_BsJpsiPhi2",
            BranchPrefix               = "BsJpsiPhi2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"BsJpsiPhiCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass, BPHY8cf.GlobalKaonMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
        # e) for BJpsiPi
    if "BJpsiPi" in BPHY8cf.doChannels:
        # augment B+/- ->JpsiPi+/- candidates
        BPHY8_MuMassTools["BJpsiPi2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_BJpsiPi2",
            BranchPrefix               = "BJpsiPi2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"BJpsiPiCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalPionMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
    # f) for Bhh
    if "Bhh" in BPHY8cf.doChannels:
    # augment B->hh candidates
        BPHY8_MuMassTools["Bhh2"] = DerivationFramework__BPhysAddMuonBasedInvMass(
            name                       = "BPHY8_MuMass_Bhh2",
            BranchPrefix               = "Bhh2",
            OutputLevel                = WARNING,
            AdjustToMuonKinematics     = False,
            VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
            TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
            AddMinChi2ToAnyPVMode      = BPHY8cf.AddMinChi2ToAnyPVMode,
            PrimaryVertexContainerName = BPHY8cf.PVContName,
            MinNTracksInPV             = BPHY8cf.minNTracksInPV,
            PVTypesToConsider          = BPHY8cf.MinChi2ToAnyPVTypes,
            DoVertexType               = BPHY8cf.doVertexType)
        
######################## duplication for debugging only #######################
    
ToolSvc += BPHY8_MuMassTools.values()
for BPHY8_name in BPHY8_MuMassTools.keys():
    print BPHY8_MuMassTools[BPHY8_name] 
    pprint(BPHY8_MuMassTools[BPHY8_name].properties())
    
#--------------------------------------------------------------------
# Augmentation of vertices by vertex track isolation values,
# closest track information and muon isolation values.
#
# First: Set up track selections
#
from InDetTrackSelectionTool.InDetTrackSelectionToolConf \
    import InDet__InDetTrackSelectionTool

# a) for BVertexTrackIsoTool
BPHY8_IsoTrkSelTools = OrderedDict()
for i in xrange(len(BPHY8cf.IsoTrackCategoryName)):
    BPHY8_name = BPHY8cf.DerivationName+"_iso_"+BPHY8cf.IsoTrackCategoryName[i]
    BPHY8_Tool = InDet__InDetTrackSelectionTool(
        name        = BPHY8_name,
        OutputLevel = INFO
    )
    if BPHY8cf.IsoTrackCutLevel[i] != "Custom" :
        BPHY8_Tool.CutLevel                     = BPHY8cf.IsoTrackCutLevel[i]
    if BPHY8cf.IsoTrackPtCut[i] > -1. :
        BPHY8_Tool.minPt                        = BPHY8cf.IsoTrackPtCut[i]
    if BPHY8cf.IsoTrackEtaCut[i] > -1. :
        BPHY8_Tool.maxAbsEta                    = BPHY8cf.IsoTrackEtaCut[i]
    if BPHY8cf.IsoTrackPixelHits[i] > -1 :
        BPHY8_Tool.minNPixelHits                = BPHY8cf.IsoTrackPixelHits[i]
    if BPHY8cf.IsoTrackSCTHits[i] > -1 :
        BPHY8_Tool.minNSctHits                  = BPHY8cf.IsoTrackSCTHits[i]
    if BPHY8cf.IsoTrackIBLHits[i] > -1 :
        BPHY8_Tool.minNInnermostLayerHits       = BPHY8cf.IsoTrackIBLHits[i]
    if BPHY8cf.IsoTrackbLayerHits[i] > -1 :
        BPHY8_Tool.minNNextToInnermostLayerHits = BPHY8cf.IsoTrackbLayerHits[i]
    BPHY8_IsoTrkSelTools[BPHY8_name] = BPHY8_Tool

ToolSvc += BPHY8_IsoTrkSelTools.values()
for BPHY8_name in BPHY8_IsoTrkSelTools.keys():
    print BPHY8_IsoTrkSelTools[BPHY8_name] 
    pprint(BPHY8_IsoTrkSelTools[BPHY8_name].properties())

# b) for BMuonTrackIsoTool
BPHY8_MuIsoTrkSelTools = OrderedDict()
for i in xrange(len(BPHY8cf.MuIsoTrackCategoryName)):
    BPHY8_name = BPHY8cf.DerivationName+"_muiso_" + \
                 BPHY8cf.MuIsoTrackCategoryName[i]
    BPHY8_Tool = InDet__InDetTrackSelectionTool(
        name        = BPHY8_name,
        OutputLevel = INFO
    )
    if BPHY8cf.MuIsoTrackCutLevel[i] != "Custom" :
        BPHY8_Tool.CutLevel                     = BPHY8cf.MuIsoTrackCutLevel[i]
    if BPHY8cf.MuIsoTrackPtCut[i] > -1. :
        BPHY8_Tool.minPt                        = BPHY8cf.MuIsoTrackPtCut[i]
    if BPHY8cf.MuIsoTrackEtaCut[i] > -1. :
        BPHY8_Tool.maxAbsEta                    = BPHY8cf.MuIsoTrackEtaCut[i]
    if BPHY8cf.MuIsoTrackPixelHits[i] > -1 :
        BPHY8_Tool.minNPixelHits                = BPHY8cf.MuIsoTrackPixelHits[i]
    if BPHY8cf.MuIsoTrackSCTHits[i] > -1 :
        BPHY8_Tool.minNSctHits                  = BPHY8cf.MuIsoTrackSCTHits[i]
    if BPHY8cf.MuIsoTrackIBLHits[i] > -1 :
        BPHY8_Tool.minNInnermostLayerHits       = BPHY8cf.MuIsoTrackIBLHits[i]
    if BPHY8cf.MuIsoTrackbLayerHits[i] > -1 :
        BPHY8_Tool.minNNextToInnermostLayerHits \
            = BPHY8cf.MuIsoTrackbLayerHits[i]
    BPHY8_MuIsoTrkSelTools[BPHY8_name] = BPHY8_Tool

ToolSvc += BPHY8_MuIsoTrkSelTools.values()
for BPHY8_name in BPHY8_MuIsoTrkSelTools.keys():
    print BPHY8_MuIsoTrkSelTools[BPHY8_name] 
    pprint(BPHY8_MuIsoTrkSelTools[BPHY8_name].properties())

# c) for ClosestTrackTool
BPHY8_CtTrkSelTools = OrderedDict()
for i in xrange(len(BPHY8cf.CloseTrackCategoryName)):
    BPHY8_name = BPHY8cf.DerivationName+"_ct_"+BPHY8cf.CloseTrackCategoryName[i]
    BPHY8_Tool = InDet__InDetTrackSelectionTool(
        name        = BPHY8_name,
        OutputLevel = INFO
    )
    if BPHY8cf.CloseTrackCutLevel[i] != "Custom" :
        BPHY8_Tool.CutLevel                     = BPHY8cf.CloseTrackCutLevel[i]
    if BPHY8cf.CloseTrackPtCut[i] > -1. :
        BPHY8_Tool.minPt                        = BPHY8cf.CloseTrackPtCut[i]
    if BPHY8cf.CloseTrackEtaCut[i] > -1. :
        BPHY8_Tool.maxAbsEta                    = BPHY8cf.CloseTrackEtaCut[i]
    if BPHY8cf.CloseTrackPixelHits[i] > -1 :
        BPHY8_Tool.minNPixelHits                = BPHY8cf.CloseTrackPixelHits[i]
    if BPHY8cf.CloseTrackSCTHits[i] > -1 :
        BPHY8_Tool.minNSctHits                  = BPHY8cf.CloseTrackSCTHits[i]
    if BPHY8cf.CloseTrackIBLHits[i] > -1 :
        BPHY8_Tool.minNInnermostLayerHits       = BPHY8cf.CloseTrackIBLHits[i]
    if BPHY8cf.CloseTrackbLayerHits[i] > -1 :
        BPHY8_Tool.minNNextToInnermostLayerHits \
            = BPHY8cf.CloseTrackbLayerHits[i]
    BPHY8_CtTrkSelTools[BPHY8_name] = BPHY8_Tool

ToolSvc += BPHY8_CtTrkSelTools.values()
for BPHY8_name in BPHY8_CtTrkSelTools.keys():
    print BPHY8_CtTrkSelTools[BPHY8_name] 
    pprint(BPHY8_CtTrkSelTools[BPHY8_name].properties())

#
# Second: Set up the B candidate vertex container arrays
#
BPHY8cf.VtxContNames  = [];
BPHY8cf.RefPVContNames = [];
BPHY8cf.BranchPrefixes = [];
if [i for i in BPHY8cf.doChannels \
    if i in ["Bsmumu", "BJpsiK", "BsJpsiPhi", "BJpsiPi", "Bhh"]]:
    BPHY8cf.VtxContNames   += [ BPHY8cf.DerivationName+"DiMuonCandidates" ]
    BPHY8cf.RefPVContNames += [ BPHY8cf.DerivationName
                              +"DiMuonRefittedPrimaryVertices" ]
    BPHY8cf.BranchPrefixes += [ "DiMuon" ];
if "BJpsiK" in BPHY8cf.doChannels:
    BPHY8cf.VtxContNames   += [ BPHY8cf.DerivationName+"BJpsiKCandidates" ]
    BPHY8cf.RefPVContNames += [ BPHY8cf.DerivationName
                              +"BJpsiKRefittedPrimaryVertices" ]
    BPHY8cf.BranchPrefixes += [ "BJpsiK" ];
if "BsJpsiPhi" in BPHY8cf.doChannels:
    BPHY8cf.VtxContNames   += [ BPHY8cf.DerivationName+"BsJpsiPhiCandidates" ]
    BPHY8cf.RefPVContNames += [ BPHY8cf.DerivationName
                              +"BsJpsiPhiRefittedPrimaryVertices" ]
    BPHY8cf.BranchPrefixes += [ "BsJpsiPhi" ];
if "BJpsiPi" in BPHY8cf.doChannels:
    BPHY8cf.VtxContNames   += [ BPHY8cf.DerivationName+"BJpsiPiCandidates" ]
    BPHY8cf.RefPVContNames += [ BPHY8cf.DerivationName
                              +"BJpsiPiRefittedPrimaryVertices" ]
    BPHY8cf.BranchPrefixes += [ "BJpsiPi" ];
    
#
# Third: Set up the real tools
#
BPHY8_IsoTools = OrderedDict()

# a) BVertexTrackIsoTool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__BVertexTrackIsoTool

BPHY8_IsoTools["TrackVtxIso"] = DerivationFramework__BVertexTrackIsoTool(
    name                       = "BPHY8_VtxIsoTool",
    BranchPrefixes             = BPHY8cf.BranchPrefixes,
    BranchBaseName             = "iso",
    OutputLevel                = INFO,
    VertexContainerNames       = BPHY8cf.VtxContNames,
    RefPVContainerNames        = BPHY8cf.RefPVContNames,
    TrackParticleContainerName = BPHY8cf.TrkPartContName,
    PVContainerName            = BPHY8cf.PVContName,
    TrackSelectionTools        = BPHY8_IsoTrkSelTools.values(),
    IsolationConeSizes         = BPHY8cf.IsolationConeSizes,
    IsoTrkImpLogChi2Max        = BPHY8cf.IsoTrkImpLogChi2Max,    
    DoVertexType               = BPHY8cf.doVertexType,
    UseTrackTypes              = BPHY8cf.useIsoTrackTypes,
    UseOptimizedAlgo           = BPHY8cf.IsoUseOptimizedAlgo)

# b) BMuonTrackIsoTool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__BMuonTrackIsoTool

BPHY8_IsoTools["TrackMuonIso"] = DerivationFramework__BMuonTrackIsoTool(
    name                       = "BPHY8_MuonIsoTool",
    BranchPrefixes             = BPHY8cf.BranchPrefixes,
    BranchBaseName             = "muiso",
    OutputLevel                = INFO,
    VertexContainerNames       = BPHY8cf.VtxContNames,
    RefPVContainerNames        = BPHY8cf.RefPVContNames,
    TrackParticleContainerName = BPHY8cf.TrkPartContName,
    PVContainerName            = BPHY8cf.PVContName,
    MuonContainerName          = BPHY8cf.UsedMuonCollection,
    TrackSelectionTools        = BPHY8_MuIsoTrkSelTools.values(),
    IsolationConeSizes         = BPHY8cf.MuIsolationConeSizes,
    IsoTrkImpLogChi2Max        = BPHY8cf.MuIsoTrkImpLogChi2Max,    
    DoVertexType               = BPHY8cf.doVertexType,
    UseTrackTypes              = BPHY8cf.useMuIsoTrackTypes)

# c) BVertexClosestTrackTool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__BVertexClosestTrackTool

BPHY8_IsoTools["TrackVtxCt"] = DerivationFramework__BVertexClosestTrackTool(
    name                       = "BPHY8_VtxClosestTrkTool",
    BranchPrefixes             = BPHY8cf.BranchPrefixes,
    BranchBaseName             = "ct",
    OutputLevel                = INFO,
    VertexContainerNames       = BPHY8cf.VtxContNames,
    RefPVContainerNames        = BPHY8cf.RefPVContNames,
    TrackParticleContainerName = BPHY8cf.TrkPartContName,
    PVContainerName            = BPHY8cf.PVContName,
    TrackSelectionTools        = BPHY8_CtTrkSelTools.values(),
    DoVertexType               = BPHY8cf.doVertexType,
    UseTrackTypes              = BPHY8cf.useCloseTrackTypes,
    CloseTrackMinDCAin3D       = BPHY8cf.CloseTrackMinDCAin3D,
    CloseTrackMaxLogChi2       = BPHY8cf.CloseTrackMaxLogChi2,
    NCloseTrackMaxLogChi2      = BPHY8cf.NCloseTrackMaxLogChi2)

#
# Fourth: Add track-to-vertex map debugging
#
BPHY8_TtvmTools = OrderedDict();

# a) configure BPhysTrackVertexMapTools
# Configure only if not 0 events requested
if BPHY8cf.DebugTrkToVtxMaxEvents != 0 :
    for BPHY8_prefix, BPHY8_SVcont, BPHY8_refPVcont in \
        zip(BPHY8cf.BranchPrefixes, BPHY8cf.VtxContNames,
            BPHY8cf.RefPVContNames):
        BPHY8_hypos = BPHY8_prefix
        if BPHY8_hypos == "DiMuon":
            BPHY8_hypos += "|Bsmumu|Jpsimumu"
        BPHY8_TtvmTools[BPHY8_prefix] = CfgMgr.xAOD__BPhysTrackVertexMapTool(
            "BPHY8_ttvm_"+BPHY8_prefix,
            OutputLevel                = INFO,
            VertexContainerName        = BPHY8_SVcont,
            RefPVContainerName         = BPHY8_refPVcont, 
            PVContainerName            = BPHY8cf.PVContName,
            TrackParticleContainerName = BPHY8cf.TrkPartContName, 
            DebugTrkToVtxMaxEvents     = BPHY8cf.DebugTrkToVtxMaxEvents,
            DumpPrefix                 = "TTV2> ",
            HypoName                   = BPHY8_hypos )

    ToolSvc += BPHY8_TtvmTools.values()
    for BPHY8_name in BPHY8_TtvmTools.keys():
        print BPHY8_TtvmTools[BPHY8_name] 
        pprint(BPHY8_TtvmTools[BPHY8_name].properties())

# b) wrap into logger algorithm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__BTrackVertexMapLogger

# Configure only if not 0 events requested
if BPHY8cf.DebugTrkToVtxMaxEvents != 0 :
    BPHY8_IsoTools["TvmLogger"] = DerivationFramework__BTrackVertexMapLogger(
        name                    = "BPHY8_TrackVertexMapLogger",
        OutputLevel             = INFO,
        TrackVertexMapTools     = BPHY8_TtvmTools.values(),
        Enable                  = True)
    
#
# Fifth: Attach to ToolSvc
#
ToolSvc += BPHY8_IsoTools.values()
for BPHY8_name in BPHY8_IsoTools.keys():
    print BPHY8_IsoTools[BPHY8_name] 
    pprint(BPHY8_IsoTools[BPHY8_name].properties())

#--------------------------------------------------------------------
# Record the original counts for primary vertices and tracks
#--------------------------------------------------------------------
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__AugOriginalCounts
BPHY8_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
    name = "BPHY8_AugOriginalCounts",
    VertexContainer   = BPHY8cf.PVContName,
    TrackContainer    = BPHY8cf.TrkPartContName,
    AddPVCountsByType = True)
    
ToolSvc += BPHY8_AugOriginalCounts
pprint(BPHY8_AugOriginalCounts.properties())

#--------------------------------------------------------------------
# Setup the vertex selection and augmentation tool(s). These tools decorate
# the vertices with variables that depend on the vertex mass hypothesis,
# e.g. invariant mass, proper decay time, etc.
# Property HypothesisName is used as a prefix for these decorations.
# They also perform tighter selection, flagging the vertecis that passed.
# The flag is a Char_t branch named "passed_"+HypothesisName. It is used
# later by the "SelectEvent" and "Thin_vtxTrk" tools to determine which
# events and candidates should be kept in the output stream.
# Multiple instances of the Select_* tools can be used on a single input
# collection as long as they use different "HypothesisName" flags.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__Select_Bmumu

BPHY8_SelectTools = OrderedDict()
# a) for Bsmumu
if "Bsmumu" in BPHY8cf.doChannels:
    # augment and select B(s)->mumu candidates
    BPHY8_SelectTools["Bsmumu"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_Bsmumu",
        HypothesisName         = "Bsmumu",
        InputVtxContainerName  = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        VtxMassHypo            = BPHY8cf.GlobalBsMass,
        MassMin                = BPHY8cf.GlobalBMassLowerCut,
        MassMax                = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut2Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        BlindMassMin           = BPHY8cf.GlobalBlindLowerCut,
        BlindMassMax           = BPHY8cf.GlobalBlindUpperCut,
        DoBlinding             = BPHY8cf.doBmumuBlinding,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        OutputLevel            = WARNING)
# b) for BJpsiK and BsJpsiPhi retain the Jpsi
if [i for i in BPHY8cf.doChannels if i in ["BJpsiK", "BsJpsiPhi"]]:
    BPHY8_SelectTools["Jpsimumu"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_Jpsimumu",
        HypothesisName         = "Jpsimumu",
        InputVtxContainerName  = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        VtxMassHypo            = BPHY8cf.GlobalJpsiMass,
        MassMin                = BPHY8cf.GlobalJpsiMassLowerCut,
        MassMax                = BPHY8cf.GlobalJpsiMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut2Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        OutputLevel            = WARNING)
# c) for BJpsiK
if "BJpsiK" in BPHY8cf.doChannels:
    # augment and select B+/- ->JpsiK+/- candidates
    BPHY8_SelectTools["BJpsiK"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_BJpsiK",
        HypothesisName         = "BJpsiK",
        InputVtxContainerName  = BPHY8cf.DerivationName+"BJpsiKCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass],
        VtxMassHypo            = BPHY8cf.GlobalBplusMass,
        MassMin                = BPHY8cf.GlobalBMassLowerCut,
        MassMax                = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut3Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        SubDecVtxContNames     = [BPHY8cf.DerivationName+"DiMuonCandidates"],
        SubDecVtxHypoCondNames = ["Jpsimumu"],
        SubDecVtxHypoFlagNames = ["JpsimumuSubDecay"],
        OutputLevel            = WARNING)
# d) for BsJpsiPhi
if "BsJpsiPhi" in BPHY8cf.doChannels:
    # augment and select Bs ->JpsiPhi candidates
    BPHY8_SelectTools["BsJpsiPhi"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_BsJpsiPhi",
        HypothesisName         = "BsJpsiPhi",
        InputVtxContainerName  = BPHY8cf.DerivationName+"BsJpsiPhiCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass, BPHY8cf.GlobalKaonMass],
        VtxMassHypo            = BPHY8cf.GlobalBsMass,
        MassMin                = BPHY8cf.GlobalBMassLowerCut,
        MassMax                = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut4Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        SubDecVtxContNames     = [BPHY8cf.DerivationName+"DiMuonCandidates"],
        SubDecVtxHypoCondNames = ["Jpsimumu"],
        SubDecVtxHypoFlagNames = ["JpsimumuSubDecay"],
        OutputLevel            = WARNING)
# e) for BJpsiPi
if "BJpsiPi" in BPHY8cf.doChannels:
    # augment and select B+/- ->JpsiPi+/- candidates
    BPHY8_SelectTools["BJpsiPi"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_BJpsiPi",
        HypothesisName         = "BJpsiPi",
        InputVtxContainerName  = BPHY8cf.DerivationName+"BJpsiPiCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalPionMass],
        VtxMassHypo            = BPHY8cf.GlobalBplusMass,
        MassMin                = BPHY8cf.GlobalBMassLowerCut,
        MassMax                = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut3Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        SubDecVtxContNames     = [BPHY8cf.DerivationName+"DiMuonCandidates"],
        SubDecVtxHypoCondNames = ["Jpsimumu"],
        SubDecVtxHypoFlagNames = ["JpsimumuSubDecay"],
        OutputLevel            = WARNING)
# f) for Bhh
if "Bhh" in BPHY8cf.doChannels:
    # augment and select B->hh candidates
    BPHY8_SelectTools["Bhh"] = DerivationFramework__Select_Bmumu(
        name                   = "BPHY8_Select_Bhh",
        HypothesisName         = "Bhh",
        InputVtxContainerName  = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses              = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        VtxMassHypo            = BPHY8cf.GlobalBsMass,
        MassMin                = BPHY8cf.GlobalBMassLowerCut,
        MassMax                = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max                = BPHY8cf.Chi2Cut2Prong,
        DoVertexType           = BPHY8cf.doVertexType,
        UseMuCalcMass          = BPHY8cf.useMuCalcMass,
        OutputLevel            = WARNING)
  
ToolSvc += BPHY8_SelectTools.values()
for BPHY8_name in BPHY8_SelectTools.keys():
    print BPHY8_SelectTools[BPHY8_name] 
    pprint(BPHY8_SelectTools[BPHY8_name].properties())

#====================================================================
# Skimming tool to select only events with the correct vertices
#====================================================================
#--------------------------------------------------------------------
# Select the event. We only want to keep events that contain certain
# vertices which passed certain selection.  This is specified by the
# "SelectionExpression" property, which contains the expression in the
# following format:
#
#       "ContainerName.passed_HypoName > count"
#
# where "ContainerName" is output container form some Reco_* tool,
# "HypoName" is the hypothesis name setup in some "Select_*" tool and
# "count" is the number of candidates passing the selection you want to keep.
#

# Build expression depending on "select tools" used:
# If any of them marked any candidate passed, add expression for it.
BPHY8_expressions = []
for BPHY8_tool in BPHY8_SelectTools.values():
    BPHY8_passName = BPHY8_tool.HypothesisName
    if BPHY8_tool.HypothesisName == "Jpsimumu" and BPHY8cf.thinLevel > 0:
        BPHY8_passName = "JpsimumuSubDecay"
    BPHY8_expressions += [ "count(%s.passed_%s) > 0" % \
                           (BPHY8_tool.InputVtxContainerName,
                            BPHY8_passName) ]
BPHY8cf.SelExpression = " || ".join(BPHY8_expressions)

from DerivationFrameworkTools.DerivationFrameworkToolsConf \
    import DerivationFramework__xAODStringSkimmingTool
BPHY8_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name       = "BPHY8_SelectEvent",
    expression = BPHY8cf.SelExpression)

ToolSvc += BPHY8_SkimmingTool
print BPHY8_SkimmingTool
pprint(BPHY8_SkimmingTool.properties())

# Check for global ToolSvc:
print ">>> Checking ToolSvc tools: <<<"
for BPHY8_i in ToolSvc:
    print BPHY8_i
print ">>> End of ToolSvc tools. <<<"

#====================================================================
# SET UP STREAM   
#====================================================================
BPHY8_streamName  = derivationFlags.WriteDAOD_BPHY8Stream.StreamName
BPHY8_fileName    = buildFileName( derivationFlags.WriteDAOD_BPHY8Stream )
BPHY8Stream = MSMgr.NewPoolRootStream(BPHY8_streamName, BPHY8_fileName )
BPHY8Stream.AcceptAlgs(["BPHY8Kernel"])
#
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
##
## NOTE: We use the ThinningHelper which already instantiates a ThinningSvc
##
## from AthenaServices.Configurables import ThinningSvc, createThinningSvc
## BPHY8_augStream = MSMgr.GetStream( BPHY8_streamName )
## BPHY8_evtStream = BPHY8_augStream.GetEventStream()
## svcMgr += createThinningSvc(svcName=BPHY8cf.DerivationName+"ThinningSvc",
##           outStreams=[BPHY8_evtStream] )

# Additional metadata output
BPHY8Stream.AddMetaDataItem([ "xAOD::FileMetaData#%s*" %
                              BPHY8cf.DerivationName,
                              "xAOD::FileMetaDataAuxInfo#%s*Aux." %
                              BPHY8cf.DerivationName] )

#====================================================================
# Thinning Helper and various thinning tools
#====================================================================
#--------------------------------------------------------------------
# Setup the thinning helper, only tool able to perform thinning
# of trigger navigation information.
#
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY8ThinningHelper = ThinningHelper( BPHY8cf.DerivationName+"ThinningHelper" )
## BPHY8ThinningHelper.TriggerChains = 'HLT_.*mu.*' #triggerList	# . = any character; * = 0 or more times; + = 1 or more times; ? 0 or 1 times  "Regular_Expression"
BPHY8ThinningHelper.AppendToStream( BPHY8Stream )

#--------------------------------------------------------------------
# Thinning tools
BPHY8ThinningTools = []

#
# MC Truth Thinning
#
if BPHY8cf.isSimulation:
    #
    # PDG-ID list of truth decay particles whose decay chains are to be recorded
    # B mesons
    BPHY8cf.TruthDecayParents = [511, 521, 10511, 10521, 513, 523, 10513, 10523, 20513, 20523, 515, 525, 531, 10531, 533, 10533, 20533, 535, 541, 10541, 543, 10543, 20543, 545]
    # b bbar mesons
    BPHY8cf.TruthDecayParents += [551,10551,100551,110551,200551,210551,553,10553,20553,30553,100553,110553,120553,130553,200553,210553,220553,300553,9000553,9010553,555,10555,20555,100555,110555,120555,200555,557,100557]
    BPHY8cf.TruthDecayParents += [5122,5112,5212,5222,5114,5214,5224,5132,5232,5312,5322,5314,5324,5332,5334,5142,5242,5412,5422,5414,5424,5342,5432,5434,5442,5444,5512,5522,5514,5524,5532,5534,5542,5544,5554]
    # Charmed mesons
    ## BPHY8cf.TruthDecayParents += [411, 421, 10411, 10421, 413, 423, 10413, 10423, 20413, 20423, 415, 425, 431, 10431, 433, 10433, 20433, 435]
    # c cbar mesons
    ## BPHY8cf.TruthDecayParents += [441, 10441, 100441, 443, 10443, 20443, 100443, 30443, 9000443, 9010443, 9020443, 445, 100445]
    # charmed baryons
    ## BPHY8cf.TruthDecayParents += [4122, 4222, 4212, 4112, 4224, 4214, 4114, 4232, 4132, 4322, 4312, 4324, 4314, 4332, 4334, 4412, 4422, 4414, 4424, 4432, 4434, 4444]

    # compose ParticleSelectionString
    BPHY8_ParticleSelConds = []
    for BPHY8_pdgid in BPHY8cf.TruthDecayParents:
        BPHY8_ParticleSelConds.append("abs(TruthParticles.pdgId) == %d" %
                                      BPHY8_pdgid) 
        BPHY8_ParticleSelection = " || ".join(BPHY8_ParticleSelConds)

    # Only save truth information directly associated with B decays.
    # We'll skip the GEANT particles (barcode >= 200000).
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf \
        import DerivationFramework__GenericTruthThinning
    BPHY8TruthThinTool = DerivationFramework__GenericTruthThinning(
        name                    = BPHY8cf.DerivationName+"TruthThinTool",
        ThinningService         = BPHY8ThinningHelper.ThinningSvc(),
        ParticleSelectionString = BPHY8_ParticleSelection,
        PreserveGeneratorDescendants = True,
        PreserveDescendants     = False,
        PreserveAncestors       = False)

    ToolSvc += BPHY8TruthThinTool
    BPHY8ThinningTools.append(BPHY8TruthThinTool)
    print BPHY8TruthThinTool
    pprint(BPHY8TruthThinTool.properties())

#
# Vertex thinning
#
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__Thin_vtxTrk

# Build list of input containers and passed flags depending on
# "select tools" used.
import re
BPHY8_vtxContainers   = []
BPHY8_refPVContainers = []
BPHY8_passedFlags     = []
for BPHY8_tool in BPHY8_SelectTools.values():
    BPHY8_vtxContainers.append(BPHY8_tool.InputVtxContainerName)
    BPHY8_refPVContainers.append(re.sub('Candidates$',
                                        'RefittedPrimaryVertices',
                                        BPHY8_tool.InputVtxContainerName))
    BPHY8_passName = BPHY8_tool.HypothesisName
    if BPHY8_tool.HypothesisName == "Jpsimumu" and BPHY8cf.thinLevel > 0:
        BPHY8_passName = "JpsimumuSubDecay"
    BPHY8_passedFlags.append("passed_%s" %  BPHY8_passName)
#
# Use the general Thin_vtxTrk tool to thin the vertex containers only.
#
if BPHY8cf.thinLevel < 2:
    BPHY8Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
        name                       = BPHY8cf.DerivationName+"Thin_vtxTrk",
        ThinningService            = BPHY8ThinningHelper.ThinningSvc(),
        TrackParticleContainerName = BPHY8cf.TrkPartContName,
        VertexContainerNames       = BPHY8_vtxContainers,
        PassFlags                  = BPHY8_passedFlags,
        ThinTracks                 = False)
    ToolSvc += BPHY8Thin_vtxTrk
    BPHY8ThinningTools.append(BPHY8Thin_vtxTrk)
    print BPHY8Thin_vtxTrk
    pprint(BPHY8Thin_vtxTrk.properties())
    
#
# Bmumu PV, muon collections and ID track thinnning
#
if BPHY8cf.thinLevel > 1:
    from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
        import DerivationFramework__BmumuThinningTool
    BPHY8BmumuThinningTool = DerivationFramework__BmumuThinningTool(
        name                       = BPHY8cf.DerivationName+"BmumuThinningTool",
        ThinningService            = BPHY8ThinningHelper.ThinningSvc(),
        TrackParticleContainerName = BPHY8cf.TrkPartContName,
        VertexContainerNames       = BPHY8_vtxContainers,
        VertexPassFlags            = BPHY8_passedFlags,
        RefPVContainerNames        = BPHY8_refPVContainers,
        # RefPVContainerNames        = BPHY8cf.RefPVContNames,
        AlignPassToVertexList      = True,
        PVContainerName            = BPHY8cf.PVContName,
        MuonContainerName          = BPHY8cf.MuonCollection,
        CalibMuonContainerName     = BPHY8cf.CalMuonCollection,
        MatchCalibratedMuons       = (BPHY8cf.useCalibratedMuons > 2),
        MarkMatchedMuons           = (BPHY8cf.useCalibratedMuons > 2),
        MarkMatchedCalMuons        = (BPHY8cf.useCalibratedMuons > 1
                                      and BPHY8cf.useCalibratedMuons < 3),
        SyncMatchedMuonsBothWays   = True,
        AllowFastMuonMaskSync      = True,
        KeepTracksForMuons         = True,
        KeepTracksForCalMuons      = True,
        KeepMuonsForTracks         = True,
        KeepCalMuonsForTracks      = True,
        KeepCloseTracks            = True,
        ThinMuons                  = True,
        CloseTrackBranchPrefixes   = BPHY8cf.BranchPrefixes,
        CloseTrackBranchBaseName   = BPHY8_IsoTools["TrackVtxCt"].BranchBaseName,
        ThinPVs                    = True,
        ThinRefittedPVs            = True,
        ThinTracks                 = True,
        KeepTracksForSelectedPVs   = False,
        OutputLevel                = INFO)
    ToolSvc += BPHY8BmumuThinningTool
    BPHY8ThinningTools.append(BPHY8BmumuThinningTool)
    print BPHY8BmumuThinningTool
    pprint(BPHY8BmumuThinningTool.properties())

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel!
# If you don't do that, they will not be be executed!
# The name of the kernel (BPHY8Kernel in this case) must be unique to
# this derivation.
# Make use of a AthSequence in order to run the muon calibrations
# beforehand if requested.
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY8_seq = CfgMgr.AthSequencer("BPHY8Sequence")
DerivationFrameworkJob += BPHY8_seq
BPHY8_seq += BPHY8_CalibrationAlgs.values()
BPHY8_seq += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY8Kernel",
    OutputLevel = INFO,
    AugmentationTools = [ BPHY8_MetaDataTool, BPHY8_AugOriginalCounts ] \
    + BPHY8_RecoTools.values() + BPHY8_MuMassTools.values() \
    + BPHY8_IsoTools.values() \
    + BPHY8_SelectTools.values(),
    SkimmingTools     = [BPHY8_SkimmingTool],
    ThinningTools     = BPHY8ThinningTools
   )

#====================================================================
# Slimming 
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY8SlimmingHelper = SlimmingHelper("BPHY8SlimmingHelper")
BPHY8_AllVariables     = []
BPHY8_StaticContent    = []
BPHY8_SmartCollections = []
BPHY8_ExtraVariables   = []

# Needed for trigger objects
BPHY8SlimmingHelper.IncludeMuonTriggerContent  = BPHY8cf.doTriggerInfo
BPHY8SlimmingHelper.IncludeBPhysTriggerContent = BPHY8cf.doTriggerInfo

# primary vertices
BPHY8_AllVariables += [BPHY8cf.PVContName]
for BPHY8_reco in BPHY8_recoList:
    BPHY8_StaticContent \
        += ["xAOD::VertexContainer#BPHY8"+BPHY8_reco+"RefittedPrimaryVertices"]
    BPHY8_StaticContent \
        += ["xAOD::VertexAuxContainer#BPHY8"+BPHY8_reco+"RefittedPrimaryVerticesAux."]

# combined / extrapolated muon track particles 
# (note: for tagged muons there is no extra TrackParticle collection since
# the ID tracks are stored in InDetTrackParticles collection)
BPHY8_AllVariables += ["CombinedMuonTrackParticles"]
BPHY8_AllVariables += ["ExtrapolatedMuonTrackParticles"]
# TODO: copy smart slimming for calibrated muons.
if BPHY8cf.useCalibratedMuons > 1:
    BPHY8_AllVariables += [BPHY8cf.CalMuonCollection]
    BPHY8SlimmingHelper.AppendToDictionary = {
        '%s'    % BPHY8cf.CalMuonCollection : 'xAOD::MuonContainer',
        '%sAux' % BPHY8cf.CalMuonCollection : 'xAOD::ShallowAuxContainer' }

# muon container
## AllVariables += [BPHY8cf.MuonCollection]
# smart collection adds info needed for CP tools
BPHY8_SmartCollections += [BPHY8cf.MuonCollection]
BPHY8_ExtraVariables   += ["%s.etcone30.etcone40" % BPHY8cf.MuonCollection
                           +".momentumBalanceSignificance"
                           +".scatteringCurvatureSignificance"
                           +".scatteringNeighbourSignificance"
                           +".msInnerMatchDOF.msInnerMatchChi2"
                           +".msOuterMatchDOF.msOuterMatchChi2"
                           +".EnergyLoss.ParamEnergyLoss.MeasEnergyLoss"
                           +".ET_Core" ]

# ID track particles
BPHY8_SmartCollections += [BPHY8cf.TrkPartContName]
BPHY8_ExtraVariables += ["%s.vx.vy" % BPHY8cf.TrkPartContName]

# decay candidates 
# we have to disable vxTrackAtVertex branch since it is not xAOD compatible
for BPHY8_name in BPHY8_RecoTools.keys():
    BPHY8_StaticContent += ["xAOD::VertexContainer#%s" %
                            BPHY8_RecoTools[BPHY8_name].OutputVtxContainerName]
    BPHY8_StaticContent += ["xAOD::VertexAuxContainer#%sAux." %
                            BPHY8_RecoTools[BPHY8_name].OutputVtxContainerName]
    BPHY8_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" %
                            BPHY8_RecoTools[BPHY8_name].OutputVtxContainerName]

# Truth information for MC only
if BPHY8cf.isSimulation:
    BPHY8_AllVariables += ["TruthEvents","TruthParticles","TruthVertices"]

BPHY8SlimmingHelper.AllVariables     = BPHY8_AllVariables
BPHY8SlimmingHelper.SmartCollections = BPHY8_SmartCollections
BPHY8SlimmingHelper.StaticContent    = BPHY8_StaticContent
BPHY8SlimmingHelper.ExtraVariables   = BPHY8_ExtraVariables
BPHY8SlimmingHelper.AppendContentToStream(BPHY8Stream)

#====================================================================
# END OF BPHY8.py
#====================================================================
