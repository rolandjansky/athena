#==============================================================
# Job Configuration parameters:
#==============================================================

from RecExConfig.RecFlags import rec
rec.doHeavyIon = True 

from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.ppMode = True

jobproperties.HIRecExampleFlags.doHIGlobal = True
jobproperties.HIRecExampleFlags.doHIEventView = False
jobproperties.HIRecExampleFlags.doHIJetRec = False
jobproperties.HIRecExampleFlags.doHIJetAnalysis = False

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
DQMonFlags.doTauMon.set_Value_and_Lock(False)
DQMonFlags.doJetTagMon.set_Value_and_Lock(False)
