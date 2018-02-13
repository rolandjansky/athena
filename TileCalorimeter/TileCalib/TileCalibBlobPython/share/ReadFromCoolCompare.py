#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadFromCoolCompare.py
# (Based on A. Solodkov's script ReadCsFromCool.py)
# Andrei Artamonov 2010-09-22
# Note: this is a low level tool to be used only for tests
# It reads conditions from two sources (COOL and/or sqlite),
# compares values and dumps to the file difference, if it is above
# specified threshould
# Update: Yuri Smirnov 01/28/2016: folder tag protection added including
# not only short (reduced) tag names, but also full tag names/paths
#
#=== examples of folder/tag names
#folderPath = "/TILE/OFL02/CALIB/CES"
#folderPath = "/TILE/OFL02/TIME/CHANNELOFFSET/PHY"
#folderPath = "/TILE/OFL02/NOISE/SAMPLE"
#tag = "RUN2-HLT-UPD1-01"
#folderPath = "/TILE/ONL01/CALIB/CIS/LIN"
#folderPath = "/TILE/ONL01/MUID"
#folderPath = "/TILE/ONL01/FRAG1"
#folderTag = ""
#==================================================

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *
import os, sys, getopt
os.environ['TERM'] = 'linux'

# main defaults are here - can be modified from command line
run=999999999
run2=0       # will be set to "run" if not on command line
lumi=0
lumi2=-1     # will be set to "lumi" if not on command line
maxdiff=-1.0 # dump all values
maxdiffpercent=-1.0
folderPath = "/TILE/OFL02/CALIB/CES"
folderPath2 = "none" # will be set to "folderPath" if not on command line
tag = "RUN2-HLT-UPD1-01"
tag2 = "none"
schema = "COOLOFL_TILE"
schema2 = "none"
instance = "CONDBR2"
instance2 = "none"
sqlfn = "none"
sqlfn2 = "none"
help = 0

#------------------------------- parse arguments and change defaults
# print 'ARGV      :', sys.argv[1:]
options, remainder = getopt.getopt(sys.argv[1:], 'h', ['run=','run2=','lumi=','lumi2=','maxdiff=','maxdiffpercent=','folder=','folder2=','tag=','tag2=','schema=','schema2=','instance=','instance2=','sqlfn=','sqlfn2='])
# print 'OPTIONS   :', options
for opt, arg in options:
    if opt in ('--h'):
        help = 1
    elif opt in ('--run'):
        run = int(arg)
    elif opt in ('--run2'):
        run2 = int(arg)
    elif opt in ('--lumi'):
        lumi = int(arg)
    elif opt in ('--lumi2'):
        lumi2 = int(arg)
    elif opt in ('--maxdiff'):
        maxdiff = float(arg)
    elif opt in ('--maxdiffpercent'):
        maxdiffpercent = float(arg)
    elif opt in ('--folder'):
        folderPath=arg
    elif opt in ('--folder2'):
        folderPath2=arg
    elif opt in ('--tag'):
        tag=arg
    elif opt in ('--tag2'):
        tag2=arg
    elif opt in ('--schema'):
        schema=arg
    elif opt in ('--schema2'):
        schema2=arg
    elif opt in ('--instance'):
        instance=arg
    elif opt in ('--instance2'):
        instance2=arg
    elif opt in ('--sqlfn'):
        sqlfn=arg
    elif opt in ('--sqlfn2'):
        sqlfn2=arg

if not run2:
    run2=run
if lumi2<0:
    lumi2=lumi
if folderPath2=="none":
    folderPath2=folderPath
if tag2=="none":
    tag2=tag
if schema2=="none":
    schema2=schema
if instance2=="none":
    instance2=instance
if sqlfn2=="same":
    sqlfn2=sqlfn

# if maxdiffpercent>-1:
#    maxdiff=-1;

print 'run ',run, 'lumi ',lumi, 'run2 ',run2, 'lumi2 ',lumi2
print 'maxdiff ',maxdiff
print 'maxdiffpercent ',maxdiffpercent
print 'folder ',folderPath, 'folder2 ',folderPath2
print 'tag ',tag, 'tag2 ',tag2
print 'schema ', schema, 'schema2 ', schema2
print 'instance ', instance, 'instance2 ', instance2
print 'sqlfn ',sqlfn, 'sqlfn2 ',sqlfn2

