# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Elliot Lipeles, Univ. of Pennsylvania (2011)
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Read Rates and trigger config from COOL
#
# Utility functions
# - IncludeLB(lb, lbbeg, lbend)
# - InitDB()
# - CheckFolder(foldername)
# - GetFolderItrForRun(foldername, run)
#
# Luminosity block info -- returns LumiBlockSet()
# - GetLumiblocks(runnumber, lb_beg, lb_end)
#
# COOL configuration -- 'Menu', 'PrescalesKey', 'Prescale' folders
# - GetConfig(runnumber)
#
# COOL rates
# - GetRates(runnumber, lb_beg, lb_end)
# - GetHLTRates(runnumber, config, lbset, lb_beg, lb_end, lvl)
# - FillL1Chain(chain, lb, itemNo, payload, config, lbset)
# - FillHLTChain(chain, pos, blob, lb, lvl, config, lbset)
# - getEntryCounter(pos, buf)   <-- HLT blob decoder (Joerg Stelzer)
# - getCounts(pos, buf)         <-- ''
# - bConvert(b)                 <-- Bunch blob decoder (Eric Torrence)
#
# Examples:
"""
ratecoll = GetRates(runnumber, lb_beg, lb_end, options)
"""

#print 'Hello world TrigCostCool'

import sys
import os
import math
import traceback
import logging
#	logging.basicConfig(level=logging.INFO)
log = logging.getLogger('TrigCostCool')
#print '\tLoaded standard packages'

from PyCool import cool                                 ;# print '\tLoaded special PyCool.cool package'
from PyCool import coral                                ;# print '\tLoaded special PyCool.coral package'
from CoolConvUtilities.AtlCoolLib import indirectOpen   ;# print '\tLoaded special CoolConvUtilitie.AtlCoolLib import indirectOpen package'
from CoolConvUtilities import AtlCoolTool               ;# print '\tLoaded special CoolConvUtilities.AtlCoolTool package'

# for decoding blob
from collections import namedtuple                      ;# print '\tLoaded special collections package'
from struct import unpack_from, unpack                  ;# print '\tLoaded special struct package'
from TrigCostAnalysis import *                          ;# print '\tLoaded special TrigCostAnalysis.* package'
#print '\tDone prelim import for TrigCostCool'

#------------------------------------------------------------
#
# Folder names
#   /TRIGGER/ in https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData
#   /TDAQ/    in https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData
#   /LHC/     in https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LuminosityOnlineCool
#
#------------------------------------------------------------
bgcontent_foldername    = '/TRIGGER/LVL1/BunchGroupContent'     # Idx by run-LB
bgdesc_foldername       = '/TRIGGER/LVL1/BunchGroupDescription' # Idx by run-LB
lumi_foldername         = '/TDAQ/OLC/LUMINOSITY'                # Idx by timestamp
lblb_foldername         = '/TRIGGER/LUMI/LBLB'                  # Idx by run-LB
lbtime_foldername       = '/TRIGGER/LUMI/LBTIME'                # Idx by timestamp, same as lblb
lvl1lbdata_foldername   = '/TRIGGER/LVL1/CTPCORELBDATA'         # Idx by run-LB
fillparams_foldername   = '/TDAQ/OLC/LHC/FILLPARAMS'            # Idx by timestamp
bunchlumis_foldername   = '/TDAQ/OLC/BUNCHLUMIS'                # Idx by timestamp
lhcdb_foldername        = '/LHC/DCS/FILLSTATE'                  # Idx by timestamp
lvl1menu_foldername     = '/TRIGGER/LVL1/Menu'                  # Idx by run-LB
hltmenu_foldername      = '/TRIGGER/HLT/Menu'                   # Idx by run-LB
l1ps_foldername         = '/TRIGGER/LVL1/Prescales'             # Idx by run-LB
hltps_foldername        = '/TRIGGER/HLT/Prescales'              # Idx by run-LB
l1pskey_foldername      = '/TRIGGER/LVL1/Lvl1ConfigKey'         # Idx by run-LB
#hltpskey_foldername      = '/TRIGGER/HLT/HltConfigKey'         # Idx by run-LB
hltpskey_foldername     = '/TRIGGER/HLT/PrescaleKey'            # Idx by run-LB

#------------------------------------------------------------
# Verbosity
#------------------------------------------------------------
def SetLogging(options=[]):
    if   "silent" in options:
        logging.basicConfig(level=logging.ERROR)
    elif "quiet"  in options:
        logging.basicConfig(level=logging.WARNING)
    elif "debug"  in options:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    return

#------------------------------------------------------------
#
# Lumiblock range control
#
#------------------------------------------------------------
def IncludeLB(lb,lbbeg,lbend):
    if lbbeg==-1 and lbend==-1:
        return True
    if lb == 0: return False # LB starts at 1 -- tmhong
    if lb < 0:
        return False
    if lbbeg >= 0 and lb < lbbeg:
        return False
    if lbend >= 0 and lb > lbend:
        return False
    return True

#------------------------------------------------------------
#
# Check database service and folders
#
#------------------------------------------------------------
dbProd=None
dbLhc=None
dbMonp=None
dbComp=None
dbTrig=None
def CloseDB(foldertype='ALL'):
    procAll = foldertype=='ALL'
    if (foldertype=="PROD"    or procAll) and dbProd: dbProd.closeDatabase()
    if (foldertype=="LHC"     or procAll) and dbLhc : dbLhc .closeDatabase()
    if (foldertype=="MONP"    or procAll) and dbMonp: dbMonp.closeDatabase()
    if (foldertype=="COMP"    or procAll) and dbComp: dbComp.closeDatabase()
    if (foldertype=="TRIGGER" or procAll) and dbTrig: dbTrig.closeDatabase()
    return

def InitDB(foldertype='TRIGGER'):

    # "PROD" option added -- for bunch groups
    if foldertype=="PROD":
        global dbProd
        if dbProd:
            return #already initialized

        try:
            #dbProd = indirectOpen('COOLONL_TRIGGER/COMP200', oracle=True)
            dbProd = indirectOpen('COOLONL_TRIGGER/CONDBR2', oracle=True)
            log.info("Connected to database: "+'COOLONL_TRIGGER/CONDBR2')
        except Exception.e:
            log.error('Error connecting to database:'+str(e))
            sys.exit(-1)
        return


    # "LHC" option added
    if foldertype=="LHC":
        global dbLhc
        if dbLhc:
            return #already initialized

        try:
            dbLhc = indirectOpen('COOLOFL_DCS/CONDBR2', oracle=True)
            log.info("Connected to database: "+'COOLOFL_DCS/CONDBR2')
        except Exception.e:
            log.error('Error connecting to database:'+str(e))
            sys.exit(-1)
        return

    # "MONP" option added ("TDAQ" --> "COMP")
    if foldertype=="MONP":
        global dbMonp
        if dbMonp:
            return #already initialized

        try:
            dbMonpString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
            dbMonp = indirectOpen('COOLONL_TDAQ/MONP200', oracle=True) # last two = oracle, debug
            log.info("Connected to database: "+dbMonpString)
        except Exception,e:
            log.error('Error connecting to database:'+str(e))
            sys.exit(-1)
        return

    # "COMP" option added
    if foldertype=="COMP":
        global dbComp
        if dbComp:
            return #already initialized

        try:
            dbCompString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=CONDBR2;user=ATLAS_COOL_READER;password=COOLRED4PRO"
            dbComp = indirectOpen('COOLONL_TDAQ/CONDBR2', oracle=True)
            log.info("Connected to database: "+dbCompString)
        except Exception,e:
            log.error('Error connecting to database:'+str(e))
            sys.exit(-1)
        return

    # Default is TRIGGER
    global dbTrig
    if dbTrig:
        return #already initialized

    dbSvc=cool.DatabaseSvcFactory.databaseService()
    dbTrigString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2;user=ATLAS_COOL_READER;password=COOLRED4PRO"

    try:
        dbTrig=dbSvc.openDatabase(dbTrigString, False)
        log.info("OpenRed database: "+dbTrigString)
    except Exception,e:
        log.error('Error connecting to database:'+str(e))
        sys.exit(-1)


