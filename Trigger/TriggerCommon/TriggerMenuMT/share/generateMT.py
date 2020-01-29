# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

setMenu             = "LS2_v1"
doWriteRDOTrigger   = False
doWriteBS           = False
endJobAfterGenerate = False
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.generateMenuDiagnostics = True
include("TriggerJobOpts/runHLT_standalone.py")

