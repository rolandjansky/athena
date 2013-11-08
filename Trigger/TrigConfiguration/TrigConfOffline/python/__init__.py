__version__ = '1.0.0'
__author__  = 'Joerg Stelzer <joerg.stelzer@cern.ch>'
__all__ = [ 'TrigConf', 'l1menuloader', 'hltmenuloader' ]

import ROOT, PyCintex
PyCintex.Cintex.Enable()

PyCintex.loadDictionary("libTrigConfL1DataDict")
PyCintex.loadDictionary("libTrigConfHLTDataDict")
PyCintex.loadDictionary("libTrigConfStorageDict")

TrigConf = PyCintex.makeNamespace('TrigConf')

# modify a few functions
TrigConf.Menu.items = TrigConf.Menu.itemsV
TrigConf.HLTFrame.chains = TrigConf.HLTFrame.chainsV
TrigConf.HLTFrame.sequences = TrigConf.HLTFrame.sequencesV



#from TrigConfOffline.menuloader import *
from TrigConfOffline import menuloader
hltmenuloader = menuloader.hltmenuloader
l1menuloader = menuloader.l1menuloader
