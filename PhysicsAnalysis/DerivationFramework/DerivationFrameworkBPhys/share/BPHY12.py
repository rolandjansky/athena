#==============================================================================
# BPHY12.py (use reductionConf flag BPHY12 in Reco_tf.py)
# Semileptonic rare b -> s/d mu+mu- decays:
#  - Bd -> K*(Kpi)mumu
#  - TODO: Other channels
#  - TODO: Rewrite to python3-valid code (namely printing)
#==============================================================================

# General scheme:
#  - list of triggers: two options: any trigger and a complied list
#  - di-muon vertex
#  - B-vertex
#  - Redone K* vertices
#  - include both Bd and \bar{Bd}
#  - decorations
#  - MC truth
#  - slimming / thinning / slimming
#  - keep all tracks without covariances (finders may try to remove used signal tracks !)
#  - keep all PV without covariances
#  - keep all muons without covariances

# Lists for better code organization
augsList          = [] # List of active augmentation tools
skimList          = [] # List of active skimming algorithms
thinList          = [] # List of active thinning algorithms
outVtxList        = [] # List of reconstructed candidates to store
outRePVList       = [] # List of candidates holding refitted primary vertices
thinTrkVtxList    = [] # List of reconstructed candidates to use for the thinning of tracks from vertices
thinPassFlagsList = [] # List of pass-flags in the reconstructed candidates to se for the thinning



#------------------------------------------------------------------------------
# Common services
#  - DONE: Test isSimulation flag
#------------------------------------------------------------------------------

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *



from pprint import pprint

#------------------------------------------------------------------------------
# Metadata for this derivation settings
#  - TODO: Test the metadata are stored and work
#------------------------------------------------------------------------------

# Set up specific metadata configuration tool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BdKstarMuMu_metadata
BPHY12_MetaData = DerivationFramework__BdKstarMuMu_metadata( name              = "BPHY12_Metadata",
                                                             DerivationName    = "BPHY12",
                                                             version           = "v0.9", # derivation version (update with every update of the derivation)
                                                             OutputLevel       = WARNING,
                                                             verbose           = 1,      # TODO: verbose output (currently by default)
                                                             looseCuts         = False,  # TODO: apply loose cuts (debugging only)
                                                             skimTrig          = False,  # skim data by selected triggers
                                                             skimData          = True,   # skim data by passed B-candidates
                                                             thinData          = True,   # thin ID tracks, muons and PVs
                                                             slimData          = False,  # TODO: data slimming
                                                             thinMC            = True,   # thin MC-truth (keep wide range of heavy hadrons)
                                                             thinMCsignal      = False,  # thin MC-truth to signal-only (keep only signal PDG b-hadrons)
                                                             trigObjects       = True )  # store trigger objects for B-physics and muons

# Local shorthand and ensure default contents of __slots__ dict are available as attributes
from DerivationFrameworkBPhys.BPhysPyHelpers import BPhysEnsureAttributes
BPHY12cf = BPhysEnsureAttributes(BPHY12_MetaData)

print '********** BPHY12 Default Metadata **********'
ToolSvc += BPHY12_MetaData

# Force verbose output (debugging only)
#svcMgr.MessageSvc.debugLimit = 5000000

# Data or simulation?
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
BPHY12cf.isSimulation = DerivationFrameworkHasTruth

# Project tag
BPHY12cf.projectTag = rec.projectName()

# Trigger stream name
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary is not None:
  BPHY12cf.triggerStream = inputFileSummary['tag_info']['triggerStreamOfFile']

# Release 21 or newer?
from PyJobTransforms.trfUtils import releaseIsOlderThan
BPHY12cf.isRelease21 = not releaseIsOlderThan(21,0)

# MC campaigns by MC run number (keep in sync with BPHY8)
BPHY12MCcampaigns = { 284500 : 'mc16a',
                      300000 : 'mc16d',
                      310000 : 'mc16e' }

# Run number and MC campaign by MC run number
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as BPHY12_af
BPHY12_f = BPHY12_af.fopen(athenaCommonFlags.PoolAODInput()[0])
BPHY12cf.mcCampaign = 'unknown'
if len(BPHY12_f.run_numbers) > 0:
  BPHY12cf.runNumber = int(BPHY12_f.run_numbers[0])
  if BPHY12cf.isSimulation and BPHY12cf.runNumber in BPHY12MCcampaigns:
    BPHY12cf.mcCampaign = BPHY12MCcampaigns[BPHY12cf.runNumber]

