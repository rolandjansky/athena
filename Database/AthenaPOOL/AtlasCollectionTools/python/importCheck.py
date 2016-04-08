#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# listDatasets.py - from an input list of GUIDs, gets a corresponding list of DQ2 datasets.
import cx_Oracle
import os, getopt
import re
import sys, signal
import commands
import random

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

# Usage summary
def _usage(exit = None):
   print """Command line options: [-h | --help], [-n | --nevnt], [-l | --lfn], [-g | --guid], [-s | --schema] <schema name>, [-c | --collection] <collection name (comma separated)>."""
   if exit != None:
      sys.exit()

def main():
   #dbuser='atlas_tags_pat'
   #dbpasswd='tag#test'
   #oracle_sid='intr'
   dbuser='atlas_tags_stream_test'
   dbpasswd='l0ne%star'
   oracle_sid='int8r'

   #open connection to database
   connection = cx_Oracle.Connection("%s/%s@%s" % (dbuser, dbpasswd, oracle_sid))
   cursor = cx_Oracle.Cursor(connection)

   # User options allowed
   _useropts = "hlgns:c:"
   _userlongopts = [ "help", "lfn", "guid", "nevnt", "schema=", "collection="]
   schema=""
   debugFlag=False

   #Collections=["mc11_004201_ZeeJimmy_recon_TAG_v11000401"]
   #Collections=["streamtest_exclEle","streamtest_exclJet"]
   Collections=["streamtest_Overlap", "streamtest_exclEle", "streamtest_exclJet", "streamtest_exclMuo", "streamtest_exclPho", "streamtest_exclTau", "streamtest_inclEle", "streamtest_inclJet", "streamtest_inclMuo", "streamtest_inclPho", "streamtest_inclTau"]

   viewLfn = False
   viewGuid = False
   countNumber = False


   try:
      optlist, args = getopt.getopt( sys.argv[1:], _useropts, _userlongopts )
   except getopt.error:
      print sys.exc_value
      _usage( 2 )

   if args:
      print "Unhandled arguments:", args
      _usage( 2 )

   for opt, arg in optlist:
      if opt in ("-h", "--help"):
         _usage()
         sys.exit()
      elif opt in ("-l", "--lfn"):
         viewLfn = True
      elif opt in ("-g", "--guid"):
         viewGuid = True
      elif opt in ("-s", "--schema"):
         schema = arg
      elif opt in ("-c", "--collection"):
         Collections = arg
      elif opt in ("-n", "--nevnt"):
         countNumber = True

   if (len(Collections)==0):
      print "No collections specified on relational DB"
      print "--> The python variable Collections is not set"
      sys.exit()

   #newTuple = {}
   masterDict = {}
   numberDict = {}
   for coll in Collections:
      level = "  COLL (TAB="+coll+") "
      if (debugFlag): print "  BEGIN ", level

      attributes = "COLLECTION_NAME,DATA_TABLE_NAME,LINKS_TABLE_NAME"
      if (schema !=""):
         querySql = "SELECT "+attributes+" from "+schema+".pool_collections where collection_name=\'" + coll + "\'"
      else:
         querySql = "SELECT "+attributes+" from pool_collections where collection_name=\'" + coll + "\'"

      #print level, "query = ", querySql
      cursor.execute(querySql)
      returnSql = cursor.fetchall()
      #print level, "Resultset = ", returnSql
      #make sure we only get one table name back
      if len(returnSql) == 0:
         print level, "problem executing query " + querySql
         print level, "no table names returned in output:" + str(returnSql)
         print level, "--> Skipping coll"
         continue
      if len(returnSql) > 1:
         print level, "problem executing query " + querySql
         print level, "too many table names in output:" + str(returnSql)
         print level, "--> Skipping coll"
         continue
 
      #print returnSql
      dataTableName = returnSql[0][1]
      linksTableName = returnSql[0][2]

      try:
         dq = DQ2 (
               con_url = Config().getConfig('dq2-content-client').get('dq2-content-client', 'insecure'),
               con_urlsec = Config().getConfig('dq2-content-client').get('dq2-content-client', 'secure'),
                                                                                                                                           
               loc_url = Config().getConfig('dq2-location-client').get('dq2-location-client', 'insecure'),
               loc_urlsec = Config().getConfig('dq2-location-client').get('dq2-location-client', 'secure'),
                                                                                                                                           
               rep_url = Config().getConfig('dq2-repository-client').get('dq2-repository-client', 'insecure'),
               rep_urlsec = Config().getConfig('dq2-repository-client').get('dq2-repository-client', 'secure'),
                                                                                                                                              
               sub_url = Config().getConfig('dq2-subscription-client').get('dq2-subscription-client', 'insecure'),
               sub_urlsec = Config().getConfig('dq2-subscription-client').get('dq2-subscription-client', 'secure'),
           )

         if (schema !=""):
            #queryID = "SELECT link_data from " + schema + '.' + linksTableName 
            queryID = "SELECT * from " + schema + '.' + linksTableName 
         else:
            #queryID = "SELECT link_data from " + linksTableName 
            queryID = "SELECT * from " + linksTableName 
            cursor.execute(queryID)
            returnSqlId = cursor.fetchall()

         
         guidList =[]
         linkIdDict = {}
         #print queryID
         #print returnSqlId
         for element in returnSqlId:
            linkId = element[0]
            element = element[1].strip()
            element = element.split("][")[0].split("=")[1]
            guidList.append(element)
            linkIdDict[element] = linkId
         print ""
         print str(len(guidList)) + " unique guids found in collection " + coll
         #print guidList
         #print linkIdDict
         if (debugFlag): print guidList
         
            
                                                                                                                                           
         # get datasets name by doing GUID lookup in DQ2
         dataset_names = []
         keptVuids = []
         fileList = []

         for guid in guidList:
            if (debugFlag): print "Processing GUID = " + guid
            vuid = dq.contentClient.queryDatasetsWithFileByGUID(guid)
            if (debugFlag): print "VUID(S) = " + str(vuid)
            if len(vuid) == 0:
               print "Error: guid " + guid + " returned by query is not registered in any DQ2 dataset!"
               print "Skipping to next file..."
               continue
            else: 
               #for each dataset vuid
               for v in vuid:
                  #get the dataset name
                  dataset = dq.repositoryClient.resolveVUID(v)
                  name = dataset.get('dsn')
                  if (debugFlag): print "dataset name = " + str(name)
                  #if the dataset isn't a _tid dataset, ignore, else do stuff.
                  if (re.search(r'tid',name)):
                     continue
                  elif (re.search(r'streamtest',name)):
                     #keep track of the unique vuids and dataset names
                     if keptVuids.count(v) == 0:
                        keptVuids.append(v)
                     if dataset_names.count(name) == 0:
                        dataset_names.append(name)
         #it appears that you need to send the vuid to queryFilesInDataset as a list-type
         #"files" contains a tuple of the lfns for all of the (one) dataset
         #print "guids = " +str(guidList)
         print "Getting files for VUIDs"
         #print "getting files for VUIDs=" + str(keptVuids) 
         files = dq.contentClient.queryFilesInDataset(keptVuids)
         #print files
         #now just need to get the corresponding lfn for a given guid
         print str(len(guidList)) + " GUID:LFN pairs found:"
         dataDict = {}
         for guid in guidList:
            dataDict[guid]= files[0][guid]['lfn']
            #count the number of times which GUID appears in the data_table
            #this will tell us how many events have been loaded from a file.
            if (countNumber):
               countGuids = "SELECT count(OID_1) from " + dataTableName + " where OID_1=\'" + str(linkIdDict[guid]) + "\'"
               cursor.execute(countGuids)
               returnCountGuids = cursor.fetchall()
               #print returnCountGuids[0][0]
               numberDict[guid] = returnCountGuids[0][0]
         masterDict[coll] = dataDict
    
      except DQException, e:
         print "Error", e

   for x in masterDict.keys():
      print ""
      print "######################################################"
      if (viewLfn) and not (viewGuid):
         print "LFNs found for collection " + x
         print "######################################################"
         total = 0
         if (countNumber):
            for y in masterDict[x].keys():
               print masterDict[x][y] + "\t" + str(numberDict[y])
               total = total + numberDict[y]
         else:
            for y in masterDict[x].keys():
               print masterDict[x][y]
      elif (viewLfn) and (viewGuid):
         print "LFNs & GUIDs found for collection " + x
         print "######################################################"
         total = 0
         if (countNumber):
            for y in masterDict[x].keys():
               print y + "\t" + masterDict[x][y] + "\t" + str(numberDict[y])
               total = total + numberDict[y]
         else:
            for y in masterDict[x].keys():
               print y + "\t" + masterDict[x][y]
      elif (viewGuid) and not (viewLfn):
         print "GUIDs found for collection " + x
         print "######################################################"
         total = 0
         if (countNumber):
            for y in masterDict[x].keys():
               print y + "\t" + str(numberDict[y])
               total = total + numberDict[y]
         else:
            for y in masterDict[x].keys():
               print y 
         
      print "#########################################"
      print "Total number of events loaded = " + str(total)
      print "#########################################"
      print " "
         

if __name__ == '__main__':
    main()
