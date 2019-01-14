# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Trigger specific flags  """


__author__  = 'T. Bold, J. Baines'
__version__=""
__doc__="Trigger specific flags  "


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )
log.setLevel(logging.DEBUG)

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenu.menu.CommonSliceHelper import AllowedList
from TrigConfigSvc.TrigConfigSvcUtils import getKeysFromNameRelease, getMenuNameFromDB


_flags = []

# Define Default Flags
class doLVL1(JobProperty):
    """ run the LVL1 simulation (set to FALSE to read the LVL1 result from BS file) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doLVL1)

class doL1Topo(JobProperty):
    """ Run the L1 Topo simulation (set to FALSE to read the L1 Topo result from BS file) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doL1Topo)

class readLVL1Calo(JobProperty):
    """  read LVL1 Calo info from pool or BS """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readLVL1Calo)

class readLVL1Muon(JobProperty):
    """ read LVL1 Muon in from Pool or BS """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readLVL1Muon)

class fakeLVL1(JobProperty):
    """ create fake RoI from KINE info  """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(fakeLVL1)

class useCaloTTL(JobProperty):
    """ False for DC1. Can use True for Rome files with Digits or post-Rome data """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(useCaloTTL)

class useL1CaloCalibration(JobProperty):
    """ Should be false for early data, true for later """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(useL1CaloCalibration)

class useRun1CaloEnergyScale(JobProperty):
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(useRun1CaloEnergyScale)

class doCosmicSim(JobProperty):
    """ run the LVL1 simulation with special setup for cosmic simulation (set to FALSE by default, to do collisions simulation) """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doCosmicSim)

class disableRandomPrescale(JobProperty):
    """ if True, disable Random Prescales """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(disableRandomPrescale)

class doLVL2(JobProperty):
    """ if False, disable LVL2 selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doLVL2)

class doEF(JobProperty):
    """ if False, disable EF selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doEF)


class doHLT(JobProperty):
    """ if False, disable HLT selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
    def _do_action(self):
        """ setup flag level consistency """
        if self.get_Value() is True:
            if TriggerFlags.doEF.is_locked():
                TriggerFlags.doEF.unlock()
                TriggerFlags.doEF.set_Off()
                TriggerFlags.doEF.lock()
            else:
                TriggerFlags.doEF.set_Off()
            if TriggerFlags.doEF.is_locked():
                TriggerFlags.doLVL2.unlock()
                TriggerFlags.doLVL2.set_Off()
                TriggerFlags.doLVL2.lock()
            else:
                TriggerFlags.doLVL2.set_Off()
            log = logging.getLogger( 'TriggerFlags.doHLT' )
            log.info("doHLT is True: force doLVL2=False and doEF=False"  )

            
_flags.append(doHLT)


class doMergedHLTResult(JobProperty):
    """ if False disable decoding of the merged HLT Result (so decoding L2/EF Result) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doMergedHLTResult)

class doAlwaysUnpackDSResult(JobProperty):
    """ if False disable decoding of DS results for all files but for real DS files """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doAlwaysUnpackDSResult)

class writeL1TopoValData(JobProperty):
    """ if False disable writing out of the xAOD L1Topo validation object """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(writeL1TopoValData)

class EDMDecodingVersion(JobProperty):
    """ if 1, Run1 decoding version is set; if 2, Run2 """
    statusOn=True
    allowedType=['int']
    StoredValue=2

_flags.append(EDMDecodingVersion)



class doFEX(JobProperty):
    """ if False disable Feature extraction algorithms """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doFEX)

class doHypo(JobProperty):
    """ if False disable all Hypothesis algorithms (HYPO)"""
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doHypo)

class doTruth(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doTruth)

# FTK simulation switch

class doFTK(JobProperty):
    """ if False, disable FTK result reader """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doFTK)

# monitoring switch
class enableMonitoring(JobProperty):
    """ enables certain monitoring type: Validation, Online, Time"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]

_flags.append(enableMonitoring)

# trigger configuration source list
class configurationSourceList(JobProperty):
    """ define where to read trigger configuration from. Allowed values: ['xml','aod','ds']"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]
    allowedValues = AllowedList( ['aod','xml','ds'] )

_flags.append(configurationSourceList)

class doTriggerConfigOnly(JobProperty):
    """ if True only the configuration services should be set, no algorithm """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doTriggerConfigOnly)
              