pprint( BPHY12_MetaData.properties() )
print '********** BPHY12 Default Metadata (end) **********'



#------------------------------------------------------------------------------
# Trigger counting metadata
#  - TODO: Test metadata
#------------------------------------------------------------------------------

triggers2Metadata = [ # Main bBmumuxv2 triggers
                      "HLT_mu11_mu6_bBmumuxv2",
                      "HLT_2mu10_bBmumuxv2",
                      "HLT_2mu6_bBmumuxv2_L1LFV-MU6",
                      "HLT_mu11_mu6_bBmumux_BpmumuKp",
                      "HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
                      # Additional pure di-muon triggers
                      "HLT_mu11_mu6_bDimu",
                      "HLT_4mu4_bDimu6000" ]

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY12_TrigCountMetadata = DerivationFramework__TriggerCountToMetadata( name        = "BPHY12_TrigCountMetadata",
                                                                        TriggerList = triggers2Metadata,
                                                                        FolderName  = "BPHY12" )
print '********** BPHY12 Trigger Counts Metadata **********'
ToolSvc += BPHY12_TrigCountMetadata
pprint   ( BPHY12_TrigCountMetadata.properties() )
print '********** BPHY12 Trigger Counts Metadata (end) **********'



#------------------------------------------------------------------------------
# Trigger skimming
#  - Partly DONE: Test that skimming works (not all MC events accepted if active, TODO: Exactly test passed triggers)
#------------------------------------------------------------------------------

if BPHY12cf.skimTrig:
  triggersSkim = triggers2Metadata

  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
  BPHY12_Skim_Trig = DerivationFramework__TriggerSkimmingTool( name           = "BPHY12_Skim_Trig",
                                                               TriggerListOR  = triggersSkim,
                                                               TriggerListAND = [] )
  print '********** BPHY12 Trigger Skimming **********'
  skimList += [ BPHY12_Skim_Trig ]
  ToolSvc  +=   BPHY12_Skim_Trig
  pprint      ( BPHY12_Skim_Trig.properties() )
  print '********** BPHY12 Trigger Skimming (end) **********'



#------------------------------------------------------------------------------
# Vertexing tools and services
#------------------------------------------------------------------------------

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY12_VertexTools = BPHYVertexTools("BPHY12")

print '********** BPHY12 Vertex Tools **********'
print BPHY12_VertexTools
print BPHY12_VertexTools.TrkV0Fitter
print '********** BPHY12 Vertex Tools (end) **********'



#------------------------------------------------------------------------------
# Augment orignal counts of vertices and tracks ?
#  - DONE: Test newly agumented data
#  - TODO: Possibly remove if case basic info on all tracks and vertices is kept
#------------------------------------------------------------------------------

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY12_AugOriginalCounts = DerivationFramework__AugOriginalCounts( name            = "BPHY12_AugOriginalCounts",
                                                                   VertexContainer = "PrimaryVertices",
                                                                   TrackContainer  = "InDetTrackParticles" )
print '********** BPHY12 Augment Original Counts **********'
augsList += [ BPHY12_AugOriginalCounts ]
ToolSvc  +=   BPHY12_AugOriginalCounts
pprint      ( BPHY12_AugOriginalCounts.properties() )
print '********** BPHY12 Augment Original Counts (end) **********'



#------------------------------------------------------------------------------
# Di-muon vertex finder
#  - TODO: Make sure these are all options needed and check defaults
#  - TODO: Start invariant mass at zero? (check size enlargement)
#  - TODO: Rename HypothesisName in BPHY12_Select_DiMuons to better match reality?
#  - TODO: Can Select_onia2mumu be forced to use reconstruced mass for the pseudo-proper decay time etc. calculations?
#------------------------------------------------------------------------------

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY12_Finder_DiMuon = Analysis__JpsiFinder( name                        = "BPHY12_Finder_DiMuon",
                                             OutputLevel                 = INFO,
                                             muAndMu                     = True,
                                             muAndTrack                  = False,
                                             TrackAndTrack               = False,
                                             assumeDiMuons               = True,
                                             muonThresholdPt             = 3000.,
                                             higherPt                    = 3500.,
                                             invMassUpper                = 7000.,
                                             invMassLower                = 1.,
                                             Chi2Cut                     = 30.,
                                             oppChargesOnly              = False,
                                             allChargeCombinations       = True,
                                             atLeastOneComb              = True,
                                             useCombinedMeasurement      = False, # Only takes effect if combOnly=True
                                             muonCollectionKey           = "Muons",
                                             TrackParticleCollection     = "InDetTrackParticles",
                                             V0VertexFitterTool          = BPHY12_VertexTools.TrkV0Fitter,
                                             useV0Fitter                 = False,
                                             TrkVertexFitterTool         = BPHY12_VertexTools.TrkVKalVrtFitter,
                                             TrackSelectorTool           = BPHY12_VertexTools.InDetTrackSelectorTool,
                                             ConversionFinderHelperTool  = BPHY12_VertexTools.InDetConversionHelper,
                                             VertexPointEstimator        = BPHY12_VertexTools.VtxPointEstimator,
                                             useMCPCuts                  = False )
