#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
import getopt,sys,os,string
 
def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Write the TileCal drawer trips probabilities from ASCII file into local sqlite file"
    print "ASCII file format:"
    print "#CondId  |  frag  |  trip probability"
    print "Trip    0x100    0.01"
    print ""
    print "-h, --help     shows this help"
    print "-t, --tag=     specify tag to use, by default TEST-00"
    print "-i, --input=   input ASCII file, by default Tile.trips"
    print "-f, --folder=  specify status folder to use OFL01 or OFL02, by default OFL01 "


letters = "ht:i:f:"
keywords = ["help", "tag=", "input=","folder="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:], letters, keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)



folderPath =  "/TILE/OFL01/STATUS/ADC"
tag = "TEST-00"
fileName = "Tile.trips"

for o, a in opts:
    if o in ("-i","--input"):
        fileName = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-f","--folder"):
        folderPath = "/TILE/%s/STATUS/ADC" % a    
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandled option"

 
import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeTripsProbs")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillTripsProbs(fileTrips, folderPath, tag, since
                   , until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):
    

    #=== get full folder tag
    folderTag = TileCalibUtils.getFullTag(folderPath, tag)

    util = cppyy.gbl.TileCalibUtils()
    
    default = cppyy.gbl.std.vector('unsigned int')()
    for i in xrange(util.max_drawer() + 1):
        default.push_back( 0 )

    defVec = cppyy.gbl.std.vector('std::vector<unsigned int>')()  
    defVec.push_back(default)
    
    #=====================================================
    #=== fill 
    #=====================================================
    writer = TileCalibTools.TileBlobWriter(db, folderPath, 'Bch')
   
    precisions = [[0 for drawer in xrange(util.max_drawer())] for ros in xrange(util.max_ros())]
    trips = [[0 for drawer in xrange(util.max_drawer())] for ros in xrange(util.max_ros())]

    parser = TileCalibTools.TileASCIIParser3(fileTrips, "Trip");
    dict = parser.getDict()
    log.info("Updating dictionary from file with %i entries" % len(dict))
    log.info("... filename: %s" % fileTrips )
    for key, trip in dict.iteritems():
        ros = key[0]
        mod = key[1]
        precisions[ros][mod] = len(trip[0]) - 2
        trips[ros][mod] = float(trip[0])

    
    tripsCalibDrawer = writer.getDrawer(util.trips_ros(), util.trips_drawer())
    tripsCalibDrawer.init(defVec, util.max_ros(), 1)

    for ros in xrange(util.max_ros()):
        denominator = 10**max(precisions[ros])
        for mod in xrange(util.max_drawer()):
            trip = int(trips[ros][mod] * denominator)
            tripsCalibDrawer.setData(ros, 0, mod, trip)
        tripsCalibDrawer.setData(ros, 0, util.max_drawer(), denominator)



    #=== register in DB
    writer.register(since, until, folderTag)

    
#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'OFLP200', 'UPDATE')

#=== source data
runfrom = 0
fillTripsProbs(fileName, folderPath, tag, (runfrom,0))

#=== close the database connection
db.closeDatabase()

   
    
    