#------------------------------------------------------------
def CheckFolder(foldername,foldertype="TRIGGER"):

    # "PROD" option added -- for bunch groups
    if foldertype=="PROD":
        if not dbProd.existsFolder(foldername) :
            print "Folder",foldername,"not found in PROD"
            dbProd.closeDatabase()
            sys.exit(-1)
        return

    # "LHC" option added
    if foldertype=="LHC":
        if not dbLhc.existsFolder(foldername) :
            print "Folder",foldername,"not found in LHC"
            dbLhc.closeDatabase()
            sys.exit(-1)
        return

    # "MONP" option added ("TDAQ" --> "COMP")
    if foldertype=="MONP":
        if not dbMonp.existsFolder(foldername) :
            print "Folder",foldername,"not found in MONP"
            dbMonp.closeDatabase()
            sys.exit(-1)
        return

    # "COMP" option added
    if foldertype=="COMP":
        if not dbComp.existsFolder(foldername) :
            print "Folder",foldername,"not found in COMP"
            dbComp.closeDatabase()
            sys.exit(-1)
        return

    # Default is TRIGGER
    if not dbTrig.existsFolder(foldername) :
        print "Folder",foldername,"not found in TRIGGER"
        dbTrig.closeDatabase()
        sys.exit(-1)

#------------------------------------------------------------
def GetFolderItrForRun(foldername, run, foldertype='TRIGGER', run_beg_time=-1, run_end_time=-1, channel=0):

    logging.debug('foldername:'+foldername)
    logging.debug('foldertype:'+foldertype)
    
    print "Getting DB for ",foldername,foldertype

    # "PROD" option added -- for bunch groups
    if foldertype=="PROD":
        InitDB(foldertype)
        CheckFolder(foldername, foldertype)
        folder=dbProd.getFolder(foldername)
        log.info('Opening folder: %s' % foldername)
        return folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())

    # "LHC" option added
    if foldertype=="LHC":
        InitDB(foldertype)
        CheckFolder(foldername, foldertype)
        folder=dbLhc.getFolder(foldername)
        log.info('Opening folder: %s' % foldername)
        return folder.browseObjects(run_beg_time,run_end_time,cool.ChannelSelection.all())

    # "MONP" option added ("TDAQ" --> "COMP")
    # NB. Channel 102 is LUCID_ZERO_AND -- used for "MONP" here
    # NB. Channel 0 is ATLAS_PREFERRED -- used for "COMP" below
    if foldertype=="MONP":
        InitDB(foldertype)
        CheckFolder(foldername, foldertype)
        folder=dbMonp.getFolder(foldername)
        log.info('Folder %s opening' % foldername)
        return folder.browseObjects(run_beg_time,run_end_time,cool.ChannelSelection(102))

    # "COMP" option added
    #
    # https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_BUNCHLUMIS
    #
    # ---- See this email from Eric ----
    # From: torrence@uoregon.edu on Sun, Sep 25, 2011 at 14:04
    # Hi Tae, During the technical stop, it (BUNCHLUMIS) was moved
    # from MONP200 to CONDBR2.  A few channels were also backfilled in
    # CONDBR2 back to the start of 2011, so that you can run over the
    # entire year.  The channel 0 designation is the channel chosen as
    # the 'preferred' channel.  This is actually the best thing to
    # use.  The hardcoded 201 in my script is a bit historical, and I
    # should probably change it.  In practice, the preferred channel
    # has been 201 all year, so it hasn't made a difference.
    #
    log.debug('Folder %s opening' % foldername)
    if foldertype=="COMP":
        InitDB(foldertype)
        CheckFolder(foldername, foldertype)
        folder=dbComp.getFolder(foldername)
        return folder.browseObjects(run_beg_time,run_end_time,cool.ChannelSelection(channel))

    InitDB(foldertype)
    logging.info('Initialized DB %s' % foldertype)
    CheckFolder(foldername)
    logging.info('Checked folder %s' % foldername)
    folder=dbTrig.getFolder(foldername)
    if foldertype=="LBTIME":
        return folder.browseObjects(run_beg_time,run_end_time,cool.ChannelSelection.all())

    # Default is TRIGGER
    if foldertype != "TRIGGER":
        print 'ERROR - default foldertype should be TRIGGER'
        return None

    return folder.browseObjects(run << 32,((run+1) << 32),cool.ChannelSelection.all())


#------------------------------------------------------------
def UnpackBCIDData(StartTime, EndTime, payload):
    ncol = payload['LuminousBunches']   # length
    nb1  = payload['Beam1Bunches']      # length
    nb2  = payload['Beam2Bunches']      # length
    blob = payload['BCIDmasks']

    colBCID = bSet(blob, ncol)  # colliding bunches data
    b1BCID  = bSet(blob, nb1)   # beam1 bunches data
    b2BCID  = bSet(blob, nb2)   # beam2 bunches data

    bcidData = dict()
    bcidData['StartTime']   = StartTime
    bcidData['EndTime']     = EndTime
    bcidData['ncol']        = ncol
    bcidData['nb1']         = nb1
    bcidData['nb2']         = nb2
    bcidData['colBCID']     = colBCID
    bcidData['b1BCID']      = b1BCID
    bcidData['b2BCID']      = b2BCID
    bcidData['allBCID']     = b1BCID | b2BCID # merge beam1, beam2
    return bcidData


#------------------------------------------------------------
def UnpackFillData(StartTime, EndTime, payload):
    beam    = payload['BeamMode']
    fill    = 'None'
    ebeam   = 'None'
    if (beam == 'NO BEAM') : # Cosmics?
        fill    = 0
        ebeam   = 0.
    else : 
        fill    = payload['FillNumber']
        ebeam   = payload['BeamEnergyGeV']
    stable  = payload['StableBeams']
    lhc     = payload['MachineMode']

    fillData = dict()
    fillData['StartTime']   = StartTime
    fillData['EndTime']     = EndTime
    fillData['fill']        = fill
    fillData['stable']      = stable
    fillData['lhc']         = lhc
    fillData['beam']        = beam
    fillData['ebeam']       = ebeam
    return fillData


#------------------------------------------------------------
def UnpackRunData(StartTime, EndTime, lbbeg, lbend, run):
    runData = dict()
    runData['StartTime']    = StartTime
    runData['EndTime']      = EndTime
    runData['run']          = run
    runData['lbbeg']        = lbbeg
    runData['lbend']        = lbend
    return runData


