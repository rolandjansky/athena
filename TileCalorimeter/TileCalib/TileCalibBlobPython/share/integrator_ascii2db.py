#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_integratorExample
# Nils Gollub <nils.gollub@cern.ch>, 2008-02-07
# change Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import sys, re, os.path

#=== get a logger
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("int_ascii2db")

#=== process command line arguments
usage = "Usage: integrator_ascii2db.py "
usage+= "[-a author] [-c comment] "
usage+= "[-from \"yyyy-mm-dd hh:mm:ss\"] "
usage+= "file1 [file2 file3 ...]"
author        = "noAuthor"
comment       = "noComment"
regFrom = -1
fileList = []
if (len(sys.argv)<2) or ("-h" in sys.argv) or ("--help" in sys.argv):
    print usage
    sys.exit(0)
sys.argv.pop(0)
while len(sys.argv)>0:
    arg = sys.argv.pop(0)
    if   arg=="-a" and len(sys.argv)>0:
        author = sys.argv.pop(0)
    elif arg=="-c" and len(sys.argv)>0:
        comment = sys.argv.pop(0)
    elif arg=="-from" and len(sys.argv)>0:
        regFrom = sys.argv.pop(0)
    else:
        #=== check if file exists
        if os.path.exists(arg):
            fileList.append(arg)
        else:
            log.error("File \"%s\" does not exist, ignoring input!" % arg)
        

import cppyy
from TileCalibBlobPython import TileCalibTools

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')

#=== get a blob writer
blobWriter = TileCalibTools.TileBlobWriter(db,"/TILE/ONL01/INTEGRATOR",True,False)

#=== declare default vector
default = cppyy.gbl.std.vector('float')()
default.push_back(0.)     # gain            0
default.push_back(0.)     # sigma(gain)     1
default.push_back(0.)     # chi2            2
default.push_back(0.)     # DAC             3
default.push_back(0.)     # pedestal        4
default.push_back(0.)     # sigma(pedestal) 5
default.push_back(0.)     # rms             6
default.push_back(0.)     # sigma(rms)      7
defVec = cppyy.gbl.std.vector('std::vector<float>')()
defVec.push_back(default) # gain 1
defVec.push_back(default) # gain 2
defVec.push_back(default) # gain 3
defVec.push_back(default) # gain 4
defVec.push_back(default) # gain 5
defVec.push_back(default) # gain 6

#=== define search patterns
re_file = re.compile("int_gains_(\D{3})(\d{2})")
re_Res  = re.compile("Res\s*channel:\s*(.*)\s*gain:\s*(.*)\s*" +
                     "gain:\s*(.*)\s*\+\-\s*(.*)\s*"           +
                     "chi2/DF:\s*(.*)"                         )
re_Ped  = re.compile("Ped\s*gain:\s*(.*)\s*dac:\s*(.*)\s*chan:\s*(.*)\s*" +
                     "pedestal:\s*(.*)\s*\+\-\s*(.*)\s*"                  +
                     "rms:\s*(.*)\s*\+\-\s*(.*)\s*"                       )

#=== define rosId mapping
rosId = {'LBA' : 1,
         'LBC' : 2,
         'EBA' : 3,
         'EBC' : 4}

#=== loop over all input files
for fileName in fileList:

    #=== extract ros and drawer from fileName
    result_file = re_file.search(fileName)
    if result_file:
        rosIdx = rosId[result_file.groups()[0]]
        modIdx = int(result_file.groups()[1]) - 1
    else:
        log.error("Invalid file name \"%s\", skipping file..." , fileName)

    log.info("Processing file %s" % fileName)

    #=== get drawer object and initialize
    flt = blobWriter.getDrawer('Flt',rosIdx,modIdx)
    flt.init(defVec,48,0)

    #=== loop through file
    lines = open(fileName,"r").readlines()
    resCount=0
    pedCount=0
    for line in lines:
        #=== look for "Res" lines
        result_Res = re_Res.search(line)
        if result_Res:
            pmtIdx  = int(  result_Res.groups()[0]) - 1 #=== ordered by PMT!!
            gainIdx = int(  result_Res.groups()[1]) - 1
            gain    = float(result_Res.groups()[2])
            gainErr = float(result_Res.groups()[3])
            chi2    = float(result_Res.groups()[4])
            #=== fill into blob
            flt.setData(pmtIdx,gainIdx,0,gain)
            flt.setData(pmtIdx,gainIdx,1,gainErr)
            flt.setData(pmtIdx,gainIdx,2,chi2)
            resCount += 1

        #=== look for "Res" lines
        result_Ped = re_Ped.search(line)
        if result_Ped:
            gainIdx = int(  result_Ped.groups()[0]) - 1
            dac     = float(result_Ped.groups()[1])
            pmtIdx  = int(  result_Ped.groups()[2]) - 1 #=== ordered by PMT!!
            ped     = float(result_Ped.groups()[3])
            pedErr  = float(result_Ped.groups()[4])
            rms     = float(result_Ped.groups()[5])
            rmsErr  = float(result_Ped.groups()[6])
            flt.setData(pmtIdx,gainIdx,3,dac)
            flt.setData(pmtIdx,gainIdx,4,ped)
            flt.setData(pmtIdx,gainIdx,5,pedErr)
            flt.setData(pmtIdx,gainIdx,6,rms)
            flt.setData(pmtIdx,gainIdx,7,rmsErr)
            pedCount += 1 

    log.info("---> %s%s, registered %3i Res and %3i Ped lines" % (result_file.groups()[0],
                                                                  result_file.groups()[1],
                                                                  resCount, pedCount))
             
#=== register all drawers in the database
blobWriter.setComment(author,comment)
blobWriter.register(regFrom,-1)

#=== close the database connection
db.closeDatabase()
