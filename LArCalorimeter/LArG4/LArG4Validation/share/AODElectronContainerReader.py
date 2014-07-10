#!/usr/bin/env physh

#
# $Id: AODElectronContainerReader.py,v 1.0 2010-08-12 13:00:12 Morse Exp $
#
# python script for launching AODReader job
#

__version__ = '$Revision: 484742 $'
__author__  = 'Radist Morse radist.morse@gmail.com'
__doc__     = 'Script for launching AODReader job'

from sys import exit
from os.path import isfile

from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog [options] input1 [...]", version="%prog v0.0.1 $Id: AODElectronContainerReader.py,v 0.1 2010-11-10 13:00:12 Morse Exp $")

parser.add_option("-o","--outputFile",dest="output",default='ntuple.root',help="select the output file name (default: %default)")

(options, args) = parser.parse_args()

for arg in args :
	if (not isfile (arg)) :
		print "ERROR: wrong input: nonexistent file:",arg
		exit(1)

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from LArG4Validation.LArG4ValidationConf import AODReader
topSequence += AODReader()

topSequence.AODReader.OutputLevel = WARNING

AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = ERROR


del args[0]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = args
ServiceMgr.EventSelector.BackNavigation = False

theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE DATAFILE=\'"+options.output+"\'  TYP='ROOT'  OPT='NEW'" ]
