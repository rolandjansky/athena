#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadNoiseFromCool.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-08-05

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal noise from SAMPLE and OFNI folders"
    print ""
    print "-h, --help      shows this help"
    print "-t, --tag=      specify tag to use, f.i. RUN2-HLT-UPD1-01 or COM-01"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-p, --ros=      specify partition (ros number), default is 1"
    print "-d, --drawer=   specify drawer number, default is 0"
    print "-c, --channel=  specify channel number, default is 0"
    print "-g, -a, --adc=  specify gain (adc number), default is 0"
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    
letters = "hr:l:s:t:p:d:c:a:g:"
keywords = ["help","run=","lumi=","schema=","tag=","ros=","drawer=","channel=","adc=","gain="]

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
tag = "HLT-UPD1-01" # tag is needed only for COMP200, ignored in CONDBR2
ros     = 1
drawer  = 0
channel = 0
adc     = 0

for o, a in opts:
    if o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-r","--run"):
        run = int(a) 
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-p","--ros"):
        ros = int(a)
    elif o in ("-d","--drawer"):
        drawer = int(a)
    elif o in ("-c","--channel"):
        channel = int(a)
    elif o in ("-a","--adc","-g","--gain"):
        adc = int(a)
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

        
if not 'COOLONL_TILE' in schema and not 'sqlite' in schema:
    print "This script works on the 'COOLONL_TILE/COMP200' or 'COOLONL_TILE/CONDBR2' schema" 
    sys.exit(2)

        
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("readNoise")
import logging
log.setLevel(logging.DEBUG)


#=== set database
db = TileCalibTools.openDbConn(schema,'READONLY')

folder1="/TILE/ONL01/NOISE/SAMPLE"
folder2="/TILE/ONL01/NOISE/OFNI"
if 'COMP200' in schema: folder1="/TILE/OFL01/NOISE/SAMPLE"
log.info("Initializing ros %d, drawer %d for run %d, lumiblock %d" % (ros,drawer,run,lumi))
for folderPath in [folder1, folder2]:

    folderTag = TileCalibTools.getFolderTag(db, folderPath, tag)
    log.info("Initializing folder %s with tag %s" % (folderPath, folderTag) )
    blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
    log.info("... %s" % blobReader.getComment((run,lumi)))
    blob = blobReader.getDrawer(ros, drawer,(run,lumi))

    if folderPath.find("SAMPLE")!=-1:
        ped = blob.getData(channel, adc, 0)
        hfn = blob.getData(channel, adc, 1)
        lfn = blob.getData(channel, adc, 2)
    else:    
        rms = blob.getData(channel, adc, 0)
        plp = blob.getData(channel, adc, 1)
        
log.info( "\n" )
print ( "%s ch %i gn %i :  PED = %f  HFN = %f  LFN = %f    OF_RMS = %f  PILEUP = %f" %
        ( TileCalibUtils.getDrawerString(ros,drawer), channel, adc,
          ped, hfn, lfn, rms, plp) )

#=== close DB
db.closeDatabase()

