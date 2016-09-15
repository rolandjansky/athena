#====================================================================
# BPHY8.py
#
# THIS IS WORK IN PROGRESS.  ONLY BASIC FEATURES ARE IMPLEMENTED SO FAR.
#
# Author : W. Walkowiak, <wolfgang.walkowiak@cern.ch>
# Changes:
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

# Set up Bmumu configuration (metadata) tracking tool.
# This tool imports our defaults from Bmumu_metadata.cxx.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf \
    import DerivationFramework__Bmumu_metadata
BPHY8_MetaDataTool = DerivationFramework__Bmumu_metadata( 
    name           = "BPHY8_metadata",
    DerivationName = "BPHY8",
    OutputLevel    = INFO,
    # verbosity of python script (0 - 10)
    verbose        = 10
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

print "BPHY8: isSimulation = %s" % BPHY8cf.isSimulation
print "BPHY8: DerivationFrameworkIsMonteCarlo = %s" % \
    DerivationFrameworkIsMonteCarlo

#====================================================================
# MC dataset categories (lists of dataset numbers)
#====================================================================
BPHY8cf.mcBmumu          = [300426,300430,300446,300447]
BPHY8cf.mcBplusJpsiKplus = [300999,300404,300405]
BPHY8cf.mcBsJpsiPhi      = [300401,300438,300448,300449]

#====================================================================
# Defaults for BPHY8 configuration
#============================================pw========================
# Blind search?
BPHY8cf.doBmumuBlinding = True

# wide mumu mass range?
BPHY8cf.doUseWideMuMuMassRange = False

# other default settings
BPHY8cf.GlobalChi2Cut    = 15.

# Global mass values (in MeV, from PDG 2015)
BPHY8cf.GlobalMuonMass  = 105.6584
BPHY8cf.GlobalKaonMass  = 493.677
BPHY8cf.GlobalJpsiMass  = 3096.92
BPHY8cf.GlobalBplusMass = 5279.29
BPHY8cf.GlobalB0Mass    = 5279.61
BPHY8cf.GlobalBsMass    = 5366.79

# Cut values for kaon candidates
BPHY8cf.GlobalKaonPtCut  = 1000.
BPHY8cf.GlobalKaonEtaCut = 2.5

# primary vertex association types (interpreted bit-wise)
BPHY8cf.doVertexType = 7

# use invariant mass based on combined muon track information in mass cuts?
BPHY8cf.useMuCalcMass = True

# MCP cuts for JpsiFinder
BPHY8cf.useJpsiFinderMCPCuts = False

# reject muons in JpsiPlus1Track or JpsiPlus2Track finders
BPHY8cf.GlobalMuonsUsedInJpsi = "NONE"  # default to turn it off

#====================================================================
# General job setup
#====================================================================
# run number
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
BPHY8_f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
if len(BPHY8_f.run_numbers) > 0:
    BPHY8cf.runNumber = int(BPHY8_f.run_numbers[0])

# for MC run specific channel(s) only,
# for data run 2-, 3- and 4-prong algorithms in parallel
if BPHY8cf.isSimulation:
    # MC channel number (ie dataset number for MC)
    if len(BPHY8_f.infos['mc_channel_number']) > 0:
        BPHY8cf.mcChNumber = int((BPHY8_f.infos['mc_channel_number'])[0])
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBmumu):
            BPHY8cf.doChannels.append("Bmumu") 
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBplusJpsiKplus):
            BPHY8cf.doChannels.append("BJpsiK")
        if (BPHY8cf.mcChNumber in BPHY8cf.mcBsJpsiPhi):
            BPHY8cf.doChannels.append("BsJpsiPhi")
    # no blind search for MC
    BPHY8cf.doBmumuBlinding = False
else:
    # for data
    BPHY8cf.doChannels += ["Bmumu", "BJpsiK", "BsJpsiPhi"]
    
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
# AUGMENTATION TOOLS 
#====================================================================
# setup vertexing tools and services
include( "JpsiUpsilonTools/configureServices.py" )

