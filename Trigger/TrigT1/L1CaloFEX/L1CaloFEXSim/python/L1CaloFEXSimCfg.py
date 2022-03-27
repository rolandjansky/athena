#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ReadSCellFromPoolFileCfg(flags, key='SCell'):
    '''Configure reading SCell container from a Pool file like RDO or ESD'''
    acc = ComponentAccumulator()

    # Ensure SCell container is in the input file
    assert key in flags.Input.Collections or not flags.Input.Collections, 'MC input file is required to contain SCell container'

    # Need geometry and conditions for the SCell converter from POOL
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    return acc


def ReadSCellFromByteStreamCfg(flags, key='SCell'):
    acc=ComponentAccumulator()

    # Geometry, conditions and cabling setup
    from TileGeoModel.TileGMConfig import TileGMCfg
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from LArCabling.LArCablingConfig import LArLATOMEMappingCfg
    acc.merge(TileGMCfg(flags))
    acc.merge(LArGMCfg(flags))
    acc.merge(LArLATOMEMappingCfg(flags))

    # Conversion from ByteStream to LArRawSCContainer
    decoderTool = CompFactory.LArLATOMEDecoder('LArLATOMEDecoder', ProtectSourceId = True)
    decoderAlg = CompFactory.LArRawSCDataReadingAlg('LArRawSCDataReadingAlg', LATOMEDecoder=decoderTool)
    acc.addEventAlgo(decoderAlg)

    # Conversion from LArRawSCContainer to SCell
    scellAlg = CompFactory.LArRAWtoSuperCell('LArRAWtoSuperCell', SCellContainerOut=key)
    acc.addEventAlgo(scellAlg)

    return acc


def TriggerTowersInputCfg(flags):
    '''Configuration to provide TriggerTowers as input to the Fex simulation'''
    if flags.Input.isMC:
        # For MC produce TT with R2TTMaker
        from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMakerCfg
        return Run2TriggerTowerMakerCfg(flags)
    else:
        # For data decode TT from ByteStream
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        return LVL1CaloRun2ReadBSCfg(flags)


def L1CaloFEXSimCfg(flags):
    acc = ComponentAccumulator()

    # Configure SCell inputs
    sCellType = "SCell"
    if flags.Input.isMC:
        # Read SCell directly from input RDO file
        acc.merge(ReadSCellFromPoolFileCfg(flags,sCellType))
    else:
        from AthenaConfiguration.Enums import LHCPeriod
        if flags.GeoModel.Run is LHCPeriod.Run2:
            # Run-2 data inputs, emulate SCells
            sCellType = "EmulatedSCell"
            from TrigT1CaloFexPerf.EmulationConfig import emulateSC_Cfg
            acc.merge(emulateSC_Cfg(flags,SCOut=sCellType))
        else:
            # Run-3+ data inputs, decode SCells from ByteStream
            acc.merge(ReadSCellFromByteStreamCfg(flags,sCellType))

    # Need also TriggerTowers as input
    acc.merge(TriggerTowersInputCfg(flags))

    eFEX = CompFactory.LVL1.eFEXDriver('eFEXDriver')
    eFEX.eSuperCellTowerMapperTool = CompFactory.LVL1.eSuperCellTowerMapper('eSuperCellTowerMapper', SCell=sCellType)
    acc.addEventAlgo(eFEX)

    # jFEX part
    jFEX = CompFactory.LVL1.jFEXDriver('jFEXDriver')
    jFEX.jSuperCellTowerMapperTool = CompFactory.LVL1.jSuperCellTowerMapper('jSuperCellTowerMapper', SCell=sCellType)
    jFEX.jFEXSysSimTool = CompFactory.LVL1.jFEXSysSim('jFEXSysSimTool')
    acc.addEventAlgo(jFEX)

    gFEX = CompFactory.LVL1.gFEXDriver('gFEXDriver')
    gFEX.gSuperCellTowerMapperTool = CompFactory.LVL1.gSuperCellTowerMapper('gSuperCellTowerMapper', SCell=sCellType)
    gFEX.gFEXSysSimTool = CompFactory.LVL1.gFEXSysSim('gFEXSysSimTool')
    acc.addEventAlgo(gFEX)

    return acc


