# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Simple wrapper for specific RttScriptRunner instances
# This is done to get "pretty" names on the RTT web pages

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys, os
sys.path.append(os.path.dirname(sys.modules[__name__].__file__))

from RttScriptRunner_TriggerTest import RttScriptRunner_TriggerTest

class CheckForTrigCostD3PD_TriggerTest(RttScriptRunner_TriggerTest):
    pass

