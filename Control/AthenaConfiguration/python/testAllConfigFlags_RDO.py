#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.RDO
ConfigFlags._loadDynaFlags("Detector")
ConfigFlags._loadDynaFlags("Sim")
ConfigFlags.initAll()
ConfigFlags.dump()

