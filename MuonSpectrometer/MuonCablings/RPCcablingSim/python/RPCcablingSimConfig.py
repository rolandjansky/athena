# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RPCcablingSim.RPCcablingSimConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class RPCcablingSimConfig (RPCcablingSimSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(RPCcablingSimConfig ,self).__init__(name)

        if ( globalflags.DetDescrVersion().startswith('DC1') or globalflags.DetDescrVersion().startswith('DC2') ):
            self.ConfFileName = "P03conf.data"
            self.CorrFileName = "P03conf.corr"
        
        #temporary hack since TrigT1ConfigVersion is not into a python module        	
        try:
            from __main__ import TrigT1ConfigVersion
            if TrigT1ConfigVersion.find("900GeV")!=-1:
                self.ConfFileName = "900GeVconf.data"
                self.CorrFileName = "900GeVconf.corr"
        except ImportError:
            pass			

    
    def setDefaults(cls,handle):
        if hasattr(handle,'ConfFileName'):
            if handle.ConfFileName!="LVL1conf.data":
                print "RPCcablingSvc uses a non standard configuration: ", handle.ConfFileName

        if hasattr(handle,'CorrFileName'):    
            if handle.CorrFileName!="LVL1conf.corr":
                print "RPCcablingSvc uses a non standard correction set: ", handle.CorrFileName



RPCcablingSimSvc = RPCcablingSimConfig()
ServiceMgr += RPCcablingSimSvc
