#--------------------------------
# Heavy Ion main switch settings
#--------------------------------
include.block ('HIRecExample/HIRec_jobOptions.py')

# General adjustments
topSequence.TimeOut=3600* Units.s

# Heavy Ion specific
include("HIRecExample/HICalibrationsCoolFolders.py")

from AthenaCommon.DetFlags import DetFlags
from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.ppMode or globalflags.DataSource == "data":
   jobproperties.HIRecExampleFlags.withHijingEventPars = False

# include reconstruction algorithms
# only in ESD step
from RecExConfig.RecFlags import rec
if rec.doESD :
  print "Including HI algorithms in ESD step"

  if jobproperties.HIRecExampleFlags.doHIEventView:
      include( "HIRecExample/HIEventView_jobOptions.py" )

  if jobproperties.HIRecExampleFlags.doHIGlobal:
      include( "HIGlobal/HIGlobal_jobOptions.py" )
    
  if (jobproperties.HIRecExampleFlags.doHIJetRec and \
      DetFlags.haveRIO.Calo_on() and \
      rec.triggerStream() != 'CosmicCalo') :
      include( "HIJetRec/HIJetRec_jobOptions.py" )
  else :
      jobproperties.HIRecExampleFlags.doHIJetRec=False

if jobproperties.HIRecExampleFlags.doValidation:
      include( "HIValidation/HIValidation_jobOptions.py")
