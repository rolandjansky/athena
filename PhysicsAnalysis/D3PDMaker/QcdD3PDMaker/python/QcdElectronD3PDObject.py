# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from egammaD3PDMaker.isem_version           import isem_version
from egammaD3PDMaker.PAUGammaAssociation    import PAUGammaAssociation
from D3PDMakerCoreComps.release_version     import release_version
from egammaD3PDAnalysis.ElectronUserDataConfig import ElectronUserDataConfig
from EventCommonD3PDMaker.DRAssociation     import DRAssociation
from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation    import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.resolveSGKey        import resolveSGKey
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
from TriggerD3PDMaker.defineTriggerBits     import defineTriggerBits
from AthenaCommon.AlgSequence               import AlgSequence
from RecExConfig.RecFlags                   import rec
import egammaD3PDMaker
import EventCommonD3PDMaker
import TrackD3PDMaker
import CaloD3PDMaker
import D3PDMakerCoreComps

from ROOT import egammaParameters
from ROOT import egammaPID


ElectronD3PDObject = \
           make_SGDataVector_D3PDObject ('ElectronContainer',
                                         D3PDMakerFlags.ElectronSGKey(),
                                         'el_', 'QcdElectronD3PDObject')
def _electronAlgHook (c, prefix, sgkey, typeName,
                      seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                      *args, **kw):
    ElectronUserDataConfig (seq = seq,
                            prefix = prefix,
                            sgkey = sgkey,
                            typeName = typeName)

    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import jobproperties
    # set some options here.
    jobproperties.PhotonAnalysisUtilsFlags.DumpNtuple        = False
    jobproperties.PhotonAnalysisUtilsFlags.DumpHistograms    = False
    if hasattr (jobproperties.PhotonAnalysisUtilsFlags, 'DumpTimeDifference'):
        jobproperties.PhotonAnalysisUtilsFlags.DumpTimeDifference = False
    jobproperties.PhotonAnalysisUtilsFlags.AlgSequence       = seq.getName()
    jobproperties.PhotonAnalysisUtilsFlags.DoReRunJetFinding = True
    # probably a few more need to be set

    rlist = release_version()
    if rlist == None:
        # Assume an old version if no version string was stored.
        jobproperties.PhotonAnalysisUtilsFlags.ProductionRelease = 15.0
    elif rlist:
        jobproperties.PhotonAnalysisUtilsFlags.ProductionRelease = \
           rlist[0] + float(rlist[1])/10
    
    #commented out PUU
    #from PhotonAnalysisUtils.PhotonAnalysisUtilsGetter import PhotonAnalysisUtilsGetter
    #pau = PhotonAnalysisUtilsGetter()

    return
ElectronD3PDObject.defineHook (_electronAlgHook)


ElectronD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
ElectronD3PDObject.defineBlock (0, 'Charge',
                                EventCommonD3PDMaker.ChargeFillerTool)
ElectronD3PDObject.defineBlock (0, 'Author',
                                egammaD3PDMaker.egammaAuthorFillerTool)
ElectronD3PDObject.defineBlock (0, 'IsEM',
                                egammaD3PDMaker.egammaIsEMoneFillerTool
                                )
ElectronD3PDObject.defineBlock (0, 'Conversion0',
                                egammaD3PDMaker.egammaConversion0FillerTool,
                                )

#commented out to avoid segmentation error in MCTruthClassifier
#if rec.doTruth():
#    import TruthD3PDMaker.MCTruthClassifierConfig
#    ElectronD3PDObject.defineBlock (1, 'TruthClassification',
#                                    egammaD3PDMaker.egammaTruthClassificationFillerTool)
#    ElectronGenPartAssoc = SimpleAssociation \
#        (ElectronD3PDObject,
#         egammaD3PDMaker.egammaGenParticleAssociationTool,
#         prefix = 'truth_',
#         matched = 'matched',
#         blockname = 'Truth')
#    ElectronGenPartAssoc.defineBlock (0, 'Truth',
#                                      EventCommonD3PDMaker.GenParticleFillerTool,
#                                      WriteE = True,
#                                      WriteM = False)
#    ElectronGenPartAssoc.defineBlock (0, 'TruthBrem',
#                                      EventCommonD3PDMaker.GenParticleBremFillerTool)



