#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TriggerInterface.py
@author P. Onyisi
@date 2019-04-12
@brief Simple new configuration framework functions for getting the TrigDecisionTool. 
Wrapping underling call in case any additional monitoring configuration is required in the future
'''
from PyUtils.Decorators import memoize

@memoize
def getTrigDecisionTool(flags):
    from TrigDecisionTool.TrigDecisionToolConfig import getTrigDecisionTool as getTDT
    return getTDT(flags)
