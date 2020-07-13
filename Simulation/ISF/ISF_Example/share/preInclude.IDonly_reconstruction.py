from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

import MagFieldServices.SetupField

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doBremRecovery.set_Value_and_Lock(False)
InDetFlags.doPhysValMon.set_Value_and_Lock(True)

#from GaudiSvc.GaudiSvcConf import THistSvc

# --- controls what is written out. ESD includes AOD, so it's normally enough
# --- ESD writing temporarily disabled post 2013 migration - to be reinstated! 

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

#rec.doESD.set_Value_and_Lock             (True)
#rec.doWriteESD.set_Value_and_Lock        (True)###
#rec.doAOD.set_Value_and_Lock             (True)
#rec.doWriteAOD.set_Value_and_Lock        (True)
#rec.doDPD.set_Value_and_Lock             (False)
#rec.doCBNT.set_Value_and_Lock            (False)
#rec.doWriteTAG.set_Value_and_Lock        (False)

# --- turn on InDet
rec.doInDet.set_Value_and_Lock           (True)
# --- turn off calo
rec.doCalo.set_Value_and_Lock            (False)
# --- turn off muons
rec.doMuon.set_Value_and_Lock            (False) 
# --- turn off forward detectors
rec.doForwardDet.set_Value_and_Lock      (False)
# --- turn off trigger
rec.doTrigger.set_Value_and_Lock         (False)

# --- turn off combined reconstruction
rec.doEgamma.set_Value_and_Lock          (False)
rec.doMuonCombined.set_Value_and_Lock    (False)
rec.doTau.set_Value_and_Lock             (False)
rec.doJetMissingETTag.set_Value_and_Lock (False)

# --- turn of calo stuff we don't need anyway
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock  (False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoTower.set_Value_and_Lock    (False)

# --- turn of jets (Hack!!!)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock          (False)

# --- turn off egamma Brem
recAlgs.doEgammaBremReco.set_Value_and_Lock  (False)
# --- turn off Eflow and missing ET
recAlgs.doEFlow.set_Value_and_Lock           (False)
recAlgs.doEFlowJet.set_Value_and_Lock        (False)
recAlgs.doMissingET.set_Value_and_Lock       (False)
recAlgs.doMissingETSig.set_Value_and_Lock    (False)
recAlgs.doObjMissingET.set_Value_and_Lock    (False)
# --- turn off combined muons
recAlgs.doMuGirl.set_Value_and_Lock          (False)
recAlgs.doMuTag.set_Value_and_Lock           (False)
recAlgs.doMuidLowPt.set_Value_and_Lock       (False)
recAlgs.doMuonIDCombined.set_Value_and_Lock  (False)
recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
recAlgs.doMuonSpShower.set_Value_and_Lock    (False)
recAlgs.doStaco.set_Value_and_Lock           (False)
recAlgs.doCaloTrkMuId.set_Value_and_Lock     (False)
recAlgs.doTileMuID.set_Value_and_Lock        (False)
# --- trigger
recAlgs.doTrigger.set_Value_and_Lock         (False)
