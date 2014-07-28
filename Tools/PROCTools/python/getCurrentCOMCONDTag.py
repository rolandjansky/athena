#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# To know the CURRENT COMCOND tag.

import sys
from PyCool import cool
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias

currentBLKP = resolveAlias.getCurrent()
print currentBLKP

currentES1P = resolveAlias.getCurrentES()
print currentES1P
