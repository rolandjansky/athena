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


def interpretConnection(connection, debug=False, resolveAlias=True):
    # connection needs to be of the following format (this is also the order of checking)
    # <ALIAS>                              -- any string without a colon ':' will be checked for in the dblookup.xml file
    # type:<detail>                        -- no dblookup will be used, type has to be oracle, mysql, or sqlite_file
    # sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2  -- a service description without user and password, requires lookup in authentication.xml
    # oracle://ATLR/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    log.info("Specified connection string '%s'" % connection)

    # not needed any longer
    # connection = connection.lstrip("dblookup://")
    # connection = connection.lstrip("dblookup:")
    # connection = connection.rstrip(";")
    #print connection

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


    # If TriggerFlags.triggerUseFrontier=true then we remove sqlite files
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    #if tf.triggerUseFrontier() or os.getenv('TRIGGER_USE_FRONTIER',False):
    #    connectionServices = filter(lambda conn: not conn.startswith("sqlite_file"), connectionServices)
    #    if 'ATLAS_TRIGGERDB_FORCESQLITE' in os.environ:
    #        log.fatal("Inconsistent setup: environment variable ATLAS_TRIGGERDB_FORCESQLITE is defined and use of Frontier is requested" )


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
            connectionParameters = _getConnectionParameters( oracleconnections[0] )
            log.info("Using oracle connection %s" % oracleconnections[0])
            #connstr='oracle://%s;schema=ATLAS_%s;dbname=TRIGCONF' % (connectionParameters["server"],connectionParameters["schema"])
            break

    return connectionParameters


def getTriggerDBCursor(connection):

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

    try: from cx_Oracle import connect
    except ImportError: raise RuntimeError, "ERROR: Can't import cx_Oracle?"
    connection = connect (user, passwd, tns, threaded=True)
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


def isRun2(cursor,schemaname):
    import cx_Oracle
    if not hasattr(cursor,'connection') or type(cursor.connection)!=cx_Oracle.Connection:
        log.warning('Detection of DB schema only supported for Oracle. Will assume run-2')
        return True

    owner = schemaname.rstrip('.')
    query = "select table_name from all_tables where table_name='ACTIVE_MASTERS' and owner='%s'" % owner
    cursor.execute(query)
    return (len(cursor.fetchall())>0)
    
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



def getAlgorithmsForMenu(connection, smk):
    cursor,schemaname = getTriggerDBCursor(connection)

    output = ['TC.HTC_L2_OR_EF', 'C.HCP_NAME', 'C.HCP_ALIAS' ]
    
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['HM']    = 'HLT_MASTER_TABLE'
    tables['TM']    = 'HLT_TRIGGER_MENU'
    tables['M2C']   = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'
    tables['C2S']   = 'HLT_TC_TO_TS'
    tables['S2TE']  = 'HLT_TS_TO_TE'
    tables['TE2C']  = 'HLT_TE_TO_CP'
    tables['TE2TE'] = 'HLT_TE_TO_TE'
    tables['C']     = 'HLT_COMPONENT'

    condition = [ "SM.SMT_ID = '%i'" % smk,
                  'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
                  'HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = C2S.HTC2TS_TRIGGER_CHAIN_ID',
                  'C2S.HTC2TS_TRIGGER_SIGNATURE_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID',
                  'S2TE.HTS2TE_TRIGGER_ELEMENT_ID = TE2C.HTE2CP_TRIGGER_ELEMENT_ID',
                  'TE2C.HTE2CP_COMPONENT_ID = C.HCP_ID' ]

    res = executeQuery(cursor, output, condition, schemaname, tables)

    l2algs=[]
    efalgs=[]
    for x in res:        
        if x[0]=='L2':
            l2algs += ["%s/%s" % (x[1],x[2])]
        else:
            efalgs += ["%s/%s" % (x[1],x[2])]

    return l2algs, efalgs




