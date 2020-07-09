# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: Configurables.py
# @purpose: customized Configurable classes for AthenaServices
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

## import the automatically generated Configurables
from AthenaServices.AthenaServicesConf import CoreDumpSvc as _CoreDumpSvc
from AthenaServices.AthenaServicesConf import EvtIdModifierSvc as _EvtIdModifierSvc

class CoreDumpSvc( _CoreDumpSvc ):
   
   __slots__ = ()  # enforce no new properties

   def __init__(self, name = "CoreDumpSvc", **kwargs):
      
      # have to call base init
      kwargs['name'] = name
      super( CoreDumpSvc, self ).__init__( **kwargs )
      
      # make sure the application manager explicitly creates the service
      from AthenaCommon.AppMgr import theApp
      handleName = self.getFullJobOptName()
      if handleName not in theApp.CreateSvc:
         theApp.CreateSvc += [ handleName ]
         
      return

   pass # class CoreDumpSvc

###
class EvtIdModifierSvc(_EvtIdModifierSvc):
    __slots__ = ()
    def __init__(self, name="EvtIdModifierSvc", **kwargs):
        kwargs['name'] = name
        _EvtIdModifierSvc.__init__(self, **kwargs)

    def add_modifier(self,
                     run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None, 
                     nevts=1):
        if run_nbr is None:
            modify_run_nbr = 0
            run_nbr = 0
        else:
            modify_run_nbr = 1
            
        if evt_nbr is None:
            modify_evt_nbr = 0
            evt_nbr = 0
        else:
            modify_evt_nbr = 1
            
        if time_stamp is None:
            modify_time_stamp = 0
            time_stamp = 0
        else:
            modify_time_stamp = 1

        if lbk_nbr is None:
            modify_lbk_nbr = 0
            lbk_nbr = 0
        else:
            modify_lbk_nbr = 1

        mod_bit = int(0b0000
                      | (modify_run_nbr << 0)
                      | (modify_evt_nbr << 1)
                      | (modify_time_stamp << 2)
                      | (modify_lbk_nbr << 3))

        self.Modifiers += [run_nbr, evt_nbr, time_stamp, lbk_nbr,
                           nevts, mod_bit]
        return
    
