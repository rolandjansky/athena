# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


import MuonD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TriggerD3PDMaker
import TrackD3PDMaker
from TriggerD3PDMaker.defineTriggerBits   import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject         import DeferArg
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation  import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
#import TrigMuonD3PDMaker
import TruthD3PDMaker

from RecExConfig.RecFlags import rec
from RecExConfig.ObjKeyStore import cfgKeyStore

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Include import include

from MuonD3PDMaker.MuonD3PDMakerFlags    import MuonD3PDFlags


MuonD3PDObject = \
           make_SGDataVector_D3PDObject ('DataVector<xAOD::Muon_v1>',
                                         D3PDMakerFlags.MuonSGKey(),
                                         'mu_', 'MuonD3PDObject')

MuonD3PDObject.defineBlock (0, 'Kinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteRect = True,
                            WriteE = True,
                            WriteM = True)
MuonD3PDObject.defineBlock (
    0, 'AllAuthor',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['allauthor = allAuthors'])

MuonD3PDObject.defineBlock (
    1, 'Authors',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['author'])
MuonD3PDObject.defineBlock (1, 'NumberOfSegments',
                            MuonD3PDMaker.MuonNumberOfSegmentsFillerTool)

MuonD3PDObject.defineBlock (
    1, 'Isolation',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['etcone20 < float: 0',
            'etcone30 < float: 0',
            'etcone40 < float: 0',
            'ptcone20 < float: 0',
            'ptcone30 < float: 0',
            'ptcone40 < float: 0',
            'scatteringCurvatureSignificance < float: 0',
            'scatteringNeighbourSignificance < float: 0',
            'momentumBalanceSignificance < float: 0'])

MuonD3PDObject.defineBlock (
    1, 'CaloEnergyLoss',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['energyLossType'])
MuonD3PDObject.defineBlock (
    1, 'MuonType',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['muonType'])

MuonD3PDObject.defineBlock (2, 'MuonHitSummary',
                            MuonD3PDMaker.MuonTrkHitFillerTool)

MuonD3PDObject.defineBlock (
    2, 'MuonSpectrometerHitSummary',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['primarySector',
            'secondarySector',
            'innerSmallHits',
            'innerLargeHits',
            'middleSmallHits',
            'middleLargeHits',
            'outerSmallHits',
            'outerLargeHits',
            'extendedSmallHits',
            'extendedLargeHits',
            'innerSmallHoles',
            'innerLargeHoles',
            'middleSmallHoles',
            'middleLargeHoles',
            'outerSmallHoles',
            'outerLargeHoles',
            'extendedSmallHoles',
            'extendedLargeHoles',
            'phiLayer1Hits',
            'phiLayer2Hits',
            'phiLayer3Hits',
            'phiLayer4Hits',
            'etaLayer1Hits',
            'etaLayer2Hits',
            'etaLayer3Hits',
            'etaLayer4Hits',
            'phiLayer1Holes',
            'phiLayer2Holes',
            'phiLayer3Holes',
            'phiLayer4Holes',
            'etaLayer1Holes',
            'etaLayer2Holes',
            'etaLayer3Holes',
            'etaLayer4Holes',
            ])
            
MuonD3PDObject.defineBlock (
    4, 'MuonSpectrometerFieldIntegral',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['MSFieldIntegral = spectrometerFieldIntegral < float: 0'])


############################################################################
# TrackParticle variables
############################################################################


MuonTPAssoc = SimpleAssociation \
    (MuonD3PDObject,
     MuonD3PDMaker.MuonTrackParticleAssociationTool,
     matched = 'hastrack',
     blockname = 'TrkInfo')
MuonTPAssoc.defineBlock (
    0, 'Charge',
    TrackD3PDMaker.TrackParticleChargeFillerTool)
TrackParticlePerigeeAssoc = SimpleAssociation \
    (MuonTPAssoc,
     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
     prefix = 'track',
     blockname = 'TrackParticlePerigeeAssoc')
