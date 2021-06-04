# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigConfigSvc.TrigConfigSvcConf import (TrigConf__LVL1ConfigSvc,
                                             TrigConf__HLTConfigSvc,
                                             TrigConf__DSConfigSvc,
                                             TrigConf__TrigConfigSvc)

from os.path import exists, join, abspath

from AthenaCommon.Logging import logging  # loads logger
from PyUtils.Decorators import memoize

@memoize
def findFileInXMLPATH(filename):
    """ Use XMLPATH to find files (cache result through memoize decorator)"""

    if filename=="NONE":
        return filename

    filename = str(filename)

    mlog = logging.getLogger("TrigConfigSvcConfig.py")
    mlog.debug("Searching XML config file for HLT")
    if filename.find('./') == 0: ## this expected to be local file, name starts from ./
        return filename
    else:
        mlog.debug("Nonlocal XML config file for HLT")
        from os import environ
        ## even if ./ not as file name prefix look first in PWD
        if exists(filename):
            mlog.info(filename+" XML configuration file taken from working directory")
            return filename

        ## search XMLPATH path
        if 'XMLPATH' not in environ: ## XMLPATH is not known ... no search is performed
            mlog.info("XML file: "+filename + " not found and XMLPATH not given" )
            return filename

        xmlpath = environ['XMLPATH']
        paths = xmlpath.split(":")
        for path in paths:

            test = join(path, filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

            test = join(path, "TriggerMenuMT",filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

        return filename



class HLTConfigSvc( TrigConf__HLTConfigSvc ):
    #__slots__ = []
    def __init__( self, name="HLTConfigSvc" ):
        super( HLTConfigSvc, self ).__init__( name )

    def setDefaults(self, handle):
        pass


class LVL1ConfigSvc ( TrigConf__LVL1ConfigSvc ):
    __slots__ = []
    def __init__( self, name="LVL1ConfigSvc" ):
        super( LVL1ConfigSvc, self ).__init__( name )

    @property
    def XMLFile(self):
        '''The xml file name for backward compatibility with old property name'''
        return  self.XMLMenuFile

    @XMLFile.setter
    def XMLFile(self, xmlfile):
        log = logging.getLogger("LVL1ConfigSvc")
        log.warning( "LVL1ConfigSvc property XMLFile will soon be deprecated. Please use XMLMenuFile instead" )
        self.XMLMenuFile = xmlfile

    def setDefaults(self, handle):
        pass

class DSConfigSvc ( TrigConf__DSConfigSvc ) :
    """Detector Store implementation of the TrigConfigSvc"""
    __slots__ = []
    def __init__(self, name="DSConfigSvc" ):
        super (DSConfigSvc, self).__init__(name)

    def setDetaults(self, handle):
        pass

class TrigConfigSvc( TrigConf__TrigConfigSvc ):
    __slots__ = []
    def __init__(self, name="TrigConfigSvc" ):
        super( TrigConfigSvc, self).__init__(name)

    def setDefaults(self, handle):
        pass


# singleton class used for all
# clients of any TrigConfigSvc
# usage:
# from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
# svc = SetupTrigConfigSvc()
# svc.SetStates( ["ds"] )
# svc.InitialiseSvc()
#
# after InitialiseSvc() is called, the state is fixed
# possible states are: ds, xml


class SetupTrigConfigSvc(object):
    """ A python singleton class for configuring the trigger configuration services"""

    class __impl(object):
        """ Implementation of the singleton interface """

        def __init__(self):
            """
            state == xml -> read the trigger configuration from 2 xml files, one for L1, one for HLT
            stats == ds  -> read the trigger configuration from the detector store = esd header
            state == none -> service is not directly serving the run3 configuration
            """
            self.states = ["xml"]
            self.allowedStates = set(['none','xml','ds'])
            self.initialised = False

            from AthenaCommon.Logging import logging
            self.mlog = logging.getLogger("TrigConfigSvcConfig.py")

            # svc properties:
            self.hltXmlFile = 'HLT_XML_FILE_NOT_SET'
            self.l1XmlFile  = 'L1_XML_FILE_NOT_SET'


        def SetStates(self, state):
            if self.initialised:
                raise (RuntimeError, 'state cannot be changed anymore, the athena service has already been added!')

            if not type(state) is list:
                state = [state]

            if not set(state) <= self.allowedStates:
                raise (RuntimeError, 'unknown state %s, cannot set it!' % state)
            else:
                self.mlog.info( "setting TrigConfigSvc states to %s", self.states )
                self.states = state


        def InitialiseSvc(self):
            if self.initialised:
                raise (RuntimeError, 'athena service has already been added, do nothing.')

            self.initialised = True

            from AthenaCommon.AppMgr import ServiceMgr

            self.mlog.info( "initialising TrigConfigSvc using state %s", self.states )
            if 'xml' in self.states:
                from TriggerJobOpts.TriggerFlags import TriggerFlags


                if TriggerFlags.doHLT() or TriggerFlags.configForStartup()=='HLToffline':
                    self.mlog.info( "setup HLTConfigSvc and add instance to ServiceMgr (xml file="+self.hltXmlFile+")" )
                    hlt = HLTConfigSvc("HLTConfigSvc")
                    hlt.XMLMenuFile = self.hltXmlFile
                    hlt.doMergedHLT = TriggerFlags.doHLT()
                    ServiceMgr += hlt
                else:
                    self.mlog.info( "Will not setup HLTConfigSvc, since doHLT() is False" )
                    self.states[self.states.index("xml")] = "xmll1"

                self.mlog.info("setup LVL1 ConfigSvc and add instance to ServiceMgr")
                self.mlog.info("xml file = %s", self.l1XmlFile)

                l1 = LVL1ConfigSvc("LVL1ConfigSvc")
                l1.XMLMenuFile = self.l1XmlFile
                ServiceMgr += l1


            if 'ds' in self.states:

                self.mlog.info( "setup DSConfigSvc and add instance to ServiceMgr" )
                ds = DSConfigSvc("DSConfigSvc")
                ServiceMgr += ds

            self.mlog.info( "setup TrigConfigSvc and add instance to ServiceMgr" )
            trigSvc = TrigConfigSvc("TrigConfigSvc")
            trigSvc.PriorityList = self.states
            ServiceMgr += trigSvc
            from AthenaCommon.AppMgr import theApp
            theApp.CreateSvc += [ ServiceMgr.TrigConfigSvc.getFullName() ]


    # storage for the instance reference
    __instance = None

    def __init__(self):
        """ Create singleton instance """
        # Check whether we already have an instance
        if SetupTrigConfigSvc.__instance is None:
            # Create and remember instance
            SetupTrigConfigSvc.__instance = SetupTrigConfigSvc.__impl()

        # Store instance reference as the only member in the handle
        self.__dict__['_SetupTrigConfigSvc__instance'] = SetupTrigConfigSvc.__instance

    def __getattr__(self, attr):
        """ Delegate access to implementation """
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        """ Delegate access to implementation """
        return setattr(self.__instance, attr, value)


def TrigConfigSvcCfg( flags ):
    from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg
    return TrigConfigSvcCfg( flags )

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()
    acc = TrigConfigSvcCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")
