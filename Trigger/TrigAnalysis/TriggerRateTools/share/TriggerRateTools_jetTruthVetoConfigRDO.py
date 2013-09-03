from JetRec.JetGetters import *

from JetRec.JetRecFlags import jobproperties
jobproperties.JetRecFlags.Enabled.set_Value(False)
jobproperties.JetRecFlags.doDetailedCBNT.set_Value(True)
jobproperties.JetRecFlags.evgenJets.set_Value(True)
jobproperties.JetRecFlags.noStandardConfig.set_Value(True)
jobproperties.JetRecFlags.doBTagging.set_Value(False)

jetFlags.inputFileType.set_Value('RDO')

Cone7alg = make_StandardJetGetter('Cone', 0.7, 'Truth').jetAlgorithmHandle()
Cone7alg.OutputLevel = 3

