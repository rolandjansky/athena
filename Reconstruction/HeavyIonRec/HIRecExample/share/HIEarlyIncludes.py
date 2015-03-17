os.system('get_files -data HIFlowWeight.root')

# temporary until RUN2 configuration is ready
from RecExConfig.RecFlags import rec
rec.doTrigger=False

rec.doTau = False
rec.doJetMissingETTag = False

from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.doHIJetRec = False
jobproperties.HIRecExampleFlags.doHIGlobal = False
jobproperties.HIRecExampleFlags.ppMode = True

# turn off pp pilup noise suppression
from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
      
# turning off some monitoring
if rec.doMonitoring() :
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
    DQMonFlags.doTauMon.set_Value_and_Lock(False)
    DQMonFlags.doJetTagMon.set_Value_and_Lock(False)

#turn off some EventTags
from RecExConfig.RecFlags import rec
if rec.doWriteTAG() :
    include ("EventTagAlgs/EventTagFlags.py")
    EventTagFlags.set_AnalysisOff()
    EventTagFlags.set_TauJetOff()
    EventTagFlags.set_MissingETOff()
