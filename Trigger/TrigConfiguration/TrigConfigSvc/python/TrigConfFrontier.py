#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
import time

def getFrontierCursor(url, schema, loglevel = logging.INFO):
    log = logging.getLogger( "TrigConfFrontier.py" )
    log.setLevel(loglevel)
    try:
        from TrigConfDBConnection import frontier_client
        return FrontierCursor2( url = url, schema = schema)
    except:
        log.warning("Couldn't import frontier_client from TrigConfDBConnection, falling back to pure python implementation without proper url resolution")
        return FrontierCursor( url = url, schema = schema)
        

class FrontierCursor2:
    def __init__(self, url, schema, refreshFlag=False):
        log = logging.getLogger( "TrigConfFrontier.py" )
        self.url = url
        self.schema = schema
        self.refreshFlag = refreshFlag
        from TrigConfDBConnection import frontier_client as fc
        fc.init("PyFrontier","debug")
        log.debug("Frontier URL      : %s" % self.url)
        log.debug("Schema            : %s" % self.schema)
        log.debug("Refresh cache     : %s" % self.refreshFlag)

    @classmethod
    def resolvebindvars(cls, query, bindvars):
        """Replaces the bound variables :xyz with a ? in the query and
        adding the value behind a : at the end"""
        log = logging.getLogger( "TrigConfFrontier.py" )
        import re
        varsextract = re.findall(':([A-z0-9]*)',query)
        values = map(bindvars.get, varsextract)
        log.debug("Resolving bound variable %r with %r" % (varsextract,values))
        appendix = ":".join([str(v) for v in values])
        queryWithQuestionMarks = re.sub(':[A-z0-9]*','?', query)
        query = queryWithQuestionMarks + ':' + appendix
        return query

    @classmethod
    def replacebindvars(cls, query, bindvars):
        """Replaces the bound variables with the specified values,
        disables variable binding
        """
        log = logging.getLogger( "TrigConfFrontier.py" )
        for var,val in bindvars.items():
            if query.find(":%s" % var)<0:
                raise NameError("variable '%s' is not a bound variable in this query: %s" % (var, query) )
            if type(val) == long:
                query = query.replace(":%s" % var,"%s" % val)
            else:
                query = query.replace(":%s" % var,"%r" % val)
            log.debug("Resolving bound variable '%s' with %r" % (var,val))
        log.debug("Resolved query: %s" % query)
        return query

    def execute(self, query, bindvars={}):
        if len(bindvars)>0:
            query = FrontierCursor2.resolvebindvars(query,bindvars)
            
        from TrigConfDBConnection import frontier_client as fc
        log = logging.getLogger( "TrigConfFrontier.py" )
        log.debug("Executing query : %s" % query)

        conn = fc.Connection(self.url)
        session = fc.Session(conn)

        doReload = self.refreshFlag
        conn.setReload(doReload)
        
        queryStart = time.localtime()
        log.debug("Query started: %s" % time.strftime("%m/%d/%y %H:%M:%S %Z", queryStart))

        t1 = time.time()
        req = fc.Request("frontier_request:1:DEFAULT", fc.encoding_t.BLOB)
        param = fc.Request.encodeParam(query)
        req.addKey("p1",param)
        
        session.getData([req])
        t2 = time.time()

        #session.printHeader()
        
        #nfield = session.getNumberOfFields()
        #print "\nNumber of fields:", nfield, "\n"
    
        #nrec = session.getNumberOfRecords()
        #print "\nResult contains", nrec, "objects.\n"
        
        #session.printRecords2()
        queryEnd = time.localtime()
        
        self.result = [r for r in session.getRecords2()]
        log.debug("Query ended: %s" % time.strftime("%m/%d/%y %H:%M:%S %Z", queryEnd))
        log.debug("Query time: %s seconds" % (t2-t1))
        log.debug("Result size: %i entries" % len(self.result))
            
    def fetchall(self):
        return self.result

    def __str__(self):
        return """FrontierCursor2:
Using Frontier URL: %s
Schema: %s
Refresh cache:  %s""" % (self.url, self.schema, self.refreshFlag)



    
class FrontierCursor:
    def __init__(self, url, schema, refreshFlag=False, doDecode=True, retrieveZiplevel="zip"):
        log = logging.getLogger( "TrigConfFrontier.py" )
        if url.startswith('('):
            self.servertype, self.url  = FrontierCursor.getServerUrls(url)[0]
            self.url += "/Frontier"
        else:
            self.url = url
        self.schema = schema
        self.refreshFlag = refreshFlag
        self.retrieveZiplevel = retrieveZiplevel
        self.doDecode = doDecode
        # connstr='frontier://ATLF/%s;schema=%s;dbname=TRIGCONF' % (connectionParameters['url'],connectionParameters["schema"])

    def __str__(self):
        return """FrontierCursor:
Using Frontier URL: %s
Refresh cache:  %s""" % (self.url, self.refreshFlag)

    @classmethod
    def getServerUrls(cls, frontier_servers):
        from re import findall
        return findall('\((serverurl)=(.*?)\)',frontier_servers)

    @classmethod
    def testUrl(cls, url):
        import urllib2
        try:
            urllib2.urlopen(url)
        except urllib2.URLError, e:
            print e
            
    def execute(self, query, bindvars={}):
        if len(bindvars)>0:
            query = FrontierCursor2.replacebindvars(query,bindvars)
        
        log = logging.getLogger( "TrigConfFrontier.py" )
        log.debug("Using Frontier URL: %s" % self.url)
        log.debug("Refresh cache     : %s" % self.refreshFlag)
        log.debug("Query             : %s" % query)
        
        import base64, zlib, urllib2, time

        self.result = None

        encQuery = base64.binascii.b2a_base64(zlib.compress(query,9)).replace("+", ".").replace("\n","").replace("/","-").replace("=","_")

        frontierRequest="%s/type=frontier_request:1:DEFAULT&encoding=BLOB%s&p1=%s" % (self.url, self.retrieveZiplevel, encQuery)
        request = urllib2.Request(frontierRequest)

        if self.refreshFlag:
            request.add_header("pragma", "no-cache")

        frontierId = "TrigConfFrontier 1.0"
        request.add_header("X-Frontier-Id", frontierId)

        queryStart = time.localtime()
        log.debug("Query started: %s" % time.strftime("%m/%d/%y %H:%M:%S %Z", queryStart))

        t1 = time.time()
        result = urllib2.urlopen(request,None,10).read()
        t2 = time.time()

        queryEnd = time.localtime()
        log.debug("Query ended: %s" % time.strftime("%m/%d/%y %H:%M:%S %Z", queryEnd))
        log.debug("Query time: %s [seconds]" % (t2-t1))
        log.debug("Result size: %i [seconds]" % len(result))
        self.result = result

    def fetchall(self):
        if self.doDecode: self.decodeResult()
        return self.result


    def decodeResult(self):
        log = logging.getLogger( "TrigConfFrontier.py" )
        from xml.dom.minidom import parseString
        import base64,zlib, curses.ascii
        #print "Query result:\n", self.result
        dom = parseString(self.result)
        dataList = dom.getElementsByTagName("data")
        keepalives = 0
        # Control characters represent records, but I won't bother with that now,
        # and will simply replace those by space.
        for data in dataList:
            for node in data.childNodes:
                # <keepalive /> elements may be present, combined with whitespace text
                if node.nodeName == "keepalive":
                    # this is of type Element
                    keepalives += 1
                    continue
                # else assume of type Text
                if node.data.strip() == "":
                    continue
                if keepalives > 0:
                    print keepalives, "keepalives received\n"
                    keepalives = 0
            
                row = base64.decodestring(node.data)
                if self.retrieveZiplevel != "":
                    row = zlib.decompress(row)
            
                endFirstRow = row.find('\x07')
                firstRow = row[:endFirstRow]
                for c in firstRow:
                    if curses.ascii.isctrl(c):
                        firstRow = firstRow.replace(c, ' ')
                fields = [x for i,x in enumerate(firstRow.split()) if i%2==0]
                types = [x for i,x in enumerate(firstRow.split()) if i%2==1]
                Nfields = len(fields)
                ptypes = []
                for t in types:
                    if t.startswith("NUMBER"):
                        if ",0" in t:
                            ptypes.append(int)
                        else:
                            ptypes.append(float)
                    else:
                        ptypes.append(str)


                log.debug("Fields      : %r" % fields)
                log.debug("DB Types    : %r" % types)
                log.debug("Python Types: %r" % ptypes)
                
                row = str(row[endFirstRow+1:])

                row_h = row.rstrip('\x07')

                import re
                row_h = row_h.replace("\x07\x06",'.nNn.\x06')

                pattern = re.compile("\x06\x00\x00\x00.",flags=re.S)
                row_h = pattern.sub('.xXx.',row_h)
                row_h = row_h.replace("\x86", '.xXx.')

                row_h = row_h.split('.nNn.')
                row_h = [r.split('.xXx.') for r in row_h]

                result = []
                for r in row_h:
                    if r[0]=='': r[0:1]=[]
                    r = tuple([t(v) for t,v in zip(ptypes,r)])
                    result.append( r )

        self.result = result


