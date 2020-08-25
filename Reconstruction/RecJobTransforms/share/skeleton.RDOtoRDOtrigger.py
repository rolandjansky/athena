# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from TriggerJobOpts.TriggerFlags import TriggerFlags

if TriggerFlags.doMT():
    include('RecJobTransforms/skeleton.RDOtoRDOtriggerMT.py')
else:
    include('RecJobTransforms/skeleton.RDOtoRDOtriggerLegacy.py')