# Flags to switch on/off Detector Slices
class doID(JobProperty):
    """ if False, disable ID algos at LVL2 and EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doID)

class doCalo(JobProperty):
    """ if False, disable Calo algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doCalo)

class doCaloOffsetCorrection(JobProperty):
    """ enable Calo pileup offset BCID correction """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doCaloOffsetCorrection)

class doBcm(JobProperty):
    """ if False, disable BCM algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doBcm)

class doTrt(JobProperty):
    """ if False, disable TRT algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doTrt)

class doZdc(JobProperty):
    """ if False, disable ZDC algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doZdc)

class doLucid(JobProperty):
    """ if False, disable Lucid algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doLucid)

class doMuon(JobProperty):
    """ if FAlse, disable Muons, note: muons need input file containing digits"""
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doMuon)

class doHLTpersistency(JobProperty):
    """ serialise L2result """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doHLTpersistency)

class useOfflineSpacePoints(JobProperty):
    """ use online convertors for Si SpacePoints"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(useOfflineSpacePoints)

class doNtuple(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doNtuple)


class writeBS(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(writeBS)

class readBS(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readBS)


class AODEDMSet(JobProperty):
    """ Define which sets of object go to AOD """
    statusOn=True
    allowedType=['list']
    StoredValue='AODSLIM'

_flags.append(AODEDMSet)

class ESDEDMSet(JobProperty):
    """ Define which sets of object go to ESD (or derived ESD) """
    statusOn=True
    allowedType=['list']
    StoredValue='ESD'

_flags.append(ESDEDMSet)

class OnlineCondTag(JobProperty):
    """ Default (online) HLT conditions tag """
    statusOn=True
    allowedType=['str']
    StoredValue='CONDBR2-HLTP-2018-01'

_flags.append(OnlineCondTag)

class OnlineGeoTag(JobProperty):
    """ Default (online) HLT geometry tag """
    statusOn=True
    allowedType=['str']
    StoredValue='ATLAS-R2-2016-01-00-01'
    
_flags.append(OnlineGeoTag)

# =========
#
# trigger flags used by trigger configuration
#

class configForStartup(JobProperty):
    """ A temporary flag to determine the actions to be taken for the different cases of HLT running in the startup phase"""
    statusOn=True
    allowedType=['string']
    StoredValue = 'HLTonlineNoL1Thr'
    
    allowedValues = [
        'HLTonline',
        'HLToffline',
        'HLTonlineNoL1Thr',
        'HLTofflineNoL1Thr'
        ]

_flags.append(configForStartup)

class run2Config(JobProperty):
    """ A flag to specify 2016 or 2017 (tunes, etc) running conditions """
    statusOn=True
    allowedType=['string']
    StoredValue = '2017'
    allowedValues = [
        '2016',
        '2017',
        ]
    def _do_action(self):
        from TriggerMenu.egamma.EgammaSliceFlags import run2ConfigAction as egammaRun2ConfigAction
        egammaRun2ConfigAction(self.get_Value())

_flags.append(run2Config)

class dataTakingConditions(JobProperty):
    """ A flag that describes the conditions of the Trigger at data taking, and determines which part of it will be processed in reconstruction."""
    statusOn=True
    allowedType=['string']
    StoredValue = 'FullTrigger'
    
    allowedValues = [
        'HltOnly',
        'Lvl1Only',
        'FullTrigger',
        'NoTrigger'
        ]

_flags.append(dataTakingConditions)

class triggerUseFrontier(JobProperty):
    """Flag determines if frontier should be used to connect to the oracle database, current default is False"""
    statusOn=True
    allowedType=['bool']
    StoredValue = False
    def _do_action(self):
        log = logging.getLogger( 'TriggerFlags.triggerUseFrontier' )
        log.info("Setting TriggerFlags.triggerUseFrontier to %r" % self.get_Value())
        
_flags.append(triggerUseFrontier)



class triggerConfig(JobProperty):
    """ Flag to set various menus and options (read from XML or DB)
    Allowed values:

    Note that we use LVL1 prefix here in order not to touch the
    HLT if we're only running a LVL1 digitization job. The
    prefix is automatically added in the Digi job transform.
    
    NONE or OFF                             -trigger off 

    For digitization (L1) only use LVL1 prefix:
    LVL1:DEFAULT                            -default L1 menu
    LVL1:MenuName                           -takes the L1 xml representation of this menu
    LVL1:DB:connectionstring:SMKey,L1PSKey  -takes these db keys
    LVL1:DB:connectionstring:MenuName,Rel   -takes this menu from the db - not yet supported

    For MC reconstruction use MCRECO prefix:
    MCRECO:DEFAULT                                       -default L1 and HLT menu
    MCRECO:MenuName                                      -takes the L1 and HLT xml respresentations of the menu
    MCRECO:L1CaloCalib=True/False:MenuName               -takes the L1 and HLT xml respresentations of the menu, sets L1 calo calib
    MCRECO:DB:connectionstring:SMKey,L1PSK,HLTPSK[,BGK]  -takes these db keys
    MCRECO:DB:L1CaloCalib=True/False:connectionstring:SMKey,L1PSK,HLTPSK  -takes these db keys, sets L1 calo calib
    MCRECO:DB:connectionstring:MenuName,Rel              -takes this menu from the db (looks up the SMK)
                                                         -NB for the above: move to alias tables?
                                                   
    For data reconstruction: use DATARECO prefix. TO BE IMPLEMENTED. 
    DATARECO:ONLINE
    DATARECO:OFFLINE
    DATARECO:DB:connectionstring:SMKey,L1PSK,HLTPSK
    
    InFile: TO BE IMPLEMENTED

    connectionstring can be one of the following
    1)  <ALIAS>                              -- usually TRIGGERDB or TRIGGERDBMC (generally any string without a colon ':')
    2)  <type>:<detail>                      -- <type> has to be oracle, mysql, or sqlite_file, <detail> is one of the following
    2a) sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    2b) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2  -- a service description type://server/schema without user and password
    2c) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    Note: specifying :DBF: instead of :DB: will set the trigger flag triggerUseFrontier to true
    """
    
    statusOn=''
    allowedType=['string']
    StoredValue = 'MCRECO:DEFAULT'


    def _do_action(self):
        """ setup some consistency """
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
                
        log = logging.getLogger( 'TriggerFlags.triggerConfig' )
        log.info("triggerConfig: \""+self.get_Value()+"\"")
        # We split the string passed to the flag
        configs = self.get_Value().split(":")
        
        ## ------
        ##  OFF or NONE: we want to turn everything related to trigger to false (via rec flag)
        ##  Note that this is true for reconstruction only at the moment. For LVL1 Digitization jobs,
        ##  which don't use rec flags, this is still done in the skeleton. Might be changed in future.
        ## ------
        if (configs[0] == 'OFF' or configs[0] == 'NONE'):
            from RecExConfig.RecFlags  import rec
            rec.doTrigger=False
            log.info("triggerConfig: Setting rec.doTrigger to False")
            
            
        ## ------
        ## DATARECO : We deal with data (cosmics, single run, collisions)
        ## ------               
        elif configs[0] == 'DATARECO':
            if configs[1] == 'ONLINE': # We read config from COOL directly
                log.warning("triggerConfig: DATARECO:ONLINE (reco from cool) is not yet implemented. You should not use it.")
            elif configs[1] == 'OFFLINE': # We read config from XML
                log.warning("triggerConfig: DATARECO:OFFLINE (reco from xml) is not yet implemented. You should not use it.")
            elif configs[1] == 'REPR': # We read config from XML
                log.info("triggerConfig: DATARECO:REPR is designed to configure the offline reconstruction in a trigger reprocessing job")
                try:
                    f = open("MenuCoolDbLocation.txt",'r')
                    tf.triggerCoolDbConnection = f.read()
                    f.close()
                except IOError, e:
                    log.fatal("triggerConfig=DATARECO:REPR requires 'MenuCoolDbLocation.tx' to be present in the local directory (reco part of trigger reprocessing)")
                    
            elif configs[1] == 'DB' or configs[1] == 'DBF': # We read config from a private DB
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: DATARECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: DATARECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: DATARECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                else:
                    log.info("triggerConfig: DATARECO from DB configured with wrong number of keys/arguments" )

        ## ---------
        ##  InFile : We wish to read a file with config info already in it (ESD, AOD, ...)
        ## ---------            
        elif configs[0] == 'InFile': 
            log.warning("triggerConfig: Infile is not yet implemented. You should not use it.")

        ## ------
        ##  LVL1 : For LVL1 simulation only in Digitization job 
        ## ------
        elif configs[0] == 'LVL1':
            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read config from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 2): #We got either 2 keys (SM, L1PS) or menu name plus release. If latter, second object will contain a .
                    if '.' not in str(DBkeys[1]):
                        tf.triggerDbKeys=[int(x) for x in DBkeys] +[-1,1] # SMkey, L1PSkey, HLTPSkey, BGkey
                        log.info("triggerConfig: LVL1 from DB with specified keys SMK %i and L1 PSK %i." % tuple(tf.triggerDbKeys()[0:2])   )
                    else:
                        log.info("triggerConfig: LVL1 from DB with speficied menu name and release: finding keys...")
                        tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],True) + [-1,1]
                        log.info("triggerConfig: LVl1 from DB with keys SMK %i and L1 PSK %i" % tuple(tf.triggerDbKeys()[0:2])   )
                else:                  #We got a menu name which we need to look up - not implemented yet
                    log.info("triggerConfig: LVL1 from DB configured with wrong number of keys/arguments" )

            else:
                ### We read config from XML
                tf.readLVL1configFromXML=True
                if (configs[1] == 'DEFAULT' or configs[1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[1]
                log.info("triggerConfig: LVL1 menu from xml (%s)" % tf.triggerMenuSetup())

                

        #------
        # MCRECO: Reconstruction of MC
        #------            
        elif configs[0] == 'MCRECO':
            from RecExConfig.RecFlags  import rec
            from RecJobTransforms.RecConfig import recConfig
            rec.doTrigger = True

            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                
                #see if L1 calib arg supplied
                if "L1CaloCalib" in configs[2]:
                    if configs[2].split("=")[-1] == "True" or configs[2].split("=")[-1] == "true":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[2].split("=")[-1])
                        tf.useL1CaloCalibration=True
                    elif configs[2].split("=")[-1] == "False" or configs[2].split("=")[-1] == "false":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[2].split("=")[-1])
                        tf.useL1CaloCalibration=False
                    else:
                        log.warning("Unknown value for L1CaloCalib ('%s'), will use default" % configs[2].split("=")[-1])
                    tf.triggerDbConnection = ':'.join(configs[3:-1])  # the dbconnection goes from third to last ':', it can contain ':'
                else:
                    tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 4):                            # we got 4 keys (SM, L1PS, HLTPS,BGK)
                    tf.triggerDbKeys=[int(x) for x in DBkeys]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, HLT PSK %i, and BGK %i." % tuple(tf.triggerDbKeys()[0:4])   )
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: MCRECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: MCRECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                else:
                    log.info("triggerConfig: MCRECO from DB configured with wrong number of keys/arguments" )

                # we need to set triggerMenuSetup to the correct name
                # that we get from the triggerDB, otherwise
                # TriggerGetter->GenerateMenu.generateMenu() would be
                # run with the wrong menu and the configuration of the
                # algorithms would be incorrect (bug 72547)
                tf.triggerMenuSetup=getMenuNameFromDB(tf.triggerDbConnection(),tf.triggerDbKeys()[2])
                log.info("triggerConfig: Setting tf.triggerMenuSetup to " + tf.triggerMenuSetup())
            else:
                ### We read the menu from xml
                if "L1CaloCalib" in configs[1]:
                    if configs[1].split("=")[-1] == "True" or configs[1].split("=")[-1] == "true":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[1].split("=")[-1])
                        tf.useL1CaloCalibration=True
                    elif configs[1].split("=")[-1] == "False" or configs[1].split("=")[-1] == "false":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " %  configs[1].split("=")[-1])
                        tf.useL1CaloCalibration=False
                    else:
                        log.warning("Unknown value for L1CaloCalib ('%s'), will use default" % configs[1].split("=")[-1])
                if (configs[-1] == 'DEFAULT' or configs[-1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[-1]

                tf.readLVL1configFromXML=True
                tf.readHLTconfigFromXML=True
                log.info("triggerConfig: MCRECO menu from xml (%s)" % tf.triggerMenuSetup())

            # This part was there in the original (old) csc_reco_trigger.py snippet
            # Still wanted?
            if rec.doTrigger:
                # Switch off trigger of sub-detectors
                for detOff in recConfig.detectorsOff:
                    cmd = 'TriggerFlags.do%s = False' % detOff
                    # possibly not all DetFlags have a TriggerFlag
                    try:
                        exec cmd
                        log.info(cmd)
                    except AttributeError:
                        pass
        #------            
        # We passed a wrong argument to triggerConfig
        #------
        else:
            log.error("triggerConfig argument \""+self.get_Value()+"\" not understood. "
                      + "Please check in TriggerFlags.py to see the allowed values.")
            

_flags.append(triggerConfig)



class readL1TopoConfigFromXML(JobProperty):
    """Use to specify external l1topo xml configuration file
    (e.g. from the release or a local directory)
    
    If set to True:
    the L1Topo config will be taken from TriggerFlags.inputL1TopoConfigFile()
    
    If set to False:    
    the L1Topo config xml file is read from the python generated XML
    file, which is specified in TriggerFlags.outputL1TopoconfigFile()
    """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False # once the python generation is implemented the default should be False

    def _do_action(self):
        """ setup some consistency """
        if self.get_Value() is False:
            TriggerFlags.inputL1TopoConfigFile = TriggerFlags.outputL1TopoConfigFile()

_flags.append(readL1TopoConfigFromXML)




class readLVL1configFromXML(JobProperty):
    """ If set to True the LVL1 config file is read from earlier generated XML file """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False

    def _do_action(self):
        """ setup some consistency """
        import os
        log = logging.getLogger( 'TriggerFlags.readLVL1configFromXML' )

        import TriggerMenu.l1.Lvl1Flags
        
        if self.get_Value() is False:
            TriggerFlags.inputLVL1configFile = TriggerFlags.outputLVL1configFile()
            TriggerFlags.Lvl1.items.set_On()
        else:
            xmlFile=TriggerFlags.inputLVL1configFile()
            from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
            if xmlFile!='NONE' and not os.path.exists(findFileInXMLPATH(xmlFile)):
                log.error("Cannot find LVL1 xml file %s" % xmlFile)

            TriggerFlags.Lvl1.items.set_Off()

_flags.append(readLVL1configFromXML)



class readHLTconfigFromXML(JobProperty):
    """ If set to True the HLT config file is read from earlier generated XMl file """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputHLTconfigFile
    # StoredValue=False
    StoredValue = False

    def _do_action(self):
        """ Disable all subcontainers defining slices ON/OFF flags """

        import os
        log = logging.getLogger( 'TriggerFlags.readHLTconfigFromXML' )

        ## loop over all properties in the container
        for prop in TriggerFlags.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                for slice_prop_name in prop.__dict__:
                    slice_prop = prop.__dict__.get(slice_prop_name)
                    if issubclass(slice_prop.__class__, JobProperty):
                        if self.get_Value() is True: ## now depending on the value set flags are on/off
                            slice_prop.set_Off()
                        else:
                            slice_prop.set_On()
        ## in addition set inputLVL1configFile to be the same as outputLVL1configFile
        if self.get_Value() is False:
            TriggerFlags.inputHLTconfigFile = TriggerFlags.outputHLTconfigFile()
        else:
            if TriggerFlags.inputHLTconfigFile != 'NONE':
                
                TriggerFlags.inputHLTconfigFile = "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
                nightlyPaths=os.environ['XMLPATH'].split(':')

                for p in nightlyPaths:
                    #print p+"/TriggerMenuXML/HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
                    full_path_name = p+"/TriggerMenuXML/"+TriggerFlags.inputHLTconfigFile()
                    if os.path.exists(full_path_name) is True:
                        log.info("The HLT xml file is: "+full_path_name)
                        success = True
                        break
                    else:
                        success = False

                if success is False:
                    log.error("The HLT xml file is missing: HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml")
                
_flags.append(readHLTconfigFromXML)


# trigger configuration source list
class readMenuFromTriggerDb(JobProperty):
    """ define the TriggerDb to be the source of the LVL1 and HLT trigger menu"""
    statusOn=False
    allowedType=['bool']
    StoredValue=False
#    def _do_action(self):
#        """ setup reading from DB requires menu readingFromXML """
#        if self.get_Value() is True:
#            TriggerFlags.readLVL1configFromXML = True
#            TriggerFlags.readHLTconfigFromXML = True
_flags.append(readMenuFromTriggerDb)

# trigger configuration source list
class readConfigFromTriggerDb(JobProperty):
    """ define the TriggerDb to be the source of the LVL1 and HLT trigger menu"""
    statusOn=False
    allowedType=['bool']
    StoredValue=False

    def _do_action(self):
        """ setup reading from DB requires menu readingFromXML """
        if self.get_Value() is True:
            # readMenuFromTriggerDb dumps only the HLTMenu to an XML file - it is of no use since HLTConfigSvc is set for the DB
            TriggerFlags.readMenuFromTriggerDb = False
            TriggerFlags.readLVL1configFromXML = False
            TriggerFlags.readHLTconfigFromXML = False

_flags.append(readConfigFromTriggerDb)

class triggerDbKeys(JobProperty):
    """ define the keys [Configuration, LVL1Prescale, HLTPrescale, L1BunchGroupSet] in that order!"""
    statusOn=False
    allowedType=['list']
    StoredValue=[0,0,0,1]

_flags.append(triggerDbKeys)

class triggerDbConnection(JobProperty):
    """ define triggerDB connection parameters"""
    statusOn=False
    allowedType=['dict']
    StoredValue="TRIGGERDB"

_flags.append(triggerDbConnection)

class triggerCoolDbConnection(JobProperty):
    """ define connection parameters to cool if external sqlite file is to be used"""
    statusOn=True
    allowedType=['str']
    StoredValue=''

_flags.append(triggerCoolDbConnection)

class outputL1TopoConfigFile(JobProperty):
    """ File name for output L1Topo configuration XML file produced by the python menu generation """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            # triggerMenuSetup contains the prescale mode in many
            # cases, e.g. MC_pp_v5_tight_mc_prescale. Prescaling is
            # not available for L1Topo, therefore that part is being
            # removed.
            import re
            menuSetup = TriggerFlags.triggerMenuSetup()
            m = re.match('(.*v\d).*', menuSetup)
            if m:
                menuSetup = m.groups()[0]
            return "L1Topoconfig_" + menuSetup + "_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputL1TopoConfigFile)

class outputLVL1configFile(JobProperty):
    """ File name for output LVL1 configuration XML file """
    statusOn=True
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputLVL1configFile)

class outputHLTconfigFile(JobProperty):
    """ File name for output HLT configuration XML file """
    statusOn=True
#    allowedType=['str']
    StoredValue=""
    
    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()

_flags.append(outputHLTconfigFile)



class inputL1TopoConfigFile(JobProperty):
    """Used to define an external L1Topo configuration file. To be
    used together with trigger flag readL1TopoConfigFromXML.

    If TriggerFlags.readL1TopoConfigFromXML()==True, then this file is
    used for L1TopoConfiguration.
    
    Defaults to L1TopoConfig_<triggerMenuSetup>_<menuVersion>.xml
    """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "L1TopoConfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputL1TopoConfigFile)



class inputLVL1configFile(JobProperty):
    """ File name for input LVL1 configuration XML file """
    statusOn=True
#    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputLVL1configFile)



class inputHLTconfigFile(JobProperty):
    """ File name for input HLT configuration XML file """
    statusOn=True
    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputHLTconfigFile)

class abortOnConfigurationError(JobProperty):
    """ Should the job be stoped if there is an error in configuration"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(abortOnConfigurationError)


