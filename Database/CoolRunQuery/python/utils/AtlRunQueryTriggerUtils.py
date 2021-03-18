#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryTriggerUtils.py
# Project: AtlRunQuery
# Purpose: Trigger Utility functions for AtlRunQuery
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Oct 27, 2010
# ----------------------------------------------------------------


from __future__ import print_function


__cursor_schema = [ (None ,"") , (None , "") ] # run 1 and run 2 cursor



# returns True if run is RUN 2 (2015-2018)
def isTriggerRun2(run_number = None , smk = None , isRun2 = None ):
    if run_number is None and smk is None and isRun2 is None:
        raise RuntimeError("Cannot determine if trigger db is run 1 or 2")

    if smk is not None :
        return smk >= 2000
    elif run_number is not None :
        return run_number > 249000
    else:
        return isRun2


def triggerDBAlias(run_number = None , smk = None, isRun2 = None ):
    return "TRIGGERDB" if isTriggerRun2( run_number=run_number, smk=smk, isRun2 = isRun2) else "TRIGGERDB_RUN1"



from xml.dom import minidom
import os, re, logging
log = logging.getLogger( __name__ )

class TriggerChain:
    def __init__(self, name, counter, lowername="", lowercounter=-1, forshow=False, forselect=False, level=0):
        self.name         = name     
        self.counter      = counter       
        self.lowername    = "" if lowername=="~" else lowername
        self.lowercounter = lowercounter  
        self.forshow      = forshow  
        self.forselect    = forselect
        self.multiseeded  = (self.lowername=="") or (',' in self.lowername)
        self.level        = level
        self.lower        = None
        
    def __repr__(self):
        return "TC:"+self.name

### helper ----------------------------------------------------------------------
def FindFile( filename, pathlist, access ):
   """Find <filename> with rights <access> through <pathlist>."""

 # special case for those filenames that already contain a path
   if os.path.dirname( filename ):
      if os.access( filename, access ):
         return filename

 # test the file name in all possible paths until first found
   for path in pathlist:
      f = os.path.join( path, filename )
      if os.access( f, access ):
         return f

 # no such accessible file avalailable
   return None



def _getFileLocalOrPath(filename, pathenv):
    """looks for filename in local directory and then in all paths specified in environment variable 'pathenv'
    returns path/filename if existing, otherwise None
    """
    log = logging.getLogger( "TrigConfigSvcUtils.py" )
    if os.path.exists(filename):
        log.info( "Using local file %s", filename)
        return filename

    pathlist = os.getenv(pathenv,'').split(os.pathsep)
    resolvedfilename = FindFile(filename, pathlist, os.R_OK)
    if resolvedfilename:
        return resolvedfilename

    log.fatal("No file %s found locally nor in %s" % (filename, os.getenv('CORAL_DBLOOKUP_PATH')) )
    return None


def _getConnectionServicesForAlias(alias):
    log = logging.getLogger( "TrigConfigSvcUtils.py" )

    connectionServices = None # list of services

    dblookupfilename = _getFileLocalOrPath('dblookup.xml','CORAL_DBLOOKUP_PATH')
    if dblookupfilename is None:
        return None

    doc = minidom.parse(dblookupfilename)
    for ls in doc.getElementsByTagName('logicalservice'):
        if ls.attributes['name'].value != alias:
            continue
        connectionServices = [str(s.attributes['name'].value) for s in ls.getElementsByTagName('service')]
    doc.unlink()

    log.info( "For alias '%s' found list of connections %r", (alias,connectionServices) )
    if connectionServices is None:
        log.fatal("Trigger connection alias '%s' is not defined in %s" % (alias,dblookupfilename))
    return connectionServices


def _readAuthentication():
    """ read authentication.xml, first from local directory, then from all paths specified in CORAL_AUTH_PATH
    returns dictionary d with d[connection] -> (user,pw)
    """

    authDict = {}
    dbauthfilename = _getFileLocalOrPath('authentication.xml','CORAL_AUTH_PATH')
    if dbauthfilename is None:
        return authDict

    doc = minidom.parse(dbauthfilename)        
    for cn in doc.getElementsByTagName('connection'):
        user = ""
        pw = ""
        svc = cn.attributes['name'].value
        for p in cn.getElementsByTagName('parameter'):
            if p.attributes['name'].value == 'user':
                user = p.attributes['value'].value
            elif p.attributes['name'].value == 'password':
                pw = p.attributes['value'].value
        authDict[svc] = (user,pw)
    doc.unlink()
    return authDict

