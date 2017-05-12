#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from xml.dom import minidom
import re
import os

try:
    _set = set
except NameError:
    from sets import Set
    _set = Set

  
from AthenaCommon.Logging import logging
from AthenaCommon.Utils.unixtools import FindFile

log = logging.getLogger( "TrigConfigSvcUtils.py" )
if log.level==0: log.setLevel(logging.INFO)
    
#**
# In this section:
#
# interpretation of DB alias and creating DB connection when running Reco transforms
#**
def _getFileLocalOrPath(filename, pathenv):
    """looks for filename in local directory and then in all paths specified in environment variable 'pathenv'
    returns path/filename if existing, otherwise None
    """
    if os.path.exists(filename):
        log.info( "Using local file %s" % filename)
        return filename

    pathlist = os.getenv(pathenv,'').split(os.pathsep)
    resolvedfilename = FindFile(filename, pathlist, os.R_OK)
    if resolvedfilename:
        return resolvedfilename

    log.fatal("No file %s found locally nor in %s" % (filename, os.getenv('CORAL_DBLOOKUP_PATH')) )
    return None
    

def _getConnectionServicesForAlias(alias):

    connectionServices = None # list of services

    dblookupfilename = _getFileLocalOrPath('dblookup.xml','CORAL_DBLOOKUP_PATH')
    if dblookupfilename == None: return None

    doc = minidom.parse(dblookupfilename)
    for ls in doc.getElementsByTagName('logicalservice'):
        if ls.attributes['name'].value != alias: continue
        connectionServices = [str(s.attributes['name'].value) for s in ls.getElementsByTagName('service')]
    doc.unlink()

    log.info( "For alias '%s' found list of connections %r" % (alias,connectionServices) )
    if connectionServices == None:
        log.fatal("Trigger connection alias '%s' is not defined in %s" % (alias,dblookupfilename))
    return connectionServices


def _readAuthentication():
    """ read authentication.xml, first from local directory, then from all paths specified in CORAL_AUTH_PATH

    returns dictionary d with d[connection] -> (user,pw)
    """

    authDict = {}

    dbauthfilename = _getFileLocalOrPath('authentication.xml','CORAL_AUTH_PATH')
    if dbauthfilename == None: return authDict

    doc = minidom.parse(dbauthfilename)        
    for cn in doc.getElementsByTagName('connection'):
        user = ""
        pw = ""
        svc = cn.attributes['name'].value
        for p in cn.getElementsByTagName('parameter'):
            if p.attributes['name'].value == 'user': user = p.attributes['value'].value
            if p.attributes['name'].value == 'password': pw = p.attributes['value'].value
        authDict[cn.attributes['name'].value] = (user,pw)
    doc.unlink()
    return authDict



authDict = None

def _getConnectionParameters(connection):
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
            #print 'Groups ', m
            if not m:
                log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern))
            (techno, server, schema, user, passwd) = m.groups()
            #print (techno, server, schema, user, passwd)
        else:
            global authDict
            if not authDict: authDict = _readAuthentication()
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
        pattern = "frontier://ATLF/\(\)/(.*)"
        m = re.match(pattern,connection)
        if not m:
            log.fatal("connection string '%s' doesn't match the pattern '%s'?" % (connection,pattern) )
        (schema, ) = m.groups()
        connectionParameters["techno"] = 'frontier'
        connectionParameters["schema"] = schema

    return connectionParameters


