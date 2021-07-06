#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import time
import sys


def getServerUrls(frontier_servers):
    """
    turns
    '(serverurl=http://atlasfrontier-local.cern.ch:8000/atlr)(serverurl=http://atlasfrontier-ai.cern.ch:8000/atlr)'
    into
    ['http://atlasfrontier-local.cern.ch:8000/atlr','http://atlasfrontier-ai.cern.ch:8000/atlr']
    """
    from re import findall
    return findall(r'\(serverurl=(.*?)\)',frontier_servers)


def testUrl(url):
    import urllib.request, urllib.error, urllib.parse
    try:
        urllib.request.urlopen(url)
    except urllib.error.URLError:
        return False
    return True

def resolveUrl(url):
    """
    Expects input string to be a URL or $FRONTIER_SERVER
    Returns an accessible URL or None"""
    import re
    if re.match("http://",url): # simple URL specification http://...
        return url if testUrl(url) else None

    if re.match(r'\(serverurl=(.*?)\)',url): # syntax of FRONTIER_SERVER
        for url in getServerUrls(url):
            if testUrl(url):
                return url     
        return None


def getFrontierCursor(url, schema, loglevel = logging.INFO):
    log = logging.getLogger( "TrigConfFrontier.py" )
    log.setLevel(loglevel)
    url = resolveUrl(url)
    if url is None:
        log.warning("Cannot find a valid frontier connection, will not return a Frontier cursor")
        return None
    else:
        log.info(f"Will use Frontier server at {url}")

    return FrontierCursor( url = url, schema = schema)
        

# used by FrontierCursor
def replacebindvars(query, bindvars):
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


class FrontierCursor(object):
    def __init__(self, url, schema, refreshFlag=False, doDecode=True, retrieveZiplevel="zip"):
        self.url = url + "/Frontier"
        self.schema = schema
        self.refreshFlag = refreshFlag
        self.retrieveZiplevel = retrieveZiplevel
        self.doDecode = doDecode

    def __str__(self):
        s = "Using Frontier URL: %s\n" % self.url
        s += "Schema: %s\n" % self.schema
        s += "Refresh cache:  %s" % self.refreshFlag
        return s

    def execute(self, query, bindvars={}):
        if len(bindvars)>0:
            query = replacebindvars(query,bindvars)
        
        log = logging.getLogger( "TrigConfFrontier.py" )
        log.debug("Frontier URL  : %s", self.url)
        log.debug("Refresh cache : %s", self.refreshFlag)
        log.debug("Query         : %s", query)
        
        import base64, zlib, urllib.request, urllib.error, urllib.parse

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




def testQuery(query, bindvars):
    log = logging.getLogger( "TrigConfFrontier.py" )
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerUseFrontier = True

    from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection
    connectionParameters = interpretConnection("TRIGGERDBMC")
    cursor = getFrontierCursor( url = connectionParameters['url'], schema = connectionParameters['schema'])
    cursor.execute(query, bindvars)
    log.info("Raw response:")
    print(cursor.result)
    cursor.decodeResult()
    log.info("Decoded response:")
    log.info(cursor.result[0][0])        
    if cursor.result[0][0] != 'MC_pp_v7':
        return 1
    return 0

    
if __name__=="__main__":
    log = logging.getLogger( "TrigConfFrontier.py" )
    log.setLevel(logging.DEBUG)

    dbalias = "TRIGGERDBMC"
    query = "select distinct HPS.HPS_NAME from ATLAS_CONF_TRIGGER_RUN2_MC.HLT_PRESCALE_SET HPS where HPS.HPS_ID = :psk"
    bindvars = { "psk": 260 }

    res = testQuery(query, bindvars)  # pure python frontier query
    sys.exit(res)
