# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from egammaD3PDMaker.isem_version import isem_version
import egammaD3PDMaker
import EventCommonD3PDMaker
import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaParameters
from ROOT import egammaPID
from RecExConfig.RecFlags import rec
from egammaD3PDMaker.ElectronD3PDObject import ElectronD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
import TrackD3PDMaker


EFElectronD3PDObject = \
           make_SGDataVector_D3PDObject ('egammaContainer',
                                         'HLT_egamma_Electrons,HLT_egamma', 
                                         'trig_EF_el_', 'EFElectronD3PDObject')

EFElectronD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
EFElectronD3PDObject.defineBlock (0, 'Charge',
                                EventCommonD3PDMaker.ChargeFillerTool)
EFElectronD3PDObject.defineBlock (0, 'Author',
                                egammaD3PDMaker.egammaAuthorFillerTool)
EFElectronD3PDObject.defineBlock (0, 'IsEM',
                                egammaD3PDMaker.egammaIsEMoneFillerTool
                                )
## EFElectronD3PDObject.defineBlock (0, 'Conversion0',
##                                 egammaD3PDMaker.egammaConversion0FillerTool,
##                                 )

## if rec.doTruth():
##     import TruthD3PDMaker.MCTruthClassifierConfig
##     EFElectronD3PDObject.defineBlock (1, 'TruthClassification',
##                                     egammaD3PDMaker.egammaTruthClassificationFillerTool)
##     ElectronGenPartAssoc = SimpleAssociation \
##         (EFElectronD3PDObject,
##          egammaD3PDMaker.egammaGenParticleAssociationTool,
##          prefix = 'truth_',
##          matched = 'matched',
##          blockname = 'Truth')
##     ElectronGenPartAssoc.defineBlock (0, 'Truth',
##                                       EventCommonD3PDMaker.GenParticleFillerTool,
##                                       WriteE = True,
##                                       WriteM = False)
##     ElectronGenPartAssoc.defineBlock (0, 'TruthBrem',
##                                       EventCommonD3PDMaker.GenParticleBremFillerTool)



if isem_version() == 1:
    EFElectronD3PDObject.defineBlock (0, 'IsEMCuts',
                                    egammaD3PDMaker.egammaIsEMFillerTool,
                                    IsEM  = [(egammaPID.ElectronLooseOLDRel,
                                              None),
                                             'loose',
                                             (egammaPID.ElectronMediumOLDRel,
                                              None),
                                             'medium',
                                             (egammaPID.ElectronMediumNoIsoOLDRel,
                                              None),
                                             'mediumNoIso',
                                             (egammaPID.ElectronTightOLDRel,
                                              None),
                                             'tight',
                                             (egammaPID.ElectronTightTRTNoIsoOLDRel,
                                              None),
                                             'tightTRTNoIso',
                                             (egammaPID.ElectronTightNoIsolationOLDRel,
                                              None),
                                             'tightNoIso',
                                    ])
else:
    EFElectronD3PDObject.defineBlock (0, 'IsEMCuts',
                                    egammaD3PDMaker.egammaIsEMFillerTool,
                                    IsEM  =
       [(egammaPID.ElectronLoose,
         egammaPID.frwdElectronLoose),                    'loose',
        (egammaPID.ElectronLooseIso,  None),              'looseIso',
        (egammaPID.ElectronMedium,    None),              'medium',
        (egammaPID.ElectronMediumIso, None),              'mediumIso',
        (egammaPID.ElectronMedium_WithoutTrackMatch, None),'mediumWithoutTrack',
        (egammaPID.ElectronMediumIso_WithoutTrackMatch, None),'mediumIsoWithoutTrack',
        (egammaPID.ElectronTight,
         egammaPID.frwdElectronTight),                    'tight',
        (egammaPID.ElectronTightIso,              None),  'tightIso',
        (egammaPID.ElectronTight_WithoutTrackMatch,None), 'tightWithoutTrack',
        (egammaPID.ElectronTightIso_WithoutTrackMatch,None), 'tightIsoWithoutTrack',

        (egammaPID.ElectronLoosePP,     None),            'loosePP',
        (egammaPID.ElectronLoosePPIso,  None),            'loosePPIso',
        (egammaPID.ElectronMediumPP,    None),            'mediumPP',
        (egammaPID.ElectronMediumPPIso, None),            'mediumPPIso',
        (egammaPID.ElectronTightPP,     None),            'tightPP',
        (egammaPID.ElectronTightPPIso,  None),            'tightPPIso',
        ])


