from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'CaloRingerOutputItemList_jobOptions.py' )
mlog.info('Entering')

from CaloRingerAlgs.CaloRingerAlgorithmBuilder \
    import CaloRingerAlgorithmBuilder

from CaloRingerAlgs.CaloRingerMetaDataBuilder \
    import CaloRingerMetaDataBuilder

from CaloRingerAlgs.CaloRingerFlags import jobproperties
CaloRingerFlags = jobproperties.CaloRingerFlags
RingerOutputLevel = CaloRingerFlags.OutputLevel()

# Get instance to the builder:
CRAlgBuilder = CaloRingerAlgorithmBuilder()

if CRAlgBuilder.usable():
  mainAlg = CRAlgBuilder.getCaloRingerAlgHandle()
  if mainAlg:
    mlog.verbose('Changing %r output level to %s', mainAlg, RingerOutputLevel)
    mainAlg.OutputLevel = RingerOutputLevel

  builderHandles = CRAlgBuilder.getCaloRingerBuilders()
  for builder in builderHandles:
    mlog.verbose('Changing %r output level to %s', builder, RingerOutputLevel)
    builder.OutputLevel = RingerOutputLevel

  selectorHandles = CRAlgBuilder.getRingerSelectors()
  for selector in selectorHandles:
    mlog.verbose('Changing %r output level to %s', selector, RingerOutputLevel)
    selector.OutputLevel = RingerOutputLevel

# Get instance to the metadata builder:
CRMetaBuilder = CaloRingerMetaDataBuilder()

if CRMetaBuilder.usable():
  configWriter = CRMetaBuilder.getConfigWriterHandle()
  if configWriter:
    configWriter.OutputLevel = RingerOutputLevel
    mlog.verbose('Changing %r output level to %s', configWriter, RingerOutputLevel)

from CaloRingerAlgs.CaloRingerLocker import CaloRingerLocker
CaloRingerLocker.OutputLevel = RingerOutputLevel
mlog.verbose('Changing %r output level to %s', CaloRingerLocker, RingerOutputLevel)

