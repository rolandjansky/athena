#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @author Marcin Nowak
# @date 09.2012
# @brief utility to list datasets for a given Run that are in the TAG DB
#

import commands, time, tempfile, os, sys, re
from optparse import OptionParser
from xml.dom import minidom
from eventLookupClient import eventLookupClient


parser = OptionParser()
parser.add_option("--server", dest="server", default='atlas-tagservices.cern.ch',
                  help="TAG services WWW server (note: may need full domain name)")
parser.add_option("-r", "--runnumber", dest="run", 
                  help="run number for the dataset listing")
parser.add_option("-a", "--amitag", dest="amitag", default='',
                  help="dataset amitag (accepts wildcards). if not apecified, the latest dataset is returned")
parser.add_option("--type", dest="tagtype", default='ANY',
                  help="TAG type: TAG,TAG_MC,TAG_COMM  default=ANY")


(options, args) = parser.parse_args()

starttime = time.time()
EL = eventLookupClient()
formatparams = dict( server = options.server,
                     url = 'tagservices/TASK/catalog_lookup.php',
                     run = options.run,
                     type = options.tagtype,
                     amitag = options.amitag
                     )
cmd = EL.getCurlCmd() + " 'https://{server}/{url}?runnr={run}&tagtype={type}&amitag={amitag}' ".format( **formatparams )

(rc,out) = commands.getstatusoutput(cmd)
if rc != 0:
    print "ERROR!" 
    code = EL.checkError(out)
    if code:
        sys.exit(code)
    print out
    sys.exit(-1)

try:
    xmldoc = minidom.parseString( out )
    trlist = xmldoc.getElementsByTagName('tr')
    table = xmldoc.childNodes[0]
    for x in range (0, trlist.length):  # start with 0 here because there is no header!
        row = table.childNodes[x]
        coll = row.childNodes[1].firstChild.data.encode().replace('_READ','')
        db   = row.childNodes[0].firstChild.data.encode().split('/')[2]
        print coll, '  ', db
except Exception, e:
    raise RuntimeError( 'The result of the TAG catalog query could not be parsed: '
                        +str(e) + "\n" + out )

