from HIGlobal.HIGlobalFlags import jobproperties 
if jobproperties.HIGlobalFlags.doHIGlobalVars:
   if jobproperties.HIGlobalFlags.doHIFlow:
     HIESDItemList += [ "HIFlowData#ReactionPlaneAndV2" ]
   if jobproperties.HIRecExampleFlags.ppMode:
     HIESDItemList += [ "HIEtData#PPGlobalEt" ]
   else:
     HIESDItemList += [ "HIEtData#HIGlobalEt" ]
   if jobproperties.HIGlobalFlags.doHICentrality:
     HIESDItemList += [ "HICentralityData#HICentrality" ]
   if jobproperties.HIGlobalFlags.doHITRT:
      HIESDItemList += [ "HITRTInfo#HITRT" ]
#    from  RecExConfig.ObjKeyStore import  objKeyStore
#    objKeyStore.addStreamESD("HITRTInfo", "myHI_TRTInfoObject" )

if jobproperties.HIGlobalFlags.doHIGlobalNSiCluster:
   HIESDItemList += [ "EtaPhiBins#NSiClusterL0" ]
   HIESDItemList += [ "EtaPhiBins#NSiClusterL1" ]
   HIESDItemList += [ "EtaPhiBins#NSiClusterL2" ]

# Converter not available yet
#if jobproperties.HIGlobalFlags.doHIPixelTracklet :
#   HIESDItemList += [ "HIPixelTrackletsCollection#pixelTrackletsCollection" ]
#   if rec.doTruth() :
#      HIESDItemList += [ "HIPixelTrackletsTruthCollection#pixelTrackletsTruth" ]
