# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import TauD3PDMaker
import EventCommonD3PDMaker
import JetD3PDMaker
import TrackD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject


# Ensure that TauCommonDetailsMerger is run.
from TauTools.TauToolsConf import TauCommonDetailsMerger
merg = TauCommonDetailsMerger("TauCommonDetailsMerger")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if not merg in topSequence:
    # Make sure it goes before the d3pd algorithm.
    topSequence.insert (-1, merg)


#-----------------------------------------------------------------------------
# Tau D3PDObject Maker
#-----------------------------------------------------------------------------
TauD3PDObject = make_SGDataVector_D3PDObject( "Analysis::TauJetContainer",
                                              D3PDMakerFlags.TauSGKey(),
                                              "tau_",
                                              "QcdTauD3PDObject" )

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

TauD3PDObject.defineBlock (2, 'TauClustersDetails',
        TauD3PDMaker.TauClustersDetailsFillerTool)

TauD3PDObject.defineBlock (2, 'TauPi0Clusters',
        TauD3PDMaker.TauPi0ClusterFillerTool)

TauD3PDObject.defineBlock (2, 'TauSecVtx',
        TauD3PDMaker.TauSecVtxFillerTool)

TauD3PDObject.defineBlock (3, 'TauCellsDetails',
        TauD3PDMaker.TauCellsDetailsFillerTool)

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
             JetD3PDMaker.JetMomentFillerTool,Moments=['WIDTH', 'n90',
                                   'n90constituents','fracSamplingMax', 'SamplingMax',
	                           'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_JET_FORCELL',
	                           'ENG_BAD_CELLS', 'N_BAD_CELLS', 'N_BAD_CELLS_CORR',
                                   'BAD_CELLS_CORR_E'],
             prefix='jet_')

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
# Associate tau with its tracks
TauTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             TauD3PDMaker.TauTrackAssociationTool,prefix='track_')

TrackParticlePerigeeAssoc = SimpleAssociation \
	    (TauTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool)
TrackParticlePerigeeAssoc.defineBlock (2, 'TrkOO',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)

TrackParticlePerigeePVAssoc = SimpleAssociation \
	    (TauTrackAssoc,
	     TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
	     prefix = 'atPV_')
TrackParticlePerigeePVAssoc.defineBlock (2, 'TrkPV',
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
                               SiHits = True,
                               TRTRatio = False)


#-----------------------------------------------------------------------------
# Trigger matching
#-----------------------------------------------------------------------------
if D3PDMakerFlags.DoTrigger():

#comment out trigger bit
#    from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
#
## include all the tau-specific trigger terms at detail level 1 or above
## defineTriggerBits is clever enough to only include bits which are present in the
## run being processed.
#
#    defineTriggerBits (TauD3PDObject, 1, 'L1_TAU.*')
#    defineTriggerBits (TauD3PDObject, 1, 'L1_2TAU.*')
#    defineTriggerBits (TauD3PDObject, 1, 'L2_tau.*')
#    defineTriggerBits (TauD3PDObject, 1, 'EF_tau.*')
#    defineTriggerBits (TauD3PDObject, 1, 'tau.*')
        
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
