#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.EVNT
ConfigFlags._loadDynaFlags("Detector")
ConfigFlags._loadDynaFlags("Sim")
ConfigFlags.initAll()
ConfigFlags.dump()

