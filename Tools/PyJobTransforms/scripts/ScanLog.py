#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

print('''
This script is deprecated. Please use

   ValidateFiles_tf.py --logfile MYLOGFILE

instead.
''', file=sys.stderr)
sys.exit(1)