# ordered dicts
from collections import OrderedDict

# we need to have the DiMuon finder running for channels with Jpsi as well
BPHY8_recoList = []
if [BPHY8_i for BPHY8_i in BPHY8cf.doChannels \
    if BPHY8_i in ["Bmumu", "BJpsiK", "BsJpsiPhi"]]:
    BPHY8_recoList += ["DiMuon"]
if "BJpsiK"    in BPHY8cf.doChannels: BPHY8_recoList += ["BJpsiK"]
if "BsJpsiPhi" in BPHY8cf.doChannels: BPHY8_recoList += ["BsJpsiPhi"]

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
            OutputLevel                 = DEBUG,
            muAndMu                     = True,
            muAndTrack                  = False,
            TrackAndTrack               = False,
            doTagAndProbe               = False,
            assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
            track1Mass                  = BPHY8cf.GlobalMuonMass,
            track2Mass                  = BPHY8cf.GlobalMuonMass,
            muonThresholdPt             = 0.,
            trackThresholdPt            = 0.,
            invMassUpper                = BPHY8cf.GlobalDiMuonMassUpperCut,
            invMassLower                = BPHY8cf.GlobalDiMuonMassLowerCut,
            Chi2Cut                     = BPHY8cf.GlobalChi2Cut,
            oppChargesOnly	        = True,
            sameChargesOnly             = False,
            allChargeCombinations       = False,
            allMuons                    = True,
            combOnly                    = False,
            atLeastOneComb              = False,
            useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
            muonCollectionKey           = "Muons",
            TrackParticleCollection     = "InDetTrackParticles",
            V0VertexFitterTool          = BPHY8_VertexTools[BPHY8_reco].TrkV0Fitter,             # V0 vertex fitter
            useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
            TrkVertexFitterTool         = BPHY8_VertexTools[BPHY8_reco].TrkVKalVrtFitter,        # VKalVrt vertex fitter
            TrackSelectorTool           = BPHY8_VertexTools[BPHY8_reco].InDetTrackSelectorTool,
            ConversionFinderHelperTool  = BPHY8_VertexTools[BPHY8_reco].InDetConversionHelper,
            VertexPointEstimator        = BPHY8_VertexTools[BPHY8_reco].VtxPointEstimator,
            useMCPCuts                  = BPHY8cf.useJpsiFinderMCPCuts )
    
# b) for BJpsiK
    if BPHY8_reco == "BJpsiK":
        BPHY8_FinderTools[BPHY8_reco] = Analysis__JpsiPlus1Track(
            name                        = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Finder",
            OutputLevel                 = DEBUG,
            pionHypothesis              = False,
            kaonHypothesis              = True,
            trkThresholdPt              = BPHY8cf.GlobalKaonPtCut,
            trkMaxEta                   = BPHY8cf.GlobalKaonEtaCut,
            BThresholdPt                = 1000.,
            BMassUpper                  = BPHY8cf.GlobalBMassUpperCut,
            BMassLower                  = BPHY8cf.GlobalBMassLowerCut,
            JpsiContainerKey            = BPHY8cf.DerivationName+"DiMuonCandidates",
            JpsiMassUpper               = BPHY8cf.GlobalJpsiMassUpperCut,
            JpsiMassLower               = BPHY8cf.GlobalJpsiMassLowerCut,
            MuonsUsedInJpsi             = BPHY8cf.GlobalMuonsUsedInJpsi,
            TrackParticleCollection     = "InDetTrackParticles",
            TrkVertexFitterTool         = BPHY8_VertexTools[BPHY8_reco].TrkVKalVrtFitter,        # VKalVrt vertex fitter
            TrackSelectorTool           = BPHY8_VertexTools[BPHY8_reco].InDetTrackSelectorTool,
            UseMassConstraint           = True)
    
