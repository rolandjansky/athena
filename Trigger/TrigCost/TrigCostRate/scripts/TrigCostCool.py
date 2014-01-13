# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# Read LB stable beams from COOL
#
#

import sys
import os
import math
import traceback
import logging
log = logging.getLogger('TrigCostCool')

from PyCool import cool
from PyCool import coral
from CoolConvUtilities.AtlCoolLib import indirectOpen
from CoolConvUtilities import AtlCoolTool
from collections import namedtuple # for decoding blob
from struct import unpack_from, unpack # for decoding blob

#------------------------------------------------------------
#
# Folder names
#   /TRIGGER/ in https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData
#   /TDAQ/    in https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData
#   /LHC/     in https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LuminosityOnlineCool
#
#------------------------------------------------------------
test_foldername   = '/TDAQ/RunCtrl/EventCounters'      # Idx by run-LB
stb_foldername    = '/TDAQ/RunCtrl/DataTakingMode'     # Idx by run-LB
lblb_foldername   = '/TRIGGER/LUMI/LBLB'               # Idx by run-LB  
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
            dbProd = indirectOpen('COOLONL_TRIGGER/COMP200', oracle=True)
            log.info("Connected to database: "+'COOLONL_TRIGGER/COMP200')
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
            dbLhc = indirectOpen('COOLOFL_DCS/COMP200', oracle=True)
            log.info("Connected to database: "+'COOLOFL_DCS/COMP200')
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
            dbCompString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
            dbComp = indirectOpen('COOLONL_TDAQ/COMP200', oracle=True)
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
    dbTrigString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"

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
def GetFolderItrForRun(foldername,run,foldertype='TRIGGER',run_beg_time=-1,run_end_time=-1, channel=0):

    logging.debug('foldername:'+foldername)
    logging.debug('foldertype:'+foldertype)

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
    # from MONP200 to COMP200.  A few channels were also backfilled in
    # COMP200 back to the start of 2011, so that you can run over the
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
        return folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection(channel))

    # Default is TRIGGER
    InitDB()
    logging.info('Initalized DB')
    CheckFolder(foldername)
    logging.info('Checked Folder')
    folder=dbTrig.getFolder(foldername)
    if foldertype=="LBTIME":
        return folder.browseObjects(run_beg_time,run_end_time,cool.ChannelSelection.all())

    return folder.browseObjects(run << 32,(run+1) << 32,cool.ChannelSelection.all())


#------------------------------------------------------------
#
# Check first LB ready for Physics
#
#------------------------------------------------------------
def LBReadForPhysics(runnumber,lb_beg=0,lb_end=1000,options=[],getMin=True):
    SetLogging(options)

    log.info("Getting luminosity info for run=%d, %d <= LB <= %d" % (runnumber, lb_beg, lb_end))

    #
    # Get time info
    #
    log.debug('#')
    log.debug('# %s = %s loading'%('stb_foldername', stb_foldername))
    log.debug('#')
    try:
        readLBs=[]
        itr = GetFolderItrForRun(stb_foldername,runnumber,foldertype='COMP', channel=0) # Indexed by run-LB
        while itr.goToNext() :
            obj     = itr.currentRef()
            lb      = (obj.since() & 0xffff)
            payload = obj.payload()

            ready = payload['ReadyForPhysics']
            #ready = payload['L1Events']
            log.info('LB: %s ready: %s' %(lb,ready))
            if ready==1:
                readLBs+=[lb]

        itr.close()
        
        if len(readLBs)==0:
            log.info('No stable physics runs')
            return -1
        if not getMin:
            return max(readLBs)
        return min(readLBs)

    except Exception,e:
        log.error('Reading data from '+stb_foldername+' failed: '+str(e))

    return -1
