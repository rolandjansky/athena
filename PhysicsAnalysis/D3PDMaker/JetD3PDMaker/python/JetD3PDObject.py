# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# import JetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
# import BackgroundD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject, \
                                          make_SGDataVector_D3PDObject
# from D3PDMakerCoreComps.release_version import at_least_version
# from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
# from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
# from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
# from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
# from EventCommonD3PDMaker.DRAssociation import DRAssociation

# from JetD3PDAnalysis.JetVertexFractionConfig import JetVertexFractionConfig
# from JetD3PDMaker import JetD3PDMakerConf
# from JetRecTools.JetRecToolsConf import *
# from JetMomentTools.JetMomentToolsConf import *
# from RecExConfig.RecFlags import rec
# from AthenaCommon.AlgSequence import AlgSequence
# from JetRec import JetRecConf
# from JetRecTools import JetRecToolsConf
# from JetTagD3PDMaker.AddBTagD3PDInfo import addBTagInfoToJetObject
# from AthenaCommon.Logging import logging



# #
# # Useful options:
# #  Specify ConstitIndex_Target to fill a list of indices of jet constituents.
# #  Target should be target collection for the association.
# #
# #  Specify JetVertexFraction_FromUD=True to get JVF information
# #  from UserData rather than from jet moments.
# #
# #  Specify JetVertexFraction_FillFullJVF=True to fill JVF info for
# #  all vertices (and force taking JVF from UserData).
# #



# # Reconstruction versions prior to 15.6.8 didn't fill in the jet timing/quality.
# # Fix up such jets here.
# def _fixJetTiming (c, **kw):
#     if at_least_version ('15.6.8'): return

#     sgkey_in = c.Getter.SGKey
#     try:
#         sgkey_in = resolveSGKey ('JetCollection', sgkey_in)
#     except:
#         if not kw.get('allowMissing'): raise
#         return
#     sgkey_out = sgkey_in + '_timing'

#     seq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
#     if not hasattr (seq, sgkey_out):
#         tools = []
#         caloqual_kw = {'doCellBasedVariables' : True,
#                        'doConstituentBasedVariables' : True,
#                        'doSamplingBasedVariables' : True,
#                        'cutOnTile' : 254,
#                        'cutOnLAr' : 4000,
#                        'doTileQuality' : True,
#                        'doLArQuality' : True}

#         if hasattr (JetRecToolsConf, 'JetTimeQualityTool'):
#             # rel 15 setup
#             tool1 = JetRecToolsConf.JetTimeQualityTool (sgkey_out + 'TimeTool',
#                                                         DoTime = False,
#                                                         DoQuality = False,
#                                                         UseCells = False)
#             if rec.readESD() or rec.readRDO():
#                 tool1.DoTime    = True
#                 tool1.DoQuality = True
#                 tool1.UseCells  = True
#             if rec.readAOD() and (sgkey_in.find ('H1Topo') >= 0 or
#                                   sgkey_in.find ('LCTopo') >= 0):
#                 tool1.DoTime    = True
#             tools.append (tool1)
#         else:
#             # rel 16
#             caloqual_kw['doTiming'] = True

#         tool2 = JetCaloQualityTool (sgkey_out + 'QualTool',
#                                     **caloqual_kw)
#         tools.append (tool2)

#         alg = JetD3PDMaker.JetFixupAlg (sgkey_out,
#                                         InputKey = sgkey_in,
#                                         OutputKey = sgkey_out,
#                                         Tools = tools)
#         for t in tools:
#             alg += t

#         seq += alg

#     c.Getter.SGKey = sgkey_out
#     return


# def _getprop (c, n):
#     return getattr (c, n, c.getDefaultProperty(n))

# def _jetVertexAlgHook (c, prefix, sgkey, typeName,
#                        seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
#                        *args, **kw):
#     jvf = getattr(c, c.getName() + '_JetVertexFraction', None)
#     if jvf :setattr(jvf,'FillFullJVF',True) # impose FillFullJVF in this case
#     if jvf and (_getprop (jvf, 'FromUD') or _getprop (jvf, 'FillFullJVF')):
#         JetVertexFractionConfig (seq = seq,
#                                  prefix = prefix,
#                                  # nb. Use this rather than the sgkey
#                                  # passed in; _fixJetTiming may
#                                  # have changed it.
#                                  sgkey = c.Getter.SGKey,
#                                  typeName = typeName)


#     return


# # Don't make this unless target has been specified.
# def _constitAssocLevel (reqlev, args):
#     if reqlev < 1: return False
#     if not args.has_key ('Target') or not args['Target']: return False
#     return True


