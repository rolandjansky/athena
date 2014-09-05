#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## A simple logfile scanner for post-facto analysis of existing logs
# @version $Id: ScanLog.py 609222 2014-07-29 14:43:50Z graemes $ 

import argparse
import pprint
import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfLogger import stdLogLevels
from PyJobTransforms.trfValidation import athenaLogFileReport, ignorePatterns

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    # This should change to be a transform so as to be able support transform
    # validation option switches 
    parser = argparse.ArgumentParser(description='Subject a pre-existing athena logfile to standard transform analysis',
                                     argument_default=argparse.SUPPRESS)
    parser.add_argument('--logfile', help='Athena logfile to examine', required=True)
    parser.add_argument('--ignoreFiles', default = ['atlas_error_mask.db'],
                        help='Files containing error patterns to be ignored during logfile scans (use "None" to disable the standard "atlas_error_mask.db")', nargs='+')
    parser.add_argument('--ignorePatterns', default = [],
                        help='Regexp error patterns to be ignored during logfile scans (will be applied as a search against the whole logfile line)', nargs='+')
    args = vars(parser.parse_args())
        
    ignPat = ignorePatterns(files = args['ignoreFiles'], extraSearch = args['ignorePatterns'])
        
    msg.info('Scanning logfile {0}'.format(args['logfile']))
    logReport = athenaLogFileReport(logfile = args['logfile'], ignoreList = ignPat)
    pprint.pprint(logReport.python)
    
    # Maybe this code should be shared with the athena executor? 
    try:
        worstError = logReport.worstError()
        if worstError['nLevel'] >= stdLogLevels['ERROR']:
            errmsg = 'Fatal error in athena logfile (level %s)' % worstError['level']
            msg.error(errmsg)
            if worstError['firstError']:
                raise trfExceptions.TransformLogfileErrorException(trfExit.nameToCode('TRF_EXEC_LOGERROR'), 
                                                                   'Fatal error in athena logfile: "%s" (occurred %d times, first occurrance line %d)' % 
                                                                   (worstError['firstError']['message'], worstError['firstError']['count'], worstError['firstError']['firstLine']))
            else:
                raise trfExceptions.TransformLogfileErrorException(trfExit.nameToCode('TRF_EXEC_LOGERROR'), 
                                                                   'Fatal error in athena logfile (specific message unknown).')
    except trfExceptions.TransformException, e:
        msg.error('%s' % e.errMsg)
        sys.exit(e.errCode)
        
    sys.exit(0)


if __name__ == '__main__':
    main()
