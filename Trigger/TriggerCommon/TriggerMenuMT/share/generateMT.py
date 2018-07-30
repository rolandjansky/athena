#!/bin/env python

from AthenaCommon.AthenaCommonFlags import jobproperties 
from AthenaCommon.GlobalFlags import jobproperties

jobproperties.print_JobProperties('tree&value')


from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenu import GenerateMenuMT

g = GenerateMenuMT()
g.generateMT()

jobproperties.AthenaCommonFlags.EvtMax=0

jobproperties.print_JobProperties('tree&value')
