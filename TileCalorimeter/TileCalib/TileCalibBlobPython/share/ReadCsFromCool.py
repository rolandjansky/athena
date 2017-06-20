#!/bin/env python
# ReadCsFromCool.py -h
# Sanya Solodkov 2011-07-15

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal cesium from various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use ONL01 or OFL02 or full path /TILE/OFL02/CALIB/CES"
    print "-t, --tag=      specify tag to use, f.i. UPD1 or UPD4 or full suffix like RUN2-HLT-UPD1-01"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-n, --nval=     specify number of values to output, default is all"
    print "-g, -a, --adc=  specify adc(gain) to print default is 0"
    print "-d, --default   print also default values stored in AUX01-AUX20 "
    print "-b, --blob      print additional blob info"
    print "-H, --hex       print frag id instead of module name"
    print "-P, --pmt       print pmt number in addition to channel number"
    print "-p, --prefix=   print some prefix on every line "
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2' or tileSqlite.db"
    print "-D, --dbname=   specify dbname part of schema if schema only contains file name, default is CONDBR2'"

letters = "hr:l:s:t:f:D:n:a:g:p:dbHP"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","dbname=","nval=","adc=","gain=","prefix=","default","blob","hex","pmt"]

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
folderPath =  "/TILE/OFL02/CALIB/CES"
tag = "UPD4"
nval = 0
nadc = 0
rosmin = 1
blob = False
hexid = False
pmt = True
prefix = None

for o, a in opts:
    if o in ("-f","--folder"):
        if a=='ONL01' or a=='OFL01' or a=='OFL02':
            folderPath = "/TILE/%s/CALIB/CES" % a
        else:
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
    elif o in ("-P","--pmt"):
        pmt = True
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
log = getLogger("ReadCsFromCool")
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
    if not (folderPath.startswith('/TILE/ONL01/') or folderPath.startswith('/TILE/OFL01/')):
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLONL_TILE/CONDBR2':
    if not folderPath.startswith('/TILE/ONL01/'):
        print "Folder %s doesn't exist in schema %s, only /TILE/ONL01 " % (folderPath,schema) 
        sys.exit(2)
        
if schema=='COOLOFL_TILE/COMP200' or schema=='COOLOFL_TILE/CONDBR2':
    if not folderPath.startswith('/TILE/OFL02/'):
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')
folderTag = TileCalibTools.getFolderTag(schema if 'COMP200' in schema or 'OFLP200' in schema else db, folderPath, tag)
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
    if nadc>=mgain or nadc<0: nadc=mgain-1
else:
    mgain=1
    if nadc!=0: nadc=1
gname=[]
if mgain!=2: 
    for i in xrange(mgain+1): gname+=[ "g "+str(i) ]
else: gname = [ 'Low gain','High gain' ]
log.info( "Using gain %d - %s"%(nadc,gname[nadc]) )

log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

##channel2pmt
##negative means not connected !
##
dummy = [0]*48
barrel = [ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
          27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34,
          39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46]
extbar = [ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
          13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,
         -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34,
          44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]

ch2pmt = [ dummy, barrel, barrel, extbar, extbar ]

#=== loop over all partitions,modules,channels
cnt=0
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
            if flt is None or isinstance(flt, (int)):
                if rosmin==0:
                    print "%s is missing in DB" % modName
                    cnt+=1
            elif nadc<flt.getNGains():
                if blob:
                    print "%s  Blob type: %d  Version: %d  Nchannels: %d  Ngains: %d  Nval: %d" % (modName, flt.getObjType(), flt.getObjVersion(), flt.getNChans(), flt.getNGains(), flt.getObjSizeUint32())
                mval0 = 0
                if nval<1:
                    mval = flt.getObjSizeUint32()
                    if nval<0 and -nval<mval:
                        mval=-nval
                        mval0=mval-1
                else:
                    mval = nval
                for chn in xrange(flt.getNChans()):
                    msg = "%s pm %02i ch %02i " % ( modName, abs(ch2pmt[ros][chn]), chn )
                    for val in xrange(mval0,mval):
                        msg += "  %f" % flt.getData(chn, nadc, val)
                    print msg
                    cnt+=1
            else:
                print "%s gain %d not found" % (modName,nadc)
                cnt+=1
        except Exception, e:
            print e

if cnt==0:
    print "Nothing to dump"

#=== close DB
db.closeDatabase()

