# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from   AthenaCommon.Logging import logging

def CaloFwdTopoTowerCfg(flags,**kwargs):

    mlog = logging.getLogger('MakeFwdTopoTowerCfg')

    result=ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg

    result.merge(LArGMCfg(flags))
    result.merge(TileGMCfg(flags))


    result.addCondAlgo(CompFactory.CaloTowerGeometryCondAlg(TowerEtaBins = 100,
                                                            TowerEtaMin  = -5,
                                                            TowerEtaMax  =  5.,))


    kwargs.setdefault("PrepareLCW",flags.Calo.FwdTower.prepareLCW)
    kwargs.setdefault("TopoClusterRange",flags.Calo.FwdTower.clusterRange)
    kwargs.setdefault("BuildCombinedTopoSignal",True)

    #This property is for the algorithm, not for tool doing most of the work
    towerContainerKey=kwargs.pop('towerContainerKey','CaloCalFwdTopoTowers')

    #instatiate tool using kwargs
    towerBuilder=CompFactory.CaloTopoTowerFromClusterMaker(**kwargs)

    #Remember oderedByPt to (possibly) apply later:
    orderbyPt=towerBuilder.OrderClusterByPt

    if flags.Calo.FwdTower.prepareLCW:
        ## order by pt after LCW calibration!
        towerBuilder.OrderClusterByPt=False
                
    mlog.info('Consistency check')
    if towerBuilder.PrepareLCW and not towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] applying LCW requires to build topo-towers'.format(towerBuilder.name()))
    if towerBuilder.BuildCombinedTopoSignal and not towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] building combined topo-cluster/topo-tower signals requires to build topo-towers'.format(towerBuilder.name()))
    if towerBuilder.ApplyCellEnergyThreshold and towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] applying cell energy thresholds for topo-towers not yet implemented'.format(towerBuilder.name()))

    ''' Tower converter configuration summary
    '''
    if towerBuilder.BuildTopoTowers:
        if towerBuilder.PrepareLCW:
            ''' LCW topo-towers
            '''
            mlog.info('################################################')
            mlog.info('## Produce LCW calibrated topo-tower clusters ##')
            mlog.info('################################################')
            mlog.info('CaloTopoClusterContainerKey .. {0}'.format(towerBuilder.CaloTopoClusterContainerKey))
            #mlog.info('CellClusterWeightKey ......... {0}'.format(towerBuilder.CellClusterWeightKey))
        else:
            ''' EM topo-towers
            '''
            mlog.info('###############################################')
            mlog.info('## Produce EM calibrated topo-tower clusters ##')
            mlog.info('###############################################')
            mlog.info('CaloTopoClusterContainerKey .. {0}'.format(towerBuilder.CaloTopoClusterContainerKey))

        if towerBuilder.BuildCombinedTopoSignal:
            mlog.info(' ')
            mlog.info('Combined topo-cluster/topo-towermode with y_max = {0}'.format(towerBuilder.TopoClusterRange))
    else:    
        ''' EM towers
        '''
        mlog.info('########################################')
        mlog.info('## Produce EM standard tower clusters ##')
        mlog.info('########################################')

    #Configure moments maker (requires noise ...) 
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags, noisetype="totalNoise"))
   
    baseName=towerBuilder.name.replace("Builder","").replace("Maker","")
    from AthenaCommon.SystemOfUnits   import deg

    clusterMoments                  = CompFactory.CaloClusterMomentsMaker (baseName+'MomentMaker')
    clusterMoments.MaxAxisAngle     = 20*deg
    clusterMoments.TwoGaussianNoise = flags.Calo.TopoCluster.doTwoGaussianNoise
    clusterMoments.MinBadLArQuality = 4000
    clusterMoments.MomentsNames     = [
        # "CENTER_LAMBDA", 
        # "CENTER_MAG",
        "LONGITUDINAL",
        # "FIRST_ENG_DENS",
        # "ENG_FRAC_MAX",
        "ENG_FRAC_EM",
        # "PTD",
        "SIGNIFICANCE",
        # "ENG_POS",
    ]

    if not flags.Common.isOnline:
        from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
        result.merge(LArElecCalibDbCfg(flags,["HVScaleCorr"]))
        if flags.Input.isMC:
            clusterMoments.LArHVFraction=CompFactory.LArHVFraction(HVScaleCorrKey="LArHVScaleCorr")
        else:
            clusterMoments.LArHVFraction=CompFactory.LArHVFraction(HVScaleCorrKey="LArHVScaleCorrRecomputed")

        clusterMoments.MomentsNames       += ["ENG_BAD_HV_CELLS","N_BAD_HV_CELLS"]
       
    #instantiate Algorithm:
    towerMaker                    = CompFactory.CaloTopoTowerMaker(baseName+"Alg")
    towerMaker.TowersOutputName   = towerContainerKey
    towerMaker.TowerMakerTool     = towerBuilder

    from CaloBadChannelTool.CaloBadChanToolConfig import CaloBadChanToolCfg
    caloBadChanTool = result.popToolsAndMerge( CaloBadChanToolCfg(flags) )
    CaloClusterBadChannelList=CompFactory.CaloClusterBadChannelList
    BadChannelListCorr = CaloClusterBadChannelList(badChannelTool = caloBadChanTool)


    towerMaker.TowerCorrectionTools += [BadChannelListCorr,clusterMoments]
    
    if towerBuilder.PrepareLCW:
        ''' Configure name for calibration tool
        '''
        towerCalName    = baseName+'Calibrator'
        towerCalibrator = CompFactory.CaloTopoTowerFromClusterCalibrator(towerCalName)
        towerCalibrator.OrderClusterByPt = orderbyPt

    towerMaker.TowerCalibratorTool = towerCalibrator

    result.addEventAlgo(towerMaker)

    #merging
    caloTowerMerger                         = CompFactory.CaloTopoClusterTowerMerger("CaloTopoSignalMaker")
    caloTowerMerger.TopoClusterRange        = towerBuilder.TopoClusterRange                       #### caloTowerAlgo.CaloFwdTopoTowerBuilder.TopoClusterRange
    caloTowerMerger.TopoClusterContainerKey = towerBuilder.CaloTopoClusterContainerKey
    caloTowerMerger.TopoTowerContainerKey   = towerMaker.TowersOutputName
    caloTowerMerger.TopoSignalContainerKey  = 'CaloCalTopoSignals'
    
    result.addEventAlgo(caloTowerMerger)
    return result


if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]  
    ConfigFlags.Output.ESDFileName="esdOut.pool.root"

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    towerAcc=CaloFwdTopoTowerCfg(ConfigFlags,towerContainerKey="NewTowers",CaloTopoClusterContainerKey="CaloCalTopoClusters")

    cfg.merge(towerAcc)

    cfg.run(10)