if __name__ == '__main__':
    ##################################################
    # Add an argument parser
    ##################################################
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument('-i', '--input',
                   metavar='KEY',
                   default='ttbar',
                   help='Key of the input from TrigValInputs to be used, default=%(default)s')
    p.add_argument('-e', '--execute',
                   action='store_true',
                   help='After building the configuration, also process a few events')
    p.add_argument('-n', '--nevents',
                   metavar='N',
                   type=int,
                   default=25,
                   help='Number of events to process if --execute is used, default=%(default)s')
    args = p.parse_args()

    ##################################################
    # Configure all the flags
    ##################################################
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from TrigValTools.TrigValSteering import Input
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    flags.Input.Files = Input.get_input(args.input).paths
    flags.Output.AODFileName = 'AOD.pool.root'
    flags.Common.isOnline = not flags.Input.isMC
    flags.Exec.MaxEvents = args.nevents
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents = 1
    flags.Scheduler.ShowDataDeps = True
    flags.Scheduler.CheckDependencies = True
    flags.Scheduler.ShowDataFlow = True
    flags.Trigger.EDMVersion = 3
    flags.Trigger.doLVL1 = True
    flags.Trigger.enableL1CaloPhase1 = True
    if flags.Common.isOnline:
        flags.IOVDb.GlobalTag = flags.Trigger.OnlineCondTag

    # TODO 1: Reverse this into a special setting for Run-2 data input when the default geo tag is changed to Run-3
    # TODO 2: Any better way of figuring this out than run number?
    if not flags.Input.isMC and flags.Input.RunNumber[0] > 400000:
        flags.GeoModel.AtlasVersion = 'ATLAS-R3S-2021-02-00-00'

    # Enable only calo for this test
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(flags,['LAr','Tile','MBTS'],True)

    flags.lock()

    ##################################################
    # Set up central services: Main + Input reading + L1Menu + Output writing
    ##################################################
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    from AthenaConfiguration.Enums import Format
    if flags.Input.Format == Format.POOL:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        acc.merge(PoolReadCfg(flags))
    else:
        from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags))

    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg, generateL1Menu, createL1PrescalesFileFromMenu
    acc.merge(L1ConfigSvcCfg(flags))
    generateL1Menu(flags)
    createL1PrescalesFileFromMenu(flags)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    FexEDMList = [
        'xAOD::eFexEMRoIContainer#L1_eEMRoI','xAOD::eFexEMRoIAuxContainer#L1_eEMRoIAux.',
        'xAOD::eFexTauRoIContainer#L1_eTauRoI','xAOD::eFexTauRoIAuxContainer#L1_eTauRoIAux.',
        'xAOD::jFexTauRoIContainer#L1_jFexTauRoI','xAOD::jFexTauRoIAuxContainer#L1_jFexTauRoIAux.',
        'xAOD::jFexSRJetRoIContainer#L1_jFexSRJetRoI','xAOD::jFexSRJetRoIAuxContainer#L1_jFexSRJetRoIAux.',
        'xAOD::jFexLRJetRoIContainer#L1_jFexLRJetRoI','xAOD::jFexLRJetRoIAuxContainer#L1_jFexLRJetRoIAux.',
        'xAOD::jFexMETRoIContainer#L1_jFexMETRoI','xAOD::jFexMETRoIAuxContainer#L1_jFexMETRoIAux.',
        'xAOD::jFexSumETRoIContainer#L1_jFexSumETRoI','xAOD::jFexSumETRoIAuxContainer#L1_jFexSumETRoIAux.',
        'xAOD::gFexJetRoIContainer#L1_gFexSRJetRoI','xAOD::gFexJetRoIAuxContainer#L1_gFexSRJetRoIAux.',
        'xAOD::gFexJetRoIContainer#L1_gFexLRJetRoI','xAOD::gFexJetRoIAuxContainer#L1_gFexLRJetRoIAux.',
        'xAOD::gFexJetRoIContainer#L1_gFexRhoRoI','xAOD::gFexJetRoIAuxContainer#L1_gFexRhoRoIAux.',
        'xAOD::gFexGlobalRoIContainer#L1_gScalarEJwoj','xAOD::gFexGlobalRoIAuxContainer#L1_gScalarEJwojAux.',
        'xAOD::gFexGlobalRoIContainer#L1_gMETComponentsJwoj','xAOD::gFexGlobalRoIAuxContainer#L1_gMETComponentsJwojAux.',
        'xAOD::gFexGlobalRoIContainer#L1_gMHTComponentsJwoj','xAOD::gFexGlobalRoIAuxContainer#L1_gMHTComponentsJwojAux.',
        'xAOD::gFexGlobalRoIContainer#L1_gMSTComponentsJwoj','xAOD::gFexGlobalRoIAuxContainer#L1_gMSTComponentsJwojAux.',
    ]
    acc.merge(OutputStreamCfg(flags, 'AOD', ItemList=FexEDMList))

    ##################################################
    # The configuration fragment to be tested
    ##################################################
    acc.merge(L1CaloFEXSimCfg(flags))

    ##################################################
    # Save and optionally run the configuration
    ##################################################
    with open("L1Sim.pkl", "wb") as f:
        acc.store(f)
        f.close()

    if args.execute:
        sc = acc.run()
        if sc.isFailure():
            exit(1)
