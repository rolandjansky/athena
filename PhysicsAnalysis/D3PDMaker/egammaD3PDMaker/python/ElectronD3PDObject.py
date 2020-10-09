# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/ElectronD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date 2009
# @brief Configure electron D3PD object.
#

from egammaD3PDMaker.defineBlockAndAlg   \
     import defineAlgLODFunc, defineBlockAndAlg
from EventCommonD3PDMaker.DRAssociation     import DRAssociation
from TrackD3PDMaker.xAODTrackSummaryFiller  import xAODTrackSummaryFiller
from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject          import DeferArg
from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation, IdentityAssociation
from D3PDMakerCoreComps.IndexAssociation    import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.resolveSGKey        import resolveSGKey, testSGKey
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
# from TriggerD3PDMaker.defineTriggerBits     import defineTriggerBits
from RecExConfig.RecFlags                   import rec
import egammaD3PDMaker
import EventCommonD3PDMaker
import TrackD3PDMaker
import TruthD3PDMaker
import D3PDMakerCoreComps


ElectronD3PDObject = \
           make_SGDataVector_D3PDObject ('xAOD::ElectronContainer',
                                         D3PDMakerFlags.ElectronSGKey(),
                                         'el_', 'ElectronD3PDObject')

# AuxPrefix args need to be deferred in order to add in the sgkey.
auxprefix = DeferArg ('D3PDMakerFlags.EgammaUserDataPrefix() + "_" +'
                      'resolveSGKey (typeName, sgkey) + "_"',
                      globals())


ElectronD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
ElectronD3PDObject.defineBlock (
    0, 'Charge',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['charge'])
ElectronD3PDObject.defineBlock (
    0, 'Author',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['author'])
ElectronD3PDObject.defineBlock (
    0, 'Pass',
    D3PDMakerCoreComps.AuxDataFillerTool,
    # These are present only for central electrons.
    Vars = ['loose = Loose < int8_t: 0',
            'medium = Medium < int8_t: 0',
            'tight = Tight < int8_t : 0'
            ])
ElectronD3PDObject.defineBlock (
    0, 'OQ',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['OQ'])

defineBlockAndAlg \
        (ElectronD3PDObject,
        999, 'MaxEcell',
        D3PDMakerCoreComps.AuxDataFillerTool,
        'egammaMaxECellAlgConfig',
        AuxPrefix = auxprefix,
        Vars = ['maxEcell_time',
                'maxEcell_energy',
                'maxEcell_onlId',
                'maxEcell_gain',
                'maxEcell_x',
                'maxEcell_y',
                'maxEcell_z',
        ])

defineBlockAndAlg \
        (ElectronD3PDObject,
        999, 'SumCellsGain',
        D3PDMakerCoreComps.AuxDataFillerTool,
        'egammaSumCellsGainAlgConfig',
        AuxPrefix = auxprefix,
        Vars = [
        'Es0LowGain',
        'Es0MedGain',
        'Es0HighGain',
        'Es1LowGain',
        'Es1MedGain',
        'Es1HighGain',
        'Es2LowGain',
        'Es2MedGain',
        'Es2HighGain',
        'Es3LowGain',
        'Es3MedGain',
        'Es3HighGain',
        ])


defineBlockAndAlg \
       (ElectronD3PDObject,
        999, 'NbCellsGain',
        D3PDMakerCoreComps.AuxDataFillerTool,
        'egammaNbCellsGainAlgConfig',
         AuxPrefix = auxprefix,
         Vars = [
         'nbCells_s0LowGain',
         'nbCells_s0MedGain',
         'nbCells_s0HighGain',
         'nbCells_s1LowGain',
         'nbCells_s1MedGain',
         'nbCells_s1HighGain',
         'nbCells_s2LowGain',
         'nbCells_s2MedGain',
         'nbCells_s2HighGain',
         'nbCells_s3LowGain',
         'nbCells_s3MedGain',
         'nbCells_s3HighGain',
         ])
            

