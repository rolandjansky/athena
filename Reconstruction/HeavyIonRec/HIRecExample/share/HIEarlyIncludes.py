from RecExConfig.RecFlags import rec
rec.doTrigger=False

#disabling reco algs not needed in HI mode
rec.doTau = False
rec.doJetMissingETTag = False
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled = False

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow = False
recAlgs.doMissingET = False
recAlgs.doMissingETSig = False

#need this eventually, but for now it breaks egamma isolation 
#from CaloRec.CaloRecFlags import jobproperties
#jobproperties.CaloRecFlags.doCaloTopoCluster=False
#jobproperties.CaloRecFlags.doCaloEMTopoCluster=False

from HIRecExample.HIRecExampleFlags import jobproperties
if rec.doESD() :
    if jobproperties.HIRecExampleFlags.doHIJetRec() and jobproperties.HIRecExampleFlags.doHIegamma() :
        jobproperties.CaloRecFlags.doCaloCluster=False
        jobproperties.CaloRecFlags.doEmCluster=False
        rec.doEgamma=False

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doMuGirlLowBeta = False

#do not save AOD cells
rec.doAODCaloCells =False

from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.ppMode = True


# turn off pp pilup noise suppression
from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
      
# turning off some monitoring
if rec.doMonitoring() :
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
    DQMonFlags.doTauMon.set_Value_and_Lock(False)
    #change this?
    DQMonFlags.doJetTagMon.set_Value_and_Lock(False)

#turn off some EventTags
from RecExConfig.RecFlags import rec
if rec.doWriteTAG() :
    include ("EventTagAlgs/EventTagFlags.py")
    EventTagFlags.set_AnalysisOff()
    EventTagFlags.set_TauJetOff()
    EventTagFlags.set_MissingETOff()


