#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# listDatasets.py - from an input list of GUIDs, gets a corresponding list of DQ2 datasets.

import os
import re
import sys, signal
import commands
import random
import getopt

try:
   import dq2.clientapi.cli.cliutil
   from dq2.common.DQDashboardTool import DQDashboardTool
   from dq2.clientapi.cli.cliutil import getDQ2
   from dq2.clientapi.DQ2 import DQ2
   from dq2.common.Config import Config
   from dq2.common.DQException import *
   from dq2.location.client.LocationClient import LocationClient
except ImportError:
   print "Environment not set [error importing DQ2 dependencies]!"
   print "Try setting PYTHONPATH to the dq2-client directory."
   sys.exit(1)


def usage():
   """
   listDatasets.py --colls <list of collections> OR listDatasets.py --guids <list of guids>  """
   print usage.__doc__

def fillMaps(guids):

   dq = DQ2 (
      con_url = Config().getConfig('dq2-content-client').get('dq2-content-client', 'insecure'),
      con_urlsec = Config().getConfig('dq2-content-client').get('dq2-content-client', 'secure'),
                                                            
      loc_url = Config().getConfig('dq2-location-client').get('dq2-location-client', 'insecure'),
      loc_urlsec = Config().getConfig('dq2-location-client').get('dq2-location-client', 'secure'),
                                               
      rep_url = Config().getConfig('dq2-repository-client').get('dq2-repository-client', 'insecure'),
      rep_urlsec = Config().getConfig('dq2-repository-client').get('dq2-repository-client', 'secure'),
    
      sub_url = Config().getConfig('dq2-subscription-client').get('dq2-subscription-client', 'insecure'),
      
   )

   dsets = {}
   # Fetch datasets for the guids ==> map[guid] = [datasets]
   dsets = dq.listDatasetsByGUIDs(guids)
   dslist = []
   dsguids = {}
   # Reverset the map map[dataset] = [guids]
   for guid in guids:
      for ds in dsets[guid]:
         if ds not in dslist:
            dslist += [ds]
         try:
            dsguids[ds] += [guid]
         except:
            dsguids[ds] = [guid]
         
   dslist.sort()
   # Loop over datasets and fill lfnmap
   lfnmap = {}
   for ds in dslist:
      # Get the files for the dataset
      (filelist,date) = dq.listFilesInDataset(ds)
      # Loop over guids for that dataset and filter filelist by guid list
      for guid in dsguids[ds]:
         try: 
            info = filelist[guid]
            #print guid,info['lfn']
            lfnmap[guid] = info['lfn']
            #print "DS="+ds+"; GUID="+guid+"; LFN="+info['lfn']
            #print "(" + str(cnt) + ") LFN=" + info['lfn'] + "; DS=" + ds
         except:
            pass

   return (dsets,lfnmap)

def main():

   if len(sys.argv) < 2:
      usage()
      sys.exit(0)

   opts = []
   try:
      longopts = ['guids=','colls=']
      opts,args=getopt.getopt(sys.argv[1:],'',longopts)
   except getopt.GetoptError,e:
      print e
      usage()
      sys.exit(0)

   try:

      refguids = {}
      refnames = []
      # get GUIDs from the collection(s)
      guids = []
      collections = []

      for o,a in opts:
         if (o=='--guids') : 
            guids=str(a).split(',')
            refnames = ['GENERIC']
            refguids['GENERIC'] = []
            for guid in guids:
               refguids['GENERIC'] += [guid]
         if (o=='--colls') : collections=str(a).split()

      # Check if guids are already specified
      if len(guids) == 0 and len(collections) != 0 :
        # If not, then fetch from collection
        for collection in collections:
          coll_name = collection[:-5]
          command_string = "CollListFileGUID.exe -src " + coll_name + " RootCollection"
          guid_string = commands.getoutput(command_string)
          clfgout = guid_string.split('\n')
          for line in clfgout:
            words = line.split()
            if words[0] not in refnames: 
              refnames += [words[0]]
              refguids[words[0]] = [words[1]]
            else:
              if words[1] not in refguids[words[0]]: 
                 refguids[words[0]] += [words[1]]
            guids += [words[1]]
 
        if len(guids) == 0:
          print "Error: query returned no files"
          sys.exit(1)

      elif len(guids) == 0:
        print "Error: Did not specify properly input"
        usage()
        sys.exit(1)

      for ref in refnames:
         dsets = {}
         (dsets,lfnmap) = fillMaps(refguids[ref])
         cnt = 0
         print "\nINFORMATION FOR REF = ",ref,"\n"
         for guid in refguids[ref]:
            dslist = ""
            for ds in dsets[guid]:
               if dslist != "" : dslist += ", "
               dslist += ds
            if dslist == "" : dslist = "NONE FOUND"
            print "(" + str(guid) + ") LFN=" + lfnmap[guid] + "; DS=(" + str(len(dsets[guid])) + ") " + dslist
 
      sys.exit(0)

   except DQException, e:
      print "Error", e

if __name__ == '__main__':
    main()

