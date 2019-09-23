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
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArMonitoringOutput.root'
   ConfigFlags.lock()

   ## Cell building
   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfig
   aff_acc = LArAffectedRegionsConfig(ConfigFlags)
   cfg.merge(aff_acc)

   # try collision time algo 
   from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
   cfg.merge(LArCollisionTimeCfg(ConfigFlags))
   cfg.getEventAlgo("LArCollisionTimeAlg").cutIteration=False

   # and finally monitoring algo
   from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfig
   collmon=LArCollisionTimeMonConfig(ConfigFlags)
   collmon.getEventAlgo("larCollTimeMonAlg").timeDiffCut=5.0
   collmon.getEventAlgo("larCollTimeMonAlg").nCells=1
   collmon.getEventAlgo("larCollTimeMonAlg").TrainFrontDistance=30
   cfg.merge(collmon) 


   ConfigFlags.dump()
   f=open("LArMonMaker.pkl","w")
   cfg.store(f)
   f.close()

   #cfg.run(10,OutputLevel=DEBUG)