# def _jetMomentsHook (c, prefix, sgkey, typeName,
#                      seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
#                      *args, **kw):

#     from JetD3PDAnalysis.JetMomentCalcConfig import JetMomentCalcConfig

#     # Always turn this on if we're making jets --- it changes jvf results.
#     from JetRec.JetRecFlags import jetFlags
#     if hasattr(jetFlags, 'jetPerformanceJob'):
#                jetFlags.jetPerformanceJob = True

#     if hasattr (c, c.name() + '_Special'):
#         JetMomentCalcConfig (c.Getter.SGKey, 'all', seq=seq)
#     else:
#         key = resolveSGKey ('JetCollection', c.Getter.SGKey)
#         if hasattr (c, c.name() + '_ActiveArea'):
#             JetMomentCalcConfig (c.Getter.SGKey, None, seq=seq)
#         if hasattr (c, c.name() + '_TruthMF') and 'Topo'  in key:
#             JetMomentCalcConfig (c.Getter.SGKey, ['truthMF'], seq=seq)
#         if hasattr (c, c.name() + '_TracksMoments') and 'Topo'  in key:
#             JetMomentCalcConfig (c.Getter.SGKey, ['trackMoments'], seq=seq)
#     if hasattr (c, c.name() + '_OriginCorrection'):
#         JetMomentCalcConfig (c.Getter.SGKey, ['origin'], seq=seq)
#     return

# def _jetClusterMomentsHook (c, prefix, sgkey, typeName,
#                      seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
#                      *args, **kw):

#     if hasattr (c, c.name() + '_JetClusterMoment'):
#         from JetRec.JetMomentGetter import make_JetMomentGetter
#         try:
#             from JetMomentTools.JetMomentToolsConf import JetMomentsFromClusters
#         except ImportError:
#             # JetMomentsFromClusters available only in AtlasPhysics, not
#             # in AtlasProduction or development.
#             log = logging.getLogger ('JetD3PDObject')
#             log.warn ('JetMomentsFromClusters not available; skipping.')
#             return
#         jt = JetMomentsFromClusters(ClusterMoments=["CENTER_LAMBDA","SECOND_R","SECOND_LAMBDA"],
#                                     ClusterPt=True)
#         make_JetMomentGetter(resolveSGKey ('JetCollection', c.Getter.SGKey),
#                              [jt])

#     return

def getJetD3PDObject(objectname='JetD3PDObject', prefix='jet_', btagleveloffset=7):


    object = make_SGDataVector_D3PDObject ('DataVector<xAOD::Jet_v1>',
                                           D3PDMakerFlags.JetSGKey(),
                                           prefix, objectname)

#     object.defineHook (_fixJetTiming)
#     object.defineHook (_jetVertexAlgHook)
#     object.defineHook (_jetMomentsHook)
#     object.defineHook (_jetClusterMomentsHook)

    object.defineBlock(0, 'Kinematics',
                       EventCommonD3PDMaker.FourMomFillerTool,
                       WriteE  = True)

#     object.defineBlock(1, 'OriginCorrection',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=['EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin'
#                                        ])

#     object.defineBlock(1, 'WIDTH',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['WIDTH'
#                                 ])

    object.defineBlock (
        1, 'DQMoments',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['n90 = N90Cells < int: 0',
                'Timing < float: 0',
                'LArQuality < float: 0',
                'OriginIndex < float: 0',
                'HECQuality < float: 0',
                'NegativeE < float: 0',
                'AverageLArQF < float: 0',
                'BCH_CORR_CELL = BchCorrCell < float:0',
                'BCH_CORR_DOTX = BchCorrDotx < int:0',
                'BCH_CORR_JET  = BchCorrJet < int:0',
                'BCH_CORR_JET_FORCELL = BchCorrJetForCell < int:0',
                'NumTowers < float: 0',
                'ootFracCells5 < float: 0',
                'ootFracCells10 < float: 0',
                'ootFracClusters5 < float: 0',
                'ootFracClusters10 < float: 0',
        ])
  
#     object.defineBlock(1, 'JetSamplingsFrac',
#                               JetD3PDMaker.JetSamplingsFracFillerTool)
#     object.defineBlock(1, 'JetQual',
#                               JetD3PDMaker.JetCaloUtilsFillerTool)
    object.defineBlock (
        1, 'JetQual',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['isBadLoose < int: 0',
                'isBadMedium < int: 0',
                'isBadTight < int: 0',
                'isUgly < int: 0',
                'hecf = HECFrac < float: 0',
                ])
#     object.defineBlock(1, 'EMFraction',
#                               JetD3PDMaker.JetEMFractionFillerTool)
  
