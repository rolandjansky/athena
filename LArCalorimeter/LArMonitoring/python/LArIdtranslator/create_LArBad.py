#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys,os,sqlite3
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
current = resolveAlias.getCurrent()
current=current.replace('BLKP*','%sPST')
from PyCool import cool
import PyCintex as PyLCGDict
PyLCGDict.loadDict('BadChanDict')

from ROOT import LArBadChannelDBTools,AthenaAttributeList
bct = LArBadChannelDBTools()
connect = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=COMP200'
dbSvc = cool.DatabaseSvcFactory.databaseService()
db = dbSvc.openDatabase(connect,False)
A = AthenaAttributeList(bct.createCoolSpec())
log = bct.getDefaultMsgStream()

conn = sqlite3.connect('/afs/cern.ch/user/l/larmon/public/LArIdtranslator/tmp.db')
c = conn.cursor()
Tags = ['ES1','BLK']

for u,upd in enumerate(['1','4']):
  for b,bad in enumerate(['BadChannels','MissingFEBs']):
    table = bad+'UPD'+upd
    c.execute('create table %s (since integer,until integer,channel integer,status blob)'%(table))
    cmd = 'insert into %s values (?,?,?,?)' % (table)
    fld = db.getFolder('/LAR/BadChannelsOfl/'+bad)
    tag = fld.resolveTag(current%(Tags[u]))
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
        id = bc[0].get_compact()
        bit = bc[1].packedData() 
        C[id] = bit
      load = (since,until,obj.channelId(),sqlite3.Binary(str(C)))
      #print load
      c.execute(cmd,load)

conn.commit() 
conn.close()
os.system('mv -f /afs/cern.ch/user/l/larmon/public/LArIdtranslator/tmp.db /afs/cern.ch/user/l/larmon/public/LArIdtranslator/LArBad.db')
