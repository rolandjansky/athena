#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os, cgi, urllib2, libxml2, time, calendar
#import time, calendar
import cgitb; cgitb.enable()
server="ATLAS_COOLPROD"
dbname="COMP200"
detlist=['Pix B','Pix Disks','Pix B-layer','Pix Other','SCT BA','SCT BC','SCT EA','SCT EC','TRT BA','TRT BC','TRT EA','TRT EC','LAr EMBA','LAr EMBC','LAr EMECA','LAr EMECC','LAr HECA','LAr HECC','LAr FCALA','LAr FCALC','Til BA','Til BC','Til EA','Til EC','MDT BA','MDT BC','MDT EA','MDT EC','RPC BA','RPC BC','TGC EA','TGC EC','CSC EA','CSC EC','L1calo pre','L1calo cluDAQ','L1calo cluRoI','L1calo JetEDAQ','L1calo JetERoI','MUCTPI','CTP','L2SV','SFI','SFO','LVL2','EF','BCM','Lucid','ZDC','Alfa']

# local 
def dateStart(dateString):
  t=time.strptime(dateString,"%d/%m/%Y")
  startOfDay=calendar.timegm(t)
  return long(startOfDay)*1000000000L

def dateEnd(dateString):
  t=time.strptime(dateString,"%d/%m/%Y")
  endOfDay=calendar.timegm(t) + (24*3600) - 1
  return long(endOfDay)*1000000000L
    
def timestampStart(runNum):
  return long(runNum) << 32
  
def timestampEnd(runNum):
  return ((long(runNum) +1L) << 32) - 1L
  
def runFromTime(timestamp):
  return str(long(timestamp) >> 32)
  
def timeRep(timestamp):
  if (timestamp is not None):
    stime=int(timestamp/1000000000L)
    return time.strftime('%F:%T', time.gmtime(stime))
  else:
    return 'unknown'
    
def listStatusFromRuns(iov0, iov1):
  #iov0 = str(timestampStart(runStart))
  #iov1 = str(timestampEnd(runEnd))
  schema = "ATLAS_COOLOFL_GLOBAL"
  folder = "/GLOBAL/DETSTATUS/LBSUMM"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def listTriggerLbTimes(iov0,iov1):
  schema="ATLAS_COOLONL_TRIGGER"
  folder="/TRIGGER/LUMI/LBTIME"
  tag="HEAD"
  channelSelection="0"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml

#dae in format dd/mm/yyyy
def listTdaqRunDb(date0,date1):
  time0=" 00:00:00"
  time1=" 23:59:59"
  splitDate0=date0.split("/")
  splitdate1=date1.split("/")
  s="-"
  #service accepts yyyy-mm-dd hh:mm:ss
  formattedDate0=splitDate0[2]+s+splitDate0[1]+s+splitDate0[0]+time0
  formattedDate1=splitDate1[2]+s+splitDate1[1]+s+splitDate1[0]+time1
  urlbase="http://atlas-service-runinformation.web.cern.ch/atlas-service-runinformation/cgi-bin/runnumbers.py?start.date="
  url=urlbase+formattedDate0+"&end.date="+formattedDate1+"&format=html"
  textData=None
  httpHeaders={'Accept':'text/html'}
  request=urllib2.Request(url,textData, httpHeaders)
  u=urllib2.urlopen(request)
  xmlDocString = u.read()
  return xmlDocString
  
def listTdaqStartOfRunParameters(iov0, iov1):
  #iov0 = str(timestampStart(runStart))
  #iov1 = str(timestampEnd(runEnd))
  schema = "ATLAS_COOLONL_TDAQ"
  folder = "/TDAQ/RunCtrl/SOR_Params"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def listTdaqEndOfRunParameters(iov0, iov1):
  schema = "ATLAS_COOLONL_TDAQ"
  folder = "/TDAQ/RunCtrl/EOR_Params"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml

def listTdaqSfoStatus(iov0, iov1):
  schema = "ATLAS_COOLONL_TDAQ"
  folder = "/TDAQ/RunCtrl/FinalSFOStat"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def listTdaqEventCounters(iov0, iov1):
  schema = "ATLAS_COOLONL_TDAQ"
  folder = "/TDAQ/RunCtrl/EventCounters"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def listHltTriggerKey(iov0, iov1):
  server = "ATLAS_COOLPROD"
  schema = "ATLAS_COOLONL_TRIGGER"
  dbname = "COMP200"
  folder = "/TRIGGER/HLT/HltConfigKeys"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def listTriggerKey(iov0, iov1):
  server = "ATLAS_COOLPROD"
  schema = "ATLAS_COOLONL_TRIGGER"
  dbname = "COMP200"
  folder = "/TRIGGER/LVL1/Lvl1ConfigKey"
  tag = "HEAD"
  channelSelection="*"
  unfilteredXml = channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channelSelection)
  return unfilteredXml
  