if rec.doTruth():
    from TruthD3PDMaker.MCTruthClassifierConfig \
         import D3PDMCTruthClassifier
    ElectronD3PDObject.defineBlock (1, 'TruthClassification',
                                    egammaD3PDMaker.egammaTruthClassificationFillerTool,
                                    DoBkgElecOrigin = True,
                                    Classifier = D3PDMCTruthClassifier)
    ElectronTruthPartAssoc = SimpleAssociation \
        (ElectronD3PDObject,
         egammaD3PDMaker.egammaGenParticleAssociationTool,
         prefix = 'truth_',
         matched = 'matched',
         blockname = 'TruthAssoc',
         Classifier = D3PDMCTruthClassifier)
    ElectronTruthPartAssoc.defineBlock (0, 'TruthKin',
                                        EventCommonD3PDMaker.FourMomFillerTool,
                                        WriteE = True,
                                        WriteM = False)
    ElectronTruthPartAssoc.defineBlock (0, 'Truth',
                                        TruthD3PDMaker.TruthParticleFillerTool,
                                        PDGIDVariable = 'type')
    ElectronTruthPartMotherAssoc = SimpleAssociation \
      (ElectronTruthPartAssoc,
       D3PDMakerCoreComps.FirstAssociationTool,
       Associator = TruthD3PDMaker.TruthParticleParentAssociationTool
         ('ElectronTruthPartMotherAssoc2'),
       blockname = 'ElectronTruthPartMotherAssoc',
       prefix = 'mother')
    ElectronTruthPartMotherAssoc.defineBlock (0, 'MotherTruth',
                                              TruthD3PDMaker.TruthParticleFillerTool,
                                              PDGIDVariable = 'type')
    ElectronTruthPartAssoc.defineBlock (0, 'TruthBrem',
                                        TruthD3PDMaker.TruthParticleBremFillerTool)
    ElectronTruthPartAssoc.defineBlock (0, 'TruthAssocIndex',
                                        D3PDMakerCoreComps.IndexFillerTool,
                                        Target = 'mc_')


ElectronD3PDObject.defineBlock (
    1, 'HadLeakage',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Ethad = ethad < float: 0',
            'Ethad1 = ethad1 < float: 0'])
ElectronD3PDObject.defineBlock (
    1, 'Layer1Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f1',
            'f1core',
            'Emins1 = emins1 < float: 0',
            'fside = fracs1',
            'Emax2 = e2tsts1 < float: 0',
            'ws3 = weta1',
            'wstot = wtots1',
            'emaxs1 < float: 0'])
ElectronD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                                egammaD3PDMaker.egammaLayer1ExtraFillerTool)
ElectronD3PDObject.defineBlock (
    1, 'Layer2Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E233 = e233 < float: 0',
            'E237 = e237 < float: 0',
            'E277 = e277 < float: 0',
            'weta2'])
ElectronD3PDObject.defineBlock (
    1, 'Layer3Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f3', 'f3core'])
ElectronD3PDObject.defineBlock (
    1, 'Iso',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rphiallcalo = r33over37allcalo < float: 0',
            'Etcone20 = etcone20 < float: 0',
            'Etcone30 = etcone30 < float: 0',
            'Etcone40 = etcone40 < float: 0',
            'ptcone20 < float: 0',
            'ptcone30 < float: 0',
            'ptcone40 < float: 0',
        ])
ElectronD3PDObject.defineBlock (
    2, 'IsoPtCorrected',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Etcone20_pt_corrected = etcone20_ptcorrected  < float: 0 #pt-corrected isolation energy within DR=0.20 cone',
            'Etcone30_pt_corrected = etcone30_ptcorrected  < float: 0 #pt-corrected isolation energy within DR=0.30 cone',
            'Etcone40_pt_corrected = etcone40_ptcorrected  < float: 0 #pt-corrected isolation energy within DR=0.40 cone',
            ])
ElectronD3PDObject.defineBlock (
    1, 'TrkMatch',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['pos7 < float: 0',
            'deltaeta1 = deltaEta1',
            'deltaeta2 = deltaEta2',
            'deltaphi2 = deltaPhi2',
            'deltaphiRescaled = deltaPhiRescaled2',
            ])
#ElectronD3PDObject.defineBlock (1, 'Retaphi',
#                               egammaD3PDMaker.egammaRetaphiFillerTool)


ElectronD3PDObject.defineBlock (
    1, 'TopoClusterIsolationCones',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['topoEtcone20 = topoetcone20 < float: 0',
            'topoEtcone30 = topoetcone30 < float: 0',
            'topoEtcone40 = topoetcone40 < float: 0',
            ])

ElectronD3PDObject.defineBlock (2, 'TraversedMaterial',
                               egammaD3PDMaker.egammaTraversedMaterialFillerTool )
    
ElectronD3PDObject.defineBlock (
    2, 'PointingShape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['zvertex < float: 0',
            'errz < float: 0',
            'etap < float: 0',
            'depth < float: 0'])