if isem_version() == 1:
    ElectronD3PDObject.defineBlock (0, 'IsEMCuts',
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
    ElectronD3PDObject.defineBlock (0, 'IsEMCuts',
                                    egammaD3PDMaker.egammaIsEMFillerTool,
                                    IsEM  = [(egammaPID.ElectronLoose,
                                              egammaPID.frwdElectronLoose),
                                             'loose',
                                             (egammaPID.ElectronMedium,
                                              None),
                                             'medium',
                                             (egammaPID.ElectronMediumIso,
                                              None),
                                             'mediumIso',
                                             (egammaPID.ElectronTight,
                                              egammaPID.frwdElectronTight),
                                             'tight',
                                             (egammaPID.ElectronTightIso,
                                              None),
                                             'tightIso',
                                    ])


ElectronD3PDObject.defineBlock (1, 'HadLeakage',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.ethad,
                                           'Ethad',
                                           egammaParameters.ethad1,
                                           'Ethad1',
                                           ])
ElectronD3PDObject.defineBlock (1, 'Layer1Shape',
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
ElectronD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                                egammaD3PDMaker.egammaLayer1ExtraFillerTool)
ElectronD3PDObject.defineBlock (1, 'Layer2Shape',
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
ElectronD3PDObject.defineBlock (1, 'Layer3Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f3,
                                           'f3',
                                           egammaParameters.f3core,
                                           'f3core',
                                           ])
ElectronD3PDObject.defineBlock (1, 'Iso',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.r33over37allcalo,
                                           'rphiallcalo',
                                           egammaParameters.etcone,
                                           'Etcone45',
                                           egammaParameters.etcone20,
                                           'Etcone20',
                                           egammaParameters.etcone30,
                                           'Etcone30',
                                           egammaParameters.etcone40,
                                           'Etcone40',
                                           egammaParameters.ptcone30,
                                           'ptcone30',
                                           ])
ElectronD3PDObject.defineBlock (1, 'ConvFlags',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.convAngleMatch,
                                           'convanglematch',
                                           egammaParameters.convTrackMatch,
                                           'convtrackmatch',
                                           ])
ElectronD3PDObject.defineBlock (1, 'Conversion',
                                egammaD3PDMaker.egammaConversionFillerTool)
ElectronD3PDObject.defineBlock (1, 'TrkMatch',
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
                                           egammaParameters.deltaPhiRescaled,
                                           'deltaphiRescaled',
                                           egammaParameters.expectHitInBLayer,
                                           'expectHitInBLayer',
                                    ])
ElectronD3PDObject.defineBlock (1, 'Retaphi',
                                egammaD3PDMaker.egammaRetaphiFillerTool)


ElectronD3PDObject.defineBlock (2, 'Rings',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.etringnoisedR03Sig2,
               'EtringnoisedR03sig2',
               egammaParameters.etringnoisedR03Sig3,
               'EtringnoisedR03sig3',
               egammaParameters.etringnoisedR03Sig4,
               'EtringnoisedR03sig4',
               ])
ElectronD3PDObject.defineBlock (2, 'ElecDiscrim',
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
ElectronD3PDObject.defineBlock (2, 'Pointing',
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
ElectronD3PDObject.defineBlock (2, 'Brem',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.bremInvpT,
               'breminvpt',
               egammaParameters.bremRadius,
               'bremradius',
               egammaParameters.bremX,
               'bremx',
               egammaParameters.bremClusterRadius,
               'bremclusterradius',
               egammaParameters.bremInvpTerr,
               'breminvpterr',
               egammaParameters.bremTrackAuthor,
               'bremtrackauthor',
               egammaParameters.hasBrem,
               'hasbrem',
               egammaParameters.bremDeltaQoverP,
               'bremdeltaqoverp',
               egammaParameters.bremMaterialTraversed,
               'bremmaterialtraversed',
               ])
ElectronD3PDObject.defineBlock (1, 'RefittedTrk',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.refittedTrack_qOverP,
               'refittedtrackqoverp',
               egammaParameters.refittedTrack_d0,
               'refittedtrackd0',
               egammaParameters.refittedTrack_z0,
               'refittedtrackz0',
               egammaParameters.refittedTrack_theta,
               'refittedtracktheta',
               egammaParameters.refittedTrack_phi0,
               'refittedtrackphi',
               ])

