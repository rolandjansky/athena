#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadBadBitsFromCool.py   --schema='COOLOFL_TILE/CONDBR2'  --folder='OFL02' --tag='UPD4'
# Sanya Solodkov 2011-07-15

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
    print "-s, --schema=   specify schema to use, like 'COOLOFL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2' or tileSqlite.db"
    print "-D, --dbname=   specify dbname part of schema if schema only contains file name, default is CONDBR2'"
    print "-w, --warning   suppress warning messages about missing drawers in DB"
    
letters = "hr:l:s:t:f:D:w"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","dbname=","warning"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 2147483647
lumi = 0
schema = 'COOLOFL_TILE/CONDBR2'
dbname = 'CONDBR2'
folderPath =  "/TILE/OFL02/STATUS/ADC"
tag = "UPD4"
warn = 1

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/STATUS/ADC" % a
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
log = getLogger("ReadBadBits")
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
for ros in xrange(0,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        modName = TileCalibUtils.getDrawerString(ros,mod)
        flt = blobReader.getDrawer(ros, mod,(run,lumi))
        for chn in xrange(TileCalibUtils.max_chan()):
            adcLG = flt.getData(chn, 0, 0)
            adcHG = flt.getData(chn, 1, 0)
            chanS = flt.getData(chn, 2, 0)
            if adcLG or adcHG or chanS:
                msg = "%s %2i   %5i  %5i  %5i " % ( modName, chn, adcLG, adcHG, chanS)
                print msg

#=== close DB
db.closeDatabase()