def getAlgorithmsForMenuRun2(connection, smk):
    cursor,schemaname = getTriggerDBCursor(connection)

    output = [ 'C.HCP_NAME', 'C.HCP_ALIAS' ]
    
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['HM']    = 'HLT_MASTER_TABLE'
    tables['TM']    = 'HLT_TRIGGER_MENU'
    tables['M2C']   = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'
    tables['C2S']   = 'HLT_TC_TO_TS'
    tables['S2TE']  = 'HLT_TS_TO_TE'
    tables['TE2C']  = 'HLT_TE_TO_CP'
    tables['TE2TE'] = 'HLT_TE_TO_TE'
    tables['C']     = 'HLT_COMPONENT'

    condition = [ "SM.SMT_ID = '%i'" % smk,
                  'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
                  'HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = C2S.HTC2TS_TRIGGER_CHAIN_ID',
                  'C2S.HTC2TS_TRIGGER_SIGNATURE_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID',
                  'S2TE.HTS2TE_TRIGGER_ELEMENT_ID = TE2C.HTE2CP_TRIGGER_ELEMENT_ID',
                  'TE2C.HTE2CP_COMPONENT_ID = C.HCP_ID' ]

    res = executeQuery(cursor, output, condition, schemaname, tables)

    allalgs=[]
    for x in res:        
        allalgs += ["%s/%s" % (x[0],x[1])]

    return allalgs



def getPropertyFromDB(connection, smk, component, parameter):
    """Get property value from DB. smk can be a single SMK or a list/tuple of SMKs.
    SQL wildcards (%) can be used in both component and parameter names.
    Return [(SMK,Component,Parameter,Value,Level)]
    """
    
    cursor,schemaname = getTriggerDBCursor(connection)

    isrun2 = isRun2(cursor,schemaname)
    output = ['SM.SMT_ID', 'HCP.HCP_NAME', 'PAR.HPA_NAME', 'PAR.HPA_VALUE', 'HS.HST_ID']
    output += ['HM.HMT_SETUP_ID' if isrun2 else 'HM.HMT_L2_SETUP_ID']
    
    tables = {'SM' : 'SUPER_MASTER_TABLE',
              'HM' : 'HLT_MASTER_TABLE',
              'HCP': 'HLT_COMPONENT',
              'HS' : 'HLT_SETUP',
              'HST2CP' : 'HLT_ST_TO_CP',
              'HCP2PA' : 'HLT_CP_TO_PA',
              'PAR'    : 'HLT_PARAMETER'
              }

    if type(smk)!=list and type(smk)!=tuple:
        smk = [smk]

    condition = ["SM.SMT_ID IN (%s)" % ",".join([str(i) for i in smk]),
                 'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
                 ('HM.HMT_SETUP_ID = HS.HST_ID' if isrun2 else '(HM.HMT_L2_SETUP_ID = HS.HST_ID or HM.HMT_EF_SETUP_ID = HS.HST_ID)'),
                 'HST2CP.HST2CP_SETUP_ID = HS.HST_ID',
                 'HST2CP.HST2CP_COMPONENT_ID = HCP2PA.HCP2PA_COMPONENT_ID',
                 'HST2CP.HST2CP_COMPONENT_ID = HCP.HCP_ID',
                 'HCP2PA.HCP2PA_PARAMETER_ID = PAR.HPA_ID',
                 "HCP.HCP_NAME like '%s'" % component,
                 "PAR.HPA_NAME like '%s'" % parameter]

    res = executeQuery(cursor, output, condition, schemaname, tables)
    
    if isrun2: return [ tuple(x[:4]+("HLT",) ) for x in res ]
    else: return [ tuple(x[:4]+("L2" if x[4]==x[5] else "EF",) ) for x in res ]
    
def getMenuNameFromDB(connection, hltprescalekey):

    cursor,schemaname = getTriggerDBCursor(connection)
    
    tables = { 'HPS' : 'HLT_PRESCALE_SET' }

    output = ['HPS.HPS_NAME']

    condition = [ "HPS.HPS_ID = '%i'" % hltprescalekey ]
    
    res = executeQuery(cursor, output, condition, schemaname, tables)

    # now we need to do some logic, related to the 

    print res

    hltpsName = str(res[0][0])

    # temporarily here, but should use the function from TMP
    m = re.match( "(.*)_default_prescale", hltpsName)
    menuName = m.group(1) if m else hltpsName

    log.info("Interpreting menu name from HLT prescale key %i: %s" % (hltprescalekey,menuName))

    return menuName

