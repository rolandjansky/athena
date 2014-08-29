#!/usr/bin/env python
# print the magnetic field for a given run
# coolMagneticField.py <run>

import sys
from CoolConvUtilities.MagFieldUtils import MagFieldDCSInfo, getFieldForRun

if len(sys.argv)<2:
    print "Syntax",sys.argv[0],'<run> {<lumiblock>}'
    sys.exit(-1)
run=int(sys.argv[1])
lb=None
if len(sys.argv)>2:
    lb=int(sys.argv[2])
magfield=getFieldForRun(run,quiet=True,lumiblock=lb)
print "Magnetic field information for run %i" % run,
if lb is None:
    print
else:
    print "lumiblock %i" % lb
if (magfield is not None):
    print "Solenoid current %8.2f (requested %8.2f)" % (magfield.solenoidCurrent(),magfield.solenoidSetCurrent())
    print "Toroid   current %8.2f (requested %8.2f)" % (magfield.toroidCurrent(),magfield.toroidSetCurrent())
    print "Filename Tag: %s" % magfield.fileNameTag()
else:
    print "Not available (no DCS data or run to timestamp mapping)"
