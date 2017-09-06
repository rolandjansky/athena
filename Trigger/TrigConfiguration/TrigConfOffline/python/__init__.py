__version__ = '1.0.0'
__author__  = 'Joerg Stelzer <joerg.stelzer@cern.ch>'
__all__ = [ 'TrigConf', 'l1menuloader', 'hltmenuloader' ]

import cppyy

cppyy.loadDictionary("libTrigConfL1DataDict")
cppyy.loadDictionary("libTrigConfHLTDataDict")
cppyy.loadDictionary("libTrigConfStorageDict")

TrigConf = cppyy.makeNamespace('TrigConf')

# modify a few functions
TrigConf.Menu.items = TrigConf.Menu.itemsV
TrigConf.HLTFrame.chains = TrigConf.HLTFrame.chainsV
TrigConf.HLTFrame.sequences = TrigConf.HLTFrame.sequencesV



#from TrigConfOffline.menuloader import *
from TrigConfOffline import menuloader
hltmenuloader = menuloader.hltmenuloader
l1menuloader = menuloader.l1menuloader