def interpretConnection(connection, debug=True, resolveAlias=True):
    # connection needs to be of the following format (this is also the order of checking)
    # <ALIAS>                              -- any string without a colon ':' will be checked for in the dblookup.xml file
    # type:<detail>                        -- no dblookup will be used, type has to be oracle, mysql, or sqlite_file
    # sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2  -- a service description without user and password, requires lookup in authentication.xml
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    log.info("Specified connection string '%s'" % connection)

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
    if connectionServices == None:
        return connectionParameters

    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf

    # SQLite
    sqliteconnections = [conn for conn in connectionServices if conn.startswith("sqlite_file")]
    if len(sqliteconnections)>0:
        for conn in sqliteconnections:
            connectionParameters = _getConnectionParameters( conn )
            if connectionParameters["filename"] != None:
                break # stop at the first sqlite file that exists
        if connectionParameters["filename"] != None:
            log.info("Using sqlite connection %s" % connectionParameters)
            return connectionParameters
        else:
            if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
                log.fatal("environment ATLAS_TRIGGERDB_FORCESQLITE is defined but non of the sqlite files defined in dblookup.xml exists" )
    else:
        if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
            log.fatal("environment ATLAS_TRIGGERDB_FORCESQLITE is defined but no sqlite connection defined in dblookup.xml" )

    # replicaList
    from CoolConvUtilities.AtlCoolLib import replicaList
    serverlist=['ATLAS_CONFIG' if s=='ATLAS_COOLPROD' else s for s in replicaList()]  # replicaList is for COOL, I need ATLAS_CONFIG instead of ATLAS_COOLPROD
    
    #serverlist=['ATLF']
    log.info("Trying these servers in order %r" % serverlist)
    for server in serverlist:
        
        log.info("Trying server %s" % server)

        if server=='ATLF':
            #if not tf.triggerUseFrontier() and not os.getenv('TRIGGER_USE_FRONTIER',False): continue
            frontierconnections = [conn for conn in connectionServices if conn.startswith("frontier")]
            if len(frontierconnections) == 0:
                log.debug("FroNTier connection not defined for alias %s in dblookup" % connection )
                continue
            log.info("Environment FRONTIER_SERVER: %s" % os.getenv('FRONTIER_SERVER','not defined'))
            frontierServer = os.getenv('FRONTIER_SERVER',None)
            if not frontierServer:
                log.debug("No environment variable FRONTIER_SERVER" )
                continue
            connectionParameters = _getConnectionParameters( frontierconnections[0] )
            connectionParameters['url'] = frontierServer
            log.info("Using frontier connection %s" % frontierconnections[0])
            #connstr='frontier://ATLF/%s;schema=%s;dbname=TRIGCONF' % (connectionParameters['url'],connectionParameters["schema"])
            break
        elif server=='atlas_dd': continue
        else:
            oracleconnections = [conn for conn in connectionServices if conn.lower().startswith("oracle://%s/" % server.lower())]
            if len(oracleconnections) == 0:
                log.debug("Oracle connection not defined for server %s in dblookup" % server )
                continue
            print oracleconnections
            connectionParameters = _getConnectionParameters( oracleconnections[0] )
            log.info("Using oracle connection %s" % oracleconnections[0])
            #connstr='oracle://%s;schema=ATLAS_%s;dbname=TRIGCONF' % (connectionParameters["server"],connectionParameters["schema"])
            break

    return connectionParameters


def getTriggerDBCursor(connection):

    print "Connection  " + connection
    connectionParameters = interpretConnection(connection)
    
    print connectionParameters

    technology = connectionParameters["techno"]
    
    print "############## " + technology

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

    return cursor,schema

def _get_sqlite_cursor (filename):
    try: import sqlite3
    except ImportError: raise RuntimeError, "ERROR: Can't import sqlite3?"
    os.lstat(filename)
    connection = sqlite3.connect(filename)
    return connection.cursor()

def _get_oracle_cursor (tns, user, passwd=""):
    if passwd=="":
        from getpass import getpass
        passwd = getpass("[Oracle] database password for %s@%s: " % (user, tns))

    try: import cx_Oracle as cx
    except ImportError: raise RuntimeError, "ERROR: Can't import cx_Oracle?"
    connection = cx.connect (user, passwd, tns, threaded=True)
    return connection.cursor()


def _get_mysql_cursor (host, db, user, passwd=""):
    if passwd=="":
        from getpass import getpass
        passwd = getpass("[MySQL] `%s' database password for %s@%s: " % (db, user, host))

    try: from MySQLdb import connect
    except ImportError: raise RuntimeError, "ERROR: Can't import MySQLdb?"
    connection = connect(host=host, user=user, passwd=passwd, db=db, connect_timeout=10)
    return connection.cursor()




def getUsedTables(output, condition, schemaname, tables):
    usedtables = _set()
    
    for o in output:
        usedtables.add(o.split('.')[0])

    for c in condition:
        for p in c.split():
            if '.' in p and not '\'' in p: usedtables.add(p.split('.')[0].lstrip('('))
    return ["%s%s %s" % (schemaname,tables[t],t) for t in usedtables]


