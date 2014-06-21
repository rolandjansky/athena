# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TauD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import JetD3PDMaker
import TrackD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.resolveSGKey import resolveSGKey

#PanTau flags to switch PanTau specifics on/off
from PanTauAnalysis.PanTauFlags import panTauFlags
from PanTauAnalysis.PanTauValidationFlags import jobproperties as pantauJP


# Ensure that TauCommonDetailsMerger is run.
from TauTools.TauToolsConf import TauCommonDetailsMerger
merg = TauCommonDetailsMerger("TauCommonDetailsMerger")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if not merg in topSequence:
    # Make sure it goes before the d3pd algorithm.
    topSequence.insert (-1, merg)


TauD3PDObject = make_SGDataVector_D3PDObject( "Analysis::TauJetContainer",
                                              D3PDMakerFlags.TauSGKey(),
                                              "tau_", "TauD3PDObject" )

# Redo tau ID, if desired.
def _rerunTauID (c, **kw):
    DoUpstreamAlgs = False
    DoOnlyFakeBits = False
    DoTJVA         = False

    if not D3PDMakerFlags.RerunTauID():
        DoOnlyFakeBits = True   # no TauID

    if D3PDMakerFlags.RerunTauRec():
       DoUpstreamAlgs = True    # rerun tauRec
       DoOnlyFakeBits = False  # force to update TauID as well

    if DoUpstreamAlgs:
       from tauRec.TauRecAODBuilder import TauRecAODProcessor
       TauRecAODProcessor(inAODmode=True, sequence=AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()))

    from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
    TauDiscriGetter(do_upstream_algs=DoUpstreamAlgs,
                    do_only_fakebits=DoOnlyFakeBits,
                    do_Pi0=True,
        sequence=AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()))

    return

    
TauD3PDObject.defineHook (_rerunTauID)
    


#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------
TauD3PDObject.defineBlock (0, 'Kinematics',
        EventCommonD3PDMaker.FourMomFillerTool,
        WriteEt = True,
        WritePt = True)

TauD3PDObject.defineBlock (0, 'Charge',
        EventCommonD3PDMaker.ChargeFillerTool)

TauD3PDObject.defineBlock (0, 'PID',
        TauD3PDMaker.TauPIDFillerTool)

TauD3PDObject.defineBlock (0, 'TauJet',
        TauD3PDMaker.TauJetFillerTool)

TauD3PDObject.defineBlock (1, 'TauCommonDetails',
        TauD3PDMaker.TauCommonDetailsFillerTool)

TauD3PDObject.defineBlock (1, 'TauCalcVars',
        TauD3PDMaker.TauCalcVarsFillerTool)

TauD3PDObject.defineBlock (1, 'TauLeadTrack',
        TauD3PDMaker.TauLeadTrackFillerTool)

TauD3PDObject.defineBlock (2, 'TauClustersDetails',
        TauD3PDMaker.TauClustersDetailsFillerTool)

TauD3PDObject.defineBlock (2, 'TauPi0Clusters',
        TauD3PDMaker.TauPi0ClusterFillerTool)

TauD3PDObject.defineBlock (2, 'TauSecVtx',
        TauD3PDMaker.TauSecVtxFillerTool)

TauD3PDObject.defineBlock (2, 'TauPriVtx',
        TauD3PDMaker.TauPriVtxFillerTool)

TauD3PDObject.defineBlock (3, 'TauCellsDetails',
        TauD3PDMaker.TauCellsDetailsFillerTool)

# the pantau details from the TauPanTauDetailsContainer
TauD3PDObject.defineBlock (3, 'TauPanTauDetails',
        TauD3PDMaker.TauPanTauDetailsFillerTool)

#the complete set of PanTau details (only available when using PanTau_User_TopOptions.py manually)
#(needs the transient PanTau::TauSeed class)
#if panTauFlags.ExtractAllFeatures():
#    TauD3PDObject.defineBlock (3, 'PanTauDetails',
#                               TauD3PDMaker.PanTauDetailsFillerTool,
#                               FeatureNames = pantauJP.PanTauValidationFlags.AllFeatureNames(),
#                               FeatureDefaultValues = pantauJP.PanTauValidationFlags.AllFeatureDefaultValues(),
#                               FeatureDefaultValueSmearingSigma = pantauJP.PanTauValidationFlags.AllFeatureDefaultValueSmearingSigma(),
#                               SmearDefaultValuesOfFeatures = pantauJP.PanTauValidationFlags.DoFeatureDefaultSmearing() )

