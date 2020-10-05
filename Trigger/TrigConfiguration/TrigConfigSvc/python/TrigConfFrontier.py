#!/usr/bin/env python

from __future__ import print_function

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from future import standard_library
standard_library.install_aliases()
from builtins import map
from builtins import str
from builtins import zip
from builtins import object
from AthenaCommon.Logging import logging
import time
import sys

def getFrontierCursor(url, schema, loglevel = logging.INFO):
    log = logging.getLogger( "TrigConfFrontier.py" )
    log.setLevel(loglevel)
    try:
        from TrigConfDBConnection import frontier_client  # noqa: F401
        return FrontierCursor2( url = url, schema = schema)
    except Exception:
        log.warning("Couldn't import frontier_client from TrigConfDBConnection, falling back to pure python implementation without proper url resolution")
        return FrontierCursor( url = url, schema = schema)
        

class FrontierCursor2(object):
    def __init__(self, url, schema, refreshFlag=False):
        log = logging.getLogger( "TrigConfFrontier.py" )
        self.url = url
        self.schema = schema
        self.refreshFlag = refreshFlag
        from TrigConfDBConnection import frontier_client as fc
        fc.init("PyFrontier","debug")
        log.debug("Frontier URL      : %s", self.url)
        log.debug("Schema            : %s", self.schema)
        log.debug("Refresh cache     : %s", self.refreshFlag)

    @classmethod
    def resolvebindvars(cls, query, bindvars):
        """Replaces the bound variables :xyz with a ? in the query and
        adding the value behind a : at the end"""
        log = logging.getLogger( "TrigConfFrontier.py" )
        import re
        varsextract = re.findall(':([A-z0-9]*)',query)
        values = list(map(bindvars.get, varsextract))
        log.debug("Resolving bound variable %r with %r", varsextract,values)
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
        from builtins import int
        for var,val in list(bindvars.items()):
            if query.find(":%s" % var)<0:
                raise NameError("variable '%s' is not a bound variable in this query: %s" % (var, query) )
            if isinstance (val, int):
                query = query.replace(":%s" % var,"%s" % val)
            else:
                query = query.replace(":%s" % var,"%r" % val)
            log.debug("Resolving bound variable '%s' with %r", var,val)
        log.debug("Resolved query: %s", query)
        return query

    def execute(self, query, bindvars={}):
        if len(bindvars)>0:
            query = FrontierCursor2.resolvebindvars(query,bindvars)
            
        from TrigConfDBConnection import frontier_client as fc
        log = logging.getLogger( "TrigConfFrontier.py" )
        log.debug("Executing query : %s", query)

        conn = fc.Connection(self.url)
        session = fc.Session(conn)

        doReload = self.refreshFlag
        conn.setReload(doReload)
        
        queryStart = time.localtime()
        log.debug("Query started: %s", time.strftime("%m/%d/%y %H:%M:%S %Z", queryStart))

        t1 = time.time()
        req = fc.Request("frontier_request:1:DEFAULT", fc.encoding_t.BLOB)
        param = fc.Request.encodeParam(query)
        req.addKey("p1",param)
        
        session.getData([req])
        t2 = time.time()

        #session.printHeader()
        
        #nfield = session.getNumberOfFields()
        #print ("\nNumber of fields:", nfield, "\n")
    
        #nrec = session.getNumberOfRecords()
        #print ("\nResult contains", nrec, "objects.\n")
        
        #session.printRecords2()
        queryEnd = time.localtime()
        
        self.result = [r for r in session.getRecords2()]
        log.debug("Query ended: %s", time.strftime("%m/%d/%y %H:%M:%S %Z", queryEnd))
        log.debug("Query time: %s seconds", (t2-t1))
        log.debug("Result size: %i entries", len(self.result))
            
    def fetchall(self):
        return self.result

    def __str__(self):
        return """FrontierCursor2:
Using Frontier URL: %s
Schema: %s
Refresh cache:  %s""" % (self.url, self.schema, self.refreshFlag)



    
class FrontierCursor(object):
    def __init__(self, url, schema, refreshFlag=False, doDecode=True, retrieveZiplevel="zip"):
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
        return findall(r'\((serverurl)=(.*?)\)',frontier_servers)

    @classmethod
    def testUrl(cls, url):
        import urllib.request, urllib.error, urllib.parse
        try:
            urllib.request.urlopen(url)
        except urllib.error.URLError:
            import traceback
            traceback.print_exc()
            
    def execute(self, query, bindvars={}):
        if len(bindvars)>0:
            query = FrontierCursor2.replacebindvars(query,bindvars)
        
        log = logging.getLogger( "TrigConfFrontier.py" )
        log.debug("Using Frontier URL: %s", self.url)
        log.debug("Refresh cache     : %s", self.refreshFlag)
        log.debug("Query             : %s", query)
        
        import base64, zlib, urllib.request, urllib.error, urllib.parse, time

        self.result = None

        compQuery = zlib.compress(query.encode("utf-8"),9)
        base64Query = base64.binascii.b2a_base64(compQuery).decode("utf-8")
        encQuery = base64Query.replace("+", ".").replace("\n","").replace("/","-").replace("=","_")

        frontierRequest="%s/type=frontier_request:1:DEFAULT&encoding=BLOB%s&p1=%s" % (self.url, self.retrieveZiplevel, encQuery)
        request = urllib.request.Request(frontierRequest)

        if self.refreshFlag:
            request.add_header("pragma", "no-cache")

        frontierId = "TrigConfFrontier 1.0"
        request.add_header("X-Frontier-Id", frontierId)

        queryStart = time.localtime()
        log.debug("Query started: %s", time.strftime("%m/%d/%y %H:%M:%S %Z", queryStart))

        t1 = time.time()
        result = urllib.request.urlopen(request,None,10).read().decode()
        t2 = time.time()

        queryEnd = time.localtime()
        log.debug("Query ended: %s", time.strftime("%m/%d/%y %H:%M:%S %Z", queryEnd))
        log.debug("Query time: %s [seconds]", (t2-t1))
        log.debug("Result size: %i [seconds]", len(result))
        self.result = result

    def fetchall(self):
        if self.doDecode: self.decodeResult()
        return self.result


    def decodeResult(self):
        log = logging.getLogger( "TrigConfFrontier.py" )
        from xml.dom.minidom import parseString
        import base64, zlib, curses.ascii
        #print ("Query result:\n", self.result)
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
                    print (keepalives, "keepalives received\n")
                    keepalives = 0
            
                row = base64.decodebytes(node.data.encode())
                if self.retrieveZiplevel != "":
                    row = zlib.decompress(row).decode("utf-8")

                #Hack to get these lines to work in python 2
                if sys.version_info[0] < 3: 
                    row = row.encode('ascii', 'xmlcharrefreplace')
             
                endFirstRow = row.find('\x07')
                firstRow = row[:endFirstRow]
                for c in firstRow:
                    if curses.ascii.isctrl(c):
                        firstRow = firstRow.replace(c, ' ')
                fields = [x for i,x in enumerate(firstRow.split()) if i%2==0]
                types = [x for i,x in enumerate(firstRow.split()) if i%2==1]
                ptypes = []
                for t in types:
                    if t.startswith("NUMBER"):
                        if ",0" in t:
                            ptypes.append(int)
                        else:
                            ptypes.append(float)
                    else:
                        ptypes.append(str)


                log.debug("Fields      : %r", fields)
                log.debug("DB Types    : %r", types)
                log.debug("Python Types: %r", ptypes)
                
                row = row[endFirstRow+1:]

                row_h = row.rstrip('\x07')
                
                import re
                row_h = row_h.replace("\x07\x06",'.nNn.\x06')

