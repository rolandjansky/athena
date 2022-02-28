# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import traceback
from . import CaloRingerAlgsConf
# We use egammaRec.Factories as a helper to instantiate CaloRingerAlgorithm
# handle
from GaudiKernel.GaudiHandles import PublicToolHandleArray, GaudiHandle, GaudiHandleArray
from egammaRec.Factories import AlgFactory, FcnWrapper
from AthenaCommon.Resilience import treatException
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from CaloRingerAlgs import CaloRingerKeys as ringer

from CaloRingerTools.CaloRingerToolsConf import Ringer__CaloRingsBuilder, Ringer__CaloAsymRingsBuilder # noqa: F401

mlog = logging.getLogger( 'CaloRingerAlgorithmBuilder.py' )
mlog.info("Entering")


#---------------------------------------
# CaloRinger flags
from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags

def inputAvailable(inputType, inputKey):
  """ inputAvailable(inputType, inputKey) 
  Return true if inputType with inputKey is available."""
  flag = False
  from RecExConfig.ObjKeyStore import objKeyStore
  if objKeyStore.isInInput(inputType, inputKey):
    mlog.verbose(("objKeyStore does have ContainerType input %s with "
      "key %s."), inputType, inputKey)
    flag = True
  else:
    mlog.verbose(("objKeyStore does NOT ContainerType input %s with "
      "key %s."), inputType, inputKey)
  return flag


def checkBuildElectronCaloRings():
  """ Return true if it can build CaloRings for Electrons. Raise if it was
  expected to build electrons and it won't be possible. Return false if not
  asked to build."""
  if caloRingerFlags.buildElectronCaloRings():
    if not rec.doESD():
      mlog.info("Turning off ringer algorithm electron reconstruction since not doing ESD.")
      caloRingerFlags.buildElectronCaloRings = False
      return False
    if not inputAvailable(ringer.inputElectronType(), ringer.inputElectronKey()):
      if not ConfigFlags.Egamma.doCentral:
        mlog.warning(("Requested to build ElectronCaloRings but egamma"
          " calo seeded is off. Deactivating ElectronCaloRings and electron selection.")
          )
        caloRingerFlags.buildElectronCaloRings = False
        caloRingerFlags.doElectronIdentification = False
        return False
      else:
        mlog.verbose(("Input not available in the file, but it is requested"
          " to be reconstructed so it will be build during reconstruction."))
    else:
      if ConfigFlags.Egamma.doCentral:
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
  if caloRingerFlags.doElectronIdentification():
    if not caloRingerFlags.buildElectronCaloRings():
      if not ( inputAvailable(ringer.outputCaloRingsType(), ringer.outputElectronCaloRingsKey())       or \
               inputAvailable(ringer.outputCaloRingsType(), ringer.outputElectronCaloAsymRingsKey()) ) or \
         not ( inputAvailable(ringer.outputRingSetType(),   ringer.outputElectronRingSetsKey())        or \
               inputAvailable(ringer.outputRingSetType(),   ringer.outputElectronAsymRingSetsKey()) ):

        mlog.warning(("Requested to do Electron identification using "
          "Ringer discrimination, but one or more of the Ringer needed "
          "discrimination inputs are not available. We will request to "
          "build ElectronCaloRings using default configuration."))
        # In this case, the input file does not have CaloRings, we need to build them:
        caloRingerFlags.buildElectronCaloRings = True
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
  if caloRingerFlags.buildPhotonCaloRings():
    if not rec.doESD():
      mlog.info("Turning off ringer algorithm photon reconstruction since not doing ESD.")
      caloRingerFlags.buildPhotonCaloRings = False
      return False
    if not inputAvailable(ringer.inputPhotonType(), ringer.inputPhotonKey()):

      # Deadtivate photon calo rings if egamma calo seeded is off:
      if not ConfigFlags.Egamma.doCentral:
        mlog.warning(("Requested to build PhotonCaloRings but egamma"
          " calo seeded is off. Deactivating buildPhotonCaloRings.")
            )
        caloRingerFlags.buildPhotonCaloRings = False
        #caloRingerFlags.doPhotonIdentification = False
        return False
      else:
        mlog.verbose(("Input not available in the file. No problem: it will "
          " be reconstructed"))
    else:
      if ConfigFlags.Egamma.doCentral:
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
  if caloRingerFlags.doPhotonIdentification():
    if not caloRingerFlags.buildPhotonCaloRings():
      if not ( inputAvailable(ringer.outputCaloRingsType(), ringer.outputPhotonCaloRingsKey())       or \
               inputAvailable(ringer.outputCaloRingsType(), ringer.outputPhotonCaloAsymRingsKey()) ) or \
         not ( inputAvailable(ringer.outputRingSetType(),   ringer.outputPhotonRingSetsKey())        or \
               inputAvailable(ringer.outputRingSetType(),   ringer.outputPhotonAsymRingSetsKey()) ):

        mlog.warning(("Requested to do Photon identification using "
          "Ringer discrimination, but one or more of the Ringer needed "
          "discrimination inputs are not available. We will request to "
          "build PhotonCaloRings using default configuration."))
        # In this case, the input file does not have CaloRings, we need to build them:
        caloRingerFlags.buildPhotonCaloRings = True
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
    if caloRingerFlags.buildElectronCaloRings():
      mlog.error(("It won't be possible to build ElectronCaloRings!"
        " Switching it off!"))
      caloRingerFlags.buildElectronCaloRings = False
    if caloRingerFlags.doElectronIdentification():
      mlog.error(("It won't be possible to do Electron identification!"
        " Switching it off!"))
      caloRingerFlags.doElectronIdentification = False
    treatException("Could not set up Ringer Electrons reader!")

  try:
    if checkBuildPhotonCaloRings() or checkDoPhotonIdentification():
      from CaloRingerTools.CaloRingerInputReaderFactories \
          import CaloRingerPhotonsReaderTool
      inputReaders.append(CaloRingerPhotonsReaderTool())
      mlog.verbose("Added Ringer Photon reader successfully.")
  except Exception:
    if caloRingerFlags.buildPhotonCaloRings():
      mlog.error(("It won't be possible to build PhotonCaloRings!"
        " Switching it off!"))
      caloRingerFlags.buildPhotonCaloRings = False
    if caloRingerFlags.doPhotonIdentification():
      mlog.error(("It won't be possible to build do Photon identification!"
        " Switching it off!"))
      caloRingerFlags.doPhotonIdentification = False
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
    if caloRingerFlags.useAsymBuilder():
      crBuilder = CfgMgr.Ringer__CaloAsymRingsBuilder(reader.crBuilder.getName())
    else:
      crBuilder = CfgMgr.Ringer__CaloRingsBuilder(reader.crBuilder.getName())
    if addCaloRingsContainers:
      outputList.append(crBuilder.CaloRingsContainerName)
    if addRingSetsContainers:
      outputList.append(crBuilder.RingSetContainerName)
  return outputList