authDict = None

def _getConnectionParameters(connection):
    log = logging.getLogger( "TrigConfigSvcUtils.py" )
    connection = str(connection)
    connectionParameters = {}

    if connection.startswith("sqlite_file:"):
        filename = connection[connection.find(':')+1:]
        connectionParameters["techno"] = "sqlite"
        connectionParameters["filename"] = filename if os.path.exists(filename) else None
        
    elif connection.startswith("oracle://"):
        if connection.count(';') == 2:
            pattern = "(.*)://(.*)/(.*);username=(.*);password=(.*)"
            m = re.match(pattern,connection)
            #print ('Groups ', m)
            if not m:
                log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern))
            (techno, server, schema, user, passwd) = m.groups()
            #print ((techno, server, schema, user, passwd))
        else:
            global authDict
            if not authDict:
                authDict = _readAuthentication()
            pattern = "oracle://(.*)/(.*)"
            m = re.match(pattern,connection)
            if not m:
                log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern))
            (server, schema) = m.groups()
            (user,passwd) = authDict[connection]
        #info from auth file also unicode
        connectionParameters["techno"] = 'oracle'
        connectionParameters["server"] = str(server)
        connectionParameters["schema"] = str(schema)
        connectionParameters["user"  ] = str(user)
        connectionParameters["passwd"] = str(passwd)

    elif connection.startswith("mysql://"):
        pattern = "mysql://(.*);dbname=(.*);user=(.*);passwd=(.*);"
        m = re.match(pattern,connection)
        if not m:
            log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern) )
        (server, dbname, user, passwd) = m.groups()
        connectionParameters["techno"] = 'mysql'
        connectionParameters["server"] = server
        connectionParameters["dbname"] = dbname
        connectionParameters["user"  ] = user
        connectionParameters["passwd"] = passwd

    elif connection.startswith("frontier://"):
        pattern = r"frontier://ATLF/\(\)/(.*)"
        m = re.match(pattern,connection)
        if not m:
            log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern) )
        (schema, ) = m.groups()
        connectionParameters["techno"] = 'frontier'
        connectionParameters["schema"] = schema

    return connectionParameters

def interpretConnection(connection, debug=False, resolveAlias=True):
    # connection needs to be of the following format (this is also the order of checking)
    # <ALIAS>                              -- any string without a colon ':' will be checked for in the dblookup.xml file
    # sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2  -- a service description without user and password, requires lookup in authentication.xml
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    log.info("Specified connection string '%s'", connection)

    # not needed any longer
    # connection = connection.lstrip("dblookup://")
    # connection = connection.lstrip("dblookup:")
    # connection = connection.rstrip(";")
    #print (connection)

    # what to return
    connectionParameters = {}
    connection = str(connection)

    # connection explicitly specified (no DB alias)
    if ':' in connection:
        connectionParameters = _getConnectionParameters( connection )
        return connectionParameters

    # connection is a DB alias
    connectionParameters["alias"] = connection
    if not resolveAlias:
        return connectionParameters

    connectionServices = _getConnectionServicesForAlias( connection ) # alias resolution via dblookup
    if connectionServices is None:
        return connectionParameters

    # If TriggerFlags.triggerUseFrontier=true then we remove sqlite files
    if os.getenv('TRIGGER_USE_FRONTIER',False):
        connectionServices = filter(lambda conn: not conn.startswith("sqlite_file"), connectionServices)
        if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
            log.fatal("Inconsistent setup: environment variable ATLAS_TRIGGERDB_FORCESQLITE is defined and use of Frontier is requested" )

    # SQLite
    sqliteconnections = [conn for conn in connectionServices if conn.startswith("sqlite_file")]
    if len(sqliteconnections)>0:
        for conn in sqliteconnections:
            connectionParameters = _getConnectionParameters( conn )
            if connectionParameters["filename"] is not None:
                break # stop at the first sqlite file that exists
        if connectionParameters["filename"] is not None:
            log.info("Using sqlite connection %s", connectionParameters)
            return connectionParameters
        else:
            if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
                log.fatal("environment ATLAS_TRIGGERDB_FORCESQLITE is defined but non of the sqlite files defined in dblookup.xml exists" )
    else:
        if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
            log.fatal("environment ATLAS_TRIGGERDB_FORCESQLITE is defined but no sqlite connection defined in dblookup.xml" )

    from CoolConvUtilities.AtlCoolLib import replicaList
    serverlist=['ATLAS_CONFIG' if s=='ATLAS_COOLPROD' else s for s in replicaList()]  # replicaList is for COOL, I need ATLAS_CONFIG instead of ATLAS_COOLPROD
    log.info("Trying these servers in order %r", serverlist)
    for server in serverlist:
        log.info("Trying server %s", server)

        if server=='ATLF':
            if not os.getenv('TRIGGER_USE_FRONTIER',False):
                continue
            frontierconnections = [conn for conn in connectionServices if conn.startswith("frontier")]
            if len(frontierconnections) == 0:
                log.debug("FroNTier connection not defined for alias %s in dblookup", connection )
                continue
            log.info("Environment FRONTIER_SERVER: %s", os.getenv('FRONTIER_SERVER','not defined'))
            frontierServer = os.getenv('FRONTIER_SERVER',None)
            if not frontierServer:
                log.debug("No environment variable FRONTIER_SERVER" )
                continue
            connectionParameters = _getConnectionParameters( frontierconnections[0] )
            connectionParameters['url'] = frontierServer
            log.info("Using frontier connection %s", frontierconnections[0])
            #connstr='frontier://ATLF/%s;schema=%s;dbname=TRIGCONF' % (connectionParameters['url'],connectionParameters["schema"])
            break
        elif server=='atlas_dd':
            continue
        else:
            oracleconnections = [conn for conn in connectionServices if conn.startswith("oracle://%s/" % server)]
            if len(oracleconnections) == 0:
                log.debug("Oracle connection not defined for server %s in dblookup", server )
                continue
            connectionParameters = _getConnectionParameters( oracleconnections[0] )
            log.info("Using oracle connection %s", oracleconnections[0])
            break

    return connectionParameters