if help:
    print '******** Above, the defaults are shown, one can change them'
    print ' using command-line options: ReadFromCoolCompare.py --option=value'
    print ' Any number of options in random order can be given'
    print ' If run2,lumi2,schema2,tag2 or instance2 are not given, the values'
    print '       of run,lumi,schema,tag and instance will be assigned to them'
    print 'For singleversion folders (ONL01) there should be option with empty tag: --tag='
    print 'conditions with options maxdiffpercent and maxdiff are used'
    print '   in logical AND, so, both have to be TRUE to print the data'
    print '\n Examples:'
    print '\npython ReadFromCoolCompare.py --maxdiff=100 --run=100012  --folder=/TILE/ONL01/CALIB/CES  --tag= --schema=COOLONL_TILE --tag2=HLT-REPC-004 --sqlfn2=tileSqlite.db --folder2=/TILE/OFL02/CALIB/CES --schema2=COOLOFL_TILE'
    print '\npython ReadFromCoolCompare.py --folder=/TILE/ONL01/FRAG1 --schema=COOLONL_TILE --tag= --maxdiffpercent=5.5 --run=160000 --run2=999999999'
    sys.exit()


connStr=schema+'/'+instance
connStr2=schema2+'/'+instance2

#===================================================================
#====================== FILL DB parameters BELOW ===================
#===================================================================
#--- Read from COOL server or from local sqlite file:

if sqlfn == 'none':
    db = TileCalibTools.openDbConn(connStr, 'READONLY')
else:
    db = TileCalibTools.openDb('SQLITE', instance, 'READONLY',schema,sqlfn)        
if sqlfn2 == 'none':
    db2 = TileCalibTools.openDbConn(connStr2, 'READONLY')
else:
    db2 = TileCalibTools.openDb('SQLITE', instance2, 'READONLY',schema2,sqlfn2)

#if tag:
#    folderTag = TileCalibUtils.getFullTag(folderPath, tag)
#else:
#    folderTag = ""
#if tag2:
#    folderTag2 = TileCalibUtils.getFullTag(folderPath2, tag2)
#else:
#    folderTag2 = ""

if tag.startswith('TileO'):
    folderTag = tag
elif tag:
    folderTag = TileCalibUtils.getFullTag(folderPath, tag)
else:
    folderTag = ""

if tag2.startswith('TileO'):
    folderTag2 = tag2
elif tag2:
    folderTag2 = TileCalibUtils.getFullTag(folderPath, tag2)
else:
    folderTag2 = ""

#==================================================

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("readFromCool")
import logging
log.setLevel(logging.DEBUG)
f=open('output.ascii', 'w')
if run2!=run and tag2==tag and folderPath2==folderPath and folderPath.startswith("/TILE/OFL02/TIME"):
    fd=open('from_%d_to_%d.dif'%(run2,run), 'w')
    writedif=True
else:
    writedif=False

log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))
log.info("Initializing folder %s with tag %s" % (folderPath2, folderTag2))

blobReader = TileCalibTools.TileBlobReader(db,folderPath, folderTag)
blobReader2 = TileCalibTools.TileBlobReader(db2,folderPath2, folderTag2)
#blobReader.log().setLevel(logging.DEBUG)

#=== get drawer with status at given run

ros    = 1 # ros 0 sometimes contains obsolete header !
drawer = 0
log.info("Initializing for run1 %d lumi %d run2 %d lumi2 %d maxdiff %f maxdiffpercent %f" %  (run, lumi, run2, lumi2, maxdiff,maxdiffpercent))
log.info("Comment1: %s" % blobReader.getComment((run,lumi)))
log.info("Comment2: %s" % blobReader2.getComment((run2,lumi2)))

flt=None
r=5
d=0
while not flt:
    d-=1
    if d<0:
        r-=1
        if r<0:
            log.info("ERROR!!! No valid drawers in first database")
            sys.exit()
        d=TileCalibUtils.getMaxDrawer(r)-1
    flt = blobReader.getDrawer(r, d, (run,lumi), False, False)
