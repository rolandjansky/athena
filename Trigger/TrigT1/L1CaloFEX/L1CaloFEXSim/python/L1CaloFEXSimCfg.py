# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def L1CaloFEXSimCfg(flags):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()

    if flags.Input.isMC:
      sCellType = "SCell"
    else:
      sCellType = "EmulatedSCell"
      from TrigT1CaloFexPerf.EmulationConfig import emulateSC_Cfg
      acc.merge(emulateSC_Cfg(flags,SCOut=sCellType))


    eFEX = CompFactory.LVL1.eFEXDriver('eFEXDriver',
        SCell=sCellType )
    eFEX.eSuperCellTowerMapperTool = CompFactory.LVL1.eSuperCellTowerMapper('eSuperCellTowerMapper', SCell=sCellType)
    eFEX.eFEXSysSimTool = CompFactory.LVL1.eFEXSysSim('eFEXSysSimTool', SCell=sCellType)
    acc.addEventAlgo(eFEX)

    # jFEX part
    jFEX = CompFactory.LVL1.jFEXDriver('jFEXDriver',
        SCell=sCellType )
    jFEX.jSuperCellTowerMapperTool = CompFactory.LVL1.jSuperCellTowerMapper('jSuperCellTowerMapper', SCell=sCellType)
    jFEX.jFEXSysSimTool = CompFactory.LVL1.jFEXSysSim('jFEXSysSimTool', SCell=sCellType)
    acc.addEventAlgo(jFEX)

    gFEX = CompFactory.LVL1.gFEXDriver('gFEXDriver',
        SCell=sCellType )
    gFEX.gSuperCellTowerMapperTool = CompFactory.LVL1.gSuperCellTowerMapper('gSuperCellTowerMapper', SCell=sCellType)
    gFEX.gFEXSysSimTool = CompFactory.LVL1.gFEXSysSim('gFEXSysSimTool', SCell=sCellType)
    acc.addEventAlgo(gFEX)

    return acc

if __name__ == '__main__':    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO.e4993_s3214_d1697_tid26494053_00/RDO.26494053._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.Scheduler.ShowDataDeps=True
    flags.Scheduler.CheckDependencies=True
    flags.Scheduler.ShowDataFlow=True
    flags.Trigger.enableL1MuonPhase1=True
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))

    from TrigT1CaloSim.TrigT1CaloSimRun2Config import L1CaloLegacySimCfg
    acc.merge(L1CaloLegacySimCfg(flags))

    acc.merge(L1CaloFEXSimCfg(flags))

    with open("L1Sim.pkl", "wb") as p:
        acc.store(p)
        p.close()

