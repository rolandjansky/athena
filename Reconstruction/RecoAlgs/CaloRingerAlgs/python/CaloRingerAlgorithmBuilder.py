# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import traceback
import CaloRingerAlgsConf
# We use egammaRec.Factories as a helper to instantiate CaloRingerAlgorithm
# handle
from GaudiKernel.GaudiHandles import PublicToolHandleArray, GaudiHandle, GaudiHandleArray
from egammaRec.Factories import AlgFactory, FcnWrapper
from AthenaCommon.Resilience import treatException
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from AthenaCommon.Include import include
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec
from CaloRingerAlgs.CaloRingerKeys import *
from egammaRec import egammaKeys

from CaloRingerTools.CaloRingerToolsConf import Ringer__CaloRingsBuilder, \
    Ringer__CaloAsymRingsBuilder, \
    Ringer__CaloRingerElectronsReader, \
    Ringer__CaloRingerPhotonsReader

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRinger_joboptions.py' )
mlog.info("Entering")


#---------------------------------------
# CaloRinger flags
from CaloRingerAlgs.CaloRingerFlags import jobproperties 
CaloRingerFlags = jobproperties.CaloRingerFlags

def egammaBuilderAvailable():
  " Return true if egammaBuilder is available."
  flag = False
  if rec.readRDO() or rec.readESD() and not rec.readxAOD():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()                     
    if hasattr(topSequence,'egamma'):
      flag = True
  return flag


def inputAvailable(inputType, inputKey):
  """ inputAvailable(inputType, inputKey) 
  Return true if inputType with inputKey is available."""
  flag = False
  from RecExConfig.ObjKeyStore import objKeyStore
  if objKeyStore.isInInput(inputType, inputKey):
    mlog.verbose(("objKeyStore does have in input the ContainerType %s with "
      "key %s."), inputType, inputKey)
    flag = True
  else:
    # Check if it is on metadata:
    # FIXME How can I do that using objKeyStore??
    from RecExConfig.InputFilePeeker import inputFileSummary
    metaItemList=inputFileSummary.get('metadata_itemsList')
    if ( '%s#%s' % (inputType, inputKey) ) in metaItemList:
      flag = True
      mlog.verbose(("metaItemList does have in input the ContainerType %s with "
        "key %s."), inputType, inputKey)
    else:
      mlog.verbose(("objKeyStore does NOT have in input the ContainerType %s with "
        "key %s."), inputType, inputKey)
  return flag


def checkBuildElectronCaloRings():
  """ Return true if it can build CaloRings for Electrons. Raise if it was
  expected to build electrons and it won't be possible. Return false if not
  asked to build."""
  if CaloRingerFlags.buildElectronCaloRings():
    if not inputAvailable(inputElectronType(), inputElectronKey()):

      # Try to force egammaBuilder startup if it is not already started:
      if not egammaBuilderAvailable(): 
        mlog.warning(("Requested to build ElectronCaloRings but egamma"
          " builder was not available. Trying to add egamma to joboptions.")
          )
        __enableEgamma()
      else:
        mlog.verbose(("Input not available in the file, but it is requested"
          " to be reconstructed so it will be build during reconstruction."))
    else:
      if egammaBuilderAvailable():
        mlog.verbose(("There already exists the egamma objects in file, but"
          " they will be updated during reconstruction to new ones."))
      else:
        mlog.verbose(("Ringer will use the egamma objects available "
          "in the file to build its patterns."))
    return True
  else: 
    mlog.verbose("It wasn't requested to build Electron CaloRings.")
    return False