def getKeysFromName(connection, name, MCOnly=False):
    cursor,schemaname = getTriggerDBCursor(connection)
    
    output = ['SM.SMT_ID', 'SM.SMT_NAME']
    
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'

    condition = [ "SM.SMT_NAME like '%s'" % name ]

    res = executeQuery(cursor, output, condition, schemaname, tables)
    return res

def getKeysFromNameRelease(connection, name, release, l1only):
    
    cursor,schemaname = getTriggerDBCursor(connection)
    
    smname = name.split('__')[0]
    print 'SM name ', smname
    print 'PS name ', name
    print 'release ', release
    keys = []

    #Find the Release id
    tables = {}
    tables['RE'] = 'HLT_RELEASE'

    output = ['RE.HRE_ID']
    condition = [ "RE.HRE_NAME like '%s'" %release ]

    rel = executeQuery(cursor, output, condition, schemaname, tables)
    relid = (str(rel[-1])).lstrip('(').rstrip(')').split(',')[0]
    
    #Find the supermaster key
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['SM2RE'] = 'HLT_SMT_TO_HRE'

    output = ['SM.SMT_ID']
    condition = [ "SM.SMT_NAME like '%s'" % smname,
                  "SM.SMT_ID = SM2RE.SMT2RE_SUPER_MASTER_TABLE_ID",
                  "SM2RE.SMT2RE_RELEASE_ID = '%s'" % relid]
    
    smk  = executeQuery(cursor, output, condition, schemaname, tables)
    smid = (str(smk[-1])).lstrip('(').rstrip(')').split(',')[0]
    #NB no check if we return more than one smk! (take the first here with the split!)

    #Find the L1 prescale key with the name and linked to this L1 master
    tables = {}
    tables['LPS']   = 'L1_PRESCALE_SET'
    tables['PS2RE'] = 'L1_HRE_TO_PS'
    tables['RE']    = 'HLT_RELEASE'
    
    output = ['LPS.L1PS_ID']
    condition = [ "PS2RE.L1RE2PS_RELEASE_ID = '%s'" % relid,
                  "PS2RE.L1RE2PS_PRESCALE_ID = LPS.L1PS_ID",
                  "LPS.L1PS_NAME like '%s'" % name]
    
    l1k  = executeQuery(cursor, output, condition, schemaname, tables)
    l1id = (str(l1k[-1])).lstrip('(').rstrip(')').split(',')[0]

    #Check the PSkey returned matches the SMK
    tables = {}
    tables['LPS']   = 'L1_PRESCALE_SET'
    tables['TM2PS'] = 'L1_TM_TO_PS'
    tables['LTM']  = 'L1_TRIGGER_MENU'
    tables['LMT']    = 'L1_MASTER_TABLE'
    tables['SM']    = 'SUPER_MASTER_TABLE'
    
    output = ['SM.SMT_ID']
    condition = [ "LPS.L1PS_ID = '%s'" % l1id,
                  "TM2PS.L1TM2PS_PRESCALE_SET_ID = '%s'" % l1id,
                  "TM2PS.L1TM2PS_TRIGGER_MENU_ID = LTM.L1TM_ID",
                  "LTM.L1TM_ID = LMT.L1MT_TRIGGER_MENU_ID",
                  "LMT.L1MT_ID = SM.SMT_ID"]

    l1chk  = executeQuery(cursor, output, condition, schemaname, tables)
    smk_chk = (str(l1chk[-1])).lstrip('(').rstrip(')').split(',')[0]
    if smk_chk != smid:
        log.fatal("SMK returned by release, SMK '%s', does not match the one returned by L1PS key, SMK '%s' " %(smid,smk_chk) )

    if not l1only:
        #Find the HLT prescale key with the name and linked to this HLT master
        tables = {}
        tables['HPS']   = 'HLT_PRESCALE_SET'
        tables['PS2RE'] = 'HLT_HRE_TO_PS'
        tables['RE']    = 'HLT_RELEASE'
        
        output = ['HPS.HPS_ID']
        condition = [ "PS2RE.HLTRE2PS_RELEASE_ID = '%s'" % relid,
                      "PS2RE.HLTRE2PS_PRESCALE_ID = HPS.HPS_ID",
                      "HPS.HPS_NAME like '%s'" % name]
        
        hltk  = executeQuery(cursor, output, condition, schemaname, tables)
        hltid = (str(hltk[-1])).lstrip('(').rstrip(')').split(',')[0]
        print 'HLT PS gotten ', hltid

        keys = [int(smid), int(l1id), int(hltid)]

        #Check the PSkey returned matches the SMK
        tables = {}
        tables['HPS']   = 'HLT_PRESCALE_SET'
        tables['TM2PS'] = 'HLT_TM_TO_PS'
        tables['HTM']  = 'HLT_TRIGGER_MENU'
        tables['HMT']    = 'HLT_MASTER_TABLE'
        tables['SM']    = 'SUPER_MASTER_TABLE'
        
        output = ['SM.SMT_ID']
        condition = [ "HPS.HPS_ID = '%s'" % hltid,
                      "TM2PS.HTM2PS_PRESCALE_SET_ID = '%s'" % hltid,
                      "TM2PS.HTM2PS_TRIGGER_MENU_ID = HTM.HTM_ID",
                      "HTM.HTM_ID = HMT.HMT_TRIGGER_MENU_ID",
                      "HMT.HMT_ID = SM.SMT_ID"]
        
        hltchk  = executeQuery(cursor, output, condition, schemaname, tables)
        smk_chk = (str(hltchk[-1])).lstrip('(').rstrip(')').split(',')[0]
        if smk_chk != smid:
            log.fatal("SMK returned by release, SMK '%s', does not match the one returned by L1PS key, SMK '%s' " %(smid,smk_chk) )

            
    else:
        keys = [int(smid), int(l1id)]
        
    return keys

