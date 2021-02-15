# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetD3PDMaker.JetD3PDObject             import getJetD3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import JetSubstructureD3PDMaker
from D3PDMakerCoreComps.D3PDObject          import make_SG_D3PDObject
from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
import JetTagD3PDMaker
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags


from AthenaCommon.AlgSequence               import AlgSequence
topSequence = AlgSequence()

def getJetCollectionD3PDObject(objectname = 'JetCollectionD3PDObject', prefix = 'jet_'):
    return make_SG_D3PDObject('JetCollection','','jet_', objectname)


def AddUniqueMoments(obj, momlist, theblockname = 'ConfigParams', level = 0):
    obj.defineBlock(level, theblockname, 
                    JetSubstructureD3PDMaker.JetMomentUniqueFillerTool, 
                    Moments = momlist, OutputLevel=3)


def AddHadronicInfo(obj, additionalMoments = [], theblockname = 'SubjetMomentsHadronic', level=0):

   from JetD3PDMaker import JetMomentFillerTool
   obj.defineBlock(level, theblockname, JetMomentFillerTool,Moments= [] + additionalMoments)


def AddAssocJetsIndex(obj, jetassocname, jettarget, intermediate_names = [], level=0):
   
   _ = IndexMultiAssociation(obj,
                             JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,
                             jettarget,level = level,
                             prefix=jetassocname+'_',
                             blockname=jetassocname+'blockindex',
                             AssociationName = jetassocname ,
                             IntermediateAssociationNames=intermediate_names,
                             OutputLevel=3 )

def AddConstitIndex(object, typename='CaloCluster', target='cl_', myprefix='', level=0):
 
   import EventCommonD3PDMaker
   import D3PDMakerCoreComps
   ConstitAssoc = ContainedVectorMultiAssociation \
             (object,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_',
             blockname=typename+'NavigableConstituentAssociationTool',
             TypeName = typename, WriteWeight = True,
             level = level)

   ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, 
                             Target = target, prefix=myprefix)

def AddConstitTruthIndex(object, typename='TruthParticle', level=0):

    import EventCommonD3PDMaker
    import TruthD3PDMaker
    ConstitAssoc = ContainedVectorMultiAssociation \
            (object,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_',
             blockname=typename+'NavigableConstituentAssociationTool',
             TypeName = typename, WriteWeight = False,
             level = level)
           
    _ = IndexAssociation (ConstitAssoc,
                          TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                          TruthD3PDFlags.GenParticleAssocLabel(),
                          prefix='mcpart_')



def AddBTaggingInfo(obj, level = 0, block_prefix = ""):

    obj.defineBlock(level,block_prefix+JetTagD3PDKeys.BTagWeightsBlockName(),
                              JetTagD3PDMaker.JetTagBTagWeightsFillerTool,
                              prefix=JetTagD3PDKeys.BTagWeightsPrefix(),
                              TaggerNames=JetTagD3PDFlags.Taggers())
    #addBTagInfoToJetObject(obj,0)

def AddBTaggingInfoFromSubjets(obj,  sj_assoc_name = 'SplitFiltSubjets', sj_prefix = "subjets", level=0):

    t = ContainedVectorMultiAssociation(obj, 
                                        JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,
                                        level = level, prefix= sj_prefix+"_",
                                        blockname='blockbtag'+sj_assoc_name, 
                                        nrowName = 'nsj', OutputLevel=3, 
                                        AssociationName=sj_assoc_name, IntermediateAssociationNames=['Parent'])

    #t.defineBlock(0, 'myKinematics', EventCommonD3PDMaker.FourMomFillerTool, WriteE  = True)
   
    AddBTaggingInfo(t, 0, sj_assoc_name)   


