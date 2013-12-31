# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import MuonD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TriggerD3PDMaker
import TrackD3PDMaker
from TriggerD3PDMaker.defineTriggerBits   import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags

from RecExConfig.RecFlags import rec

#from MuonD3PDMaker.MuonD3PDMakerConf import MuonWZTruthTool
#TruthTool = MuonWZTruthTool()
#ToolSvc += TruthTool

MuonD3PDObject = \
           make_SGDataVector_D3PDObject ('Analysis::MuonContainer',
                                         D3PDMakerFlags.MuonSGKey(),
                                         'mu_', 'QcdMuonD3PDObject')

MuonD3PDObject.defineBlock (0, 'Kinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteRect = True,
                            WriteE = True,
                            WriteM = True)
MuonD3PDObject.defineBlock (0, 'Charge',
                            EventCommonD3PDMaker.ChargeFillerTool)
MuonD3PDObject.defineBlock (0, 'AllAuthor',
                            MuonD3PDMaker.MuonAllAuthorFillerTool)

MuonD3PDObject.defineBlock (1, 'Authors',
                            MuonD3PDMaker.MuonAuthorFillerTool)

MuonD3PDObject.defineBlock (1, 'TrkMatchQuality',
                            MuonD3PDMaker.MuonMatchQualityFillerTool)
MuonD3PDObject.defineBlock (1, 'Isolation', 
                            MuonD3PDMaker.MuonParameterFillerTool, 
                            Parameters = ['1', 'etcone20',
                                          '2', 'etcone30',
                                          '3', 'etcone40',
                                          '9', 'nucone20',
                                          '10','nucone30',
                                          '11','nucone40',
                                          '17','ptcone20',
                                          '18','ptcone30',
                                          '19','ptcone40'])
#MuonD3PDObject.defineBlock (1, 'IsolationPtCut',
#                            MuonD3PDMaker.MuonIDIsolTool)

MuonD3PDObject.defineBlock (1, 'CaloEnergyLoss',
                            MuonD3PDMaker.MuonELossFillerTool)
MuonD3PDObject.defineBlock (1, 'AcceptMethods',
                            MuonD3PDMaker.MuonAcceptMethodFillerTool)
MuonD3PDObject.defineBlock (1, 'Quality',
                            MuonD3PDMaker.MuonQualityFillerTool)

MuonD3PDObject.defineBlock (1, 'ParametersAtPV',
                            MuonD3PDMaker.MuonTrackAtPVFiller)

# In principle move off-diag to level 2
#...but PV more important than origin
MuonD3PDObject.defineBlock (1, 'CovarianceAtPV',
                            MuonD3PDMaker.MuonTrackCovarianceAtPV)

MuonD3PDObject.defineBlock (1, 'ComponentTrkParameters',
                            MuonD3PDMaker.MuonComponentTrkParameters)

MuonD3PDObject.defineBlock (2, 'MuonHitSummary',
                            MuonD3PDMaker.MuonTrkHitFillerTool)

MuonD3PDObject.defineBlock (2, 'MuonSpectrometerHitSummary',
                            MuonD3PDMaker.MuonSpectrometerTrkSummary)


MuonTPAssoc = SimpleAssociation \
    (MuonD3PDObject,
     MuonD3PDMaker.MuonTrackParticleAssociationTool,
     matched = 'hastrack',
     blockname = 'TrkInfo')
TrackParticlePerigeeAssoc = SimpleAssociation \
    (MuonTPAssoc,
     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
     prefix = 'track')
TrackParticlePerigeeAssoc.defineBlock (1, 'TrkParameters',
                                       TrackD3PDMaker.TrackPerigeeFillerTool)

TrackParticleCovarAssoc = SimpleAssociation (TrackParticlePerigeeAssoc,
                                             TrackD3PDMaker.PerigeeCovarianceAssociationTool)
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

MuonFitQualityAssoc = \
   SimpleAssociation (MuonTPAssoc,
                      TrackD3PDMaker.TrackParticleFitQualityAssociationTool)
MuonFitQualityAssoc.defineBlock (2, 'TrkFitQuality',
                                 TrackD3PDMaker.TrackFitQualityFillerTool,
                                 prefix = 'trackfit')


#commented out to avoid segmentation error in MCTruthClassifier
#############################################################################
## Truth matching
#############################################################################
#if rec.doTruth():
#    import TrutrhD3PDMaker.MCTruthClassifierConfig
#    MuonTruthPartAssoc = SimpleAssociation \
#        (MuonD3PDObject,
#         MuonD3PDMaker.MuonGenParticleAssociationTool,
#         prefix = 'truth_',
#         matched = 'matched',
#         blockname = 'Truth',
#         DRVar = 'dr')
#    MuonTruthPartAssoc.defineBlock (0, 'Truth',
#                                      EventCommonD3PDMaker.GenParticleFillerTool,
#                                      WriteE = True,
#                                      WriteM = False)

