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
import EventCommonD3PDMaker
from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
import JetTagD3PDMaker


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
                                                IndexedMoments=['contained'], IndexedMomentsSuffix = "_E" )
    # IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR','X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime', 'contained']
    ############# Muon #########
    IndJetMuonAssoc = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,'mu_',level = level,
                                            prefix='Muon_', blockname='muonblockindex', AssociationName='jetsubstructure_muons',
                                            IndexedMoments=['contained'], IndexedMomentsSuffix = "_U")
    # IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR', 'X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime','contained']

def AddTruthLeptonicInfo(object, typename='TruthParticle', target='mc_', level=0, extra_list_m=[]):
    import TruthD3PDMaker
    LeptAssoc = ContainedVectorMultiAssociation \
                (object,
                 JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,
                 'Lepton_',
                 blockname=typename+'Leptonblockindex',
                 level = level, AssociationName='jetsubstructure_truthleptons', IndexedMoments=['contained'] + extra_list_m, IndexedMomentsSuffix = "_L")
    genPart = SimpleAssociation(LeptAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool, blockname = 'TruthParticleGenParticleAssociationTool_AddTruthLeptonicInfo')
    trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool, blockname = 'GenParticleTruthParticleAssociationTool_AddTruthLeptonicInfo',
                                 target = 'mc_',
                                 Target = 'mc_')


def AddHadronicInfo(obj, additionalMoments = [], theblockname = 'SubjetMomentsHadronic', level=0):

   from JetD3PDMaker import JetMomentFillerTool
   obj.defineBlock(level, theblockname, JetMomentFillerTool,Moments= [] + additionalMoments)

def AddAssocJetsIndex(obj, jetassocname, jettarget, intermediate_names = [], level=0):
   
   JetAssocTrack = IndexMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,jettarget,level = level,
                                         prefix=jetassocname+'_',
                                         blockname=jetassocname+'blockindex', AssociationName = jetassocname , IntermediateAssociationNames=intermediate_names, OutputLevel=3 )

def AddConstitIndex(object, typename='CaloCluster', target='cl_lc_', level=0):
 
   import EventCommonD3PDMaker
   import D3PDMakerCoreComps
   ConstitAssoc = ContainedVectorMultiAssociation \
             (object,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_',
             blockname=typename+'NavigableConstituentAssociationTool',
             TypeName = typename, WriteWeight = True,
             level = level)

   ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, Target = target)

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
            
    genPart = SimpleAssociation(ConstitAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool)
    trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                                 target = 'mc_',
                                 Target = 'mc_')



def AddBTaggingInfo(obj, level = 0, block_prefix = "my"):

    obj.defineBlock(level,block_prefix+JetTagD3PDKeys.BTagWeightsBlockName(),
                              JetTagD3PDMaker.JetTagBTagWeightsFillerTool,
                              prefix=JetTagD3PDKeys.BTagWeightsPrefix(),
                              TaggerNames=JetTagD3PDFlags.Taggers())
    #addBTagInfoToJetObject(obj,0)

def AddBTaggingInfoFromSubjets(obj,  sj_assoc_name = 'SplitFiltSubjets', sj_prefix = "subjets", level=0):
    t = ContainedVectorMultiAssociation(obj, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,level = level, prefix= sj_prefix+"_",
                                        blockname='blockbtag'+sj_assoc_name, nrowName = 'nsj', OutputLevel=3, AssociationName=sj_assoc_name, IntermediateAssociationNames=['Parent'])

    #t.defineBlock(0, 'myKinematics', EventCommonD3PDMaker.FourMomFillerTool, WriteE  = True)
   
    AddBTaggingInfo(t, 0, "my"+sj_assoc_name)    

myJSD3PD = []

def CommonJSD3PD(jetsToWrite, alg):

    global myJSD3PD

    if not alg:
        raise TypeError("alg should be specified!")

    jsD3PD=None    
    for val in myJSD3PD:
        if val[0]==jetsToWrite:
            print "using existing JSD3PD"
            jsD3PD=val[1]
            break
        
    if jsD3PD==None:        
        jsD3PD = JSD3PD_Tool(jetsToWrite)
        myJSD3PD += [ (jetsToWrite, jsD3PD) ]

    jsD3PD.addToAlg(alg)
    