#------------------------------------------------------------
#
# Luminosity block info
#
# returns LumiBlockSet
#------------------------------------------------------------
LumiblockSetCache=None
LumiblockSetCacheRun=-1
LumiblockSetCacheLbRange=[-1,-1]
def GetLumiblocks(runnumber,lb_beg,lb_end,options=[]):
    SetLogging(options)

    log.info(" #")
    log.info(" #")
    log.info(" # Start GetLumiblocks()")
    log.info(" #")
    log.info(" #")
    log.info("Getting luminosity info for run=%d, %d <= LB <= %d" % (runnumber, lb_beg, lb_end))

    # check cache
    global LumiblockSetCache,LumiblockSetCacheRun,LumiblockSetCacheLbRange
    if LumiblockSetCacheRun==runnumber and \
       LumiblockSetCacheLbRange[0] == lb_beg and \
       LumiblockSetCacheLbRange[1] == lb_end:
        return LumiblockSetCache

    # puts the data here
    lbset = LumiBlockSet()

    #
    # Get time info
    #
    run_beg_time = -1
    run_end_time = -1
    log.debug('#')
    log.debug('# %s = %s loading'%('lblb_foldername', lblb_foldername))
    log.debug('#')
    try:
        itr = GetFolderItrForRun(lblb_foldername,runnumber) # Indexed by run-LB
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            payload = obj.payload()

            if not IncludeLB(lb,lb_beg,lb_end):
                continue

            if lb not in lbset.lbs:
                lbset.lbs[lb] = LumiBlock()
                if lbset.lbbeg==-1:
                    lbset.lbbeg=lb
            else:
                print "%d LB? Why isn't it already there?" % lb
            #lbset.lbs[lb] = LumiBlock() # Looks redundant TMH

            lbData          = lbset.lbs[lb]
            lbData.beg_time = payload['StartTime']
            lbData.end_time = payload['EndTime']
            lbData.lb       = lb
            lbset.lbend=lb

            if run_beg_time < 0 or lbData.beg_time < run_beg_time:
                run_beg_time = lbData.beg_time

            if run_end_time < 0 or lbData.end_time > run_end_time:
                run_end_time = lbData.end_time

        itr.close()
    except Exception,e:
        log.error('Reading data from '+lblbdata_foldername+' failed: '+str(e))


    # These should be set
    if run_beg_time==-1 or run_end_time==-1:
        log.error('Call GetLumiblocks() to set start & end times, exiting()')
        exit(-1)


    #
    # Get Global Counter Info
    #
    log.debug('#')
    log.debug('# %s = %s loading' %('lvl1lbdata_foldername', lvl1lbdata_foldername))
    log.debug('#')
    try:
        itr = GetFolderItrForRun(lvl1lbdata_foldername,runnumber)
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            itemNo  = obj.channelId()
            payload = obj.payload()

            if not IncludeLB(lb,lb_beg,lb_end):
                continue

            if lb not in lbset.lbs:
                print "2: LB? Why isn't it already there?"
                lbset.lbs[lb] = LumiBlock()

            lbData           = lbset.lbs[lb]
            lbData.lb        = lb
            lbData.l1_accept = payload['GlobalL1AcceptCounter']
            lbData.l1a_turn  = payload['GlobalL1AcceptTurnCounter']
            lbData.tav_turn  = payload['L1AcceptTurnCounter']
            lbData.tap_turn  = payload['AfterPrescaleTurnCounter']
            lbData.tbp_turn  = payload['BeforePrescaleTurnCounter']

            #print lb,lbData.tap_turn,lbData.tav_turn,lbData.tbp_turn,lbset.lbs[lb].GetDT(), \
            #lbData.tbp_turn/LumiBlock.orbitFreq, \
            #(lbData.tbp_turn/LumiBlock.orbitFreq)/lbset.lbs[lb].GetDT(), \
            #float(lbData.tap_turn)/lbData.tbp_turn,float(lbData.tav_turn)/lbData.tap_turn


        itr.close()
    except Exception,e:
        log.error('Reading data from '+lvl1lbdata_foldername+' failed: '+str(e))

    #
    # Get bunch info -- do before luminosity info.... I think
    #
    # Read the bunch information: Because this isn't synchronized with
    # our LB data, first store everything, then we will match it to
    # LB-based IOVs.  We loosely follow Eric Torrence's loadBCIDMask()
    # in AtlDataSumLumiBCID.py
    #
    maskList = []
    log.debug('#')
    log.debug('# %s = %s loading' %('fillparams_foldername', fillparams_foldername))
    log.debug('#')
    try:
        idx = -1
        itr = GetFolderItrForRun(fillparams_foldername,runnumber,'COMP',run_beg_time,run_end_time)
        while itr.goToNext():
            idx += 1
            obj       = itr.currentRef()
            StartTime = obj.since()
            EndTime   = obj.until()
            payload   = obj.payload()

            # no lumiblock range implementation -- they're timestamped!

            ncol = payload['LuminousBunches']   # length
            nb1  = payload['Beam1Bunches']      # length
            nb2  = payload['Beam2Bunches']      # length
            blob = payload['BCIDmasks']         # masked data
            beg_ = AtlCoolTool.time.ctime(StartTime/1.0E9)
            end_ = AtlCoolTool.time.ctime(EndTime/1.0E9)
            log.debug('%s APPEND=%d (%s)--(%s) ncol=%d nb1=%d nb2=%d' %
                      (fillparams_foldername,idx,beg_,end_,ncol,nb1,nb2))

            maskList.append( UnpackBCIDData(StartTime, EndTime, payload) )

        itr.close()
    except Exception,e:
        log.error('Reading data from '+fillparams_foldername+' failed: '+str(e))

    #
    # Read LHC fill number -- timestamped, so save later
    #
    fillList = []
    log.debug('#')
    log.debug('# %s = %s loading' %('lhcdb_foldername', lhcdb_foldername))
    log.debug('#')
    try:
        itr = GetFolderItrForRun(lhcdb_foldername, runnumber, 'LHC', run_beg_time, run_end_time)
        while itr.goToNext() :
            obj         = itr.currentRef()
            StartTime   = obj.since()
            EndTime     = obj.until()
            payload     = obj.payload()

            # no lumiblock range implementation -- they're timestamped!
            beam    = payload['BeamMode']
            fill    = 'None'
            ebeam   = 'None'
            if (beam == 'NO BEAM') : # Cosmics?
                fill    = 0
                ebeam   = 0.
            else : 
                fill    = payload['FillNumber']
                ebeam   = payload['BeamEnergyGeV']

            stable  = payload['StableBeams']
            lhc     = payload['MachineMode']

            beg_    = 'None'
            end_    = 'None'
            try:
                beg_    = AtlCoolTool.time.ctime(StartTime/1.0E9)
                end_    = AtlCoolTool.time.ctime(EndTime/1.0E9)
            except:
                pass

            log.debug('time=%s,%s fill=%d stable=%s lhc=%s beam=%s ebeam=%f' %
                      (beg_,end_,fill,stable,lhc,beam,ebeam))

            # Protection against bogus values
            iserr = False
            if fill  ==None: fill=0      ; iserr=iserr and True; log.error('NULL FillNumber')
            if stable==None: stable=False; iserr=iserr and True; log.error('NULL StableBeams')
            if lhc   ==None: lhc=''      ; iserr=iserr and True; log.error('NULL MachineMode')
            if beam  ==None: beam=''     ; iserr=iserr and True; log.error('NULL BeamMode')
            if ebeam ==None: ebeam= 0    ; iserr=iserr and True; log.error('NULL BeamEnergyGeV')
            if iserr:
                log.error('fill=%d time=%s,%s stable=%s lhc="%s" beam="%s" ebeam=%f' %
                          (fill,beg_,end_,stable,lhc,beam,ebeam))

            # Add fill
            fillList.append( UnpackFillData(StartTime, EndTime, payload) )

        itr.close()
    except Exception,e:
        log.error('Reading data from '+lhcdb_foldername+' failed: '+str(e))
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    #
    # Load bunch luminosity information
    #
    log.debug('#')
    log.debug('# %s = %s loading' %('bunchlumis_foldername', bunchlumis_foldername))
    log.debug('#')
    try:
        itr = None
        if runnumber < 188902:
            # Before the technical stop ending Sept 7, 2011
            itr = GetFolderItrForRun(bunchlumis_foldername, runnumber, 'MONP', run_beg_time, run_end_time)
        else:
            # Before the technical stop ending Sept 7, 2011
            itr = GetFolderItrForRun(bunchlumis_foldername, runnumber, 'COMP', run_beg_time, run_end_time, 201)

        while itr.goToNext():
            obj       = itr.currentRef()
            StartTime = obj.since()
            EndTime   = obj.until()
            payload   = obj.payload()

            run = payload['RunLB'] >> 32
            lb  = payload['RunLB'] & 0xFFFFFFFF

            # lumiblock range implementation
            if not IncludeLB(lb,lb_beg,lb_end):
                continue

            # Find lb in our timestamp list
            idx = -1
            for bcidIdx, bcidData in enumerate(maskList):
                if bcidData == None:
                    log.debug('%s SKIPPING %d'%(bunchlumis_foldername,bcidIdx))
                    continue

                bcidStart = bcidData['StartTime']
                bcidEnd   = bcidData['EndTime']
                if bcidStart <= StartTime and EndTime <= bcidEnd:
                    if idx != -1:
                        log.error('Got %d. Already found idx=%d. What is going on?',bcidIdx,idx)

                    idx = bcidIdx

            if idx > -1:
                thisData = maskList[idx]
                lbData          = lbset.lbs[lb]
                lbData.ncol     = thisData['ncol']
                lbData.nb1      = thisData['nb1']
                lbData.nb2      = thisData['nb2']
                lbData.colBCID  = thisData['colBCID']
                lbData.b1BCID   = thisData['b1BCID']
                lbData.b2BCID   = thisData['b2BCID']
                lbData.allBCID  = thisData['allBCID']

            beg_ = 'None'
            end_ = 'None'
            try:
                beg_ = AtlCoolTool.time.ctime(StartTime/1.0E9)
                end_ = AtlCoolTool.time.ctime(EndTime/1.0E9)
            except:
                pass

            log.debug('%s idx=%d lb=%d (%s)--(%s) ncol=%d'%
                      (bunchlumis_foldername,idx,lb,beg_,end_,lbData.ncol))

            if idx==-1:
                log.warning('%s not found for run=%d lb=%d'%(bunchlumis_foldername,run,lb))

            if lbData.ncol!=len(lbData.colBCID):
                log.error('%s MISMATCH lb=%d ncol=%d != %d'%
                          (bunchlumis_foldername,lb,lbData.ncol,len(lbData.colBCID)))

            # Fill: Find lb in our timestamp list
            idx = -1
            for fillIdx, fillData in enumerate(fillList):
                if idx > -1:
                    break

                fillStart = fillData['StartTime']
                fillEnd   = fillData['EndTime']
                if fillStart <= StartTime:
                    idx = fillIdx

            if idx > -1:
                thisData = fillList[idx]
                lbData          = lbset.lbs[lb]
                lbData.fill     = thisData['fill']
                lbData.stable   = thisData['stable']
                lbData.lhc      = thisData['lhc']
                lbData.beam     = thisData['beam']
                lbData.ebeam    = thisData['ebeam']

            log.debug('%s idx=%d lb=%d (%s)--(%s) fill=%d'%
                      (bunchlumis_foldername,idx,lb,beg_,end_,lbData.fill))

            if idx==-1:
                log.error('%s Fill NOT FOUND lb=%d'%(bunchlumis_foldername,lb))

        itr.close()
    except Exception,e:
        log.error('Reading data from '+bunchlumis_foldername+' failed: '+str(e))
        print 'error'

    #
    # Read luminosity information
    #
    # db needs time before first block starts
    log.debug('#')
    log.debug('# %s = %s loading' %('lumi_foldername', lumi_foldername))
    log.debug('#')
    try:
        itr = GetFolderItrForRun(lumi_foldername, runnumber, 'COMP', run_beg_time, run_end_time)
        while itr.goToNext():
            obj     = itr.currentRef()
            payload = obj.payload()

            run = payload['RunLB'] >> 32
            lb  = payload['RunLB'] & 0xffff

            if not IncludeLB(lb,lb_beg,lb_end): continue
            if run != runnumber:
                log.warning('Skipping run mismatch in lumi: '+str(run)+'!='+str(runnumber))
                continue

            if lb not in lbset.lbs:
                log.warning('Found lumi for out of range lumiblock:'+str(lb))
                lbset.lbs[lb] = LumiBlock()

            lbset.lbs[lb].avr_lumi = payload['LBAvInstLumPhys']
            lbset.lbs[lb].avr_evts = payload['LBAvEvtsPerBXPhys']

            log.debug('New lumi block: %d'%lb+lbset.lbs[lb].AsString())

        itr.close()
    except Exception,e:
        log.error('Reading data from '+lumi_foldername+' failed: '+str(e))

    ###########################################################################
    #
    # Bunch group information
    #
    ###########################################################################

    #
    # Read bunch group names for 1--8
    #
    bgNamesCollection = []
    log.debug('#')
    log.debug('# %s = %s loading' %('bgdesc_foldername', bgdesc_foldername))
    log.debug('#')
    try:
        itr = GetFolderItrForRun(bgdesc_foldername, runnumber, 'PROD')
        while itr.goToNext() :
            obj       = itr.currentRef()
            payload   = obj.payload()
            lb        = (obj.since() & 0xffff)
            bgNames = []
            bgNames.append(str(payload['BunchGroup0']))
            bgNames.append(str(payload['BunchGroup1']))
            bgNames.append(str(payload['BunchGroup2']))
            bgNames.append(str(payload['BunchGroup3']))
            bgNames.append(str(payload['BunchGroup4']))
            bgNames.append(str(payload['BunchGroup5']))
            bgNames.append(str(payload['BunchGroup6']))
            bgNames.append(str(payload['BunchGroup7']))
            bgNamesCollection.append( bgNames )

        itr.close()
    except Exception,e:
        log.error('Reading data from '+bgdesc_foldername+' failed: '+str(e))

    #
    # Read bunch group content
    #
    # List is only filled for the first LB whenever a new config is loaded!
    #
    # From Trigger/TrigT1/TrigT1CTMonitoring/scripts/read_bunchgroups.py
    #
    # Constants
    nbunchgroup = 8
    nbcid = 3564
    bgListCollection = []
    bgLengthCollection = []
    bgStartLBCollection = []
    log.debug('#')
    log.debug('# %s = %s loading' %('bgcontent_foldername', bgcontent_foldername))
    log.debug('#')
    try:
        idx = 0
        itr = GetFolderItrForRun(bgcontent_foldername, runnumber, 'PROD')
        while itr.goToNext() :
            obj       = itr.currentRef()
            payload   = obj.payload()
            lb        = (obj.since() & 0xffff)
            blb       = payload['BunchCode']
            bgCont    = blb.startingAddress()
            bgCont.SetSize( blb.size() )
            bgList = []
            for _ in range(nbunchgroup): bgList.append([])
            for bcid in range(nbcid):
                for bgrp in range(nbunchgroup):
                    if(bgCont[bcid]>>bgrp & 1):
                        bgList[bgrp].append(bcid)
            bgListCollection.append( bgList )

            # It seems that bg names are only defined for the first lb.
            bgNames = bgNamesCollection[0]
            try:
                # But we'll try anyway
                bgNames = bgNamesCollection[idx]
            except:
                pass

            # Notice
            if idx > 0:
                log.info("*** BG changed on lb=%d! ***" % lb)

            # Get bg info
            bgLength = []
            for grp in range(nbunchgroup):
                bgLen = len(bgList[grp])
                bgPrintLen = 10 if bgLen > 10 else bgLen
                log.info("lb=%-4d BG%d=%-20s size=%-6d in BCID=%s%s" %
                         (lb,grp,bgNames[grp],bgLen,bgList[grp][:bgPrintLen],('...' if bgLen > 15 else '')))
                bgLength.append(bgLen)

            bgLengthCollection.append( bgLength )
            bgStartLBCollection.append( lb )

            idx += 1
            
        itr.close()
    except Exception,e:
        log.error('Reading data from '+bgdesc_foldername+' failed: '+str(e))

    # Save bunch group stuff per lb
    idx = 0
    lenBg = len(bgStartLBCollection)
    for lb in xrange(lbset.lbbeg, lbset.lbend+1):

        # By default we're always in the first idx
        saveidx = 0
        if lenBg > 1:
            for i in xrange(1, lenBg):
                if lb >= bgStartLBCollection[i]:
                    saveidx = i
                    break

        bgStartLB   = bgStartLBCollection   [saveidx]  # list of starting LB
        bgList      = bgListCollection      [saveidx]  # list of BCID
        bgLength    = bgLengthCollection    [saveidx]  # list of len(BCID)
        bgNames     = bgNamesCollection     [0]        # list of BG names
        try:
            bgNames = bgNamesCollection     [saveidx]
        except:
            pass

        lbset.lbs[lb].bgStartLB = bgStartLB
        lbset.lbs[lb].bgBCIDList0 = bgList[0]
        lbset.lbs[lb].bgBCIDList1 = bgList[1]
        lbset.lbs[lb].bgBCIDList2 = bgList[2]
        lbset.lbs[lb].bgBCIDList3 = bgList[3]
        lbset.lbs[lb].bgBCIDList4 = bgList[4]
        lbset.lbs[lb].bgBCIDList5 = bgList[5]
        lbset.lbs[lb].bgBCIDList6 = bgList[6]
        lbset.lbs[lb].bgBCIDList7 = bgList[7]
        lbset.lbs[lb].bgLength0 = bgLength[0]
        lbset.lbs[lb].bgLength1 = bgLength[1]
        lbset.lbs[lb].bgLength2 = bgLength[2]
        lbset.lbs[lb].bgLength3 = bgLength[3]
        lbset.lbs[lb].bgLength4 = bgLength[4]
        lbset.lbs[lb].bgLength5 = bgLength[5]
        lbset.lbs[lb].bgLength6 = bgLength[6]
        lbset.lbs[lb].bgLength7 = bgLength[7]
        lbset.lbs[lb].bgName0 = bgNames[0]
        lbset.lbs[lb].bgName1 = bgNames[1]
        lbset.lbs[lb].bgName2 = bgNames[2]
        lbset.lbs[lb].bgName3 = bgNames[3]
        lbset.lbs[lb].bgName4 = bgNames[4]
        lbset.lbs[lb].bgName5 = bgNames[5]
        lbset.lbs[lb].bgName6 = bgNames[6]
        lbset.lbs[lb].bgName7 = bgNames[7]

 
    # save to cache
    LumiblockSetCache=lbset
    LumiblockSetCacheRun=runnumber
    LumiblockSetCacheLbRange=[lb_beg,lb_end]

    return lbset

