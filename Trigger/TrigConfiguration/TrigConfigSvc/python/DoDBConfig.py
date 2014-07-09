# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file  DoDBConfig.py
## @brief Configure services to extract the trigger configuration for DB upload
## $Id: DoDBConfig.py,v 1.4 2009-03-05 14:52:41 pbell Exp $

def _doDBConfig():
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.AppMgr import theApp
   from TriggerJobOpts.TriggerFlags import TriggerFlags
   from AthenaCommon.Logging import logging

   log = logging.getLogger( 'TrigConfigSvc::DoDBConfig:' )

   if TriggerFlags.doLVL2(): SetupOutput = "l2"
   else: SetupOutput = "ef"
   SetupOutput += "_" + TriggerFlags.Online.doDBConfigBaseName() + "_setup.txt"

   if hasattr(svcMgr,'LVL1ConfigSvc'):
      log.info("DBCONFIG LVL1XML %s" % svcMgr.LVL1ConfigSvc.XMLFile)
   if hasattr(svcMgr,'HLTConfigSvc'):            
      log.info("DBCONFIG HLTXML  %s" % svcMgr.HLTConfigSvc.XMLMenuFile)

   log.info("DBCONFIG SETUP   %s" % SetupOutput)   
   
   # Replace the common HistorySvc by the TrigHistorySvc
   from TrigConfOffline.TrigHistorySvc import TrigHistorySvc
   if not svcMgr.__dict__.has_key('HistorySvc') or not isinstance( svcMgr.HistorySvc, TrigHistorySvc) :
      if svcMgr.__dict__.has_key('HistorySvc'): del svcMgr.HistorySvc
      svcMgr += TrigHistorySvc('HistorySvc')
   
   svcMgr.HistorySvc.OutputFile = SetupOutput
   svcMgr.HistorySvc.Dump       = True
   svcMgr.HistorySvc.Activate   = True
   theApp.ActivateHistory = True

   # Fill the meta data from the PSC configuration if available
   try:
      from TrigPSC import PscConfig      
   except:
      log.warning('Cannot import TrigPSC.PscConfig. TrigConfMetaData will not be filled.')
      pass

   if 'PscConfig' in dir():
      from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
      meta = TrigConfMetaData()      
      meta.PreCommand = PscConfig.optmap['PRECOMMAND']
      meta.JobOptions = PscConfig.optmap['JOBOPTIONSPATH']
   
   return

# Create meta data configurable (needs to be at module-level)
from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
meta = TrigConfMetaData()

# configure
_doDBConfig()

# clean-up
del _doDBConfig

