#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaReFastAlgo
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from CaloTools.CaloLumiBCIDConfig import CaloLUMIBCIDToolCfg


def fastL2EgammaClusteringAlg( flags, roisKey="EMCaloRoIs"):
    acc = ComponentAccumulator()
    from AthenaCommon.Constants import DEBUG
    # configure calo data access
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import trigCaloDataAccessSvcCfg
    cdaSvcAcc = trigCaloDataAccessSvcCfg( flags )
    cdaSvc = cdaSvcAcc.getService("TrigCaloDataAccessSvc")
    acc.merge( cdaSvcAcc )

    from TileConditions.TileConditionsConfig import tileCondCfg
    acc.merge( tileCondCfg( flags ) )    

    acc.merge( CaloLUMIBCIDToolCfg( flags ) ) # this tool is required by the LArCellCont - 

    # configure tools (this can be simplified further,
    from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReEmEnFex, EgammaReHadEnFex, EgammaReSamp1Fex, EgammaReSamp2Fex
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import RingerFexConfig # use existing setup

    samp2 = EgammaReSamp2Fex(name='FaAlgoSamp2FexConfig', MaxDetaHotCell=0.15, MaxDphiHotCell=0.15)
    acc.addPublicTool( samp2 )

    samp1 = EgammaReSamp1Fex('FaAlgoSamp1FexConfig')
    acc.addPublicTool( samp1 )

    sampe = EgammaReEmEnFex('FaAlgoEmEnFexConfig')
    sampe.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]
    acc.addPublicTool( sampe )

    samph = EgammaReHadEnFex('FaAlgoHadEnFexConfig')
    samph.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]
    acc.addPublicTool( samph )

    #ring = RingerFexConfig('RingsMaker')
    #ring.OutputLevel=DEBUG
    #ring.RingsKey='CaloRings'
    #acc.addPublicTool( ring )

    __fex_tools = [ samp2, samp1, sampe, samph] #, ring ]
    for t in __fex_tools:
        t.TrigTimerSvc = ""
        t.trigDataAccessMT = cdaSvc # set data access svc
        t.OutputLevel = DEBUG

    
    from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaReFastAlgo
    alg = T2CaloEgammaReFastAlgo("FastEMCaloAlgo")    
    from TrigEDMConfig.TriggerEDMRun3 import recordable

    alg.IReAlgToolList = __fex_tools
    alg.ClustersName   = recordable('HLT_L2CaloEMClusters')
    alg.RoIs           = roisKey
    alg.EtaWidth       = 0.2
    alg.PhiWidth       = 0.2
    alg.OutputLevel = DEBUG

    # set calibration
    from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaSshapeCalibrationBarrelConfig, EgammaHitsCalibrationBarrelConfig, EgammaGapCalibrationConfig, EgammaTransitionRegionsConfig
    alg.CalibListBarrel  = [EgammaSshapeCalibrationBarrelConfig()]
    alg.CalibListBarrel += [EgammaHitsCalibrationBarrelConfig()]
    alg.CalibListBarrel += [EgammaGapCalibrationConfig()]
    alg.CalibListBarrel += [EgammaTransitionRegionsConfig()]

    from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaSshapeCalibrationEndcapConfig, EgammaHitsCalibrationEndcapConfig, EgammaGapCalibrationConfig
    alg.CalibListEndcap  = [EgammaSshapeCalibrationEndcapConfig()]
    alg.CalibListEndcap += [EgammaHitsCalibrationEndcapConfig()]
    alg.CalibListEndcap += [EgammaGapCalibrationConfig()]


    acc.addEventAlgo( alg ) 
    return acc
