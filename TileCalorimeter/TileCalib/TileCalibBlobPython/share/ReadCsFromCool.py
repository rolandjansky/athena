#!/bin/env python
# ReadCsFromCool.py -h
# Sanya Solodkov 2011-07-15

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal cesium from various schemas / folders / tags"
    print ""
    print "-h, --help     shows this help"
    print "-f, --folder=  specify status folder to use ONL01 or OFL02 "
    print "-t, --tag=     specify tag to use, f.i. UPD1 or UPD4 or full suffix like RUN2-HLT-UPD1-01"
    print "-r, --run=     specify run  number, by default uses latest iov"
    print "-l, --lumi=    specify lumi block number, default is 0"
    print "-n, --nval=    specify number of values to output, default is 3"
    print "-s, --schema=  specify schema to use, like 'COOLOFL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"

letters = "hr:l:s:t:f:n:"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","nval="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetOptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 2147483647
lumi = 0
schema = 'COOLOFL_TILE/CONDBR2'
folderPath =  "/TILE/OFL02/CALIB/CES"
tag = "UPD4"
nval = 3

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/CALIB/CES" % a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-n","--nval"):
        nval = int(a)
    elif o in ("-r","--run"):
        run = int(a) 
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

        
if schema=='COOLONL_TILE/COMP200':
    if folderPath!="/TILE/ONL01/CALIB/CES" and folderPath!="/TILE/OFL01/CALIB/CES":
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLONL_TILE/CONDBR2':
    if folderPath!="/TILE/ONL01/CALIB/CES":
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLOFL_TILE/COMP200' or schema=='COOLOFL_TILE/CONDBR2':
    if folderPath!="/TILE/OFL02/CALIB/CES":
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ReadCs")
import logging
log.setLevel(logging.DEBUG)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')
folderTag = TileCalibTools.getFolderTag(db, folderPath, tag)
log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== initialize blob reader
blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
#blobReader.log().setLevel(logging.DEBUG)

#=== get drawer with status at given run
log.info("Initializing for run %d, lumiblock %d" % (run,lumi))
log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

#=== loop over all partitions,modules,channels
for ros in xrange(1,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        modName = TileCalibUtils.getDrawerString(ros,mod)
        #log.info("ros %d, drawer %s at run %d" % (ros, modName, run))
        flt = blobReader.getDrawer(ros, mod,(run,lumi))
        if nval<1:
            mval = flt.getObjSizeUint32()
        else:
            mval = nval
        for chn in xrange(TileCalibUtils.max_chan()):
            msg = "%s %2i  " % ( modName, chn )
            for val in xrange(0,mval):
                msg += "  %f" % flt.getData(chn, 0, val)
            print msg

#=== close DB
db.closeDatabase()