EFElectronD3PDObject.defineBlock (1, 'HadLeakage',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.ethad,
                                           'Ethad',
                                           egammaParameters.ethad1,
                                           'Ethad1',
                                           ])
EFElectronD3PDObject.defineBlock (1, 'Layer1Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f1,
                                           'f1',
                                           egammaParameters.f1core,
                                           'f1core',
                                           egammaParameters.emins1,
                                           'Emins1',
                                           egammaParameters.fracs1,
                                           'fside',
                                           egammaParameters.e2tsts1,
                                           'Emax2',
                                           egammaParameters.weta1,
                                           'ws3',
                                           egammaParameters.wtots1,
                                           'wstot',
                                           egammaParameters.emaxs1,
                                           'emaxs1',
                                           ])
EFElectronD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                                egammaD3PDMaker.egammaLayer1ExtraFillerTool)
EFElectronD3PDObject.defineBlock (1, 'Layer2Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.e233,
                                           'E233',
                                           egammaParameters.e237,
                                           'E237',
                                           egammaParameters.e277,
                                           'E277',
                                            egammaParameters.weta2,
                                           'weta2',
                                           ])
EFElectronD3PDObject.defineBlock (1, 'Layer3Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f3,
                                           'f3',
                                           egammaParameters.f3core,
                                           'f3core',
                                           ])
EFElectronD3PDObject.defineBlock (1, 'Iso',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.r33over37allcalo,
                                           'rphiallcalo',
                                           egammaParameters.etcone,
                                           'Etcone45',
                                           egammaParameters.etcone15,
                                           'Etcone15',
                                           egammaParameters.etcone20,
                                           'Etcone20',
                                           egammaParameters.etcone25,
                                           'Etcone25',
                                           egammaParameters.etcone30,
                                           'Etcone30',
                                           egammaParameters.etcone35,
                                           'Etcone35',
                                           egammaParameters.etcone40,
                                           'Etcone40',
                                           egammaParameters.ptcone20,
                                           'ptcone20',
                                           egammaParameters.ptcone30,
                                           'ptcone30',
                                           egammaParameters.ptcone40,
                                           'ptcone40',
                                           ])
## EFElectronD3PDObject.defineBlock (1, 'ConvFlags',
##                                 egammaD3PDMaker.egammaDetailFillerTool,
##                                 Details = [egammaParameters.convAngleMatch,
##                                            'convanglematch',
##                                            egammaParameters.convTrackMatch,
##                                            'convtrackmatch',
##                                            ])
## EFElectronD3PDObject.defineBlock (1, 'Conversion',
##                                 egammaD3PDMaker.egammaConversionFillerTool)
EFElectronD3PDObject.defineBlock (1, 'TrkMatch',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.pos7,
                                           'pos7',
                                           egammaParameters.EtaCorrMag,
                                           'etacorrmag',
                                           egammaParameters.deltaEta1,
                                           'deltaeta1',
                                           egammaParameters.deltaEta2,
                                           'deltaeta2',
                                           egammaParameters.deltaPhi2,
                                           'deltaphi2',
                                           egammaParameters.expectHitInBLayer,
                                           'expectHitInBLayer:Do we expect to see a hit from this track in the B pixel layer?',
                                           egammaParameters.trackd0_physics,
                                           'trackd0_physics:Transverse impact parameter with respect to the beam spot.',
                                    ])
EFElectronD3PDObject.defineBlock (1, 'Retaphi',
                                egammaD3PDMaker.egammaRetaphiFillerTool)


EFElectronD3PDObject.defineBlock (2, 'Rings',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.etringnoisedR03Sig2,
               'EtringnoisedR03sig2',
               egammaParameters.etringnoisedR03Sig3,
               'EtringnoisedR03sig3',
               egammaParameters.etringnoisedR03Sig4,
               'EtringnoisedR03sig4',
               ])
