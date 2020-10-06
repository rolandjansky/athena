# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/PhotonD3PDObject.py
# @author Haifeng Li <Haifeng.Li@cern.ch>, scott snyder <snyder@bnl.gov>
# @date 13 Aug. 2009
# @brief Configure photon D3PD object.
#


# from egammaD3PDMaker.isem_version            import isem_version
from egammaD3PDMaker.defineBlockAndAlg   \
     import defineAlgLODFunc, defineBlockAndAlg
from EventCommonD3PDMaker.DRAssociation      import DRAssociation
from D3PDMakerCoreComps.D3PDObject           import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject           import DeferArg
from D3PDMakerCoreComps.SimpleAssociation    import SimpleAssociation
# from D3PDMakerCoreComps.IndexAssociation     import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey, testSGKey
from TrackD3PDMaker.xAODTrackSummaryFiller   import xAODTrackSummaryFiller
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
# from TriggerD3PDMaker.defineTriggerBits      import defineTriggerBits
# from AthenaCommon.AlgSequence                import AlgSequence
from RecExConfig.RecFlags                    import rec
import egammaD3PDMaker
# import egammaD3PDAnalysis
import EventCommonD3PDMaker
# import CaloD3PDMaker
import D3PDMakerCoreComps
import TruthD3PDMaker
import TrackD3PDMaker




PhotonD3PDObject = \
           make_SGDataVector_D3PDObject ('xAOD::PhotonContainer',
                                         D3PDMakerFlags.PhotonSGKey(),
                                         'ph_', 'PhotonD3PDObject')

# AuxPrefix args need to be deferred in order to add in the sgkey.
auxprefix = DeferArg ('D3PDMakerFlags.EgammaUserDataPrefix() + "_" +'
                     'resolveSGKey (typeName, sgkey) + "_"',
                     globals())


PhotonD3PDObject.defineBlock (0, 'Kinematics',
                              EventCommonD3PDMaker.FourMomFillerTool,
                              WriteE  = True,
                              WriteEt = True,
                              WriteRect = True)
PhotonD3PDObject.defineBlock (
    0, 'Author',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['author'])
PhotonD3PDObject.defineBlock (
    0, 'Pass',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['loose = Loose',
            'tight = Tight'
            ])
PhotonD3PDObject.defineBlock (
    0, 'OQ',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['OQ'])

PhotonD3PDObject.defineBlock (0, 'Conversion0',
                              egammaD3PDMaker.egammaConversion0FillerTool,
                              )

defineBlockAndAlg \
        (PhotonD3PDObject,
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
        (PhotonD3PDObject,
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
       (PhotonD3PDObject,
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
    PhotonD3PDObject.defineBlock (1, 'TruthClassification',
                                  egammaD3PDMaker.egammaTruthClassificationFillerTool,
                                  Classifier = D3PDMCTruthClassifier)
    PhotonTruthPartAssoc = SimpleAssociation \
        (PhotonD3PDObject,
         egammaD3PDMaker.egammaGenParticleAssociationTool,
         prefix = 'truth_',
         matched = 'matched',
         blockname = 'TruthAssoc',
         DRVar = 'deltaRRecPhoton',
         Classifier = D3PDMCTruthClassifier)
    PhotonTruthPartAssoc.defineBlock (0, 'TruthKin',
                                      EventCommonD3PDMaker.FourMomFillerTool,
                                      WriteE = True,
                                      WriteM = False)
    PhotonTruthPartAssoc.defineBlock (0, 'Truth',
                                      TruthD3PDMaker.TruthParticleFillerTool,
                                      PDGIDVariable = 'type')
    PhotonTruthPartMotherAssoc = SimpleAssociation \
      (PhotonTruthPartAssoc,
       D3PDMakerCoreComps.FirstAssociationTool,
       Associator = TruthD3PDMaker.TruthParticleParentAssociationTool
         ('PhotonTruthPartMotherAssoc2'),
       blockname = 'PhotonTruthPartMotherAssoc',
       prefix = 'mother')
    PhotonTruthPartMotherAssoc.defineBlock (0, 'MotherTruth',
                                            TruthD3PDMaker.TruthParticleFillerTool,
                                            PDGIDVariable = 'type')
    PhotonTruthPartAssoc.defineBlock (0, 'TruthAssocIndex',
                                      D3PDMakerCoreComps.IndexFillerTool,
                                      Target = 'mc_')



PhotonD3PDObject.defineBlock (
    1, 'HadLeakage',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Ethad = ethad',
            'Ethad1 = ethad1'])
PhotonD3PDObject.defineBlock (
    1, 'Layer0Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E033 = e033 < float : 0'])
PhotonD3PDObject.defineBlock (
    1, 'Layer1Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f1',
            'f1core',
            'Emins1 = emins1',
            'fside = fracs1',
            'Emax2 = e2tsts1',
            'ws3 = weta1',
            'wstot = wtots1',
            'E132 = e132',
            'E1152 = e1152',
            'emaxs1'])