def transformGaudiTo(inputList, typeList):
  """Transform GaudiHandle into a list of configurable tools of the desired type.
     if input is not a Gaudi type return input.
  """
  outputList = []
  if isinstance(inputList,(GaudiHandle,GaudiHandleArray)):
    if not isinstance(inputList,list):
      inputList = [inputList]
    for input_ in inputList:
      inputType = input_.getType()
      for desiredType in typeList:
        if inputType == desiredType:
          cls = getattr(CfgMgr, inputType.replace(':','_'))
          try:
            conf = cls.configurables[ input_.getName() ]
          except KeyError:
            conf = cls.allConfigurables[ input_.getName() ]
          outputList.append(conf)
          break
      else:
        treatException("Unknown input type is %s and does not match searched type list %r.", inputType, typeList)
  return outputList


def transformGaudiToReaders(inputList):
  "Transform GaudiHandle into a list of configurable input readers"
  return transformGaudiTo(inputList, ['Ringer::CaloRingerElectronsReader', 
                                      'Ringer::CaloRingerPhotonsReader'])

def transformGaudiToBuilders(inputList):
  "Transform GaudiHandle into a list of configurable builders"
  return transformGaudiTo(inputList, ['Ringer::CaloRingsBuilder', 
                                      'Ringer::CaloAsymRingsBuilder'])

def transformGaudiToSelectors(inputList):
  "Transform GaudiHandle into a list of configurable builders"
  return transformGaudiTo(inputList, ['Ringer::AsgElectronRingerSelector', 
                                      ])

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
  for reader in inputReaderTools:
    try:
      selectors += [selector for selector in reader.ElectronSelectors \
          if selector.getType() == "Ringer::AsgElectronRingerSelector"]
    except AttributeError:
      pass
    try:
      selectors += [selector for selector in reader.PhotonSelectors \
          if selector.getType() == "Ringer::AsgPhotonRingerSelector"]
    except AttributeError:
      pass
  return selectors

def postponeEgammaLock(ringerAlg):
  "Postpone egamma locker tool to execute after the last tool on toolList"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
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
    print ("Removing tool %s from ToolSvc." % tool.getName())
    ToolSvc.remove(tool.getName())

def removeFromTopSequence( alg ):
  "removeFromTopSequence( alg ) --> remove alg from TopSequence"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if alg in topSequence:
    print ("Removing Algorithm %s from topSequence." % alg.getName())
    # If this doesnt work, change by alg.getName()
    topSequence.remove( alg )