def checkDoElectronIdentification():
  """ Return true if it can doElectronIdentification. Raise if it was
  expected to build electrons and it won't be possible. Return false if not
  asked to run electron identification."""
  if CaloRingerFlags.doElectronIdentification():
    if not CaloRingerFlags.buildElectronCaloRings():
      if not inputAvailable(outputRingSetConfType(), outputElectronRingSetsConfKey())    or \
         not ( inputAvailable(outputCaloRingsType(), outputElectronCaloRingsKey())       or \
               inputAvailable(outputCaloRingsType(), outputElectronCaloAsymRingsKey()) ) or \
         not ( inputAvailable(outputRingSetType(),   outputElectronRingSetsKey())        or \
               inputAvailable(outputRingSetType(),   outputElectronAsymRingSetsKey()) ):

        mlog.warning(("Requested to do Electron identification using "
          "Ringer discrimination, but one or more of the Ringer needed "
          "discrimination inputs are not available. We will request to "
          "build ElectronCaloRings using default configuration."))
        # In this case, the input file does not have CaloRings, we need to build them:
        CaloRingerFlags.buildElectronCaloRings = True
        if not checkBuildElectronCaloRings():
          mlog.error(("Couldn't add ElectronCaloRings reconstruction to "
            "joboptions."))
          raise  RuntimeError("Can't add ElectronCaloRings to reconstruction.")
      else:
        mlog.verbose("All Electron identification discrimination input "
        "available in the file.")
    else:
      mlog.verbose(("It will be used the ElectronCaloRings build within this "
          "reconstruction to make the classification."))
    return True
  else:
    return False

def checkBuildPhotonCaloRings():
  """ Return true if it can build CaloRings for Photons. Raise if it was
  expected to build electrons and it won't be possible. Return false if not
  asked to build."""
  if CaloRingerFlags.buildPhotonCaloRings():
    if not inputAvailable(inputPhotonType(), inputPhotonKey()):

      # Try to force egammaBuilder startup if it is not already started:
      if not egammaBuilderAvailable(): 
        mlog.warning(("Requested to build PhotonCaloRings but egamma"
          " builder was not available. Trying to add egamma to joboptions.")
          )
        __enableEgamma()
      else:
        mlog.verbose(("Input not available in the file. No problem: it will "
          " be reconstructed"))
    else:
      if egammaBuilderAvailable():
        mlog.verbose(("There already exists the egamma objects in file, but"
          " they will be updated during reconstruction to new ones."))
      else:
        mlog.verbose(("Ringer will use the egamma objects available "
          "in the file to build its patterns."))
    return True
  else: 
    mlog.verbose("It wasn't requested to build Photon CaloRings.")
    return False

def checkDoPhotonIdentification():
  """ Return true if it can doPhotonIdentification. Raise if it was
  expected to build electrons and it won't be possible. Return false if not
  asked to run electron identification."""
  if CaloRingerFlags.doPhotonIdentification():
    if not CaloRingerFlags.buildPhotonCaloRings():
      if not inputAvailable(outputRingSetConfType(), outputPhotonRingSetsConfKey())    or \
         not ( inputAvailable(outputCaloRingsType(), outputPhotonCaloRingsKey())       or \
               inputAvailable(outputCaloRingsType(), outputPhotonCaloAsymRingsKey()) ) or \
         not ( inputAvailable(outputRingSetType(),   outputPhotonRingSetsKey())        or \
               inputAvailable(outputRingSetType(),   outputPhotonAsymRingSetsKey()) ):

        mlog.warning(("Requested to do Photon identification using "
          "Ringer discrimination, but one or more of the Ringer needed "
          "discrimination inputs are not available. We will request to "
          "build PhotonCaloRings using default configuration."))
        # In this case, the input file does not have CaloRings, we need to build them:
        CaloRingerFlags.buildPhotonCaloRings = True
        if not checkBuildPhotonCaloRings():
          mlog.error(("Couldn't add PhotonCaloRings reconstruction to "
            "joboptions."))
          raise RuntimeError("Can't add PhotonCaloRings to reconstruction.")
      else:
        mlog.verbose("All Photon identification discrimination input "
          "available in the file.")
    else:
      mlog.verbose(("It will be used the PhotonCaloRings build within this "
          "reconstruction to make the classification."))
    return True
  else:
    return False