def executeQuery(cursor, output, condition, schemaname, tables, bindvars=()):
    query = 'select distinct %s from %s' % \
            (', '.join(output),
             ', '.join(getUsedTables(output, condition, schemaname, tables)))
    

    if condition:
        query += ' where ' + ' and '.join(condition)

    if len(bindvars)==0:
        log.debug("Executing query %s" % query)
        cursor.execute(str(query))
    else:
        log.debug("Executing query %s with bound variables %r" % (query, bindvars))
        cursor.execute(str(query),bindvars)

    return cursor.fetchall()



def getL1TopoMenu(cursor, schemaname, smk):
    
    output = ['TMT.TMT_VERSION', 'TMT.TMT_NAME', 'TMT.TMT_TRIGGER_MENU_ID']
    
    tables = {}
    tables['SMT']  = 'SUPER_MASTER_TABLE'
    tables['TMT']  = 'TOPO_MASTER_TABLE'

    condition = [ 'SMT.SMT_ID = :smk',
                  'SMT.SMT_TOPO_MASTER_TABLE_ID = TMT.TMT_ID'
                  ]
    
    bindvars = { "smk": smk }
    
    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    version = ""
    name = ""
    menu_id = 0

    if (len(res) == 0):
        log.fatal(" Can not retrieve the L1Topo menu")
    else :
        version = res[0][0]
        name = res[0][1]
        menu_id = res[0][2]
   
    return version, name, menu_id

def getL1TopoOutputLines(cursor, schemaname, menu_id):
    
    output = [ 'LINE.TOL_TRIGGERLINE', 'LINE.TOL_ALGO_NAME', 'LINE.TOL_ALGO_ID']
    output += ['LINE.TOL_MODULE', 'LINE.TOL_FPGA', 'LINE.TOL_CLOCK', 'LINE.TOL_FIRST_BIT']
    
    tables = {}
    tables['TTM']  = 'TOPO_TRIGGER_MENU'
    tables['LIST'] = 'TOPO_OUTPUT_LIST'
    tables['LINK'] = 'TOPO_OUTPUT_LINK'
    tables['LINE'] = 'TOPO_OUTPUT_LINE'

    condition = [ 'TTM.TTM_ID = :menu_id',
                  'TTM.TTM_CTPLINK_ID = LIST.OL_ID' ,
                  'LIST.OL_ID = LINK.TL_LINK_ID',
                  'LINK.TL_OUTPUT_ID = LINE.TOL_ID'
                  ]
    
    bindvars = { "menu_id": menu_id }
    
    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)

    if (len(res) == 0):
        log.fatal(" Can not retrieve the L1Topo output lines")

    # get a list of output lines
    lines = []
    for x in res:
        bit_index = 0
        for  y in x[0].split(","):
            # increment the bit position from first_bit
            bit = x[6]+bit_index
            lines.append((y.split(",")[0],) + x[1:6] + (bit,))
            bit_index += 1

    return lines

def getL1TopoTriggerItems(cursor, schemaname, smk, topo_name):
    
    output = [ 'L1TI.L1TI_CTP_ID', 'L1TI.L1TI_NAME']
   
    tables = {}
    tables['SMT']   = 'SUPER_MASTER_TABLE'
    tables['L1MT']  = 'L1_MASTER_TABLE'
    tables['L1TM']  = 'L1_TRIGGER_MENU'
    tables['TM2TI'] = 'L1_TM_TO_TI' 
    tables['L1TI']  = 'L1_TRIGGER_ITEM'
    tables['TI2TT'] = 'L1_TI_TO_TT' 
    tables['L1TT']  = 'L1_TRIGGER_THRESHOLD'

    condition = [ 'SMT.SMT_ID = :smk',
                  'SMT.SMT_L1_MASTER_TABLE_ID = L1MT.L1MT_ID',
                  'L1MT.L1MT_TRIGGER_MENU_ID = L1TM.L1TM_ID',
                  'L1TM.L1TM_ID = TM2TI.L1TM2TI_TRIGGER_MENU_ID',
                  'TM2TI.L1TM2TI_TRIGGER_ITEM_ID = L1TI.L1TI_ID',
                  'L1TI.L1TI_ID = TI2TT.L1TI2TT_TRIGGER_ITEM_ID',
                  'TI2TT.L1TI2TT_TRIGGER_THRESHOLD_ID = L1TT.L1TT_ID',
                  'L1TT.L1TT_NAME = :topo_name'
                  ]
    
    bindvars = { "smk": smk, "topo_name": topo_name }
    

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)

    return res