def getChainsAndStreams(connection, smk):
    l2, ef = getChains(connection, smk)
    strm = getStreams(connection, smk)
    return l2, ef, strm


def getChains(connection, smk):
    cursor,schemaname = getTriggerDBCursor(connection)
       
    isrun2 = isRun2(cursor,schemaname)

    output = []
    
    if isrun2:
        output = ['TC.HTC_ID', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_NAME']
    else:
        output = ['TC.HTC_ID', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_NAME', 'TC.HTC_L2_OR_EF']
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['M2C']   = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'
    tables['MT']    = 'HLT_MASTER_TABLE'

    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_HLT_MASTER_TABLE_ID = MT.HMT_ID',
                  'MT.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID' ]
    
    bindvars = { "smk": smk }
    
    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    chainsl2 = {}
    chainsef = {}
    for x in res:
        if isrun2:
            chainsef[x[1]] = x[2]
        else:
            if x[3]=='L2': chainsl2[x[1]] = x[2]
            else: chainsef[x[1]] = x[2]

    return chainsl2, chainsef

def getStreams(connection, smk):
    cursor,schemaname = getTriggerDBCursor(connection)
        
    # now the streams
    output = ['TS.HTR_NAME']
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['M2C']   = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'
    tables['MT']    = 'HLT_MASTER_TABLE'
    tables['TS']    = 'HLT_TRIGGER_STREAM'
    tables['C2S']   = 'HLT_TC_TO_TR'


    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_HLT_MASTER_TABLE_ID = MT.HMT_ID',
                  'MT.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = C2S.HTC2TR_TRIGGER_CHAIN_ID',
                  'C2S.HTC2TR_TRIGGER_STREAM_ID = TS.HTR_ID' ]

    bindvars = { "smk": smk }
    
    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    streams = [x[0] for x in res]


    return streams

def getL1Items(connection, smk):     

    cursor,schemaname = getTriggerDBCursor(connection)

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

    res = executeQuery( cursor, output, condition, schemaname, tables, bindvars)     
    
    items = {}
    for r in res:
        items[r[0]] = r[1]
    return items 

def getBunchGroupContent(connection, bgsk):     

    cursor,schemaname = getTriggerDBCursor(connection)

    output = [ "BGS.L1BGS2BG_INTERNAL_NUMBER","BG.L1BG2B_BUNCH_NUMBER" ]

    tables = { 'BGS' : 'L1_BGS_TO_BG',
               'BG' : 'L1_BG_TO_B'}

    condition = [ "BGS.L1BGS2BG_BUNCH_GROUP_SET_ID = :bgsk",
                 "BGS.L1BGS2BG_BUNCH_GROUP_ID=BG.L1BG2B_BUNCH_GROUP_ID" ]


    bindvars = { "bgsk": bgsk }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars) 

    bg = dict( enumerate( [[] for x in xrange(16)] ) )

    for e in res:
        bg[e[0]] += [e[1]]

    return bg