EFElectronD3PDObject.defineBlock (2, 'ElecDiscrim',
                                egammaD3PDMaker.egammaPIDFillerTool,
    PID  = [egammaPID.IsolationLikelihood_jets,
            'isolationlikelihoodjets',
            egammaPID.IsolationLikelihood_HQDelectrons,
            'isolationlikelihoodhqelectrons',
            egammaPID.ElectronWeight,
            'electronweight',
            egammaPID.BgWeight,
            'electronbgweight',
            egammaPID.SofteElectronWeight,
            'softeweight',
            egammaPID.SofteBgWeight,
            'softebgweight',
            egammaPID.NeuralNet,
            'neuralnet',
            egammaPID.Hmatrix,
            'Hmatrix',
            egammaPID.Hmatrix5,
            'Hmatrix5',
            egammaPID.AdaBoost,
            'adaboost',
            egammaPID.SofteNeuralNet,
            'softeneuralnet',
            ])
EFElectronD3PDObject.defineBlock (2, 'Pointing',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.zvertex,
                                           'zvertex',
                                           egammaParameters.errz,
                                           'errz',
                                           egammaParameters.etap,
                                           'etap',
                                           egammaParameters.depth,
                                           'depth'
                                           ])
## EFElectronD3PDObject.defineBlock (2, 'Brem',
##                                 egammaD3PDMaker.egammaDetailFillerTool,
##     Details = [egammaParameters.bremInvpT,
##                'breminvpt',
##                egammaParameters.bremRadius,
##                'bremradius',
##                egammaParameters.bremX,
##                'bremx',
##                egammaParameters.bremClusterRadius,
##                'bremclusterradius',
##                egammaParameters.bremInvpTerr,
##                'breminvpterr',
##                egammaParameters.bremTrackAuthor,
##                'bremtrackauthor',
##                egammaParameters.hasBrem,
##                'hasbrem',
##                egammaParameters.bremDeltaQoverP,
##                'bremdeltaqoverp',
##                egammaParameters.bremMaterialTraversed,
##                'bremmaterialtraversed',
##                ])
## EFElectronD3PDObject.defineBlock (2, 'RefittedTrk',
##                                 egammaD3PDMaker.egammaDetailFillerTool,
##     Details = [egammaParameters.refittedTrack_qOverP,
##                'refittedtrackqoverp',
##                egammaParameters.refittedTrack_d0,
##                'refittedtrackd0',
##                egammaParameters.refittedTrack_z0,
##                'refittedtrackz0',
##                egammaParameters.refittedTrack_theta,
##                'refittedtracktheta',
##                egammaParameters.refittedTrack_phi0,
##                'refittedtrackphi',
##                ])

ElectronClusterAssoc = SimpleAssociation \
    (EFElectronD3PDObject,
     egammaD3PDMaker.egammaClusterAssociationTool)
ElectronClusterAssoc.defineBlock \
    (2, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
ElectronClusterAssoc.defineBlock \
    (1, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     prefix = 'cl_',
     WriteE = True,
     WriteM = False)
def _clustereta2_lod (lod, args):
    return lod==1        # Only include this block if exactly level 1.
ElectronClusterAssoc.defineBlock \
    (_clustereta2_lod, 'ClusterEta2', CaloD3PDMaker.ClusterEMSamplingFillerTool,
     prefix = 'cl_',
     WriteEnergy = False,
     WritePhi = False,
     Samplings = [2])
## from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
## ElectronClusterAssoc.defineBlock \
##     (1, 'FwdEVars', CMFT,
##      Moments = [CMFT.FIRST_ENG_DENS,   'firstEdens',
##                 CMFT.ENG_FRAC_MAX,     'cellmaxfrac',
##                 CMFT.LONGITUDINAL,     'longitudinal',
##                 CMFT.SECOND_LAMBDA,    'secondlambda',
##                 CMFT.LATERAL,          'lateral',
##                 CMFT.SECOND_R,         'secondR',
##                 CMFT.CENTER_LAMBDA,    'centerlambda',
##                 ])

## ElectronRawClusterAssoc = SimpleAssociation \
##     (EFElectronD3PDObject,
##      egammaD3PDMaker.egammaRawClusterAssociationTool,
##      prefix = 'rawcl_',
##      AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
##         ('ElectronRawClusterAssocGetter',
##          SGKey = D3PDMakerFlags.ElectronRawClustersAssocSGKey(),
##          TypeName = 'INav4MomAssocs'))
## ElectronRawClusterAssoc.defineBlock \
##     (2, 'RawSamplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
## ElectronRawClusterAssoc.defineBlock \
##     (2, 'RawClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
##      WriteE = True,
##      WriteM = False)


EFElectronD3PDObject.defineBlock (3, 'RefittedTrkCov',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.refittedTrack_Covd0d0,
               'refittedtrackcovd0',
               egammaParameters.refittedTrack_Covz0z0,
               'refittedtrackcovz0',
               egammaParameters.refittedTrack_Covphiphi,
               'refittedtrackcovphi',
               egammaParameters.refittedTrack_Covthetatheta,
               'refittedtrackcovtheta',
               egammaParameters.refittedTrack_CovqOverPqOverP,
               'refittedtrackcovqoverp',
               egammaParameters.refittedTrack_Covd0z0,
               'refittedtrackcovd0z0',
               egammaParameters.refittedTrack_Covz0phi,
               'refittedtrackcovz0phi',
               egammaParameters.refittedTrack_Covz0theta,
               'refittedtrackcovz0theta',
               egammaParameters.refittedTrack_Covz0qOverP,
               'refittedtrackcovz0qoverp',
               egammaParameters.refittedTrack_Covd0phi,
               'refittedtrackcovd0phi',
               egammaParameters.refittedTrack_Covd0theta,
               'refittedtrackcovd0theta',
               egammaParameters.refittedTrack_Covd0qOverP,
               'refittedtrackcovd0qoverp',
               egammaParameters.refittedTrack_Covphitheta,
               'refittedtrackcovphitheta',
               egammaParameters.refittedTrack_CovphiqOverP,
               'refittedtrackcovphiqoverp',
               egammaParameters.refittedTrack_CovthetaqOverP,
               'refittedtrackcovthetaqoverp',
               ])


