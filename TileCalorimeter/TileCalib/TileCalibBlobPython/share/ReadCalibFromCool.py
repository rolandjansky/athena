#!/bin/env python
# ReadCalibFromCool.py
# Andrei Artamonov 2009-11-03

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal constants from various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN "
    print "-t, --tag=      specify tag to use, f.i. UPD1 or UPD4 or full suffix like RUN2-HLT-UPD1-00"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-n, --nval=     specify number of values to output, default is all"
    print "-g, -a, --adc=  specify adc to print or number of adcs to print with - sign, default is -2"
    print "-d, --default   print also default values stored in AUX01-AUX20 "
    print "-b, --blob      print additional blob info"
    print "-H, --hex       write frag id instead of module name"
    print "-p, --prefix=   write some prefix on every line "
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    
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
schema = 'COOLOFL_TILE/CONDBR2'
dbname = ''
folderPath =  "/TILE/OFL02/CALIB/CIS/LIN"
tag = "UPD4"
nval = 0
nadc = -99
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
    elif o in ("-D","--dbname"):
        dbname = a
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
logLevel=logging.DEBUG
log.setLevel(logLevel)
log1 = getLogger("TileCalibTools")
log1.setLevel(logLevel)


#=== check parameters
if len(dbname)<7 and run!=2147483647:
    dbname = 'COMP200' if run<232000 else 'CONDBR2'

if not 'COOLO' in schema and not ':' in schema and not ';' in schema:
    schema='sqlite://;schema='+schema+';dbname='+(dbname if len(dbname) else 'CONDBR2')

if schema=='COOLONL_TILE/COMP200':
    if not '/TILE/ONL01' in folderPath and not '/TILE/OFL01' in folderPath:
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLONL_TILE/CONDBR2':
    if not '/TILE/ONL01' in folderPath:
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLOFL_TILE/COMP200' or schema=='COOLOFL_TILE/CONDBR2':
    if not '/TILE/OFL02' in folderPath:
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')
folderTag = TileCalibTools.getFolderTag(schema if 'COMP200' in schema else db, folderPath, tag)
log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== initialize blob reader
blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
#blobReader.log().setLevel(logging.DEBUG)

#=== get drawer with status at given run
log.info("Initializing for run %d, lumiblock %d" % (run,lumi))
flt=None
r=5
d=0
nchan=TileCalibUtils.max_chan()
while not flt:
    d-=1
    if d<0:
        r-=1
        if r<0: break
        d=TileCalibUtils.getMaxDrawer(r)-1
    flt = blobReader.getDrawer(r, d, (run,lumi), False, False)
if flt:
    blobT=flt.getObjType()
    blobV=flt.getObjVersion()
    mchan=flt.getNChans()
    mgain=flt.getNGains()
    mval=flt.getObjSizeUint32()
    log.info( "Blob type: %d  Version: %d  Nchannels: %d  Ngains: %d  Nval: %d" % (blobT,blobV,mchan,mgain,mval) )
    if nadc<-mgain: nadc=-mgain
    if nchan<mchan: nchan=mchan
else:
    if nadc==-99: nadc=-TileCalibUtils.max_gain()

log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

if nadc<0:
    a1=0
    a2=-nadc
else:
    a1=nadc
    a2=nadc+1

#=== loop over all partitions,modules,channels
miss=0
good=0
for ros in xrange(rosmin,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        if hexid:
            modName = "0x%x" % ((ros<<8)+mod)
        else:
            modName = TileCalibUtils.getDrawerString(ros,mod)
        if prefix:
            modName = prefix + " " + modName
        try:
            flt = blobReader.getDrawer(ros, mod,(run,lumi), False, False)
            if flt is None:
                miss+=1
                print "%s is missing in DB" % modName
            else:
                good+=1
                if blob:
                    print "%s  Blob type: %d  Version: %d  Nchannels: %d  Ngains: %d  Nval: %d" % (modName, flt.getObjType(), flt.getObjVersion(), flt.getNChans(), flt.getNGains(), flt.getObjSizeUint32())
                if nval<1:
                    mval = flt.getObjSizeUint32()
                else:
                    mval = nval
                a3=flt.getNGains()
                a4=(a3 if a3<a2 else a2)
                for chn in xrange(flt.getNChans()):
                    for adc in xrange(a1,a4):
                        msg = "%s %2i %1i  " % ( modName, chn, adc )
                        for val in xrange(0,mval):
                            msg += "  %f" % flt.getData(chn, adc, val)
                        print msg
        except Exception, e:
            print e
if miss: 
    if run!=2147483647:
        print "%3i drawers are preseint in DB for run %d lb %d" % (good,run,lumi)
        print "%3i drawers are missing  in DB for run %d lb %d" % (miss,run,lumi)
    else:
        print "%3i drawers are preseint in DB" % (good)
        print "%3i drawers are missing  in DB" % (miss)
    if good==0:
        print "Please, check that you are using correct schema and correct tag"

#=== close DB
db.closeDatabase()

