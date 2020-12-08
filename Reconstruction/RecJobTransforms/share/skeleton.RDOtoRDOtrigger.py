# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from AthenaCommon.ConcurrencyFlags import jobproperties as cfjp
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if cfjp.ConcurrencyFlags.NumThreads() > 0 or ConfigFlags.Concurrency.NumThreads > 0:
    # AthenaMT Run-3 Trigger
    include('RecJobTransforms/skeleton.RDOtoRDOtriggerMT.py')
else:
    # Serial Athena Run-2 Trigger
    include('RecJobTransforms/skeleton.RDOtoRDOtriggerLegacy.py')