def _get_sqlite_cursor (filename):
    try:
        import sqlite3
    except ImportError:
        raise RuntimeError ("ERROR: Can't import sqlite3?")
    os.lstat(filename)
    connection = sqlite3.connect(filename)
    return connection.cursor()

def _get_oracle_cursor (tns, user, passwd=""):
    if passwd=="":
        from getpass import getpass
        passwd = getpass("[Oracle] database password for %s@%s: " % (user, tns))
    try:
        from cx_Oracle import connect
    except ImportError:
        raise RuntimeError ("ERROR: Can't import cx_Oracle?")
    connection = connect (user, passwd, tns, threaded=True)
    return connection.cursor()


def _get_mysql_cursor (host, db, user, passwd=""):
    if passwd=="":
        from getpass import getpass
        passwd = getpass("[MySQL] `%s' database password for %s@%s: " % (db, user, host))
    try:
        from MySQLdb import connect
    except ImportError:
        raise RuntimeError ("ERROR: Can't import MySQLdb")
    connection = connect(host=host, user=user, passwd=passwd, db=db, connect_timeout=10)
    return connection.cursor()



def getTriggerDBCursor(run_number = None, smk = None, isRun2 = None):

    index = 1 if isTriggerRun2(run_number = run_number, smk = smk, isRun2 = isRun2 ) else 0

    global __cursor_schema
    if __cursor_schema[index][0]:
        return __cursor_schema[index] # return the correct cursor and schema name

    # need to connect
    connection = triggerDBAlias(run_number = run_number, smk = smk, isRun2 = isRun2)

    connectionParameters = interpretConnection(connection)
    technology = connectionParameters["techno"]

    if technology == 'sqlite':
        cursor = _get_sqlite_cursor(connectionParameters["filename"])
        schema = ''

    elif technology == 'oracle':
        cursor = _get_oracle_cursor(connectionParameters["server"], connectionParameters["user"], connectionParameters["passwd"])
        schema = connectionParameters["schema"].rstrip('.') + '.'

    elif technology == 'frontier':
        from TrigConfigSvc.TrigConfFrontier import getFrontierCursor 
        cursor = getFrontierCursor(connectionParameters["url"], connectionParameters["schema"], logging.getLogger("TrigConfigSvcUtils.py").level)
        schema = connectionParameters["schema"].rstrip('.') + '.'

    elif technology == 'mysql':
        cursor = _get_mysql_cursor(connectionParameters["server"], connectionParameters["dbname"], connectionParameters["user"], connectionParameters["passwd"]),''
        schema = ''

    __cursor_schema[index] = (cursor,schema)

    return __cursor_schema[index] # return the correct cursor and schema name




