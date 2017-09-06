#!/bin/env python

import os,sys,commands

def usage():
   print "Syntax for open-end IoV time constant update"
   print " The first parameter is the run number of IoV start, the second parameter is the lumiblock number for IoV start"
   print " The third and fourth parameters are the Run/lb for IoV end (if run is -1, uses open ended IoV)"
   print " the fifth and sixth parameters are the inputfile for allconstants and runconstants"

if len(sys.argv)<7:
   usage()
   sys.exit(-1)

runSince = sys.argv[1]
lbkSince = sys.argv[2]
runUntil = sys.argv[3]
lbkUntil = sys.argv[4]

print "runSince ", runSince, lbkSince
print "runUntil ", runUntil, lbkUntil

import cppyy
from PyCool import cool
from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger

#==================================================
#===
#=== Configuration section
#===
#==================================================
inputFile_con = sys.argv[5] 
inputFile_run = sys.argv[6] 

#=== IOV range
iovSince_con = CaloCondTools.iovFromRunLumi(runSince,lbkSince)
iovUntil_con = CaloCondTools.iovFromRunLumi(runUntil,lbkUntil)

iovMin = cool.ValidityKeyMin
iovMax = cool.ValidityKeyMax

print " iovSince_con ", iovSince_con
print " iovUntil_con ", iovUntil_con

print " iovMin ", iovMin
print " iovMax ", iovMax

lbkdown = 0
lbkup = 4294967295

#=== folder tag suffix 
tagCon      = "LARTimeCorrectionOflNonRunCon-00"
tagRun      = "LARTimeCorrectionOflRunCon-00"
#=== values for the comment channel
author   = "dhu"
comment  = "Updated time constant values"

#==================================================
#===
#=== Code starts below here
#===
#==================================================
#=== set shortcut
g = cppyy.gbl
cppyy.makeClass('std::vector<float>')

#=== get a logger
log = CaloCondLogger.getLogger("CaloTimeConsWriter")

#=== (re-)create the database
db = CaloCondTools.openDb('SQLITE', 'COMP200', 'UPDATE')