# c) for BsJpsiPhi
    if BPHY8_reco == "BsJpsiPhi":
        BPHY8_FinderTools[BPHY8_reco] = Analysis__JpsiPlus2Tracks(
            name                        = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Finder",
            OutputLevel                 = DEBUG,
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
            TrackParticleCollection     = "InDetTrackParticles",
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
# and augmentation tools as e.g. the Jpsimumu containter ist needed for
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
            PVContainerName        = "PrimaryVertices",
            RefPVContainerName     = BPHY8cf.DerivationName+"DiMuonRefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            DoVertexType           = 7)
# b) for BJpsiK
    if BPHY8_reco == "BJpsiK":
        BPHY8_RecoTools[BPHY8_reco] = DerivationFramework__Reco_dimuTrk(
            name                   = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Reco",
            Jpsi1PlusTrackName     = BPHY8_FinderTools[BPHY8_reco],
            OutputVtxContainerName = BPHY8cf.DerivationName+BPHY8_reco+"Candidates",
            PVContainerName        = "PrimaryVertices",
            RefPVContainerName     = BPHY8cf.DerivationName+BPHY8_reco+"RefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            DoVertexType           = 7)
# c) for BsJpsiPhi
    if BPHY8_reco == "BsJpsiPhi":
        BPHY8_RecoTools[BPHY8_reco] = DerivationFramework__Reco_dimuTrkTrk(
            name                   = BPHY8cf.DerivationName+"_"+BPHY8_reco+"_Reco",
            Jpsi2PlusTrackName     = BPHY8_FinderTools[BPHY8_reco],
            OutputVtxContainerName = BPHY8cf.DerivationName+BPHY8_reco+"Candidates",
            PVContainerName        = "PrimaryVertices",
            RefPVContainerName     = BPHY8cf.DerivationName+BPHY8_reco+"RefittedPrimaryVertices",
            RefitPV                = True,
            MaxPVrefit             = 100000,
            DoVertexType           = 7)
        
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
# a) for Bmumu
if "Bmumu" in BPHY8cf.doChannels:
    # augment B(s)->mumu candidates
    BPHY8_MuMassTools["Bmumu"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_Bsmumu",
        BranchPrefix               = "Bsmumu",
        OutputLevel                = INFO,
        VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrackParticleContainerName = "InDetTrackParticles",
        MuonContainerName          = "Muons",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass])
# b) for BJpsiK and BsJpsiPhi retain the Jpsi
if [i for i in BPHY8cf.doChannels if i in ["BJpsiK", "BsJpsiPhi"]]:
    BPHY8_MuMassTools["Jpsimumu"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_Jpsimumu",
        BranchPrefix               = "Jpsimumu",
        OutputLevel                = INFO,
        VertexContainerName        = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrackParticleContainerName = "InDetTrackParticles",
        MuonContainerName          = "Muons",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass])
# c) for BJpsiK
if "BJpsiK" in BPHY8cf.doChannels:
    # augment B+/- ->JpsiK+/- candidates
    BPHY8_MuMassTools["BJpsiK"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_BJpsiK",
        BranchPrefix               = "BJpsiK",
        OutputLevel                = INFO,
        VertexContainerName        = BPHY8cf.DerivationName+"BJpsiKCandidates",
        TrackParticleContainerName = "InDetTrackParticles",
        MuonContainerName          = "Muons",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass])
# d) for BsJpsiPhi
if "BsJpsiPhi" in BPHY8cf.doChannels:
    # augment Bs ->JpsiPhi candidates
    BPHY8_MuMassTools["BsJpsiPhi"] = DerivationFramework__BPhysAddMuonBasedInvMass(
        name                       = "BPHY8_MuMass_BsJpsiPhi",
        BranchPrefix               = "BsJpsiPhi",
        OutputLevel                = INFO,
        VertexContainerName        = BPHY8cf.DerivationName+"BsJpsiPhiCandidates",
        TrackParticleContainerName = "InDetTrackParticles",
        MuonContainerName          = "Muons",
        TrkMasses                  = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass, BPHY8cf.GlobalKaonMass])
  