#                pattern = re.compile("\x06\x00\x00\x00.",flags=re.S)
#replace pattern above  more restrictive version, as longerstrings in the results
#have a size variable in the column separate that becomes visible if the string
#is large enough - this then broke the prevous  decoding
                pattern = re.compile("\x06\x00\x00..",flags=re.S)
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
    log = logging.getLogger( "TrigConfFrontier.py::testConnection()" )
    log.setLevel(logging.DEBUG)

    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerUseFrontier = True

    from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection
    connectionParameters = interpretConnection("TRIGGERDBMC")

    cursor = FrontierCursor( url = connectionParameters['url'], schema = connectionParameters['schema'])

    query = "select distinct HPS.HPS_NAME from ATLAS_CONF_TRIGGER_RUN2_MC.HLT_PRESCALE_SET HPS where HPS.HPS_ID = '260'"

    cursor.execute(query)
    print(cursor.result)
    cursor.decodeResult()
    print(cursor.result[0][0])
    assert cursor.result[0][0] == 'MC_pp_v7'

    return 0


def testBindVarResolution():
    query = "SELECT :bar WHERE :foo = :bar sort by :ups asc, :foo"
    bindvars = {"foo": 500, "bar": 8, "ups": 42 }
    print("Query")
    print(query)
    print("is translated to")
    print(FrontierCursor2.resolvebindvars(query, bindvars))
    return 0

    
if __name__=="__main__":
    res = testBindVarResolution()
    res = max(res, testConnection())
    sys.exit(res)
