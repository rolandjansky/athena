# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



## @file TriggerConfig.py
## @brief Core job python configurable to setup the trigger configuration source
## @author Till Eifert <eifert@cern.ch>
## @author Joerg Stelzer <stelzer@cern.ch>
## $Id: TriggerConfig.py,v 1.4 2008-03-10 13:03:47 stelzer Exp $

###############################################################
#
# The jobOptions to enable reading and writing of configuration
# data from COOL into AOD
#
#
#==============================================================



# Required libs:
import os.path

## all we need is the standard job to
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import theApp

## the environment for this module
from AthenaCommon.JobProperties import jobproperties as jp
import RecExConfig.RecFlags
import TriggerJobOpts.TriggerFlags

from AthenaCommon.Logging import logging as log
 
def _setupConfig():
    rf = {}
    rf['readTAG'] = jp.Rec.readTAG()
    rf['readESD'] = jp.Rec.readESD()
    rf['readAOD'] = jp.Rec.readAOD()
    rf['readBS']  = jp.Rec.Trigger.readBS()
    rf['readRDO'] = jp.Rec.readRDO()
    rf['DataSource'] = jp.Global.DataSource()
    rf['readRDO'] = jp.Rec.readRDO()
    rf['doWriteESD'] = jp.Rec.doWriteESD()
    rf['doWriteAOD'] = jp.Rec.doWriteAOD()
    rf['doWriteTAG'] = jp.Rec.doWriteTAG()
    rf['TrigCoolSQLite'] = False

    global readRDO
    print "global readRDO=", readRDO
    varInit = globals().keys()
    print varInit
    print rf.keys()
    for o in [o for o in rf.keys() if o in varInit]:
        rf[o] = eval(o)

    #print "JJJJ DEBUG Print jobproperties"
    #jp.print_JobProperties('tree&value')
    #print "JJJJ DEBUG Print rec.jobproperties"
    #jp.Rec.print_JobProperties('tree&value')
    #print "JJJJ DEBUG Print rec.Trigger.jobproperties aka TriggerFlags"
    #jp.Rec.Trigger.print_JobProperties('tree&value')

    ## logging for this module
    msg = log.getLogger( 'TriggerConfig.py' )

    ## the generic trigger configuration service needs to be set up
    from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
    msg.info('Creating the trigger configuration service wrapper')
    svc = SetupTrigConfigSvc()

    ## ================================================================================
    ## the service depends on the condition we are running in 
    ##
    ##     environment       ConfigSvc      Data input   Config input   Flags
    ##     ---------------   ------------   ----------   ------------   -----
    ## (1) data taking       HLTConfigSvc   BS           TriggerDB      DataSource='data',...
    ##     
    ## (2) MC simulation     HTLConfigSvc   BS, RDO      XML            DataSource='geant3/4' and (readBS=True or readRDO=True) and doFEX=True and doHypo=True
    ##                       (to be replaced by DSConfigSvc later)
    ##
    ## (3) data processing   DSConfigSvc    BS, RDO      COOL/Oracle    readBS=True or readRDO=True
    ##     
    ## (4) AOD processing    DSConfigSvc    ESD/AOD      ESD/AOD(run)   (readESD=True or readAOD=True) and doFEX=False and doHypo=False
    ##                       AODConfigSvc                ESD/AOD(ev)
    ##     
    ## (5) Trigger ESD/AOD   HLTConfigSvc   ESD/AOD      XML            (readESD or readAOD)=True and doFEX=True and doHypo=True
    ##     
    ## (6) Trigger rerun     HLTConfigSvc   ESD/AOD      XML            (readBS or readRDO or readESD or readAOD)=True and doFEX=False and doHypo=True
    ##     
    ## (7) TAG reading       HLTConfigSvc   TAG          XML            readTAG=True
    ##     
    ##
    ## ================================================================================
    ## In cases (2), (3) and (5) the configuration information needs to be written
    ## into the header of the AOD file. This is completely independent of the configuration
    ## service, but will depend on the flags readESD, readAOD, doWriteESD, doWriteAOD, doWriteTAG
    ##
    ## ================================================================================
    ## two flags are steering the execution of the trigger, if neither
    ## is True, no trigger is used
    ##
    ## - doTrigger: if True, "TriggerRelease/jobOfragment_forRecExCommon.py"
    ## is included
    ##
    ## - doTriggerConfigOnly: if True, no TrigDecision is build/used
    ##
    ## ================================================================================
    ## flags that control the setup of the trigger are
    ##
    ## boolean:
    ## readBS,  readAOD,    readESD,    readRDO,    readTAG
    ## writeBS, doWriteAOD, doWriteESD, doWriteRDO, doWriteTAG
    ## readLVL1configFromXML, readHLTconfigFromXML
    ##
    ## string:
    ## inputHLTconfigFile, inputLVL1configFile, outputHLTconfigFile, outputLVL1configFile
    ##
    ## on/offline:
    ## DataSource: ['data','geant3','geant4']
    ## ================================================================================

    svc.hltXmlFile = lv1_menu_file_name = jp.Rec.Trigger.inputHLTconfigFile()
    svc.l1XmlFile  = jp.Rec.Trigger.inputLVL1configFile()

    myList = []
    scenario = 0;
    if len( jp.Rec.Trigger.configurationSourceList() ) > 0:
        # take the user-defined list of trigger configuration sources:
        myList = jp.Rec.Trigger.configurationSourceList()

    else:
        print jp.Rec
        if rf['readTAG']:  # TAG
            myList = ['xml']
            
        elif rf['readESD'] or rf['readAOD']:  # AOD/ESD
            myList = ['ds','aod','xml']
            
        elif rf['readBS'] or rf['readRDO']: # RDO/BS
            if rf['DataSource'] == 'data':
                myList = ['ds','xml']
            else:
                myList = ['xml']

        else:
            msg.fatal('no reading of BS, RDO, AOD, ESD, or TAG specified')

    message = 'setup the following services in order: ';
    for s in myList: message += s + " "
    msg.info(message)

    try:
        svc.SetStates( myList )
    except:
        msg.error( 'failed to set state of TrigConfigSvc ...')

    try:
        svc.InitialiseSvc()
    except:
        msg.error( 'failed to activate TrigConfigSvc ...')


    if rf['doWriteESD'] or rf['doWriteAOD'] or rf['doWriteTAG'] or ('ds' in myList):

        ## setup the copying of configuration data from input to output metadatastore
        if rf['doWriteESD'] or rf['doWriteAOD'] or rf['doWriteTAG']:
            msg.info( 'writing of ESD, AOD, or TAG requested, will setup IOVDbSvc to access configuration meta data')
        else:
            msg.info( 'DSConfigSvc enabled, will setup IOVDbSvc to access configuration meta data')

        # first we need a new algorithm that triggers the copying
        from AthenaCommon.AlgSequence import AlgSequence
        from TrigConfigSvc.TrigConfigSvcConf import TrigConf__TrigConfDataIOVChanger as TrigConfDataIOVChanger
        topAlgs = AlgSequence()
        TrigConfDataIOVChanger = TrigConfDataIOVChanger('TrigConfDataIOVChanger')
        topAlgs += TrigConfDataIOVChanger

            
        from AthenaCommon.AppMgr  import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc

        if not hasattr( ToolSvc, 'IOVDbMetaDataTool' ):
            from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
            ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )

        
        if not hasattr( svcMgr, 'MetaDataSvc' ):
            from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
            svcMgr += MetaDataSvc( "MetaDataSvc" )
            svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
            svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

        # for debugging uncomment the following three lines
        from AthenaCommon.Constants import VERBOSE
        ToolSvc.IOVDbMetaDataTool.OutputLevel = VERBOSE
        svcMgr.MetaDataSvc.OutputLevel = VERBOSE

        dbConnection = "<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOLONL_READER;devdb10:COOLTEST:atlas_trig_stelzer</dbConnection>"

        
        ### when moving to configurable IOVDbSvc use the ### lines and remove those with ## at the end
        ### import IOVDbSvc.IOVDb    
        ### IOVDbSvc = svcMgr.IOVDbSvc
        from AthenaCommon.Include import include      ##
        include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )  ##
        IOVDbSvc = theApp.service( "IOVDbSvc" )       ##
        #from AthenaCommon.Constants import VERBOSE
        #IOVDbSvc.OutputLevel = VERBOSE

        ## if we process MC from an XML file the dbConnection needs to
        ## be set to a local SQlite file
        
        if (rf['readRDO'] or rf['readBS']) and (rf['DataSource']=='geant3' or rf['DataSource']=='geant4' or rf['TrigCoolSQLite']):
            from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
            dbConnection = theConfCOOLWriter.dbConnection
            theConfCOOLWriter.isWritingNeeded = True
            TrigConfDataIOVChanger.AdjustIOV = True
        
        msg.info("COOL DBConnection: " + dbConnection )

        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/Menu <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Menu <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Prescales <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/ItemDef <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/Prescales <tag>HEAD</tag>"]
        ### svcMgr.IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/PrescaleKey <tag>HEAD</tag>"]

        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/Menu <tag>HEAD</tag>"]           ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>"]  ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>"] ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Menu <tag>HEAD</tag>"]          ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/Prescales <tag>HEAD</tag>"]     ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/LVL1/ItemDef <tag>HEAD</tag>"]       ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/Prescales <tag>HEAD</tag>"]      ##
        IOVDbSvc.Folders+=[dbConnection + "/TRIGGER/HLT/PrescaleKey <tag>HEAD</tag>"]    ##

        
        # only in jobs that write AOD or ESD
        if rf['doWriteAOD'] or rf['doWriteESD']:
            msg.info( 'writing of ESD or AOD enabled, will setup IOVDbSvc to write configuration meta data')
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/Menu"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/HltConfigKeys"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Lvl1ConfigKey"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Menu"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Prescales"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/ItemDef"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/Prescales"]
            ### svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/PrescaleKey"]

            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/Menu"]           ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/HltConfigKeys"]  ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Lvl1ConfigKey"] ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Menu"]          ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/Prescales"]     ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/LVL1/ItemDef"]       ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/Prescales"]      ##
            IOVDbSvc.FoldersToMetaData+=["/TRIGGER/HLT/PrescaleKey"]    ##


## load basic services configuration at module import
_setupConfig()

## clean-up: avoid running multiple times this method
del _setupConfig

