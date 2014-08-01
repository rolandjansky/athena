#==============================================================
# Job Configuration parameters:
#==============================================================

include ( "HIRecExample/HIEarlyIncludes.py" )

from RecExConfig.RecFlags import rec
rec.doHeavyIon = True 

from HIRecExample.HIRecExampleFlags import jobproperties
jobproperties.HIRecExampleFlags.withHijingEventPars = False
