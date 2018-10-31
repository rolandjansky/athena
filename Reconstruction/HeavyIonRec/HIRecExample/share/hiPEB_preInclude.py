from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.doHIJetRec.set_Value_and_Lock(False)
jobproperties.HIRecExampleFlags.doHIegamma.set_Value_and_Lock(False)
jobproperties.HIRecExampleFlags.doHIJetAnalysis.set_Value_and_Lock(False)
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(100)
from RecExConfig.RecFlags import rec
rec.doMuon.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doZdc.set_Value_and_Lock(True)
rec.doLucid.set_Value_and_Lock(False)
rec.doAlfa.set_Value_and_Lock(False)
rec.doAFP.set_Value_and_Lock(False)
rec.doFTK.set_Value_and_Lock(False)
rec.doBTagging.set_Value_and_Lock(False)
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly.set_Value_and_Lock(True)
TriggerFlags.doHLT=False
TriggerFlags.doFEX=False
TriggerFlags.doHypo=False
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(True)
DQMonFlags.doStreamAwareMon.set_Value_and_Lock(False)
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.cutLevel.set_Value_and_Lock(2)
