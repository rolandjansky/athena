#!/usr/bin/env python

#from TrigConfOffline import *
#import TrigConfOffline as tc
from TrigConfOffline.menuloader import *

hltmenu = hltmenuloader("HLTconfig_Physics_pp_v4_18.1.0.xml").menu
c = hltmenu.chains()[0]
s = hltmenu.sequences()[0]
print c

#for c in (x for x in hltmenu.chains() if '4j45' in x.name()):
#    print c.name()

l1menu = l1menuloader("LVL1config_Physics_pp_v4.xml").menu
i = l1menu.menu().items()[0]
print i

#for item in l1menu.menu().items():
#    print item.name()

