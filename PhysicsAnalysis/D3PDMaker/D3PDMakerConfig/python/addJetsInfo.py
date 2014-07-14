# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from RecExConfig.RecFlags                               import rec
from D3PDMakerConfig.CommonD3PDMakerFlags               import CommonD3PDMakerFlags
from QcdD3PDMaker.QcdD3PDMakerFlags                     import QcdD3PDMakerFlags
from JetTagD3PDMaker.JetTagD3PDMakerKeys                import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags               import JetTagD3PDFlags
from D3PDMakerConfig.D3PDMakerFlags                     import D3PDMakerFlags
from JetD3PDMaker.JetD3PDObject                         import JetD3PDObject
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation               import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation                import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation           import IndexMultiAssociation
from RecExConfig.ObjKeyStore                            import objKeyStore
from JetTagD3PDMaker.JetTagMSVVtxInJetD3PDObject        import getJetTagMSVVtxInfoInJetD3PDObject
from TruthD3PDMaker.TruthJetD3PDObject                  import TruthJetD3PDObject

from D3PDMakerConfig.CoreCommonD3PD                     import ExtendedJetD3PDObject
from JetTagD3PDMaker.JetTagTwoTrackVertexD3PDObject     import getJetTagTwoTrackVertexD3PDObject


def AddConstitIndex(object, typename='CaloCluster', target='cl_', level=0):
    import EventCommonD3PDMaker
    import D3PDMakerCoreComps
    ConstitAssoc = ContainedVectorMultiAssociation \
                   (object,
                    EventCommonD3PDMaker.NavigableConstituentAssociationTool,
                    'constit_',
                    blockname=typename+'NavigableConstituentAssociationTool',
                    TypeName = typename, WriteWeight = False,
                    level = level)
    ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, Target = target)

def AddConstitTruthIndex(object, typename='CaloCluster', target='cl_', level=0):
    import EventCommonD3PDMaker
    import D3PDMakerCoreComps
    import TruthD3PDMaker
    ConstitAssoc = ContainedVectorMultiAssociation \
                   (object,
                    EventCommonD3PDMaker.NavigableConstituentAssociationTool,
                    'constit_',
                    blockname=typename+'NavigableConstituentAssociationTool',
                    TypeName = typename, WriteWeight = False,
                    level = level)
    genPart = SimpleAssociation(ConstitAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool)
    trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                                 target = 'mc_',
                                 Target = 'mc_')

def AddTrackAssocIndex(obj, level=99):
    import JetTagD3PDMaker
    JetTrackAssoc = IndexMultiAssociation(obj, JetTagD3PDMaker.JetTagJetTrackAssociationTool, 'trk',
                                          prefix = 'TrackAssoc_',TrackAssocName="TrackAssoc",
                                          blockname='TrackAssocblockindex',
                                          level = level, FillVariables=False)


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw        


