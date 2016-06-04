__doc__ = """Joboption fragment to add ringer containers to AOD/ESD streamers
             using input file containers."""

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRinger_in2out.py' )
mlog.info('Entering')

from CaloRingerAlgs.CaloRingerFlags import jobproperties
jobproperties.CaloRingerFlags.Enabled = True
jobproperties.CaloRingerFlags.doWriteRingsToFile.set_Value_and_Lock(True)

# Make sure we have the main algorithm disabled:
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder
CRAlgBuilder = CaloRingerAlgorithmBuilder(disable = True)
from CaloRingerAlgs.CaloRingerMetaDataBuilder import CaloRingerMetaDataBuilder
CRMetaBuilder = CaloRingerMetaDataBuilder()

# And then include the output item list joboption to check for 
# available ringer containers:
include('CaloRingerAlgs/CaloRingerOutputItemList_joboptions.py')

# Change the output level for the Ringer algorithm/tools
include('CaloRingerAlgs/CaloRinger_OutputLevel.py')
