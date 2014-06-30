include.block ("EventTagUtils/RegistrationStreamLumiTrig_jobOptions.py")

from EventTagUtils.EventTagUtilsConf import RegistrationStreamLumiTrig as ConfiguredRegistrationStreamLumiTrig
from RecExConfig.RecFlags import rec
topSequence += ConfiguredRegistrationStreamLumiTrig(GetTriggerConf = rec.doTrigger())
if not rec.doTrigger() :
   print "RegistrationStreamLumiTrig_jobOptions.py::WARNING: No Trigger keys in metadata if rec.doTrigger()=false"

print topSequence