PhotonD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                              egammaD3PDMaker.egammaLayer1ExtraFillerTool)
PhotonD3PDObject.defineBlock (
    1, 'Layer2Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E233 = e233',
            'E237 = e237',
            'E277 = e277',
            'weta2'])
PhotonD3PDObject.defineBlock (
    1, 'Layer3Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f3', 'f3core'])
PhotonD3PDObject.defineBlock (
    1, 'Iso',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rphiallcalo = r33over37allcalo',
            'Etcone20 = etcone20',
            'Etcone30 = etcone30',
            'Etcone40 = etcone40',
            'ptcone20',
            'ptcone30',
            'ptcone40',
        ])
PhotonD3PDObject.defineBlock (
    2, 'IsoPtCorrected',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Etcone20_pt_corrected = etcone20_ptcorrected < float: 0 #pt-corrected isolation energy within DR=0.20 cone',
            'Etcone30_pt_corrected = etcone30_ptcorrected < float: 0 #pt-corrected isolation energy within DR=0.30 cone',
            'Etcone40_pt_corrected = etcone40_ptcorrected < float: 0 #pt-corrected isolation energy within DR=0.40 cone',
            ])
PhotonD3PDObject.defineBlock (1, 'Conversion',
                              egammaD3PDMaker.egammaConversionFillerTool)
PhotonD3PDObject.defineBlock (1, 'Retaphi',
                              egammaD3PDMaker.egammaRetaphiFillerTool)
                                           


PhotonD3PDObject.defineBlock (
    1, 'TopoClusterIsolationCones',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['topoEtcone20 = topoetcone20_core57cells,topoetcone20',
            'topoEtcone30 = topoetcone30_core57cells,topoetcone30',
            'topoEtcone40 = topoetcone40_core57cells,topoetcone40',
            ])
PhotonD3PDObject.defineBlock (2, 'TraversedMaterial',
                              egammaD3PDMaker.egammaTraversedMaterialFillerTool )


PhotonD3PDObject.defineBlock (
    2, 'Pointing',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['zvertex < float:0 ',
            'errz < float:0 ',
            'etap < float: 0',
            'depth < float: 0'])

from egammaD3PDMaker.egammaCluster import egammaCluster
PhotonClusterAssoc = egammaCluster (PhotonD3PDObject)


PhotonD3PDObject.defineBlock (
    1, 'ConvDeltaEtaPhi',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['convMatchDeltaEta1',
            'convMatchDeltaEta2',
            'convMatchDeltaPhi1',
            'convMatchDeltaPhi2',
        ])


#----------------------------------------------
# This part gets all conversion vertices
# associated with the photon.  So it makes vector<vector<vector<T>>>'s, again with the
# index pointing towards the trk_ block.
#

# Associate the photon with secondary vertices
ConversionVertexAssociation = ContainedVectorMultiAssociation (
    PhotonD3PDObject,
    egammaD3PDMaker.ConversionVertexAssociationTool,
    prefix='vx_')
ConversionVertexAssociation.defineBlock (
    10, 'ConversionVertexPosition',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['x < float:0 ', 'y < float:0', 'z < float:0'])
ConversionVertexAssociation.defineBlock (
    10, 'ConversionVertexKinematics',
    TrackD3PDMaker.VertexKineFillerTool)


ConversionVertexTrackParticleAssociation = \
   ContainedVectorMultiAssociation (ConversionVertexAssociation,
                                    TrackD3PDMaker.VertexTrackParticleAssociationTool,
                                    prefix = 'convTrk_')


# `target' arg needs to be passed in from the caller;
# otherwise, we don't make this block.
def _cvAssocLevel (reqlev, args):
    if reqlev < 10: return False
    if 'target' not in args: return False
    args['Target'] = args['target']
    del args['target']
    return True
ConversionVertexTrackParticleAssociation.defineBlock (
    _cvAssocLevel, "ConversionVertexTrackIndex",
    D3PDMakerCoreComps.IndexFillerTool,
    # Target will be filled in by level function.
    Target = '')

ConversionVertexTrackParticleAssociation.defineBlock (
    10, 'ConversionVertexTrackInfo',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['fitter = trackFitter < unsigned char: 0',
            'patternReco = patternRecoInfo < unsigned long: 0',
            'trackProperties < unsigned char: 0',
            'particleHypothesis < unsigned char: 0',
            ])
     
xAODTrackSummaryFiller (ConversionVertexTrackParticleAssociation,
                        10, 'CoversionTrackSummary',
                        IDHits = True,
                        IDHoles = True,
                        IDSharedHits = True,
                        IDOutliers = True,
                        PixelInfoPlus = True,
                        SCTInfoPlus = True,
                        TRTInfoPlus = True,
                        InfoPlus = True,
                        MuonHits = False,
                        MuonHoles = False,
                        ExpectBLayer = True,
                        HitSum = False,
                        HoleSum = False,
                        )
ConversionVertexTrackParticleAssociation.defineBlock (
    10, 'ConversionTrackSummaryPID',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['eProbabilityComb',
            'eProbabilityHT',
            'eProbabilityToT < float:0',
            'eProbabilityBrem < float:0'])
