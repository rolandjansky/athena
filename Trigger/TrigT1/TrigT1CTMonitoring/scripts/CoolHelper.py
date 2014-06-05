# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from PyCool import cool,coral
from CoolConvUtilities import AtlCoolTool
from AtlDataSummary.AtlDataSumLumiData import LumiBCIDData
from AtlDataSummary.AtlDataSumIOV import IOVTime, IOVRange
# import AtlDataSummary.AtlDataSumIOV
from AtlDataSummary.AtlDataSumLumiBCID import BCIDHandler
from AtlDataSummary.AtlDataSumLumi import LumiHandler

def openDb(MONP = False, TDAQ = False):
# get database service and open database
    dbSvc=cool.DatabaseSvcFactory.databaseService()
    schemaName = "COMP200"
    if MONP : schemaName = "MONP200"
    accountName = "ATLAS_COOLONL_TRIGGER"
    if TDAQ : accountName = "ATLAS_COOLONL_TDAQ"
    dbstring="oracle://ATLAS_COOLPROD;schema="+accountName+";dbname="+schemaName+";user=ATLAS_COOL_READER;password=COOLRED4PRO"
    try:
        db=dbSvc.openDatabase(dbstring,False)
    except Exception,e:
        print e
        sys.exit(-1)

    return db

def readLvl1TotalRate(run):
    
    """ Read global L1A counter and turn counter, return dict indexed by LB, each value is a pair of (counter,turncounter) """

    counts = {}
    db = openDb()
    fname='/TRIGGER/LVL1/CTPCORELBDATA'
    if not db.existsFolder(fname) :
        print "Folder",fname,"not found"
        db.closeDatabase()
        sys.exit(-1)
    folder=db.getFolder(fname)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            lb = int(obj.since() & 0xffff)
            payload=obj.payload()
            counter = payload['GlobalL1AcceptCounter']
            turnCounter = payload['GlobalL1AcceptTurnCounter']
            counts[lb] = (counter,turnCounter)
        itr.close()
    except Exception,e:
        print "Reading data from",fname,"failed:",e
    return counts

def readLvl1Turns(run, label):
    
    """ Read turn counters needed for rate calculations, return vector indexed by LB """

    payloadName = ''
    TBPTurns = {}
    if label == 'TBP': payloadName = 'BeforePrescale'
    if label == 'TAP': payloadName = 'AfterPrescale'
    if label == 'TAV': payloadName = 'L1Accept'
    if not payloadName :
        print "Invalid label",label," / valid values are 'TBP','TAP','TAV'"
        sys.exit(-1)

    db = openDb()
    fname='/TRIGGER/LVL1/CTPCORELBDATA'
    if not db.existsFolder(fname) :
        print "Folder",fname,"not found"
        db.closeDatabase()
        sys.exit(-1)
    folder=db.getFolder(fname)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            lb = int(obj.since() & 0xffff)
            payload=obj.payload()
            TBPTurns[lb] = payload[payloadName + 'TurnCounter']
            
        itr.close()
    except Exception,e:
        print "Reading data from",fname,"failed:",e
    return TBPTurns

def readBunchGroupKeys(run, debug = False):
    
    """ Read bunch group configuration keys from COOL """

    db = openDb()

    bgKeys = []    
    bgkey_fname='/TRIGGER/LVL1/BunchGroupKey'
    if not db.existsFolder(bgkey_fname) :
        print "Folder",bgkey_fname,"not found"
        db.closeDatabase()
        sys.exit(-1)
    folder=db.getFolder(bgkey_fname)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            payload=obj.payload()
            bgKeys.append(int(payload['Lvl1BunchGroupConfigurationKey']))
        itr.close()
    except Exception,e:
        print "Reading data from",bgkey_fname,"failed:",e

    if debug : print "Run",run,"bg keys",bgKeys
    return bgKeys

