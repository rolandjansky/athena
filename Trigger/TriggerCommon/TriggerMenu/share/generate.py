#!/bin/env python

from AthenaCommon.AthenaCommonFlags import jobproperties 
import AthenaCommon.GlobalFlags import jobproperties

jobproperties.print_JobProperties('tree&value')


from TriggerMenu.GenerateMenu import GenerateMenu

g = GenerateMenu()
g.generate()

jobproperties.AthenaCommonFlags.EvtMax=0

jobproperties.print_JobProperties('tree&value')
