#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# python script for launching AODReader job
#

__version__ = '$Revision: 484742 $'
__author__  = 'Radist Morse radist.morse@gmail.com'
__doc__     = 'Script for launching AODReader job'

import os
from sys import exit
from os.path import isfile

from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog [options] input1 [...]", version="%prog v0.0.1 $Id: AODElectronContainerReader.py,v 0.1 2010-11-10 13:00:12 Morse Exp $")

parser.add_option("-o","--outputFile",dest="output",default='ntuple.root',help="select the output file name (default: %default)")

(options, args) = parser.parse_args()

for arg in args :
	if (not isfile (arg)) :
		print ("ERROR: wrong input: nonexistent file:",arg)
		exit(1)

exec = __file__.replace("AODElectronContainerReader.py","AODElectronContainerReaderBody.py")
os.system('athena -c "args={:s}" {:s}'.format(str(args),exec))