from egammaD3PDMaker.egammaCluster import egammaCluster
ElectronClusterAssoc = egammaCluster (ElectronD3PDObject,
                                      allSamplings = True,
                                      fwdEVars = True)


############################################################################
# TrackParticle variables
#


ElectronTPAssoc = SimpleAssociation \
    (ElectronD3PDObject,
     egammaD3PDMaker.ElectronTrackParticleAssociationTool,
     matched = 'hastrack',
     blockname = 'TrkInfo')

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


ElectronTPAssoc.defineBlock (
    2, 'TrkFitQuality',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['chi2 = chiSquared < float:0',
            'ndof = numberDoF < float:0'],
    prefix = 'trackfit')

xAODTrackSummaryFiller (ElectronTPAssoc, 1, 'IDHits',
                        IDSharedHits = True,
                        IDHits = True,
                        IDOutliers = True,
                        MuonHits = False,
                        HitSum = False,
                        HoleSum = False,
                        SCTInfoPlus = True,
                        PixelInfoPlus = True)

ElectronTPAssoc.defineBlock (
    1, 'TrackSummaryPID',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['eProbabilityComb < float:0',
            'eProbabilityHT < float:0',
            'eProbabilityToT < float:0',
            'eProbabilityBrem < float:0'])


ElectronVertAssoc = SimpleAssociation \
                    (ElectronTPAssoc,
                     TrackD3PDMaker.TrackParticleVertexAssociationTool,
                     Fast = False,
                     prefix = 'vert')
ElectronVertAssoc.defineBlock (
    2, 'Vertex',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['x < float:0 ', 'y < float:0', 'z < float:0'])


from TrackD3PDMaker.TrackParticleImpactParameters \
     import TrackParticleImpactParameters
TrackParticleImpactParameters (ElectronTPAssoc)


###Original Unrefitted track

TPPerigeePairAssoc = SimpleAssociation \
    (ElectronTPAssoc,
     TrackD3PDMaker.TrackParticlePerigeePairAtOOAssociationTool,
     AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
     ('GSFAssocGetter',
      SGKey = D3PDMakerFlags.GSFTrackAssocSGKey(),
      TypeName = 'TrackParticleAssocs'),
     blockname = 'TPPerigeePairAssoc')

UnRefittedTrackParticlePerigeeAssoc = SimpleAssociation \
    (TPPerigeePairAssoc,
     TrackD3PDMaker.PerigeePairOldPerigeeAssociationTool,
     blockname = 'UnrefittedTrackPerigeeAssoc',
     prefix = 'Unrefittedtrack_')

UnRefittedTrackParticlePerigeeAssoc.defineBlock (1, 'UnrefitTrk',
                                                 TrackD3PDMaker.PerigeeFillerTool,
                                                 FillMomentum = True)


############################################################################
# From UserData
#

if D3PDMakerFlags.HaveEgammaUserData() or D3PDMakerFlags.MakeEgammaUserData():
    #defineBlockAndAlg (ElectronD3PDObject, 
    #                   1, 'UDLayer1Shape',
    #                   D3PDMakerCoreComps.AuxDataFillerTool,
    #                   'egammaDeltaEmax2Config',
    #                   AuxPrefix = auxprefix,
    #                   Vars = ['deltaEmax2',
    #                           ])


    # `target' arg needs to be passed in from the caller;
    # otherwise, we don't make this block.
    def _jetAssocLevel (reqlev, args):
        if reqlev < 2: return False
        if 'target' not in args: return False
        args['Target'] = args['target']
        return True

    EgammaJetDRAssoc = IndexMultiAssociation\
                       (ElectronD3PDObject,
                        egammaD3PDMaker.ElectronJetDRAssociator,
                        '', # Overridden by the level function.
                        blockname='EgammaJetSignedIPAndPTRel',
                        prefix = 'jetcone_',
                        level = _jetAssocLevel,
                        DRCut=0.7,
                        VertexContainerName = D3PDMakerFlags.VertexSGKey() )
    EgammaJetDRAssoc.defineBlock \
          (2,
           'EgammaJetSignedIPAndPTRelKin',
           EventCommonD3PDMaker.FourMomFillerTool,
           WriteE = True)


    EgammaTrackJetDRAssoc = IndexMultiAssociation\
                            (ElectronD3PDObject,
                             egammaD3PDMaker.ElectronJetDRAssociator,
                             '', # Overridden by the level function.
                             blockname='EgammaTrackJetSignedIPAndPTRel',
                             prefix = 'jettrack_',
                             level = _jetAssocLevel,
                             DRCut=0.7,
                             VertexContainerName = D3PDMakerFlags.VertexSGKey() )
    EgammaTrackJetDRAssoc.defineBlock \
          (2,
           'EgammaTrackJetSignedIPAndPTRelKin',
           EventCommonD3PDMaker.FourMomFillerTool,
           WriteE = True)


   