def getUsedTables(output, condition, schemaname, tables):
    schemaname = schemaname.rstrip('.')+'.'
    usedtables = set()
    for o in output:
        usedtables.add(o.split('.')[0])
    for c in condition:
        for p in c.split():
            if '.' in p and '\'' not in p:
                usedtables.add(p.split('.')[0])
    return ["%s%s %s" % (schemaname,tables[t],t) for t in usedtables]


def executeQuery(cursor, output, condition, schemaname, tables, bindvars=()):
    query = 'select distinct %s from %s where %s' % \
            (', '.join(output),
             ', '.join(getUsedTables(output, condition, schemaname, tables)),
             ' and '.join(condition))
    if len(bindvars)==0:
        cursor.execute(str(query))
    else:
        cursor.execute(str(query),bindvars)

    return cursor.fetchall()


def getL1PskNames(psk, run_number = None , smk = None , isRun2 = None ):
    if type(psk) == set:
        psk = list(psk)
    if type(psk) != list:
        psk = [psk]

    keyslist = ','.join([str(k) for k in psk if k])

    if keyslist=="":
        return {}

    tables = { 'L' : 'L1_PRESCALE_SET' }
    output = [ 'L.L1PS_ID', 'L.L1PS_NAME' ]
    condition = [ "L.L1PS_ID in (%s)" % keyslist ]

    cursor, schema = getTriggerDBCursor(run_number = run_number, smk = smk, isRun2 = isRun2 )
    res = executeQuery(cursor, output, condition, schema, tables)

    return dict(res)



def getHLTPskNames(psk, run_number = None , smk = None , isRun2 = None ):
    cursor,schema = getTriggerDBCursor(run_number = run_number, smk = smk, isRun2 = isRun2 )

    if type(psk) == set:
        psk = list(psk)
    if type(psk) != list:
        psk = [psk]

    if len(psk)==0:
        return {}

    prescales = [str(k) for k in psk if k]
    if len(prescales)==0:
        return {}

    tables = { 'L' : 'HLT_PRESCALE_SET' }
    output = ['L.HPS_ID', 'L.HPS_NAME']
    condition = [ "L.HPS_ID in (%s)" % ','.join(prescales) ]

    res = executeQuery( cursor, output, condition, schema, tables)


    return dict(res)


def getSmkNames( allSMK ):
    """
    takes a single SMK or a list of SMKs

    returns a map from SMK to (name,version,comment)
    """

    if type( allSMK ) == set:
        allSMK = list( allSMK )
    if type( allSMK ) != list:
        allSMK = [ allSMK ]

    if len( allSMK )==0:
        return {}


    run1smks = [smk for smk in  allSMK if not isTriggerRun2(smk = smk ) ]
    run2smks = [smk for smk in  allSMK if isTriggerRun2(smk = smk ) ]

    smkMap = {}  # map 

    for smklist in [ run1smks, run2smks]:

        if len(smklist)==0:
            continue
    
        tables = { 'S' : 'SUPER_MASTER_TABLE' }
        output = ['S.SMT_ID', 'S.SMT_NAME', 'S.SMT_VERSION', 'S.SMT_COMMENT']
        condition = [ "S.SMT_ID in (%s)" % ','.join([str(k) for k in smklist]) ]

        cursor, schema = getTriggerDBCursor( smk = smklist[0] )
        res = executeQuery( cursor, output, condition, schema, tables)

        smkMap.update( dict([ (r[0],(r[1],r[2],r[3])) for r in res]) )

    return smkMap




