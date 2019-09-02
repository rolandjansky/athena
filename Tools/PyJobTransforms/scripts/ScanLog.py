#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys

print('''
This script is deprecated. Please use

   ValidateFiles_tf.py --logfile MYLOGFILE

instead.
''', file=sys.stderr)
sys.exit(1)
