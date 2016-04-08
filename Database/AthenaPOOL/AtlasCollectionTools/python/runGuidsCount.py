#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
from countGuidsClient import countGuidsClient
import sys, re, os
from sys import exit

# Simple command-line driver for the countGuidsClient
# author:  Marcin.Nowak@cern.ch

parser = OptionParser()
parser.add_option("--ds", dest="dataset", default="",
                  help="Dataset name", metavar="DATASET")
parser.add_option("-q", "--query", dest="query", default="",
                  help="Event selection query", metavar="QUERY")
parser.add_option("-d", "--debug", dest="debug", default=False, action="store_true",
                  help="enable debug output")
parser.add_option("-t", "--tokens", dest="tokens", default="",
                  help="token names for which GUIDs are returned (separated by comma)", metavar="TOKENS")
parser.add_option("-g", "--get_file", dest="remoteFile", default=None,
                  help="attempt to retrieve results after an interrupted wait, from remote FILE (results are deleted after retrieval)", metavar="FILE")
parser.add_option("--workerhost", dest="workerhost", default=None,
                  help="INTERNAL: select worker node manually", metavar="HOST")
parser.add_option("--old", dest="athenaeum", default=False, action="store_true",
                  help="use the old EventLookup service")

(options, args) = parser.parse_args()

if not options.dataset and not options.remoteFile:
   print os.path.basename(sys.argv[0]) + " requires dataset name or remote file name (-g option) (-h for help)"
   exit(10)

client = countGuidsClient()
if options.workerhost:
   client.workerHost = options.workerhost
client.debug = options.debug

# ------  Call Event Lookup
try:
   if not options.athenaeum:
      results = client.countGuidsSSL(options.dataset, options.query, options.tokens)
   elif options.remoteFile:
      results = client.waitForFile(options.remoteFile)
   else:
      results = client.countGuids(options.dataset, options.query, options.tokens)

except KeyboardInterrupt, e:
   print "Keyboard interrupt " + str(e)
   exit(100)

# ------  Results processing
if options.debug:
  for line in client.output:
      print line.rstrip()	

if results == None:
   code = client.checkError()
   if code:
      exit(code)
   print "ERROR! GUID count probably failed"
   for line in client.output:
      print line.rstrip()
   exit(1)

# -----  Print out GUIDs
if not results[0]:
   print "No GUIDs found"
else:
   print "#Events, GUIDs: " + str(results[0])
   for line in results[1]:
      print line[0] + " " + str( line[1])



