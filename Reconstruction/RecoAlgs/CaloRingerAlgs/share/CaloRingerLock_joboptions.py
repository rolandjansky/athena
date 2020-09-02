__doc__ = "Lock containers in ESD/AOD ItemList using the definitions from CaloRingerKeys"

import traceback

from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import treatException

mlog = logging.getLogger( 'CaloRingerLock_joboptions' )
mlog.info("Entering")

from RecExConfig.RecFlags import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
from CaloRingerAlgs.CaloRingerKeys import CaloRingerKeysDict

# Get CaloRinger algorithm handles
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder
CRAlgBuilder = CaloRingerAlgorithmBuilder()
from CaloRingerAlgs.CaloRingerMetaDataBuilder import CaloRingerMetaDataBuilder
CRMetaDataBuilder = CaloRingerMetaDataBuilder(disable=True)

if rec.doESD() and caloRingerFlags.buildCaloRingsOn() and CRAlgBuilder.usable():
  LastCRWriter = CRMetaDataBuilder.getLastWriterHandle()
  CRMainAlg = CRAlgBuilder.getCaloRingerAlgHandle()
  CRMetaDataWriterAlg = CRMetaDataBuilder.getMetaDataWriterHandle()
  try:
    # FIXME: It seems that the python implementation takes a lot of memory 
    # (https://its.cern.ch/jira/browse/ATLASRECTS-2769?filter=-1),
    # replace it as egamma is doing to use a C++ implementation
    pass
    # Make sure we add it before streams:
    for pos, alg in enumerate(topSequence):
      if LastCRWriter.getName() == alg.getName():
        from CaloRingerAlgs.CaloRingerLocker import CaloRingerLocker
        CRLocker = CaloRingerLocker(name = "CaloRingerLocker", \
                doElectron = caloRingerFlags.buildElectronCaloRings(), \
                doPhoton = caloRingerFlags.buildPhotonCaloRings(), \
                EvtStoreName = CRMainAlg.EvtStore.getName(), \
                MetaDataStoreName = CRMetaDataWriterAlg.MetaDataStore.getName(), \
                CaloRingerDict = CRAlgBuilder.eventOutputs(), \
                CaloRingerMetaDict = CRMetaDataBuilder.metaDataOutputs() \
                )
        topSequence.insert(pos + 1, CRLocker)
        mlog.verbose("Successfully added CaloRingerLocker to TopSequence.")
        break
    else:
      treatException("Could not find CaloRingerDecorator algorithm.")
  except Exception,e:
    treatException("Could not set up CaloRingerLocker. Switched off ! Reason:\n %s" % traceback.format_exc())

  from CaloRingerAlgs.CaloRingerLocker import CaloRingerLocker
  CaloRingerLocker.OutputLevel = RingerOutputLevel
  mlog.verbose('Changing %r output level to %s', CaloRingerLocker, RingerOutputLevel)

