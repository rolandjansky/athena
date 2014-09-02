#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Resolve the Current or Next BLK tags to find the beamspot folder tag
"""

__version__ = ''
__usage__   = '''%prog [options]

Resolve the Current or Next BLK tags to find the beamspot folder tag
'''

import sys
from InDetBeamSpotExample import COOLUtils

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-n', '--next', dest='next', action='store_true', default=False, help='Resolve the next tag.  Default is False')
(options,args) = parser.parse_args()


if options.next:
    tag = COOLUtils.resolveNextBeamSpotFolder()
else:
    tag = COOLUtils.resolveCurrentBeamSpotFolder()

print tag