#TauD3PDObject.defineBlock (3, 'TauRadiusRecalc',
#        TauD3PDMaker.TauRadiusFillerTool)

###########
# Associate tau with its jet seed
TauJetAssoc = SimpleAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauJetAssociationTool)
TauJetAssoc.defineBlock \
            (2, 'TauJetAssocKine',
             EventCommonD3PDMaker.FourMomFillerTool,
             prefix='jet_',
             WriteEt = True,
             WritePt = True)

TauJetAssoc.defineBlock \
            (2, 'TauJetAssocMoments',
             JetD3PDMaker.JetMomentFillerTool, Moments=['WIDTH', 'n90', 'n90constituents', 'BCH_CORR_CELL',
                                                        'BCH_CORR_JET', 'BCH_CORR_JET_FORCELL',
                                                        'ENG_BAD_CELLS', 'N_BAD_CELLS', 'N_BAD_CELLS_CORR',
                                                        'BAD_CELLS_CORR_E', 'Timing', 'LArQuality', 'nTrk',
                                                        'sumPtTrk', 'OriginIndex', 'HECQuality', 'NegativeE',
                                                        'YFlip12', 'YFlip23', 'BCH_CORR_DOTX'],
             prefix='jet_')

TauJetAssoc.defineBlock \
                        (2, 'TauJetAssocJetSamplingsFrac',
                         JetD3PDMaker.JetSamplingsFracFillerTool, prefix='jet_')

TauJetAssoc.defineBlock(2, 'TauJetAssocCaloQual',
	                           JetD3PDMaker.JetCaloUtilsFillerTool, prefix='jet_')

TauJetAssoc.defineBlock(2, 'TauJetAssocEMFraction',
                        JetD3PDMaker.JetEMFractionFillerTool, prefix='jet_')

TauJetAssoc.defineBlock(2, 'TauJetAssocJESMoments',
                        JetD3PDMaker.JetMomentFillerTool,Moments=['GCWJES','EMJES'], prefix='jet_')
	                           
TauJetAssoc.defineBlock(2, 'TauJetAssocEMScale',
                        JetD3PDMaker.JetSignalStateFillerTool,
                        WriteE = True,
                        SignalStateNumber = 0,
                        SignalStatePrefix = 'emscale', prefix='jet_')
	
TauJetAssoc.defineBlock(2, 'TauJetAssocJVtx',
                        JetD3PDMaker.JetVtxFillerTool, prefix='jet_')

TauJetAssoc.defineBlock(2, 'JetVertexFraction',
                        JetD3PDMaker.JetVertexFractionFillerTool,
                        UDPrefix=D3PDMakerFlags.JetUserDataPrefix(),
                        prefix='jet_')

TauJetAssoc.defineBlock(2, 'TauJetAssocBTagDefault',
                        JetD3PDMaker.JetBTagWeightFillerTool,
                        BTagKey = '',
                        DoAllTags = False, prefix='jet_')
	
TauJetAssoc.defineBlock(2, 'TauJetAssocBTag',
                        JetD3PDMaker.JetBTagWeightFillerTool,
                        BTagKey = '',
                        DoAllTags = True, prefix='jet_')


TauJetAssoc.defineBlock(2, 'TauJetAssocSamplings',
                        JetD3PDMaker.JetSamplingsFillerTool, prefix='jet_')
	
TauJetAssoc.defineBlock(2, 'TauJetAssocJetShape',
                        JetD3PDMaker.JetShapeFillerTool, prefix='jet_')
	
###########
# HACK KG: Remove this block in the future?
# Associate tau with its track's perigees
TauTrackPerigeeAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauTrackPerigeeAssociationTool,prefix='track_atTJVA_')
TauTrackPerigeeAssoc.defineBlock (2, 'TrkTJVA',
                                  TrackD3PDMaker.TrackPerigeeFillerTool,
                                  FillMomentum = True)

