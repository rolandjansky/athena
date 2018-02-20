# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



from libTrigConfDBFrontier import Session as _Session
from libTrigConfDBFrontier import AnyData as __AnyData
from libTrigConfDBFrontier import *

class AnyData(__AnyData):
    def __str__(self):
        return "%s %s" % (self.type(), self.getString() )


class Session:

    def __getattr__(self,name):
        # forward all calls to self.__session
        if name in dir(self.__session):
            return getattr(self.__session,name)
        raise AttributeError,"Session instance does not attribute %s" % name

    def __init__(self, conn):
        self.__fieldNames = []
        self.__fieldTypeStr = []
        self.__fieldType = []
        self.__numberOfFields = 0
        self.__records = []
        self.__session = _Session(conn)

    def getData(self, requests):
        self.__session.getData(requests)
        self.__getHeader()
        self.__getAllFields()
        

    def getHeader(self):
        return (self.__fieldNames,self.__fieldTypeStr)


    def getRecords(self):
        return self.__records


    def getRecords2(self):
        return ( self.__mapRecord(record) for record in self.__records )


    def __mapRecord(self,record):
        return tuple( [ t(r) for (r,t) in zip(record,self.__fieldType) ] )
        


    def printHeader(self):
        for i, (fname, ftype) in enumerate(zip(self.__fieldNames,self.__fieldTypeStr)):
            print "%2i :  %-20s %-10s" % (i+1, fname, ftype)


    def printRecords(self):
        for i,record in enumerate(self.getRecords()):
            print "record %4i :  %r" % (i+1, record)


    def printRecords2(self):
        for i,record in enumerate(self.getRecords2()):
            print "record %4i :  %r" % (i+1, record)


    def getNumberOfFields(self):
        return self.__numberOfFields


    def __getHeader(self):
        self.__fieldNames = []
        self.__fieldTypeStr = []
        self.__fieldType = []
        self.__numberOfFields = 0
        # jump to start
        self.__session.setCurrentLoad(1)
        # read first field and move 
        self.__session.next()
        # and then the rest
        while not self.isEOR():
            self.__fieldNames     += [self.__session.assignString()]
            self.__fieldTypeStr     += [self.__session.assignString()]
            if self.__fieldTypeStr[-1].startswith("NUMBER"):
                if ",0" in self.__fieldTypeStr[-1]:
                    self.__fieldType += [int]
                else:
                    self.__fieldType += [float]
            elif self.__fieldTypeStr[-1].startswith("VARCHAR"):
                self.__fieldType += [str]
            else:
                self.__fieldType += [str]
            self.__numberOfFields += 1


    def __getAllFields(self):
        self.__records = []

        ad = AnyData()
        while self.next():
            record = tuple()
            for k in range(self.__numberOfFields):
                self.getAnyData(ad)
                if   ad.type()==BLOB_TYPE_INT4:        record += (ad.getInt(),)
                elif ad.type()==BLOB_TYPE_INT8:        record += (ad.getLongLong(),)
                elif ad.type()==BLOB_TYPE_FLOAT:       record += (ad.getFloat(),)
                elif ad.type()==BLOB_TYPE_DOUBLE:      record += (ad.getDouble(),)
                elif ad.type()==BLOB_TYPE_TIME:        record += (ad.getLongLong(),)
                elif ad.type()==BLOB_TYPE_ARRAY_BYTE:
                    if not ad.getRawStrP():        record += ('NULL',)
                    elif (ad.getRawStrS() == 0):   record += ('',)
                    else:                          record += ('%s' % str_escape_quota(ad.getString()),)
                else: 
                    print "Error: unknown typeId ", ((int)(ad.type())),"\n"
                    exit(1);
                ad.clean()

            self.__records += [record]
            ad.clean()

        if not self.isEOF():
            print "Error: must be EOF here\n"
            sys.exit(1);


