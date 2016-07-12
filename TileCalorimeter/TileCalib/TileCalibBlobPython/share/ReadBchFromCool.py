#!/bin/env python
# ReadBchFromCool.py  --schema='COOLOFL_TILE/CONDBR2'  --folder='OFL02' --tag='UPD4'
# Sanya Solodkov 2011-07-15
# change: Yuri Smirnov 2015-08-29, correction for EB* modules' nbad counter

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal status bits from various schemas / folders"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use ONL01 or OFL02, don't need to specify full path"
    print "-t, --tag=      specify tag to use, f.i. UPD1 or UPD4 or full suffix like RUN2-HLT-UPD1-00"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-d, --default   print also default values stored in AUX01-AUX20 "
    print "-b, --blob      print additional blob info"
    print "-H, --hex       print frag id instead of module name"
    print "-P, --pmt       print pmt number in addition to channel number"
    print "-s, --schema=   specify schema to use, like 'COOLOFL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2' or tileSqlite.db"
    print "-D, --dbname=   specify dbname part of schema if schema only contains file name, default is CONDBR2'"
    print "-w, --warning   suppress warning messages about missing drawers in DB"
    
letters = "hr:l:s:t:f:D:dbHPw"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","dbname=","default","blob","hex","pmt","warning"]

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
folderPath =  "/TILE/OFL02/STATUS/ADC"
tag = "UPD4"
rosmin = 1
blob = False
hexid = False
pmt = False
warn = 1

for o, a in opts:
    if o in ("-f","--folder"):
        if '/TILE' in a: folderPath = a
        else: folderPath = "/TILE/%s/STATUS/ADC" % a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-D","--dbname"):
        dbname = a
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
    elif o in ("-w","--warning"):
        warn = -1
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ReadBch")
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

if not 'sqlite:' in schema and len(dbname):
    schema=schema.replace('CONDBR2',dbname)
    schema=schema.replace('COMP200',dbname)

if len(tag)==0:
    folderPath='/TILE/ONL01/STATUS/ADC'
    if 'COOLOFL' in schema:
        if 'COMP200' in schema: schema='COOLONL_TILE/COMP200'
        if 'CONDBR2' in schema: schema='COOLONL_TILE/CONDBR2'
    log.warning("tag is empty, using %s folder and schema %s" % (folderPath,schema))

if schema=='COOLONL_TILE/COMP200':
    if folderPath!="/TILE/ONL01/STATUS/ADC" and folderPath!="/TILE/OFL01/STATUS/ADC":
        log.warning("Folder %s doesn't exist in schema %s" % (folderPath,schema)) 
        folderPath="/TILE/ONL01/STATUS/ADC"
        log.warning("Changing folder to %s" % folderPath)

if schema=='COOLONL_TILE/CONDBR2':
    if folderPath!="/TILE/ONL01/STATUS/ADC":
        log.warning("Folder %s doesn't exist in schema %s " % (folderPath,schema))
        folderPath="/TILE/ONL01/STATUS/ADC"
        log.warning("Changing folder to %s" % folderPath)

if schema=='COOLOFL_TILE/COMP200' or schema=='COOLOFL_TILE/CONDBR2':
    if folderPath!="/TILE/OFL02/STATUS/ADC":
        log.warning("Folder %s doesn't exist in schema %s " % (folderPath,schema)) 
        folderPath="/TILE/OFL02/STATUS/ADC"
        log.warning("Changing folder to %s" % folderPath)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')
folderTag = TileCalibTools.getFolderTag(db if 'CONDBR2' in schema else schema, folderPath, tag)
log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== create bad channel manager
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logLevel)
mgr.initialize(db, folderPath, folderTag, (run,lumi), warn)
if warn<0: 
    reader = TileCalibTools.TileBlobReader(db,folderPath,folderTag)

#=== Dump the current isBad definition
isBadDef = mgr.getAdcProblems(0, TileCalibUtils.definitions_draweridx(), TileCalibUtils.bad_definition_chan(), 0)
if len(isBadDef.keys()):
    log.info( "isBad Definition: " )
    for prbCode in sorted(isBadDef.keys()):
        prbDesc = isBadDef[prbCode]
        msg = "- %2i (%s)" % (prbCode,prbDesc)
        log.info( msg )
