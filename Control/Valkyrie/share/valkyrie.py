#! /usr/bin/env python

# @file:    valkyrie.py
# @purpose: wrapper script to launch Valgrind on top of ChapPy scripts
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    January 2007
#
# @example:
# lxplus> valkyrie.py --massif MyPkg/MyJobOptions.py
# lxplus> valkyrie.py --memcheck MyPkg/MyJobOptions.py
# lxplus> valkyrie.py --callgrind MyPkg/MyJobOptions.py
#

__version__ = "$Revision: 1.3 $"
__author__  = "Sebastien Binet"
__doc__     = "wrapper script to launch Valgrind on top of ChapPy scripts"

import sys
import os

from optparse import OptionParser
parser = OptionParser(usage="usage: %prog [options] [--job-option] jobO.py")
parser.add_option(
    "--memcheck",
    dest = "doMemCheck",
    action  = "store_true",
    default = False,
    help = "Activate the memcheck tool from the valgrind toolkit"
    )
parser.add_option(
    "--callgrind",
    dest = "doCallGrind",
    action  = "store_true",
    default = False,
    help = "Activate the callgrind tool from the valgrind toolkit"
    )
parser.add_option(
    "--massif",
    dest = "doMassif",
    action  = "store_true",
    default = False,
    help = "Activate the massif tool from the valgrind toolkit"
    )
parser.add_option(
    "--job-option",
    dest = "jobOption",
    default = None,
    help = "Name of the jobOption file to run"
    )

(options, args) = parser.parse_args()

## get the job-option
if len(args) > 0 and args[0][0] != "-":
    options.jobOption = args[0]
    pass

## check we have a jobOption to run !
if not options.jobOption:
    str(parser.print_help() or "")
    sys.exit(1)
    
## require one and only one tool to be activated:
## => enforced by the ValgrindFlags

## transmit the options to the ValgrindFlags
from Valkyrie.ValgrindFlags import jobproperties
jobproperties.ValgrindFlags.doMemCheck  = options.doMemCheck
jobproperties.ValgrindFlags.doMassif    = options.doMassif
jobproperties.ValgrindFlags.doCallGrind = options.doCallGrind

from AthenaCommon.Include import include
include.setShowIncludes( False )

from AthenaCommon.ChapPy import JobOptions
from Valkyrie.Valkyrie import Valkyrie

print ":"*80
print "::: Valgrind is launching script [%s] :::" % options.jobOption
if   options.doMemCheck:  print ":::   using tool  [memcheck]"
elif options.doMassif:    print ":::   using tool  [massif]"
elif options.doCallGrind: print ":::   using tool  [callgrind]"
else:                     print ":::   using tool  [none]"
valkyrie = Valkyrie(
    jobOptions = [ JobOptions(options.jobOption) ],
    logFile    = "valkyrie.%i.log" % os.getpid(),
    )
sc = valkyrie.run()
print ":"*80

## return the status code to our parent process
sys.exit(sc)