def __enableEgamma():
  " Local function to enable Egamma Reconstruction."
  # Set egamma flags to True
  rec.doEgamma = True
  from egammaRec.egammaRecFlags import jobproperties
  egammaRecFlags = jobproperties.egammaRecFlags
  egammaRecFlags.Enabled = True 
  egammaRecFlags.doEgammaCaloSeeded = True 
  egammaRecFlags.doEgammaForwardSeeded = False 
  include("egammaRec/egammaRec_jobOptions.py")
  if not egammaBuilderAvailable(): 
    mlog.error("Couldn't add egamma reconstruction to joboptions")
    raise RuntimeError("Coudln't add egamma reconstruction to joboptions.")

def getCaloRingerInputReaderTools():
  """ Returns a list with the CaloRinger tools to get the input objects to be
  decorated and pass them to their CaloRingerTools"""

  inputReaders = []

  try:
    if checkBuildElectronCaloRings() or checkDoElectronIdentification():
      from CaloRingerTools.CaloRingerInputReaderFactories \
          import CaloRingerElectronsReaderTool
      inputReaders.append(CaloRingerElectronsReaderTool())
      mlog.verbose("Added Ringer Electrons reader successfully.")
  except Exception:
    if CaloRingerFlags.buildElectronCaloRings():
      mlog.error(("It won't be possible to build ElectronCaloRings!"
        " Switching it off!"))
      CaloRingerFlags.buildElectronCaloRings = False
    if CaloRingerFlags.doElectronIdentification():
      mlog.error(("It won't be possible to do Electron identification!"
        " Switching it off!"))
      CaloRingerFlags.doElectronIdentification = False
    treatException("Could not set up Ringer Electrons reader!")

  try:
    if checkBuildPhotonCaloRings() or checkDoPhotonIdentification():
      from CaloRingerTools.CaloRingerInputReaderFactories \
          import CaloRingerPhotonsReaderTool
      inputReaders.append(CaloRingerPhotonsReaderTool())
      mlog.verbose("Added Ringer Photon reader successfully.")
  except Exception:
    if CaloRingerFlags.buildPhotonCaloRings():
      mlog.error(("It won't be possible to build PhotonCaloRings!"
        " Switching it off!"))
      CaloRingerFlags.buildPhotonCaloRings = False
    if CaloRingerFlags.doPhotonIdentification():
      mlog.error(("It won't be possible to build do Photon identification!"
        " Switching it off!"))
      CaloRingerFlags.doPhotonIdentification = False
    treatException("Could not set up CaloRingerAlgorithm for Photons!")

  return inputReaders

def getCaloRingerOutputs(inputReaders,addCaloRingsContainers=True,
    addRingSetsContainers=True):
  """   Returns a list with the Ringer__CaloRingerElectronsReader container names. 
      This should be used after the CaloRingerInputReaderTools is 
      used to call the CaloRIngerBuilderFactories."""

  inputReadersTools = transformGaudiToReaders(inputReaders)

  outputList = []
  # Loop over inputReaderTools and add their CaloRingsBuilder outputs:
  for reader in inputReadersTools:
    if CaloRingerFlags.useAsymBuilder():
      crBuilder = CfgMgr.Ringer__CaloAsymRingsBuilder(reader.crBuilder.getName())
    else:
      crBuilder = CfgMgr.Ringer__CaloRingsBuilder(reader.crBuilder.getName())
    if addCaloRingsContainers:
      outputList.append(crBuilder.CaloRingsContainerName)
    if addRingSetsContainers:
      outputList.append(crBuilder.RingSetContainerName)
  return outputList

def getCaloRingerConfOutputs():
  """   Returns a list with the RingSetConfWriter container names.  """

  outputList = []
  if CaloRingerFlags.buildElectronCaloRings():
    outputList.append(outputElectronRingSetsConfKey())
  if CaloRingerFlags.buildPhotonCaloRings():
    outputList.append(outputPhotonRingSetsConfKey())
  return outputList

