#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys,os,sqlite3
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
currentes = resolveAlias.getCurrentES()
if 'CONDBR2' not in currentes:
  currentes='CONDBR2-ES1PA-2014-01'
current = resolveAlias.getCurrent()
if 'CONDBR2' not in currentes:
  current='CONDBR2-BLKPA-2014-01'

from PyCool import cool
import PyCintex as PyLCGDict
PyLCGDict.loadDict('BadChanDict')

def GetTag(u):
     resolver=resolveAlias()
     currentGlobal = ''
     if   u=='1': currentGlobal=resolver.getCurrentES().replace("*","ST")
     elif u=='4': currentGlobal=resolver.getCurrent()  .replace("*","ST")
     return currentGlobal


from ROOT import LArBadChannelDBTools,AthenaAttributeList
bct = LArBadChannelDBTools()
connect = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2'
dbSvc = cool.DatabaseSvcFactory.databaseService()
db = dbSvc.openDatabase(connect,False)
A = AthenaAttributeList(bct.createCoolSpec())
log = bct.getDefaultMsgStream()

conn = sqlite3.connect('LArBad_tmp.db')
c = conn.cursor()
Tags = [currentes,current]

for u,upd in enumerate(['1','4']):
  print "Tag used for LArBad.db UPD%s = %s"% (upd,GetTag(upd)) 
  for b,bad in enumerate(['BadChannels','MissingFEBs']):
    table = bad+'UPD'+upd
    c.execute('create table %s (since integer,until integer,channel integer,status blob)'%(table))
    cmd = 'insert into %s values (?,?,?,?)' % (table)
    fld = db.getFolder('/LAR/BadChannelsOfl/'+bad)
    tag = fld.resolveTag(Tags[u])
    objs = fld.browseObjects(cool.ValidityKeyMin,cool.ValidityKeyMax,cool.ChannelSelection.all(),tag)

    for obj in objs:
      since = int(obj.since()>>32)
      #if since<158255: continue
      #if obj.channelId()!=2: continue
      p = obj.payload()
      for i in ['Blob','ChannelSize','StatusWordSize','Endianness','Version']: A[i] = p[i]
      until = int(obj.until()>>32)
      a = bct.readBadChan(A,log)
      C = {}
      I = a.size()
      for i in range(I):
        bc = a.at(i)
        id = bc[0].get_identifier32().get_compact()
        bit = bc[1].packedData() 
        C[id] = bit
      load = (since,until,obj.channelId(),sqlite3.Binary(str(C)))
      #print load
      c.execute(cmd,load)

conn.commit() 
conn.close()
