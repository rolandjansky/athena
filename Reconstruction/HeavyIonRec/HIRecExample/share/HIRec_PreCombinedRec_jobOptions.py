from HIJetRec.HIJetRecFlags import jobproperties
from CaloRec.CaloRecFlags import jobproperties

if jobproperties.HIJetRecFlags.doHIPhoton :
    rec.doEgamma = False
    jobproperties.CaloRecFlags.doEmCluster = True
    jobproperties.CaloRecFlags.doCaloCluster = True
    
    #turning off TopoClusters
    jobproperties.CaloRecFlags.doCaloTopoCluster = False
    jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
    jobproperties.CaloRecFlags.doCaloTopoTower = False

