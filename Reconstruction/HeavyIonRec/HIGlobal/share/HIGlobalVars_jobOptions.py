from HIGlobal.HIGlobalFlags import jobproperties
if jobproperties.HIGlobalFlags.doHIFlow:
  from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
  CaloTowerCmbGetter()

  from HIGlobal.HIGlobalConf import HIFlow
  for iharmonic in range(jobproperties.HIGlobalFlags.FlowHarmonicMin.get_Value(),jobproperties.HIGlobalFlags.FlowHarmonicMax.get_Value()+1) :
    flowObject = HIFlow("HIFlowAlg_v%i" % iharmonic)
    flowObject.HIFlowDataContainerName= jobproperties.HIGlobalFlags.HIFlowContainerPrefix.get_Value() + str(iharmonic)
    flowObject.Harmonic=iharmonic
    if globalflags.DataSource == "data":
      flowObject.isData = True
      
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += flowObject
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addStreamESD('HIFlowData',flowObject.HIFlowDataContainerName)
      
if jobproperties.HIRecExampleFlags.ppMode: 
   theApp.TopAlg   += [ "PPGlobalEt" ]
else:  
   theApp.TopAlg   += [ "HIGlobalEt" ]
if jobproperties.HIGlobalFlags.doHICentrality:
   from HIGlobal.HIGlobalConf import HICentrality
   HICentrality = HICentrality()
   if globalflags.DataSource == "data":
      HICentrality.isData = True
   theApp.TopAlg   += [ "HICentrality" ]
if jobproperties.HIGlobalFlags.doHITRT:
  from HIGlobal.HIGlobalConf import HITRT
  theApp.TopAlg   += [ "HITRT" ]
    # === TRT ===
    #from  RecExConfig.ObjKeyStore import  objKeyStore
    #objKeyStore.addStreamESD("HITRTInfo", "myHI_TRTInfoObject" )

