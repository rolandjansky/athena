__doc__ = """JobOption fragment to add CaloRinger algorithm to run. It is not
the definitive joboption, but expected to be included at a global joboption
that will configure the reconstruction."""

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRinger_joboptions.py' )
mlog.info('Entering')

from CaloRingerAlgs.CaloRingerFlags import jobproperties
if not jobproperties.CaloRingerFlags.Enabled:
  mlog.info('Enabling CaloRingerFlags!')
  jobproperties.CaloRingerFlags.Enabled = True


# Add main algorithm builder
from CaloRingerAlgs.CaloRingerAlgorithmBuilder \
    import CaloRingerAlgorithmBuilder
CRAlgBuilder = CaloRingerAlgorithmBuilder()
# Add metadata builder/reader
from CaloRingerAlgs.CaloRingerMetaDataBuilder \
    import CaloRingerMetaDataBuilder
MetaDataBuilder = CaloRingerMetaDataBuilder()

