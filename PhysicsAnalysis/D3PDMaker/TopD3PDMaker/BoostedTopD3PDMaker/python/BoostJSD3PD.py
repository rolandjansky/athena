# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetD3PDMaker.JetD3PDObject             import getJetD3PDObject
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
from RecExConfig.RecFlags                   import rec
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import JetSubstructureD3PDMaker
from D3PDMakerCoreComps.D3PDObject          import make_SG_D3PDObject, make_SGDataVector_D3PDObject

from AthenaCommon.AlgSequence               import AlgSequence
topSequence = AlgSequence()


def getJetCollectionD3PDObject(objectname = 'JetCollectionD3PDObject', prefix =	'jet_'):
    return make_SG_D3PDObject('JetCollection','','jet_', objectname)


def AddUniqueMoments(obj, momlist, theblockname = 'ConfigParams', level = 0):
    obj.defineBlock(level, theblockname, JetSubstructureD3PDMaker.JetMomentUniqueFillerTool, Moments = momlist, OutputLevel=3)

def AddLeptonicInfo(obj, level = 0):

   ############ Electron #####
   IndJetElectronAssoc = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,'el_',level = level,
                                               prefix='Electron_', blockname='elecblockindex', AssociationName='jetsubstructure_electrons',
                                               IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR','X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime', 'contained'], IndexedMomentsSuffix = "_E" )

   ############# Muon #########
   IndJetMuonAssoc = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,'mu_',level = level,
                                           prefix='Muon_', blockname='muonblockindex', AssociationName='jetsubstructure_muons',
                                           IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR', 'X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime','contained'], IndexedMomentsSuffix = "_U")


def AddHadronicInfo(obj, additionalMoments = [], theblockname = 'SubjetMomentsHadronic', level=0):

   from JetD3PDMaker import JetMomentFillerTool
   obj.defineBlock(level, theblockname, JetMomentFillerTool,Moments= [] + additionalMoments)

def AddAssocJetsIndex(obj, jetassocname, jettarget, level=0):
   
   JetAssocTrack = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,jettarget,level = level,
                                         prefix=jetassocname+'_',
                                         blockname=jetassocname+'blockindex', AssociationName = jetassocname ,OutputLevel=3 )

def AddConstitIndex(object, typename='CaloCluster', target='cl_', level=0):
 
   import EventCommonD3PDMaker
   import D3PDMakerCoreComps
   ConstitAssoc = ContainedVectorMultiAssociation \
             (object,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_',
             blockname=typename+'NavigableConstituentAssociationTool',
             TypeName = typename,
             level = level)

   ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, Target = target)

def AddConstitTruthIndex(object, typename='TruthParticle', target='mc_', level=0):

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

   genPart = SimpleAssociation(ConstitAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool, blockname = 'TruthParticleGenParticleAssociationTool_AddConstitTruthIndex')
   trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool, blockname = 'GenParticleTruthParticleAssociationTool_AddConstitTruthIndex',
                                          target = 'mc_',
                                          Target = 'mc_')


def AddTruthLeptonicInfo(object, typename='TruthParticle', target='mc_', level=0, extra_list_m=[]):
   import TruthD3PDMaker
   LeptAssoc = ContainedVectorMultiAssociation \
             (object,
             JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,
             'Lepton_',
             blockname=typename+'Leptonblockindex',
             level = level, AssociationName='jetsubstructure_truthleptons', IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR','X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime', 'contained'] + extra_list_m, IndexedMomentsSuffix = "_L")
 
   genPart = SimpleAssociation(LeptAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool, blockname = 'TruthParticleGenParticleAssociationTool_AddTruthLeptonicInfo')
   trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool, blockname = 'GenParticleTruthParticleAssociationTool_AddTruthLeptonicInfo',
                                          target = 'mc_',
                                          Target = 'mc_')

def AddAssocConstitIndex(object, typename='Analysis::Electron', target='el_', level=0):
 
   import EventCommonD3PDMaker
   import D3PDMakerCoreComps
   ConstitAssoc = ContainedVectorMultiAssociation \
             (object,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_'+target,
             blockname=typename+'NavigableConstituentAssociationTool',
             TypeName = typename, WriteWeight = True,
             level = level)

   ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, Target = target) #, ContainerIndexName = 'contidx')


class myLOD:

  jetprefix = 'jet_'

  def __init__(self, jetprefix):
    self.jetprefix = jetprefix

  def _prefixLOD (self, reqlev, blockargs, hookargs):
    #print 'yoman: ', self.jetprefix, ' ', hookargs['prefix'] 
    return hookargs['prefix'] in [self.jetprefix]

