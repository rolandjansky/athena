#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @author Marcin Nowak
# @date 09.2012
# @brief utility to extract TAG DB Collection to a local ROOT file
#

import commands, time, tempfile, os, sys, re
from optparse import OptionParser
from eventLookupClient import eventLookupClient

parser = OptionParser()
parser.add_option("--server", dest="server", default='atlas-tagservices.cern.ch',
                  help="TAG services WWW server (note: may need full domain name)")
parser.add_option("-d", "--dataset", dest="dataset", 
                  help="dataset to extract from DB")
parser.add_option("-o", "--output", dest="output", default='/tmp/TAGs.root',
                  help="file name for the output list of guids")
parser.add_option("-t", "--tokens", dest="tokens", default='',
                  help="list of GUID references ")
parser.add_option("-a", "--attributes", dest="attributes", default='RunNumber,EventNumber',
                  help="list of TAG attributes. '*' selects all ")
parser.add_option("-q", "--query", dest="query", default='',
                  help="query for TAG selection")
parser.add_option("-i", "--taskid", dest="taskID", default=None,
                  help="taskID to group reqests")


(options, args) = parser.parse_args()
output = open(options.output, "w")

starttime = time.time()
#extract collection from the database
EL = eventLookupClient()
print "# Extracting TAG collection into %s" % options.output
formatparams = dict( server = options.server,
                     cert = EL.certProxyFileName,
                     url = 'tagservices/EventLookup/www/extract',
                     dataset = options.dataset,
                     query = options.query,
                     attribs = options.attributes,
                     tokens = options.tokens,
                     outfile = options.output )
cmd = EL.getCurlCmd() + " -F 'certificate=<{cert}' 'https://{server}/{url}?dataset={dataset}' --form-string 'query={query}' --form-string 'attribs={attribs}' --form-string 'tokens={tokens}' -o {outfile}".format( **formatparams )
if options.taskID:
    cmd += ' --form-string taskID=%d' % int(options.taskID)
    
didRetry = 0
while True:
    restarttime = time.time()
    (rc,out) = commands.getstatusoutput(cmd)
    if rc != 0:
        print "ERROR!" 
        code = EL.checkError(out)
        if code:
            sys.exit(code)
        print out
        sys.exit(-1)

    cmd2 = 'file ' + options.output
    (rc,out) = commands.getstatusoutput(cmd2)
    if re.search('empty', out):
        print "ERROR: Extraction process did not return any data"
        sys.exit(1)
    if re.search('ASCII', out) or re.search('text', out):
        CF = open(options.output, "r")
        found_retry = False
        for line in CF:
            if line.find('retry') > 0:
                found_retry = True
                break
        CF.close()
        if found_retry:
            print line
            didRetry += 1
            time.sleep(5)
            continue
        print "Extraction process probably failed with message:"
        with open(options.output, 'r') as CF:
            print CF.read()
        sys.exit(2)
    # finished OK!
    break

print "# %s extracted in %.0ds (may include server queue time)" %(options.output, time.time()-restarttime)
if didRetry > 0:
    print "# reqest retried %d time (server busy). Total time %d" % (didRetry, time.time()-starttime)




