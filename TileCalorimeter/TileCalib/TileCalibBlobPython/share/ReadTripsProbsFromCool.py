#!/bin/env python
# ./ReadTripsProbsFromCool.py  --schema='COOLOFL_TILE/OFLP200'  --folder='OFL02' --tag='UPD4'

import getopt,sys,os,string

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal drawer trips probabilities from various schemas / folders"
    print ""
    print "-h, --help     shows this help"
    print "-f, --folder=  specify status folder to use OFL01 or OFL02, by default OFL01 "
    print "-t, --tag=     specify tag to use, f.i. UPD1 or UPD4"
    print "-r, --run=     specify run  number, by default uses latest iov"
    print "-l, --lumi=    specify lumi block number, default is 0"
    print "-s, --schema=  specify schema to use, like 'COOLONL_TILE/OFLP200' or 'sqlite://;schema=tileSqlite.db;dbname=OFLP200'"
    


letters = "hr:l:s:t:f:"
keywords = ["help","run=","lumi=","schema=","tag=","folder="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetOptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 2147483647
lumi = 0
schema = 'COOLOFL_TILE/OFLP200'
folderPath =  "/TILE/OFL01/STATUS/ADC"
tag = "UPD1"

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/STATUS/ADC" % a 
    elif  o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-r","--run"):
        run = int(a) 
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandled option"



from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ReadTripsProbs")
import logging
logLevel=logging.DEBUG
log.setLevel(logLevel)
log1 = getLogger("TileCalibTools")
log1.setLevel(logLevel)


#=== set database
db = TileCalibTools.openDbConn(schema, 'READONLY')

if tag == "UPD4":
    sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
    from AtlCoolBKLib import resolveAlias
    gtagUPD4 = resolveAlias.getCurrent().replace('*','')
    log.info("global tag: %s" % gtagUPD4)
    folderTag = TileCalibTools.getFolderTag(db, folderPath, gtagUPD4 )
else:
    folderTag = TileCalibUtils.getFullTag(folderPath, tag )


log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

blobReader = TileCalibTools.TileBlobReader(db, folderPath, folderTag)

util = PyCintex.gbl.TileCalibUtils()
tripsCalibDrawer = blobReader.getDrawer(util.trips_ros(), util.trips_drawer(), (run,lumi))

if tripsCalibDrawer.getNChans() != util.max_ros() \
   or tripsCalibDrawer.getObjSizeUint32() != (util.max_drawer() + 1):
    log.info("There no drawer trips probabilities in tag %s" % (folderTag))
    sys.exit(2)


print ""
print "Module\tTrip (probability)"
print ""

for ros in xrange(1, util.max_ros()):
    denominator = tripsCalibDrawer.getData(ros, 0, util.max_drawer())
    for mod in xrange(0, min(64, TileCalibUtils.getMaxDrawer(ros))):
        modName = TileCalibUtils.getDrawerString(ros, mod)
        trip = tripsCalibDrawer.getData(ros, 0, mod)
        #log.info("%s\t%s" % (modName, str(float(trip)/ denominator)))
        print "%s\t%s" % (modName, str(float(trip)/ denominator))


#=== close DB
db.closeDatabase()