ToolSvc += BPHY8_MuMassTools.values()
for BPHY8_name in BPHY8_MuMassTools.keys():
    print BPHY8_MuMassTools[BPHY8_name] 
    pprint(BPHY8_MuMassTools[BPHY8_name].properties())
    
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
# a) for Bmumu
if "Bmumu" in BPHY8cf.doChannels:
    # augment and select B(s)->mumu candidates
    # WILL NEED TO ADD BLINDING!
    BPHY8_SelectTools["Bmumu"] = DerivationFramework__Select_Bmumu(
        name                  = "BPHY8_Select_Bsmumu",
        HypothesisName        = "Bsmumu",
        InputVtxContainerName = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses             = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        VtxMassHypo           = BPHY8cf.GlobalBsMass,
        MassMin               = BPHY8cf.GlobalBMassLowerCut,
        MassMax               = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max               = BPHY8cf.GlobalChi2Cut,
        DoVertexType          = BPHY8cf.doVertexType,
        BlindMassMin          = BPHY8cf.GlobalBlindLowerCut,
        BlindMassMax          = BPHY8cf.GlobalBlindUpperCut,
        DoBlinding            = BPHY8cf.doBmumuBlinding,
        UseMuCalcMass         = BPHY8cf.useMuCalcMass)
# b) for BJpsiK and BsJpsiPhi retain the Jpsi
if [i for i in BPHY8cf.doChannels if i in ["BJpsiK", "BsJpsiPhi"]]:
    BPHY8_SelectTools["Jpsimumu"] = DerivationFramework__Select_Bmumu(
        name                  = "BPHY8_Select_Jpsimumu",
        HypothesisName        = "Jpsimumu",
        InputVtxContainerName = BPHY8cf.DerivationName+"DiMuonCandidates",
        TrkMasses             = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass],
        VtxMassHypo           = BPHY8cf.GlobalJpsiMass,
        MassMin               = BPHY8cf.GlobalJpsiMassLowerCut,
        MassMax               = BPHY8cf.GlobalJpsiMassUpperCut,
        Chi2Max               = BPHY8cf.GlobalChi2Cut,
        DoVertexType          = BPHY8cf.doVertexType,
        UseMuCalcMass         = BPHY8cf.useMuCalcMass)
# c) for BJpsiK
if "BJpsiK" in BPHY8cf.doChannels:
    # augment and select B+/- ->JpsiK+/- candidates
    BPHY8_SelectTools["BJpsiK"] = DerivationFramework__Select_Bmumu(
        name                  = "BPHY8_Select_BJpsiK",
        HypothesisName        = "BJpsiK",
        InputVtxContainerName = BPHY8cf.DerivationName+"BJpsiKCandidates",
        TrkMasses             = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass],
        VtxMassHypo           = BPHY8cf.GlobalBplusMass,
        MassMin               = BPHY8cf.GlobalBMassLowerCut,
        MassMax               = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max               = BPHY8cf.GlobalChi2Cut,
        DoVertexType          = BPHY8cf.doVertexType,
        UseMuCalcMass         = BPHY8cf.useMuCalcMass)
