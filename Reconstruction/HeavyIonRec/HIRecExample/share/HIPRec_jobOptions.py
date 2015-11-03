#--------------------------------
# Heavy Ion main switch settings
#--------------------------------

include.block ('HIRecExample/HIPRec_jobOptions.py')

from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.doHIegamma.set_Value_and_Lock(False)
import HIJetRec.HIJetRecConfig
HIJetRec.HIJetRecConfig.SetHIPMode()

#include main HI-mode job options                                                                                                                                                                         
include('HIRecExample/HIRec_jobOptions.py')