#=== Dump the current isBadTiming definition
isBadTimingDef = mgr.getAdcProblems(0, TileCalibUtils.definitions_draweridx(), TileCalibUtils.badtiming_definition_chan(), 0)
if len(isBadTimingDef.keys()):
    log.info( "isBadTiming Definition: " )
    for prbCode in sorted(isBadTimingDef.keys()):
        prbDesc = isBadTimingDef[prbCode]
        msg = "- %2i (%s)" % (prbCode,prbDesc)
        log.info( msg )
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
gname = [ "LG", "HG" ]

#=== Get ADC problems

#=== get the channel status (if it is bad and will be masked)
#=== the channel status depends on the definition of isBad stored
#=== in the database drawer 1, channel 0
#=== isAffected = has a problem not included in isBad definition
#=== isGood = has no problem at all

modOk = False
miss  = 0
good  = 0
aff   = 0
bad   = 0
for ros in xrange(rosmin,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        if hexid:
            modName = "0x%x" % ((ros<<8)+mod)
        else:
            modName = TileCalibUtils.getDrawerString(ros,mod)
        if warn<0:
            bch = reader.getDrawer(ros, mod, (run,lumi), False, False)
            if bch is None:
                modOk = False
                miss+=1
                #print "%s is missing in DB" % modName
            else:
                modOk = True
                if blob and bch:
                    print "%s  Blob type: %d  Version: %d  Nchannels: %d  Ngains: %d  Nval: %d" % (modName, bch.getObjType(), bch.getObjVersion(), bch.getNChans(), bch.getNGains(), bch.getObjSizeUint32())
        nBad=0
        for chn in xrange(TileCalibUtils.max_chan()):
            chnName = " %2i" % chn
            for adc in xrange(TileCalibUtils.max_gain()):

                stat = mgr.getAdcStatus(ros,mod,chn,adc)
                #log.info( "- ADC status = isBad:      %d" % stat.isBad()      )
                #log.info( "- ADC status = isGood:     %d" % stat.isGood()     )
                #log.info( "- ADC status = isAffected: %d" % stat.isAffected() )

                #=== get all problems of the channel
                prbs = mgr.getAdcProblems(ros,mod,chn,adc)
                #log.info( "ADC Problems: " )
                if len(prbs):
                    modOk = False
                    if pmt:
                        msg = "%s pm %02i ch %02i %s " % ( modName, abs(ch2pmt[ros][chn]), chn, gname[adc] )
                    else:
                        msg = "%s %2i %1i " % ( modName,chn,adc )
                    for prbCode in sorted(prbs.keys()):
                        prbDesc = prbs[prbCode]
                        msg += " %5i (%s)" % (prbCode,prbDesc)
                    if stat.isBad():
                        msg += "  => BAD"
                        nBad+=1
                    elif stat.isAffected():
                        msg += "  => Affected"
                    elif stat.isGood():
                        msg += "  => good"
                    print msg
        if modOk:
            good+=1
            print "%s ALL GOOD" % (modName)
        elif nBad==0:
            aff+=1
        elif nBad==TileCalibUtils.max_gain()*TileCalibUtils.max_chan() or (nBad==90 and 'LB' in modName) or (nBad==64 and 'EB'in modName) or (nBad==60 and 'EBA15' in modName) or (nBad==60 and 'EBC18' in modName):
            bad+=1
if warn<0: 
    if miss: print "%3i drawers are missing in DB" % miss
    print "%3i drawers are absolutely good" % good
    print "%3i drawers have good and affected channels" % (aff-miss)
    print "%3i drawers have some bad channels" % ((256 if rosmin else 276)-good-bad-aff)
    print "%3i drawers are completely bad" % bad
#=== print all bad channels
#log.info("listing bad channels")
#mgr.listBadAdcs()

#=== close DB
db.closeDatabase()

