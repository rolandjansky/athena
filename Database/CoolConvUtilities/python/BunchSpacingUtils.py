# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyCool import cool
from CoolConvUtilities.MagFieldUtils import getTimeForLB
from CoolConvUtilities.AtlCoolLib import indirectOpen


def bunchSpacingOfRun(runnumber,LB,verbose=False):
    if (runnumber<236107):
        print "WARNING BunchSpacingUtils don't work for run-1 data"
        return None

    tdaqDBName="COOLONL_TDAQ/CONDBR2"
    folder="/TDAQ/OLC/LHC/FILLPARAMS"
    iovtime=getTimeForLB(runnumber,LB,readOracle=True)

    if iovtime==0:
        print "ERROR, can't get start time of run %i, LB %i" % (runnumber,LB)
        return None

    obj=None
    db = None
    try:
        db=indirectOpen(tdaqDBName,oracle=True)
        f=db.getFolder(folder)
        obj=f.findObject(cool.ValidityKey(iovtime),0)
    except Exception,e:
        print e.args[0]
        if len(e.args)>1 and e.args[0].find("Object not found - 0"):
            print "WARNING No data found in folder %s for run/LB %i/%i" % (folder,runnumber,LB)
        else:
            print "BunchSpacingUtils: ERROR accesssing folder",folder,"on db",tdaqDBName
            print e

        if db is not None:    
            db.closeDatabase()
        return None

    pl=obj.payload()
    buf=pl["BCIDmasks"]


    bucketDiff=0
    firstFilled=-1
    lastFilled=-1
    bucketDiffs=[]

    for iBucket,filled in enumerate(buf):
        if filled!=0:
            if (verbose): print "Bucket",iBucket,"filled"
            lastFilled=iBucket
            if firstFilled<0: 
                firstFilled=iBucket
                if (verbose): print "First filled bucket=",iBucket
            else:
                if (verbose): print "Bucket #%i, bunch spacing=%i * 25ns" % (iBucket,bucketDiff);
                bucketDiffs.append(bucketDiff)
            bucketDiff=1
            
            pass 
        else: # not filled 
            if (verbose): print "Bucket",iBucket,"not filled"
            bucketDiff+=1
        pass

    #Handle wrap-around:
    if (firstFilled>=0 and lastFilled>0):
        bucketDiffs.append(len(buf)-lastFilled+firstFilled)
        if (verbose): print "Bunchdiff at wrap-around:",(len(buf)-lastFilled+firstFilled)
    if db is not None:
        db.closeDatabase()
    if len(bucketDiffs)==0:
        return None
    else: 
        return min(bucketDiffs)


if __name__=="__main__":
    import sys
    if len(sys.argv)<3:
        print "Usage: BunchSpacingUtils.py <run> <lb>"
        sys.exit(-1)
    rn=int(sys.argv[1])
    lb=int(sys.argv[2])
    print "Checking bunch spacing for run,lb",rn,lb
    print bunchSpacingOfRun(rn,lb,True)