def JSD3PD  (jetsToWrite, alg = None, scheduleOnly=False, level = 10,
             file = 'js.root',
             tuplename = 'js'):

    if not alg:
       from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
       alg = MSMgr.NewRootStream(tuplename, file, asAlg=True)

    commonExcludeList = ['El02Match','Mu02Match','L1Info','L2Info','EFInfo']
    commonIncludeList = []

    import re
    JetAlgPattern = re.compile(r'^(\D+)(\d{1,2})(\D+)Jets')  #assuming standard notation
    ars = JetAlgPattern.match(jetsToWrite[0]).groups()

    parentJets = ''.join(ars)  #remove the 'Jets' at the end
    groomedJetsList = jetsToWrite[1]

    parentJetsWithoutJS = parentJets 
    if parentJets[-2:] == 'JS': parentJetsWithoutJS = parentJets[:-2] 

    parentJetsWithoutJSForGrooming = parentJetsWithoutJS
    if scheduleOnly: parentJetsWithoutJS += "Jets"
    filteredSequence = AlgSequence('TopWGFilter')

    theZeroLevel = 0
    if not scheduleOnly:
      MyJetD3PDObject = getJetD3PDObject(objectname='MyJetD3PDObject')
      MyJetD3PDObject._hookArgs['seq']=filteredSequence
    else:
      from TopInputsD3PDMaker.topInputsD3PD import JetD3PDObject
      MyJetD3PDObject = JetD3PDObject
      mylod = myLOD('jet_'+parentJetsWithoutJS+'_')
      theZeroLevel = mylod._prefixLOD 

    MyGroomedJetD3PDObject = getJetD3PDObject(objectname='MyGroomedJetD3PDObject')
    MyGroomedJetD3PDObject._hookArgs['seq']=filteredSequence

    ## Electron LC cluster overlap map
    if ('LCTopo' in parentJets) and (not hasattr(alg, 'ElecTopoClusterAssoc_Filler')):
      MyJetAssocD3PDObject = make_SGDataVector_D3PDObject('DataVector<INavigable4Momentum>', 'ElectronAODCollection_CaloCalTopoCluster_jetsubstructure_electrons', 'ElecTopoClusterAssoc_', 'myJetAssocD3PDObject')
      AddAssocConstitIndex(MyJetAssocD3PDObject)
      AddAssocConstitIndex(MyJetAssocD3PDObject, 'CaloCluster', 'cl_')
      alg += MyJetAssocD3PDObject(0)


    if 'Track' in parentJets:
        AddConstitIndex(MyGroomedJetD3PDObject, typename='Rec::TrackParticle', target='trk')
        AddConstitIndex(MyJetD3PDObject, typename='Rec::TrackParticle', target='trk', level=theZeroLevel)
        AddLeptonicInfo(MyJetD3PDObject,level=theZeroLevel)
        AddLeptonicInfo(MyGroomedJetD3PDObject,99)
    elif 'Topo' in parentJets:
        AddConstitIndex(MyGroomedJetD3PDObject)
        AddConstitIndex(MyJetD3PDObject, level=theZeroLevel)
        AddLeptonicInfo(MyJetD3PDObject, level=theZeroLevel)
        AddLeptonicInfo(MyGroomedJetD3PDObject,99)
    elif 'Truth' in parentJets:
        if ars[1] == '4':
           from JetD3PDMaker.JetD3PDMakerConf import D3PD__JetShapeFillerTool
           from JetD3PDMaker.JetD3PDMakerConf import D3PD__JetTrueTagFillerTool
           MyJetD3PDObject.defineBlock(theZeroLevel, 'myJetShape', D3PD__JetShapeFillerTool)
           MyJetD3PDObject.defineBlock(theZeroLevel, 'TrueFlavorComponents', D3PD__JetTrueTagFillerTool)
        AddConstitTruthIndex(MyJetD3PDObject, level=theZeroLevel)
        AddTruthLeptonicInfo(MyJetD3PDObject, level=theZeroLevel)
        AddTruthLeptonicInfo(MyGroomedJetD3PDObject, level=99)
        AddConstitTruthIndex(MyGroomedJetD3PDObject)
      
    if 'Track' not in parentJets and 'Truth' not in parentJets:
        AddAssocJetsIndex(MyJetD3PDObject, 'TrackJets', 'jet_' + ars[0] + ars[1] + 'TrackZ_', level=theZeroLevel)
        if rec.doTruth():
           AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_', level=theZeroLevel)

    JetQualInclude = []

    if 'Topo' in parentJets:
       commonIncludeList += ['ConstituentScale', 'EMScale']
       if not scheduleOnly:
          AddHadronicInfo(MyJetD3PDObject, ['OriginIndex','nTrk','sumPtTrk','EtaOrigin'  , 'PhiOrigin', 'MOrigin' ], theblockname = 'OriginIndexBlock')
       AddHadronicInfo(MyGroomedJetD3PDObject, ['EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin' ], theblockname = 'OriginIndexBlock', level=99)
       if (parentJetsWithoutJS == 'AntiKt10LCTopo') or (parentJetsWithoutJS == 'CamKt12LCTopo'):
          AddHadronicInfo(MyJetD3PDObject, ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ], theblockname='LCJESBlock', level=theZeroLevel)
          AddHadronicInfo(MyGroomedJetD3PDObject, ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ], theblockname='LCJESBlock')
       if ars[1] == '4' or ars[1] == '6':
          if  not scheduleOnly:
             AddHadronicInfo(MyJetD3PDObject, ['EMJES', 'EMJES_EtaCorr' ], theblockname='EMJESBlock', level=99)
             AddHadronicInfo(MyJetD3PDObject, ['LCJES', 'LCJES_EtaCorr' ], theblockname='LCJESBlock', level=99)
             AddHadronicInfo(MyJetD3PDObject, ['Timing', 'LArQuality', 'HECQuality', 'NegativeE', 'AverageLArQF'], theblockname= 'QMomentsBlock') 
          if 'TopoEM' in ars[2]:   
             JetQualInclude += ['EMJESBlock', 'JetQual']
          elif 'LCTopo' in ars[2]:
             JetQualInclude += ['LCJESBlock', 'JetQual']

    VtxInclude = []
    if 'Topo' in parentJets:
       VtxInclude += ['JVtx', 'JetVertexFraction']
    elif 'Track' in parentJets:
       VtxInclude += ['JVtx']

    AddAssocJetsIndex(MyGroomedJetD3PDObject, 'Parent', 'jet_' + parentJetsWithoutJS + '_')
    for gr in groomedJetsList:
        AddAssocJetsIndex(MyJetD3PDObject, gr, 'jet_' + parentJetsWithoutJSForGrooming + gr + '_', level=theZeroLevel)

    MyGroomedJetCollectionD3PDObject = getJetCollectionD3PDObject(objectname = "MyGroomedJetCollectionD3PDObject")
    AddUniqueMoments(MyGroomedJetCollectionD3PDObject,  ['SmallR','PtFrac'], theblockname = 'TrimConfigMoments', level=99)
    #AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   ['NSubjets','ktycut2','massFraction','minRfilt','minSplitR'], theblockname = 'FiltConfigMoments', level = 99)
    #AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   ['RcutFactor', 'Zcut'], theblockname = 'PrunConfigMoments', level = 99)

    AddHadronicInfo(MyGroomedJetD3PDObject, ['CORE_RBB','CORE_RFILT'], theblockname = 'FiltMoments', level=99)

    listOfMoments = ['Tau1','Tau2','Tau3','SPLIT12', 'SPLIT23', 'SPLIT34', 'ZCUT12', 'ZCUT23', 'ZCUT34','QW', 'PullMag', 'PullPhi', 'Pull_C00', 'Pull_C01', 'Pull_C10', 'Pull_C11','WIDTH']
    
    if not scheduleOnly: listOfMoments_ = listOfMoments
    else: listOfMoments_  = listOfMoments[:-1]  # without WIDTH

    AddHadronicInfo(MyGroomedJetD3PDObject, listOfMoments, theblockname = 'jsObsblock', level=99 )
    AddHadronicInfo(MyJetD3PDObject, listOfMoments_, theblockname = 'jsObsblock', level=theZeroLevel )    
   
    if not scheduleOnly:
        alg += MyJetD3PDObject(0,  prefix='jet_' + parentJetsWithoutJS + '_', sgkey=parentJets+'Jets', exclude=commonExcludeList, include = commonIncludeList + VtxInclude + JetQualInclude)
    for gr in groomedJetsList:
        includelist = []
        includelist_unique = []
        if 'Trim' in gr:
            includelist_unique += ['TrimConfigMoments']
            includelist += ['jsObsblock']
            if 'Topo' in parentJets:
              includelist += ['OriginIndexBlock', 'EMScale','ConstituentScale','elecblockindex','muonblockindex']
            elif 'Truth' in parentJets:
              includelist += ['TruthParticleLeptonblockindex']
            elif 'Track' in parentJets:
              includelist += ['elecblockindex','muonblockindex']

            alg += MyGroomedJetCollectionD3PDObject( 0, prefix='jet_' + parentJetsWithoutJSForGrooming + gr + '_config_', sgkey=parentJets + gr +'Jets', include = includelist_unique)
        elif 'Filt' in gr:
            includelist += ['FiltMoments', 'jsObsblock']
            #includelist_unique += ['FiltConfigMoments']
        elif 'Prun' in gr:
            #includelist_unique += ['PrunConfigMoments']
            includelist	+= ['jsObsblock']

        alg += MyGroomedJetD3PDObject(0, prefix='jet_' + parentJetsWithoutJSForGrooming + gr + '_', sgkey=parentJets + gr +'Jets', include = includelist,exclude=commonExcludeList)

    return alg
