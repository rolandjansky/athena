#!/bin/env python
# ./ReadBchFromCool.py  --schema='COOLOFL_TILE/COMP200'  --folder='OFL02' --tag='UPD4'
# Sanya Solodkov 2011-07-15

import getopt,sys,os,string


def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal status bits from various schemas / folders"
    print ""
    print "-h, --help     shows this help"
    print "-f, --folder=  specify status folder to use ONL01, OFL01 or OFL02 "
    print "-t, --tag=     specify tag to use, f.i. UPD1 or UPD4"
    print "-r, --run=     specify run  number, by default uses latest iov"
    print "-l, --lumi=    specify lumi block number, default is 0"
    print "-s, --schema=  specify schema to use, like 'COOLONL_TILE/COMP200' or 'sqlite://;schema=tileSqlite.db;dbname=COMP200'"
    


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
schema = 'COOLOFL_TILE/COMP200'
folderPath =  "/TILE/OFL02/STATUS/ADC"
tag = "UPD1"

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/STATUS/ADC" % a
    elif o in ("-t","--tag"):
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
        assert False, "unhandeled option"


if schema=='COOLONL_TILE/COMP200':
    if folderPath!="/TILE/ONL01/STATUS/ADC" and folderPath!="/TILE/OFL01/STATUS/ADC":
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)

if schema=='COOLOFL_TILE/COMP200':
    if folderPath!="/TILE/OFL02/STATUS/ADC":
        print "Folder %s doesn't exist in schema %s " % (folderPath,schema) 
        sys.exit(2)


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


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')


if "/TILE/ONL01" in folderPath:
    folderTag = ""
elif "/TILE/OFL01" in folderPath:
    folderTag = TileCalibUtils.getFullTag(folderPath, "HLT-UPD1-00")
elif "/TILE/OFL02" in folderPath:
    if tag == "UPD1":
        folderTag = TileCalibUtils.getFullTag(folderPath, "HLT-UPD1-00" )
    elif tag == "UPD4":
        sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
        from AtlCoolBKLib import resolveAlias
        gtagUPD4 = resolveAlias.getCurrent().replace('*','')
        log.info("global tag: %s" % gtagUPD4)
        folderTag = TileCalibTools.getFolderTag(db, folderPath, gtagUPD4 )
    else:
        folderTag = TileCalibUtils.getFullTag(folderPath, tag)
else:
    print "Unknown folder %s " % folderPath 
    sys.exit(2)


log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== create bad channel manager
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logLevel)
mgr.initialize(db, folderPath, folderTag, (run,lumi))

#=== Dump the current isBad definition
isBadDef = mgr.getAdcProblems(0,1,0,0)
log.info( "isBad Definition: " )
for prbCode in sorted(isBadDef.keys()):
    prbDesc = isBadDef[prbCode]
    msg = "- %2i (%s)" % (prbCode,prbDesc)
    log.info( msg )
log.info( "\n" )

#=== Get ADC problems

#=== get the channel status (if it is bad and will be masked)
#=== the channel status depends on the definition of isBad stored
#=== in the database drawer 1, channel 0
#=== isAffected = has a problem not included in isBad definition
#=== isGood = has no problem at all

for ros in xrange(1,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        modName = TileCalibUtils.getDrawerString(ros,mod)
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
                    msg = "%s %2i %1i " % (modName,chn,adc)
                    for prbCode in sorted(prbs.keys()):
                        prbDesc = prbs[prbCode]
                        msg += " %5i (%s)" % (prbCode,prbDesc)
                    if stat.isBad():
                        msg += "  => BAD"
                    elif stat.isAffected():
                        msg += "  => Affected"
                    elif stat.isGood():
                        msg += "  => good"
                    print msg

#=== print all bad channels
#log.info("listing bad channels")
#mgr.listBadAdcs()

#=== close DB
db.closeDatabase()