print '********** BPHY12 DiMuon Finder **********'
ToolSvc += BPHY12_Finder_DiMuon
pprint   ( BPHY12_Finder_DiMuon.properties() )
print '********** BPHY12 DiMuon Finder (end) **********'

# Select and write the di-muon candidates, do no do PV refit
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY12_SelectAndWrite_DiMuon = DerivationFramework__Reco_mumu( name                     = "BPHY12_SelectAndWrite_DiMuon",
                                                               JpsiFinder               = BPHY12_Finder_DiMuon,
                                                               OutputVtxContainerName   = "BPHY12_DiMuon_Candidates",
                                                               PVContainerName          = "PrimaryVertices",
                                                               RefPVContainerName       = "SHOULDNOTBEUSED", # The container would be created if PV refit was requested (not needed at this point)
                                                               DoVertexType             = 7 ) # Vertex type marking our own reconstruced secondary candidates
print '********** BPHY12 DiMuon Writer **********'
augsList += [ BPHY12_SelectAndWrite_DiMuon ]
ToolSvc  +=   BPHY12_SelectAndWrite_DiMuon
pprint      ( BPHY12_SelectAndWrite_DiMuon.properties() )
print '********** BPHY12 DiMuon Writer (end) **********'

# Final selection of the di-muon candidates
thinTrkVtxList    += [ "BPHY12_DiMuon_Candidates" ]
outVtxList        += [ "BPHY12_DiMuon_Candidates" ]
thinPassFlagsList += [ "passed_Jpsi" ] # TODO: is this really needed?
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY12_Select_DiMuons = DerivationFramework__Select_onia2mumu( name                  = "BPHY12_Select_DiMuons",
                                                               HypothesisName        = "Jpsi", # should be changed ?
                                                               InputVtxContainerName = "BPHY12_DiMuon_Candidates",
                                                               VtxMassHypo           = BPHY12cf.mass_Jpsi, # used only for pseudo-proper decay time etc. calculations
                                                               MassMax               = 10000., # loose cut to keep selection from BPHY12_Finder_DiMuon
                                                               MassMin               = 0.,     # loose cut to keep selection from BPHY12_Finder_DiMuon
                                                               Chi2Max               = 1000.,  # loose cut to keep selection from BPHY12_Finder_DiMuon (chi2, not chi2/NDF)
                                                               DoVertexType          = 7 ) # Vertex type marking our own reconstruced secondary candidates
print '********** BPHY12 DiMuon Selector **********'
augsList += [ BPHY12_Select_DiMuons ]
ToolSvc  +=   BPHY12_Select_DiMuons
pprint      ( BPHY12_Select_DiMuons.properties() )
print '********** BPHY12 DiMuon Selector (end) **********'



#------------------------------------------------------------------------------
# Bd -> K*(Kpi)mumu finder (including same-charge of hadronic tracks)
#  - TODO: Is BPHY12_VertexFit_Bmeson really needed? (should only be needed when requesting mass-constraint)
#  - TODO: Make sure these are all options needed and check defaults
#  - TODO: Inefficiently too wide TrkQuadrupletMassLower and TrkQuadrupletMassUpper
#------------------------------------------------------------------------------

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY12_VertexFit_Bmeson = Trk__TrkVKalVrtFitter( name                = "BPHY12_VertexFit_Bmeson",
                                                 Extrapolator        = BPHY12_VertexTools.InDetExtrapolator,
                                                 FirstMeasuredPoint  = True,
                                                 MakeExtendedVertex  = True )
