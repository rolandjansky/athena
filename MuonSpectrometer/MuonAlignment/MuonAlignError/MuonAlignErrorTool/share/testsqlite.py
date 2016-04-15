#!/bin/env python
from PyCool import cool,coral
import cx_Oracle
import json, sys
from collections import OrderedDict

dbfile="test.sqlite"
dbname="COMP200"
Tag = "MuonAlignErrors-TEST-00"
fdname='/MUONALIGN/ERRS'

# main script
#
#
# remove the old db file so that we can write the new one
try:
    import os
    os.remove(dbfile)
except:
    pass
# ----------------------------------------------------------------------- #
# get database service and open database
# ----------------------------------------------------------------------- #
dbSvc=cool.DatabaseSvcFactory.databaseService()
# database accessed via physical name
dbstring="sqlite://;schema=%s;dbname=%s" % (dbfile,dbname)
try:
    db=dbSvc.createDatabase(dbstring)
except Exception,e:
    print 'Problem creating database',e
    sys.exit(-1)

print "Created database",dbstring

# ----------------------------------------------------------------------- #
# setup a folder
# ----------------------------------------------------------------------- #
#spec=cool.RecordSpecification()
recspec = cool.RecordSpecification()
recspec.extend("version",cool.StorageType.String4k)
recspec.extend("syserrors",cool.StorageType.String4k)
spec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION,recspec,True)

#spec.extend(fdspec,cool.StorageType.String16M)
# folder meta-data - note for Athena this has a special meaning

desc='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'

####<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>

# create the folder - single version
# last argument is createParents - if true, automatically creates parent folders if needed
# note this will not work if the database already exists - delete mycool.db first
#folder=db.createFolder(fdname,spec,desc,cool.FolderVersioning.MULTI_VERSION,True)
folder=db.createFolder(fdname,spec,desc,True)

folder.createChannel( 0, "muonalignmenterrors", "muon alignment spectrometer errors" );

# ----------------------------------------------------------------------- #
# now fill in data - create a record and fill it
# ----------------------------------------------------------------------- #
data=cool.Record(recspec)

clob = "#This is an empty clob \n"
# FIRST COLUMN: for documentation/version
data['version']='V0: Test'
# SECOND COLUMN: contains the actual input
# reading from the ASCII file
infile = open('input.txt', 'r')
inputsys = infile.read()
infile.close()
data['syserrors']= inputsys #'# Empty Errors'
print "Will store this object: "
print data['syserrors']

chann=0
# store object with IOV valid from 0-max, channel 0
iov_min = 1335972949000000000 
iov_max = 1336005661000000000 # cool.ValidityKeyMax 
folder.storeObject(iov_min, cool.ValidityKeyMax, data,chann,Tag)
#
folder.storeObject(1336010821000000000, cool.ValidityKeyMax,data,chann,Tag) # run 202740
print 'created folder valid from %lf to %lf' % (iov_min, iov_max)
#-------------------

db.closeDatabase()
sys.exit()