def getHLTMenu(smk):

    if not smk.isdigit():
        return [],[]

    if isTriggerRun2(smk):
        output = ['TC.HTC_NAME', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_LOWER_CHAIN_NAME' ]
    else:
        output = ['TC.HTC_NAME', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_LOWER_CHAIN_NAME', 'TC.HTC_L2_OR_EF' ]
    
    tables = { 'SM'  : 'SUPER_MASTER_TABLE',
               'HM'  : 'HLT_MASTER_TABLE',
               'M2C' : 'HLT_TM_TO_TC',
               'TC'  : 'HLT_TRIGGER_CHAIN'
               }

    
    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
                  'HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID' ]
    
    bindvars = { "smk": smk }
    
    cursor,schema = getTriggerDBCursor(smk)
    res = executeQuery( cursor, output, condition, schema, tables, bindvars)

    l2chains = []
    efchains = []

    for r in res:
        if len(r)==4 and r[3]=='L2':
            l2chains += [TriggerChain(r[0],r[1],r[2], level=2)]
        else:
            efchains += [TriggerChain(r[0],r[1],r[2], level=3)]

    return l2chains, efchains




def getL1Menu(smk):

    if not smk.isdigit():
        return []

    output = ['TI.L1TI_NAME', 'TI.L1TI_CTP_ID' ]
    
    tables = { 'SM'  : 'SUPER_MASTER_TABLE',
               'M'   : 'L1_MASTER_TABLE',
               'M2I' : 'L1_TM_TO_TI',
               'TI'  : 'L1_TRIGGER_ITEM'
               }
    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_L1_MASTER_TABLE_ID = M.L1MT_ID',
                  'M.L1MT_TRIGGER_MENU_ID = M2I.L1TM2TI_TRIGGER_MENU_ID',
                  'M2I.L1TM2TI_TRIGGER_ITEM_ID = TI.L1TI_ID' ]
    bindvars = { "smk": smk }

    cursor,schema = getTriggerDBCursor(smk)
    res = executeQuery( cursor, output, condition, schema, tables, bindvars)

    maxNumberItems = 512 if isTriggerRun2(smk=smk) else 256
    items = maxNumberItems*[None]

    for r in res:
        items[r[1]] = TriggerChain(r[0],r[1])
    return items



def getL1Prescales(l1prescalekey, run_number):

    maxNumberItems = 512 if isTriggerRun2( run_number = run_number ) else 256

    tables = { 'L' : 'L1_PRESCALE_SET' }
    output = ['L.L1PS_VAL%i' % i for i in range( 1, maxNumberItems + 1)]
    condition = [ "L.L1PS_ID = :psk" ]
    bindvars = { "psk": l1prescalekey }

    cursor,schema = getTriggerDBCursor( run_number = run_number )
    res = executeQuery(cursor, output, condition, schema, tables, bindvars)
    
    return res[0][0:maxNumberItems]


def getHLTPrescales(hltprescalekey, run_number):

    l2ps = {}
    efps = {}

    if isTriggerRun2( run_number = run_number ):
        return l2ps,efps
    
    tables = { 'H' : 'HLT_PRESCALE_SET',
               'P' : 'HLT_PRESCALE'
               }
    output = [ 'P.HPR_L2_OR_EF', 'P.HPR_CHAIN_COUNTER', 'P.HPR_PRESCALE', 'P.HPR_PASS_THROUGH_RATE' ]
    condition = [ "P.HPR_PRESCALE_SET_ID = :psk", "P.HPR_L2_OR_EF not like 'express'" ]
    bindvars = { "psk": hltprescalekey }

    cursor,schema = getTriggerDBCursor(isRun2 = True)
    res = executeQuery(cursor, output, condition, schema, tables, bindvars)

    for r in res:
        if r[0]=='L2':
            l2ps[r[1]] = ( float(r[2]), float(r[3]) )
        elif (r[0]=='EF' or r[0]=='HLT' or r[0]=='HL'):
            efps[r[1]] = ( float(r[2]), float(r[3]) )

    return l2ps, efps


def getRandom(smk):
    if smk=='n.a.':
        return ('n.a.','n.a.')

    tables = { 'S' : 'SUPER_MASTER_TABLE',
               'M' : 'L1_MASTER_TABLE',
               'R' : 'L1_RANDOM'
               }
    if isTriggerRun2(smk=smk):
        output = [ 'R.L1R_CUT0', 'R.L1R_CUT1', 'R.L1R_CUT2', 'R.L1R_CUT3' ]
    else:
        output = [ 'R.L1R_RATE1', 'R.L1R_RATE2' ]
    
    condition = [ "S.SMT_ID=:smk", "S.SMT_L1_MASTER_TABLE_ID=M.L1MT_ID", "M.L1MT_RANDOM_ID=R.L1R_ID" ]
    
    bindvars = { "smk": smk }

    cursor,schema = getTriggerDBCursor(smk=smk)
    res = executeQuery(cursor, output, condition, schema, tables, bindvars)
    if len(res) > 0:
        return res[0]
    else:
        return [0,0,0,0]


if __name__=="__main__":
    #print (getL1PskNames([2517, 2518, 2284, 2536]))

    #print (getHLTPskNames([2517, 2530, 2537, 2491]))
    
    #print (getSmkNames([931, 932, 933]))

    #print (getL1Menu(931))

    #print (getHLTMenu(931))

    #print (getL1Prescales(2517))

    #print (getHLTPrescales(2530))

    print (getRandom(1038))
