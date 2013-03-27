from HIJetRec.HIJetRecFlags import jobproperties

if jobproperties.HIJetRecFlags.doHIPhoton :
    jobproperties.CaloRecFlags.doEmCluster = False
    jobproperties.CaloRecFlags.doCaloCluster = False
else:    
    jobproperties.CaloRecFlags.doEmCluster = True
    jobproperties.CaloRecFlags.doCaloCluster = True

#turning off TopoClusters
jobproperties.CaloRecFlags.doCaloTopoCluster = False
jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
jobproperties.CaloRecFlags.doCaloTopoTower = False
