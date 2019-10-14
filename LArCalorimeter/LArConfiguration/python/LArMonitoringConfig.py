#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def LArMonitoringConfig(inputFlags):


    from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfig
    from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfig
    from LArMonitoring.LArDigitMonAlg import LArDigitMonConfig
    from LArMonitoring.LArRODMonAlg import  LArRODMonConfig

    # algos which could run anytime:
    acc = LArAffectedRegionsConfig(inputFlags)
    acc.merge(LArCollisionTimeMonConfig(inputFlags))

    # and others on RAW data only
    from LArConfiguration.LArConfigFlags import  _getLArRunInfo
    lri=_getLArRunInfo(inputFlags)
    if lri is not None and lri.runType is not None:
       acc.merge(LArDigitMonConfig(inputFlags))
       acc.merge(LArRODMonConfig(inputFlags))

    return acc



if __name__=='__main__':

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
   ConfigFlags.DQ.enableLumiAccess = True
   ConfigFlags.DQ.useTrigger = True
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()


   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   #from CaloD3PDMaker.CaloD3PDConfig import CaloD3PDCfg,CaloD3PDAlg
   #cfg.merge(CaloD3PDCfg(ConfigFlags, filename=ConfigFlags.Output.HISTFileName, streamname='CombinedMonitoring'))

   acc = LArMonitoringConfig(ConfigFlags)
   cfg.merge(acc)

   cfg.printConfig()

   ConfigFlags.dump()
   f=open("LArMonitoring.pkl","w")
   cfg.store(f)
   f.close()

   #cfg.run(100,OutputLevel=WARNING)
