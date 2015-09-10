#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from PyCool import cool,coral
import cx_Oracle,sys
from collections import OrderedDict

dbfile="madqm.sqlite"
#dbname="COMP200"
dbname="CONDBR2"
#Tag = "MDT-GEO-ILINES-01"
#fdname='/MUONALIGN/ILINES'
fdname='/TRIGGER/HLT/MenuAwareDQMonitoring5'

# main script
#
# WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# OBSOLETE: Please use ilines2cool instead !
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
payloadSpec  = cool.RecordSpecification()
payloadSpec.extend("MCKname",cool.StorageType.String255)
payloadSpec.extend("MCKdata",cool.StorageType.UInt32)
#recspec.extend("geotag",cool.StorageType.String4k)
#recspec.extend("geoversion",cool.StorageType.String4k)
#spec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION,recspec,True)

folderMode=cool.FolderVersioning.SINGLE_VERSION
folderSpec = cool.FolderSpecification(folderMode, payloadSpec)

#spec=cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION,recspec,True)

#spec.extend(fdspec,cool.StorageType.String16M)
# folder meta-data - note for Athena this has a special meaning

#desc='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'

####<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>

# create the folder - single version
# last argument is createParents - if true, automatically creates parent folders if needed
# note this will not work if the database already exists - delete mycool.db first
#folder=db.createFolder(fdname,spec,desc,cool.FolderVersioning.MULTI_VERSION,True)

#folderDescr='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'

folderDescr='<timeStamp>run-lumi</timeStamp>'
folderDescr+='<addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader>'
# Use CondAttrListCollection if your filder will have several channels and AthenaAttributeList if your folder has only one channel
folderDescr+='<typeName>CondAttrListCollection</typeName>'
#folderDescr+='<typeName>AthenaAttributeList</typeName>'
#folderDescr=getAthenaFolderDescr()
folder=db.createFolder(fdname,folderSpec,folderDescr,True)

# ----------------------------------------------------------------------- #
# now fill in data - create a record and fill it
# ----------------------------------------------------------------------- #

data=cool.Record(payloadSpec)
name="MCK"
myMCK = 2 # MCK=2
# get a reference to the blob
data['MCKname']=name
data['MCKdata']=myMCK
#data['geotag']='none'
#data['geoversion']='Version R1'
#print "clob size ",len(data['data'])
print "data for ", data['MCKname']
print "Will store this object",data['MCKdata']

chann=1
#Tag=""
# store object with IOV valid from 0-max, channel 0
# folder.storeObject(0,cool.ValidityKeyMax,data,chann,Tag)
folder.storeObject(0,cool.ValidityKeyMax,data,chann)
#-------------------
data=cool.Record(payloadSpec)

#tk geotag='ISZT-R06.2011'
#geotag='some geotag'

myMCK = 3 # MCK=3
# get a reference to the blob
data['MCKname']=name
data['MCKdata']=myMCK
#print "clob size ",len(data['MCKdata'])
print "data for ", data['MCKname']
print "Will store this object",data['MCKdata']

# now we want to store some MCK for Run: 212967 and LB: 291

runNum=212967
lbkNum=291

# iov From Run Lumi(runNum, lbkNum):
# Returns COOL timeStamp built from run and lumi block numbers
my_iov = (int(runNum)<<32) + int(lbkNum)
my_cool_iov = cool.ValidityKey(my_iov)

#chann=1
# store object with IOV valid from 0-max, channel 0
# folder.storeObject(0,cool.ValidityKeyMax,data,chann,Tag)
#folder.storeObject(1293836400000000000,cool.ValidityKeyMax,data,chann,Tag)

print "min iov = ", my_cool_iov
print "max iov = ", cool.ValidityKeyMax

#folder.storeObject(1293836400000000000,cool.ValidityKeyMax,data,chann)

print "Storing object",data['MCKdata'] 
folder.storeObject(my_cool_iov,cool.ValidityKeyMax,data,chann)
print "Object stored"

# store IOV for Jiri's sample having MCK=4 (as example)
# looks like it has the same run number as before


db.closeDatabase()
sys.exit()