# =================
#
# trigger menu flags - menu version, prescale sets
#
# =================
class menuVersion(JobProperty):
    """ Defines the menu version to use, usually the same as the release number. This is part of the XML file name. """
    statusOn=True
    allowedType=['str']
    
    from AthenaCommon.AppMgr import release_metadata
    StoredValue = release_metadata()['release']  # returns '?' if missing
    
_flags.append(menuVersion)


class triggerMenuSetup(JobProperty):
    """ Defines the luminosity dependent setup of trigger lumi01 == 10^33, switches on/off signatures """
    statusOn=True
    allowedType=['str']
    allowedValues = [
        'default', 'cosmic_default', 'InitialBeam_default',
        # menus for 10^31 with EMScale L1 calib
        'Physics_lumi1E31_simpleL1Calib','Physics_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib','MC_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib_physics_prescale',
        # menus for 10^32 with EMScale L1 calib
        'Physics_lumi1E32_simpleL1Calib','Physics_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib','MC_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib_physics_prescale',
        # menus for 10^33
        'Physics_lumi1E33','Physics_lumi1E33_no_prescale',
        'MC_lumi1E33','MC_lumi1E33_no_prescale',
        'Physics_lumi1E34','Physics_lumi1E34_no_prescale',
        'MC_lumi1E34','MC_lumi1E34_no_prescale',
        #
        'Cosmics','Cosmic_v1', 'Cosmic2009_v1', 'Cosmic2009_v2', 
        'InitialBeam_v1', 'MC_InitialBeam_v1', 'MC_InitialBeam_v1_no_prescale',
        'Cosmic2009_simpleL1Calib', 'Cosmic2009_inclMuons',
        'enhBias',
        # for 2010 running
        'Cosmic_v2','Cosmic_v3',
        'InitialBeam_v2', 'MC_InitialBeam_v2', 'MC_InitialBeam_v2_no_prescale',
        'InitialBeam_v3', 'MC_InitialBeam_v3', 'MC_InitialBeam_v3_no_prescale',
        #for 2010-2011 running
        'Physics_pp_v1', 'Physics_pp_v1_no_prescale', 'Physics_pp_v1_cosmics_prescale',
        'MC_pp_v1', 'MC_pp_v1_no_prescale',
        'MC_pp_v1_tight_mc_prescale', 'MC_pp_v1_loose_mc_prescale',
        #v2 
        'Physics_pp_v2', 'Physics_pp_v2_no_prescale', 'Physics_pp_v2_cosmics_prescale', 
        'MC_pp_v2', 'MC_pp_v2_primary', 'MC_pp_v2_no_prescale', 
        'MC_pp_v2_tight_mc_prescale', 'MC_pp_v2_loose_mc_prescale',
        #v3
        'Physics_pp_v3', 'Physics_pp_v3_no_prescale', 'Physics_pp_v3_cosmics_prescale', 
        'MC_pp_v3', 'MC_pp_v3_primary', 'MC_pp_v3_no_prescale', 
        'MC_pp_v3_tight_mc_prescale', 'MC_pp_v3_loose_mc_prescale',
        #v4
        'Physics_pp_v4', 'Physics_pp_v4_no_prescale', 'Physics_pp_v4_cosmics_prescale',
        'MC_pp_v4', 'MC_pp_v4_primary', 'MC_pp_v4_no_prescale',
        'MC_pp_v4_upgrade_mc_prescale','MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_loose_mc_prescale',
        # L1 v2 for testing
        'L1_pp_v2',
        'L1_pp_v3',
        'L1_pp_v4',
        'L1_pp_test',
        'L1_alfa_v1',
        'L1_alfa_v2',
        # for HeavyIon
        'InitialBeam_HI_v1', 'InitialBeam_HI_v1_no_prescale',
        'MC_InitialBeam_HI_v1', 'MC_InitialBeam_HI_v1_no_prescale',
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale',
        'MC_HI_v1',     'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',
        #
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale', 
        'MC_HI_v2',  'MC_HI_v2_no_prescale', 'MC_HI_v2_pPb_mc_prescale',
        #
        'Physics_default', 'MC_loose_default', 'MC_tight_default', 'default_loose', 'default_tight',
        # -----------------------------------------------------------------
        # Run 2
        'MC_pp_v5', 'MC_pp_v5_no_prescale', 'MC_pp_v5_tight_mc_prescale', 'MC_pp_v5_loose_mc_prescale','MC_pp_v5_special_mc_prescale', # for development and simulation
        'Physics_pp_v5', # for testing algorithms and software quality during LS1, later for data taking
        'Physics_pp_v5_cosmics_prescale',
        'Physics_pp_v5_tight_physics_prescale', 
        'LS1_v1', # for P1 detector commissioning (cosmics, streamers)
        'DC14', 'DC14_no_prescale', 'DC14_tight_mc_prescale', 'DC14_loose_mc_prescale', # for DC14
        'Physics_HI_v3', 'Physics_HI_v3_no_prescale', # for 2015 lead-lead menu 
        'MC_HI_v3', 'MC_HI_v3_tight_mc_prescale',
        'Physics_HI_v4', 'Physics_HI_v4_no_prescale', # for 2016 proton-lead menu 
        'MC_HI_v4', 'MC_HI_v4_tight_mc_prescale',
        'Physics_HI_v5', 'Physics_HI_v5_tight_physics_prescale', 'Physics_HI_v5_no_prescale', # for 2015 lead-lead menu 
        'MC_HI_v5', 'MC_HI_v5_tight_mc_prescale',

        'MC_pp_v6','Physics_pp_v6','MC_pp_v6_no_prescale', 'MC_pp_v6_tight_mc_prescale', 'MC_pp_v6_tightperf_mc_prescale', 'MC_pp_v6_loose_mc_prescale','Physics_pp_v6_tight_physics_prescale',
        'MC_pp_v7','Physics_pp_v7','Physics_pp_v7_primaries','MC_pp_v7_no_prescale', 'MC_pp_v7_loose_mc_prescale',
        'MC_pp_v7_BulkMCProd_mc_prescale', 'MC_pp_v7_CPSampleProd_mc_prescale','MC_pp_v7_TriggerValidation_mc_prescale',
        'Physics_pp_v7_tight_physics_prescale','Physics_pp_v7_unprescaled_only_physics_prescale',
        'MC_pp_v8', 'Physics_pp_v8', 'Physics_pp_v8_lowmu',

        # -----------------------------------------------------------------
        # Upgrade
        'MC_PhaseII',
        ]

    _default_menu='MC_pp_v7_BulkMCProd_mc_prescale'
    _default_cosmic_menu='Physics_pp_v5_cosmics_prescale'
    _default_InitialBeam_menu='MC_InitialBeam_v3_no_prescale'
    
    StoredValue = _default_menu

    def _do_action(self):
        """ setup some consistency """

        # meaning full default menu
        if self.get_Value() == 'default':
            self.set_Value(self._default_menu)
            self._log.info("%s - trigger menu 'default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
        elif self.get_Value() == 'cosmic_default':
            self.set_Value(self._default_cosmic_menu)
            self._log.info("%s - trigger menu 'cosmic_default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
        elif self.get_Value() == 'InitialBeam_default':
            self.set_Value(self._default_InitialBeam_menu)
            self._log.info("%s - trigger menu 'InitialBeam_default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
            
        # filenames for LVL1 and HLT
        if TriggerFlags.readLVL1configFromXML() is True:
            TriggerFlags.inputLVL1configFile = "LVL1config_"+self.get_Value()+"_" + TriggerFlags.menuVersion() + ".xml"
        if TriggerFlags.readHLTconfigFromXML() is True and (TriggerFlags.inputHLTconfigFile=="" or TriggerFlags.inputHLTconfigFile is None):
            TriggerFlags.inputHLTconfigFile = "HLTconfig_"+self.get_Value()+"_" + TriggerFlags.menuVersion() + ".xml"

_flags.append(triggerMenuSetup)

class L1PrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['', 'None']
    StoredValue = ''
_flags.append(L1PrescaleSet)

class HLTPrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['', 'None']
    StoredValue = ''
_flags.append(HLTPrescaleSet)


# the container of all trigger flags

class Trigger(JobPropertyContainer):
    """ Trigger top flags """
      
    def Slices_LVL2_setOn(self):
        """ Runs setL2 flags in all slices. Effectivelly enable LVL2. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setL2()

    def Slices_EF_setOn(self):
        """ Runs setEF flags in all slices. Effectivelly enable EF. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setEF()

    def Slices_all_setOn(self):
        """ Runs setL2 and setEF in all slices. Effectivelly enable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setAll()

    def Slices_LVL2_setOff(self):
        """ Runs unsetL2 flags in all slices.  Effectivelly disable LVL2. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetL2()


    def Slices_EF_setOff(self):
        """ Runs unsetEF flags in all slices.  Effectivelly disable EF. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetEF()

    def Slices_all_setOff(self):
        """ Runs unsetAll in all slices. Effectivelly disable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetAll()


## attach yourself to the RECO flags
from RecExConfig.RecFlags import rec
rec.add_Container(Trigger)


for flag in _flags:
    rec.Trigger.add_JobProperty(flag)
del _flags

## make an alias for trigger flags which looks like old TriggerFlags class
TriggerFlags = rec.Trigger




## add online specific flags
from TriggerJobOpts.TriggerOnlineFlags      import OnlineFlags

## add slices generation flags

from TriggerMenu.menu.SliceFlags import *
from TriggerJobOpts.Tier0TriggerFlags       import Tier0TriggerFlags
from TrigTier0.NtupleProdFlags              import NtupleProductionFlags


def sync_Trigger2Reco():
    from AthenaCommon.Include import include
    from RecExConfig.RecAlgsFlags import recAlgs
    from AthenaCommon.GlobalFlags  import globalflags
    from RecExConfig.RecFlags import rec
    
    if  recAlgs.doTrigger() and rec.readRDO() and not globalflags.InputFormat()=='bytestream':
        include( "TriggerRelease/TransientBS_DetFlags.py" )

    from RecExConfig.RecFlags import rec
    if globalflags.InputFormat() == 'bytestream':
        TriggerFlags.readBS = True
        TriggerFlags.doLVL1 = False
        TriggerFlags.doLVL2 = False
        TriggerFlags.doEF   = False

    if rec.doWriteBS():
        TriggerFlags.writeBS = True

del log
