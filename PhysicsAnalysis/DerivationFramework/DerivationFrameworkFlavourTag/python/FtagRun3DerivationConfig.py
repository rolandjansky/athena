# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import findAllAlgorithms
from AthenaCommon.AthenaCommonFlags import jobproperties as jps

from GaudiKernel.Configurable import WARNING




def FtagJetCollection(jetcol, seq, OutputLevel=WARNING):
    

    jetcol_name_without_Jets = jetcol.replace('Jets','')

    from AthenaCommon.AppMgr import athCondSeq

    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
    from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
    from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
    from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
    from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags


    kwargs = {}
    kwargs['Release'] = '22'


    cfgFlags.Input.Files = jps.AthenaCommonFlags.FilesInput.get_Value()

    
    acc = ComponentAccumulator()


    taggerlist = ['IP2D', 'IP3D', 'SV1', 'SoftMu']

    
    CalibrationChannelAliases = ["AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo"]

    grades= cfgFlags.BTagging.Grades

    RNNIPConfig = {'rnnip':''}

    JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
    jettagcalibcondalg = "JetTagCalibCondAlg"
    readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
    connSchema = "GLOBAL_OFL"
    if not cfgFlags.Input.isMC:
        readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
        connSchema = "GLOBAL"
    histoskey = "JetTagCalibHistosKey"
    from IOVDbSvc.CondDB import conddb
      
    conddb.addFolder(connSchema, readkeycalibpath, className='CondAttrListCollection')
    JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = taggerlist, 
        channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)

    athCondSeq+=conf2toConfigurable( JetTagCalib, indent="  " )
    
    acc.merge(JetParticleAssociationAlgCfg(cfgFlags, jetcol_name_without_Jets, "InDetTrackParticles", 'BTagTrackToJetAssociator', **kwargs))

    SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
    for k, v in SecVertexingAndAssociators.items():

        acc.merge(JetSecVtxFindingAlgCfg(cfgFlags, jetcol_name_without_Jets, "PrimaryVertices", k, v))

        
        acc.merge(JetSecVertexingAlgCfg(cfgFlags, jetcol_name_without_Jets, "PrimaryVertices", k, v))

    
    acc.merge( JetBTaggingAlgCfg(cfgFlags, JetCollection = jetcol_name_without_Jets, PrimaryVertexCollectionName="PrimaryVertices", TaggerList = taggerlist, SVandAssoc = SecVertexingAndAssociators) )
    


    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
        #'BTagging/201903/smt/antikt4empflow/network.json',
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
        ]
    }

    acc.merge(BTagTrackAugmenterAlgCfg(cfgFlags))


    Analysis__BTagHighLevelAugmenterAlg=CompFactory.Analysis.BTagHighLevelAugmenterAlg
    
    options = {}
    options['JetCollectionName'] = jetcol
    options['BTaggingCollectionName'] =  'BTagging_'+jetcol_name_without_Jets
    options['JetLinkName'] = options['BTaggingCollectionName'] + '.jetLink'
    options['BTagTrackToJetAssociatorName'] = 'BTagTrackToJetAssociator'
    options['name'] = ( 'BTagging_'+jetcol_name_without_Jets+ '_Augment').lower()
    options['OutputLevel'] = OutputLevel

    acc.addEventAlgo(Analysis__BTagHighLevelAugmenterAlg(**options))


    Analysis__HighLevelBTagAlg=CompFactory.Analysis.HighLevelBTagAlg
    FlavorTagDiscriminants__DL2Tool=CompFactory.FlavorTagDiscriminants.DL2Tool

    for jsonfile in postTagDL2JetToTrainingMap[jetcol_name_without_Jets]:

        options = {}
        options['nnFile'] = jsonfile
        options['name'] = "decorator"
        dl2tool = FlavorTagDiscriminants__DL2Tool(**options)

        Name = jsonfile.replace("/", "_").replace("_network.json", "")

        options = {}
        options['BTaggingCollectionName'] = 'BTagging_'+jetcol_name_without_Jets
        options['JetDecorator'] = dl2tool
        options['name'] = Name.lower()

        acc.addEventAlgo(Analysis__HighLevelBTagAlg(**options))
    


    Configurable.configurableRun3Behavior=0


    algs = findAllAlgorithms(acc.getSequence("AthAlgSeq"))
    
    for alg in algs:

        seq += conf2toConfigurable(alg)

    acc.wasMerged()

    
    return






