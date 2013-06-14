#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def main(sql, doReload):

    (ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL) = range(7)
    from AthenaCommon.Logging import logging
    log = logging.getLogger( "TrigConfigSvcUtils.py" )
    log.setLevel(VERBOSE)

    connection = "TRIGGERDBREPR"

    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerUseFrontier = True

    from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection
    connectionParameters = interpretConnection(connection)

    print "Connection: ", connectionParameters

    from TrigConfDBConnection import frontier_client as fc

    fc.init("testFrontier.log","debug")

    conn = fc.Connection(connectionParameters['url'])

    session = fc.Session(conn)

    conn.setReload(doReload)

    req = fc.Request("frontier_request:1:DEFAULT", fc.encoding_t.BLOB)
    param = fc.Request.encodeParam(sql)
    req.addKey("p1",param)

    session.getData([req])

    session.printHeader()

    nfield = session.getNumberOfFields()
    print "\nNumber of fields:", nfield, "\n"

    
    nrec = session.getNumberOfRecords()
    print "\nResult contains", nrec, "objects.\n"

    session.printRecords2()

    return 0



if __name__=="__main__":
    from sys import exit

    doReload = True
    sql = "select distinct SM.SMT_ID, SM.SMT_NAME, SM.SMT_VERSION, SM.SMT_COMMENT, SM.SMT_ORIGIN, SM.SMT_USERNAME, SM.SMT_STATUS from ATLAS_CONF_TRIGGER_REPR.SUPER_MASTER_TABLE SM"
    
    exit(main(sql=sql, doReload=doReload))
    