class JSD3PD_Tool(object):

    def __init__(self,jetsToWrite):

        self.commonExcludeList = [] #'El02Match','Mu02Match','L1Info','L2Info','EFInfo']
        self.commonIncludeList = ['El02Match','Mu02Match','L1Info','L2Info','EFInfo', 'SubjetMomentsHadronic']

        MyJetD3PDObject = getJetD3PDObject(objectname='MyJetD3PDObject')
        MyGroomedJetD3PDObject = getJetD3PDObject(objectname='MyGroomedJetD3PDObject')

        import re
        JetAlgPattern = re.compile(r'^(\D+)(\d{1,2})(\D+)Jets')  #assuming standard notation
        ars = JetAlgPattern.match(jetsToWrite[0]).groups()
    
        self.parentJets = ''.join(ars)  #remove the 'Jets' at the end
        #global self.parentJets
        self.groomedJetsList = jetsToWrite[1]

        ## Electron LC cluster overlap map
        if ('LCTopo' in self.parentJets):
            MyJetAssocD3PDObject = make_SGDataVector_D3PDObject('DataVector<INavigable4Momentum>', 'ElectronAODCollection_CaloCalTopoCluster_jetsubstructure_electrons', 'ElecTopoClusterAssoc_', 'myJetAssocD3PDObject')
            AddAssocConstitIndex(MyJetAssocD3PDObject)
            AddAssocConstitIndex(MyJetAssocD3PDObject, 'CaloCluster', 'cl_lc_')
            #alg += MyJetAssocD3PDObject(0)
            
        if 'Track' in self.parentJets:
            AddConstitIndex(MyGroomedJetD3PDObject, typename='Rec::TrackParticle', target='trk')
            AddConstitIndex(MyJetD3PDObject, typename='Rec::TrackParticle', target='trk')
            #AddLeptonicInfo(MyJetD3PDObject,level=0)
            #AddLeptonicInfo(MyGroomedJetD3PDObject,99)
        elif 'Topo' in self.parentJets:
            AddConstitIndex(MyGroomedJetD3PDObject)
            AddConstitIndex(MyJetD3PDObject)
            AddLeptonicInfo(MyJetD3PDObject, level=0)
            AddLeptonicInfo(MyGroomedJetD3PDObject,99)
            AddAssocJetsIndex(MyJetD3PDObject, 'TrackAssoc', 'trk')
            AddAssocJetsIndex(MyGroomedJetD3PDObject, 'TrackAssoc', 'trk')
        elif 'Truth' in self.parentJets:
            AddConstitTruthIndex(MyJetD3PDObject)
            AddConstitTruthIndex(MyGroomedJetD3PDObject)
            AddTruthLeptonicInfo(MyJetD3PDObject, level=0)
            AddTruthLeptonicInfo(MyGroomedJetD3PDObject, level=0)
             

        if 'Track' not in self.parentJets and 'Truth' not in self.parentJets:
            AddAssocJetsIndex(MyJetD3PDObject, 'TrackJets', 'jet_' + ars[0] + ars[1] + 'TrackZ_')
            if rec.doTruth():
               if self.parentJets == "AntiKt10LCTopo" or self.parentJets == "CamKt12LCTopo":
                   AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_')
               else :
                   AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_')
               #AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_')
        
        self.JetQualInclude = []

        if 'Topo' in self.parentJets or 'LCTopo' in self.parentJets:
            self.commonIncludeList += ['ConstituentScale', 'EMScale', 'EMFraction', 'JetSamplingsFrac'] #, 'El02Match', 'Mu02Match', 'L1Info', 'L2Info', 'EFInfo']
            AddHadronicInfo(MyJetD3PDObject, ['OriginIndex','nTrk','sumPtTrk','EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin'], theblockname = 'OriginIndexBlock')
            AddHadronicInfo(MyGroomedJetD3PDObject, ['EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin' ], theblockname = 'OriginIndexBlock')

            if (self.parentJets == 'AntiKt10LCTopo') or (self.parentJets == 'CamKt12LCTopo'):
                AddHadronicInfo(MyJetD3PDObject, ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ], theblockname='LCJESBlock')
                AddHadronicInfo(MyGroomedJetD3PDObject, ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ], theblockname='LCJESBlock')
                
        self.VtxInclude = []

        #if 'Topo' in self.parentJets:
        #    self.VtxInclude += ['JVtx','JetVertexFraction']
        self.VtxInclude += ['JVtx','JetVertexFraction']

        tmpParentJets = self.parentJets.replace("JS", "")    
        AddAssocJetsIndex(MyGroomedJetD3PDObject, 'Parent', 'jet_' + tmpParentJets + '_')
        for gr in self.groomedJetsList:
            AddAssocJetsIndex(MyJetD3PDObject, gr, 'jet_' + tmpParentJets + gr + '_')

        MyGroomedJetCollectionD3PDObject = getJetCollectionD3PDObject(objectname = "MyGroomedJetCollectionD3PDObject")
        AddUniqueMoments(MyGroomedJetCollectionD3PDObject,  ['SmallR','PtFrac', 'NSub'], theblockname = 'TrimConfigMoments', level=99)
        AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   ['NSubjets','ktycut2','massFraction','minRfilt','minSplitR'], theblockname = 'FiltConfigMoments', level = 0)
        
        AddHadronicInfo(MyGroomedJetD3PDObject, ['CORE_RBB','CORE_RFILT'], theblockname = 'FiltMoments', level=99)

        AddHadronicInfo(MyGroomedJetD3PDObject, ['Tau1','Tau2','Tau3', 'WIDTH', 'SPLIT12', 'SPLIT23', 'SPLIT34', 'ZCUT12', 'ZCUT23', 'ZCUT34','Dip12', 'Dip13', 'Dip23','DipExcl12','PlanarFlow','Angularity','ActiveArea', 'ActiveArea_px', 'ActiveArea_py', 'ActiveArea_pz', 'ActiveArea_e', 'VoronoiArea', 'QW', 'PullMag', 'PullPhi', 'Pull_C00', 'Pull_C01', 'Pull_C10', 'Pull_C11'], theblockname = 'jsObsblock' )
        AddHadronicInfo(MyJetD3PDObject, ['Tau1','Tau2','Tau3', 'WIDTH', 'SPLIT12', 'SPLIT23', 'SPLIT34', 'ZCUT12', 'ZCUT23', 'ZCUT34','Dip12', 'Dip13', 'Dip23','DipExcl12','PlanarFlow','Angularity','ActiveArea','ActiveArea_px', 'ActiveArea_py', 'ActiveArea_pz', 'ActiveArea_e','VoronoiArea', 'QW', 'PullMag', 'PullPhi', 'Pull_C00', 'Pull_C01', 'Pull_C10', 'Pull_C11'], theblockname = 'jsObsblock' )

        #add area corrected kinematics for the groomed jets
        from D3PDMakerConfig.CommonGroomedJetsConfig import getGroomedJetsConfig
        dictsConfig = getGroomedJetsConfig()
        from D3PDMakerConfig.CommonJSjets import buildName
        self.AreaCorr = []
        for dC in dictsConfig:
            for l in dC[1]:
                if 'AreaCorr' in l['args']:
                    if l['args']['AreaCorr']: self.AreaCorr.append(buildName(l))
        if (self.AreaCorr) and ((self.parentJets == 'AntiKt10LCTopo') or (self.parentJets == 'CamKt12LCTopo')):
            AddHadronicInfo(MyGroomedJetD3PDObject,  ['m_areacorrected', 'pt_areacorrected', 'phi_areacorrected','eta_areacorrected', 'm_voronoiareacorrected', 'pt_voronoiareacorrected', 'phi_voronoiareacorrected','eta_voronoiareacorrected', 'm_voronoighostareacorrected', 'pt_voronoighostareacorrected', 'phi_voronoighostareacorrected','eta_voronoighostareacorrected'], theblockname = 'AreaCorrectedMoments', level=99)

        ## in groomedJetsList, subjets name should follow the name of the 'full/composite' jet.
        for idx,gr in enumerate(self.groomedJetsList):
                ## names are unique
            if 'KtSubjets' in gr:
                #AddBTaggingInfoFromSubjets(MyGroomedJetD3PDObject, sj_assoc_name = gr, sj_prefix = gr, level = 99)
                AddAssocJetsIndex(MyGroomedJetD3PDObject, gr, 'jet_' + tmpParentJets + gr + '_', intermediate_names = ['Parent'], level=99)
            elif 'Subjets' in gr and idx != 0 and 'Subjets' not in self.groomedJetsList[idx-1]:
                #AddBTaggingInfoFromSubjets(MyGroomedJetD3PDObject, sj_assoc_name = gr, sj_prefix = gr, level = 99)
                AddAssocJetsIndex(MyGroomedJetD3PDObject, gr, 'jet_' + tmpParentJets + gr + '_', intermediate_names = ['Parent'], level=99)
                if 'LCTopo' in self.parentJets:
                    AddBTaggingInfoFromSubjets(MyGroomedJetD3PDObject, sj_assoc_name = gr, sj_prefix = gr, level = 99)
            elif 'Subjets' in gr:
                raise ValueError("Subjets without full composite jet ?") ## not suppose to happen

        self.MyJetD3PDObject = MyJetD3PDObject
        self.MyGroomedJetD3PDObject = MyGroomedJetD3PDObject
        self.MyGroomedJetCollectionD3PDObject = MyGroomedJetCollectionD3PDObject
        if ('LCTopo' in self.parentJets):
            self.MyJetAssocD3PDObject = MyJetAssocD3PDObject



    def addToAlg(self,alg):
        outputJetsName = self.parentJets
        if(self.parentJets.find("JS")>-1):
            #outputJetsName = self.parentJets.replace("JS", "")
            pass

        if ('LCTopo' in self.parentJets) and (not hasattr(alg, 'ElecTopoClusterAssoc_Filler')):
            alg += self.MyJetAssocD3PDObject(0)

        jet_suffix = 'Jets'
        if 'Subjets' in self.parentJets:
            jet_suffix = 'JetsReTagged'# replacing by NewJets
            #jet_suffix = 'NewJets'
        alg += self.MyJetD3PDObject(0, prefix='jet_' + outputJetsName.replace("JS", "") + '_', sgkey=self.parentJets+jet_suffix, exclude=self.commonExcludeList, include = self.commonIncludeList + self.VtxInclude + self.JetQualInclude + ['elecblockindex','muonblockindex', 'LCJESBlock', 'OriginIndexBlock'])
 
        for idx,gr in enumerate(self.groomedJetsList):

            kw = {}
            includelist = []
            includelist_unique = []
            
            if 'Trim' in gr and 'Subjets' not in gr:
                includelist_unique += ['TrimConfigMoments', 'blockindex']
                if [x for x in self.AreaCorr if x in gr]:
                    includelist += ['AreaCorrectedMoments']
                if len(self.groomedJetsList) > idx+1 and 'Subjets' in self.groomedJetsList[idx+1]:
                    includelist += [self.groomedJetsList[idx+1]+'blockindex']
                    #if 'LCTopo' in self.parentJets:
                    #    includelist += ['blockbtag'+self.groomedJetsList[idx+1]]
                
            elif 'Filt' in gr and 'Subjets' not in gr:
                includelist += ['FiltMoments']
                includelist_unique += ['FiltConfigMoments']
                #if len(self.groomedJetsList) > idx+1 and 'Subjets' in self.groomedJetsList[idx+1]:
                #    includelist += ['blockbtag'+self.groomedJetsList[idx+1], self.groomedJetsList[idx+1]+'blockindex']
                    
            elif 'Subjets' in gr:
                includelist += ['minSplitR']
                if 'LCTopo' in self.parentJets:
                    includelist += [JetTagD3PDKeys.TruthInfoBlockName(), JetTagD3PDKeys.BTagWeightsBlockName()]
                    #kw[JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']= "trk" 
                    #kw[JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk"


            alg += self.MyGroomedJetD3PDObject(0, prefix='jet_' + outputJetsName.replace("JS", "") + gr + '_', sgkey=self.parentJets + gr + 'Jets', include = includelist + self.commonIncludeList + ['OriginIndexBlock','FiltMoments','LCJESBlock','minSplitR', 'elecblockindex', 'muonblockindex'],exclude=self.commonExcludeList,**kw)
        
            alg += self.MyGroomedJetCollectionD3PDObject( 0, prefix='jet_' + outputJetsName.replace("JS", "") + gr + '_config_', sgkey=self.parentJets + gr + 'Jets', include = includelist_unique)

        return
