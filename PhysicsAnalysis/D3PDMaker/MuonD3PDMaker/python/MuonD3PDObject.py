# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


import MuonD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TrackD3PDMaker
from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags
import TruthD3PDMaker

from RecExConfig.RecFlags import rec
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


