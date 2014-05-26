#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" EI classes"""


class IOV(object):

    _iovs={}
    _cached={}

    def __init__(self):
        pass

    def add(self,key,value,interval):
        if key not in self._iovs:
            self._iovs[key]=[]
        self._iovs[key].append((interval,value))

    def dump(self):
        for k,v in self._iovs.iteritems():
            print k,v

    def get(self,key,point):
        (r,e)=point
        if key not in self._iovs:
            return None
        if key in self._cached:
            i,v = self._cached[key]
            (r0,e0,r1,e1) = i
            if r >= r0 and ( e >= e0  or e0 == 0 ) and \
                    r <= r1 and ( e < e1 or e1 == 0 ):
                return v
        ivalues=self._iovs[key]
        for i,v in ivalues:
            (r0,e0,r1,e1) = i
            if r >= r0 and ( e >= e0 or e0 == 0 ) and \
                    r <= r1 and ( e < e1 or e1 == 0 ):
                self._cached[key]=(i,v)
                return v
            
        return None


class EIrecord(object):

    _version=1

    _attrs=['RunNumber','EventNumber','LumiBlockN','EventTime','EventTimeNanoSec','EventWeight',
            'McChannelNumber', 'BunchId','Lvl1ID','IsSimulation','IsCalibration','IsTestBeam',
            'L1PassedTrigMask','L2PassedTrigMask','EFPassedTrigMask', 'SMK','HLTPSK','L1PSK',
            'Snam0','Snam1','Snam2','Snam3','Sref0','Sref1','Sref2','Sref3',
            ]
    
    def __init__(self):
        for key in self._attrs:
            setattr(self,key,None)
        pass

    def __setitem__(self, key, value):
        if key not in self._attrs:
            raise KeyError(key)
        setattr(self,key,value)
        
    def getRec(self):
        rec = []
        for attr in self._attrs:
            rec.append(getattr(self, attr, None))
        return rec

    def getRecSchema(self):
        rec = []
        for attr in self._attrs:
            rec.append(attr)
        return rec

    def getVersion(self):
        return self._version
        