#flt.dump()
ot = flt.getObjType()
ov = flt.getObjVersion()
os = flt.getObjSizeByte()/4
no = flt.getNObjs()
nc = flt.getNChans()
ng = flt.getNGains()

flt2=None
r=5
d=0
while not flt2:
    d-=1
    if d<0:
        r-=1
        if r<0:
            log.info("ERROR!!! No valid drawers in second database")
            sys.exit()
        d=TileCalibUtils.getMaxDrawer(r)-1
    flt2 = blobReader2.getDrawer(r, d, (run2,lumi2), False, False)
ot2 = flt2.getObjType()
os2 = flt2.getObjSizeByte()/4

if (os <> os2) or (ot <> ot2):
    log.info("ERROR!!! object sizes %s %s or types %s %s are different" % (os, os2, ot, ot2))
    sys.exit()

v =[]
v2=[]
for ind in xrange(0,os):
    v.append(0)
    v2.append(0)

f.write("Command line parameters used: \n %s \n" % sys.argv[1:])
f.write("---- Object header for ros=%d drawer=%d \n" % (r,d))
f.write("ObjType        : %d \n" % ot)
f.write("ObjVersion     : %d \n" % ov)
f.write("ObjSize[4bytes]: %d \n" % os)
f.write("NObjs          : %d \n" % no)
f.write("NChannels      : %d \n" % nc)
f.write("NGains         : %d \n" % ng)

answ='n'
#=== get value for a gived channel
for ros in xrange(0,5):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
        modName = TileCalibUtils.getDrawerString(ros,mod)        
        #log.info("ros %d, drawer %s at run %d" % (ros, modName, run))
        flt = blobReader.getDrawer(ros, mod,(run,lumi), False, False)
        flt2 = blobReader2.getDrawer(ros, mod,(run2,lumi2), False, False)
        if flt and flt2:
            osc = flt.getObjSizeByte()/4
            os2c = flt2.getObjSizeByte()/4
            if (((os <> osc) or (os2 <> os2c)) and answ <> 'y'):
                log.info("ERROR!!! object sizes are different for ROS 0 (%s %s) and ROS %s (%s %s) drawer %s" % (os, os2, ros, osc, os2c, modName))
                answ=raw_input(' continue anyway? (y/n)')
                if (answ <> 'y'):
                    sys.exit()
                else:
                    for ind in xrange(0,osc):
                        v.append(0)
                        v2.append(0)
                    
        
            for chn in xrange(TileCalibUtils.max_chan()):
                chnName = " %2i" % chn
                for adc in xrange(ng):
                    for ind in xrange(0,osc):
                        v[ind]  =  flt.getData(chn, adc, ind)
                        v2[ind] =  flt2.getData(chn, adc, ind)
                        dv12 = v[ind] - v2[ind]
                        if v2[ind] == 0:
                            dv12percent=0
                        else:
                            dv12percent=dv12*100/v2[ind]
#                        print  modName, ' chann ',  repr(chn),  ' adc ',  repr(adc),  ' ind ',  repr(ind),  ' val1 ',  repr(v[ind]),' val2 ',  repr(v2[ind]), ' diff ',  repr(dv12), 'percent ', repr(dv12percent)
                        if abs(dv12) > maxdiff and abs(dv12percent) > maxdiffpercent:
                            if ot==30: # integers
                                f.write('%s chann %2d adc %d ind %d val1 %d val2 %d  diff %d \n' % (modName,chn,adc,ind,v[ind],v2[ind],dv12))
                            elif ot==20: # bad channels
                                f.write('%s chann %2d adc %d ind %d val1 %s val2 %s  diff %f \n' % (modName,chn,adc,ind,hex(int(v[ind])),hex(int(v2[ind])),dv12))                             
                            else:       # floats
                                f.write('%s chann %2d adc %d ind %d val1 %.4f val2 %.4f  diff %.4f %.2f%%\n' % (modName,chn,adc,ind,v[ind],v2[ind],dv12,dv12percent))
                                if writedif and adc==0 and ind==0:
                                    fd.write("%s ch %2d %.4f\n" % (modName,chn,dv12))
                                
#                        f.write(s + "\n")
                    
#=== close DB and output file
db.closeDatabase()
db2.closeDatabase()
f.close()
if writedif: fd.close()

