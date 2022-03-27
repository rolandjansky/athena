# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

class Run2TriggerTowerMakerBase (LVL1__Run2TriggerTowerMaker):
    __slots__ = []
    def __init__(self, name):
        super( Run2TriggerTowerMakerBase, self ).__init__( name )
        self.DigiEngine =  "%s_Digitization"%name
    
class Run2TriggerTowerMaker(Run2TriggerTowerMakerBase):
    """ Baseline Run2 TriggerTower configuration:
      - with pedestal correction
      - noise cuts
    """
    __slots__ = []
    def __init__(self, name):
        super(Run2TriggerTowerMaker, self).__init__(name)
        self.CellType = 3 # TTL1 input
        self.ZeroSuppress = True

        from SGComps.AddressRemappingSvc import addInputRename
        addInputRename ( 'xAOD::TriggerTowerContainer', 'xAODTriggerTowers_rerun', 'xAODTriggerTowers')
        
class Run2TriggerTowerMaker25ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 25ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker25ns, self).__init__(name)

class Run2TriggerTowerMaker50ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 50ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker50ns, self).__init__(name)


def L1CaloCondFoldersCfg(flags):
        L1CaloFolders = {}
        L1CaloFolders['PprChanCalib'] = '/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib'
        L1CaloFolders['PprChanDefaults'] = '/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults'
        # Different folders for data and MC
        ver = 'V2' if flags.Input.isMC else 'V1'
        L1CaloFolders['DisabledTowers'] = f'/TRIGGER/L1Calo/{ver}/Conditions/DisabledTowers'
        L1CaloFolders['PpmDeadChannels'] = f'/TRIGGER/L1Calo/{ver}/Calibration/PpmDeadChannels'

        # TODO decide what is needed form below items, (likely only needed when re-running on the data)
        #L1CaloFolderList += ['/TRIGGER/L1Calo/V1/Conditions/RunParameters']
        #L1CaloFolderList += ['/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars']
        #L1CaloFolderList += ['/TRIGGER/Receivers/Conditions/VgaDac']
        #L1CaloFolderList += ['/TRIGGER/Receivers/Conditions/Strategy']

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        db = 'TRIGGER_ONL' if flags.Common.isOnline else 'TRIGGER_OFL'
        return addFolders(flags, list(L1CaloFolders.values()), db), L1CaloFolders


def Run2TriggerTowerMakerCfg(flags, name='Run2TriggerTowerMaker25ns'):
    '''
    Basic setup of tower maker cfg for new JO 
    WARNING: need to add dependencies on digi flags (as above) that are missing as of now
    '''

    acc = ComponentAccumulator()
    # TODO this is only needed when re-running 
    #    from SGComps.AddressRemappingConfig import InputRenameCfg
    #    acc.merge(InputRenameCfg('xAOD::TriggerTowerContainer', 'xAODTriggerTowers_rerun', 'xAODTriggerTowers'))

    # Configure conditions used by R2TTMaker
    from CaloConditions.CaloConditionsConfig import CaloTTIdMapCfg
    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge(CaloTTIdMapCfg(flags))
    acc.merge(TileInfoLoaderCfg(flags))
    condFoldersAcc, condFolders = L1CaloCondFoldersCfg(flags)
    acc.merge(condFoldersAcc)

    # R2TTMaker reads TTL1 containers from input POOL file (RDO, ESD, ...)
    if flags.Input.Format is Format.POOL:
        ttl1Containers = [
            ('LArTTL1Container', 'LArTTL1EM'),
            ('LArTTL1Container', 'LArTTL1HAD'),
            ('TileTTL1Container', 'TileTTL1Cnt'),
        ]
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, Load=ttl1Containers))

    # R2TTMaker reads L1Menu in BeginRun incident, so needs L1ConfigSvc
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))

    # R2TTMaker needs the mu information available as EventInfo decoration
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    acc.merge(LumiBlockMuWriterCfg(flags))

    alg = CompFactory.LVL1.Run2TriggerTowerMaker(name,
                                                 DigiEngine = "{}_Digitization".format(name),
                                                 # TODO make these settings flags dependent
                                                 CellType = 3,
                                                 inputTTLocation = 'unused',
                                                 TriggerTowerLocationRerun = 'also_unused',
                                                 ZeroSuppress = True, 
                                                 ChanCalibFolderKey = condFolders['PprChanCalib'],
                                                 ChanDefaultsFolderKey = condFolders['PprChanDefaults'],
                                                 DisabledTowersFolderKey = condFolders['DisabledTowers'],
                                                 DeadChannelsFolderKey = condFolders['PpmDeadChannels'],
                                                 #ExtraInputs = ['LArTTL1Container#LArTTL1EM', 'LArTTL1Container#LArTTL1HAD', 'TileTTL1Container#TileTTL1Cnt']
                                                 )
    acc.addEventAlgo(alg)

    return acc



def L1CaloLegacySimCfg(flags):
    '''
    Configures Legacy 1 calo in new JO style
    '''
    acc = ComponentAccumulator()
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc.merge (GeoModelCfg (flags))

    from CaloConditions.CaloConditionsConfig import CaloTriggerTowerCfg,LArTTCellMapCfg,CaloTTIdMapCfg
    acc.merge(CaloTriggerTowerCfg(flags))
    acc.merge(LArTTCellMapCfg(flags))
    acc.merge(CaloTTIdMapCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    
    acc.merge(LArGMCfg(flags))
    acc.merge(TileGMCfg(flags))

    # necessary conditions    
    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
    acc.merge(LArBadChannelCfg(flags))
    acc.merge(LArBadFebCfg(flags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge(TileInfoLoaderCfg(flags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    acc.merge( TileEMScaleCondAlgCfg(flags) )

    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))

    if not flags.Input.isMC:
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        acc.merge(LVL1CaloRun2ReadBSCfg(flags))

    if flags.Input.isMC:
        acc.merge(Run2TriggerTowerMakerCfg(flags))
        acc.addEventAlgo(CompFactory.LVL1.Run2CPMTowerMaker('CPMTowerMaker'))
        acc.addEventAlgo(CompFactory.LVL1.Run2JetElementMaker('JetElementMaker'))

    acc.addEventAlgo(CompFactory.LVL1.CPMSim('CPMSim'))
    acc.addEventAlgo(CompFactory.LVL1.JEMJetSim('JEMJetSim'))
    acc.addEventAlgo(CompFactory.LVL1.JEMEnergySim('JEMEnergySim'))
    acc.addEventAlgo(CompFactory.LVL1.CPCMX('CPCMX'))
    acc.addEventAlgo(CompFactory.LVL1.JetCMX('JetCMX'))
    acc.addEventAlgo(CompFactory.LVL1.EnergyCMX('EnergyCMX'))
    acc.addEventAlgo(CompFactory.LVL1.RoIROD('RoIROD'))
    if flags.Input.isMC:
        acc.addEventAlgo(CompFactory.LVL1.TrigT1MBTS())
    return acc

if __name__ == '__main__':
    import sys
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.lock()

    acc = MainServicesCfg(flags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))

    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
    generateL1Menu(flags)

    from AthenaCommon.CFElements import seqAND
    acc.addSequence(seqAND('L1CaloLegacySimSeq'), parentName='AthAlgSeq')
    acc.merge(L1CaloLegacySimCfg(flags), sequenceName='L1CaloLegacySimSeq')

    acc.printConfig(withDetails=True, summariseProps=True, printDefaults=True)
    with open("L1CaloSim.pkl", "wb") as p:
        acc.store(p)
        p.close()

    sys.exit(acc.run().isFailure())
