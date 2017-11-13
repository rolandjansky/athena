#!/bin/env python

import os,sys,commands
import math

def usage():
   print "Syntax for UPD4 open-end IoV noise update"
   print " The first parameter is the run number of IoV start, the second parameter is the lumiblock number for IoV start"
   print " The third and fourth parameter are the Run/lb for IoV end (if run is -1, uses open ended IoV)"
   print " The fifth parameter is the upd4 tag name"
   print " The sixth parameter is input text file name (default calonoise.txt)"
   print " The seventh parameter is output sqlite file name (default caloSqlite.db)"
   print " The eigth parameter is output DB  name (default CONDBR2)"
   print " The nineth parameter is output folder name (default /LAR/NoiseOfl/CellNoise) "
   print " The tenth parameter is mu (default 30)"
   print " The eleventh parameter is dt (default 25)"

if len(sys.argv)<6:
   usage()
   sys.exit(-1)

runSince = sys.argv[1]
lbkSince = sys.argv[2]
runUntil = sys.argv[3]
lbkUntil = sys.argv[4]
tag = sys.argv[5]
if len(sys.argv)>6:
   inputFile=sys.argv[6]
else:
   inputFile="calonoise.txt"

if len(sys.argv)>7:
   filename=sys.argv[7]
else:
   filename="larnoisesqlite.db"

if len(sys.argv)>8:
   dbname=sys.argv[8]
else:
   dbname="CONDBR2"

if len(sys.argv)>9:
   folderPath = sys.argv[9]
else:
   folderPath = "/LAR/NoiseOfl/CellNoise"

