#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadOfcFromCool.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-07-25

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal OFC values various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify status folder to use CIS, LAS, PHY "
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-p, --ros=      specify partition (ros number), default is 1"
    print "-d, --drawer=   specify drawer number, default is 0"
    print "-c, --channel=  specify channel number, default is 0"
    print "-g, -a, --adc=  specify gain (adc number), default is 0"
    print "-i, --field=    specify field number, default is 0"
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    
letters = "hr:l:s:t:f:p:d:c:a:g:i:"
keywords = ["help","run=","lumi=","schema=","tag=","folder=","ros=","drawer=","channel=","adc=","gain=","field="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 2147483647
lumi = 0
schema = 'COOLONL_TILE/CONDBR2'
folderPath =  "/TILE/ONL01/FILTER/OF2/PHY"
tag = ""
ros     = 1
drawer  = 0
channel = 0
adc     = 0
field   = 0

for o, a in opts:
    if o in ("-f","--folder"):
        if a.startswith("/TILE"): folderPath = a
        elif a.startswith("OF"): folderPath = "/TILE/ONL01/FILTER/%s" % a
        else: folderPath = "/TILE/ONL01/FILTER/OF2/%s" % a
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
    elif o in ("-i","--field"):
        field = int(a)
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
log = getLogger("ReadOfc")
import logging
log.setLevel(logging.DEBUG)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')
folderTag = TileCalibTools.getFolderTag(schema if 'COMP200' in schema or 'OFLP200' in schema else db, folderPath, tag)

#=== required OF2 folder
if len(tag)>0: log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))
else: log.info("Initializing folder %s" % (folderPath))
blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
#blobReader.log().setLevel(logging.DEBUG)

#=== get drawer with status at given run
log.info("Initializing ros %d, drawer %d for run %d, lumiblock %d" % (ros,drawer,run,lumi))
log.info("... %s" % blobReader.getComment((run,lumi)))
ofc = blobReader.getDrawer(ros, drawer,(run,lumi))

#=== get other OFC parameters
nchann   = ofc.getNChans()
nfields  = ofc.getNFields()
nsamples = ofc.getNSamples()
nphases  = ofc.getNPhases()
log.info( "" )
log.info( "nchann   = %d" % nchann  )
log.info( "nfields  = %d" % nfields )
log.info( "nphases  = %d" % nphases )
log.info( "nsamples = %d" % nsamples)
log.info( "" )
log.info( "OFC for %s channel %d adc %d field %d\n" % (TileCalibUtils.getDrawerString(ros,drawer),channel,adc,field) )

#=== get OFC field for given adc, sample and phase
for iphase in xrange(abs(nphases)):
    phase = ofc.getPhase(channel, adc, iphase)
    msg = "phase %6.1f ns :" % phase
    for smp in xrange(0, nsamples):
        msg += " %f" % ofc.getOfc(field, channel, adc, phase, smp)
    print msg

#=== close DB
db.closeDatabase()