#     object.defineBlock(1, 'JES',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=[ 'Offset', 'EMJES', 'EMJES_EtaCorr','EMJESnooffset' ])
  
#     object.defineBlock(1, 'JESMoments',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=['LCJES', 'LCJES_EtaCorr' ])
  
#     object.defineBlock(1, 'EMScale',
#                               JetD3PDMaker.JetSignalStateFillerTool,
#                               WriteE = True,
#                               SignalStateNumber = 0,
#                               SignalStatePrefix = 'emscale')
  
#     object.defineBlock(1, 'ActiveArea',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['ActiveArea', 'ActiveAreaPx', 'ActiveAreaPy', 'ActiveAreaPz', 'ActiveAreaE'])

#     # Including JetVertexFraction should also include JVtx and TruthMF,
#     # unless explicitly excluded.
#     def _jvfLOD (reqlev, args, hookargs):
#         if reqlev < 3: return False
#         if not 'JVtx' in hookargs['exclude']:
#             hookargs['include'].append ('JVtx')
#         if not 'TruthMF' in hookargs['exclude']:
#             hookargs['include'].append ('TruthMF')
#         return True
#     try:
#         # tracksAvailableForJets not defined in every release. Temporarily put it in
#         # a try / except block
#         from JetRec.TrackSelectionForJets import tracksAvailableForJets
#         dotrackVars = tracksAvailableForJets()
#     except:
#         dotrackVars = True
#     if dotrackVars:
#         object.defineBlock(_jvfLOD, 'JetVertexFraction',
#                            JetD3PDMaker.JetVertexFractionFillerTool,
#                            UDPrefix=D3PDMakerFlags.JetUserDataPrefix(),
#                            )
#         object.defineBlock(3, 'JVtx',
#                            JetD3PDMaker.JetVtxFillerTool)
    
#     object.defineBlock(3, 'TruthMF',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['TruthMFindex','TruthMF'])
  
#     object.defineBlock(3, 'Layer',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=['GSCFactorF','WidthFraction'])
  
#     object.defineBlock(4, 'Samplings',
#                               JetD3PDMaker.JetSamplingsFillerTool)
  
#     object.defineBlock(4, 'JetShape',
#                               JetD3PDMaker.JetShapeFillerTool)
  
  
#     object.defineBlock(4, 'Constituents',
#                               JetD3PDMaker.JetConstituentFillerTool,
#                               SignalStateNumber = -1,
#                               SignalStatePrefix = 'default')
  
#     object.defineBlock(4, 'ConstituentScale',
#                               JetD3PDMaker.JetSignalStateFillerTool,
#                               WriteE = True,
#                               SignalStateNumber = 2,
#                               SignalStatePrefix = 'constscale')
  
#     object.defineBlock(5, 'JetLArHVMoment',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=['LArBadHVEnergy','LArBadHVRatio'])

#     object.defineBlock(9, 'JetClusterMoment',
#                               JetD3PDMaker.JetMomentFillerTool,
#                               Moments=['CENTER_LAMBDA','SECOND_R','SECOND_LAMBDA',
#                                        'CLUSTER_PT'])

#     addBTagInfoToJetObject(object,btagleveloffset)

#     # Track association cannot be done unless there are tracks.
#     # As often this is not the case track association will be done only
#     # if this is explicitly asked for. by defining it in the include list.
#     #
#     # You can also enable it by passing an argument to JetD3PDObject
#     # like:
#     #   AssocTrackCont_target='mytarget_'
#     def _jetAssocLevel (reqlev, args):
#         return args.has_key ('target') or (reqlev >= 999)
#     IndexMultiAssociation (object,
#                            JetD3PDMaker.FlavorAssociationTool,
#                            'trk',
#                            prefix = 'flavor_assoctrk_',
#                            level = _jetAssocLevel,
#                            blockname = 'AssocTrackCont')

#     object.defineBlock(10, 'JetRoIword',
#                               JetD3PDMaker.JetROIWordFillerTool
#                               )


#     #JetElAssoc = DRAssociation(object,"ElectronContainer",D3PDMakerFlags.ElectronSGKey(),0.2,prefix='el_',matched="matched",blockname="El02Match")

#     #JetMuAssoc = DRAssociation(object,"Analysis::MuonContainer",D3PDMakerFlags.MuonSGKey(),0.2,prefix='mu_',matched="matched",blockname="Mu02Match")

# #JetTrkAssoc = DRAssociation(object,"Rec::TrackParticleContainer",D3PDMakerFlags.TrackSGKey(),0.4,prefix='trk_',matched="matched",blockname="Trk04Match")

