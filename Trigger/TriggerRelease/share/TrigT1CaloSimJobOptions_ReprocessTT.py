# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger('TrigT1CaloSimJobOptions_ReprocessTT')

#default database
# test COOL folder version 03 i.e. folder tag V2-PHYSICS-CHANCALIB-00-03
conddb.blockFolder("/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib")
conddb.addFolder("","<dbConnection>sqlite://;schema=/afs/cern.ch/work/h/hristova/public/l1calo/l1calo.sqlite;dbname=OFLP200</dbConnection> /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib <tag>V2-PHYSICS-CHANCALIB-00-03</tag>", force=True)

#reprocess
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get RunNumber from input file
_my_run_number = None
try:
    # first attempt RecExConfig ...
    from RecExConfig.AutoConfiguration import GetRunNumber
    _my_run_number = GetRunNumber()
    # sometime GetRunNumber doesn't raise an exception if something goes wrong ...
    if _my_run_number < 100000: raise ValueError('non-sensible RunNumber')
except BaseException, E:
    log.info('Exception retrieving RunNumber from RecExConfig.AutoConfiguration: %s' % E)
    #  let's see if we are lucky with AthFile directly ...
    try:
        import PyUtils.AthFile as athFile
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
        _my_run_number = af.run_number[0] # don't modify global state
    except BaseException, EE:
        log.info('Exception retrieving RunNumber from AthFile: %s' % EE)
        # one last shot ...
        try:
            global _run_number
            _my_run_number = _run_number
        except:
            # now give up
            _my_run_number = None
            log.warning("Could not retrieve RunNumber from inputs. Assuming Run-2 data.")
            
# on Run-1 data, schedule the {CPMTower,JetElement}CnvAlg if necessary
if _my_run_number:
    log.info('Using RunNumber %d' % _my_run_number)
if _my_run_number and _my_run_number < 222222:
    job += CfgMgr.xAODMaker__CPMTowerCnvAlg()
    job += CfgMgr.xAODMaker__JetElementCnvAlg()
    # add to bytestream decoder if not already there
    if globalflags.InputFormat() == 'bytestream':
        if not CfgMgr.ByteStreamAddressProviderSvc() in svcMgr:
            # might not have been added to the ServiceMgr yet ...
            svcMgr += CfgMgr.ByteStreamAddressProviderSvc()
        if not "DataVector<LVL1::CPMTower>/CPMTowers" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowers" ]
        if not "DataVector<LVL1::JetElement>/JetElements" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElements" ]

        try:
            # unpacking Run-1 data directly to xAOD::CPMTowers is not supported
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::CPMTowerContainer/CPMTowers")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::CPMTowerAuxContainer/CPMTowersAux.")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::JetElementContainer/JetElements")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::JetElementAuxContainer/JetElementsAux.")
        except ValueError:
            pass
# clean up
del _my_run_number

log.fatal("David l1standalone!")
#add algorithms to sequence
topSequence += LVL1__Run2TriggerTowerMaker( 'Run2TriggerTowerMaker',
                                            CellType=2,)
topSequence.Run2TriggerTowerMaker.ZeroSuppress = False # store all trigger towers 
topSequence += LVL1__Run2CPMTowerMaker( 'CPMTowerMaker',)
topSequence += LVL1__Run2JetElementMaker( 'JetElementMaker',)
topSequence += LVL1__CPMSim( 'CPMSim',)
topSequence += LVL1__JEMJetSim( 'JEMJetSim',)
topSequence += LVL1__JEMEnergySim( 'JEMEnergySim',)
topSequence += LVL1__CPCMX( 'CPCMX',)
topSequence += LVL1__JetCMX( 'JetCMX',)
topSequence += LVL1__EnergyCMX( 'EnergyCMX',)
topSequence += LVL1__RoIROD( 'RoIROD',)

#RoIBuilder

roib = ROIB__RoIBuilder(
      'RoIBuilder',
      DoMuon = False,
      DoCalo = True,
      
      #CTPSLinkLocation = 'Event/CTPSLinkLocation'+self.modName,
      #CaloEMTauLocation = 'CaloTriggerDataLocation/EmTauSlink'+self.modName,
      #CaloJetEnergyLocation = 'CaloTriggerDataLocation/JEPSlink'+self.modName,
      RoIBRDOLocation = 'RoIBResult'
)
algos.append(roib)

roiBxAOD = RoIBResultToxAOD(
  'RoIBResultToxAOD',
  DoMuon = False,
  DoCalo = True,
      
  CPMTowerLocation = 'CPMTowers',
  JetElementLocation = 'JetElements',
  RoIBResultInputKey = 'RoIBResult',
  LVL1_ROIOutputKey = 'LVL1_ROI',
      
  xAODKeyEmTau = 'LVL1EmTauRoIs',
  xAODKeyEsum = 'LVL1EnergySumRoI',
  xAODKeyJetEt = 'LVL1JetEtRoI',
  xAODKeyJet = 'LVL1JetRoIs',
  xAODKeyMu = 'LVL1MuonRoIs'
) 