def channelValueQuery(server, schema, dbname, folder, iov0,iov1, tag, channels):
  timespan= str(iov0) + '-' + str(iov1)
  #prefix='http://atlmysql02.cern.ch:8080/cooldb/'
  prefix='http://coolcherrypy.cern.ch:8080/cooldb/'
  d = '/'
  url = prefix+server+d+schema+d+dbname+folder+'/timespan/'+timespan+'/tag/'+tag+'/channels/'+channels
  httpHeaders={'Accept':'text/xml'}
  textData=None
  request=urllib2.Request(url,textData, httpHeaders)
  u=urllib2.urlopen(request)
  xmlDocString = u.read()
  return xmlDocString
  
def cutAndPaste(targetChannel, sourceChannel, xpathString):
  fragments=sourceChannel.xpathEval(xpathString)
  if (fragments):
    fragment=fragments[0]
    fragment.unlinkNode()
    targetChannel.addChild(fragment)
  
def cutAndPasteValue(targetChannel, sourceChannel, valueName):
  fragments=sourceChannel.xpathEval("value[@name='"+valueName+"']")
  if (fragments):
    fragment=fragments[0]
    fragment.unlinkNode()
    targetChannel.addChild(fragment)

def insertNewValue(targetChannel, valueName, valueData):
  childValue=targetChannel.newChild(None, 'value',valueData)
  childValue.setProp('name',valueName)
  
def channelValue(channel, valueName):
  values= channel.xpathEval("value[@name='"+valueName+"']/text()")
  if(values):
    return values[0].content
  else:
    return False

def anyKindOf(choice):
  if (str(choice).upper() in ['*', 'ANY', 'ALL', '']):
    return True
  else:
    return False

#simple dictionary wrapper to hold a filter and generate the appropriate xpath
class ChannelFilter:
  def __init__(self):
    self.filterDict={}
    self.empty=True
    
  def add(self,attrName, comparison, attrValue):
    filterArray=[comparison, attrValue]
    self.filterDict[attrName]=filterArray
    self.empty=False
    
  def xpathString(self):
    preName="(value[@name='"
    postName="']"
    orLink=") or " #join filters with 'or'
    lastTruncation=len(orLink) - 1 #remove the trailing 'or' conjunction when the loop ends
    tmp=""
    for f in self.filterDict.keys():
      xpOperator=self.filterDict[f][0]
      v=self.filterDict[f][1]
      #special case: strings should be quoted
      if (isinstance(v,str)):
        d=self.quote(v)
      else:
        d=v
      #special case, invented operator !ciContains , '!case insensitive contains'
      if (xpOperator == "!ciContains"):
        u=d.upper()
        tmp=tmp+"(value[@name='"+f+"' and not(contains(translate(.,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),"+u+"))]) or "
      else:
        tmp=tmp + preName+f+postName+xpOperator+str(d)+orLink
    x=tmp[:-lastTruncation]
    #print x
    return x
  def isEmpty(self):
    return self.empty
    
  def quote(self, attributeValue):
    return "'"+attributeValue+"'"
    
  def xpUpper(self, lcString):
    xpUpper="translate("+lcString+",'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"
    
def filterOut(channel,channelFilter):
  if (channelFilter.isEmpty()):
    return False
  values=channel.xpathEval(channelFilter.xpathString())
  if (values):
    channel.unlinkNode()
    return True
  else:
    return False

def maskDecode(mask):
  """Decode the detector mask into a string list of detectors"""
  # data taken from Giovana L, August 2008
  masklist=['Pix B','Pix Disks','Pix B-layer','Pix Other','SCT BA','SCT BC','SCT EA','SCT EC','TRT BA','TRT BC','TRT EA','TRT EC','LAr EMBA','LAr EMBC','LAr EMECA','LAr EMECC','LAr HECA','LAr HECC','LAr FCALA','LAr FCALC','Til BA','Til BC','Til EA','Til EC','MDT BA','MDT BC','MDT EA','MDT EC','RPC BA','RPC BC','TGC EA','TGC EC','CSC EA','CSC EC','L1calo pre','L1calo cluDAQ','L1calo cluRoI','L1calo JetEDAQ','L1calo JetERoI','MUCTPI','CTP','L2SV','SFI','SFO','LVL2','EF','BCM','Lucid','ZDC','Alfa']
  res=""
  for i in range(0L,(len(masklist))-1L):
      if (mask & (1L << i)): 
        res+=masklist[i]+", "
  # chop off last comma-space
  if len(res)>1: res=res[:-2]
  return res

