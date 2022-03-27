#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
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
ConfigFlags.lock()
ConfigFlags.dump()