# #---------------- Trigger Object ----------------------------

#     #if D3PDMakerFlags.DoTrigger():
#     if False:
#         JetL1Assoc = SimpleAssociation(object,
#                                        JetD3PDMaker.JetL1TriggerObjectAssociationTool,
#                                        matched = 'matched',
#                                        blockname = 'L1Info',
#                                        prefix = 'L1_',
#                                        MaxDR = 0.6,
#                                        ChainPattern = 'L1_J.*')

#         JetL1Assoc.defineBlock (99, 'L1Kinematics',
#                                 EventCommonD3PDMaker.FourMomFillerTool,
#                                 WriteE = True,
#                                 WriteEt = False,
#                                 WriteM = False)

#         JetL2Assoc = SimpleAssociation(object,
#                                        JetD3PDMaker.JetL2TriggerObjectAssociationTool,
#                                        matched = 'matched',
#                                        blockname = 'L2Info',
#                                        prefix = 'L2_',
#                                        MaxDR = 0.25,
#                                        ChainPattern = 'L2_j.*')

#         JetL2Assoc.defineBlock (99, 'L2Kinematics',
#                                 EventCommonD3PDMaker.FourMomFillerTool,
#                                 WriteE = True,
#                                 WriteEt = False,
#                                 WriteM = False)

#         JetEFAssoc = SimpleAssociation(object,
#                                        JetD3PDMaker.JetEFTriggerObjectAssociationTool,
#                                        matched = 'matched',
#                                        blockname = 'EFInfo',
#                                        prefix = 'EF_',
#                                        MaxDR = 0.25,
#                                        ChainPattern = 'EF_j.*')

#         JetEFAssoc.defineBlock (99, 'EFKinematics',
#                                 EventCommonD3PDMaker.FourMomFillerTool,
#                                 WriteE = True,
#                                 WriteEt = False,
#                                 WriteM = False)
#         pass

#     ConstitAssoc = ContainedVectorMultiAssociation \
#         (object,
#          #JetD3PDMaker.JetConstituentAssociationTool,
#          EventCommonD3PDMaker.NavigableConstituentAssociationTool,
#          'constit_',
#          TypeName = 'CaloCluster',
#          nrowName = '',
#          level = 1)
  

#     ConstitAssoc.defineBlock (_constitAssocLevel, 'ConstitIndex',
#                               D3PDMakerCoreComps.IndexFillerTool,
#                               Target = '')


#     # Beam Background Identification Method
#     object.defineBlock( 999, 'BeamBackground',
#                         BackgroundD3PDMaker.BeamBackgroundJetFillerTool)

# #---------------- special jet moments ----------------------------

#     object.defineBlock(999, 'TrackMF',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['TrackMFindex','TrackMF'])

#     object.defineBlock(999, 'TracksMoments',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['nTrk_pv0_1GeV', 'sumPtTrk_pv0_1GeV', 'nTrk_allpv_1GeV', 'sumPtTrk_allpv_1GeV',
#                                 'nTrk_pv0_500MeV', 'sumPtTrk_pv0_500MeV',
#                                 'trackWIDTH_pv0_1GeV','trackWIDTH_allpv_1GeV'
#                                 ])
  
#     object.defineBlock(999, 'JetLabel',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['JetLabel'])

#     object.defineBlock(999, 'Special',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['LikeLihood_0',
#                                 'LowEtConstituentsFrac','KtDr',
#                                 ])

#     object.defineBlock(999, 'VoronoiArea',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['VoronoiArea', 'VoronoiAreaPx', 'VoronoiAreaPy', 'VoronoiAreaPz', 'VoronoiAreaE',
#                                 ])

#     object.defineBlock(999, 'Isolation',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['IsoKR20Perp', 'IsoKR20Par', 'IsoKR20SumPt', 'IsoDelta2Perp', 'IsoDelta2Par',
#                                 'IsoDelta2SumPt', 'IsoFixedCone8Perp', 'IsoFixedCone8Par', 'IsoFixedCone8SumPt',
#                                 'IsoFixedArea13Perp', 'IsoFixedArea13Par', 'IsoFixedArea13SumPt',
#                                 'Iso6To88Perp', 'Iso6To88Par', 'Iso6To88SumPt',
#                                 ])

#     object.defineBlock(999, 'SplitInfo',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['SPLIT12', 'SPLIT23', 'SPLIT34'])

#     object.defineBlock(999, 'Uncertainties',
#                        JetD3PDMaker.JetMomentFillerTool,
#                        Moments=['SmearingFactor'])

    return object




JetD3PDObject = getJetD3PDObject()
