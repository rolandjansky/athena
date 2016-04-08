# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------------------------
# Replacing MessageSvc with LoggedMessageSvc
#--------------------------------------------------------------------------------
try:
   from AthenaServices import SummarySvc
except:
   print 'Could not import AthenaServices.SummarySvc'
   try:
      from AthenaCommon.AppMgr import ServiceMgr, theApp
      import AthenaCommon.ConfigurableDb as ConfDb
      from AthenaServices.AthenaServicesConf import AthenaSummarySvc
      from AthenaCommon.OldStyleConfig import Service
   except:
      print 'Could not import required modules to enable use of LoggedMessageSvc.'
   else:
      print 'Using AthenaServices...'
      from AthenaServices.AthenaServicesConf import AthenaSummarySvc
      AthenaSummarySvc.SummaryFile = 'AthenaSummary_ESDtoAOD.txt'
      theApp.CreateSvc += ['AthenaSummarySvc']
      AthenaSummarySvc = Service('AthenaSummarySvc')
      ServiceMgr += AthenaSummarySvc
      theApp.MessageSvcType = 'LoggedMessageSvc'
      try:
         oldMsgSvcName = ServiceMgr.MessageSvc.name()
      except AttributeError:
         oldMsgSvcName = 'MessageSvc'
         oldMsgSvcItems = []
      else:
         oldMsgSvcItems = ServiceMgr.MessageSvc.getValuedProperties().items()
         del ServiceMgr.MessageSvc
      try:
         del ServiceMgr.allConfigurables[ oldMsgSvcName ]
      except KeyError:
         pass
      newMsgSvc = ConfDb.getConfigurable( theApp.MessageSvcType )( oldMsgSvcName )
      for name, value in oldMsgSvcItems:
         setattr( newMsgSvc, name, value )
      ServiceMgr += newMsgSvc
      MessageSvc = ServiceMgr.MessageSvc
else:
   print 'Using AthenaServices.SummarySvc...'
   from AthenaServices.AthenaServicesConf import AthenaSummarySvc
   AthenaSummarySvc.SummaryFile = 'AthenaSummary_ESDtoAOD.txt'
   SummarySvc.useAthenaSummarySvc()
