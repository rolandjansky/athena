#!/bin/env python
# CaloCondBlobAlgs_fillPedestalFromASCII
# Nils Gollub <nils.gollub@cern.ch>, 2008-01-11

import cppyy
from PyCool import cool
from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger

#==================================================
#===
#=== Configuration section
#===
#==================================================
#=== Input files with pedestals
inputFile = "pedestal.txt"
#=== IOV range
runSince =  0
lbkSince =  0
iovSince = CaloCondTools.iovFromRunLumi(runSince,lbkSince)
iovUntil = cool.ValidityKeyMax
#=== folder tag suffix 
tag      = "000-00" 
#=== values for the comment channel
author   = "ngollub"
comment  = "to fill pedestal shifts"


#==================================================
#===
#=== Code starts below here
#===
#==================================================
#=== set shortcut
g = cppyy.gbl
cppyy.makeClass('std::vector<float>')

#=== get a logger
log = CaloCondLogger.getLogger("CaloNoiseWriter")

#=== (re-)create the database
db = CaloCondTools.openDb('SQLITE', 'COMP200', 'UPDATE')

try:
    #=== creating folder specifications
    spec = cool.RecordSpecification()
    spec.extend( 'CaloCondBlob16M', cool.StorageType.Blob16M )
     
    #=== create the folder
    folderPath = CaloCondTools.getCaloPrefix()+"Pedestal/CellPedestal"
    folderTag  = g.CaloCondUtils.getFullTag(folderPath,tag)
    log.info( "Filling COOL folder %s with tag %s" % ( folderPath, folderTag ))
    desc = CaloCondTools.getAthenaFolderDescr()
    try:
        folder = db.getFolder(folderPath)
    except Exception, e:
        log.warning("Folder %s not found, creating it..." % folderPath)
        folder = db.createFolder(folderPath, spec, desc, cool.FolderVersioning.MULTI_VERSION, True)
        
    #==================================================
    #=== Create the CaloCondBlobFlt objects
    #==================================================   
    #=== default a and b to be used for each gain 
    gainDefVec = g.std.vector('float')()
    gainDefVec.push_back(0.) # a 
    gainDefVec.push_back(0.) # b 
    #=== three gains per channel for LAr
    defVecLAr = g.std.vector('std::vector<float>')()
    defVecLAr.push_back(gainDefVec)
    defVecLAr.push_back(gainDefVec)
    defVecLAr.push_back(gainDefVec)
    #=== four "gains" per channel for Tile
    defVecTile = g.std.vector('std::vector<float>')()
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
        nChannel = info[0] 
        defVec   = info[2]
        sysName  = info[3]
        log.info("Creating BLOB for %s" % sysName)
        data = cool.Record( spec )
        blob = data['CaloCondBlob16M']
        flt = g.CaloCondBlobFlt.getInstance(blob)
        flt.init(defVec,nChannel,2,author,comment)
        fltDict[systemId] = [data,flt]
        mbSize = float(blob.size()) / 1024.
        log.info("---> BLOB size is %4.1f kB" % mbSize)

    #=== read noise values from file
    lines = open(inputFile,"r").readlines()
    for line in lines:
        fields = line.split()
        
        systemId = int(fields[0])
        hash     = int(fields[1]) - systemDict[systemId][1]
        gain     = g.CaloCondUtils.getDbCaloGain(int(fields[2]))
        noiseA   = float(fields[3])
        noiseB   = float(fields[4])
        flt = fltDict[systemId][1]
        
        flt.setData(hash,gain,0,noiseA)
        flt.setData(hash,gain,1,noiseB)
        
    #=== write to DB
    for systemId, dataList in fltDict.iteritems():
        sysName  = systemDict[systemId][3]
        log.info("Committing BLOB for %s" % sysName)
        channelId = cool.ChannelId(systemId)
        data = dataList[0]
        folder.storeObject(iovSince, iovUntil, data, channelId, folderTag)
        
except Exception, e:
    log.fatal("Exception caught:")
    print e

#=== close the database
db.closeDatabase()

