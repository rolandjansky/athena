# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##  from AthenaCommon.GlobalFlags import jobproperties
## ## from AthenaCommon.GlobalFlags import GlobalFlags
##  from AthenaCommon.AthenaCommonFlags import jobproperties
##  from RecExConfig.RecFlags  import jobproperties

from TriggerJobOpts.TriggerFlags import TriggerFlags


from AthenaCommon.Logging import logging



from AthenaCommon.AppMgr import ServiceMgr

log = logging.getLogger( "T0TriggerGetter.py" )


try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
    from TriggerMenuPython.GenerateMenu import GenerateMenu


from RecExConfig.Configured import Configured 

def withLVL1():
    return TriggerFlags.dataTakingConditions()=='Lvl1Only' or TriggerFlags.dataTakingConditions()=='FullTrigger'

def withHLT():
    return TriggerFlags.dataTakingConditions()=='HltOnly' or TriggerFlags.dataTakingConditions()=='FullTrigger'

class T0TriggerGetter(Configured):

    #_output = {"HLT::HLTResult" : ["HLTResult_L2", "HLTResult_EF"] }
    _configured=True
    _done=False
    
    def configure(self):
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True
        
        # setup configuration services
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg =  TriggerConfigGetter()

        # after the menu xml file has been created or the TriggerDB access is configured,
        # the COOL/SQlite db can be written 
        from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
        theConfCOOLWriter.writeConf2COOL()

        if withLVL1():
            # setup Lvl1
            # initialize LVL1ConfigSvc
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
            lvl1 = Lvl1ResultBuilderGetter()

        if withHLT():
            # setup HLT
            # initialize HLT config svc
            log.info("configuring hlt")
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hlt = HLTTriggerResultGetter()

        #Call the tools to unpack the bytestream
        #bsu=ByteStreamUnpackGetter()
        
        #Call the tool to make the TrigDecision object for ESD or AOD
        #tdg=TrigDecisionGetter()
        
        return True
