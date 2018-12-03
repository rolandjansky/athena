#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, os
try:    from             DCubeApp import DCubeApp
except: from DCubeClient.DCubeApp import DCubeApp
if __name__ == "__main__":
    ## to run ROOT in batch mode
    sys.argv.append("-b")
    DCubeApp( )