def readBunchGroupDescription(run,debug = False):

    """ read bunch group description (names and item to bunch group mapping) from COOL """

    bgNames = []
    bgItemMap = {}
    itemNames = readLvl1ItemNames(run)

    db = openDb()

    bgdesc_fname='/TRIGGER/LVL1/BunchGroupDescription'    
    if not db.existsFolder(bgdesc_fname) :
        print "Folder",bgdesc_fname,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(bgdesc_fname)
    bgMap = []
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            lb = int(obj.since() & 0xffff)
            payload=obj.payload()
            bgNames.append(str(payload['BunchGroup0']))
            bgNames.append(str(payload['BunchGroup1']))
            bgNames.append(str(payload['BunchGroup2']))
            bgNames.append(str(payload['BunchGroup3']))
            bgNames.append(str(payload['BunchGroup4']))
            bgNames.append(str(payload['BunchGroup5']))
            bgNames.append(str(payload['BunchGroup6']))
            bgNames.append(str(payload['BunchGroup7']))
            bgMap = payload['ItemToBunchGroupMap']

        itr.close()
    except Exception,e:
        print "Reading data from",bgkey_fname,"failed:",e
        
    for n in range(len(bgNames)):
        if debug : print "BG",n,":",bgNames[n]
    ctpId = 0
    for ii in range(len(bgMap)):
        bitCode = ''
        bgItemMap[itemNames[ctpId]] = int(bgMap[ii])
        ctpId += 1
        for bit in range(7,-1,-1):
            if (bgMap[ii]>>bit & 1) : bitCode += '1'
            else : bitCode += '0'
        if debug : print "Item",ii,"BG Code",bitCode
    return bgNames, bgItemMap

def extractBcids(mask, bgFullContent):

    """ Given a list of item names, generate a list of BCIDs during which the item can fire, return dict mapping LBs to BCIDs """

    bcidList = {}
    for lb in bgFullContent.keys():
        bunchGroupBcidList = bgFullContent[lb]
        crossings = []
        bunchGroupsToCheck = []
        for nn in range(8):
            if (mask >> nn) & 0x1:
                bunchGroupsToCheck.append(nn)
        for bx in range(3564):
            foundOverlap = True
            for nn in bunchGroupsToCheck:
                if not foundOverlap:
                    continue
                if not bx in bunchGroupBcidList[nn]:
                    foundOverlap = False
                    
            if foundOverlap:
                crossings.append(bx)
        bcidList[lb] = crossings

    return bcidList

def readBunchGroupContent(run, debug = False):

    """ Read full bunch group content from COOL """

    bgFullContent = {}

    db = openDb()

    bgcont_fname='/TRIGGER/LVL1/BunchGroupContent'
    if not db.existsFolder(bgcont_fname) :
        print "Folder",bgcont_fname,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(bgcont_fname)
    try:
        index = 0
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            lb= int(obj.since() & 0xffff)
            if debug : print "LB",lb
            bgList = []
            for _ in range(8): bgList.append([])
            payload=obj.payload()
            bgcont = payload['BunchCode']
            for bcid in range(3564): 
                for bgrp in range(8):
                    if(bgcont[bcid]>>bgrp & 1): bgList[bgrp].append(bcid)
                    ++index
                    for grp in range(8):
                        pass
                        # print "BG",grp,"size :",len(bgList[grp])
                        for grp in range(8):
                            pass
                            # print "BG",grp,":",bgList[grp]
            bgFullContent[lb] = bgList

        itr.close()
    except Exception,e:
        print "Reading data from",bgcont_fname,"failed:",e

    return bgFullContent

def readLvl1ItemNames(run):

    """ Read LVL1 menu and extract item names indexed by their CTP ID """

    db = openDb()

    itemNames=[""]*256
    lvl1menu_foldername='/TRIGGER/LVL1/Menu'
    if not db.existsFolder(lvl1menu_foldername) :
        print "Folder",lvl1menu_foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)
    folder=db.getFolder(lvl1menu_foldername)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            itemNo=obj.channelId()
            payload=obj.payload()
            itemNames[itemNo] = payload['ItemName']
        itr.close()
    except Exception,e:
        print "Reading data from",lvl1menu_foldername,"failed:",e

    return itemNames

