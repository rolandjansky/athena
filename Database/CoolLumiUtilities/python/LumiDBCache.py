# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LumiDBCache
#
# Eric Torrence - October 2011
#
# Contents
# LumiDBCache - simple caching object to store slowly-varying DB data
#
# This works in conjunction with CoolDataReader to access COOL
# User must set up the reader before trying to access payload
from CoolLumiUtilities.CoolDataReader import CoolDataReader

from PyCool import cool

class LumiDBCache:

    def __init__(self, dbstr=None, folderstr=None):

        self.reader = CoolDataReader(dbstr, folderstr)
        self.clearValidity()

    def clearValidity(self):

        self.validStartTime = cool.ValidityKeyMax
        self.validEndTime = cool.ValidityKeyMin
        self.payload = None

    def setValidity(self, obj):
        self.validStartTime = obj.since()
        self.validEndTime = obj.until()
        self.payload = obj.payload()
        
    def isValid(self, time):
        return (self.validStartTime <= time < self.validEndTime)

    def getPayload(self, time):

        # Is existing version valid?
        if self.isValid(time):
            return self.payload

        # Nope, invalidate cache
        self.clearValidity()
        
        # Check if we already have the pre-fetched in the reader
        for obj in self.reader.data:
            if not obj.since() <= time < obj.until(): continue
            self.setValidity(obj)

        # Did we find it?
        if self.isValid(time):
            return self.payload

        # Guess not, try reading directly from DB
        self.reader.setIOVRange(time, time)
        self.reader.readData()

        # Try once more to find the object
        for obj in self.reader.data:
            if not obj.since() <= time < obj.until(): continue
            self.setValidity(obj)

        # Did we find it?
        if self.isValid(time):
            return self.payload

        return None
    