############################################################################
# Jet associations
#

EleJetD3PDAssoc = DRAssociation (ElectronD3PDObject,
                                 'DataVector<xAOD::Jet_v1>',
                                 D3PDMakerFlags.JetSGKey(),
                                 0.2,
                                 'jet_',
                                 level = 2,
                                 blockname = 'JetMatch')
EleJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                             EventCommonD3PDMaker.FourMomFillerTool,
                             WriteE = True)


if rec.doTruth() and testSGKey ('DataVector<xAOD::Jet_v1>', D3PDMakerFlags.TruthJetSGKey()):
    JetTruthJetD3PDAssoc = DRAssociation (EleJetD3PDAssoc,
                                          'DataVector<xAOD::Jet_v1>',
                                          D3PDMakerFlags.TruthJetSGKey(),
                                          0.2,
                                          'truth_',
                                          level = 2,
                                          blockname = 'TrueJetMatch')
    JetTruthJetD3PDAssoc.defineBlock (2, 'TrueJetKinematics',
                                      EventCommonD3PDMaker.FourMomFillerTool,
                                      WriteE = True)


############################################################################
# Trigger matching
#


# if D3PDMakerFlags.DoTrigger():

#     defineTriggerBits (ElectronD3PDObject, 0,
#                        ['EF_2e5_medium',
#                         'EF_e10_medium',
#                         'EF_e20_loose',
#                         'EF_em105_passHLT',
#                         ])
#     defineTriggerBits (ElectronD3PDObject, 1,
#                        D3PDMakerFlags.egammaL1TrigPattern())
#     defineTriggerBits (ElectronD3PDObject, 1,
#                        D3PDMakerFlags.ElectronL2TrigPattern())
#     defineTriggerBits (ElectronD3PDObject, 1,
#                        D3PDMakerFlags.ElectronEFTrigPattern())


#     ### Matching indices.

#     # Unfortunately this can't be modified with a preExec after all, but
#     # I'll still keep it like this for the moment for the lack of time...
#     if not "UseEgammaMCMatching" in dir():
#         UseEgammaMCMatching = rec.doTruth()

#     from AthenaCommon.Logging import logging
#     logger = logging.getLogger( "ElectronD3PDObject" )
#     if UseEgammaMCMatching:
#         logger.info( "Using MC10a/MC10b matching for trigger electrons" )
#     else:
#         logger.info( "Using default matching for trigger electrons" )

#     ElectronEFIndexAssoc = IndexAssociation(
#         ElectronD3PDObject,
#         egammaD3PDMaker.egammaEFElectronTriggerObjectAssociationTool,
#         target = "trig_EF_el_", prefix = "EF_", level = 1,
#         Target = "trig_EF_el_",
#         blockname = "EFIndex",
#         ChainPattern = D3PDMakerFlags.ElectronEFTrigPattern(),
#         UseMCMetric = UseEgammaMCMatching, # Use special metric for MC files
#         MaxDR = 0.15,
#         allowMissing = True )
    
#     ElectronL2IndexAssoc = IndexAssociation(
#         ElectronD3PDObject,
#         egammaD3PDMaker.ElectronL2TriggerObjectAssociationTool,
#         target = "trig_L2_el_", # For IndexAssociation.
#         Target = "trig_L2_el_", # Passed directly to assoc tool.
#         prefix = "L2_", level = 1,
#         blockname = "L2Index",
#         ChainPattern = D3PDMakerFlags.ElectronL2TrigPattern(),
#         UseMCMetric = UseEgammaMCMatching, # Use special metric for MC files
#         MaxDR = 0.15,
#         allowMissing = True )
    
#     ElectronL1IndexAssoc = IndexAssociation(
#         ElectronD3PDObject,
#         egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
#         target = "trig_L1_emtau_", prefix = "L1_", level = 1,
#         ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
#         MaxDR = 0.15,
#         blockname = "L1Index")


