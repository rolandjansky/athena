#!/usr/bin/env python
"""Run unit tests on RandomServices.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from RandomServices import dSFMT, Ranlux64, Ranecu, RNG

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Make each
t1 = dSFMT("TestSeed1")
t2 = Ranlux64("TestSeed2")
t3 = Ranecu("TestSeed3")
# Make RNG with all three agruments and none
t4 = RNG(name="RNGdSFMT1")
t5 = RNG("dSFMT", name="RNGdSFMT2")
t6 = RNG("Ranlux64", name="RNGRanlux64")
t7 = RNG("Ranecu", name="RNGRanecu")
# Merge
t1.mergeAll((dSFMT("test_seed7"), t2, t3, t4, t5, t6, t7,))
# Flag as merged to prevent error on destruction
t1._wasMerged = True