def testConnection():
    import os
    log = logging.getLogger( "TrigConfFrontier.py::testConnection()" )
    log.setLevel(logging.DEBUG)

    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerUseFrontier = True

    from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection
    connectionParameters = interpretConnection("TRIGGERDBREPR")

    cursor = FrontierCursor2( url = connectionParameters['url'], schema = connectionParameters['schema'])

    query = "select distinct SM.SMT_ID, SM.SMT_NAME, SM.SMT_VERSION, SM.SMT_COMMENT, SM.SMT_ORIGIN, SM.SMT_USERNAME, SM.SMT_STATUS from ATLAS_CONF_TRIGGER_REPR.SUPER_MASTER_TABLE SM order by SM.SMT_ID"

    cursor.execute(query)

    for r in cursor.result[:20]:
        print r

    query = """
SELECT DISTINCT
CP.HCP_NAME,
CP.HCP_ALIAS,
TE2CP.HTE2CP_ALGORITHM_COUNTER,
TE.HTE_ID,
TE.HTE_NAME,
TE2TE.HTE2TE_TE_INP_ID,
TE2TE.HTE2TE_TE_INP_TYPE,
TE2TE.HTE2TE_TE_COUNTER
FROM
ATLAS_CONF_TRIGGER_REPR.SUPER_MASTER_TABLE    SM,
ATLAS_CONF_TRIGGER_REPR.HLT_MASTER_TABLE      HM,
ATLAS_CONF_TRIGGER_REPR.HLT_TM_TO_TC          M2C,
ATLAS_CONF_TRIGGER_REPR.HLT_TC_TO_TS          TC2TS,
ATLAS_CONF_TRIGGER_REPR.HLT_TS_TO_TE          S2TE,
ATLAS_CONF_TRIGGER_REPR.HLT_TRIGGER_ELEMENT   TE,
ATLAS_CONF_TRIGGER_REPR.HLT_TE_TO_CP          TE2CP,
ATLAS_CONF_TRIGGER_REPR.HLT_TE_TO_TE          TE2TE,
ATLAS_CONF_TRIGGER_REPR.HLT_COMPONENT         CP
WHERE
SM.SMT_ID     = 539
AND HM.HMT_ID = SM.SMT_HLT_MASTER_TABLE_ID
AND HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID
AND M2C.HTM2TC_TRIGGER_CHAIN_ID = TC2TS.HTC2TS_TRIGGER_CHAIN_ID
AND TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID
AND TE.HTE_ID = S2TE.HTS2TE_TRIGGER_ELEMENT_ID
AND TE.HTE_ID = TE2CP.HTE2CP_TRIGGER_ELEMENT_ID
AND TE.HTE_ID = TE2TE.HTE2TE_TE_ID
AND CP.HCP_ID = TE2CP.HTE2CP_COMPONENT_ID
ORDER BY
TE.HTE_ID ASC,
TE2CP.HTE2CP_ALGORITHM_COUNTER DESC"""

    cursor.execute(query)

    for r in cursor.result[:20]:
        print r

    return 0


def testBindVarResolution():
    query = "SELECT :bar WHERE :foo = :bar sort by :ups asc, :foo"
    bindvars = {"foo": 500, "bar": 8, "ups": 42 }
    print "Query"
    print query
    print "is translated to"
    print FrontierCursor2.resolvebindvars(query, bindvars)

    
if __name__=="__main__":
    import sys
    res = testBindVarResolution()
    res = max(res, testConnection())
    sys.exit(res)