TrackPerigeeCovarianceAssoc = SimpleAssociation(TauTrackPerigeeAssoc,
                                                TrackD3PDMaker.PerigeeCovarianceAssociationTool)

TrackPerigeeCovarianceAssoc.defineBlock(2, 'Covariance',
                                        TrackD3PDMaker.CovarianceFillerTool,
                                        DiagCovariance = True,
                                        OffDiagCovariance = True,
                                        IsTrackPerigee = True)

TauWideTrackPerigeeAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauWideTrackPerigeeAssociationTool,prefix='seedCalo_wideTrk_atTJVA_')
TauWideTrackPerigeeAssoc.defineBlock (2, 'WideTrkTJVA',
                                  TrackD3PDMaker.TrackPerigeeFillerTool,
                                  FillMomentum = True)
TauOtherTrackPerigeeAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauOtherTrackPerigeeAssociationTool,prefix='otherTrk_atTJVA_')
TauOtherTrackPerigeeAssoc.defineBlock (3, 'OtherTrkTJVA',
                                  TrackD3PDMaker.TrackPerigeeFillerTool,
                                  FillMomentum = True)


# Associate tau with its tracks
TauTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauTrackAssociationTool,prefix='track_')

TauSeedCaloTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauSeedCaloTrackAssociationTool,prefix='seedCalo_track_')

TauWideTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauWideTrackAssociationTool,prefix='seedCalo_wideTrk_')

TauOtherTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauOtherTrackAssociationTool,prefix='otherTrk_')



TrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool)
TrackParticlePerigeeAssoc.defineBlock (2, 'TrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

TrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauSeedCaloTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
             blockname = 'seedCaloTrkOOAssoc')
TrackParticlePerigeeAssoc.defineBlock (3, 'seedCaloTrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

WideTrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauWideTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
             blockname = 'WideTrkOOAssoc')
WideTrackParticlePerigeeAssoc.defineBlock (3, 'WideTrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

OtherTrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauOtherTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
             blockname = 'OtherTrkOOAssoc')
OtherTrackParticlePerigeeAssoc.defineBlock (3, 'OtherTrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)



TrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_',
             blockname = 'SeedCaloTrkPVAssoc')