# d) for BsJpsiPhi
if "BsJpsiPhi" in BPHY8cf.doChannels:
    # augment and select Bs ->JpsiPhi candidates
    BPHY8_SelectTools["BsJpsiPhi"] = DerivationFramework__Select_Bmumu(
        name                  = "BPHY8_Select_BsJpsiPhi",
        HypothesisName        = "BsJpsiPhi",
        InputVtxContainerName = BPHY8cf.DerivationName+"BsJpsiPhiCandidates",
        TrkMasses             = [BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalMuonMass, BPHY8cf.GlobalKaonMass, BPHY8cf.GlobalKaonMass],
        VtxMassHypo           = BPHY8cf.GlobalBsMass,
        MassMin               = BPHY8cf.GlobalBMassLowerCut,
        MassMax               = BPHY8cf.GlobalBMassUpperCut,
        Chi2Max               = BPHY8cf.GlobalChi2Cut,
        DoVertexType          = BPHY8cf.doVertexType,
        UseMuCalcMass         = BPHY8cf.useMuCalcMass)
  
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
    BPHY8_expressions += [ "count(%s.passed_%s) > 0" % \
                           (BPHY8_tool.InputVtxContainerName,
                            BPHY8_tool.HypothesisName) ]
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
BPHY8Stream.AddMetaDataItem( "IOVMetaDataContainer#*" )

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
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk

# Build list of input containers and passed flags depending on
# "select tools" used.
BPHY8_vtxContainers = []
BPHY8_passedFlags   = []
for BPHY8_tool in BPHY8_SelectTools.values():
    BPHY8_vtxContainers.append(BPHY8_tool.InputVtxContainerName)
    BPHY8_passedFlags.append("passed_%s" %  BPHY8_tool.HypothesisName)

# TODO: For now we use the general Thin_vtxTrk tool to thin the
# vertex containers only. Will likely create our own tool to
# add thinning of the track collection as well.
BPHY8Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
    name                       = BPHY8cf.DerivationName+"Thin_vtxTrk",
    ThinningService            = BPHY8ThinningHelper.ThinningSvc(),
    TrackParticleContainerName = "InDetTrackParticles",
    VertexContainerNames       = BPHY8_vtxContainers,
    PassFlags                  = BPHY8_passedFlags,
    ThinTracks                 = False)

ToolSvc += BPHY8Thin_vtxTrk
BPHY8ThinningTools.append(BPHY8Thin_vtxTrk)
print BPHY8Thin_vtxTrk
pprint(BPHY8Thin_vtxTrk.properties())
    
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel!
# If you don't do that, they will not be be executed!
# The name of the kernel (BPHY8Kernel in this case) must be unique to
# this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY8Kernel",
    AugmentationTools = [ BPHY8_MetaDataTool ] \
    + BPHY8_RecoTools.values() + BPHY8_MuMassTools.values() \
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
BPHY8SlimmingHelper.IncludeMuonTriggerContent  = True
BPHY8SlimmingHelper.IncludeBPhysTriggerContent = True

# primary vertices
BPHY8_AllVariables += ["PrimaryVertices"]
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

# muon container
## AllVariables += ["Muons"]
# smart collection adds info needed for CP tools
BPHY8_SmartCollections += ["Muons"]

# ID track particles
BPHY8_SmartCollections += ["InDetTrackParticles"]
BPHY8_ExtraVariables += ["InDetTrackParticles.vx.vy"]

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
# QUARRY AREA -- skip the rest for now -- QUARRY AREA
#====================================================================

# #====================================================================
# # AUGMENTATION TOOLS 
# #====================================================================
# ## 1/ setup vertexing tools and services
# include( "JpsiUpsilonTools/configureServices.py" )

# #--------------------------------------------------------------------
# ## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
# ##    These are general tools independent of DerivationFramework that do the 
# ##    actual vertex fitting and some pre-selection.
# from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
# BPHY8JpsiFinder = Analysis__JpsiFinder(
#   name                        = "BPHY8JpsiFinder",
#   OutputLevel                 = INFO,
#   muAndMu                     = True,
#   muAndTrack                  = False,
#   TrackAndTrack               = False,
#   assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
#   invMassUpper                = 100000.0,
#   invMassLower                = 0.0,
#   Chi2Cut                     = 200.,
#   oppChargesOnly	            = True,
#   atLeastOneComb              = True,
#   useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
#   muonCollectionKey           = "Muons",
#   TrackParticleCollection     = "InDetTrackParticles",
#   V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
#   useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
#   TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
#   TrackSelectorTool           = InDetTrackSelectorTool,
#   ConversionFinderHelperTool  = InDetConversionHelper,
#   VertexPointEstimator        = VtxPointEstimator,
#   useMCPCuts                  = False )
  
