# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# File: __init__.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Date: September 2012
# Description: Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TileD3PDMaker.TileD3PDMakerConf.D3PD__xxxTool
#
# we can write
#
#   TileD3PDMaker.xxxTool
#


import TileD3PDMakerConf
for k, v in TileD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