def addJetsInfo ( alg=None, file = 'physics.root', tuplename = 'physics', tmpFlag=True, **kw):

    print "adding Jets info..."

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(tuplename, file)

    flags=CommonD3PDMakerFlags

    #ExtendedJetD3PDObject = JetD3PDObject.copy()
        
    # add track index for track jets
    if not kw.has_key ('AntiKt4Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt4Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('AntiKt3Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt3Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('AntiKt2Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt2Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('Kt3Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['Kt3Track_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('AntiKt4TopoEMJets_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt4TopoEMJets_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('AntiKt4LCTopoJets_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt4LCTopoJets_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"
    if not kw.has_key ('AntiKt5LCTopo_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        kw['AntiKt5LCTopo_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']="trk"


    AddConstitIndex(ExtendedJetD3PDObject, typename='CaloCluster', target='cl_lc_', level=99)
    AddTrackAssocIndex(ExtendedJetD3PDObject, level=99)
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

    # Jet information
    jetInclude = flags.smallJetInclude()
    jetIncludeTrack = jetInclude[:]
    jetIncludeTrack += ['Rec::TrackParticleNavigableConstituentAssociationTool','WIDTH', 'DQMoments']
    jetIncludeTower = jetInclude[:]
    jetIncludeTower += ['JVtx','JetVertexFraction']
    jetInclude += ['Samplings',
                   'CaloClusterNavigableConstituentAssociationTool',
                   'TrackAssocblockindex',
                   'TracksMoments','Special','TrackMF','JetClusterMoment']
    jetIncludeLC = jetInclude[:]
    jetIncludeLC += ['ConstituentScale']
    jetIncludeALL = jetInclude[:]

    btagging_blocks = [JetTagD3PDKeys.BTagWeightsBlockName(), JetTagD3PDKeys.TruthInfoBlockName(), 
                       JetTagD3PDKeys.IP2DInfoBaseBlockName(), JetTagD3PDKeys.IP2DIPInfoBaseBlockName(),
                       JetTagD3PDKeys.IP3DInfoBaseBlockName(), JetTagD3PDKeys.IP3DIPInfoBaseBlockName(), JetTagD3PDKeys.JetFitterInfoBaseBlockName(),
                       JetTagD3PDKeys.JetFitterTagInfoBlockName(), JetTagD3PDKeys.SVInfoPlusBlockName(),JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
                       "JetTag_Gaia", JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName(), JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(), # "JetTag_GaiaNeg",]
                       JetTagD3PDKeys.JetJFTwoTrackVertexAssocBlockName(), JetTagD3PDKeys.JetFitterVxAssocBlockName(),
                       JetTagD3PDKeys.SV1InfoBaseBlockName(),JetTagD3PDKeys.SV0InfoPlusBlockName()]

    #jetIncludeALL += btagging_blocks
    jetIncludeALL += ['Uncertainties','BeamBackground','JetVertexFraction','CaloQual']

    jetIncludeALL_LC = jetIncludeALL[:]
    jetIncludeALL_LC += ['ConstituentScale']

    jetExclude = flags.smallJetExclude()[:]
    jetExclude += ['JES','JESMoments', 'Layer']
    
    alg += ExtendedJetD3PDObject              (**_args ( 3, 'AntiKt4TopoEMJets', kw,
                                                 #sgkey='AntiKt4TopoEMJetsReTagged', prefix='jet_AntiKt4TopoEM_',
                                                 sgkey='AntiKt4TopoEMNewJets', prefix='jet_AntiKt4TopoEM_',
                                                 include = jetIncludeALL+['SplitInfo','ConstituentScale']+btagging_blocks,
                                                 exclude = jetExclude,
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 #AssocTrackCont_target="trk",
                                                 JetTag_SoftMuonChi2InfoMuonAssoc_target="mu_staco_",        
                                                 allowMissing = True))

    #alg += ExtendedJetD3PDObject              (**_args ( 3, 'AntiKt6TopoEMJets', kw,
    #                                             sgkey='AntiKt6TopoEMNewJets', prefix='jet_AntiKt6TopoEM_',
    #                                             include = jetIncludeALL+['SplitInfo'],
    #                                             exclude = jetExclude,
    #                                             JetVertexFraction_FromUD=True,
    #                                             JetVertexFraction_FillFullJVF=True,
    #                                             allowMissing = True))

    alg += ExtendedJetD3PDObject              (**_args ( 3, 'AntiKt4LCTopoJets', kw,
                                                 sgkey='AntiKt4LCTopoNewJets', prefix='jet_AntiKt4LCTopo_',
                                                 include = jetIncludeALL_LC+['SplitInfo']+btagging_blocks,
                                                 exclude = jetExclude,
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 JetTag_SoftMuonChi2InfoMuonAssoc_target="mu_staco_",        
                                                 allowMissing = True))

    #alg += ExtendedJetD3PDObject              (**_args ( 3, 'AntiKt6LCTopoJets', kw,
    #                                             sgkey='AntiKt6LCTopoNewJets', prefix='jet_AntiKt6LCTopo_',
    #                                             include = jetIncludeALL_LC+['SplitInfo'],
    #                                             exclude = jetExclude,
    #                                             JetVertexFraction_FromUD=True,
    #                                             JetVertexFraction_FillFullJVF=True,
    #                                             allowMissing = True))

    alg += ExtendedJetD3PDObject              (**_args (2, 'JetElectronContainer90', kw,
                                                sgkey='JetElectronContainer90',
                                                prefix='eljet_',
                                                include=flags.eljetInclude(),
                                                exclude =['Constituents','ConstituentScale','JetShape', 'WIDTH', 'DQMoments', 'ActiveArea', 'OriginCorrection'],
                                                allowMissing = True))

    
    ### add the MSV block
    MSVJetCollections = []
    MSVJetCollections += ['AntiKt4TopoEMNewJets', 'AntiKt4LCTopoNewJets'] #, 'AntiKt6LCTopoNewJets', 'AntiKt6TopoEMNewJets']

    ### link multisv to tracks
    if not kw.has_key ('msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target'):
        kw['msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target']= "trk"
    #from JetTagD3PDMaker.JetTagMSVVtxInJetD3PDObject import getJetTagMSVVtxInfoInJetD3PDObject
    alg += getJetTagMSVVtxInfoInJetD3PDObject(**_args(1,"msvvtxinfo", kw,
                                                      jetCollections=MSVJetCollections ))

    # add TwoTrackVertex object
    alg += getJetTagTwoTrackVertexD3PDObject( level=0, jetCollections=MSVJetCollections )
            
    if rec.doTruth():
        AddConstitTruthIndex(ExtendedJetD3PDObject,typename='TruthParticle', target='mc_', level=99)

# below all for QCD group   

    #TopoJets (EM, GCW)

    # lowPt jets
    if flags.doRecJet() and flags.doTopoEMLowPtJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt4TopoEMLowPt', kw, prefix = 'jet_AntiKt4TopoEMLowPt_', sgkey = 'AntiKt4TopoEMLowPtJets',
                                      include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True,
                                      JetVertexFraction_FillFullJVF=True))
        
        #alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt6TopoEMLowPt', kw, prefix = 'jet_AntiKt6TopoEMLowPt_', sgkey = 'AntiKt6TopoEMLowPtJets',
        #                              include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True,
        #                              JetVertexFraction_FillFullJVF=True))

    # Tower Jets (EM)
    if flags.doTowerJet() and rec.readAOD():
        #alg += ExtendedJetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt6Tower', kw, prefix = 'jet_AntiKt6Tower_', sgkey = 'AntiKt6TowerAODJets',
        #                              include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        pass

    # additional: TowerEM
    if not rec.readAOD() and flags.doRecJet() and flags.doTowerEMJet():
        alg += ExtendedJetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt4TowerEM', kw, prefix = 'jet_AntiKt4TowerEMNew_', sgkey = 'AntiKt4TowerEMNewJets',
                                      include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        #alg += ExtendedJetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt6TowerEM', kw, prefix = 'jet_AntiKt6TowerEMNew_', sgkey = 'AntiKt6TowerEMNewJets',
        #                              include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        

    # additional: LCTopo addional R-sizes
    if flags.doRecJet() and flags.doLCTopoJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt5LCTopo', kw, prefix = 'jet_AntiKt5LCTopo_', sgkey = 'AntiKt5LCTopoNewJets',
                                      include = btagging_blocks,
                                      exclude = jetExclude+['DQMoments'], JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        #alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt7LCTopo', kw, prefix = 'jet_AntiKt7LCTopo_', sgkey = 'AntiKt7LCTopoNewJets',
        #                              exclude = jetExclude+['DQMoments'], JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt2LCTopo', kw, prefix = 'jet_AntiKt2LCTopo_', sgkey = 'AntiKt2LCTopoNewJets',
                                      exclude = jetExclude+['DQMoments'], JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt3LCTopo', kw, prefix = 'jet_AntiKt3LCTopo_', sgkey = 'AntiKt3LCTopoNewJets',
                                      exclude = jetExclude+['DQMoments'], JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    # additional: LCTopoLowPt
    if flags.doRecJet() and  flags.doLCTopoLowPtJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt4LCTopoLowPt', kw, prefix = 'jet_AntiKt4LCTopoLowPt_', sgkey = 'AntiKt4LCTopoLowPtJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

        #alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt6LCTopoLowPt', kw, prefix = 'jet_AntiKt6LCTopoLowPt_', sgkey = 'AntiKt6LCTopoLowPtJets',
        #                              include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    # additional: LCTopo CB
    if flags.doRecJet() and  flags.doLCTopoCBJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
        alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt4LCTopoCB', kw, prefix = 'jet_AntiKt4LCTopoCB_', sgkey = 'AntiKt4LCTopoCBJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

        #alg += ExtendedJetD3PDObject (**_args(flags.smallJetLevel(), 'AntiKt6LCTopoCB', kw, prefix = 'jet_AntiKt6LCTopoCB_', sgkey = 'AntiKt6LCTopoCBJets',
        #                              include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        

    # GhostJets
    if ((not flags.doRecJet()) or flags.doRecJetOnlyAdditional()) and rec.readAOD():
        if flags.doGhostJet():
            #alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt6GhostTower', kw, prefix = 'jet_AntiKt6GhostTower_', sgkey = 'AntiKt6GhostTowerAODJets',
            #                              include = jetIncludeTower, exclude = jetExclude))
            pass
    else:
        if not rec.readAOD() and flags.doRecJet() and flags.doGhostJet():
            alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt4GhostTowerNew', kw, prefix = 'jet_AntiKt4GhostTowerNew_', sgkey = 'AntiKt4GhostTowerNewJets',
                                          include = jetIncludeTower, exclude = jetExclude))

            #alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt6GhostTowerNew', kw, prefix = 'jet_AntiKt6GhostTowerNew_', sgkey = 'AntiKt6GhostTowerNewJets',
            #                              include = jetIncludeTower, exclude = jetExclude))

    # TrackJets

    trackjets_btag_include = btagging_blocks
    jetExcludeTrack = jetExclude[:]
    #jetExcludeTrack += ['JetSamplingsFrac','EMFraction','ActiveArea','JES','JESMoments','EMScale']
    
    AddConstitIndex(ExtendedJetD3PDObject, typename='Rec::TrackParticle', target='trk', level=99)
    if flags.doRecJet() and flags.doTrackJet():
        alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt4Track', kw, prefix = 'jet_AntiKt4Track_', sgkey = 'AntiKt4TrackJets',
                                      include = jetIncludeTrack+['SplitInfo', 'substructureblocks']+trackjets_btag_include, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        #alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt6Track', kw, prefix = 'jet_AntiKt6Track_', sgkey = 'AntiKt6TrackJets',
        #                              include = jetIncludeTrack, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        #alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt5Track', kw, prefix = 'jet_AntiKt5Track_', sgkey = 'AntiKt5TrackJets',
        #                              include = jetIncludeTrack, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        #alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt7Track', kw, prefix = 'jet_AntiKt7Track_', sgkey = 'AntiKt7TrackJets',
        #                              include = jetIncludeTrack, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt2Track', kw, prefix = 'jet_AntiKt2Track_', sgkey = 'AntiKt2TrackJets',
                                      include = jetIncludeTrack+trackjets_btag_include, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        alg += ExtendedJetD3PDObject (**_args(0, 'AntiKt3Track', kw, prefix = 'jet_AntiKt3Track_', sgkey = 'AntiKt3TrackJets',
                                      include = jetIncludeTrack+trackjets_btag_include, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))

        alg += ExtendedJetD3PDObject (**_args(0, 'Kt3Track', kw, prefix = 'jet_Kt3Track_', sgkey = 'Kt3TrackJets',
                                      include = jetIncludeTrack+trackjets_btag_include, exclude = jetExcludeTrack, JetVertexFraction_FromUD=False, JetVertexFraction_FillFullJVF=False))
                                      

    # Truth
    if rec.doTruth():
        D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)
        #if ( stdTruthParticleContainer != 'None'):
        #    alg += TruthParticleD3PDObject (**_args(1, 'TruthParticle', kw, sgkey = stdTruthParticleContainer, prefix='mcQCD_', exclude=['child_TruthParticleChildAssociation','parent_TruthParticleParentAssociation']))
        truthjetInclude = flags.truthJetInclude()
        truthjetInclude += ['TruthParticleNavigableConstituentAssociationTool',
                            'WIDTH'
                            ]
        
        if flags.doTruthJet():
            if flags.doRecTruthJet():
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthNew', kw, sgkey='AntiKt4TruthNewJets', prefix='AntiKt4Truth_',
                                              include=truthjetInclude+[JetTagD3PDKeys.TruthInfoBlockName()], exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthNew', kw, sgkey='AntiKt6TruthNewJets', prefix='AntiKt6Truth_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithNu', kw, sgkey='AntiKt4TruthWithNuJets', prefix='AntiKt4TruthWithNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithNu', kw, sgkey='AntiKt6TruthWithNuJets', prefix='AntiKt6TruthWithNu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithMuNu', kw, sgkey='AntiKt4TruthWithMuNuJets', prefix='AntiKt4TruthWithMuNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithMuNu', kw, sgkey='AntiKt6TruthWithMuNuJets', prefix='AntiKt6TruthWithMuNu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithMu', kw, sgkey='AntiKt4TruthWithMuJets', prefix='AntiKt4TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithMu', kw, sgkey='AntiKt6TruthWithMuJets', prefix='AntiKt6TruthWithMu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))

                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt5TruthNew', kw, sgkey='AntiKt5TruthNewJets', prefix='AntiKt5Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt7TruthNew', kw, sgkey='AntiKt7TruthNewJets', prefix='AntiKt7Truth_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt5TruthWithNu', kw, sgkey='AntiKt5TruthWithNuJets', prefix='AntiKt5TruthWithNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt7TruthWithNu', kw, sgkey='AntiKt7TruthWithNuJets', prefix='AntiKt7TruthWithNu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt5TruthWithMuNu', kw, sgkey='AntiKt5TruthWithMuNuJets', prefix='AntiKt5TruthWithMuNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt7TruthWithMuNu', kw, sgkey='AntiKt7TruthWithMuNuJets', prefix='AntiKt7TruthWithMuNu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt5TruthWithMu', kw, sgkey='AntiKt5TruthWithMuJets', prefix='AntiKt5TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                #alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt7TruthWithMu', kw, sgkey='AntiKt7TruthWithMuJets', prefix='AntiKt7TruthWithMu_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))

                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt2TruthNew', kw, sgkey='AntiKt2TruthNewJets', prefix='AntiKt2Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt3TruthNew', kw, sgkey='AntiKt3TruthNewJets', prefix='AntiKt3Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt2TruthWithNu', kw, sgkey='AntiKt2TruthWithNuJets', prefix='AntiKt2TruthWithNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt3TruthWithNu', kw, sgkey='AntiKt3TruthWithNuJets', prefix='AntiKt3TruthWithNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt2TruthWithMuNu', kw, sgkey='AntiKt2TruthWithMuNuJets', prefix='AntiKt2TruthWithMuNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt3TruthWithMuNu', kw, sgkey='AntiKt3TruthWithMuNuJets', prefix='AntiKt3TruthWithMuNu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt2TruthWithMu', kw, sgkey='AntiKt2TruthWithMuJets', prefix='AntiKt2TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt3TruthWithMu', kw, sgkey='AntiKt3TruthWithMuJets', prefix='AntiKt3TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
 
                # truth WZ jets
                alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJetWZ', kw,
                                                         sgkey='AntiKt4TruthJets_WZ', prefix='jet_AntiKt4TruthWZ_',
                                                         allowMissing = True))

        if flags.doTruthPileupJet():
            if objKeyStore.isInInput("JetCollection","InTimeAntiKt4TruthJets"):
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'InTimeAntiKt4Truth', kw, sgkey='InTimeAntiKt4TruthJets', prefix='InTimeAntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
            if objKeyStore.isInInput("JetCollection","OutOfTimeAntiKt4TruthJets"):
                alg += ExtendedJetD3PDObject (**_args(flags.truthJetLevel(), 'OutOfTimeAntiKt4Truth', kw, sgkey='OutOfTimeAntiKt4TruthJets', prefix='OutOfTimeAntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
            pass
        
                

    return alg