TrackParticlePerigeePVAssoc.defineBlock (2, 'TrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

TrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauSeedCaloTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_')
TrackParticlePerigeePVAssoc.defineBlock (3, 'SeedCaloTrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

WideTrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauWideTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_',
             blockname = 'WideTrkPVAssoc')
WideTrackParticlePerigeePVAssoc.defineBlock (3, 'WideTrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

OtherTrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauOtherTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_',
             blockname = 'OtherTrkPVAssoc')
OtherTrackParticlePerigeePVAssoc.defineBlock (3, 'OtherTrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)


TrackSummaryAssoc = \
   SimpleAssociation (TauTrackAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool)
TrackSummaryAssoc.defineBlock (2, 'IDHits',
                               TrackD3PDMaker.TrackTrackSummaryFillerTool,
                               IDSharedHits = True,
                               IDHits = True,
                               IDOutliers = True,
                               MuonHits = False,
                               HitSum = True,
                               HoleSum = False,
                               HitPattern = False,
                               SiHits = False,
                               TRTRatio = True,
                               PixeldEdx = False,
                               SCTInfoPlus = True,
                               PixelInfoPlus = True)

TrackSummaryAssoc = \
   SimpleAssociation (TauSeedCaloTrackAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool,
                      blockname = 'SeedCaloTrackSummaryAssoc')
TrackSummaryAssoc.defineBlock (3, 'SeedCaloIDHits',
                               TrackD3PDMaker.TrackTrackSummaryFillerTool,
                               IDSharedHits = True,
                               IDHits = True,
                               IDOutliers = True,
                               MuonHits = False,
                               HitSum = True,
                               HoleSum = False,
                               HitPattern = False,
                               SiHits = False,
                               TRTRatio = False,
                               PixeldEdx = False,
                               SCTInfoPlus = True,
                               PixelInfoPlus = True)

WideTrackSummaryAssoc = \
   SimpleAssociation (TauWideTrackAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool,
                      blockname = 'WideIDHitsAssoc')
WideTrackSummaryAssoc.defineBlock (2, 'WideIDHits',
                               TrackD3PDMaker.TrackTrackSummaryFillerTool,
                               IDSharedHits = True,
                               IDHits = True,
                               IDOutliers = True,
                               MuonHits = False,
                               HitSum = True,
                               HoleSum = False,
                               HitPattern = False,
                               SiHits = False,
                               TRTRatio = False,
                               PixeldEdx = False,
                               SCTInfoPlus = True,
                               PixelInfoPlus = True)

OtherTrackSummaryAssoc = \
   SimpleAssociation (TauOtherTrackAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool,
                      blockname = 'OtherIDHitsAssoc')
OtherTrackSummaryAssoc.defineBlock (3, 'OtherIDHits',
                               TrackD3PDMaker.TrackTrackSummaryFillerTool,
                               IDSharedHits = True,
                               IDHits = True,
                               IDOutliers = True,
                               MuonHits = False,
                               HitSum = True,
                               HoleSum = False,
                               HitPattern = False,
                               SiHits = False,
                               TRTRatio = False,
                               PixeldEdx = False,
                               SCTInfoPlus = True,
                               PixelInfoPlus = True)



#-----------------------------------------------------------------------------
# Trigger matching
#-----------------------------------------------------------------------------
if D3PDMakerFlags.DoTrigger():
    from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits

# include all the tau-specific trigger terms at detail level 1 or above
# defineTriggerBits is clever enough to only include bits which are present in the
# run being processed.

    defineTriggerBits (TauD3PDObject, 1, 'L1_TAU.*')
    defineTriggerBits (TauD3PDObject, 1, 'L1_2TAU.*')
    defineTriggerBits (TauD3PDObject, 1, 'L2_tau.*')
    defineTriggerBits (TauD3PDObject, 1, 'EF_tau.*')
    defineTriggerBits (TauD3PDObject, 1, 'tau.*')
    defineTriggerBits (TauD3PDObject, 1, 'L2_2tau.*')
    defineTriggerBits (TauD3PDObject, 1, 'EF_2tau.*')
    defineTriggerBits (TauD3PDObject, 1, 'EF_j.*')
        
# now fill 3 variables: tau_EF_Matched, tau_L1_Matched, tau_L2_Matched
# which tell us whether or not the reco tau is matched to something
# at each trigger level. Also, use containment to dump basic 4-vec info
# for matched objects. That way, you can find the eta/phi (for example) of the
# EF object matched to a specific tau.

    TauEFAssoc = SimpleAssociation(TauD3PDObject,
            TauD3PDMaker.TauEFTriggerObjectAssociationTool,
            matched = 'matched',
            blockname = 'EFInfo',
            prefix = 'EF_',
            ChainPattern = 'EF_tau.*')

    TauEFAssoc.defineBlock (1, 'EFKinematics',
            EventCommonD3PDMaker.FourMomFillerTool,
            WriteE = True,
            WriteEt = True,
            WriteM = False)

    
    TauL2Assoc = SimpleAssociation(TauD3PDObject,
            TauD3PDMaker.TauL2TriggerObjectAssociationTool,
            matched = 'matched',
            blockname = 'L2Info',
            prefix = 'L2_')
   
    TauL2Assoc.defineBlock (1, 'L2Kinematics',
            EventCommonD3PDMaker.FourMomFillerTool,
            WriteE = True,
            WriteEt = True,
            WriteM = False)
   
   
    TauL1Assoc = SimpleAssociation(TauD3PDObject,
            TauD3PDMaker.TauL1TriggerObjectAssociationTool,
            matched = 'matched',
            blockname = 'L1Info',
            prefix = 'L1_')
   
    TauL1Assoc.defineBlock (1, 'L1Kinematics',
            EventCommonD3PDMaker.FourMomFillerTool,
            WriteEt = True,
            WriteM = False)

## EOF
