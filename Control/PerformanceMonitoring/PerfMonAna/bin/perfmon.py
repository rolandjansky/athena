#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file: perfmon.py
# @purpose: launch the performance monitoring analysis script
# @author: Sebastien Binet <binet@.cern.ch>
# $Id: perfmon.py,v 1.9 2008-02-22 17:32:29 binet Exp $

from __future__ import print_function

__author__  = "Sebastien Binet"
__version__ = "$Revision: 1.9 $"
__doc__     = "The executable file to launch the performance monitoring analysis script"

import os, glob
import sys
import traceback

def main():
    """main entry point"""

    from optparse import OptionParser
    parser = OptionParser( usage = "usage: %prog [options] -o out.root [-i] f1.pmon f2.pmon ..." )
    p = parser.add_option
    p( "-i",
       "--input-files",
       dest    = "inputFiles",
       default = [],
       help    = "path to the performance monitoring files to analyze"
       )

    p( "-o",
       "--out",
       dest    = "outFileName",
       default = None,
       help = "path to the output file which will contain analyzed performance monitoring data/infos"
       )

    p( "-a", "--analyzers",
       dest    = "analyzers",
       default = "cpu,mem",
       help    = "comma-separated list of analyzers to be run during perfmon "\
                 "processing (eg: cpu,mem,io)"
       )

    p( "-l",
       "--labels",
       dest    = "dsLabels",
       default = None,
       help = "comma separated list of labels associated to the input files (eg: rel_1,rel_2)"
       )

    p( "-s",
       "--selection",
       dest    = "selectionUri",
       default = "m.type == 'summary'",
       #default = "True",
       help = "A lambda expression (eg: 'm.name.count('MyAlgName')>0')"\
       " or the path to a python file containing the definition of a "\
       "filtering function (def filterFct(m): return m.name.count('MyAlgName')>0) "\
       "which will be used to select or discard histograms from the PDF and "\
       "output ROOT files. Note that the filter function **HAS** to be "\
       "called 'filterFct' and takes only 1 argument (the MonitoredComponent)."\
       " Note that a simple 'passthrough' expression could be: -s 'True' "
       )

    p( "-f",
       "--fit-slice",
       dest    = "fitSlice",
       default = "0.80",
       help = "A (python) slice defining the event-range over to which one "\
       "wants to apply the various fits. The slice can either be provided as "\
       "direct event bin indices or a ratio of the whole event range "\
       "(starting from the end of the range, e.g.: 0.80 will fit over the "\
       "last 80% of the whole range of events)"
       )

    p( "--no-batch",
       action  = "store_false",
       dest    = "rootBatch",
       default = False,
       help    = "Switch to tell ROOT to load graphics libraries"
       )

    p( "-b",
       "--batch",
       action  = "store_true",
       dest    = "rootBatch",
       default = True,
       help    = "Switch to tell ROOT _NOT_ to load graphics libraries"
       )

    (options, args) = parser.parse_args()

    from PerfMonAna.App import ExitCodes
    if options.outFileName == "":
        print ("ERROR: you need to give an output file name !")
        parser.print_help()
        return ExitCodes.ERROR

    if isinstance(options.inputFiles, str):
        options.inputFiles = [ options.inputFiles ]

    for arg in args:
        if arg[0] != '-':
            options.inputFiles += [ arg ]

    from PerfMonAna.PyRootLib import importRoot
    ROOT = importRoot( batch = options.rootBatch ) # noqa: F841

    inputFiles = []
    for f in options.inputFiles:
        f = glob.glob(os.path.expandvars(os.path.expanduser(f)))
        f.sort()
        inputFiles += f

    ## we want to preserve the potential order of files
    ## => don't use a set
    inputFileNames = []
    for f in inputFiles:
        if f not in inputFileNames:
            inputFileNames += [ f ]

    if len(inputFileNames) == 0:
        print ("ERROR: invalid input files (do they exist ?)")
        print ("ERROR: got: %r" % options.inputFiles)
        #parser.print_help()
        return ExitCodes.ERROR

    if options.outFileName is None:
        outFileName = os.path.basename(inputFileNames[0])
        options.outFileName = outFileName.replace(".pmon.gz",
                                                  ".perfmon.root")

    outFileName = os.path.expandvars(os.path.expanduser(options.outFileName))

    ## massage the supposedly comma-separated list of dataset labels
    dsLabels = None
    if isinstance(options.dsLabels, str):
        options.dsLabels = options.dsLabels.strip()
        if options.dsLabels.count(",") > 0:
            dsLabels = options.dsLabels.split(",")
        else:
            dsLabels = [ options.dsLabels ]

    analyzers = []
    options.analyzers = options.analyzers.strip()
    if options.analyzers.count(",") > 0:
        analyzers = options.analyzers.split(",")
    else:
        analyzers = ( options.analyzers, )

    ## loads and install the user filtering function
    from PerfMonAna.UserFct import loadFilterFct
    loadFilterFct(options.selectionUri)

    try:
        from PerfMonAna import App as pm
        ana = pm.AppMgr( inputFileNames = [f for f in inputFileNames],
                         outFileName    = outFileName,
                         analyzers      = analyzers,
                         dsLabels       = dsLabels,
                         fitSlice       = options.fitSlice )
        sc = ana.run()
    except Exception as err:
        print ("::: Caught:",err)
        traceback.print_exc( file = sys.stdout )
        sc = ExitCodes.ERROR
        pass

    return sc



if __name__ == "__main__":
    print (":"*80)
    print ("::: perfmon analysis script :::")
    print ("")

    sc = main()

    print ("")
    print ("::: bye")
    print (":"*80)
    sys.exit( sc )

