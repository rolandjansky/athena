#!/usr/bin/env python

import sys,os
from re import match
from PyCool import cool
import CoolConvUtilities.AtlCoolLib as AtlCoolLib


def main():
    connectString = "TRIGGERDBREPR"
    if len(sys.argv)>1:
        connectString = sys.argv[1]
    if connectString == None: return 0

    (ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL) = range(7)

    from AthenaCommon.Logging import logging
    log = logging.getLogger( "TrigConfigSvcUtils.py" )
    log.setLevel(VERBOSE)
    log = logging.getLogger( "TrigConfFrontier.py" )
    log.setLevel(VERBOSE)
    

    from TrigConfigSvc.TrigConfigSvcUtils import getTriggerDBCursor, executeQuery

    cursor,schemaname = getTriggerDBCursor(connectString)

    output = ["SM.SMT_ID", "SM.SMT_NAME", "SM.SMT_VERSION", "SM.SMT_COMMENT", "SM.SMT_ORIGIN", "SM.SMT_USERNAME", "SM.SMT_STATUS"]
    condition = []
    tables = { 'SM' : 'SUPER_MASTER_TABLE' }

    result = executeQuery(cursor, output, condition, schemaname, tables)

    print "TEST result:\n============"
    for r in sorted(result):
        print r

    return 0

if __name__ == '__main__':
    """This test does a simple query using different technologies

    if environment TRIGGER_USE_FRONTIER is set then local frontier is used, otherwise oracle
    """

    sys.exit(main())
    