def readLvl1Prescales(run, itemList):

    """ Read LVL1 prescales from COOL for the whole run, return dict indexed by item name, value is a list of prescales per LB """

    # first get item names mapped to CTP IDs
    itemNames = readLvl1ItemNames(run)
    itemIDs = []
    id = 0
    for name in itemNames:        
        if name in itemList:
            itemIDs.append(id)
        id += 1
    # now construct channels we want to read
    channels = cool.ChannelSelection(cool.ChannelSelection.channelBeforeSince)
    for id in itemIDs:
        channels.addChannel(id)

    Prescales = {}
    db = openDb()
    foldername='/TRIGGER/LVL1/Prescales'
    if not db.existsFolder(foldername) :
        print "Folder",foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(foldername)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,channels)
        while itr.goToNext():
            obj=itr.currentRef()
            lb = int(obj.since() & 0xffff)
            itemNo=obj.channelId()
            if not itemNo in itemIDs: continue
            payload=obj.payload()            
            try:
                Prescales[itemNames[itemNo]][lb] = int(payload['Lvl1Prescale'])
            except KeyError:
                newDict = {}
                Prescales[itemNames[itemNo]] = newDict
                Prescales[itemNames[itemNo]][lb] = int(payload['Lvl1Prescale'])
        itr.close()
        return Prescales
    except Exception,e:
        print "Reading data from",foldername,"failed:",e

def readLvl1Counters(run, itemList, label):

    """ Read LVL1 item rates from COOL for the whole run, return dict indexed by item name, value is a list of rates per LB """

    # first get item names mapped to CTP IDs
    itemNames = readLvl1ItemNames(run)
    itemIDs = []
    id = 0
    for name in itemNames:        
        if name in itemList:
            itemIDs.append(id)
        id += 1
        
    payloadName = ''
    if label == 'TBP': payloadName = 'BeforePrescale'
    if label == 'TAP': payloadName = 'AfterPrescale'
    if label == 'TAV': payloadName = 'L1Accept'
    if not payloadName :
        print "Invalid label",label," / valid values are 'TBP','TAP','TAV'"
        sys.exit(-1)

    # now construct channels we want to read
    channels = cool.ChannelSelection(cool.ChannelSelection.channelBeforeSince)
    for id in itemIDs:
        channels.addChannel(id)

    TBPCounts={}
#    LBs = []

    db = openDb()

    lvl1counters_foldername='/TRIGGER/LUMI/LVL1COUNTERS'
    if not db.existsFolder(lvl1counters_foldername) :
        print "Folder",lvl1counters_foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(lvl1counters_foldername)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,channels)
        while itr.goToNext():
            obj=itr.currentRef()
            lb = int(obj.since() & 0xffff)
            itemNo=obj.channelId()
#            if not lb in LBs : LBs.append(lb)
            if not itemNo in itemIDs: continue
            payload=obj.payload()            
            try:
                TBPCounts[itemNames[itemNo]][lb] = int(payload[payloadName])
            except KeyError:
                newDict = {}
                TBPCounts[itemNames[itemNo]] = newDict
                TBPCounts[itemNames[itemNo]][lb] = int(payload[payloadName])

        itr.close()
        return TBPCounts

    except Exception,e:
        print "Reading data from",lvl1lbdata_foldername,"failed:",e

def getLumiCalibFunction(tnow, channel):
    import struct
    db = openDb(False,True)
    # gets the calibration function and parameters
    fcal=db.getFolder("/TDAQ/OLC/CALIBRATIONS")
    objcal=fcal.findObject(tnow,channel)
    plcal=objcal.payload()
    numOfPar = plcal["NumOfParameters"]
    functionType = plcal["Function"]
    muToLumi =  plcal["MuToLumi"]
    blob=cool.Blob64k()
    blob=plcal["Parameters"]
    s=blob.read()
    parVec=[]
    for i in range(0,len(blob),4):
        parVec.append(struct.unpack('f',s[i:i+4])[0])
    db.closeDatabase()
    return functionType,muToLumi,numOfPar,parVec

def calibrateLumi(raw,cType,nPar,parVec):
  import math
