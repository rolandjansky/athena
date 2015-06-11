# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TimeStampToRunLumi(tmstmp,guard=1,dbInstance="CONDBR2"):
    from PyCool import cool
    from time import asctime,localtime
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase("COOLONL_TRIGGER/"+dbInstance)
    folder=db.getFolder("/TRIGGER/LUMI/LBTIME")
    range=guard*24*60*60*1e9 # 2 days in ns
    t1=long(tmstmp-range)
    t2=long(tmstmp+range)
    itr=folder.browseObjects(t1,t2,cool.ChannelSelection.all())
    while itr.goToNext():
        obj=itr.currentRef()
        #print "Working on obj.until() =",asctime(localtime(obj.until()/1e9)) 
        if obj.until()>tmstmp:
            pl=obj.payload()
            run=pl["Run"]
            lb=pl["LumiBlock"]
            print "Found Run/Lumi [%i/%i] lasting from %s to %s" %\
                  (run,lb,asctime(localtime(obj.since()/1e9)),asctime(localtime(obj.until()/1e9)))
            itr.close()
            db.closeDatabase()
            return (run,lb)
    print "WARNING: No run/lumi block found for time",asctime(localtime(tmstmp/1e9)),"in folder /TRIGGER/LUMI/LBTIME of DB COOLONL_TRIGGER/CONDBR2"
    itr.close()
    db.closeDatabase()
    return None
