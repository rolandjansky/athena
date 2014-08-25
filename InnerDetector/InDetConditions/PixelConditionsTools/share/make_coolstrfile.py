#!/bin/env python
# make_coolstrfile.py <connect_string> <folder>
# create a the folder <folder> in the database identified by <connect_string>
# such that it can be used to store CLOB (technology=1) objects using the
# Athena CoolStrFileSvc (in DetectorDescription/DetDescrCond/DetDescrCondTools)
#   creates the folder structure
#     - columns tech (int), file (string) and data (CLOB up to 16MB)
#   sets the folder description for an Athena CondMultiChanCollection
#   accessed by run/event
#
# Richard Hawkings, 24/9/05, from example by Sven Schmidt

import sys
import traceback

from PyCool import cool

if len(sys.argv) < 3:
    print 
    print 'usage:', sys.argv[0], '<connect string> <folder>'
    print '<connect string>: a COOL (RAL) compatible connect string, e.g.'
    print ( '    "oracle://devdb10;schema=atlas_cool_sas;'
            'user=atlas_cool_sas;dbname=COOLTEST;password=xxxx"' )
    print '<folder>: full pathname of the COOL folder'
    sys.exit(-1)
    
connect = sys.argv[1]
foldername=sys.argv[2]
    
dbSvc = cool.DatabaseSvcFactory.databaseService()

try:
    db = dbSvc.openDatabase(connect)
except Exception, e:
    print e
    print "could not connect to the database"
# Try to create if not exist
    print " Try to Create a new conditions database: "
    try: 
        db = dbSvc.createDatabase(connect);
    except Exception, ex:
        print ex
        print "Coudn't create a new conditions database: "
        sys.exit(-1)

    try:
        db = dbSvc.openDatabase(connect)
    except Exception, ey:
        print ey
        print "still could not connect to the database"
        print "check your seal.opts and authentication.xml"
        sys.exit(-1)
        
    except:
        print "Unexpected error:", sys.exc_info()[0]
        print traceback.print_tb(sys.exc_info()[2])
        sys.exit(-1)

#        
except:
    print "Unexpected error:", sys.exc_info()[0]
    print traceback.print_tb(sys.exc_info()[2])
    sys.exit(-1)

print "Make folder: ",foldername
# setup extended attributelist
extSpec = cool.ExtendedAttributeListSpecification()
#extSpec.push_back('tech','int')
#extSpec.push_back('file','string')
extSpec.push_back('data','string','16M')

newdesc='<timeStamp>run-event</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'

try:
    folder=db.dropNode(foldername)
    
except Exception, e:
    print e
    print "deleteFolder failed"
    sys.exit(-1)

try:
    folder=db.createFolderExtended(foldername,extSpec,newdesc,
                                   cool.FolderVersioning.MULTI_VERSION,True)
except Exception, e:
    print e
    print "createFolderExtended failed"
    sys.exit(-1)

print "Current folder string: ",folder.description()

print "All done - folder: ",foldername, " created"