# ToolSvc += BPHY8JpsiFinder
# print      BPHY8JpsiFinder

# #--------------------------------------------------------------------
# ## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
# ##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
# ##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
# ##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
# ##    Reco tool is the JpsiFinder mass window is wide enough.

# from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
# BPHY8_Reco_mumu = DerivationFramework__Reco_mumu(
#   name                   = "BPHY8_Reco_mumu",
#   JpsiFinder             = BPHY8JpsiFinder,
#   OutputVtxContainerName = "BPHY8OniaCandidates",
#   PVContainerName        = "PrimaryVertices",
#   RefPVContainerName     = "BPHY8RefittedPrimaryVertices",
#   RefitPV                = True,
#   MaxPVrefit             = 100000,
#   DoVertexType           = 7)
  
# ToolSvc += BPHY8_Reco_mumu
# print BPHY8_Reco_mumu

# #--------------------------------------------------------------------
# ## 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
# ##    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
# ##    Property HypothesisName is used as a prefix for these decorations.
# ##    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
# ##    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
# ##    to determine which events and candidates should be kept in the output stream.
# ##    Multiple instances of the Select_* tools can be used on a single input collection as long as they 
# ##    use different "HypothesisName" flags.

# from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

# ## a/ augment and select Jpsi->mumu candidates
# BPHY8_Select_B2mumu = DerivationFramework__Select_onia2mumu(
#   name                  = "BPHY8_Select_B2mumu",
#   HypothesisName        = "Bmumu",
#   InputVtxContainerName = "BPHY8OniaCandidates",
#   VtxMassHypo           = 5366.79,
#   MassMin               = 4000.0,
#   MassMax               = 7000.0,
#   Chi2Max               = 200,
#   DoVertexType          = 7)
  
# ToolSvc += BPHY8_Select_B2mumu
# print BPHY8_Select_B2mumu

# #--------------------------------------------------------------------
# ## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
# ##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
# ##
# ##       "ContainerName.passed_HypoName > count"
# ##
# ##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
# ##    tool and "count" is the number of candidates passing the selection you want to keep. 

# expression = "count(BPHY8OniaCandidates.passed_Bmumu) > 0"
# from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
# BPHY8_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY8_SelectEvent",
#                                                                 expression = expression)
# ToolSvc += BPHY8_SelectEvent
# print BPHY8_SelectEvent

# #--------------------------------------------------------------------
# ## 6/ track and vertex thinning. We want to remove all reconstructed secondary vertices
# ##    which hasn't passed any of the selections defined by (Select_*) tools.
# ##    We also want to keep only tracks which are associates with either muons or any of the
# ##    vertices that passed the selection. Multiple thinning tools can perform the 
# ##    selection. The final thinning decision is based OR of all the decisions (by default,
# ##    although it can be changed by the JO).

# ## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
# ##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
# ##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
# ##    satisfy any of the listed selections.

# from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
# BPHY8Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
#   name                       = "BPHY8Thin_vtxTrk",
#   ThinningService            = "BPHY8ThinningSvc",
#   TrackParticleContainerName = "InDetTrackParticles",
#   VertexContainerNames       = ["BPHY8OniaCandidates"],
#   PassFlags                  = ["passed_Bmumu"] )

# ToolSvc += BPHY8Thin_vtxTrk

# ## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
# ##    between decision from this and the previous tools.
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
# BPHY8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY8MuonTPThinningTool",
#                                                                          ThinningService         = "BPHY8ThinningSvc",
#                                                                          MuonKey                 = "Muons",
#                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
# ToolSvc += BPHY8MuonTPThinningTool