def getL1Prescales(connection, l1prescalekey): 
    
    cursor,schemaname = getTriggerDBCursor(connection) 
    isrun2 = isRun2(cursor,schemaname)

    maxitems = 512 if isrun2 else 256
    tables = { 'L' : 'L1_PRESCALE_SET' } 

    output = ['L.L1PS_NAME'] + ['L.L1PS_VAL%i' % i for i in xrange(1,maxitems+1)] 

    condition = [ "L.L1PS_ID = '%i'" % l1prescalekey ] 

    res = executeQuery(cursor, output, condition, schemaname, tables) 

    name,prescales = res[0][0], res[0][1:maxitems+1]
    if isrun2: prescales = map(getPrescaleFromCut, prescales)

    return (name,prescales)

def getPrescaleFromCut(cut):
    """Convert (run-2) prescale cuts into prescale value"""
    sign = -1 if cut<0 else 1
    ucut = abs(cut)
    return (sign*0xFFFFFF ) / float( 0x1000000 - ucut );
    
def queryHLTPrescaleTable(connection,psk):
    """returns content of prescale set table and prescale table for a
    given HLT prescale key
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key

    @return (ps name, [('L2/EF/express',chainId,prescale,pass-through),...])
    In case of 'express', the express stream prescale is returned in the position of the pass-through
    """

    cursor,schemaname = getTriggerDBCursor(connection)

    tables = { 'S' : 'HLT_PRESCALE_SET'
               }
    output = [ 'S.HPS_NAME' ]

    condition = [
        "S.HPS_ID = '%i'" % psk
        ]

    res = executeQuery(cursor, output, condition, schemaname, tables) 

    name = res[0][0]

    tables = { 'PS': 'HLT_PRESCALE'
               }
    output = ['PS.HPR_L2_OR_EF', 'PS.HPR_CHAIN_COUNTER', 'PS.HPR_PRESCALE', 'PS.HPR_PASS_THROUGH_RATE']

    condition = [
        "PS.HPR_PRESCALE_SET_ID = '%i'" % psk
        ]

    res = executeQuery(cursor, output, condition, schemaname, tables) 

    return name, res


def getHLTPrescales(connection,psk):
    """returns set name, prescale and passthrough values for a
    given HLT prescale key
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key

    @return (ps name, [('L2/EF',chainId,prescale,pass-through),...])
    """

    name, res = queryHLTPrescaleTable(connection,psk)

    return name, [r for r in res if r[0]!='express']


def getExpressStreamPrescales(connection,psk):
    """returns the express stream prescales for a
    given HLT prescale key
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key

    @return (ps name, [chainId,prescale),...])
    """
    
    name, res = queryHLTPrescaleTable(connection,psk)

    return name, [(r[1],r[3]) for r in res if r[0]=='express']


def getHLTPrescalesRun2(connection,psk,smk):
    """returns set name, prescale and passthrough 
    values for a given HLT prescale key 
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key
    @smk - Supermaster key
    @return (ps name, [('L2/EF',chainId,prescale,pass-through),...])
    """

    res = queryHLTPrescaleTableRun2(connection,psk,smk)

    if res == 0:
        return res

    return [(r) for r in res if r[3]!='express']

def getExpressStreamPrescalesRun2(connection,psk,smk):
    """returns the express stream prescales for a given HLT prescale key
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key
    @smk - Supermaster key
    @return (ps name, [chainId,prescale),...])
    """

    res = queryHLTPrescaleTableRun2(connection,psk, smk)

    if res == 0:
        return res
    
    return [(r) for r in res if r[3]=='express']



def getReRunPrescalesRun2(connection,psk,smk):
    """returns the express stream prescales for a given HLT prescale key
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key
    @smk - Supermaster key
    @return (ps name, [chainId,prescale),...])
    """

    res = queryHLTPrescaleTableRun2(connection,psk, smk)

    if res == 0:
        return res

    return [(r) for r in res if r[2]=='ReRun']



