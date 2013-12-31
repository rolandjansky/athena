# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetD3PDMaker.JetD3PDObject             import getJetD3PDObject
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
from RecExConfig.RecFlags                   import rec
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import JetSubstructureD3PDMaker
from D3PDMakerCoreComps.D3PDObject          import make_SG_D3PDObject

from AthenaCommon.AlgSequence               import AlgSequence
topSequence = AlgSequence()


def getJetCollectionD3PDObject(objectname = 'JetCollectionD3PDObject', prefix =	'jet_'):
    return make_SG_D3PDObject('JetCollection','','jet_', objectname)


def AddUniqueMoments(obj, momlist, theblockname = 'ConfigParams', level = 0):
    obj.defineBlock(level, theblockname, JetSubstructureD3PDMaker.JetMomentUniqueFillerTool, Moments = momlist, OutputLevel=3)


def AddHadronicInfo(obj, additionalMoments = [], theblockname = 'SubjetMomentsHadronic', level=0):

   from JetD3PDMaker import JetMomentFillerTool
   obj.defineBlock(level, theblockname, JetMomentFillerTool,Moments= [] + additionalMoments)

def AddAssocJetsIndex(obj, jetassocname, jettarget, level=0):
   
   JetAssocTrack = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,jettarget,level = level,
                                         prefix=jetassocname+'_',
                                         blockname=jetassocname+'blockindex', AssociationName = jetassocname ,OutputLevel=3 )

def AddConstitIndex(object, typename='CaloCluster', target='cl_had_', level=0):
 
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

def AddConstitTruthIndex(object, typename='CaloCluster', target='mc_', level=0):

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