def transformGaudiToReaders(inputReaders):
  "Transform GaudiHandle into a list of configurable tools"
  inputReaderTools = []
  if isinstance(inputReaders,(GaudiHandle,GaudiHandleArray)):
    if not isinstance(inputReaders,list):
      inputReaders = [inputReaders]
    for reader in inputReaders:
      readerType = reader.getType()
      if readerType == 'Ringer::CaloRingerElectronsReader':
        inputReaderTools.append(CfgMgr.Ringer__CaloRingerElectronsReader(reader.getName()))
      elif readerType == 'Ringer::CaloRingerPhotonsReader':
        inputReaderTools.append(CfgMgr.Ringer__CaloRingerPhotonsReader(reader.getName()))
      else:
        mlog.error("Unknown inputReader type %s.", readerType)
        raise RuntimeError("GaudiHandle is not of inputReader type.")
  else:
    inputReaderTools = inputReaders
  return inputReaderTools


def getCaloRingerBuilders(inputReaders):
  """   Returns a list with the __CaloRingerElectronsReader container names.  """

  inputReadersTools = transformGaudiToReaders(inputReaders)

  builders = PublicToolHandleArray()
  builders += [reader.crBuilder for reader in inputReadersTools \
      if reader.crBuilder.getType() in ("Ringer::CaloRingsBuilder", \
                                        "Ringer::CaloAsymRingsBuilder")]
  mlog.verbose("Found the following builders: [%s] ", \
      ', '.join(map(str,[builder.getName() for builder in builders])) )
  return builders

def getSelectors(inputReaders):
  """   Returns a list with the __CaloRingerElectronsReader container names.  """

  inputReaderTools = transformGaudiToReaders(inputReaders)

  selectors = PublicToolHandleArray()
  for reader in inputReadersTools:
    try:
      selectors += [selector for selector in reader.ElectronSelectors \
          if selector.getType() == "Ringer::AsgElectronRingerSelector"]
    except Exception:
      pass
    try:
      selectors += [selector for selector in reader.PhotonSelectors \
          if selector.getType() == "Ringe::AsgPhotonRingerSelector"]
    except Exception:
      pass
  return selectors

def postponeEgammaLock(ringerAlg):
  "Postpone egamma locker tool to execute after the last tool on toolList"
  from AthenaCommon.AlgSequence import AlgSequence, dumpSequence
  topSequence = AlgSequence()
  from egammaRec.egammaLocker import egammaLocker
  # Make sure we add it before streams:
  foundRingerAlg = False
  foundEgammaLocker = False
  for pos, alg in enumerate(topSequence):
    if ringerAlg.getName() == alg.getName():
      foundRingerAlg = True
      ringerPos = pos
    if 'egLocker' == alg.getName():
      egammaLockerPos = pos
      egammaLockerAlg = alg
      foundEgammaLocker = True
    if foundRingerAlg and foundEgammaLocker:
      if ringerPos > egammaLockerPos:
        topSequence.remove(egammaLockerAlg)
        topSequence.insert(ringerPos, egammaLockerAlg)
      break
  else:
    mlog.debug("Couldn't find egammaLocker or RingerAlg.")


def removeFromToolSvc( tool ):
  "removeFromToolSvc( tool ) --> remove tool from ToolSvc"
  from AthenaCommon.AppMgr import ToolSvc
  if hasattr(ToolSvc, tool.getName()):
    print "Removing tool %s from ToolSvc." % tool.getName()
    ToolSvc.remove(tool.getName())

def removeFromTopSequence( alg ):
  "removeFromTopSequence( alg ) --> remove alg from TopSequence"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if alg in topSequence:
    print "Removing Algorithm %s from topSequence." % alg.getName()
    # If this doesnt work, change by alg.getName()
    topSequence.remove( alg )