# performs the same calibration as the OLC
  cal=-1.0
  if cType == "Polynomial":
    nrange=parVec[0]
    for i in range(int(nrange)):
      rmin=parVec[i+1]
      rmax=parVec[i+2]
      a=[parVec[i+3]]
      a.append(parVec[i+4])
      a.append(parVec[i+5])
      a.append(parVec[i+6])
      a.append(parVec[i+7])
      a.append(parVec[i+8])
      if raw<rmax and raw>=rmin:
          cal=0.0
          for k in range(6):
            cal=cal+a[k]*pow(raw,k)
  elif cType == "Logarithm":
      try:
          cal=-parVec[0]*math.log(1.0-raw)
      except ValueError:
          print "ValueError: raw",raw
          cal = 0
  elif cType == "None":
    print 'Warning: calibration type is None - normally should not get here...'
    return 1
  else:
    print 'Unknown calibration type ',cType
    sys.exit()
  return cal

def readReadyForPhysics(run):
    """ Read ready-for-physics flag for the given run. Return dictionary indexed by LB number, Bool flag to indicate ready-for-physics flag """

    db = openDb(False,True)
    foldername = "/TDAQ/RunCtrl/DataTakingMode"
    if not db.existsFolder(foldername) :
        print "Folder",foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)
    folder=db.getFolder(foldername)
    lbList = {}
    try:
        itr=folder.browseObjects(run<<32, (run<<32)+0xFFFFFFFF ,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            payload=obj.payload()
            lb = int(obj.since() & 0xffff)
            ready = payload['ReadyForPhysics']
            lbList[lb] = bool(ready)
        itr.close()
    except Exception,e:
        print "Reading data from",foldername,"failed:",e
    db.closeDatabase()
    return lbList

def readLhcFillNumber(run):
    """ Read LHC fill number for this run """

    fillList = []
    startRun, stopRun = runStartStopTime(run)
    startTime = IOVTime(timerunlb=startRun, timebased=True)
    endTime = IOVTime(timerunlb=stopRun, timebased=True) 
    iovrange = IOVRange(starttime = startTime, endtime = endTime)

    lhcfolder = '/LHC/DCS/FILLSTATE'
    dbstring="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
    try:
        dbSvc=cool.DatabaseSvcFactory.databaseService()
        db=dbSvc.openDatabase(dbstring,False)
    except Exception,e:
        print e
        sys.exit(-1)

    folder = db.getFolder(lhcfolder)
    itr = folder.browseObjects(iovrange.startTime.time, iovrange.endTime.time, cool.ChannelSelection.all())
    while itr.goToNext():
        obj = itr.currentRef()
        fillNumber = obj.payload()['FillNumber']
        stable = obj.payload()['StableBeams']

        # Protection against bogus values
        if fillNumber == None or stable == None:
            print 'Fill number or stable flag is NULL for',iovrange
            fillNumber = 0
        else:
            if stable and fillNumber not in fillList:
                print "run",run,"fill",fillNumber
                fillList.append(fillNumber)

    db.closeDatabase()
    return fillList



def readLumiBCID(run):
    """ Read luminosity per BCID from COOL """
    
    lbTimes = getLbTimes(run)
    startRun, stopRun = runStartStopTime(run)
    startTime = IOVTime(timerunlb=startRun, timebased=True)
    endTime = IOVTime(timerunlb=stopRun, timebased=True) 
    iovrange = IOVRange(starttime = startTime, endtime = endTime)
    bh = BCIDHandler()
    bh.lumiChan = 101
    if run > 170000: bh.lumiChan = 201
    bh.loadBCIDData(iovrange)    
    calType,muToLumi,calNumOfPar,calParVec = getLumiCalibFunction(startRun, bh.lumiChan)

#    lh = LumiHandler()
#    lh.loadLumiData(iovrange)

    lbList = dict()
    for key in bh.bcidDict.keys():
        if bh.bcidDict[key].runLB.lb == 0: continue
        bcidDico = {}
        duration = (lbTimes[bh.bcidDict[key].runLB.lb][1] - lbTimes[bh.bcidDict[key].runLB.lb][0]) / 1e9
        # fix this
        for bcid in bh.bcidDict[key].rawLumi.keys():
            calibLumi = muToLumi * calibrateLumi(bh.bcidDict[key].rawLumi[bcid],calType,calNumOfPar,calParVec)
#            print "LB",bh.bcidDict[key].runLB.lb,"BCID",bcid,"mu",calibLumi/muToLumi
#            calibLumi = calibrateLumi(bh.bcidDict[key].rawLumi[bcid],calType,calNumOfPar,calParVec)
#            bh.bcidDict[key].rawLumi[bcid] = calibLumi
#            if bcid == 1: print "Cal",calibLumi
            bcidDico[bcid] = (calibLumi * duration,calibLumi/muToLumi)
        lbList[bh.bcidDict[key].runLB.lb] = bcidDico

    return lbList

def readBunchCurrents(run, bcidList):

    """ Read bunch currents for the given run, beam, and BCID list. Bunch groups might change on LB change, hence the BCID list must be a dictionary keyed by LB numbers, values are BCIDs"""

    lbDict1 = {}
    lbDict2 = {}
#    db = openDb(True,True)
#    lbTimes = getLbTimes(run)
    startRun, stopRun = runStartStopTime(run)

    startTime = IOVTime(timerunlb=startRun, timebased=True)
    endTime = IOVTime(timerunlb=stopRun, timebased=True) 
    iovrange = IOVRange(starttime = startTime, endtime = endTime)
    bh = BCIDHandler()
    bh.loadBCIDData(iovrange)

    # the BCID list is keyed by the first LB at which the BCID list was
    # valid, hence have to figure out here the largest LB in the bcidList
    # that is <= the current LB 
    for key in bh.bcidDict.keys():       
        latestLb = 0
        for lb in bcidList.keys():
            if bh.bcidDict[key].runLB.lb >= lb and lb > latestLb:
                latestLb = lb

        # beam 1
        bunchCurrents1 = 0.
        for bcid in bh.bcidDict[key].b1Curr.keys():
            # account for the BCT phase set up wrongly, hence try also BCID+1, -1
            if bcid in bcidList[latestLb]:
                bunchCurrents1 = bunchCurrents1 + bh.bcidDict[key].b1Curr[bcid]
            elif (bcid+1) in bcidList[latestLb]:
                bunchCurrents1 = bunchCurrents1 + bh.bcidDict[key].b1Curr[bcid]
            elif (bcid-1) in bcidList[latestLb]:
                bunchCurrents1 = bunchCurrents1 + bh.bcidDict[key].b1Curr[bcid]
        lbDict1[bh.bcidDict[key].runLB.lb] = bunchCurrents1

        # beam 2
        bunchCurrents2 = 0.
        for bcid in bh.bcidDict[key].b2Curr.keys():
            # account for the BCT phase set up wrongly, hence try also BCID+1, -1
            if bcid in bcidList[latestLb]:
                bunchCurrents2 = bunchCurrents2 + bh.bcidDict[key].b2Curr[bcid]
            elif (bcid+1) in bcidList[latestLb]:
                bunchCurrents2 = bunchCurrents2 + bh.bcidDict[key].b2Curr[bcid]
            elif (bcid-1) in bcidList[latestLb]:
                bunchCurrents2 = bunchCurrents2 + bh.bcidDict[key].b2Curr[bcid]
        lbDict2[bh.bcidDict[key].runLB.lb] = bunchCurrents2

#       print bh.bcidDict[key].runLB.lb,":",bh.bcidDict[key].specLumi
#        print bh.bcidDict[key].runLB.lb,":",bh.bcidDict[key].b2Curr

    return (lbDict1,lbDict2)


#     bcidData = LumiBCIDData()
#     folder=db.getFolder(foldername)
#     try:
#         itr=folder.browseObjects(startRun,stopRun,cool.ChannelSelection.all())
#         while itr.goToNext() :
#             bcidData.clear()
#             obj=itr.currentRef()
#             # channel 1 is fast BCT
#             if not obj.channelId() == 1: continue
#             payload=obj.payload()
#             lbCool = payload['RunLB'] & 0xffff
#             if lbCool == 0: continue
#             bcidData.fillCurrents(payload)
#             latestLb = 1
#             for lb in bcidList.keys():
#                 if lbCool >= lb:
#                     latestLb = lb


#             if lbCool == 200:
#                 print bcidData.b1Curr
#             bunchCurrents1 = 0.
#             for bcid in bcidData.b1Curr.keys():
#                 # account for the BCT phase set up wrongly, hence try also BCID+1, -1
#                 if bcid in bcidList[latestLb]:
#                     bunchCurrents1 = bunchCurrents1 + bcidData.b1Curr[bcid]
#                 if (bcid+1) in bcidList[latestLb]:
#                     bunchCurrents1 = bunchCurrents1 + bcidData.b1Curr[bcid]
#                 if (bcid-1) in bcidList[latestLb]:
#                     bunchCurrents1 = bunchCurrents1 + bcidData.b1Curr[bcid]
#             lbDict1[lbCool] = bunchCurrents1
#             bunchCurrents2 = 0.
#             #print bcidData.b2Curr
#             for bcid in bcidData.b2Curr.keys():
#                 # account for the BCT phase set up wrongly, hence try also BCID+1, -1
#                 if bcid in bcidList[latestLb]:
#                     bunchCurrents2 = bunchCurrents2 + bcidData.b2Curr[bcid]
#                 if (bcid+1) in bcidList[latestLb]:
#                     bunchCurrents2 = bunchCurrents2 + bcidData.b2Curr[bcid]
#                 if (bcid-1) in bcidList[latestLb]:
#                     bunchCurrents2 = bunchCurrents2 + bcidData.b2Curr[bcid]
#             lbDict2[lbCool] = bunchCurrents2


#         itr.close()
#     except Exception,e:
#         print "Reading data from",foldername,"failed:",e

def runStartStopTime(run):
    
    """ Retrieve start and stop time of the given run """
    db = openDb(False,True)
    foldername = "/TDAQ/RunCtrl/EOR_Params"
    if not db.existsFolder(foldername) :
        print "Folder",foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(foldername)
    try:
        itr=folder.browseObjects(run<<32, (run<<32)+0xFFFFFFFF ,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            payload=obj.payload()
            runCool = payload['RunNumber']
            startTime = payload['SORTime']
            stopTime = payload['EORTime']
#            print "Searched run",run,"found run",runCool,"start",startTime,"stop",stopTime
        itr.close()
    except Exception,e:
        print "Reading data from",foldername,"failed:",e

    db.closeDatabase()
    return startTime, stopTime

def runLbToTime(run,lb):

    """ Convert Run-LB pair to start-endtime pair """

    db = openDb()
    start = 0
    end = 0
    foldername='/TRIGGER/LUMI/LBLB'
    if not db.existsFolder(foldername) :
        print "Folder",foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(foldername)
    try:
        itr=folder.browseObjects((run << 32)+lb,(run << 32)+lb,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            payload=obj.payload()
            start = payload['StartTime']
            end = payload['EndTime']
            
        itr.close()
    except Exception,e:
        print "Reading data from",foldername,"failed:",e

    return (start, end)

def getLbTimes(run):

    """ For given run return all  start-endtime pairs in a dict, indexed by lumi block """

    db = openDb()
    lbDict = {}
    foldername='/TRIGGER/LUMI/LBLB'
    if not db.existsFolder(foldername) :
        print "Folder",foldername,"not found"
        db.closeDatabase()
        sys.exit(-1)

    folder=db.getFolder(foldername)
    try:
        itr=folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())
        while itr.goToNext() :
            obj=itr.currentRef()
            payload=obj.payload()
            since = int(obj.since() & 0xffff)
            start = payload['StartTime']
            end = payload['EndTime']
            lbDict[since] = (start, end)
        itr.close()
    except Exception,e:
        print "Reading data from",foldername,"failed:",e

    db.closeDatabase()
    return lbDict
