__doc__ = """Joboption fragment to add ringer containers to AOD/ESD streamers
             using input file containers."""

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRinger_in2out.py' )
mlog.info('Entering')

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
caloRingerFlags.Enabled = True
caloRingerFlags.doWriteRingsToFile = True

# Make sure we have the main algorithm disabled:
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder
CRAlgBuilder = CaloRingerAlgorithmBuilder(disable = True)
from CaloRingerAlgs.CaloRingerMetaDataBuilder import CaloRingerMetaDataBuilder
CRMetaBuilder = CaloRingerMetaDataBuilder(disable=True)

# Make sure all MetaData algoritms have the ringerOutputLevel
if CRMetaBuilder.usable():
  ringerOutputLevel = caloRingerFlags.OutputLevel()
  # Get the ringer configuration writter handle
  configWriter = CRMetaBuilder.getConfigWriterHandle()

  if configWriter:
    # Change its output level
    mlog.verbose('Changing %r output level to %s', configWriter, ringerOutputLevel)
    configWriter.OutputLevel = ringerOutputLevel

# And then include the output item list joboption to check for 
# available ringer containers:
include('CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py')
