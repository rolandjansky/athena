__doc__ = "JobOption fragment to add CaloRinger algorithm to run."

from AthenaCommon.Logging import logging

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder
from CaloRingerAlgs.CaloRingerMetaDataBuilder import CaloRingerMetaDataBuilder
from egammaRec.egammaRecFlags import jobproperties

mlog = logging.getLogger( 'CaloRinger_joboptions.py' )
mlog.info('Entering')

ringerOutputLevel = caloRingerFlags.OutputLevel()

# Check if we have our other algorithm dependencies enabled, otherwise disable
# ringer:
if caloRingerFlags.buildCaloRingsOn():
  # Check egamma related objects:
  if not rec.doEgamma() or not jobproperties.egammaRecFlags.doEgammaCaloSeeded():
    if caloRingerFlags.buildElectronCaloRings():
      caloRingerFlags.buildElectronCaloRings.set_Value( False )
      caloRingerFlags.doElectronIdentification.set_Value( False )
      if not caloRingerFlags.buildElectronCaloRings():
        mlog.info('No egamma builder available... disabling ElectronCaloRings reconstruction and electron selection.')
    if caloRingerFlags.buildPhotonCaloRings():
      caloRingerFlags.buildPhotonCaloRings.set_Value( False )
      #caloRingerFlags.doPhotonIdentification.set_Value( False )
      if not caloRingerFlags.buildPhotonCaloRings():
        mlog.info('No egamma builder available... disabling PhotonCaloRings reconstruction and electron selection.')


# To build CaloRings it is required to have doESD flag set to on.
if not rec.doESD() and caloRingerFlags.buildCaloRingsOn():
  caloRingerFlags.buildCaloRingsOn.set_Value( False )
  caloRingerFlags.doIdentificationOn.set_Value( False )
  if not caloRingerFlags.buildCaloRingsOn():
    mlog.info('Job will not build CaloRings as we are not doing ESD.')
  if not caloRingerFlags.doIdentificationOn():
    mlog.info('Job will not run Ringer selectors as we are not doing ESD.')

if caloRingerFlags.buildCaloRingsOn() or caloRingerFlags.doIdentificationOn():

  # Add main algorithm builder
  CRAlgBuilder = CaloRingerAlgorithmBuilder()

  # TODO This code can be cleaned by using the flag on the Factories configuration

  # Make sure all algoritms have the ringerOutputLevel
  if CRAlgBuilder.usable():
    # Get the main logger algorithm
    mainAlg = CRAlgBuilder.getCaloRingerAlgHandle()
    if mainAlg:
      # Change the main algorithm output level
      mlog.verbose('Changing %r output level to %s', mainAlg, ringerOutputLevel)
      mainAlg.OutputLevel = ringerOutputLevel

    # Get the builder handles
    inputReaderHandles = CRAlgBuilder.getInputReaders()
    for reader in inputReaderHandles:
      # Change builders output level
      mlog.verbose('Changing %r output level to %s', reader, ringerOutputLevel)
      reader.OutputLevel = ringerOutputLevel

    # Get the builder handles
    builderHandles = CRAlgBuilder.getCaloRingerBuilders()
    for builder in builderHandles:
      # Change builders output level
      mlog.verbose('Changing %r output level to %s', builder, ringerOutputLevel)
      builder.OutputLevel = ringerOutputLevel

    # Get the selector handles
    selectorHandles = CRAlgBuilder.getRingerSelectors()
    for selector in selectorHandles:
      # Change the builders output level
      mlog.verbose('Changing %r output level to %s', selector, ringerOutputLevel)
      selector.OutputLevel = ringerOutputLevel
else:
  # Otherwise we disable the main algorithm
  CRAlgBuilder = CaloRingerAlgorithmBuilder( disable = True )

from PyUtils.MetaReaderPeeker import convert_metadata_items, metadata
metaItemDict = convert_metadata_items(layout='dict')

if CRAlgBuilder.usable() or (metaItemDict and any( ['RingSetConf' in key for key in metaItemDict ] )):
  MetaDataBuilder = CaloRingerMetaDataBuilder(disable=True)

  # Make sure all MetaData algoritms have the ringerOutputLevel
  if MetaDataBuilder.usable():
    # Get the ringer configuration writter handle
    configWriter = MetaDataBuilder.getConfigWriterHandle()

    if configWriter:
      # Change its output level
      mlog.verbose('Changing %r output level to %s', configWriter, ringerOutputLevel)
      configWriter.OutputLevel = ringerOutputLevel
else:
  # Otherwise we disable the metadata algorith:
  MetaDataBuilder = CaloRingerMetaDataBuilder( disable = True )

