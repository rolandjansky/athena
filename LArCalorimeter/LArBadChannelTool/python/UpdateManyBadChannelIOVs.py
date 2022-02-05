# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from PyCool import cool
from struct import pack,unpack
from copy import deepcopy

def addAndRemove(dbname,folder,tag,ts1,ts2,chansToAdd=dict(),chansToRemove=()):

    print ("Add:",chansToAdd)
    print ("Rem:",chansToRemove)

    dbSvc = cool.DatabaseSvcFactory.databaseService()
  
    try:
        db1= dbSvc.openDatabase(dbname,False)
    except Exception as e:
        print ("Problems connecting to database:",e)
        return 1


    if not db1.existsFolder(folder):
        print ("ERROR folder",folder,"does not exist in",dbname)
        return

    fd1=db1.getFolder(folder)
    
    itr=fd1.browseObjects(ts1,ts2,cool.ChannelSelection.all(),tag)
    while itr.goToNext():
        obj=itr.currentRef()
        payload=obj.payload()
        since=obj.since()
        cc=obj.channelId()
        #print ("Working on channel %i valid since run %i" % (cc,since>>32))
        if since<ts1:
            since=ts1
            
        bcBlob=payload["Blob"]

        touched=False

        if cc in chansToAdd:
            toAdd=deepcopy(chansToAdd[cc])
            touched=True
        else:
            toAdd=dict()

        newData=()
        for i in range(int(bcBlob.size()/8)):
            (ch,bc)=unpack('II',bcBlob.read(8))
            if ch in chansToRemove:
                print ("Removing entry [0x%x 0x%x] from COOL channel %i, IOV start run %i" % (ch,bc,cc, since>>32))
                touched=True
                continue
            
            if ch in toAdd:
                bc|=toAdd.pop(ch)
                print ("Augmenting existing channel 0x%x to 0x%x in COOL channel %i, IOV start run %i" % (ch,bc,cc, since>>32))
            
            newData+=((ch,bc),)

        #Adding channels:
        for (ch,bc) in toAdd.items():
            print ("Adding entry [0x%x 0x%x] to COOL channel %i, IOV start run %i" % (ch,bc,cc, since>>32))
            newData+=((ch,bc),)


        if touched:     
            bcBlob.resize(8*len(newData))
            bcBlob.seek(0)
            for i in range(len(newData)):
                bcBlob.write(pack("II",newData[i][0],newData[i][1]))

            print ("Storing changed object channel %i, valid from run %i to %i" % (cc,since>>32,obj.until()>>32))
            fd1.storeObject(since,obj.until(),payload,cc,tag,True)
                    
    itr.close()
    db1.closeDatabase()



#########################

# Usage example:

toRemove=(0x380cf300,0x380cfc00,0x380cff00)

toAdd={1 : {0x380cf100 : 0x41, 940404992 : 0x41}}
        
addAndRemove("sqlite://;schema=test.db;dbname=CONDBR2",
             "/LAR/BadChannelsOfl/BadChannels","LARBadChannelsOflBadChannels-UPD4-00",170000<<32,cool.ValidityKeyMax,toAdd,toRemove)
    
