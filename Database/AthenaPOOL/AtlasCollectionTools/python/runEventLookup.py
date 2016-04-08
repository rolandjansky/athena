#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
from eventLookupClient import eventLookupClient
import sys, re, os, time
from sys import exit

PandaSys = '/afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest'

# Simple command-line driver for the EventLookupClient
# author:  Marcin.Nowak@cern.ch

parser = OptionParser()
parser.add_option("-d", "--debug", dest="debug", default=False, action="store_true",
                  help="enable debug output")
parser.add_option("-a", "--async", dest="async", default=None, action="store_true",
                  help="OLD mode: asks the server to run EventLookup in background to prevent timeouts for large queries")
parser.add_option("-f", "--runs_events_file", dest="filename",
                  help="read the list of run-event pairs from FILE", metavar="FILE")
parser.add_option("-e", "--runs_events", dest="runs_events",
                  help="list of run-event pairs (pairs separated by comas, space between run and event number)", metavar="LIST")
parser.add_option("-s", "--stream", dest="stream", default="",
                  help="stream name", metavar="STREAM")
parser.add_option("-t", "--tokens", dest="tokens", default="",
                  help="token names for which GUIDs are returned (separated by comma)", metavar="TOKENS")
parser.add_option("-p", "--amitag", dest="amitag", default="",
                  help="amitag pattern that can be used to identify TAG reprocessing pass", metavar="AMITAG")
parser.add_option("--mcarlo", dest="mcarlo", default=False, action="store_true",
                  help="request to return only MC TAGs")
parser.add_option("--gettags", dest="gettags", default=False, action="store_true",
                  help="return list of guids per event (slower)")
parser.add_option("-g", "--get_file", dest="remoteFile", default=None,
                  help="OLD mode:attempt to retrieve results after an interrupted wait, from remote FILE (results are deleted after retrieval)", metavar="FILE")
parser.add_option("--workerhost", dest="workerhost", default=None,
                  help="INTERNAL: select worker node manually", metavar="HOST")
parser.add_option("--resolve_guids", dest="getDatasets", default=False, action="store_true",
                 help="resolve GUIDs into dataset and file names using PanDA+DQ2. Does nothing when --gettags switch is also present")
parser.add_option("--old", dest="athenaeum", default=False, action="store_true",
                  help="use the OLD EventLookup service")

(options, args) = parser.parse_args()

runs_events = []
if options.filename:
   if not options.athenaeum:
      runs_events = options.filename
   else:
      file = open(options.filename)
      try:
         for line in file:
            line = line.strip()
            if len(line) > 2:
               runs_events.append(line.split())
      finally:
         file.close()
    

if options.runs_events:
   for run_ev in options.runs_events.split(','):
      runs_events.append( run_ev.split() )

action = 0   
if options.filename: action += 1
if options.runs_events: action += 1
if options.remoteFile: action += 1
if action > 1:
   print os.path.basename(sys.argv[0]) + " -e, -f and -g options are exclusive (-h for help)"
   exit(11)
if action == 0:
   print os.path.basename(sys.argv[0]) + " requires  -e, -f or -g option (-h for help)"
   exit(10)

if options.debug:
	print "Event numbers: "
	print runs_events
	
if options.workerhost:
   eventLookupClient.workerHost = options.workerhost

client = eventLookupClient()
client.debug = options.debug

# ------  Call Event Lookup
try:
   if not options.athenaeum:
      guids = client.doLookupSSL(runs_events, 
                                 stream = options.stream, tokens = options.tokens,
                                 amitag = options.amitag, mcarlo = options.mcarlo,
                                 extract = options.gettags)
   elif options.remoteFile:
      guids = client.waitForFile(options.remoteFile)
   else:
      guids = client.doLookup(runs_events, async = options.async,
                              stream = options.stream, tokens = options.tokens,
                              amitag = options.amitag, extract = options.gettags)
except KeyboardInterrupt, e:
   print "Keyboard interrupt " + str(e)
   exit(100)

# ------  Results processing
if options.debug:
  for line in client.output:
      print line.rstrip()	

