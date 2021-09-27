# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import AllowedList
from TrigConfigSvc.TrigConfigSvcUtils import getKeysFromNameRelease, getMenuNameFromDB


_flags = []

# Define simple boolean flags
def bool_flag_with_default(name, val):
    return type(name,
                (JobProperty, ),
                {
                   "statusOn": True,
                   "allowedType": ['bool'],
                   "StoredValue": val,
                })

default_true_flags = [
    "doLVL1", # run the LVL1 simulation (set to FALSE to read the LVL1 result from BS file)
    "doL1Topo", # Run the L1 Topo simulation (set to FALSE to read the L1 Topo result from BS file)
    "doID",  # if False, disable ID algos at LVL2 and EF """
    "doCalo",  # if False, disable Calo algorithms at LVL2 & EF """
    "doCaloOffsetCorrection",  # enable Calo pileup offset BCID correction """
    "doMuon", # if FAlse, disable Muons, note: muons need input file containing digits"""
    "doNavigationSlimming",  # Enable the trigger navigation slimming"""
]

default_false_flags = [
    "useRun1CaloEnergyScale",
    "doTruth",
    "doTriggerConfigOnly",  # if True only the configuration services should be set, no algorithm """
    "readBS",
    "readMenuFromTriggerDb", # define the TriggerDb to be the source of the LVL1 and HLT trigger menu
]

for name in default_true_flags:
    newFlag = bool_flag_with_default(name, True)
    globals()[newFlag.__name__] = newFlag
    _flags.append(newFlag)

for name in default_false_flags:
    newFlag = bool_flag_with_default(name, False)
    globals()[newFlag.__name__] = newFlag
    _flags.append(newFlag)

class doHLT(JobProperty):
    """ if True, run HLT selection algorithms """
    statusOn=True
    allowedType=['bool']
    StoredValue=False
    
_flags.append(doHLT)

class doValidationMonitoring(JobProperty):
    """Enables extra validation monitoring"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doValidationMonitoring)

# trigger configuration source list
class configurationSourceList(JobProperty):
    """ define where to read trigger configuration from. Allowed values: ['aod','ds']"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]
    allowedValues = AllowedList( ['aod','ds'] )

_flags.append(configurationSourceList)

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
    StoredValue = 'HLTonline'
    
    allowedValues = [
        'HLTonline',
        'HLToffline'
        ]

_flags.append(configForStartup)


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
        log.info("Setting TriggerFlags.triggerUseFrontier to %r", self.get_Value())
        
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
    MCRECO:DB:connectionstring:SMKey,L1PSK,HLTPSK[,BGK]  -takes these db keys
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

        # FW, 09/21: This flag is obsolete but need a bit more cleanup before removing it
        # to avoid accidentally breaking job options
        log.warning("The use of TriggerFlags.triggerConfig is deprecated")

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
                except IOError:
                    log.fatal("triggerConfig=DATARECO:REPR requires 'MenuCoolDbLocation.tx' to be present in the local directory (reco part of trigger reprocessing)")
                    
            elif configs[1] == 'DB' or configs[1] == 'DBF': # We read config from a private DB
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: DATARECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: DATARECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: DATARECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
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
                        log.info("triggerConfig: LVL1 from DB with specified keys SMK %i and L1 PSK %i.", *tuple(tf.triggerDbKeys()[0:2]))
                    else:
                        log.info("triggerConfig: LVL1 from DB with speficied menu name and release: finding keys...")
                        tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],True) + [-1,1]
                        log.info("triggerConfig: LVl1 from DB with keys SMK %i and L1 PSK %i", *tuple(tf.triggerDbKeys()[0:2]))
                else:                  #We got a menu name which we need to look up - not implemented yet
                    log.info("triggerConfig: LVL1 from DB configured with wrong number of keys/arguments" )

            else:
                log.error("Reading the LVL1 menu from XML is no longer supported")

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
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 4):                            # we got 4 keys (SM, L1PS, HLTPS,BGK)
                    tf.triggerDbKeys=[int(x) for x in DBkeys]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, HLT PSK %i, and BGK %i.", *tuple(tf.triggerDbKeys()[0:4]))
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: MCRECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: MCRECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
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
                log.error("Reading the LVL1 menu from XML is no longer supported")

            # This part was there in the original (old) csc_reco_trigger.py snippet
            # Still wanted?
            if rec.doTrigger:
                # Switch off trigger of sub-detectors
                for detOff in recConfig.detectorsOff:
                    cmd = 'TriggerFlags.do%s = False' % detOff
                    # possibly not all DetFlags have a TriggerFlag
                    try:
                        exec(cmd)
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


# the container of all trigger flags

class Trigger(JobPropertyContainer):
    """ Trigger top flags """
      

    def Slices_all_setOn(self):
        """ Runs setL2 and setEF in all slices. Effectivelly enable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setAll()


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

## add slices generation flags
log.info("TriggerFlags importing SliceFlags"  )
from TriggerJobOpts.SliceFlags import *                             # noqa: F401, F403


def sync_Trigger2Reco():
    from AthenaCommon.Include import include
    from RecExConfig.RecAlgsFlags import recAlgs
    from AthenaCommon.GlobalFlags  import globalflags
    from RecExConfig.RecFlags import rec
    
    if  recAlgs.doTrigger() and rec.readRDO() and not globalflags.InputFormat()=='bytestream':
        include( "TriggerJobOpts/TransientBS_DetFlags.py" )

    if globalflags.InputFormat() == 'bytestream':
        TriggerFlags.readBS = True
        TriggerFlags.doLVL1 = False
        TriggerFlags.doHLT   = False

del log