EFElectronD3PDObject.defineBlock \
       (1, 'Triggers',
        egammaD3PDMaker.egammaObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.ElectronEFTrigPattern() ])


ElectronTPAssoc = SimpleAssociation \
    (EFElectronD3PDObject,
     egammaD3PDMaker.egammaTrackParticleAssociationTool,
     matched = 'hastrack',
     blockname = 'TrkInfo')

# This generates ERROR messages.  Disabled for now.
#ElectronTPAssoc.defineBlock (1, 'TrackIso',
#                             TrackD3PDMaker.TrackIsolationFillerTool)

TrackParticlePerigeeAssoc = SimpleAssociation \
    (ElectronTPAssoc,
     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
     prefix = 'track')

TrackParticlePerigeeAssoc.defineBlock (1, 'Trk',
                                       TrackD3PDMaker.PerigeeFillerTool,
                                       FillMomentum = True)
TrackParticleCovarAssoc = SimpleAssociation (TrackParticlePerigeeAssoc,
                                             TrackD3PDMaker.PerigeeCovarianceAssociationTool)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     DiagCovariance = True)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovOffDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     OffDiagCovariance = True)


ElectronFitQualityAssoc = \
   SimpleAssociation (ElectronTPAssoc,
                      TrackD3PDMaker.TrackParticleFitQualityAssociationTool)
ElectronFitQualityAssoc.defineBlock (2, 'TrkFitQuality',
                                     TrackD3PDMaker.TrackFitQualityFillerTool,
                                     prefix = 'trackfit')

TrackSummaryAssoc = \
   SimpleAssociation (ElectronTPAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool)
TrackSummaryAssoc.defineBlock (1, 'IDHits',
                               TrackD3PDMaker.TrackTrackSummaryFillerTool,
                               IDSharedHits = True,
                               IDHits = True,
                               IDOutliers = True,
                               MuonHits = False,
                               HitSum = False,
                               HoleSum = False,
                               HitPattern = False,
                               SiHits = True,
                               TRTRatio = True)
TrackSummaryAssoc.defineBlock (1, 'TrackSummaryPID',
                               TrackD3PDMaker.TrackPIDFillerTool,
                               PID = [TrackD3PDMaker.eProbabilityComb,
                                      'eProbabilityComb',
                                      TrackD3PDMaker.eProbabilityHT,
                                      'eProbabilityHT',
                                      TrackD3PDMaker.eProbabilityToT,
                                      'eProbabilityToT',
                                      TrackD3PDMaker.eProbabilityBrem,
                                      'eProbabilityBrem',
                                      ])

ElectronVertAssoc = SimpleAssociation \
                    (ElectronTPAssoc,
                     TrackD3PDMaker.TrackParticleVertexAssociationTool,
                     prefix = 'vert')
ElectronVertAssoc.defineBlock (2, 'Vertex',
                               TrackD3PDMaker.VertexPositionFillerTool)


