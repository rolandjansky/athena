#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
from optparse import OptionParser

parser = OptionParser(usage="usage: %prog lib1 [lib2...]", version="%prog v0.1 $Id: LArG4ShowerLibSvcTest.py 489535 2012-03-20 01:19:04Z gsedov $")

(options, args) = parser.parse_args()

exec = __file__.replace("LArG4ShowerLibSvcTest.py","LArG4ShowerLibSvcTestBody.py")
os.system('athena -c "args={:s}" {:s}'.format(str(args),exec))