ConversionVertexTrackParticleAssociation.defineBlock (
    10, 'ConversionTrackFitQuality',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['chi2 = chiSquared',
            'ndof = numberDoF'])
    


############################################################################
# From UserData
#

if D3PDMakerFlags.HaveEgammaUserData() or D3PDMakerFlags.MakeEgammaUserData():
    if rec.doTruth():
        defineBlockAndAlg \
          (PhotonD3PDObject,
           0, 'UDTruthFlags',
           D3PDMakerCoreComps.AuxDataFillerTool,
           'PhotonTruthConfig',
           AuxPrefix = auxprefix,
           Vars = ['truth_isConv',               
                   'truth_isBrem',               
                   'truth_isFromHardProc',       
                   'truth_isPhotonFromHardProc', 
                   ])

        defineBlockAndAlg \
          (PhotonD3PDObject,
           1, 'UDTruthConv',
           D3PDMakerCoreComps.AuxDataFillerTool,
           'PhotonTruthConfig',
           AuxPrefix = auxprefix,
           Vars = ['truth_Rconv', 
                   'truth_zconv', 
                   ])

    defineBlockAndAlg (PhotonD3PDObject, 
                       1, 'UDLayer1Shape',
                       D3PDMakerCoreComps.AuxDataFillerTool,
                       'egammaDeltaEmax2Config',
                       AuxPrefix = auxprefix,
                       Vars = ['deltaEmax2',
                               ])

        

############################################################################
# Jet associations
#

PhotonJetD3PDAssoc = DRAssociation (PhotonD3PDObject,
                                    'DataVector<xAOD::Jet_v1>',
                                    D3PDMakerFlags.JetSGKey(),
                                    0.2,
                                    'jet_',
                                    level = 2,
                                    blockname = 'JetMatch')
PhotonJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE = True)


if rec.doTruth() and testSGKey ('DataVector<xAOD::Jet_v1>', D3PDMakerFlags.TruthJetSGKey()):
    JetTruthJetD3PDAssoc = DRAssociation (PhotonJetD3PDAssoc,
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
# Topo cluster associations.
#


PhotonTopoD3PDAssoc = DRAssociation (PhotonD3PDObject,
                                     'DataVector<xAOD::CaloCluster_v1>',
                                     D3PDMakerFlags.ClusterSGKey(),
                                     0.1,
                                     'topo',
                                     level = 2,
                                     blockname = 'TopoMatch')
PhotonTopoD3PDAssoc.defineBlock (2, 'TopoKinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteM = False)

# PhotonTopoEMD3PDAssoc = DRAssociation (PhotonD3PDObject,
#                                        'DataVector<xAOD::CaloCluster_v1>',
#                                        D3PDMakerFlags.EMTopoClusterSGKey(),
#                                        0.1,
#                                        'topoEM',
#                                        level = 2,
#                                        blockname = 'TopoEMMatch')
# PhotonTopoEMD3PDAssoc.defineBlock (2, 'TopoEMKinematics',
#                                    EventCommonD3PDMaker.FourMomFillerTool,
#                                    WriteM = False)


############################################################################
# Trigger matching
#

# if D3PDMakerFlags.DoTrigger():

#     defineTriggerBits (PhotonD3PDObject, 0,
#                        ['EF_g20_loose',
#                         ])
#     defineTriggerBits (PhotonD3PDObject, 1,
#                        D3PDMakerFlags.egammaL1TrigPattern())
#     defineTriggerBits (PhotonD3PDObject, 1,
#                        D3PDMakerFlags.PhotonL2TrigPattern())
#     defineTriggerBits (PhotonD3PDObject, 1,
#                        D3PDMakerFlags.PhotonEFTrigPattern())


#     ### Matching indices.
    
#     PhotonEFIndexAssoc = IndexAssociation(
#         PhotonD3PDObject,
#         egammaD3PDMaker.egammaEFPhotonTriggerObjectAssociationTool,
#         target = "trig_EF_ph_", prefix = "EF_", level = 1,
#         blockname = "EFIndex",
#         ChainPattern = D3PDMakerFlags.PhotonEFTrigPattern(),
#         MaxDR = 0.15,
#         allowMissing = True )
    
#     PhotonL2IndexAssoc = IndexAssociation(
#         PhotonD3PDObject,
#         egammaD3PDMaker.PhotonL2TriggerObjectAssociationTool,
#         target = "trig_L2_ph_", # For IndexAssociation.
#         prefix = "L2_", level = 1,
#         blockname = "L2Index",
#         ChainPattern = D3PDMakerFlags.PhotonL2TrigPattern(),
#         MaxDR = 0.15,
#         allowMissing = True )
    
#     PhotonL1IndexAssoc = IndexAssociation(
#         PhotonD3PDObject,
#         egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
#         target = "trig_L1_emtau_", prefix = "L1_", level = 1,
#         ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
#         MaxDR = 0.15,
#         blockname = "L1Index")
