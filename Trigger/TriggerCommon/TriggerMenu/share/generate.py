#!/bin/env python

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

athenaCommonFlags.print_JobProperties('tree&value')

from TriggerMenu.menu.GenerateMenu import GenerateMenu

g = GenerateMenu()
g.generate()

athenaCommonFlags.EvtMax=0

globalflags.print_JobProperties('tree&value')
