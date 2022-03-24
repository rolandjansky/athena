# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @brief this function sets up the top L1 simulation sequence 
##
## it covers the two cases of running L1 in the MC simulation and for rerunning on data 


def Lvl1SimulationCfg(flags, seqName = None):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    if seqName:
        from AthenaCommon.CFElements import parOR
        acc = ComponentAccumulator(sequence=parOR(seqName))
    else:
        acc = ComponentAccumulator()

    from AthenaCommon.CFElements import seqAND
    acc.addSequence(seqAND('L1SimSeq'))

    if flags.Trigger.enableL1CaloLegacy:
        acc.addSequence(seqAND('L1CaloLegacySimSeq'), parentName='L1SimSeq')
        from TrigT1CaloSim.TrigT1CaloSimRun2Config import L1CaloLegacySimCfg
        acc.merge(L1CaloLegacySimCfg(flags), sequenceName='L1CaloLegacySimSeq')

    acc.addSequence(seqAND('L1CaloSimSeq'), parentName='L1SimSeq')

    if flags.Trigger.enableL1CaloPhase1:
        from L1CaloFEXSim.L1CaloFEXSimCfg import L1CaloFEXSimCfg
        acc.merge(L1CaloFEXSimCfg(flags), sequenceName = 'L1CaloSimSeq')


    if flags.Trigger.enableL1MuonPhase1:
        acc.addSequence(seqAND('L1MuonSimSeq'), parentName='L1SimSeq')
        from TriggerJobOpts.Lvl1MuonSimulationConfig import Lvl1MuonSimulationCfg
        acc.merge(Lvl1MuonSimulationCfg(flags), sequenceName='L1MuonSimSeq')

    if flags.Trigger.L1.doTopo:
        acc.addSequence(seqAND('L1TopoSimSeq'), parentName='L1SimSeq')
        from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulationCfg
        acc.merge(L1TopoSimulationCfg(flags), sequenceName='L1TopoSimSeq')

        if flags.Trigger.enableL1CaloLegacy:
            acc.addSequence(seqAND('L1LegacyTopoSimSeq'), parentName='L1SimSeq')
            from L1TopoSimulation.L1TopoSimulationConfig import L1LegacyTopoSimulationCfg
            acc.merge(L1LegacyTopoSimulationCfg(flags), sequenceName='L1LegacyTopoSimSeq')

    
    acc.addSequence(seqAND('L1CTPSimSeq'), parentName='L1SimSeq')
    from TrigT1CTP.CTPSimulationConfig import CTPSimulationCfg
    acc.merge(CTPSimulationCfg(flags), sequenceName="L1CTPSimSeq")


    return acc

if __name__ == '__main__':
    import sys
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.Scheduler.ShowDataDeps=True
    flags.Scheduler.CheckDependencies=True
    flags.Scheduler.ShowDataFlow=True
    flags.Trigger.enableL1MuonPhase1=True
    flags.Trigger.triggerMenuSetup='Dev_pp_run3_v1'
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))

    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
    generateL1Menu(flags)

    acc.merge(Lvl1SimulationCfg(flags))
    from AthenaCommon.Constants import DEBUG
    acc.getEventAlgo("CTPSimulation").OutputLevel=DEBUG  # noqa: ATL900

    acc.printConfig(withDetails=True, summariseProps=True, printDefaults=True)
    with open("L1Sim.pkl", "wb") as p:
        acc.store(p)
        p.close()

    sys.exit(acc.run().isFailure())