if guids == None:
   code = client.checkError()
   if code:
      exit(code)
   print "ERROR!  Event lookup probably failed"
   for line in client.output:
      print line.rstrip()
   exit(1)


#--- taken from pandatools/Client 
def printGUIDsWithDatasets(guids):
    """ list datasets by GUIDs
    """
    try:
      from pandatools import Client
    except ImportError:
      if os.environ.has_key('PANDA_SYS'):
         pandapath = os.environ['PANDA_SYS']
      else:
         pandapath = PandaSys
      sys.path.append(pandapath + '/lib/python2.4/site-packages')
      try:
         from pandatools import Client
      except ImportError:
         print "EventLookup failed to import PanDA client, GUID->dataset name resolution disabled"
         return False

    # instantiate curl
    curl = Client._Curl()
    curl.verbose = False
    iLookUp = 0
    guidLfnMap = {}
    checkedDSList = []
    # loop over all GUIDs
    for guid in guids.keys():
        # check existing map to avid redundant lookup
        if guidLfnMap.has_key(guid):
            continue
        iLookUp += 1
        if iLookUp % 20 == 0:
            time.sleep(1)
        # get vuids
        url = Client.baseURLDQ2 + '/ws_content/rpc'
        data = {'operation': 'queryDatasetsWithFileByGUID','guid':guid,
                'API':'0_3_0','tuid': Client.MiscUtils.wrappedUuidGen()}
        status,out = curl.get(url,data,rucioAccount=True)
        if status != 0 or re.search('Exception', str(out)) != None:
           # failed
           guidLfnMap[guid] = "DQ2 query ERROR: " + str(out)
           continue
        if out == '\x00' or out == ():
           guidLfnMap[guid] =  "DQ2.queryDatasetsWithFileByGUID() returned no results"
           continue
        tmpVUIDs = list(out)
        # get dataset name
        url = Client.baseURLDQ2 + '/ws_repository/rpc'
        data = {'operation':'queryDatasetByVUIDs','vuids':tmpVUIDs,
                'API':'0_3_0','tuid': Client.MiscUtils.wrappedUuidGen()}
        status,out = curl.post(url, data, rucioAccount=True)
        if status != 0 or re.search('Exception', str(out)) != None:
           # failed
           guidLfnMap[guid] = "DQ2 query ERROR: " + str(out)
           continue
        if out == '\x00':
           guidLfnMap[guid] =  "DQ2.queryDatasetByVUIDs() returned no results"
           continue
        for tmpDsName in out.keys():
            # ignore junk datasets
            if not (tmpDsName.startswith('panda') or \
                    tmpDsName.startswith('user') or \
                    tmpDsName.startswith('group') or \
                    re.search('_sub\d+$',tmpDsName) != None or \
                    re.search('_dis\d+$',tmpDsName) != None or \
                    re.search('_shadow$',tmpDsName) != None \
                    or tmpDsName in checkedDSList ):
               tmpMap = Client.queryFilesInDataset(tmpDsName)
               for tmpLFN,tmpVal in tmpMap.iteritems():
                   guidLfnMap.setdefault(tmpVal['guid'],[]).append([tmpLFN,tmpDsName])
               checkedDSList.append(tmpDsName)

    for guid in guids.keys():
       print guid, guids[guid], guidLfnMap.setdefault(guid,"")
    return True


      
# -----  Print out GUIDs
if len(guids) == 0:
   print "No GUIDs found"
   for line in client.output:
      if re.search("Warning: no TAG collections were found", line):
         print "Warning: no TAG Collections matched the criteria (run,stream,amitag)"   
else:
   if type(guids)==type({}):
      # default GUID printout
      if options.getDatasets:
         if printGUIDsWithDatasets(guids):
            exit(0)
      for guid in guids.keys():
         print guid, guids[guid]
   else:
      # this is the per-event listing from --gettags option
      (attrNames, attrVals) = guids
      print "TAG attributes are: " + str(attrNames)
      for tag in attrVals:
         print tag



