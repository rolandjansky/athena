# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# DetStatusCoolLib.py
# python functions for dealing with detector status read from COOL
# Richard Hawkings, 22/10/07

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen,RangeList
from DetectorStatus.DetStatusLib import DetStatusNames,DetStatusReq

def statusCutsToRange(dbconn,foldername,since,until,tag,statusreq):
    """Return a RangeList giving the good IOV range corresponding to the
    specified set of daetector status cuts, using the given DB and folder"""
    range=RangeList(since,until)
    reqs=DetStatusReq()
    reqs.setFromString(statusreq)
    folder=dbconn.getFolder(foldername)
    # loop over all requested status cuts
    for (ichan,req) in reqs.getDict().items():
        itr=folder.browseObjects(since,until,cool.ChannelSelection(ichan),tag)
        lastiov=since
        while itr.goToNext():
            obj=itr.currentRef()
            status=obj.payload()['Code']
            if (lastiov<obj.since()):
                # veto gaps where no status data was provided - assume bad
                range.vetoRange(lastiov,obj.since())
            if (status<req):
                range.vetoRange(obj.since(),obj.until())
            lastiov=obj.until()
        # veto final gap (if any)
        range.vetoRange(lastiov,until)
        itr.close()
    return range

def testStatusCutsToRange():
    dbconn=indirectOpen('COOLOFL_GLOBAL/COMP200')
    myrange=statusCutsToRange(dbconn,'/GLOBAL/DETSTATUS/LBSUMM',0,cool.ValidityKeyMax,'TRTB 3')
            
if __name__=='__main__':
    testStatusCutsToRange()