try:
    #=== creating folder specifications
    spec = cool.RecordSpecification()
    spec.extend( 'CaloCondBlob16M', cool.StorageType.Blob16M )
     
    #=== create the folder
    folderPath = CaloCondTools.getCaloPrefix()+"LAR/TimeCorrectionOfl/NonRunCon"
    folderTag  = tagCon
    print "Filling COOL folder ", folderPath, " with tag ", folderTag   
    desc = CaloCondTools.getAthenaFolderDescr()
    try:
        folder = db.getFolder(folderPath)
    except Exception, e:
        log.warning("Folder %s not found, creating it..." % folderPath)
        print "Folder ", folderPath, " not found, creating it... " 
        folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)
        folder = db.createFolder(folderPath, folderSpec, desc, True)
        
    #==================================================
    #=== Create the CaloCondBlobFlt objects
    #==================================================   
    #=== default a and b to be used for each gain 
    gainDefVec_1 = g.std.vector('float')()
    gainDefVec_1.push_back(0.) # febOffset+channelOffset 
    gainDefVec_1.push_back(0.) # energy depended error parameter 1
    gainDefVec_1.push_back(0.) # energy depended error parameter 2 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 1
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 2 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 3  
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 4  
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 5 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 6 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 7 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 8 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 9 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 10 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 11
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 12 
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 13  
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 14  
    gainDefVec_1.push_back(0.) # feb depended energy correction parameter 15
    #=== three gains per channel for LAr
    defVecLAr_1 = g.std.vector('std::vector<float>')()
    defVecLAr_1.push_back(gainDefVec_1)
    defVecLAr_1.push_back(gainDefVec_1)
    defVecLAr_1.push_back(gainDefVec_1)


    #=== system specific data: sysId  -> (nChannel, hash-offset, default-vector, name)
    systemDict = {  0 : (31872,      0, defVecLAr_1 , 'EMEC, z<0'),
                    1 : (54784,  31872, defVecLAr_1 , 'EMB , z<0'),
                    2 : (54784,  86656, defVecLAr_1 , 'EMB , z>0'),
                    3 : (31872, 141440, defVecLAr_1 , 'EMEC, z>0'),
                 }

    print "before fill virtual database"
    for systemId, info in systemDict.iteritems():
     print "before fill virtual database for sysId=", systemId
     if (systemId<=3) :
        print "Creating BLOB for Calo sysId=", systemId
        nChannel = info[0]
        defVec   = info[2]
        sysName  = info[3]
        log.info("Creating BLOB for %s" % sysName)
        data_vir = cool.Record( spec )
        blob_vir = data_vir['CaloCondBlob16M']
        flt_vir = g.CaloCondBlobFlt.getInstance(blob_vir)
        flt_vir.init(defVec,nChannel,1,author,comment)

        log.info("Committing BLOB for %s" % sysName)
        channelId = cool.ChannelId(systemId)
        log.info("Cool channel ID %s" % channelId)
        folder.storeObject(iovMin, iovMax, data_vir, channelId, folderTag)

    print "after fill virtual database"

    fltDict_1 = {}
    print "Prepare BLOB for Calo"
    for systemId, info in systemDict.iteritems():
     print "before check sysId=", systemId 
     if (systemId<=3) :
        print "Creating BLOB for Calo sysId=", systemId 
        nChannel = info[0]
        defVec   = info[2]
        sysName  = info[3]
        log.info("Creating BLOB for %s" % sysName)
        data = cool.Record( spec )
        blob = data['CaloCondBlob16M']
        flt = g.CaloCondBlobFlt.getInstance(blob)
        flt.init(defVec,nChannel,1,author,comment)
        fltDict_1[systemId] = [data,flt]
        kbSize = float(blob.size()) / 1024
        print "BLOB size is ", kbSize, " KB"
        mbSize = float(kbSize) / 1024
        print "BLOB size is ", mbSize, " MB"


    #=== read time values from file
    print "before read time values from file"
    lines = open(inputFile_con,"r").readlines()
    print "before enter file line loop"
    for line in lines:
        fields = line.split()
        
        systemId = int(fields[0])
        hash_offset = int(systemDict[systemId][1])
        hash_raw    = int(fields[6]) 
        hash = hash_raw - hash_offset
        hash_check     = int(fields[7]) 
        gain     = g.CaloCondUtils.getDbCaloGain(int(fields[8]))
        febchanOffset = float(fields[9])
        errPar1   = float(fields[10])
        errPar2   = float(fields[11])
        enePar1   = float(fields[12])
        enePar2   = float(fields[13])
        enePar3   = float(fields[14])
        enePar4   = float(fields[15])
        enePar5   = float(fields[16])
        enePar6   = float(fields[17])
        enePar7   = float(fields[18])
        enePar8   = float(fields[19])
        enePar9   = float(fields[20])
        enePar10   = float(fields[21])
        enePar11   = float(fields[22])
        enePar12   = float(fields[23])
        enePar13   = float(fields[24])
        enePar14   = float(fields[25])
        enePar15   = float(fields[26])

        if not hash==hash_check:
           print "hash!=hash_check:", hash , hash_check
           continue

        if systemId==0:
           if hash>=31872:
             print "hash>=31872 for hash ", hash , ", sys ", systemId
             continue

        if systemId==1:
           if hash>=54784:
             print "hash>=54784 for hash ", hash , ", sys ", systemId
             continue

        if systemId==2:
           if hash>=54784:
             print "hash>=54784 for hash ", hash , ", sys ", systemId
             continue

        if systemId==3:
           if hash>=31872:
             print "hash>=31872 for hash ", hash , ", sys ", systemId
             continue

        print systemId, hash, hash_check, gain, febchanOffset, errPar1, errPar2, enePar1, enePar2, enePar3, enePar4, enePar5, enePar6, enePar7, enePar8, enePar9, enePar10, enePar11, enePar12, enePar13, enePar14, enePar15

        print "before get flt"
        flt = fltDict_1[systemId][1]
        flt.setData(hash,gain,0,febchanOffset)
        flt.setData(hash,gain,1,errPar1)
        flt.setData(hash,gain,2,errPar2)
        flt.setData(hash,gain,3,enePar1)
        flt.setData(hash,gain,4,enePar2)
        flt.setData(hash,gain,5,enePar3)
        flt.setData(hash,gain,6,enePar4)
        flt.setData(hash,gain,7,enePar5)
        flt.setData(hash,gain,8,enePar6)
        flt.setData(hash,gain,9,enePar7)
        flt.setData(hash,gain,10,enePar8)
        flt.setData(hash,gain,11,enePar9)
        flt.setData(hash,gain,12,enePar10)
        flt.setData(hash,gain,13,enePar11)
        flt.setData(hash,gain,14,enePar12)
        flt.setData(hash,gain,15,enePar13)
        flt.setData(hash,gain,16,enePar14)
        flt.setData(hash,gain,17,enePar15)

    print "outside file line loop"

    #=== write to DB
    print "Committing BLOB for Calo"
    for systemId, dataList in fltDict_1.iteritems():
      if (systemId<=3):
        sysName  = systemDict[systemId][3]
        log.info("Committing BLOB for %s" % sysName)
        channelId = cool.ChannelId(systemId)
        log.info("Cool channel ID %s" % channelId)
        data = dataList[0]
        folder.storeObject(iovSince_con, iovUntil_con, data, channelId, folderTag)

except Exception, e:
    log.fatal("Exception caught: fill LAR/TimeCorrectionOfl/NonRunCon")
    print "Exception caught: fill LAR/TimeCorrectionOfl/NonRunCon"
    print e