def JSD3PD  (jetsToWrite, alg = None):

    if not alg:
       raise TypeError("alg should be specified!")

    commonExcludeList = ['El02Match','Mu02Match','L1Info','L2Info','EFInfo']
    commonIncludeList = []

    MyJetD3PDObject = getJetD3PDObject(objectname='MyJetD3PDObject')
    MyGroomedJetD3PDObject = getJetD3PDObject(objectname='MyGroomedJetD3PDObject')

    import re
    JetAlgPattern = re.compile(r'^(\D+)(\d{1,2})(\D+)Jets')  #assuming standard notation
    ars = JetAlgPattern.match(jetsToWrite[0]).groups()

    parentJets = ''.join(ars)  #remove the 'Jets' at the end
    groomedJetsList = jetsToWrite[1]

    if 'Track' in parentJets:
        AddConstitIndex(MyGroomedJetD3PDObject, typename='Rec::TrackParticle', target='trk')
        AddConstitIndex(MyJetD3PDObject, typename='Rec::TrackParticle', target='trk')
    elif 'Topo' in parentJets:
        AddConstitIndex(MyGroomedJetD3PDObject)
        AddConstitIndex(MyJetD3PDObject)
    elif 'Truth' in parentJets:
        AddConstitTruthIndex(MyJetD3PDObject,typename='TruthParticle', target='mc_')
        AddConstitTruthIndex(MyGroomedJetD3PDObject,typename='TruthParticle', target='mc_')
      
    if 'Track' not in parentJets and 'Truth' not in parentJets:
        AddAssocJetsIndex(MyJetD3PDObject, 'TrackJets', 'jet_' + ars[0] + ars[1] + 'TrackZ_')
        if rec.doTruth():
           AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_')

    JetQualInclude = []

    if 'Topo' in parentJets:
       commonIncludeList += ['ConstituentScale', 'EMScale','EMFraction','JetSamplingsFrac']
       AddHadronicInfo(MyJetD3PDObject, ['OriginIndex','nTrk','sumPtTrk','EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin' ], theblockname = 'OriginIndexBlock')
       AddHadronicInfo(MyGroomedJetD3PDObject, ['EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin' ], theblockname = 'OriginIndexBlock')
       if ('LCTopo' in parentJets)  and  (ars[1] != '4'):
          AddHadronicInfo(MyJetD3PDObject, ['LCJES', 'LCJES_EtaCorr','LCJES_MassCorr' ], theblockname = 'FullLCJESBlock')
          AddHadronicInfo(MyGroomedJetD3PDObject,['LCJES', 'LCJES_EtaCorr','LCJES_MassCorr' ], theblockname = 'FullLCJESBlock')
       if ars[1] == '4': 
          AddHadronicInfo(MyJetD3PDObject, ['EMJES', 'EMJES_EtaCorr' ], theblockname='EMJESBlock', level=99)
          AddHadronicInfo(MyJetD3PDObject, ['LCJES', 'LCJES_EtaCorr' ], theblockname='LCJESBlock', level=99)
          AddHadronicInfo(MyJetD3PDObject, ['Timing', 'LArQuality', 'HECQuality', 'NegativeE', 'AverageLArQF'], theblockname= 'QMomentsBlock') 
          if ars[2] == 'TopoEM':   
             JetQualInclude += ['EMJESBlock', 'JetQual']
          elif ars[2] == 'LCTopo':
             JetQualInclude += ['LCJESBlock', 'JetQual']

    VtxInclude = []
    if 'Truth' not in parentJets:
       VtxInclude += ['JVtx', 'JetVertexFraction']

    AddAssocJetsIndex(MyGroomedJetD3PDObject, 'Parent', 'jet_' + parentJets + '_')
    for gr in groomedJetsList:
        AddAssocJetsIndex(MyJetD3PDObject, gr, 'jet_' + parentJets + gr + '_')

    MyGroomedJetCollectionD3PDObject = getJetCollectionD3PDObject(objectname = "MyGroomedJetCollectionD3PDObject")
    AddUniqueMoments(MyGroomedJetCollectionD3PDObject,  ['SmallR','PtFrac', 'NSub'], theblockname = 'TrimConfigMoments', level=99)
    AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   ['NSubjets','ktycut2','massFraction','minRfilt','minSplitR'], theblockname = 'FiltConfigMoments', level = 99)
    AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   ['RcutFactor', 'Zcut'], theblockname = 'PrunConfigMoments', level = 99)

    AddHadronicInfo(MyGroomedJetD3PDObject, ['CORE_RBB','CORE_RFILT'], theblockname = 'FiltMoments', level=99)

    AddHadronicInfo(MyGroomedJetD3PDObject, ['Tau1','Tau2','Tau3','WIDTH', 'SPLIT12', 'SPLIT23', 'SPLIT34', 'ZCUT12', 'ZCUT23', 'ZCUT34','Dip12', 'Dip13', 'Dip23','DipExcl12','ActiveArea', 'ActiveArea_px', 'ActiveArea_py', 'ActiveArea_pz', 'ActiveArea_e', 'VoronoiArea'], theblockname = 'jsObsblock' )
    AddHadronicInfo(MyJetD3PDObject, ['Tau1','Tau2','Tau3','WIDTH', 'SPLIT12', 'SPLIT23', 'SPLIT34', 'ZCUT12', 'ZCUT23', 'ZCUT34','Dip12', 'Dip13', 'Dip23','DipExcl12','ActiveArea','ActiveArea_px', 'ActiveArea_py', 'ActiveArea_pz', 'ActiveArea_e','VoronoiArea'], theblockname = 'jsObsblock' )    

    alg += MyJetD3PDObject(0, prefix='jet_' + parentJets + '_', sgkey=parentJets+'Jets', exclude=commonExcludeList, include = commonIncludeList + VtxInclude + JetQualInclude)
    for gr in groomedJetsList:
        includelist = []
        includelist_unique = []
        if 'Trim' in gr:
            includelist_unique += ['TrimConfigMoments']
        elif 'Filt' in gr:
            includelist += ['FiltMoments']
            includelist_unique += ['FiltConfigMoments']
        elif 'Prun' in gr:
            includelist_unique += ['PrunConfigMoments']

        alg += MyGroomedJetD3PDObject(0, prefix='jet_' + parentJets + gr + '_', sgkey=parentJets + gr +'Jets', include = includelist + commonIncludeList,exclude=commonExcludeList)
        alg += MyGroomedJetCollectionD3PDObject( 0, prefix='jet_' + parentJets + gr + '_config_', sgkey=parentJets + gr +'Jets', include = includelist_unique)

    return alg
