# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: TrigServicesConfig.py
# @purpose: customized configurables

from TrigServicesConf import TrigCOOLUpdateHelper as _TrigCOOLUpdateHelper
      
class TrigCOOLUpdateHelper(_TrigCOOLUpdateHelper):
   __slots__ = ()

   def __init__(self, name='TrigCOOLUpdateHelper'):
      super(TrigCOOLUpdateHelper, self).__init__(name)
      return

   def setDefaults(cls,handle):
      """Set default property values (is being called more than once!)"""
      
      # Folder allowed to be updated during the run 
      handle.allowedFolderUpdates = ["/Indet/Onl/Beampos"]
      
      return

   def enable(self, folder='/TRIGGER/HLT/COOLUPDATE', tag=None):
      """Enable the COOL folder updates (only use this for data)"""
      
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      if not hasattr(svcMgr,'IOVDbSvc'): return
      
      self.coolFolder = folder
      from IOVDbSvc.CondDB import conddb
      if tag==None:
         conddb.addFolder('TRIGGER',self.coolFolder)
      else:
         conddb.addFolderWithTag('TRIGGER',self.coolFolder,tag)
      
