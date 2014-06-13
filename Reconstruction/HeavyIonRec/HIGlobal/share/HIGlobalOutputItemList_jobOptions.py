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

  # Upper-lower case problem with ID of this object
  # if jobproperties.HIGlobalFlags.doHIPixelTracklet :
  #    HIESDItemList += [ "EtaPhiBins#*" ]