class JSD3PD_Tool(object):

    def __init__(self,jetsToWrite):

        self.commonExcludeList = ['El02Match','Mu02Match','L1Info','L2Info','EFInfo']
        self.commonIncludeList = []

        MyJetD3PDObject = getJetD3PDObject(objectname='MyJetD3PDObject')
        MyGroomedJetD3PDObject = getJetD3PDObject(objectname='MyGroomedJetD3PDObject')
   
        import re
        JetAlgPattern = re.compile(r'^(\D+)(\d{1,2})(\D+)Jets')  #assuming standard notation
        ars = JetAlgPattern.match(jetsToWrite[0]).groups()
   
        self.parentJets = ''.join(ars)  #remove the 'Jets' at the end


        self.groomedJetsList = jetsToWrite[1]
           
        if 'Track' in self.parentJets and JetTagD3PDFlags.TrackAssocLabel()!= "":
            AddConstitIndex(MyGroomedJetD3PDObject, typename='Rec::TrackParticle', 
                            target=JetTagD3PDFlags.TrackAssocLabel())
            AddConstitIndex(MyJetD3PDObject, typename='Rec::TrackParticle', 
                            target=JetTagD3PDFlags.TrackAssocLabel())
        elif 'Topo' in self.parentJets and False:
            AddConstitIndex(MyGroomedJetD3PDObject)
            AddConstitIndex(MyJetD3PDObject)
        elif 'Truth' in self.parentJets:
            AddConstitTruthIndex(MyJetD3PDObject)
            AddConstitTruthIndex(MyGroomedJetD3PDObject)

        #if 'Track' not in self.parentJets and 'Truth' not in self.parentJets:
        #    AddAssocJetsIndex(MyJetD3PDObject, 'TrackJets', 'jet_' + ars[0] + ars[1] + 'TrackZ_')
        #    if rec.doTruth():
        #       AddAssocJetsIndex(MyJetD3PDObject, 'TruthJets', 'jet_' + ars[0] + ars[1] + 'Truth_')
       
        self.JetQualInclude = []

        if 'Topo' in self.parentJets:
            self.commonIncludeList += ['ConstituentScale', 'EMScale']
            AddHadronicInfo(MyJetD3PDObject, 
                            ['OriginIndex','nTrk','sumPtTrk','EtaOrigin', 'PhiOrigin', 'MOrigin'],
                            theblockname = 'OriginIndexBlock')
            AddHadronicInfo(MyGroomedJetD3PDObject, 
                            ['EtaOrigin', 'PhiOrigin', 'MOrigin' ],
                            theblockname = 'OriginIndexBlock')


            if (self.parentJets == 'AntiKt10LCTopo') or (self.parentJets == 'CamKt12LCTopo'):
                AddHadronicInfo(MyJetD3PDObject, 
                                ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ], 
                                theblockname='LCJESBlock')
                AddHadronicInfo(MyGroomedJetD3PDObject, 
                                ['LCJES', 'LCJES_EtaCorr', 'LCJES_MassCorr' ],
                                theblockname='LCJESBlock')
               
        self.VtxInclude = []

        if 'Topo' in self.parentJets:
            self.VtxInclude += ['JVtx','JetVertexFraction']
           
        AddAssocJetsIndex(MyGroomedJetD3PDObject, 'Parent', 'jet_' + self.parentJets + '_')
        for gr in self.groomedJetsList:
            AddAssocJetsIndex(MyJetD3PDObject, gr, 'jet_' + self.parentJets + gr + '_')

        MyGroomedJetCollectionD3PDObject = getJetCollectionD3PDObject(objectname = "MyGroomedJetCollectionD3PDObject")
        AddUniqueMoments(MyGroomedJetCollectionD3PDObject,  
                         ['SmallR','PtFrac', 'NSub'],
                         theblockname = 'TrimConfigMoments', level=99)
        AddUniqueMoments(MyGroomedJetCollectionD3PDObject,   
                         ['NSubjets','ktycut2','massFraction','minRfilt','minSplitR'], 
                         theblockname = 'FiltConfigMoments', level = 99)
       
        AddHadronicInfo(MyGroomedJetD3PDObject, ['CORE_RBB','CORE_RFILT'], 
                        theblockname = 'FiltMoments', level=99)

        ## in groomedJetsList, subjets name should follow the name of the 'full/composite' jet.
        for idx,gr in enumerate(self.groomedJetsList):
                ## names are unique
            if 'Subjets' in gr and idx != 0 and 'Subjets' not in self.groomedJetsList[idx-1]:
                AddBTaggingInfoFromSubjets(MyGroomedJetD3PDObject, 
                                           sj_assoc_name = gr, 
                                           sj_prefix = gr, 
                                           level = 99)
                AddAssocJetsIndex(MyGroomedJetD3PDObject, 
                                  gr, 
                                  'jet_' + self.parentJets + gr + '_', 
                                  intermediate_names = ['Parent'], 
                                  level=99)             
            elif 'Subjets' in gr:
                raise ValueError("Subjets without full composite jet ?") ## not suppose to happen

        self.MyJetD3PDObject = MyJetD3PDObject
        self.MyGroomedJetD3PDObject = MyGroomedJetD3PDObject
        self.MyGroomedJetCollectionD3PDObject = MyGroomedJetCollectionD3PDObject



    def addToAlg(self,alg):
               
        alg += self.MyJetD3PDObject(0, 
                                    prefix='jet_' + self.parentJets + '_', 
                                    sgkey=self.parentJets+'Jets',
                                    exclude=self.commonExcludeList, 
                                    include = self.commonIncludeList + self.VtxInclude + self.JetQualInclude)
 
        for idx,gr in enumerate(self.groomedJetsList):
                       
            kw = {}
            includelist = []
            includelist_unique = []
           
            if 'Trim' in gr:
                includelist_unique += ['TrimConfigMoments']
               
            elif 'Filt' in gr and 'Subjets' not in gr:
                includelist += ['FiltMoments']
                includelist_unique += ['FiltConfigMoments']
                if len(self.groomedJetsList) > idx+1 and 'Subjets' in self.groomedJetsList[idx+1]:
                    includelist += ['blockbtag'+self.groomedJetsList[idx+1], self.groomedJetsList[idx+1]+'blockindex']
                   
            elif 'Subjets' in gr:
                includelist += [JetTagD3PDKeys.BTagWeightsBlockName()]               
                includelist += [JetTagD3PDKeys.JetTrackAssocBlockName()] 
                includelist += [JetTagD3PDKeys.JetTrackBlockName()] 
                includelist += [JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()] 
                includelist += [JetTagD3PDKeys.IPInfoPlusTrackBlockName()] 
                if JetTagD3PDFlags.TrackAssocLabel()!= "":
                    kw[JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']=  JetTagD3PDFlags.TrackAssocLabel()
                    kw[JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']=  JetTagD3PDFlags.TrackAssocLabel()
               
            alg += self.MyGroomedJetD3PDObject(0, 
                                               prefix='jet_' + self.parentJets + gr + '_', 
                                               sgkey=self.parentJets + gr +'Jets', 
                                               include = includelist + self.commonIncludeList,
                                               exclude=self.commonExcludeList,**kw)
       
            alg += self.MyGroomedJetCollectionD3PDObject(0, 
                                                         prefix='jet_' + self.parentJets + gr + '_config_', 
                                                         sgkey=self.parentJets + gr +'Jets', 
                                                         include = includelist_unique)

        return
