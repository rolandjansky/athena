#! /usr/bin/env python

# @file:    chappy.py
# @purpose: wrapper script to launch ChapPy scripts
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    November 2006
#
# @example:
# lxplus> chappy McParticleAlgs/rwtest_genEvent.py
#

import sys

from AthenaCommon.Include import include
include.setShowIncludes( False )

print ":"*80
print "::: ChapPy is launching script [%s] :::" % sys.argv[1]
include( sys.argv[1] )
print ":"*80
