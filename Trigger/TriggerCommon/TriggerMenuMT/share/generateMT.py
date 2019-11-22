# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

setMenu             = "LS2_v1"
endJobAfterGenerate = False
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.generateMenuDiagnostics = True
include("TrigUpgradeTest/testHLT_MT.py")