print '********** BPHY12 B-Vertex Tools **********'
ToolSvc += BPHY12_VertexFit_Bmeson
pprint   ( BPHY12_VertexFit_Bmeson.properties() )
print '********** BPHY12 B-Vertex Tools (end) **********'

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY12_Finder_BdKstarKpiMuMu = Analysis__JpsiPlus2Tracks( name                    = "BPHY12_Finder_BdKstarKpiMuMu",
                                                          OutputLevel             = INFO,
                                                          kaonkaonHypothesis      = False,
                                                          pionpionHypothesis      = False,
                                                          kaonpionHypothesis      = True,
                                                          kaonprotonHypothesis    = False,
                                                          oppChargesOnly          = False,  # keep same-side candidates for background cross-checks
                                                          SameChargesOnly         = False,
                                                          trkThresholdPt          = 500.,   # before vertexing
                                                          trkMaxEta               = 3.,     # before vertexing
                                                          DiTrackMassUpper        = 1110.,  # before vertexing, checks both Kpi and piK combination in OR
                                                          DiTrackMassLower        = 690.,   # before vertexing, checks both Kpi and piK combination in OR
                                                          DiTrackPt               = 500.,   # before vertexing
                                                          FinalDiTrackPt          = 500.,   # after vertexing
                                                          TrkQuadrupletMassUpper  = 10000., # before vertexing
                                                          TrkQuadrupletMassLower  = 1000.,  # before vertexing
                                                          BMassUpper              = 6500,   # after vertexing
                                                          BMassLower              = 3000.,  # after vertexing
                                                          BThresholdPt            = 1000.,  # after vertexing
                                                          Chi2Cut                 = 30./5., # after vertexing (cut on chi2/NDF)
                                                          JpsiContainerKey        = "BPHY12_DiMuon_Candidates",
                                                          TrackParticleCollection = "InDetTrackParticles",
                                                          ExcludeCrossJpsiTracks  = False,
                                                          TrkVertexFitterTool     = BPHY12_VertexFit_Bmeson,
                                                          TrackSelectorTool       = BPHY12_VertexTools.InDetTrackSelectorTool,
                                                          UseMassConstraint       = False )
print '********** BPHY12 Bd->K*mumu Finder **********'
ToolSvc += BPHY12_Finder_BdKstarKpiMuMu
pprint   ( BPHY12_Finder_BdKstarKpiMuMu.properties() )
print '********** BPHY12 Bd->K*mumu Finder (end) **********'

# Select and write the Bd->K*mumu candidates, run PV refit
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk
BPHY12_SelectAndWrite_BdKstarKpiMuMu  = DerivationFramework__Reco_dimuTrkTrk( name                   = "BPHY12_SelectAndWrite_BdKstarKpiMuMu",
                                                                              Jpsi2PlusTrackName     = BPHY12_Finder_BdKstarKpiMuMu,
                                                                              OutputVtxContainerName = "BPHY12_BdKstarKpiMuMu_Candidates",
                                                                              PVContainerName        = "PrimaryVertices",
                                                                              RefPVContainerName     = "BPHY12_BdKstarKpiMuMu_refitPV",
                                                                              RefitPV                = True,
                                                                              MaxPVrefit             = 10000,
                                                                              DoVertexType           = 7 )
print '********** BPHY12 Bd->K*mumu Writer and PV refitter **********'
augsList += [ BPHY12_SelectAndWrite_BdKstarKpiMuMu ]
ToolSvc  +=   BPHY12_SelectAndWrite_BdKstarKpiMuMu
pprint      ( BPHY12_SelectAndWrite_BdKstarKpiMuMu.properties() )
print '********** BPHY12 Bd->K*mumu Writer and PV refitter (end) **********'

