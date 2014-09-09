#!/bin/env python
# ReadLUTFromCool.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2010-03-18

import getopt,sys,os,string


def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal CIS/FIT/NLN  from various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use OFL01 or OFL02 "
    print "-t, --tag=      specify tag to use, f.i. HLT-UPD1-00 or COM-02 "
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-p, --ros=      specify partition (ros number), default is 1"
    print "-d, --drawer=   specify drawer number, default is 0"
    print "-c, --channel=  specify channel number, default is 0"
    print "-g, -a, --adc=  specify gain (adc number), default is 0"
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/COMP200' or 'sqlite://;schema=tileSqlite.db;dbname=COMP200'"
    


letters = "hr:l:s:t:f:p:d:c:a:g:"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","ros=","drawer=","channel=","adc=","gain="]

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
folderPath =  "/TILE/OFL02/CALIB/CIS/FIT/NLN"
tag = "HLT-UPD1-00"
ros     = 1
drawer  = 0
channel = 0
adc     = 0

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/CALIB/CIS/FIT/NLN" % a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-p","--ros"):
        ros = int(a)
    elif o in ("-d","--drawer"):
        drawer = int(a)
    elif o in ("-c","--channel"):
        channel = int(a)
    elif o in ("-a","--adc","-g","--gain"):
        adc = int(a)
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ReadLUT")
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
log.info("Initializing ros %d, drawer %d for run %d, lumiblock %d" % (ros,drawer,run,lumi))
log.info("... %s" % blobReader.getComment((run,lumi)))
flt = blobReader.getDrawer(ros, drawer,(run,lumi))
maxidx = flt.getObjSizeUint32()
log.info( "Maxidx = %d" % maxidx )
log.info( "\n" )

#=== get float for a given ADC
modName = TileCalibUtils.getDrawerString(ros,drawer)
print ( "%s ch %i gn %i :" % ( modName, channel, adc ) )
for idx in xrange(0,maxidx):
    print ( " %2d  %f" % (idx, flt.getData(channel, adc, idx) ) )

#=== close DB
db.closeDatabase()

