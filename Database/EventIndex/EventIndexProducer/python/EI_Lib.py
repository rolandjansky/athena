#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

""" EI classes"""

class IOV(object):

    _iovs={}
    _cached={}

    def __init__(self):
        pass

    def cmpBN(self,a,b):

        # compare a and b and return -1,0,1 if a is <,=,> than b
        # a (and b) are represented as a pair of numbers (most significant part, least significant part)

        try:
            (r,e) = a
            (r1,e1) = b
        except:
            raise TypeError("element must be a tuple with two components")

        c = cmp(r,r1)       # compare most significant part
        if c == 0:
            c = cmp(e,e1)   # if equal, compare least significant part

        return c


    def isInInterval(self,point,interval):

        try:
            if len(interval) != 4:
                raise TypeError("interval must contain 4 elements")
        except:
            raise TypeError("interval must be a tuple or list")

        # broke down the two parts of the interval

        i1 = interval[:2]
        i2 = interval[2:]

        # look if point belongs to [i1,i2[
        if self.cmpBN(point,i1) >= 0 and self.cmpBN(point,i2) < 0:
            return True
        else:
            return False

    def add(self,key,value,interval):
        if key not in self._iovs:
            self._iovs[key]=[]

        try:
            if len(interval) != 4:
                raise TypeError("interval must contain 4 elements")
        except:
            raise TypeError("interval must be a tuple or list")

        try:
            # convert inverval into longs to prevent lexicographic comparison
            interval2=[]
            for number in interval:
                interval2.append(long(number))
        except:
            raise TypeError("interval ranges must be a integers or longs")
        
        self._iovs[key].append((interval2,value))

    def dump(self):
        for k,v in self._iovs.iteritems():
            print k
            for e in v:
                print " ", e

    def get(self,key,point):
        if key not in self._iovs:
            return None
        if key in self._cached:
            i,v = self._cached[key]
            if self.isInInterval(point,i):
                return v
        ivalues=self._iovs[key]
        for i,v in ivalues:
            if self.isInInterval(point,i):
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
        