############################################################################
# Trigger matching
############################################################################

############################# Event Filter #################################
if D3PDMakerFlags.DoTrigger():

#comment out trigger bit
#    defineTriggerBits (MuonD3PDObject, 0,
#                       ['L1_MU0',
#                        'EF_mu4',
#                        'L2_mu4',
#                        'EF_mu6',
#                        'L2_mu6',
#                        'L1_MU6',
#                        'EF_mu10',
#                        'L2_mu10',
#                        'L1_MU10',
#                        'EF_mu20',
#                        'EF_mu20_MSonly',
#                        'L2_mu20',
#                        'L2_mu20_MSonly',
#                        'L1_MU20',
#                        'EF_2mu4',
#                        'L2_2mu4',
#                        'L1_2MU4',
#                        'EF_mu6',
#                        'EF_mu4',
#                        'EF_2mu10',
#    ])
#    defineTriggerBits (MuonD3PDObject, 1,
#                       D3PDMakerFlags.MuonL1TrigPattern())
#    defineTriggerBits (MuonD3PDObject, 1,
#                       D3PDMakerFlags.MuonL2TrigPattern())
#    defineTriggerBits (MuonD3PDObject, 1,
#                       D3PDMakerFlags.MuonEFTrigPattern())

    MuonEFAssoc = SimpleAssociation \
        (MuonD3PDObject,
         MuonD3PDMaker.MuonEFInfoTriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'EFInfo',
         level = 1,
         prefix = 'EF_',
         ChainPattern = D3PDMakerFlags.MuonEFTrigPattern())
    MuonEFAssoc.defineBlock (1, 'EFKinematics', 
                             MuonD3PDMaker.EFInfoMuonKinematicsFiller)
#    MuonEFAssoc = SimpleAssociation \
#        (MuonD3PDObject,
#         MuonD3PDMaker.MuonEFTriggerObjectAssociationTool,
#         matched = 'matched',
#         blockname = 'EFInfo',
#         level = 1,
#         prefix = 'EF_',
#         ChainPattern = 'EF_mu.*')
#    MuonEFAssoc.defineBlock (1, 'EFKinematics', 
#                             EventCommonD3PDMaker.FourMomFillerTool,
#                             WriteM = False)
#    MuonEFAssoc.defineBlock (1, 'EFDetail',
#                             MuonD3PDMaker.EFMuonFillerTool)
#

############################# Level Two ######################################
    MuonL2Assoc = SimpleAssociation \
        (MuonD3PDObject,
         MuonD3PDMaker.MuonL2TriggerObjectAssociationTool,
         level = 1,
         matched = 'matched',
         blockname = 'L2CBInfo',
         prefix = 'L2CB_',
         ChainPattern = D3PDMakerFlags.MuonL2TrigPattern())
#    MuonL2Assoc.defineBlock (1, 'L2Kinematics',
#                                 EventCommonD3PDMaker.FourMomFillerTool,
#                                 WriteE = True,
#                                 WriteEt = True,
#                                 WriteM = False)
    MuonL2Assoc.defineBlock (1, 'L2CBKinematics',
                             EventCommonD3PDMaker.FourMomFillerTool,
                             WriteM = False)
    MuonL2Assoc.defineBlock (1, 'L2CBDetail1',
                             MuonD3PDMaker.L2MuonCB1FillerTool)
    MuonL2Assoc.defineBlock (2, 'L2CBDetail2',
                             MuonD3PDMaker.L2MuonCB2FillerTool)
#    MuonL2Assoc.defineBlock (1, 'L2kinem',
#                                 MuonD3PDMaker.CombinedMuonFeatureFillerTool)

############################# Level One ######################################
    MuonL1Assoc = SimpleAssociation \
        (MuonD3PDObject,
         MuonD3PDMaker.MuonL1TriggerObjectAssociationTool,
         level = 1,
         matched = 'matched',
         blockname = 'L1Info',
         prefix = 'L1_',
         ChainPattern = D3PDMakerFlags.MuonL1TrigPattern())
    MuonL1Assoc.defineBlock (1, 'L1Kinematics',
                             EventCommonD3PDMaker.FourMomFillerTool,
                             WriteM = False)
    MuonL1Assoc.defineBlock (2, 'L1Detail2',
                             TriggerD3PDMaker.Muon_ROIFillerTool,
                             WriteDecodedInfo = True)
