#!/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys
import time
import urllib2,re
from PyCool import cool, coral

from CoolRunQuery.utils.AtlRunQueryLookup import InitDetectorMaskDecoder

print 'Please use the runInfo script from python/runInfo.py!'
sys.exit(-1)   

coolCherryPyRoot='http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLONL_TDAQ/COMP200/TDAQ/RunCtrl/EOR_Params/timespan/'

def sinceUntilStr(runNum):
  since = ( runNum << 32 )
  until = ( (runNum+1) << 32 )-1
  return str(since)+'-'+str(until)
  
        
def channelValueQuery(url):
  httpHeaders={'Accept':'text/xml'}
  textData=None
  request=urllib2.Request(url,textData, httpHeaders)
  u=urllib2.urlopen(request)
  xmlDocString = u.read()
  return xmlDocString
  
#never know which xml library is available, so we'll use regex instead  
def extractValue(xmlString, valueName):
  regex='<value name="'+valueName+'">(.+)</value>'
  m=re.search(regex,xmlString)
  try:
    return m.group(1)
  except AttributeError:
    return None

def extractChannelIov(xmlString):
  regex='<channel .+ since="(\d+)" until="(\d+)"'
  m=re.search(regex,xmlString)
  try:
    return long(m.group(1)), long(m.group(2))
  except AttributeError:
    return None
     
     
def main( runNum=None ):
  if runNum == None:
    print 'ERROR no runNumber given'
    sys.exit(-1)
  url=coolCherryPyRoot+sinceUntilStr(runNum)+"/channels/0"
  print 'runInfo from scripts folder'
  #e.g. : http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLONL_TDAQ/COMP200/TDAQ/RunCtrl/EOR_Params/timespan/732227499458560-732231794425855/channels/0
  
  xmlString       = channelValueQuery(url)
  RunNumber       = extractValue(xmlString,'RunNumber')
  iovstart,iovend = extractChannelIov(xmlString)
  nLB             = iovend - ( iovstart + 1 )
  ProjectTag      = extractValue(xmlString,'FilenameTag' )
  DetectorMask    = extractValue(xmlString,'DetectorMask' )
  
  ts1 = long(extractValue(xmlString,"SORTime"))/1000000000L
  ts2 = long(extractValue(xmlString,"EORTime"))/1000000000L
  #--- Detector mask
  mask=long(DetectorMask)
  dName, NotInAll, vetoedbits = InitDetectorMaskDecoder()
  res=''
  signatures={"SCT BA":"SCTBA,", "SCT BC": "SCTBC,", "SCT EA":"SCTEA,", "SCT EC":"SCTEC,"}
  for i in range(64):
      if ( mask & (1 << i) ) :
        for k,v in signatures.iteritems():
          if k in dName[i] :
              res += v
  res = res[:-1] #chop off that last comma
  if not res: #if res is empty...make it 'NONE'
    res = 'NONE'
      
  #print RunNumber, ProjectTag, ts1, ts2, nLB, res
  fw = open( 'runInfo.txt', 'w' )
  fw.write( str(RunNumber) + ' ' + str(ProjectTag) + ' ' + str(ts1) + ' ' + str(ts2) + ' ' + str(nLB) + ' ' + res )
  fw.close()
     
if __name__ == "__main__":
    try:
        runNumber = int( sys.argv[1] )
    except:
        print "The run number could not be converted to int"
        sys.exit(-1)
        
    main(runNum = runNumber)