class CaloRingerAlgorithmBuilder ( Configured ):

  _caloRingerAlg = None
  _confWriter = None
  _output = {}
  _eventOutputs = {}
  _outputMetaData = {}

  def getCaloRingerAlgHandle(self):
    "Return the CaloRinger main algorithm handle."
    return self._caloRingerAlg

  def getMetaDataWriterHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._confWriter

  def getConfigWriterHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._confWriter

  def getLastWriterHandle(self):
    "Return last CaloRinger algorithm to write into StoreGate Services."
    return self._confWriter

  def getInputReadersHandles(self):
    "Return our holden CaloRings builder handles"
    return self._caloRingerAlg.inputReaderTools

  def getCaloRingsBuilderHandles(self):
    "Return our holden CaloRings builder handles"
    return getCaloRingerBuilders(self._caloRingerAlg.inputReaderTools)

  def getRingerSelectorHandles(self):
    "Return our holden CaloRings builder handles"
    return getSelectors(self._caloRingerAlg.inputReaderTools)

  def eventOutputs(self):
    "Return the event store gate outputs from this builder"
    return self._eventOutputs

  def metaDataOutputs(self):
    "Return the metadata outputs from this builder"
    return self._outputMetaData

  def __init__( self, disable=False,
      ignoreExistingDataObject=CaloRingerFlags.ignoreRingerExistingDataObject(), 
      ignoreConfigError=False ):
    "Call Configured init, but with new default ignoreExistingDataObject"
    Configured.__init__(self,disable,ignoreExistingDataObject,ignoreConfigError)

  def configure(self):
    "This routine will be called when object is initialized"

    mlog = logging.getLogger( 'CaloRingerAlgorithmBuilder::configure:%s:' \
        % self.__class__.__name__.replace( ".", '_' )  )
    mlog.info('entering')
    try:

      # Instantiate the main algorithm:
      MainCaloRingerAlgorithm = AlgFactory(CaloRingerAlgsConf.Ringer__CaloRingerAlgorithm, 
          name = "MainCaloRingerAlgorithm",
          inputReaderTools = FcnWrapper(getCaloRingerInputReaderTools))

      self._caloRingerAlg = MainCaloRingerAlgorithm()

      if CaloRingerFlags.buildCaloRingsOn():
        postponeEgammaLock(self._caloRingerAlg)
        pass

      # Check if CaloRingerAlgorithm has readers:
      if not self._caloRingerAlg.inputReaderTools:
        raise RuntimeError(("Cannot instantiate CaloRingerAlgorithm " 
          "without readers."))

      # Instantiate the MetaDataWriter:
      try:
        MetaDataWriter = AlgFactory(CaloRingerAlgsConf.Ringer__xAODRingSetConfWriter, 
            name = "xAODRingSetConfWriter",
            CaloRingsBuilderTools = self.getCaloRingsBuilderHandles(),
            RingSetConfContainerNames = getCaloRingerConfOutputs())
        self._confWriter = MetaDataWriter()
      except Exception:
        removeFromTopSequence(self._confWriter)
        mlog.error(("Could not get handle to xAODRingSetConfWriter."
            " Reason:\n %s"),traceback.format_exc())
        raise RuntimeError("Couldn't set xAODRingSetConfWriter")

      if CaloRingerFlags.buildCaloRingsOn():
        self._eventOutputs = { outputCaloRingsType() : \
    getCaloRingerOutputs(self._caloRingerAlg.inputReaderTools,addRingSetsContainers=False), \
                               outputRingSetType() : \
    getCaloRingerOutputs(self._caloRingerAlg.inputReaderTools,addCaloRingsContainers=False) \
                             }
        self._output.update(self._eventOutputs)
        self._outputMetaData = {outputRingSetConfType() : getCaloRingerConfOutputs() }
        self._output.update(self._outputMetaData)

      # Check for existing output:
      self.checkExistingOutput()

      if self._existingOutput and not self.ignoreExistingDataObject():
        raise RuntimeError(("Already existing output and not set flag to"
            "ignoreExistingDataObject."))

    except Exception:
      removeFromTopSequence(self._caloRingerAlg)
      CaloRingerFlags.Enabled = False
      self._disabled = True
      treatException(("Could not get handle to CaloRingerAlgorithm."
          " Reason:\n%s") % traceback.format_exc())
      
      return False

    return True