TrackParticlePerigeeAssoc.defineBlock (1, 'TrkParameters',
                                       TrackD3PDMaker.PerigeeFillerTool)

TrackParticleCovarAssoc = SimpleAssociation (TrackParticlePerigeeAssoc,
                                             TrackD3PDMaker.PerigeeCovarianceAssociationTool,
                                             blockname = 'TrackParticleCovarAssoc')
TrackParticleCovarAssoc.defineBlock (1, 'TrkCovDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     DiagCovariance = True)
TrackParticleCovarAssoc.defineBlock (2, 'TrkCovOffDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     OffDiagCovariance = True)

MuonTPAssoc.defineBlock (
    2, 'TrkFitQuality',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['chi2 = chiSquared',
            'ndof = numberDoF'],
    prefix = 'trackfit')


MuonIDTPAssoc = SimpleAssociation \
                (MuonD3PDObject,
                 MuonD3PDMaker.MuonTrackParticleAssociationTool,
                 blockname = 'TrkInfoInDet',
                 Type = 'InDet')

if not MuonD3PDFlags.doSingleMuons():
    #
    # Impact parameter variables.  Use the ID track for this.
    # These are likely not very useful for standalone muons anyway.
    #
    from TrackD3PDMaker.TrackParticleImpactParameters \
         import TrackParticleImpactParameters
    TrackParticleImpactParameters (MuonIDTPAssoc)


MuonPVPerigeeAssoc = SimpleAssociation (
    MuonTPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
    suffix = '_exPV',
    blockname = 'MuonPVPerigeeAssoc')
MuonPVPerigeeAssoc.defineBlock (
    1, 'ParametersAtPV',
    TrackD3PDMaker.PerigeeFillerTool)
MuonPVCovarAssoc = SimpleAssociation (
    MuonPVPerigeeAssoc,
    TrackD3PDMaker.PerigeeCovarianceAssociationTool,
    blockname = 'MuonPVCovarAssoc')
MuonPVCovarAssoc.defineBlock (
    1, 'PrimaryTrackDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = True)
MuonPVCovarAssoc.defineBlock (
    1, 'PrimaryTrackOffDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = False,
    OffDiagCovariance = True)

MuonCBTPAssoc = SimpleAssociation \
    (MuonD3PDObject,
     MuonD3PDMaker.MuonTrackParticleAssociationTool,
     Type = 'Combined',
     prefix = 'cb_',
     blockname = 'CBTrkInfo')
MuonCBPVPerigeeAssoc = SimpleAssociation (
    MuonCBTPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
    suffix = '_exPV',
    blockname = 'MuonCBPVPerigeeAssoc')
MuonCBPVPerigeeAssoc.defineBlock (
    1, 'CBParametersAtPV',
    TrackD3PDMaker.PerigeeFillerTool)

MuonIDTPAssoc = SimpleAssociation \
    (MuonD3PDObject,
     MuonD3PDMaker.MuonTrackParticleAssociationTool,
     Type = 'InDet',
     prefix = 'id_',
     blockname = 'IDTrkInfo')
MuonIDPerigeeAssoc = SimpleAssociation (
    MuonIDTPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
    blockname = 'MuonIDPerigeeAssoc')
MuonIDPerigeeAssoc.defineBlock (
    1, 'IDParameters',
    TrackD3PDMaker.PerigeeFillerTool)
MuonIDPVPerigeeAssoc = SimpleAssociation (
    MuonIDTPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
    suffix = '_exPV',
    blockname = 'MuonIDPVPerigeeAssoc')
MuonIDPVPerigeeAssoc.defineBlock (
    1, 'IDParametersAtPV',
    TrackD3PDMaker.PerigeeFillerTool)

MuonIDPVCovarAssoc = SimpleAssociation (
    MuonIDPVPerigeeAssoc,
    TrackD3PDMaker.PerigeeCovarianceAssociationTool,
    blockname = 'MuonIDPVCovarAssoc')
MuonIDPVCovarAssoc.defineBlock (
    2, 'IDTrackDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = True)
MuonIDPVCovarAssoc.defineBlock (
    3, 'IDTrackOffDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = False,
    OffDiagCovariance = True)


MuonMETPAssoc = SimpleAssociation \
    (MuonD3PDObject,
     MuonD3PDMaker.MuonTrackParticleAssociationTool,
     Type = 'MuonSpectrometer',
     prefix = 'me_',
     blockname = 'METrkInfo')
MuonMEPerigeeAssoc = SimpleAssociation (
    MuonMETPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool)
MuonMEPerigeeAssoc.defineBlock (
    1, 'MEParametersAt',
    TrackD3PDMaker.PerigeeFillerTool)
MuonMEPVPerigeeAssoc = SimpleAssociation (
    MuonMETPAssoc,
    TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
    suffix = '_exPV',
    blockname = 'MuonMEPVPerigeeAssoc')
MuonMEPVPerigeeAssoc.defineBlock (
    1, 'MEParametersAtPV',
    TrackD3PDMaker.PerigeeFillerTool)

MuonMEPVCovarAssoc = SimpleAssociation (
    MuonMEPVPerigeeAssoc,
    TrackD3PDMaker.PerigeeCovarianceAssociationTool,
    blockname = 'MuonMEPVCovarAssoc')
MuonMEPVCovarAssoc.defineBlock (
    2, 'METrackDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = True)
MuonMEPVCovarAssoc.defineBlock (
    3, 'METrackOffDiagonalCovarianceAtPV',
    TrackD3PDMaker.CovarianceFillerTool,
    IsTrackPerigee = True,
    Error = False,
    OffDiagCovariance = True)


############################################################################
# Truth matching
############################################################################
if rec.doTruth():
    from TruthD3PDMaker.MCTruthClassifierConfig \
         import D3PDMCTruthClassifier

    MuonD3PDObject.defineBlock (1, 'TruthClassification',
                                MuonD3PDMaker.MuonTruthClassificationFillerTool,
                                Classifier = D3PDMCTruthClassifier)

    MuonTruthPartAssoc = SimpleAssociation \
        (MuonD3PDObject,
         MuonD3PDMaker.MuonGenParticleAssociationTool,
         Classifier = D3PDMCTruthClassifier,
         blockname = 'TruthAssoc',
         DRVar = 'dr')
    MuonTruthPartAssoc.defineBlock (0, 'TruthKin',
                                    EventCommonD3PDMaker.FourMomFillerTool,
                                    WriteE = True,
                                    WriteM = False,
                                    prefix = 'truth_')
    MuonTruthPartAssoc.defineBlock (0, 'Truth',
                                    TruthD3PDMaker.TruthParticleFillerTool,
                                    PDGIDVariable = 'type',
                                    prefix = 'truth_')
    MuonTruthPartMotherAssoc = SimpleAssociation \
      (MuonTruthPartAssoc,
       D3PDMakerCoreComps.FirstAssociationTool,
       Associator = TruthD3PDMaker.TruthParticleParentAssociationTool
         ('MuonTruthPartMotherAssoc2'),
       blockname = 'MuonTruthPartMotherAssoc',
       prefix = 'truth_mother')
    MuonTruthPartMotherAssoc.defineBlock (0, 'MotherTruth',
                                          TruthD3PDMaker.TruthParticleFillerTool,
                                          PDGIDVariable = 'type')
    MuonTruthPartAssoc.defineBlock (0, 'TruthAssocIndex',
                                    D3PDMakerCoreComps.IndexFillerTool,
                                    prefix = 'mc_',
                                    Target = 'mc_')


# ############################################################################
# # Trigger matching
# ############################################################################

# ############################# Event Filter #################################
# if D3PDMakerFlags.DoTrigger():

#     defineTriggerBits (MuonD3PDObject, 0,
#                        ['L1_MU0',
#                         'EF_L1ItemStreamer_L1_MU0',
#                         'EF_mu4',
#                         'L2_mu4',
#                         'L2_mu4_MSonly',
#                         'EF_mu4_MSonly',                        
#                         'EF_mu6',
#                         'L2_mu6',
#                         'L1_MU6',
#                         'L2_mu6_MSonly',
#                         'EF_mu6_MSonly',                        
#                         'EF_mu10',
#                         'L2_mu10',
#                         'L1_MU10',
#                         'EF_mu10i_loose',
#                         'L2_mu10i_loose',                        
#                         'EF_mu13',
#                         'L2_mu13',
#                         'EF_mu20',
#                         'EF_mu20_MSonly',
#                         'L2_mu20',
#                         'L2_mu20_MSonly',
#                         'L1_MU20',
#                         'EF_2mu4',
#                         'L2_2mu4',
#                         'L1_2MU4',
#                         'EF_mu6',
#                         'EF_mu4',
#                         'EF_2mu10',
#     ])
#     defineTriggerBits (MuonD3PDObject, 1,
#                        D3PDMakerFlags.MuonL1TrigPattern())
#     defineTriggerBits (MuonD3PDObject, 1,
#                        D3PDMakerFlags.MuonL2TrigPattern())
#     defineTriggerBits (MuonD3PDObject, 1,
#                        D3PDMakerFlags.MuonEFTrigPattern())

#     ###################################################################################
#     # Event filter. Associations to combined muons (EF_muXX / muid type trigger muons)
#     ###################################################################################
#     efcbpattern = 'EF_mu[0-9]*$|EF_mu[0-9]*_tight$'
#     efmepattern = 'EF_mu[0-9]*_MSonly$|EF_mu[0-9]*_MSonly_tight$'
#     efmgpattern = 'EF_mu[0-9]*_MG$|EF_mu[0-9]*_MG_tight$'

#     MuonEFCBIndexAssoc = IndexAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          target = 'trig_EF_trigmuonef_',
#          prefix = 'EFCB_',
#          level = 11,
#          blockname = 'EFCBInfoIndex',
#          EFTrigMatchType = "CB",
# #         matched = 'matched',
#          WriteDR = False,
#          ChainPattern = efcbpattern)


#     MuonEFCBAssoc = SimpleAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          EFTrigMatchType = "CB",
#          matched = 'matched',
#          blockname = 'EFCBInfo',
#          level = 1,
#          prefix = 'EFCB_',
#          ChainPattern = efcbpattern)

#     CBInfoTrackAssoc = \
#         ContainedVectorMultiAssociation( MuonEFCBAssoc,
#                                          TrigMuonD3PDMaker.TrigMuonEFInfoTrackAssociationTool,
#                                          prefix = "", blockname = "EFCBInfoTrackAssoc" )
#     CBInfoTrackAssoc.defineBlock( 0, "CBTrackDetails",
#                                 TrigMuonD3PDMaker.TrigMuonEFInfoTrackFillerTool )   
#     CombinedTrackAssoc = \
#         SimpleAssociation( CBInfoTrackAssoc,
#                            TrigMuonD3PDMaker.TrigMuonEFInfoTrackCombinedTrackAssociationTool,
#                            matched = "hasCB", blockname = "CombinedTrackAssoc", prefix = "" )
#     CombinedTrackAssoc.defineBlock( 1, "CombinedTrackBasics",
#                                     EventCommonD3PDMaker.FourMomFillerTool,
#                                     WriteM = False )
#     CombinedTrackAssoc.defineBlock( 12, "CombinedTrackFitResults1",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveFitResults = True )
#     CombinedTrackAssoc.defineBlock( 13, "CombinedTrackFitResults2",
#                                     TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                     SaveFitResults = True )
#     CombinedTrackAssoc.defineBlock( 13, "CombinedTrackFitDetails1",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveHitInfo = True )
#     CombinedTrackAssoc.defineBlock( 13, "CombinedTrackFitDetails2",
#                                     TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                     SaveHitInfo = True )
 

#     ###################################################################################
#     # Event filter. Associations to MuGirl muons (EF_muXX_MG type trigger muons)
#     ###################################################################################
#     MuonEFMGIndexAssoc = IndexAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          target = 'trig_EF_trigmugirl_',
#          prefix = 'EFMG_',
#          level = 11,
#          blockname = 'EFMGInfoIndex',
#          EFTrigMatchType = "MG",
# #         matched = 'matched',
#          WriteDR = False,
#          ChainPattern = efmgpattern) # D3PDMakerFlags.MuonEFTrigPattern())

#     MuonEFMGAssoc = SimpleAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          EFTrigMatchType = "MG",
#          matched = 'matched',
#          blockname = 'EFMGInfo',
#          level = 1,
#          prefix = 'EFMG_',
#          ChainPattern = efmgpattern) #D3PDMakerFlags.MuonEFTrigPattern())

#     MGInfoTrackAssoc = \
#         ContainedVectorMultiAssociation( MuonEFMGAssoc,
#                                          TrigMuonD3PDMaker.TrigMuonEFInfoTrackAssociationTool,
#                                          prefix = "", blockname = "EFMGInfoTrackAssoc" )
#     MGInfoTrackAssoc.defineBlock( 0, "MGTrackDetails",
#                                 TrigMuonD3PDMaker.TrigMuonEFInfoTrackFillerTool )   
#     # Associate combined TrigMuonEFCbTrack objects to the TrigMuonEFInfoTrack objects:
#     MuGirlTrackAssoc = \
#         SimpleAssociation( MGInfoTrackAssoc,
#                            TrigMuonD3PDMaker.TrigMuonEFInfoTrackCombinedTrackAssociationTool,
#                            matched = "hasMG", blockname = "MuGirlTrackAssoc", prefix = "" )
#     MuGirlTrackAssoc.defineBlock( 1, "MuGirlTrackBasics",
#                                     EventCommonD3PDMaker.FourMomFillerTool,
#                                     WriteM = False )
#     MuGirlTrackAssoc.defineBlock( 12, "MuGirlTrackFitResults1",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveFitResults = True )
#     MuGirlTrackAssoc.defineBlock( 13, "MuGirlTrackFitResults2",
#                                     TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                     SaveFitResults = True )
#     MuGirlTrackAssoc.defineBlock( 13, "MuGirlTrackFitDetails1",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveHitInfo = True )
#     MuGirlTrackAssoc.defineBlock( 13, "MuGirlTrackFitDetails2",
#                                     TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                     SaveHitInfo = True )

#     ###################################################################################
#     # Event filter. Associations to MSonly muons (EF_muXX_MSonly type trigger muons)
#     ###################################################################################
#     MuonEFMEIndexAssoc = IndexAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          target = 'trig_EF_trigmuonef_',
#          prefix = 'EFME_',
#          level = 11,
#          blockname = 'EFMEInfoIndex',
#          EFTrigMatchType = "ME",
# #         matched = 'matched',
#          WriteDR = False,
#          ChainPattern = efmepattern) #D3PDMakerFlags.MuonEFTrigPattern())

#     MuonEFMEAssoc = SimpleAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
#          EFTrigMatchType = "ME",
#          matched = 'matched',
#          blockname = 'EFMEInfo',
#          level = 1,
#          prefix = 'EFME_',
#          ChainPattern = efmepattern) #D3PDMakerFlags.MuonEFTrigPattern())

#     MEInfoTrackAssoc = \
#         ContainedVectorMultiAssociation( MuonEFMEAssoc,
#                                          TrigMuonD3PDMaker.TrigMuonEFInfoTrackAssociationTool,
#                                          prefix = "", blockname = "EFMEInfoTrackAssoc" )
#     MEInfoTrackAssoc.defineBlock( 0, "METrackDetails",
#                                 TrigMuonD3PDMaker.TrigMuonEFInfoTrackFillerTool )   
#     # Associate spectrometer-extrapolated TrigMuonEFTrack objects to the TrigMuonEFInfoTrack objects:
#     ExtrapolatedTrackAssoc = \
#         SimpleAssociation( MEInfoTrackAssoc,
#                            TrigMuonD3PDMaker.TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool,
#                            matched = "hasME", blockname = "ExtrapolatedTrackAssoc", prefix = "" )
#     ExtrapolatedTrackAssoc.defineBlock( 1, "ExtrapolatedTrackBasics",
#                                         EventCommonD3PDMaker.FourMomFillerTool,
#                                         WriteM = False )
#     ExtrapolatedTrackAssoc.defineBlock( 12, "ExtrapolatedTrackFitResults",
#                                         TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                         SaveFitResults = True)
#     ExtrapolatedTrackAssoc.defineBlock( 13, "ExtrapolatedTrackDetails",
#                                         TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                         SaveHitInfo = True )


# ############################# Level Two ######################################
#     MuonL2IndexAssoc = IndexAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonL2TriggerObjectAssociationTool,
#          target = 'trig_L2_combmuonfeature_',
#          prefix = 'L2CB_',
#          level = 11,
#          blockname = 'L2CBInfoIndex',
#          WriteDR = False,
#          ChainPattern = D3PDMakerFlags.MuonL2TrigPattern())
    
#     MuonL2Assoc = SimpleAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonL2TriggerObjectAssociationTool,
#          level = 1,
#          matched = 'matched',
#          blockname = 'L2CBInfo',
#          prefix = 'L2CB_',
#          ChainPattern = D3PDMakerFlags.MuonL2TrigPattern())
#     MuonL2Assoc.defineBlock (1, 'L2CBKinematics',
#                              EventCommonD3PDMaker.FourMomFillerTool,
#                              WriteM = False)
#     MuonL2Assoc.defineBlock (1, 'L2CBDetail1',
#                              MuonD3PDMaker.L2MuonCB1FillerTool)
#     MuonL2Assoc.defineBlock (2, 'L2CBDetail2',
#                              MuonD3PDMaker.L2MuonCB2FillerTool)

# ############################# Level One ######################################
#     MuonL1IndexAssoc = IndexAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonL1TriggerObjectAssociationTool,
#          target = 'trig_L1_mu_',
#          prefix = 'L1_',
#          level = 11,
#          MaxDR = 0.5,
#          blockname = 'L1InfoIndex',
#          WriteDR = False,
#          ChainPattern = D3PDMakerFlags.MuonL1TrigPattern())

#     MuonL1Assoc = SimpleAssociation \
#         (MuonD3PDObject,
#          MuonD3PDMaker.MuonL1TriggerObjectAssociationTool,
#          level = 1,
#          matched = 'matched',
#          blockname = 'L1Info',
#          MaxDR = 0.5,
#          prefix = 'L1_',
#          ChainPattern = D3PDMakerFlags.MuonL1TrigPattern())
#     MuonL1Assoc.defineBlock (1, 'L1Kinematics',
#                              EventCommonD3PDMaker.FourMomFillerTool,
#                              WriteM = False)
#     MuonL1Assoc.defineBlock (2, 'L1Detail2',
#                              TriggerD3PDMaker.Muon_ROIFillerTool,
#                              WriteDecodedInfo = True)


# MuonIDOrPTPAssoc = SimpleAssociation \
#                    (MuonD3PDObject,
#                     MuonD3PDMaker.MuonTrackParticleAssociationTool,
#                     blockname = 'TrkInfoInDetOrPrimary',
#                     Type = 'InDetOrPrimary')