ElectronClusterAssoc = SimpleAssociation \
    (ElectronD3PDObject,
     egammaD3PDMaker.egammaClusterAssociationTool)
ElectronClusterAssoc.defineBlock \
    (1, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
ElectronClusterAssoc.defineBlock \
    (2, 'AllSamplings', CaloD3PDMaker.ClusterSamplingFillerTool,
     EmHadEnergies = False,
     SamplingEnergies = True)
ElectronClusterAssoc.defineBlock \
    (1, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     prefix = 'cl_',
     WriteE = True,
     WriteM = False)
from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
ElectronClusterAssoc.defineBlock \
    (1, 'FwdEVars', CMFT,
     Moments = [CMFT.FIRST_ENG_DENS,   'firstEdens',
                CMFT.ENG_FRAC_MAX,     'cellmaxfrac',
                CMFT.LONGITUDINAL,     'longitudinal',
                CMFT.SECOND_LAMBDA,    'secondlambda',
                CMFT.LATERAL,          'lateral',
                CMFT.SECOND_R,         'secondR',
                CMFT.CENTER_LAMBDA,    'centerlambda',
                ])

ElectronRawClusterAssoc = SimpleAssociation \
    (ElectronD3PDObject,
     egammaD3PDMaker.egammaRawClusterAssociationTool,
     prefix = 'rawcl_',
     AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
        ('ElectronRawClusterAssocGetter',
         #SGKey filled in by hook fn below.
         TypeName = 'INav4MomAssocs'))
ElectronRawClusterAssoc.defineBlock \
    (2, 'RawSamplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
ElectronRawClusterAssoc.defineBlock \
    (2, 'RawClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     WriteE = True,
     WriteM = False)
def _electronRawClusterAssocHook (c, prefix, sgkey, typeName,
                                  *args, **kw):
    assoc = getattr(c, c.name() + '_D3PD__egammaRawClusterAssociationTool',
                    None)
    if assoc:
        assoc.Associator.AssocGetter.SGKey = \
                  (resolveSGKey(typeName, sgkey) +
                   D3PDMakerFlags.RawClustersAssocSGKeySuffix())
    return
ElectronD3PDObject.defineHook (_electronRawClusterAssocHook)



ElectronD3PDObject.defineBlock (3, 'RefittedTrkCov',
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



ElectronTPAssoc = SimpleAssociation \
    (ElectronD3PDObject,
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
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
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
ElectronVertAssoc.defineBlock (1, 'Vertex',
                               TrackD3PDMaker.VertexPositionFillerTool)



############################################################################
# From UserData
#

if D3PDMakerFlags.HaveEgammaUserData() or D3PDMakerFlags.MakeEgammaUserData():
    # `target' arg needs to be passed in from the caller;
    # otherwise, we don't make this block.
    def _jetAssocLevel (reqlev, args):
        if reqlev < 2: return False
        if not args.has_key ('target'): return False
        args['Target'] = args['target']
        return True
    EgammaSignedIPAndPTRelAssoc = IndexMultiAssociation\
                                   (ElectronD3PDObject,
                                    egammaD3PDMaker.egammaJetDRAssociator,
                                    '', # Overridden by the level function.
                                    blockname='EgammaJetSignedIPAndPTRel',
                                    prefix = 'jetcone_',
                                    level = _jetAssocLevel,
                                    DRCut=0.7,
                                    VertexContainerName = D3PDMakerFlags.VertexSGKey() )
    EgammaSignedIPAndPTRelAssoc.defineBlock \
          (2,
           'EgammaJetSignedIPAndPTRelKin',
           EventCommonD3PDMaker.FourMomFillerTool,
           WriteE = True)




############################################################################
# Jet associations
#

EleJetD3PDAssoc = DRAssociation (ElectronD3PDObject,
                                 'JetCollection',
                                 D3PDMakerFlags.JetSGKey(),
                                 0.2,
                                 'jet_',
                                 level = 2,
                                 blockname = 'JetMatch')
EleJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                             EventCommonD3PDMaker.FourMomFillerTool,
                             WriteE = True)


#if rec.doTruth():
#    JetTruthJetD3PDAssoc = DRAssociation (EleJetD3PDAssoc,
#                                          'JetCollection',
#                                          D3PDMakerFlags.TruthJetSGKey(),
#                                          0.2,
#                                          'truth_',
#                                          level = 2,
#                                          blockname = 'TrueJetMatch')
#    JetTruthJetD3PDAssoc.defineBlock (2, 'TrueJetKinematics',
#                                      EventCommonD3PDMaker.FourMomFillerTool,
#                                      WriteE = True)
#

#commented out PUU
############################################################################
# PhotonAnalysisUtils associations
#
#
#ElectronPAUAssoc = PAUGammaAssociation (ElectronD3PDObject,
#                                      'PAU::gammaContainer',
#                                      'PAUelectronCollection',
#                                      level = 2,
#                                      blockname = 'PAU')
#ElectronPAUAssoc.defineBlock (2, 'PAUVariables',
#                              egammaD3PDMaker.PAUFillerTool,
#                              FillPhotons = False,
#                              FillHPV = False,
#                              FillAllCorrectedIsolation = False)

############################################################################
# Trigger matching
#


if D3PDMakerFlags.DoTrigger():

#comment out trigger bit
#    defineTriggerBits (ElectronD3PDObject, 0,
#                       ['EF_2e5_medium',
#                        'EF_e10_medium',
#                        'EF_e20_loose',
#                        'EF_em105_passHLT',
#                        ])
#    defineTriggerBits (ElectronD3PDObject, 1,
#                       D3PDMakerFlags.egammaL1TrigPattern())
#    defineTriggerBits (ElectronD3PDObject, 1,
#                       D3PDMakerFlags.ElectronL2TrigPattern())
#    defineTriggerBits (ElectronD3PDObject, 1,
#                       D3PDMakerFlags.ElectronEFTrigPattern())
#

    ### Matching indices.
    
    ElectronEFIndexAssoc = IndexAssociation(
        ElectronD3PDObject,
        egammaD3PDMaker.egammaEFTriggerObjectAssociationTool,
        target = "trig_EF_el_", prefix = "EF_", level = 1,
        Target = "trig_EF_el_",
        blockname = "EFIndex",
        ChainPattern = D3PDMakerFlags.ElectronEFTrigPattern(),
        allowMissing = True )
    
    ElectronL2IndexAssoc = IndexAssociation(
        ElectronD3PDObject,
        egammaD3PDMaker.ElectronL2TriggerObjectAssociationTool,
        target = "trig_L2_el_", # For IndexAssociation.
        Target = "trig_L2_el_", # Passed directly to assoc tool.
        prefix = "L2_", level = 1,
        blockname = "L2Index",
        ChainPattern = D3PDMakerFlags.ElectronL2TrigPattern(),
        allowMissing = True )
    
    ElectronL1IndexAssoc = IndexAssociation(
        ElectronD3PDObject,
        egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
        target = "trig_L1_emtau_", prefix = "L1_", level = 1,
        ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
        blockname = "L1Index")

    ### Matched objects by containment.

    ElectronEFAssoc = SimpleAssociation \
        (ElectronD3PDObject,
         egammaD3PDMaker.egammaEFTriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'EFInfo',
         prefix = 'EF_',
         ChainPattern = D3PDMakerFlags.ElectronEFTrigPattern(),
         WriteDR = False)
    ElectronEFAssoc.defineBlock (1, 'EFKinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE = True,
                                 WriteEt = True,
                                 WriteM = False)
    ElectronEFAssoc.defineBlock (1, 'EFCharge',
                                 EventCommonD3PDMaker.ChargeFillerTool)
    ElectronEFAssoc.defineBlock (1, 'EFHadLeakage',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.ethad,
                                            'Ethad',
                                            egammaParameters.ethad1,
                                            'Ethad1',
                                            ])
    ElectronEFAssoc.defineBlock (1, 'EFLayer1Shape',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.f1,
                                            'f1',
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
                                            ])
    ElectronEFAssoc.defineBlock (1, 'EFLayer2Shape',
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
    ElectronEFAssoc.defineBlock (1, 'EFIso',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.r33over37allcalo,
                                            'rphiallcalo',
                                            ])
    ElectronEFAssoc.defineBlock (1, 'EFTrkMatch',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.deltaEta1,
                                            'deltaeta1',
                                            egammaParameters.deltaEta2,
                                            'deltaeta2',
                                            egammaParameters.deltaPhi2,
                                            'deltaphi2',
                                            egammaParameters.expectHitInBLayer,
                                            'expectHitInBLayer',
                                        ])

    ElectronEFAssoc.defineBlock (2, 'EFIso2',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.etcone,
                                            'Etcone45',
                                            egammaParameters.etcone20,
                                            'Etcone20',
                                            egammaParameters.etcone30,
                                            'Etcone30',
                                            egammaParameters.etcone40,
                                            'Etcone40',
                                            ])
    ElectronEFAssoc.defineBlock (2, 'EFTrkMatch2',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.EtaCorrMag,
                                            'etacorrmag',
                                        ])
    ElectronEFAssoc.defineBlock (2, 'EFPointing',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.zvertex,
                                            'zvertex',
                                            egammaParameters.errz,
                                            'errz',
                                            ])

    ElectronEFTPAssoc = SimpleAssociation \
        (ElectronEFAssoc,
         egammaD3PDMaker.egammaTrackParticleAssociationTool,
         matched = 'hastrack',
         blockname = 'EFTrkInfo')
    EFTrackParticlePerigeeAssoc = SimpleAssociation \
        (ElectronEFTPAssoc,
         TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
         blockname = 'EFPerigeeAssoc',
         prefix = 'track')
    EFTrackParticlePerigeeAssoc.defineBlock (1, 'EFTrk',
                                          TrackD3PDMaker.TrackPerigeeFillerTool,
                                          FillMomentum = True)
    EFTrackSummaryAssoc = \
        SimpleAssociation (ElectronEFTPAssoc,
                      TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool,
                           blockname = 'EFTrackSummaryAssoc')
    EFTrackSummaryAssoc.defineBlock (1, 'EFIDHits',
                                     TrackD3PDMaker.TrackTrackSummaryFillerTool,
                                     IDHoles = False,
                                     IDHits = True,
                                     MuonHits = False,
                                     HitSum = False,
                                     HoleSum = False,
                                     HitPattern = False,
                                     SiHits = True,
                                     TRTRatio = True)


    ElectronL2Assoc = SimpleAssociation \
        (ElectronD3PDObject,
         egammaD3PDMaker.ElectronL2TriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'L2Info',
         prefix = 'L2_',
         ChainPattern = D3PDMakerFlags.ElectronL2TrigPattern(),
         WriteDR = False )
    ElectronL2Assoc.defineBlock (1, 'L2Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE = True,
                                 WriteEt = True,
                                 WriteM = False)
    ElectronL2Assoc.defineBlock (1, 'L2Charge',
                                 egammaD3PDMaker.TrigElectronChargeFillerTool)
    ElectronL2Assoc.defineBlock (1, 'L2Calo1',
                                 egammaD3PDMaker.TrigElectronCalo1FillerTool)
    ElectronL2Assoc.defineBlock (1, 'L2Track1',
                                 egammaD3PDMaker.TrigElectronTrack1FillerTool)
    ElectronL2Assoc.defineBlock (2, 'L2Track2',
                                 egammaD3PDMaker.TrigElectronTrack2FillerTool)
    ElectronL2ClusAssoc = SimpleAssociation \
       (ElectronL2Assoc,
        egammaD3PDMaker.TrigElectronClusterAssociationTool)
    ElectronL2ClusAssoc.defineBlock (2, 'L2Clus2',
                                     egammaD3PDMaker.TrigEMClusterVarsFillerTool)


    ElectronL1Assoc = SimpleAssociation \
        (ElectronD3PDObject,
         egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'L1Info',
         prefix = 'L1_',
         ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
         WriteDR = False)
    ElectronL1Assoc.defineBlock (1, 'L1Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteEt = False,
                                 WritePt = False,
                                 WriteM = False)
    ElectronL1Assoc.defineBlock (1, 'L1Isol',
                                 egammaD3PDMaker.EmTau_ROIIsoFillerTool)
    ElectronL1Assoc.defineBlock (1, 'L1EmET',
                                 egammaD3PDMaker.EmTau_ROIEmETFillerTool)