if len(sys.argv)>10:
   mu = int(sys.argv[10]
else:         
   mu = -1

if len(sys.argv)>11:
   dt = int(sys.argv[11])
else:
   dt = -1

print "runUntil ", runUntil, lbkUntil
print "output:",filename
print "input:  ",inputFile

import ROOT
import cppyy
from PyCool import cool
from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger

#==================================================
#===
#=== Configuration section
#===
#==================================================
#=== IOV range
iovSince = CaloCondTools.iovFromRunLumi(runSince,lbkSince)
iovUntil = cool.ValidityKeyMax
print " iovUntil max ",iovUntil
if int(runUntil) > 0:
   print " use run number to define iobUntil ", runUntil
   iovUntil = CaloCondTools.iovFromRunLumi(runUntil,lbkUntil)

print " iovSince ", iovSince
print " iovUntil ", iovUntil

#=== values for the comment channel
author   = "gunal"
comment  = "Updated noise values"

#==================================================
#===
#=== Code starts below here
#===
#==================================================
#=== set shortcut
#g = PyCintex.gbl
#PyCintex.makeClass('std::vector<float>')
#PyCintex.loadDict("STLAddRflx")
cppyy.loadDictionary("STLAddRflx")

#=== get a logger
log = CaloCondLogger.getLogger("CaloNoiseWriter")

#=== (re-)create the database
#db = CaloCondTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
dbSvc = cool.DatabaseSvcFactory.databaseService()
try:
   db=dbSvc.openDatabase("sqlite://;schema="+filename+";dbname="+dbname, False)
except:
   db=dbSvc.createDatabase("sqlite://;schema="+filename+";dbname="+dbname)
   
try:
    #=== creating folder specifications
    spec = cool.RecordSpecification()
    spec.extend( 'CaloCondBlob16M', cool.StorageType.Blob16M )
    fspec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)
     
    #=== create the folder
    folderTag  = tag
    log.info( "Filling COOL folder %s with tag %s" % ( folderPath, folderTag ))
    desc = CaloCondTools.getAthenaFolderDescr()
    try:
        folder = db.getFolder(folderPath)
    except Exception, e:
        log.warning("Folder %s not found, creating it..." % folderPath)
        #folder = db.createFolder(folderPath, spec, desc, cool.FolderVersioning.MULTI_VERSION, True)
        folder = db.createFolder(folderPath, fspec, desc, True)
        
    #==================================================
    #=== Create the CaloCondBlobFlt objects
    #==================================================   
    #=== default a and b to be used for each gain 
    gainDefVec = ROOT.std.vector('float')()
    #gainDefVec = PyCintex.gbl.std.vector('float')()
    gainDefVec.push_back(0.) # a 
    gainDefVec.push_back(0.) # b 
    #=== three gains per channel for LAr
    #defVecLAr = g.std.vector('std::vector<float>')()
    defVecLAr = ROOT.std.vector('std::vector<float>')(
    defVecLAr.push_back(gainDefVec)
    defVecLAr.push_back(gainDefVec)
    defVecLAr.push_back(gainDefVec)
    #=== four "gains" per channel for Tile
    #defVecTile = g.std.vector('std::vector<float>')()
    defVecTile = ROOT.std.vector('std::vector<float>')()
    defVecTile.push_back(gainDefVec)
    defVecTile.push_back(gainDefVec)
    defVecTile.push_back(gainDefVec)
    defVecTile.push_back(gainDefVec)
    
    #=== system specific data: sysId  -> (nChannel, hash-offset, default-vector, name)
    systemDict = {  0 : (31872,      0, defVecLAr , 'EMEC, z<0'),
                    1 : (54784,  31872, defVecLAr , 'EMB , z<0'),
                    2 : (54784,  86656, defVecLAr , 'EMB , z>0'),
                    3 : (31872, 141440, defVecLAr , 'EMEC, z>0'),
                   16 : ( 5632,      0, defVecLAr , 'HEC'      ),
                   32 : ( 3524,      0, defVecLAr , 'FCAL'     ),
                   48 : ( 5184,      0, defVecTile, 'TILE'     ) 
                   }
    fltDict = {}
    for systemId, info in systemDict.iteritems():
     if (systemId<48) :
        nChannel = info[0] 
        defVec   = info[2]
        sysName  = info[3]
        log.info("Creating BLOB for %s" % sysName)
        data = cool.Record( spec )
        blob = data['CaloCondBlob16M']
        flt = ROOT.CaloCondBlobFlt.getInstance(blob)
        flt.init(defVec,nChannel,1,author,comment)
        fltDict[systemId] = [data,flt]
        mbSize = float(blob.size()) / 1024.
        log.info("---> BLOB size is %4.1f kB" % mbSize)

    #=== read noise values from file
    lines = open(inputFile,"r").readlines()
    for line in lines:
        fields = line.split()
        if len(fields) < 5:
           log.info("---> wrong line length %d entries " % len(fields))
           log.info("---> on line %d " % lcount)
           lcount +=1
           continue
        pass
        
        systemId = int(fields[0])
        hash     = int(fields[1]) - systemDict[systemId][1]
        gain     = ROOT.CaloCondUtils.getDbCaloGain(int(fields[2]))
        noiseA   = float(fields[3])
        noiseB   = float(fields[4])
        flt = fltDict[systemId][1]
        
        flt.setData(hash,gain,0,noiseA)
        if mu > 0 and dt > 0:
           # new normalization
           if dt > 25:
              noiseB /= math.sqrt(mu/53.*10.)
           else:
              noiseB /= math.sqrt(mu/29.*10.) 
        pass
        flt.setData(hash,gain,1,noiseB)
        
    #=== write to DB
    for systemId, dataList in fltDict.iteritems():
      if (systemId<48):
        sysName  = systemDict[systemId][3]
        log.info("Committing BLOB for %s" % sysName)
        channelId = cool.ChannelId(systemId)
        log.info("Cool channel ID %s" % channelId)
        data = dataList[0]
        folder.storeObject(iovSince, iovUntil, data, channelId, folderTag)
        
except Exception, e:
    log.fatal("Exception caught:")
    print e

#=== close the database
db.closeDatabase()

