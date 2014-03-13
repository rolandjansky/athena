# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.ObjKeyStore  import cfgKeyStore
from JetRecTools.JetRecToolsConf import *
from JetRec.JetGetters import *

def JetVarToolConfig (seq = AlgSequence):

    from RecExConfig.ObjKeyStore import cfgKeyStore
    if (cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo') and
        cfgKeyStore.isInInput ('CaloCellLinkContainer','CaloCalTopoCluster_Link') ):
                               
        from JetRec.JetMomentGetter import make_JetMomentGetter
        from JetRec.JetRecFlags import jetFlags

        from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
        cellcalibtool = getCellWeightTool('Kt', 0.4, 'Topo', onlyCellWeight=True)

        jetFlags.doJVF = False ##the following jet getter seems to fail if we don't deactivate the jet vertex finder.
        #make_StandardJetGetter('AntiKt',0.4,'Topo',disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt4TopoJets',seq=seq)
        #make_StandardJetGetter('AntiKt',0.6,'Topo',disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt6TopoJets',seq=seq)
	# MB: April 19, 2011, Suggested fix from SS
        make_StandardJetGetter('AntiKt',0.4,'Topo',disable=False, doCalib=True, calibName='EM:H1NumInv', outputCollectionName='AntiKt4TopoJets',seq=seq)
        make_StandardJetGetter('AntiKt',0.6,'Topo',disable=False, doCalib=True, calibName='EM:H1NumInv', outputCollectionName='AntiKt6TopoJets',seq=seq)

	# MB: April 7, 2011 : Suggested fix from SS
        from JetMomentTools.JetMomentToolsConf import \
             JetBadChanCorrTool, JetBadCellCorrTool, JetCaloQualityTool
        atool4 = JetBadChanCorrTool("JetBadChanCorrTool_anti04",ConeDr=0.4,UseCalibScale=False)
        atool4.CellCalibrator = cellcalibtool
        atool6 = JetBadChanCorrTool("JetBadChanCorrTool_anti06",ConeDr=0.6,UseCalibScale=False)
        atool6.CellCalibrator = cellcalibtool

        #atool4 = JetBadChanCorrTool("JetBadChanCorrTool_anti04",UpdateEvery=False,ConeDr=0.4,UseCalibScale=False)
        #atool4.CellCalibrator = cellcalibtool
        #atool6 = JetBadChanCorrTool("JetBadChanCorrTool_anti06",UpdateEvery=False,ConeDr=0.6,UseCalibScale=False)
        #atool6.CellCalibrator = cellcalibtool

        cqual_tool = JetCaloQualityTool("CaloQualTool")
        cqual_tool.doSamplingBasedVariables=True
        cqual_tool.doCellBasedVariables=True

        BadCellCorrT = JetBadCellCorrTool("JetBadCellCorrTool")


        BadCellCorrT.CellCalibrator = cellcalibtool
        ##print BadCellCorrT
        ##toolist += [BadCellCorrT] 

        make_JetMomentGetter("AntiKt4TopoJets",[atool4,cqual_tool,BadCellCorrT],
                             seq = seq)
        make_JetMomentGetter("AntiKt6TopoJets",[atool6,cqual_tool,BadCellCorrT],
                             seq = seq)
    else:
        from AthenaCommon.Logging    import logging
        log = logging.getLogger ('MissingETGoodness')
        log.warning ('No cells; not rebuilding jets.')


    ###############################################

    jet_collection_key = 'AntiKt4TopoJets'
    if cfgKeyStore.isInInput ('JetCollection', 'AntiKt4TopoEMJets'):
        jet_collection_key = 'AntiKt4TopoEMJets'

    truth_jet_collection_key = 'Cone4TruthJets'
    if cfgKeyStore.isInInput ('JetCollection', 'AntiKt4TruthJets'):
        truth_jet_collection_key = 'AntiKt4TruthJets'
    

    from MissingETGoodness.MissingETGoodnessConf import JetVarTool as ConfiguredJetVarTool
    JetVarTool = ConfiguredJetVarTool(

        JetCollectionKey            =    jet_collection_key,
        JetTruthCollectionKey       =    truth_jet_collection_key,
        TrackParticleContainerKey   =    'TrackParticleCandidate',

        )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += JetVarTool
    print      JetVarTool