def getL1TopoAlgorithms(cursor, schemaname, menu_id):
    
    output = ['TA.TA_ID', 'TA.TA_NAME', 'TA.TA_ALGO_ID', 'TA.TA_TYPE', 'TA.TA_SORT_DECI']
    
    tables = {}
    tables['M2A'] = 'TTM_TO_TA'
    tables['TA']  = 'TOPO_ALGO'

    condition = [ 'M2A.TTM2TA_MENU_ID = :menu_id',
                  'M2A.TTM2TA_ALGO_ID = TA.TA_ID'
                  ]
    
    bindvars = { "menu_id": menu_id }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    if (len(res) == 0):
        log.fatal(" Can not retrieve the L1Topo algorithms for menu_id " + str(menu_id))
   
    return res 

def getL1TopoAlgoInputs(cursor, schemaname, to_id):
    
    output = ['TI.TAI_NAME', 'TI.TAI_VALUE', 'TI.TAI_POSITION']
    
    tables = {}
    tables['A2I'] = 'TA_TO_TI'
    tables['TI']  = 'TOPO_ALGO_INPUT'

    condition = [ 'A2I.TA2TI_ALGO_ID = :to_id',
                  'A2I.TA2TI_INPUT_ID = TI.TAI_ID' 
                  ]
    
    bindvars = { "to_id": to_id }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    if (len(res) == 0):
        log.warning(" Can not retrieve the L1Topo algorithms inputs for algoId " + str(to_id))
   
    return res 

def getL1TopoAlgoOutputs(cursor, schemaname, to_id):
    
    output = ['TAO.TAO_NAME', 'TAO.TAO_VALUE', 'TAO.TAO_SELECTION', 'TAO.TAO_BITNAME']
    
    tables = {}
    tables['A2O'] = 'TA_TO_TO'
    tables['TAO'] = 'TOPO_ALGO_OUTPUT'

    condition = [ 'A2O.TA2TO_ALGO_ID = :to_id',
                  'A2O.TA2TO_OUTPUT_ID = TAO.TAO_ID' 
                  ]
    
    bindvars = { "to_id": to_id }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    if (len(res) == 0):
        log.warning(" Can not retrieve the L1Topo algorithms outputs for algoId " + str(to_id))
   
    return res 

def getL1TopoAlgoFixedParameters(cursor, schemaname, to_id):
    
    output = ['TG.TG_NAME', 'TG.TG_VALUE']
    
    tables = {}
    tables['A2G'] = 'TA_TO_TG'
    tables['TG'] = 'TOPO_GENERIC'

    condition = [ 'A2G.TA2TG_ALGO_ID = :to_id',
                  'A2G.TA2TG_GENERIC_ID = TG.TG_ID' 
                  ]
    
    bindvars = { "to_id": to_id }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    if (len(res) == 0):
        log.warning(" Can not retrieve the L1Topo algorithms fixed parameters for algoId " + str(to_id))
   
    return res 

def getL1TopoAlgoRegisterParameters(cursor, schemaname, to_id):
    
    output = ['TP.TP_NAME', 'TP.TP_VALUE', 'TP.TP_POSITION', 'TP.TP_SELECTION']
    
    tables = {}
    tables['A2P'] = 'TA_TO_TP'
    tables['TP'] = 'TOPO_PARAMETER'

    condition = [ 'A2P.TA2TP_ALGO_ID = :to_id',
                  'A2P.TA2TP_PARAM_ID = TP.TP_ID' 
                  ]
    
    bindvars = { "to_id": to_id }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    if (len(res) == 0):
        log.warning(" Can not retrieve the L1Topo algorithms register parameters for algoId " + str(to_id))
   
    return res 
