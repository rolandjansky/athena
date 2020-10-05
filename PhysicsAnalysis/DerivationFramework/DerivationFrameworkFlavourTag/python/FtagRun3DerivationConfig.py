# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import findAllAlgorithms
from AthenaCommon.AthenaCommonFlags import jobproperties as jps

from GaudiKernel.Configurable import WARNING




def FtagJetCollection(jetcol, seq, OutputLevel=WARNING):
    

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
    from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
    from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags


    jetcol_name_without_Jets = jetcol.replace('Jets','')
    BTaggingCollection = cfgFlags.BTagging.OutputFiles.Prefix + jetcol_name_without_Jets

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
    
        acc.merge(JetSecVertexingAlgCfg(cfgFlags, BTaggingCollection, jetcol_name_without_Jets, "PrimaryVertices", k, v))

    
    acc.merge( JetBTaggingAlgCfg(cfgFlags, BTaggingCollection = BTaggingCollection, JetCollection = jetcol_name_without_Jets, PrimaryVertexCollectionName="PrimaryVertices", TaggerList = taggerlist, SVandAssoc = SecVertexingAndAssociators) )
    


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

    acc.merge(BTagHighLevelAugmenterAlgCfg(cfgFlags, JetCollection=jetcol_name_without_Jets, BTagCollection=BTaggingCollection, Associator='BTagTrackToJetAssociator'))

    for jsonfile in postTagDL2JetToTrainingMap[jetcol_name_without_Jets]:
        acc.merge(HighLevelBTagAlgCfg(cfgFlags, BTaggingCollection=BTaggingCollection, TrackCollection='InDetTrackParticles', NNFile=jsonfile) )


    Configurable.configurableRun3Behavior=0


    algs = findAllAlgorithms(acc.getSequence("AthAlgSeq"))
    
    for alg in algs:

        seq += conf2toConfigurable(alg)

    acc.wasMerged()

    
    return