class CaloRingerAlgorithmBuilder ( Configured ):

  _caloRingerAlg = None
  _confWriter = None
  _output = {}
  _eventOutputs = {}

  def getCaloRingerAlgHandle(self):
    "Return the CaloRinger main algorithm handle."
    return self._caloRingerAlg

  def getInputReadersHandles(self):
    "Return our holden CaloRinger builder handles"
    return self._caloRingerAlg.inputReaderTools if self._caloRingerAlg \
                                                else PublicToolHandleArray([])

  def getCaloRingerBuilderHandles(self):
    "Return our holden CaloRinger builder handles"
    return getCaloRingerBuilders(self._caloRingerAlg.inputReaderTools) if self._caloRingerAlg \
                                                                       else PublicToolHandleArray([])

  def getRingerSelectorHandles(self):
    "Return our holden CaloRinger builder handles"
    return getSelectors(self._caloRingerAlg.inputReaderTools) if self._caloRingerAlg \
                                                              else PublicToolHandleArray([]) 

  def getInputReaders(self):
    "Return our holden CaloRinger builder handles"
    return transformGaudiToReaders(self.getInputReadersHandles())

  def getCaloRingerBuilders(self):
    "Return our holden CaloRinger builder handles"
    return transformGaudiToBuilders(self.getCaloRingerBuilderHandles())

  def getRingerSelectors(self):
    "Return our holden CaloRinger builder handles"
    return transformGaudiToSelectors(self.getRingerSelectorHandles())

  def eventOutputs(self):
    "Return the event store gate outputs from this builder"
    return self._eventOutputs

  def __init__( self, disable=False,
      ignoreExistingDataObject=caloRingerFlags.ignoreRingerExistingDataObject(), 
      ignoreConfigError=False ):
    "Call Configured init, but with new default ignoreExistingDataObject"
    Configured.__init__(self,disable,ignoreExistingDataObject,ignoreConfigError)

  def configure(self):
    "This method will be called when object is initialized"

    mlog = logging.getLogger( 'CaloRingerAlgorithmBuilder::configure:%s:' \
        % self.__class__.__name__.replace( ".", '_' )  )
    mlog.info('entering')
    try:

      # Instantiate the main algorithm:
      MainCaloRingerAlgorithm = AlgFactory(CaloRingerAlgsConf.Ringer__CaloRingerAlgorithm, 
          name = "MainCaloRingerAlgorithm",
          inputReaderTools = FcnWrapper(getCaloRingerInputReaderTools))

      self._caloRingerAlg = MainCaloRingerAlgorithm()

      if caloRingerFlags.buildCaloRingsOn():
        # Egamma locker not being used anymore.
        #postponeEgammaLock(self._caloRingerAlg)
        pass

      # Check if CaloRingerAlgorithm has readers:
      if not self._caloRingerAlg.inputReaderTools:
        raise RuntimeError(("Cannot instantiate CaloRingerAlgorithm " 
          "without readers."))

      if caloRingerFlags.buildCaloRingsOn():
        self._eventOutputs = { ringer.outputCaloRingsType() : \
    getCaloRingerOutputs(self._caloRingerAlg.inputReaderTools,addRingSetsContainers=False), \
                               ringer.outputRingSetType() : \
    getCaloRingerOutputs(self._caloRingerAlg.inputReaderTools,addCaloRingsContainers=False) \
                             }
        self._output.update(self._eventOutputs)

      # Check for existing output:
      self.checkExistingOutput()

      if not self.ignoreExistingDataObject()                                                     \
         and ( (  caloRingerFlags.buildElectronCaloRings()                                   and \
            ( inputAvailable(ringer.outputCaloRingsType(), ringer.outputElectronCaloRingsKey())            or  \
                   inputAvailable(ringer.outputCaloRingsType(), ringer.outputElectronCaloAsymRingsKey()) ) or  \
            ( inputAvailable(ringer.outputRingSetType(), ringer.outputElectronRingSetsKey())             or  \
                   inputAvailable(ringer.outputRingSetType(), ringer.outputElectronAsymRingSetsKey()) ) )      \
         or  ( caloRingerFlags.buildPhotonCaloRings()                                        and \
            ( inputAvailable(ringer.outputCaloRingsType(), ringer.outputPhotonCaloRingsKey())              or  \
                   inputAvailable(ringer.outputCaloRingsType(), ringer.outputPhotonCaloAsymRingsKey()) )   or  \
            ( inputAvailable(ringer.outputRingSetType(), ringer.outputPhotonRingSetsKey())               or  \
                   inputAvailable(ringer.outputRingSetType(), ringer.outputPhotonAsymRingSetsKey()) ) ) ):
        raise RuntimeError(("Already existing input will be overwriten and not set Ringer flag "
            "ignoreExistingDataObject."))

    except Exception:
      removeFromTopSequence(self._caloRingerAlg)
      caloRingerFlags.Enabled = False
      self._disabled = True
      treatException(("Could not get handle to CaloRingerAlgorithm."
          " Reason:\n%s") % traceback.format_exc())
      return False

    return True


