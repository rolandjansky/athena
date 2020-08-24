#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
import sys
from getopt import getopt
from AthenaCommon.Logging import logging
from AthenaCommon import ExitCodes
from PyJobTransformsCore.trferr import TransformErrorDiagnoser, AthenaLogChecker
from PyJobTransformsCore.JobReport import JobReport, JobInfo
from PyJobTransformsCore.trfutil import get_atlas_release

def usage():
    print("Parse an athena logfile for errors.")
    print("Usage: %s [options] <logfilename>" % os.path.basename(sys.argv[0]))
    print("Options:")
    print("  -h : print short help")
    print("  -d : print details on the error matching")
    print("  -x : write jobInfo.xml file")
    print("  -r <release> : assume atlas release <release>")

if len(sys.argv) <= 1:
    usage()
    sys.exit(1)

#options
debug = False
writeXML = False
atlas_release = None
opts,args = getopt(sys.argv[1:],"dxhr:")
for opt,arg in opts:
    if opt == '-d': debug = True
    if opt == '-x': writeXML = True
    if opt == '-r': atlas_release = arg
    if opt == '-h':
        usage()
        sys.exit()


if len(args) < 1:
    usage()
    sys.exit(1)

# setup logger
log = logging.getLogger('AthenaLogChecker')
if debug:
    log.setLevel( logging.DEBUG )
else:
    log.setLevel( logging.INFO )


# get ATLAS release if not given on command line
if not atlas_release: atlas_release = get_atlas_release()

# jobReport to store results
report = JobReport()
producer = os.path.basename(sys.argv[0])
if len(args) > 1:
    comment = 'for files %s' % ( ','.join(args) )
else:
    comment = 'for file %s' % ( ','.join(args) )
report.setProducer( producer, comment=comment )
report.addInfo( JobInfo( 'athCode', '0' ) )
report.addInfo( JobInfo( 'athAcronym', str( ExitCodes.what( 0 ) ) ) )
checker = AthenaLogChecker(atlas_release)
for filename in args:
    fileReport = JobReport()
    fileReport.setProducer( os.path.basename(filename), comment='from logfile' )
    checker.checkLogFile(filename,fileReport,log)
    try:
        report.addReport(fileReport)
    except KeyError:
        pass
    del fileReport

# run the error diagnoser on all errors
errorDocter = TransformErrorDiagnoser()
for error in report.errors():
    errorDocter.diagnoseError(error)

print(report)
if writeXML: report.writeJobInfoXML()

# exit with appropriate code
sys.exit(report.exitCode())
