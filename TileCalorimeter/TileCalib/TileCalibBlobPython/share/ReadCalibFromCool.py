#!/bin/env python
# ReadCalibFromCool.py
# Andrei Artamonov 2009-11-03

import getopt,sys,os,string


def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal calibrations from various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use f.i. /TILE/OFL02/CALIB/EMS or /TILE/OFL02/CALIB/CIS/FIT/LIN "
    print "-t, --tag=      specify tag to use, f.i. HLT-UPD1-01 or UPD4"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-n, --nval=     specify number of values to output, default is all"
    print "-g, -a, --adc=  specify adc to print or number of adcs to print with - sign, default is -2"
    print "-d, --default   print also default values stored in AUX01-AUX20 "
    print "-b, --blob      print additional blob info"
    print "-H, --hex       write frag id instead of module name"
    print "-p, --prefix=   write some prefix on every line "
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/COMP200' or 'sqlite://;schema=tileSqlite.db;dbname=COMP200'"
    


letters = "hr:l:s:t:f:n:a:g:p:dbH"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","nval=","adc=","gain=","prefix=","default","blob","hex"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetOptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 2147483647
lumi = 0
schema = 'COOLOFL_TILE/COMP200'
folderPath =  "/TILE/OFL02/CALIB/CIS/FIT/LIN"
tag = "HLT-UPD1-00"
nval = 0
nadc = -2
rosmin = 1
blob = False
hexid = False
prefix = None

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-n","--nval"):
        nval = int(a)
    elif o in ("-a","--adc","-g","--gain"):
        nadc = int(a)
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-d","--default"):
        rosmin = 0
    elif o in ("-b","--blob"):
        blob = True
    elif o in ("-H","--hex"):
        hexid = True
    elif o in ("-p","--prefix"):
        prefix = a
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ReadCalibFromCool")
import logging
log.setLevel(logging.DEBUG)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')


if "/TILE/ONL01" in folderPath:
    folderTag = ""
elif tag == "UPD4":
    sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
    from AtlCoolBKLib import resolveAlias
    gtagUPD4 = resolveAlias.getCurrent().replace('*','')
    log.info("global tag: %s" % gtagUPD4)
    folderTag = TileCalibTools.getFolderTag(db, folderPath, gtagUPD4 )
else:
    folderTag = TileCalibUtils.getFullTag(folderPath, tag )


log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== initialize blob reader
blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
#blobReader.log().setLevel(logging.DEBUG)

#=== get drawer with status at given run
log.info("Initializing for run %d, lumiblock %d" % (run,lumi))
log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

if nadc<0:
    a1=0
    a2=-nadc
else:
    a1=nadc
    a2=nadc+1

#=== loop over all partitions,modules,channels
for ros in xrange(rosmin,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        if hexid:
            modName = "0x%x" % ((ros<<8)+mod)
        else:
            modName = TileCalibUtils.getDrawerString(ros,mod)
        if prefix:
            modName = prefix + " " + modName
        try:
            flt = blobReader.getDrawer(ros, mod,(run,lumi))
            if blob: 
                print "%s blob type %d version %d size %d" % (modName, flt.getObjType(), flt.getObjVersion(), flt.getObjSizeUint32())
            if nval<1:
                mval = flt.getObjSizeUint32()
            else:
                mval = nval
            for chn in xrange(TileCalibUtils.max_chan()):
                for adc in xrange(a1,a2):
                    msg = "%s %2i %1i  " % ( modName, chn, adc )
                    for val in xrange(0,mval):
                        msg += "  %f" % flt.getData(chn, adc, val)
                    print msg
        except Exception, e:
            print e

#=== close DB
db.closeDatabase()

