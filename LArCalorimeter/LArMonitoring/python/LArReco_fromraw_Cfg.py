# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.isMC=False
   ConfigFlags.Input.Files = defaultTestFiles.RAW
   ConfigFlags.Tile.RunType = 'PHY'
   ConfigFlags.Tile.NoiseFilter = 1

   ConfigFlags.Output.HISTFileName = 'LArAffectedRegionsOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()


   # LArRawChannels building
   from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
   from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
   from LArROD.LArRawChannelBuilderAlgConfig import  LArRawChannelBuilderAlgCfg
   from LArROD.LArFebErrorSummaryMakerConfig import  LArFebErrorSummaryMakerCfg

   cfg=MainServicesSerialCfg()
   #from GaudiCoreSvc.GaudiCoreSvcConf import MessageSvc
   #msgsvc = MessageSvc()
   #msgsvc.defaultLimit = 0 
   #msgsvc.OutputLevel=DEBUG
   #msgFmt = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
   #msgFmt = "% F%18W%S%7W%R%T %0W%M"
   #msgsvc.Format = msgFmt
   #cfg.addService(msgsvc)
   #cfg.addService(MessageSvc(OutputLevel=DEBUG))

   cfg.merge(LArRawDataReadingCfg(ConfigFlags))
   cfg.merge(LArFebErrorSummaryMakerCfg(ConfigFlags))
   cfg.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
   

   ## Cell building
   ##from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

   ##cfg=ComponentAccumulator()

   #from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
   #cfg.addEventAlgo(xAODMaker__EventInfoCnvAlg())

   #from CaloRec.CaloCellMakerConfig import CaloCellMakerCfg
   #acc=CaloCellMakerCfg(ConfigFlags)
   #acc.getPrimary().CaloCellsOutputName="AllCalo"
   #cfg.merge(acc)

   # now affected regions creation, and geometry needed....
   from LArGeoAlgsNV.LArGMConfig import LArGMCfg
   cfg.merge(LArGMCfg(ConfigFlags))
   from LArMonitoring.LArHVDBConfig import LArHVDBCfg
   cfg.merge(LArHVDBCfg(ConfigFlags))
   from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfig
   aff_acc = LArAffectedRegionsConfig(ConfigFlags)
   cfg.merge(aff_acc)

   # try collision time algo - doesn't work somehow
   #from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
   #cfg.merge(CaloNoiseCondAlgCfg(ConfigFlags))

   #from LArMonitoring.LArCollisionTimeConfig import LArCollisionTimeCfg
   #cfg.merge(LArCollisionTimeCfg(ConfigFlags))

   #from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfig
   #cfg.merge(LArCollisionTimeMonConfig(ConfigFlags)) 

   ConfigFlags.dump()
   f=open("LArRawMaker.pkl","w")
   cfg.store(f)
   f.close()

   cfg.run(10,OutputLevel=DEBUG)

