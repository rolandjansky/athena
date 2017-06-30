# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'ElectronRingerSelectorDefs.py' )

from AthenaCommon.Configurable import Configurable

# Import from Ringer utilities:
import cppyy

try :
  cppyy.loadDictionary('RingerSelectorToolsDict')
except RuntimeError, e:
  mlog.error("Could not load RingerSelectorTools dictionary from cppyy.")
  raise RuntimeError(e)

from ROOT import Ringer
from ROOT.Ringer import ElectronTAccept_v1
from RingerSelectorTools.RingerSelectorToolsConf import Ringer__AsgElectronRingerSelector
from CaloRingerAlgs.CaloRingerKeys import outputElectronRingSetsConfKey

# Import from CutID utilities:
try :
  cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except RuntimeError, e:
  mlog.error("Could not load ElectronPhotonSelectorTools dictionary from cppyy.")
  raise RuntimeError(e)
from ROOT import egammaPID
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors \
    import ConfiguredAsgElectronIsEMSelector

def removeClusterCutsFromIsEMMask(CutIDIsEMMask):
  "Remove egammaPID Electron Calorimeter cuts from isEM"
  CutIDIsEMMask = CutIDIsEMMask & (~egammaPID.CALO_ELECTRON)

class BaseElectronSelectorConf ( Ringer__AsgElectronRingerSelector ):
  def _setDefault(self,attrName, default, **kwargs):
    "Overwrites AsgElectronRingerSelector default to new default value."
    if not kwargs.has_key(attrName):
      setattr(self, attrName, default)

  def _setCutIDSelector(self,defaultName,quality,menu,**kwargs):
    """
    Set the AsgElectronRingerSelector CutIDSelector to default or to the configuration
    available in the kwargs
    """
    removeMask = False
    if not kwargs.has_key("cutIDConfDict") and not kwargs.has_key("CutIDSelector"):
      removeMask = True
    if not kwargs.has_key("CutIDSelector"):
      CutIDSelectorTool = ConfiguredAsgElectronIsEMSelector(defaultName, \
        quality, \
        menu, \
        **kwargs.pop("cutIDConfDict",{}))
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc += CutIDSelectorTool
      self.CutIDSelector = CutIDSelectorTool
    else:
      self.CutIDSelector = kwargs.pop("CutIDSelector")
    if removeMask:
      removeClusterCutsFromIsEMMask(self.CutIDSelector.isEMMask)

  def __init__(self, name, **kwargs):
    Ringer__AsgElectronRingerSelector.__init__(self,name,**kwargs)
    self._setDefault("RingSetConfContainerName",
                     outputElectronRingSetsConfKey(),
                     **kwargs)
    self._setDefault("CacheConfData", True, **kwargs)
    self._setDefault("useCutIDTrack", False, **kwargs)


class ElectronRingerSelectorTestLoose( BaseElectronSelectorConf ):
  def __init__(self, name = "ElectronRingerSelector_TestLoose", **kwargs):
    BaseElectronSelectorConf.__init__(self,name,**kwargs)
    self._setDefault("DiscriminationFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineDiscrLoose.root",
                     **kwargs)
    self._setDefault("ThresholdFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineThresLoose.root",
                     **kwargs)
    self._setDefault("CutsMask",
                     ElectronTAccept_v1.getAppliedCutMsk(Ringer.Loose,
                                                         kwargs.get("useCutIDTrack",False)),
                     **kwargs)
    self._setCutIDSelector("LooseRingerIsEMSelector",\
        egammaPID.ElectronIDLoosePP, \
        electronPIDmenu.menuDC14, **kwargs)

class ElectronRingerSelectorTestMedium( BaseElectronSelectorConf ):
  def __init__(self, name = "ElectronRingerSelector_TestMedium", **kwargs):
    BaseElectronSelectorConf.__init__(self,name,**kwargs)
    self._setDefault("DiscriminationFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineDiscrMedium.root",
                     **kwargs)
    self._setDefault("ThresholdFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineThresMedium.root",
                     **kwargs)
    self._setDefault("CutsMask",
                     ElectronTAccept_v1.getAppliedCutMsk(Ringer.Medium,
                                                         kwargs.get("useCutIDTrack",False)),
                     **kwargs)
    self._setCutIDSelector("MediumRingerIsEMSelector", \
        egammaPID.ElectronIDMediumPP, \
        electronPIDmenu.menuDC14, \
        **kwargs)

class ElectronRingerSelectorTestTight( BaseElectronSelectorConf ):
  def __init__(self, name = "ElectronRingerSelector_TestTight", **kwargs):
    BaseElectronSelectorConf.__init__(self,name,**kwargs)
    self._setDefault("DiscriminationFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineDiscrTight.root",
                     **kwargs)
    self._setDefault("ThresholdFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineThresTight.root",
                     **kwargs)
    self._setDefault("CutsMask",
                     ElectronTAccept_v1.getAppliedCutMsk(Ringer.Tight,
                                                         kwargs.get("useCutIDTrack",False)),
                     **kwargs)
    self._setCutIDSelector("TightRingerIsEMSelector", \
        egammaPID.ElectronIDTightPP, \
        electronPIDmenu.menuDC14, **kwargs)

class ElectronRingerSelectorTestNoCut( BaseElectronSelectorConf ):
  def __init__(self, name = "ElectronRingerSelector_TestNoCut", **kwargs):
    # FIXME This doesn't work, for now...
    BaseElectronSelectorConf.__init__(self,name,**kwargs)
    self._setDefault("DiscriminationFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineDiscrMedium.root",
                     **kwargs)
    self._setDefault("ThresholdFileName",
                     "RingerSelectorTools/TestMenu_20150605_v1/ElectronRingerOfflineThresMedium.root",
                     **kwargs)
    self._setDefault("CutsMask",
                     ElectronTAccept_v1.getAppliedCutMsk(Ringer.NoCut,
                                                         kwargs.get("useCutIDTrack",False)),
                     **kwargs)
    self._setCutIDSelector("NoCutRingerIsEMSelector", \
        egammaPID.ElectronIDNoCut, \
        electronPIDmenu.menuDC14, \
        **kwargs)