# TimM - Import prescale conversion
maxPrescaleCut = 0xFFFFFF; #2**24 - 1
   
#@brief calculate cut value for hardware configuration
#   cut = 2*24/prescale - 1
#   
#   prescale =     1 --> C = 2*24-1
#   prescale =     2 --> C = 2*23-1
#   prescale =    10 --> C = 1677720
#   prescale =  1024 --> C = 2*14-1
#   prescale =    50 --> C = 335543
#   prescale =   500 --> C = 33553
#   prescale =  5000 --> C = 3354
#   prescale = 50000 --> C = 334
def getCutFromPrescale(prescale):
    sign = -1 if prescale < 0 else 1
    uprescale = float(abs(prescale))
    return sign * (  0x1000000 - int( 0xFFFFFF/uprescale )  )

#   prescale = 2*24/(cut+1.)
#
#   cut = 2*24-1  --> prescale =     1
#   cut = 2*23-1  --> prescale =     2
#   cut = 1677720 --> prescale =    10.000002980233305
#   cut = 2*14-1  --> prescale =  1024
#   cut = 335543  --> prescale =    50.0000447035
#   cut = 33553   --> prescale =   500.006407582 
#   cut = 3354    --> prescale =  5000.66020864  
#   cut = 334     --> prescale = 50081.238806    
def getPrescaleFromCut(cut):
    sign = -1 if cut < 0 else 1
    ucut = int(abs(cut))
    return (sign * 0xFFFFFF ) / float(0x1000000 - ucut)