#special filter due to the absence of bitwise operators in xpath
def detectorMaskFilter(channel, detMask):
  if (detMask==''):
     return False
  thisChannelMask=channelValue(channel, 'DetectorMask')
  thisMaskStr=maskDecode(long(thisChannelMask))
  maskArray=detMask.split(':')
  #print detMask
  f=False
  for det in maskArray:
    f = f or (thisMaskStr.find(det)!=-1)
  if (not(f)):
    channel.unlinkNode()
  return not(f)

  
# we prepare the xml here, so send xml back
print "Cache-Control: no-store, no-cache"
print "Expires: Fri, 30 Oct 1998 14:19:41 GMT"
print "Content-type: text/xml\n"
#blank line means end of http headers

form=cgi.FieldStorage()
run0 = 0
run1 = 0

if form == None:
  
  print 'remplir le formulaire!'
  run0=183003
  
if form == None:
  
  print 'remplir le formulaire!'
  run1=183003

#run1= form["end"].value
#run1= form["end"].value

if('format' in form):
  format= form["format"].value
else:
  format='run'
#format='run'
cacheFile=""
useCache=False
if ((run0 > 27777) and (run1 < 29581)):
  cacheFile="../xml/M5Summary.xml"
  useCache=True
if ((run0 > 36526) and (run1 < 38001)):
  cacheFile="../xml/Feb08aSummary.xml"
  useCache=True
if ((run0 > 38000) and (run1 < 39501)):
  cacheFile="../xml/Feb08bSummary.xml"
  useCache=True
if ((run0 > 39500) and (run1 < 40953)):
  cacheFile="../xml/Feb08cSummary.xml"
  useCache=True

#convert to iov
if (format=='date'):
  #iov0 = str(dateStart(run0))
  #iov1 = str(dateEnd(run1))
  #lbTimeXml=listTriggerLbTimes(iov0,iov1)
  lbTimeXml=listTdaqRunDb(run0,run1)
  lbt=libxml2.parseDoc(lbTimeXml)
  #find min and max runs
  chans=[]
  #c0=lbt.xpathEval('//value[@name="Run"]')
  c0=lbt.xpathEval('/html/body/table/tr/td[2]')
  for j in c0:
    chans.append(j.content)
  run0=str(min(chans))
  run1=str(max(chans))
  lbt.freeDoc()

iov0 = str(timestampStart(run0))
iov1 = str(timestampEnd(run1))

#inverted logic on recording enabled
if ('rec' in form):
  recStr=form["rec"].value
else:
  recStr='*'
  
if (not anyKindOf(recStr)):
  if (int(run1) < 58788):
    rec=1-int(recStr)
  else:
    rec=int(recStr)
else:
  rec=None
#
if ('minEvents' in form):
  dur=form["minEvents"].value
else:
  dur='1'
  
  #
if ('rtype' in form):
  rtype=form["rtype"].value
else:
  rtype='*'
  
if ('ftag' in form):
  ftag=form["ftag"].value
else:
  #default is to exclude the data_test tag
  ftag='!data_test'
    
duration=str(dur)
runType=str(rtype).split()[0]
fileTag=str(ftag).split()[0]
#
detMask=''
if ('det' in form):
  detMask=str(form["det"].value)

sorfilter=ChannelFilter()
eorfilter=ChannelFilter()

#
#statusXml = listStatusFromRuns(run0,run1)
if (not useCache):
  sorXml = listTdaqStartOfRunParameters(iov0,iov1)
  eorXml = listTdaqEndOfRunParameters(iov0,iov1)
  eventCounterXml = listTdaqEventCounters(iov0,iov1)
  trigKeyXml=listTriggerKey(iov0, iov1)
  hltKeyXml = listHltTriggerKey(iov0, iov1)
  sorXml='<?xml-stylesheet type="text/xsl" href="../xsl/runList.xsl"?>'+sorXml
  sor= libxml2.parseDoc(sorXml)
  eor=libxml2.parseDoc(eorXml)
  configKey=libxml2.parseDoc(trigKeyXml)
  hltKey=libxml2.parseDoc(hltKeyXml)
