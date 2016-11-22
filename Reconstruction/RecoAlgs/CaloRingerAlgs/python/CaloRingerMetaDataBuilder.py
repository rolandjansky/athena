# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import traceback
import CaloRingerAlgsConf
from AthenaCommon.Resilience import treatException
from RecExConfig.Configured import Configured
from CaloRingerAlgs.CaloRingerKeys import *
from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import removeFromTopSequence
from egammaRec.Factories import AlgFactory, FcnWrapper

# We assume that someone tried to configure the crBuild propertly before...
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder
crBuilder = CaloRingerAlgorithmBuilder()


from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRingerMetaDataBuilder.py' )
mlog.info("Entering")


def metaDataInputAvailable(inputType, inputKey):
  """ metaDataInputAvailable(inputType, inputKey) 
  Return true if inputType with inputKey is available."""
  # Check if it is on metadata:
  # FIXME How can I do that using objKeyStore??
  flag = False
  from RecExConfig.InputFilePeeker import inputFileSummary
  metaItemList=inputFileSummary.get('metadata_itemsList')
  if ( '%s#%s' % (inputType, inputKey) ) in metaItemList:
    flag = True
    mlog.verbose(("metaItemList does have ContainerType input %s with "
      "key %s."), inputType, inputKey)
  else:
    mlog.verbose(("metaItemList does NOT have ContainerType input %s with "
      "key %s."), inputType, inputKey)
  return flag

def getCaloRingerConfOutputs():
  """   Returns a list with the RingSetConfWriter container names.  """

  outputList = []
  overwriting = False
  builderNames = [tool.getName() for tool in crBuilder.getCaloRingerBuilderHandles()]

  if any(['Electron' in builderName for builderName in builderNames]):
    outputList.append(outputElectronRingSetsConfKey())
    electronMetaAvailable = metaDataInputAvailable(outputRingSetConfType(), outputElectronRingSetsConfKey())
    if electronMetaAvailable:
      overwriting = True
  if any(['Photon' in builderName for builderName in builderNames]):
    outputList.append(outputPhotonRingSetsConfKey())
    photonMetaAvailable = metaDataInputAvailable(outputRingSetConfType(), outputPhotonRingSetsConfKey())
    if photonMetaAvailable:
      overwriting = True
  return outputList, overwriting


class CaloRingerMetaDataBuilder ( Configured ):

  _confWriter = None
  _outputMetaData = {}
  _output = {}

  def getLastWriterHandle(self):
    "Return last CaloRinger algorithm to write into StoreGate Services."
    return self._confWriter

  def metaDataOutputs(self):
    "Return the metadata outputs from this builder"
    return self._outputMetaData

  def getMetaDataWriterHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._confWriter

  def getConfigWriterHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._confWriter

  def __init__( self, disable=False,
      ignoreExistingDataObject=caloRingerFlags.ignoreRingerExistingDataObject(), 
      ignoreConfigError=False ):
    "Call Configured init, but with new default ignoreExistingDataObject"
    Configured.__init__(self,disable,ignoreExistingDataObject,ignoreConfigError)

  def configure(self):
    "This method will be called when object is initialized"

    mlog = logging.getLogger( 'CaloRingerMetaDataBuilder::configure:%s:' \
        % self.__class__.__name__.replace( ".", '_' )  )
    mlog.info('entering')

    # Instantiate the MetaDataWriter:
    try:
      outputs, overwrite = getCaloRingerConfOutputs()
      MetaDataWriter = AlgFactory(CaloRingerAlgsConf.Ringer__xAODRingSetConfWriter, 
          name = "xAODRingSetConfWriter",
          CaloRingsBuilderTools = crBuilder.getCaloRingerBuilderHandles(),
          RingSetConfContainerNames = outputs,
          )
      self._confWriter = MetaDataWriter()
      self._outputMetaData = {outputRingSetConfType() : outputs }
      self._output.update(self._outputMetaData)

      # Check for existing output:
      self.checkExistingOutput()

      if not self.ignoreExistingDataObject() and overwrite:
        raise RuntimeError(("Already existing input will be overwriten and not set flag to"
            "ignoreExistingDataObject."))

    except Exception:
      removeFromTopSequence(self._confWriter)
      mlog.error(("Could not get handle to xAODRingSetConfWriter."
          " Reason:\n %s"),traceback.format_exc())
      treatException(("Couldn't set xAODRingSetConfWriter."
          " Reason:\n%s") % traceback.format_exc())
      return False
    return True