#------------------------------------------------------------
#
# Reads Configuration from COOL
#
# returns a RunConfig
#------------------------------------------------------------
ConfigCache=None
ConfigCacheRun=None
def GetConfig(runnumber,options=[]):
    SetLogging(options)

    log.debug("Getting configuration info")

    # check cache
    global ConfigCache,ConfigCacheRun
    if ConfigCacheRun==runnumber:
        return ConfigCache

    # puts the data here
    config = RunConfig()

    #
    # Read L1 Menu
    #
    
    itr = GetFolderItrForRun(lvl1menu_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            itemNo  = obj.channelId()
            payload = obj.payload()
            name    = TRPNameStrip(payload['ItemName'])

            config.L1CtpId2ChainName[itemNo]=name
            config.L1ChainName2CtpId[name]=itemNo

            print "L1 Read ", name, " from DB  id=", itemNo

    except Exception,e:
        log.error('Reading data from '+lvl1menu_foldername+' failed: '+str(e))

    print " L1 read ok ",runnumber," size=", len( config.L1CtpId2ChainName)

    #
    # Read HLT Menu
    #
    itr = GetFolderItrForRun(hltmenu_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            itemNo  = obj.channelId()
            payload = obj.payload()

            chain = HLTChain()
            chain.ChainName      = TRPNameStrip(payload['ChainName'])
            chain.ChainVersion   = int(payload['ChainVersion'])
            chain.ChainCounter   = int(payload['ChainCounter'])
            chain.TriggerLevel   = payload['TriggerLevel']
            chain.LowerChainName = TRPNameStrip(payload['LowerChainName'])

            config.ChainName2HLTChain[chain.ChainName]=chain

            if chain.TriggerLevel=='HLT':
                config.HLTCounter2HLTChain[chain.ChainCounter] = chain

            # HLT map: counter <-> name
            if chain.TriggerLevel=='HLT':
                config.HLTCounter2ChainName[chain.ChainCounter]  = chain.ChainName
                config.HLTChainName2Counter[chain.ChainName] = chain.ChainCounter
                print "Read HLT ", chain.ChainName, " from DB, counter=", chain.ChainCounter

    except Exception,e:
        log.error('Reading data from '+hltmenu_foldername+' failed: '+str(e))

    #
    # Read L1 PS key
    #
    l1keys    = [] # preserves order
    l1keymap  = {} # lb->key
    itr = GetFolderItrForRun( l1pskey_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            payload = obj.payload()
            l1pskey = payload['Lvl1PrescaleConfigurationKey']

            l1keys.append((lb,l1pskey))
            l1keymap[lb]=l1pskey

    except Exception,e:
        log.error('Reading data from '+l1pskey_foldername+' failed: '+str(e))


    #
    # Read HLT PS key
    #
    hltkeys   = [] # preserves order
    hltkeymap = {} # lb->key
    itr = GetFolderItrForRun( hltpskey_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            payload = obj.payload()
            hltpskey = payload['HltPrescaleKey']

            hltkeys.append((lb,hltpskey))
            hltkeymap[lb]=hltpskey

    except Exception,e:
        log.error('Reading data from '+hltpskey_foldername+' failed: '+str(e))


    #
    # Correlate HLT and L1 keys
    #
    l1idx  = 0
    hltidx = 0
    lastrange = None

    while True:
        psrange = PrescaleRange()
        psrange.l1key  = l1keys[l1idx][1]
        psrange.hltkey = hltkeys[hltidx][1]
        psrange.lbbeg  = max(l1keys[l1idx][0],hltkeys[hltidx][0])
        if lastrange:
            lastrange.lbend = psrange.lbbeg-1

        log.debug("New psset: "+str(psrange.lbbeg)+" "+str(psrange.l1key)+" "+str(psrange.hltkey))

        config.PrescaleRanges.append(psrange)
        lastrange=psrange

        if l1idx != len(l1keys)-1 and hltidx != len(hltkeys)-1:
            # increment the one that comes first
            if l1keys[l1idx+1][0] < hltkeys[hltidx+1][0]:
                l1idx+=1
            elif l1keys[l1idx+1][0] == hltkeys[hltidx+1][0]:
                l1idx+=1
                hltidx+=1
            else:
                hltidx+=1
        elif l1idx  != len(l1keys)-1:
            # on last hlt but not l1
            l1idx+=1
        elif hltidx != len(hltkeys)-1:
            # on last l1 but not hlt
            hltidx+=1
        else:
            # last of both
            break


    #
    # Read L1 PS
    #
    itr = GetFolderItrForRun( l1ps_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            itemNo  = obj.channelId()
            payload = obj.payload()

            prescale     = Prescale()
            prescale.ps  = getPrescaleFromCut(payload['Lvl1Prescale'])

            #try: 
            #    print "GETL1PS: itemNo ", itemNo , " name " ,config.L1CtpId2ChainName[itemNo]," PS ", prescale.ps
            #except:
            #    pass

            if not l1keymap.has_key(lb):
                print "Expected new L1 prescales for new L1 prescale key:",lb,
                sys.exit(-1)

            pskey = l1keymap[lb]
            if not config.L1Prescales.has_key(pskey):
                config.L1Prescales[pskey]={}

            if config.L1CtpId2ChainName.has_key(itemNo):
                name = config.L1CtpId2ChainName[itemNo]
                config.L1Prescales[pskey][name]=prescale


    except Exception,e:
        log.error('Reading data from '+l1ps_foldername+' failed: '+str(e))
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)



    #
    # Read HLT PS
    #
    itr = GetFolderItrForRun(hltps_foldername,runnumber)
    try:
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            lvl     = "HLT"
            #if obj.channelId()%2 == 1:
            #    lvl = "EF"
            chainctr= obj.channelId() - 20000 # div 2? No - for run 2 take off 20,000
            payload = obj.payload()

            # unpack
            prescale       = Prescale()
            prescale.ps    = payload['Prescale']
            prescale.pt    = payload['Passthrough']
            prescale.rerun = payload['RerunPrescale']

            # make new map if needed
            if not hltkeymap.has_key(lb):
                print "Expected new HLT prescales for new HLT prescale key:",lb,
                sys.exit(-1)

            pskey = hltkeymap[lb]
            if not config.HLTPrescales.has_key(pskey):
                config.HLTPrescales[pskey]={}

            # fill map
            if lvl=='HLT':
                if config.HLTCounter2HLTChain.has_key(chainctr):
                    chain=config.HLTCounter2HLTChain[chainctr]
                    config.HLTPrescales[pskey][chain.ChainName]=prescale
                    #print "GETHLTPS: itemNo ", chainctr , " name " ,chain.ChainName, " ps ", prescale.ps
                else:
                    log.error('Could not find a chain for HLT counter '+str(chainctr)+" with PS "+str(prescale.ps))

    except Exception,e:
        log.error('Reading data from '+hltps_foldername+' failed: '+str(e))
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    # save to cache
    ConfigCache=config
    ConfigCacheRun=runnumber

    return config


#------------------------------------------------------------
# Copied from CoolConvUtilities, but fixed so two functions are opposite of each other
# Convert argument to time in seconds, treating as a literal or date
#
def timeVal(val):

    try:
        a=int(val)
        return a
    except ValueError:

        import time
        import calendar

        try:
            ts=time.strptime(val,'%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))*1000000000L

        # Try again with UTC attached
        except ValueError:
            try:
                ts=time.strptime(val+'/UTC', '%Y-%m-%d:%H:%M:%S/%Z')
                return int(calendar.timegm(ts))*1000000000L

            except ValueError,e:
                print "ERROR in time specification:",val,"- use e.g. 2007-05-25:14:01:00/UTC"
                sys.exit(-1)

    return 0

#------------------------------------------------------------
timenow=0
def GetTimeRange(updateWindow=8760,begTime='',endTime=''): # Window hrs for updating fills

    import datetime

    # Set once
    global timenow
    if timenow == 0:
        timenow = datetime.datetime.utcnow()

    # Difference between timenow & updateWindow
    deltat = datetime.timedelta(hours=updateWindow)
    timesince  = timenow - deltat

    # Add an hour to "now" to be inclusive of the end time
    timenow += datetime.timedelta(hours=1)

    # Save to global -- relative to timenow
    if len(begTime)==0: begTime = timesince.strftime('%Y-%m-%d:%H:%M:%S')
    if len(endTime)==0: endTime = timenow.strftime('%Y-%m-%d:%H:%M:%S')

    log.info("Looking for time window (%d hrs) from %s -- %s UTC" % (updateWindow, begTime, endTime))

    return timeVal(begTime), timeVal(endTime)


#------------------------------------------------------------
def GetFillList(updateWindow=8760, begTime='', endTime='', verbose=False):

    run_beg_time, run_end_time = GetTimeRange(updateWindow,begTime,endTime)

    #
    # Read LHC fill numbers
    #
    lastFillNumber = 0
    fillList = []
    first = True
    itr = GetFolderItrForRun(lhcdb_foldername, -1, 'LHC', run_beg_time, run_end_time)
    try:
        while itr.goToNext() :
            obj         = itr.currentRef()
            StartTime   = obj.since()
            EndTime     = obj.until()
            payload     = obj.payload()

            # no lumiblock range implementation -- they're timestamped!

            fill    = payload['FillNumber']
            stable  = payload['StableBeams']
            lhc     = payload['MachineMode']
            beam    = payload['BeamMode']
            ebeam   = payload['BeamEnergyGeV']
            try:
                beg_    = AtlCoolTool.time.ctime(StartTime/1.0E9)
                end_    = AtlCoolTool.time.ctime(EndTime/1.0E9)
            except Exception,e:
                log.error('Could not convert time: '+str(e))
                continue

            # Start on Tue Mar 30, 09:13 UTC
            if fill < 1005:
                continue

            # Protection against bogus values
            iserr = False
            if fill  ==None: fill=0      ; iserr=iserr and True; log.error('NULL FillNumber')
            if stable==None: stable=False; iserr=iserr and True; log.error('NULL StableBeams')
            if lhc   ==None: lhc=''      ; iserr=iserr and True; log.error('NULL MachineMode')
            if beam  ==None: beam=''     ; iserr=iserr and True; log.error('NULL BeamMode')
            if ebeam ==None: ebeam= 0    ; iserr=iserr and True; log.error('NULL BeamEnergyGeV')
            if iserr:
                log.error('fill=%d %s--%s stable=%s lhc="%s" beam="%s" ebeam=%f' %
                          (fill,beg_,end_,stable,lhc,beam,ebeam))

            # New fill found
            if fill != lastFillNumber:
                fillList.append( UnpackFillData(StartTime, EndTime, payload) )
                if verbose:
                    log.info('fill=%d, %d--%d, %s--%s, lhc="%s", beam="%s", ebeam=%f' % (fill,StartTime,EndTime,beg_,end_,lhc,beam,ebeam))
                lastFillNumber = fill

        itr.close()
    except Exception,e:
        log.error('Reading data from '+lhcdb_foldername+' failed: '+str(e))
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    return fillList


#------------------------------------------------------------
# Get list of runs
# .) with beg time > updateWindow
# .) with beg time < updateWindow but > 72 AND end time > updateWindow
#
def GetRunList(updateWindow=8760, begTime='', endTime='', verbose=False):

    run_beg_time, run_end_time = GetTimeRange(updateWindow, begTime, endTime)

    #
    # Read ATLAS run numbers
    #
    lastRun = 0
    runList = []
    lbbeg = -1
    lbend = -1
    timebeg = -1
    timeend = -1
    first = True
    itr = GetFolderItrForRun(lbtime_foldername, -1, 'LBTIME', run_beg_time, run_end_time)
    try:
        while itr.goToNext() :
            obj         = itr.currentRef()
            StartTime   = obj.since()
            EndTime     = obj.until()
            payload     = obj.payload()

            # no lumiblock range implementation -- they're timestamped!

            # Don't include any LBs which start before our range
            if EndTime < run_beg_time:
                continue

            # Data
            run = int(payload['Run'])
            lb  = int(payload['LumiBlock'])

            print 'run,lb', run, lb

            # Initialize
            if lastRun==0:
                lastRun = run

            # Start on Tue Mar 30, 2011, 09:13 UTC
            if run < 152166:
                continue

            # Protection against bogus values
            if run == None:
                run = 0;
                log.error('run=%d, lb=%d' % (run,lb))

            # New run found
            if run != lastRun:
                runList.append( UnpackRunData(timebeg, timeend, lbbeg, lbend, run) ) # This run
#               runList.append( UnpackRunData(timebeg, timeend, lbbeg, lbend, lastRun) ) # Last run
                try:
                    beg_ = AtlCoolTool.time.ctime(timebeg/1.0E9)
                    end_ = AtlCoolTool.time.ctime(timeend/1.0E9)
                except Exception,e:
                    log.error('Could not convert time: '+str(e))
                if verbose:
                    log.info('run=%d, lbbeg=%d, lbend=%d, %d--%d, %s--%s' % (lastRun,lbbeg,lbend,timebeg,timeend,beg_,end_))
                lbbeg = -1
                lbend = -1
                timebeg = -1
                timeend = -1
                lastRun = run

            # Save for next
            if lbbeg == -1 or lb <= lbbeg: lbbeg = lb
            if lbend == -1 or lb >= lbend: lbend = lb
            if timebeg == -1 or StartTime < timebeg: timebeg = StartTime
            if timeend == -1 or EndTime >= timeend: timeend = EndTime

        itr.close()
    except Exception,e:
        log.error('Reading data from '+lbtime_foldername+' failed: '+str(e))
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    # Recursively look for non-empty run list unless window > 48 hrs
    if len(runList)==0:
        if updateWindow < 48:
            log.info('GetRunList() no run for %d hours; trying %d...' % (updateWindow,2*updateWindow))
            runList = GetRunList(updateWindow*2)
            return runList
        else:
            log.info('GetRunList() no run for %d > 48 hours, so aborting' % updateWindow)

    log.info('GetRunList() found')
    log.info('\n'+'\n'.join([('%s'%i) for i in runList]))
    return runList



#------------------------------------------------------------
#
# Read Rates from COOL
#
# returns a CostResultCollection
#------------------------------------------------------------

def GetRates(runnumber,lb_beg,lb_end,options=[]):
    SetLogging(options)

    # this needs both the lumi and config information
    lbset  = GetLumiblocks(runnumber,lb_beg,lb_end,options)
    config = GetConfig(runnumber,options)

    # output
    collection = CostResultCollection()

    # this reads
    lvl1counters_foldername = '/TRIGGER/LUMI/LVL1COUNTERS'

    #
    # Read Counters
    #
    itr = GetFolderItrForRun(lvl1counters_foldername,runnumber)
    try:
        while itr.goToNext():
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            itemNo  = obj.channelId()
            payload = obj.payload()

            # lumiblock range implementation
            if not IncludeLB(lb,lb_beg,lb_end):
                continue
            if collection.lbbeg==-1:
                collection.lbbeg=lb
            collection.lbbeg=min(lb,collection.lbbeg)
            collection.lbend=max(lb,collection.lbend)
            log.debug("Getting rates data for run=%d lb=%d" % (runnumber,lb))

            # find name from ctpid
            chain          = CostChain()
            if not config.L1CtpId2ChainName.has_key(itemNo):
                if int(payload['AfterPrescale'])!=0:
                    log.warning("Counts for item with no name? %d %d %d" %
                                (chain.GetTAVCnt(),chain.GetTAPCnt(),chain.GetTBPCnt()))
                continue

            name =""
            name = config.L1CtpId2ChainName[itemNo]
            chain.SetName(name)
            chain.SetLevel('L1')
            for ps_range in config.PrescaleRanges:
                if lb >= ps_range.FirstLB() and lb <= ps_range.LastLB():
                    chain.SetPrescale(config.L1Prescales[ps_range.l1key][name].ps)

            # Get Raw Counts
            chain.SetTAVCnt(int(payload['L1Accept']))
            chain.SetTBPCnt(int(payload['BeforePrescale']))
            chain.SetTAPCnt(int(payload['AfterPrescale']))
            chain.SetTAVOver(bool(payload['L1AcceptOverflow']))
            chain.SetTBPOver(bool(payload['BeforePrescaleOverflow']))
            chain.SetTAPOver(bool(payload['AfterPrescaleOverflow']))

            # Convert to Rates
            lbobj=lbset.GetLB(lb)
            tavrate = float(chain.GetTAVCnt())/lbobj.tav_turn*LumiBlock.orbitFreq
            chain.SetTAVRate(tavrate)
            # err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
            if lbobj.GetDT() > 0:
                chain.SetRateErr(math.sqrt(tavrate/lbobj.GetDT()))
            taprate = float(chain.GetTAPCnt())/lbobj.tap_turn*LumiBlock.orbitFreq
            chain.SetTAPRate(taprate)
            tbprate = float(chain.GetTBPCnt())/lbobj.tbp_turn*LumiBlock.orbitFreq
            chain.SetTBPRate(tbprate)
            chain.SetFill(-1)              # \todo
            chain.SetRun(runnumber)
            FillL1Chain(chain, lb, itemNo, payload, config, lbset)

            # save
            collection.SetCostChain(lb,chain.name,chain)

#	            if lb >= lb_end:
#	                break

        itr.close()
    except Exception,e:
        print "Reading data from",lvl1counters_foldername,"failed:",e
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    # Add HLT
    collection.AddHLT( GetHLTRates(runnumber,config,lbset,lb_beg,lb_end,'HLT',options) )
    collection.SetRun( runnumber )

    return collection


#------------------------------------------------------------
#
# Read HLT Rates from COOL
#
# returns a CostResultCollection
#------------------------------------------------------------
def GetHLTRates(runnumber, config, lbset, lb_beg, lb_end, lvl, options=[]):
    SetLogging(options)

    # Output
    collection = CostResultCollection()
    collection.SetName('Get%sRates' % lvl)

    #
    # Read Counters
    #
    hltcounters_foldername = '/TRIGGER/LUMI/HLTCOUNTERS'
    itr = GetFolderItrForRun(hltcounters_foldername, runnumber)
    try:
        while itr.goToNext():
            obj = itr.currentRef()
            lb  = (obj.since() & 0xffff)

            # lumiblock range implementation
            if not IncludeLB(lb,lb_beg,lb_end):
                continue

            # Blob encodes data
            blob = obj.payload()['Data'].read()
            if len(blob)==0:
                continue

            # Blob should be multiples of 20
            bloblength = 20
            if len(blob)%bloblength != 0:
                raise RuntimeError, "blob length %i != mod %d" % (len(blob), bloblength)

            # Set FirstLB(), LastLB()
            if collection.lbbeg < 0:
                collection.lbbeg=lb

            collection.lbbeg=min(lb, collection.lbbeg)
            collection.lbend=max(lb, collection.lbend)

            # Loop over chains in the blob
            for pos in xrange(0, len(blob)/bloblength):

                chain = CostChain()
                chain.SetFill(-1)              # \todo
                chain.SetRun(runnumber)

                # Fill
                FillHLTChain(chain, pos, blob, lb, lvl, config, lbset, bloblength)

                # Save
                collection.SetCostChain(lb, chain.name, chain)

#		    itr.close() # Iterator already closed, so cannot close
        log.debug("GetHLTRates() -- Read counters")
    except Exception,e:
        print "Reading data from",hltcounters_foldername,"failed:",e
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)

    return collection


#------------------------------------------------------------
# Get L1 chain
#
# returns void
#
def FillL1Chain(chain, lb, itemNo, payload, config, lbset):

    chain.SetLb(lb)
    chain.SetCounter(itemNo)

    # Divide trigger counts before prescale by LB time to get rate
    # This is instLumi delivered, so may need to take row livefraction,
    # if this is important for HLT
    if lbset.GetLB(lb).tbp_turn > 0:
        tbprate = float(chain.GetTBPCnt()) / lbset.GetLB(lb).tbp_turn * LumiBlock.orbitFreq
        taprate = float(chain.GetTAPCnt()) / lbset.GetLB(lb).tap_turn * LumiBlock.orbitFreq
        tavrate = float(chain.GetTAVCnt()) / lbset.GetLB(lb).tav_turn * LumiBlock.orbitFreq
        chain.SetTBPRate(tbprate)
        chain.SetTAPRate(taprate)
        chain.SetTAVRate(tavrate)

        # Make sure lumi isn't too low (and protect against zero) in 10^30 cm-2 s-1
        lumi = lbset.GetAvgInstantaneousLumi(lb, lb) # Was GetRecordedLumi(), a bug!
        if lumi > 0:
            # Convert to Cross sections
            tbpxsec = float(chain.GetTBPRate()) / lumi
            tapxsec = float(chain.GetTAPRate()) / lumi
            tavxsec = float(chain.GetTAVRate()) / lumi
            chain.SetTBPXsec(tbpxsec)
            chain.SetTAPXsec(tapxsec)
            chain.SetTAVXsec(tavxsec)

    return


#------------------------------------------------------------
# Get HLT chain from the blob
#
# returns void
#
def FillHLTChain(chain, pos, blob, lb, lvl, config, lbset, bloblength):

    # Decode
    chainctr = getEntryCounter(pos, blob, bloblength)
    counts   = getCounts(pos, blob, bloblength)

    # Initialize
    chain.SetLevel(lvl)
    chain.SetName('')
    chain.SetCounter(chainctr)
    chain.SetLb(lb)

    # Find name from counter
    findHLT = bool(lvl=='HLT' and not config.HLTCounter2ChainName.has_key(chainctr))
    if (findHLT) and counts:
        if int(counts.AfterPrescale)!=0:
            log.warning("Counts for %s item %d with no name? %s %d %d %d" % (
                lvl,
                chainctr,
                chain.GetTAVCnt(),
                chain.GetTAPCnt(),
                chain.GetTBPCnt()))

    # Set info
    if counts != None:

        # Chain name
        name = config.HLTCounter2ChainName[chainctr]
        chain.SetName(name)

        # Check LB range for prescale values
        ps = 0
        for ps_range in config.PrescaleRanges:
            if ps_range.FirstLB() <= lb <= ps_range.LastLB():
                ps = config.HLTPrescales[ps_range.hltkey][name].ps

        chain.SetPrescale(ps)

        # Set Raw Counts
        chain.SetTBPCnt(int(counts.Input))
        chain.SetTAPCnt(int(counts.AfterPrescale))
        chain.SetTAVCnt(int(counts.Raw))

        # Divide trigger counts before prescale by LB time to get rate
        # This is instLumi delivered, so may need to take row livefraction,
        # if this is important for HLT

#        if lb==2: print 'Processing', lb, lvl, name, chain.GetTBPCnt() # TODO

        if lbset.GetLB(lb).tbp_turn > 0:

            tbprate = float(chain.GetTBPCnt()) / lbset.GetLB(lb).tbp_turn * LumiBlock.orbitFreq
            taprate = float(chain.GetTAPCnt()) / lbset.GetLB(lb).tap_turn * LumiBlock.orbitFreq
            tavrate = float(chain.GetTAVCnt()) / lbset.GetLB(lb).tav_turn * LumiBlock.orbitFreq
            chain.SetTBPRate(tbprate)
            chain.SetTAPRate(taprate)
            chain.SetTAVRate(tavrate)

            # Make sure lumi isn't too low (and protect against zero) in 10^30 cm-2 s-1
            lumi = lbset.GetAvgInstantaneousLumi(lb, lb)

            if lumi > 0:

                # Convert to Cross sections
                tbpxsec = float(chain.GetTBPRate()) / lumi
                tapxsec = float(chain.GetTAPRate()) / lumi
                tavxsec = float(chain.GetTAVRate()) / lumi
                chain.SetTBPXsec(tbpxsec)
                chain.SetTAPXsec(tapxsec)
                chain.SetTAVXsec(tavxsec)

    return

#------------------------------------------------------------
#
# HLT blob decoder
# From Joerg Stelzer's /afs/cern.ch/user/s/stelzer/public/RateReader.py
#
#------------------------------------------------------------

#
# Convert blob & position -> HLT trigger id "counter "
#
def getEntryCounter(pos, buf, bloblength=20):
    return unpack_from('I', buf, bloblength*pos)[0]

#
# Convert blob & position -> HLT trigger accept "counts"
#
CountRec=namedtuple('CountRec','ChainCounter Input AfterPrescale Raw Total')
def getCounts(pos, buf, bloblength=20):
    if bloblength*pos >= len(buf):
        return None
    return CountRec._make( unpack_from('IIIII', buf, bloblength*pos) )


#------------------------------------------------------------
#
# Bunch blob decoder
# From Eric Torrence's Database/AtlDataSummary/python/AtlDataSumLumiData.py
#
#------------------------------------------------------------
# Retrieve stored unsigned int (1, 2, 4 or 8 byte) from blob
def bConvert(b, nbyte=1):

    #packopt=dict([[1,'B'],[2,'H'],[4,'I'],[8,'L']])
    packopt=dict([[1,'B'],[2,'H'],[4,'f'],[8,'L']])
    if nbyte in packopt:
        ival = unpack( packopt[nbyte], b )
    else:
        print 'bConvert: Unrecognized pack option'
        sys.exit()
    return ival[0]

# Bunch blob decoding loop
def bSet(blob, nbunch):

    bset = set()
    if blob.size()==0: return bset

    # Unpack
    for i in range(nbunch):
        b = blob.read(2)
        val = bConvert(b,2)
        bset.add(val)

    return bset



