# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TauD3PDMaker
import TrigTauD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TrackD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

#-----------------------------------------------------------------------------
# Trig Tau EF D3PDObject Maker
#-----------------------------------------------------------------------------
TrigEFTauD3PDObject = make_SGDataVector_D3PDObject( "Analysis::TauJetContainer",
                                                    "HLT_TrigTauRecMerged",
                                                    "trig_EF_tau_",
                                                    "TrigEFTauD3PDObject" )

#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------
TrigEFTauD3PDObject.defineBlock (0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteEt = True,
                                 WritePt = True,
                                 WriteRect = True)


TrigEFTauD3PDObject.defineBlock (0, 'TauJet',
                                 TauD3PDMaker.TauJetFillerTool)
TrigEFTauD3PDObject.defineBlock (1, 'TauCommonDetailsInfo',
                                 TrigTauD3PDMaker.TrigEFTauCommonDetailsFiller)
#TrigEFTauD3PDObject.defineBlock (1, 'TauCalcVars',
#                                 TauD3PDMaker.TauCalcVarsFillerTool)
TrigEFTauD3PDObject.defineBlock (1, 'Decision',
                                 TrigTauD3PDMaker.TrigEFTauObjectDecisionFillerTool,
                                 AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                 Chains = [ "EF_tau.*","EF_2tau.*" ])


# Associate tau with its tracks
TauTrackAssoc = ContainedVectorMultiAssociation \
            (TrigEFTauD3PDObject,
             TauD3PDMaker.TauTrackAssociationTool,prefix='track_')

TauSeedCaloTrackAssoc = ContainedVectorMultiAssociation \
            (TrigEFTauD3PDObject,
             TauD3PDMaker.TauSeedCaloTrackAssociationTool,prefix='seedCalo_track_')

TauWideTrackAssoc = ContainedVectorMultiAssociation \
            (TrigEFTauD3PDObject,
             TauD3PDMaker.TauWideTrackAssociationTool,prefix='seedCalo_wideTrk_')

TauOtherTrackAssoc = ContainedVectorMultiAssociation \
            (TrigEFTauD3PDObject,
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
WideTrackParticlePerigeeAssoc.defineBlock (2, 'WideTrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

OtherTrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauOtherTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
             blockname = 'OtherTrkOOAssoc')
OtherTrackParticlePerigeeAssoc.defineBlock (2, 'OtherTrkOO',
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
WideTrackParticlePerigeePVAssoc.defineBlock (2, 'WideTrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

OtherTrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauOtherTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_',
             blockname = 'OtherTrkPVAssoc')
OtherTrackParticlePerigeePVAssoc.defineBlock (2, 'OtherTrkPV',
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
OtherTrackSummaryAssoc.defineBlock (2, 'OtherIDHits',
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




# EOF
