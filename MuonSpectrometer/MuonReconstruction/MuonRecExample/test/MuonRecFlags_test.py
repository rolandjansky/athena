#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Unit test for module MuonRecFlags.py"""

from MuonRecExample.MuonRecUtils import RecConfigInfo
import UnitTest # from current test directory
import os
import __main__

from AthenaCommon.Logging import logging

log = logging.getLogger("MuonRecFlags_test.py")
log.setLevel(logging.DEBUG)

# execute the tests
if __name__ == "__main__":
    UnitTest.main()