# Final selection of the Bd->K*mumu candidates
#  - DONE: check that the hadronic tracks charge order corresponds to Bd and not BbBar (JpsiPlus2Tracks sets first hadronic track positive)
thinTrkVtxList    += [ "BPHY12_BdKstarKpiMuMu_Candidates" ]
outVtxList        += [ "BPHY12_BdKstarKpiMuMu_Candidates" ]
outRePVList       += [ "BPHY12_BdKstarKpiMuMu_refitPV" ]
thinPassFlagsList += [ "passed_Bd" ]
BPHY12_Select_BdKstarKpiMuMu = DerivationFramework__Select_onia2mumu( name                  = "BPHY12_Select_BdKstarKpiMuMu",
                                                                      HypothesisName        = "Bd",
                                                                      InputVtxContainerName = "BPHY12_BdKstarKpiMuMu_Candidates",
                                                                      TrkMasses             = [ BPHY12cf.mass_mu, BPHY12cf.mass_mu, BPHY12cf.mass_K, BPHY12cf.mass_pi ],
                                                                      VtxMassHypo           = BPHY12cf.mass_Bd,
                                                                      MassMax               = 10000., # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                      MassMin               = 0.,     # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                      Chi2Max               = 1000. ) # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu (chi2, not chi2/NDF)
print '********** BPHY12 Bd->K*mumu Selector **********'
augsList += [ BPHY12_Select_BdKstarKpiMuMu ]
ToolSvc  +=   BPHY12_Select_BdKstarKpiMuMu
pprint      ( BPHY12_Select_BdKstarKpiMuMu.properties() )
print '********** BPHY12 Bd->K*mumu Selector (end) **********'

# Final selection of the Bdbar->K*barmumu candidates
#  - DONE: check that the hadronic tracks charge order corresponds to Bd and not BbBar (JpsiPlus2Tracks sets first hadronic track positive)
thinPassFlagsList += [ "passed_Bdbar" ]
BPHY12_Select_BdKstarKpiMuMu_anti = DerivationFramework__Select_onia2mumu( name                  = "BPHY12_Select_BdKstarKpiMuMu_anti",
                                                                           HypothesisName        = "Bdbar",
                                                                           InputVtxContainerName = "BPHY12_BdKstarKpiMuMu_Candidates",
                                                                           TrkMasses             = [ BPHY12cf.mass_mu, BPHY12cf.mass_mu, BPHY12cf.mass_pi, BPHY12cf.mass_K ],
                                                                           VtxMassHypo           = BPHY12cf.mass_Bd,
                                                                           MassMax               = 10000., # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                           MassMin               = 0.,     # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                           Chi2Max               = 1000. ) # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu (chi2, not chi2/NDF)
print '********** BPHY12 Bdbar->K*barmumu Selector **********'
augsList += [ BPHY12_Select_BdKstarKpiMuMu_anti ]
ToolSvc  +=   BPHY12_Select_BdKstarKpiMuMu_anti
pprint      ( BPHY12_Select_BdKstarKpiMuMu_anti.properties() )
print '********** BPHY12 Bdbar->K*barmumu Selector (end) **********'

# Revertex the di-track candidates alone
outVtxList += [ "BPHY12_Kstar_ReVertexCandidates" ]
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex
BPHY12_ReVertex_Kstar = DerivationFramework__ReVertex( name                   = "BPHY12_ReVertex_Kstar",
                                                       InputVtxContainerName  = "BPHY12_BdKstarKpiMuMu_Candidates",
                                                       TrackIndices           = [ 2, 3 ],
                                                       TrkVertexFitterTool    = BPHY12_VertexFit_Bmeson,
                                                       OutputVtxContainerName = "BPHY12_Kstar_ReVertexCandidates" )
print '********** BPHY12 K*->Kpi ReVertexer **********'
augsList += [  BPHY12_ReVertex_Kstar ]
ToolSvc  +=    BPHY12_ReVertex_Kstar
pprint      (  BPHY12_ReVertex_Kstar.properties() )
print '********** BPHY12 K*->Kpi ReVertexer (end) **********'

# Final selection of the K*->Kpi revertexed candidates
BPHY12_Select_KstarKpi = DerivationFramework__Select_onia2mumu( name                  = "BPHY12_Select_KstarKpi",
                                                                HypothesisName        = "Kstar",
                                                                InputVtxContainerName = "BPHY12_Kstar_ReVertexCandidates",
                                                                TrkMasses             = [ BPHY12cf.mass_K, BPHY12cf.mass_pi ],
                                                                VtxMassHypo           = BPHY12cf.mass_Kstar,
                                                                MassMax               = 10000., # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                MassMin               = 0.,     # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                Chi2Max               = 1000. ) # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu (chi2, not chi2/NDF)
print '********** BPHY12 K*->Kpi Selector **********'
augsList += [ BPHY12_Select_KstarKpi ]
ToolSvc  +=   BPHY12_Select_KstarKpi
pprint      ( BPHY12_Select_KstarKpi.properties() )
print '********** BPHY12 K*->Kpi Selector (end) **********'

# Final selection of the K*bar->piK revertexed candidates
BPHY12_Select_KstarKpi_anti = DerivationFramework__Select_onia2mumu( name                  = "BPHY12_Select_KstarKpi_anti",
                                                                     HypothesisName        = "Kstarbar",
                                                                     InputVtxContainerName = "BPHY12_Kstar_ReVertexCandidates",
                                                                     TrkMasses             = [ BPHY12cf.mass_pi, BPHY12cf.mass_K ],
                                                                     VtxMassHypo           = BPHY12cf.mass_Kstar,
                                                                     MassMax               = 10000., # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                     MassMin               = 0.,     # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu
                                                                     Chi2Max               = 1000. ) # loose cut to keep selection from BPHY12_Finder_BdKstarKpiMuMu (chi2, not chi2/NDF)
print '********** BPHY12 K*bar->piK Selector **********'
augsList += [ BPHY12_Select_KstarKpi_anti ]
ToolSvc  +=   BPHY12_Select_KstarKpi_anti
pprint      ( BPHY12_Select_KstarKpi_anti.properties() )
print '********** BPHY12 K*bar->piK Selector (end) **********'



#------------------------------------------------------------------------------
# b -> X mumu
#  - TODO: Implement other semileptonic rare b-hadrons:
#     - B+ -> K+ mumu
#     - Bs -> phi mumu
#     - B0 -> rho mumu
#     - B+ -> K*+ mumu
#     - Lambda_b -> Lambda0 mumu
#     - B0 -> K0s mumu
#------------------------------------------------------------------------------



#------------------------------------------------------------------------------
# B-candidates skimming (only for real data)
#  - Partly DONE: test that skimming works (not all MC events are accepted if active, while number of B-candidates remains same)
#------------------------------------------------------------------------------

if BPHY12cf.skimData and not BPHY12cf.isSimulation:

  # TODO: Include other semileptonic rare b-hadrons and use OR in the skimming (or just one large expression bSkim)
  bSkim = "(count(BPHY12_BdKstarKpiMuMu_Candidates.passed_Bd > 0) + count(BPHY12_BdKstarKpiMuMu_Candidates.passed_Bdbar > 0)) > 0";

  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
  BPHY12_Skim_Bcandidates = DerivationFramework__xAODStringSkimmingTool( name       = "BPHY12_Skim_Bcandidates",
                                                                         expression = bSkim )
  print '********** BPHY12 B-Candidates Skimming **********'
  skimList += [ BPHY12_Skim_Bcandidates ]
  ToolSvc  +=   BPHY12_Skim_Bcandidates
  pprint      ( BPHY12_Skim_Bcandidates.properties() )
  print '********** BPHY12 B-Candidates Skimming (end) **********'



#------------------------------------------------------------------------------
# Combine all skimming together (AND)
#  - TODO: Test that it works (combines two skimmings together) as expected
#------------------------------------------------------------------------------

if len(skimList) > 1:

  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
  BPHY12_Skim = CfgMgr.DerivationFramework__FilterCombinationAND(name       = "BPHY12_Skim",
                                                                 FilterList = skimList )
  print '********** BPHY12 Combined Skimming **********'
  ToolSvc += BPHY12_Skim
  pprint   ( BPHY12_Skim.properties() )
  print '********** BPHY12 Combined Skimming (end) **********'

elif len(skimList) == 1:

  BPHY12_Skim = skimList[0]



#------------------------------------------------------------------------------
# Data thinning: tracks, muons, PV
#  - TODO: Test that each of the thinning works
#------------------------------------------------------------------------------

if BPHY12cf.thinData:

  # ID tracks
  from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
  BPHY12_Thin_VtxTracks = DerivationFramework__Thin_vtxTrk( name                       = "BPHY12_Thin_VtxTracks",
                                                            ThinningService            = "BPHY12ThinningSvc",
                                                            TrackParticleContainerName = "InDetTrackParticles",
                                                            VertexContainerNames       = thinTrkVtxList,
                                                            PassFlags                  = thinPassFlagsList )
  print '********** BPHY12 Track Thinning **********'
  thinList += [ BPHY12_Thin_VtxTracks ]
  ToolSvc  +=   BPHY12_Thin_VtxTracks
  pprint      ( BPHY12_Thin_VtxTracks.properties() )
  print '********** BPHY12 Track Thinning (end) **********'

  # Muons (TODO: thinning not used muons or something else ?)
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
  BPHY12_Thin_Muons = DerivationFramework__MuonTrackParticleThinning( name                   = "BPHY12_Thin_Muons",
                                                                      ThinningService        = "BPHY12ThinningSvc",
                                                                      MuonKey                = "Muons",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles" )
  print '********** BPHY12 Muon Thinning **********'
  thinList += [ BPHY12_Thin_Muons ]
  ToolSvc  +=   BPHY12_Thin_Muons
  pprint      ( BPHY12_Thin_Muons.properties() )
  print '********** BPHY12 Muon Thinning (end) **********'

  # Primary vertices
  from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
  BPHY12_Thin_PV = DerivationFramework__BPhysPVThinningTool( name                 = "BPHY12_Thin_PV",
                                                             ThinningService      = "BPHY12ThinningSvc",
                                                             CandidateCollections = thinTrkVtxList,
                                                             KeepPVTracks         = True )
  print '********** BPHY12 Primary Vertices Thinning **********'
  thinList += [ BPHY12_Thin_PV ]
  ToolSvc  +=   BPHY12_Thin_PV
  pprint      ( BPHY12_Thin_PV.properties() )
  print '********** BPHY12 Primary Vertices Thinning (end) **********'



#------------------------------------------------------------------------------
# MC thinning: only same b/c-meson decays and quarks
#  - TODO: Test that each of the thinning works (seems OK in log-files, but not in the size of the MC containers ! Try to get inspired by BPHY8)
#------------------------------------------------------------------------------

if BPHY12cf.isSimulation and (BPHY12cf.thinMC or BPHY12cf.thinMCsignal):

  # Keep all muons and electrons
  keepParticles = ('abs(TruthParticles.pdgId) == 11 || ' # mu
                   'abs(TruthParticles.pdgId) == 13')    # e
  # Keep only the potentially signal b-hadrons
  if BPHY12cf.thinMCsignal:
    keepParticles += (' || '
                      'abs(TruthParticles.pdgId) == 511 || ' # B0
                      'abs(TruthParticles.pdgId) == 513 || ' # B0*
                      'abs(TruthParticles.pdgId) == 515')    # B0**
  # Keep all heavy particles (B and D mesons)
  else:
    keepParticles += (' || '
                      '(abs(TruthParticles.pdgId) >=  400 && abs(TruthParticles.pdgId) <  500) || ' # D-mesons
                      '(abs(TruthParticles.pdgId) >=  500 && abs(TruthParticles.pdgId) <  600) || ' # B-mesons
                      '(abs(TruthParticles.pdgId) >= 4000 && abs(TruthParticles.pdgId) < 5000) || ' # D-baryons
                      '(abs(TruthParticles.pdgId) >= 5000 && abs(TruthParticles.pdgId) < 6000) || ' # B-baryons
                      'abs(TruthParticles.pdgId) == 100553 || ' # Upsilon(2S)
                      'abs(TruthParticles.pdgId) == 200553 || ' # Upsilon(3S)
                      'abs(TruthParticles.pdgId) == 3122 || abs(TruthParticles.pdgId) == 3124 || abs(TruthParticles.pdgId) == 4122 || abs(TruthParticles.pdgId) == 4124 || abs(TruthParticles.pdgId) == 5122 || abs(TruthParticles.pdgId) == 13122 || abs(TruthParticles.pdgId) == 14122 || abs(TruthParticles.pdgId) == 23122 || abs(TruthParticles.pdgId) == 33122') # Lambdas

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  BPHY12_Thin_TruthHadrons = DerivationFramework__GenericTruthThinning( name                    = "BPHY12_Thin_TruthHadrons",
                                                                        ParticleSelectionString = keepParticles,
                                                                        PreserveDescendants     = True,
                                                                        PreserveAncestors       = True)
  print '********** BPHY12 MC Particles Thinning **********'
  thinList += [ BPHY12_Thin_TruthHadrons ]
  ToolSvc  +=   BPHY12_Thin_TruthHadrons
  pprint      ( BPHY12_Thin_TruthHadrons.properties() )
  print '********** BPHY12 MC Particles Thinning (end) **********'

  # Save also neutrinos and b-quarks, without their decay trees
  BPHY12_Thin_TruthQuarks = DerivationFramework__GenericTruthThinning( name                    = "BPHY12_Thin_TruthQuarks",
                                                                       ParticleSelectionString = ('abs(TruthParticles.pdgId) ==  5 || '
                                                                                                  'abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14' ),
                                                                       PreserveDescendants     = False,
                                                                       PreserveAncestors       = False)
  print '********** BPHY12 MC Quarks Thinning **********'
  thinList += [ BPHY12_Thin_TruthQuarks ]
  ToolSvc  +=   BPHY12_Thin_TruthQuarks
  pprint      ( BPHY12_Thin_TruthQuarks.properties() )
  print '********** BPHY12 MC Quarks Thinning (end) **********'



#------------------------------------------------------------------------------
# Apply the augmentation, skimming and thinning
#------------------------------------------------------------------------------

print "BPHY12: List of augmentations: ", augsList
print "BPHY12: List of skimmigs: "     , skimList
print "BPHY12: List of thinnings: "    , thinList

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "BPHY12Kernel",
                                                                        AugmentationTools = augsList,
                                                                        SkimmingTools     = skimList,
                                                                        ThinningTools     = thinList )
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "BPHY12Kernel_TrigCountMetadata",
                                                                        AugmentationTools = [ BPHY12_TrigCountMetadata ],
                                                                        SkimmingTools     = [],
                                                                        ThinningTools     = [] )



#------------------------------------------------------------------------------
# Setup the output stream
#------------------------------------------------------------------------------

streamName   = derivationFlags.WriteDAOD_BPHY12Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY12Stream )
BPHY12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY12Stream.AcceptAlgs( [ "BPHY12Kernel" ] )



#------------------------------------------------------------------------------
# Thinning service
#------------------------------------------------------------------------------

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY12ThinningSvc = createThinningSvc( svcName    = "BPHY12ThinningSvc",
                                       outStreams = [ evtStream ] )
svcMgr += BPHY12ThinningSvc



#------------------------------------------------------------------------------
# Slimming service, defining what to store
#------------------------------------------------------------------------------

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY12SlimmingHelper = SlimmingHelper( "BPHY12SlimmingHelper" )

AllVariables   = []
StaticContent  = []
ExtraVariables = []

# Smart collections
# TODO: What is the difference w.r.t. adding them into AllVariables?
BPHY12SlimmingHelper.SmartCollections = [ "Electrons", "Muons", "InDetTrackParticles" ]

# Full combined muon-ID tracks
AllVariables += [ "CombinedMuonTrackParticles" ]
AllVariables += [ "ExtrapolatedMuonTrackParticles" ]

# Extra variables
# TODO: Where do these come from? Are they missed due to the "smart" collection?
ExtraVariables += [ "Muons.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Hits.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits",
                    "Muons.numberOfTriggerEtaLayers.numberOfPhiLayers",
                    "InDetTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits.vx.vy.vz",
                    "PrimaryVertices.chiSquared.covariance" ]

# Include also trigger objects
# DONE: Test it works (HLT objects appear/not-present)
if BPHY12cf.trigObjects:
  BPHY12SlimmingHelper.IncludeMuonTriggerContent  = True
  BPHY12SlimmingHelper.IncludeBPhysTriggerContent = True

# Include primary vertices
AllVariables  += [ "PrimaryVertices" ]
print "BPHY12: List of refitted-PV output: ", outRePVList
for i in outRePVList:
  StaticContent += [ "xAOD::VertexContainer#%s"        % i ]
  StaticContent += [ "xAOD::VertexAuxContainer#%sAux." % i ]

# B-vertexing output
print "BPHY12: List of B-vertexing output: ", outVtxList
for i in outVtxList:
  StaticContent += [ "xAOD::VertexContainer#%s"                        % i ]
  StaticContent += [ "xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % i ]
  if [ "ReVertex" in i ]:
    StaticContent += [ "xAOD::VertexAuxContainer#%sAux." % i ]

print "BPHY12: Full list of B-augmentation: ", StaticContent

# Truth information for MC only
if BPHY12cf.isSimulation:
  AllVariables += [ "TruthEvents",
                    "TruthParticles",
                    "TruthVertices",
                    "MuonTruthParticles" ]

# Remove duplicates
AllVariables = list(set(AllVariables))

BPHY12SlimmingHelper.AllVariables   = AllVariables
BPHY12SlimmingHelper.ExtraVariables = ExtraVariables
BPHY12SlimmingHelper.StaticContent  = StaticContent
BPHY12SlimmingHelper.AppendContentToStream(BPHY12Stream)