else:
  sorXml = libxml2.parseFile(cacheFile)
  if (format != 'date'):
    sorfilter.add('RunNumber','<',run0)
    sorFilter.add('RunNumber','>',run1)

channelNodes=sor.xpathEval("/channels/channel")
#

  
if (rec is not None):
  sorfilter.add('RecordingEnabled','!=',rec)

if (not anyKindOf(runType)):
  sorfilter.add('RunType','!ciContains',runType)
  
if (not anyKindOf(fileTag)):
  if (fileTag[0]=='!'):
    condition='='
    realTag=fileTag[1:]
  else:
    condition='!='
    realTag=fileTag
  sorfilter.add('FilenameTag',condition,realTag)
#
for channel in channelNodes:
  if (detectorMaskFilter(channel, detMask)):
    continue
  if (filterOut(channel,sorfilter)):
    continue
  runNumber=channelValue(channel, 'RunNumber')
  sorTimestamp=channelValue(channel,'SORTime')
  sorDate=timeRep(long(sorTimestamp))
  insertNewValue(channel,'Start',sorDate)
  maskString=maskDecode(long(channelValue(channel, 'DetectorMask')))
  insertNewValue(channel,'MaskInterpreted', maskString)
  # add trigger channels
  thisSince=channel.xpathEval("@since")[0].content
  thisUntil=channel.xpathEval("@until")[0].content
  triggerChannels=hltKey.xpathEval("/channels/channel[@since='"+thisSince+"']")
  if (len(triggerChannels) > 0):
    triggerChannel=triggerChannels[0]
    cutAndPasteValue(channel, triggerChannel,'MasterConfigurationKey')
    cutAndPasteValue(channel, triggerChannel,'HltPrescaleConfigurationKey')
  #
  lvl1PrescaleChannels=configKey.xpathEval("/channels/channel[@since='"+str(long(thisSince) + 1L)+"']")
  if (len(lvl1PrescaleChannels) > 0):
    lvl1PrescaleChannel=lvl1PrescaleChannels[0]
    cutAndPasteValue(channel, lvl1PrescaleChannel,'Lvl1PrescaleConfigurationKey')
  #add any relevant eor channels
  if (not useCache):
    eorChannels=(eor.xpathEval("/channels/channel[value[@name='RunNumber']="+runNumber+"]"))
    if (eorChannels):
      eorChannel=eorChannels[0]
      cutAndPasteValue(channel, eorChannel, 'EORTime')
      cutAndPasteValue(channel,eorChannel,'TotalTime')
      cutAndPasteValue(channel,eorChannel,'CleanStop')
      cutAndPasteValue(channel,eorChannel,'DAQConfiguration')
    #
      eorTimestamp=channelValue(channel,'EORTime')
      if (eorTimestamp):
        eorDate=timeRep(long(eorTimestamp))
        insertNewValue(channel,'End',eorDate)
  #if theres an eor filter defined, filter on eor criteria 
  filterOut(channel,eorfilter)
   
       
if (not useCache):  
  eor.freeDoc()
  ctr=libxml2.parseDoc(eventCounterXml)
  ctrChannelNodes=ctr.xpathEval("/channels/channel")
  for ctrChannel in ctrChannelNodes:
    since=long(ctrChannel.xpathEval("@since")[0].content)
    until=long(ctrChannel.xpathEval("@until")[0].content)
    runNumber=runFromTime(since)
    #print runNumber
    sorChannels=(sor.xpathEval("/channels/channel[value[@name='RunNumber']="+runNumber+"]"))
    if (sorChannels):
      sorChannel=sorChannels[0]
      if (long(channelValue(ctrChannel, 'RecordedEvents')) < long(duration)):
        sorChannel.unlinkNode()
        continue
      cutAndPasteValue(sorChannel, ctrChannel, 'PartitionName')
      cutAndPasteValue(sorChannel, ctrChannel, 'L1Events')
      cutAndPasteValue(sorChannel, ctrChannel, 'L2Events')
      cutAndPasteValue(sorChannel, ctrChannel, 'EFEvents')
      cutAndPasteValue(sorChannel, ctrChannel, 'RecordedEvents')
    #
      lb=(until-since-1)
      if (lb):
        lbString=str(lb)
        insertNewValue(sorChannel,'LB',lbString)
  ctr.freeDoc()
  
print sor.serialize()
sor.freeDoc()


  