try:
    #=== creating folder specifications
    spec = cool.RecordSpecification()
    spec.extend( 'CaloCondBlob16M', cool.StorageType.Blob16M )

    #=== create the folder
    folderPath = CaloCondTools.getCaloPrefix()+"LAR/TimeCorrectionOfl/RunCon"
    folderTag  = tagRun
    print "Filling COOL folder ", folderPath,  "with tag ", folderTag
    desc = CaloCondTools.getAthenaFolderDescr()
    try:
        folder = db.getFolder(folderPath)
    except Exception, e:
        log.warning("Folder %s not found, creating it..." % folderPath)
        print "Folder ", folderPath, " not found, creating it... " 
         
        folder = db.createFolder(folderPath, spec, desc, cool.FolderVersioning.MULTI_VERSION, True)

    #==================================================
    #=== Create the CaloCondBlobFlt objects
    #==================================================   
    #=== default a and b to be used for each gain 
    gainDefVec_2 = g.std.vector('float')()
    gainDefVec_2.push_back(0.) # runOffset 
    gainDefVec_2.push_back(0.) # runEntry 
    #=== three gains per channel for LAr
    defVecLAr_2 = g.std.vector('std::vector<float>')()
    defVecLAr_2.push_back(gainDefVec_2)
    defVecLAr_2.push_back(gainDefVec_2)
    defVecLAr_2.push_back(gainDefVec_2)

    # fill database with virtual value 
    print "before fill virtual database"
    nChannel = 116 #  0-31: Barrel C; 32-63: Barrel A; 64-88: EMEC; 89-113: EMEA; 
    channelId = cool.ChannelId(1)
    data_vir = cool.Record( spec )
    blob_vir = data_vir['CaloCondBlob16M']
    flt_vir = g.CaloCondBlobFlt.getInstance(blob_vir)
    flt_vir.init(defVecLAr_2,nChannel,1,author,comment)
    folder.storeObject(iovMin, iovMax, data_vir, channelId, folderTag)
    print "after fill virtual database"

    run_list  = []
    lines = open(inputFile_run,"r").readlines()

    print "before fill run_list"
    for line in lines:
        fields = line.split()
        run = int(fields[0])
        if run_list.count(run)==0:  
           run_list.append(run)

    print "after fill run_list"
    run_list.sort()
    print "number of runs: ", len(run_list)
    print "run_list: ", run_list

    fltDict_2 = {}
    for runId in run_list:
        print "Creating BLOB for run ", runId 
        nChannel = 116 #  0-31: Barrel C; 32-63: Barrel A; 64-88: EMEC; 89-113: EMEA; 
        data = cool.Record( spec )
        blob = data['CaloCondBlob16M']
        flt = g.CaloCondBlobFlt.getInstance(blob)
        flt.init(defVecLAr_2,nChannel,1,author,comment)
        fltDict_2[runId] = [data,flt]
        kbSize = float(blob.size()) / 1024.
        print "BLOB size is ", kbSize, " kB"
        mbSize = float(kbSize) / 1024
        print "BLOB size is ", mbSize, " MB"

    
    print "before enter file line loop"
    for line in lines:
        fields = line.split()

        runId = int(fields[0])
        ftID = int(fields[5])
        runOffsetHigh = float(fields[6])
        runEntryHigh = float(fields[7])
        runOffsetMed = float(fields[8])
        runEntryMed = float(fields[9])
        runOffsetLow = float(fields[10])
        runEntryLow = float(fields[11])
        gain0  = g.CaloCondUtils.getDbCaloGain(0)
        gain1  = g.CaloCondUtils.getDbCaloGain(1)
        gain2  = g.CaloCondUtils.getDbCaloGain(2)

        flt = fltDict_2[runId][1]       
        flt.setData(ftID,gain0,0,runOffsetHigh)
        flt.setData(ftID,gain0,1,runEntryHigh)
        flt.setData(ftID,gain1,0,runOffsetMed)
        flt.setData(ftID,gain1,1,runEntryMed)
        flt.setData(ftID,gain2,0,runOffsetLow)
        flt.setData(ftID,gain2,1,runEntryLow)

        print runId, ftID, runOffsetHigh, runEntryHigh,  runOffsetMed, runEntryMed, runOffsetLow, runEntryLow

    print "after enter file line loop"

    for runId, dataList in fltDict_2.iteritems():
        print "Committing BLOB for run ", runId
        iovSince_run = CaloCondTools.iovFromRunLumi(runId,lbkdown)
        iovUntil_run = CaloCondTools.iovFromRunLumi(runId,lbkup)
        channelId = cool.ChannelId(1) 
        print "Cool channel ID ", channelId  
        data = dataList[0]
        folder.storeObject(iovSince_run, iovUntil_run, data, channelId, folderTag)

except Exception, e:
    log.fatal("Exception caught: LAR/TimeCorrectionOfl/RunCon")
    print "Exception caught: LAR/TimeCorrectionOfl/RunCon"
    print e

#=== close the database
db.closeDatabase()

