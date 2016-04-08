# Simple joboption that will include all necessary joboptions for
# the Ringer reconstruction to happen.
include('CaloRingerAlgs/CaloRinger_joboptions.py') 
from CaloRingerAlgs.CaloRingerFlags import jobproperties
if jobproperties.CaloRingerFlags.doWriteRingsToFile():
  include('CaloRingerAlgs/CaloRingerOutputItemList_joboptions.py')
else:
  from RecExConfig.RecFlags import rec
  if rec.doWriteAOD() or rec.doWriteESD():
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'CaloRinger_reconstruction.py' )
    mlog.warning(('Rings information will not be added to output file' \
        '. Make sure that this is the desired behavior!'))
include('CaloRingerAlgs/CaloRingerLock_joboptions.py')

# Change the output level for the Ringer algorithm/tools
include('CaloRingerAlgs/CaloRinger_OutputLevel.py')
