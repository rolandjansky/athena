
include.block( "MissingETGoodness/JetVarTool_jobOptions.py" )

###############################################

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
    from JetRecTools.JetRecToolsConf import *
    from JetRec.JetMomentGetter import make_JetMomentGetter
    from JetRec.JetGetters import *
    from JetRec.JetRecFlags import jetFlags

    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    cellcalibtool = getCellWeightTool('Kt', 0.4, 'Topo', onlyCellWeight=True)


    jetFlags.doJVF = False ##the following jet getter seems to fail if we don't deactivate the jet vertex finder.
    make_StandardJetGetter('AntiKt',0.4,'Topo',disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt4TopoJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo',disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt6TopoJets')

    atool4 = JetBadChanCorrTool("JetBadChanCorrTool_anti04",UpdateEvery=False,ConeDr=0.4,UseCalibScale=False)
    atool4.CellCalibrator = cellcalibtool
    atool6 = JetBadChanCorrTool("JetBadChanCorrTool_anti06",UpdateEvery=False,ConeDr=0.6,UseCalibScale=False)
    atool6.CellCalibrator = cellcalibtool

    cqual_tool = JetCaloQualityTool("CaloQualTool")
    cqual_tool.doSamplingBasedVariables=True
    cqual_tool.doCellBasedVariables=True

    BadCellCorrT = JetBadCellCorrTool("JetBadCellCorrTool")


    BadCellCorrT.CellCalibrator = cellcalibtool
    ##print BadCellCorrT
    ##toolist += [BadCellCorrT]
    
    make_JetMomentGetter("AntiKt4TopoJets",[atool4,cqual_tool,BadCellCorrT])
    make_JetMomentGetter("AntiKt6TopoJets",[atool6,cqual_tool,BadCellCorrT])

else:
    from AthenaCommon.Logging    import logging
    log = logging.getLogger ('MissingETGoodness')
    log.warning ('No cells; not rebuilding jets.')


###############################################

from MissingETGoodness.MissingETGoodnessConf import JetVarTool as ConfiguredJetVarTool
JetVarTool = ConfiguredJetVarTool(

    JetCollectionKey            =    'AntiKt4TopoJets',
    JetTruthCollectionKey       =    'Cone4TruthJets',
    TrackParticleContainerKey   =    'TrackParticleCandidate',

    )
ToolSvc += JetVarTool
print      JetVarTool


