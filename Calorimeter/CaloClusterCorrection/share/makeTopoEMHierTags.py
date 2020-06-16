#!/usr/bin/env python
# $Id: makeTopoEMHierTags.py,v 1.2 2009-04-29 04:50:10 ssnyder Exp $
#
# File: CaloClusterCorrection/share/makeSwHierTags.py
# Created: Apr 2009, sss
# Purpose: Add hierarchical cluster correction tags to a COOL database.
#
# Run this to make hierarchical tags for SW cluster corrections.
# Takes the sqlite cool database file as an argument.
# Tags the generation string as an optional second argument.
#

from __future__ import print_function
from CaloClusterCorrection.CaloTopoEMCorrections import CaloTopoEMCorrections
from CaloClusterCorrection.MakeHierTags          import MakeHierTags
import sys


dryrun = False
nomagic = False
while sys.argv[1][0] == '-':
    if sys.argv[1] == '-n':
        dryrun = True
        del sys.argv[1]
    elif sys.argv[1] == '--nomagic':
        nomagic = True
        del sys.arg
    else:
        print ("Unknown switch", sys.arv[1])
        sys.exit(1)

coolfile = sys.argv[1]
generation = None
if len(sys.argv) > 2:
    generation = sys.argv[2]

mh = MakeHierTags (coolfile, CaloTopoEMCorrections, dryrun, nomagic=nomagic)
mh.run (generation)
