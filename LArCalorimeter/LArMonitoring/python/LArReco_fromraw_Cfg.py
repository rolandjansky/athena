# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArMonitoringOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.lock()

   ## Cell building
   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

  #larCoverage monitoring
   from LArMonitoring.LArCoverageAlg import LArCoverageConfig
   cov_acc = LArCoverageConfig(ConfigFlags)
   cfg.merge(cov_acc)

   #affectedRegions monitoring
   from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfig
   aff_acc = LArAffectedRegionsConfig(ConfigFlags)
   cfg.merge(aff_acc)

   #collision time algo 
   from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
   cfg.merge(LArCollisionTimeCfg(ConfigFlags, cutIteration=False))

   # and collision time monitoring algo
   from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfig
   collmon=LArCollisionTimeMonConfig(ConfigFlags)
   cfg.merge(collmon) 

   #ROD monitoring
   from LArMonitoring.LArRODMonAlg import LArRODMonConfig
   rodmon = LArRODMonConfig(ConfigFlags)
   cfg.merge(rodmon)

   #Digit monitoring

   from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
   cfg.merge(LArNoisyROSummaryCfg(ConfigFlags))

   from LArMonitoring.LArDigitMonAlg import LArDigitMonConfig
   digimon = LArDigitMonConfig(ConfigFlags)
   cfg.merge(digimon)


   ConfigFlags.dump()
   f=open("LArMonMaker.pkl","w")
   cfg.store(f)
   f.close()

   #cfg.run(10,OutputLevel=DEBUG)