# # Added by ASC
# # Only save truth informtion directly associated with Onia
# from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
# BPHY8TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY8TruthThinTool",
#                                                         ThinningService         = "BPHY8ThinningSvc",
#                                                         ParticleSelectionString = "TruthParticles.pdgId == 511 || TruthParticles.pdgId == -511 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == -531",
#                                                         PreserveDescendants     = True,
#                                                         PreserveAncestors      = True)
# ToolSvc += BPHY8TruthThinTool
# print BPHY8TruthThinTool


# #====================================================================
# # CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
# #====================================================================
# ## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
# ##    be executed!

# # Added by ASC
# BPHY8ThinningTools = [BPHY8Thin_vtxTrk, BPHY8MuonTPThinningTool]
# if globalflags.DataSource()=='geant4':
#     BPHY8ThinningTools.append(BPHY8TruthThinTool)

# # The name of the kernel (BPHY8Kernel in this case) must be unique to this derivation
# from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
# DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#   "BPHY8Kernel",
#    AugmentationTools = [BPHY8_Reco_mumu, BPHY8_Select_B2mumu],
#    SkimmingTools     = [BPHY8_SelectEvent],
#    ThinningTools     = BPHY8ThinningTools
#    )

# #====================================================================
# # SET UP STREAM   
# #====================================================================
# streamName = derivationFlags.WriteDAOD_BPHY8Stream.StreamName
# fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY8Stream )
# BPHY8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# BPHY8Stream.AcceptAlgs(["BPHY8Kernel"])
# # Special lines for thinning
# # Thinning service name must match the one passed to the thinning tools
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="BPHY8ThinningSvc", outStreams=[evtStream] )


# #====================================================================
# # Slimming 
# #====================================================================

# # Added by ASC
# from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
# BPHY8SlimmingHelper = SlimmingHelper("BPHY8SlimmingHelper")
# AllVariables = []
# StaticContent = []

# # Needed for trigger objects
# BPHY8SlimmingHelper.IncludeMuonTriggerContent = True
# BPHY8SlimmingHelper.IncludeBPhysTriggerContent = True

# ## primary vertices
# AllVariables += ["PrimaryVertices"]
# StaticContent += ["xAOD::VertexContainer#BPHY8RefittedPrimaryVertices"]
# StaticContent += ["xAOD::VertexAuxContainer#BPHY8RefittedPrimaryVerticesAux."]

# ## ID track particles
# AllVariables += ["InDetTrackParticles"]

# ## combined / extrapolated muon track particles 
# ## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
# ##        are store in InDetTrackParticles collection)
# AllVariables += ["CombinedMuonTrackParticles"]
# AllVariables += ["ExtrapolatedMuonTrackParticles"]

# ## muon container
# AllVariables += ["Muons"]

# ## Jpsi candidates 
# StaticContent += ["xAOD::VertexContainer#%s"        % BPHY8_Reco_mumu.OutputVtxContainerName]
# StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY8_Reco_mumu.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
# StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY8_Reco_mumu.OutputVtxContainerName]

# # Added by ASC
# # Truth information for MC only
# if isSimulation:
#     AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

# BPHY8SlimmingHelper.AllVariables = AllVariables
# BPHY8SlimmingHelper.StaticContent = StaticContent
# BPHY8SlimmingHelper.AppendContentToStream(BPHY8Stream)

#====================================================================
# Dummy skimming tool 
#====================================================================
# from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__SkimmingToolExample
# BPHY8_SkimmingTool = \
#    DerivationFramework__SkimmingToolExample(name = "BPHY8SkimmingTool",
#                                             MuonContainerKey = "Muons",
#                                             NumberOfMuons    = 1,
#                                             MuonPtCut        = 1000.0)

# ToolSvc += BPHY8_SkimmingTool
# print BPHY8_SkimmingTool

