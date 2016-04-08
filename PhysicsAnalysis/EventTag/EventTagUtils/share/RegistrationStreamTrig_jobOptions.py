include.block ("EventTagUtils/RegistrationStreamTrig_jobOptions.py")

from EventTagUtils.EventTagUtilsConf import RegistrationStreamTrig as ConfiguredRegistrationStreamTrig
from RecExConfig.RecFlags import rec
topSequence += ConfiguredRegistrationStreamTrig(GetTriggerConf = rec.doTrigger())
if not rec.doTrigger() :
   print "RegistrationStreamTrig_jobOptions.py::WARNING: No Trigger keys in metadata if rec.doTrigger()=false"

print topSequence