def queryHLTPrescaleTableRun2(connection,psk,smk):

    prescales = getHLTPrescalesFromSMK(connection, smk)

    valid = False
    for entry in prescales:
        if psk in entry:
            valid = True

    if not valid:
        print "WARNING: Selected HLT Prescale Key not associated with Supermaster key"
        return 0

    cursor,schemaname = getTriggerDBCursor(connection)

    output = [ "HTC.HTC_NAME", "PS.HPR_CHAIN_COUNTER", "PS.HPR_TYPE", "PS.HPR_CONDITION" , "PS.HPR_VALUE"]

    tables = {}
    tables['PS'] = 'HLT_PRESCALE' 
    tables['HTC'] = 'HLT_TRIGGER_CHAIN'
    tables['TM2TC'] = 'HLT_TM_TO_TC'
    tables['HTM']  = 'HLT_TRIGGER_MENU'
    tables['HMT']    = 'HLT_MASTER_TABLE'
    tables['SM']    = 'SUPER_MASTER_TABLE'

    condition = [ "PS.HPR_PRESCALE_SET_ID = :psk",
                  "HTC.HTC_CHAIN_COUNTER = PS.HPR_CHAIN_COUNTER",
                  "TM2TC.HTM2TC_TRIGGER_CHAIN_ID = HTC.HTC_ID",
                  "TM2TC.HTM2TC_TRIGGER_MENU_ID = HTM.HTM_ID",
                  "HTM.HTM_ID = HMT.HMT_TRIGGER_MENU_ID",
                  "HMT.HMT_ID = SM.SMT_HLT_MASTER_TABLE_ID",
                  "SM.SMT_ID = %s" % smk 
                  ]

    bindvars = { "psk": psk }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)

    return res

def getHLTPrescalesFromSMK(connection, smk):

    cursor,schemaname = getTriggerDBCursor(connection)

    tables = {}
    tables['HPS']   = 'HLT_PRESCALE_SET'
    tables['TM2PS'] = 'HLT_TM_TO_PS'
    tables['HTM']  = 'HLT_TRIGGER_MENU'
    tables['HMT']    = 'HLT_MASTER_TABLE'
    tables['SM']    = 'SUPER_MASTER_TABLE'

    output = ['TM2PS.HTM2PS_PRESCALE_SET_ID']
    condition = [ "TM2PS.HTM2PS_TRIGGER_MENU_ID = HTM.HTM_ID",
                  "HTM.HTM_ID = HMT.HMT_TRIGGER_MENU_ID",
                  "HMT.HMT_ID = SM.SMT_HLT_MASTER_TABLE_ID",
                  "SM.SMT_ID = %s" % smk ]

    hltpsk  = executeQuery(cursor, output, condition, schemaname, tables)

    return hltpsk


def test():
    log.setLevel(logging.DEBUG)

    ### oracle
    print """####################################
##
##   Testing ORACLE
##
####################################"""
    l2, ef = getChains("TRIGGERDBREPR", 539)
    strm_oracle = getStreams("TRIGGERDBREPR", 539)
    print "\nList of Streams in SMK 539",strm_oracle,"\n"

    ### frontier
    print """####################################
##
##   Testing FRONTIER
##
####################################"""
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerUseFrontier = True
    strm_frontier = getStreams("TRIGGERDBREPR", 539)
    print "\nList of Streams in SMK 539",strm_frontier,"\n"

    if strm_oracle == strm_frontier:
        print """####################################
##
##   ORACLE and FRONTIER give IDENTICAL results
##
####################################"""
    else:
        print """####################################
##
##   ERROR: ORACLE and FRONTIER give DIFFERENT results
##
####################################"""
    



def test2():
    log.setLevel(logging.WARNING)

    connections = [ "TRIGGERDB",
                    "TRIGGERDB_RUN1",
                    "TRIGGERDBV1",
                    "TRIGGERDBMC_RUN1",
                    "TRIGGERDBMC",
                    "TRIGGERDBDEV1",
                    "TRIGGERDBDEV2",
                    "TRIGGERDBATN",
                    "TRIGGERDBREPR",
                    "TRIGGERDB_JOERG"
                    ]


    for c in connections:
        print "\nConnecting to alias ",c
        cursor, schema = getTriggerDBCursor(c)
        if cursor:
            print "SUCCESS :  %s" % schema
        else:
            print "FAILURE"



if __name__=="__main__":
    import sys
    sys.exit(test2())
