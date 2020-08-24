#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.EVNT

havedet = True
try:
    import DetDescrCnvSvc # noqa: F401
except ImportError:
    havedet = False
if havedet:
    ConfigFlags._loadDynaFlags("Detector")

# Don't fail just because G4AtlasApps isn't present in this build.
havesim = True
try:
    import G4AtlasApps # noqa: F401
except ImportError:
    havesim = False
if havesim:
    ConfigFlags._loadDynaFlags("Sim")

ConfigFlags.initAll()
ConfigFlags.dump()

