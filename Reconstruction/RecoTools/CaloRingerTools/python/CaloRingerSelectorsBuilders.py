# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import traceback

from GaudiKernel.GaudiHandles import GaudiHandle, PublicToolHandleArray
from AthenaCommon.Resilience import treatException

from CaloRingerAlgs.CaloRingerAlgorithmBuilder import removeFromToolSvc

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags

mlog = logging.getLogger( 'CaloRingerSelectorsBuilders.py' )
mlog.info("Entering")

import cppyy
try:
  cppyy.load_library('libRingerSelectorToolsDict')
except RuntimeError as e:
  mlog.error("Couldn't load RingerSelectorTools dictionary from cppyy.")
  raise RuntimeError(e)

from ROOT import Ringer 

class CaloRingerElectronSelectorsBuilder ( Configured ):

  _looseSelector = GaudiHandle('')
  _mediumSelector = GaudiHandle('')
  _tightSelector = GaudiHandle('')

  _customSelectors = PublicToolHandleArray()

  _allSelectors = PublicToolHandleArray()

  def getLooseSelectorHandle(self):
    "Return the CaloRinger main algorithm handle."
    return self._looseSelector

  def getMediumSelectorHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._mediumSelector

  def getTightSelectorHandle(self):
    "Return the CaloRinger MetaData configuration writer algorithm handle."
    return self._tightSelector

  def getCustomSelectorHandles(self):
    "Return custom selector handles."
    return self._customSelectors

  def getAllSelectorHandles(self):
    "Return all CaloRinger algorithm to write into StoreGate Services."
    return self._allSelectors

  def getAllSelectorNames(self):
    "Return all selector names"
    selectors = self._allSelectors
    return [selector.getName() for selector in selectors]

  def addCustomElectronSelectors(self, customElectronSelectors):
    """Add custom electron selector to reconstruction. It will work only if it is added
    before the CaloRingerAlgorithmBuilder is instatiated, otherwise it will only
    add to its configuration the default tool handles added during this configuration tool
    default setup.

    It will only be successful if it is passed a electron selector tool handle."""
    try:
      if not isinstance(customElectronSelectors,(list)):
        customElectronSelectors = list(customElectronSelectors)
      for selector in customElectronSelectors:
        if not selector.getType() is "Ringer::AsgElectronRingerSelector":
          raise ValueError(("Selector must be of type Ringer::AsgElectronRingerSelector"
                            "and is of type %s"),selector.getType())
      self._customSelectors.extend( customElectronSelectors )
      self._allSelectors.extend( customElectronSelectors )
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc += customElectronSelectors
    except Exception:
      mlog.error("Could not add custom electron selector. Reason: \n%s",
                 traceback.format_exc())

  def configure(self):
    "This routine will be called when object is initialized"

    conflog = logging.getLogger( 'CaloRingerSelectorsBuilders::configure:%s:' \
        % self.__class__.__name__.replace( ".", '_' )  )
    conflog.info('entering')

    try:
      menu = caloRingerFlags.electronMenuToUse()

      from RingerSelectorTools.ConfiguredAsgElectronRingerSelector \
          import ConfiguredAsgElectronRingerSelector
      from AthenaCommon.AppMgr import ToolSvc
      #try:
      self._looseSelector = ConfiguredAsgElectronRingerSelector(Ringer.Loose, menu)
      # Adding it into ToolSvc makes it a PublicTool:
      ToolSvc += self._looseSelector
      self._allSelectors.append(self._looseSelector)
      #except Exception:
      #  conflog.error(("Could not set up Ringer Loose electron "
      #    "selector. Reason:\n%s"),traceback.format_exc())

      #try:
      self._mediumSelector = ConfiguredAsgElectronRingerSelector(Ringer.Medium, menu)
      ToolSvc += self._mediumSelector
      self._allSelectors.append(self._mediumSelector)
      #except Exception:
      #  traceback.print_exc()
      #  conflog.error(("Could not set up Ringer Medium electron "
      #    "selector. Reason:\n%s"),traceback.format_exc())

      #try:
      self._tightSelector = ConfiguredAsgElectronRingerSelector(Ringer.Tight, menu)
      ToolSvc += self._tightSelector
      self._allSelectors.append(self._tightSelector)
      #except Exception:
      #  traceback.print_exc()
      #  conflog.error(("Could not set up Ringer Tight electron "
      #    "selector. Reason:\n%s"),traceback.format_exc())

      mlog.info (self._allSelectors)

    except Exception:
      for selector in self._allSelectors:
        removeFromToolSvc(selector)
      caloRingerFlags.doElectronIdentification = False
      treatException(("An unexpected error occurred. Due to it, all "
                      "Ringer electron selectors won't be executed."))
      traceback.print_exc()
      return False

    return True
