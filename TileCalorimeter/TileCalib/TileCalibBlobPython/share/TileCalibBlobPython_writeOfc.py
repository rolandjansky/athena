#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeOfc.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-19
# change: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import cppyy

from PyCool import cool, coral
from TileCalibBlobPython import TileCalibTools, TileCalibLogger
import re

#==================================================
#===
#=== Configuration section
#===
#==================================================
#=== Input files with OFCs
ofcDataPath = "/afs/cern.ch/user/t/tiledaq/public/tilercd/tile-5.3.0.0/TileDSPofc/v4r3p3/share/"
ofcFiles = [ofcDataPath+'W_OF2_PHYSICS_7s.dat',
            ofcDataPath+'W_OF1_PHYSICS_7s.dat',
            ofcDataPath+'W_OF2_LED_7s.dat',
            ofcDataPath+'W_OF1_LED_7s.dat',
            ofcDataPath+'W_OF2_CIS_7s.dat',
            ofcDataPath+'W_OF1_CIS_7s.dat']
#=== pattern to decode filenames
re_fileName = re.compile("W_(OF\d)_(.*)_7s\.dat")

#=== IOV range for all folders
iovSince = cool.ValidityKeyMin
iovUntil = cool.ValidityKeyMax
#=== values for the comment channel
author   = "ngollub"
comment  = "OFC weights (7 samples) imported from TileDSPofc v4r1p4"


#==================================================
#===
#=== Code starts below here
#===
#==================================================
#=== set shortcut
g = cppyy.gbl

#=== get a logger
log = TileCalibLogger.getLogger("WriteOfc")

#=== (re-)create the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
#=== creating folder specifications
spec = cool.RecordSpecification()
#=== Note special name '*Ofc' and 16M instead of 64k for OFCs
spec.extend( 'TileCalibBlobOfc', cool.StorageType.Blob16M )

folderMode = cool.FolderVersioning.SINGLE_VERSION
folderSpec = cool.FolderSpecification(folderMode, spec)

#=== loop over all input files
for fileName in [ofcFiles[0]]:
    log.info( "Reading file: %s" % fileName )

    #=== determine COOL folder from input file name
    fileNameParts = re_fileName.search(fileName).groups()
    ofMethod   = fileNameParts[0]
    ofRun      = fileNameParts[1][:3]
    #=== fix LED->LAS
    if ofRun=="LED":
        ofRun="LAS"
    objVersion = int(ofMethod[2])
    folderPath = TileCalibTools.getTilePrefix(False)+"FILTER/"+ofMethod+"/"+ofRun
    log.info( "Filling COOL folder %s" % ( folderPath ) )
    desc = TileCalibTools.getAthenaFolderDescr()
    folder = db.createFolder(folderPath, folderSpec, desc, True)

    #=== loop over all drawers (COOL channels)
    for chan in xrange(276):
        
        data = cool.Record( spec )
        blob = data['TileCalibBlobOfc']

        #=== only fill defaults in COOL channel 0
        if chan==0:
            #=== initialize with negative nPhases, so only one common table is stored
            #=== for all channels
            drawerOfc = g.TileCalibDrawerOfc.getInstance(blob,objVersion,7,-2001,1,2)
            #=== set phases as ints: phase[ns] * 10
            phases = g.std.vector('float')()
            for phase in xrange(-1000,1001):
                phases.push_back(phase/10.)
            drawerOfc.setPhases(0,0,phases);
    
            #=== read file and fill OFCs
            phase   = -1000
            channel = 0
            adc     = 0
            sample  = 0
            lines = open(fileName,"r").readlines()
            for line in lines:

                #=== check for consistency
                if phase>1000:
                    raise ("Phase out of range: %i" % phase)
                phaseF=phase/10.

                ofc = line.split()
                #=== fill OFCs depending on OF version number
                if objVersion==1:
                    if len(ofc)!=3: raise ("OF1, but len(ofc)=%i" % len(ofc))
                    drawerOfc.setOfc(drawerOfc.FieldA,channel,adc,phaseF,sample,float(ofc[0]))
                    drawerOfc.setOfc(drawerOfc.FieldB,channel,adc,phaseF,sample,float(ofc[1]))
                    drawerOfc.setOfc(drawerOfc.FieldG,channel,adc,phaseF,sample,float(ofc[2]))
                elif objVersion==2:
                    if len(ofc)!=4: raise ("OF2, but len(ofc)=%i" % len(ofc))
                    drawerOfc.setOfc(drawerOfc.FieldA,channel,adc,phaseF,sample,float(ofc[0]))
                    drawerOfc.setOfc(drawerOfc.FieldB,channel,adc,phaseF,sample,float(ofc[1]))
                    drawerOfc.setOfc(drawerOfc.FieldC,channel,adc,phaseF,sample,float(ofc[2]))
                    drawerOfc.setOfc(drawerOfc.FieldG,channel,adc,phaseF,sample,float(ofc[3]))
                else:
                    raise ("Impossible objVersion: %i" % objVersion)

                #=== increment counters
                adc += 1
                if adc>1:
                    adc     = 0
                    sample += 1
                if sample>6:
                    sample = 0
                    phase += 1
                    
        #=== store blob in folder/channel
        channelId = cool.ChannelId(chan)
        folder.storeObject(iovSince, iovUntil, data, channelId)
    #<--end of chan loop

    #=== add comment to comment channel
    data = cool.Record( spec )
    blob = data['TileCalibBlobOfc']
    cmt = g.TileCalibDrawerCmt.getInstance(blob,author,comment)
    channelId = cool.ChannelId(g.TileCalibUtils.getCommentChannel())
    folder.storeObject(iovSince, iovUntil, data, channelId)
    #<---end of file loop

db.closeDatabase()

