# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigConfigSvc.TrigConfigSvcConf import (TrigConf__LVL1ConfigSvc,
                                             TrigConf__HLTConfigSvc,
                                             TrigConf__DSConfigSvc,
                                             TrigConf__TrigConfigSvc)

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

class SetupTrigConfigSvc(object):
    """ A python singleton class for configuring the trigger configuration services"""

    class __impl(object):
        """ Implementation of the singleton interface """

        def __init__(self):
            """
            stats == ds  -> read the trigger configuration from the detector store = esd header
            state == none -> service is not directly serving the run3 configuration
            """
            self.states = ['none']
            self.allowedStates = set(['none','ds'])
            self.initialised = False

            from AthenaCommon.Logging import logging
            self.mlog = logging.getLogger("TrigConfigSvcConfig.py")


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
            if 'ds' in self.states:

                self.mlog.info( "setup DSConfigSvc and add instance to ServiceMgr" )
                ds = DSConfigSvc("DSConfigSvc")
                ServiceMgr += ds

            self.mlog.info( "setup TrigConfigSvc and add instance to ServiceMgr" )
            ServiceMgr += TrigConfigSvc("TrigConfigSvc")
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
