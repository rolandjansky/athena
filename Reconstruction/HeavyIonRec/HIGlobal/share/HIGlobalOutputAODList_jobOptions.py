from HIGlobal.HIGlobalFlags import jobproperties 
if jobproperties.HIGlobalFlags.doHIGlobalVars:
   if jobproperties.HIGlobalFlags.doHIFlow:
     HIAODItemList += [ "HIFlowData#ReactionPlaneAndV2" ]
   if jobproperties.HIRecExampleFlags.ppMode:
     HIAODItemList += [ "HIEtData#PPGlobalEt" ]
   else:
     HIAODItemList += [ "HIEtData#HIGlobalEt" ]
   if jobproperties.HIGlobalFlags.doHICentrality:
     HIAODItemList += [ "HICentralityData#HICentrality" ]

if jobproperties.HIGlobalFlags.doHIGlobalNSiCluster :
   HIAODItemList += [ "EtaPhiBins#NSiClusterL0" ]
   HIAODItemList += [ "EtaPhiBins#NSiClusterL1" ]
   HIAODItemList += [ "EtaPhiBins#NSiClusterL2" ]
                
# Converter not available yet
#if jobproperties.HIGlobalFlags.doHIPixelTracklet :
#   HIAODItemList += [ "HIPixelTrackletsCollection#pixelTrackletsCollection" ]
#   if rec.doTruth() :
#      HIAODItemList += [ "HIPixelTrackletsTruthCollection#pixelTrackletsTruth" ]
