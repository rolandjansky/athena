#!/bin/env python2.5
#$Name: not supported by cvs2svn $

""" Publisher example """
import sys, re, time, datetime, os
from random import choice

def index(q=''):

    if q.startswith('arq_'):
        (page, fullpath) = _cache_request(q)
    else:
        (page, fullpath) = _new_request(q)

    _touch(fullpath)

    return page

def _new_request(q=''):
    installpath = os.path.dirname(__file__)

    timeofrequest = time.gmtime()

    # data stream uses current time 
    queryday = time.strftime("%y%m%d",timeofrequest)
    queryid = time.strftime("%y%m%d%H%M%S",timeofrequest)

    # add random string to avoid conflict from coincident query
    queryid += "".join([choice('abcdefghijklmnopqrstyz') for x in xrange(4)])

    # the query datapath
    datapath = 'data/arq_%s/arq_%s' % (queryday,queryid)
    
    # where to store the result
    #fulldatapath = '%s/%s' % (installpath,datapath)
    fulldatapath = '/%s' % datapath
    os.makedirs(fulldatapath)

    # the query that should be run
    queryfile = '%s/query.txt' % fulldatapath
    fh = open(queryfile,"w")
    print >> fh, "%s" % q
    fh.close()

    # global log file
    logpath = fulldatapath
    logfile = '%s/log.txt' % logpath
    fh = open(logfile,"a")
    print >> fh, "%s / [id %s] - received query: %s" % (timeofrequest, queryid, q if q else "none" )
    fh.close()
    
    com = "cd %s; ./CoolRunQueryWrapper.sh fileindex %s" % (installpath,queryid)

    # run the query
    from commands import getoutput
    log = getoutput(com)
    logfile = '%s/log.txt' % fulldatapath
    fh = open(logfile,"w")
    print >> fh, log
    fh.close()


    # forward file
    outputfile = '%s/index.html' % fulldatapath
    try:
        fh = open(outputfile,"r")
        page = fh.read()
        fh.close()
        if not page.rstrip().endswith("</html>"):
            page = _error_page(datapath)
    except IOError:
        page = "<html><body>No web page created! Here the log file:<pre><br><br><br>%s</pre></body></html>" % (log.replace("<","&lt;").replace(">","&gt;")) 

    return (page,fulldatapath)



def _error_page(datapath):
    
    s = """<html>
    <head><title>Error</title></head>
    <body>
    Found incomplete web page! Would you like to see the
    <a target="_blank" href="query.py?q=%s">web page fragment</a> or the
    <a target="_blank" href="%s/log.txt">log file</a> ?
    </body>
    </html>
    """ % (datapath.split('/')[-1], datapath)
    return s



def _cache_request(q):
    installpath = os.path.dirname(__file__)
    fulldatapath = '%s/data/%s/%s' % (installpath,q[:10],q)
    try:
        # open cache
        fh = open('%s/index.html' % fulldatapath,"r")
        page = fh.read()
        fh.close()
        return (page,fulldatapath)
    except IOError:
        return ("Could not find cache %s" % q, None)

def _touch(fullpath):
    if fullpath==None: return
    installpath = os.path.dirname(__file__)
    try:
        # open cache
        fh = open('%s/access.log' % fullpath,"a")
        timeofaccess = time.gmtime()
        querytime = time.strftime("%y%m%d",timeofaccess)
        print >> fh, querytime
        fh.close()
    except IOError:
        pass
